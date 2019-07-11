// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__LAYERED_REQUEST_HANDLER_H
#define NOSYNC__LAYERED_REQUEST_HANDLER_H

#include <memory>
#include <nosync/event-loop.h>
#include <nosync/request-handler.h>
#include <vector>


namespace nosync
{

template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_layered_request_handler(
    event_loop &evloop, std::vector<std::shared_ptr<request_handler<Req, Res>>> &&layers_req_handlers,
    std::function<bool(std::error_code)> &&next_allowed_predicate);


/*!
Create request_handler<> that processes requests using a vector of underlying request handlers.

When processing a request, its copy is (sequentially) passed to consecutive
underlying request handlers until either a) ok result is received, b) there
are no more handlers left.
*/
template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_layered_request_handler(
    event_loop &evloop, std::vector<std::shared_ptr<request_handler<Req, Res>>> &&layers_req_handlers);

}

#include <nosync/layered-request-handler-impl.h>

#endif /* NOSYNC__LAYERED_REQUEST_HANDLER_H */
