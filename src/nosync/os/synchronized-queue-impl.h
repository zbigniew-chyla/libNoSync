// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__SYNCHRONIZED_QUEUE_IMPL_H
#define NOSYNC__OS__SYNCHRONIZED_QUEUE_IMPL_H

#include <iterator>


namespace nosync::os
{

template<typename T>
synchronized_queue<T>::synchronized_queue()
    : elements(), elements_mutex(), elements_present_cv()
{
}


template<typename T>
void synchronized_queue<T>::push(T &&element)
{
    std::scoped_lock lock(elements_mutex);
    bool was_empty = elements.empty();
    elements.push_back(std::move(element));
    if (was_empty) {
        elements_present_cv.notify_all();
    }
}


template<typename T>
T synchronized_queue<T>::pop()
{
    std::unique_lock<std::mutex> lock(elements_mutex);
    elements_present_cv.wait(
        lock,
        [&]() {
            return !elements.empty();
        });

    auto element = std::move(elements.front());
    elements.pop_front();

    return element;
}


template<typename T>
void synchronized_queue<T>::enqueue(T element)
{
    push(std::move(element));
}


template<typename T>
T synchronized_queue<T>::dequeue()
{
    return pop();
}


template<typename T>
std::deque<T> synchronized_queue<T>::pop_group(std::size_t max_group_size)
{
    std::unique_lock<std::mutex> lock(elements_mutex);
    elements_present_cv.wait(
        lock,
        [&]() {
            return !elements.empty();
        });

    return pop_group_unlocked(max_group_size);
}


template<typename T>
std::deque<T> synchronized_queue<T>::try_pop_group(
    std::chrono::nanoseconds timeout, std::size_t max_group_size)
{
    std::unique_lock<std::mutex> lock(elements_mutex);
    elements_present_cv.wait_for(
        lock, timeout,
        [&]() {
            return !elements.empty();
        });

    return pop_group_unlocked(max_group_size);
}


template<typename T>
std::deque<T> synchronized_queue<T>::pop_group_unlocked(std::size_t max_group_size)
{
    std::deque<T> group;
    if (elements.size() <= max_group_size) {
        swap(group, elements);
    } else {
        group.insert(
            group.end(),
            std::move_iterator(elements.begin()),
            std::move_iterator(elements.begin() + max_group_size));
        elements.erase(elements.begin(), elements.begin() + max_group_size);
    }

    return group;
}

}

#endif /* NOSYNC__OS__SYNCHRONIZED_QUEUE_IMPL_H */
