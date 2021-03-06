// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__THROTTLING_REQUEST_HANDLER_IMPL_H
#define NOSYNC__THROTTLING_REQUEST_HANDLER_IMPL_H

#include <nosync/result-handler.h>
#include <utility>

namespace nosync
{

namespace throttling_request_handler_impl
{

template<typename Req, typename Res>
class throttling_request_handler : public request_handler<Req, Res>, public std::enable_shared_from_this<throttling_request_handler<Req, Res>>
{
public:
    throttling_request_handler(
        event_loop &evloop, eclock::duration min_req_time_distance,
        std::shared_ptr<request_handler<Req, Res>> &&base_handler);

    void handle_request(
        Req &&request, eclock::duration timeout,
        result_handler<Res> &&response_handler) override;

private:
    using std::enable_shared_from_this<throttling_request_handler<Req, Res>>::shared_from_this;

    event_loop &evloop;
    eclock::duration min_req_time_distance;
    std::shared_ptr<request_handler<Req, Res>> base_handler;
    eclock::time_point min_allowed_req_time;
};


template<typename Req, typename Res>
throttling_request_handler<Req, Res>::throttling_request_handler(
    event_loop &evloop, eclock::duration min_req_time_distance,
    std::shared_ptr<request_handler<Req, Res>> &&base_handler)
    : evloop(evloop), min_req_time_distance(min_req_time_distance), base_handler(std::move(base_handler)),
    min_allowed_req_time(eclock::time_point::min())
{
}


template<typename Req, typename Res>
void throttling_request_handler<Req, Res>::handle_request(
    Req &&request, eclock::duration timeout,
    result_handler<Res> &&response_handler)
{
    const auto now = evloop.get_etime();
    if (now >= min_allowed_req_time) {
        min_allowed_req_time = now + min_req_time_distance;
        base_handler->handle_request(std::move(request), timeout, std::move(response_handler));
    } else {
        evloop.invoke_at(
            min_allowed_req_time,
            [handler = shared_from_this(), request = std::move(request), timeout, response_handler = std::move(response_handler)]() mutable {
                handler->handle_request(std::move(request), timeout, std::move(response_handler));
            });
    }
}

}


template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_throttling_request_handler(
    event_loop &evloop, eclock::duration min_req_time_distance,
    std::shared_ptr<request_handler<Req, Res>> &&base_handler)
{
    return std::make_shared<throttling_request_handler_impl::throttling_request_handler<Req, Res>>(
        evloop, min_req_time_distance, std::move(base_handler));
}

}

#endif
