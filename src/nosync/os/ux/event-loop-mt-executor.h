// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__EVENT_LOOP_MT_EXECUTOR_H
#define NOSYNC__OS__UX__EVENT_LOOP_MT_EXECUTOR_H

#include <functional>
#include <nosync/result.h>
#include <nosync/os/ux/fd-watching-event-loop.h>


namespace nosync::os::ux
{

result<std::function<void(std::function<void()>)>> make_event_loop_mt_executor(fd_watching_event_loop &evloop);

}

#endif /* NOSYNC__OS__UX__EVENT_LOOP_MT_EXECUTOR_H */
