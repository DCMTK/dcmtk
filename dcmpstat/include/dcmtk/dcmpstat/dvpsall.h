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
 *    classes: DVPSOverlayCurveActivationLayer
 *
 */

#ifndef DVPSALL_H
#define DVPSALL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmpstat/dvpstyp.h"     /* for enum types */
#include "dcmtk/dcmpstat/dpdefine.h"

class DVPSGraphicLayer_PList;
class DVPSOverlay_PList;
class DVPSOverlayCurveActivationLayer;

/** the list of curve and overlay activation layers contained in a presentation state (internal use only).
 *  This class manages the data structures comprising the list of curve
 *  activation layers and overlay activation layers
 *  (all instances of the Curve Activation Layer Module and 
 *  Overlay Activation Layer Module repeating elements)
 *  contained in a Presentation State object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSOverlayCurveActivationLayer_PList
{
public:
  /// default constructor
  DVPSOverlayCurveActivationLayer_PList();
  
  /// copy constructor
  DVPSOverlayCurveActivationLayer_PList(const DVPSOverlayCurveActivationLayer_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSOverlayCurveActivationLayer_PList object containing
   *  a deep copy of this object.
   */
  DVPSOverlayCurveActivationLayer_PList *clone() { return new DVPSOverlayCurveActivationLayer_PList(*this); }

  /// destructor
  virtual ~DVPSOverlayCurveActivationLayer_PList();

  /** reads the curve and overlay activations from a DICOM dataset.
   *  The DICOM elements of the activations are copied from the dataset to this object.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset from which the activations are to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);
  
  /** writes the curve and overlay activations managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the activations are written
   *  @return EC_Normal if successful, an error code otherwise.
   */
   OFCondition write(DcmItem &dset);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();
  
  /** create graphic layers and activations for DICOM image.
   *  This method is used when a default presentation state
   *  for a DICOM image is created. Depending on the given flags,
   *  graphic layer and curve/overlay activations for the curves
   *  and overlays present in the DICOM dataset are created.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset containing the image IOD
   *  @param gLayerList the list of graphic layers to be created
   *  @param overlayList the list of overlays internal to the presentation state
   *  @param overlayActivation flag defining how overlays should be handled
   *  @param curveActivation flag defining how curves should be handled
   *  @param layering flag defining how graphic layers should be created
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition createFromImage(DcmItem &dset,
    DVPSGraphicLayer_PList &gLayerList,
    DVPSOverlay_PList &overlayList,
    DVPSoverlayActivation overlayActivation,
    OFBool                curveActivation,
    DVPSGraphicLayering   layering);
    
  /** set activation layer for given repeating group.
   *  The activation is created if necessary and the layer name
   *  is assigned. This method check if a valid repeating group
   *  number is passed and returns an error code otherwise.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setActivation(Uint16 group, const char *layer);

  /** remove activation for given repeating group.
   */
  void removeActivation(Uint16 group);

  /** get activation layer name of the given repeating group.
   *  @return a pointer to the activation layer name if found, NULL otherwise.
   */
  const char *getActivationLayer(Uint16 group);
  
  /** renames the activation layer name in all activations
   *  with a matching old activation layer name.
   *  Required to keep the presentation consistent when a
   *  graphic layer is renamed.
   *  @param oldName the old activation layer name
   *  @param newName the new activation layer name
   */
  void renameLayer(const char *oldName, const char *newName);

  /** deletes all activation belonging to the given activation
   *  layer name.
   *  @param name name of the deleted activation layer
   */
  void removeLayer(const char *name);

  /** checks if the given layer name is used for any of the
   *  activation layers managed by this object.
   *  @param name name of the layer
   *  @return OFTrue if name is used
   */
  OFBool usesLayerName(const char *name);

  /** returns the number of activations for the given
   *  graphic layer and object type (curve or overlay).
   *  This method does not check whether an image object is attached to the
   *  presentation state and if all activations really have a matching 
   *  curve structure in the attached image.
   *  @param layer name of the graphic layer
   *  @param isCurve if OFTrue, curves are counted, otherwise overlays are counted.
   *  @return number of curves or overlays on the given layer.
   */   
  size_t getNumberOfActivations(const char *layer, OFBool isCurve);

  /** gets the repeating group of the overlay/curve activation with the given index
   *  on the given layer. If the activation or the graphic layer does
   *  not exist, 0 is returned.
   *  @param layer name of the graphic layer
   *  @param idx index of the object, must be < getNumberOfActivations(layer, isCurve)
   *  @param isCurve if OFTrue, curves are searched, otherwise overlays are searched.
   *  @return the repeating group number of the activation
   */   
  Uint16 getActivationGroup(const char *layer, size_t idx, OFBool isCurve);

private:

  /// private undefined assignment operator
  DVPSOverlayCurveActivationLayer_PList& operator=(const DVPSOverlayCurveActivationLayer_PList&);

  /** the list maintained by this object
   */
  OFList<DVPSOverlayCurveActivationLayer *> list_;

};

#endif
