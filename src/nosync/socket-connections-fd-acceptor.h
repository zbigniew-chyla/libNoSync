// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__SOCKET_CONNECTIONS_FD_ACCEPTOR_H
#define NOSYNC__SOCKET_CONNECTIONS_FD_ACCEPTOR_H

#include <nosync/fd-watcher.h>
#include <nosync/owned-fd.h>
#include <nosync/shared-fd.h>
#include <nosync/ux/socket-connections-fd-acceptor.h>


namespace nosync
{

using ux::make_full_socket_connections_fd_acceptor;

using ux::make_socket_connections_fd_acceptor;

}

#endif /* NOSYNC__SOCKET_CONNECTIONS_FD_ACCEPTOR_H */
