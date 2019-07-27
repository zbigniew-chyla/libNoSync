// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__REQUEST_HANDLER_SEMAPHORE_H
#define NOSYNC__REQUEST_HANDLER_SEMAPHORE_H

#include <functional>
#include <memory>
#include <nosync/eclock.h>
#include <nosync/event-loop.h>
#include <nosync/interface-type.h>
#include <nosync/request-handler.h>
#include <nosync/requests-queue.h>


namespace nosync
{

/*!
Async variant of semaphore that can be used with any request_handler<> types.
*/
class request_handler_semaphore : public std::enable_shared_from_this<request_handler_semaphore>, public interface_type
{
public:
    /*!
    Call req_handler->handle_request() with specified parameters, keeping the semaphore acquired during processing.

    The semaphore is acquired before calling handle_request() (if needed, the call is postponed
    until the semaphore's value becomes non-zero) and released after the call to res_handler
    (with the request's result) returns.
    */
    template<typename Req, typename Res>
    void handle_request_with_acquired_semaphore(
        const std::shared_ptr<request_handler<Req, Res>> &req_handler,
        Req &&request, eclock::duration timeout, result_handler<Res> &&res_handler);

protected:
    request_handler_semaphore(event_loop &evloop, unsigned init_value);

private:
    void handle_release();

    event_loop &evloop;
    unsigned sem_value;
    requests_queue<std::function<void(eclock::duration, std::function<void()>)>, void> pending_requests;
};


/*!
Create semaphore for request_handler<> objects, allowing for synchronization of processing different requests.
*/
std::shared_ptr<request_handler_semaphore> make_request_handler_semaphore(event_loop &evloop, unsigned init_value = 1);

}

#include <nosync/request-handler-semaphore-impl.h>

#endif
