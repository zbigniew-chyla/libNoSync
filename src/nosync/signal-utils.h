// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__SIGNAL_UTILS_H
#define NOSYNC__SIGNAL_UTILS_H

#include <nosync/result.h>


namespace nosync
{

result<void> block_signal_for_thread(int signal_num);
result<void> ignore_signal_for_process(int signal_num);

void block_signal(int signal_num);
void ignore_signal(int signal_num);

}

#endif /* NOSYNC__SIGNAL_UTILS_H */
