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
 *    classes: DVPSHardCopy
 *
 *  Last Update:      $Author: thiel $
 *  Update Date:      $Date: 1999-08-26 09:30:58 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSHC_H__
#define __DVPSHC_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dctk.h"
#include "dvpstyp.h"     /* for enum types */


/** the representation of a Hardcopy object
 */  

class DVPSHardCopy
{
 public:
  /// default constructor
  DVPSHardCopy();
  
  /// copy constructor
  DVPSHardCopy(const DVPSHardCopy& copy);

  /** clone method.
   *  @return a pointer to a new DVPSHardCopy object containing
   *  a copy of this object.
   */
  DVPSHardCopy *clone() { return new DVPSHardCopy(*this); }

  /// destructor
  virtual ~DVPSHardCopy();

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** reads a Hardcopy object from a DICOM dataset.
   *  The DICOM elements of the Hardcopy object are copied
   *  from the dataset to this object.
   *  The completeness of the object (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the dataset from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);
  
  /** writes the Hardcopy object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);


  /** creates a Hardcopy object from a DICOM dataset.
   *  The DICOM elements are copied
   *  from the dataset to this object.
   *  The dataset should be a printable Image or a PresentationStateStorage Object
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the dataset from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition createFromItem(DcmItem &dset);
  
 private:

  /// private undefined assignment operator
  DVPSHardCopy& operator=(const DVPSHardCopy&);
	
  /* checks if given SOP class UID is an image storage SOP class
   * @return OFTrue if image SOP class, OFFalse otherwise.
   */
  OFBool isImageStorageSOPClass(OFString& sopclassuid);
  
  /** create default values for all missing type 1 elements.
   *  Called before a stored print object is written.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition createDefaultValues();

  
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

  /* Module: General Equipment (U)
   */
  /// Module=General_Equipment, VR=LO, VM=1, Type 2 
  /// DcmLongString            manufacturer;


  /* Module: Hardcopy Equipment (M)
   */
	
  /// Module=Hardcopy Equipment, VR=CS, VM=1, Type 1 
  /// DcmCodeString modality   filled on the fly
  /* Module: SOP Common (M) 
   * we don't store the SOP Class UID because it is well known.
   */
  /// Module=SOP_Common, VR=UI, VM=1, Type 1 
  DcmUniqueIdentifier      sOPInstanceUID;
  /// Module=SOP_Common, VR=CS, VM=1-n, Type 1C 
  DcmCodeString            specificCharacterSet;

  /// used when creating hardcopy image objects
  DcmUniqueIdentifier      imageSeriesInstanceUID;

 
  /* Module: General Image (M)
   */
  /// Module=General Image, VR=IS, VM=1, Type 2
  /// 
  DcmIntegerString         instanceNumber;
  /// Module=General Image, VR=CS, VM=2, Type 2C
  /// DcmCodeString					patientOrientation;

  /* Module: HC GrayScaleImage (M)
   */
	
  /// Module=General Image, VR=US, VM=1, Type 1
  DcmUnsignedShort   samplesPerPixel;
  /// Module=General Image, VR=CS, VM=1, Type 1
  DcmCodeString      photometricInterpretation;
  /// Module=General Image, VR=US, VM=1, Type 1
  DcmUnsignedShort   rows;
  /// Module=General Image, VR=US, VM=1, Type 1
  DcmUnsignedShort   columns;
  /// Module=General Image, VR=IS, VM=2, Type 1C
  DcmIntegerString   pixelAspectRatio;
  /// Module=General Image, VR=US, VM=1, Type 1
  DcmUnsignedShort   bitsAllocated;
  /// Module=General Image, VR=US, VM=1, Type 1
  DcmUnsignedShort	 bitsStored;
  /// Module=General Image, VR=US, VM=1, Type 1
  DcmUnsignedShort   highBit;
  /// Module=General Image, VR=US, VM=1, Type 1
  //pixelRepresentation always 0
  /// Module=General Image, VR=Ox, VM=1, Type 1
  DcmPixelData pixelData;


};

#endif


