// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__COMPAT__STRING_VIEW_H
#define NOSYNC__COMPAT__STRING_VIEW_H

#include <experimental/string_view>


namespace nosync
{
namespace compat
{

using string_view = std::experimental::string_view;

}
}

#endif /* NOSYNC__COMPAT__STRING_VIEW_H */
