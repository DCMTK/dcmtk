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
 *    classes: DVPSGraphicLayer
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-12-14 16:10:29 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSGL_H__
#define __DVPSGL_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctk.h"


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
  E_Condition read(DcmItem &dset);
  
  /** writes the graphic layer managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the GraphicLayerSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);
  
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

  /** checks whether a recommended display value exists.
   *  @return OFTrue if a recommended display value exists
   */
  OFBool haveGLRecommendedDisplayValue();

  /** checks whether a recommended display value
   *  exists and is monochrome.
   *  @return OFTrue if a recommended display value exists and is monochrome
   */
  OFBool isGrayGLRecommendedDisplayValue();

  /** gets the recommended display value (monochrome). 
   *  If the recommended display value is a color,
   *  it is implicitly converted to grayscale.
   *  @param gray the recommended display value as an unsigned 16-bit P-value
   *    is returned in this parameter.
   *  @return EC_Normal upon success, an error code otherwise
   */
  E_Condition getGLRecommendedDisplayValueGray(Uint16& gray);

  /** gets the recommended display value (color). If the recommended display value is monochrome,
   *  identical R, G and B components are passed back.
   *  @param r returns the R component of the recommended display value as unsigned 16-bit P-value
   *  @param g returns the G component of the recommended display value as unsigned 16-bit P-value
   *  @param b returns the B component of the recommended display value as unsigned 16-bit P-value
   *  @return EC_Normal upon success, an error code otherwise
   */
  E_Condition getGLRecommendedDisplayValueRGB(Uint16& r, Uint16& g, Uint16& b);
  
  /** set graphic layer name of this layer.
   *  @param gl a pointer to the graphic layer name, which is copied into this object.
   */
  void setGL(const char *gl);

  /** set graphic layer order of this layer.
   *  @param glOrder the graphic layer order.
   */
  void setGLOrder(Sint32 glOrder);

  /** set graphic layer recommended display value (gray) of this layer.
   *  @param gray the recommended display value in P-values 0..0xffff.
   */
  void setGLRecommendedDisplayValue(Uint16 gray);

  /** set graphic layer recommended display value (color) of this layer.
   *  @param r the red component of the recommended display value in P-values 0..0xffff.
   *  @param g the green component of the recommended display value in P-values 0..0xffff.
   *  @param b the blue component of the recommended display value in P-values 0..0xffff.
   */
  void setGLRecommendedDisplayValue(Uint16 r, Uint16 g, Uint16 b);

  /** set graphic layer description of this layer.
   *  @param glDescription a pointer to the graphic layer description, which is copied into this object.
   */
  void setGLDescription(const char *glDescription);

private:
  /// VR=CS, VM=1, Type 1
  DcmCodeString            graphicLayer;
  /// VR=IS, VM=1, Type 1
  DcmIntegerString         graphicLayerOrder;
  /// VR=US, VM=1-3, Type 3
  DcmUnsignedShort         graphicLayerRecommendedDisplayValue;
  /// VR=LO, VM=1, Type 3
  DcmLongString            graphicLayerDescription;
};

#endif

/*
 *  $Log: dvpsgl.h,v $
 *  Revision 1.2  1998-12-14 16:10:29  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:27  meichel
 *  Initial Release.
 *
 *
 */
