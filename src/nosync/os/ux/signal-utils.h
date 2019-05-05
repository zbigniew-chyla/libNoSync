// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__SIGNAL_UTILS_H
#define NOSYNC__OS__UX__SIGNAL_UTILS_H

#include <nosync/result.h>


namespace nosync::os::ux
{

result<void> block_signal_for_thread(int signal_num);
result<void> ignore_signal_for_process(int signal_num);

}

#endif /* NOSYNC__OS__UX__SIGNAL_UTILS_H */
