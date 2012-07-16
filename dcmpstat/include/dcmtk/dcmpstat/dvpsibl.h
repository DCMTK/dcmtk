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
 *    classes: DVPSImageBoxContent_PList
 *
 */

#ifndef DVPSIBL_H
#define DVPSIBL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpstyp.h"     /* for enum types */
#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcvrui.h"

class DVPSImageBoxContent;
class DVPSPresentationLUT_PList;

/** the list of Image Boxes contained in a stored print object.
 *  This class manages the data structures comprising one complete
 *  Image Box Content Sequence in a Stored Print object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSImageBoxContent_PList
{
public:

  /// default constructor
  DVPSImageBoxContent_PList();

  /// copy constructor
  DVPSImageBoxContent_PList(const DVPSImageBoxContent_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSImageBoxContent_PList object containing
   *  a deep copy of this object.
   */
  DVPSImageBoxContent_PList *clone() { return new DVPSImageBoxContent_PList(*this); }

  /// destructor
  virtual ~DVPSImageBoxContent_PList();

  /** reads a list of image boxes (ImageBoxContentSequence) from a DICOM dataset.
   *  The DICOM elements of the image boxes are copied from the dataset to this object.
   *  The completeness of all items (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset from which the sequence is to be read
   *  @param presentationLUTList list of presentation LUTs which may be referenced
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset, DVPSPresentationLUT_PList& presentationLUTList);

  /** writes the list of image boxes managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the ImageBoxContentSequence is written
   *  @param writeRequestedImageSize if false, the Requested Image Size attributes are not written,
   *    e.g. because they are not supported by the target printer.
   *  @param numItems the number of items (from the beginning of the list) to be written.
   *    Default: all items are written.
   *  @param ignoreEmptyImages if true, all image boxes without image box position are ignored
   *    when writing.
   *  @param writeReferencedPLUTSQ if false, the Referenced Presentation LUT Sequence is not written,
   *    e.g. because a "general" presentation LUT is used instead of those referenced by the image boxes.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(
    DcmItem &dset,
    OFBool writeRequestedImageSize,
    size_t numItems,
    OFBool ignoreEmptyImages,
    OFBool writeReferencedPLUTSQ = OFTrue);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** gets the number of image boxes in this list.
   *  @return the number of image boxes.
   */
  size_t size() const { return list_.size(); }

  /** create default values for all missing type 1 elements.
   *  Called before a stored print object is written.
   *  @param renumber if true, new imageBoxPosition values are created
   *  @param ignoreEmptyImages if true, an empty image box position does not cause an error.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition createDefaultValues(OFBool renumber, OFBool ignoreEmptyImages);

  /** adds all image SOP classes referenced in the image box list to
   *  the given sequence. Duplicate entries are suppressed.
   *  @param seq sequence to be added to, should be a PrintManagementCapabilitiesSequence.
   *  @param numItems number of items of this list to be taken into account.
   *    Default: all items.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addImageSOPClasses(DcmSequenceOfItems& seq, size_t numItems=0);

  /** creates a new image box object and sets the content of this image box object.
   *  @param instanceuid SOP instance UID of this image box
   *  @param retrieveaetitle retrieve AETITLE of the referenced image
   *  @param refstudyuid study instance UID of the referenced image
   *  @param refseriesuid series instance UID of the referenced image
   *  @param refsopclassuid SOP class UID of the referenced image
   *  @param refsopinstanceuid SOP instance UID of the referenced image
   *  @param requestedimagesize requested images size for this image, default: absent
   *  @param patientid patient ID for the referenced image, default: absent
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addImageBox(
    const char *instanceuid,
    const char *retrieveaetitle,
    const char *refstudyuid,
    const char *refseriesuid,
    const char *refsopclassuid,
    const char *refsopinstanceuid,
    const char *requestedimagesize,
    const char *patientid,
    const char *presentationlutuid);

  /** adds the given image box object to the list managed by this object.
   *  @param box image box object to be added.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addImageBox(DVPSImageBoxContent *box);

  /** sets the (optional) requested decimate/crop behaviour
   *  for all image boxes managed by this object.
   *  @param value new enumerated value. The caller is responsible for
   *    making sure that the selected printer supports decimate/crop
   *    if a non-default value is set.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setRequestedDecimateCropBehaviour(DVPSDecimateCropBehaviour value);

  /** deletes one of the registered images.
   *  @param idx index, must be < size()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition deleteImage(size_t idx);

  /** deletes multiple of the registered images, starting with the first one.
   *  @param number number of images to delete, must be <= size()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition deleteMultipleImages(size_t number);

  /** checks if one of the registered images has additional settings that are not
   *  default values on the image box level.
   *  @param idx index, must be < getNumberOfImages()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFBool imageHasAdditionalSettings(size_t idx);

  /** sets the polarity for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value (NORMAL or REVERSE), may be NULL.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setImagePolarity(size_t idx, const char *value);

  /** sets the requested size for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value, may be NULL.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setImageRequestedSize(size_t idx, const char *value);

  /** sets the (optional) magnification type for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setImageMagnificationType(size_t idx, const char *value);

  /** sets the (optional) smoothing type for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setImageSmoothingType(size_t idx, const char *value);

  /** sets the (optional) configuration information for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setImageConfigurationInformation(size_t idx, const char *value);

  /** sets the SOP instance UID for the given image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value, must not be NULL.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setImageSOPInstanceUID(size_t idx, const char *value);

  /** sets magnification type, smoothing type and configuration information back to default
   *  for all registered images.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setAllImagesToDefault();

  /** gets the polarity for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @return polarity, may be NULL.
   */
  const char *getImagePolarity(size_t idx);

  /** gets the requested size for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @return requested size, may be NULL.
   */
  const char *getImageRequestedSize(size_t idx);

  /** gets the (optional) magnification type for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @return magnification type, may be NULL.
   */
  const char *getImageMagnificationType(size_t idx);

  /** gets the (optional) smoothing type for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @return smoothing type, may be NULL.
   */
  const char *getImageSmoothingType(size_t idx);

  /** gets the (optional) configuration information for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @return configuration information, may be NULL.
   */
  const char *getImageConfigurationInformation(size_t idx);

  /** gets the current SOP Instance UID for the given registered image box..
   *  @param idx index, must be < getNumberOfImages()
   *  @return SOP Instance UID, may be NULL.
   */
  const char *getSOPInstanceUID(size_t idx);

  /** gets the referenced Presentation LUT SOP Instance UID for the given registered image box..
   *  @param idx index, must be < getNumberOfImages()
   *  @return SOP Instance UID, may be NULL.
   */
  const char *getReferencedPresentationLUTInstanceUID(size_t idx);

  /** returns the image UIDs that are required to look up the referenced image in the database
   *  @param idx index, must be < getNumberOfImages()
   *  @param studyUID Study UID of the image
   *  @param seriesUID series UID of the image
   *  @param instanceUID instance UID of the image
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition getImageReference(size_t idx, const char *&studyUID, const char *&seriesUID, const char *&instanceUID);

  /** writes the attributes managed by the referenced object that are part of a
   *  basic grayscale image box N-SET request into the DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param idx index, must be < getNumberOfImages()
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition prepareBasicImageBox(size_t idx, DcmItem &dset);

  /** checks whether the given SOP instance UID is used as
   *  referenced Presentation LUT SOP Instance UID inside the list of
   *  image boxes managed by this object.
   *  @param uid name of UID
   *  @return OFTrue if UID is used, OFFalse otherwise.
   */
  OFBool presentationLUTInstanceUIDisUsed(const char *uid);

  /** if only one Presentation LUT is required to render all images
   *  managed by this object, returns the UID of this presentation LUT.
   *  Otherwise returns NULL.
   *  @param filmBox UID of Presentation LUT specified on Film Box
   *    level. May be NULL or empty string if absent.
   *  @return UID of Presentation LUT if found, NULL or empty string otherwise.
   */
  const char *haveSinglePresentationLUTUsed(const char *filmBox);

  /** creates a number of image boxes as part of a Print SCP N-CREATE operation.
   *  The previous content of the list is deleted.
   *  @param numBoxes number of boxes to be created
   *  @param studyUID study instance UID under which Hardcopy Grayscale images will be saved
   *  @param seriesUID series instance UID under which Hardcopy Grayscale images will be saved
   *  @param aetitle retrieve AE title for Hardcopy Grayscale images
   *  @return OFTrue if successful, OFFalse otherwise.
   */
  OFBool printSCPCreate(
    unsigned long numBoxes,
    DcmUniqueIdentifier& studyUID,
    DcmUniqueIdentifier& seriesUID,
    const char *aetitle);

  /** writes a Referenced Image Box Sequence for the image boxes
   *  managed by this object. Used in a Print SCP N-CREATE operation.
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition writeReferencedImageBoxSQ(DcmItem &dset);

  /** checks whether the given Presentation LUT type could be used together
   *  with all image boxes in this list on a Print SCP that requires a matching
   *  alignment between a Presentation LUT and the image pixel data.
   *  @param align LUT alignment type
   *  @return OFTrue if matching, OFFalse otherwise
   */
  OFBool matchesPresentationLUT(DVPSPrintPresentationLUTAlignment align) const;

  /** looks up the image box with the given SOP instance UID in this list
   *  and returns a pointer to a new object containing a copy of this
   *  image box. If the object is not found, NULL is returned.
   *  @param uid SOP instance UID of the image box to be looked up
   *  @return pointer to copied image box object, may be NULL.
   */
  DVPSImageBoxContent *duplicateImageBox(const char *uid);

  /** checks whether any of the image boxes managed by this list
   *  has the same position as the given one, but a different
   *  SOP instance UID.  This is used during an Print SCP N-SET operation
   *  to check whether a position clash exists.
   *  @param uid SOP instance UID of the image box to be looked up
   *  @param position image position to be looked up
   */
  OFBool haveImagePositionClash(const char *uid, Uint16 position);

  /** adds the given image box object to this list. Any other object existing
   *  in the list with the same SOP instance UID is removed.
   *  Used during a Print SCP image box N-SET operation.
   *  @param newImageBox new image box object to be added to the list.
   */
  void replace(DVPSImageBoxContent *newImageBox);

  /** checks whether any of the image boxes has an image box position
   *  assigned. If no image box position is assigned, the object list
   *  cannot be written and a Print SCP should return a warning
   *  status upon receipt of an N-ACTION request.
   *  @return OFTrue if empty page (no image box position assigned), OFFalse otherwise.
   */
  OFBool emptyPageWarning();

private:

  /** private undefined assignment operator
   */
  DVPSImageBoxContent_PList& operator=(const DVPSImageBoxContent_PList&);

  /** returns a pointer to the image box with the given
   *  index or NULL if it does not exist.
   *  @param idx index, must be < size()
   *  @return pointer to image box object or NULL
   */
  DVPSImageBoxContent *getImageBox(size_t idx);

  /** the list maintained by this object
   */
  OFList<DVPSImageBoxContent *> list_;

};


#endif
