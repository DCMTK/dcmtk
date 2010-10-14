/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: general purpose 32-bit CRC in C++
 *           Code is based on the CRC32 implementation (C)1986 Gary S. Brown
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:50 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef OFCRC32_H
#define OFCRC32_H


/** general-purpose 32-bit CRC algorithm.
 */
class OFCRC32
{
public:

  /// constructor
  OFCRC32()
  : value(0)
  {
  }

  /// destructor
  ~OFCRC32()
  {
  }

  /// reset object to initial state (zero CRC)
  void reset()
  {
    value=0;
  }

  /** add block of raw data to CRC
   *  @param ptr pointer to raw data
   *  @param size length of raw data block in bytes
   */
  void addBlock(const void *ptr, unsigned long size);

  /// returns the current CRC as unsigned int
  unsigned int getCRC32() const
  {
    return value;
  }

  /** compute CRC for given block of data using a temporary CRC object
   *  @param ptr pointer to raw data
   *  @param size length of raw data block in bytes
   *  @return CRC32 as unsigned int
   */
  static unsigned int compute(const void *ptr, unsigned long size);

private:
  /// CRC look-up table
  static const unsigned int crctab[256];

  /// current CRC
  unsigned int value;
};

#endif


/*
 * CVS/RCS Log:
 * $Log: ofcrc32.h,v $
 * Revision 1.4  2010-10-14 13:15:50  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.3  2005/12/08 16:05:53  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2003/12/05 10:37:41  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright date where appropriate.
 *
 * Revision 1.1  2002/01/08 10:18:48  joergr
 * Added general purpose class which computes a CRC32 checksum on arbitrary
 * data.
 *
 *
 */
