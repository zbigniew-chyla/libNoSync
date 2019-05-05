// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__SHARED_FD_H
#define NOSYNC__OS__UX__SHARED_FD_H

#include <nosync/os/ux/owned-fd.h>
#include <memory>


namespace nosync::os::ux
{

class shared_fd
{
public:
    shared_fd();

    explicit shared_fd(int fd);
    shared_fd(owned_fd &&ofd);

    shared_fd(const shared_fd &other) = default;
    shared_fd &operator=(const shared_fd &other) = default;

    shared_fd(shared_fd &&other) = default;
    shared_fd &operator=(shared_fd &&other) = default;

    explicit operator bool() const noexcept;

    int operator*() const noexcept;

private:
    std::shared_ptr<owned_fd> ofd;
};

}

#endif /* NOSYNC__OS__UX__SHARED_FD_H */
