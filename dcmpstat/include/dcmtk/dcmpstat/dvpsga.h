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
 *    classes: DVPSGraphicAnnotation
 *
 */

#ifndef DVPSGA_H
#define DVPSGA_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrcs.h"       /* for DcmCodeString */
#include "dcmtk/dcmpstat/dvpstxl.h"     /* for DVPSTextObject_PList */
#include "dcmtk/dcmpstat/dvpsgrl.h"     /* for DVPSGraphicObject_PList */
#include "dcmtk/dcmpstat/dvpsril.h"     /* for DVPSReferencedImage_PList */
#include "dcmtk/dcmpstat/dvpstyp.h"     /* for enum types */

/** an item of the graphic annotation sequence in a presentation state (internal use only).
 *  This class manages the data structures comprising one item
 *  of the Graphic Annotation Sequence in a Presentation State object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSGraphicAnnotation
{
public:
  /// default constructor
  DVPSGraphicAnnotation();
  
  /// copy constructor
  DVPSGraphicAnnotation(const DVPSGraphicAnnotation& copy);

  /** clone method.
   *  @return a pointer to a new DVPSGraphicAnnotation object containing
   *  a deep copy of this object.
   */
  DVPSGraphicAnnotation *clone() { return new DVPSGraphicAnnotation(*this); }

  /// destructor
  virtual ~DVPSGraphicAnnotation();

  /** reads a graphic annotation from a DICOM dataset.
   *  The DICOM elements of the Graphic Annotation item are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the item of the GraphicAnnotationSequence from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);

  /** writes the graphic annotation managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the GraphicAnnotationSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

  /** get annotation layer name.
   *  @return a pointer to the annotation layer name
   */
  const char *getAnnotationLayer();

  /** set annotation layer name of this annotation.
   *  @param aLayer a pointer to the annotation layer name, which is copied into this object.
   */
  void setAnnotationLayer(const char *aLayer);

  /** add a new image reference.
   *  Checks if the referenced SOP instance UID already exists in this sequence.
   *  If it exists, an error code is returned. Otherwise a new image reference
   *  is created and added to the ReferencedImageSequence.
   *  @param sopclassUID the SOP class UID of the image reference to be added.
   *  @param instanceUID the SOP instance UID of the image reference to be added.
   *  @param frame the frame number of the image reference (current image) to be added.
   *  @param applicability the applicability of the image reference (DVPSB_currentFrame or DVPSX_currentImage)
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addImageReference(
    const char *sopclassUID,
    const char *instanceUID, 
    unsigned long frame,
    DVPSObjectApplicability applicability);
    
  /** checks if this annotation layer is empty.
   *  An annotation layer is empty when it contains no text object and no graphic object.
   *  @return OFTrue if empty.
   */
  OFBool isEmpty();

  /** checks if this annotation layer is applicable to the given image and frame.
   *  @param instanceUID SOP instance UID of the current image
   *  @param frame number of the current frame
   *  @param applicability the required (minimum) applicability of the reference. Default:
   *    annotation layer applies to the current frame of the current image.
   *  @return OFTrue if applicable.
   */
  OFBool isApplicable(    
    const char *instanceUID, 
    unsigned long frame,
    DVPSObjectApplicability applicability=DVPSB_currentFrame);
    
  /** returns the number of text objects in this annotation.
   *  @return number of text objects
   */
  size_t getNumberOfTextObjects();
  
  /** returns the number of graphic objects in this annotation.
   *  @return number of graphic objects
   */
  size_t getNumberOfGraphicObjects();
  
  /** returns a pointer to the text object with the given
   *  index or NULL if it does not exist.
   *  @param idx index, must be < getNumberOfTextObjects()
   *  @return pointer to text object or NULL
   */
  DVPSTextObject *getTextObject(size_t idx);

  /** returns a pointer to the graphic object with the given
   *  index or NULL if it does not exist.
   *  @param idx index, must be < getNumberOfGraphicObjects()
   *  @return pointer to graphic object or NULL
   */
  DVPSGraphicObject *getGraphicObject(size_t idx);

  /** adds the given text object to
   *  the list of text objects managed by this object.
   *  @param text text object to be inserted.
   */
  void addTextObject(DVPSTextObject *text);

  /** adds the given graphic object to
   *  the list of graphic objects managed by this object.
   *  @param text graphic object to be inserted.
   */
  void addGraphicObject(DVPSGraphicObject *graphic);
  
  /** returns a pointer to the text object with the given
   *  index (or NULL if it does not exist) and removes it from the list.
   *  @param idx index, must be < getNumberOfTextObjects()
   *  @return pointer to text object or NULL
   */
  DVPSTextObject *removeTextObject(size_t idx);

  /** returns a pointer to the graphic object with the given
   *  index (or NULL if it does not exist) and removes it from the list.
   *  @param idx index, must be < getNumberOfGraphicObjects()
   *  @return pointer to graphic object or NULL
   */
  DVPSGraphicObject *removeGraphicObject(size_t idx);

private:

  /// private undefined assignment operator
  DVPSGraphicAnnotation& operator=(const DVPSGraphicAnnotation&);

  /// ReferencedImageSequence, Type 1c
  DVPSReferencedImage_PList referencedImageList;
  /// VR=CS, VM=1, Type 1 
  DcmCodeString             graphicAnnotationLayer;
  /// TextObjectSequence, Type 1c 
  DVPSTextObject_PList      textObjectList;
  /// GraphicObjectSequence, Type 1c   
  DVPSGraphicObject_PList   graphicObjectList;

};

#endif
