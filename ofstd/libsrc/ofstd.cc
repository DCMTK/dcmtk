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
 *
 *  Furthermore, the "Base64" encoder/decoder has been derived from an
 *  implementation with the following copyright notice:
 *
 *  Copyright (c) 1999, Bob Withers - bwit@pobox.com
 *
 *  This code may be freely used for any purpose, either personal or commercial,
 *  provided the authors copyright notice remains intact.
 *
 *
 *  Module: ofstd
 *
 *  Author: Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose: Class for various helper functions
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-05-16 15:56:20 $
 *  CVS/RCS Revision: $Revision: 1.5 $
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
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#include <windows.h>     /* for GetFileAttributes() */

#ifndef R_OK /* windows defines access() but not the constants */
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


const OFString &OFStandard::convertToMarkupString(const OFString &sourceString,
                                                  OFString &markupString,
                                                  const OFBool convertNonASCII,
                                                  const OFBool xmlMode,
                                                  const OFBool newlineAllowed)
{
    /* char ptr allows fastest access to the string */
    const char *str = sourceString.c_str();
    /* start with empty string */
    markupString.clear();
    /* avoid to resize the string too often */
    markupString.resize(strlen(str));
    while (*str != 0)
    {
        /* less than */
        if (*str == '<')
            markupString += "&lt;";
        /* greater than */
        else if (*str == '>')
            markupString += "&gt;";
        /* ampers and */
        else if (*str == '&')
            markupString += "&amp;";
        /* quotation mark */
        else if (*str == '"')
            markupString += "&quot;";
        /* newline: LF, CR, LF CR, CR LF */
        else if ((*str == '\012') || (*str == '\015'))
        {
            /* skip next character if it belongs to the newline sequence */
            if (((*str == '\012') && (*(str + 1) == '\015')) || ((*str == '\015') && (*(str + 1) == '\012')))
                str++;
            if (xmlMode)
            {
                /* "<br>" and "&param;" not defined in XML - requires DTD definition */
                markupString += "&#182;";
            } else {
                if (newlineAllowed)
                    markupString += "<br>\n";
                else
                    markupString += "&para;";
            }
        }
        else {
            /* other character: ... */
            const size_t charValue = (size_t)(*(const unsigned char *)str);
            if (convertNonASCII && (charValue > 127))
            {
                char buffer[16];
                sprintf(buffer, "%lu", (unsigned long)charValue);
                /* convert > #127 to Unicode (ISO Latin-1), what is about < #32 ? */
                markupString += "&#";
                markupString += buffer;
                markupString += ";";
            } else {
                /* just append */
                markupString += *str;
            }
        }
        str++;
    }
    return markupString;
}


// Base64 translation table as described in RFC 2045 (MIME)
static const char enc_base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const OFString &OFStandard::encodeBase64(const unsigned char *data,
                                         const size_t length,
                                         OFString &result,
                                         const size_t width)
{
    result.clear();
    /* check data buffer to be encoded */
    if (data != NULL)
    {
        unsigned char c;
        size_t j = 0;
        size_t w = 0;
        /* reserve expected output size: +33%, even multiple of 4 */
        result.reserve(((length + 2) / 3) * 4);
        /* iterate over all data elements */
        for (size_t i = 0; i < length; i++)
        {
            /* encode first 6 bits */
            result[j++] = enc_base64[(data[i] >> 2) & 0x3f];
            /* insert line break (if width > 0) */
            if (++w == width)
            {
                result[j++] = '\n';
                w = 0;
            }
            /* encode remaining 2 bits of the first byte and 4 bits of the second byte */
            c = (data[i] << 4) & 0x3f;
            if (++i < length)
                c |= (data[i] >> 4) & 0x0f;
            result[j++] = enc_base64[c];
            /* insert line break (if width > 0) */
            if (++w == width)
            {
                result[j++] = '\n';
                w = 0;
            }
            /* encode remaining 4 bits of the second byte and 2 bits of the third byte */
            if (i < length)
            {
                c = (data[i] << 2) & 0x3f;
                if (++i < length)
                    c |= (data[i] >> 6) & 0x03;
                result[j++] = enc_base64[c];
            } else {
                i++;
                /* append fill char */
                result[j++] = '=';
            }
            /* insert line break (if width > 0) */
            if (++w == width)
            {
                result[j++] = '\n';
                w = 0;
            }
            /* encode remaining 6 bits of the third byte */
            if (i < length)
                result[j++] = enc_base64[data[i] & 0x3f];
            else /* append fill char */
                result[j++] = '=';
            /* insert line break (if width > 0) */
            if (++w == width)
            {
                result[j++] = '\n';
                w = 0;
            }
        }
        /* append trailing 0 byte */
        result[j] = '\0';
    }
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

const size_t OFStandard::decodeBase64(const OFString &data,
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
                while ((i < length) && ((data[i] < '+') || (data[i] > 'z') || ((c1 = dec_base64[data[i] - '+']) > 63)))
                    i++;
                if (++i < length)
                {
                    /* skip invalid characters and assign second decoded char */
                    while ((i < length) && ((data[i] < '+') || (data[i] > 'z') || ((c2 = dec_base64[data[i] - '+']) > 63)))
                        i++;
                    if (i < length)
                    {
                        /* decode first byte */
                        result[count++] = (c1 << 2) | ((c2 >> 4) & 0x3);
                        if (++i < length)
                        {
                            /* skip invalid characters and assign third decoded char */
                            while ((i < length) && ((data[i] < '+') || (data[i] > 'z') || ((c1 = dec_base64[data[i] - '+']) > 63)))
                                i++;
                            if (i < length)
                            {
                                /* decode second byte */
                                result[count++] = ((c2 << 4) & 0xf0) | ((c1 >> 2) & 0xf);
                                if (++i < length)
                                {
                                    /* skip invalid characters and assign fourth decoded char */
                                    while ((i < length) && ((data[i] < '+') || (data[i] > 'z') || ((c2 = dec_base64[data[i] - '+']) > 63)))
                                        i++;
                                    /* decode third byte */
                                    if (i < length)
                                        result[count++] = ((c1 << 6) & 0xc0) | c2;
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


/*
 *  $Log: ofstd.cc,v $
 *  Revision 1.5  2002-05-16 15:56:20  meichel
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
