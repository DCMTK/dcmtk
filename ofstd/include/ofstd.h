/*
 *
 *  Copyright (C) 2000, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  ofstd
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Class for various helper functions
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-03-02 12:42:57 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofstd.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __OFSTD_H
#define __OFSTD_H

#include "osconfig.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** A class for various helper functions.
 *  This class is used to comprise a number of "global" helper functions.
 */
class OFStandard
{

 public:
 
    /** copies specified (maximum) number of chars from source to destination.
     *  If the source string is longer than the specified length, only the
     *  the specified number of chars is copied, otherwise the complete
     *  source string is copied to the destination buffer.
     *  A closing zero byte is always added to the destination string.
     *  The size of the destination buffer is not checked!
     *
     ** @param  dest  destination string (should be len+1 large)
     *  @param  src   source string
     *  @param  len   number of characters to be copied (without zero byte)
     */
    static char *strlcpy(char *dest,
                         const char *src,
                         size_t len)
    {
        if (strlen(src) > len)
        {
            if (len > 0)
                strncpy(dest, src, len);
            *(dest + len) = 0;
        } else
            strcpy(dest, src);
        return dest;
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: ofstd.h,v $
 * Revision 1.1  2000-03-02 12:42:57  joergr
 * Added new class comprising all general purpose helper functions (first
 * entry: strlcpy - a mixture of strcpy and strncpy).
 *
 *
 *
 */
