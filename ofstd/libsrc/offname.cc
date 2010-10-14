/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:53 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/offname.h"
#include "dcmtk/ofstd/ofcast.h"

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

OFBool OFFilenameCreator::makeFilename(unsigned int seed, const char *dir, const char *prefix, const char *postfix, OFString &filename)
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
    addLongToString(((myrand_r(&seed) << 16) | myrand_r(&seed)), filename);
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

int OFFilenameCreator::myrand_r(unsigned int *seed)
{
  unsigned long val = OFstatic_cast(unsigned long, *seed);  
  val = val * 1103515245 + 12345;
  *seed = OFstatic_cast(unsigned int, val %(OFstatic_cast(unsigned long, 0x80000000)));
  return OFstatic_cast(int, *seed);
}

/*
 *  $Log: offname.cc,v $
 *  Revision 1.12  2010-10-14 13:14:53  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.11  2005/12/08 15:48:57  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.10  2003/07/09 13:58:04  meichel
 *  Adapted type casts to new-style typecast operators defined in ofcast.h
 *
 *  Revision 1.9  2002/11/27 11:23:10  meichel
 *  Adapted module ofstd to use of new header file ofstdinc.h
 *
 *  Revision 1.8  2001/06/01 15:51:38  meichel
 *  Updated copyright header
 *
 *  Revision 1.7  2000/10/12 08:13:17  joergr
 *  Added explicit typecast to avoid compiler warnings.
 *
 *  Revision 1.6  2000/10/10 12:01:07  meichel
 *  Implemented thread safe random number generator, needed on systems
 *    where threads but no Posix rand_r function are available.
 *
 *  Revision 1.5  2000/04/14 15:20:16  meichel
 *  If compiled on Unix platforms with -D_REENTRANT, OFFilenameCreator uses
 *    the reentrant Posix routine rand_r instead of rand and srand.
 *
 *  Revision 1.4  2000/03/08 16:36:06  meichel
 *  Updated copyright header.
 *
 *  Revision 1.3  2000/02/23 15:13:48  meichel
 *  Corrected macro for Borland C++ Builder 4 workaround.
 *
 *  Revision 1.2  2000/02/01 10:09:40  meichel
 *  Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *    workaround for bug in compiler header files.
 *
 *  Revision 1.1  1999/03/03 13:58:11  meichel
 *  Implemented new class for automatically creating filenames.
 *
 *
 */

