// Module:  Log4CPLUS
// File:    stringhelper.h
// Created: 3/2003
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

#ifndef LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_
#define LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_

#include "dcmtk/oflog/config.h"
#include "dcmtk/oflog/tstring.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofstdinc.h"

//#include <algorithm>
//#include <limits>
//#include <iterator>


namespace log4cplus {
    namespace helpers {

        /**
         * Returns <code>s</code> in upper case.
         */
        LOG4CPLUS_EXPORT log4cplus::tstring toUpper(const log4cplus::tstring& s);


        /**
         * Returns <code>s</code> in lower case.
         */
        LOG4CPLUS_EXPORT log4cplus::tstring toLower(const log4cplus::tstring& s);


        /**
         * Tokenize <code>s</code> using <code>c</code> as the delimiter and
         * put the resulting tokens in <code>_result</code>.  If
         * <code>collapseTokens</code> is false, multiple adjacent delimiters
         * will result in zero length tokens.
         *
         * <b>Example:</b>
         * <pre>
         *   string s = // Set string with '.' as delimiters
         *   list<log4cplus::tstring> tokens;
         *   tokenize(s, '.', back_insert_iterator<list<string> >(tokens));
         * </pre>
         */
        template <class StringType>
        void tokenize(const StringType& s, typename StringType::value_type c,
                      OFList<tstring>& result , bool collapseTokens = true)
        {
            size_t const slen = s.length();
            size_t first = 0;
            size_t i = 0;
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


        template<class intType>
        inline
        tstring
        convertIntegerToString (intType value)
        {
            if (value == 0)
                return LOG4CPLUS_TEXT("0");
            bool const negative = value < 0;

            static const size_t buffer_size
                //= STD_NAMESPACE numeric_limits<intType>::digits10 + 2;
                = 30; // More than enough space, even for 64 bit integers
            tchar buffer[buffer_size];
            tchar * it = &buffer[buffer_size];
            tchar const * const buf_end = it;

            // The sign of the result of the modulo operator is implementation
            // defined. That's why we work with positive counterpart instead.
            // Also, in twos complement arithmetic the smallest negative number
            // does not have positive counterpart; the range is asymetric.
            // That's why we handle the case of value == min() specially here.
            if (negative)
            {
                // The modulo operator on
                //if (value == (STD_NAMESPACE numeric_limits<intType>::min) ())

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
                const intType minSigned = -1 - maxSigned;

                // This is the minimum value that intType can represent;
                const intType minVal = isUnsigned ? 0 : minSigned;

                if (value == minVal)
                {
                    intType const r = value / 10;
                    intType const a = (-r) * 10;
                    intType const mod = -(a + value);
                    value = -r;
                    *(it - 1) = LOG4CPLUS_TEXT('0') + static_cast<tchar>(mod);
                    --it;
                }
                else
                    value = -value;
            }

            for (; value != 0; --it)
            {
                intType mod = value % 10;
                value = value / 10;
                *(it - 1) = LOG4CPLUS_TEXT('0') + static_cast<tchar>(mod);
            }

            if (negative)
            {
                --it;
                *it = LOG4CPLUS_TEXT('-');
            }

            // return the string from it to buf_end
            return tstring (static_cast<tchar const *>(it), static_cast<size_t>(buf_end - it));
        }

    } // namespace helpers

} //namespace log4cplus


#endif // LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_
