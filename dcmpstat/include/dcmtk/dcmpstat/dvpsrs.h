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
 *    classes: DVPSReferencedSeries
 *
 */

#ifndef DVPSRS_H
#define DVPSRS_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsril.h"   /* for DVPSReferencedImage_PList */
#include "dcmtk/dcmdata/dcvrae.h"
#include "dcmtk/dcmdata/dcvrsh.h"
#include "dcmtk/dcmdata/dcvrui.h"

/** an item of the referenced series sequence in a presentation state (internal use only).
 *  This class manages the data structures comprising one item
 *  of the Referenced Series Sequence in a Presentation State object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSReferencedSeries
{
public:
  /// default constructor
  DVPSReferencedSeries();

  /// copy constructor
  DVPSReferencedSeries(const DVPSReferencedSeries& copy);

  /** clone method.
   *  @return a pointer to a new DVPSReferencedSeries object containing
   *  a deep copy of this object.
   */
  DVPSReferencedSeries *clone() { return new DVPSReferencedSeries(*this); }

  /// destructor
  virtual ~DVPSReferencedSeries();

  /** reads an series reference from a DICOM dataset.
   *  The DICOM elements of the referenced series item are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the item of the ReferencedSeriesSequence from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);

  /** writes the series reference managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the ReferencedSeriesSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

  /** check if the passed SOP Class UID is equal to the ones stored in this object.
   *  This method checks whether this object contains at least one image reference
   *  and whether all image references use the same SOP class UID as the one passed.
   *  If sopclassuid is empty, the referencedSOPClassUID of the first image reference
   *  is assigned to it.
   *  @param sopclassuid the SOP class UID to be validated
   *  @return OFTrue if successful, OFFalse if unsuccessful.
   */
  OFBool isValid(OFString& sopclassuid);

  /** compare SOP Instance UID.
   *  @param uid the UID to be compared
   *  @return OFTrue if the SeriesInstanceUID of this object is
   *    equal to uid, OFFalse otherwise.
   */
  OFBool isSeriesUID(const char *uid);


  /** checks if an image reference with the given SOP instance UID exists
   *  in the ReferencedImageSequence of this series reference.
   *  @param sopinstanceuid the SOP instance UID of the searched image reference
   *  @return a pointer to the matching DVPSReferencedImage if found, NULL otherwise.
   */
  DVPSReferencedImage *findImageReference(const char *sopinstanceuid);

  /** remove image reference from this series reference if present.
   *  If an image reference with a SOP instance UID corresponding to the
   *  passed UID is found, it is removed from this series reference.
   *  @param uid a pointer to the image SOP instance UID.
   */
  void removeImageReference(const char *sopinstanceuid);

  /** set Series Instance UID of this series reference.
   *  @param uid a pointer to the UID, which is copied into this object.
   */
  void setSeriesInstanceUID(const char *uid);

  /** set the optional retrieve location for this series reference.
   *  @param aetitle the retrieveAETitle, must be a valid DICOM 'AE' value. Default: value absent.
   *  @param filesetID the storageMediaFileSetID, must be a valid DICOM 'SH' value. Default: value absent.
   *  @param filesetUID the storageMediaFileSetUID, must be a valid DICOM UID. Default: value absent.
   */
  void setRetrieveLocation(const char *aetitle=NULL, const char *filesetID=NULL, const char *filesetUID=NULL);

   /** gets the retrieveAETitle for this series reference.
    *  @return retrieveAETitle if present, NULL otherwise.
    */
  const char *getRetrieveAETitle();

   /** gets the storageMediaFileSetID for this series reference.
    *  @return storageMediaFileSetID if present, NULL otherwise.
    */
  const char *getStorageMediaFileSetID();

   /** gets the storageMediaFileSetUID for this series reference.
    *  @return storageMediaFileSetUID if present, NULL otherwise.
    */
  const char *getStorageMediaFileSetUID();

  /** add image reference to this series reference.
   *  A new image reference for this series is created.
   *  The image SOP instance UID must be unique (must not yet exist
   *  as a reference in this series), otherwise an error code is returned.
   *  @param sopclassUID a pointer to the Image SOP Class UID, which is copied into
   *    the new image reference
   *  @param instanceUID a pointer to the Image SOP Instance UID, which is copied into
   *    the new image reference
   *  @param frames a list of frame numbers in DICOM IS format
   *    (integer numbers separated by '\' characters). Default: frame numbers absent.
   *    The frame numbers are required if the referenced image is a multiframe image.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addImageReference(
    const char *sopclassUID,
    const char *instanceUID,
    const char *frames=NULL);

  /** gets the number of image references in this object.
   *  @return the number of image references.
   */
  size_t numberOfImageReferences() const { return referencedImageList.size(); }

  /** gets an image reference with the given index.
   *  @param idx index, must be < numberOfImageReferences().
   *  @param seriesUID the Series Instance UID is returned in this string
   *  @param sopclassUID the SOP Class UID is returned in this string
   *  @param instanceUID the SOP Instance UID is returned in this string
   *  @param frames the list of frames is returned in this string
   *  @param aetitle the series retrieveAETitle is returned in this string
   *  @param filesetID the series storageMediaFileSetID is returned in this string
   *  @param filesetUID the series storageMediaFileSetUID is returned in this string
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition getImageReference(
    size_t idx,
    OFString& seriesUID,
    OFString& sopclassUID,
    OFString& instanceUID,
    OFString& frames,
    OFString& aetitle,
    OFString& filesetID,
    OFString& filesetUID);

private:

  /// private undefined assignment operator
  DVPSReferencedSeries& operator=(const DVPSReferencedSeries&);

  /// Referenced Image Sequence
  DVPSReferencedImage_PList referencedImageList;
  /// VR=UI, VM=1, Type 1c
  DcmUniqueIdentifier       seriesInstanceUID;
  /// VR=AE, VM=1, Type 3
  DcmApplicationEntity      retrieveAETitle;
  /// VR=SH, VM=1, Type 3
  DcmShortString            storageMediaFileSetID;
  /// VR=UI, VM=1, Type 3
  DcmUniqueIdentifier       storageMediaFileSetUID;

};

#endif
