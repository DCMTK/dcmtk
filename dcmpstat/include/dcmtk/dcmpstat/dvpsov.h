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
 *    classes: DVPSOverlay
 *
 */

#ifndef DVPSOV_H
#define DVPSOV_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmpstat/dpdefine.h"

class DicomImage;

/** an overlay contained in a presentation state (internal use only).
 *  This class manages the data structures comprising a single overlay
 *  (one instance of the Overlay Plane Module repeating elements)
 *  that is contained in a Presentation State object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSOverlay
{
public:
  /// default constructor
  DVPSOverlay();

  /// copy constructor
  DVPSOverlay(const DVPSOverlay& copy);

  /** clone method.
   *  @return a pointer to a new DVPSOverlay object containing
   *  a copy of this object.
   */
  DVPSOverlay *clone() const { return new DVPSOverlay(*this); }

  /// destructor
  virtual ~DVPSOverlay();

  /** reads the specified overlay group from a DICOM dataset.
   *  The DICOM elements of the Overlay Plane module are copied
   *  from the dataset to this object. The OverlayData element, which is
   *  optional in the Overlay Plane Module but required for presentation states,
   *  must be present.
   *  The completeness of the overlay plane (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset from which the overlay is to be read
   *  @param ovGroup the lower byte of the overlay group to be read
   *  @param asGroup the lower byte of the overlay group to be assigned to the
   *    overlay plane in the presentation state. Default: identical to ovGroup.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset, Uint8 ovGroup, Uint8 asGroup=0xFF);

  /** writes the overlay plane managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the overlay is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

  /** get group number of overlay repeating group managed by this object.
   *  @return the lower byte of the overlay group
   */
  Uint8 getOverlayGroup() const { return overlayGroup; }

  /** sets the group number for the overlay repeating group managed
   *  by this object.
   *  @param newGroup lower byte of the repeating group number.
   */
  void setOverlayGroup(Uint8 newGroup) { overlayGroup = newGroup; }

  /** checks if the overlay is suitable as a bitmap shutter
   *  for an image with the given image size. Checks overlay type,
   *  origin and size.
   *  @param x image width in pixels
   *  @param y image height in pixels
   *  @return OFTrue if the specified overlay group matches the image size.
   */
  OFBool isSuitableAsShutter(unsigned long x, unsigned long y);

  /** gets the overlay label if present.
   *  If the label string is absent, this method returns NULL.
   *  @return overlay label
   */
  const char *getOverlayLabel();

  /** gets the overlay description if present.
   *  If the label string is absent, this method returns NULL.
   *  @return overlay description
   */
  const char *getOverlayDescription();

  /** checks whether this overlay is ROI type.
   *  @return OFTrue if overlay is ROI, OFFalse if overlay is Graphic.
   */
  OFBool isROI();

  /** retrieve origin and size values for this overlay
   *  @param originX horizontal origin returned in this parameter
   *  @param originY vertical origin returned in this parameter
   *  @param sizeX horizontal size returned in this parameter
   *  @param sizeY vertical size returned in this parameter
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition getValues(
    Sint16& originX,
    Sint16& originY,
    Uint16& sizeX,
    Uint16& sizeY);

  /// return reference to overlay data
  DcmOverlayData& getData() { return overlayData; }

  /// return reference to overlay label
  DcmLongString& getLabel() { return overlayLabel; }

  /// return reference to overlay description
  DcmLongString& getDescription() { return overlayDescription; }

private:

  /// private undefined assignment operator
  DVPSOverlay& operator=(const DVPSOverlay&);

  /// lower byte of the overlay repeating group managed by this object
  Uint8                    overlayGroup;
  /// VR=US, VM=1, Type=1
  DcmUnsignedShort         overlayRows;
  /// VR=US, VM=1, Type=1
  DcmUnsignedShort         overlayColumns;
  /// VR=CS, VM=1, Type=1
  DcmCodeString            overlayType;
  /// VR=SS, VM=2, Type=1
  DcmSignedShort           overlayOrigin;
  /// VR=US, VM=1, Type=1
  DcmUnsignedShort         overlayBitsAllocated;
  /// VR=US, VM=1, Type=1
  DcmUnsignedShort         overlayBitPosition;
  /// VR=OW, VM=1, Type=1C
  DcmOverlayData           overlayData;
  /// VR=LO, VM=1, Type=3
  DcmLongString            overlayDescription;
  /// VR=LO, VM=1, Type=3
  DcmLongString            overlayLabel;

};

#endif
