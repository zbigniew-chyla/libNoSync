// This file is part of libnosync library. See LICENSE file for license details.
#include <nosync/event-loop-based-mt-executor.h>
#include <nosync/event-loop-mt-executor.h>
#include <nosync/result-utils.h>

using std::function;


namespace nosync
{

function<void(function<void()>)> make_event_loop_based_mt_executor(fd_watching_event_loop &evloop)
{
    return get_result_value_or_throw(make_event_loop_mt_executor(evloop));
}

}
