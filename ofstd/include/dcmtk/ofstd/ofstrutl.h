/*
 *
 *  Copyright (C) 2019, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  ofstd
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class with OFString utility methods
 *
 */

#ifndef OFSTRUTL_H
#define OFSTRUTL_H

#include "dcmtk/config/osconfig.h" /* include OS specific configuration first */
#include "dcmtk/ofstd/ofstring.h"

class DCMTK_OFSTD_EXPORT OFStringUtil
{
public:
    /** replace all occurrences of a pattern in the given string (in-place) with
     *  another string.
     *  @param src string to perform replacement on and that will contain the result
     *  @param pattern the pattern to search for in src string
     *  @param rplc the replacement string that is inserted whenever pattern is found
     */
    static void replace_all(OFString& src, const OFString& pattern, const OFString& rplc);

    /** replace all occurrences of a pattern in the given string with
     *  another string and return a copy of result.
     *  @param src source string to perform replacement on
     *  @param pattern the pattern to search for in src string
     *  @param rplc replacement string that is inserted whenever pattern is found
     *  @return resulting string with all occurrences of pattern being replaced with rplc string
     */
    static OFString replace_all(const OFString& src, const OFString& pattern, const OFString& rplc);
};

#endif /* OFSTRUTL_H*/
