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
 *    classes: DVPSGraphicLayer
 *
 */

#ifndef DVPSGL_H
#define DVPSGL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmpstat/dpdefine.h"


/** an item of the graphic layer sequence in a presentation state (internal use only).
 *  This class manages the data structures comprising one item
 *  of the Graphic Layer Sequence in a Presentation State object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSGraphicLayer
{
public:
  /// default constructor
  DVPSGraphicLayer();

  /// copy constructor
  DVPSGraphicLayer(const DVPSGraphicLayer& copy);

  /** clone method.
   *  @return a pointer to a new DVPSGraphicLayer object containing
   *  a copy of this object.
   */
  DVPSGraphicLayer *clone() { return new DVPSGraphicLayer(*this); }

  /// destructor
  virtual ~DVPSGraphicLayer();

  /** reads a graphic layer from a DICOM dataset.
   *  The DICOM elements of the Graphic Layer item are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the item of the GraphicLayerSequence from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);

  /** writes the graphic layer managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the GraphicLayerSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

  /** get graphic layer name of this layer.
   *  @return a pointer to the graphic layer name
   */
  const char *getGL();

  /** get graphic layer description of this layer.
   *  @return a pointer to the graphic layer description
   */
  const char *getGLDescription();

  /** gets the graphic layer order of this layer.
   *  @return the graphic layer order
   */
  Sint32 getGLOrder();

  /** checks whether a recommended display value (grayscale, color or both) exists.
   *  @return OFTrue if a recommended display value exists
   */
  OFBool haveGLRecommendedDisplayValue();

  /** gets the recommended grayscale display value.
   *  If the graphic layer contains an RGB display value but no grayscale
   *  display value, the RGB value is implicitly converted to grayscale.
   *  @param gray the recommended display value as an unsigned 16-bit P-value
   *    is returned in this parameter.
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition getGLRecommendedDisplayValueGray(Uint16& gray);

  /** gets the recommended RGB display value.
   *  If the graphic layer contains a grayscale display value but no RGB
   *  display value, the grayscale value is implicitly converted to RGB.
   *  @param r returns the R component of the recommended display value as unsigned 16-bit P-value
   *  @param g returns the G component of the recommended display value as unsigned 16-bit P-value
   *  @param b returns the B component of the recommended display value as unsigned 16-bit P-value
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition getGLRecommendedDisplayValueRGB(Uint16& r, Uint16& g, Uint16& b);

  /** removes recommended display values.
   *  @param rgb if true, the RGB recommended display value is removed
   *  @param monochrome if true the monochrome recommended display value is removed
   */
  void removeRecommendedDisplayValue(OFBool rgb, OFBool monochrome);

  /** set graphic layer name of this layer.
   *  @param gl a pointer to the graphic layer name, which is copied into this object.
   */
  void setGL(const char *gl);

  /** set graphic layer order of this layer.
   *  @param glOrder the graphic layer order.
   */
  void setGLOrder(Sint32 glOrder);

  /** set graphic layer recommended grayscale display value of this layer.
   *  This method does not affect (set or modify) the recommended RGB display value
   *  which should be set separately.
   *  @param gray the recommended display value in P-values 0..0xffff.
   */
  void setGLRecommendedDisplayValueGray(Uint16 gray);

  /** set graphic layer recommended RGB display value of this layer.
   *  This method does not affect (set or modify) the recommended grayscale display value
   *  which should be set separately.
   *  @param r the red component of the recommended display value in P-values 0..0xffff.
   *  @param g the green component of the recommended display value in P-values 0..0xffff.
   *  @param b the blue component of the recommended display value in P-values 0..0xffff.
   */
  void setGLRecommendedDisplayValueRGB(Uint16 r, Uint16 g, Uint16 b);

  /** set graphic layer description of this layer.
   *  @param glDescription a pointer to the graphic layer description, which is copied into this object.
   */
  void setGLDescription(const char *glDescription);

private:

  /// private undefined assignment operator
  DVPSGraphicLayer& operator=(const DVPSGraphicLayer&);

  /// VR=CS, VM=1, Type 1
  DcmCodeString            graphicLayer;
  /// VR=IS, VM=1, Type 1
  DcmIntegerString         graphicLayerOrder;
  /// VR=US, VM=1, Type 3
  DcmUnsignedShort         graphicLayerRecommendedDisplayGrayscaleValue;
  /// VR=US, VM=3, Type 3
  DcmUnsignedShort         graphicLayerRecommendedDisplayRGBValue;
  /// VR=LO, VM=1, Type 3
  DcmLongString            graphicLayerDescription;

};

#endif
