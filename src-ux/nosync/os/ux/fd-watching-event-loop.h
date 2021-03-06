// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__FD_WATCHING_EVENT_LOOP_H
#define NOSYNC__OS__UX__FD_WATCHING_EVENT_LOOP_H

#include <nosync/event-loop.h>
#include <nosync/os/ux/fd-watcher.h>


namespace nosync::os::ux
{

/*!
Interface "aggregating" fd_watcher and event_loop.

This interface inherits from both fd_watcher and event_loop without adding any
extra operations. It's intended to be used in code which creates (or provides
access to) event loop implementation which handles both basic event
loop functionality and fd watching.
*/
class fd_watching_event_loop : public fd_watcher, public event_loop
{
};

}

#endif
