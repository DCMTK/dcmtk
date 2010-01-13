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

#define LOG4CPLUS_TEXT2(STRING) STRING
#define LOG4CPLUS_TEXT(STRING) LOG4CPLUS_TEXT2(STRING)

namespace log4cplus {
    typedef char tchar;
    typedef OFString tstring;
}

#define LOG4CPLUS_C_STR_TO_TSTRING(STRING) log4cplus::tstring(STRING)
#define LOG4CPLUS_STRING_TO_TSTRING(STRING) STRING
#define LOG4CPLUS_TSTRING_TO_STRING(STRING) STRING

#endif // LOG4CPLUS_TSTRING_HEADER_
