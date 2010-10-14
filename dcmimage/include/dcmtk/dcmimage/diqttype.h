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
 *  Purpose: typdefs and constants for the color quantization classes
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:30 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIQTTYPE_H
#define DIQTTYPE_H


#include "dcmtk/config/osconfig.h"

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
 * Revision 1.6  2010-10-14 13:16:30  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.5  2009-11-25 14:36:20  joergr
 * Added new source and header file for the module logger.
 *
 * Revision 1.4  2009-10-13 14:08:33  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.3  2005-12-08 16:01:55  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2003/12/23 12:21:09  joergr
 * Updated copyright header.
 *
 * Revision 1.1  2002/01/25 13:32:08  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
