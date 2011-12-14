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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2011-12-14 08:54:00 $
 *  CVS/RCS Revision: $Revision: 1.12 $
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

/*
 *  $Log: offname.h,v $
 *  Revision 1.12  2011-12-14 08:54:00  uli
 *  Make it possible to correctly build ofstd as a DLL.
 *
 *  Revision 1.11  2011-10-11 10:57:29  uli
 *  Made sure that makeFilename() influences the caller's seed.
 *
 *  Revision 1.10  2011-10-11 09:57:42  uli
 *  Move OFFileNameCreator::myrand_r to class OFStandard.
 *
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
