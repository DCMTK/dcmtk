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
 *    classes: DVPSStoredPrint
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-08-27 15:57:57 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DVPSSP_H__
#define __DVPSSP_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dctk.h"
#include "dvpstyp.h"     /* for enum types */
#include "dvpspl.h"      /* for class DVPSPresentationLUT */
#include "dvpsibl.h"     /* for class DVPSImageBoxContent_PList */
#include "dvpstat.h"		 /* for class DVPresentationState */
#include "dvpspr.h"			 /* for class DVPrintMessageHandler */

/** the representation of a Stored Print object
 */  

class DVPSStoredPrint
{
 public:
  /// default constructor
  DVPSStoredPrint();
  
  /// copy constructor
  DVPSStoredPrint(const DVPSStoredPrint& copy);

  /** clone method.
   *  @return a pointer to a new DVPSStoredPrint object containing
   *  a copy of this object.
   */
  DVPSStoredPrint *clone() { return new DVPSStoredPrint(*this); }

  /// destructor
  virtual ~DVPSStoredPrint();

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** reads a Stored Print object from a DICOM dataset.
   *  The DICOM elements of the stored print object are copied
   *  from the dataset to this object.
   *  The completeness of the object (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the dataset from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);
  
  /** writes the Stored Print object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the data is written
   *  @param limitImages if true, only the number of image references
   *    that are needed for the current image display format (film layout) are written.
   *    If false, all image references are written.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset, OFBool limitImages);
  
  /** writes the general study and series module attributes for a grayscale hardcopy image
   *  that is related to this stored print object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition writeHardcopyImageAttributes(DcmItem &dset);
  
  /** creates a new image box object and sets the content of this image box object.
   *  @param retrieveaetitle retrieve AETITLE of the referenced image
   *  @param refsopinstanceuid SOP instance UID of the referenced image
   *  @param requestedimagesize requested images size for this image, default: absent
   *  @param patientid patient ID for the referenced image, default: absent
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition addImageBox(
    const char *retrieveaetitle,
    const char *refsopinstanceuid,
    const char *requestedimagesize=NULL,
    const char *patientid=NULL);
  
  /** creates a Stored Print object from a DICOM dataset.
   *  The DICOM elements are copied
   *  from the dataset to this object.
   *  The dataset should be a printable Image or a PresentationStateStorage Object
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the dataset from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition createFromItem(DcmItem &dset);
  
  /** create a ImageBox with this image and append it to the ImageBoxList
   *  @image the printable image 
   *  @aETitle the title where we can get the image
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition addImage(DcmItem &image,char *aETitle);
  
  /** sets a new SOP Instance UID for the Stored Print object.
   *  @param uid new SOP Instance UID
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setInstanceUID(const char *uid);
  
  /** create a ImageBox with this image and presentation state and append it to the ImageBoxList
   *  The image have to be stored 
   *  @param pstate the presentation state corresponding to the image
   *  @param image the printable image 
   *  @param aETitle the title where we can get the image
   *  @return EC_Normal if successful, an error code otherwise.
   */ 
  E_Condition addPresentationState(DVPresentationState &pstate,DcmItem &image,char *aETitle);
  
  /** starts a print job
   *  @param printJob a already connected DICOM association to a remote printer
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition startPrint(DVPSPrintMessageHandler &printJob);
  
  /** gets the next needed image reference, used in combination with setImage
   *  @param aETitle  AETitle where the image can be found
   *  @param patID Patient ID of the image
   *  @param studyUID Study UID of the image
   *  @param seriesUID series UID of the image 
   *  @param instanceUID instance UID of the image
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition getNextImageReference(char *aETitle,char *patID,char *studyUID,char *seriesUID,char *instanceUID);
  
  /** transfer the preformatted image to the print job
   *  @param image the preformatted image, used in combination with getNextImageReference
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setImage(DcmItem &image);
  
  /** test if the print job is done
   *
   *  @return true if the job is still printing and needs images
   */
  
  OFBool printPending();
  
  

 private:

  /// private undefined assignment operator
  DVPSStoredPrint& operator=(const DVPSStoredPrint&);
	
  /* checks if given SOP class UID is an image storage SOP class
   * @return OFTrue if image SOP class, OFFalse otherwise.
   */
  OFBool isImageStorageSOPClass(OFString& sopclassuid);
  
  /** create default values for all missing type 1 elements.
   *  Called before a stored print object is written.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition createDefaultValues();

  /** writes a Referenced Presentation LUT SQ to the given
   *  dataset. Helper function used in the more general write() method.
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition addReferencedPLUTSQ(DcmItem &dset);
  
  /** invalidates the cached number of columns and rows
   */
  void invalidateCache();

  /** updates the cached number of columns and rows
   */
  void updateCache();
  
  /* Module: Patient (M)
   */
  /// Module=Patient, VR=PN, VM=1, Type 1 
  DcmPersonName            patientName;
  /// Module=Patient, VR=LO, VM=1, Type 2 
  DcmLongString            patientID;
  /// Module=Patient, VR=DA, VM=1, Type 2 
  DcmDate                  patientBirthDate;
  /// Module=Patient, VR=CS, VM=1, Type 2 
  DcmCodeString            patientSex;

  /* Module: General Study (M)  
   */
  /// Module=General_Study, VR=UI, VM=1, Type 1 
  DcmUniqueIdentifier      studyInstanceUID;
  /// Module=General_Study, VR=DA, VM=1, Type 2 
  DcmDate                  studyDate;
  /// Module=General_Study, VR=TM, VM=1, Type 2 
  DcmTime                  studyTime;
  /// Module=General_Study, VR=PN, VM=1, Type 2 
  DcmPersonName            referringPhysiciansName;
  /// Module=General_Study, VR=SH, VM=1, Type 2 
  DcmShortString           studyID;
  /// Module=General_Study, VR=SH, VM=1, Type 2 
  DcmShortString           accessionNumber;

  /* Module: General Series (M)
   */
  /// Module=General_Series, VR=UI, VM=1, Type 1 
  DcmUniqueIdentifier      seriesInstanceUID;
  /// Module=General_Series, VR=IS, VM=1, Type 2 
  DcmIntegerString         seriesNumber;

  /* Module: General Equipment (M)
   */
  /// Module=General_Equipment, VR=LO, VM=1, Type 2 
  DcmLongString            manufacturer;

  /* Module: Printer Characteristics (M)
   */
  // the PrintManagementCapabilitiesSequence is only created/checked on the fly
  // the PrinterCharacteristicsSequence is ignored since we save before printing

  /* Module: Film Box (M)
   */
  /// Module=Film_Box_Module, VR=IS, VM=1, Type 2
  DcmIntegerString         instanceNumber; 
  /* the following attributes belong to the Film Box Content SQ (Type 1) */
  /// Module=Film_Box_Module, VR=ST, VM=1, Type 1
  DcmShortText             imageDisplayFormat;
  /// Module=Film_Box_Module, VR=CS, VM=1, Type 3
  DcmCodeString            annotationDisplayFormatID;
  /// Module=Film_Box_Module, VR=CS, VM=1, Type 2
  DcmCodeString            filmOrientation;
  /// Module=Film_Box_Module, VR=CS, VM=1, Type 2
  DcmCodeString            filmSizeID;
  /// Module=Film_Box_Module, VR=CS, VM=1, Type 2
  DcmCodeString            magnificationType;
  /// Module=Film_Box_Module, VR=CS, VM=1, Type 3
  DcmCodeString            smoothingType;
  /// Module=Film_Box_Module, VR=CS, VM=1, Type 3
  DcmCodeString            borderDensity;
  /// Module=Film_Box_Module, VR=CS, VM=1, Type 3
  DcmCodeString            emptyImageDensity;
  /// Module=Film_Box_Module, VR=US, VM=1, Type 3
  DcmUnsignedShort         minDensity;
  /// Module=Film_Box_Module, VR=US, VM=1, Type 2
  DcmUnsignedShort         maxDensity;
  /// Module=Film_Box_Module, VR=CS, VM=1, Type 3
  DcmCodeString            trim;
  /// Module=Film_Box_Module, VR=ST, VM=1, Type 2
  DcmShortText             configurationInformation;
  /// Module=Film_Box_Module, VR=US, VM=1, Type 2c required if presentation SOP class present
  DcmUnsignedShort         illumination;
  /// Module=Film_Box_Module, VR=US, VM=1, Type 2c required if presentation SOP class present
  DcmUnsignedShort         reflectedAmbientLight;
  /// Module=Film_Box_Module (Supplement 38), VR=CS, VM=1, Type 3
  DcmCodeString            requestedResolutionID;
  // the ReferencedPresentationLUTSequence is only created/checked on the fly

  /* Module: Image Box List (M)
   */
  /// Module=Image_Box_List_Module, VR=SQ, VM=1, Type 1
  DVPSImageBoxContent_PList imageBoxContentList;

  /* Module: Presentation LUT List (U)
   * we only support a single item for the Presentation LUT Content Sequence
   */   
  DVPSPresentationLUT      presentationLUT;  
  DcmUniqueIdentifier      presentationLUTInstanceUID;

  /* Module: SOP Common (M) 
   * we don't store the SOP Class UID because it is well known.
   */
  /// Module=SOP_Common, VR=UI, VM=1, Type 1 
  DcmUniqueIdentifier      sOPInstanceUID;
  /// Module=SOP_Common, VR=CS, VM=1-n, Type 1C 
  DcmCodeString            specificCharacterSet;
  /// Module=SOP_Common, VR=DA, VM=1, Type 3 
  DcmDate                  instanceCreationDate;
  /// Module=SOP_Common, VR=TM, VM=1, Type 3 
  DcmTime                  instanceCreationTime;

  /// used when creating hardcopy image objects
  DcmUniqueIdentifier      imageSeriesInstanceUID;

  /// flag indicating whether the currentXX values are up to date
  OFBool currentValuesValid;
  /// current number of columns
  unsigned long currentNumCols;
  /// current number of rows
  unsigned long currentNumRows;
};

#endif

/*
 *  $Log: dvpssp.h,v $
 *  Revision 1.3  1999-08-27 15:57:57  meichel
 *  Added methods for saving hardcopy images and stored print objects
 *    either in file or in the local database.
 *
 *  Revision 1.2  1999/08/26 09:31:00  thiel
 *  Add extensions for the usage of the StoredPrint
 *
 *  Revision 1.1  1999/07/30 13:34:50  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */

