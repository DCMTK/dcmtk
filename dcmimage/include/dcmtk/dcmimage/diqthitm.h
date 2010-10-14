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
 *  Module:  dcmimage
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmQuantHistogramItem
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:29 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIQTHITM_H
#define DIQTHITM_H


#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"   /* for OFBool */
#include "dcmtk/dcmimage/diqtpix.h"   /* for DcmQuantPixel */


/** this class extends DcmQuantPixel by an integer value
 *  which is used for various purposes.
 *  The class is used as a helper class during computation of an image
 *  histogram, as a member of a color LUT and as a member of a color hash
 *  table.
 */
class DcmQuantHistogramItem: public DcmQuantPixel
{
public:
  /** constructor
   *  @param colorP pixel value
   *  @param val initial value
   */
  DcmQuantHistogramItem(const DcmQuantPixel& colorP, int val)
  : DcmQuantPixel(colorP)
  , value(val)
  {
  }

  /// default constructor
  DcmQuantHistogramItem()
  : DcmQuantPixel()
  , value(0)
  {
  }

  // we don't declare a destructor here, but the standard destructor will do.

  /** compares the stored pixel value with the given pixel.
   *  @param colorP pixel to compare with
   *  @return true if pixel values are equal, false otherwise
   */
  inline OFBool equals(const DcmQuantPixel& colorP) const
  {
    return *this == colorP;
  }

  /// returns the integer value maintained by this object
  inline int getValue() const
  {
    return value;
  }

  /** assigns a new integer value to this object
   *  @param v new value
   */
  inline void setValue(int v)
  {
  	value = v;
  }

  /// increases the integer value maintained by this object by one
  inline void incValue()
  {
    ++value;
  }

private:

  /** integer value assigned to this pixel.  This value is used for different
   *  purposes.
   *  During computation of a histogram it is used as a counter that counts
   *  the instances of the current color.
   *  In a color hash table, it contains the index value from the color LUT
   *  assigned to this color.
   *  In a color LUT, it is the cluster value, i.e. the radius in which all
   *  color are guaranteed to be mapped to this palette color.
   */
  int value;

};


/// typedef for a pointer to a DcmQuantHistogramItem object
typedef DcmQuantHistogramItem *DcmQuantHistogramItemPointer;


#endif


/*
 * CVS/RCS Log:
 * $Log: diqthitm.h,v $
 * Revision 1.4  2010-10-14 13:16:29  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.3  2005/12/08 16:01:50  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2003/12/23 12:18:05  joergr
 * Updated copyright header.
 *
 * Revision 1.1  2002/01/25 13:32:06  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
