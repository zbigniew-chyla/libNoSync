// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__INT_RANGE_H
#define NOSYNC__INT_RANGE_H

#include <iterator>
#include <type_traits>


namespace nosync
{

template<typename T>
class int_range_iterator
{
public:
    static_assert(std::is_integral_v<T>);

    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::make_signed_t<T>;
    using pointer = const T *;
    using reference = const T &;

    explicit constexpr int_range_iterator(T value = T());

    constexpr T operator*() const;

    constexpr bool operator==(const int_range_iterator<T> &other) const;
    constexpr bool operator!=(const int_range_iterator<T> &other) const;

    constexpr int_range_iterator &operator++();

private:
    T value;
};


template<typename T>
class int_range
{
public:
    static_assert(std::is_integral_v<T>);

    explicit constexpr int_range(T upper_bound);

    constexpr int_range_iterator<T> begin() const;
    constexpr int_range_iterator<T> end() const;

private:
    T upper_bound;
};

}

#include <nosync/int-range-impl.h>

#endif
