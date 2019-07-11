// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__IDLE_NOTIFYING_REQUEST_HANDLER_H
#define NOSYNC__IDLE_NOTIFYING_REQUEST_HANDLER_H

#include <functional>
#include <memory>
#include <nosync/request-handler.h>


namespace nosync
{

/*!
Decorate request_handler<> object with sending notifications about changes of its idleness.

Each time the idle state of the returned request_handler<> changes, idle_change_listener is called
with new value of the "idle" flag as an argument. The object is considered "idle" when it's not
processing any requests (request processing starts with handle_request() call and ends after
calling the corresponding result_handler<> object).

The idle_change_listener is called:
 - with false argument - immediately before becoming non-idle,
 - with true argument - immediately after becoming idle.
*/
template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_idle_notifying_request_handler(
    std::shared_ptr<request_handler<Req, Res>> &&base_req_handler,
    std::function<void(bool)> &&idle_change_listener);


/*!
Decorate request_handler<> object with sending notifications about becoming idle.

This is simplified variant of "full" make_idle_notifying_request_handler() that sends notifications
only when the object becomes idle (not when it becomes non-idle).
*/
template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_idle_notifying_request_handler(
    std::shared_ptr<request_handler<Req, Res>> &&base_req_handler,
    std::function<void()> &&idle_listener);

}

#include <nosync/idle-notifying-request-handler-impl.h>

#endif /* NOSYNC__IDLE_NOTIFYING_REQUEST_HANDLER_H */
