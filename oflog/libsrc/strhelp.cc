// Module:  Log4CPLUS
// File:    stringhelper.cxx
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

#include "dcmtk/oflog/helpers/strhelp.h"
#include "dcmtk/oflog/streams.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/oflog/internal/internal.h"

#include <iterator>
#include <algorithm>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <cctype>
#include <cassert>


namespace dcmtk
{
namespace log4cplus
{

namespace internal
{

tstring const empty_str;

} // namespace internal

} // namespace log4cplus
} // end namespace dcmtk


//////////////////////////////////////////////////////////////////////////////
// Global Methods
//////////////////////////////////////////////////////////////////////////////

#if defined (DCMTK_OFLOG_UNICODE) && defined (DCMTK_LOG4CPLUS_ENABLE_GLOBAL_C_STRING_STREAM_INSERTER)

log4cplus::tostream& 
operator <<(log4cplus::tostream& stream, const char* str)
{
    return (stream << log4cplus::helpers::towstring(str));
}

#endif


namespace dcmtk
{
namespace log4cplus
{

namespace helpers
{

void clear_mbstate (mbstate_t & mbs);

void
clear_mbstate (mbstate_t & mbs)
{
    // Initialize/clear mbstate_t type.
    // XXX: This is just a hack that works. The shape of mbstate_t varies
    // from single unsigned to char[128]. Without some sort of initialization
    // the codecvt::in/out methods randomly fail because the initial state is
    // random/invalid.
    memset (&mbs, 0, sizeof (mbstate_t));
}


#if defined (DCMTK_LOG4CPLUS_POOR_MANS_CHCONV)

static
void
tostring_internal (OFString & ret, wchar_t const * src, size_t size)
{
    ret.resize(size);
    for (size_t i = 0; i < size; ++i)
    {
        STD_NAMESPACE char_traits<wchar_t>::int_type src_int
            = STD_NAMESPACE char_traits<wchar_t>::to_int_type (src[i]);
        ret[i] = src_int <= 127
            ? STD_NAMESPACE char_traits<char>::to_char_type (src_int)
            : '?';
    }
}


OFString
tostring(const STD_NAMESPACE wstring& src)
{
    OFString ret;
    tostring_internal (ret, src.c_str (), src.size ());
    return ret;
}


OFString
tostring(wchar_t const * src)
{
    assert (src);
    OFString ret;
    tostring_internal (ret, src, wcslen (src));
    return ret;
}


static
void
towstring_internal (STD_NAMESPACE wstring & ret, char const * src, size_t size)
{
    ret.resize(size);
    for (size_t i = 0; i < size; ++i)
    {
        STD_NAMESPACE char_traits<char>::int_type src_int
            = STD_NAMESPACE char_traits<char>::to_int_type (src[i]);
        ret[i] = src_int <= 127
            ? STD_NAMESPACE char_traits<wchar_t>::to_char_type (src_int)
            : L'?';
    }
}


STD_NAMESPACE wstring towstring(const STD_NAMESPACE string& src);

STD_NAMESPACE wstring 
towstring(const STD_NAMESPACE string& src)
{
    STD_NAMESPACE wstring ret;
    towstring_internal (ret, src.c_str (), src.size ());
    return ret;
}


STD_NAMESPACE wstring 
towstring(char const * src)
{
    assert (src);
    STD_NAMESPACE wstring ret;
    towstring_internal (ret, src, strlen (src));
    return ret;
}

#endif // DCMTK_LOG4CPLUS_POOR_MANS_CHCONV


namespace
{


struct toupper_func
{
    tchar
    operator () (tchar ch) const
    {
        return STD_NAMESPACE char_traits<tchar>::to_char_type (
#ifdef DCMTK_OFLOG_UNICODE
            STD_NAMESPACE towupper
#else
            toupper
#endif
            (STD_NAMESPACE char_traits<tchar>::to_int_type (ch)));
    }
};


struct tolower_func
{
    tchar
    operator () (tchar ch) const
    {
        return STD_NAMESPACE char_traits<tchar>::to_char_type (
#ifdef DCMTK_OFLOG_UNICODE
            STD_NAMESPACE towlower
#else
            tolower
#endif
            (STD_NAMESPACE char_traits<tchar>::to_int_type (ch)));
    }
};


} // namespace


tstring
toUpper(const tstring& s)
{
    OFString tmp;
    return OFStandard::toUpper(tmp, s);
}


tstring
toLower(const tstring& s)
{
    OFString tmp;
    return OFStandard::toLower(tmp, s);
}


} // namespace helpers

} // namespace log4cplus
} // end namespace dcmtk
