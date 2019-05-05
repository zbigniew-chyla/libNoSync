// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__REQUEST_HANDLER_SEMAPHORE_IMPL_H
#define NOSYNC__REQUEST_HANDLER_SEMAPHORE_IMPL_H

#include <nosync/time-utils.h>
#include <system_error>
#include <utility>


namespace nosync
{

request_handler_semaphore::request_handler_semaphore(event_loop &evloop, unsigned init_value)
    : evloop(evloop), sem_value(init_value), pending_requests_launching_tasks(evloop)
{
}


template<typename Req, typename Res>
void request_handler_semaphore::handle_request_with_lock(
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
                    if (shared_sem->pending_requests_launching_tasks.has_tasks()) {
                        shared_sem->pending_requests_launching_tasks.run_next_task();
                    } else {
                        ++shared_sem->sem_value;
                    }
                }
            });
    } else {
        auto timeout_end = time_point_sat_add(evloop.get_etime(), timeout);
        auto weak_req_handler = weak_from_shared(req_handler);
        pending_requests_launching_tasks.push_task(
            timeout,
            [weak_sem = weak_from_this(), &evloop = evloop, timeout_end, weak_req_handler = move(weak_req_handler), request = move(request), res_handler = move(res_handler)](auto err) mutable {
                if (err) {
                    res_handler(raw_error_result(err));
                    return;
                }

                auto shared_req_handler = weak_req_handler.lock();
                if (shared_req_handler) {
                    auto time_left = std::max(timeout_end - evloop.get_etime(), eclock::duration(0));
                    shared_req_handler->handle_request(
                        move(request), time_left,
                        [weak_sem = move(weak_sem), res_handler = move(res_handler)](auto res) mutable {
                            res_handler(move(res));
                            res_handler = nullptr;
                            auto shared_sem = weak_sem.lock();
                            if (shared_sem) {
                                if (shared_sem->pending_requests_launching_tasks.has_tasks()) {
                                    shared_sem->pending_requests_launching_tasks.run_next_task();
                                } else {
                                    ++shared_sem->sem_value;
                                }
                            }
                        });
                } else {
                    res_handler(raw_error_result(std::errc::operation_canceled));
                }
            }
        );
    }
}


std::shared_ptr<request_handler_semaphore> make_request_handler_semaphore(event_loop &evloop, unsigned init_value)
{
    class request_handler_semaphore_impl : public request_handler_semaphore
    {
    public:
        request_handler_semaphore_impl(event_loop &evloop, unsigned init_value)
            : request_handler_semaphore(evloop, init_value)
        {
        }
    };

    return std::make_shared<request_handler_semaphore_impl>(evloop, init_value);
}

}

#endif /* NOSYNC__REQUEST_HANDLER_SEMAPHORE_IMPL_H */
