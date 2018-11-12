// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__INT_RANGE_IMPL_H
#define NOSYNC__INT_RANGE_IMPL_H


namespace nosync
{

template<typename T>
constexpr int_range<T>::int_range(T upper_bound)
    : upper_bound(upper_bound)
{
}


template<typename T>
constexpr int_range_iterator<T> int_range<T>::begin() const
{
    return int_range_iterator(T());
}


template<typename T>
constexpr int_range_iterator<T> int_range<T>::end() const
{
    return int_range_iterator(upper_bound);
}


template<typename T>
constexpr int_range_iterator<T>::int_range_iterator(T value)
    : value(value)
{
}


template<typename T>
constexpr T int_range_iterator<T>::operator*() const
{
    return value;
}


template<typename T>
constexpr bool int_range_iterator<T>::operator==(const int_range_iterator<T> &other) const
{
    return value == other.value;
}


template<typename T>
constexpr bool int_range_iterator<T>::operator!=(const int_range_iterator<T> &other) const
{
    return value != other.value;
}


template<typename T>
constexpr int_range_iterator<T> &int_range_iterator<T>::operator++()
{
    ++value;
    return *this;
}

}

#endif /* NOSYNC__INT_RANGE_IMPL_H */
