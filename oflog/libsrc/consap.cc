// Module:  Log4CPLUS
// File:    consoleappender.cxx
// Created: 6/2001
// Author:  Tad E. Smith
//
//
// Copyright 2001-2010 Tad E. Smith
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
#include "dcmtk/oflog/helpers/property.h"
#include "dcmtk/oflog/spi/logevent.h"
#include "dcmtk/oflog/thread/syncpub.h"
#include <ostream>


namespace dcmtk
{
namespace log4cplus
{


namespace helpers
{

extern log4cplus::thread::Mutex const & getConsoleOutputMutex ();

} // namespace helpers


log4cplus::thread::Mutex const &
ConsoleAppender::getOutputMutex ()
{
    return helpers::getConsoleOutputMutex ();
}


//////////////////////////////////////////////////////////////////////////////
// ConsoleAppender ctors and dtor
//////////////////////////////////////////////////////////////////////////////

ConsoleAppender::ConsoleAppender(bool logToStdErr_,
    bool immediateFlush_)
: logToStdErr(logToStdErr_),
  immediateFlush(immediateFlush_)
{
}



ConsoleAppender::ConsoleAppender(const helpers::Properties & properties)
: Appender(properties),
  logToStdErr(false),
  immediateFlush(false)
{
    properties.getBool (logToStdErr, DCMTK_LOG4CPLUS_TEXT("logToStdErr"));
    properties.getBool (immediateFlush, DCMTK_LOG4CPLUS_TEXT("ImmediateFlush"));
}



ConsoleAppender::~ConsoleAppender()
{
    destructorImpl();
}



//////////////////////////////////////////////////////////////////////////////
// ConsoleAppender public methods
//////////////////////////////////////////////////////////////////////////////

void 
ConsoleAppender::close()
{
    helpers::getLogLog().debug(
        DCMTK_LOG4CPLUS_TEXT("Entering ConsoleAppender::close().."));
    closed = true;
}



//////////////////////////////////////////////////////////////////////////////
// ConsoleAppender protected methods
//////////////////////////////////////////////////////////////////////////////

void
ConsoleAppender::append(const spi::InternalLoggingEvent& event)
{
    thread::MutexGuard guard (getOutputMutex ());

    tostream& output = (logToStdErr ? tcerr : tcout);
    layout->formatAndAppend(output, event);
    if(immediateFlush) {
        output.flush();
    }
}


} // namespace log4cplus
} // end namespace dcmtk
