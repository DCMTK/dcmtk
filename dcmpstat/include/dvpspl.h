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
 *    classes: DVPSPresentationLUT
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-07-30 13:34:49 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSPL_H__
#define __DVPSPL_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dctk.h"
#include "dvpstyp.h"     /* for enum types */

class DicomImage;

/** the representation of a Presentation LUT Content SQ item for Stored Print
 */  

class DVPSPresentationLUT
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
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);
  
  /** writes the Presentation LUT managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the PresentationLUTContentSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);

  /** checks whether current presentation LUT is inverse, i.e.
   *  shape is INVERSE or first LUT entry larger than last LUT entry.
   */
  OFBool isInverse();

  /** gets the current Presentation LUT type.
   *  @return the current presentation LUT type
   */
  DVPSPresentationLUTType getType() { return presentationLUT; }

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

  /** stores a presentation lookup table and activates it.
   *  The LUT is copied. If the method returns EC_Normal,
   *  any old presentation LUT is overwritten.
   *  If the method returns an error code, an old LUT is left unchanged.
   *  @param lutDescriptor the LUT Descriptor in DICOM format (VM=3)
   *  @param lutData the LUT Data in DICOM format
   *  @param lutExplanation the LUT Explanation in DICOM format, may be empty.
   *  @return EC_Normal if successful, an error code otherwise.
   */ 
  E_Condition setLUT(
    DcmUnsignedShort& lutDescriptor,
    DcmUnsignedShort& lutData,
    DcmLongString& lutExplanation);

  /** sets the current Presentation LUT type.
   *  DVPSP_table can only be used if the object
   *  contains a lookup table, i.e. if haveTable() returns OFTrue.
   *  @param newType the new presentation LUT type.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setType(DVPSPresentationLUTType newType);

  /** inverts presentation LUT or presentation state LUT shape.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition invert();

  /** activates the current presentation transform in the given DicomImage.
   *  @param image the DicomImage for which the presentation transform is to be set.
   *  @return OFTrue if successful, OFFalse otherwise.
   */
  OFBool activate(DicomImage *image);
  
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

};

#endif

/*
 *  $Log: dvpspl.h,v $
 *  Revision 1.1  1999-07-30 13:34:49  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */

