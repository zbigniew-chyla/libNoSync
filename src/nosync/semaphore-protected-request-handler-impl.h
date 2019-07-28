// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__SEMAPHORE_PROTECTED_REQUEST_HANDLER_IMPL_H
#define NOSYNC__SEMAPHORE_PROTECTED_REQUEST_HANDLER_IMPL_H

#include <nosync/func-request-handler.h>
#include <utility>


namespace nosync
{

template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_semaphore_protected_request_handler(
    std::shared_ptr<request_handler_semaphore> sem,
    std::shared_ptr<request_handler<Req, Res>> &&base_req_handler)
{
    using std::move;

    return make_func_request_handler<Req, Res>(
        [sem = move(sem), base_req_handler = move(base_req_handler)](Req &&request, eclock::duration timeout, result_handler<Res> &&res_handler) {
            sem->handle_request_with_acquired_semaphore(
                base_req_handler, move(request), timeout, move(res_handler));
        });
}


template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_semaphore_protected_request_handler(
    event_loop &evloop, unsigned semaphore_init_value,
    std::shared_ptr<request_handler<Req, Res>> &&base_req_handler)
{
    return make_semaphore_protected_request_handler(
        make_request_handler_semaphore(evloop, semaphore_init_value),
        std::move(base_req_handler));
}

}

#endif
