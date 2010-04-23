// Module:  Log4CPLUS
// File:    tstring.h
// Created: 4/2003
// Author:  Tad E. Smith
//
//
// Copyright 2003-2009 Tad E. Smith
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

#define LOG4CPLUS_C_STR_TO_TSTRING(STRING) OFString(STRING)
#define LOG4CPLUS_STRING_TO_TSTRING(STRING) STRING
#define LOG4CPLUS_TSTRING_TO_STRING(STRING) STRING

#endif // LOG4CPLUS_TSTRING_HEADER_
