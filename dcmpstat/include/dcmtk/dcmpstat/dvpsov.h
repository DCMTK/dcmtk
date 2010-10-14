/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:36 $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DVPSOV_H
#define DVPSOV_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctk.h"

class DicomImage;

/** an overlay contained in a presentation state (internal use only).
 *  This class manages the data structures comprising a single overlay
 *  (one instance of the Overlay Plane Module repeating elements)
 *  that is contained in a Presentation State object.
 */

class DVPSOverlay
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

/*
 *  $Log: dvpsov.h,v $
 *  Revision 1.13  2010-10-14 13:16:36  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.12  2010-10-07 14:31:36  joergr
 *  Removed leading underscore characters from preprocessor symbols (reserved).
 *
 *  Revision 1.11  2009-11-24 14:12:57  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.10  2005-12-08 16:03:53  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.9  2003/08/27 14:59:41  meichel
 *  Changed API of class DVPSOverlay to avoid dependency on module dcmimgle
 *
 *  Revision 1.8  2001/09/26 15:36:13  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.7  2001/06/01 15:50:18  meichel
 *  Updated copyright header
 *
 *  Revision 1.6  2000/06/02 16:00:48  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.5  2000/03/08 16:28:53  meichel
 *  Updated copyright header.
 *
 *  Revision 1.4  1999/02/09 15:58:56  meichel
 *  Implemented bitmap shutter activation amd method for
 *    exchanging graphic layers.
 *
 *  Revision 1.3  1998/12/22 17:57:06  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *  Revision 1.2  1998/12/14 16:10:31  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:30  meichel
 *  Initial Release.
 *
 *
 */
