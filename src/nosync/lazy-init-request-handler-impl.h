// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__LAZY_INIT_REQUEST_HANDLER_IMPL_H
#define NOSYNC__LAZY_INIT_REQUEST_HANDLER_IMPL_H

#include <nosync/raw-error-result.h>
#include <nosync/requests-queue.h>
#include <system_error>
#include <utility>


namespace nosync
{

namespace lazy_init_request_handler_impl
{

template<typename Req, typename Res>
class lazy_init_request_handler : public request_handler<Req, Res>, public std::enable_shared_from_this<lazy_init_request_handler<Req, Res>>
{
public:
    lazy_init_request_handler(
        event_loop &evloop,
        std::shared_ptr<request_handler<std::nullptr_t, std::shared_ptr<request_handler<Req, Res>>>> &&base_req_handler_factory);

    void handle_request(
        Req &&request, eclock::duration timeout,
        result_handler<Res> &&response_handler) override;

private:
    using std::enable_shared_from_this<lazy_init_request_handler<Req, Res>>::weak_from_this;

    void start_base_req_handler_factory();

    event_loop &evloop;
    std::shared_ptr<request_handler<std::nullptr_t, std::shared_ptr<request_handler<Req, Res>>>> base_req_handler_factory;
    bool base_req_handler_factory_running;
    requests_queue<Req, Res> pre_init_pending_requests;
    std::shared_ptr<request_handler<Req, Res>> base_req_handler;
};


template<typename Req, typename Res>
lazy_init_request_handler<Req, Res>::lazy_init_request_handler(
    event_loop &evloop,
    std::shared_ptr<request_handler<std::nullptr_t, std::shared_ptr<request_handler<Req, Res>>>> &&base_req_handler_factory)
    : evloop(evloop), base_req_handler_factory(std::move(base_req_handler_factory)),
    base_req_handler_factory_running(false), pre_init_pending_requests(evloop), base_req_handler()
{
}


template<typename Req, typename Res>
void lazy_init_request_handler<Req, Res>::handle_request(
    Req &&request, eclock::duration timeout, result_handler<Res> &&res_handler)
{
    if (base_req_handler) {
        base_req_handler->handle_request(std::move(request), timeout, std::move(res_handler));
    } else {
        if (!base_req_handler_factory_running) {
            base_req_handler_factory_running = true;
            start_base_req_handler_factory();
        }
        pre_init_pending_requests.push_request(std::move(request), timeout, std::move(res_handler));
    }
}


template<typename Req, typename Res>
void lazy_init_request_handler<Req, Res>::start_base_req_handler_factory()
{
    base_req_handler_factory->handle_request(
        nullptr, eclock::duration::max(),
        [self_wptr = weak_from_this()](auto res) mutable {
            auto self = self_wptr.lock();
            if (!self) {
                return;
            }

            self->base_req_handler_factory.reset();
            self->base_req_handler_factory_running = false;

            self->base_req_handler = res.is_ok()
                ? (res.get_value()
                    ? std::move(res.get_value())
                    : make_const_response_request_handler<Req, Res>(self->evloop, raw_error_result(std::errc::no_message_available)))
                : make_const_response_request_handler<Req, Res>(self->evloop, raw_error_result(res));

            while (self->pre_init_pending_requests.has_requests()) {
                self->pre_init_pending_requests.pull_next_request_to_consumer(
                    [self](Req &&request, eclock::duration timeout, result_handler<Res> &&res_handler) {
                        self->base_req_handler->handle_request(std::move(request), timeout, std::move(res_handler));
                    });
            }
        });
}

}


template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_lazy_init_request_handler(
    event_loop &evloop,
    std::shared_ptr<request_handler<std::nullptr_t, std::shared_ptr<request_handler<Req, Res>>>> &&base_req_handler_factory)
{
    using namespace lazy_init_request_handler_impl;
    return std::make_shared<lazy_init_request_handler<Req, Res>>(evloop, std::move(base_req_handler_factory));
}

}

#endif
