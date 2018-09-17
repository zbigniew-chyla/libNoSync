// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC_UX__IO_UTILS_H
#define NOSYNC_UX__IO_UTILS_H

#include <array>
#include <nosync/result.h>
#include <nosync/ux/owned-fd.h>


namespace nosync::ux
{

/*!
Create pipe to be used inside the current process (with O_CLOEXEC and O_NONBLOCK flags set).
*/
result<std::array<owned_fd, 2>> open_pipe();

}

#endif /* NOSYNC_UX__IO_UTILS_H */
