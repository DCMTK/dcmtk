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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2009-08-20 12:08:02 $
 *  CVS/RCS Revision: $Revision: 1.2 $
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
    OFLogger(log4cplus::Logger);

    /// these are the log levels that you can feed to isEnabledFor()
    enum LogLevel {
        /// trace: used to "trace" entry and exiting of methods
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

    /** this function is only used internally by OFLOG_FATAL and friends */
    using log4cplus::Logger::forcedLog;
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
     *  @param mode the verbosity that you want
     */
    static void configureLogger(log4cplus::LogLevel level);

 public:

    /// create a new Logger object
    static OFLogger getLogger(const char *name);

    /** set up the logging and enable it
     *  @param mode the verbosity that you want
     */
    static void configure(OFLogger::LogLevel level = OFLogger::WARN_LOG_LEVEL);

    /** turn a list of flags into a logging verbosity
     *  @param debug should debug output and all higher levels be enabled?
     *  @param verbose should verbose output and all higher levels be enabled?
     *  @param quiet should quiet mode be enabled?
     *  @return the highest logging verbosity that was asked for
     */
    static OFLogger::LogLevel toLogMode(OFBool debug = false, OFBool verbose = false, OFBool quiet = false);

    /** handle the command line options used for logging
     *  @param cmd the command line whose options are handled
     *  @param app for debug mode, OFConsoleApplication::printIdentifier() is called
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
 * Revision 1.2  2009-08-20 12:08:02  joergr
 * Added documentation on log level values.
 *
 * Revision 1.1  2009-08-19 11:54:00  joergr
 * Added new module "oflog" which is based on log4cplus.
 *
 *
 *
 */
