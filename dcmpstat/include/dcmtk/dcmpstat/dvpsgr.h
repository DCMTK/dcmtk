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
 *    classes: DVPSGraphicObject
 *
 */

#ifndef DVPSGR_H
#define DVPSGR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmpstat/dvpstyp.h"
#include "dcmtk/dcmpstat/dpdefine.h"

/** an item of the graphic object sequence in a presentation state (internal use only).
 *  This class manages the data structures comprising one item
 *  of the Graphic Object Sequence which is contained
 *  in the Graphic Annotation Sequence in a Presentation State object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSGraphicObject
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
  OFCondition read(DcmItem &dset);

  /** writes the graphic object managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the GraphicObjectSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

  /** gets the graphic annotation units.
   *  @return annotation units
   */
  DVPSannotationUnit getAnnotationUnits();

  /** gets the number of graphic points.
   *  @return number of graphic points
   */
  size_t getNumberOfPoints();

  /** gets one point from the graphic data.
   *  @param idx index of the graphic point, must be < getNumberOfPoints();
   *  @param x upon success the x value of the point is returned in this parameter
   *  @param y upon success the y value of the point is returned in this parameter
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition getPoint(size_t idx, Float32& x, Float32& y);

  /** gets the graphic type of this graphic object.
   *  @return graphic type
   */
  DVPSGraphicType getGraphicType();

  /** checks if the graphic is filled
   *  @return OFTrue if graphic is filled.
   */
  OFBool isFilled();

  /** sets the graphic data for this graphic object.
   *  @param number number of graphic points in parameter "data"
   *  @param data pointer to an array of Float32 values with a size of (at least)
   *  2*number. The values are copied into the graphic object.
   *  @param unit the graphic annotation units for this data.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setData(size_t number, const Float32 *data, DVPSannotationUnit unit);

  /** sets the graphic type for the graphic object
   *  @param gtype the graphic type
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setGraphicType(DVPSGraphicType gtype);

  /** sets the graphic filled status for the graphic object
   *  @param isFilled OFTrue if graphic is filled, OFFalse otherwise.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setFilled(OFBool filled);

private:

  /// private undefined assignment operator
  DVPSGraphicObject& operator=(const DVPSGraphicObject&);

  /// VR=CS, VM=1, Type 1
  DcmCodeString            graphicAnnotationUnits;
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
