/*
 *
 *  Copyright (C) 2001-2010, OFFIS e.V.
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
 *  As an exception of the above notice, the code for OFStandard::strlcpy
 *  and OFStandard::strlcat in this file have been derived from the BSD
 *  implementation which carries the following copyright notice:
 *
 *  Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 *  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 *  THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 *  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 *  The code for OFStandard::atof has been derived from an implementation
 *  which carries the following copyright notice:
 *
 *  Copyright 1988 Regents of the University of California
 *  Permission to use, copy, modify, and distribute this software and
 *  its documentation for any purpose and without fee is hereby granted,
 *  provided that the above copyright notice appear in all copies.  The
 *  University of California makes no representations about the
 *  suitability of this software for any purpose.  It is provided "as
 *  is" without express or implied warranty.
 *
 *
 *  The code for OFStandard::ftoa has been derived from an implementation
 *  which carries the following copyright notice:
 *
 *  Copyright (c) 1988 Regents of the University of California.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Redistribution and use in source and binary forms are permitted
 *  provided that the above copyright notice and this paragraph are
 *  duplicated in all such forms and that any documentation,
 *  advertising materials, and other materials related to such
 *  distribution and use acknowledge that the software was developed
 *  by the University of California, Berkeley.  The name of the
 *  University may not be used to endorse or promote products derived
 *  from this software without specific prior written permission.
 *  THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 *  WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  The "Base64" encoder/decoder has been derived from an implementation
 *  with the following copyright notice:
 *
 *  Copyright (c) 1999, Bob Withers - bwit@pobox.com
 *
 *  This code may be freely used for any purpose, either personal or
 *  commercial, provided the authors copyright notice remains intact.
 *
 *
 *  Module: ofstd
 *
 *  Author: Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose: Class for various helper functions
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-01 08:55:56 $
 *  CVS/RCS Revision: $Revision: 1.66 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/ofstream.h"

#define INCLUDE_CMATH
#define INCLUDE_CFLOAT
#define INCLUDE_CSTRING
#define INCLUDE_CSTDIO
#define INCLUDE_CCTYPE
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>    /* for stat() */
#endif
#ifdef HAVE_IO_H
#include <io.h>          /* for access() on Win32 */
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>   /* for opendir() and closedir() */
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>      /* for opendir() and closedir() */
#else
#define dirent direct
#ifdef HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#ifdef HAVE_NDIR_H
#include <ndir.h>
#endif
#endif
#ifdef HAVE_FNMATCH_H
#include <fnmatch.h>     /* for fnmatch() */
#endif
#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>     /* for finite() on Solaris 2.5.1 */
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#include <windows.h>     /* for GetFileAttributes() */

#ifndef R_OK /* windows defines access() but not the constants */
#define W_OK 02 /* Write permission */
#define R_OK 04 /* Read permission */
#define F_OK 00 /* Existance only */
#endif /* R_OK */

#endif /* HAVE_WINDOWS_H */

#ifdef _WIN32
#include <process.h>     /* needed for declaration of getpid() */
#endif

// --- ftoa() processing flags ---

const unsigned int OFStandard::ftoa_format_e  = 0x01;
const unsigned int OFStandard::ftoa_format_f  = 0x02;
const unsigned int OFStandard::ftoa_uppercase = 0x04;
const unsigned int OFStandard::ftoa_alternate = 0x08;
const unsigned int OFStandard::ftoa_leftadj   = 0x10;
const unsigned int OFStandard::ftoa_zeropad   = 0x20;


/* Some MacOS X versions define isinf() and isnan() in <math.h> but not in <cmath> */
#if defined(__APPLE__) && defined(__MACH__)
#undef HAVE_PROTOTYPE_ISINF
#undef HAVE_PROTOTYPE_ISNAN
#endif


// some systems don't properly define isnan()
#ifdef HAVE_ISNAN
#ifndef HAVE_PROTOTYPE_ISNAN
extern "C"
{
  int isnan(double value);
}
#endif
#endif


// some systems don't properly define finite()
#ifdef HAVE_FINITE
#ifndef HAVE_PROTOTYPE_FINITE
extern "C"
{
  int finite(double value);
}
#endif
#endif


// some systems don't properly define isinf()
#ifdef HAVE_ISINF
#ifndef HAVE_PROTOTYPE_ISINF
extern "C"
{
  int isinf(double value);
}
#endif

#else /* HAVE_ISINF */

static int my_isinf(double x)
{
#ifdef HAVE_WINDOWS_H
  return (! _finite(x)) && (! _isnan(x));
#else
  // Solaris 2.5.1 has finite() and isnan() but not isinf().
  return (! finite(x)) && (! isnan(x));
#endif
}
#endif /* HAVE_ISINF */


// --- string functions ---

#ifndef HAVE_STRLCPY
/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t OFStandard::my_strlcpy(char *dst, const char *src, size_t siz)
{
  register char *d = dst;
  register const char *s = src;
  register size_t n = siz;

  /* Copy as many bytes as will fit */
  if (n != 0 && --n != 0)
  {
    do
    {
      if ((*d++ = *s++) == 0)
         break;
    } while (--n != 0);
  }

  /* Not enough room in dst, add NUL and traverse rest of src */
  if (n == 0)
  {
     if (siz != 0)
        *d = '\0'; /* NUL-terminate dst */
     while (*s++) /* do_nothing */ ;
  }

  return(s - src - 1);    /* count does not include NUL */
}
#endif /* HAVE_STRLCPY */


#ifndef HAVE_STRLCAT
/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(src) + MIN(siz, strlen(initial dst)).
 * If retval >= siz, truncation occurred.
 */
size_t OFStandard::my_strlcat(char *dst, const char *src, size_t siz)
{
  register char *d = dst;
  register const char *s = src;
  register size_t n = siz;
  size_t dlen;

  /* Find the end of dst and adjust bytes left but don't go past end */
  while (n-- != 0 && *d != '\0') d++;
  dlen = d - dst;
  n = siz - dlen;

  if (n == 0) return(dlen + strlen(s));
  while (*s != '\0')
  {
    if (n != 1)
    {
      *d++ = *s;
      n--;
    }
    s++;
  }
  *d = '\0';

  return(dlen + (s - src));       /* count does not include NUL */
}
#endif /* HAVE_STRLCAT */


#ifdef HAVE_PROTOTYPE_STRERROR_R
/*
 * convert a given error code to a string. This function wraps the various
 * approaches found on different systems. Internally, the standard function
 * strerror() or strerror_r() is used.
 */
const char *OFStandard::strerror(const int errnum,
                                 char *buf,
                                 const size_t buflen)
{
    const char *result = "";
    if ((buf != NULL) && (buflen > 0))
    {
        // be paranoid and initialize the buffer to empty string
        buf[0] = 0;
        // two incompatible interfaces for strerror_r with different return types exist
#ifdef HAVE_CHARP_STRERROR_R
        // we're using the GNU specific version that returns the result, which may
        // or may not be a pointer to buf
        result = strerror_r(errnum, buf, buflen);
#else
        // we're using the X/OPEN version that always stores the result in buf
        (void) strerror_r(errnum, buf, buflen);
        result = buf;
#endif
    }
    return result;
}
#else
const char *OFStandard::strerror(const int errnum,
                                 char * /*buf*/,
                                 const size_t /*buflen*/)
{
    // we only have strerror() which is thread unsafe on Posix platforms, but thread safe on Windows
    return STDIO_NAMESPACE strerror(errnum);
}
#endif


OFString &OFStandard::toUpper(OFString &result,
                              const OFString &value)
{
    result = value;
    return OFStandard::toUpper(result);
}


OFString &OFStandard::toUpper(OFString &value)
{
    const size_t length = value.length();
    unsigned char c;
    for (size_t i = 0; i < length; i++)
    {
        c = value.at(i);
        value.at(i) = OFstatic_cast(char, toupper(c));
    }
    return value;
}


OFString &OFStandard::toLower(OFString &result,
                              const OFString &value)
{
    result = value;
    return OFStandard::toLower(result);
}


OFString &OFStandard::toLower(OFString &value)
{
    const size_t length = value.length();
    unsigned char c;
    for (size_t i = 0; i < length; i++)
    {
        c = value.at(i);
        value.at(i) = OFstatic_cast(char, tolower(c));
    }
    return value;
}


// --- file system functions ---

OFBool OFStandard::pathExists(const OFString &pathName)
{
    OFBool result = OFFalse;
    /* check for valid path name */
    if (!pathName.empty())
    {
#if HAVE_ACCESS
        /* check whether path exists */
        result = (access(pathName.c_str(), F_OK) == 0);
#else
#ifdef HAVE_WINDOWS_H
        /* check whether path exists */
        result = (GetFileAttributes(pathName.c_str()) != 0xffffffff);
#else
#ifdef HAVE_SYS_STAT_H
        /* check existence with "stat()" */
        struct stat stat_buf;
        result = (stat(pathName.c_str(), &stat_buf) == 0);
#else
        /* try to open the given "file" (or directory) in read-only mode */
        FILE* filePtr = fopen(pathName.c_str(), "r");
        result = (filePtr != NULL);
        fclose(filePtr);
#endif /* HAVE_SYS_STAT_H */
#endif /* HAVE_WINDOWS_H */
#endif /* HAVE_ACCESS */
    }
    return result;
}


OFBool OFStandard::fileExists(const OFString &fileName)
{
    OFBool result = OFFalse;
    /* check for valid file name */
    if (!fileName.empty())
    {
#ifdef HAVE_WINDOWS_H
        /* get file attributes */
        DWORD fileAttr = GetFileAttributes(fileName.c_str());
        if (fileAttr != 0xffffffff)
        {
            /* check file type (not a directory?) */
            result = ((fileAttr & FILE_ATTRIBUTE_DIRECTORY) == 0);
        }
#else
        /* check whether path exists (but does not point to a directory) */
        result = pathExists(fileName) && !dirExists(fileName);
#endif /* HAVE_WINDOWS_H */
    }
    return result;
}


OFBool OFStandard::dirExists(const OFString &dirName)
{
    OFBool result = OFFalse;
    /* check for valid directory name */
    if (!dirName.empty())
    {
#ifdef HAVE_WINDOWS_H
        /* get file attributes of the directory */
        DWORD fileAttr = GetFileAttributes(dirName.c_str());
        if (fileAttr != 0xffffffff)
        {
            /* check file type (is a directory?) */
            result = ((fileAttr & FILE_ATTRIBUTE_DIRECTORY) != 0);
        }
#else
        /* try to open the given directory */
        DIR *dirPtr = opendir(dirName.c_str());
        if (dirPtr != NULL)
        {
            result = OFTrue;
            closedir(dirPtr);
        }
#endif /* HAVE_WINDOWS_H */
    }
    return result;
}


OFBool OFStandard::isReadable(const OFString &pathName)
{
#if HAVE_ACCESS
    return (access(pathName.c_str(), R_OK) == 0);
#else
    OFBool result = OFFalse;
    /* try to open the given "file" (or directory) in read-only mode */
    FILE* filePtr = fopen(pathName.c_str(), "r");
    result = (filePtr != NULL);
    fclose(filePtr);
    return result;
#endif /* HAVE_ACCESS */
}


OFBool OFStandard::isWriteable(const OFString &pathName)
{
#if HAVE_ACCESS
    return (access(pathName.c_str(), W_OK) == 0);
#else
    OFBool result = OFFalse;
    /* try to open the given "file" (or directory) in write mode */
    FILE* filePtr = fopen(pathName.c_str(), "w");
    result = (filePtr != NULL);
    fclose(filePtr);
    return result;
#endif /* HAVE_ACCESS */
}


OFString &OFStandard::getDirNameFromPath(OFString &result,
                                         const OFString &pathName,
                                         const OFBool assumeDirName)
{
    const size_t pos = pathName.find_last_of(PATH_SEPARATOR);
    /* path separator found? */
    if (pos == OFString_npos)
    {
        if (assumeDirName)
            result = pathName;
        else
            result.clear();
    } else
        result = pathName.substr(0, pos);
    return result;
}


OFString &OFStandard::getFilenameFromPath(OFString &result,
                                          const OFString &pathName,
                                          const OFBool assumeFilename)
{
    const size_t pos = pathName.find_last_of(PATH_SEPARATOR);
    /* path separator found? */
    if (pos == OFString_npos)
    {
        if (assumeFilename)
            result = pathName;
        else
            result.clear();
    } else
        result = pathName.substr(pos + 1);
    return result;
}


OFString &OFStandard::normalizeDirName(OFString &result,
                                       const OFString &dirName,
                                       const OFBool allowEmptyDirName)
{
    result = dirName;
    /* remove trailing path separators (keep it if appearing at the beginning of the string) */
    while ((result.length() > 1) && (result.at(result.length() - 1) == PATH_SEPARATOR))
        result.erase(result.length() - 1, 1);
    if (allowEmptyDirName)
    {
        /* avoid "." as a directory name, use empty string instead */
        if (result == ".")
            result.clear();
    } else {
        /* avoid empty directory name (use "." instead) */
        if (result.empty())
            result = ".";
    }
    return result;
}


OFString &OFStandard::combineDirAndFilename(OFString &result,
                                            const OFString &dirName,
                                            const OFString &fileName,
                                            const OFBool allowEmptyDirName)
{
    // # might use system function realpath() in the future to resolve paths including ".."
    // # or combinations of absolute paths in both 'dirName' and 'fileName'

    /* check whether 'fileName' contains absolute path */
    /* (this check also covers UNC syntax, e. g. "\\server\...") */
    if (!fileName.empty() && (fileName.at(0) == PATH_SEPARATOR))
    {
        result = fileName;
        return result;
    }
#ifdef HAVE_WINDOWS_H
    else if ((fileName.length() >= 3))
    {
        /* check for absolute path containing windows drive name, e. g. "c:\..." */
        char c = fileName.at(0);
        if (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')))
        {
            if (fileName.substr(1, 2) == ":\\")
            {
                result = fileName;
                return result;
            }
        }
    }
#endif
    /* we only get here, if we don't have an absolute directory in "fileName" */
    /* now normalize the directory name */
    normalizeDirName(result, dirName, allowEmptyDirName);
    /* check file name */
    if (!fileName.empty() && (fileName != "."))
    {
        /* add path separator (if required) ... */
        if (!result.empty() && (result.at(result.length() - 1) != PATH_SEPARATOR))
            result += PATH_SEPARATOR;
        /* ...and file name */
        result += fileName;
    }
    return result;
}


OFCondition OFStandard::removeRootDirFromPathname(OFString &result,
                                                  const OFString &rootDir,
                                                  const OFString &pathName,
                                                  const OFBool allowLeadingPathSeparator)
{
    OFCondition status = EC_IllegalParameter;
    const size_t rootLength = rootDir.length();
    /* check for "compatible" length */
    if (rootLength <= pathName.length())
    {
        /* check for same prefix */
        if (pathName.compare(0, rootLength, rootDir) == 0)
        {
            /* remove root dir prefix from path name */
            result = pathName.substr(rootLength);
            if (!allowLeadingPathSeparator)
            {
                /* remove leading path separator (if present) */
                if (!result.empty() && (result.at(0) == PATH_SEPARATOR))
                    result.erase(0, 1);
            }
            status = EC_Normal;
        }
    }
    /* return empty string in case of error */
    if (status.bad())
        result = "";
    return status;
}


size_t OFStandard::searchDirectoryRecursively(const OFString &directory,
                                              OFList<OFString> &fileList,
                                              const OFString &pattern,
                                              const OFString &dirPrefix,
                                              const OFBool recurse)
{
    const size_t initialSize = fileList.size();
    OFString dirname, pathname, tmpString;
    combineDirAndFilename(dirname, dirPrefix, directory);
#ifdef HAVE_WINDOWS_H
    /* check whether given directory exists */
    if (dirExists(dirname))
    {
        HANDLE handle;
        WIN32_FIND_DATA data;
        /* check whether file pattern is given */
        if (!pattern.empty())
        {
            /* first, search for matching files on this directory level */
            handle = FindFirstFile(combineDirAndFilename(tmpString, dirname, pattern, OFTrue /*allowEmptyDirName*/).c_str(), &data);
            if (handle != INVALID_HANDLE_VALUE)
            {
                do {
                    /* avoid leading "." */
                    if (dirname == ".")
                        pathname = data.cFileName;
                    else
                        combineDirAndFilename(pathname, directory, data.cFileName, OFTrue /*allowEmptyDirName*/);
                    /* ignore directories and the like */
                    if (fileExists(combineDirAndFilename(tmpString, dirPrefix, pathname, OFTrue /*allowEmptyDirName*/)))
                        fileList.push_back(pathname);
                } while (FindNextFile(handle, &data));
                FindClose(handle);
            }
        }
        /* then search for _any_ file/directory entry */
        handle = FindFirstFile(combineDirAndFilename(tmpString, dirname, "*.*", OFTrue /*allowEmptyDirName*/).c_str(), &data);
        if (handle != INVALID_HANDLE_VALUE)
        {
            do {
                /* filter out current and parent directory */
                if ((strcmp(data.cFileName, ".") != 0) && (strcmp(data.cFileName, "..") != 0))
                {
                    /* avoid leading "." */
                    if (dirname == ".")
                        pathname = data.cFileName;
                    else
                        combineDirAndFilename(pathname, directory, data.cFileName, OFTrue /*allowEmptyDirName*/);
                    if (dirExists(combineDirAndFilename(tmpString, dirPrefix, pathname, OFTrue /*allowEmptyDirName*/)))
                    {
                        /* recursively search sub directories */
                        if (recurse)
                            searchDirectoryRecursively(pathname, fileList, pattern, dirPrefix, recurse);
                    }
                    else if (pattern.empty())
                    {
                        /* add filename to the list (if no pattern is given) */
                        fileList.push_back(pathname);
                    }
                }
            } while (FindNextFile(handle, &data));
            FindClose(handle);
        }
    }
#else
    /* try to open the directory */
    DIR *dirPtr = opendir(dirname.c_str());
    if (dirPtr != NULL)
    {
        struct dirent *entry = NULL;
        while ((entry = readdir(dirPtr)) != NULL)
        {
            /* filter out current and parent directory */
            if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
            {
                /* avoid leading "." */
                if (dirname == ".")
                    pathname = entry->d_name;
                else
                    combineDirAndFilename(pathname, directory, entry->d_name, OFTrue /*allowEmptyDirName*/);
                if (dirExists(combineDirAndFilename(tmpString, dirPrefix, pathname, OFTrue /*allowEmptyDirName*/)))
                {
                    /* recursively search sub directories */
                    if (recurse)
                        searchDirectoryRecursively(pathname, fileList, pattern, dirPrefix, recurse);
                } else {
#ifdef HAVE_FNMATCH_H
                    /* check whether filename matches pattern */
                    if ((pattern.empty()) || (fnmatch(pattern.c_str(), entry->d_name, FNM_PATHNAME) == 0))
#else
                        /* no pattern matching, sorry :-/ */
#endif
                        fileList.push_back(pathname);
                }
            }
        }
        closedir(dirPtr);
    }
#endif
    /* return number of added files */
    return fileList.size() - initialSize;
}


OFBool OFStandard::deleteFile(const OFString &filename)
{
  int err = unlink(filename.c_str());
  return (err == 0);
}


size_t OFStandard::getFileSize(const OFString &filename)
{
    size_t fileSize = 0;
    struct stat fileStat;
    if (stat(filename.c_str(), &fileStat) == 0)
        fileSize = OFstatic_cast(size_t, fileStat.st_size);
    return fileSize;
}


OFBool OFStandard::checkForMarkupConversion(const OFString &sourceString,
                                            const OFBool convertNonASCII)
{
    OFBool result = OFFalse;
    /* char pointer allows faster access to the string */
    const char *str = sourceString.c_str();
    unsigned char c;
    /* check for characters to be converted */
    do {
        c = OFstatic_cast(unsigned char, *(str++));
        if ((c == '<') || (c == '>') || (c == '&') || (c == '"') || (c == '\'') ||
            (c == 10) || (c == 13) || (convertNonASCII && ((c < 32) || (c >= 127))))
        {
            result = OFTrue;
            c = 0;
        }
    } while (c != 0);
    return result;
}


OFCondition OFStandard::convertToMarkupStream(STD_NAMESPACE ostream &out,
                                              const OFString &sourceString,
                                              const OFBool convertNonASCII,
                                              const E_MarkupMode markupMode,
                                              const OFBool newlineAllowed)
{
    /* char pointer allows faster access to the string */
    const char *str = sourceString.c_str();
    /* replace HTML/XHTML/XML reserved characters */
    while (*str != 0)
    {
        /* less than */
        if (*str == '<')
            out << "&lt;";
        /* greater than */
        else if (*str == '>')
            out << "&gt;";
        /* ampers and */
        else if (*str == '&')
            out << "&amp;";
        /* quotation mark */
        else if (*str == '"')
        {
            /* entity "&quot;" is not defined in HTML 3.2 */
            if (markupMode == MM_HTML32)
                out << "&#34;";
            else
                out << "&quot;";
        }
        /* apostrophe */
        else if (*str == '\'')
        {
            /* entity "&apos;" is not defined in HTML */
            if ((markupMode == MM_HTML) || (markupMode == MM_HTML32))
                out << "&#39;";
            else
                out << "&apos;";
        }
        /* newline: LF, CR, LF CR, CR LF */
        else if ((*str == '\012') || (*str == '\015'))
        {
            if (markupMode == MM_XML)
            {
                /* encode CR and LF exactly as specified */
                if (*str == '\012')
                    out << "&#10;";    // '\n'
                else
                    out << "&#13;";    // '\r'
            } else {  /* HTML/XHTML mode */
                /* skip next character if it belongs to the newline sequence */
                if (((*str == '\012') && (*(str + 1) == '\015')) || ((*str == '\015') && (*(str + 1) == '\012')))
                    str++;
                if (newlineAllowed)
                {
                    if (markupMode == MM_XHTML)
                        out << "<br />\n";
                    else
                        out << "<br>\n";
                } else
                    out << "&para;";
            }
        } else {
            /* other character: ... */
            const size_t charValue = OFstatic_cast(unsigned char, *str);
            if ((convertNonASCII || (markupMode == MM_HTML32)) && ((charValue < 32) || (charValue >= 127)))
            {
                /* convert < #32 and >= #127 to Unicode (ISO Latin-1) */
                out << "&#" << charValue << ";";
            } else {
                /* just append */
                out << *str;
            }
        }
        ++str;
    }
    return EC_Normal;
}


const OFString &OFStandard::convertToMarkupString(const OFString &sourceString,
                                                  OFString &markupString,
                                                  const OFBool convertNonASCII,
                                                  const E_MarkupMode markupMode,
                                                  const OFBool newlineAllowed)
{
    OFStringStream stream;
    /* call stream variant of convert to markup */
    if (OFStandard::convertToMarkupStream(stream, sourceString, convertNonASCII, markupMode, newlineAllowed).good())
    {
        stream << OFStringStream_ends;
        /* convert string stream into a character string */
        OFSTRINGSTREAM_GETSTR(stream, buffer_str)
        markupString.assign(buffer_str);
        OFSTRINGSTREAM_FREESTR(buffer_str)
    } else
        markupString.clear();
    return markupString;
}


// Base64 translation table as described in RFC 2045 (MIME)
static const char enc_base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

OFCondition OFStandard::encodeBase64(STD_NAMESPACE ostream &out,
                                     const unsigned char *data,
                                     const size_t length,
                                     const size_t width)
{
    OFCondition status = EC_IllegalParameter;
    /* check data buffer to be encoded */
    if (data != NULL)
    {
        unsigned char c;
        size_t w = 0;
        /* iterate over all data elements */
        for (size_t i = 0; i < length; i++)
        {
            /* encode first 6 bits */
            out << enc_base64[(data[i] >> 2) & 0x3f];
            /* insert line break (if width > 0) */
            if (++w == width)
            {
                out << OFendl;
                w = 0;
            }
            /* encode remaining 2 bits of the first byte and 4 bits of the second byte */
            c = (data[i] << 4) & 0x3f;
            if (++i < length)
                c |= (data[i] >> 4) & 0x0f;
            out << enc_base64[c];
            /* insert line break (if width > 0) */
            if (++w == width)
            {
                out << OFendl;
                w = 0;
            }
            /* encode remaining 4 bits of the second byte and 2 bits of the third byte */
            if (i < length)
            {
                c = (data[i] << 2) & 0x3f;
                if (++i < length)
                    c |= (data[i] >> 6) & 0x03;
                out << enc_base64[c];
            } else {
                i++;
                /* append fill char */
                out << '=';
            }
            /* insert line break (if width > 0) */
            if (++w == width)
            {
                out << OFendl;
                w = 0;
            }
            /* encode remaining 6 bits of the third byte */
            if (i < length)
                out << enc_base64[data[i] & 0x3f];
            else /* append fill char */
                out << '=';
            /* insert line break (if width > 0) */
            if (++w == width)
            {
                out << OFendl;
                w = 0;
            }
        }
        /* flush stream */
        out.flush();
        status = EC_Normal;
    }
    return status;
}


const OFString &OFStandard::encodeBase64(const unsigned char *data,
                                         const size_t length,
                                         OFString &result,
                                         const size_t width)
{
    OFStringStream stream;
    /* call stream variant of base64 encoder */
    if (OFStandard::encodeBase64(stream, data, length, width).good())
    {
        stream << OFStringStream_ends;
        /* convert string stream into a character string */
        OFSTRINGSTREAM_GETSTR(stream, buffer_str)
        result.assign(buffer_str);
        OFSTRINGSTREAM_FREESTR(buffer_str)
    } else
        result.clear();
    return result;
}


// Base64 decoding table: maps #43..#122 to #0..#63 (255 means invalid)
static const unsigned char dec_base64[] =
  { 62, 255, 255, 255, 63,                                                                                  // '+' .. '/'
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61,                                                                 // '0' .. '9'
    255, 255, 255, 255, 255, 255, 255,                                                                      // ':' .. '@'
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,           // 'A' .. 'Z'
    255, 255, 255, 255, 255, 255,                                                                           // '[' .. '`'
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51  // 'a' .. 'z'
  };

size_t OFStandard::decodeBase64(const OFString &data,
                                unsigned char *&result)
{
    size_t count = 0;
    /* search for fill char to determine the real length of the input string */
    const size_t fillPos = data.find('=');
    const size_t length = (fillPos != OFString_npos) ? fillPos : data.length();
    /* check data buffer to be decoded */
    if (length > 0)
    {
        /* allocate sufficient memory for the decoded data */
        result = new unsigned char[((length + 3) / 4) * 3];
        if (result != NULL)
        {
            unsigned char c1 = 0;
            unsigned char c2 = 0;
            /* iterate over all data elements */
            for (size_t i = 0; i < length; i++)
            {
                /* skip invalid characters and assign first decoded char */
                while ((i < length) && ((data.at(i) < '+') || (data.at(i) > 'z') || ((c1 = dec_base64[data.at(i) - '+']) > 63)))
                    i++;
                if (++i < length)
                {
                    /* skip invalid characters and assign second decoded char */
                    while ((i < length) && ((data.at(i) < '+') || (data.at(i) > 'z') || ((c2 = dec_base64[data.at(i) - '+']) > 63)))
                        i++;
                    if (i < length)
                    {
                        /* decode first byte */
                        result[count++] = OFstatic_cast(unsigned char, (c1 << 2) | ((c2 >> 4) & 0x3));
                        if (++i < length)
                        {
                            /* skip invalid characters and assign third decoded char */
                            while ((i < length) && ((data.at(i) < '+') || (data.at(i) > 'z') || ((c1 = dec_base64[data.at(i) - '+']) > 63)))
                                i++;
                            if (i < length)
                            {
                                /* decode second byte */
                                result[count++] = OFstatic_cast(unsigned char, ((c2 << 4) & 0xf0) | ((c1 >> 2) & 0xf));
                                if (++i < length)
                                {
                                    /* skip invalid characters and assign fourth decoded char */
                                    while ((i < length) && ((data.at(i) < '+') || (data.at(i) > 'z') || ((c2 = dec_base64[data.at(i) - '+']) > 63)))
                                        i++;
                                    /* decode third byte */
                                    if (i < length)
                                        result[count++] = OFstatic_cast(unsigned char, ((c1 << 6) & 0xc0) | c2);
                                }
                            }
                        }
                    }
                }
            }
            /* delete buffer if no data has been written to the output */
            if (count == 0)
                delete[] result;
        }
    } else
        result = NULL;
    return count;
}

#ifdef DISABLE_OFSTD_ATOF

// we use sscanf instead of atof because atof doesn't return a status flag

double OFStandard::atof(const char *s, OFBool *success)
{
  double result;
  if (success)
  {
    *success = (1 == sscanf(s,"%lf",&result));
  }
  else
  {
    (void) sscanf(s,"%lf",&result);
  }
  return result;
}

#else

// --- definitions and constants for atof() ---

/* Largest possible base 10 exponent.  Any exponent larger than this will
 * already produce underflow or overflow, so there's no need to worry
 * about additional digits.
 */
#define ATOF_MAXEXPONENT 511

/* Table giving binary powers of 10.  Entry is 10^2^i.
 * Used to convert decimal exponents into floating-point numbers.
 */
static const double atof_powersOf10[] =
{
    10.,
    100.,
    1.0e4,
    1.0e8,
    1.0e16,
    1.0e32,
    1.0e64,
    1.0e128,
    1.0e256
};

double OFStandard::atof(const char *s, OFBool *success)
{
    if (success) *success = OFFalse;
    register const char *p = s;
    register char c;
    int sign = 0;
    int expSign = 0;
    double fraction;
    int exponent = 0; // Exponent read from "EX" field.
    const char *pExp; // Temporarily holds location of exponent in string.

    /* Exponent that derives from the fractional part.  Under normal
     * circumstatnces, it is the negative of the number of digits in F.
     * However, if I is very long, the last digits of I get dropped
     * (otherwise a long I with a large negative exponent could cause an
     * unnecessary overflow on I alone).  In this case, fracExp is
     * incremented one for each dropped digit.
     */
    int fracExp = 0;

    // Strip off leading blanks and check for a sign.
    while (isspace(OFstatic_cast(unsigned char, *p))) ++p;

    if (*p == '-')
    {
        sign = 1;
        ++p;
    }
    else
    {
        if (*p == '+') ++p;
    }

    // Count the number of digits in the mantissa (including the decimal
    // point), and also locate the decimal point.

    int decPt = -1; // Number of mantissa digits BEFORE decimal point.
    int mantSize;     // Number of digits in mantissa.
    for (mantSize = 0; ; ++mantSize)
    {
        c = *p;
        if (!isdigit(OFstatic_cast(unsigned char, c)))
        {
            if ((c != '.') || (decPt >= 0)) break;
            decPt = mantSize;
        }
        ++p;
    }

    /*
     * Now suck up the digits in the mantissa.  Use two integers to
     * collect 9 digits each (this is faster than using floating-point).
     * If the mantissa has more than 18 digits, ignore the extras, since
     * they can't affect the value anyway.
     */

    pExp = p;
    p -= mantSize;
    if (decPt < 0)
      decPt = mantSize;
      else mantSize -= 1; // One of the digits was the point

    if (mantSize > 18)
    {
        fracExp = decPt - 18;
        mantSize = 18;
    }
    else
    {
        fracExp = decPt - mantSize;
    }

    if (mantSize == 0)
    {
      // subject sequence does not have expected form.
      // return 0 and leave success flag set to false
      return 0.0;
    }
    else
    {
        int frac1 = 0;
        for ( ; mantSize > 9; mantSize -= 1)
        {
            c = *p;
            ++p;
            if (c == '.')
            {
                c = *p;
                ++p;
            }
            frac1 = 10*frac1 + (c - '0');
        }
        int frac2 = 0;
        for (; mantSize > 0; mantSize -= 1)
        {
            c = *p;
            ++p;
            if (c == '.')
            {
                c = *p;
                ++p;
            }
            frac2 = 10*frac2 + (c - '0');
        }
        fraction = (1.0e9 * frac1) + frac2;
    }

    // Skim off the exponent.
    p = pExp;
    if ((*p == 'E') || (*p == 'e'))
    {
        ++p;
        if (*p == '-')
        {
            expSign = 1;
            ++p;
        }
        else
        {
            if (*p == '+') ++p;
            expSign = 0;
        }
        while (isdigit(OFstatic_cast(unsigned char, *p)))
        {
            exponent = exponent * 10 + (*p - '0');
            ++p;
        }
    }

    if (expSign)
       exponent = fracExp - exponent;
       else exponent = fracExp + exponent;

    /*
     * Generate a floating-point number that represents the exponent.
     * Do this by processing the exponent one bit at a time to combine
     * many powers of 2 of 10. Then combine the exponent with the
     * fraction.
     */

    if (exponent < 0)
    {
        expSign = 1;
        exponent = -exponent;
    }
    else expSign = 0;

    if (exponent > ATOF_MAXEXPONENT) exponent = ATOF_MAXEXPONENT;
    double dblExp = 1.0;
    for (const double *d = atof_powersOf10; exponent != 0; exponent >>= 1, ++d)
    {
        if (exponent & 01) dblExp *= *d;
    }

    if (expSign)
      fraction /= dblExp;
      else fraction *= dblExp;

    if (success) *success = OFTrue;
    if (sign) return -fraction;
    return fraction;
}

#endif /* DISABLE_OFSTD_ATOF */


/* 11-bit exponent (VAX G floating point) is 308 decimal digits */
#define FTOA_MAXEXP          308
/* 128 bit fraction takes up 39 decimal digits; max reasonable precision */
#define FTOA_MAXFRACT        39
/* default precision */
#define FTOA_DEFPREC         6
/* internal buffer size for ftoa code */
#define FTOA_BUFSIZE         (FTOA_MAXEXP+FTOA_MAXFRACT+1)

#define FTOA_TODIGIT(c)      ((c) - '0')
#define FTOA_TOCHAR(n)       ((n) + '0')

#define FTOA_FORMAT_MASK 0x03 /* and mask for format flags */
#define FTOA_FORMAT_E         OFStandard::ftoa_format_e
#define FTOA_FORMAT_F         OFStandard::ftoa_format_f
#define FTOA_FORMAT_UPPERCASE OFStandard::ftoa_uppercase
#define FTOA_ALTERNATE_FORM   OFStandard::ftoa_alternate
#define FTOA_LEFT_ADJUSTMENT  OFStandard::ftoa_leftadj
#define FTOA_ZEROPAD          OFStandard::ftoa_zeropad

#ifdef DISABLE_OFSTD_FTOA

void OFStandard::ftoa(
  char *dst,
  size_t siz,
  double val,
  unsigned int flags,
  int width,
  int prec)
{
  // this version of the function uses sprintf to format the output string.
  // Since we have to assemble the sprintf format string, this version might
  // even be slower than the alternative implementation.

  char buf[FTOA_BUFSIZE];
  OFString s("%"); // this will become the format string
  unsigned char fmtch = 'G';

  // check if val is NAN
#ifdef HAVE_WINDOWS_H
  if (_isnan(val))
#else
  if (isnan(val))
#endif
  {
    OFStandard::strlcpy(dst, "nan", siz);
    return;
  }

  // check if val is infinity
#ifdef HAVE_ISINF
  if (isinf(val))
#else
  if (my_isinf(val))
#endif
  {
    if (val < 0)
        OFStandard::strlcpy(dst, "-inf", siz);
        else OFStandard::strlcpy(dst, "inf", siz);
    return;
  }

  // determine format character
  if (flags & FTOA_FORMAT_UPPERCASE)
  {
    if ((flags & FTOA_FORMAT_MASK) == FTOA_FORMAT_E) fmtch = 'E';
    else if ((flags & FTOA_FORMAT_MASK) == FTOA_FORMAT_F) fmtch = 'f'; // there is no uppercase for 'f'
    else fmtch = 'G';
  }
  else
  {
    if ((flags & FTOA_FORMAT_MASK) == FTOA_FORMAT_E) fmtch = 'e';
    else if ((flags & FTOA_FORMAT_MASK) == FTOA_FORMAT_F) fmtch = 'f';
    else fmtch = 'g';
  }

  if (flags & FTOA_ALTERNATE_FORM) s += "#";
  if (flags & FTOA_LEFT_ADJUSTMENT) s += "-";
  if (flags & FTOA_ZEROPAD) s += "0";
  if (width > 0)
  {
    sprintf(buf, "%d", width);
    s += buf;
  }
  if (prec >= 0)
  {
    sprintf(buf, ".%d", prec);
    s += buf;
  }
  s += fmtch;

  sprintf(buf, s.c_str(), val);
  OFStandard::strlcpy(dst, buf, siz);
}

#else

/** internal helper class that maintains a string buffer
 *  to which characters can be written. If the string buffer
 *  gets full, additional characters are discarded.
 *  The string buffer does not guarantee zero termination.
 */
class FTOAStringBuffer
{
public:
  /** constructor
   *  @param theSize desired size of string buffer, in bytes
   */
  FTOAStringBuffer(unsigned long theSize)
  : buf_(NULL)
  , offset_(0)
  , size_(theSize)
  {
    if (size_ > 0) buf_ = new char[size_];
  }

  /// destructor
  ~FTOAStringBuffer()
  {
    delete[] buf_;
  }

  /** add one character to string buffer. Never overwrites
   *  buffer boundary.
   *  @param c character to add
   */
  inline void put(unsigned char c)
  {
    if (buf_ && (offset_ < size_)) buf_[offset_++] = c;
  }

  // return pointer to string buffer
  const char *getBuffer() const
  {
    return buf_;
  }

private:
  /// pointer to string buffer
  char *buf_;

  /// current offset within buffer
  unsigned long offset_;

  /// size of buffer
  unsigned long size_;

  /// private undefined copy constructor
  FTOAStringBuffer(const FTOAStringBuffer &old);

  /// private undefined assignment operator
  FTOAStringBuffer &operator=(const FTOAStringBuffer &obj);
};


/** writes the given format character and exponent to output string p.
 *  @param p pointer to target string
 *  @param exponent exponent to print
 *  @param fmtch format character
 *  @return pointer to next unused character in output string
 */
static char *ftoa_exponent(char *p, int exponent, char fmtch)
{
  char expbuf[FTOA_MAXEXP];

  *p++ = fmtch;
  if (exponent < 0)
  {
    exponent = -exponent;
    *p++ = '-';
  }
  else *p++ = '+';
  register char *t = expbuf + FTOA_MAXEXP;
  if (exponent > 9)
  {
    do
    {
      *--t = OFstatic_cast(char, FTOA_TOCHAR(exponent % 10));
    }
    while ((exponent /= 10) > 9);
    *--t = OFstatic_cast(char, FTOA_TOCHAR(exponent));
    for (; t < expbuf + FTOA_MAXEXP; *p++ = *t++) /* nothing */;
  }
  else
  {
    *p++ = '0';
    *p++ = OFstatic_cast(char, FTOA_TOCHAR(exponent));
  }

  return p;
}


/** round given fraction and adjust text string if round up.
 *  @param fract  fraction to round
 *  @param expon  pointer to exponent, may be NULL
 *  @param start  pointer to start of string to round
 *  @param end    pointer to one char after end of string
 *  @param ch     if fract is zero, this character is interpreted as fraction*10 instead
 *  @param signp  pointer to sign character, '-' or 0.
 *  @return adjusted pointer to start of rounded string, may be start or start-1.
 */
static char *ftoa_round(double fract, int *expon, char *start, char *end, char ch, char *signp)
{
  double tmp;

  if (fract) (void) modf(fract * 10, &tmp);
  else tmp = FTOA_TODIGIT(ch);

  if (tmp > 4)
  {
    for (;; --end)
    {
      if (*end == '.') --end;
      if (++*end <= '9') break;
      *end = '0';
      if (end == start)
      {
        if (expon) /* e/E; increment exponent */
        {
          *end = '1';
          ++*expon;
        }
        else /* f; add extra digit */
        {
          *--end = '1';
          --start;
        }
        break;
      }
    }
  }
  /* ``"%.3f", (double)-0.0004'' gives you a negative 0. */
  else if (*signp == '-')
  {
    for (;; --end)
    {
      if (*end == '.') --end;
      if (*end != '0') break;
      if (end == start) *signp = 0; // suppress negative 0
    }
  }

  return start;
}


/** convert double value to string, without padding
 *  @param val double value to be formatted
 *  @param prec    precision, adjusted for FTOA_MAXFRACT
 *  @param flags   formatting flags
 *  @param signp   pointer to sign character, '-' or 0.
 *  @param fmtch   format character
 *  @param startp  pointer to start of target buffer
 *  @param endp    pointer to one char after end of target buffer
 *  @return
 */
static int ftoa_convert(double val, int prec, int flags, char *signp, char fmtch, char *startp, char *endp)
{
  register char *p;
  register double fract;
  int dotrim = 0;
  int expcnt = 0;
  int gformat = 0;
  double integer, tmp;

  fract = modf(val, &integer);

  /* get an extra slot for rounding. */
  register char *t = ++startp;

  /*
   * get integer portion of val; put into the end of the buffer; the
   * .01 is added for modf(356.0 / 10, &integer) returning .59999999...
   */
  for (p = endp - 1; integer; ++expcnt)
  {
    tmp = modf(integer / 10, &integer);
    *p-- = OFstatic_cast(char, FTOA_TOCHAR(OFstatic_cast(int, (tmp + .01) * 10)));
  }

  switch(fmtch)
  {
    case 'f':
      /* reverse integer into beginning of buffer */
      if (expcnt)
      {
        for (; ++p < endp; *t++ = *p);
      }
      else *t++ = '0';

      /*
       * if precision required or alternate flag set, add in a
       * decimal point.
       */
      if (prec || flags & FTOA_ALTERNATE_FORM) *t++ = '.';

      /* if requires more precision and some fraction left */
      if (fract)
      {
        if (prec) do
        {
          fract = modf(fract * 10, &tmp);
          *t++ = OFstatic_cast(char, FTOA_TOCHAR(OFstatic_cast(int, tmp)));
        } while (--prec && fract);
        if (fract)
        {
          startp = ftoa_round(fract, OFstatic_cast(int *, NULL), startp, t - 1, OFstatic_cast(char, 0), signp);
        }
      }
      for (; prec--; *t++ = '0');
      break;

    case 'e':
    case 'E':
eformat:
      if (expcnt)
      {
        *t++ = *++p;
        if (prec || flags&FTOA_ALTERNATE_FORM)
                *t++ = '.';
        /* if requires more precision and some integer left */
        for (; prec && ++p < endp; --prec)
                *t++ = *p;
        /*
         * if done precision and more of the integer component,
         * round using it; adjust fract so we don't re-round
         * later.
         */
        if (!prec && ++p < endp)
        {
          fract = 0;
          startp = ftoa_round(OFstatic_cast(double, 0), &expcnt, startp, t - 1, *p, signp);
        }
        /* adjust expcnt for digit in front of decimal */
        --expcnt;
      }
      /* until first fractional digit, decrement exponent */
      else if (fract)
      {
        /* adjust expcnt for digit in front of decimal */
        for (expcnt = -1;; --expcnt) {
                fract = modf(fract * 10, &tmp);
                if (tmp)
                        break;
        }
        *t++ = OFstatic_cast(char, FTOA_TOCHAR(OFstatic_cast(int, tmp)));
        if (prec || flags&FTOA_ALTERNATE_FORM) *t++ = '.';
      }
      else
      {
        *t++ = '0';
        if (prec || flags&FTOA_ALTERNATE_FORM) *t++ = '.';
      }

      /* if requires more precision and some fraction left */
      if (fract)
      {
        if (prec) do
        {
          fract = modf(fract * 10, &tmp);
          *t++ = OFstatic_cast(char, FTOA_TOCHAR(OFstatic_cast(int, tmp)));
        } while (--prec && fract);
        if (fract)
        {
          startp = ftoa_round(fract, &expcnt, startp, t - 1, OFstatic_cast(char, 0), signp);
        }
      }

      /* if requires more precision */
      for (; prec--; *t++ = '0');

      /* unless alternate flag, trim any g/G format trailing 0's */
      if (gformat && !(flags&FTOA_ALTERNATE_FORM))
      {
        while (t > startp && *--t == '0') /* nothing */;
        if (*t == '.') --t;
        ++t;
      }
      t = ftoa_exponent(t, expcnt, fmtch);
      break;

    case 'g':
    case 'G':
      /* a precision of 0 is treated as a precision of 1. */
      if (!prec) ++prec;
      /*
       * ``The style used depends on the value converted; style e
       * will be used only if the exponent resulting from the
       * conversion is less than -4 or greater than the precision.''
       *      -- ANSI X3J11
       */
      if (expcnt > prec || (!expcnt && fract && fract < .0001))
      {
        /*
         * g/G format counts "significant digits, not digits of
         * precision; for the e/E format, this just causes an
         * off-by-one problem, i.e. g/G considers the digit
         * before the decimal point significant and e/E doesn't
         * count it as precision.
         */
        --prec;
        fmtch -= 2;             /* G->E, g->e */
        gformat = 1;
        goto eformat;
      }

      /*
       * reverse integer into beginning of buffer,
       * note, decrement precision
       */
      if (expcnt)
      {
        for (; ++p < endp; *t++ = *p, --prec);
      }
      else *t++ = '0';
      /*
       * if precision required or alternate flag set, add in a
       * decimal point.  If no digits yet, add in leading 0.
       */
      if (prec || flags&FTOA_ALTERNATE_FORM)
      {
        dotrim = 1;
        *t++ = '.';
      }
      else dotrim = 0;

      /* if requires more precision and some fraction left */
      if (fract)
      {
        if (prec)
        {
          do
          {
            fract = modf(fract * 10, &tmp);
            *t++ = OFstatic_cast(char, FTOA_TOCHAR(OFstatic_cast(int, tmp)));
          } while(!tmp);
          while (--prec && fract)
          {
            fract = modf(fract * 10, &tmp);
            *t++ = OFstatic_cast(char, FTOA_TOCHAR(OFstatic_cast(int, tmp)));
          }
        }
        if (fract)
        {
          startp = ftoa_round(fract, OFstatic_cast(int *, NULL), startp, t - 1, OFstatic_cast(char, 0), signp);
        }
      }
      /* alternate format, adds 0's for precision, else trim 0's */
      if (flags&FTOA_ALTERNATE_FORM) for (; prec--; *t++ = '0') /* nothing */;
      else if (dotrim)
      {
        while (t > startp && *--t == '0') /* nothing */;
        if (*t != '.') ++t;
      }
  } /* end switch */

  return (t - startp);
}

void OFStandard::ftoa(
  char *dst,
  size_t siz,
  double val,
  unsigned int flags,
  int width,
  int prec)
{
  // if target string is NULL or zero bytes long, bail out.
  if (!dst || !siz) return;

  // check if val is NAN
#ifdef HAVE_WINDOWS_H
  if (_isnan(val))
#else
  if (isnan(val))
#endif
  {
    OFStandard::strlcpy(dst, "nan", siz);
    return;
  }

  // check if val is infinity
#ifdef HAVE_ISINF
  if (isinf(val))
#else
  if (my_isinf(val))
#endif
  {
    if (val < 0)
        OFStandard::strlcpy(dst, "-inf", siz);
        else OFStandard::strlcpy(dst, "inf", siz);
    return;
  }

  int fpprec = 0;     /* `extra' floating precision in [eEfgG] */
  char softsign = 0;  /* temporary negative sign for floats */
  char buf[FTOA_BUFSIZE];      /* space for %c, %[diouxX], %[eEfgG] */
  char sign = '\0';   /* sign prefix (' ', '+', '-', or \0) */
  register int n;
  unsigned char fmtch = 'G';
  FTOAStringBuffer sb(FTOA_BUFSIZE+1);

  // determine format character
  if (flags & FTOA_FORMAT_UPPERCASE)
  {
    if ((flags & FTOA_FORMAT_MASK) == FTOA_FORMAT_E) fmtch = 'E';
    else if ((flags & FTOA_FORMAT_MASK) == FTOA_FORMAT_F) fmtch = 'f'; // there is no uppercase for 'f'
    else fmtch = 'G';
  }
  else
  {
    if ((flags & FTOA_FORMAT_MASK) == FTOA_FORMAT_E) fmtch = 'e';
    else if ((flags & FTOA_FORMAT_MASK) == FTOA_FORMAT_F) fmtch = 'f';
    else fmtch = 'g';
  }

  // don't do unrealistic precision; just pad it with zeroes later,
  // so buffer size stays rational.
  if (prec > FTOA_MAXFRACT)
  {
    if ((fmtch != 'g' && fmtch != 'G') || (flags&FTOA_ALTERNATE_FORM)) fpprec = prec - FTOA_MAXFRACT;
    prec = FTOA_MAXFRACT;
  }
  else if (prec == -1) prec = FTOA_DEFPREC;

  /*
   * softsign avoids negative 0 if val is < 0 and
   * no significant digits will be shown
   */
  if (val < 0)
  {
    softsign = '-';
    val = -val;
  }
  else softsign = 0;

  /*
   * ftoa_convert may have to round up past the "start" of the
   * buffer, i.e. ``intf("%.2f", (double)9.999);'';
   * if the first char isn't \0, it did.
   */
  *buf = 0;
  int size = ftoa_convert(val, prec, flags, &softsign, fmtch, buf, buf + sizeof(buf));
  if (softsign) sign = '-';
  register char *t = *buf ? buf : buf + 1;

  /* At this point, `t' points to a string which (if not flags&FTOA_LEFT_ADJUSTMENT)
   * should be padded out to `width' places.  If flags&FTOA_ZEROPAD, it should
   * first be prefixed by any sign or other prefix; otherwise, it should be
   * blank padded before the prefix is emitted.  After any left-hand
   * padding, print the string proper, then emit zeroes required by any
   * leftover floating precision; finally, if FTOA_LEFT_ADJUSTMENT, pad with blanks.
   *
   * compute actual size, so we know how much to pad
   */
  int fieldsz = size + fpprec;
  if (sign) fieldsz++;

  /* right-adjusting blank padding */
  if ((flags & (FTOA_LEFT_ADJUSTMENT|FTOA_ZEROPAD)) == 0 && width)
  {
    for (n = fieldsz; n < width; n++) sb.put(' ');
  }

  /* prefix */
  if (sign) sb.put(sign);

  /* right-adjusting zero padding */
  if ((flags & (FTOA_LEFT_ADJUSTMENT|FTOA_ZEROPAD)) == FTOA_ZEROPAD)
          for (n = fieldsz; n < width; n++)
                  sb.put('0');

  /* the string or number proper */
  n = size;
  while (--n >= 0) sb.put(*t++);

  /* trailing f.p. zeroes */
  while (--fpprec >= 0) sb.put('0');

  /* left-adjusting padding (always blank) */
  if (flags & FTOA_LEFT_ADJUSTMENT)
          for (n = fieldsz; n < width; n++)
                  sb.put(' ');

  /* zero-terminate string */
  sb.put(0);

  /* copy result from char buffer to output array */
  const char *c = sb.getBuffer();
  if (c) OFStandard::strlcpy(dst, c, siz); else *dst = 0;
}

#endif /* DISABLE_OFSTD_FTOA */


unsigned int OFStandard::my_sleep(unsigned int seconds)
{
#ifdef HAVE_WINDOWS_H
  // on Win32 we use the Sleep() system call which expects milliseconds
  Sleep(1000*seconds);
  return 0;
#elif defined(HAVE_SLEEP)
  // just use the original sleep() system call
  return sleep(seconds);
#elif defined(HAVE_USLEEP)
  // usleep() expects microseconds
  (void) usleep(((unsigned long)seconds)*1000000UL);
  return 0;
#else
  // don't know how to sleep
  return 0;
#endif
}

long OFStandard::getProcessID()
{
#ifdef _WIN32
  return _getpid();
#elif defined(HAVE_GETPID)
  return getpid();
#else
  return 0; // Workaround for MAC
#endif
}


/*
 *  $Log: ofstd.cc,v $
 *  Revision 1.66  2010-11-01 08:55:56  uli
 *  Moved variable declarations in front of their loop.
 *
 *  Revision 1.65  2010-10-20 08:09:34  uli
 *  Also made sure toupper() and tolower() get a valid argument.
 *
 *  Revision 1.64  2010-10-20 07:41:37  uli
 *  Made sure isalpha() & friends are only called with valid arguments.
 *
 *  Revision 1.63  2010-10-14 13:14:53  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.62  2010-08-19 08:43:09  uli
 *  Fixed a comparision that checks for a valid buffer.
 *
 *  Revision 1.61  2010-08-10 09:34:20  uli
 *  Fixed some unlikely problems with NULL pointers.
 *
 *  Revision 1.60  2010-06-02 12:56:54  joergr
 *  Introduced new helper function strerror() which is used as a wrapper to the
 *  various approaches found on different systems.
 *
 *  Revision 1.59  2010-06-02 12:36:48  joergr
 *  Appended missing OFStringStream_ends to the end of output streams because
 *  this is required when OFOStringStream is mapped to ostrstream.
 *
 *  Revision 1.58  2010-05-20 09:20:32  joergr
 *  Added new method for determining the size of a given file (in bytes).
 *
 *  Revision 1.57  2010-04-28 13:27:38  joergr
 *  Added type cast to integer variable in order to avoid compiler warnings
 *  reported by VisualStudio 2005.
 *  Fixed wrong use of comparison operator in if statement.
 *
 *  Revision 1.56  2010-01-21 14:38:38  joergr
 *  Added stream variant of method convertToMarkupString().
 *  Changed implementation of convertToMarkupString(). This helper function is
 *  now based on the new stream variant which is much more efficient when
 *  converting large text strings with many special characters.
 *
 *  Revision 1.55  2010-01-20 13:49:47  uli
 *  Added OFStandard::getProcessID().
 *
 *  Revision 1.54  2010-01-04 16:02:46  joergr
 *  Added new method getDirNameFromPath() and enhanced existing method
 *  getFilenameFromPath().
 *
 *  Revision 1.53  2009-09-15 10:48:37  joergr
 *  Changed implementation of string variant of encodeBase64(). This helper
 *  function is now based on the stream variant of encodeBase64() because the
 *  previous implementation had an issue with HAVE_STD_STRING defined.
 *
 *  Revision 1.52  2009-08-26 07:44:31  joergr
 *  Added parentheses around && within || in order to avoid warnings reported by
 *  gcc 4.3.2.
 *
 *  Revision 1.51  2009-08-19 10:40:54  joergr
 *  Added new string helper functions toUpper() and toLower().
 *
 *  Revision 1.50  2009-04-27 14:25:31  joergr
 *  Added comment on absolute path names e.g. in UNC syntax.
 *
 *  Revision 1.49  2009-03-13 09:46:40  joergr
 *  Added new helper function getFilenameFromPath().
 *
 *  Revision 1.48  2008-08-28 10:44:30  onken
 *  Introduced deleteFile() method.
 *
 *  Revision 1.47  2008-08-28 10:34:02  onken
 *  Added #ifdef to combineDirAndFilename() to only check for absolute paths with
 *  driver letter when on Windows.
 *
 *  Revision 1.46  2008-08-28 10:20:43  onken
 *  Modified combineDirAndFilename() to also recognize Windows absolute paths
 *  starting with a drive letter.
 *
 *  Revision 1.45  2008-07-15 09:49:38  joergr
 *  Removed unused function OFStandard::stringMatchesCharacterSet().
 *
 *  Revision 1.44  2008-04-28 12:02:49  joergr
 *  Adapted OFStandard::checkForMarkupConversion() to the new behavior of
 *  parameter "convertNonASCII" of OFStandard::convertToMarkupString().
 *
 *  Revision 1.43  2008-04-28 09:05:06  meichel
 *  OFStandard::convertToMarkupString now also converts control characters
 *    less than 32 and 127 (which does not exist in ASCII or ISO 8859)
 *    when run in "convert non-ASCII" or MM_HTML32 markup mode.
 *
 *  Revision 1.42  2008-04-18 09:11:29  joergr
 *  Added support for current directory (".") to searchDirectoryRecursively().
 *
 *  Revision 1.41  2008-03-10 12:02:57  joergr
 *  Added "ofstream.h" include in order to compile when HAVE_STL is defined.
 *
 *  Revision 1.40  2007/11/15 16:12:53  joergr
 *  Introduced new markup mode for convertToMarkupString() that is used to
 *  distinguish between HTML, HTML 3.2, XHTML and XML.
 *  Enhanced convertToMarkupString() in order to be more compliant to the HTML
 *  specification (e.g. do not use "&apos;").
 *
 *  Revision 1.39  2007/06/26 16:20:39  joergr
 *  Added new variant of encodeBase64() method that outputs directly to a stream
 *  (avoids using a memory buffer for large binary data).
 *
 *  Revision 1.38  2007/03/09 16:35:49  joergr
 *  Fixed issue with new parameter "recurse" in searchDirectoryRecursively().
 *
 *  Revision 1.37  2007/03/09 14:55:38  joergr
 *  Added optional parameter "recurse" to searchDirectoryRecursively().
 *
 *  Revision 1.36  2007/02/20 13:13:38  joergr
 *  Added function that removes a given prefix from a pathname (e.g. root dir).
 *
 *  Revision 1.35  2006/10/13 10:01:36  joergr
 *  Added new helper function that allows to check whether the conversion to an
 *  HTML/XML markup string is required.
 *
 *  Revision 1.34  2005/12/08 15:49:00  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.33  2004/08/04 12:11:52  joergr
 *  Replaced non-Unix newline characters.
 *
 *  Revision 1.32  2004/08/03 11:45:48  meichel
 *  Headers libc.h and unistd.h are now included via ofstdinc.h
 *
 *  Revision 1.31  2004/05/26 10:14:47  meichel
 *  Completed isinf() workaround for MacOS X
 *
 *  Revision 1.30  2004/05/07 11:25:38  meichel
 *  Added workaround for MacOS X where isinf() and isnan() are defined in <math.h>
 *    but not in <cmath>.
 *
 *  Revision 1.29  2004/05/03 17:19:50  meichel
 *  my_isinf() now also works on systems where finite() or isinf()
 *    are defined but not properly declared in <math.h> or <cmath>.
 *
 *  Revision 1.28  2004/04/30 15:52:33  meichel
 *  my_isinf() now also works on systems where finite() or isinf()
 *    are defined but not properly declared in <math.h> or <cmath>.
 *
 *  Revision 1.27  2004/04/16 12:47:53  joergr
 *  Renamed local function "isinf" to "my_isinf" to avoid possible conflicts.
 *
 *  Revision 1.26  2004/01/16 10:33:57  joergr
 *  Replaced OFString::resize() by ..reserve() in convertToMarkupString()
 *  because of STL problems with Metrowerks CodeWarrior 8.3 compiler.
 *
 *  Revision 1.25  2003/10/22 15:04:40  meichel
 *  Added private implementation of isinf on platforms that have finite()
 *    and isnan() but not isinf(), such as Solaris 2.5.1.
 *
 *  Revision 1.24  2003/08/12 13:11:10  joergr
 *  Improved implementation of normalizeDirName().
 *
 *  Revision 1.23  2003/08/07 11:43:12  joergr
 *  Improved implementation of combineDirAndFilename().
 *
 *  Revision 1.22  2003/07/17 14:57:34  joergr
 *  Added new function searchDirectoryRecursively().
 *
 *  Revision 1.21  2003/07/09 13:58:04  meichel
 *  Adapted type casts to new-style typecast operators defined in ofcast.h
 *
 *  Revision 1.20  2003/07/08 14:39:15  meichel
 *  Fixed bug in OFStandard::ftoa that could cause a segmentation fault
 *    if the number to be converted was NAN or infinity.
 *
 *  Revision 1.19  2003/07/03 14:23:51  meichel
 *  Minor changes to make OFStandard::sleep compile on MinGW
 *
 *  Revision 1.18  2003/06/06 09:44:01  meichel
 *  Added static sleep function in class OFStandard. This replaces the various
 *    calls to sleep(), Sleep() and usleep() throughout the toolkit.
 *
 *  Revision 1.17  2003/04/17 15:53:15  joergr
 *  Replace LF and CR by &#10; and &#13; in XML mode instead of &#182; (para).
 *  Enhanced performance of base64 encoder and decoder routines.
 *
 *  Revision 1.16  2003/03/21 13:10:42  meichel
 *  Minor code purifications for warnings reported by MSVC in Level 4
 *
 *  Revision 1.15  2003/03/12 14:57:51  joergr
 *  Added apostrophe (') to the list of characters to be replaced by the
 *  corresponding HTML/XML mnenonic.
 *
 *  Revision 1.14  2002/12/13 13:45:35  meichel
 *  Removed const from decodeBase64() return code, needed on MIPSpro
 *
 *  Revision 1.13  2002/12/09 13:10:46  joergr
 *  Renamed parameter/local variable to avoid name clash with global function
 *  exp().
 *  Added private undefined copy constructor and/or assignment operator.
 *
 *  Revision 1.12  2002/12/05 13:50:08  joergr
 *  Moved definition of ftoa() processing flags to implementation file to avoid
 *  compiler errors (e.g. on Sun CC 2.0.1).
 *
 *  Revision 1.11  2002/12/04 09:13:03  meichel
 *  Implemented a locale independent function OFStandard::ftoa() that
 *    converts double to string and offers all the flexibility of the
 *    sprintf family of functions.
 *
 *  Revision 1.10  2002/11/27 11:23:11  meichel
 *  Adapted module ofstd to use of new header file ofstdinc.h
 *
 *  Revision 1.9  2002/07/18 12:14:19  joergr
 *  Corrected typos.
 *
 *  Revision 1.8  2002/07/02 15:18:24  wilkens
 *  Added function OFStandard::stringMatchesCharacterSet(...).
 *
 *  Revision 1.7  2002/06/20 12:06:47  meichel
 *  Fixed typo in ofstd.cc
 *
 *  Revision 1.6  2002/06/20 12:02:39  meichel
 *  Implemented a locale independent function OFStandard::atof() that
 *    converts strings to double and optionally returns a status code
 *
 *  Revision 1.5  2002/05/16 15:56:20  meichel
 *  Minor fixes to make ofstd compile on NeXTStep 3.3
 *
 *  Revision 1.4  2002/05/14 08:13:27  joergr
 *  Added support for Base64 (MIME) encoding and decoding.
 *
 *  Revision 1.3  2002/04/25 09:13:55  joergr
 *  Moved helper function which converts a conventional character string to an
 *  HTML/XML mnenonic string (e.g. using "&lt;" instead of "<") from module
 *  dcmsr to ofstd.
 *
 *  Revision 1.2  2002/04/11 12:08:06  joergr
 *  Added general purpose routines to check whether a file exists, a path points
 *  to a directory or a file, etc.
 *
 *  Revision 1.1  2001/12/04 16:57:18  meichel
 *  Implemented strlcpy and strlcat routines compatible with the
 *    corresponding BSD libc routines in class OFStandard
 *
 */
