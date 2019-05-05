// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__FD_WATCH_UTILS_H
#define NOSYNC__OS__UX__FD_WATCH_UTILS_H

#include <nosync/eclock.h>
#include <nosync/result-handler.h>
#include <nosync/os/ux/fd-watching-event-loop.h>


namespace nosync::os::ux
{

void watch_fd_with_timeout(
    fd_watching_event_loop &evloop, int fd, fd_watch_mode mode, eclock::duration timeout,
    result_handler<void> &&res_handler);

}

#endif /* NOSYNC__OS__UX__FD_WATCH_UTILS_H */
