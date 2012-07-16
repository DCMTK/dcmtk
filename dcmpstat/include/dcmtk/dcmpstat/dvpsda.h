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
 *    classes: DVPSDisplayedArea
 *
 */

#ifndef DVPSDA_H
#define DVPSDA_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmpstat/dvpsril.h"     /* for DVPSReferencedImage_PList */
#include "dcmtk/dcmpstat/dvpstyp.h"     /* for enum types */

class DVPSReferencedSeries_PList;

/** an item of the displayed area selection sequence in a presentation state.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSDisplayedArea
{
public:
  /// default constructor
  DVPSDisplayedArea();
  
  /// copy constructor
  DVPSDisplayedArea(const DVPSDisplayedArea& copy);

  /** clone method.
   *  @return a pointer to a new DVPSDisplayedArea object containing
   *  a copy of this object.
   */
  DVPSDisplayedArea *clone() { return new DVPSDisplayedArea(*this); }

  /// destructor
  virtual ~DVPSDisplayedArea();

  /** reads an displayed area selection from a DICOM dataset.
   *  The DICOM elements of the displayed area selection item are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the item of the DisplayedAreaSelectionSequence from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);
  
  /** writes the displayed area selection managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the DisplayedAreaSelectionSequence to which the data is written
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

  /** gets the presentation size mode for this displayed area selection.
   *  @return presentation size mode
   */
  DVPSPresentationSizeMode getPresentationSizeMode();
  
  /** gets the presentation pixel aspect ratio for this displayed area selection.
   *  Pixel aspect ratio is defined here as the width of a pixel divided
   *  by the height of a pixel (x/y).
   *  @return pixel aspect ratio
   */
  double getPresentationPixelAspectRatio();
  
  /** gets the displayed area top lefthand corner and
   *  bottom righthand corner.
   *  @param tlhcX the displayed area top lefthand corner X value is returned in this parameter
   *  @param tlhcY the displayed area top lefthand corner Y value is returned in this parameter
   *  @param brhcX the displayed area bottom righthand corner X value is returned in this parameter
   *  @param brhcY the displayed area bottom righthand corner Y value is returned in this parameter
   */
  void getDisplayedArea(Sint32& tlhcX, Sint32& tlhcY, Sint32& brhcX, Sint32& brhcY);

  /** gets the presentation pixel spacing for this displayed area if present.
   *  @param x the horizontal pixel spacing (mm) is returned in this parameter upon success
   *  @param y the vertical pixel spacing (mm) is returned in this parameter upon success
   *  @return EC_Normal if successful, an error code if no presentation pixel spacing is available.
   */
  OFCondition getPresentationPixelSpacing(double& x, double& y);
  
  /** gets the presentation pixel magnification ratio for this displayed area if present.
   *  @param magnification the magnification ratio is returned in this parameter upon success
   *  @return EC_Normal if successful, an error code if no magnification ratio is available.
   */
  OFCondition getPresentationPixelMagnificationRatio(double& magnification);

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
   *  which means that the current reference becomes "global". This case must be handled by the caller.
   *  @param allReferences list of series/instance references registered for the presentation state.
   *  @param instanceUID SOP instance UID of the current image
   *  @param frame number of the current frame
   *  @param numberOfFrames the number of frames of the current image   
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
    
  /** checks if the TRUE SIZE mode is possible, i.e.
   *  if pixel spacing is known.
   *  @return OFTrue if TRUE SIZE mode is available, OFFalse otherwise.
   */
  OFBool canUseTrueSize();
  
  /** sets the displayed area pixel spacing and
   *  removes any pixel aspect ratio setting.
   *  @param spacingX horizontal pixel spacing in mm
   *  @param spacingY vertical pixel spacing in mm
   *  @return EC_Normal if successful, an error code otherwise.
   */   
  OFCondition setDisplayedAreaPixelSpacing(double spacingX, double spacingY);

  /** sets the displayed area pixel spacing and
   *  removes any pixel aspect ratio setting.
   *  @param spacing vertical/horizontal spacing in DICOM DS format.
   *  @return EC_Normal if successful, an error code otherwise.
   */   
  OFCondition setDisplayedAreaPixelSpacing(const char *spacing);
  
  /** sets the displayed area pixel spacing and
   *  removes any pixel spacing setting.
   *  Pixel aspect ratio is defined here as the width of a pixel divided
   *  by the height of a pixel (x/y).
   *  @param ratio pixel aspect ratio
   *  @return EC_Normal if successful, an error code otherwise.
   */   
  OFCondition setDisplayedAreaPixelAspectRatio(double ratio);

  /** sets the displayed area pixel spacing and
   *  removes any pixel spacing setting.
   *  @param ratio pixel aspect ratio in DICOM IS format.
   *  @return EC_Normal if successful, an error code otherwise.
   */   
  OFCondition setDisplayedAreaPixelAspectRatio(const char *ratio);
 
  /** sets the displayed area and size mode.
   *  @param sizeMode presentation size mode.
   *  @param tlhcX displayed area top lefthand corner X
   *  @param tlhcY displayed area top lefthand corner Y
   *  @param brhcX displayed area bottom righthand corner X
   *  @param brhcY displayed area bottom righthand corner Y
   *  @param magnification magnification factor - ignored unless
   *    sizeMode==DVPSD_magnify.
   *  @return EC_Normal if successful, an error code otherwise.
   */   
  OFCondition setDisplayedArea(
    DVPSPresentationSizeMode sizeMode,
    Sint32 tlhcX, Sint32 tlhcY, 
    Sint32 brhcX, Sint32 brhcY,
    double magnification=1.0);

  /** adjusts the displayed area coordinates for the rotation and flipping
   *  status of the image.
   *  @param rotationFrom previous rotation
   *  @param isFlippedFrom previous flip status
   *  @param rotationTo new rotation
   *  @param isFlippedTo new flip status
   */
  void rotateAndFlip(
    DVPSRotationType rotationFrom, 
    OFBool isFlippedFrom,
    DVPSRotationType rotationTo, 
    OFBool isFlippedTo);

private:
  /** undefined private assignment operator
   */
  DVPSDisplayedArea& operator=(const DVPSDisplayedArea& source);

  /// swaps the horizontal (X) components of TLHC and BRHC
  void switchHorizontalCorners();

  /// swaps the vertical (Y) components of TLHC and BRHC
  void switchVerticalCorners();

  /** adjusts the displayed area coordinates for rotation and flipping.
   *  This method can either be used to adjust from unrotated/unflipped
   *  to a rotated/flipped status or back from this status to original.
   *  @param rotation rotation
   *  @param isFlipped flip status
   */   
  void rotateAndFlipFromOrTo(DVPSRotationType rotation, OFBool isFlipped);

  /// ReferencedImageSequence, Type 1c
  DVPSReferencedImage_PList referencedImageList;
  /// VR=SL, VM=2, Type 1 
  DcmSignedLong            displayedAreaTopLeftHandCorner;
  /// VR=SL, VM=2, Type 1 
  DcmSignedLong            displayedAreaBottomRightHandCorner;
  /// VR=CS, VM=1, Type 1 
  DcmCodeString            presentationSizeMode;
  /// VR=DS, VM=2, Type 1c (required if presentationSizeMode is "TRUE SIZE")
  DcmDecimalString         presentationPixelSpacing;
  /// VR=IS, VM=2, Type 1c (required if presentationPixelSpacing is absent)
  DcmIntegerString         presentationPixelAspectRatio;
  /// VR=FL, VM=1, Type 1c (required if presentationSizeMode is "MAGNIFY")
  DcmFloatingPointSingle   presentationPixelMagnificationRatio;

};

#endif
