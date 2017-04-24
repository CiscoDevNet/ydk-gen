/*  ----------------------------------------------------------------
 Copyright 2017 Cisco Systems

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
------------------------------------------------------------------*/
#pragma once

#ifndef _LOGGING_CALLBACK_H
#define _LOGGING_CALLBACK_H

#include <sstream>
#include "spdlog/spdlog.h"

namespace ydk
{
typedef void (*logging_callback)(const char *msg);

void set_logging_callback(const char* level, logging_callback func);
logging_callback get_logging_callback(const char* level);

static const char trace [] = "trace";
static const char debug [] = "debug";
static const char info [] = "info";
static const char warn [] = "warn";
static const char error[] = "error";
static const char critical[] = "critical";

template <typename... Args>
void write_fmt_msg(const std::string& name, const char* level, const char* fmt, std::stringstream& buffer, const Args&... args)
{
    spdlog::level::level_enum lvl;

    if (!strcmp(level, trace)) { lvl = spdlog::level::level_enum::trace ; }
    if (!strcmp(level, debug)) { lvl = spdlog::level::level_enum::debug ; }
    if (!strcmp(level, info)) { lvl = spdlog::level::level_enum::info ; }
    if (!strcmp(level, warn)) { lvl = spdlog::level::level_enum::warn ; }
    if (!strcmp(level, error)) { lvl = spdlog::level::level_enum::err ; }
    if (!strcmp(level, critical)) { lvl = spdlog::level::level_enum::critical ; }

    spdlog::details::log_msg log_msg(&name, lvl);
    log_msg.raw.write(fmt, args...);

    // need to keep buffer alive until logging callback function finished execution
    buffer << log_msg.raw.str();
}

}

#endif /* _LOGGING_CALLBACK_H */
