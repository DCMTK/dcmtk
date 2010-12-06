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
 *  Purpose: Simplify the usage of log4cplus to other modules
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-12-06 09:47:30 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofdate.h"
#include "dcmtk/ofstd/oftime.h"

#include "dcmtk/oflog/oflog.h"
#include "dcmtk/oflog/configrt.h"
#include "dcmtk/oflog/consap.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/helpers/socket.h"
#include "dcmtk/oflog/helpers/strhelp.h"

OFauto_ptr<log4cplus::helpers::Properties> OFLog::configProperties_;

OFLogger::OFLogger(const log4cplus::Logger &base)
    : log4cplus::Logger(base)
{
}

static void OFLog_init()
{
    // we don't have to protect against threads here, this function is guaranteed
    // to be called at least once before main() starts -> no threads yet
    static int initialized = 0;
    if (initialized)
        return;
    initialized = 1;

#ifdef HAVE_WINSOCK_H
    // initialize Winsock DLL in order to use gethostname() and the like
    WSAData winSockData;
    // we need at least version 1.1
    WORD winSockVersionNeeded = MAKEWORD(1, 1);
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif

    // we default to a really simple pattern: loglevel_prefix: message\n
    const char *pattern = "%P: %m%n";
    OFauto_ptr<log4cplus::Layout> layout(new log4cplus::PatternLayout(pattern));
    log4cplus::SharedAppenderPtr console(new log4cplus::ConsoleAppender(OFTrue /* logToStrErr */, OFTrue /* immediateFlush */));
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
} static initializer;

void OFLog::configureLogger(log4cplus::LogLevel level)
{
    // This assumes that OFLog_init() was already called. We keep using its
    // setup and just change the log level.
    log4cplus::Logger rootLogger = log4cplus::Logger::getRoot();
    rootLogger.setLogLevel(level);
}

OFLogger OFLog::getLogger(const char *loggerName)
{
    OFLog_init();
    // logger objects have a reference counting copy-constructor, so returning by-value is cheap
    return log4cplus::Logger::getInstance(loggerName);
}

/** Adds our known variables to the Properties instance
 *  @param props instance to add the variables to
 *  @param cmd command line which we use for getting the program name,
 *             may be NULL
 */
static void addVariables(log4cplus::helpers::Properties &props, OFCommandLine* cmd)
{
    OFString date;
    OFString time;

    // Set ${appname}, if possible
    if (cmd) {
        OFString app;
        OFStandard::getFilenameFromPath(app, cmd->getProgramName());
        props.setProperty("appname", app);
    }

    OFDate::getCurrentDate().getISOFormattedDate(date, OFFalse);
    OFTime::getCurrentTime().getISOFormattedTime(time, OFTrue, OFFalse, OFFalse, OFFalse);

    // Set some other useful variables
    props.setProperty("hostname", log4cplus::helpers::getHostname(OFFalse));
    props.setProperty("pid", log4cplus::helpers::convertIntegerToString(OFStandard::getProcessID()));
    props.setProperty("date", date);
    props.setProperty("time", time);
}

void OFLog::reconfigure(OFCommandLine *cmd)
{
    log4cplus::helpers::Properties *props = configProperties_.get();

    // If configProperties_ is a NULL pointer, --log-config was never used and
    // there is nothing we could parse again.
    if (!props)
        return;

    // Add some useful variables to the properties, this really just pretends
    // the user wrote "variable = value" in the config.
    addVariables(*props, cmd);

    unsigned int flags = 0;
    // Recursively expand ${vars}
    flags |= log4cplus::PropertyConfigurator::fRecursiveExpansion;
    // Try to look up ${vars} internally before asking the environment
    flags |= log4cplus::PropertyConfigurator::fShadowEnvironment;

    // Configure log4cplus based on our settings
    log4cplus::PropertyConfigurator conf(*props, log4cplus::Logger::getDefaultHierarchy(), flags);
    conf.configure();
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
    if (cmd.findOption("--debug"))
        level = OFLogger::DEBUG_LOG_LEVEL;
    if (cmd.findOption("--verbose"))
        level = OFLogger::INFO_LOG_LEVEL;
    if (cmd.findOption("--quiet"))
        level = OFLogger::FATAL_LOG_LEVEL;
    cmd.endOptionBlock();

    if (cmd.findOption("--log-level"))
    {
        app.checkConflict("--log-level", "--verbose, --debug or --quiet", level != log4cplus::NOT_SET_LOG_LEVEL);

        app.checkValue(cmd.getValue(logLevel));
        level = log4cplus::getLogLevelManager().fromString(logLevel);
        if (level == log4cplus::NOT_SET_LOG_LEVEL)
            app.printError("Invalid log level for --log-level option");
    }

    if (cmd.findOption("--log-config"))
    {
        app.checkConflict("--log-config", "--log-level", !logLevel.empty());
        app.checkConflict("--log-config", "--verbose, --debug or --quiet", level != log4cplus::NOT_SET_LOG_LEVEL);

        app.checkValue(cmd.getValue(logConfig));

        // check wether config file exists at all and is readable
        if (!OFStandard::fileExists(logConfig))
            app.printError("Specified --log-config file does not exist");
        if (!OFStandard::isReadable(logConfig))
            app.printError("Specified --log-config file cannot be read");

        // There seems to be no way to get an error value here :(
        //log4cplus::PropertyConfigurator::doConfigure(logConfig);

        // This does the same stuff that line above would have done, but it also
        // does some sanity checks on the config file.
        configProperties_.reset(new log4cplus::helpers::Properties(logConfig));
        if (configProperties_->size() == 0)
            app.printError("Specified --log-config file does not contain any settings");
        if (configProperties_->getPropertySubset("log4cplus.").size() == 0)
            app.printError("Specified --log-config file does not contain any valid settings");
        if (!configProperties_->exists("log4cplus.rootLogger"))
            app.printError("Specified --log-config file does not set up log4cplus.rootLogger");

        reconfigure(&cmd);
    }
    else
    {
        // if --log-level was not used...
        if (level == log4cplus::NOT_SET_LOG_LEVEL)
            level = OFLogger::WARN_LOG_LEVEL;

        configureLogger(level);
    }

    log4cplus::Logger rootLogger = log4cplus::Logger::getRoot();
    // if --quiet or something equivalent was used
    if (!rootLogger.isEnabledFor(OFLogger::ERROR_LOG_LEVEL))
    {
        app.setQuietMode();
        log4cplus::helpers::LogLog::getLogLog()->setQuietMode(OFTrue);
    }
    else
    {
        log4cplus::helpers::LogLog::getLogLog()->setQuietMode(OFFalse);
    }

    // print command line arguments
    if (cmd.findOption("--arguments"))
    {
        OFStringStream stream;
        stream << "calling '" << cmd.getProgramName() << "' with " << cmd.getArgCount() << " arguments: ";
        const char *arg;
        // iterate over all command line arguments
        if (cmd.gotoFirstArg())
        {
            do {
                if (cmd.getCurrentArg(arg))
                    stream << "'" << arg << "' ";
            } while (cmd.gotoNextArg());
        }
        stream << OFendl << OFStringStream_ends;
        OFSTRINGSTREAM_GETOFSTRING(stream, tmpString)
        // always output this message, i.e. without checking the log level
        rootLogger.forcedLog(OFLogger::INFO_LOG_LEVEL, tmpString);
    }
}

void OFLog::addOptions(OFCommandLine &cmd)
{
    cmd.addOption("--arguments",            "print expanded command line arguments");
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
 * Revision 1.15  2010-12-06 09:47:30  uli
 * Fixed crash in oflog on Mac OS X 10.4.1 with gcc 4.0.1 due to different order
 * of execution of global destructors.
 *
 * Revision 1.14  2010-10-14 13:14:51  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.13  2010-10-08 12:37:34  uli
 * Fixed an invalid use of OFauto_ptr.
 *
 * Revision 1.12  2010-05-14 12:29:55  uli
 * Added new function OFLog::reconfigure() which reinterprets the logger config
 * file. This can e.g. be useful for logging to a different file after fork().
 *
 * Revision 1.11  2010-01-20 15:18:05  uli
 * Added variables for the appname, date, time, hostname and pid to logger.cfg.
 *
 * Revision 1.10  2009-12-23 12:14:49  joergr
 * Changed output of option --arguments.
 *
 * Revision 1.9  2009-12-08 15:38:59  uli
 * Renaming files to make them fit into 8.3 format
 *
 * Revision 1.8  2009-09-17 14:33:57  joergr
 * Made sure that log4cplus' internal quiet mode is always set appropriately.
 *
 * Revision 1.7  2009-09-16 10:01:06  joergr
 * Changed OFLogger's copy constructor: use "const &" for the parameter.
 *
 * Revision 1.6  2009-09-15 13:02:04  joergr
 * Added initialization of Winsock DLL in order to use gethostname() et al.
 *
 * Revision 1.5  2009-09-14 10:52:31  joergr
 * Introduced new placeholder for the pattern layout: %P can be used to output
 * only the first character of the log level. Used for the default layout.
 * Slightly changed evaluation of log-related command line options.
 * Removed (now) unused helper function toLogMode().
 *
 * Revision 1.4  2009-09-07 10:02:20  joergr
 * Moved --arguments option and corresponding output to oflog module in order
 * to use the correct output stream. Fixed issue with --quiet mode.
 * Moved output of resource identifier back from oflog to the application.
 *
 * Revision 1.3  2009-09-04 12:45:41  joergr
 * Changed default behavior of the logger: output log messages to stderr (not
 * stdout) and flush stream immediately; removed "EARLY STARTUP" prefix from
 * messages which was only used for testing purposes.
 *
 * Revision 1.2  2009-08-20 10:43:30  joergr
 * Added more checks when reading a log configuration from file.
 *
 * Revision 1.1  2009-08-19 11:58:22  joergr
 * Added new module "oflog" which is based on log4cplus.
 *
 *
 *
 */
