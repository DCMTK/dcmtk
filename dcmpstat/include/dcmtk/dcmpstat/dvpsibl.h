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
 *    classes: DVPSImageBoxContent_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:36 $
 *  CVS/RCS Revision: $Revision: 1.28 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DVPSIBL_H
#define DVPSIBL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmpstat/dvpstyp.h"     /* for enum types */
#include "dcmtk/dcmdata/dcvrui.h"

class DVPSImageBoxContent;
class DVPSPresentationLUT_PList;

/** the list of Image Boxes contained in a stored print object.
 *  This class manages the data structures comprising one complete
 *  Image Box Content Sequence in a Stored Print object.
 */

class DVPSImageBoxContent_PList
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

/*
 *  $Log: dvpsibl.h,v $
 *  Revision 1.28  2010-10-14 13:16:36  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.27  2010-10-07 14:31:36  joergr
 *  Removed leading underscore characters from preprocessor symbols (reserved).
 *
 *  Revision 1.26  2010-03-01 09:08:49  uli
 *  Removed some unnecessary include directives in the headers.
 *
 *  Revision 1.25  2009-11-24 14:12:57  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.24  2009-09-30 10:42:38  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.23  2005-12-08 16:03:51  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.22  2003/06/04 10:18:06  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.21  2001/09/26 15:36:13  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.20  2001/06/01 15:50:18  meichel
 *  Updated copyright header
 *
 *  Revision 1.19  2000/07/04 15:58:02  joergr
 *  Added support for overriding the presentation LUT settings made for the
 *  image boxes.
 *
 *  Revision 1.18  2000/06/14 11:28:14  joergr
 *  Added methods to access the attributes Polarity and Requested Image Size.
 *
 *  Revision 1.17  2000/06/08 10:44:29  meichel
 *  Implemented Referenced Presentation LUT Sequence on Basic Film Session level.
 *    Empty film boxes (pages) are not written to file anymore.
 *
 *  Revision 1.16  2000/06/07 14:17:41  joergr
 *  Added methods to access the image polarity attribute.
 *
 *  Revision 1.15  2000/06/02 16:00:48  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.14  2000/05/31 12:56:39  meichel
 *  Added initial Print SCP support
 *
 *  Revision 1.13  2000/03/08 16:28:53  meichel
 *  Updated copyright header.
 *
 *  Revision 1.12  1999/10/19 14:46:03  meichel
 *  added support for the Basic Annotation Box SOP Class
 *    as well as access methods for Max Density and Min Density.
 *
 *  Revision 1.11  1999/10/07 17:21:48  meichel
 *  Reworked management of Presentation LUTs in order to create tighter
 *    coupling between Softcopy and Print.
 *
 *  Revision 1.10  1999/09/24 15:23:45  meichel
 *  Print spooler (dcmprtsv) now logs diagnostic messages in log files
 *    when operating in spool mode.
 *
 *  Revision 1.9  1999/09/17 14:33:57  meichel
 *  Completed print spool functionality including Supplement 22 support
 *
 *  Revision 1.8  1999/09/15 17:43:28  meichel
 *  Implemented print job dispatcher code for dcmpstat, adapted dcmprtsv
 *    and dcmpsprt applications.
 *
 *  Revision 1.7  1999/09/10 12:46:47  meichel
 *  Added implementations for a number of print API methods.
 *
 *  Revision 1.6  1999/09/09 14:57:33  thiel
 *  Added methods for print spooler
 *
 *  Revision 1.5  1999/09/01 16:14:40  meichel
 *  Added support for requested image size to print routines
 *
 *  Revision 1.4  1999/08/31 14:09:11  meichel
 *  Added get/set methods for stored print attributes
 *
 *  Revision 1.3  1999/08/27 15:57:56  meichel
 *  Added methods for saving hardcopy images and stored print objects
 *    either in file or in the local database.
 *
 *  Revision 1.2  1999/08/26 09:30:59  thiel
 *  Add extensions for the usage of the StoredPrint
 *
 *  Revision 1.1  1999/07/30 13:34:48  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */
