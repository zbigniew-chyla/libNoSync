// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__NET_UTILS_H
#define NOSYNC__NET_UTILS_H

#include <nosync/owned-fd.h>
#include <nosync/socket-address.h>
#include <nosync/ux/net-utils.h>


namespace nosync
{

using ux::get_local_socket_domain;

using ux::make_local_abstract_socket_address;
using ux::make_local_filesystem_socket_address;

using ux::make_ipv4_socket_address;
using ux::make_ipv4_localhost_socket_address;

using ux::open_stream_socket;
using ux::open_datagram_socket;

using ux::open_connected_datagram_socket;
using ux::open_bound_datagram_socket;

using ux::open_connected_stream_socket;
using ux::open_listening_stream_socket;

using ux::receive_datagram_via_socket;

using ux::is_stream_socket;
using ux::is_datagram_socket;

using ux::get_socket_int_option;

using ux::bind_local_socket_to_auto_abstract_path;

using ux::connect_local_socket_to_abstract_path;

using ux::open_local_abstract_connected_stream_socket;
using ux::open_local_abstract_listening_stream_socket;

}

#endif /* NOSYNC__NET_UTILS_H */
