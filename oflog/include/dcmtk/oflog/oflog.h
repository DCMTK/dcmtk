/*
 *
 *  Copyright (C) 2009-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:20:52 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef OFLOG_H
#define OFLOG_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/oflog/logger.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofconapp.h"

#define OFLOG_TRACE(logger, msg) LOG4CPLUS_TRACE(logger, msg)
#define OFLOG_DEBUG(logger, msg) LOG4CPLUS_DEBUG(logger, msg)
#define OFLOG_INFO(logger, msg)  LOG4CPLUS_INFO(logger, msg)
#define OFLOG_WARN(logger, msg)  LOG4CPLUS_WARN(logger, msg)
#define OFLOG_ERROR(logger, msg) LOG4CPLUS_ERROR(logger, msg)
#define OFLOG_FATAL(logger, msg) LOG4CPLUS_FATAL(logger, msg)

/** simple wrapper around the "low-level" Logger object to make it easier to
 *  switch to a different system
 */
class OFLogger : private log4cplus::Logger
{
public:
    /** copy constructor
     *  @param base object to be copied
     */
    OFLogger(const log4cplus::Logger &base);

    /// these are the log levels that you can feed to isEnabledFor()
    enum LogLevel {
        /// trace: output more details on the internal application state, a kind of "verbose debug"
        TRACE_LOG_LEVEL = log4cplus::TRACE_LOG_LEVEL,
        /// debug: fine-grained informational events that are most useful to debug an application
        DEBUG_LOG_LEVEL = log4cplus::DEBUG_LOG_LEVEL,
        /// info: informational messages that highlight the progress of the application at coarse-grained level
        INFO_LOG_LEVEL  = log4cplus::INFO_LOG_LEVEL,
        /// warn: potentially harmful situations
        WARN_LOG_LEVEL  = log4cplus::WARN_LOG_LEVEL,
        /// error: events that might still allow the application to continue running
        ERROR_LOG_LEVEL = log4cplus::ERROR_LOG_LEVEL,
        /// fatal: very severe error events that will presumably lead the application to abort
        FATAL_LOG_LEVEL = log4cplus::FATAL_LOG_LEVEL
    };

    /** check if the given log level was activated.
     *  This can be used to check if a given log level is activated before
     *  spending cpu time on generating a log message. The OFLOG_* macros use
     *  this automatically, so you should never do something like:
     *     if (myLogger.isEnabledFor(INFO_LOG_LEVEL))
     *         OFLOG_INFO("Doing it like this is pointless);
     *  @param ll the log level to check for
     *  @return true if messages on this level won't be discarded
     */
    bool isEnabledFor(log4cplus::LogLevel ll) const {
        return Logger::isEnabledFor(ll);
    }

    /// this function is only used internally by OFLOG_FATAL and friends
    void forcedLog(log4cplus::LogLevel ll, const log4cplus::tstring& message,
                   const char* file=NULL, int line=-1, const char* function=NULL) const {
        Logger::forcedLog(ll, message, file, line, function);
    }

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
class OFLog
{
 private:

    /** private constructor, don't create instances of this class
     */
    OFLog() { }

    /** set up the logging and enable it
     *  @param level the verbosity that you want
     */
    static void configureLogger(log4cplus::LogLevel level);

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
    static void configureFromCommandLine(OFCommandLine &cmd, OFConsoleApplication &app);

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
    static OFauto_ptr<log4cplus::helpers::Properties> configProperties_;
};

#endif


/*
 *
 * CVS/RCS Log:
 * $Log: oflog.h,v $
 * Revision 1.12  2010-10-14 13:20:52  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.11  2010-08-05 08:38:11  uli
 * Fixed some warnings from -Wold-style-cast.
 *
 * Revision 1.10  2010-05-14 12:29:55  uli
 * Added new function OFLog::reconfigure() which reinterprets the logger config
 * file. This can e.g. be useful for logging to a different file after fork().
 *
 * Revision 1.9  2010-04-26 12:22:30  uli
 * Fixed a some minor doxygen warnings.
 *
 * Revision 1.8  2010-04-23 11:04:46  uli
 * Updated oflog to log4cplus revision 1200 from PRODUCTION_1_0_x branch.
 *
 * Revision 1.7  2010-03-08 10:50:40  uli
 * Don't use "using" to fix compilation with MSVC6.
 *
 * Revision 1.6  2009-11-17 14:26:21  uli
 * Make OFLogger::getChainedLogLevel() accessible.
 *
 * Revision 1.5  2009-11-06 09:41:22  joergr
 * Changed documentation on log level "trace".
 *
 * Revision 1.4  2009-09-16 10:04:26  joergr
 * Changed OFLogger's copy constructor: use "const &" for the parameter.
 * Added missing documentation on parameters.
 *
 * Revision 1.3  2009-09-14 10:51:05  joergr
 * Removed (now) unused helper function toLogMode().
 *
 * Revision 1.2  2009-08-20 12:08:02  joergr
 * Added documentation on log level values.
 *
 * Revision 1.1  2009-08-19 11:54:00  joergr
 * Added new module "oflog" which is based on log4cplus.
 *
 *
 */
