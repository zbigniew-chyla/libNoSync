// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC_UNIX__EVENT_LOOP_MT_EXECUTOR_H
#define NOSYNC_UNIX__EVENT_LOOP_MT_EXECUTOR_H

#include <functional>
#include <nosync/result.h>
#include <nosync/ux/fd-watching-event-loop.h>


namespace nosync::ux
{

result<std::function<void(std::function<void()>)>> make_event_loop_mt_executor(fd_watching_event_loop &evloop);

}

#endif /* NOSYNC_UNIX__EVENT_LOOP_MT_EXECUTOR_H */
