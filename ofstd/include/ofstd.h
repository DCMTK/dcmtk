/*
 *
 *  Copyright (C) 2000-2002, OFFIS
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-25 09:13:52 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofstd.h,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
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

#include "ofstring.h"   /* for class OFString */


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
     *  Characters with special meaning for HTML/XML (e.g. '<' and '&') are replace by the
     *  corresponding mnenonics (e.g. "&lt;" and "&amp;").  If flag 'convertNonASCII' is OFTrue
     *  all characters > #127 are also converted (useful if only HTML 3.2 is supported which does
     *  not allow to specify the character set).
     ** @param  sourceString     source string to be converted
     *  @param  markupString     reference to character string where the result should be stored
     *  @param  convertNonASCII  convert non-ASCII characters (> #127) to numeric value (&#nnn;)
     *                           if OFTrue
     *  @param  xmlMode          convert to XML markup string if OFTrue, HTML string otherwise.
     *                           Newlines are always encoded as "&#182;" in XML mode, the flag
     *                           'newlineAllowed' has no meaning in this case.
     *  @param  newlineAllowed   optional flag indicating whether newlines are allowed or not.
     *                           If they are allowed the text "<br>" is used, "&para;" otherwise.
     *                           The following combinations are accepted: LF, CR, LF CR, CF LF.
     ** @return reference to resulting 'markupString' (might be empty if 'sourceString' was empty)
     */
    static const OFString &convertToMarkupString(const OFString &sourceString,
                                                 OFString &markupString,
                                                 const OFBool convertNonASCII = OFFalse,
                                                 const OFBool xmlMode = OFTrue,
                                                 const OFBool newlineAllowed = OFFalse);


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
 * Revision 1.5  2002-04-25 09:13:52  joergr
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
