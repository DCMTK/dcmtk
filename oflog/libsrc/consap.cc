// Module:  Log4CPLUS
// File:    consoleappender.cxx
// Created: 6/2001
// Author:  Tad E. Smith
//
//
// Copyright 2001-2009 Tad E. Smith
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

#include "dcmtk/oflog/layout.h"
#include "dcmtk/oflog/consap.h"
#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/helpers/strhelp.h"
#include "dcmtk/oflog/spi/logevent.h"
#include "dcmtk/ofstd/ofconsol.h"

using namespace std;
using namespace dcmtk::log4cplus;
using namespace dcmtk::log4cplus::helpers;


//////////////////////////////////////////////////////////////////////////////
// dcmtk::log4cplus::ConsoleAppender ctors and dtor
//////////////////////////////////////////////////////////////////////////////

ConsoleAppender::ConsoleAppender(bool logToStdErr_, bool immediateFlush_)
: logToStdErr(logToStdErr_),
  immediateFlush(immediateFlush_)
{
}



ConsoleAppender::ConsoleAppender(const helpers::Properties properties, tstring&)
: Appender(properties),
  logToStdErr(false),
  immediateFlush(false)
{
    tstring val = toLower(properties.getProperty(DCMTK_LOG4CPLUS_TEXT("logToStdErr")));
    if(val == DCMTK_LOG4CPLUS_TEXT("true")) {
        logToStdErr = true;
    }
    if(properties.exists( DCMTK_LOG4CPLUS_TEXT("ImmediateFlush") )) {
        tstring tmp = properties.getProperty( DCMTK_LOG4CPLUS_TEXT("ImmediateFlush") );
        immediateFlush = (toLower(tmp) == DCMTK_LOG4CPLUS_TEXT("true"));
    }
}



ConsoleAppender::~ConsoleAppender()
{
    destructorImpl();
}



//////////////////////////////////////////////////////////////////////////////
// dcmtk::log4cplus::ConsoleAppender public methods
//////////////////////////////////////////////////////////////////////////////

void
ConsoleAppender::close()
{
    getLogLog().debug(DCMTK_LOG4CPLUS_TEXT("Entering ConsoleAppender::close().."));
    closed = true;
}



//////////////////////////////////////////////////////////////////////////////
// ConsoleAppender protected methods
//////////////////////////////////////////////////////////////////////////////

// Normally, append() methods do not need to be locked since they are
// called by doAppend() which performs the locking.  However, this locks
// on the LogLog instance, so we don't have multiple threads writing to
// tcout and tcerr
void
ConsoleAppender::append(const spi::InternalLoggingEvent& event)
{
    DCMTK_LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( getLogLog().mutex )
        tostream& output = (logToStdErr ? ofConsole.lockCerr() : ofConsole.lockCout());
        layout->formatAndAppend(output, event);
        if(immediateFlush) {
            output.flush();
        }
        if (logToStdErr) {
            ofConsole.unlockCerr();
        } else {
            ofConsole.unlockCout();
        }
    DCMTK_LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
}


