// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__EXCEPTIONS_H
#define NOSYNC__EXCEPTIONS_H

#include <exception>
#include <iostream>
#include <string>


namespace nosync
{

[[noreturn]] void throw_system_error_from_errno();
[[noreturn]] void throw_system_error_from_errno(const std::string &msg);

void print_exception_info(const std::exception_ptr &eptr, std::ostream &output, const std::string &line_prefix = {});
void print_current_exception_info(std::ostream &output, const std::string &line_prefix = {});

}

#endif /* NOSYNC__EXCEPTIONS_H */
