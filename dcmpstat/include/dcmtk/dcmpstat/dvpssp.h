/*
 *
 *  Copyright (C) 1999-2012, OFFIS e.V.
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
 *    classes: DVPSStoredPrint
 *
 */


#ifndef DVPSSP_H
#define DVPSSP_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmpstat/dvpstyp.h"         /* for enum types */
#include "dcmtk/dcmpstat/dvpspll.h"         /* for class DVPSPresentationLUT_PList */
#include "dcmtk/dcmpstat/dvpsibl.h"         /* for class DVPSImageBoxContent_PList */
#include "dcmtk/dcmpstat/dvpsabl.h"         /* for class DVPSAnnotationContent_PList */
#include "dcmtk/dcmpstat/dvpstat.h"		 /* for class DVPresentationState */
#include "dcmtk/dcmpstat/dvpspr.h"			 /* for class DVPrintMessageHandler */

class DicomImage;
class DVPSPresentationLUT;
class DVConfiguration;
/** the representation of a Stored Print object
 */

class DCMTK_DCMPSTAT_EXPORT DVPSStoredPrint
{
 public:
  /** constructor
   *  @param illumin default Illumination setting
   *  @param reflection default Reflected Ambient Light setting
   *  @param aetitle application entity title of the print originator (SCU)
   */
  DVPSStoredPrint(Uint16 illumin, Uint16 reflection, const char *aetitle = NULL);

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
  OFCondition read(DcmItem &dset);

  /** writes the Stored Print object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the data is written
   *  @param writeRequestedImageSize if false, the Requested Image Size attributes are not written,
   *    e. g. because they are not supported by the target printer.
   *  @param limitImages if true, only the number of image references
   *    that are needed for the current image display format (film layout) are written.
   *    If false, all image references are written.
   *  @param updateDecimateCrop if true, the decimate/crop attribute on image box level
   *    is replaced by the global stored print level setting in all image boxes
   *    prior to writing the dataset.
   *  @param ignoreEmptyImages if true, all image boxes without image box position are ignored
   *    when writing.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(
    DcmItem &dset,
    OFBool writeRequestedImageSize,
    OFBool limitImages,
    OFBool updateDecimateCrop,
    OFBool ignoreEmptyImages);

  /** sets the name of the current printer.
   *  This name is identical to the unique entry used in the configuration file.
   *  @return name of the current printer
   */
  OFCondition setOriginator(const char *aetitle);

  /** sets the application entity title of the print SCU.
   *  @return application entity title of the print SCU
   */
  OFCondition setDestination(const char *aetitle);

  /** sets the application entity title of the print SCP.
   *  @return application entity title of the print SCP
   */
  OFCondition setPrinterName(const char *name);

  /** sets the image display format to 'STANDARD\columns,rows'.
   *  The caller must make sure that the column and row values are
   *  valid for the selected printer.
   *  @param columns number of columns
   *  @param rows number of rows
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setImageDisplayFormat(unsigned long columns, unsigned long rows);

  /** sets the (optional) film size ID.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setFilmSizeID(const char *value);

  /** sets the (optional) magnification type.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setMagnificationType(const char *value);

  /** sets the (optional) smoothing type.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setSmoothingType(const char *value);

  /** sets the (optional) configuration information.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setConfigurationInformation(const char *value);

  /** sets the (optional) requested resolution ID.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setResolutionID(const char *value);

  /** sets the (optional) film orientation.
   *  @param value new enumerated value. The caller is responsible for
   *    making sure that the selected printer supports film orientation
   *    if a non-default value is set.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setFilmOrientation(DVPSFilmOrientation value);

  /** sets the (optional) trim (printing of borders).
   *  @param value new enumerated value. The caller is responsible for
   *    making sure that the selected printer supports trim
   *    if a non-default value is set.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setTrim(DVPSTrimMode value);

  /** sets the (optional) requested decimate/crop behaviour
   *  for all image boxes managed by this stored print object.
   *  @param value new enumerated value. The caller is responsible for
   *    making sure that the selected printer supports decimate/crop
   *    if a non-default value is set.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setRequestedDecimateCropBehaviour(DVPSDecimateCropBehaviour value);

  /** sets the (optional) border density.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setBorderDensity(const char *value);

  /** sets the (optional) empty image density.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setEmtpyImageDensity(const char *value);

  /** sets the (optional) max density.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setMaxDensity(const char *value);

  /** sets the (optional) min density.
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setMinDensity(const char *value);

  /** deletes all optional attribute values that might not be
   *  supported by all printers. Film size ID, magnification and smoothing type,
   *  configuration information, requested resolution ID,
   *  trim and requested decimate/crop behaviour, border and empty image density
   *  are reset to default. For all registered images, magnification, smoothing type
   *  and configuration information are also set back to default.
   *  @param name name of the new printer (optional)
   *  @param aetitle of the new printer (optional)
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition newPrinter(const char *name = NULL, const char *destinationAE = NULL); // short cut, delete all optional settings

  /** gets the the application entity title of the print SCU.
   *  @return application entity title of the print SCP
   */
  const char *getOriginator();
  /** gets the the application entity title of the print SCP.
   *  @return application entity title of the print SCP
   */
  const char *getDestination();
  /** gets the name of the current printer.
   *  @return name of the current printer
   */
  const char *getPrinterName();

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

  /** gets the (optional) max density.
   *  @return max density (default: 300).
   */
  Uint16 getMaxDensityValue();

  /** gets the (optional) min density.
   *  @return min density (default: 20).
   */
  Uint16 getMinDensityValue();

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
  OFCondition deleteImage(size_t idx);

  /** deletes multiple of the registered
   *  images, starting with the first one.
   *  @param number number of images to delete, must be <= getNumberOfImages()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition deleteMultipleImages(size_t number);

  /** deletes as many images as fit on the current page according
   *  to the image display format settings. Used to remove images
   *  from the queue after a print job with one film box has been
   *  spooled.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition deleteSpooledImages();

  /** checks if one of the registered images has additional settings that are not
   *  default values on the image box level.
   *  @param idx index, must be < getNumberOfImages()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFBool imageHasAdditionalSettings(size_t idx)
  {
    return imageBoxContentList.imageHasAdditionalSettings(idx);
  }

  /** sets the polarity for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value (NORMAL or REVERSE), may be NULL.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setImagePolarity(size_t idx, const char *value)
  {
    return imageBoxContentList.setImagePolarity(idx, value);
  }

  /** sets the requested size for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value (in mm), may be NULL.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setImageRequestedSize(size_t idx, const char *value)
  {
    return imageBoxContentList.setImageRequestedSize(idx, value);
  }

  /** sets the (optional) magnification type for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @param value new attribute value, may be NULL.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setImageMagnificationType(size_t idx, const char *value)
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
  OFCondition setImageSmoothingType(size_t idx, const char *value)
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
  OFCondition setImageConfigurationInformation(size_t idx, const char *value)
  {
    return imageBoxContentList.setImageConfigurationInformation(idx, value);
  }

  /** gets the polarity for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @return polarity (NORMAL or REVERSE), may be NULL.
   */
  const char *getImagePolarity(size_t idx)
  {
    return imageBoxContentList.getImagePolarity(idx);
  }

  /** gets the requested size for the given registered image box.
   *  @param idx index, must be < getNumberOfImages()
   *  @return requested size (in mm), may be NULL.
   */
  const char *getImageRequestedSize(size_t idx)
  {
    return imageBoxContentList.getImageRequestedSize(idx);
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

  /** gets the presentation LUT for the given registered image box.
   *  If not available the presentation LUT of the film box is used.
   *  @param idx index, must be < getNumberOfImages()
   *  @return pointer to presentation LUT, may be NULL.
   */
  DVPSPresentationLUT *getImagePresentationLUT(size_t idx);

  /** gets the "global" presentation LUT which overrides the settings for the image boxes.
   *  If not available the presentation LUT of the image box is used.
   *  @return pointer to presentation LUT, may be NULL.
   */
  DVPSPresentationLUT *getPresentationLUT();

  /** resets the Presentation LUT to the default setting. i.e. the presentation LUT
   *  which is specified separately for each image box is used.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setDefaultPresentationLUT();

  /** sets the current Presentation LUT shape (overrides the image box settings).
   *  Only DVPSP_identity and DVPSP_lin_od are allowed.
   *  @param shape the new presentation LUT shape.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setPresentationLUTShape(DVPSPresentationLUTType shape);

  /** stores a presentation lookup table in the stored print object.
   *  This method stores a presentation lookup table in the
   *  stored print object and activates it. This LUT overrides the
   *  settings made for the individual image boxes. If unsuccessful,
   *  LUT is not set.
   *  @param dset dataset from which the Presentation LUT SQ or Shape is read.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setPresentationLookupTable(DcmItem &dset);

  /** converts an optical density (OD) value to an 8/12/16-bit P-value which is linear to luminance.
   *  The output is not calibrated according to the GSDF.  This can be done by convertPValueToDDL() in
   *  class DVPSPresentationState.  The attributes illumination, reflected ambient light and min/max
   *  density (default 20/300) from this stored print object are used for the calculation.
   *  @param density in hundreds of OD (e.g. 150 corressponds to 1.5 OD)
   *  @param bits number of bits used for the output value (8, 12, 16)
   *  @return P-Value, 0..0xFF, 0..0xFFF, 0..0xFFFF, < 0 if an error occurred.
   */
  Sint32 convertODtoPValue(Uint16 density, unsigned int bits = 8);

  /** writes the general study and series module attributes for a grayscale hardcopy image
   *  that is related to this stored print object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition writeHardcopyImageAttributes(DcmItem &dset);

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
  OFCondition addImageBox(
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
  OFCondition addImageBox(
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
  OFCondition setSingleAnnotation(
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
  OFCondition setInstanceUID(const char *uid);

  /** clears the SOP instance UID for the Stored Print object.
   *  a new UID is assigned automatically when writing the object.
   */
  void clearInstanceUID() { sOPInstanceUID.clear(); }

  /** returns the image UIDs that are required to look up the referenced image in the database
   *  @param idx index, must be < getNumberOfImages()
   *  @param studyUID Study UID of the image
   *  @param seriesUID series UID of the image
   *  @param instanceUID instance UID of the image
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition getImageReference(size_t idx, const char *&studyUID, const char *&seriesUID, const char *&instanceUID)
  {
    return imageBoxContentList.getImageReference(idx, studyUID, seriesUID, instanceUID);
  }

  /** returns a description of the currently activated Presentation LUT (if
   *  any) in terms of the Presentation LUT matching rule (see description
   *  of enum type for details).
   *  @return Presentation LUT alignment
   */
  DVPSPrintPresentationLUTAlignment getReferencedPresentationLUTAlignment() { return referencedPresentationLUTAlignment; }

  /** Requests the properties of the printer (Printer SOP Instance N-GET).
   *  The properties are not returned, but if the message handler is switched to "dump mode",
   *  the DIMSE communication will be printed.
   *  @param printHandler print communication handler, association must be open.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition printSCUgetPrinterInstance(DVPSPrintMessageHandler& printHandler);

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
  OFCondition printSCUpreparePresentationLUT(
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
  OFCondition printSCUcreateBasicFilmSession(
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
  OFCondition printSCUcreateBasicFilmBox(DVPSPrintMessageHandler& printHandler, OFBool plutInSession);

  /** Transmits a DICOM image to the printer (Basic Grayscale Image Box N-SET).
   *  @param printHandler print communication handler, association must be open.
   *  @param idx index of the image reference from which the Image Box settings are taken,
   *     must be < getNumberOfImages().
   *  @param image DICOM image to be printed
   *  @param useMonochrome1 if true, the image is transmitted in MONOCHROME1 photometric interpretation.
   *    Default is false, image is transmitted in MONOCHROME2 in this case.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition printSCUsetBasicImageBox(
    DVPSPrintMessageHandler& printHandler,
    size_t idx,
    DicomImage& image,
    OFBool useMonochrome1=OFFalse);

  /** Transmits a DICOM annotation to the printer (Basic Annotation Box N-SET).
   *  @param printHandler print communication handler, association must be open.
   *  @param idx index of the annotation from which the settings are taken,
   *     must be < getNumberOfAnnotations().
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition printSCUsetBasicAnnotationBox(
    DVPSPrintMessageHandler& printHandler,
    size_t idx);

  /** Prints the current DICOM Basic Film Box SOP Instance.
   *  @param printHandler print communication handler, association must be open.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition printSCUprintBasicFilmBox(DVPSPrintMessageHandler& printHandler);

  /** Prints the current DICOM Basic Film Session.
   *  @param printHandler print communication handler, association must be open.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition printSCUprintBasicFilmSession(DVPSPrintMessageHandler& printHandler);

  /** Deletes all objects currently present in the print association.
   *  @param printHandler print communication handler, association must be open.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition printSCUdelete(DVPSPrintMessageHandler& printHandler);

  /** sets the illumination to be used
   *  with the print Presentation LUT SOP Class.
   *  @param value new attribute value, in cd/m2.
   *    The caller is responsible for making sure
   *    that the value is valid for the selected printer.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setPrintIllumination(Uint16 value);

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
  OFCondition setPrintReflectedAmbientLight(Uint16 value);

  /** gets the current reflected ambient light setting
   *  used with the print Presentation LUT SOP Class.
   *  @return reflected ambient light in cd/m2
   */
  Uint16 getPrintReflectedAmbientLight();

  /** performs a Print SCP Basic Film Box N-CREATE operation on a newly
   *  created instance of this class. The results of the operation are
   *  stored in the objects passed as rsp, rspDataset and
   *  globalPresentationLUTList.
   *  @param cfg config file facility
   *  @param cfgname symbolic printer name in config file
   *  @param rqDataset N-CREATE request dataset, may be NULL
   *  @param rsp N-CREATE response message
   *  @param rspDataset N-CREATE response dataset passed back in this parameter
   *  @param presentationLUTnegotiated
   *    OFTrue if support for the Presentation LUT SOP class
   *    has been negotiated at association negotiation and is supported on
   *    Basic Film Box level
   *  @param globalPresentationLUTList
   *    list of presentation LUTs managed by the Print SCP.
   *    If a SCP default Presentation LUT needs to be created as the result
   *    of the N-CREATE operation, it is stored in this list.
   *  @param filmSessionUID
   *    SOP instance UID of the Basic Film Session object
   *  @param study study UID to be used when storing Stored Print or image objects
   *  @param psSeries series UID to be used when storing Stored Print objects
   *  @param imgSeries series UID to be used when storing image objects (Hardcopy Grayscale)
   *  @return OFTrue if N-CREATE was successful, OFFalse otherwise.
   */
  OFBool printSCPCreate(
    DVConfiguration& cfg,
    const char *cfgname,
    DcmDataset *rqDataset,
    T_DIMSE_Message& rsp,
    DcmDataset *& rspDataset,
    OFBool presentationLUTnegotiated,
    DVPSPresentationLUT_PList& globalPresentationLUTList,
    const char *filmSessionUID,
    DcmUniqueIdentifier& study,
    DcmUniqueIdentifier& psSeries,
    DcmUniqueIdentifier& imgSeries);

  /** performs a Print SCP Basic Film Box N-SET operation on an instance of
   *  this class. The results of the N-SET operation are stored in the
   *  objects passed as rsp and rspDataset.
   *  @param cfg config file facility
   *  @param cfgname symbolic printer name in config file
   *  @param rqDataset N-SET request dataset
   *  @param rsp N-SET response message
   *  @param rspDataset N-SET response dataset passed back in this parameter
   *  @param presentationLUTnegotiated
   *    OFTrue if support for the Presentation LUT SOP class
   *    has been negotiated at association negotiation and is supported on
   *    Basic Film Box level
   *  @param globalPresentationLUTList
   *    list of presentation LUTs managed by the Print SCP
   *  @return OFTrue if N-SET was successful, OFFalse otherwise.
   */
  OFBool printSCPSet(
    DVConfiguration& cfg,
    const char *cfgname,
    DcmDataset *rqDataset,
    T_DIMSE_Message& rsp,
    DcmDataset *& rspDataset,
    OFBool presentationLUTnegotiated,
    DVPSPresentationLUT_PList& globalPresentationLUTList);

  /** checks whether the given UID string matches the film box UID.
   *  @param c uid to be compared
   *  @return OFTrue if equal, OFFalse otherwise
   */
  OFBool isFilmBoxInstance(const char *c) { if (c && (filmBoxInstanceUID == c)) return OFTrue; else return OFFalse; }

  /** checks whether the Presentation LUT with the given UID
   *  is referenced by this Stored Print object on the film box level.
   *  Presentation LUT references on Image Box level are ignored.
   *  @param c uid to be compared
   *  @return OFTrue if equal, OFFalse otherwise
   */
  OFBool usesPresentationLUT(const char *c);

  /** looks up the image box with the given SOP instance UID in the image box list
   *  and returns a pointer to a new object containing a copy of this
   *  image box. If the object is not found, NULL is returned.
   *  @param uid SOP instance UID of the image box to be looked up
   *  @return pointer to copied image box object, may be NULL.
   */
  DVPSImageBoxContent *duplicateImageBox(const char *uid) { return imageBoxContentList.duplicateImageBox(uid); }

  /** checks whether any of the image boxes managed by the image box list
   *  has the same position as the given one, but a different
   *  SOP instance UID.  This is used during a Print SCP basic grayscale
   *  image box N-SET operation to check whether an image position clash exists.
   *  @param uid SOP instance UID of the image box to be looked up
   *  @param position image position to be looked up
   */
  OFBool haveImagePositionClash(const char *uid, Uint16 position)  { return imageBoxContentList.haveImagePositionClash(uid, position); }

  /** adds the given image box object to the image box list.
   *  Any other object existing in the list with the same SOP instance UID is removed.
   *  Used during a Print SCP basic grayscale image box N-SET operation.
   *  @param newImageBox new image box object to be added to the list.
   */
  void replaceImageBox(DVPSImageBoxContent *newImageBox) { imageBoxContentList.replace(newImageBox); }

  /** updates the list of Presentation LUTs managed by the Stored Print object
   *  from a global list. If a Presentation LUT is active on Film Box level, the corresponding
   *  LUT is copied from the global presentation LUT list.
   *  Presentation LUT references on Image Box level are ignored.
   *  Used during a Print SCP N-ACTION operation.
   *  @param globalPresentationLUTList list of presentation LUTs managed by the Print SCP
   */
  void updatePresentationLUTList(DVPSPresentationLUT_PList& globalPresentationLUTList);

  /** checks whether the given Presentation LUT type could be used together
   *  with all image boxes in this film box on a Print SCP that requires a matching
   *  alignment between a Presentation LUT and the image pixel data.
   *  @param align LUT alignment type
   *  @return OFTrue if matching, OFFalse otherwise
   */
  OFBool matchesPresentationLUT(DVPSPrintPresentationLUTAlignment align) const
  {
    return imageBoxContentList.matchesPresentationLUT(align);
  }

  /** replaces the settings for illumination, reflected ambient light and
   *  referenced Presentation LUT in this film box.
   *  Used by a Print SCP if Presentation LUT is implemented on Film Session
   *  level.
   *  @param newIllumination new value for illumination
   *  @param newReflectedAmbientLight new value for reflectedAmbientLight
   *  @param newReferencedPLUT new value for referenced presentation LUT instance UID
   *  @param newAlignment new alignment type of active presentation LUT
   */
  void overridePresentationLUTSettings(
      DcmUnsignedShort& newIllumination,
      DcmUnsignedShort& newReflectedAmbientLight,
      DcmUniqueIdentifier& newReferencedPLUT,
      DVPSPrintPresentationLUTAlignment newAlignment);

  /** checks whether any of the image boxes has an image box position
   *  assigned. If no image box position is assigned, the stored print object
   *  cannot be written and a Print SCP should return a warning
   *  status upon receipt of an N-ACTION request.
   *  @return OFTrue if empty page (no image box position assigned), OFFalse otherwise.
   */
  OFBool emptyPageWarning() { return imageBoxContentList.emptyPageWarning(); }

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
  OFCondition createDefaultValues();

  /** writes a Referenced Presentation LUT SQ to the given
   *  dataset. Helper function used in the more general write() method.
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addReferencedPLUTSQ(DcmItem &dset);

  /** writes a Referenced Presentation LUT SQ, Illumination and
   *  reflected ambient light to the given dataset.
   *  Helper function used when creating Basic Film Session or
   *  Basic Film Box.
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addPresentationLUTReference(DcmItem& dset);

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
  DcmPersonName            referringPhysicianName;
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

  // PrinterCharacteristicsSequence
  /// Module=Printer_Characteristics_Module, VR=AE, VM=1, Type 2
  DcmApplicationEntity     originator;
  /// Module=Printer_Characteristics_Module, VR=AE, VM=1, Type 2
  DcmApplicationEntity     destination;
  /// Module=Printer_Characteristics_Module, VR=LO, VM=1, Type 3
  DcmLongString            printerName;

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
  /// the ReferencedPresentationLUTSequence is only created/read on the fly
  DcmUniqueIdentifier      referencedPresentationLUTInstanceUID;

  /** The Print SCP can be configured to enforce a rule requiring that the
   *  number of entries in a Presentation LUT matches the bit depth of the
   *  image pixel data. This member variable describes the type of the
   *  current presentation LUT (if any).
   */
  DVPSPrintPresentationLUTAlignment referencedPresentationLUTAlignment;

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

  /** the current presentation LUT instance. If used as Print SCU, the
   *  content of this string can differ from referencedPresentationLUTInstanceUID
   *  which contains the UID from the Stored Print object wheras this
   *  string contains the UID assigned by the remote Print SCP.
   *  If used as Print SCP, these strings should always be equal.
   */
  OFString presentationLUTInstanceUID;

  /// stores the "global" presentation LUT that overrides the image box LUTs (optional)
  DVPSPresentationLUT globalPresentationLUT;

  /// flag indicating whether the globalPresentationLUT is currently valid or not
  OFBool globalPresentationLUTValid;

  /// transmit images in 12 bit for the current print job
  OFBool transmitImagesIn12Bit;

  /// presentation LUTs are rendered on SCP side
  OFBool renderPresentationLUTinSCP;

  /// temporary buffer for getMaxDensity and getMinDensity
  OFString tempDensity;

};

#endif
