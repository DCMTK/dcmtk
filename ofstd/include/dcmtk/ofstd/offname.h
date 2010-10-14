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
 *  Update Date:      $Date: 2010-10-14 13:15:50 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef OFFNAME_H
#define OFFNAME_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofstring.h"

/** A class for automatically creating unique pseudo-random filenames.
 */  
class OFFilenameCreator
{
public:
  /// default constructor
  OFFilenameCreator();
  
  /// copy constructor
  OFFilenameCreator(const OFFilenameCreator& copy);

  /// destructor
  virtual ~OFFilenameCreator();

  /// assignment operator
  OFFilenameCreator &operator=(const OFFilenameCreator& copy);

  /** creates a new unique filename.
   *  @param seed seed of random number generator
   *  @param dir directory in which the file name should be located
   *  @param prefix filename prefix
   *  @param postfix filename postfix
   *  @param filename string in which the filename (path) is returned
   *  @return OFTrue if successful, OFFalse if no filename could be created.
   */
  OFBool makeFilename(unsigned int seed, const char *dir, const char *prefix, const char *postfix, OFString &filename);

  /** creates hash value. (i.e. random seed) from character string.
   *  @param str null-terminated string
   *  @return hash value
   */
  static unsigned int hashString(const char *str);

  /** simple but thread safe random number generator. The interface is derived
   *  from the Posix rand_r function. Uses a multiplicative congruential 
   *  random-number generator with period 2**32 that returns successive 
   *  pseudo-random numbers in the range of 0 to 0x7fffffff.
   *  @param seed pointer to seed of random number generator, must not be NULL.
   *  @return pseudo-random number in the range of 0 to 0x7fffffff.
   */
  static int myrand_r(unsigned int *seed);

private:

  /// date/time of creation of this object
  unsigned long creation_time;

   /** appends the lower 32 bit of the given number to the given string.
    *  Always appends exactly 8 digits (padded with leading zeroes).
    *  @param l number to be appended to string
    *  @param s string to be added to
    */
  static void addLongToString(unsigned long l, OFString& s);
};

#endif

/*
 *  $Log: offname.h,v $
 *  Revision 1.9  2010-10-14 13:15:50  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.8  2005/12/08 16:05:56  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.7  2003/12/05 10:37:41  joergr
 *  Removed leading underscore characters from preprocessor symbols (reserved
 *  symbols). Updated copyright date where appropriate.
 *
 *  Revision 1.6  2003/06/12 13:15:59  joergr
 *  Fixed inconsistent API documentation reported by Doxygen.
 *
 *  Revision 1.5  2001/06/01 15:51:34  meichel
 *  Updated copyright header
 *
 *  Revision 1.4  2000/10/10 12:01:04  meichel
 *  Implemented thread safe random number generator, needed on systems
 *    where threads but no Posix rand_r function are available.
 *
 *  Revision 1.3  2000/03/08 16:36:01  meichel
 *  Updated copyright header.
 *
 *  Revision 1.2  1999/03/22 09:00:50  joergr
 *  Added/Changed comments.
 *
 *  Revision 1.1  1999/03/03 13:58:04  meichel
 *  Implemented new class for automatically creating filenames.
 *
 *
 */

