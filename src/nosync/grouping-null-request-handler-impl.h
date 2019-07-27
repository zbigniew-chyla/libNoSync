// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__GROUPING_NULL_REQUEST_HANDLER_IMPL_H
#define NOSYNC__GROUPING_NULL_REQUEST_HANDLER_IMPL_H

#include <nosync/memory-utils.h>
#include <nosync/requests-queue.h>
#include <nosync/result-handler.h>
#include <nosync/time-utils.h>
#include <utility>


namespace nosync
{

namespace grouping_null_request_handler_impl
{

template<typename Res>
class grouping_null_request_handler : public request_handler<std::nullptr_t, Res>, public std::enable_shared_from_this<grouping_null_request_handler<Res>>
{
public:
    grouping_null_request_handler(event_loop &evloop, std::shared_ptr<request_handler<std::nullptr_t, Res>> &&base_req_handler);

    void handle_request(std::nullptr_t &&request, eclock::duration timeout, result_handler<Res> &&res_handler) override;

private:
    using std::enable_shared_from_this<grouping_null_request_handler<Res>>::weak_from_this;

    event_loop &evloop;
    std::shared_ptr<request_handler<std::nullptr_t, Res>> base_req_handler;
    requests_queue<std::nullptr_t, Res> pending_requests;
    bool request_ongoing;
};


template<typename Res>
grouping_null_request_handler<Res>::grouping_null_request_handler(event_loop &evloop, std::shared_ptr<request_handler<std::nullptr_t, Res>> &&base_req_handler)
    : evloop(evloop), base_req_handler(std::move(base_req_handler)), pending_requests(evloop), request_ongoing(false)
{
}


template<typename Res>
void grouping_null_request_handler<Res>::handle_request(std::nullptr_t &&request, eclock::duration timeout, result_handler<Res> &&res_handler)
{
    if (request_ongoing) {
        pending_requests.push_request(std::move(request), timeout, std::move(res_handler));
        return;
    }

    request_ongoing = true;

    base_req_handler->handle_request(
        std::move(request), timeout,
        [&evloop = evloop, req_handler_wptr = weak_from_this(), res_handler = std::move(res_handler)](auto res) mutable {
            requests_queue<std::nullptr_t, Res> grouped_reqs(evloop);

            auto req_handler_ptr = req_handler_wptr.lock();
            if (req_handler_ptr) {
                while (req_handler_ptr->pending_requests.has_requests()) {
                    req_handler_ptr->pending_requests.pull_next_request_to_consumer(
                        [&](auto &&req, auto timeout, auto &&res_handler) {
                            grouped_reqs.push_request(std::move(req), timeout, std::move(res_handler));
                        });
                }
                req_handler_ptr->request_ongoing = false;
            }

            res_handler(res);
            res_handler = nullptr;

            while (grouped_reqs.has_requests()) {
                grouped_reqs.pull_next_request_to_consumer(
                    [&](auto &&, auto, auto &&res_handler) {
                        res_handler(res);
                    });
            }
        });
}

}


template<typename Res>
std::shared_ptr<request_handler<std::nullptr_t, Res>> make_grouping_null_request_handler(
    event_loop &evloop, std::shared_ptr<request_handler<std::nullptr_t, Res>> &&base_req_handler)
{
    return std::make_shared<grouping_null_request_handler_impl::grouping_null_request_handler<Res>>(
        evloop, std::move(base_req_handler));
}

}

#endif
