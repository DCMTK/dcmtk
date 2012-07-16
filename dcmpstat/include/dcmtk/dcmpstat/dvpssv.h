/*
 *
 *  Copyright (C) 1998-2012, OFFIS e.V.
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSSoftcopyVOI
 *
 */

#ifndef DVPSSV_H
#define DVPSSV_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmpstat/dvpsril.h"     /* for DVPSReferencedImage_PList */
#include "dcmtk/dcmpstat/dvpstyp.h"     /* for enum types */

class DVPSReferencedSeries_PList;

/** the representation of one item of the Softcopy VOI LUT Sequence
 */

class DCMTK_DCMPSTAT_EXPORT DVPSSoftcopyVOI
{
public:
  /// default constructor
  DVPSSoftcopyVOI();

  /// copy constructor
  DVPSSoftcopyVOI(const DVPSSoftcopyVOI& copy);

  /** clone method.
   *  @return a pointer to a new DVPSSoftcopyVOI object containing
   *  a copy of this object.
   */
  DVPSSoftcopyVOI *clone() { return new DVPSSoftcopyVOI(*this); }

  /// destructor
  virtual ~DVPSSoftcopyVOI();

  /** reads a softcopy VOI LUT item from a DICOM dataset.
   *  The DICOM elements of the softcopy VOI LUT item are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the item of the SoftcopyVOILUTSequence from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);

  /** writes the dsoftcopy VOI LUT item managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the SoftcopyVOILUTSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

  /** checks if this displayed area is applicable to the given image and frame.
   *  @param instanceUID SOP instance UID of the current image
   *  @param frame number of the current frame
   *  @return OFTrue if applicable.
   */
  OFBool isApplicable(const char *instanceUID, unsigned long frame);

  /** checks if this displayed area matches exactly the applicability
   *  defined by the instanceUID, frame and applicability parameters.
   *  @param instanceUID SOP instance UID of the current image
   *  @param frame number of the current frame
   *  @return OFTrue if matching.
   */
  OFBool matchesApplicability(const char *instanceUID, unsigned long frame, DVPSObjectApplicability applicability);

  /** add a new image reference.
   *  Checks if the referenced SOP instance UID already exists in this sequence.
   *  If it exists, an error code is returned. Otherwise a new image reference
   *  is created and added to the ReferencedImageSequence.
   *  @param sopclassUID the SOP class UID of the image reference to be added.
   *  @param instanceUID the SOP instance UID of the image reference to be added.
   *  @param frame the frame number of the image reference (current image) to be added.
   *  @param applicability the applicability of the image reference (DVPSB_currentFrame or DVPSB_currentImage)
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addImageReference(
    const char *sopclassUID,
    const char *instanceUID,
    unsigned long frame,
    DVPSObjectApplicability applicability);

  /** removes a reference to an image or frame. If the current reference is empty ("global"), an
   *  explicit list of references is constructed from the list of series/instance references.
   *  The image or frame reference is removed from the total list of references in this object.
   *  If the only reference contained in this object is removed, the reference list becomes empty
   *  which means that the current reference becomes "global". This case must be handled by the
   *  called (e.g. by deleting the displayed area selection object).
   *  @param allReferences list of series/instance references registered for the presentation state.
   *  @param instanceUID SOP instance UID of the current image
   *  @param frame number of the current frame
   *  @param numberOfFrames the number of frames of the current image
   *  @param applicability applicability of the new displayed area selection
   *  @param applicability the applicability of the image reference to be removed
   *    (DVPSB_currentFrame or DVPSB_currentImage)
   */
  void removeImageReference(
    DVPSReferencedSeries_PList& allReferences,
    const char *instanceUID,
    unsigned long frame,
    unsigned long numberOfFrames,
    DVPSObjectApplicability applicability);

  /** removes all image references for this displayed area.
   */
  void clearImageReferences() { referencedImageList.clear(); }

  /** checks if the list of image references for this displayed area is empty.
   *  @return OFTrue if list of image references is empty, OFFalse otherwise.
   */
  OFBool imageReferencesEmpty() { if (referencedImageList.size()==0) return OFTrue; else return OFFalse; }

  /** check if a VOI LUT is currently active
   *  @return OFTrue if a VOI LUT is active, OFFalse if VOI Window is active.
   */
  OFBool haveLUT() { return useLUT; }

  /** returns a description string for a currently active VOI transform.
   *  If no description is available, NULL is returned.
   *  @return a pointer to a string or NULL.
   */
  const char *getCurrentVOIDescription();

  /** gets the width of the current VOI window.
   *  May only be called if haveLUT() is OFFalse.
   *  @param w the window width is returned in this parameter
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition getCurrentWindowWidth(double &w);

  /** get the center of the current VOI window.
   *  May only be called if haveLUT() is OFFalse.
   *  @param c the window center is returned in this parameter
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition getCurrentWindowCenter(double &c);

  /** returns a reference to the current VOI LUT descriptor.
   *  May only be called if haveLUT() is OFTrue.
   *  @return reference to the current VOI LUT descriptor
   */
  DcmUnsignedShort& getLUTDescriptor() { return voiLUTDescriptor; }

  /** returns a reference to the current VOI LUT data.
   *  May only be called if haveLUT() is OFTrue.
   *  @return reference to the current VOI LUT data
   */
  DcmUnsignedShort& getLUTData() { return voiLUTData; }

  /** sets a user defined VOI window center and width.
   *  @param wCenter the window center
   *  @param wWidth  the window width
   *  @param description an optional description. Default: absent.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition setVOIWindow(double wCenter, double wWidth, const char *description=NULL);

  /** stores (copies) a VOI lookup table.
   *  If the method returns an error code, an old LUT is left unchanged.
   *  @param lutDescriptor the LUT Descriptor in DICOM format (VM=3)
   *  @param lutData the LUT Data in DICOM format
   *  @param lutExplanation the LUT Explanation in DICOM format, may be empty.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setVOILUT(
    DcmUnsignedShort& lutDescriptor,
    DcmUnsignedShort& lutData,
    DcmLongString& lutExplanation);

private:
  /// private undefined assignment operator
  DVPSSoftcopyVOI& operator=(const DVPSSoftcopyVOI&);

  /* since the VOI LUT sequence in the Softcopy VOI LUT module must
   * not contain more than one item, we do not need to manage a list of
   * VOI LUT SQ items.
   */

  /// ReferencedImageSequence, Type 1c
  DVPSReferencedImage_PList referencedImageList;
  /// If true, a VOI LUT is set, otherwise a VOI Window is set.
  OFBool                   useLUT;
  /// Module=VOI_LUT, VR=xs, VM=3, Type 1c
  DcmUnsignedShort         voiLUTDescriptor;
  /// Module=VOI_LUT, VR=LO, VM=1, Type 3
  DcmLongString            voiLUTExplanation;
  /// Module=VOI_LUT, VR=xs, VM=1-n, Type 1c
  DcmUnsignedShort         voiLUTData;
  /// Module=VOI_LUT, VR=DS, VM=1-n, Type 1c (unlike VOI LUT module!)
  DcmDecimalString         windowCenter;
  /// Module=VOI_LUT, VR=DS, VM=1-n, Type 1c
  DcmDecimalString         windowWidth;
  /// Module=VOI_LUT, VR=LO, VM=1-n, Type 3
  DcmLongString            windowCenterWidthExplanation;

};

#endif
