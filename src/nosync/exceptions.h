// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__EXCEPTIONS_H
#define NOSYNC__EXCEPTIONS_H

#include <exception>
#include <iostream>
#include <string>
#include <system_error>


namespace nosync
{

[[noreturn]] void throw_from_error_code(std::error_code ec);
[[noreturn]] void throw_from_error_code(std::error_code ec, const std::string &msg);

[[noreturn]] void throw_system_error_from_errno();
[[noreturn]] void throw_system_error_from_errno(const std::string &msg);

void print_exception_info(const std::exception_ptr &eptr, std::ostream &output, const std::string &line_prefix = {});
void print_current_exception_info(std::ostream &output, const std::string &line_prefix = {});

}

#endif /* NOSYNC__EXCEPTIONS_H */
