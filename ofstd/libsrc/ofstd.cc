/*
 *
 *  Copyright (C) 1997-2002, OFFIS
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
 *  As an exception of the above notice, the code for OFStandard::strlcpy
 *  and OFStandard::strlcat in this file have been derived from the BSD
 *  implementation which carries the following copyright notice:
 *
 *  Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 *  All rights reserved.
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
 *  Module: ofstd
 *
 *  Author: Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose: Class for various helper functions
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-11 12:08:06 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstd.h"


BEGIN_EXTERN_C
#include <stdio.h>       /* for fopen() and fclose() */
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>    /* for stat() */
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>      /* for access() */
#endif
#ifdef HAVE_IO_H
#include <io.h>          /* for access() on Win32 */
#endif
END_EXTERN_C

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>   /* for opendir() and closedir() */
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>      /* for opendir() and closedir() */
#endif /* HAVE_SYS_TYPES_H */

#ifdef HAVE_WINDOWS_H
#include <windows.h>     /* for GetFileAttributes() */

#ifndef R_OK /* windows defines access but not the constants */
#define W_OK 02 /* Write permission */
#define R_OK 04 /* Read permission */
#define F_OK 00 /* Existance only */
#endif /* R_OK */

#endif /* HAVE_WINDOWS_H */


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


// --- file system functions ---

OFBool OFStandard::pathExists(const OFString &pathName)
{
    OFBool result = OFFalse;
    /* check for valid path name */
    if (pathName.length() > 0)
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
    if (fileName.length() > 0)
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
    if (dirName.length() > 0)
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


OFString &OFStandard::normalizeDirName(OFString &result,
                                       const OFString &dirName,
                                       const OFBool allowEmptyDirName)
{
    result = dirName;
    /* remove trailing path separators (keep it if at the beginning of the string) */
    while ((result.length() > 1) && (result[result.length() - 1] == PATH_SEPARATOR))
        result.erase(result.length() - 1, 1);
    /* avoid empty directory name (use "." instead) */
    if ((result.length() == 0) && !allowEmptyDirName)
        result = ".";
    return result;
}


OFString &OFStandard::combineDirAndFilename(OFString &result,
                                            const OFString &dirName,
                                            const OFString &fileName,
                                            const OFBool allowEmptyDirName)
{
    /* normalize the directory name */
    normalizeDirName(result, dirName, allowEmptyDirName);
    /* check file name */
    if (fileName.length() > 0)
    {
        /* add path separator (if required) ... */
        if ((result.length() > 0) && (result[result.length() - 1] != PATH_SEPARATOR))
            result += PATH_SEPARATOR;
        /* ...and file name */
        result += fileName;
    }
    return result;
}


/*
 *  $Log: ofstd.cc,v $
 *  Revision 1.2  2002-04-11 12:08:06  joergr
 *  Added general purpose routines to check whether a file exists, a path points
 *  to a directory or a file, etc.
 *
 *  Revision 1.1  2001/12/04 16:57:18  meichel
 *  Implemented strlcpy and strlcat routines compatible with the
 *    corresponding BSD libc routines in class OFStandard
 *
 */
