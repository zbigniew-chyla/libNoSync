// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__UX__IO_UTILS_H
#define NOSYNC__UX__IO_UTILS_H

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

#endif /* NOSYNC__UX__IO_UTILS_H */
