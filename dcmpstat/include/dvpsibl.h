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
 *  Update Date:      $Date: 1999-07-30 13:34:48 $
 *  CVS/RCS Revision: $Revision: 1.1 $
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

class DVPSImageBoxContent_PList: private OFList<DVPSImageBoxContent *>
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
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);
  
  /** writes the list of image boxes managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the ImageBoxContentSequence is written
   *  @param numItems the number of items (from the beginning of the list) to be written.
   *    Default: all items are written.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset, size_t numItems=0);

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
   
};


#endif

/*
 *  $Log: dvpsibl.h,v $
 *  Revision 1.1  1999-07-30 13:34:48  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */

