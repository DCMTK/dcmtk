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

#include "dcmtk/ofstd/ofstrutl.h"
#include "dcmtk/config/osconfig.h" /* include OS specific configuration first */

void OFStringUtil::replace_all(OFString& src, const OFString& pattern, const OFString& rplc)
{
    if (!pattern.empty())
    {
        size_t pos = src.find(pattern, 0);
        while (pos != OFString_npos)
        {
            src.replace(pos, pattern.length(), rplc);
            pos += rplc.length();
            pos = src.find(pattern, pos);
        }
    }
}

OFString
OFStringUtil::replace_all(const OFString& src, const OFString& pattern, const OFString& rplc)
{
    OFString result(src);
    OFStringUtil::replace_all(result, pattern, rplc);
    return result;
}
