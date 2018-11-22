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
 *  Purpose: Analyze behavior of an iconv implementation.
 */

#include <iconv.h>
#include <stdio.h>

int main()
{
    char input[2] = { '\366', '\0' };
    char output[8];
    iconv_t i = iconv_open( "ASCII", "ISO-8859-1" );
    if( (iconv_t)-1 != i )
    {
        iconv( i, 0, 0, 0, 0 );
#ifdef LIBICONV_SECOND_ARGUMENT_CONST
        const
#endif
        char* in = input;
        char* out = output;
        size_t ins = 1;
        size_t outs = 8;
        const size_t result = iconv( i, &in, &ins, &out, &outs );
        iconv_close( i );
        if( ~(size_t)0 == result )
        {
            printf( "AbortTranscodingOnIllegalSequence" );
            return 0;
        }
        if( 8 == outs ) // output buffer not touched
        {
            printf( "DiscardIllegalSequences" );
            return 0;
        }
    }
    return 1;
}
