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
 *    classes: DVPSReferencedImage
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:36 $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DVPSRI_H
#define DVPSRI_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcvris.h"

/** an item of the referenced image sequence in a presentation state (internal use only).
 *  This class manages the data structures comprising one item
 *  of the Reference Image Sequence which is contained
 *  in the Referenced Series Sequence in a Presentation State object.
 */

class DVPSReferencedImage
{
public:
  /// default constructor
  DVPSReferencedImage();

  /// copy constructor
  DVPSReferencedImage(const DVPSReferencedImage& copy);

  /** clone method.
   *  @return a pointer to a new DVPSReferencedImage object containing
   *  a copy of this object.
   */
  DVPSReferencedImage *clone() { return new DVPSReferencedImage(*this); }

  /// destructor
  virtual ~DVPSReferencedImage();

  /** reads an image reference from a DICOM dataset.
   *  The DICOM elements of the referenced image item are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the item of the ReferencedImageSequence from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);

  /** writes the image reference managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the ReferencedImageSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

  /** check if the passed SOP Class UID is equal to the one stored in this object.
   *  If sopclassuid is empty, the referencedSOPClassUID is assigned to it and the method returns
   *  OFTrue. Otherwise the passed UID is compared with the referencedSOPClassUID.
   *  OFTrue is returned if they are equal, OFFalse otherwise.
   *  This method is used to check whether all image references in a presentation state
   *  share the same SOP Class UID.
   *  @param sopclassuid the SOP class UID to be validated
   *  @return OFTrue if successful, OFFalse if unsuccessful.
   */
  OFBool validateSOPClassUID(OFString& sopclassuid);

  /** set SOP Class UID of this image reference.
   *  @param uid a pointer to the UID, which is copied into this object.
   */
  void setSOPClassUID(const char *uid);

  /** set SOP Instance UID of this image reference.
   *  @param uid a pointer to the UID, which is copied into this object.
   */
  void setSOPInstanceUID(const char *uid);

  /** set the list of frame numbers of this image reference.
   *  @param frames a list of frame numbers in DICOM IS format
   *    (integer numbers separated by '\' characters)
   */
  void setFrameNumbers(const char *frames);

  /** compare SOP Instance UID.
   *  @param uid the UID to be compared
   *  @return OFTrue if the referencedSOPInstanceUID of this object is
   *    equal to uid, OFFalse otherwise.
   */
  OFBool isSOPInstanceUID(const char *uid);

  /** gets the image reference managed by this object.
   *  @param sopclassUID the SOP Class UID is returned in this string
   *  @param instanceUID the SOP Instance UID is returned in this string
   *  @param frames the list of frames is returned in this string
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition getImageReference(
    OFString& sopclassUID,
    OFString& instanceUID,
    OFString& frames);

  /** checks whether this image reference applies to the given frame number.
   *  An image reference applies to a frame if the frame number is explicitly
   *  listed in the referencedFrameNumber attribute or if the referencedFrameNumber
   *  is empty.
   *  @param frame frame number to be checked
   *  @return OFTrue if the image reference applies to the given frame number, OFFalse otherwise.
   */
  OFBool appliesToFrame(unsigned long frame);

  /** checks whether this image reference applies exactly to the given frame number.
   *  This is the case if the referencedFrameNumber only contains the given frame number.
   *  @param frame frame number to be checked
   *  @return OFTrue if the image reference applies only to the given frame number, OFFalse otherwise.
   */
  OFBool appliesOnlyToFrame(unsigned long frame);

  /** checks whether this image reference applies all frames
   *  because the referencedFrameNumber is empty.
   *  @return OFTrue if the image reference applies to all frames, OFFalse otherwise.
   */
  OFBool appliesToAllFrames();

  /** update the reference such that the given frame is not referenced any more.
   *  @param frame the frame reference
   *  @param numberOfFrames the number of frames of the image reference
   */
  void removeFrameReference(unsigned long frame, unsigned long numberOfFrames);

private:

  /** undefined private assignment operator
   */
  DVPSReferencedImage& operator=(const DVPSReferencedImage& source);

  /** flushes the frame cache.
   */
  void flushCache();
  /** updated the frame cache.
   */
  void updateCache();

  /// VR=UI, VM=1, Type 1c
  DcmUniqueIdentifier      referencedSOPClassUID;
  /// VR=UI, VM=1, Type 1c
  DcmUniqueIdentifier      referencedSOPInstanceUID;
  /// VR=IS, VM=1-n, Type 1c
  DcmIntegerString         referencedFrameNumber;
  /// if exists, contains binary representation of referencedFrameNumber
  Sint32 *frameCache;
  /// describes array size of frameCache
  unsigned long frameCacheEntries;

};

#endif

/*
 *  $Log: dvpsri.h,v $
 *  Revision 1.13  2010-10-14 13:16:36  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.12  2010-10-07 14:31:36  joergr
 *  Removed leading underscore characters from preprocessor symbols (reserved).
 *
 *  Revision 1.11  2009-11-24 14:12:57  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.10  2009-09-30 10:42:38  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.9  2005-12-08 16:03:59  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.8  2001/09/26 15:36:14  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.7  2001/06/01 15:50:20  meichel
 *  Updated copyright header
 *
 *  Revision 1.6  2000/06/02 16:00:50  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.5  2000/03/08 16:28:55  meichel
 *  Updated copyright header.
 *
 *  Revision 1.4  1999/07/22 16:39:09  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.3  1999/01/15 17:33:03  meichel
 *  added methods to DVPresentationState allowing to access the image
 *    references in the presentation state.  Also added methods allowing to
 *    get the width and height of the attached image.
 *
 *  Revision 1.2  1998/12/14 16:10:32  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:31  meichel
 *  Initial Release.
 *
 *
 */
