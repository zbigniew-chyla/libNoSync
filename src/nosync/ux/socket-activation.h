// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC_UX__SOCKET_ACTIVATION_H
#define NOSYNC_UX__SOCKET_ACTIVATION_H

#include <nosync/ux/owned-fd.h>
#include <vector>


namespace nosync::ux
{

std::vector<owned_fd> takeover_socket_activated_fds();

}

#endif /* NOSYNC_UX__SOCKET_ACTIVATION_H */
