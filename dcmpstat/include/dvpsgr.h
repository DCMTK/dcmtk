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
 *    classes: DVPSGraphicObject
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:28 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSGR_H__
#define __DVPSGR_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctk.h"

/** an item of the graphic object sequence in a presentation state (internal use only).
 *  This class manages the data structures comprising one item
 *  of the Graphic Object Sequence which is contained
 *  in the Graphic Annotation Sequence in a Presentation State object.
 */

class DVPSGraphicObject
{
public:
  /// default constructor
  DVPSGraphicObject();
  
  /// copy constructor
  DVPSGraphicObject(const DVPSGraphicObject& copy);

  /** clone method.
   *  @return a pointer to a new DVPSGraphicObject object containing
   *  a copy of this object.
   */
  DVPSGraphicObject *clone() { return new DVPSGraphicObject(*this); }

  /// destructor
  virtual ~DVPSGraphicObject();

  /** reads a graphic object from a DICOM dataset.
   *  The DICOM elements of the Graphic Object item are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the item of the GraphicObjectSequence from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);

  /** writes the graphic object managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the GraphicObjectSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);

private:
  /// VR=CS, VM=1, Type 1 
  DcmCodeString            graphicAnnotationUnits;
  /// VR=US, VM=1, Type 1 
  DcmUnsignedShort         graphicDimensions;
  /// VR=US, VM=1, Type 1 
  DcmUnsignedShort         numberOfGraphicPoints;
  /// VR=FL, VM=2-n, Type 1 
  DcmFloatingPointSingle   graphicData;
  /// VR=CS, VM=1, Type 1 
  DcmCodeString            graphicType;
  /// VR=CS, VM=1, Type 1c 
  DcmCodeString            graphicFilled;
};

#endif

/*
 *  $Log: dvpsgr.h,v $
 *  Revision 1.1  1998-11-27 14:50:28  meichel
 *  Initial Release.
 *
 *
 */

