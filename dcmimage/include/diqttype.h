/*
 *
 *  Copyright (C) 2002-2003, OFFIS
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
 *  Purpose: typdefs and constants for the color quantization classes
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-12-23 12:21:09 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIQTTYPE_H
#define DIQTTYPE_H


#include "osconfig.h"


/// the basic type for pixel color components
typedef unsigned char DcmQuantComponent;

/// size of color hash table. Don't touch this.
#define DcmQuantHashSize 20023UL

/** maximum number of colors we count in a color histogram.
 *  If the number of colors turns out to be larger than this,
 *  then the image bit depth is reduced until the number
 *  of colors becomes smaller than this maximum.
 */
#define DcmQuantMaxColors 65536


/** defines the algorithm used for determining the
 *  largest dimension in the Median Cut algorithm
 */
enum DcmLargestDimensionType
{
  /// determine dimension by comparing the range in RGB space (default)
  DcmLargestDimensionType_default,

  /// determine dimension by transforming into luminance before comparison
  DcmLargestDimensionType_luminance
};


/** defines the algorithm for choosing a representative color for each
 *  box in the Median Cut algorithm
 */
enum DcmRepresentativeColorType
{
  /// average all the colors in the box (as specified in Heckbert's paper; this is the default)
  DcmRepresentativeColorType_default,

  /// average all pixels in the box
  DcmRepresentativeColorType_averagePixels,

  /// choose center of the box, ignoring any structure within the boxes
  DcmRepresentativeColorType_centerOfBox

};

#endif


/*
 * CVS/RCS Log:
 * $Log: diqttype.h,v $
 * Revision 1.2  2003-12-23 12:21:09  joergr
 * Updated copyright header.
 *
 * Revision 1.1  2002/01/25 13:32:08  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
