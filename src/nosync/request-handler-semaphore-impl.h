// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__REQUEST_HANDLER_SEMAPHORE_IMPL_H
#define NOSYNC__REQUEST_HANDLER_SEMAPHORE_IMPL_H

#include <nosync/memory-utils.h>
#include <nosync/raw-error-result.h>
#include <nosync/result-handler-utils.h>
#include <system_error>
#include <utility>


namespace nosync
{

namespace sequest_handler_semaphore_impl
{

template<typename Req, typename Res>
std::function<void(eclock::duration)> make_type_erased_request_launching_function(
    event_loop &evloop, const std::shared_ptr<request_handler<Req, Res>> &req_handler,
    Req &&request, result_handler<Res> &&res_handler)
{
    using std::move;

    return [&evloop, weak_req_handler = weak_from_shared(req_handler), request = move(request), res_handler = move(res_handler)](eclock::duration timeout) mutable {
        auto shared_req_handler = weak_req_handler.lock();
        if (shared_req_handler) {
            shared_req_handler->handle_request(move(request), timeout, move(res_handler));
        } else {
            invoke_result_handler_later(
                evloop, move(res_handler), raw_error_result(std::errc::operation_canceled));
        }
    };
}

}


template<typename Req, typename Res>
void request_handler_semaphore::handle_request_with_acquired_semaphore(
    const std::shared_ptr<request_handler<Req, Res>> &req_handler,
    Req &&request, eclock::duration timeout, result_handler<Res> &&res_handler)
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
        auto shared_res_handler = move_to_shared(move(res_handler));
        pending_requests.push_request(
            sequest_handler_semaphore_impl::make_type_erased_request_launching_function<Req, Res>(
                evloop, req_handler, move(request),
                [weak_sem = weak_from_this(), shared_res_handler](auto res) {
                    (*shared_res_handler)(move(res));
                    *shared_res_handler = nullptr;
                    auto shared_sem = weak_sem.lock();
                    if (shared_sem) {
                        shared_sem->handle_release();
                    }
                }),
            timeout,
            [shared_res_handler](auto res) {
                if (!res.is_ok()) {
                    (*shared_res_handler)(raw_error_result(res));
                }
            });
    }
}

}

#endif
