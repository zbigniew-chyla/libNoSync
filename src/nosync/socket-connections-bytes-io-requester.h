// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__SOCKET_CONNECTIONS_BYTES_IO_REQUESTER_H
#define NOSYNC__SOCKET_CONNECTIONS_BYTES_IO_REQUESTER_H

#include <nosync/fd-watching-event-loop.h>
#include <nosync/socket-address.h>
#include <nosync/ux/socket-connections-bytes-io-requester.h>


namespace nosync
{

using ux::request_socket_connection_bytes_io;

using ux::make_socket_connections_bytes_io_requester;

}

#endif /* NOSYNC__SOCKET_CONNECTIONS_BYTES_IO_REQUESTER_H */
