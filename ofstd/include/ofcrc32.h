/*
 *
 *  Copyright (C) 2002, OFFIS
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
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: general purpose 32-bit CRC in C++
 *           Code is based on the CRC32 implementation (C)1986 Gary S. Brown
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-01-08 10:18:48 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofcrc32.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __OFCRC32_H
#define __OFCRC32_H


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
 * Revision 1.1  2002-01-08 10:18:48  joergr
 * Added general purpose class which computes a CRC32 checksum on arbitrary
 * data.
 *
 *
 */
