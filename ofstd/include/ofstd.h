/*
 *
 *  Copyright (C) 2000-2003, OFFIS
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
 *  Author:  Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose: Class for various helper functions
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-06-06 09:43:54 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofstd.h,v $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __OFSTD_H
#define __OFSTD_H

#include "osconfig.h"
#include "ofstring.h"   /* for class OFString */

#define INCLUDE_CSTRING
#define INCLUDE_CSTDIO
#include "ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>  /* for size_t */
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

    // --- string functions ---

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
     *    create, i.e. strlen(src).  While this may seem somewhat
     *    confusing it was done to make truncation detection simple.
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
     *    create, i.e. the initial length of dst plus the length of src.
     *    While this may seem somewhat confusing it was done to make
     *    truncation detection simple.
     */
    static inline size_t strlcat(char *dst, const char *src, size_t siz)
    {
#ifdef HAVE_STRLCAT
      return ::strlcat(dst, src, siz);
#else
      return my_strlcat(dst, src, siz);
#endif
    }

    // --- file system functions ---

    /** check whether the given path exists.
     *  This function does not distinguish files from directories (use 'fileExists()'
     *  or 'directoryExists()' if required).
     *  @param pathName name of the path to be checked
     *  @return OFTrue if path exists, OFFalse otherwise
     */
    static OFBool pathExists(const OFString &pathName);

    /** check whether the given file exists.
     *  This function also checks that the specified path points to file and not to
     *  a directory (or the like).
     *  @param fileName name of the file to be checked
     *  @return OFTrue if file exists, OFFalse otherwise
     */
    static OFBool fileExists(const OFString &fileName);

    /** check whether the given directory exists.
     *  This function also checks that the specified path points to directory and
     *  not to a file (or the like).
     *  @param dirName name of the directory to be checked
     *  @return OFTrue if directory exists, OFFalse otherwise
     */
    static OFBool dirExists(const OFString &dirName);

    /** check whether the given path is readable.
     *  This function works for both files and directories.
     *  @param pathName name of the path to be checked
     *  @return OFTrue if path is readable, OFFalse otherwise
     */
    static OFBool isReadable(const OFString &pathName);

    /** check whether the given path is writeable.
     *  This function works for both files and directories.
     *  @param pathName name of the path to be checked
     *  @return OFTrue if path is writeable, OFFalse otherwise
     */
    static OFBool isWriteable(const OFString &pathName);

    /** normalize the given directory name.
     *  Removes trailing path separators from the directory name. If the resulting
     *  directory name is an empty string and the flag 'allowEmptyDirName' is OFFalse
     *  the directory name is to "." (current directory).
     *  @param result string variable in which the resulting directory name is stored
     *  @param dirName directory name to be normalized
     *  @param allowEmptyDirName flag indicating whether an empty directory name is allowed
     *  @return reference to the resulting directory name (same as 'result')
     */
    static OFString &normalizeDirName(OFString &result,
                                      const OFString &dirName,
                                      const OFBool allowEmptyDirName = OFFalse);

    /** combine the given directory and file name.
     *  Normalizes the directory name and appends the file name (with a path separator)
     *  if not empty. If both 'dirName' and 'fileName' are empty strings and the flag
     *  'allowEmptyDirName' is OFFalse the resulting path name is set "." (current
     *  directory).
     *  NB: This function neither checks whether the given 'dirName' exists nor whether
     *      the resulting path name points to a valid or existing file name.
     *  @param result string variable in which the resulting path name is stored
     *  @param dirName directory name to be combined with the file name
     *  @param fileName file name to be combined with the directory name
     *  @param allowEmptyDirName flag indicating whether an empty directory name is allowed
     *  @return reference to the resulting path name (same as 'result')
     */
    static OFString &combineDirAndFilename(OFString &result,
                                           const OFString &dirName,
                                           const OFString &fileName,
                                           const OFBool allowEmptyDirName = OFFalse);

    // --- other functions ---

    /** convert character string to HTML/XML mnenonic string.
     *  Characters with special meaning for HTML/XML (e.g. '<' and '&') are replaced by the
     *  corresponding mnenonics (e.g. "&lt;" and "&amp;").  If flag 'convertNonASCII' is OFTrue
     *  all characters > #127 are also converted (useful if only HTML 3.2 is supported which does
     *  not allow to specify the character set).
     ** @param sourceString source string to be converted
     *  @param markupString reference to character string where the result should be stored
     *  @param convertNonASCII convert non-ASCII characters (> #127) to numeric value (&#nnn;)
     *    if OFTrue
     *  @param xmlMode convert to XML markup string if OFTrue, HTML string otherwise.
     *    LF and CR are encoded as "&#10;" and "&#13;" in XML mode, the flag 'newlineAllowed'
     *    has no meaning in this case.
     *  @param newlineAllowed optional flag indicating whether newlines are allowed or not.
     *    If they are allowed the text "<br>" is used, "&para;" otherwise. The following
     *    combinations are accepted: LF, CR, LF CR, CF LF.
     ** @return reference to resulting 'markupString' (might be empty if 'sourceString' was empty)
     */
    static const OFString &convertToMarkupString(const OFString &sourceString,
                                                 OFString &markupString,
                                                 const OFBool convertNonASCII = OFFalse,
                                                 const OFBool xmlMode = OFTrue,
                                                 const OFBool newlineAllowed = OFFalse);

    /** encode binary data according to "Base64" as described in RFC 2045 (MIME).
     *  Basic algorithm: groups of 3 bytes from the binary input are coded as groups of 4 bytes in
     *  the textual output.  The input data is 'padded' with zeros to create a length that is an
     *  even multiple of 3.  A special character ('=') is used to denote padding so that the output
     *  can be decoded back to its exact size.
     *  If the input data is NULL an empty string is returned.
     ** @param data buffer with binary data to be encoded (big endian required!)
     *  @param length length of the input data buffer (in bytes)
     *  @param result reference to resulting string variable (Base64 encoded)
     *  @param width maximum number of characters per line in the output string
     *    (default: 0 = no line breaks, typical for MIME = 72)
     ** @return reference to the resulting string
     */
    static const OFString &encodeBase64(const unsigned char *data,
                                        const size_t length,
                                        OFString &result,
                                        const size_t width = 0);

    /** decode "Base64" encoded string.
     *  Any character that does not belong to the Base64 alphabet (0..9, A..Z, a..z, + and /) is
     *  ignored when decoding the input string.  This is especially true for line breaks which are
     *  usually contained in MIME (RFC 2045) encoded streams (see above).  The first occurence of
     *  a '=' character is taken as evidence that the end of the data has been reached.
     *  NB: The memory buffer in which the binary output is stored is allocated inside this function
     *      and has to to be freed (using "delete[]") by the caller!  Do not pass a pointer to an
     *      already allocated buffer to this function, the caller does not know the exact size anyway.
     ** @param data Base64 encoded input data (possibly padded with '=' at the end)
     *  @param result receives pointer to resulting buffer with binary data (big endian encoded)
     ** @return length of the resulting binary data (0 if an error occurred, in this case the buffer
     *    is deleted internally)
     */
    static size_t decodeBase64(const OFString &data,
                               unsigned char *&result);

    /** converts a floating-point number from an ASCII
     *  decimal representation to internal double-precision format.
     *  Unlike the atof() function defined in Posix, this implementation
     *  is not affected by a locale setting, the radix character is always
     *  assumed to be '.'
     *  This implementation does not set errno if the input cannot be parsed
     *  and it does not implement special handling for overflow/underflow
     *  or NaN values.  However, a return code indicates whether or not
     *  a successful conversion could be performed.
     *  The precision of this implementation is limited to approx. 9
     *  decimal digits.
     *  The use of this implementation can be disabled by defining
     *  the macro DISABLE_OFSTD_ATOF at compile time; in this case,
     *  the locale dependent Posix implementation of sscanf is used and
     *  the application is responsible for making sure that the Posix locale
     *  is activated at all times.
     *
     *  @param s
     *    A decimal ASCII floating-point number, optionally preceded by white
     *    space. Must have form "-I.FE-X", where I is the integer part of the
     *    mantissa, F is the fractional part of the mantissa, and X is the
     *    exponent.  Either of the signs may be "+", "-", or omitted.  Either I
     *    or F may be omitted, or both.  The decimal point isn't necessary
     *    unless F is present. The "E" may actually be an "e".  E and X may both
     *    be omitted (but not just one).
     *  @param success pointer to return status code, may be NULL.
     *    if present, a status code is stored in the variable pointed to by this
     *    parameter.  The status is OFTrue if a conversion could be performed
     *    and OFFalse if the string does not have the expected format.
     *  @return
     *    floating-point equivalent of string.
     *    If a terminating character is found before any floating-point
     *    digits, then zero is returned.
     */
     static double atof(const char *s,
                        OFBool *success = NULL);

     /** formats a floating-point number into an ASCII string.
      *  This function works similar to sprintf(), except that this
      *  implementation is not affected by a locale setting.
      *  The radix character is always '.'.
      *
      *  This implementation guarantees that the given string size
      *  is always respected by using strlcpy to copy the formatted
      *  string into the target buffer.
      *
      *  The use of this implementation can be disabled by defining
      *  the macro DISABLE_OFSTD_FTOA at compile time; in this case,
      *  the locale dependent Posix implementation of sprintf is used and
      *  the application is responsible for making sure that the Posix locale
      *  is activated at all times.
      *
      *  @param target pointer to target string buffer
      *  @param siz size of target string buffer
      *  @param val double value to be formatted
      *  @param flags processing flags. Any of the flags defined below
      *    can be combined by bit-wise or.
      *  @param width width from format (%8d), or 0
      *  @param precision precision from format (%.3d), or -1
      */
     static void ftoa(char *target,
                      size_t targetSize,
                      double value,
                      unsigned int flags = 0,
                      int width = 0,
                      int precision = -1);

     /** @name ftoa() processing flags.
      *  These flags can be combined by bit-wise or.
      */
     //@{

     /// Use %e or %E conversion format instead of %g or %G
     static const unsigned int ftoa_format_e;

     /// Use %f or %F conversion format instead of %g or %G
     static const unsigned int ftoa_format_f;

     /// Use %E, %F or %G conversion format instead of %e, %f or %g
     static const unsigned int ftoa_uppercase;

     /** convert value to alternate form. The result will always contain
      *  a decimal point, even if no digits follow the point. For g and G
      *  conversions, trailing zeroes will not be removed from the result.
      */
     static const unsigned int ftoa_alternate;

     /// left-justify number be within the field
     static const unsigned int ftoa_leftadj;

     /// pad with zeroes instead of blanks
     static const unsigned int ftoa_zeropad;

     //@}

    /** Checks if a given string consists only of characters which are specified in a
     *  given charset. Note that in case one of the parameters equals NULL, OFTrue will
     *  be returned.
     *  @param str String which shall be checked.
     *  @param charset Possible character set for s.
     *  @return OFTrue if the given string consists only of characters which are specified
     *    in the given charset; OFFalse otherwise.
     */
     static OFBool stringMatchesCharacterSet( const char *str, const char *charset );

    /** makes the current process sleep until seconds seconds have 
     *  elapsed or a signal arrives which is not ignored.
     *  @param seconds number of seconds to sleep
     *  @return Zero if the requested time has elapsed, or the number of seconds left to sleep.
     */
    static inline unsigned int sleep(unsigned int seconds)
    {
#ifdef HAVE_SLEEP
      return ::sleep(seconds);
#else
      return my_sleep(seconds);
#endif
    }

 private:

    /** private implementation of strlcpy. Called when strlcpy
     *  is not available in the standard library.
     *  @param dst destination buffer of size siz, must not be NULL
     *  @param src source string, must not be NULL
     *  @param siz size of destination buffer
     *  @return the total length of the string the function tried to
     *    create, i.e. strlen(src).
     */
    static size_t my_strlcpy(char *dst, const char *src, size_t siz);

    /** private implementation of strlcat. Called when strlcat
     *  is not available in the standard library.
     *  @param dst destination buffer of size siz, must not be NULL
     *  @param src source string, must not be NULL
     *  @param siz size of destination buffer
     *  @return the total length of the string the function tried to
     *    create, i.e. the initial length of dst plus the length of src.
     */
    static size_t my_strlcat(char *dst, const char *src, size_t siz);

    /** makes the current process sleep until seconds seconds have 
     *  elapsed or a signal arrives which is not ignored.
     *  @param seconds number of seconds to sleep
     *  @return Zero if the requested time has elapsed, or the number of seconds left to sleep.
     */
    static unsigned int my_sleep(unsigned int seconds);

};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: ofstd.h,v $
 * Revision 1.15  2003-06-06 09:43:54  meichel
 * Added static sleep function in class OFStandard. This replaces the various
 *   calls to sleep(), Sleep() and usleep() throughout the toolkit.
 *
 * Revision 1.14  2003/04/17 15:50:51  joergr
 * Replace LF and CR by &#10; and &#13; in XML mode instead of &#182; (para).
 *
 * Revision 1.13  2003/03/12 14:57:47  joergr
 * Added apostrophe (') to the list of characters to be replaced by the
 * corresponding HTML/XML mnenonic.
 *
 * Revision 1.12  2002/12/13 13:45:33  meichel
 * Removed const from decodeBase64() return code, needed on MIPSpro
 *
 * Revision 1.11  2002/12/05 13:49:36  joergr
 * Moved definition of ftoa() processing flags to implementation file to avoid
 * compiler errors (e.g. on Sun CC 2.0.1).
 *
 * Revision 1.10  2002/12/04 09:13:00  meichel
 * Implemented a locale independent function OFStandard::ftoa() that
 *   converts double to string and offers all the flexibility of the
 *   sprintf family of functions.
 *
 * Revision 1.9  2002/11/27 11:23:06  meichel
 * Adapted module ofstd to use of new header file ofstdinc.h
 *
 * Revision 1.8  2002/07/02 15:17:57  wilkens
 * Added function OFStandard::stringMatchesCharacterSet(...).
 *
 * Revision 1.7  2002/06/20 12:02:38  meichel
 * Implemented a locale independent function OFStandard::atof() that
 *   converts strings to double and optionally returns a status code
 *
 * Revision 1.6  2002/05/14 08:12:51  joergr
 * Added support for Base64 (MIME) encoding and decoding.
 *
 * Revision 1.5  2002/04/25 09:13:52  joergr
 * Moved helper function which converts a conventional character string to an
 * HTML/XML mnenonic string (e.g. using "&lt;" instead of "<") from module
 * dcmsr to ofstd.
 *
 * Revision 1.4  2002/04/11 12:06:42  joergr
 * Added general purpose routines to check whether a file exists, a path points
 * to a directory or a file, etc.
 *
 * Revision 1.3  2001/12/04 16:57:15  meichel
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
