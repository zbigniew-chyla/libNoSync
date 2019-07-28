// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__RESULT_UTILS_IMPL_H
#define NOSYNC__RESULT_UTILS_IMPL_H

#include <nosync/exceptions.h>
#include <utility>


namespace nosync
{

template<typename T>
void throw_if_error_result(const result<T> &res, const std::string &throw_msg)
{
    if (!res.is_ok()) {
        if (!throw_msg.empty()) {
            throw_from_error_code(res.get_error(), throw_msg);
        } else {
            throw_from_error_code(res.get_error());
        }
    }
}


template<typename T>
T get_result_value_or_throw(result<T> &&res, const std::string &throw_msg)
{
    throw_if_error_result(res, throw_msg);
    return std::move(res.get_value());
}

}

#endif
