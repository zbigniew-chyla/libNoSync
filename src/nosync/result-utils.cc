// This file is part of libnosync library. See LICENSE file for license details.
#include <cerrno>
#include <nosync/result-utils.h>
#include <system_error>

using std::errc;
using std::error_code;
using std::make_error_code;


namespace nosync
{

raw_error_result make_raw_error_result_from_errno()
{
    return make_raw_error_result_from_errno_value(errno);
}


raw_error_result make_raw_error_result_from_errno_value(int errno_value)
{
    return raw_error_result(error_code(errno_value, std::generic_category()));
}


raw_error_result make_timeout_raw_error_result()
{
    return raw_error_result(make_error_code(errc::timed_out));
}


template<>
void get_result_value_or_throw(result<void> &&res, const std::string &throw_msg)
{
    throw_if_error_result(res, throw_msg);
}

}
