// This file is part of libnosync library. See LICENSE file for license details.
#include <csignal>
#include <nosync/exceptions.h>
#include <nosync/result-utils.h>
#include <nosync/signal-utils.h>


namespace nosync
{

result<void> block_signal_for_thread(int signal_num)
{
    sigset_t sigset;
    if (::sigemptyset(&sigset) < 0 || ::sigaddset(&sigset, signal_num) < 0) {
        return make_raw_error_result_from_errno();
    }

    return sigprocmask(SIG_BLOCK, &sigset, nullptr) == 0
        ? make_ok_result()
        : make_raw_error_result_from_errno();
}


result<void> ignore_signal_for_process(int signal_num)
{
    return ::signal(signal_num, SIG_IGN) != SIG_ERR
        ? make_ok_result()
        : make_raw_error_result_from_errno();
}


void block_signal(int signal_num)
{
    throw_if_error_result(block_signal_for_thread(signal_num));
}


void ignore_signal(int signal_num)
{
    (void) ignore_signal_for_process(signal_num);
}

}
