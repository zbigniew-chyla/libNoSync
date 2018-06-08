// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__SIGNAL_WATCHER_H
#define NOSYNC__SIGNAL_WATCHER_H

#include <functional>
#include <nosync/fd-watching-event-loop.h>
#include <nosync/interface-type.h>
#include <nosync/result.h>
#include <memory>


namespace nosync
{

result<std::shared_ptr<interface_type>> make_signal_watcher(
    fd_watching_event_loop &evloop, int signal_num, std::function<void()> &&signal_handler);

}

#endif /* NOSYNC__SIGNAL_WATCHER_H */
