// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__SEMAPHORE_LOCKING_REQUEST_HANDLER_H
#define NOSYNC__SEMAPHORE_LOCKING_REQUEST_HANDLER_H

#include <memory>
#include <nosync/request-handler.h>
#include <nosync/request-handler-semaphore.h>


namespace nosync
{

/*!
Create request_handler<> decorator that keeps semaphore lock during processing of each request.
*/
template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_semaphore_locking_request_handler(
    std::shared_ptr<request_handler_semaphore> sem,
    std::shared_ptr<request_handler<Req, Res>> &&base_req_handler);

}

#include <nosync/semaphore-locking-request-handler-impl.h>

#endif /* NOSYNC__SEMAPHORE_LOCKING_REQUEST_HANDLER_H */
