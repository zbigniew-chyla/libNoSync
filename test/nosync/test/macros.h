// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC_TEST__MACROS_H
#define NOSYNC_TEST__MACROS_H

#if defined(__cpp_exceptions) && __cpp_exceptions

#define NOSYNC_TEST_EXCEPTIONS_ENABLED 1

#else

#define NOSYNC_TEST_EXCEPTIONS_ENABLED 0

#endif

#endif /* NOSYNC_TEST__MACROS_H */
