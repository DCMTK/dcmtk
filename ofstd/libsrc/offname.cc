/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Module: ofstd
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: OFFilenameCreator
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-03 13:58:11 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "offname.h"

/* give up after this number of unsuccessful attempts to create a unique filename */
#define MAX_TRY 1024  
                        
BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>    /* for time_t */
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>    /* for stat() */
#endif
#ifdef HAVE_TIME_H
#include <time.h>         /* for time() */
#endif
#include <errno.h>
#include <string.h>
#include <stdlib.h>
END_EXTERN_C

OFFilenameCreator::OFFilenameCreator()
: creation_time(0)
{
  creation_time = (unsigned long) time(NULL);
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
  srand(seed);
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
    addLongToString(((rand() << 16) | rand()), filename);
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
    if (m>9) chr_array[idx--] = (char)('a'+(m-10)); else chr_array[idx--] = (char)('0'+m);
  }
  s += chr_array;
  return;
}

unsigned int OFFilenameCreator::hashString(const char *str)
{
  /* very simple hash function: XOR result with string character and rotate left by 1 bit. */
  unsigned int result = (unsigned int)-1;
  if (str)
  {
    while (*str != '\0')
    {
      result %= *str++;
      if ((signed int)result <0) result = (result << 1)| 1; else result <<= 1;
    }
  }
  return result;
}

/*
 *  $Log: offname.cc,v $
 *  Revision 1.1  1999-03-03 13:58:11  meichel
 *  Implemented new class for automatically creating filenames.
 *
 *
 */

