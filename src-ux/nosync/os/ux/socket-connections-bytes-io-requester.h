// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__SOCKET_CONNECTIONS_BYTES_IO_REQUESTER_H
#define NOSYNC__OS__UX__SOCKET_CONNECTIONS_BYTES_IO_REQUESTER_H

#include <memory>
#include <nosync/bytes-io.h>
#include <nosync/eclock.h>
#include <nosync/request-handler.h>
#include <nosync/result-handler.h>
#include <nosync/os/ux/fd-watching-event-loop.h>
#include <nosync/os/ux/socket-address.h>


namespace nosync::os::ux
{

void request_socket_connection_bytes_io(
    fd_watching_event_loop &evloop, const std::shared_ptr<socket_address> &addr,
    eclock::duration timeout, result_handler<std::shared_ptr<bytes_io>> &&res_handler);

std::shared_ptr<request_handler<std::shared_ptr<socket_address>, std::shared_ptr<bytes_io>>> make_socket_connections_bytes_io_requester(
    fd_watching_event_loop &evloop);

}

#endif
