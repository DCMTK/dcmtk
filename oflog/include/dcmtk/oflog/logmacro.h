// -*- C++ -*-
// Module:  Log4CPLUS
// File:    logmacro.h
// Created: 8/2003
// Author:  Tad E. Smith
//
//
// Copyright 2003-2010 Tad E. Smith
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/** @file
 * This header defines the logging macros. */

#ifndef DCMTK_LOG4CPLUS_LOGGING_MACROS_HEADER_
#define DCMTK_LOG4CPLUS_LOGGING_MACROS_HEADER_

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/logger.h"
#include "dcmtk/oflog/helpers/snprintf.h"
#include "dcmtk/oflog/tracelog.h"
#include <sstream>
#include <utility>

#if defined(DCMTK_LOG4CPLUS_DISABLE_FATAL) && !defined(DCMTK_LOG4CPLUS_DISABLE_ERROR)
#define DCMTK_LOG4CPLUS_DISABLE_ERROR
#endif
#if defined(DCMTK_LOG4CPLUS_DISABLE_ERROR) && !defined(DCMTK_LOG4CPLUS_DISABLE_WARN)
#define DCMTK_LOG4CPLUS_DISABLE_WARN
#endif
#if defined(DCMTK_LOG4CPLUS_DISABLE_WARN) && !defined(DCMTK_LOG4CPLUS_DISABLE_INFO)
#define DCMTK_LOG4CPLUS_DISABLE_INFO
#endif
#if defined(DCMTK_LOG4CPLUS_DISABLE_INFO) && !defined(DCMTK_LOG4CPLUS_DISABLE_DEBUG)
#define DCMTK_LOG4CPLUS_DISABLE_DEBUG
#endif
#if defined(DCMTK_LOG4CPLUS_DISABLE_DEBUG) && !defined(DCMTK_LOG4CPLUS_DISABLE_TRACE)
#define DCMTK_LOG4CPLUS_DISABLE_TRACE
#endif


namespace dcmtk
{
namespace log4cplus
{

namespace detail
{


inline
Logger
macros_get_logger (Logger const & logger)
{
    return logger;
}


inline
Logger const &
macros_get_logger (Logger & logger)
{
    return logger;
}


#if defined (DCMTK_LOG4CPLUS_HAVE_RVALUE_REFS)
inline
Logger
macros_get_logger (Logger && logger)
{
    return STD_NAMESPACE move (logger);
}

#endif


inline
Logger
macros_get_logger (tstring const & logger)
{
    return Logger::getInstance (logger);
}


inline
Logger
macros_get_logger (tchar const * logger)
{
    return Logger::getInstance (logger);
}


DCMTK_LOG4CPLUS_EXPORT void clear_tostringstream (tostringstream &);


DCMTK_LOG4CPLUS_EXPORT log4cplus::tostringstream & get_macro_body_oss ();
DCMTK_LOG4CPLUS_EXPORT log4cplus::helpers::snprintf_buf & get_macro_body_snprintf_buf ();
DCMTK_LOG4CPLUS_EXPORT void macro_forced_log (log4cplus::Logger const &,
    log4cplus::LogLevel, log4cplus::tstring const &, char const *, int,
    char const *);


} // namespace detail

} // namespace log4cplus
} // end namespace dcmtk


#undef DCMTK_LOG4CPLUS_MACRO_FUNCTION
#define DCMTK_LOG4CPLUS_MACRO_FUNCTION() 0
#if ! defined (DCMTK_LOG4CPLUS_DISABLE_FUNCTION_MACRO)
#  if defined (DCMTK_LOG4CPLUS_HAVE_FUNCSIG_MACRO)
#    undef DCMTK_LOG4CPLUS_MACRO_FUNCTION
#    define DCMTK_LOG4CPLUS_MACRO_FUNCTION() __FUNCSIG__
#  elif defined (DCMTK_LOG4CPLUS_HAVE_PRETTY_FUNCTION_MACRO)
#    undef DCMTK_LOG4CPLUS_MACRO_FUNCTION
#    define DCMTK_LOG4CPLUS_MACRO_FUNCTION() __PRETTY_FUNCTION__
#  elif defined (DCMTK_LOG4CPLUS_HAVE_FUNCTION_MACRO)
#    undef DCMTK_LOG4CPLUS_MACRO_FUNCTION
#    define DCMTK_LOG4CPLUS_MACRO_FUNCTION() __FUNCTION__
#  elif defined (DCMTK_LOG4CPLUS_HAVE_FUNC_SYMBOL)
#    undef DCMTK_LOG4CPLUS_MACRO_FUNCTION
#    define DCMTK_LOG4CPLUS_MACRO_FUNCTION() __func__
#  endif
#endif


// Make TRACE and DEBUG log level unlikely and INFO, WARN, ERROR and
// FATAL log level likely.
#define DCMTK_LOG4CPLUS_MACRO_TRACE_LOG_LEVEL(pred) \
    DCMTK_LOG4CPLUS_UNLIKELY (pred)
#define DCMTK_LOG4CPLUS_MACRO_DEBUG_LOG_LEVEL(pred) \
    DCMTK_LOG4CPLUS_UNLIKELY (pred)
#define DCMTK_LOG4CPLUS_MACRO_INFO_LOG_LEVEL(pred) \
    DCMTK_LOG4CPLUS_LIKELY (pred)
#define DCMTK_LOG4CPLUS_MACRO_WARN_LOG_LEVEL(pred) \
    DCMTK_LOG4CPLUS_LIKELY (pred)
#define DCMTK_LOG4CPLUS_MACRO_ERROR_LOG_LEVEL(pred) \
    DCMTK_LOG4CPLUS_LIKELY (pred)
#define DCMTK_LOG4CPLUS_MACRO_FATAL_LOG_LEVEL(pred) \
    DCMTK_LOG4CPLUS_LIKELY (pred)


//! Dispatch to DCMTK_LOG4CPLUS_MACRO_LOGLEVEL_* depending on log level.
#define DCMTK_LOG4CPLUS_MACRO_LOGLEVEL_PRED(pred, logLevel)    \
    DCMTK_LOG4CPLUS_MACRO_ ## logLevel (pred)


#define DCMTK_LOG4CPLUS_MACRO_BODY(logger, logEvent, logLevel)                \
    do {                                                                \
        dcmtk::log4cplus::Logger const & _l                                    \
            = dcmtk::log4cplus::detail::macros_get_logger (logger);            \
        if (DCMTK_LOG4CPLUS_MACRO_LOGLEVEL_PRED (                             \
                _l.isEnabledFor (dcmtk::log4cplus::logLevel), logLevel)) {     \
            dcmtk::log4cplus::tostringstream & _log4cplus_buf                  \
                = dcmtk::log4cplus::detail::get_macro_body_oss ();             \
            _log4cplus_buf << logEvent;                                 \
            dcmtk::log4cplus::detail::macro_forced_log (_l,                    \
                dcmtk::log4cplus::logLevel, OFString(_log4cplus_buf.str().c_str(), _log4cplus_buf.str().length()), \
                __FILE__, __LINE__, DCMTK_LOG4CPLUS_MACRO_FUNCTION ());       \
        }                                                               \
    } while (0)


#define DCMTK_LOG4CPLUS_MACRO_STR_BODY(logger, logEvent, logLevel)            \
    do {                                                                \
        dcmtk::log4cplus::Logger const & _l                                    \
            = dcmtk::log4cplus::detail::macros_get_logger (logger);            \
        if (DCMTK_LOG4CPLUS_MACRO_LOGLEVEL_PRED (                             \
                _l.isEnabledFor (dcmtk::log4cplus::logLevel), logLevel)) {     \
            dcmtk::log4cplus::detail::macro_forced_log (_l,                    \
                dcmtk::log4cplus::logLevel, logEvent,                          \
                __FILE__, __LINE__, DCMTK_LOG4CPLUS_MACRO_FUNCTION ());       \
        }                                                               \
    } while(0)

#if defined (DCMTK_LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_MACRO_FMT_BODY(logger, logLevel, logFmt, ...)         \
    do {                                                                \
        dcmtk::log4cplus::Logger const & _l                                    \
            = dcmtk::log4cplus::detail::macros_get_logger (logger);            \
        if (DCMTK_LOG4CPLUS_MACRO_LOGLEVEL_PRED (                             \
                _l.isEnabledFor (dcmtk::log4cplus::logLevel), logLevel)) {     \
            dcmtk::log4cplus::helpers::snprintf_buf & _snpbuf                  \
                = dcmtk::log4cplus::detail::get_macro_body_snprintf_buf ();    \
            dcmtk::log4cplus::tchar const * _logEvent                          \
                = _snpbuf.print (logFmt, __VA_ARGS__);                  \
            dcmtk::log4cplus::detail::macro_forced_log (_l,                    \
                dcmtk::log4cplus::logLevel, _logEvent,                         \
                __FILE__, __LINE__, DCMTK_LOG4CPLUS_MACRO_FUNCTION ());       \
        }                                                           \
    } while(0)

#elif defined (DCMTK_LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_MACRO_FMT_BODY(logger, logLevel, logFmt, logArgs...)  \
    do {                                                                \
        dcmtk::log4cplus::Logger const & _l                                    \
            = dcmtk::log4cplus::detail::macros_get_logger (logger);            \
        if (DCMTK_LOG4CPLUS_MACRO_LOGLEVEL_PRED (                             \
                _l.isEnabledFor (dcmtk::log4cplus::logLevel), logLevel)) {     \
            dcmtk::log4cplus::helpers::snprintf_buf & _snpbuf                  \
                = dcmtk::log4cplus::detail::get_macro_body_snprintf_buf ();    \
            dcmtk::log4cplus::tchar const * _logEvent                          \
                = _snpbuf.print (logFmt, logArgs);                      \
            dcmtk::log4cplus::detail::macro_forced_log (_l,                    \
                dcmtk::log4cplus::logLevel, _logEvent,                         \
                __FILE__, __LINE__, DCMTK_LOG4CPLUS_MACRO_FUNCTION ());       \
        }                                                               \
    } while(0)

#endif


/**
 * @def DCMTK_LOG4CPLUS_TRACE(logger, logEvent) This macro creates a
 * TraceLogger to log a TRACE_LOG_LEVEL message to <code>logger</code>
 * upon entry and exiting of a method.
 * <code>logEvent</code> will be streamed into an <code>ostream</code>.
 */
#if !defined(DCMTK_LOG4CPLUS_DISABLE_TRACE)
#define DCMTK_LOG4CPLUS_TRACE_METHOD(logger, logEvent)                        \
    dcmtk::log4cplus::TraceLogger _log4cplus_trace_logger(logger, logEvent,    \
                                                   __FILE__, __LINE__);
#define DCMTK_LOG4CPLUS_TRACE(logger, logEvent)                               \
    DCMTK_LOG4CPLUS_MACRO_BODY (logger, logEvent, TRACE_LOG_LEVEL)
#define DCMTK_LOG4CPLUS_TRACE_STR(logger, logEvent)                           \
    DCMTK_LOG4CPLUS_MACRO_STR_BODY (logger, logEvent, TRACE_LOG_LEVEL)

#if defined (DCMTK_LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_TRACE_FMT(logger, logFmt, ...)                            \
    DCMTK_LOG4CPLUS_MACRO_FMT_BODY (logger, TRACE_LOG_LEVEL, logFmt, __VA_ARGS__)
#elif defined (DCMTK_LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_TRACE_FMT(logger, logFmt, logArgs...)                     \
    DCMTK_LOG4CPLUS_MACRO_FMT_BODY(logger, TRACE_LOG_LEVEL, logFmt, logArgs)
#endif

#else
#define DCMTK_LOG4CPLUS_TRACE_METHOD(logger, logEvent) do { } while (0)
#define DCMTK_LOG4CPLUS_TRACE(logger, logEvent) do { } while (0)
#define DCMTK_LOG4CPLUS_TRACE_STR(logger, logEvent) do { } while (0)
#if defined (DCMTK_LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_TRACE_FMT(logger, logFmt, ...) do { } while (0)
#elif defined (DCMTK_LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_TRACE_FMT(logger, logFmt, logArgs...) do { } while (0)
#endif

#endif

/**
 * @def DCMTK_LOG4CPLUS_DEBUG(logger, logEvent)  This macro is used to log a
 * DEBUG_LOG_LEVEL message to <code>logger</code>.
 * <code>logEvent</code> will be streamed into an <code>ostream</code>.
 */
#if !defined(DCMTK_LOG4CPLUS_DISABLE_DEBUG)
#define DCMTK_LOG4CPLUS_DEBUG(logger, logEvent)                               \
    DCMTK_LOG4CPLUS_MACRO_BODY (logger, logEvent, DEBUG_LOG_LEVEL)
#define DCMTK_LOG4CPLUS_DEBUG_STR(logger, logEvent)                           \
    DCMTK_LOG4CPLUS_MACRO_STR_BODY (logger, logEvent, DEBUG_LOG_LEVEL)

#if defined (DCMTK_LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_DEBUG_FMT(logger, logFmt, ...)                            \
    DCMTK_LOG4CPLUS_MACRO_FMT_BODY (logger, DEBUG_LOG_LEVEL, logFmt, __VA_ARGS__)
#elif defined (DCMTK_LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_DEBUG_FMT(logger, logFmt, logArgs...)                     \
    DCMTK_LOG4CPLUS_MACRO_FMT_BODY(logger, DEBUG_LOG_LEVEL, logFmt, logArgs)
#endif

#else
#define DCMTK_LOG4CPLUS_DEBUG(logger, logEvent) do { } while (0)
#define DCMTK_LOG4CPLUS_DEBUG_STR(logger, logEvent) do { } while (0)
#if defined (DCMTK_LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_DEBUG_FMT(logger, logFmt, ...) do { } while (0)
#elif defined (DCMTK_LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_DEBUG_FMT(logger, logFmt, logArgs...) do { } while (0)
#endif

#endif

/**
 * @def DCMTK_LOG4CPLUS_INFO(logger, logEvent)  This macro is used to log a
 * INFO_LOG_LEVEL message to <code>logger</code>.
 * <code>logEvent</code> will be streamed into an <code>ostream</code>.
 */
#if !defined(DCMTK_LOG4CPLUS_DISABLE_INFO)
#define DCMTK_LOG4CPLUS_INFO(logger, logEvent)                                \
    DCMTK_LOG4CPLUS_MACRO_BODY (logger, logEvent, INFO_LOG_LEVEL)
#define DCMTK_LOG4CPLUS_INFO_STR(logger, logEvent)                            \
    DCMTK_LOG4CPLUS_MACRO_STR_BODY (logger, logEvent, INFO_LOG_LEVEL)

#if defined (DCMTK_LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_INFO_FMT(logger, logFmt, ...)                             \
    DCMTK_LOG4CPLUS_MACRO_FMT_BODY (logger, INFO_LOG_LEVEL, logFmt, __VA_ARGS__)
#elif defined (DCMTK_LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_INFO_FMT(logger, logFmt, logArgs...)                      \
    DCMTK_LOG4CPLUS_MACRO_FMT_BODY(logger, INFO_LOG_LEVEL, logFmt, logArgs)
#endif

#else
#define DCMTK_LOG4CPLUS_INFO(logger, logEvent) do { } while (0)
#define DCMTK_LOG4CPLUS_INFO_STR(logger, logEvent) do { } while (0)
#if defined (DCMTK_LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_INFO_FMT(logger, logFmt, ...) do { } while (0)
#elif defined (DCMTK_LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_INFO_FMT(logger, logFmt, logArgs...) do { } while (0)
#endif

#endif

/**
 * @def DCMTK_LOG4CPLUS_WARN(logger, logEvent)  This macro is used to log a
 * WARN_LOG_LEVEL message to <code>logger</code>.
 * <code>logEvent</code> will be streamed into an <code>ostream</code>.
 */
#if !defined(DCMTK_LOG4CPLUS_DISABLE_WARN)
#define DCMTK_LOG4CPLUS_WARN(logger, logEvent)                                \
    DCMTK_LOG4CPLUS_MACRO_BODY (logger, logEvent, WARN_LOG_LEVEL)
#define DCMTK_LOG4CPLUS_WARN_STR(logger, logEvent)                            \
    DCMTK_LOG4CPLUS_MACRO_STR_BODY (logger, logEvent, WARN_LOG_LEVEL)

#if defined (DCMTK_LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_WARN_FMT(logger, logFmt, ...)                             \
    DCMTK_LOG4CPLUS_MACRO_FMT_BODY (logger, WARN_LOG_LEVEL, logFmt, __VA_ARGS__)
#elif defined (DCMTK_LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_WARN_FMT(logger, logFmt, logArgs...)                      \
    DCMTK_LOG4CPLUS_MACRO_FMT_BODY(logger, WARN_LOG_LEVEL, logFmt, logArgs)
#endif

#else
#define DCMTK_LOG4CPLUS_WARN(logger, logEvent) do { } while (0)
#define DCMTK_LOG4CPLUS_WARN_STR(logger, logEvent) do { } while (0)
#if defined (DCMTK_LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_WARN_FMT(logger, logFmt, ...) do { } while (0)
#elif defined (DCMTK_LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_WARN_FMT(logger, logFmt, logArgs...) do { } while (0)
#endif

#endif

/**
 * @def DCMTK_LOG4CPLUS_ERROR(logger, logEvent)  This macro is used to log a
 * ERROR_LOG_LEVEL message to <code>logger</code>.
 * <code>logEvent</code> will be streamed into an <code>ostream</code>.
 */
#if !defined(DCMTK_LOG4CPLUS_DISABLE_ERROR)
#define DCMTK_LOG4CPLUS_ERROR(logger, logEvent)                               \
    DCMTK_LOG4CPLUS_MACRO_BODY (logger, logEvent, ERROR_LOG_LEVEL)
#define DCMTK_LOG4CPLUS_ERROR_STR(logger, logEvent)                           \
    DCMTK_LOG4CPLUS_MACRO_STR_BODY (logger, logEvent, ERROR_LOG_LEVEL)

#if defined (DCMTK_LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_ERROR_FMT(logger, logFmt, ...)                            \
    DCMTK_LOG4CPLUS_MACRO_FMT_BODY (logger, ERROR_LOG_LEVEL, logFmt, __VA_ARGS__)
#elif defined (DCMTK_LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_ERROR_FMT(logger, logFmt, logArgs...)                     \
    DCMTK_LOG4CPLUS_MACRO_FMT_BODY(logger, ERROR_LOG_LEVEL, logFmt, logArgs)
#endif

#else
#define DCMTK_LOG4CPLUS_ERROR(logger, logEvent) do { } while (0)
#define DCMTK_LOG4CPLUS_ERROR_STR(logger, logEvent) do { } while (0)
#if defined (DCMTK_LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_ERROR_FMT(logger, logFmt, ...) do { } while (0)
#elif defined (DCMTK_LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_ERROR_FMT(logger, logFmt, logArgs...) do { } while (0)
#endif

#endif

/**
 * @def DCMTK_LOG4CPLUS_FATAL(logger, logEvent)  This macro is used to log a
 * FATAL_LOG_LEVEL message to <code>logger</code>.
 * <code>logEvent</code> will be streamed into an <code>ostream</code>.
 */
#if !defined(DCMTK_LOG4CPLUS_DISABLE_FATAL)
#define DCMTK_LOG4CPLUS_FATAL(logger, logEvent)                               \
    DCMTK_LOG4CPLUS_MACRO_BODY (logger, logEvent, FATAL_LOG_LEVEL)
#define DCMTK_LOG4CPLUS_FATAL_STR(logger, logEvent)                           \
    DCMTK_LOG4CPLUS_MACRO_STR_BODY (logger, logEvent, FATAL_LOG_LEVEL)

#if defined (DCMTK_LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_FATAL_FMT(logger, logFmt, ...)                            \
    DCMTK_LOG4CPLUS_MACRO_FMT_BODY (logger, FATAL_LOG_LEVEL, logFmt, __VA_ARGS__)
#elif defined (DCMTK_LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_FATAL_FMT(logger, logFmt, logArgs...)                     \
    DCMTK_LOG4CPLUS_MACRO_FMT_BODY(logger, FATAL_LOG_LEVEL, logFmt, logArgs)
#endif

#else
#define DCMTK_LOG4CPLUS_FATAL(logger, logEvent) do { } while (0)
#define DCMTK_LOG4CPLUS_FATAL_STR(logger, logEvent) do { } while (0)
#if defined (DCMTK_LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_FATAL_FMT(logger, logFmt, ...) do { } while (0)
#elif defined (DCMTK_LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define DCMTK_LOG4CPLUS_FATAL_FMT(logger, logFmt, logArgs...) do { } while (0)
#endif

#endif

#endif /* DCMTK_LOG4CPLUS_LOGGING_MACROS_HEADER_ */
