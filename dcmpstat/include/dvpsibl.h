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
 *    classes: DVPSImageBoxContent_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-09-10 12:46:47 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSIBL_H__
#define __DVPSIBL_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oflist.h"
#include "dctk.h"
#include "dvpstyp.h"     /* for enum types */

class DVPSImageBoxContent;

/** the list of Image Boxes contained in a stored print object.
 *  This class manages the data structures comprising one complete
 *  Image Box Content Sequence in a Stored Print object.
 */

class DVPSImageBoxContent_PList: public OFList<DVPSImageBoxContent *>
{
public:
	E_Condition addImageBox(DVPSImageBoxContent *box);
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
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);
  
  /** writes the list of image boxes managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the ImageBoxContentSequence is written
   *  @param writeRequestedImageSize if false, the Requested Image Size attributes are not written,
   *    e. g. because they are not supported by the target printer.
   *  @param numItems the number of items (from the beginning of the list) to be written.
   *    Default: all items are written.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset, OFBool writeRequestedImageSize, size_t numItems=0);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** gets the number of image boxes in this list.
   *  @return the number of image boxes.
   */
  size_t size() const { return OFList<DVPSImageBoxContent *>::size(); }

  /** create default values for all missing type 1 elements.
   *  Called before a stored print object is written.
   *  @renumber if true, new imageBoxPosition values are created 
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition createDefaultValues(OFBool renumber);
  
  /** adds all image SOP classes referenced in the image box list to
   *  the given sequence. Duplicate entries are suppressed.
   *  @param seq sequence to be added to, should be a PrintManagementCapabilitiesSequence.
   *  @param numItems number of items of this list to be taken into account.
   *    Default: all items.
   *  @return EC_Normal if successful, an error code otherwise.
   */   
  E_Condition addImageSOPClasses(DcmSequenceOfItems& seq, size_t numItems=0);

  /** static helper method that checks whether the given sequence contains an
   *  item with a ReferencedSOPClassUID element that matches the given UID string.
   *  @param seq sequence to be searched, should be a PrintManagementCapabilitiesSequence.
   *  @param uid UID string
   *  @return OFTrue if found, OFFalse otherwise. Returns OFFalse if uid is NULL.
   */
  static OFBool haveReferencedUIDItem(DcmSequenceOfItems& seq, const char *uid);

  /** static helper method that adds an item to the given sequence. The item
   *  contains a ReferencedSOPClassUID element with the given UID string as value.
   *  @param seq sequence to be added to, should be a PrintManagementCapabilitiesSequence.
   *  @param uid UID string, must not be NULL
   *  @return EC_Normal if successful, an error code otherwise.
   */   
  static E_Condition addReferencedUIDItem(DcmSequenceOfItems& seq, const char *uid);
  
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
  E_Condition addImageBox(
    const char *instanceuid,
    const char *retrieveaetitle,
    const char *refstudyuid,
    const char *refseriesuid,
    const char *refsopclassuid,
    const char *refsopinstanceuid,
    const char *requestedimagesize=NULL,
    const char *patientid=NULL);
  
   /** adds the image to the Box with retrieve AETitle and Boxnumber set
   *  @image the printable image 
   *  @AETitle the title where we can get the image
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition addImage(DcmItem &image,char *aETitle);

  /** sets the (optional) requested decimate/crop behaviour
   *  for all image boxes managed by this object.
   *  @param value new enumerated value. The caller is responsible for
   *    making sure that the selected printer supports decimate/crop
   *    if a non-default value is set.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setRequestedDecimateCropBehaviour(DVPSDecimateCropBehaviour value); 

  /** deletes one of the registered images.
   *  @param idx index, must be < size()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition deleteImage(size_t idx);
  
  /** deletes multiple of the registered images, starting with the first one.
   *  @param number number of images to delete, must be <= size()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition deleteMultipleImages(size_t number);

  /** checks if one of the registered images has additional settings that are not
   *  default values on the image box level.
   *  @param idx index, must be < getNumberOfImages()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFBool imageHasAdditionalSettings(size_t idx);

  /** sets the (optional) magnification type for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setImageMagnificationType(size_t idx, const char *value);
  
  /** sets the (optional) smoothing type for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setImageSmoothingType(size_t idx, const char *value);
  
  /** sets the (optional) configuration information for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setImageConfigurationInformation(size_t idx, const char *value);

  /** sets magnification type, smoothing type and configuration information back to default
   *  for all registered images.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setAllImagesToDefault();

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

private:

  /** returns a pointer to the image box with the given
   *  index or NULL if it does not exist.
   *  @param idx index, must be < size()
   *  @return pointer to image box object or NULL
   */
  DVPSImageBoxContent *getImageBox(size_t idx); 
  
};


#endif

/*
 *  $Log: dvpsibl.h,v $
 *  Revision 1.7  1999-09-10 12:46:47  meichel
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

