/*
 *
 *  Copyright (C) 2002-2011, OFFIS e.V.
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
 *  Purpose: class DcmQuantFloydSteinberg
 *
 */


#ifndef DIQUANT_H
#define DIQUANT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcond.h"    /* for OFCondition */
#include "dcmtk/dcmdata/dctypes.h"   /* for Uint32 etc. */
#include "dcmtk/dcmimage/diqttype.h"  /* for enums */
#include "dcmtk/ofstd/ofstring.h"   /* for class OFString */


class DicomImage;
class DcmItem;
class DcmTagKey;


class DCMTK_DCMIMAGE_EXPORT DcmQuant
{
public:

  /** converts the given color image into a palette color image.
   *  All frames of the image are converted.  The converted result
   *  is written as a complete Image Pixel module to the given
   *  target item.
   *  @param sourceImage DICOM color image
   *  @param target target item to which the palette color image is written
   *  @param writeAsOW if true, the LUT Data attributes are encoded as OW instead
   *    US.  LUT Data is always written as OW if numberOfColors is 65536.
   *  @param write16BitEntries if true, LUT data is encoded with 16 bits per entry
   *  @param floydSteinberg determines whether or not to use Floyd-Steinberg
   *    error diffusion during creation of the palette color image
   *  @param numberOfColors desired number of colors in the color palette.
   *    Valid range is [2..65536].
   *  @param description description string suitable for use as
   *    Derivation Description returned in this parameter
   *  @param largeType algorithm used for determining the largest dimension
   *    in the Median Cut algorithm
   *  @param repType algorithm for choosing a representative color for each
   *    box in the Median Cut algorithm
   *  @return EC_Normal if successful, an error code otherwise.
   */
  static OFCondition createPaletteColorImage(
    DicomImage& sourceImage,
    DcmItem& target,
    OFBool writeAsOW,
    OFBool write16BitEntries,
    OFBool floydSteinberg,
    Uint32 numberOfColors,
    OFString& description,
    DcmLargestDimensionType largeType = DcmLargestDimensionType_default,
    DcmRepresentativeColorType repType = DcmRepresentativeColorType_default);

  /** create Derivation Description. If a derivation description
   *  already exists, the old text is appended to the new text.
   *  @param dataset dataset to be modified
   *  @param description description string.
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition updateDerivationDescription(DcmItem *dataset, const char *description);

};


#endif
