// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__SEMAPHORE_PROTECTED_REQUEST_HANDLER_H
#define NOSYNC__SEMAPHORE_PROTECTED_REQUEST_HANDLER_H

#include <memory>
#include <nosync/event-loop.h>
#include <nosync/request-handler.h>
#include <nosync/request-handler-semaphore.h>


namespace nosync
{

/*!
Create request_handler<> decorator that keeps acquired semaphore during processing of each request.
*/
template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_semaphore_protected_request_handler(
    std::shared_ptr<request_handler_semaphore> sem,
    std::shared_ptr<request_handler<Req, Res>> &&base_req_handler);


/*!
Create request_handler<> decorator that keeps acquired semaphore during processing of each request.

This variant automatically creates semaphore object, initializing it with semaphore_init_value.
*/
template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_semaphore_protected_request_handler(
    event_loop &evloop, unsigned semaphore_init_value,
    std::shared_ptr<request_handler<Req, Res>> &&base_req_handler);

}

#include <nosync/semaphore-protected-request-handler-impl.h>

#endif
