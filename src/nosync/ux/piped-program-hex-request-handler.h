// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC_UX__PIPED_PROGRAM_HEX_REQUEST_HANDLER_H
#define NOSYNC_UX__PIPED_PROGRAM_HEX_REQUEST_HANDLER_H

#include <nosync/request-handler.h>
#include <nosync/ux/fd-watching-event-loop.h>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>


namespace nosync::ux
{

std::shared_ptr<request_handler<std::string, std::string>> start_piped_program_hex_request_handler(
    fd_watching_event_loop &evloop, const std::shared_ptr<request_handler<::pid_t, int>> &subproc_reaper,
    const std::string &cmd_path, const std::vector<std::string> &cmd_args);

}

#endif /* NOSYNC_UX__PIPED_PROGRAM_HEX_REQUEST_HANDLER_H */
