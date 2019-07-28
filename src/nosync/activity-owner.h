// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__ACTIVITY_OWNER_H
#define NOSYNC__ACTIVITY_OWNER_H

#include <memory>
#include <nosync/activity-handle.h>


namespace nosync
{

class activity_owner
{
public:
    activity_owner() = default;

    activity_owner(activity_owner &&other) noexcept;
    explicit activity_owner(std::unique_ptr<activity_handle> &&handle) noexcept;

    activity_owner &operator=(activity_owner &&other) noexcept;
    activity_owner &operator=(std::unique_ptr<activity_handle> &&handle) noexcept;

    activity_owner(const activity_owner &other) = delete;
    activity_owner &operator=(const activity_owner &other) = delete;

    ~activity_owner() noexcept;

    void reset() noexcept;

private:
    std::unique_ptr<activity_handle> owned_handle;
};

}

#endif
