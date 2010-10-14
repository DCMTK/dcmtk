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
 *  Purpose: class DcmQuantPixel
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:30 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIQTPIX_H
#define DIQTPIX_H


#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"   /* for OFBool */
#include "dcmtk/dcmimage/diqttype.h"  /* for DcmQuantHashSize, DcmQuantComponent */
#include "dcmtk/dcmimage/diqtstab.h"  /* for DcmScaleTable */


/** objects of this class represent individual RGB pixels.  This class is
 *  used by the color quantization classes. For efficiency considerations,
 *  all methods are declared inline.
 */
class DcmQuantPixel
{
public:

  /** default constructor, creates black pixel
   */
  DcmQuantPixel()
  : red(0)
  , green(0)
  , blue(0)
  {
  }

  /** copy assignment constructor
   */
  DcmQuantPixel(const DcmQuantPixel& arg)
  : red(arg.red)
  , green(arg.green)
  , blue(arg.blue)
  {
  }

  // we don't declare a destructor here, but the standard destructor will do.

  /// comparison operator for equality
  inline OFBool operator==(const DcmQuantPixel& src) const
  {
    return (red == src.red) && (green == src.green) && (blue == src.blue);
  }

  /** this method computes the luminance of the current pixel
   *  according to the NTSC formula.  The range of the luminance equals the
   *  range of the underlying DcmQuantComponent type. However, the
   *  luminance is returned as a double.
   *  @return luminance of this pixel
   */
  inline double luminance() const
  {
    return 0.299 * red + 0.587 * green + 0.114 * blue;
  }

  /** this method computes an unsigned long hash value for the current pixel.
   *  The hash algorithm is taken from Jef Poskanzer's ppm utility library.
   *  The return value is guaranteed to be < DcmQuantHashSize.
   *  @return hash value for current pixel
   */
  inline unsigned long hash() const
  {
    return ((OFstatic_cast(unsigned long, red) * 33023UL + OFstatic_cast(unsigned long, green) * 30013UL +
            OFstatic_cast(unsigned long, blue) * 27011UL) & 0x7fffffffUL) % DcmQuantHashSize;
  }

  /** returns the red component
   *  @return red component of this pixel
   */
  inline DcmQuantComponent getRed() const
  {
  	return red;
  }

  /** returns the green component
   *  @return green component of this pixel
   */
  inline DcmQuantComponent getGreen() const
  {
  	return green;
  }

  /** returns the blue component
   *  @return blue component of this pixel
   */
  inline DcmQuantComponent getBlue() const
  {
  	return blue;
  }

  /** assigns new R, G and B values to this pixel
   *  @param r new R
   *  @param g new G
   *  @param b new B
   */
  inline void assign(
    DcmQuantComponent r,
    DcmQuantComponent g,
    DcmQuantComponent b)
  {
    red = r;
    green = g;
    blue = b;
  }

  /** assigns new R, G and B values to this pixel
   *  using the given scale table in which all
   *  three values are looked up.
   *  @param r new R
   *  @param g new G
   *  @param b new B
   *  @param table scale table (LUT) in which R, G and B are looked up.
   */
  inline void scale(
    DcmQuantComponent r,
    DcmQuantComponent g,
    DcmQuantComponent b,
    const DcmQuantScaleTable& table)
  {
    red   = table[r];
    green = table[g];
    blue  = table[b];
  }

private:
  /// red color component of this pixel
  DcmQuantComponent red;

  /// green color component of this pixel
  DcmQuantComponent green;

  /// blue color component of this pixel
  DcmQuantComponent blue;
};


#endif


/*
 * CVS/RCS Log:
 * $Log: diqtpix.h,v $
 * Revision 1.4  2010-10-14 13:16:30  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.3  2005/12/08 16:01:53  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2003/12/23 12:20:07  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Updated copyright header.
 *
 * Revision 1.1  2002/01/25 13:32:07  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
