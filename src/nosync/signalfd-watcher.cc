// This file is part of libnosync library. See LICENSE file for license details.
#include <nosync/result-utils.h>
#include <nosync/signal-watcher.h>
#include <nosync/signalfd-watcher.h>

using std::function;
using std::move;
using std::shared_ptr;


namespace nosync
{

shared_ptr<interface_type> make_signalfd_watcher(
    fd_watching_event_loop &evloop, int signal_num, function<void()> &&signal_handler)
{
    return get_result_value_or_throw(make_signal_watcher(evloop, signal_num, move(signal_handler)));
}

}
