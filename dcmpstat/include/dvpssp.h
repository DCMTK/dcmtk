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
 *  Update Date:      $Date: 1999-09-13 15:19:10 $
 *  CVS/RCS Revision: $Revision: 1.10 $
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
   *  @param writeRequestedImageSize if false, the Requested Image Size attributes are not written,
   *    e. g. because they are not supported by the target printer.
   *  @param limitImages if true, only the number of image references
   *    that are needed for the current image display format (film layout) are written.
   *    If false, all image references are written.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset, OFBool writeRequestedImageSize, OFBool limitImages);

  /** sets the image display format to 'STANDARD\columns,rows'.
   *  The caller must make sure that the column and row values are
   *  valid for the selected printer.
   *  @param columns number of columns
   *  @param rows number of rows
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setImageDisplayFormat(unsigned long columns, unsigned long rows);
  
  /** sets the (optional) film size ID.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setFilmSizeID(const char *value);
  
  /** sets the (optional) magnification type.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setMagnificationType(const char *value);
  
  /** sets the (optional) smoothing type.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setSmoothingType(const char *value);
  
  /** sets the (optional) configuration information.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setConfigurationInformation(const char *value);
  
  /** sets the (optional) requested resolution ID.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setResolutionID(const char *value);
  
  /** sets the (optional) film orientation.
   *  @param value new enumerated value. The caller is responsible for
   *    making sure that the selected printer supports film orientation
   *    if a non-default value is set.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setFilmOrientation(DVPSFilmOrientation value);  
  
  /** sets the (optional) trim (printing of borders).
   *  @param value new enumerated value. The caller is responsible for
   *    making sure that the selected printer supports trim
   *    if a non-default value is set.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setTrim(DVPSTrimMode value);
  
  /** sets the (optional) requested decimate/crop behaviour
   *  for all image boxes managed by this stored print object.
   *  @param value new enumerated value. The caller is responsible for
   *    making sure that the selected printer supports decimate/crop
   *    if a non-default value is set.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setRequestedDecimateCropBehaviour(DVPSDecimateCropBehaviour value); 

  /** sets the (optional) border density.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setBorderDensity(const char *value);

  /** sets the (optional) empty image density.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setEmtpyImageDensity(const char *value);
    
  /** deletes all optional attribute values that might not be
   *  supported by all printers. Film size ID, magnification and smoothing type,
   *  configuration information, requested resolution ID,
   *  trim and requested decimate/crop behaviour, border and empty image density
   *  are reset to default. For all registered images, magnification, smoothing type
   *  and configuration information are also set back to default.
   *  @return EC_Normal if successful, an error code otherwise.
   */   
  E_Condition newPrinter(); // short cut, delete all optional settings

  /** gets the number of columns of the current image display format.
   *  @return number of columns.
   */
  unsigned long getImageDisplayFormatColumns();

  /** gets the number of rows of the current image display format.
   *  @return number of rows.
   */
  unsigned long getImageDisplayFormatRows();

  /** gets the current film orientation.
   *  @return film orientation.
   */
  DVPSFilmOrientation getFilmOrientation();    

  /** gets the current trim mode.
   *  @return trim mode.
   */
  DVPSTrimMode getTrim();

  /** gets the current requested decimate/crop behaviour setting
   *  that is used for all image boxes managed by this object.
   *  @return requested decimate/crop behaviour
   */
  DVPSDecimateCropBehaviour getRequestedDecimateCropBehaviour()
  {
    return decimateCropBehaviour;
  }

  /** gets the (optional) film size ID.
   *  @return film size ID, may be NULL.
   */
  const char *getFilmSizeID();

  /** gets the (optional) magnification type.
   *  @return magnification type, may be NULL.
   */
  const char *getMagnificationType();

  /** gets the (optional) smoothing type.
   *  @return smoothing type, may be NULL.
   */
  const char *getSmoothingType();

  /** gets the (optional) configuration information.
   *  @return configuration information, may be NULL.
   */
  const char *getConfigurationInformation();

  /** gets the (optional) requestes resolution ID
   *  @return requested resolution ID, may be NULL.
   */
  const char *getResolutionID();

  /** gets the (optional) border density.
   *  @return border density, may be NULL.
   */
  const char *getBorderDensity();

  /** gets the (optional) empty image density.
   *  @return empty image density, may be NULL.
   */
  const char *getEmtpyImageDensity();
     
  /** gets the number of images currently registered in this object.
   *  @return number of images.
   */
  size_t getNumberOfImages()
  {
    return imageBoxContentList.size();
  }
  
  /** deletes one of the registered images.
   *  @param idx index, must be < getNumberOfImages()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition deleteImage(size_t idx)
  {
    return imageBoxContentList.deleteImage(idx);
  }
  
  /** deletes multiple of the registered
   *  images, starting with the first one.
   *  @param number number of images to delete, must be <= getNumberOfImages()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition deleteMultipleImages(size_t number)
  {
    return imageBoxContentList.deleteMultipleImages(number);
  }

  /** checks if one of the registered images has additional settings that are not
   *  default values on the image box level.
   *  @param idx index, must be < getNumberOfImages()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFBool imageHasAdditionalSettings(size_t idx)
  {
    return imageBoxContentList.imageHasAdditionalSettings(idx);
  }

  /** sets the (optional) magnification type for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setImageMagnificationType(size_t idx, const char *value)
  {
    return imageBoxContentList.setImageMagnificationType(idx, value);
  }
  
  /** sets the (optional) smoothing type for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setImageSmoothingType(size_t idx, const char *value)
  {
    return imageBoxContentList.setImageSmoothingType(idx, value);
  }
  
  /** sets the (optional) configuration information for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setImageConfigurationInformation(size_t idx, const char *value)
  {
    return imageBoxContentList.setImageConfigurationInformation(idx, value);
  }

  /** gets the (optional) magnification type for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @return magnification type, may be NULL.
   */
  const char *getImageMagnificationType(size_t idx)
  {
    return imageBoxContentList.getImageMagnificationType(idx);
  }

  /** gets the (optional) smoothing type for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @return smoothing type, may be NULL.
   */
  const char *getImageSmoothingType(size_t idx)
  {
    return imageBoxContentList.getImageSmoothingType(idx);
  }

  /** gets the (optional) configuration information for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @return configuration information, may be NULL.
   */
  const char *getImageConfigurationInformation(size_t idx)
  {
    return imageBoxContentList.getImageConfigurationInformation(idx);
  }
  
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
   *  @param refstudyuid Study instance UID of the referenced image
   *  @param refseriesuid Series instance UID of the referenced image
   *  @param refsopclassuid SOP Class UID of the referenced image
   *  @param refsopinstanceuid SOP instance UID of the referenced image
   *  @param requestedimagesize requested images size for this image, may be NULL (absent)
   *  @param patientid patient ID for the referenced image, may be NULL (absent)
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition addImageBox(
    const char *retrieveaetitle,
    const char *refstudyuid,
    const char *refseriesuid,
    const char *refsopclassuid,
    const char *refsopinstanceuid,
    const char *requestedimagesize,
    const char *patientid);
  
  /** creates a new image box object and sets the content of this image box object.
   *  This is a specialized version of the method with the same name and more parameters.
   *  SOP Class is assumed to be Grayscale Hardcopy, Study and Series are derived from
   *  the Stored Print internal defaults.
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

  /* Presentation LUT Interface */
  
  /** gets the current Presentation LUT type.
   *  @return the current presentation LUT type
   */
  DVPSPrintPresentationLUTType getPresentationLUT();
  
  /** checks if a real Presentation LUT (not shape)
   *  is available in the presentation state.
   *  @return OFTrue if the presentation state contains
   *    a presentation LUT, no matter if it is activated or not.
   *    Returns OFFalse otherwise.
   */
  OFBool havePresentationLookupTable() { return presentationLUT.haveTable(); }
  
  /** sets the current Presentation LUT type.
   *  DVPSP_table can only be used if the presentation state
   *  contains a lookup table, i.e. if havePresentationLookupTable() returns OFTrue.
   *  @param newType the new presentation LUT type.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setCurrentPresentationLUT(DVPSPrintPresentationLUTType newType);
 
  /** stores a presentation lookup table in the presentation state.
   *  This method stores a presentation lookup table in the
   *  presentation state and activates it. The LUT is copied to
   *  the presentation state. If the method returns EC_Normal,
   *  any old presentation LUT in the presentation state is overwritten.
   *  If the method returns an error code, an old LUT is left unchanged.
   *  @param lutDescriptor the LUT Descriptor in DICOM format (VM=3)
   *  @param lutData the LUT Data in DICOM format
   *  @param lutExplanation the LUT Explanation in DICOM format, may be empty.
   *  @return EC_Normal if successful, an error code otherwise.
   */ 
  E_Condition setPresentationLookupTable(
    DcmUnsignedShort& lutDescriptor,
    DcmUnsignedShort& lutData,
    DcmLongString& lutExplanation);

  /** stores a presentation lookup table in the presentation state.
   *  This method stores a presentation lookup table in the
   *  presentation state and activates it. The LUT is copied to
   *  the presentation state. Overwrites old LUT. If unsuccessful,
   *  LUT is set to DVPSQ_none.
   *  @param dset dataset from which the Presentation LUT SQ or Shape is read.
   *  @return EC_Normal if successful, an error code otherwise.
   */ 
  E_Condition setPresentationLookupTable(DcmItem &dset);
    
  /** gets a description of the current presentation LUT.
   *  For well-known presentation LUT shapes, a standard text
   *  is returned. For presentation LUTs, the LUT explanation
   *  is returned if it exists and a standard text otherwise.
   *  Returns NULL if presentation LUT is deactivated (getPresentationLUT() == DVPSQ_none).
   *  @return a pointer to a string describing the current presentation LUT.
   */
  const char *getCurrentPresentationLUTExplanation();

  /** returns the LUT explanation of the presentation LUT
   *  if it exists and is non-empty. Otherwise returns NULL.
   *  @return a string pointer
   */
  const char *getPresentationLUTExplanation() { return presentationLUT.getLUTExplanation(); }
    
  
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
  E_Condition startPrint(DVPSPrintMessageHandler *printJob);
  
  /** gets the next needed image reference, used in combination with setImage
   *  @param aETitle  AETitle where the image can be found
   *  @param patID Patient ID of the image
   *  @param studyUID Study UID of the image
   *  @param seriesUID series UID of the image 
   *  @param instanceUID instance UID of the image
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition getNextImageReference(char *&aETitle,char *&patID,char *&studyUID,char *&seriesUID,char *&instanceUID);
  
  /** transfer the preformatted image to the print job
   *  @param image the preformatted image, used in combination with getNextImageReference
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setImage(DcmItem *image);
  
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
  
  /// requested decimate/crop behaviour used in all image boxes
  DVPSDecimateCropBehaviour decimateCropBehaviour;

  /// connection to a printer
  DVPSPrintMessageHandler *currentPrintHandler;
	  
  /// the current filmsessioninstance
  OFString filmSessionInstanceUID;
			
  /// the current filmboxinstance
  OFString filmBoxInstanceUID;

  /// the list of imagebox instances
  char **imageBoxInstanceUID;

  OFListIterator(DVPSImageBoxContent *) currentImageBox ;

  /// next wanted image
  int nextImage;

};

#endif

/*
 *  $Log: dvpssp.h,v $
 *  Revision 1.10  1999-09-13 15:19:10  meichel
 *  Added implementations for a number of further print API methods.
 *
 *  Revision 1.9  1999/09/10 12:46:47  meichel
 *  Added implementations for a number of print API methods.
 *
 *  Revision 1.8  1999/09/09 14:57:33  thiel
 *  Added methods for print spooler
 *
 *  Revision 1.7  1999/09/09 12:20:47  meichel
 *  Added print API method declarations and implementations (empty for now).
 *
 *  Revision 1.6  1999/09/08 16:46:44  meichel
 *  Added print API method declarations
 *
 *  Revision 1.5  1999/09/01 16:14:41  meichel
 *  Added support for requested image size to print routines
 *
 *  Revision 1.4  1999/08/31 14:09:12  meichel
 *  Added get/set methods for stored print attributes
 *
 *  Revision 1.3  1999/08/27 15:57:57  meichel
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

