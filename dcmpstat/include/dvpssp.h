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
 *  Update Date:      $Date: 2000-02-29 12:16:16 $
 *  CVS/RCS Revision: $Revision: 1.17 $
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
#include "dvpstyp.h"         /* for enum types */
#include "dvpspll.h"         /* for class DVPSPresentationLUT_PList */
#include "dvpsibl.h"         /* for class DVPSImageBoxContent_PList */
#include "dvpsabl.h"         /* for class DVPSAnnotationContent_PList */
#include "dvpstat.h"		 /* for class DVPresentationState */
#include "dvpspr.h"			 /* for class DVPrintMessageHandler */

class DicomImage;
class ostream;
class DVPSPresentationLUT;

/** the representation of a Stored Print object
 */  

class DVPSStoredPrint
{
 public:
  /** constructor
   *  @param illumin default Illumination setting
   *  @param reflection default Reflected Ambient Light setting
   */
  DVPSStoredPrint(Uint16 illumin, Uint16 reflection);
  
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

  /** sets the (optional) max density.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setMaxDensity(const char *value);

  /** sets the (optional) min density.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setMinDensity(const char *value);

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

  /** gets the Study Instance UID.
   *  @return Study Instance UID, may be NULL.
   */
  const char *getStudyInstanceUID();

  /** gets the Series Instance UID.
   *  @return Series Instance UID, may be NULL.
   */
  const char *getSeriesInstanceUID();

  /** gets the SOP Instance UID.
   *  @return SOP Instance UID, may be NULL.
   */
  const char *getSOPInstanceUID();

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

  /** gets the (optional) max density.
   *  The string returned becomes invalid after the next
   *  call to getMaxDensity or getMinDensity.
   *  @return max density, may be NULL.
   */
  const char *getMaxDensity();

  /** gets the (optional) min density.
   *  The string returned becomes invalid after the next
   *  call to getMaxDensity or getMinDensity.
   *  @return min density, may be NULL.
   */
  const char *getMinDensity();
     
  /** gets the number of images currently registered in this object.
   *  @return number of images.
   */
  size_t getNumberOfImages()
  {
    return imageBoxContentList.size();
  }

  /** gets the number of annotations currently registered in this object.
   *  @return number of annotations.
   */
  size_t getNumberOfAnnotations()
  {
    return annotationContentList.size();
  }
  
  /** deletes one of the registered images.
   *  @param idx index, must be < getNumberOfImages()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition deleteImage(size_t idx);
  
  /** deletes multiple of the registered
   *  images, starting with the first one.
   *  @param number number of images to delete, must be <= getNumberOfImages()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition deleteMultipleImages(size_t number);

  /** deletes as many images as fit on the current page according
   *  to the image display format settings. Used to remove images
   *  from the queue after a print job with one film box has been
   *  spooled.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition deleteSpooledImages();

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
   *  @param presentationlut presentation LUT to be used, may be NULL (absent)
   *  @param inversePLUT true if presentation LUT is for Monochrome1 and must be inversed.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition addImageBox(
    const char *retrieveaetitle,
    const char *refstudyuid,
    const char *refseriesuid,
    const char *refsopclassuid,
    const char *refsopinstanceuid,
    const char *requestedimagesize,
    const char *patientid,
    DVPSPresentationLUT *presentationlut,
    OFBool inversePLUT);
  
  /** creates a new image box object and sets the content of this image box object.
   *  This is a specialized version of the method with the same name and more parameters.
   *  SOP Class is assumed to be Grayscale Hardcopy, Study and Series are derived from
   *  the Stored Print internal defaults.
   *  @param retrieveaetitle retrieve AETITLE of the referenced image
   *  @param refsopinstanceuid SOP instance UID of the referenced image
   *  @param requestedimagesize requested images size for this image, default: absent
   *  @param patientid patient ID for the referenced image, default: absent
   *  @param presentationlut presentation LUT to be used, may be NULL (absent)
   *  @param inversePLUT true if presentation LUT is for Monochrome1 and must be inversed.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition addImageBox(
    const char *retrieveaetitle,
    const char *refsopinstanceuid,
    const char *requestedimagesize=NULL,
    const char *patientid=NULL,
    DVPSPresentationLUT *presentationlut=NULL,
    OFBool inversePLUT=OFFalse);

  /** deletes all existing annotations and creates a new one,
   *  with given text and position. Sets annotation display format
   *  to the given value.
   *  
   *  @param displayformat annotation display format
   *  @param text annotation text
   *  @param position annotation position
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setSingleAnnotation(
    const char *displayformat,
    const char *text,
    Uint16 position);
  
  /** deletes all annotations, clears annotation display format.
   */
  void deleteAnnotations();
  
  /** sets a new SOP Instance UID for the Stored Print object.
   *  @param uid new SOP Instance UID
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setInstanceUID(const char *uid);

  /** returns the image UIDs that are required to look up the referenced image in the database
   *  @param idx index, must be < getNumberOfImages()
   *  @param studyUID Study UID of the image
   *  @param seriesUID series UID of the image 
   *  @param instanceUID instance UID of the image
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition getImageReference(size_t idx, const char *&studyUID, const char *&seriesUID, const char *&instanceUID)
  {
    return imageBoxContentList.getImageReference(idx, studyUID, seriesUID, instanceUID);
  }
          
  /** Requests the properties of the printer (Printer SOP Instance N-GET).
   *  The properties are not returned, but if the message handler is switched to "dump mode",
   *  the DIMSE communication will be printed. 
   *  @param printHandler print communication handler, association must be open.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition printSCUgetPrinterInstance(DVPSPrintMessageHandler& printHandler);

  /** checks whether a presentation LUT or LUT shape is active in this stored print object.
   *  In this case, if the printer supports the Presentation LUT SOP class,
   *  a Presentation LUT SOP Instance is created in the printer.  
   *  @param printHandler print communication handler, association must be open.
   *  @param printerRequiresMatchingLUT true if printer requires presentation LUTs matching the image depth
   *  @param printerLUTRenderingPreferred true if SCP side presentation LUTs should be preferred 
   *    even if printer supports 12-bit image transmission.
   *  @param printerSupports12Bit true if printer supports 12 bit transmission
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition printSCUpreparePresentationLUT(
     DVPSPrintMessageHandler& printHandler,
     OFBool printerRequiresMatchingLUT,
     OFBool printerLUTRenderingPreferred,
     OFBool printerSupports12Bit);

  /** Creates a DICOM Basic Film Session SOP Instance in the printer.
   *  @param printHandler print communication handler, association must be open.
   *  @param dset DICOM dataset containing all Basic Film Session attributes managed outside this class
   *  @param plutInSession true if printer expects referenced presentation LUT sequence, illumination
   *    and reflected ambient light in basic film session, false if it expects them in basic film box.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition printSCUcreateBasicFilmSession(
    DVPSPrintMessageHandler& printHandler, 
    DcmDataset& dset,
    OFBool plutInSession);

  /** Creates a DICOM Basic Film Box SOP Instance in the printer. 
   *  This method only allows one basic film box to exist at any time -
   *  collation is not supported.
   *  @param printHandler print communication handler, association must be open.
   *  @param plutInSession true if printer expects referenced presentation LUT sequence, illumination
   *    and reflected ambient light in basic film session, false if it expects them in basic film box.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition printSCUcreateBasicFilmBox(DVPSPrintMessageHandler& printHandler, OFBool plutInSession);

  /** Transmits a DICOM image to the printer (Basic Grayscale Image Box N-SET).
   *  @param printHandler print communication handler, association must be open.
   *  @param idx index of the image reference from which the Image Box settings are taken,
   *     must be < getNumberOfImages().
   *  @param image DICOM image to be printed
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition printSCUsetBasicImageBox(
    DVPSPrintMessageHandler& printHandler,
    size_t idx,
    DicomImage& image);

  /** Transmits a DICOM annotation to the printer (Basic Annotation Box N-SET).
   *  @param printHandler print communication handler, association must be open.
   *  @param idx index of the annotation from which the settings are taken,
   *     must be < getNumberOfAnnotations().
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition printSCUsetBasicAnnotationBox(
    DVPSPrintMessageHandler& printHandler,
    size_t idx);
    
  /** Prints the current DICOM Basic Film Box SOP Instance.
   *  @param printHandler print communication handler, association must be open.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition printSCUprintBasicFilmBox(DVPSPrintMessageHandler& printHandler);

  /** Deletes all objects currently present in the print association.
   *  @param printHandler print communication handler, association must be open.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition printSCUdelete(DVPSPrintMessageHandler& printHandler);

    /** sets the illumination to be used
     *  with the print Presentation LUT SOP Class.
     *  @param value new attribute value, in cd/m2.
     *    The caller is responsible for making sure
     *    that the value is valid for the selected printer.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    E_Condition setPrintIllumination(Uint16 value);

    /** gets the current illumination setting
     *  used with the print Presentation LUT SOP Class.
     *  @return illumination in cd/m2
     */
    Uint16 getPrintIllumination();

    /** sets the reflected ambient light to be used
     *  with the print Presentation LUT SOP Class.
     *  @param value new attribute value, in cd/m2.
     *    The caller is responsible for making sure
     *    that the value is valid for the selected printer.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    E_Condition setPrintReflectedAmbientLight(Uint16 value);

    /** gets the current reflected ambient light setting
     *  used with the print Presentation LUT SOP Class.
     *  @return reflected ambient light in cd/m2
     */
    Uint16 getPrintReflectedAmbientLight();

    /** sets a new log stream
     *  @param o new log stream, must not be NULL
     */
    void setLog(ostream *o);

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

  /** writes a Referenced Presentation LUT SQ, Illumination and
   *  reflected ambient light to the given dataset.
   *  Helper function used when creating Basic Film Session or
   *  Basic Film Box.
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition addPresentationLUTReference(DcmItem& dset);
  
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
  // the ReferencedPresentationLUTSequence is only created/read on the fly
  DcmUniqueIdentifier      referencedPresentationLUTInstanceUID;

  /* Module: Image Box List (M)
   */
  /// Module=Image_Box_List_Module, VR=SQ, VM=1, Type 1
  DVPSImageBoxContent_PList imageBoxContentList;

  /* Module: Annotation List (U)
   */
  /// Module=Annotation_List_Module, VR=SQ, VM=1, Type 3
  DVPSAnnotationContent_PList annotationContentList;
   
  /* Module: Presentation LUT List (U)
   */   
  DVPSPresentationLUT_PList presentationLUTList;  

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

  /// the current film session instance
  OFString filmSessionInstanceUID;
			
  /// the current film box instance
  OFString filmBoxInstanceUID;

  /// the current presentation LUT instance
  OFString presentationLUTInstanceUID;

  /// transmit images in 12 bit for the current print job
  OFBool transmitImagesIn12Bit;

  /// presentation LUTs are rendered on SCP side
  OFBool renderPresentationLUTinSCP;

  /// temporary buffer for getMaxDensity and getMinDensity
  OFString tempDensity;

  /** output stream for error messages, never NULL
   */
  ostream *logstream;

};

#endif

/*
 *  $Log: dvpssp.h,v $
 *  Revision 1.17  2000-02-29 12:16:16  meichel
 *  Fixed bug in dcmpstat library that caused Monochrome1 images
 *    to be printed inverse if a Presentation LUT was applied.
 *
 *  Revision 1.16  1999/10/19 14:46:04  meichel
 *  added support for the Basic Annotation Box SOP Class
 *    as well as access methods for Max Density and Min Density.
 *
 *  Revision 1.15  1999/10/13 14:11:12  meichel
 *  Fixed bug in routine that renders P-LUTs into a print bitmap
 *    before sending an image to the printer
 *
 *  Revision 1.14  1999/10/07 17:21:50  meichel
 *  Reworked management of Presentation LUTs in order to create tighter
 *    coupling between Softcopy and Print.
 *
 *  Revision 1.13  1999/09/24 15:24:30  meichel
 *  Added support for CP 173 (Presentation LUT clarifications)
 *
 *  Revision 1.12  1999/09/17 14:33:59  meichel
 *  Completed print spool functionality including Supplement 22 support
 *
 *  Revision 1.11  1999/09/15 17:43:29  meichel
 *  Implemented print job dispatcher code for dcmpstat, adapted dcmprtsv
 *    and dcmpsprt applications.
 *
 *  Revision 1.10  1999/09/13 15:19:10  meichel
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

