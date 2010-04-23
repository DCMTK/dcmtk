// Module:  Log4CPLUS
// File:    loggerimpl.cxx
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

#include "dcmtk/oflog/spi/logimpl.h"
#include "dcmtk/oflog/appender.h"
#include "dcmtk/oflog/hierarchy.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/spi/logevent.h"
#include "dcmtk/oflog/spi/rootlog.h"
//#include <stdexcept>

using namespace log4cplus;
using namespace log4cplus::helpers;
using namespace log4cplus::spi;



//////////////////////////////////////////////////////////////////////////////
// Logger Constructors and Destructor
//////////////////////////////////////////////////////////////////////////////
LoggerImpl::LoggerImpl(const log4cplus::tstring& name_, Hierarchy& h)
  : name(name_),
    ll(NOT_SET_LOG_LEVEL),
    parent(NULL),
    additive(true),
    hierarchy(h)
{
}


LoggerImpl::~LoggerImpl()
{
}


//////////////////////////////////////////////////////////////////////////////
// Logger Methods
//////////////////////////////////////////////////////////////////////////////

void
LoggerImpl::callAppenders(const InternalLoggingEvent& event)
{
    int writes = 0;
    for(const LoggerImpl* c = this; c != NULL; c=c->parent.get()) {
        writes += c->appendLoopOnAppenders(event);
        if(!c->additive) {
            break;
        }
    }

    // No appenders in hierarchy, warn user only once.
    if(!hierarchy.emittedNoAppenderWarning && writes == 0) {
        getLogLog().error(  LOG4CPLUS_TEXT("No appenders could be found for logger (")
                          + getName()
                          + LOG4CPLUS_TEXT(")."));
        getLogLog().error(LOG4CPLUS_TEXT("Please initialize the log4cplus system properly."));
        hierarchy.emittedNoAppenderWarning = true;
    }
}


void
LoggerImpl::closeNestedAppenders()
{
    SharedAppenderPtrList appenders = getAllAppenders();
    for(SharedAppenderPtrListIterator it=appenders.begin(); it!=appenders.end(); ++it)
    {
        (*it)->close();
    }
}


bool
LoggerImpl::isEnabledFor(LogLevel ll_) const
{
    if(hierarchy.disableValue >= ll_) {
        return false;
    }
    return ll_ >= getChainedLogLevel();
}


void
LoggerImpl::log(LogLevel ll_,
                const log4cplus::tstring& message,
                const char* file,
                int line,
                const char* function)
{
    if(isEnabledFor(ll_)) {
        forcedLog(ll_, message, file, line, function);
    }
}



LogLevel
LoggerImpl::getChainedLogLevel() const
{
    for(const LoggerImpl *c=this; c != NULL; c=c->parent.get()) {
        if(c->ll != NOT_SET_LOG_LEVEL) {
            return c->ll;
        }
    }

    getLogLog().error( LOG4CPLUS_TEXT("LoggerImpl::getChainedLogLevel()- No valid LogLevel found") );
    //throw STD_NAMESPACE runtime_error("No valid LogLevel found");

    // This can only happen if the root logger is set to NOT_SET_LOG_LEVEL which
    // should *never* happen. Let's just invent something in this case.
    return TRACE_LOG_LEVEL;
}


Hierarchy&
LoggerImpl::getHierarchy() const
{
    return hierarchy;
}


bool
LoggerImpl::getAdditivity() const
{
    return additive;
}


void
LoggerImpl::setAdditivity(bool additive_)
{
    this->additive = additive_;
}


void
LoggerImpl::forcedLog(LogLevel ll_,
                      const log4cplus::tstring& message,
                      const char* file,
                      int line,
                      const char* function)
{
    callAppenders(spi::InternalLoggingEvent(this->getName(), ll_, message, file, line, function));
}



