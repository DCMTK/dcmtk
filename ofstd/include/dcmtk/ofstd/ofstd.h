/*
 *
 *  Copyright (C) 2000-2015, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose: Class for various helper functions
 *
 */


#ifndef OFSTD_H
#define OFSTD_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oflist.h"     /* for class OFList */
#include "dcmtk/ofstd/ofstring.h"   /* for class OFString */
#include "dcmtk/ofstd/oftypes.h"    /* for OFBool */
#include "dcmtk/ofstd/oftraits.h"   /* for OFenable_if, ... */
#include "dcmtk/ofstd/ofcond.h"     /* for OFCondition */
#include "dcmtk/ofstd/oflimits.h"   /* for OFnumeric_limits<T>::max() */

#define INCLUDE_CASSERT
#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>  /* for size_t */
#endif
END_EXTERN_C

/*------------------------*
 *  forward declarations  *
 *------------------------*/

class OFFilename;

/*---------------------*
 *  class declaration  *
 *---------------------*/

/** A class for various helper functions.
 *  This class is used to comprise a number of "global" helper functions.
 */
class DCMTK_OFSTD_EXPORT OFStandard
{

 public:

    // --- type definitions ---

    /** Markup language mode
     */
    enum E_MarkupMode
    {
        /// HTML (Hyper Text Markup Language)
        MM_HTML,
        /// HTML 3.2 (Hyper Text Markup Language)
        MM_HTML32,
        /// XHTML (Extensible Hyper Text Markup Language)
        MM_XHTML,
        /// XML (Extensible Markup Language)
        MM_XML
    };

    class OFHostent;
    class OFGroup;
    class OFPasswd;

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

    /** convert a given error code to a string. This function wraps the various
     *  approaches found on different systems. Internally, the standard function
     *  strerror() or strerror_r() is used.
     *  @param errnum error code to be converted
     *  @param buf buffer which is used to store the result string (if supported)
     *  @param buflen size if the buffer in bytes
     *  @return pointer to string describing the error code. Please note that depending
     *     on the implementation of the function used, the result may or may not be a
     *     pointer to buf. The return value can also be NULL if the buffer is invalid.
     */
    static const char *strerror(const int errnum,
                                char *buf,
                                const size_t buflen);

    /** returns the upper-case version of a given string
     *  @param result string variable in which the result is stored
     *  @param value string value to be converted to upper case
     *  @return reference to the resulting string (same as 'result')
     */
    static OFString &toUpper(OFString &result,
                             const OFString &value);

    /** returns the upper-case version of a given string.
     *  NB: This function changes the parameter 'value'.
     *  @param value string value to be converted to upper case
     *  @return reference to the resulting string (same as 'value')
     */
    static OFString &toUpper(OFString &value);

    /** returns the lower-case version of a given string
     *  @param result string variable in which the result is stored
     *  @param value string value to be converted to lower case
     *  @return reference to the resulting string (same as 'result')
     */
    static OFString &toLower(OFString &result,
                             const OFString &value);

    /** returns the lower-case version of a given string.
     *  NB: This function changes the parameter 'value'.
     *  @param value string value to be converted to lower case
     *  @return reference to the resulting string (same as 'value')
     */
    static OFString &toLower(OFString &value);

    // --- file system functions ---

    /** check whether the given path exists.
     *  This function does not distinguish files from directories (use 'fileExists()'
     *  or 'directoryExists()' if required).
     *  @param pathName name of the path to be checked. This path name may contain
     *    wide characters if support enabled. Since there are various constructors
     *    for the OFFilename class, a "char *", "OFString" or "wchar_t *" can also
     *    be passed directly to this parameter.
     *  @return OFTrue if path exists, OFFalse otherwise
     */
    static OFBool pathExists(const OFFilename &pathName);

    /** check whether the given file exists.
     *  This function also checks that the specified path points to file and not to
     *  a directory (or the like).
     *  @param fileName name of the file to be checked. This filename may contain wide
     *    characters if support enabled. Since there are various constructors for the
     *    OFFilename class, a "char *", "OFString" or "wchar_t *" can also be passed
     *    directly to this parameter.
     *  @return OFTrue if file exists, OFFalse otherwise
     */
    static OFBool fileExists(const OFFilename &fileName);

    /** check whether the given directory exists.
     *  This function also checks that the specified path points to directory and
     *  not to a file (or the like).
     *  @param dirName name of the directory to be checked. This directory name may
     *    contain wide characters if support enabled. Since there are various
     *    constructors for the OFFilename class, a "char *", "OFString" or "wchar_t *"
     *    can also be passed directly to this parameter.
     *  @return OFTrue if directory exists, OFFalse otherwise
     */
    static OFBool dirExists(const OFFilename &dirName);

    /** check whether the given path is readable.
     *  This function works for both files and directories.
     *  @param pathName name of the path to be checked. This path name may contain
     *    wide characters if support enabled. Since there are various constructors
     *    for the OFFilename class, a "char *", "OFString" or "wchar_t *" can also
     *    be passed directly to this parameter.
     *  @return OFTrue if path is readable, OFFalse otherwise
     */
    static OFBool isReadable(const OFFilename &pathName);

    /** check whether the given path is writeable.
     *  This function works for both files and directories.
     *  @param pathName name of the path to be checked. This path name may contain
     *    wide characters if support enabled. Since there are various constructors
     *    for the OFFilename class, a "char *", "OFString" or "wchar_t *" can also
     *    be passed directly to this parameter.
     *  @return OFTrue if path is writeable, OFFalse otherwise
     */
    static OFBool isWriteable(const OFFilename &pathName);

    /** get directory name component from given path name.
     *  Extracts the substring before the last path separator. If there is no path
     *  separator in the given path name, the value of 'pathName' is returned by
     *  default; if 'assumeDirName' is OFFalse, an empty string is returned.
     *  NB: This function neither checks whether the given 'pathName' exists nor
     *      whether the resulting name points to a valid or existing directory.
     *  @note This method is provided for reasons of backward compatibility. Internally,
     *    the following method (OFFilename version) is used.
     *  @param result string variable in which the resulting directory name is stored
     *  @param pathName path name from which the directory name should be extracted
     *  @param assumeDirName assume that there always is a directory name in 'pathName'
     *  @return reference to the resulting directory name (same as 'result')
     */
    static OFString &getDirNameFromPath(OFString &result,
                                        const OFString &pathName,
                                        const OFBool assumeDirName = OFTrue);

    /** get directory name component from given path name.
     *  Extracts the substring before the last path separator. If there is no path
     *  separator in the given path name, the value of 'pathName' is returned by
     *  default; if 'assumeDirName' is OFFalse, an empty string is returned.
     *  NB: This function neither checks whether the given 'pathName' exists nor
     *      whether the resulting name points to a valid or existing directory.
     *  @param result string variable in which the resulting directory name is stored.
     *    This name may contain wide characters if support is enabled and 'pathName'
     *    contains wide characters. In any case, the resulting string is stored with
     *    UTF-8 encoding (8-bit) as an alternative representation.
     *  @param pathName path name from which the directory name should be extracted.
     *    This name may contain wide characters if support is enabled. Since there are
     *    various constructors for the OFFilename class, a "char *", "OFString" or
     *    "wchar_t *" can also be passed directly to this parameter.
     *  @param assumeDirName assume that there always is a directory name in 'pathName'
     *  @return reference to the resulting directory name (same as 'result')
     */
    static OFFilename &getDirNameFromPath(OFFilename &result,
                                          const OFFilename &pathName,
                                          const OFBool assumeDirName = OFTrue);

    /** get file name component from given path name.
     *  Extracts the substring after the last path separator. If there is no path
     *  separator in the given path name, the value of 'pathName' is returned by
     *  default; if 'assumeFilename' is OFFalse, an empty string is returned.
     *  NB: This function neither checks whether the given 'pathName' exists nor
     *      whether the resulting name points to a valid or existing file.
     *  @note This method is provided for reasons of backward compatibility.
     *    Internally, the following method (OFFilename version) is used.
     *  @param result string variable in which the resulting file name is stored
     *  @param pathName path name from which the file name should be extracted
     *  @param assumeFilename assume that there always is a file name in 'pathName'
     *  @return reference to the resulting file name (same as 'result')
     */
    static OFString &getFilenameFromPath(OFString &result,
                                         const OFString &pathName,
                                         const OFBool assumeFilename = OFTrue);

    /** get file name component from given path name.
     *  Extracts the substring after the last path separator. If there is no path
     *  separator in the given path name, the value of 'pathName' is returned by
     *  default; if 'assumeFilename' is OFFalse, an empty string is returned.
     *  NB: This function neither checks whether the given 'pathName' exists nor
     *      whether the resulting name points to a valid or existing file.
     *  @param result string variable in which the resulting file name is stored.
     *    This name may contain wide characters if support is enabled and 'pathName'
     *    contains wide characters. In any case, the resulting string is stored with
     *    UTF-8 encoding (8-bit) as an alternative representation.
     *  @param pathName path name from which the file name should be extracted.
     *    This name may contain wide characters if support is enabled. Since there
     *    are various constructors for the OFFilename class, a "char *", "OFString"
     *    or "wchar_t *" can also be passed directly to this parameter.
     *  @param assumeFilename assume that there always is a file name in 'pathName'
     *  @return reference to the resulting file name (same as 'result')
     */
    static OFFilename &getFilenameFromPath(OFFilename &result,
                                           const OFFilename &pathName,
                                           const OFBool assumeFilename = OFTrue);

    /** normalize the given directory name.
     *  Removes trailing path separators from the directory name. If the resulting
     *  directory name is an empty string and the flag 'allowEmptyDirName' is OFFalse
     *  the directory name is set to "." (current directory). If the resulting directory
     *  name is "." and the flag 'allowEmptyDirName' is OFTrue the directory name is set
     *  to an empty string.
     *  @note This method is provided for reasons of backward compatibility. Internally,
     *    the following method (OFFilename version) is used.
     *  @param result string variable in which the resulting directory name is stored
     *  @param dirName directory name to be normalized
     *  @param allowEmptyDirName flag indicating whether an empty directory name is allowed
     *  @return reference to the resulting directory name (same as 'result')
     */
    static OFString &normalizeDirName(OFString &result,
                                      const OFString &dirName,
                                      const OFBool allowEmptyDirName = OFFalse);

    /** normalize the given directory name.
     *  Removes trailing path separators from the directory name. If the resulting
     *  directory name is an empty string and the flag 'allowEmptyDirName' is OFFalse
     *  the directory name is set to "." (current directory). If the resulting directory
     *  name is "." and the flag 'allowEmptyDirName' is OFTrue the directory name is set
     *  to an empty string.
     *  @param result string variable in which the resulting directory name is stored.
     *    This name may contain wide characters if support is enabled and 'dirName'
     *    contains wide characters. In any case, the resulting string is stored with UTF-8
     *    encoding (8-bit) as an alternative representation.
     *  @param dirName directory name to be normalized. This name may contain wide
     *    characters if support is enabled. Since there are various constructors for the
     *    OFFilename class, a "char *", "OFString" or "wchar_t *" can also be passed
     *    directly to this parameter.
     *  @param allowEmptyDirName flag indicating whether an empty directory name is allowed
     *  @return reference to the resulting directory name (same as 'result')
     */
    static OFFilename &normalizeDirName(OFFilename &result,
                                        const OFFilename &dirName,
                                        const OFBool allowEmptyDirName = OFFalse);

    /** combine the given directory and file name.
     *  Normalizes the directory name and appends the file name (with a path separator)
     *  if not empty. If both 'dirName' and 'fileName' are empty strings and the flag
     *  'allowEmptyDirName' is OFFalse the resulting path name is set to "." (current
     *  directory). If 'dirName' is "." and the flag 'allowEmptyDirName' is OFTrue an
     *  empty directory name is used.
     *  NB: This function neither checks whether the given 'dirName' exists nor whether
     *      the resulting path name points to a valid or existing file name. Furthermore,
     *      the value of 'dirName' is ignored if 'fileName' starts with a path separator.
     *  @note This method is provided for reasons of backward compatibility. Internally,
     *    the following method (OFFilename version) is used.
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

    /** combine the given directory and file name.
     *  Normalizes the directory name and appends the file name (with a path separator)
     *  if not empty. If both 'dirName' and 'fileName' are empty strings and the flag
     *  'allowEmptyDirName' is OFFalse the resulting path name is set to "." (current
     *  directory). If 'dirName' is "." and the flag 'allowEmptyDirName' is OFTrue an
     *  empty directory name is used.
     *  NB: This function neither checks whether the given 'dirName' exists nor whether
     *      the resulting path name points to a valid or existing file name. Furthermore,
     *      the value of 'dirName' is ignored if 'fileName' starts with a path separator.
     *  @param result string variable in which the resulting path name is stored. This
     *    name may contain wide characters if support is enabled and 'dirName' as well as
     *    'fileName' contain wide characters. In any case, the resulting string is stored
     *    with UTF-8 encoding (8-bit) as an alternative representation.
     *  @param dirName directory name to be combined with the file name. This name may
     *    contain wide characters if support is enabled. Since there are various
     *    constructors for the OFFilename class, a "char *", "OFString" or "wchar_t *" can
     *    also be passed directly to this parameter.
     *  @param fileName file name to be combined with the directory name. Should contain
     *    wide characters if and only if 'dirName' contains wide characters.
     *  @param allowEmptyDirName flag indicating whether an empty directory name is allowed
     *  @return reference to the resulting path name (same as 'result')
     */
    static OFFilename &combineDirAndFilename(OFFilename &result,
                                             const OFFilename &dirName,
                                             const OFFilename &fileName,
                                             const OFBool allowEmptyDirName = OFFalse);

    /** remove root directory prefix from given path name.
     *  In case 'pathName' starts with 'rootDir', the common prefix is removed.
     *  Otherwise, an empty string is returned (or a cleared OFFilename in case of error).
     *  @param result string variable in which the resulting path name is stored.
     *    This name may contain wide characters if support is enabled and 'rootDir' as
     *    well as 'pathName' contain wide characters. In any case, the resulting string
     *    is stored with UTF-8 encoding (8-bit) as an alternative representation.
     *  @param rootDir name of the root directory to be removed. This name may contain
     *    wide characters if support is enabled. Since there are various constructors for
     *    the OFFilename class, a "char *", "OFString" or "wchar_t *" can also be passed
     *    directly to this parameter.
     *  @param pathName path name from which the root directory (prefix) is removed.
     *    Should contain wide characters if and only if 'rootDir' contains wide characters.
     *  @param allowLeadingPathSeparator flag indicating whether a leading path separator
     *    is allowed for the resulting path name (automatically removed otherwise)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition removeRootDirFromPathname(OFFilename &result,
                                                 const OFFilename &rootDir,
                                                 const OFFilename &pathName,
                                                 const OFBool allowLeadingPathSeparator = OFTrue);

    /** append a filename extension to the given filename
     *  @param result string variable in which the resulting filename is stored.
     *    This name may contain wide characters if support is enabled and 'fileName'
     *    contains wide characters. In any case, the resulting string is stored with
     *    UTF-8 encoding (8-bit) as an alternative representation.
     *  @param fileName filename to which the extension should be appended. This name
     *    may contain wide characters if support is enabled. Since there are various
     *    constructors for the OFFilename class, a "char *", "OFString" or "wchar_t *"
     *    can also be passed directly to this parameter.
     *  @param fileExtension filename extension to be appended (e.g.\ ".bak"). It is
     *    converted to wide characters if 'fileName' contains wide characters.
     *  @return reference to the resulting path name (same as 'result')
     */
    static OFFilename &appendFilenameExtension(OFFilename &result,
                                               const OFFilename &fileName,
                                               const OFFilename &fileExtension);

    /** scan a given directory (recursively) and add all filenames found to a list
     *  @note This method is provided for reasons of backward compatibility. Internally,
     *    the following method (OFFilename version) is used.
     *  @param directory name of the directory to be scanned
     *  @param fileList list to which the filenames are added. Please note that the list
     *    is not not cleared automatically before new entries are added.
     *  @param pattern optional wildcard pattern used to match the filenames against.
     *    By default all files match. In order to work under Unix the system function
     *    fnmatch() is required.
     *  @param dirPrefix optional prefix added to the directory name.
     *    This prefix will, however, not be part of the filenames added to the list.
     *  @param recurse flag indicating whether to search recursively (default) or not
     *  @return number of new files added to the list
     */
    static size_t searchDirectoryRecursively(const OFString &directory,
                                             OFList<OFString> &fileList,
                                             const OFString &pattern = "",
                                             const OFString &dirPrefix = "",
                                             const OFBool recurse = OFTrue);

    /** scan a given directory (recursively) and add all filenames found to a list
     *  @param directory name of the directory to be scanned. This name may contain
     *    wide characters if support is enabled. Since there are various constructors
     *    for the OFFilename class, a "char *", "OFString" or "wchar_t *" can also be
     *    passed directly to this parameter.
     *  @param fileList list to which the filenames are added. These names may contain
     *    wide characters if support is enabled and 'directory' contains wide characters.
     *    In any case, the resulting string is stored with UTF-8 encoding (8-bit) as an
     *    alternative representation. Please note that the list is not not cleared
     *    automatically before new entries are added.
     *  @param pattern wildcard pattern used to match the filenames against. Should
     *    contain wide characters if and only if 'directory' contains wide characters.
     *    An empty pattern matches all files. In order to work under Unix the system
     *    function fnmatch() is required.
     *  @param dirPrefix prefix added to the directory name. Should contain wide
     *    characters if and only if 'directory' contains wide characters.
     *    This prefix will, however, not be part of the filenames added to the list.
     *  @param recurse flag indicating whether to search recursively (default) or not
     *  @return number of new files added to the list
     */
    static size_t searchDirectoryRecursively(const OFFilename &directory,
                                             OFList<OFFilename> &fileList,
                                             const OFFilename &pattern,
                                             const OFFilename &dirPrefix,
                                             const OFBool recurse = OFTrue);

    /** create a directory (including sub-directories) if it does not yet exist.  In other
     *  words, this function creates directories recursively, i.e. with all sub-components.
     *  @param dirName name of the directory to be created. This name may contain wide
     *    characters if support is enabled. Since there are various constructors for the
     *    OFFilename class, a "char *", "OFString" or "wchar_t *" can also be passed
     *    directly to this parameter.
     *  @param rootDir optional name of a root directory (prefix of 'dirName') that already
     *    exists and that can, therefore, be skipped during the creation of sub-directories.
     *    Should contain wide characters if and only if 'dirName' contains wide characters.
     *  @return status, EC_Normal if successful (directory created or already exists), an
     *    error code otherwise
     */
    static OFCondition createDirectory(const OFFilename &dirName,
                                       const OFFilename &rootDir);

    /** copy an existing file to a new file
     *  @param sourceFilename name of the existing file (including directory) to be copied.
     *    This filename may contain wide characters if support enabled. Since there are various
     *    constructors for the OFFilename class, a "char *", "OFString" or "wchar_t *" can also
     *    be passed directly to this parameter.
     *  @param destFilename name of the new file (including directory) to be created as a copy.
     *    This filename may contain wide characters if support enabled. Since there are various
     *    constructors for the OFFilename class, a "char *", "OFString" or "wchar_t *" can also
     *    be passed directly to this parameter.
     *  @return OFTrue if copying the file was successful, OFFalse otherwise. On most systems,
     *    the 'errno' variable is also set to a system-specific error code in case of failure.
     */
    static OFBool copyFile(const OFFilename &sourceFilename,
                           const OFFilename &destFilename);

    /** delete given file from filesystem
     *  @param filename name of the file (including directory) to delete. This filename may
     *    contain wide characters if support enabled. Since there are various constructors
     *    for the OFFilename class, a "char *", "OFString" or "wchar_t *" can also be passed
     *    directly to this parameter.
     *  @return OFTrue if deleting the file was successful, OFFalse otherwise. On most systems,
     *    the 'errno' variable is also set to a system-specific error code in case of failure.
     */
    static OFBool deleteFile(const OFFilename &filename);

    /** change name of a given file
     *  @param oldFilename current name of the file (including directory) to be renamed.
     *    This filename may contain wide characters if support enabled. Since there are various
     *    constructors for the OFFilename class, a "char *", "OFString" or "wchar_t *" can also
     *    be passed directly to this parameter.
     *  @param newFilename new name of the file (including directory), i.e.\ after renaming.
     *    Should contain wide characters if and only if 'oldFilename' contains wide characters.
     *  @return OFTrue if changing the name was successful, OFFalse otherwise. On most systems,
     *    the 'errno' variable is also set to a system-specific error code in case of failure.
     */
    static OFBool renameFile(const OFFilename &oldFilename,
                             const OFFilename &newFilename);

    /** determine size of given file (in bytes)
     *  @param filename name of the file to be checked. This filename may contain wide
     *    characters if support enabled. Since there are various constructors for the
     *    OFFilename class, a "char *", "OFString" or "wchar_t *" can also be passed
     *    directly to this parameter.
     *  @return size of the file in bytes (0 in case of error)
     */
    static size_t getFileSize(const OFFilename &filename);

    // --- other functions ---

    /** check whether conversion to a HTML/XML mnenonic string is required.
     *  This check can be performed before convertToMarkupStream() or convertToMarkupString()
     *  is called in order to speed up the process in case the conversion is not required.
     ** @param sourceString source string to be checked.  May contain one or more NULL bytes.
     *  @param convertNonASCII convert non-ASCII characters (< #32 and >= #127) to numeric
     *    value (@&@#nnn;) if OFTrue
     *  @param maxLength maximum number of characters from the source string to be converted.
     *    A value of 0 means all characters.
     ** @return OFTrue if markup conversion is required, OFFalse otherwise
     */
    static OFBool checkForMarkupConversion(const OFString &sourceString,
                                           const OFBool convertNonASCII = OFFalse,
                                           const size_t maxLength = 0);

    /** convert character string to a HTML/XHTML/XML mnenonic stream.
     *  Characters with special meaning for HTML/XHTML/XML (e.g. '<' and '&') are replaced by the
     *  corresponding mnenonics (e.g. "&lt;" and "&amp;").  If flag 'convertNonASCII' is OFTrue,
     *  all characters < #32 and >= #127 are also converted (useful if only HTML 3.2 is supported
     *  which does not allow to specify the character set).  In HTML 3.2 mode, the quotation mark
     *  (") is converted to "&#34;" instead of "&quot;" because the latter entity is not defined.
     *  In HTML mode, the apostrophe sign (') is converted to "&#39;" instead of "&apos;" for the
     *  same reason.
     ** @param out stream used for the HTML/XHTML/XML mnenonic output
     *  @param sourceString source string to be converted.  May contain one or more NULL bytes.
     *  @param convertNonASCII convert non-ASCII characters (< # 32 and >= #127) to numeric value
     *    (@&@#nnn;) if OFTrue
     *  @param markupMode convert to HTML, HTML 3.2, XHTML or XML markup.
     *    LF and CR are encoded as "&#10;" and "&#13;" in XML mode, the flag 'newlineAllowed'
     *    has no meaning in this case.
     *  @param newlineAllowed optional flag indicating whether newlines are allowed or not.
     *    If they are allowed, the text "<br>" (HTML) or "<br />" (XHTML) is used, "&para;"
     *    otherwise.  The following combinations are accepted: LF, CR, LF CR, CF LF.
     *  @param maxLength maximum number of characters from the source string to be converted.
     *    A value of 0 means all characters.
     ** @return status, always returns EC_Normal
     */
    static OFCondition convertToMarkupStream(STD_NAMESPACE ostream &out,
                                             const OFString &sourceString,
                                             const OFBool convertNonASCII = OFFalse,
                                             const E_MarkupMode markupMode = MM_XML,
                                             const OFBool newlineAllowed = OFFalse,
                                             const size_t maxLength = 0);

    /** convert character string to a HTML/XHTML/XML mnenonic string.
     *  Characters with special meaning for HTML/XHTML/XML (e.g. '<' and '&') are replaced by the
     *  corresponding mnenonics (e.g. "&lt;" and "&amp;").  If flag 'convertNonASCII' is OFTrue,
     *  all characters < #32 and >= #127 are also converted (useful if only HTML 3.2 is supported
     *  which does not allow to specify the character set).  In HTML 3.2 mode, the quotation mark
     *  (") is converted to "&#34;" instead of "&quot;" because the latter entity is not defined.
     *  In HTML mode, the apostrophe sign (') is converted to "&#39;" instead of "&apos;" for the
     *  same reason.
     ** @param sourceString source string to be converted.  May also contain one or more NULL bytes.
     *  @param markupString reference to character string where the result should be stored
     *  @param convertNonASCII convert non-ASCII characters (< # 32 and >= #127) to numeric value
     *    (@&@#nnn;) if OFTrue
     *  @param markupMode convert to HTML, HTML 3.2, XHTML or XML markup string.
     *    LF and CR are encoded as "@&@#10;" and "@&@#13;" in XML mode, the flag 'newlineAllowed'
     *    has no meaning in this case.
     *  @param newlineAllowed optional flag indicating whether newlines are allowed or not.
     *    If they are allowed, the text "<br>" (HTML) or "<br />" (XHTML) is used, "&para;"
     *    otherwise.  The following combinations are accepted: LF, CR, LF CR, CF LF.
     *  @param maxLength maximum number of characters from the source string to be converted.
     *    A value of 0 means all characters.
     ** @return reference to resulting 'markupString' (might be empty if 'sourceString' was empty)
     */
    static const OFString &convertToMarkupString(const OFString &sourceString,
                                                 OFString &markupString,
                                                 const OFBool convertNonASCII = OFFalse,
                                                 const E_MarkupMode markupMode = MM_XML,
                                                 const OFBool newlineAllowed = OFFalse,
                                                 const size_t maxLength = 0);

    /** check whether conversion to an octal format is required.
     *  This check can be performed before convertToOctalStream() or convertToOctalString()
     *  is called in order to speed up the process in case the conversion is not required.
     ** @param sourceString source string to be checked.  May contain one or more NULL bytes.
     *  @param maxLength maximum number of characters from the source string to be converted.
     *    A value of 0 means all characters.
     ** @return OFTrue if markup conversion is required, OFFalse otherwise
     */
    static OFBool checkForOctalConversion(const OFString &sourceString,
                                          const size_t maxLength = 0);

    /** convert character string to an octal format stream.
     *  All non-ASCII and control characters (code < #32 and >= #127) are converted to their
     *  octal representation, i.e. to '\\ooo' where 'ooo' are the three octal digits of the
     *  character.  All other characters are output as is.  See section 6.1.2.3 in DICOM PS 3.5.
     ** @param out stream used for the output
     *  @param sourceString source string to be converted.  May contain one or more NULL bytes.
     *  @param maxLength maximum number of characters from the source string to be converted.
     *    A value of 0 means all characters.
     ** @return status, always returns EC_Normal
     */
    static OFCondition convertToOctalStream(STD_NAMESPACE ostream &out,
                                            const OFString &sourceString,
                                            const size_t maxLength = 0);

    /** convert character string to an octal format string.
     *  All non-ASCII and control characters (code < #32 and >= #127) are converted to their
     *  octal representation, i.e. to '\\ooo' where 'ooo' are the three octal digits of the
     *  character.  All other characters are output as is.  See section 6.1.2.3 in DICOM PS 3.5.
     ** @param sourceString source string to be converted.  May contain one or more NULL bytes.
     *  @param octalString reference to character string where the result should be stored
     *  @param maxLength maximum number of characters from the source string to be converted.
     *    A value of 0 means all characters.
     ** @return reference to resulting 'octalString' (might be empty if 'sourceString' was empty)
     */
    static const OFString &convertToOctalString(const OFString &sourceString,
                                                OFString &octalString,
                                                const size_t maxLength = 0);

    /** encode binary data according to "Base64" as described in RFC 2045 (MIME).
     *  Basic algorithm: groups of 3 bytes from the binary input are coded as groups of 4 bytes in
     *  the textual output.  The input data is 'padded' with zeros to create a length that is an
     *  even multiple of 3.  A special character ('=') is used to denote padding so that the output
     *  can be decoded back to its exact size.
     *  If the input data is NULL an error code (EC_IllegalParameter) is returned.
     ** @param out output stream used for the base64 encoded data
     *  @param data buffer with binary data to be encoded (big endian required!)
     *  @param length length of the input data buffer (in bytes)
     *  @param width maximum number of characters per line in the output stream
     *    (default: 0 = no line breaks, typical for MIME = 72)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition encodeBase64(STD_NAMESPACE ostream &out,
                                    const unsigned char *data,
                                    const size_t length,
                                    const size_t width = 0);

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
     *  usually contained in MIME (RFC 2045) encoded streams (see above).  The first occurrence of
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
     *  @note The use of this implementation can be disabled by defining
     *    the macro DISABLE_OFSTD_ATOF at compile time; in this case,
     *    the locale dependent Posix implementation of sscanf is used and
     *    the application is responsible for making sure that the Posix locale
     *    is activated at all times.
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
      *  This implementation guarantees that the given string size
      *  is always respected by using strlcpy to copy the formatted
      *  string into the target buffer.
      *  @note The use of this implementation can be disabled by defining
      *    the macro DISABLE_OFSTD_FTOA at compile time; in this case,
      *    the locale dependent Posix implementation of sprintf is used and
      *    the application is responsible for making sure that the Posix locale
      *    is activated at all times.
      *  @param target pointer to target string buffer
      *  @param targetSize size of target string buffer
      *  @param value double value to be formatted
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

    /** makes the current process sleep until seconds seconds have
     *  elapsed or a signal arrives which is not ignored
     *  @param seconds number of seconds to sleep
     *  @return zero if the requested time has elapsed, or the number of seconds
     *    left to sleep
     */
    static inline unsigned int sleep(unsigned int seconds)
    {
#if defined(HAVE_SLEEP) && !defined(HAVE_WINDOWS_H)
      // we only use this call if HAVE_WINDOWS_H is undefined because
      // MinGW has sleep() but no prototype
      return ::sleep(seconds);
#else
      return my_sleep(seconds);
#endif
    }

    /** makes the current process sleep until the given number of milliseconds
     *  have elapsed or a signal which is not ignored arrives
     *  @param millisecs number of milliseconds to sleep
     */
    static void milliSleep(unsigned int millisecs);

    /** Determines the identification of the running process.
     *  @return the process ID of the currently running process.
     */
    static long getProcessID();

    /** check whether the addition of two 32-bit integers yields in an overflow
     *  @param summand1 first integer value to be added
     *  @param summand2 second integer value to be added
     *  @return OFTrue if an overflow occurred during the addition, OFFalse otherwise
     */
    static inline OFBool check32BitAddOverflow(const Uint32 summand1,
                                               const Uint32 summand2)
    {
      return (0xffffffff - summand1 < summand2);
    }

    /** check whether subtraction is safe (i.e.\ no underflow occurs) and if so,
     *  perform it (i.e.\ compute minuend-subtrahend=difference). Only works for
     *  unsigned types.
     *  @param minuend number to subtract from
     *  @param subtrahend number to subtract from minuend
     *  @param difference difference, if subtraction is safe, otherwise the
     *    parameter value is not touched by the function
     *  @return OFTrue if subtraction is safe and could be performed, OFFalse
     *   otherwise
     */
    template <typename T>
    static OFBool
    safeSubtract(T minuend, T subtrahend, T& difference)
    {
      assert(!OFnumeric_limits<T>::is_signed);
      if (minuend < subtrahend) {
        return OFFalse;
      } else {
        difference = minuend - subtrahend;
        return OFTrue;
      }
    }

    /** check whether addition is safe (i.e.\ no overflow occurs) and if so,
     *  perform it (i.e.\ compute a+b=sum). Only works for unsigned types.
     *  @param a first number to add
     *  @param b second number to add
     *  @param sum resulting sum of both numbers, if addition is safe, otherwise
     *    parameter value is not touched by the function
     *  @return OFTrue if addition is safe and could be performed, OFFalse
     *    otherwise
     */
    template <typename T>
    static OFBool
    safeAdd(T a, T b, T& sum)
    {
      assert(!OFnumeric_limits<T>::is_signed);
      if ((OFnumeric_limits<T>::max)() - a < b) {
        return OFFalse;
      } else {
        sum = a + b;
        return OFTrue;
      }
    }

    /** Thread-safe version of gethostbyname.
     *  @param name the host name.
     *  @return a OFStandard::OFHostent object.
     */
    static OFHostent getHostByName( const char* name );

    /** Thread-safe version of gethostbyaddr.
     *  @param addr see manpage.
     *  @param len see manpage.
     *  @param type see manpage.
     *  @return a OFStandard::OFHostent object.
     */
    static OFHostent getHostByAddr( const char* addr, int len, int type );

    /** Thread-safe version of getgrnam.
     *  @param name the group name.
     *  @return a OFStandard::OFGroup object.
     */
    static OFGroup getGrNam( const char* name );

    /** Thread-safe version of getpwnam.
     *  @param name the username.
     *  @return a OFStandard::OFPasswd object.
     */
    static OFPasswd getPwNam( const char* name );

    /** On Posix-like platform, this method executes setuid(getuid()),
     *  which causes the application to revert from root privileges to
     *  those of the calling user when the program is installed as
     *  setuid root. DCMTK command line tools that open a socket for
     *  incoming DICOM network connections will call this method immediately
     *  after opening the socket. Since DICOM by default operates on
     *  port 104, which on Posix platforms requires root privileges to open,
     *  this ensures that the socket can be opened, yet operation continues
     *  with the (hopefully) limited rights of the calling user.
     *  On non-Posix platforms, this method does nothing and returns success.
     *
     *  @return success or failure. This method can fail if the kernel has
     *    been configured to only permit a certain number of processes
     *    to be created for each user, and the calling user already has the
     *    maximum number of processes running. In this case, the application
     *    should terminate since otherwise it would continue to run with
     *    full root privileges.
     */
    static OFCondition dropPrivileges();

    /** Retrieve the name of the user that started the current process.
     *  @return the user name as an OFString value.
     */
    static OFString getUserName();

    /** Retrieve the local domain name, e. g. 'localhost'.
     *  @return the host name as an OFString value.
     */
    static OFString getHostName();

 private:

    /** private implementation of strlcpy. Called when strlcpy
     *  is not available in the standard library.
     *  @param dst destination buffer of size siz, must not be NULL
     *  @param src source string, must not be NULL
     *  @param siz size of destination buffer
     *  @return the total length of the string the function tried to
     *    create, i.e. strlen(src)
     */
    static size_t my_strlcpy(char *dst, const char *src, size_t siz);

    /** private implementation of strlcat. Called when strlcat
     *  is not available in the standard library.
     *  @param dst destination buffer of size siz, must not be NULL
     *  @param src source string, must not be NULL
     *  @param siz size of destination buffer
     *  @return the total length of the string the function tried to
     *    create, i.e. the initial length of dst plus the length of src
     */
    static size_t my_strlcat(char *dst, const char *src, size_t siz);

    /** makes the current process sleep until seconds seconds have
     *  elapsed or a signal arrives which is not ignored
     *  @param seconds number of seconds to sleep
     *  @return zero if the requested time has elapsed, or the number of seconds left to sleep
     */
    static unsigned int my_sleep(unsigned int seconds);
};

/** simple but thread safe random number generator. The interface is derived
 *  from the Posix rand_r function. Uses a multiplicative congruential
 *  random-number generator with period 2**32 that returns successive
 *  pseudo-random numbers in the range of 0 to OFrandr_max (0x7fffffff).
 *  @param seed pointer to seed of random number generator, must not be NULL.
 *  @return pseudo-random number in the range of 0 to OFrandr_max.
 */
int DCMTK_OFSTD_EXPORT OFrand_r(unsigned int &seed);

/// maximum value that can be returned by OFrand_r()
extern DCMTK_OFSTD_EXPORT const unsigned int OFrandr_max;

#endif
