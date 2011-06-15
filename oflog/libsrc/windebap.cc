// Module:  Log4CPLUS
// File:    win32debugappender.cxx
// Created: 12/2003
// Author:  Eduardo Francos, Odalio SARL
//
//
// Copyright 2003-2009 Odalio SARL
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


#include "dcmtk/oflog/windebap.h"


using namespace std;
using namespace dcmtk::log4cplus;
using namespace dcmtk::log4cplus::helpers;



///////////////////////////////////////////////////////////////////////////////
// dcmtk::log4cplus::Win32DebugAppender ctors and dtor
///////////////////////////////////////////////////////////////////////////////

Win32DebugAppender::Win32DebugAppender()
{
}


Win32DebugAppender::Win32DebugAppender(
    const helpers::Properties& properties, tstring& error)
    : Appender(properties)
{
}



Win32DebugAppender::~Win32DebugAppender()
{
    destructorImpl();
}



///////////////////////////////////////////////////////////////////////////////
// dcmtk::log4cplus::Win32DebugAppender public methods
///////////////////////////////////////////////////////////////////////////////

void
Win32DebugAppender::close()
{
    closed = true;
}



///////////////////////////////////////////////////////////////////////////////
// dcmtk::log4cplus::Win32DebugAppender protected methods
///////////////////////////////////////////////////////////////////////////////

// This method does not need to be locked since it is called by
// doAppend() which performs the locking
void
Win32DebugAppender::append(const spi::InternalLoggingEvent& event)
{
    tostringstream buf;
    layout->formatAndAppend(buf, event);
    OFSTRINGSTREAM_GETSTR(buf, s);
    ::OutputDebugString(s);
    OFSTRINGSTREAM_FREESTR(s);
}
