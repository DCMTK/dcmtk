// Module:  Log4CPLUS
// File:    ndc.cxx
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

#include "dcmtk/oflog/ndc.h"
#include "dcmtk/oflog/internal/internal.h"
#include <utility>
#include <algorithm>


namespace dcmtk
{
namespace log4cplus
{



///////////////////////////////////////////////////////////////////////////////
// log4cplus::DiagnosticContext ctors
///////////////////////////////////////////////////////////////////////////////


namespace
{


static
void
init_full_message (log4cplus::tstring & fullMessage,
    log4cplus::tstring const & message, DiagnosticContext const * parent)
{
    if (parent)
    {
        fullMessage.reserve (parent->fullMessage.size () + 1
            + message.size ());
        fullMessage = parent->fullMessage;
        fullMessage += DCMTK_LOG4CPLUS_TEXT(" ");
        fullMessage += message;
    }
    else
        fullMessage = message;
}


} // namespace


DiagnosticContext::DiagnosticContext()
    : message()
    , fullMessage()
{
}

DiagnosticContext::DiagnosticContext(const log4cplus::tstring& message_,
                                     DiagnosticContext const * parent)
    : message(message_)
    , fullMessage()
{
    init_full_message (fullMessage, message, parent);
}


DiagnosticContext::DiagnosticContext(tchar const * message_,
                                     DiagnosticContext const * parent)
    : message(message_)
    , fullMessage()
{
    init_full_message (fullMessage, message, parent);
}


DiagnosticContext::DiagnosticContext(const log4cplus::tstring& message_)
    : message(message_)
    , fullMessage(message)
{
}


DiagnosticContext::DiagnosticContext(tchar const * message_)
    : message(message_)
    , fullMessage(message)
{
}


DiagnosticContext::DiagnosticContext (DiagnosticContext const & other)
    : message (other.message)
    , fullMessage (other.fullMessage)
{ }


DiagnosticContext & DiagnosticContext::operator = (
    DiagnosticContext const & other)
{
    DiagnosticContext (other).swap (*this);
    return *this;
}


#if defined (DCMTK_LOG4CPLUS_HAVE_RVALUE_REFS)
DiagnosticContext::DiagnosticContext (DiagnosticContext && other)
    : message (STD_NAMESPACE move (other.message))
    , fullMessage (STD_NAMESPACE move (other.fullMessage))
{ }


DiagnosticContext &
DiagnosticContext::operator = (DiagnosticContext && other)
{
    DiagnosticContext (STD_NAMESPACE move (other)).swap (*this);
    return *this;
}

#endif


void
DiagnosticContext::swap (DiagnosticContext & other)
{
    using STD_NAMESPACE swap;
    swap (message, other.message);
    swap (fullMessage, other.fullMessage);
}

///////////////////////////////////////////////////////////////////////////////
// log4cplus::NDC ctor and dtor
///////////////////////////////////////////////////////////////////////////////

NDC::NDC() 
{ }


NDC::~NDC() 
{ }



///////////////////////////////////////////////////////////////////////////////
// log4cplus::NDC public methods
///////////////////////////////////////////////////////////////////////////////

void
NDC::clear()
{
    DiagnosticContextStack* ptr = getPtr();
    DiagnosticContextStack ().swap (*ptr);
}


DiagnosticContextStack
NDC::cloneStack() const
{
    DiagnosticContextStack* ptr = getPtr();
    return DiagnosticContextStack(*ptr);
}


void 
NDC::inherit(const DiagnosticContextStack& stack)
{
    DiagnosticContextStack* ptr = getPtr();
    DiagnosticContextStack (stack).swap (*ptr);
}


log4cplus::tstring const &
NDC::get() const
{
    DiagnosticContextStack* ptr = getPtr();
    if(!ptr->empty())
        return ptr->back().fullMessage;
    else
        return internal::empty_str;
}


size_t
NDC::getDepth() const
{
    DiagnosticContextStack* ptr = getPtr();
    return ptr->size();
}


log4cplus::tstring 
NDC::pop()
{
    DiagnosticContextStack* ptr = getPtr();
    if(!ptr->empty())
    {
        tstring message;
        message.swap (ptr->back ().message);
        ptr->pop_back();
        return message;
    }
    else
        return log4cplus::tstring ();
}


void
NDC::pop_void ()
{
    DiagnosticContextStack* ptr = getPtr ();
    if (! ptr->empty ())
        ptr->pop_back ();
}


log4cplus::tstring const &
NDC::peek() const
{
    DiagnosticContextStack* ptr = getPtr();
    if(!ptr->empty())
        return ptr->back().message;
    else
        return internal::empty_str;
}


void 
NDC::push(const log4cplus::tstring& message)
{
    DiagnosticContextStack* ptr = getPtr();
    if (ptr->empty())
        ptr->push_back( DiagnosticContext(message, NULL) );
    else
    {
        DiagnosticContext const & dc = ptr->back();
        ptr->push_back( DiagnosticContext(message, &dc) );
    }
}


void 
NDC::push(tchar const * message)
{
    DiagnosticContextStack* ptr = getPtr();
    if (ptr->empty())
        ptr->push_back( DiagnosticContext(message, NULL) );
    else
    {
        DiagnosticContext const & dc = ptr->back();
        ptr->push_back( DiagnosticContext(message, &dc) );
    }
}


void 
NDC::remove()
{
    DiagnosticContextStack* ptr = getPtr();
    DiagnosticContextStack ().swap (*ptr);
}


void 
NDC::setMaxDepth(size_t maxDepth)
{
    DiagnosticContextStack* ptr = getPtr();
    while(maxDepth < ptr->size())
        ptr->pop_back();
}


DiagnosticContextStack* NDC::getPtr()
{
    internal::per_thread_data * ptd = internal::get_ptd ();
    return &ptd->ndc_dcs;
}


//
//
//

NDCContextCreator::NDCContextCreator(const log4cplus::tstring& msg) 
{ 
    getNDC().push(msg); 
}


NDCContextCreator::NDCContextCreator(tchar const * msg)
{
    getNDC().push(msg);
}


NDCContextCreator::~NDCContextCreator() 
{ 
    getNDC().pop_void(); 
}


} // namespace log4cplus
} // end namespace dcmtk
