// This file is part of libnosync library. See LICENSE file for license details.
#include <cerrno>
#include <iostream>
#include <nosync/exceptions.h>
#include <stdexcept>
#include <string>
#include <system_error>

using std::current_exception;
using std::error_code;
using std::exception;
using std::exception_ptr;
using std::logic_error;
using std::ostream;
using std::nested_exception;
using std::rethrow_exception;
using std::string;
using std::system_category;
using std::system_error;


namespace nosync
{

namespace
{

void print_exception_info_impl(const exception_ptr &eptr, ostream &output, const string &line_prefix)
{
    static const string line_indent_str = "  ";

    if (!eptr) {
        return;
    }

    string exception_msg;
    try {
        rethrow_exception(eptr);
    } catch (const exception &e) {
        exception_msg = string("exception: ") + e.what();
    } catch (...) {
        exception_msg = "unknown exception";
    }

    output << line_prefix << exception_msg << std::endl;

    try {
        rethrow_exception(eptr);
    } catch (const nested_exception &e) {
        print_exception_info_impl(e.nested_ptr(), output, line_prefix + line_indent_str);
    } catch (...) {
        // not nested
    }
}

}


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


void throw_system_error_from_errno()
{
    auto error_code = errno;
    throw system_error(error_code, system_category());
}


void throw_system_error_from_errno(const string &msg)
{
    auto error_code = errno;
    throw system_error(error_code, system_category(), msg);
}


void print_exception_info(const exception_ptr &eptr, ostream &output, const string &line_prefix)
{
    print_exception_info_impl(eptr, output, line_prefix);
}


void print_current_exception_info(ostream &output, const string &line_prefix)
{
    print_exception_info_impl(current_exception(), output, line_prefix);
}

}
