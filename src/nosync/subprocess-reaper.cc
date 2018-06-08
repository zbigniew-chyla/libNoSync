// This file is part of libnosync library. See LICENSE file for license details.
#include <nosync/exceptions.h>
#include <nosync/result-utils.h>
#include <nosync/subprocess-reaper.h>
#include <nosync/subprocesses-reaper.h>

using std::shared_ptr;


namespace nosync
{

shared_ptr<request_handler<pid_t, int>> make_subprocess_reaper(fd_watching_event_loop &evloop)
{
    return get_result_value_or_throw(make_subprocesses_reaper(evloop));
}

}
