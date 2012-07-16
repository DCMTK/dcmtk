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
 *    classes: DVPSTextObject
 *
 */

#ifndef DVPSTX_H
#define DVPSTX_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmpstat/dvpstyp.h"
#include "dcmtk/dcmpstat/dpdefine.h"

/** an item of the text object sequence in a presentation state (internal use only).
 *  This class manages the data structures comprising one item
 *  of the Text Object Sequence which is contained
 *  in the Graphic Annotation Sequence in a Presentation State object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSTextObject
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
  OFCondition read(DcmItem &dset);

  /** writes the text object managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the TextObjectSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

   /** checks if this text object contains an anchor point.
    *  @return OFTrue if anchor point present
    */
   OFBool haveAnchorPoint();

   /** checks if this text object contains bounding box.
    *  @return OFTrue if bounding box present
    */
   OFBool haveBoundingBox();

   /** sets an anchor point for this text object.
    *  @param x anchor point X value
    *  @param y anchor point Y value
    *  @param unit anchor point annotation units (pixel/display)
    *  @param isVisible anchor point visibility
    *  @return EC_Normal if successful, an error code otherwise.
    */
   OFCondition setAnchorPoint(double x, double y, DVPSannotationUnit unit, OFBool isVisible);

   /** sets bounding box for this text object.
    *  @param TLHC_x bounding box top-lefthand corner X value
    *  @param TLHC_x bounding box top-lefthand corner Y value
    *  @param BRHC_x bounding box bottom-righthand corner X value
    *  @param BRHC_x bounding box bottom-righthand corner Y value
    *  @param unit bounding box annotation units (pixel/display)
    *  @param justification bounding box horizontal justification (left/right/center)
    *  @return EC_Normal if successful, an error code otherwise.
    */
   OFCondition setBoundingBox(double TLHC_x, double TLHC_y, double BRHC_x,
     double BRHC_y, DVPSannotationUnit unit, DVPSTextJustification justification);

   /** assigns a new "unformatted text value" for this text object.
    *  @param text unformatted text value. Must not be NULL or empty string.
    *  @return EC_Normal if successful, an error code otherwise.
    */
   OFCondition setText(const char *text);

   /** removes any anchor point from the text object.
    *  Attention: A text object must always contain either anchor point, bounding box
    *  or both. This property is not asserted by the text object itself.
    */
   void removeAnchorPoint();

   /** removes any bounding box from the text object.
    *  Attention: A text object must always contain either anchor point, bounding box
    *  or both. This property is not asserted by the text object itself.
    */
   void removeBoundingBox();

   /** gets the unformatted text value for this text object.
    *  @return unformatted text value
    */
   const char *getText();

   /** gets the bounding box TLHC x value.
    *  May only be called when a bounding box is present (haveBoundingBox()==OFTrue)
    *  @return bounding box TLHC x value
    */
   double getBoundingBoxTLHC_x();

   /** gets the bounding box TLHC y value.
    *  May only be called when a bounding box is present (haveBoundingBox()==OFTrue)
    *  @return bounding box TLHC y value
    */
   double getBoundingBoxTLHC_y();

   /** gets the bounding box BRHC x value.
    *  May only be called when a bounding box is present (haveBoundingBox()==OFTrue)
    *  @return bounding box BRHC x value
    */
   double getBoundingBoxBRHC_x();

   /** gets the bounding box BRHC y value.
    *  May only be called when a bounding box is present (haveBoundingBox()==OFTrue)
    *  @return bounding box BRHC y value
    */
   double getBoundingBoxBRHC_y();

   /** gets the bounding box annotation units.
    *  May only be called when a bounding box is present (haveBoundingBox()==OFTrue)
    *  @return bounding box annotation units
    */
   DVPSannotationUnit getBoundingBoxAnnotationUnits();

   /** gets the bounding box horizontal justification.
    *  May only be called when a bounding box is present (haveBoundingBox()==OFTrue)
    *  @return bounding box horizontal justification
    */
   DVPSTextJustification getBoundingBoxHorizontalJustification();

   /** gets the anchor point x value.
    *  May only be called when an anchor point is present (haveAnchorPoint()==OFTrue)
    *  @return anchor point x value
    */
   double getAnchorPoint_x();

   /** gets the anchor point y value.
    *  May only be called when an anchor point is present (haveAnchorPoint()==OFTrue)
    *  @return anchor point y value
    */
   double getAnchorPoint_y();

   /** gets the anchor point visibility
    *  May only be called when an anchor point is present (haveAnchorPoint()==OFTrue)
    *  @return OFTrue if anchor point is visible
    */
   OFBool anchorPointIsVisible();

   /** gets the anchor point annotation units.
    *  May only be called when an anchor point is present (haveAnchorPoint()==OFTrue)
    *  @return anchor point annotation units
    */
   DVPSannotationUnit getAnchorPointAnnotationUnits();

private:
  /** private undefined assignment operator
   */
  DVPSTextObject& operator=(const DVPSTextObject&);

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
  /// VR=CS, VM=1, Type 1c
  DcmCodeString            boundingBoxTextHorizontalJustification;
  /// VR=FL, VM=2, Type 1c
  DcmFloatingPointSingle   anchorPoint;
  /// VR=CS, VM=1, Type 1c
  DcmCodeString            anchorPointVisibility;

};

#endif
