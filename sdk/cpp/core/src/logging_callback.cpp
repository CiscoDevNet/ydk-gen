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

#include "spdlog/spdlog.h"
#include "logging_callback.hpp"

namespace ydk
{
static logging_callback ydk_logging_trace_function = nullptr;
static logging_callback ydk_logging_debug_function = nullptr;
static logging_callback ydk_logging_info_function = nullptr;
static logging_callback ydk_logging_warn_function = nullptr;
static logging_callback ydk_logging_error_function = nullptr;
static logging_callback ydk_logging_critical_function = nullptr;

void set_logging_callback(const char* level, logging_callback func)
{
    if(!strcmp(level, trace)) { ydk_logging_trace_function = func; }
    if(!strcmp(level, debug)) { ydk_logging_debug_function = func; }
    if(!strcmp(level, info)) { ydk_logging_info_function = func; }
    if(!strcmp(level, warn)) { ydk_logging_warn_function = func; }
    if(!strcmp(level, error)) { ydk_logging_error_function = func; }
    if(!strcmp(level, critical)) { ydk_logging_critical_function = func; }
}

logging_callback get_logging_callback(const char* level)
{
    if(!strcmp(level, trace)) { return ydk_logging_trace_function; }
    if(!strcmp(level, debug)) { return ydk_logging_debug_function; }
    if(!strcmp(level, info)) { return ydk_logging_info_function; }
    if(!strcmp(level, warn)) { return ydk_logging_warn_function; }
    if(!strcmp(level, error)) { return ydk_logging_error_function; }
    if(!strcmp(level, critical)) { return ydk_logging_critical_function; }
}

}
