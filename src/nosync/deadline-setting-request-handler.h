// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__DEADLINE_SETTING_REQUEST_HANDLER_H
#define NOSYNC__DEADLINE_SETTING_REQUEST_HANDLER_H

#include <memory>
#include <nosync/eclock.h>
#include <nosync/event-loop.h>
#include <nosync/request-handler.h>


namespace nosync
{

/*!
Decorate request_handler<> adjusting each request's timeout such that it doesn't exceed the deadline.

For each request submitted before the deadline the decorator modifies its timeout such that
etime + timeout is not greater than the deadline. For requests submitted after or at the
deadline a timeout set to zero.
*/
template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_deadline_setting_request_handler(
    event_loop &evloop, std::shared_ptr<request_handler<Req, Res>> &&base_req_handler,
    eclock::time_point deadline);

}

#include <nosync/deadline-setting-request-handler-impl.h>

#endif
