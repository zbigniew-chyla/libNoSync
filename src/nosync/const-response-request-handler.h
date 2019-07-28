// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__CONST_RESPONSE_REQUEST_HANDLER_H
#define NOSYNC__CONST_RESPONSE_REQUEST_HANDLER_H

#include <memory>
#include <nosync/request-handler.h>


namespace nosync
{

/*!
Make request_handler<> returning a copy of the same response for each request.
*/
template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_const_response_request_handler(
    event_loop &evloop, result<Res> &&response);

}

#include <nosync/const-response-request-handler-impl.h>

#endif
