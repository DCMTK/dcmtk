/*
 *
 *  Copyright (C) 2009-2017, OFFIS e.V.
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
#include "dcmtk/oflog/internal/internal.h"

OFunique_ptr<dcmtk::log4cplus::helpers::Properties> OFLog::configProperties_;

OFLogger::OFLogger(const dcmtk::log4cplus::Logger &base)
    : dcmtk::log4cplus::Logger(base)
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
    OFunique_ptr<dcmtk::log4cplus::Layout> layout(new dcmtk::log4cplus::PatternLayout(pattern));
    dcmtk::log4cplus::SharedAppenderPtr console(new dcmtk::log4cplus::ConsoleAppender(OFTrue /* logToStdErr */, OFTrue /* immediateFlush */));
    dcmtk::log4cplus::Logger rootLogger = dcmtk::log4cplus::Logger::getRoot();

    console->setLayout(OFmove(layout));
    rootLogger.addAppender(console);
    rootLogger.setLogLevel(dcmtk::log4cplus::INFO_LOG_LEVEL);
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

void OFLog::configureLogger(dcmtk::log4cplus::LogLevel level)
{
    // This assumes that OFLog_init() was already called. We keep using its
    // setup and just change the log level.
    dcmtk::log4cplus::Logger rootLogger = dcmtk::log4cplus::Logger::getRoot();
    rootLogger.setLogLevel(level);
}

OFLogger OFLog::getLogger(const char *loggerName)
{
    OFLog_init();
    // logger objects have a reference counting copy-constructor, so returning by-value is cheap
    return dcmtk::log4cplus::Logger::getInstance(loggerName);
}

/** Adds our known variables to the Properties instance
 *  @param props instance to add the variables to
 *  @param cmd command line which we use for getting the program name,
 *             may be NULL
 */
static void addVariables(dcmtk::log4cplus::helpers::Properties &props, OFCommandLine* cmd)
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
    props.setProperty("hostname", dcmtk::log4cplus::helpers::getHostname(OFFalse));
    props.setProperty("pid", dcmtk::log4cplus::helpers::convertIntegerToString(OFStandard::getProcessID()));
    props.setProperty("date", date);
    props.setProperty("time", time);
}

void OFLog::reconfigure(OFCommandLine *cmd)
{
    dcmtk::log4cplus::helpers::Properties *props = configProperties_.get();

    // If configProperties_ is a NULL pointer, --log-config was never used and
    // there is nothing we could parse again.
    if (!props)
        return;

    // Add some useful variables to the properties, this really just pretends
    // the user wrote "variable = value" in the config.
    addVariables(*props, cmd);

    unsigned int flags = 0;
    // Recursively expand ${vars}
    flags |= dcmtk::log4cplus::PropertyConfigurator::fRecursiveExpansion;
    // Try to look up ${vars} internally before asking the environment
    flags |= dcmtk::log4cplus::PropertyConfigurator::fShadowEnvironment;

    // Configure log4cplus based on our settings
    dcmtk::log4cplus::PropertyConfigurator conf(*props, dcmtk::log4cplus::Logger::getDefaultHierarchy(), flags);
    conf.configure();
}

void OFLog::configure(OFLogger::LogLevel level)
{
    configureLogger(level);
}

void OFLog::configureFromCommandLine(OFCommandLine &cmd,
                                     OFConsoleApplication &app,
                                     OFLogger::LogLevel defaultLevel)
{
    OFString logLevel = "";
    OFString logConfig = "";
    dcmtk::log4cplus::LogLevel level = dcmtk::log4cplus::NOT_SET_LOG_LEVEL;

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
        app.checkConflict("--log-level", "--verbose, --debug or --quiet", level != dcmtk::log4cplus::NOT_SET_LOG_LEVEL);

        app.checkValue(cmd.getValue(logLevel));
        level = dcmtk::log4cplus::getLogLevelManager().fromString(logLevel);
        if (level == dcmtk::log4cplus::NOT_SET_LOG_LEVEL)
            app.printError("Invalid log level for --log-level option");
    }

    if (cmd.findOption("--log-config"))
    {
        app.checkConflict("--log-config", "--log-level", !logLevel.empty());
        app.checkConflict("--log-config", "--verbose, --debug or --quiet", level != dcmtk::log4cplus::NOT_SET_LOG_LEVEL);

        app.checkValue(cmd.getValue(logConfig));

        // check whether config file exists at all and is readable
        if (!OFStandard::fileExists(logConfig))
            app.printError("Specified --log-config file does not exist");
        if (!OFStandard::isReadable(logConfig))
            app.printError("Specified --log-config file cannot be read");

        // There seems to be no way to get an error value here :(
        //dcmtk::log4cplus::PropertyConfigurator::doConfigure(logConfig);

        // This does the same stuff that line above would have done, but it also
        // does some sanity checks on the config file.
        configProperties_.reset(new dcmtk::log4cplus::helpers::Properties(logConfig));
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
        if (level == dcmtk::log4cplus::NOT_SET_LOG_LEVEL)
            level = defaultLevel;

        configureLogger(level);
    }

    dcmtk::log4cplus::Logger rootLogger = dcmtk::log4cplus::Logger::getRoot();
    // if --quiet or something equivalent was used
    if (!rootLogger.isEnabledFor(OFLogger::ERROR_LOG_LEVEL))
    {
        app.setQuietMode();
        dcmtk::log4cplus::helpers::LogLog::getLogLog()->setQuietMode(OFTrue);
    }
    else
    {
        dcmtk::log4cplus::helpers::LogLog::getLogLog()->setQuietMode(OFFalse);
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
