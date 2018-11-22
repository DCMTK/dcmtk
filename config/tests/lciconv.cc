/*
 *  Copyright (C) 2018, OFFIS e.V.
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
 *  Module:  config
 *
 *  Author:  Jan Schlamelcher
 *
 *  Purpose: Test whether an STDLIBC iconv defines a default encoding.
 */

#include <iconv.h>

int main()
{
    if ((iconv_t)-1 == iconv_open( "ASCII", "" ))
        return 1;
    if ((iconv_t)-1 == iconv_open( "", "ASCII" ))
        return 1;
    return 0;
}
