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
 *    classes: DVPSImageBoxContent
 *
 *  Last Update:      $Author: thiel $
 *  Update Date:      $Date: 1999-08-26 09:30:59 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSIB_H__
#define __DVPSIB_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dctk.h"
#include "dvpstyp.h"     /* for enum types */

/** the representation of a Image Box Content SQ item for Stored Print
 */  

class DVPSImageBoxContent
{
public:
 /// default constructor
  DVPSImageBoxContent();
  
  /// copy constructor
  DVPSImageBoxContent(const DVPSImageBoxContent& copy);

  /** clone method.
   *  @return a pointer to a new DVPSImageBoxContent object containing
   *  a copy of this object.
   */
  DVPSImageBoxContent *clone() { return new DVPSImageBoxContent(*this); }

  /// destructor
  virtual ~DVPSImageBoxContent();

  /** reads an image box content item from a DICOM dataset.
   *  The DICOM elements of the referenced item are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the item of the ImageBoxContentSequence from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);
  
  /** writes the image box managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the ImageBoxContentSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);

  /** create default values for all missing type 1 elements.
   *  Called before a stored print object is written.
   *  @renumber if true, a new imageBoxPosition values is created 
   *  @number new imageBoxPosition to be assigned
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition createDefaultValues(OFBool renumber, unsigned long number);
  
  /** returns the referencedSOPClassUID from the ReferencedImageSequence
   *  @return referencedSOPClassUID string
   */
  const char *getSOPClassUID();

  /** adds the image to the Box with retrieve AETitle and Boxnumber set
   *  @image the printable image 
   *  @AETitle the title where we can get the image
   *  @number imageBoxPosition to be assigned
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition addImage(DcmItem &image,char *AETitle, unsigned long number);
 
private:
  /// private undefined assignment operator
  DVPSImageBoxContent& operator=(const DVPSImageBoxContent&);

  /// Module=Image_Box_List, VR=UI, VM=1, Type 1(c) 
  DcmUniqueIdentifier      sOPInstanceUID;
  /// Module=Image_Box_List, VR=US, VM=1, Type 1 
  DcmUnsignedShort         imageBoxPosition;
  /// Module=Image_Box_List, VR=CS, VM=1, Type 2
  DcmCodeString            polarity;
  /// Module=Image_Box_List, VR=CS, VM=1, Type 3
  DcmCodeString            magnificationType;
  /// Module=Image_Box_List, VR=ST, VM=1, Type 3
  DcmShortText             configurationInformation;
  /// Module=Image_Box_List, VR=CS, VM=1, Type 3
  DcmCodeString            smoothingType;
  /// Module=Image_Box_List, VR=DS, VM=1, Type 3
  DcmDecimalString         requestedImageSize;
  /// Module=Image_Box_List (Supplement 38), VR=CS, VM=1, Type 3
  DcmCodeString            requestedDecimateCropBehavior;

  /* the following attributes belong to the ReferencedImageSequence
   * which must have exactly one item here.
   */
  /// Module=Image_Box_List, VR=AE, VM=1-n, Type 1 
  DcmApplicationEntity     retrieveAETitle;
  /// Module=Image_Box_List, VR=UI, VM=1, Type 1 
  DcmUniqueIdentifier      referencedSOPClassUID;
  /// Module=Image_Box_List, VR=UI, VM=1, Type 1 
  DcmUniqueIdentifier      referencedSOPInstanceUID;
  /// Module=Image_Box_List, VR=UI, VM=1, Type 1 
  DcmUniqueIdentifier      studyInstanceUID;
  /// Module=Image_Box_List, VR=UI, VM=1, Type 1 
  DcmUniqueIdentifier      seriesInstanceUID;
  /// Module=Image_Box_List, VR=IS, VM=1, Type 1c
  DcmIntegerString         referencedFrameNumber;
  /// Module=Image_Box_List, VR=LO, VM=1, Type 2
  DcmLongString            patientID;

  // we do not support the ReferencedImageOverlayBoxSequence which is retired in Supplement 35 anyway.
  // we do not support the ReferencedPresentationLUTSequence at Image Box level.

};

#endif

/*
 *  $Log: dvpsib.h,v $
 *  Revision 1.2  1999-08-26 09:30:59  thiel
 *  Add extensions for the usage of the StoredPrint
 *
 *  Revision 1.1  1999/07/30 13:34:47  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */

