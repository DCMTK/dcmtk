// -*- C++ -*-
// Module:  Log4CPLUS
// File:    stringhelper.h
// Created: 3/2003
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

#ifndef DCMTK_LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_
#define DCMTK_LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include "dcmtk/oflog/tstring.h"
#include "dcmtk/ofstd/ofvector.h"

//#include <algorithm>
//#include <limits>


namespace dcmtk {
namespace log4cplus {
    namespace helpers {

        /**
         * Returns <code>s</code> in upper case.
         */
        DCMTK_LOG4CPLUS_EXPORT log4cplus::tstring toUpper(const log4cplus::tstring& s);


        /**
         * Returns <code>s</code> in lower case.
         */
        DCMTK_LOG4CPLUS_EXPORT log4cplus::tstring toLower(const log4cplus::tstring& s);


        /**
         * Tokenize <code>s</code> using <code>c</code> as the delimiter and
         * put the resulting tokens in <code>_result</code>.  If
         * <code>collapseTokens</code> is false, multiple adjacent delimiters
         * will result in zero length tokens.
         *
         * <b>Example:</b>
         * <pre>
         *   string s = // Set string with '.' as delimiters
         *   list<tstring> tokens;
         *   tokenize(s, '.', back_insert_iterator<list<string> >(tokens));
         * </pre>
         */
        template <class StringType>
        inline
        void
        tokenize(const StringType& s, typename StringType::value_type c,
            OFVector<StringType>& result, bool collapseTokens = true)
        {
            typedef typename StringType::size_type size_type;
            size_type const slen = s.length();
            size_type first = 0;
            size_type i = 0;
            for (i=0; i < slen; ++i)
            {
                if (s[i] == c)
                {
                    result.push_back(StringType (s, first, i - first));
                    if (collapseTokens)
                        while (i+1 < slen && s[i+1] == c)
                            ++i;
                    first = i + 1;
                }
            }
            if (first != i)
                result.push_back(StringType (s, first, i - first));
        }


        template <typename intType>
        struct ConvertIntegerToStringHelper
        {
            static inline
            void
            step1 (tchar * & it, intType & value)
            {
                // This code assumes two-compliments'. The problem here is that
                // integer overflow of an signed type is undefined behavior :(
                // This code is based upon http://www.fefe.de/intof.html

                // True if intType is unsigned
                const OFBool isUnsigned = (OFstatic_cast(intType, -1) < 0) ? OFFalse : OFTrue;

                // If intType is a signed type, halfMaxSigned is intType_MAX / 2
                const intType halfMaxSigned = OFstatic_cast(intType, 1) << (sizeof(intType) * 8 - 2);

                // if intType is a signed type, MaxSigned is its intType_MAX
                const intType maxSigned = halfMaxSigned - 1 + halfMaxSigned;

                // if intType is a signed type, MinSigned is its intType_MIN
                const intType minSigned = OFstatic_cast(intType, -1) - maxSigned;

                // This is the minimum value that intType can represent;
                const intType minVal = isUnsigned ? 0 : minSigned;

                //if (value == (STD_NAMESPACE numeric_limits<intType>::min) ())
                if (value == minVal)
                {
                    intType const r = value / 10;
                    intType const a = (0-r) * 10;
                    intType const mod = 0-(a + value);
                    value = 0-r;

                    *(it - 1) = OFstatic_cast(tchar, DCMTK_LOG4CPLUS_TEXT('0') + mod);
                    --it;
                }
                else
                    value = 0-value;
            }

            static inline
            bool
            is_negative (intType val)
            {
                return val < 0;
            }
        };


        template<class intType>
        inline
        void
        convertIntegerToString (tstring & str, intType value)
        {
            const size_t buffer_size = 30; // More than enough space, even for 64 bit integers
                // = intTypeLimits::digits10 + 2;
            tchar buffer[buffer_size];
            tchar * it = &buffer[buffer_size];
            tchar const * const buf_end = it;

            if (value == 0)
            {
                --it;
                *it = DCMTK_LOG4CPLUS_TEXT('0');
            }

            // We can't use (value < 0) because that could cause a compiler
            // warning for unsigned types.
            const OFBool isUnsigned = (OFstatic_cast(intType, -1) < 0) ? OFFalse : OFTrue;
            bool const negative = !(value > 0 || isUnsigned || value == 0);
            if (negative)
                ConvertIntegerToStringHelper<intType>::step1(it, value);

            for (; value != 0; --it)
            {
                intType mod = value % 10;
                value = value / 10;
                *(it - 1) = OFstatic_cast(tchar, DCMTK_LOG4CPLUS_TEXT('0') + mod);
            }

            if (negative)
            {
                --it;
                *it = DCMTK_LOG4CPLUS_TEXT('-');
            }

            str.assign (OFstatic_cast(tchar const *, it), buf_end);
        }


        template<class intType>
        inline
        tstring
        convertIntegerToString (intType value)
        {
            tstring result;
            convertIntegerToString (result, value);
            return result;
        }

    } // namespace helpers

} // namespace log4cplus
} // end namespace dcmtk

#endif // DCMTK_LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_
