// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__SYNCHRONIZED_QUEUE_PUSHING_CONSUMER_IMPL_H
#define NOSYNC__OS__SYNCHRONIZED_QUEUE_PUSHING_CONSUMER_IMPL_H

#include <utility>


namespace nosync::os
{

template<typename T>
std::function<void(T)> make_synchronized_queue_pushing_consumer(
    std::shared_ptr<synchronized_queue<T>> output_queue)
{
    return [output_queue = std::move(output_queue)](T element) {
        output_queue->push(std::move(element));
    };
}

}

#endif
