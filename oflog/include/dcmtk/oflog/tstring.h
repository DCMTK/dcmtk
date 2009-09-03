// Module:  Log4CPLUS
// File:    tstring.h
// Created: 4/2003
// Author:  Tad E. Smith
//
//
// Copyright (C) Tad E. Smith  All rights reserved.
//
// This software is published under the terms of the Apache Software
// License version 1.1, a copy of which has been included with this
// distribution in the LICENSE.APL file.
//

/** @file */

#ifndef LOG4CPLUS_TSTRING_HEADER_
#define LOG4CPLUS_TSTRING_HEADER_

#include "dcmtk/oflog/config.h"
#include "dcmtk/ofstd/ofstring.h"

#ifdef UNICODE
#  ifdef LOG4CPLUS_WORKING_LOCALE
#    include <locale>
#  endif // LOG4CPLUS_WORKING_LOCALE
#  define LOG4CPLUS_TEXT2(STRING) L##STRING
#else
#  define LOG4CPLUS_TEXT2(STRING) STRING
#endif // UNICODE
#define LOG4CPLUS_TEXT(STRING) LOG4CPLUS_TEXT2(STRING)


#ifdef UNICODE
namespace log4cplus {
    typedef wchar_t tchar;
    typedef STD_NAMESPACE wstring tstring;

    namespace helpers {
#ifdef LOG4CPLUS_WORKING_LOCALE
        LOG4CPLUS_EXPORT STD_NAMESPACE string tostring(const STD_NAMESPACE wstring&,
            STD_NAMESPACE locale const & = STD_NAMESPACE locale ());

        LOG4CPLUS_EXPORT STD_NAMESPACE string tostring (wchar_t const *,
            STD_NAMESPACE locale const & = STD_NAMESPACE locale ());

        LOG4CPLUS_EXPORT STD_NAMESPACE wstring towstring(const STD_NAMESPACE string&,
            STD_NAMESPACE locale const & = STD_NAMESPACE locale ());

        LOG4CPLUS_EXPORT STD_NAMESPACE wstring towstring(char const *,
            STD_NAMESPACE locale const & = STD_NAMESPACE locale ());

#else // LOG4CPLUS_WORKING_LOCALE
        LOG4CPLUS_EXPORT STD_NAMESPACE string tostring(const STD_NAMESPACE wstring&);
        LOG4CPLUS_EXPORT STD_NAMESPACE string tostring(wchar_t const *);
        LOG4CPLUS_EXPORT STD_NAMESPACE wstring towstring(const STD_NAMESPACE string&);
        LOG4CPLUS_EXPORT STD_NAMESPACE wstring towstring(char const *);
#endif // LOG4CPLUS_WORKING_LOCALE
    }

}

#define LOG4CPLUS_C_STR_TO_TSTRING(STRING) log4cplus::helpers::towstring(STRING)
#define LOG4CPLUS_STRING_TO_TSTRING(STRING) log4cplus::helpers::towstring(STRING)
#define LOG4CPLUS_TSTRING_TO_STRING(STRING) log4cplus::helpers::tostring(STRING)

#else // UNICODE
namespace log4cplus {
    typedef char tchar;
    typedef OFString tstring;
}

#define LOG4CPLUS_C_STR_TO_TSTRING(STRING) log4cplus::tstring(STRING)
#define LOG4CPLUS_STRING_TO_TSTRING(STRING) STRING
#define LOG4CPLUS_TSTRING_TO_STRING(STRING) STRING

#endif // UNICODE

#endif // LOG4CPLUS_TSTRING_HEADER_
