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
 *    classes: DVPSReferencedImage_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:32 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSRIL_H__
#define __DVPSRIL_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oflist.h"
#include "dctk.h"

class DVPSReferencedImage;

/** the list of referenced images contained in a presentation state (internal use only).
 *  This class manages the data structures comprising one complete
 *  Reference Image Sequence which is contained in one item
 *  of the Referenced Series Sequence in a Presentation State object.
 */

class DVPSReferencedImage_PList: private OFList<DVPSReferencedImage *>
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
  E_Condition read(DcmItem &dset);
  
  /** writes the list of image references managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the ReferencedImageSequence is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);

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
   *  @param frame the frame number image reference to be added. Default: frame number absent.
   *    The frame number is required if the referenced image is a multiframe image.
   */
  E_Condition addImageReference(
    const char *sopclassUID,
    const char *instanceUID, 
    Sint32 frame=0);
};


#endif

/*
 *  $Log: dvpsril.h,v $
 *  Revision 1.1  1998-11-27 14:50:32  meichel
 *  Initial Release.
 *
 *
 */

