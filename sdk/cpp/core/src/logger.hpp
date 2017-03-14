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
#include <string>
#include "Python.h"

#include "spdlog/spdlog.h"

namespace spdlog
{
class logger;
}

namespace ydk
{


class PyLogger
{
public:
    PyLogger()
    {
    }

    ~PyLogger()
    {
        if (py_logger != nullptr)
        {
           Py_DECREF(py_logger);
        }
    }

    void set_logger()
    {
        PyObject* py_logging  = PyImport_ImportModule("logging");
        PyObject* ydk_arg = Py_BuildValue("s", "ydk");
        const char* method = "getLogger";
        const char* fmt = "O";
        py_logger = PyObject_CallMethod(py_logging, (char *)method, (char *)fmt, ydk_arg);
        Py_DECREF(ydk_arg);
        Py_DECREF(py_logging);
    }

    template <typename... Args> void py_fmt_log(const std::string& name, const char* fmt, spdlog::level::level_enum lvl, const char* py_lvl, const Args&... args)
    {
        spdlog::details::log_msg log_msg(&name, lvl);
        log_msg.raw.write(fmt, args...);
        py_log(log_msg.raw.c_str(), py_lvl);
    }

    template <typename... Args> void trace(const std::string& name, const char* fmt, const Args&... args) {
        const char* py_lvl = "debug";
        py_fmt_log(name, fmt, spdlog::level::trace, py_lvl, args...);
    }
    template <typename... Args> void debug(const std::string& name, const char* fmt, const Args&... args) {
        const char* py_lvl = "debug";
        py_fmt_log(name, fmt, spdlog::level::trace, py_lvl, args...);
    }
    template <typename... Args> void info(const std::string& name, const char* fmt, const Args&... args) {
        const char* py_lvl = "info";
        py_fmt_log(name, fmt, spdlog::level::trace, py_lvl, args...);
    }
    template <typename... Args> void warn(const std::string& name, const char* fmt, const Args&... args) {
        const char* py_lvl = "warn";
        py_fmt_log(name, fmt, spdlog::level::trace, py_lvl, args...);
    }
    template <typename... Args> void error(const std::string& name, const char* fmt, const Args&... args) {
        const char* py_lvl = "error";
        py_fmt_log(name, fmt, spdlog::level::trace, py_lvl, args...);
    }
    template <typename... Args> void critical(const std::string& name, const char* fmt, const Args&... args) {
        const char* py_lvl = "critical";
        py_fmt_log(name, fmt, spdlog::level::trace, py_lvl, args...);
    }

    template <typename T> void py_log(const T& msg, const char* py_lvl)
    {
        PyObject* py_msg_arg = Py_BuildValue("s", msg);
        const char* fmt = "O";
        if (Py_IsInitialized())
        {
            if (py_logger == nullptr)
            {
                set_logger();
            }
            PyObject_CallMethod(py_logger, (char *)py_lvl, (char *)fmt, py_msg_arg);
        }
        Py_DECREF(py_msg_arg);
    }

    template <typename T> void trace(const T& msg) {
        const char* py_lvl = "debug";
        py_log(msg, py_lvl);
    }
    template <typename T> void debug(const T& msg) {
        const char* py_lvl = "debug";
        py_log(msg, py_lvl);
    }
    template <typename T> void info(const T& msg) {
        const char* py_lvl = "info";
        py_log(msg, py_lvl);
    }
    template <typename T> void warn(const T& msg) {
        const char* py_lvl = "warn";
        py_log(msg, py_lvl);
    }
    template <typename T> void error(const T& msg) {
        const char* py_lvl = "error";
        py_log(msg, py_lvl);
    }
    template <typename T> void critical(const T& msg) {
        const char* py_lvl = "critical";
        py_log(msg, py_lvl);
    }


private:
    PyObject* py_logger = nullptr;
};

class Logger
{
    public:
        Logger()
            : internal_logger{ spdlog::get("ydk") },
              py_logger{ std::make_shared<PyLogger>() }
        {
        }

        ~Logger()
        {
        }

        #define YDKLOGLEVELARGS(loglevel) \
        template <typename... Args> \
        void loglevel(const char* fmt, const Args&... args) \
        { \
            if(!lazy_check()) { return; } \
            internal_logger->loglevel<Args...>(fmt, args...); \
            if (Py_IsInitialized()) \
            py_logger->loglevel<Args...>(internal_logger->name(), fmt, args...); \
        }

        #define YDKLOGLEVELNOARGS(loglevel) \
        template <typename T> \
        void loglevel(const T& msg) \
        { \
            if(!lazy_check()) { return; } \
            internal_logger->loglevel<T>(msg); \
            if (Py_IsInitialized()) \
            py_logger->loglevel<T>(msg); \
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
        std::shared_ptr<PyLogger> py_logger;
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
