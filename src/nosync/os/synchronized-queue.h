// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__SYNCHRONIZED_QUEUE_H
#define NOSYNC__OS__SYNCHRONIZED_QUEUE_H

#include <condition_variable>
#include <deque>
#include <mutex>


namespace nosync::os
{

template<typename T>
class synchronized_queue
{
public:
    synchronized_queue();

    void push(T element);
    T pop();
    void enqueue(T element);
    T dequeue();

private:
    std::deque<T> elements;
    std::mutex elements_mutex;
    std::condition_variable elements_present_cv;
};

}

#include <nosync/os/synchronized-queue-impl.h>

#endif /* NOSYNC__OS__SYNCHRONIZED_QUEUE_H */
