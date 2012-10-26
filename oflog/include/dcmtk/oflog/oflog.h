/*
 *
 *  Copyright (C) 2009-2011, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  oflog
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Simplify the usage of log4cplus to other modules (Header)
 *
 */


#ifndef OFLOG_H
#define OFLOG_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/oflog/logger.h"
#include "dcmtk/oflog/logmacro.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofaptr.h"
#include "dcmtk/ofstd/ofconapp.h"

#define OFLOG_TRACE(logger, msg) DCMTK_LOG4CPLUS_TRACE(logger, msg)
#define OFLOG_DEBUG(logger, msg) DCMTK_LOG4CPLUS_DEBUG(logger, msg)
#define OFLOG_INFO(logger, msg)  DCMTK_LOG4CPLUS_INFO(logger, msg)
#define OFLOG_WARN(logger, msg)  DCMTK_LOG4CPLUS_WARN(logger, msg)
#define OFLOG_ERROR(logger, msg) DCMTK_LOG4CPLUS_ERROR(logger, msg)
#define OFLOG_FATAL(logger, msg) DCMTK_LOG4CPLUS_FATAL(logger, msg)

/** simple wrapper around the "low-level" Logger object to make it easier to
 *  switch to a different system
 */
class DCMTK_LOG4CPLUS_EXPORT OFLogger : public dcmtk::log4cplus::Logger
{
public:
    /** copy constructor
     *  @param base object to be copied
     */
    OFLogger(const dcmtk::log4cplus::Logger &base);

    /// these are the log levels that you can feed to isEnabledFor()
    enum LogLevel {
        /// trace: output more details on the internal application state, a kind of "verbose debug"
        TRACE_LOG_LEVEL = dcmtk::log4cplus::TRACE_LOG_LEVEL,
        /// debug: fine-grained informational events that are most useful to debug an application
        DEBUG_LOG_LEVEL = dcmtk::log4cplus::DEBUG_LOG_LEVEL,
        /// info: informational messages that highlight the progress of the application at coarse-grained level
        INFO_LOG_LEVEL  = dcmtk::log4cplus::INFO_LOG_LEVEL,
        /// warn: potentially harmful situations
        WARN_LOG_LEVEL  = dcmtk::log4cplus::WARN_LOG_LEVEL,
        /// error: events that might still allow the application to continue running
        ERROR_LOG_LEVEL = dcmtk::log4cplus::ERROR_LOG_LEVEL,
        /// fatal: very severe error events that will presumably lead the application to abort
        FATAL_LOG_LEVEL = dcmtk::log4cplus::FATAL_LOG_LEVEL,
        /// internal: turn off logging competely
        OFF_LOG_LEVEL = dcmtk::log4cplus::OFF_LOG_LEVEL
    };

    /** Get the logger's log level.
     *  One of the checks that isEnabledFor() does looks like this:
     *    if (getChainedLogLevel() < level)
     *        return false;
     *  @return the log level to which this logger is set.
     */
    LogLevel getChainedLogLevel() const {
        return OFstatic_cast(LogLevel, Logger::getChainedLogLevel());
    }
};

/** functions for initializing the logging system
 */
class DCMTK_LOG4CPLUS_EXPORT OFLog
{
 private:

    /** private constructor, don't create instances of this class
     */
    OFLog() { }

    /** set up the logging and enable it
     *  @param level the verbosity that you want
     */
    static void configureLogger(dcmtk::log4cplus::LogLevel level);

 public:

    /** create a new Logger object
     *  @param name the name of the logger
     */
    static OFLogger getLogger(const char *name);

    /** set up the logging and enable it
     *  @param level the verbosity that you want
     */
    static void configure(OFLogger::LogLevel level = OFLogger::WARN_LOG_LEVEL);

    /** handle the command line options used for logging
     *  @param cmd the command line whose options are handled
     *  @param app the console application which is used for console output and error checking
     */
    static void configureFromCommandLine(OFCommandLine &cmd,
                                         OFConsoleApplication &app,
                                         OFLogger::LogLevel defaultLevel = OFLogger::WARN_LOG_LEVEL);

    /** add the command line options which configureFromCommandLine() checks for
     *  @param cmd the command line to which these options should be added
     */
    static void addOptions(OFCommandLine &cmd);

    /** if there was a config file loaded in configureFromCommandLine(), the
     *  in-memory copy will be parsed again, but all variables will be updated
     *  first (current time, process id, ....)
     *  @param cmd command line from which the application name will be retrieved
     */
    static void reconfigure(OFCommandLine *cmd = NULL);

 private:

    /// If we loaded a config file in configureFromCommandLine(), this is it
    static OFauto_ptr<dcmtk::log4cplus::helpers::Properties> configProperties_;
};

#endif
