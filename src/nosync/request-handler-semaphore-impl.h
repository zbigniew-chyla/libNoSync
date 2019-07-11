// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__REQUEST_HANDLER_SEMAPHORE_IMPL_H
#define NOSYNC__REQUEST_HANDLER_SEMAPHORE_IMPL_H

#include <nosync/memory-utils.h>
#include <nosync/raw-error-result.h>
#include <nosync/time-utils.h>
#include <system_error>
#include <utility>


namespace nosync
{

namespace sequest_handler_semaphore_impl
{

template<typename Req, typename Res>
std::function<void(eclock::duration, std::function<void()>)> make_type_erased_request_launching_function(
    event_loop &evloop, std::shared_ptr<request_handler<Req, Res>> req_handler,
    Req &&request, result_handler<Res> &&res_handler)
{
    using std::move;

    return [&evloop, weak_req_handler = weak_from_shared(move(req_handler)), request = move(request), res_handler = move(res_handler)](
        eclock::duration timeout, std::function<void()> continue_func) mutable {
        auto shared_req_handler = weak_req_handler.lock();
        if (shared_req_handler) {
            shared_req_handler->handle_request(
                move(request), timeout,
                [res_handler = move(res_handler), continue_func = move(continue_func)](auto res) {
                    res_handler(move(res));
                    continue_func();
                });
        } else {
            invoke_later(
                evloop,
                [res_handler = move(res_handler), continue_func = move(continue_func)]() {
                    res_handler(raw_error_result(std::errc::operation_canceled));
                    continue_func();
                });
        }
    };
}

}


template<typename Req, typename Res>
void request_handler_semaphore::handle_request_with_acquired_semaphore(
    std::shared_ptr<request_handler<Req, Res>> req_handler, Req &&request,
    eclock::duration timeout, result_handler<Res> &&res_handler)
{
    using std::move;

    if (sem_value != 0) {
        --sem_value;
        req_handler->handle_request(
            move(request), timeout,
            [weak_sem = weak_from_this(), res_handler = move(res_handler)](auto res) mutable {
                res_handler(move(res));
                res_handler = nullptr;
                auto shared_sem = weak_sem.lock();
                if (shared_sem) {
                    shared_sem->handle_release();
                }
            });
    } else {
        auto shared_res_handler = std::make_shared<result_handler<Res>>(move(res_handler));
        pending_requests.push_request(
            sequest_handler_semaphore_impl::make_type_erased_request_launching_function<Req, Res>(
                evloop, move(req_handler), move(request),
                [shared_res_handler](auto res) {
                    (*shared_res_handler)(move(res));
                }),
            timeout,
            [weak_sem = weak_from_this(), shared_res_handler](auto res) {
                if (res.is_ok()) {
                    auto shared_sem = weak_sem.lock();
                    if (shared_sem) {
                        shared_sem->handle_release();
                    }
                } else {
                    (*shared_res_handler)(raw_error_result(res));
                }
            });
    }
}

}

#endif /* NOSYNC__REQUEST_HANDLER_SEMAPHORE_IMPL_H */
