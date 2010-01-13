// Module:  Log4CPLUS
// File:    stringhelper.cxx
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

#include "dcmtk/oflog/helpers/strhelp.h"
#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/logmacro.h"
//#include <iterator>
#include "dcmtk/ofstd/ofstd.h"

//#include <cwchar>

//#include <algorithm>
//#include <cstring>
#define INCLUDE_CSTRING
//#include <cwctype>
#define INCLUDE_CWCTYPE
//#include <cctype>
#define INCLUDE_CCTYPE

#include "dcmtk/ofstd/ofstdinc.h"

using namespace log4cplus;

#if defined (LOG4CPLUS_SINGLE_THREADED)

namespace log4cplus
{

tostringstream _macros_oss;

namespace
{

static tostringstream const _macros_oss_defaults;
//static tstring const _empty_str;

} // namespace

void _clear_tostringstream (tostringstream & os)
{
    os.clear ();
    os.str ("");
    os.setf (_macros_oss_defaults.flags ());
    os.fill (_macros_oss_defaults.fill ());
    os.precision (_macros_oss_defaults.precision ());
    os.width (_macros_oss_defaults.width ());
#if defined (LOG4CPLUS_WORKING_LOCALE)
    STD_NAMESPACE locale glocale = STD_NAMESPACE locale ();
    if (os.getloc () != glocale)
        os.imbue (glocale);
#endif // defined (LOG4CPLUS_WORKING_LOCALE)
}

} // namespace log4cplus

#endif


//////////////////////////////////////////////////////////////////////////////
// Global Methods
//////////////////////////////////////////////////////////////////////////////


log4cplus::tstring
log4cplus::helpers::toUpper(const log4cplus::tstring& s)
{
    OFString tmp;
    return OFStandard::toUpper(tmp, s);
}


log4cplus::tstring
log4cplus::helpers::toLower(const log4cplus::tstring& s)
{
    OFString tmp;
    return OFStandard::toLower(tmp, s);
}
