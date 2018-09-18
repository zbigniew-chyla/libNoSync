// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__SOCKET_CONNECTIONS_FD_REQUESTER_H
#define NOSYNC__SOCKET_CONNECTIONS_FD_REQUESTER_H

#include <nosync/fd-watching-event-loop.h>
#include <nosync/shared-fd.h>
#include <nosync/socket-address.h>
#include <nosync/ux/socket-connections-fd-requester.h>


namespace nosync
{

using ux::request_socket_connection_fd;

using ux::make_socket_connections_fd_requester;

}

#endif /* NOSYNC__SOCKET_CONNECTIONS_FD_REQUESTER_H */
