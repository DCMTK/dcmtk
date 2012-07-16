/*
 *
 *  Copyright (C) 1997-2011, OFFIS e.V.
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

#ifndef OFFNAME_H
#define OFFNAME_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofstring.h"

/** A class for automatically creating unique pseudo-random filenames.
 */  
class DCMTK_OFSTD_EXPORT OFFilenameCreator
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
  OFBool makeFilename(unsigned int &seed, const char *dir, const char *prefix, const char *postfix, OFString &filename);

  /** creates hash value. (i.e. random seed) from character string.
   *  @param str null-terminated string
   *  @return hash value
   */
  static unsigned int hashString(const char *str);

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
