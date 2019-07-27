// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__RESULT_UTILS_H
#define NOSYNC__RESULT_UTILS_H

#include <nosync/raw-error-result.h>
#include <nosync/result.h>
#include <string>


namespace nosync
{

raw_error_result make_raw_error_result_from_errno();
raw_error_result make_raw_error_result_from_errno_value(int errno_value);

raw_error_result make_timeout_raw_error_result();

template<typename T>
void throw_if_error_result(const result<T> &res, const std::string &throw_msg = std::string());

template<typename T>
T get_result_value_or_throw(result<T> &&res, const std::string &throw_msg = std::string());
template<>
void get_result_value_or_throw(result<void> &&res, const std::string &throw_msg);

}

#include <nosync/result-utils-impl.h>

#endif
