// Module:  Log4CPLUS
// File:    win32debugappender.h
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

/** @file */

#ifndef DCMTK__LOG4CPLUS_WIN32DEBUG_APPENDER_HEADER_
#define DCMTK__LOG4CPLUS_WIN32DEBUG_APPENDER_HEADER_

#include "dcmtk/oflog/config.h"
#if defined(_WIN32)

#include "dcmtk/oflog/appender.h"
#include "dcmtk/oflog/helpers/property.h"


namespace dcmtk {
namespace log4cplus {

    /**
     * Prints log events using OutputDebugString().
     */
    class DCMTK_LOG4CPLUS_EXPORT Win32DebugAppender
        : public Appender
    {
    public:
      // Ctors
        Win32DebugAppender();
        Win32DebugAppender(const helpers::Properties& properties, tstring& error);

      // Dtor
        virtual ~Win32DebugAppender();

      // Methods
        virtual void close();

    protected:
        virtual void append(const spi::InternalLoggingEvent& event);

    private:
      // Disallow copying of instances of this class
        Win32DebugAppender(const Win32DebugAppender&);
        Win32DebugAppender& operator=(const Win32DebugAppender&);
    };

} // end namespace log4cplus
} // end namespace dcmtk

#endif // _WIN32
#endif // DCMTK__LOG4CPLUS_WIN32DEBUG_APPENDER_HEADER_

