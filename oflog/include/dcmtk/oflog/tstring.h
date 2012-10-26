// -*- C++ -*-
// Module:  Log4CPLUS
// File:    tstring.h
// Created: 4/2003
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

#ifndef DCMTK_LOG4CPLUS_TSTRING_HEADER_
#define DCMTK_LOG4CPLUS_TSTRING_HEADER_

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/oflog/tchar.h"

namespace dcmtk
{
namespace log4cplus
{

typedef OFString tstring;


namespace helpers
{

inline
OFString
tostring (char const * str)
{
    return OFString (str);
}


inline
OFString
tostring (OFString const & str)
{
    return str;
}



inline
STD_NAMESPACE wstring
towstring (wchar_t const * str)
{
    return STD_NAMESPACE wstring (str);
}

inline
STD_NAMESPACE wstring
towstring (STD_NAMESPACE wstring const & str)
{
    return str;
}

DCMTK_LOG4CPLUS_EXPORT OFString tostring(const STD_NAMESPACE wstring&);
DCMTK_LOG4CPLUS_EXPORT OFString tostring(wchar_t const *);

DCMTK_LOG4CPLUS_EXPORT STD_NAMESPACE wstring towstring(const OFString&);
DCMTK_LOG4CPLUS_EXPORT STD_NAMESPACE wstring towstring(char const *);

} // namespace helpers

#ifdef UNICODE

#define DCMTK_LOG4CPLUS_C_STR_TO_TSTRING(STRING) log4cplus::helpers::towstring(STRING)
#define DCMTK_LOG4CPLUS_STRING_TO_TSTRING(STRING) log4cplus::helpers::towstring(STRING)
#define DCMTK_LOG4CPLUS_TSTRING_TO_STRING(STRING) log4cplus::helpers::tostring(STRING)

#else // UNICODE

#define DCMTK_LOG4CPLUS_C_STR_TO_TSTRING(STRING) OFString(STRING)
#define DCMTK_LOG4CPLUS_STRING_TO_TSTRING(STRING) STRING
#define DCMTK_LOG4CPLUS_TSTRING_TO_STRING(STRING) STRING

#endif // UNICODE

} // namespace log4cplus
} // end namespace dcmtk


#endif // DCMTK_LOG4CPLUS_TSTRING_HEADER_
