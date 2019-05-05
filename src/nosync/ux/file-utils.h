// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__UX__FILE_UTILS_H
#define NOSYNC__UX__FILE_UTILS_H

#include <nosync/result.h>
#include <nosync/ux/owned-fd.h>
#include <string>


namespace nosync::ux
{

result<owned_fd> open_file_for_read(const std::string &path);
result<owned_fd> open_file_for_write(const std::string &path);

}

#endif /* NOSYNC__UX__FILE_UTILS_H */
