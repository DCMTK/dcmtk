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
 *  Purpose: class DcmQuantFloydSteinberg
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-01-25 13:32:08 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diquant.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DIQUANT_H
#define DIQUANT_H

#include "osconfig.h"
#include "ofcond.h"    /* for OFCondition */
#include "dctypes.h"   /* for Uint32 etc. */
#include "diqttype.h"  /* for enums */

class DicomImage;
class DcmItem;
class DcmTagKey;
class OFString;

class DcmQuant
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
   *  @param verbose verbose mode, print information to console
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
    OFBool verbose,
    DcmLargestDimensionType largeType = DcmLargestDimensionType_default,
    DcmRepresentativeColorType repType = DcmRepresentativeColorType_default);

  /** create new SOP instance UID and Source Image Sequence
   *  referencing the old SOP instance.
   *  @param dataset dataset to be modified
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition newInstance(DcmItem *dataset);

  /** set first two values of Image Type to DERIVED\SECONDARY.
   *  @param dataset dataset to be modified
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition updateImageType(DcmItem *dataset);

  /** create Derivation Description. If a derivation description
   *  already exists, the old text is appended to the new text.
   *  @param dataset dataset to be modified
   *  @param description description string.
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition updateDerivationDescription(DcmItem *dataset, const char *description);

  /** helper function that inserts a string attribute with a given value into a dataset
   *  if missing in the dataset.
   *  @param dataset dataset to insert to, must not be NULL.
   *  @param tag tag key of attribute to check/insert
   *  @param val string value, may be NULL.
   *  @return EC_Normal if successful, an error code otherwise
   */   
  static OFCondition insertStringIfMissing(DcmItem *dataset, const DcmTagKey& tag, const char *val);

  /** helper function that converts a dataset containing a DICOM image
   *  into a valid (standard extended) Secondary Capture object
   *  by inserting all attributes that are type 1/2 in Secondary Capture
   *  and missing in the source dataset.  Replaces SOP Class UID
   *  by Secondary Capture. It does not, however, change an existing SOP Instance UID.
   *  @param dataset dataset to insert to, must not be NULL.
   *  @param tag tag key of attribute to check/insert
   *  @param val string value, may be NULL.
   *  @return EC_Normal if successful, an error code otherwise
   */   
  static OFCondition convertToSecondaryCapture(DcmItem *dataset);

};


#endif

/*
 * CVS/RCS Log:
 * $Log: diquant.h,v $
 * Revision 1.1  2002-01-25 13:32:08  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
