// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__COMPAT__OPTIONAL_H
#define NOSYNC__COMPAT__OPTIONAL_H

#include <experimental/optional>


namespace nosync
{
namespace compat
{

template<typename T>
using optional = std::experimental::optional<T>;

using std::experimental::make_optional;
using std::experimental::nullopt;

}
}

#endif /* NOSYNC__COMPAT__OPTIONAL_H */
