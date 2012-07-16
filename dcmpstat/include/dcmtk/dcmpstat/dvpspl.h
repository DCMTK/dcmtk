/*
 *
 *  Copyright (C) 1998-2012, OFFIS e.V.
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
 *    classes: DVPSPresentationLUT
 *
 */

#ifndef DVPSPL_H
#define DVPSPL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpstyp.h"     /* for enum types */
#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcvrlo.h"

struct T_DIMSE_Message;

class DicomImage;
class DiLookupTable;
class DcmDataset;

/** the representation of a Presentation LUT Content SQ item for Stored Print
 */  

class DCMTK_DCMPSTAT_EXPORT DVPSPresentationLUT
{
public:
  /// default constructor
  DVPSPresentationLUT();
  
  /// copy constructor
  DVPSPresentationLUT(const DVPSPresentationLUT& copy);

  /** clone method.
   *  @return a pointer to a new DVPSPresentationLUT object containing
   *  a copy of this object.
   */
  DVPSPresentationLUT *clone() { return new DVPSPresentationLUT(*this); }

  /// destructor
  virtual ~DVPSPresentationLUT();

  /** resets the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** reads an Presentation LUT from a DICOM dataset.
   *  The DICOM elements of the referenced item are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the item of the PresentationLUTContentSequence from which the data is to be read
   *  @param withSOPInstance true if SOPinstanceUID should be read (when used with Stored Print).
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset, OFBool withSOPInstance);
  
  /** writes the Presentation LUT managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the PresentationLUTContentSequence to which the data is written
   *  @param withSOPInstance true if SOPinstanceUID should be written (when used with Stored Print).
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset, OFBool withSOPInstance);

  /** checks whether current presentation LUT is inverse, i.e.
   *  shape is INVERSE or first LUT entry larger than last LUT entry.
   */
  OFBool isInverse();

  /** gets the current Presentation LUT type.
   *  @return the current presentation LUT type
   */
  DVPSPresentationLUTType getType() { return presentationLUT; }

  /** gets a description of the Presentation LUT in terms of
   *  its restrictions for use with a Print SCP that requires
   *  Presentation LUT number of entries to match the bit depth
   *  of the image pixel data.
   *  @return the current presentation LUT alignment type
   */
  DVPSPrintPresentationLUTAlignment getAlignment();
  
  /** checks if a real Presentation LUT (not shape) is available.
   *  @return OFTrue if this object contains
   *    a presentation LUT, no matter if it is activated or not.
   *    Returns OFFalse otherwise.
   */
  OFBool haveTable();

  /** gets a description of the current presentation LUT.
   *  For well-known presentation LUT shapes, a standard text
   *  is returned. For presentation LUTs, the LUT explanation
   *  is returned if it exists and a standard text otherwise.
   *  This method never returns NULL.
   *  @return a pointer to a string describing the current presentation LUT.
   */
  const char *getCurrentExplanation();

  /** returns the LUT explanation of the presentation LUT if it exists and is non-empty. 
   *  Otherwise returns NULL.
   *  @return a string pointer
   */
  const char *getLUTExplanation();

  /** returns the SOP instance UID of the presentation LUT if present. 
   *  Otherwise returns NULL.
   *  @return a string pointer
   */
  const char *getSOPInstanceUID();

  /** stores a presentation lookup table and activates it.
   *  The LUT is copied. If the method returns EC_Normal,
   *  any old presentation LUT is overwritten.
   *  If the method returns an error code, an old LUT is left unchanged.
   *  @param lutDescriptor the LUT Descriptor in DICOM format (VM=3)
   *  @param lutData the LUT Data in DICOM format
   *  @param lutExplanation the LUT Explanation in DICOM format, may be empty.
   *  @return EC_Normal if successful, an error code otherwise.
   */ 
  OFCondition setLUT(
    DcmUnsignedShort& lutDescriptor,
    DcmUnsignedShort& lutData,
    DcmLongString& lutExplanation);

  /** sets the current Presentation LUT type.
   *  DVPSP_table can only be used if the object
   *  contains a lookup table, i.e. if haveTable() returns OFTrue.
   *  @param newType the new presentation LUT type.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setType(DVPSPresentationLUTType newType);

  /** inverts presentation LUT or presentation state LUT shape.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition invert();

  /** activates the current presentation transform in the given DicomImage.
   *  @param image the DicomImage for which the presentation transform is to be set.
   *  @param printLUT OFTrue if presentation LUT is activated for print bitmap rendering
   *    (in this case there is no implicit scaling of the input width of the LUT and,
   *    therefore, the VOI transformation - which is absent for print - is used),
   *    OFFalse otherwise (softcopy rendering, default)
   *  @return OFTrue if successful, OFFalse otherwise.
   */
  OFBool activate(DicomImage *image, OFBool printLUT = OFFalse);

  /** activates the inverse LUT of the current presentation LUT (if any) in the given DicomImage.
   *  Presentation LUT shape is not supported by this method.
   *  @param image the DicomImage for which the inverse presentation LUT is to be set.
   *  @return OFTrue if successful, OFFalse otherwise.
   */
  OFBool activateInverseLUT(DicomImage *image);

  /** creates a DiLookupTable instance from the LUT table
   *  managed by this object. The returned object must be freed by the caller.
   *  @return new DiLookupTable object, may be NULL if no LUT present.
   */
  DiLookupTable *createDiLookupTable();
  
  /** compares a DiLookupTable instance with the LUT table
   *  managed by this object. Returns OFTrue if equal, OFFalse otherwise.
   *  @param lut DiLookupTable object to compare with
   *  @return comparison, true if equal
   */
  OFBool compareDiLookupTable(DiLookupTable *lut);

  /** sets the SOP instance UID.
   *  @param value new attribute value, must not be NULL.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setSOPInstanceUID(const char *value);
  
  /** checks whether the current Presentation LUT (or shape) is
   *  legal when used with Supplement 22.
   *  @return true if current Presentation LUT is legal for print.
   */
  OFBool isLegalPrintPresentationLUT();
  
  /** checks whether the current Presentation LUT (or shape)
   *  matches the current image bit depth in number of entries and first value mapped.
   *  @param is12bit true if the image is 12 bit, false if the image is 8 bit
   *  @return true if Presentation LUT matches, false otherwise.
   */
  OFBool matchesImageDepth(OFBool is12bit);

  /** performs a Print SCP N-CREATE operation on a newly created instance of 
   *  this class. The results of the operation are stored in the objects 
   *  passed as rsp and rspDataset.
   *  @param rqDataset N-CREATE request dataset, may be NULL
   *  @param rsp N-CREATE response message
   *  @param rspDataset N-CREATE response dataset passed back in this parameter
   *  @param matchRequired boolean flag specifying whether the SCP should
   *    enforce a rule that all Presentation LUTs must match with the
   *    bit depth of the image pixel data.
   *  @param supports12Bit boolean flag specifying whether the SCP should
   *    allow 12 bit/pixel image data transmission. Affects handling of 
   *    matching rule.
   *  @return OFTrue if operation was successful, OFFalse otherwise.
   */
  OFBool printSCPCreate(
    DcmDataset *rqDataset, 
    T_DIMSE_Message& rsp, 
    DcmDataset *& rspDataset, 
    OFBool matchRequired,
    OFBool supports12Bit);

private:
  /// private undefined assignment operator
  DVPSPresentationLUT& operator=(const DVPSPresentationLUT&);
	
  /// describes active type of presentation LUT.
  DVPSPresentationLUTType  presentationLUT;
  /// Module=Softcopy_Presentation_LUT, VR=xs, VM=3, Type 1c 
  DcmUnsignedShort         presentationLUTDescriptor;
  /// Module=Softcopy_Presentation_LUT, VR=LO, VM=1, Type 3 
  DcmLongString            presentationLUTExplanation;
  /// Module=Softcopy_Presentation_LUT, VR=xs, VM=1-n, Type 1c 
  DcmUnsignedShort         presentationLUTData;

  /// Module=Presentation_LUT_List, VR=UI, VM=1, Type 1 
  DcmUniqueIdentifier      sOPInstanceUID;

};

#endif
