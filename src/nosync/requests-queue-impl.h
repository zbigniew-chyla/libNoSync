// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__REQUESTS_QUEUE_IMPL_H
#define NOSYNC__REQUESTS_QUEUE_IMPL_H

#include <algorithm>
#include <nosync/activity-handle.h>
#include <nosync/event-loop-utils.h>
#include <nosync/raw-error-result.h>
#include <nosync/result-utils.h>
#include <nosync/time-utils.h>
#include <stdexcept>
#include <system_error>
#include <utility>


namespace nosync
{

namespace requests_queue_impl
{

template<class InputIt, class UnaryOperation>
auto min_transformed_value(InputIt first, InputIt last, UnaryOperation unary_op) -> decltype(unary_op(*first))
{
    auto min_elem_iter = std::min_element(
        first, last,
        [&](const auto &lhs, const auto &rhs) {
            return unary_op(lhs) < unary_op(rhs);
        });
    return unary_op(*min_elem_iter);
}

}


template<typename Req, typename Res>
requests_queue<Req, Res>::requests_queue(event_loop &evloop)
    : evloop(evloop)
{
}


template<typename Req, typename Res>
requests_queue<Req, Res>::~requests_queue()
{
    if (!requests.empty()) {
        invoke_later(
            evloop,
            [cancel_requests = std::move(requests)]() mutable {
                for (auto &req : cancel_requests) {
                    auto &res_handler = std::get<result_handler<Res>>(req);
                    res_handler(raw_error_result(std::errc::operation_canceled));
                    res_handler = nullptr;
                }
            });
    }
}


template<typename Req, typename Res>
void requests_queue<Req, Res>::push_request(
    Req &&request, std::chrono::time_point<eclock> timeout_end,
    result_handler<Res> &&res_handler)
{
    requests.emplace_back(std::move(request), timeout_end, std::move(res_handler));
    reschedule_timeout_task();
}


template<typename Req, typename Res>
void requests_queue<Req, Res>::push_request(
    Req &&request, std::chrono::nanoseconds timeout,
    result_handler<Res> &&res_handler)
{
    push_request(std::move(request), time_point_sat_add(evloop.get_etime(), timeout), std::move(res_handler));
}


template<typename Req, typename Res>
bool requests_queue<Req, Res>::has_requests() const
{
    return !requests.empty();
}


template<typename Req, typename Res>
std::tuple<Req, std::chrono::time_point<eclock>, result_handler<Res>> requests_queue<Req, Res>::pull_next_request()
{
    if (requests.empty()) {
        throw std::logic_error("no requests in the queue");
    }

    auto next_request = std::move(requests.front());
    requests.pop_front();

    reschedule_timeout_task();

    return next_request;
}


template<typename Req, typename Res>
template<typename F>
bool requests_queue<Req, Res>::pull_next_request_to_consumer(const F &req_consumer)
{
    if (requests.empty()) {
        return false;
    }

    auto req = pull_next_request();

    req_consumer(
        std::move(std::get<Req>(req)),
        std::max(std::get<std::chrono::time_point<eclock>>(req) - evloop.get_etime(), std::chrono::nanoseconds(0)),
        std::move(std::get<result_handler<Res>>(req)));

    return true;
}


template<typename Req, typename Res>
template<typename RequestPredicate, typename Consumer>
bool requests_queue<Req, Res>::pull_next_matching_request_to_consumer(
    const RequestPredicate &predicate, const Consumer &req_consumer)
{
    auto found_req_iter = std::find_if(
        requests.begin(), requests.end(),
        [&](const auto &req) {
            return predicate(std::get<Req>(req));
        });

    bool request_found = found_req_iter != requests.end();

    if (request_found) {
        auto req = std::move(*found_req_iter);

        requests.erase(found_req_iter);
        reschedule_timeout_task();

        req_consumer(
            std::move(std::get<Req>(req)),
            std::max(std::get<std::chrono::time_point<eclock>>(req) - evloop.get_etime(), std::chrono::nanoseconds(0)),
            std::move(std::get<result_handler<Res>>(req)));
    }

    return request_found;
}


template<typename Req, typename Res>
template<typename Func>
void requests_queue<Req, Res>::for_each_request(const Func &func) const
{
    const auto etime = evloop.get_etime();
    std::for_each(
        requests.begin(), requests.end(),
        [&](const auto &req) {
            func(
                std::get<Req>(req),
                std::max(std::get<std::chrono::time_point<eclock>>(req) - etime, std::chrono::nanoseconds(0)),
                std::get<result_handler<Res>>(req));
        });
}


template<typename Req, typename Res>
void requests_queue<Req, Res>::handle_pending_timeouts()
{
    namespace ch = std::chrono;

    std::deque<result_handler<Res>> timeouting_res_handlers;

    const auto now = evloop.get_etime();
    for (auto &req : requests) {
        if (std::get<ch::time_point<eclock>>(req) <= now) {
            auto &req_res_handler = std::get<result_handler<Res>>(req);
            timeouting_res_handlers.push_back(std::move(req_res_handler));
            req_res_handler = nullptr;
        }
    }

    requests.erase(
        std::remove_if(
            requests.begin(), requests.end(),
            [](auto &req) {
                return !std::get<result_handler<Res>>(req);
            }),
        requests.end());

    for (auto &res_handler : timeouting_res_handlers) {
        res_handler(make_timeout_raw_error_result());
        res_handler = nullptr;
    }
}


template<typename Req, typename Res>
void requests_queue<Req, Res>::disable_timeout_task_if_present()
{
    if (scheduled_timeout_task) {
        scheduled_timeout_task = std::experimental::nullopt;
    }
}


template<typename Req, typename Res>
void requests_queue<Req, Res>::reschedule_timeout_task()
{
    namespace ch = std::chrono;

    if (requests.empty()) {
        disable_timeout_task_if_present();
        return;
    }

    auto min_timeout_end = requests_queue_impl::min_transformed_value(
        requests.cbegin(), requests.cend(),
        [&](const auto &elem) {
            return std::get<ch::time_point<eclock>>(elem);
        });

    if (!scheduled_timeout_task || std::get<ch::time_point<eclock>>(*scheduled_timeout_task) != min_timeout_end) {
        disable_timeout_task_if_present();
        scheduled_timeout_task.emplace(
            min_timeout_end,
            evloop.invoke_at(
                min_timeout_end,
                [this]() {
                    scheduled_timeout_task = std::experimental::nullopt;
                    handle_pending_timeouts();
                    reschedule_timeout_task();
                }));
    }
}

}

#endif /* NOSYNC__REQUESTS_QUEUE_IMPL_H */
