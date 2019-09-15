// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__EVENT_LOOP_MT_EXECUTOR_PROVIDER_H
#define NOSYNC__OS__EVENT_LOOP_MT_EXECUTOR_PROVIDER_H

#include <functional>
#include <nosync/interface-type.h>


namespace nosync::os
{

/*!
Provider of MT executors for executing tasks inside an event loop.
*/
class event_loop_mt_executor_provider : public interface_type
{
public:
    virtual std::function<void(std::function<void()>)> get_mt_executor() = 0;
};

}

#endif
