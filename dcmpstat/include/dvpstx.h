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
 *    classes: DVPSTextObject
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:34 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSTX_H__
#define __DVPSTX_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctk.h"


/** an item of the text object sequence in a presentation state (internal use only).
 *  This class manages the data structures comprising one item
 *  of the Text Object Sequence which is contained
 *  in the Graphic Annotation Sequence in a Presentation State object.
 */

class DVPSTextObject
{
public:
  /// default constructor
  DVPSTextObject();
  
  /// copy constructor
  DVPSTextObject(const DVPSTextObject& copy);

  /** clone method.
   *  @return a pointer to a new DVPSTextObject object containing
   *  a copy of this object.
   */
  DVPSTextObject *clone() { return new DVPSTextObject(*this); }

  /// destructor
  virtual ~DVPSTextObject();

  /** reads a text object from a DICOM dataset.
   *  The DICOM elements of the Text Object item are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the item of the TextObjectSequence from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);

  /** writes the text object managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the TextObjectSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);
  
private:
  /// VR=CS, VM=1-n, Type 1c
  DcmCodeString            specificCharacterSet;
  /// VR=CS, VM=1, Type 1c
  DcmCodeString            boundingBoxAnnotationUnits;
  /// VR=CS, VM=1, Type 1c
  DcmCodeString            anchorPointAnnotationUnits;
  /// VR=ST, VM=1, Type 1
  DcmShortText             unformattedTextValue;
  /// VR=FL, VM=2, Type 1c
  DcmFloatingPointSingle   boundingBoxTLHC;
  /// VR=FL, VM=2, Type 1c
  DcmFloatingPointSingle   boundingBoxBRHC;
  /// VR=FL, VM=2, Type 1c
  DcmFloatingPointSingle   anchorPoint;
  /// VR=CS, VM=1, Type 1c
  DcmCodeString            anchorPointVisibility;
};

#endif

/*
 *  $Log: dvpstx.h,v $
 *  Revision 1.1  1998-11-27 14:50:34  meichel
 *  Initial Release.
 *
 *
 */

