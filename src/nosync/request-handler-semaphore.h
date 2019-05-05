// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__REQUEST_HANDLER_SEMAPHORE_H
#define NOSYNC__REQUEST_HANDLER_SEMAPHORE_H

#include <nosync/eclock.h>
#include <nosync/event-loop.h>
#include <nosync/interface-type.h>
#include <nosync/request-handler.h>
#include <nosync/timeouting-tasks-queue.h>


namespace nosync
{

class request_handler_semaphore : public std::enable_shared_from_this<request_handler_semaphore>, public interface_type
{
public:
    template<typename Req, typename Res>
    void handle_request_with_lock(
        std::shared_ptr<request_handler<Req, Res>> req_handler, Req &&request,
        eclock::duration timeout, result_handler<Res> &&res_handler);

protected:
    request_handler_semaphore(event_loop &evloop, unsigned init_value);

private:
    event_loop &evloop;
    unsigned sem_value;
    timeouting_tasks_queue pending_requests_launching_tasks;
};


/*!
Asynchronous counterpart of a semaphore, allowing for synchronization of processing requests by request_handler<> objects.
*/
std::shared_ptr<request_handler_semaphore> make_request_handler_semaphore(event_loop &evloop, unsigned init_value = 1);

}

#include <nosync/request-handler-semaphore-impl.h>

#endif /* NOSYNC__REQUEST_HANDLER_SEMAPHORE_H */
