// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__SYNCHRONIZED_QUEUE_PUSHING_CONSUMER_H
#define NOSYNC__OS__SYNCHRONIZED_QUEUE_PUSHING_CONSUMER_H

#include <functional>
#include <memory>
#include <nosync/os/synchronized-queue.h>


namespace nosync::os
{

template<typename T>
std::function<void(T)> make_synchronized_queue_pushing_consumer(
    std::shared_ptr<synchronized_queue<T>> out_tasks_queue);

}

#include <nosync/os/synchronized-queue-pushing-consumer-impl.h>

#endif
