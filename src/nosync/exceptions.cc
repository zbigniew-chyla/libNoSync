// This file is part of libnosync library. See LICENSE file for license details.
#include <cstdio>
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

template<typename Exception>
[[noreturn]] void throw_or_abort(Exception &&e);

#if defined(__cpp_exceptions) && __cpp_exceptions

template<typename Exception>
void throw_or_abort(Exception &&e)
{
    throw e;
}

#else

template<typename Exception>
void throw_or_abort(Exception &&e)
{
    std::fprintf(stderr, "Trying to throw exception with exceptions disabled (%s). Aborting!", e.what());
    std::fflush(stderr);
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
