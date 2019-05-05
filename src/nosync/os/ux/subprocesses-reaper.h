// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__SUBPROCESSES_REAPER_H
#define NOSYNC__OS__UX__SUBPROCESSES_REAPER_H

#include <functional>
#include <nosync/request-handler.h>
#include <nosync/result.h>
#include <nosync/os/ux/fd-watching-event-loop.h>
#include <memory>
#include <sys/types.h>


namespace nosync::os::ux
{

/*!
Create object which collects information about exited sub-processes.

The generated request_handler<> automatically collects information about all
exited subprocesses (by watching SIGCHLD signals) and walling waitpid().
Additionally one can request exit status of exited sub-process by sending
request with subprocess pid.

Requesting for status should be done immediately after starting a sub-process
as the reaper quietly ignores exit statuses for sub-processes for which there
are no pending requests.

Normally there should be only one subprocesses reaper used for the whole
application process.

Note: SIGCHLD signal should be blocked when subprocess reaper is used.
*/
result<std::shared_ptr<request_handler<pid_t, int>>> make_subprocesses_reaper(fd_watching_event_loop &evloop);

}

#endif /* NOSYNC__OS__UX__SUBPROCESSES_REAPER_H */
