/*  ----------------------------------------------------------------
 Copyright 2016 Cisco Systems

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

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <memory>
#include <sstream>

#include "spdlog/spdlog.h"
#include "logging_callback.hpp"


namespace spdlog
{
class logger;
}

namespace ydk
{

class Logger
{
    public:
        Logger()
            : internal_logger{ spdlog::get("ydk") }
        {
        }

        ~Logger()
        {
        }
        #define _STRINGIFY(x) #x
        #define STRINGIFY(x) _STRINGIFY(x)
        #define YDKLOGLEVELARGS(loglevel) \
        template <typename... Args> \
        void loglevel(const char* fmt, const Args&... args) \
        { \
            logging_callback func = get_logging_callback(STRINGIFY(loglevel)); \
            if(func != nullptr) \
                { \
                    std::stringstream buffer; \
                    write_fmt_msg<Args...>(internal_logger->name(), \
                                           STRINGIFY(loglevel), \
                                           fmt, \
                                           buffer, \
                                           args...); \
                    func(buffer.str().c_str()); \
                } \
            if(!lazy_check()) { return; } \
            internal_logger->loglevel<Args...>(fmt, args...); \
        }

        #define YDKLOGLEVELNOARGS(loglevel) \
        template <typename T> \
        void loglevel(const T& msg) \
        { \
            logging_callback func = get_logging_callback(STRINGIFY(loglevel)); \
            if(func != nullptr) { func(msg); } \
            if(!lazy_check()) { return; } \
            internal_logger->loglevel<T>(msg); \
        }

        YDKLOGLEVELARGS(trace)
        YDKLOGLEVELARGS(debug)
        YDKLOGLEVELARGS(info)
        YDKLOGLEVELARGS(warn)
        YDKLOGLEVELARGS(error)
        YDKLOGLEVELARGS(critical)

        YDKLOGLEVELNOARGS(trace)
        YDKLOGLEVELNOARGS(debug)
        YDKLOGLEVELNOARGS(info)
        YDKLOGLEVELNOARGS(warn)
        YDKLOGLEVELNOARGS(error)
        YDKLOGLEVELNOARGS(critical)

        #undef YDKLOGLEVELARGS
        #undef YDKLOGLEVELNOARGS
        #undef _STRINGIFY
        #undef STRINGIFY

    private:
        bool lazy_check()
        {
            if (!is_logger_found())
            {
                internal_logger = spdlog::get("ydk");
                if (!is_logger_found())
                {
                    return false;
                }
            }
            return true;
        }

        bool is_logger_found()
        {
            return (internal_logger && internal_logger != nullptr);
        }

    private:
        std::shared_ptr<spdlog::logger> internal_logger;
};

static Logger logger{};

#define YLOG_TRACE(...) logger.trace(__VA_ARGS__)
#define YLOG_DEBUG(...) logger.debug(__VA_ARGS__)
#define YLOG_INFO(...) logger.info(__VA_ARGS__)
#define YLOG_WARN(...) logger.warn(__VA_ARGS__)
#define YLOG_ERROR(...) logger.error(__VA_ARGS__)
#define YLOG_CRITICAL(...) logger.critical(__VA_ARGS__)

}

#endif /* _LOGGER_H_ */
