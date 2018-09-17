// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__SOCKET_DATAGRAMS_ACCEPTOR_H
#define NOSYNC__SOCKET_DATAGRAMS_ACCEPTOR_H

#include <nosync/fd-watcher.h>
#include <nosync/owned-fd.h>
#include <nosync/shared-fd.h>
#include <nosync/socket-address.h>
#include <nosync/ux/socket-datagrams-acceptor.h>


namespace nosync
{

using ux::make_full_socket_datagrams_acceptor;

using ux::make_socket_datagrams_acceptor;

}

#endif /* NOSYNC__SOCKET_DATAGRAMS_ACCEPTOR_H */
