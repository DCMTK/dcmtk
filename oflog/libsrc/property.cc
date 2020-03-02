// Module:  Log4CPLUS
// File:    property.cxx
// Created: 2/2002
// Author:  Tad E. Smith
//
//
// Copyright 2002-2010 Tad E. Smith
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

#include <cstring>
#if defined (DCMTK_OFLOG_UNICODE)
#  include <cwctype>
#else
#  include <cctype>
#endif
#if defined (DCMTK_LOG4CPLUS_HAVE_CODECVT_UTF8_FACET) \
    || defined (DCMTK_LOG4CPLUS_HAVE_CODECVT_UTF16_FACET) \
    || defined (DCMTK_LOG4CPLUS_HAVE_CODECVT_UTF32_FACET)
#  include <codecvt>
#endif
#include <locale>
#include <fstream>
#include <sstream>
#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/fstreams.h"
#include "dcmtk/oflog/helpers/strhelp.h"
#include "dcmtk/oflog/helpers/property.h"
#include "dcmtk/oflog/internal/internal.h"
#include "dcmtk/oflog/internal/env.h"
#include "dcmtk/oflog/helpers/loglog.h"


namespace dcmtk {
namespace log4cplus { namespace helpers {


const tchar Properties::PROPERTIES_COMMENT_CHAR = DCMTK_LOG4CPLUS_TEXT('#');


namespace
{


static
int
is_space (tchar ch)
{
#if defined (DCMTK_OFLOG_UNICODE)
    return STD_NAMESPACE iswspace (ch);
#else
    return isspace (OFstatic_cast(unsigned char, ch));
#endif
}


static
void
trim_leading_ws (tstring & str)
{
    tstring::iterator it = str.begin ();
    for (; it != str.end (); ++it)
    {
        if (! is_space (*it))
            break;
    }
    str.erase (0, it - str.begin ());
}


static
void
trim_trailing_ws (tstring & str)
{
    size_t i;
    for (i = str.length(); i > 0; i--)
    {
        if (! is_space (str[i-1]))
            break;
    }
    str.erase (i, str.length() - i);
}


static
void
trim_ws (tstring & str)
{
    trim_trailing_ws (str);
    trim_leading_ws (str);
}


} // namespace



///////////////////////////////////////////////////////////////////////////////
// Properties ctors and dtor
///////////////////////////////////////////////////////////////////////////////

Properties::Properties() : data()
{
}



Properties::Properties(tistream& input) : data()
{
    init(input);
}



Properties::Properties(const tstring& inputFile, unsigned flags) : data()
{
    if (inputFile.empty ())
        return;

    tifstream file;

    switch (flags & fEncodingMask)
    {
#if defined (DCMTK_LOG4CPLUS_HAVE_CODECVT_UTF8_FACET) && defined (DCMTK_OFLOG_UNICODE)
    case fUTF8:
        file.imbue (
            STD_NAMESPACE locale (file.getloc (),
                new STD_NAMESPACE codecvt_utf8<tchar, 0x10FFFF,
                    OFstatic_cast(STD_NAMESPACE codecvt_mode, STD_NAMESPACE consume_header | STD_NAMESPACE little_endian)>));
        break;
#endif

#if defined (DCMTK_LOG4CPLUS_HAVE_CODECVT_UTF16_FACET) && defined (DCMTK_OFLOG_UNICODE)
    case fUTF16:
        file.imbue (
            STD_NAMESPACE locale (file.getloc (),
                new STD_NAMESPACE codecvt_utf16<tchar, 0x10FFFF,
                    OFstatic_cast(STD_NAMESPACE codecvt_mode, STD_NAMESPACE consume_header | STD_NAMESPACE little_endian)>));
        break;

#elif defined (DCMTK_OFLOG_UNICODE) && defined (WIN32)
    case fUTF16:
        file.imbue (
            STD_NAMESPACE locale (file.getloc (),
                new STD_NAMESPACE codecvt<wchar_t, wchar_t, STD_NAMESPACE mbstate_t>));
    break;

#endif

#if defined (DCMTK_LOG4CPLUS_HAVE_CODECVT_UTF32_FACET) && defined (DCMTK_OFLOG_UNICODE)
    case fUTF32:
        file.imbue (
            STD_NAMESPACE locale (file.getloc (),
                new STD_NAMESPACE codecvt_utf32<tchar, 0x10FFFF,
                    OFstatic_cast(STD_NAMESPACE codecvt_mode, STD_NAMESPACE consume_header | STD_NAMESPACE little_endian)>));
        break;
#endif

    case fUnspecEncoding:;
    default:
        // Do nothing.
        ;
    }

    file.open(DCMTK_LOG4CPLUS_FSTREAM_PREFERED_FILE_NAME(inputFile).c_str(),
        STD_NAMESPACE ios::binary);
    if (! file.good ())
        helpers::getLogLog ().error (DCMTK_LOG4CPLUS_TEXT ("could not open file ")
            + inputFile);

    init(file);
}



void 
Properties::init(tistream& input) 
{
    if (! input)
        return;

    STD_NAMESPACE string buffer_;
    while (STD_NAMESPACE getline (input, buffer_))
    {
        tstring buffer(buffer_.c_str(), buffer_.length());
        trim_leading_ws (buffer);

        tstring::size_type const buffLen = buffer.size ();
        if (buffLen == 0 || buffer[0] == PROPERTIES_COMMENT_CHAR)
            continue;
        
        // Check if we have a trailing \r because we are 
        // reading a properties file produced on Windows.
        if (buffer[buffLen-1] == DCMTK_LOG4CPLUS_TEXT('\r'))
            // Remove trailing 'Windows' \r.
            buffer.resize (buffLen - 1);

        tstring::size_type const idx = buffer.find('=');
        if (idx != OFString_npos)
        {
            tstring key = buffer.substr(0, idx);
            tstring value = buffer.substr(idx + 1);
            trim_trailing_ws (key);
            trim_ws (value);
            setProperty(key, value);
        }
    }
}



Properties::~Properties() 
{
}



///////////////////////////////////////////////////////////////////////////////
// helpers::Properties public methods
///////////////////////////////////////////////////////////////////////////////


bool
Properties::exists(const log4cplus::tstring& key) const
{
    return data.find(key) != data.end();
}


bool
Properties::exists(tchar const * key) const
{
    return data.find(key) != data.end();
}


tstring const &
Properties::getProperty(const tstring& key) const 
{
    return get_property_worker (key);
}


log4cplus::tstring const &
Properties::getProperty(tchar const * key) const
{
    return get_property_worker (key);
}


tstring
Properties::getProperty(const tstring& key, const tstring& defaultVal) const
{
    StringMap::const_iterator it (data.find (key));
    if (it == data.end ())
        return defaultVal;
    else
        return it->second;
}


OFVector<tstring>
Properties::propertyNames() const 
{
    OFVector<tstring> tmp;
    for (StringMap::const_iterator it=data.begin(); it!=data.end(); ++it)
        tmp.push_back(it->first);

    return tmp;
}



void
Properties::setProperty(const log4cplus::tstring& key,
    const log4cplus::tstring& value)
{
    data[key] = value;
}


bool
Properties::removeProperty(const log4cplus::tstring& key)
{
    return data.erase(key) > 0;
}


Properties 
Properties::getPropertySubset(const log4cplus::tstring& prefix) const
{
    Properties ret;
    size_t const prefix_len = prefix.size ();
    OFVector<tstring> keys = propertyNames();
    for (OFVector<tstring>::iterator it=keys.begin(); it!=keys.end(); ++it)
    {
        int result = it->compare (0, prefix_len, prefix);
        if (result == 0)
            ret.setProperty (it->substr (prefix_len), getProperty(*it));
    }

    return ret;
}


bool
Properties::getInt (int & val, log4cplus::tstring const & key) const
{
    return get_type_val_worker (val, key);
}


bool
Properties::getUInt (unsigned & val, log4cplus::tstring const & key) const
{
    return get_type_val_worker (val, key);
}


bool
Properties::getLong (long & val, log4cplus::tstring const & key) const
{
    return get_type_val_worker (val, key);
}


bool
Properties::getULong (unsigned long & val, log4cplus::tstring const & key) const
{
    return get_type_val_worker (val, key);
}


bool
Properties::getBool (bool & val, log4cplus::tstring const & key) const
{
    if (! exists (key))
        return false;

    log4cplus::tstring const & prop_val = getProperty (key);
    return internal::parse_bool (val, prop_val);
}


template <typename StringType>
log4cplus::tstring const &
Properties::get_property_worker (StringType const & key) const
{
    StringMap::const_iterator it (data.find (key));
    if (it == data.end ())
        return log4cplus::internal::empty_str;
    else
        return it->second;
}


template <typename ValType>
bool
Properties::get_type_val_worker (ValType & val, log4cplus::tstring const & key)
    const
{
    if (! exists (key))
        return false;

    log4cplus::tstring const & prop_val = getProperty (key);
    log4cplus::tistringstream iss (STD_NAMESPACE string(prop_val.c_str(), prop_val.length()));
    ValType tmp_val;
    tchar ch;

    iss >> tmp_val;
    if (! iss)
        return false;
    iss >> ch;
    if (iss)
        return false;

    val = tmp_val;
    return true;
}


} } // namespace log4cplus { namespace helpers {
} // end namespace dcmtk
