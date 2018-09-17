// This file is part of libnosync library. See LICENSE file for license details.
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

void throw_logic_error(const string &msg)
{
    throw logic_error(msg);
}


void throw_from_error_code(error_code ec)
{
    throw system_error(ec);
}


void throw_from_error_code(error_code ec, const string &msg)
{
    throw system_error(ec, msg);
}

}
