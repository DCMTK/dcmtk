/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Module:  dcmimage
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmQuantHistogramItem
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-01-25 13:32:06 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diqthitm.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DIQTHITM_H
#define DIQTHITM_H

#include "osconfig.h"
#include "oftypes.h"   /* for OFBool */
#include "diqtpix.h"   /* for DcmQuantPixel */

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
 * Revision 1.1  2002-01-25 13:32:06  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
