// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__LAYERED_REQUEST_HANDLER_IMPL_H
#define NOSYNC__LAYERED_REQUEST_HANDLER_IMPL_H

#include <algorithm>
#include <nosync/const-response-request-handler.h>
#include <nosync/time-utils.h>
#include <nosync/raw-error-result.h>
#include <system_error>
#include <utility>


namespace nosync
{

namespace layered_request_handler_impl
{

template<typename Req, typename Res>
class layered_request_handler : public request_handler<Req, Res>, public std::enable_shared_from_this<layered_request_handler<Req, Res>>
{
public:
    layered_request_handler(
        event_loop &evloop, std::vector<std::shared_ptr<request_handler<Req, Res>>> &&layers_req_handlers,
        std::function<bool(std::error_code)> &&next_allowed_predicate);

    void handle_request(Req &&request, std::chrono::nanoseconds timeout, result_handler<Res> &&res_handler) override;

private:
    using std::enable_shared_from_this<layered_request_handler<Req, Res>>::weak_from_this;

    void handle_request_starting_from_layer(
        Req &&request, eclock::time_point timeout_end, result_handler<Res> &&res_handler,
        typename std::vector<std::shared_ptr<request_handler<Req, Res>>>::iterator layer_iter);

    event_loop &evloop;
    std::vector<std::shared_ptr<request_handler<Req, Res>>> layers_req_handlers;
    std::function<bool(std::error_code)> next_allowed_predicate;
};


template<typename Req, typename Res>
layered_request_handler<Req, Res>::layered_request_handler(
    event_loop &evloop, std::vector<std::shared_ptr<request_handler<Req, Res>>> &&layers_req_handlers,
    std::function<bool(std::error_code)> &&next_allowed_predicate)
    : evloop(evloop), layers_req_handlers(std::move(layers_req_handlers)),
    next_allowed_predicate(std::move(next_allowed_predicate))
{
}


template<typename Req, typename Res>
void layered_request_handler<Req, Res>::handle_request(Req &&request, std::chrono::nanoseconds timeout, result_handler<Res> &&res_handler)
{
    auto timeout_end = time_point_sat_add(evloop.get_etime(), timeout);
    handle_request_starting_from_layer(
        std::move(request), timeout_end, std::move(res_handler), layers_req_handlers.begin());
}


template<typename Req, typename Res>
void layered_request_handler<Req, Res>::handle_request_starting_from_layer(
    Req &&request, eclock::time_point timeout_end, result_handler<Res> &&res_handler,
    typename std::vector<std::shared_ptr<request_handler<Req, Res>>>::iterator layer_iter)
{
    using std::move;
    auto request_copy = request;
    auto time_left = std::max(timeout_end - evloop.get_etime(), std::chrono::nanoseconds(0));
    (*layer_iter)->handle_request(
        move(request_copy),
        time_left,
        [req_handler_wptr = weak_from_this(), layer_iter, request = std::move(request), timeout_end, res_handler = move(res_handler)](auto res) mutable {
            if (res.is_ok()) {
                res_handler(move(res));
            } else {
                auto req_handler_ptr = req_handler_wptr.lock();
                if (req_handler_ptr && req_handler_ptr->next_allowed_predicate(res.get_error())) {
                    auto next_layer_iter = std::next(layer_iter);
                    if (next_layer_iter != req_handler_ptr->layers_req_handlers.end()) {
                        req_handler_ptr->handle_request_starting_from_layer(
                            std::move(request), timeout_end, std::move(res_handler), next_layer_iter);
                    } else {
                        res_handler(move(res));
                    }
                } else {
                    res_handler(move(res));
                }
            }
        });
}

}


template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_layered_request_handler(
    event_loop &evloop, std::vector<std::shared_ptr<request_handler<Req, Res>>> &&layers_req_handlers,
    std::function<bool(std::error_code)> &&next_allowed_predicate)
{
    if (layers_req_handlers.empty()) {
        return make_const_response_request_handler<Req, Res>(evloop, raw_error_result(std::errc::not_supported));
    }

    using namespace layered_request_handler_impl;
    return std::make_shared<layered_request_handler<Req, Res>>(
        evloop, std::move(layers_req_handlers), std::move(next_allowed_predicate));
}


template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_layered_request_handler(
    event_loop &evloop, std::vector<std::shared_ptr<request_handler<Req, Res>>> &&layers_req_handlers)
{
    return make_layered_request_handler(
        evloop, std::move(layers_req_handlers),
        [](auto) {
            return true;
        });
}

}

#endif
