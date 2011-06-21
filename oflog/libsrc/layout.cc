// Module:  Log4CPLUS
// File:    layout.cxx
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
#include "dcmtk/oflog/helpers/strhelp.h"
#include "dcmtk/oflog/helpers/timehelp.h"
#include "dcmtk/oflog/spi/logevent.h"


using namespace std;
using namespace dcmtk::log4cplus;
using namespace dcmtk::log4cplus::helpers;
using namespace dcmtk::log4cplus::spi;


///////////////////////////////////////////////////////////////////////////////
// dcmtk::log4cplus::SimpleLayout public methods
///////////////////////////////////////////////////////////////////////////////

void
SimpleLayout::formatAndAppend(tostream& output,
                              const InternalLoggingEvent& event)
{
    output << llmCache.toString(event.getLogLevel())
           << DCMTK_LOG4CPLUS_TEXT(" - ")
           << event.getMessage()
           << DCMTK_LOG4CPLUS_TEXT("\n");
}



///////////////////////////////////////////////////////////////////////////////
// dcmtk::log4cplus::TTCCLayout ctors and dtor
///////////////////////////////////////////////////////////////////////////////

TTCCLayout::TTCCLayout(bool use_gmtime_)
: dateFormat( DCMTK_LOG4CPLUS_TEXT("%m-%d-%y %H:%M:%S,%q") ),
  use_gmtime(use_gmtime_)
{
}


TTCCLayout::TTCCLayout(const Properties& properties, tstring&)
: Layout(properties),
  dateFormat( DCMTK_LOG4CPLUS_TEXT("%m-%d-%y %H:%M:%S,%q") ),
  use_gmtime(false)
{
    if(properties.exists( DCMTK_LOG4CPLUS_TEXT("DateFormat") )) {
        dateFormat  = properties.getProperty( DCMTK_LOG4CPLUS_TEXT("DateFormat") );
    }

    tstring tmp = properties.getProperty( DCMTK_LOG4CPLUS_TEXT("Use_gmtime") );
    use_gmtime = (toLower(tmp) == DCMTK_LOG4CPLUS_TEXT("true"));
}


TTCCLayout::~TTCCLayout()
{
}



///////////////////////////////////////////////////////////////////////////////
// dcmtk::log4cplus::TTCCLayout public methods
///////////////////////////////////////////////////////////////////////////////

void
TTCCLayout::formatAndAppend(tostream& output,
                            const InternalLoggingEvent& event)
{
    output << event.getTimestamp().getFormattedTime(dateFormat, use_gmtime)
           << DCMTK_LOG4CPLUS_TEXT(" [")
           << event.getThread()
           << DCMTK_LOG4CPLUS_TEXT("] ")
           << llmCache.toString(event.getLogLevel())
           << DCMTK_LOG4CPLUS_TEXT(" ")
           << event.getLoggerName()
           << DCMTK_LOG4CPLUS_TEXT(" <")
           << event.getNDC()
           << DCMTK_LOG4CPLUS_TEXT("> - ")
           << event.getMessage()
           << DCMTK_LOG4CPLUS_TEXT("\n");
}




