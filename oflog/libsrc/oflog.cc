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
 *  Purpose: Simplify the usage of log4cplus to other modules
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2009-08-19 11:58:22 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/oflog/oflog.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/oflog/configurator.h"
#include "dcmtk/oflog/consoleappender.h"
#include "dcmtk/oflog/helpers/loglog.h"

OFLogger::OFLogger(log4cplus::Logger base)
    : log4cplus::Logger(base)
{
}

void OFLog_init()
{
    // we don't have to protect against threads here, this function is guaranteed
    // to be called at least once before main() starts -> no threads yet
    static int initialized = 0;
    if (initialized)
        return;
    initialized = 1;

    // until someone properly configures us, we will allow most log messages and
    // make it clear that we weren't properly initialized yet ("EARLY STARTUP")
    const char *pattern = "EARLY STARTUP: %p: %m%n";
    OFauto_ptr<log4cplus::Layout> layout(new log4cplus::PatternLayout(pattern));
    log4cplus::SharedAppenderPtr console(new log4cplus::ConsoleAppender);
    log4cplus::Logger rootLogger = log4cplus::Logger::getRoot();

    console->setLayout(layout);
    rootLogger.addAppender(console);
    rootLogger.setLogLevel(log4cplus::INFO_LOG_LEVEL);
}

// private class, this class's constructor makes sure that OFLog_init() is
// called at least once before main() runs
class static_OFLog_initializer
{
 public:
     static_OFLog_initializer()
     {
         OFLog_init();
     }

     ~static_OFLog_initializer()
     {
         // without this we leak some small amounts of memory
         log4cplus::getNDC().remove();
     }
} static initializer;

void OFLog::configureLogger(log4cplus::LogLevel level)
{
    // we default to a really simple pattern: message\n
    const char *pattern = "%m%n";
    OFauto_ptr<log4cplus::Layout> layout(new log4cplus::PatternLayout(pattern));
    log4cplus::SharedAppenderPtr console(new log4cplus::ConsoleAppender(false, true));
    log4cplus::Logger rootLogger = log4cplus::Logger::getRoot();

    console->setLayout(layout);

    // if the rootLogger is configured multiple times, don't recursively add
    // appenders to it
    rootLogger.removeAllAppenders();
    rootLogger.addAppender(console);
    rootLogger.setLogLevel(level);

    // are we in "quiet-mode"?
    if (level >= OFLogger::ERROR_LOG_LEVEL)
        log4cplus::helpers::LogLog::getLogLog()->setQuietMode(true);
    else
        log4cplus::helpers::LogLog::getLogLog()->setQuietMode(false);
}

OFLogger OFLog::getLogger(const char *loggerName)
{
    OFLog_init();
    // logger objects have a reference counting copy-constructor, so returning by-value is cheap
    return log4cplus::Logger::getInstance(loggerName);
}

OFLogger::LogLevel OFLog::toLogMode(OFBool debug, OFBool verbose, OFBool quiet)
{
    if (debug)
        return OFLogger::DEBUG_LOG_LEVEL;
    if (verbose)
        return OFLogger::INFO_LOG_LEVEL;
    if (quiet)
        return OFLogger::FATAL_LOG_LEVEL;
    return OFLogger::WARN_LOG_LEVEL;
}

void OFLog::configure(OFLogger::LogLevel level)
{
    configureLogger(level);
}

void OFLog::configureFromCommandLine(OFCommandLine &cmd, OFConsoleApplication &app)
{
    OFString logLevel = "";
    OFString logConfig = "";
    log4cplus::LogLevel level = log4cplus::NOT_SET_LOG_LEVEL;

    cmd.beginOptionBlock();
    OFBool debug = cmd.findOption("--debug");
    OFBool verbose = cmd.findOption("--verbose");
    OFBool quiet = cmd.findOption("--quiet");
    cmd.endOptionBlock();

    if (cmd.findOption("--log-level"))
    {
        app.checkConflict("--log-level", "--debug", debug);
        app.checkConflict("--log-level", "--verbose", verbose);
        app.checkConflict("--log-level", "--quiet", quiet);

        app.checkValue(cmd.getValue(logLevel));
        level = log4cplus::getLogLevelManager().fromString(logLevel);
        if (level == log4cplus::NOT_SET_LOG_LEVEL)
            app.printError("Invalid log level for --log-level option");
    }

    if (cmd.findOption("--log-config"))
    {
        app.checkConflict("--log-config", "--debug", debug);
        app.checkConflict("--log-config", "--verbose", verbose);
        app.checkConflict("--log-config", "--quiet", quiet);
        app.checkConflict("--log-config", "--log-level", level != log4cplus::NOT_SET_LOG_LEVEL);

        app.checkValue(cmd.getValue(logConfig));
        // check whether config file exists at all
        if (!OFStandard::fileExists(logConfig))
            app.printError("Specified --log-config file does not exist");
        // there seems to be no way to get an error value here :(
        log4cplus::PropertyConfigurator::doConfigure(logConfig);
    }
    else
    {
        // if --log-level was not used...
        if (level == log4cplus::NOT_SET_LOG_LEVEL)
            level = toLogMode(debug, verbose, quiet);

        configureLogger(level);
    }

    log4cplus::Logger rootLogger = log4cplus::Logger::getRoot();
    // if --debug or something equivalent was used
    if (rootLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
        app.printIdentifier();
    // if --quiet or something equivalent was used
    if (!rootLogger.isEnabledFor(OFLogger::WARN_LOG_LEVEL))
        app.setQuietMode();
}

void OFLog::addOptions(OFCommandLine &cmd)
{
    cmd.addOption("--quiet",      "-q",     "quiet mode, print no warnings and errors");
    cmd.addOption("--verbose",    "-v",     "verbose mode, print processing details");
    cmd.addOption("--debug",      "-d",     "debug mode, print debug information");
    cmd.addOption("--log-level",  "-ll", 1, "[l]evel: string constant",
                                            "(fatal, error, warn, info, debug, trace)\nuse level l for the logger");
    cmd.addOption("--log-config", "-lc", 1, "[f]ilename: string",
                                            "use config file f for the logger");
}

/*
 *
 * CVS/RCS Log:
 * $Log: oflog.cc,v $
 * Revision 1.1  2009-08-19 11:58:22  joergr
 * Added new module "oflog" which is based on log4cplus.
 *
 *
 *
 */
