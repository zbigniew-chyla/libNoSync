#include <nosync/activity-owner.h>
#include <utility>

using std::move;
using std::unique_ptr;


namespace nosync
{

activity_owner::activity_owner(activity_owner &&other) noexcept
    : owned_handle(move(other.owned_handle))
{
}


activity_owner::activity_owner(unique_ptr<activity_handle> &&handle) noexcept
    : owned_handle(move(handle))
{
}


activity_owner &activity_owner::operator=(activity_owner &&other) noexcept
{
    reset();
    owned_handle = move(other.owned_handle);
    other.owned_handle.reset();

    return *this;
}


activity_owner &activity_owner::operator=(unique_ptr<activity_handle> &&handle) noexcept
{
    reset();
    owned_handle = move(handle);

    return *this;
}


activity_owner::~activity_owner() noexcept
{
    reset();
}


void activity_owner::reset() noexcept
{
    if (owned_handle && owned_handle->is_enabled()) {
        owned_handle->disable();
    }

    owned_handle.reset();
}

}
