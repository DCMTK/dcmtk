// Module:  Log4CPLUS
// File:    consoleappender.cxx
// Created: 6/2001
// Author:  Tad E. Smith
//
//
// Copyright (C) Tad E. Smith  All rights reserved.
//
// This software is published under the terms of the Apache Software
// License version 1.1, a copy of which has been included with this
// distribution in the LICENSE.APL file.
//

#include "dcmtk/oflog/layout.h"
#include "dcmtk/oflog/consap.h"
#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/helpers/strhelp.h"
#include "dcmtk/oflog/spi/logevent.h"
#include "dcmtk/ofstd/ofconsol.h"

using namespace std;
using namespace log4cplus::helpers;


//////////////////////////////////////////////////////////////////////////////
// log4cplus::ConsoleAppender ctors and dtor
//////////////////////////////////////////////////////////////////////////////

log4cplus::ConsoleAppender::ConsoleAppender(bool logToStdErr, bool immediateFlush)
: logToStdErr(logToStdErr),
  immediateFlush(immediateFlush)
{
}



log4cplus::ConsoleAppender::ConsoleAppender(const log4cplus::helpers::Properties properties)
: Appender(properties),
  logToStdErr(false),
  immediateFlush(false)
{
    tstring val = toLower(properties.getProperty(LOG4CPLUS_TEXT("logToStdErr")));
    if(val == LOG4CPLUS_TEXT("true")) {
        logToStdErr = true;
    }
    if(properties.exists( LOG4CPLUS_TEXT("ImmediateFlush") )) {
        tstring tmp = properties.getProperty( LOG4CPLUS_TEXT("ImmediateFlush") );
        immediateFlush = (toLower(tmp) == LOG4CPLUS_TEXT("true"));
    }
}



log4cplus::ConsoleAppender::~ConsoleAppender()
{
    destructorImpl();
}



//////////////////////////////////////////////////////////////////////////////
// log4cplus::ConsoleAppender public methods
//////////////////////////////////////////////////////////////////////////////

void
log4cplus::ConsoleAppender::close()
{
    getLogLog().debug(LOG4CPLUS_TEXT("Entering ConsoleAppender::close().."));
    closed = true;
}



//////////////////////////////////////////////////////////////////////////////
// log4cplus::ConsoleAppender protected methods
//////////////////////////////////////////////////////////////////////////////

void
log4cplus::ConsoleAppender::append(const spi::InternalLoggingEvent& event)
{
    LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( getLogLog().mutex )
        log4cplus::tostream& output = (logToStdErr ? ofConsole.lockCerr() : ofConsole.lockCout());
        layout->formatAndAppend(output, event);
        if(immediateFlush) {
            output.flush();
        }
        if (logToStdErr) {
            ofConsole.unlockCerr();
        } else {
            ofConsole.unlockCout();
        }
    LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
}
