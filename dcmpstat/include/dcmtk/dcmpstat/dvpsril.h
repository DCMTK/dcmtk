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
 *    classes: DVPSReferencedImage_PList
 *
 */

#ifndef DVPSRIL_H
#define DVPSRIL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmpstat/dvpstyp.h"     /* for enum types */
#include "dcmtk/dcmpstat/dpdefine.h"

class DVPSReferencedImage;
class DVPSReferencedSeries_PList;

/** the list of referenced images contained in a presentation state (internal use only).
 *  This class manages the data structures comprising one complete
 *  Reference Image Sequence which is contained in one item
 *  of the Referenced Series Sequence in a Presentation State object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSReferencedImage_PList
{
public:
  /// default constructor
  DVPSReferencedImage_PList();

  /// copy constructor
  DVPSReferencedImage_PList(const DVPSReferencedImage_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSReferencedImage_PList object containing
   *  a deep copy of this object.
   */
  DVPSReferencedImage_PList *clone() { return new DVPSReferencedImage_PList(*this); }

  /// destructor
  virtual ~DVPSReferencedImage_PList();

  /** reads a list of image references (ReferencedImageSequence) from a DICOM dataset.
   *  The DICOM elements of the image references item are copied from the dataset to this object.
   *  The completeness of all items (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset from which the sequence is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);

  /** writes the list of image references managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the ReferencedImageSequence is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** checks if images are referenced within this ReferencedImageSequence
   *  and whether all images share the same SOP class UID.
   *  @param sopclassuid should be an empty string when called.
   *    Returns the SOP class UID of the images in the list.
   *  @return OFTrue if the sequence is valid (correct), OFFalse otherwise.
   */
  OFBool isValid(OFString& sopclassuid);

  /** checks if an image reference with the given SOP instance UID exists
   *  in this ReferencedImageSequence.
   *  @param sopinstanceuid the SOP instance UID of the searched image reference
   *  @return a pointer to the matching DVPSReferencedImage if found, NULL otherwise.
   */
  DVPSReferencedImage *findImageReference(const char *sopinstanceuid);

  /** update the reference to the given image such that
   *  the given frame is not referenced any more.
   *  @param sopinstanceuid the SOP instance UID of the image reference
   *  @param frame the frame reference
   *  @param numberOfFrames the number of frames of the image reference
   */
  void removeFrameReference(const char *sopinstanceuid, unsigned long frame, unsigned long numberOfFrames);

  /** checks if an image reference with the given SOP instance UID exists
   *  in this ReferencedImageSequence and deletes it.
   *  @param sopinstanceuid the SOP instance UID of the image reference to be removed.
   */
  void removeImageReference(const char *sopinstanceuid);

  /** add a new image reference.
   *  Checks if the referenced SOP instance UID already exists in this sequence.
   *  If it exists, an error code is returned. Otherwise a new image reference
   *  is created and added to the ReferencedImageSequence.
   *  @param sopclassUID the SOP class UID of the image reference to be added.
   *  @param instanceUID the SOP instance UID of the image reference to be added.
   *  @param frames a list of frame numbers in DICOM IS format
   *    (integer numbers separated by '\' characters). Default: frame numbers absent.
   *    The frame numbers are required if the referenced image is a multiframe image.
   */
  OFCondition addImageReference(
    const char *sopclassUID,
    const char *instanceUID,
    const char *frames=NULL);

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

  /** gets the number of image references in this list.
   *  @return the number of image references.
   */
  size_t size() const { return list_.size(); }

  /** gets an image reference with the given index.
   *  @param idx index, must be < size().
   *  @param sopclassUID the SOP Class UID is returned in this string
   *  @param instanceUID the SOP Instance UID is returned in this string
   *  @param frames the list of frames is returned in this string
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition getImageReference(
    size_t idx,
    OFString& sopclassUID,
    OFString& instanceUID,
    OFString& frames);

  /** checks if the object containing this list of image references
   *  is applicable to the given image and frame.
   *  @param instanceUID SOP instance UID of the current image
   *  @param frame number of the current frame
   *  @return OFTrue if applicable.
   */
  OFBool isApplicable(const char *instanceUID, unsigned long frame);

  /** checks if the object containing this list of image references
   *  matches exactly the applicability
   *  defined by the instanceUID, frame and applicability parameters.
   *  @param instanceUID SOP instance UID of the current image
   *  @param frame number of the current frame
   *  @return OFTrue if matching.
   */
  OFBool matchesApplicability(const char *instanceUID, unsigned long frame, DVPSObjectApplicability applicability);

private:

  /// private undefined assignment operator
  DVPSReferencedImage_PList& operator=(const DVPSReferencedImage_PList&);

  /** the list maintained by this object
   */
  OFList<DVPSReferencedImage *> list_;

};


#endif
