// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__SYNCHRONIZED_QUEUE_H
#define NOSYNC__OS__SYNCHRONIZED_QUEUE_H

#include <chrono>
#include <condition_variable>
#include <deque>
#include <limits>
#include <memory>
#include <mutex>


namespace nosync::os
{

template<typename T>
class synchronized_queue
{
public:
    synchronized_queue();

    void push(T &&element);
    T pop();
    void enqueue(T element);
    T dequeue();

    std::deque<T> pop_group(std::size_t max_group_size = std::numeric_limits<std::size_t>::max());
    std::deque<T> try_pop_group(
        std::chrono::nanoseconds timeout,
        std::size_t max_group_size = std::numeric_limits<std::size_t>::max());

private:
    std::deque<T> pop_group_unlocked(std::size_t max_group_size);

    std::deque<T> elements;
    std::mutex elements_mutex;
    std::condition_variable elements_present_cv;
};


template<typename T>
std::shared_ptr<synchronized_queue<T>> make_synchronized_queue();

}

#include <nosync/os/synchronized-queue-impl.h>

#endif
