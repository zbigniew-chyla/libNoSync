// This file is part of libnosync library. See LICENSE file for license details.
#include <csignal>
#include <nosync/result-utils.h>
#include <nosync/ux/signal-utils.h>


namespace nosync::ux
{

result<void> block_signal_for_thread(int signal_num)
{
    sigset_t sigset;
    if (::sigemptyset(&sigset) < 0 || ::sigaddset(&sigset, signal_num) < 0) {
        return make_raw_error_result_from_errno();
    }

    auto sigmask_retval = ::pthread_sigmask(SIG_BLOCK, &sigset, nullptr);
    return sigmask_retval == 0
        ? make_ok_result()
        : make_raw_error_result_from_errno_value(sigmask_retval);
}


result<void> ignore_signal_for_process(int signal_num)
{
    struct ::sigaction sa = {};
    sa.sa_handler = SIG_IGN;

    return ::sigaction(signal_num, &sa, nullptr) == 0
        ? make_ok_result()
        : make_raw_error_result_from_errno();
}

}
