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
 *    classes: DVPSGraphicAnnotation_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-12-14 16:10:28 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSGAL_H__
#define __DVPSGAL_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oflist.h"
#include "dctk.h"

class DVPSGraphicAnnotation;
class DVPSTextObject;
class DVPSGraphicObject;

/** the list of graphic annotations contained in a presentation state (internal use only).
 *  This class manages the data structures comprising the complete
 *  Graphic Annotation Sequence in a Presentation State object.
 */

class DVPSGraphicAnnotation_PList: private OFList<DVPSGraphicAnnotation *>
{
public:
  /// default constructor
  DVPSGraphicAnnotation_PList();
  
  /// copy constructor
  DVPSGraphicAnnotation_PList(const DVPSGraphicAnnotation_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSGraphicAnnotation_PList object containing
   *  a deep copy of this object.
   */
  DVPSGraphicAnnotation_PList *clone() { return new DVPSGraphicAnnotation_PList(*this); }

  /// destructor
  virtual ~DVPSGraphicAnnotation_PList();

  /** reads a list of graphic annotations from a DICOM dataset.
   *  The DICOM elements of the Graphic Annotation Sequence are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the dataset from which the GraphicAnnotationSequence is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);

  /** writes the list of graphic annotations managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the GraphicAnnotationSequence is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);
  
  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** renames the graphic annotation layer name in all activations
   *  with a matching old graphic annotation layer name.
   *  Required to keep the presentation consistent when a
   *  graphic layer is renamed.
   *  @param oldName the old graphic annotation layer name
   *  @param newName the new graphic annotation layer name
   */
  void renameLayer(const char *oldName, const char *newName);

  /** deletes all graphic annotation layers belonging to the given 
   *  graphic annotation layer name.
   *  @param name name of the graphic annotation layers to be deleted
   */
  void removeLayer(const char *name);

  /** deletes all graphic annotation sequence items containing
   *  no text and no graphic object. Called before writing a presentation state.
   */
  void cleanupLayers();
  
  /** checks if the given layer name is used for any of the
   *  graphic annotation layers managed by this object.
   *  @param name name of the layer
   *  @return OFTrue if name is used
   */
  OFBool usesLayerName(const char *name);

  /** returns the number of text objects for the given
   *  graphic layer. 
   *  @param layer name of the graphic layer
   *  @return number of text objects
   */   
  size_t getNumberOfTextObjects(const char *layer);

  /** gets the text object with the given index
   *  on the given layer. If the text object or the graphic layer does
   *  not exist, NULL is returned.
   *  @param layer name of the graphic layer
   *  @param idx index of the text object, must be < getNumberOfTextObjects(layer)
   *  @return a pointer to the text object
   */   
  DVPSTextObject *getTextObject(const char *layer, size_t idx);

  /** creates a new text object on the given layer. 
   *  Returns a pointer to the new text object. If the graphic layer
   *  does not exist or if the creation of the text object fails, NULL is returned.
   *  @param layer name of the graphic layer
   *  @param text the text object to be inserted. If NULL, a new text object
   *    is created. If a pointer to an object is passed in this parameter,
   *    it gets owned by this graphic annotation object and will be deleted
   *    upon destruction of the annotation or if this method fails (returns NULL).
   *  @return a pointer to the new text object
   */   
  DVPSTextObject *addTextObject(const char *layer, DVPSTextObject *text=NULL);

  /** deletes the text object with the given index
   *  on the given layer. 
   *  @param layer name of the graphic layer
   *  @param idx index of the text object, must be < getNumberOfTextObjects(layer)
   *  @return EC_Normal upon success, an error code otherwise
   */   
  E_Condition removeTextObject(const char *layer, size_t idx);

  /** moves the text object with the given index on the given
   *  layer to a different layer. 
   *  @param old_layer name of the graphic layer on which the text object is
   *  @param idx index of the text object, must be < getNumberOfTextObjects(layer)
   *  @param new_layer name of the graphic layer to which the text object is moved
   *  @return EC_Normal upon success, an error code otherwise
   */   
  E_Condition moveTextObject(const char *old_layer, size_t idx, const char *new_layer);

  /** returns the number of graphic objects for the given
   *  graphic layer. 
   *  @param layer name of the graphic layer
   *  @return number of graphic objects
   */   
  size_t getNumberOfGraphicObjects(const char *layer);

  /** gets the graphic object with the given index
   *  on the given layer. If the graphic object or the graphic layer does
   *  not exist, NULL is returned.
   *  @param layer name of the graphic layer
   *  @param idx index of the graphic object, must be < getNumberOfGraphicObjects(layer)
   *  @return a pointer to the graphic object
   */   
  DVPSGraphicObject *getGraphicObject(const char *layer, size_t idx);

  /** creates a new graphic object on the given layer. 
   *  Returns a pointer to the new graphic object. If the graphic layer
   *  does not exist or if the creation of the graphic object fails, NULL is returned.
   *  @param layer name of the graphic layer
   *  @param graphic the graphic object to be inserted. If NULL, a new graphic object
   *    is created. If a pointer to an object is passed in this parameter,
   *    it gets owned by this graphic annotation object and will be deleted
   *    upon destruction of the annotation or if this method fails (returns NULL).
   *  @return a pointer to the new graphic object
   */   
  DVPSGraphicObject *addGraphicObject(const char *layer, DVPSGraphicObject *graphic=NULL);

  /** deletes the graphic object with the given index
   *  on the given layer. 
   *  @param layer name of the graphic layer
   *  @param idx index of the graphic object, must be < getNumberOfGraphicObjects(layer)
   *  @return EC_Normal upon success, an error code otherwise
   */   
  E_Condition removeGraphicObject(const char *layer, size_t idx);

  /** moves the graphic object with the given index on the given
   *  layer to a different layer. 
   *  @param old_layer name of the graphic layer on which the graphic object is
   *  @param idx index of the graphic object, must be < getNumberOfGraphicObjects(layer)
   *  @param new_layer name of the graphic layer to which the graphic object is moved
   *  @return EC_Normal upon success, an error code otherwise
   */   
  E_Condition moveGraphicObject(const char *old_layer, size_t idx, const char *new_layer);

};

#endif

/*
 *  $Log: dvpsgal.h,v $
 *  Revision 1.2  1998-12-14 16:10:28  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:26  meichel
 *  Initial Release.
 *
 *
 */

