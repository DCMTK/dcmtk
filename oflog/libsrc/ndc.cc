// Module:  Log4CPLUS
// File:    ndc.cxx
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

#include "dcmtk/oflog/config.h"
#include "dcmtk/oflog/ndc.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/helpers/pointer.h"
//#include <exception>
//#include <utility>
//#include <vector>

using namespace dcmtk::log4cplus;
using namespace dcmtk::log4cplus::helpers;



///////////////////////////////////////////////////////////////////////////////
// public methods
///////////////////////////////////////////////////////////////////////////////

NDC&
dcmtk::log4cplus::getNDC()
{
    static NDC singleton;
    return singleton;
}



///////////////////////////////////////////////////////////////////////////////
// dcmtk::log4cplus::DiagnosticContext ctors
///////////////////////////////////////////////////////////////////////////////

DiagnosticContext::DiagnosticContext(const tstring& message_, DiagnosticContext* parent)
 : message(message_),
   fullMessage( (  (parent == NULL)
                 ? message
                 : parent->fullMessage + DCMTK_LOG4CPLUS_TEXT(" ") + message) )
{
}


DiagnosticContext::DiagnosticContext(const tstring& message_)
 : message(message_),
   fullMessage(message)
{
}



///////////////////////////////////////////////////////////////////////////////
// dcmtk::log4cplus::NDC ctor and dtor
///////////////////////////////////////////////////////////////////////////////

NDC::NDC()
 : threadLocal(DCMTK_LOG4CPLUS_THREAD_LOCAL_INIT (0))
{
}


NDC::~NDC()
{
    DCMTK_LOG4CPLUS_THREAD_LOCAL_CLEANUP( threadLocal );
}



///////////////////////////////////////////////////////////////////////////////
// dcmtk::log4cplus::NDC public methods
///////////////////////////////////////////////////////////////////////////////

void
NDC::clear()
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL) {
            delete ptr;
            DCMTK_LOG4CPLUS_SET_THREAD_LOCAL_VALUE( threadLocal, NULL );
        }
    }
    catch(...) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("NDC::clear()- exception occured"));
    }
}


DiagnosticContextStack
NDC::cloneStack()
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL) {
            return DiagnosticContextStack(*ptr);
        }
    }
    catch(...) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("NDC::cloneStack()- exception occured"));
    }

    return DiagnosticContextStack();
}


void
NDC::inherit(const DiagnosticContextStack& stack)
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        delete ptr;

        ptr = new DiagnosticContextStack(stack);
        DCMTK_LOG4CPLUS_SET_THREAD_LOCAL_VALUE( threadLocal, ptr );
    }
    catch(...) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("NDC::inherit()- exception occured"));
    }
}


tstring
NDC::get()
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL && !ptr->empty()) {
            return ptr->top().fullMessage;
        }
    }
    catch(...) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("NDC::get()- exception occured"));
    }

    return DCMTK_LOG4CPLUS_TEXT("");
}


size_t
NDC::getDepth()
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL) {
            return ptr->size();
        }
    }
    catch(...) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("NDC::getDepth()- exception occured"));
    }

    return 0;
}


tstring
NDC::pop()
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL && !ptr->empty()) {
            DiagnosticContext dc = ptr->top();
            ptr->pop();
            if(ptr->empty()) {
                // If the NDC stack is empty we will delete it so that we can avoid
                // most memory leaks if Threads don't call remove when exiting
                delete ptr;
                DCMTK_LOG4CPLUS_SET_THREAD_LOCAL_VALUE( threadLocal, NULL );
            }
            return dc.message;
        }
    }
    catch(...) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("NDC::pop()- exception occured"));
    }

    return DCMTK_LOG4CPLUS_TEXT("");
}


tstring
NDC::peek()
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL && !ptr->empty()) {
            return ptr->top().message;
        }
    }
    catch(...) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("NDC::peek()- exception occured"));
    }

    return DCMTK_LOG4CPLUS_TEXT("");
}


void
NDC::push(const tstring& message)
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr == NULL) {
            ptr = new DiagnosticContextStack();
            DCMTK_LOG4CPLUS_SET_THREAD_LOCAL_VALUE( threadLocal, ptr );
        }

        if(ptr->empty()) {
            ptr->push( DiagnosticContext(message, NULL) );
        }
        else {
            DiagnosticContext dc = ptr->top();
            ptr->push( DiagnosticContext(message, &dc) );
        }
    }
    catch(...) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("NDC::push()- exception occured"));
    }
}


void
NDC::remove()
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        delete ptr;
        DCMTK_LOG4CPLUS_SET_THREAD_LOCAL_VALUE( threadLocal, NULL );
    }
    catch(...) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("NDC::remove()- exception occured"));
    }
}


void
NDC::setMaxDepth(size_t maxDepth)
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL) {
            while(maxDepth < ptr->size()) {
                ptr->pop();
            }
        }
    }
    catch(...) {
        getLogLog().error(DCMTK_LOG4CPLUS_TEXT("NDC::setMaxDepth()- exception occured"));
    }
}


DiagnosticContextStack* NDC::getPtr()
{
    return OFstatic_cast(DiagnosticContextStack*,
        DCMTK_LOG4CPLUS_GET_THREAD_LOCAL_VALUE( threadLocal ));
}


//
//
//

NDCContextCreator::NDCContextCreator(const tstring& msg)
{
    getNDC().push(msg);
}


NDCContextCreator::~NDCContextCreator()
{
    getNDC().pop();
}
