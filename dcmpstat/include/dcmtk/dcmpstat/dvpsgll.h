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
 *    classes: DVPSGraphicLayer_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:36 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DVPSGLL_H
#define DVPSGLL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcitem.h"

class DVPSGraphicLayer;
class DVPSOverlayCurveActivationLayer_PList;
class DVPSGraphicAnnotation_PList;

/** the list of graphic layers contained in a presentation state (internal use only).
 *  This class manages the data structures comprising the complete
 *  Graphic Layer Sequence in a Presentation State object.
 */

class DVPSGraphicLayer_PList
{
public:
  /// default constructor
  DVPSGraphicLayer_PList();
  
  /// copy constructor
  DVPSGraphicLayer_PList(const DVPSGraphicLayer_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSGraphicLayer_PList object containing
   *  a deep copy of this object.
   */
  DVPSGraphicLayer_PList *clone() { return new DVPSGraphicLayer_PList(*this); }

  /// destructor
  virtual ~DVPSGraphicLayer_PList();

  /** reads the Graphic Layer Sequence from a DICOM dataset.
   *  The completeness of the sequence items (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset from which the overlays are read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);
  
  /** writes the Graphic Layer Sequence managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the sequence is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();
  
  /** adds a new graphic layer to the list. The recommended display value
   *  of the new graphic layer remains undefined and can be set later.
   *  @param gLayer name of the graphic layer. Must be unique within the sequence,
   *   i.e. no other graphic layer with the same name may exist, otherwise an error
   *   code is returned. 
   *  @param gLayerOrder graphic layer order of the new layer.
   *  @param gLayerDescription (optional) description of the new layer. Default: description absent.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addGraphicLayer(const char *gLayer, 
    const Sint32 gLayerOrder,
    const char *gLayerDescription=NULL);

  /** creates a new graphic layer with the given
   *  name and optional description.
   *  The new name must be unique, otherwise an error code is returned.
   *  The toFrontGraphicLayer() method is implicitly called for the new layer.
   *  @param gLayer the name of the graphic layer. Must be a valid DICOM Code String.
   *  @param gLayerDescription the optional description of the graphic layer.
   *    Must be a valid DICOM Long String.
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition addGraphicLayer(
     const char *gLayer, 
     const char *gLayerDescription=NULL);

  /** get number of graphic layer objects in this list.
   *  @return the number of objects.
   */
  size_t size() const { return list_.size(); }  

  /** sorts the graphic layers according to
   *  the graphic layer order. Layers with lower order have lower
   *  indices after sorting which means that the layers can be
   *  drawn to the screen in ascending index order.
   *  Calling this routine may result in a re-numbering
   *  of the graphic layer orders in a way that does not affect
   *  their sequence.
   *  @param lowestLayer is the lowest number assigned to
   *    a layer during the renumbering.
   */
  void sortGraphicLayers(Sint32 lowestLayer=1);

  /** gets the unique name of the graphic
   *  layer with the given index. If no layer for the given
   *  index exists, NULL is returned.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return name of the graphic layer
   */
  const char *getGraphicLayerName(size_t idx);

  /** gets the index of the graphic
   *  layer with the given unique name. If no matching layer
   *  is found, DVPS_IDX_NONE is returned.
   *  @param name name of the graphic layer
   *  @return index of the graphic layer
   */
  size_t getGraphicLayerIndex(const char *name);
  
  /** gets a description string for the graphic
   *  layer with the given index. If no layer for the given
   *  index exists, or if the description is empty, NULL is returned.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return description of the graphic layer
   */
  const char *getGraphicLayerDescription(size_t idx);

  /** checks whether a recommended display value (grayscale, color or both) for the given graphic layer exists.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return OFTrue if a recommended display value exists
   */
  OFBool haveGraphicLayerRecommendedDisplayValue(size_t idx);

  /** gets the recommended grayscale display value for the given graphic layer.
   *  If the graphic layer contains an RGB display value but no grayscale
   *  display value, the RGB value is implicitly converted to grayscale.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param gray the recommended display value as an unsigned 16-bit P-value
   *    is returned in this parameter.
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition getGraphicLayerRecommendedDisplayValueGray(size_t idx, Uint16& gray);

  /** gets the recommended RGB display value for the given graphic layer.
   *  If the graphic layer contains a grayscale display value but no RGB
   *  display value, the grayscale value is implicitly converted to RGB.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param r returns the R component of the recommended display value as unsigned 16-bit P-value
   *  @param g returns the G component of the recommended display value as unsigned 16-bit P-value
   *  @param b returns the B component of the recommended display value as unsigned 16-bit P-value
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition getGraphicLayerRecommendedDisplayValueRGB(size_t idx, Uint16& r, Uint16& g, Uint16& b);

  /** set graphic layer recommended grayscale display value for the given graphic layer.
   *  This method does not affect (set or modify) the recommended RGB display value
   *  which should be set separately.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param gray the recommended display value as an unsigned 16-bit P-value
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition setGraphicLayerRecommendedDisplayValueGray(size_t idx, Uint16 gray);
 
  /** set graphic layer recommended RGB display value for the given graphic layer.
   *  This method does not affect (set or modify) the recommended grayscale display value
   *  which should be set separately.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param r the R component of the recommended display value as unsigned 16-bit P-value
   *  @param g the G component of the recommended display value as unsigned 16-bit P-value
   *  @param b the B component of the recommended display value as unsigned 16-bit P-value
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition setGraphicLayerRecommendedDisplayValueRGB(size_t idx, Uint16 r, Uint16 g, Uint16 b);

  /** removes recommended display values for the given graphic layer.
   *  @param rgb if true, the RGB recommended display value is removed
   *  @param monochrome if true the monochrome recommended display value is removed
   */
  void removeGraphicLayerRecommendedDisplayValue(size_t idx, OFBool rgb, OFBool monochrome);

  /** assigns a new unique name to the given graphic layer.
   *  The new name must be unique, otherwise an error code is returned.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param name the new name of the graphic layer. Must be a valid DICOM Code String.
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition setGraphicLayerName(size_t idx, const char *name);
  
  /** sets a new description to the given graphic layer.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param descr description of the graphic layer. Must be a valid DICOM Long String.
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition setGraphicLayerDescription(size_t idx, const char *descr);
 
  /** makes a graphic layer the highest layer for display.
   *  This method assigns a graphic layer order higher than all
   *  existing graphic layer orders to the given graphic layer,
   *  sorts and renumbers the list of graphic layers. Upon success,
   *  the given graphic layer is guaranteed to have the new index 
   *  (getNumberOfGraphicLayers()-1).
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition toFrontGraphicLayer(size_t idx);

  /** makes a graphic layer the lowest layer for display.
   *  This method assigns a graphic layer order lower than all
   *  existing graphic layer orders to the given graphic layer,
   *  sorts and renumbers the list of graphic layers. Upon success,
   *  the given graphic layer is guaranteed to have the new index 0.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition toBackGraphicLayer(size_t idx);

  /** exchanges the layer order of the two graphic layers with
   *  the given indices. This method does not sort or renumber
   *  the graphic layers.
   *  @param idx1 index of the first graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx2 index of the second graphic layer, must be < getNumberOfGraphicLayers()
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition exchangeGraphicLayers(size_t idx1, size_t idx2);
 
  /** removes and deletes a graphic layer. 
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition removeGraphicLayer(size_t idx);

  /** removes and deletes all graphic layers for which
   *  no matching text, graphic, curve or overlay object exists.
   *  Called before writing a presentation state.
   */
   void cleanupLayers(
     DVPSOverlayCurveActivationLayer_PList& activations, 
     DVPSGraphicAnnotation_PList& annotations);

private:

  /// private undefined assignment operator
  DVPSGraphicLayer_PList& operator=(const DVPSGraphicLayer_PList&);

  /** gets the the graphic layer with the given index. If no layer for the given
   *  index exists, NULL is returned.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return pointer to the graphic layer
   */
  DVPSGraphicLayer *getGraphicLayer(size_t idx);

  /** the list maintained by this object
   */
  OFList<DVPSGraphicLayer *> list_;
};

#endif

/*
 *  $Log: dvpsgll.h,v $
 *  Revision 1.15  2010-10-14 13:16:36  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.14  2010-10-07 14:31:35  joergr
 *  Removed leading underscore characters from preprocessor symbols (reserved).
 *
 *  Revision 1.13  2009-11-24 14:12:57  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.12  2009-09-30 10:42:38  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.11  2005-12-08 16:03:46  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.10  2003/06/04 10:18:06  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.9  2001/09/26 15:36:11  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.8  2001/06/01 15:50:16  meichel
 *  Updated copyright header
 *
 *  Revision 1.7  2000/06/02 16:00:46  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.6  2000/03/08 16:28:52  meichel
 *  Updated copyright header.
 *
 *  Revision 1.5  1999/07/30 13:34:46  meichel
 *  Added new classes managing Stored Print objects
 *
 *  Revision 1.4  1999/07/22 16:39:08  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.3  1999/02/09 15:58:55  meichel
 *  Implemented bitmap shutter activation amd method for
 *    exchanging graphic layers.
 *
 *  Revision 1.2  1998/12/14 16:10:29  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:28  meichel
 *  Initial Release.
 *
 *
 */
