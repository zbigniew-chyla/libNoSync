// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__PRIORITIZING_REQUEST_HANDLER_IMPL_H
#define NOSYNC__PRIORITIZING_REQUEST_HANDLER_IMPL_H

#include <algorithm>
#include <functional>
#include <nosync/container-utils.h>
#include <nosync/eclock.h>
#include <nosync/func-request-handler.h>
#include <nosync/requests-queue.h>
#include <nosync/result-handler.h>
#include <nosync/time-utils.h>
#include <utility>


namespace nosync
{

namespace prioritizing_request_handler_impl
{

template<typename Req, typename Res, std::size_t N>
class requests_prioritizer : public std::enable_shared_from_this<requests_prioritizer<Req, Res, N>>
{
public:
    requests_prioritizer(event_loop &evloop, std::shared_ptr<request_handler<Req, Res>> &&base_req_handler);

    std::array<std::shared_ptr<request_handler<Req, Res>>, N> create_input_request_handlers();

private:
    using std::enable_shared_from_this<requests_prioritizer<Req, Res, N>>::shared_from_this;

    void handle_request(
        unsigned queue_index, Req &&request, eclock::duration timeout,
        result_handler<Res> &&res_handler);

    requests_queue<Req, Res> *try_find_first_pending_request_queue();
    void forward_first_pending_request_if_present();

    event_loop &evloop;
    std::shared_ptr<request_handler<Req, Res>> base_req_handler;
    bool request_handling_ongoing;
    std::array<std::shared_ptr<requests_queue<Req, Res>>, N> queues;
};


template<typename Req, typename Res, std::size_t N>
requests_prioritizer<Req, Res, N>::requests_prioritizer(
    event_loop &evloop, std::shared_ptr<request_handler<Req, Res>> &&base_req_handler)
    : evloop(evloop), base_req_handler(std::move(base_req_handler)), request_handling_ongoing(false),
    queues(
        generate_array<std::shared_ptr<requests_queue<Req, Res>>, N>(
            [&evloop](auto) {
                return std::make_shared<requests_queue<Req, Res>>(evloop);
            }))
{
}


template<typename Req, typename Res, std::size_t N>
std::array<std::shared_ptr<request_handler<Req, Res>>, N> requests_prioritizer<Req, Res, N>::create_input_request_handlers()
{
    return generate_array<std::shared_ptr<request_handler<Req, Res>>, N>(
        [&, &prio = *this](auto queue_index) -> std::shared_ptr<request_handler<Req, Res>> {
            return make_func_request_handler<Req, Res>(
                [prio_ptr = prio.shared_from_this(), queue_index](auto req, auto timeout, auto res_handler) {
                    prio_ptr->handle_request(queue_index, std::move(req), timeout, std::move(res_handler));
                });
        });
}


template<typename Req, typename Res, std::size_t N>
void requests_prioritizer<Req, Res, N>::handle_request(
    unsigned queue_index, Req &&request, eclock::duration timeout,
    result_handler<Res> &&res_handler)
{
    if (!request_handling_ongoing) {
        base_req_handler->handle_request(
            std::move(request), timeout,
            [prio_ptr = shared_from_this(), res_handler = std::move(res_handler)](auto resp) {
                prio_ptr->request_handling_ongoing = false;
                res_handler(std::move(resp));
                prio_ptr->forward_first_pending_request_if_present();
            });
        request_handling_ongoing = true;
    } else {
        queues[queue_index]->push_request(std::move(request), timeout, std::move(res_handler));
    }
}


template<typename Req, typename Res, std::size_t N>
requests_queue<Req, Res> *requests_prioritizer<Req, Res, N>::try_find_first_pending_request_queue()
{
    auto queue_it = std::find_if(
        queues.begin(), queues.end(),
        [](auto &queue) {
            return queue->has_requests();
        });
    auto queue = queue_it != queues.end() ? queue_it->get() : nullptr;

    return queue;
}


template<typename Req, typename Res, std::size_t N>
void requests_prioritizer<Req, Res, N>::forward_first_pending_request_if_present()
{
    using std::move;

    auto queue = try_find_first_pending_request_queue();
    if (queue == nullptr) {
        return;
    }

    auto prio_ptr = shared_from_this();

    queue->pull_next_request_to_consumer(
        [&](auto &&req, auto timeout, auto &&res_handler) {
            base_req_handler->handle_request(
                move(req), timeout,
                [prio_ptr = move(prio_ptr), res_handler = move(res_handler)](auto resp) {
                    prio_ptr->request_handling_ongoing = false;
                    res_handler(move(resp));
                    prio_ptr->forward_first_pending_request_if_present();
                });
        });

    request_handling_ongoing = true;
}

}


template<typename Req, typename Res, std::size_t N>
std::array<std::shared_ptr<request_handler<Req, Res>>, N> make_prioritizing_request_handler(
    event_loop &evloop, std::shared_ptr<request_handler<Req, Res>> &&base_req_handler)
{
    auto prioritizer = std::make_shared<prioritizing_request_handler_impl::requests_prioritizer<Req, Res, N>>(
        evloop, std::move(base_req_handler));
    return prioritizer->create_input_request_handlers();
}

}

#endif
