// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__SOCKET_ACTIVATION_H
#define NOSYNC__OS__UX__SOCKET_ACTIVATION_H

#include <nosync/os/ux/owned-fd.h>
#include <vector>


namespace nosync::os::ux
{

std::vector<owned_fd> takeover_socket_activated_fds();

}

#endif
