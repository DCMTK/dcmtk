/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSReferencedImage
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:31 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSRI_H__
#define __DVPSRI_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctk.h"

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
  E_Condition read(DcmItem &dset);
  
  /** writes the image reference managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the ReferencedImageSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);
  
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
    
  /** set the frame number of this image reference.
   *  This method does not allow to set multiple frame numbers.
   *  @param frame a frame number
   */
  void setFrameNumber(Sint32 frame);

  /** compare SOP Instance UID.
   *  @param uid the UID to be compared
   *  @return OFTrue if the referencedSOPInstanceUID of this object is
   *    equal to uid, OFFalse otherwise.
   */
  OFBool isSOPInstanceUID(const char *uid);

private:
  /// VR=UI, VM=1, Type 1c
  DcmUniqueIdentifier      referencedSOPClassUID;
  /// VR=UI, VM=1, Type 1c
  DcmUniqueIdentifier      referencedSOPInstanceUID;
  /// VR=IS, VM=1-n, Type 1c
  DcmIntegerString         referencedFrameNumber;
};

#endif

/*
 *  $Log: dvpsri.h,v $
 *  Revision 1.1  1998-11-27 14:50:31  meichel
 *  Initial Release.
 *
 *
 */
