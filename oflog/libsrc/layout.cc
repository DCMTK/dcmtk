// Module:  Log4CPLUS
// File:    layout.cxx
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
#include "dcmtk/oflog/helpers/strhelp.h"
#include "dcmtk/oflog/helpers/timehelp.h"
#include "dcmtk/oflog/spi/logevent.h"
#include "dcmtk/oflog/helpers/property.h"
#include "dcmtk/oflog/internal/internal.h"
#include <ostream>
#include <iomanip>


namespace dcmtk
{
namespace log4cplus
{

void formatRelativeTimestamp (log4cplus::tostream & output,
    log4cplus::spi::InternalLoggingEvent const & event);

void
formatRelativeTimestamp (log4cplus::tostream & output,
    log4cplus::spi::InternalLoggingEvent const & event)
{
    helpers::Time const rel_time
        = event.getTimestamp () - getTTCCLayoutTimeBase ();
    tchar const old_fill = output.fill ();
    time_t const sec = rel_time.sec ();
 
    if (sec != 0)
        output << sec << STD_NAMESPACE setfill (DCMTK_LOG4CPLUS_TEXT ('0')) << STD_NAMESPACE setw (3);
 
    output << rel_time.usec () / 1000;
    output.fill (old_fill);
}

//
//
//


Layout::Layout ()
    : llmCache(getLogLevelManager())
{ }


Layout::Layout (const log4cplus::helpers::Properties&)
    : llmCache(getLogLevelManager())
{ }


Layout::~Layout()
{ }


///////////////////////////////////////////////////////////////////////////////
// log4cplus::SimpleLayout public methods
///////////////////////////////////////////////////////////////////////////////

SimpleLayout::SimpleLayout ()
{ }


SimpleLayout::SimpleLayout (const helpers::Properties& properties)
    : Layout (properties)
{ }


SimpleLayout::~SimpleLayout()
{ }


void
SimpleLayout::formatAndAppend(log4cplus::tostream& output, 
                              const log4cplus::spi::InternalLoggingEvent& event)
{
    output << llmCache.toString(event.getLogLevel()) 
           << DCMTK_LOG4CPLUS_TEXT(" - ")
           << event.getMessage() 
           << DCMTK_LOG4CPLUS_TEXT("\n");
}



///////////////////////////////////////////////////////////////////////////////
// log4cplus::TTCCLayout ctors and dtor
///////////////////////////////////////////////////////////////////////////////

TTCCLayout::TTCCLayout(bool use_gmtime_)
    : dateFormat()
    , use_gmtime(use_gmtime_)
{
}


TTCCLayout::TTCCLayout(const log4cplus::helpers::Properties& properties)
    : Layout(properties)
    , dateFormat(properties.getProperty (DCMTK_LOG4CPLUS_TEXT("DateFormat"),
            internal::empty_str))
    , use_gmtime(false)
{
    properties.getBool (use_gmtime, DCMTK_LOG4CPLUS_TEXT("Use_gmtime"));
}


TTCCLayout::~TTCCLayout()
{ }



///////////////////////////////////////////////////////////////////////////////
// log4cplus::TTCCLayout public methods
///////////////////////////////////////////////////////////////////////////////

void
TTCCLayout::formatAndAppend(log4cplus::tostream& output, 
                            const log4cplus::spi::InternalLoggingEvent& event)
{
     if (dateFormat.empty ())
         formatRelativeTimestamp (output, event);
     else
         output << event.getTimestamp().getFormattedTime(dateFormat,
             use_gmtime);

    output << DCMTK_LOG4CPLUS_TEXT(" [")
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


} // namespace log4cplus
} // end namespace dcmtk
