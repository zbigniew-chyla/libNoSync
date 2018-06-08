// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__EXCEPTIONS_IMPL_H
#define NOSYNC__EXCEPTIONS_IMPL_H


namespace nosync
{

template<typename Try, typename Catch>
void try_with_catch_all(const Try &try_func, const Catch &catch_func)
{
    try {
        try_func();
    } catch (...) {
        catch_func();
    }
}

}

#endif /* NOSYNC__EXCEPTIONS_IMPL_H */
