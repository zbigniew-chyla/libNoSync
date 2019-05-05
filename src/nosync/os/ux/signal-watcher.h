// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__SIGNAL_WATCHER_H
#define NOSYNC__OS__UX__SIGNAL_WATCHER_H

#include <functional>
#include <nosync/interface-type.h>
#include <nosync/result.h>
#include <nosync/os/ux/fd-watching-event-loop.h>
#include <memory>


namespace nosync::os::ux
{

result<std::shared_ptr<interface_type>> make_signal_watcher(
    fd_watching_event_loop &evloop, int signal_num, std::function<void()> &&signal_handler);

}

#endif /* NOSYNC__OS__UX__SIGNAL_WATCHER_H */
