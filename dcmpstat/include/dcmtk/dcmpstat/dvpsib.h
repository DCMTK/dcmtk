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
 *    classes: DVPSImageBoxContent
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:36 $
 *  CVS/RCS Revision: $Revision: 1.24 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DVPSIB_H
#define DVPSIB_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmpstat/dvpstyp.h"     /* for enum types */
#include "dcmtk/dcmnet/dimse.h"

class DVPSPresentationLUT_PList;
class DVConfiguration;

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

  /** resets the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** reads an image box content item from a DICOM dataset.
   *  The DICOM elements of the referenced item are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the item of the ImageBoxContentSequence from which the data is to be read
   *  @param presentationLUTList list of presentation LUTs which may be referenced
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset, DVPSPresentationLUT_PList& presentationLUTList);

  /** writes the image box managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the ImageBoxContentSequence to which the data is written
   *  @param writeRequestedImageSize if false, the Requested Image Size attribute is not written,
   *    e.g. because it is not supported by the target printer.
   *  @param writeReferencedPLUTSQ if false, the Referenced Presentation LUT Sequence is not written,
   *    e.g. because a "general" presentation LUT is used instead of those referenced by the image boxes.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset, OFBool writeRequestedImageSize, OFBool writeReferencedPLUTSQ = OFTrue);

  /** create default values for all missing type 1 elements.
   *  Called before a stored print object is written.
   *  @param renumber if true, a new imageBoxPosition values is created
   *  @param number new imageBoxPosition to be assigned
   *  @param ignoreEmptyImages if true, an empty image box position does not cause an error.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition createDefaultValues(OFBool renumber, unsigned long number, OFBool ignoreEmptyImages);

  /** returns the referencedSOPClassUID from the ReferencedImageSequence
   *  @return referencedSOPClassUID string
   */
  const char *getSOPClassUID();

  /** sets the content of this image box object.
   *  @param instanceuid SOP instance UID of this image box
   *  @param retrieveaetitle retrieve AETITLE of the referenced image
   *  @param refstudyuid study instance UID of the referenced image
   *  @param refseriesuid series instance UID of the referenced image
   *  @param refsopclassuid SOP class UID of the referenced image
   *  @param refsopinstanceuid SOP instance UID of the referenced image
   *  @param requestedimagesize requested images size for this image
   *  @param patientid patient ID for the referenced image
   *  @param presentationlutreference referenced SOP instance UID for the referenced Presentation LUT,
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setContent(
    const char *instanceuid,
    const char *retrieveaetitle,
    const char *refstudyuid,
    const char *refseriesuid,
    const char *refsopclassuid,
    const char *refsopinstanceuid,
    const char *requestedimagesize,
    const char *patientid,
    const char *presentationlutreference);

  /** sets the (optional) requested decimate/crop behaviour for this image box.
   *  @param value new enumerated value. The caller is responsible for
   *    making sure that the selected printer supports decimate/crop
   *    if a non-default value is set.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setRequestedDecimateCropBehaviour(DVPSDecimateCropBehaviour value);

  /** gets the current requested decimate/crop behaviour setting
   *  that is used for this image box.
   *  @return requested decimate/crop behaviour
   */
  DVPSDecimateCropBehaviour getRequestedDecimateCropBehaviour();

  /** checks whether image box has additional settings
   *  that are not default.
   *  @return OFTrue if additional settings exist, OFFalse otherwise.
   */
  OFBool hasAdditionalSettings();

  /** gets the (optional) referenced Presentation LUT Instance UID.
   *  @return UID, may be NULL.
   */
  const char *getReferencedPresentationLUTInstanceUID();

  /** gets the polarity.
   *  @return polarity, may be NULL.
   */
  const char *getPolarity();

  /** gets the requested image size.
   *  @return requested image size, may be NULL.
   */
  const char *getRequestedImageSize();

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

  /** gets the current SOP Instance UID.
   *  @return SOP Instance UID, may be NULL.
   */
  const char *getSOPInstanceUID();

  /** gets the current image box position, 0 if none is set.
   *  @return image box position
   */
  Uint16 getImageBoxPosition();

  /** sets the polarity.
   *  @param value new attribute value (NORMAL or REVERSE), may be NULL.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setPolarity(const char *value);

  /** sets the requested image size.
   *  @param value new attribute value, may be NULL.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setRequestedImageSize(const char *value);

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

  /** sets the SOP instance UID (which is returned by the Print SCP).
   *  @param value new attribute value, must not be NULL.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setSOPInstanceUID(const char *value);

  /** sets magnification type, smoothing type and configuration information back to default.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setDefault();

  /** returns the image UIDs that are required to look up the referenced image in the database
   *  @param studyUID Study UID of the image
   *  @param seriesUID series UID of the image
   *  @param instanceUID instance UID of the image
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition getImageReference(const char *&studyUID, const char *&seriesUID, const char *&instanceUID);

  /** writes the attributes managed by this objects that are part of a
   *  basic grayscale image box N-SET request into the DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition prepareBasicImageBox(DcmItem &dset);

  /** checks whether the given Presentation LUT type could be used together
   *  with this image box on a Print SCP that requires a matching alignment
   *  between a Presentation LUT and the image pixel data.
   *  @param align LUT alignment type
   *  @return OFTrue if matching, OFFalse otherwise
   */
  OFBool matchesPresentationLUT(DVPSPrintPresentationLUTAlignment align) const;

  /** performs a Print SCP Basic Grayscale Image Box N-SET operation.
   *  The results of the N-SET operation are stored in the objects passed as
   *  rsp and rspDataset.
   *  @param cfg config file facility
   *  @param cfgname symbolic printer name in config file
   *  @param rqDataset N-SET request dataset
   *  @param rsp N-SET response message
   *  @param rspDataset N-SET response dataset passed back in this parameter
   *  @param imageDataset a hardcopy grayscale image (without general study
   *     and general series modules which must be added by the caller)
   *     containing the image data from the N-SET request is written to
   *     this dataset if the method returns successfully.
   *  @param align describes the current Presentation LUT. Used if the Print
   *     SCP has been configured to enforce a matching of Presentation LUT
   *     and pixel data bit depth.
   *  @param presentationLUTnegotiated
   *    OFTrue if support for the Presentation LUT SOP class
   *    has been negotiated at association negotiation
   *  @return OFTrue if N-SET operation was successful, OFFalse otherwise.
   */
  OFBool printSCPSet(
    DVConfiguration& cfg,
    const char *cfgname,
    DcmDataset *rqDataset,
    T_DIMSE_Message& rsp,
    DcmDataset *& rspDataset,
    DcmDataset &imageDataset,
    DVPSPrintPresentationLUTAlignment align,
    OFBool presentationLUTnegotiated);

  /** assigns new values for study instance UID, series instance UID
   *  and retrieve aetitle.
   *  @param studyUID new studyUID
   *  @param seriesUID new seriesUID
   *  @param aetitle new retrieve aetitle, must not be NULL.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setUIDsAndAETitle(
    DcmUniqueIdentifier& studyUID,
    DcmUniqueIdentifier& seriesUID,
    const char *aetitle);

private:
  /// private undefined assignment operator
  DVPSImageBoxContent& operator=(const DVPSImageBoxContent&);

  /** writes a Referenced Presentation LUT SQ to the given
   *  dataset. Helper function used in the more general write() method.
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addReferencedPLUTSQ(DcmItem &dset);

  /** evaluates the contents of the Basic Grayscale Image Sequence during a
   *  Print SCP Basic Grayscale Image Box N-SET operation.
   *  The results of the N-SET operation are stored in the object passed as rsp.
   *  @param cfg config file facility
   *  @param cfgname symbolic printer name in config file
   *  @param rqDataset first item of the Basic Grayscale Image Sequence
   *  @param rsp N-SET response message
   *  @param imageDataset a hardcopy grayscale image (without general study
   *     and general series modules which must be added by the caller)
   *     containing the image data from the N-SET request is written to
   *     this dataset if the method returns successfully.
   *  @param align describes the current Presentation LUT. Used if the Print
   *     SCP has been configured to enforce a matching of Presentation LUT
   *     and pixel data bit depth.
   *  @param presentationLUTnegotiated
   *    OFTrue if support for the Presentation LUT SOP class
   *    has been negotiated at association negotiation
   *  @return OFTrue if N-SET operation was successful, OFFalse otherwise.
   */
  OFBool printSCPEvaluateBasicGrayscaleImageSequence(
   DVConfiguration& cfg,
   const char *cfgname,
   DcmItem *rqDataset,
   T_DIMSE_Message& rsp,
   DcmDataset &imageDataset,
   DVPSPrintPresentationLUTAlignment align,
   OFBool presentationLUTnegotiated);

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

  // the ReferencedPresentationLUTSequence is only created/read on the fly
  DcmUniqueIdentifier      referencedPresentationLUTInstanceUID;

  /// describes whether the image depth is 8 bit or 12 bit.
  DVPSImageDepth           imageDepth;

};

#endif

/*
 *  $Log: dvpsib.h,v $
 *  Revision 1.24  2010-10-14 13:16:36  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.23  2010-10-07 14:31:35  joergr
 *  Removed leading underscore characters from preprocessor symbols (reserved).
 *
 *  Revision 1.22  2009-11-24 14:12:57  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.21  2005-12-08 16:03:50  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.20  2001/09/26 15:36:12  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.19  2001/06/01 15:50:17  meichel
 *  Updated copyright header
 *
 *  Revision 1.18  2000/07/04 15:58:01  joergr
 *  Added support for overriding the presentation LUT settings made for the
 *  image boxes.
 *
 *  Revision 1.17  2000/06/14 11:28:13  joergr
 *  Added methods to access the attributes Polarity and Requested Image Size.
 *
 *  Revision 1.16  2000/06/08 10:44:29  meichel
 *  Implemented Referenced Presentation LUT Sequence on Basic Film Session level.
 *    Empty film boxes (pages) are not written to file anymore.
 *
 *  Revision 1.15  2000/06/07 14:17:41  joergr
 *  Added methods to access the image polarity attribute.
 *
 *  Revision 1.14  2000/06/02 16:00:47  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.13  2000/05/31 12:56:38  meichel
 *  Added initial Print SCP support
 *
 *  Revision 1.12  2000/03/08 16:28:53  meichel
 *  Updated copyright header.
 *
 *  Revision 1.11  1999/10/07 17:21:47  meichel
 *  Reworked management of Presentation LUTs in order to create tighter
 *    coupling between Softcopy and Print.
 *
 *  Revision 1.10  1999/09/24 15:23:44  meichel
 *  Print spooler (dcmprtsv) now logs diagnostic messages in log files
 *    when operating in spool mode.
 *
 *  Revision 1.9  1999/09/17 14:33:56  meichel
 *  Completed print spool functionality including Supplement 22 support
 *
 *  Revision 1.8  1999/09/15 17:43:27  meichel
 *  Implemented print job dispatcher code for dcmpstat, adapted dcmprtsv
 *    and dcmpsprt applications.
 *
 *  Revision 1.7  1999/09/10 12:46:46  meichel
 *  Added implementations for a number of print API methods.
 *
 *  Revision 1.6  1999/09/09 14:57:32  thiel
 *  Added methods for print spooler
 *
 *  Revision 1.5  1999/09/01 16:14:40  meichel
 *  Added support for requested image size to print routines
 *
 *  Revision 1.4  1999/08/31 14:09:10  meichel
 *  Added get/set methods for stored print attributes
 *
 *  Revision 1.3  1999/08/27 15:57:55  meichel
 *  Added methods for saving hardcopy images and stored print objects
 *    either in file or in the local database.
 *
 *  Revision 1.2  1999/08/26 09:30:59  thiel
 *  Add extensions for the usage of the StoredPrint
 *
 *  Revision 1.1  1999/07/30 13:34:47  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */
