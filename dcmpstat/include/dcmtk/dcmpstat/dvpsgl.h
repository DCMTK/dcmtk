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
 *    classes: DVPSGraphicLayer
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:36 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DVPSGL_H
#define DVPSGL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctk.h"


/** an item of the graphic layer sequence in a presentation state (internal use only).
 *  This class manages the data structures comprising one item
 *  of the Graphic Layer Sequence in a Presentation State object.
 */

class DVPSGraphicLayer
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

/*
 *  $Log: dvpsgl.h,v $
 *  Revision 1.11  2010-10-14 13:16:36  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.10  2010-10-07 14:31:35  joergr
 *  Removed leading underscore characters from preprocessor symbols (reserved).
 *
 *  Revision 1.9  2009-11-24 14:12:57  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.8  2005-12-08 16:03:45  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.7  2001/09/26 15:36:11  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.6  2001/06/01 15:50:16  meichel
 *  Updated copyright header
 *
 *  Revision 1.5  2000/06/02 16:00:46  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.4  2000/03/08 16:28:51  meichel
 *  Updated copyright header.
 *
 *  Revision 1.3  1999/07/22 16:39:08  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.2  1998/12/14 16:10:29  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:27  meichel
 *  Initial Release.
 *
 *
 */
