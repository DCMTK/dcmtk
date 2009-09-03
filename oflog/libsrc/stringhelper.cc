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

#include "dcmtk/oflog/helpers/stringhelper.h"
#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/loggingmacros.h"
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

#ifdef UNICODE
//#  include <cassert>
#  define INCLUDE_CASSERT
//#  include <vector>
#endif

#include "dcmtk/ofstd/ofstdinc.h"

using namespace log4cplus;

// We don't use this code since our stringstream doesn't have the necessary
// support (see loggingmacros.h)
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

#ifdef UNICODE
log4cplus::tostream&
operator <<(log4cplus::tostream& stream, const char* str)
{
    return (stream << log4cplus::helpers::towstring(str));
}


#ifdef LOG4CPLUS_WORKING_LOCALE

static
void
clear_mbstate (STD_NAMESPACE mbstate_t & mbs)
{
    // Initialize/clear mbstate_t type.
    // XXX: This is just a hack that works. The shape of mbstate_t varies
    // from single unsigned to char[128]. Without some sort of initialization
    // the codecvt::in/out methods randomly fail because the initial state is
    // random/invalid.
    ::memset (&mbs, 0, sizeof (STD_NAMESPACE mbstate_t));
}


static
void
towstring_internal (STD_NAMESPACE wstring & outstr, const char * src, size_t size,
    STD_NAMESPACE locale const & loc)
{
    typedef STD_NAMESPACE codecvt<wchar_t, char, STD_NAMESPACE mbstate_t> CodeCvt;
    const CodeCvt & cdcvt = STD_NAMESPACE use_facet<CodeCvt>(loc);
    STD_NAMESPACE mbstate_t state;
    clear_mbstate (state);

    char const * from_first = src;
    size_t const from_size = size;
    char const * const from_last = from_first + from_size;
    char const * from_next = from_first;

    STD_NAMESPACE vector<wchar_t> dest (from_size);

    wchar_t * to_first = &dest.front ();
    size_t to_size = dest.size ();
    wchar_t * to_last = to_first + to_size;
    wchar_t * to_next = to_first;

    CodeCvt::result result;
    size_t converted = 0;
    while (true)
    {
        result = cdcvt.in (
            state, from_first, from_last,
            from_next, to_first, to_last,
            to_next);
        // XXX: Even if only half of the input has been converted the
        // in() method returns CodeCvt::ok. I think it should return
        // CodeCvt::partial.
        if ((result == CodeCvt::partial || result == CodeCvt::ok)
            && from_next != from_last)
        {
            to_size = dest.size () * 2;
            dest.resize (to_size);
            converted = to_next - to_first;
            to_first = &dest.front ();
            to_last = to_first + to_size;
            to_next = to_first + converted;
            continue;
        }
        else if (result == CodeCvt::ok && from_next == from_last)
            break;
        else if (result == CodeCvt::error
            && to_next != to_last && from_next != from_last)
        {
            clear_mbstate (state);
            ++from_next;
            from_first = from_next;
            *to_next = L'?';
            ++to_next;
            to_first = to_next;
        }
        else
            break;
    }
    converted = to_next - &dest[0];

    outstr.assign (dest.begin (), dest.begin () + converted);
}


STD_NAMESPACE wstring
log4cplus::helpers::towstring(const STD_NAMESPACE string& src, STD_NAMESPACE locale const & loc)
{
    STD_NAMESPACE wstring ret;
    towstring_internal (ret, src.c_str (), src.size (), loc);
    return ret;
}


STD_NAMESPACE wstring
log4cplus::helpers::towstring(char const * src, STD_NAMESPACE locale const & loc)
{
    STD_NAMESPACE wstring ret;
    towstring_internal (ret, src, STD_NAMESPACE strlen (src), loc);
    return ret;
}


static
void
tostring_internal (STD_NAMESPACE string & outstr, const wchar_t * src, size_t size,
    STD_NAMESPACE locale const & loc)
{
    typedef STD_NAMESPACE codecvt<wchar_t, char, STD_NAMESPACE mbstate_t> CodeCvt;
    const CodeCvt & cdcvt = STD_NAMESPACE use_facet<CodeCvt>(loc);
    STD_NAMESPACE mbstate_t state;
    clear_mbstate (state);

    wchar_t const * from_first = src;
    size_t const from_size = size;
    wchar_t const * const from_last = from_first + from_size;
    wchar_t const * from_next = from_first;

    STD_NAMESPACE vector<char> dest (from_size);

    char * to_first = &dest.front ();
    size_t to_size = dest.size ();
    char * to_last = to_first + to_size;
    char * to_next = to_first;

    CodeCvt::result result;
    size_t converted = 0;
    while (from_next != from_last)
    {
        result = cdcvt.out (
            state, from_first, from_last,
            from_next, to_first, to_last,
            to_next);
        // XXX: Even if only half of the input has been converted the
        // in() method returns CodeCvt::ok with VC8. I think it should
        // return CodeCvt::partial.
        if ((result == CodeCvt::partial || result == CodeCvt::ok)
            && from_next != from_last)
        {
            to_size = dest.size () * 2;
            dest.resize (to_size);
            converted = to_next - to_first;
            to_first = &dest.front ();
            to_last = to_first + to_size;
            to_next = to_first + converted;
        }
        else if (result == CodeCvt::ok && from_next == from_last)
            break;
        else if (result == CodeCvt::error
            && to_next != to_last && from_next != from_last)
        {
            clear_mbstate (state);
            ++from_next;
            from_first = from_next;
            *to_next = '?';
            ++to_next;
            to_first = to_next;
        }
        else
            break;
    }
    converted = to_next - &dest[0];

    outstr.assign (dest.begin (), dest.begin () + converted);
}


STD_NAMESPACE string
log4cplus::helpers::tostring(const STD_NAMESPACE wstring& src, STD_NAMESPACE locale const & loc)
{
    STD_NAMESPACE string ret;
    tostring_internal (ret, src.c_str (), src.size (), loc);
    return ret;
}


STD_NAMESPACE string
log4cplus::helpers::tostring(wchar_t const * src, STD_NAMESPACE locale const & loc)
{
    STD_NAMESPACE string ret;
    tostring_internal (ret, src, STD_NAMESPACE wcslen (src), loc);
    return ret;
}


#else // LOG4CPLUS_WORKING_LOCALE


static
void
tostring_internal (STD_NAMESPACE string & ret, wchar_t const * src, size_t size)
{
    ret.resize(size);
    for (size_t i = 0; i < size; ++i)
    {
        ret[i] = static_cast<unsigned> (static_cast<int> (src[i])) < 256
            ? static_cast<char>(src[i]) : ' ';
    }
}


STD_NAMESPACE string
log4cplus::helpers::tostring(const STD_NAMESPACE wstring& src)
{
    STD_NAMESPACE string ret;
    tostring_internal (ret, src.c_str (), src.size ());
    return ret;
}


STD_NAMESPACE string
log4cplus::helpers::tostring(wchar_t const * src)
{
    STD_NAMESPACE string ret;
    tostring_internal (ret, src, STD_NAMESPACE wcslen (src));
    return ret;
}


static
void
towstring_internal (STD_NAMESPACE wstring & ret, char const * src, size_t size)
{
    ret.resize(size);
    for (size_t i = 0; i < size; ++i)
    {
        ret[i] = static_cast<wchar_t>
            (static_cast<unsigned char> (src[i]));
    }
}


STD_NAMESPACE wstring
log4cplus::helpers::towstring(const STD_NAMESPACE string& src)
{
    STD_NAMESPACE wstring ret;
    towstring_internal (ret, src.c_str (), src.size ());
    return ret;
}


STD_NAMESPACE wstring
log4cplus::helpers::towstring(char const * src)
{
    STD_NAMESPACE wstring ret;
    towstring_internal (ret, src, STD_NAMESPACE strlen (src));
    return ret;
}

#endif // LOG4CPLUS_WORKING_LOCALE

#endif // UNICODE


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
