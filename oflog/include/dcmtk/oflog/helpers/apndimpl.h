// -*- C++ -*-
// Module:  Log4CPLUS
// File:    appenderattachableimpl.h
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

#ifndef DCMTK_LOG4CPLUS_HELPERS_APPENDER_ATTACHABLE_IMPL_HEADER_
#define DCMTK_LOG4CPLUS_HELPERS_APPENDER_ATTACHABLE_IMPL_HEADER_

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include "dcmtk/oflog/tstring.h"
#include "dcmtk/oflog/helpers/pointer.h"
#include "dcmtk/oflog/spi/apndatch.h"
#include "dcmtk/oflog/thread/syncprim.h"

#include <memory>
#include "dcmtk/ofstd/ofvector.h"


namespace dcmtk {
namespace log4cplus {
    namespace helpers {
 
        /**
         * This Interface is for attaching Appenders to objects.
         */
        class DCMTK_LOG4CPLUS_EXPORT AppenderAttachableImpl 
            : public log4cplus::spi::AppenderAttachable
        {
        public:
          // Data
            thread::Mutex appender_list_mutex;

          // Ctors
            AppenderAttachableImpl();

          // Dtor
            virtual ~AppenderAttachableImpl();

          // Methods
            /**
             * Add an appender.  If the appender is already in the list in
             * won't be added again.
             */
            virtual void addAppender(SharedAppenderPtr newAppender);

            /**
             * Get all previously added appenders as an vectory.
             */
            virtual SharedAppenderPtrList getAllAppenders();

            /**
             * Look for an attached appender named as <code>name</code>.
             *
             * Return the appender with that name if in the list. Return null
             * otherwise.  
             */
            virtual SharedAppenderPtr getAppender(const log4cplus::tstring& name);

            /**
             * Remove all previously added appenders.
             */
            virtual void removeAllAppenders();

            /**
             * Remove the appender passed as parameter from the list of appenders.
             */
            virtual void removeAppender(SharedAppenderPtr appender);

            /**
             * Remove the appender with the name passed as parameter from the
             * list of appenders.  
             */
            virtual void removeAppender(const log4cplus::tstring& name);

            /**
             * Call the <code>doAppend</code> method on all attached appenders.  
             */
            int appendLoopOnAppenders(const spi::InternalLoggingEvent& event) const;

        protected:
          // Types
            typedef OFVector<SharedAppenderPtr> ListType;

          // Data
            /** Array of appenders. */
            ListType appenderList;

        private:
            AppenderAttachableImpl(AppenderAttachableImpl const &);
            AppenderAttachableImpl & operator = (AppenderAttachableImpl const &);
        };  // end class AppenderAttachableImpl

    } // end namespace helpers
} // end namespace log4cplus
} // end namespace dcmtk

#endif // DCMTK_LOG4CPLUS_HELPERS_APPENDER_ATTACHABLE_IMPL_HEADER_

