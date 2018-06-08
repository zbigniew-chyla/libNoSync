// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__EXCEPTIONS_H
#define NOSYNC__EXCEPTIONS_H

#include <string>
#include <system_error>


namespace nosync
{

template<typename Try, typename Catch>
void try_with_catch_all(const Try &try_func, const Catch &catch_func);

[[noreturn]] void throw_logic_error(const std::string &msg);

[[noreturn]] void throw_from_error_code(std::error_code ec);
[[noreturn]] void throw_from_error_code(std::error_code ec, const std::string &msg);

}

#include <nosync/exceptions-impl.h>

#endif /* NOSYNC__EXCEPTIONS_H */
