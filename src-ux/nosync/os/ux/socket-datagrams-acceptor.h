// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__SOCKET_DATAGRAMS_ACCEPTOR_H
#define NOSYNC__OS__UX__SOCKET_DATAGRAMS_ACCEPTOR_H

#include <functional>
#include <nosync/result.h>
#include <nosync/interface-type.h>
#include <nosync/result-handler.h>
#include <nosync/os/ux/fd-watcher.h>
#include <nosync/os/ux/owned-fd.h>
#include <nosync/os/ux/shared-fd.h>
#include <nosync/os/ux/socket-address.h>
#include <memory>
#include <string>
#include <tuple>


namespace nosync::os::ux
{

/*!
Take ownership of datagram socket and handle incoming datagrams.

The function takes ownership of non-blocking socket (bound to some address)
and accepts all incoming datagrams, for each one calling provided handler with
sender's address and datagram content as input.

Datagrams are being accepted as long as the returned object is alive.
*/
std::shared_ptr<interface_type> make_full_socket_datagrams_acceptor(
    fd_watcher &watcher, owned_fd &&sock_fd, std::size_t max_datagram_size,
    result_handler<std::tuple<std::unique_ptr<socket_address>, std::string>> &&datagrams_handler);


/*!
Simplified variant of make_full_socket_datagrams_acceptor.

This function should be used if the user is not interested in failures from
recvfrom(). In this case the handler function gets plain source address and
datagram data as the parameters.
*/
std::shared_ptr<interface_type> make_socket_datagrams_acceptor(
    fd_watcher &watcher, owned_fd &&sock_fd, std::size_t max_datagram_size,
    std::function<void(std::unique_ptr<socket_address>, std::string)> &&datagrams_handler);

}

#endif
