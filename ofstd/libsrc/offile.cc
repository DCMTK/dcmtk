/*
 *
 *  Copyright (C) 2011-2021, OFFIS e.V.
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
 *  Author:  Marco Eichelberg, Joerg Riesmeier
 *
 *  Purpose: C++ wrapper class for stdio FILE functions and
 *           wide character filenames
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/offile.h"
#include "dcmtk/ofstd/offilsys.h"
#include "dcmtk/ofstd/ofutil.h"
#include <cerrno>

#ifdef __SUNPRO_CC
BEGIN_EXTERN_C
#include <stdio.h>
END_EXTERN_C
#else
#include <cstdio>
#endif

#ifdef HAVE_WINDOWS_H
#include "dcmtk/ofstd/ofchrenc.h"   /* for class OFCharacterEncoding */
#endif


OFFilename::OFFilename()
  : filename_(NULL)
#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
  , wfilename_(NULL)
#endif
{
}


OFFilename::OFFilename(const char *filename,
                       const OFBool convert)
  : filename_(NULL)
#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
  , wfilename_(NULL)
#endif
{
    set(filename, convert);
}


OFFilename::OFFilename(const OFString &filename,
                       const OFBool convert)
  : filename_(NULL)
#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
  , wfilename_(NULL)
#endif
{
    set(filename, convert);
}

OFFilename::OFFilename(const OFpath &path,
                       const OFBool convert)
  : filename_(NULL)
#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
  , wfilename_(NULL)
#endif
{
    set(path, convert);
}

#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
OFFilename::OFFilename(const wchar_t *filename,
                       const OFBool convert)
  : filename_(NULL),
    wfilename_(NULL)
{
    set(filename, convert);
}
#endif


OFFilename::OFFilename(const OFFilename &arg)
  : filename_(NULL)
#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
  , wfilename_(NULL)
#endif
{
    if (arg.filename_ != NULL)
        filename_ = strdup(arg.filename_);
#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
    if (arg.wfilename_ != NULL)
        wfilename_ = _wcsdup(arg.wfilename_);
#endif
}


OFFilename::~OFFilename()
{
    clear();
}


OFFilename &OFFilename::operator=(const OFFilename &arg)
{
    if (&arg != this)
    {
        free(filename_);
        filename_ = (arg.filename_ != NULL) ? strdup(arg.filename_) : NULL;
#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
        free(wfilename_);
        wfilename_ = (arg.wfilename_ != NULL) ? _wcsdup(arg.wfilename_) : NULL;
#endif
    }
    return *this;
}


void OFFilename::clear()
{
    free(filename_);
    filename_ = NULL;
#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
    free(wfilename_);
    wfilename_ = NULL;
#endif
}


void OFFilename::swap(OFFilename &arg)
{
    OFswap(filename_, arg.filename_);
#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
    OFswap(wfilename_, arg.wfilename_);
#endif
}


OFBool OFFilename::isEmpty() const
{
    OFBool result = (filename_ == NULL) || (filename_[0] == '\0');
#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
    if (result)
        result = (wfilename_ == NULL) || (wfilename_[0] == L'\0');
#endif
    return result;
}


void OFFilename::set(const char *filename,
                     const OFBool convert)
{
    clear();
    if (filename != NULL)
    {
        filename_ = strdup(filename);
#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
#ifdef HAVE_WINDOWS_H
        if (convert)
        {
            wchar_t *tmpString = NULL;
            size_t strLen = 0;
            /* convert UTF-8 representation to wide character encoding */
            OFCharacterEncoding::convertToWideCharString(filename, strlen(filename),
                tmpString, strLen, OFCharacterEncoding::CPC_UTF8);
            wfilename_ = _wcsdup(tmpString);
            delete[] tmpString;
        }
#endif
#endif
        /* avoid compiler warning on unused variable */
        (void)convert;
    }
}


void OFFilename::set(const OFString &filename,
                     const OFBool convert)
{
    set(filename.c_str(), convert);
}


void OFFilename::set(const OFpath &path,
                     const OFBool convert)
{
    set(path.native(), convert);
}


#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
void OFFilename::set(const wchar_t *filename,
                     const OFBool convert)
{
    clear();
    if (filename != NULL)
    {
        wfilename_ = _wcsdup(filename);
#ifdef HAVE_WINDOWS_H
        if (convert)
        {
            OFString tmpString;
            /* convert wide character encoding to UTF-8 representation */
            OFCharacterEncoding::convertFromWideCharString(filename, wcslen(filename),
                tmpString, OFCharacterEncoding::CPC_UTF8);
            filename_ = strdup(tmpString.c_str());
        }
#endif
        /* avoid compiler warning on unused variable */
        (void)convert;
    }
}
#endif


STD_NAMESPACE ostream &operator<<(STD_NAMESPACE ostream &stream,
                                  const OFFilename &filename)
{
    /* always output the 8-bit representation */
    stream << OFSTRING_GUARD(filename.getCharPointer());
    return stream;
}

OFBool OFFile::popen(const char *command, const char *modes)
{
  if (file_) fclose();
#if defined(HAVE_POPEN) || defined(__SUNPRO_CC)
  // SunPro defines popen() in a header file where CMake cannot find it, but it is there.
  file_ = :: popen(command, modes);
#else
  file_ = _popen(command, modes);
#endif
  if (file_) popened_ = OFTrue; else storeLastError();
  return (file_ != NULL);
}

int OFFile::fclose()
{
  int result = 0;
  if (file_)
  {
    if (popened_)
    {
#if defined(HAVE_PCLOSE) || defined(__SUNPRO_CC)
      // SunPro defines pclose() in a header file where CMake cannot find it, but it is there.
      result = :: pclose(file_);
#else
      result = _pclose(file_);
#endif
    }
    else
    {
      result = STDIO_NAMESPACE fclose(file_);
    }
    // After calling fclose() once, the FILE* is gone even if fclose() failed.
    file_ = NULL;
  }
  if (result) storeLastError();
  return result;
}
