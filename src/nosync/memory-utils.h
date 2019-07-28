// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__MEMORY_UTILS_H
#define NOSYNC__MEMORY_UTILS_H

#include <memory>
#include <type_traits>


namespace nosync
{

/*!
Create std::shared_ptr<T> by move-constructing T object on heap from an argument.

To avoid silent unintended copying, the function supports only move-constructible types.
*/
template<typename T>
std::enable_if_t<!std::is_reference<T>::value && std::is_move_constructible<T>::value, std::shared_ptr<T>> move_to_shared(T &&obj);


template<typename T>
std::weak_ptr<T> weak_from_shared(const std::shared_ptr<T> &ptr);

template<typename T>
[[deprecated("use std::enable_shared_from_this::weak_from this instead")]] std::weak_ptr<T> weak_from_that(T *obj);

template<typename T, typename F>
auto make_weak_this_func_proxy(T *self, F func);

}

#include <nosync/memory-utils-impl.h>

#endif
