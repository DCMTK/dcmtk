// -*- C++ -*-
// Module:  Log4CPLUS
// File:    hierlock.h
// Created: 8/2003
// Author:  Tad E. Smith
//
//
// Copyright 2003-2010 Tad E. Smith
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

#ifndef DCMTK_LOG4CPLUS_HIERARCHY_LOCKER_HEADER_
#define DCMTK_LOG4CPLUS_HIERARCHY_LOCKER_HEADER_

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include "dcmtk/oflog/tstring.h"
#include "dcmtk/oflog/appender.h"
#include "dcmtk/oflog/logger.h"


namespace dcmtk
{
namespace log4cplus
{

    class Hierarchy;


    /**
     * This is used to lock a Hierarchy.  The dtor unlocks the Hierarchy.
     */
    class DCMTK_LOG4CPLUS_EXPORT HierarchyLocker {
    public:
      // ctor & dtor
        HierarchyLocker(Hierarchy& h);
        ~HierarchyLocker();
        
        /**
         * Calls the <code>resetConfiguration()</code> method on the locked Hierarchy.
         */
        void resetConfiguration(); 
        
        /**
         * Calls the <code>getInstance()</code> method on the locked Hierarchy.
         */
        Logger getInstance(const log4cplus::tstring& name);
        
        /**
         * Calls the <code>getInstance()</code> method on the locked Hierarchy.
         */
        Logger getInstance(const log4cplus::tstring& name, spi::LoggerFactory& factory);
        
        void addAppender(Logger &logger, log4cplus::SharedAppenderPtr& appender);
        
    private:
      // Data
        Hierarchy& h;
        log4cplus::thread::MutexGuard hierarchyLocker;
        LoggerList loggerList;
    };

} // end namespace log4cplus
} // end namespace dcmtk

#endif // DCMTK_LOG4CPLUS_HIERARCHY_LOCKER_HEADER_

