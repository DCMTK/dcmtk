/*
 *
 *  Copyright (C) 2009, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  oflog
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Simplify the usage of log4cplus to other modules (Header)
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2009-11-17 14:26:21 $
 *  CVS/RCS Revision: $Revision: 1.6 $
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
     *  @param level the log level to check for
     *  @return true if messages on this level won't be discarded
     */
    using log4cplus::Logger::isEnabledFor;

    /// this function is only used internally by OFLOG_FATAL and friends
    using log4cplus::Logger::forcedLog;

    /** Get the logger's log level.
     *  One of the checks that isEnabledFor() does looks like this:
     *    if (getChainedLogLevel() < level)
     *        return false;
     *  @return the log level to which this logger is set.
     */
    using log4cplus::Logger::getChainedLogLevel;
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
};

#endif


/*
 *
 * CVS/RCS Log:
 * $Log: oflog.h,v $
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
