/*
 *
 *  Copyright (C) 1997-2017, OFFIS e.V.
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
 *  Module: ofstd
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: OFFilenameCreator
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/offname.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstd.h"        /* for OFString::myrand_r */

#define INCLUDE_CERRNO
#define INCLUDE_CSTRING
#define INCLUDE_CTIME
#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>    /* for time_t */
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>    /* for stat() */
#endif
END_EXTERN_C

/* give up after this number of unsuccessful attempts to create a unique filename */
#define MAX_TRY 1024

OFFilenameCreator::OFFilenameCreator()
: creation_time(0)
{
  creation_time = OFstatic_cast(unsigned long, time(NULL));
}

OFFilenameCreator::OFFilenameCreator(const OFFilenameCreator& copy)
: creation_time(copy.creation_time)
{
}

OFFilenameCreator::~OFFilenameCreator()
{
}

OFFilenameCreator &OFFilenameCreator::operator=(const OFFilenameCreator& copy)
{
  creation_time = copy.creation_time;
  return *this;
}

OFBool OFFilenameCreator::makeFilename(unsigned int &seed, const char *dir, const char *prefix, const char *postfix, OFString &filename)
{
  OFBool done = OFFalse;
  OFBool result = OFTrue;
  struct stat stat_buf;
  int stat_result = 0;
  unsigned tries = 0;
  do
  {
    // create filename
    filename.clear();
    if (dir)
    {
      filename = dir;
      filename += PATH_SEPARATOR;
    }
    if (prefix) filename += prefix;
    addLongToString(creation_time, filename);
    // on some systems OFrand_r may produce only 16-bit random numbers.
    // To be on the safe side, we use two random numbers for the upper and the lower 16 bits.
    addLongToString((((OFrand_r(seed) & 0xFFFF) << 16) | (OFrand_r(seed) & 0xFFFF)), filename);
    if (postfix) filename += postfix;

    // check if filename exists
    stat_result = stat(filename.c_str(), &stat_buf);
    if (stat_result == 0)
    {
      if (++tries == MAX_TRY)
      {
        done = OFTrue;
        result = OFFalse;
      }
    } else {
      // file does not exists, bail out
      done = OFTrue;
      if (errno != ENOENT) result=OFFalse; // something wrong with path
    }
  } while (!done);
  return result;
}


void OFFilenameCreator::addLongToString(unsigned long l, OFString &s)
{
  l &= 0xFFFFFFFFL;
  unsigned long m;
  int idx=7;
  char chr_array[9];
  strcpy(chr_array, "00000000");
  while (l)
  {
    m = l & 0x0FL;
    l >>= 4;
    if (m > 9)
        chr_array[idx--] = OFstatic_cast(char, 'a'+(m-10));
        else chr_array[idx--] = OFstatic_cast(char, '0'+m);
  }
  s += chr_array;
  return;
}

unsigned int OFFilenameCreator::hashString(const char *str)
{
  /* very simple hash function: XOR result with string character and rotate left by 1 bit. */
  unsigned int result = OFstatic_cast(unsigned int, -1);
  if (str)
  {
    while (*str != '\0')
    {
      result %= *str++;
      if (OFstatic_cast(signed int, result) <0) result = (result << 1)| 1; else result <<= 1;
    }
  }
  return result;
}
