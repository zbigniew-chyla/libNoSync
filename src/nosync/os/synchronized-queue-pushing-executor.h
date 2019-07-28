// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__SYNCHRONIZED_QUEUE_PUSHING_EXECUTOR_H
#define NOSYNC__OS__SYNCHRONIZED_QUEUE_PUSHING_EXECUTOR_H

#include <functional>
#include <memory>
#include <nosync/os/synchronized-queue.h>


namespace nosync::os
{

std::function<void(std::function<void()>)> make_synchronized_queue_pushing_executor(
    std::shared_ptr<synchronized_queue<std::function<void()>>> out_tasks_queue);

}

#endif
