// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC_UNIX__SOCKET_DATAGRAMS_RESPONDING_ACCEPTOR_H
#define NOSYNC_UNIX__SOCKET_DATAGRAMS_RESPONDING_ACCEPTOR_H

#include <cstddef>
#include <functional>
#include <memory>
#include <nosync/bytes-io.h>
#include <nosync/interface-type.h>
#include <nosync/ux/fd-watching-event-loop.h>
#include <nosync/ux/owned-fd.h>
#include <string>


namespace nosync::ux
{

std::shared_ptr<interface_type> make_socket_datagrams_responding_acceptor(
    fd_watching_event_loop &evloop, owned_fd &&sock_fd, std::size_t max_request_datagram_size,
    std::function<void(std::shared_ptr<bytes_io> &&)> &&datagrams_processor);

}

#endif /* NOSYNC_UNIX__SOCKET_DATAGRAMS_RESPONDING_ACCEPTOR_H */
