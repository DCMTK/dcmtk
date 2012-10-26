// -*- C++ -*-
// Module:  Log4CPLUS
// File:    loggerfactory.h
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

/** @file */

#ifndef DCMTK_LOG4CPLUS_SPI_LOGGER_FACTORY_HEADER
#define DCMTK_LOG4CPLUS_SPI_LOGGER_FACTORY_HEADER

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include "dcmtk/oflog/tstring.h"


namespace dcmtk {
namespace log4cplus {
    // Forward Declarations
    class Logger;
    class Hierarchy;

    namespace spi {
        /**
         * Implement this interface to create new instances of Logger or
         * a sub-class of Logger.
         */
        class DCMTK_LOG4CPLUS_EXPORT LoggerFactory {
        public:
            /**
             * Creates a new <code>Logger</code> object.
             */
            virtual Logger makeNewLoggerInstance(const log4cplus::tstring& name,
                                                 Hierarchy& h) = 0; 
            virtual ~LoggerFactory() = 0;
        };

    } // end namespace spi
} // end namespace log4cplus
} // end namespace dcmtk

#endif // DCMTK_LOG4CPLUS_SPI_LOGGER_FACTORY_HEADER

