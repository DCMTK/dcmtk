/*
 *
 *  Copyright (C) 2000-2001, OFFIS
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
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-12-04 16:57:15 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofstd.h,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __OFSTD_H
#define __OFSTD_H

#include "osconfig.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>  /* for size_t */
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
END_EXTERN_C

/*---------------------*
 *  class declaration  *
 *---------------------*/

/** A class for various helper functions.
 *  This class is used to comprise a number of "global" helper functions.
 */
class OFStandard
{

public:

    /** This function copies up to size - 1 characters from the NUL-
     *  terminated string src to dst, NUL-terminating the result. It is 
     *  designed to be a safer, more consistent, and less error-prone 
     *  replacement for strncpy(3). strlcpy takes the full size of the 
     *  buffer (not just the length) and guarantees to NUL-terminate the 
     *  result (as long as size is larger than 0). Note that you should 
     *  include a byte for the NUL in size. Also note that strlcpy only 
     *  operates on true C strings, i. e. src must be NUL-terminated.
     *  @param dst destination buffer of size siz, must not be NULL
     *  @param src source string, must not be NULL
     *  @param siz size of destination buffer
     *  @return the total length of the string the function tried to 
     *  create, i.e. strlen(src).  While this may seem somewhat 
     *  confusing it was done to make truncation detection simple.
     */
    static inline size_t strlcpy(char *dst, const char *src, size_t siz)
    {
#ifdef HAVE_STRLCPY
      return ::strlcpy(dst, src, siz);
#else
      return my_strlcpy(dst, src, siz);
#endif
    }

    /** This function appends the NUL-terminated string src to the end of 
     *  dst. It will append at most size - strlen(dst) - 1 bytes, NUL-
     *  terminating the result. It is designed to be a safer, more 
     *  consistent, and less error-prone replacement for strncat(3). 
     *  strlcat takes the full size of the buffer (not just the length) and 
     *  guarantees to NUL-terminate the result (as long as size is larger 
     *  than 0). Note that you should include a byte for the NUL in size. 
     *  Also note that strlcat only operates on true C strings, i. e. dst 
     *  and src must be NUL-terminated.
     *  @param dst destination buffer of size siz, must not be NULL
     *  @param src source string, must not be NULL
     *  @param siz size of destination buffer
     *  @return the total length of the string the function tried to 
     *  create, i.e. the initial length of dst plus the length of src.  
     *  While this may seem somewhat confusing it was done to make 
     *  truncation detection simple.
     */
    static inline size_t strlcat(char *dst, const char *src, size_t siz)
    {   
#ifdef HAVE_STRLCAT
      return ::strlcat(dst, src, siz);
#else   
      return my_strlcat(dst, src, siz);
#endif
    }

  private:

    /** private implementation of strlcpy. Called when strlcpy
     *  is not available in the standard library.
     *  @param dst destination buffer of size siz, must not be NULL
     *  @param src source string, must not be NULL
     *  @param siz size of destination buffer
     *  @return the total length of the string the function tried to 
     *  create, i.e. strlen(src).
     */
    static size_t my_strlcpy(char *dst, const char *src, size_t siz);

    /** private implementation of strlcat. Called when strlcat
     *  is not available in the standard library.
     *  @param dst destination buffer of size siz, must not be NULL
     *  @param src source string, must not be NULL
     *  @param siz size of destination buffer
     *  @return the total length of the string the function tried to 
     *  create, i.e. the initial length of dst plus the length of src.
     */
    static size_t my_strlcat(char *dst, const char *src, size_t siz);

};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: ofstd.h,v $
 * Revision 1.3  2001-12-04 16:57:15  meichel
 * Implemented strlcpy and strlcat routines compatible with the
 *   corresponding BSD libc routines in class OFStandard
 *
 * Revision 1.2  2001/06/01 15:51:35  meichel
 * Updated copyright header
 *
 * Revision 1.1  2000/03/02 12:42:57  joergr
 * Added new class comprising all general purpose helper functions (first
 * entry: strlcpy - a mixture of strcpy and strncpy).
 *
 *
 *
 */
