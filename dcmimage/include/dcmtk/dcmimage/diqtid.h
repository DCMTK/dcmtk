/*
 *
 *  Copyright (C) 2002-2005, OFFIS
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
 *  Purpose: class DcmQuantIdent
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-12-08 16:01:51 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIQTID_H
#define DIQTID_H


#include "dcmtk/config/osconfig.h"


class DcmQuantPixel;


/** pseudo error diffusion class implementing an identity transformation.
 *  This class implements a public API mostly identical to that
 *  of class DcmQuantFloydSteinberg.
 *  Since several methods of the error diffusion class are called for each
 *  single image pixel during conversion of a color image to palette color,
 *  we do not use virtual methods and a common abstract base class here.
 *  Instead we implement all methods inline, and use a template to select
 *  the appropriate class at compile time.  With a decent optimizer this
 *  reduces the overhead for using this class to zero.
 */
class DcmQuantIdent
{
public:

  /// constructor
  DcmQuantIdent(unsigned long cols)
  : columns(cols)
  {
  }

  /// destructor
  ~DcmQuantIdent()
  {
  }

  /// dummy method needed for API compatibility with DcmQuantFloydSteinberg
  inline void adjust(DcmQuantPixel&, long, long)
  {
  }

  /// dummy method needed for API compatibility with DcmQuantFloydSteinberg
  inline void propagate(const DcmQuantPixel&, const DcmQuantPixel&, long)
  {
  }

  /** starts a new row. The initial and last column of the current row are determined.
   *  @param col initial column for the current row returned in this parameter
   *  @param limitcol limit column (one past the last valid column) for the
   *    current row returned in this parameter.
   */
  inline void startRow(long& col, long& limitcol)
  {
     col = 0;
     limitcol = columns;
  }

  /// dummy method needed for API compatibility with DcmQuantFloydSteinberg
  inline void finishRow()
  {
  }

  /** increases the column number
   *  @param col column number
   */
  inline void nextCol(long& col) const
  {
    ++col;
  }

private:

  /// number of columns in image
  unsigned long columns;

};


#endif


/*
 * CVS/RCS Log:
 * $Log: diqtid.h,v $
 * Revision 1.3  2005-12-08 16:01:51  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2003/12/23 12:18:41  joergr
 * Updated copyright header.
 *
 * Revision 1.1  2002/01/25 13:32:06  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
