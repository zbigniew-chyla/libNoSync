// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__FILE_UTILS_H
#define NOSYNC__FILE_UTILS_H

#include <nosync/owned-fd.h>
#include <nosync/ux/file-utils.h>


namespace nosync
{

using ux::open_file_for_read;
using ux::open_file_for_write;

}

#endif /* NOSYNC__FILE_UTILS_H */
