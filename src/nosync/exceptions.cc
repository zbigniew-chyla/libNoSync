// This file is part of libnosync library. See LICENSE file for license details.
#include <cstdlib>
#include <nosync/exceptions.h>
#include <stdexcept>
#include <string>
#include <system_error>

using std::error_code;
using std::logic_error;
using std::string;
using std::system_error;


namespace nosync
{

namespace
{

template<typename E>
[[noreturn]] void throw_or_abort(const E &e);

#if defined(__cpp_exceptions) && __cpp_exceptions

template<typename E>
void throw_or_abort(const E &e)
{
    throw e;
}

#else

template<typename E>
void throw_or_abort(const E &)
{
    std::abort();
}

#endif

}


void throw_logic_error(const string &msg)
{
    throw_or_abort(logic_error(msg));
}


void throw_from_error_code(error_code ec)
{
    throw_or_abort(system_error(ec));
}


void throw_from_error_code(error_code ec, const string &msg)
{
    throw_or_abort(system_error(ec, msg));
}

}
