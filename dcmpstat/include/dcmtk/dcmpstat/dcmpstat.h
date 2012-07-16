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
 *    classes: DcmPresentationState
 *
 */

#ifndef DCMPSTAT_H
#define DCMPSTAT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctk.h"

#include "dcmtk/ofstd/ofstring.h"     /* for class OFString */
#include "dcmtk/dcmpstat/dvpstyp.h"   /* for enum types */
#include "dcmtk/dcmpstat/dvpsovl.h"   /* for DVPSOverlay_PList */
#include "dcmtk/dcmpstat/dvpsgll.h"   /* for DVPSGraphicLayer_PList */
#include "dcmtk/dcmpstat/dvpsrsl.h"   /* for DVPSReferencedSeries_PList */
#include "dcmtk/dcmpstat/dvpsall.h"   /* for DVPSOverlayCurveActivationLayer_PList */
#include "dcmtk/dcmpstat/dvpsgal.h"   /* for DVPSGraphicObject_PList */
#include "dcmtk/dcmpstat/dvpscul.h"   /* for DVPSCurve_PList */
#include "dcmtk/dcmpstat/dvpsvll.h"   /* for DVPSVOILUT_PList */
#include "dcmtk/dcmpstat/dvpsvwl.h"   /* for DVPSVOIWindow_PList */
#include "dcmtk/dcmpstat/dvpsdal.h"   /* for DVPSDisplayedArea_PList */
#include "dcmtk/dcmpstat/dvpssvl.h"   /* for DVPSSoftcopyVOI_PList */
#include "dcmtk/dcmpstat/dvpspl.h"    /* for DVPSPresentationLUT */

class DVPSTextObject;
class DVPSGraphicObject;
class DVPSCurve;
class DVPSDisplayedArea;
class DVPSSoftcopyVOI;

/** a Grayscale Softcopy Presentation State.
 *  This class manages the data structures comprising a Presentation State object.
 *  Presentation states can be created, read, written, and modified.
 */

class DCMTK_DCMPSTAT_EXPORT DcmPresentationState
{
public:
  /** default constructor
   */
  DcmPresentationState();

  /// destructor
  virtual ~DcmPresentationState();

  /** reads a presentation state from a DICOM dataset.
   *  The DICOM elements of the presentation state are copied
   *  from the dataset to this object.
   *  The completeness and correctness of the
   *  presentation state (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the dataset from which the presentation state is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);

  /** writes the presentation state managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the presentation state is written
   *  @param replaceSOPInstanceUID flag indicating whether the
   *    SOP Instance UID should be replaced by a new UID.
   *    If true, a new UID is always generated. If false, a new
   *    UID is generated only if no UID has been assigned before.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset, OFBool replaceSOPInstanceUID);

  /** generates a new SOP Instance UID for the Presentation State.
   *  @return new SOP Instance UID if successfully set, NULL otherwise.
   */
  const char *createInstanceUID();

  /** returns the current SOP Instance UID for the Presentation State.
   *  @return SOP Instance UID if present, NULL otherwise.
   */
  const char *getInstanceUID();

  /** returns the (currently hard-coded) SOP Class UID of the Presentation State.
   *  @return SOP Class UID of the presentation state
   */
  const char *getSOPClassUID();

  /** returns the patient ID of the presentation state
   */
  const char *getPatientID();

  /** returns the Study Instance UID of the presentation state.
   *  @return Study Instance UID if successful, NULL otherwise.
   */
  const char *getStudyUID();

  /** adds a reference to an image to this presentation state.
   *  This method checks if the given SOP class and Study UID match
   *  for this presentation state and returns an error code otherwise.
   *  @param studyUID the Study Instance UID of the image reference to be added.
   *  @param seriesUID the Series Instance UID of the image reference to be added.
   *  @param sopclassUID the SOP class UID of the image reference to be added.
   *  @param instanceUID the SOP instance UID of the image reference to be added.
   *  @param frames a list of frame numbers in DICOM IS format
   *    (integer numbers separated by '\' characters). Default: frame numbers absent.
   *    The frame numbers are required if the referenced image is a multiframe image.
   *  @param aetitle the series retrieveAETitle. Must be a valid DICOM 'AE' value. Default: value absent.
   *  @param filesetID the series storageMediaFileSetID. Must be a valid DICOM 'SH' value. Default: value absent.
   *  @param filesetUID the series storageMediaFileSetUID. Must be a valid DICOM UID. Default: value absent.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addImageReference(
    const char *studyUID,
    const char *seriesUID,
    const char *sopclassUID,
    const char *instanceUID,
    const char *frames=NULL,
    const char *aetitle=NULL,
    const char *filesetID=NULL,
    const char *filesetUID=NULL);

  /** adds a reference to an image to this presentation state.
   *  This method checks if the given SOP class and Study UID match
   *  for this presentation state and returns an error code otherwise.
   *  @param dset the DICOM dataset containing the image IOD
   *  @param aetitle the series retrieveAETitle. Must be a valid DICOM 'AE' value. Default: value absent.
   *  @param filesetID the series storageMediaFileSetID. Must be a valid DICOM 'SH' value. Default: value absent.
   *  @param filesetUID the series storageMediaFileSetUID. Must be a valid DICOM UID. Default: value absent.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addImageReference(
    DcmItem &dset,
    const char *aetitle=NULL,
    const char *filesetID=NULL,
    const char *filesetUID=NULL);

  /** removes a reference to an image from this presentation state.
   *  @param studyUID the Study Instance UID of the image reference to be removed.
   *  @param seriesUID the Series Instance UID of the image reference to be removed.
   *  @param instanceUID the SOP instance UID of the image reference to be removed.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition removeImageReference(
    const char *studyUID,
    const char *seriesUID,
    const char *instanceUID);

  /** removes a reference to an image from this presentation state.
   *  @param dset the DICOM dataset containing the image IOD
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition removeImageReference(DcmItem &dset);

  /** gets the number of image references in all series managed by this list.
   *  @return number of image references
   */
  size_t numberOfImageReferences();

  /** gets an image reference with the given index.
   *  @param idx index, must be < numberOfImageReferences().
   *  @param studyUID the Study Instance UID is returned in this string
   *  @param seriesUID the Series Instance UID is returned in this string
   *  @param sopclassUID the SOP Class UID is returned in this string
   *  @param instanceUID the SOP Instance UID is returned in this string
   *  @param frames the list of frames is returned in this string
   *  @param aetitle the series retrieveAETitle is returned in this string
   *  @param filesetID the series storageMediaFileSetID is returned in this string
   *  @param filesetUID the series storageMediaFileSetUID is returned in this string
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition getImageReference(
    size_t idx,
    OFString& studyUID,
    OFString& seriesUID,
    OFString& sopclassUID,
    OFString& instanceUID,
    OFString& frames,
    OFString& aetitle,
    OFString& filesetID,
    OFString& filesetUID);


  /** resets the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** creates a default presentation state for a DICOM image.
   *  A number of flags specify how curves, overlays, VOI transformations,
   *  a display shutter and a presentation LUT shape
   *  should be treated when found in the image IOD.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset containing the image IOD
   *  @param overlayActivation flag defining how overlays should be handled
   *    (copied, activated or ignored). Default: Copy overlays.
   *  @param voiActivation flag defining how VOI LUTs or VOI window width/center should
   *    be handled. Default: Use VOI and prefer VOI LUT from VOI window.
   *  @param curveActivation flag defining whether curves in the image should
   *    be activated. Default: Activate curves.
   *  @param shutterActivation flag defining whether a shutter (not bitmap shutter)
   *    should be copied to the presentation state when found in the image.
   *    Default: Copy shutter.
   *  @param presentationActivation flag defining whether a presentation LUT shape
   *    should be copied to the presentation state when found in the image.
   *    Default: Copy presentation LUT shape.
   *  @param layering flag defining how graphic layers should be created for
   *    activated overlays and curves. Default: Create one layer for all overlays
   *    and another layer for all curves.
   *  @param aetitle the series retrieveAETitle. Must be a valid DICOM 'AE' value. Default: value absent.
   *  @param filesetID the series storageMediaFileSetID. Must be a valid DICOM 'SH' value. Default: value absent.
   *  @param filesetUID the series storageMediaFileSetUID. Must be a valid DICOM UID. Default: value absent.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition createFromImage(DcmItem &dset,
    DVPSoverlayActivation overlayActivation      = DVPSO_copyOverlays,
    DVPSVOIActivation     voiActivation          = DVPSV_preferVOILUT,
    OFBool                curveActivation        = OFTrue,
    OFBool                shutterActivation      = OFTrue,
    OFBool                presentationActivation = OFTrue,
    DVPSGraphicLayering   layering               = DVPSG_twoLayers,
    const char *          aetitle                = NULL,
    const char *          filesetID              = NULL,
    const char *          filesetUID             = NULL);


  /* Presentation LUT Interface */

  /** gets the current Presentation LUT type.
   *  @return the current presentation LUT type
   */
  DVPSPresentationLUTType getPresentationLUT() { return presentationLUT.getType(); }

  /** checks if a real Presentation LUT (not shape)
   *  is available in the presentation state.
   *  @return OFTrue if the presentation state contains
   *    a presentation LUT, no matter if it is activated or not.
   *    Returns OFFalse otherwise.
   */
  OFBool havePresentationLookupTable() { return presentationLUT.haveTable(); }

  /** gets a description of the current presentation LUT.
   *  For well-known presentation LUT shapes, a standard text
   *  is returned. For presentation LUTs, the LUT explanation
   *  is returned if it exists and a standard text otherwise.
   *  This method never returns NULL.
   *  @return a pointer to a string describing the current presentation LUT.
   */
  const char *getCurrentPresentationLUTExplanation() { return presentationLUT.getCurrentExplanation(); }

  /** returns the LUT explanation of the presentation LUT
   *  if it exists and is non-empty.
   *  Otherwise returns NULL.
   *  @return a string pointer
   */
  const char *getPresentationLUTExplanation() { return presentationLUT.getLUTExplanation(); }

  /** gets the current Presentation LUT object.
   *  @return the current presentation LUT object
   */
  DVPSPresentationLUT *getPresentationLUTData() { return &presentationLUT; }

  /* Rotate/Flip Interface */

  /** gets the current rotation status of the presentation state.
   *  @return the current rotation status
   */
  DVPSRotationType getRotation();

  /** gets the current horizontal flip status of the presentation state.
   *  @return OFTrue if flip is on, OFFalse if flip is off.
   */
  OFBool getFlip();

  /** sets rotation status of the presentation state.
   *  @param rotation the rotation to be set
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setRotation(DVPSRotationType rotation);

  /** sets horizontal flip status of the presentation state.
   *  @param isFlipped the flip status, OFTrue for on, OFFalse for off.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setFlip(OFBool isFlipped);


  /* VOI Transform Interface */

  /* Displayed Area Interface */


  /* shutter Interface */

  /** checks if a display shutter of given type is active.
   *  @param type the shutter type
   *  @return OFTrue if this type of shutter is currently active.
   */
  OFBool haveShutter(DVPSShutterType type);

  /* rectangular shutter Interface */

  /** gets rectangular shutter left vertical edge.
   *  May only be called if a rectangular shutter is active.
   *  @return the rect shutter LV edge.
   */
  Sint32 getRectShutterLV();

  /** gets rectangular shutter right vertical edge.
   *  May only be called if a rectangular shutter is active.
   *  @return the rect shutter RV edge.
   */
  Sint32 getRectShutterRV();

  /** gets rectangular shutter upper horitontal edge.
   *  May only be called if a rectangular shutter is active.
   *  @return the rect shutter UH edge.
   */
  Sint32 getRectShutterUH();

  /** gets rectangular shutter lower horiztonal edge.
   *  May only be called if a rectangular shutter is active.
   *  @return the rect shutter LH edge.
   */
  Sint32 getRectShutterLH();


  /* circular shutter Interface */

  /** gets circular shutter center x component.
   *  May only be called if a circular shutter is active.
   *  @return the circ shutter center x component
   */
  Sint32 getCenterOfCircularShutter_x();

  /** gets circular shutter center y component.
   *  May only be called if a circular shutter is active.
   *  @return the circ shutter center y component
   */
  Sint32 getCenterOfCircularShutter_y();

  /** gets circular shutter radius.
   *  May only be called if a circular shutter is active.
   *  Note: In DICOM, a circular shutter must be rendered
   *  with consideration of the image pixel aspect ratio.
   *  The radius returned by this method is the number
   *  of pixels describing a horizontal line from the
   *  center of the circle to its border. See sample figures
   *  in NEMA PS3.3:1998.
   *  @return the circ shutter radius
   */
  Sint32 getRadiusOfCircularShutter();

  /* polygonal shutter Interface */

  /** gets polygonal shutter number of points.
   *  May only be called if a polygonal shutter is active.
   *  @return the number of points describing the poly shutter
   */
  size_t getNumberOfPolyShutterVertices();

  /** get polygonal shutter point.
   *  May only be called if a polygonal shutter is active.
   *  Shutter points are relative to the origin 1\1 which is
   *  the left upper edge of the image.
   *  @param idx the index of the shutter point, must be < getNumberOfPolyShutterVertices()
   *  @param x returns the x component of the point
   *  @param y returns the y component of the point
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition getPolyShutterVertex(size_t idx, Sint32& x, Sint32& y);

  /** sets polygonal display shutter origin.
   *  This method creates a
   *  polygonal shutter consisting only of a single point.
   *  The polygonal display shutter is deactivated after this method.
   *  @param x the x component of the shutter origin
   *  @param y the x component of the shutter origin
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition setPolyShutterOrigin(Sint32 x, Sint32 y);

  /* bitmap shutter Interface
   *
   * see methods:
   *    overlayIsBitmapShutter(),
   *    overlayIsSuitableAsBitmapShutter(),
   *    activateOverlayAsBitmapShutter()
   * in overlay interface definitions.
   */

  /* shutter presentation value Interface */

  /** gets the shutter presentation value. If no shutter display
   *  value exists, a default of 0 (black) is set.
   *  @return the shutter presentation value as 16bit unsigned P-value
   */
  Uint16 getShutterPresentationValue();

  /** sets the shutter presentation value to the given P-value.
   *  @param pvalue the shutter presentation value.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition setShutterPresentationValue(Uint16 pvalue);

  /* Presentation State Label, Description and Name Interface */

  /** returns a label for the presentation state.
   *  If no label is available, NULL is returned.
   *  @return a pointer to a string or NULL.
   */
  const char *getPresentationLabel();

  /** returns a description for the presentation state.
   *  If no description is available, NULL is returned.
   *  @return a pointer to a string or NULL.
   */
  const char *getPresentationDescription();

  /** returns the creator's name for the presentation state.
   *  If no name is available, NULL is returned.
   *  @return a pointer to a string or NULL.
   */
  const char *getPresentationCreatorsName();

  /** sets the presentation state label.
   *  The passed string must be a valid DICOM Code String
   *  (i.e. max 16 characters, only uppercase and numbers).
   *  @param label the new presentation state label
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition setPresentationLabel(const char *label);

  /** sets the presentation state description.
   *  The passed string must be a valid DICOM Long String
   *  (i.e. max 64 characters, no backslash or control chars).
   *  @param descr the new presentation state description
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition setPresentationDescription(const char *descr);

  /** sets the presentation state creator's name.
   *  The passed string must be a valid DICOM Person Name String
   *  (see NEMA PS3.5:1998).
   *  @param name the new creator's name
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition setPresentationCreatorsName(const char *name);

  /* specific character set */

  /** sets the specific character set for this presentation state.
   *  @param charset the new character set for this text object
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setCharset(DVPScharacterSet charset);

  /** gets the specific character set for this presentation state.
   *  @return character set identifier
   */
  DVPScharacterSet getCharset();

  /** gets the specific character set string for this presentation state.
   *  @return character set if present, NULL otherwise
   */
  const char *getCharsetString();

  /* graphic layers */

  /** sorts the graphic layers according to
   *  the graphic layer order. Layers with lower order have lower
   *  indices after sorting which means that the layers can be
   *  drawn to the screen in ascending index order.
   *  Calling this routine may result in a re-numbering
   *  of the graphic layer orders in a way that does not affect
   *  their sequence.
   */
  void sortGraphicLayers();

  /** returns the number of graphic layers.
   *  @return number of graphic layers
   */
  size_t getNumberOfGraphicLayers();

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
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param rgb if true, the RGB recommended display value is removed
   *  @param monochrome if true the monochrome recommended display value is removed
   */
  void removeGraphicLayerRecommendedDisplayValue(size_t idx, OFBool rgb, OFBool monochrome);

  /** assigns a new unique name to the given graphic layer.
   *  The new name must be unique, otherwise an error code is returned.
   *  Upon success, all references (for graphic annotations, curves and overlays) to the given
   *  graphic layer are also renamed so that the presentation state remains
   *  consistent.
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


  /* text objects */

  /* graphic objects */

  /* curves */

  /** returns the number of curve activations for the given
   *  graphic layer.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return number of curves
   */
  size_t getNumberOfCurves(size_t layer);

  /** deletes the curve activation with the given index
   *  on the given layer.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the curve activation, must be < getNumberOfCurves(layer)
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition removeCurve(size_t layer, size_t idx);

  /** moves the curve activation with the given index on the given
   *  layer to a different layer.
   *  @param old_layer index of the graphic layer on which the curve is,
   *    must be < getNumberOfGraphicLayers()
   *  @param idx index of the curve activation, must be < getNumberOfCurves(layer)
   *  @param new_layer index of the graphic layer to which the curve is moved,
   *    must be < getNumberOfGraphicLayers()
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition moveCurve(size_t old_layer, size_t idx, size_t new_layer);


  /* overlays */

  /** gets the number of overlays that are currently activated
   *  on the given graphic layer.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return number of active overlays
   */
  size_t getNumberOfActiveOverlays(size_t layer);

  /** gets the repeating group number of the given activated overlay.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the overlay, must be < getNumberOfActiveOverlays().
   *  @return repeating group number if found, 0 otherwise.
   */
  Uint16 getActiveOverlayGroup(size_t layer, size_t idx);

  /** gets the number of overlays which are embedded in the
   *  presentation state.
   *  @return number of overlays in presentation state
   */
  size_t getNumberOfOverlaysInPresentationState();

  /** gets the repeating group number of the given overlay in the presentation state.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInPresentationState().
   *  @return repeating group number if found, 0 otherwise.
   */
  Uint16 getOverlayInPresentationStateGroup(size_t idx);

  /** gets the overlay label of the given overlay in the presentation state.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInPresentationState().
   *  @return label string if it exists, NULL otherwise.
   */
  const char *getOverlayInPresentationStateLabel(size_t idx);

  /** gets the overlay description of the given overlay in the presentation state.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInPresentationState().
   *  @return description string if it exists, NULL otherwise.
   */
  const char *getOverlayInPresentationStateDescription(size_t idx);

  /** gets the index of the activation layer on which the given
   *  overlay from the presentation state is activated. If an overlay is used
   *  as a bitmap shutter, it is reported as being not activated by this method.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInPresentationState().
   *  @return layer index (which is < getNumberOfGraphicLayers()) if overlay exists
   *     and is activated, DVPS_IDX_NONE otherwise.
   */
  size_t getOverlayInPresentationStateActivationLayer(size_t idx);

  /** checks if the given overlay in the presentation state
   *  is currently activated as a bitmap shutter.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInPresentationState().
   *  @return OFTrue if overlay exists and is activated as bitmap shutter, OFFalse otherwise.
   */
  OFBool overlayIsBitmapShutter(size_t idx);

  /** checks whether the given overlay in the presentation state is a ROI
   *  (region of interest) overlay.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInPresentationState().
   *  @return OFTrue if overlay exists and is ROI, OFFalse otherwise.
   */
  OFBool overlayInPresentationStateIsROI(size_t idx);

  /** moves the overlay activation with the given index on the given
   *  layer to a different layer.
   *  @param old_layer index of the graphic layer on which the curve is,
   *    must be < getNumberOfGraphicLayers()
   *  @param idx index of the overlay activation, must be < getNumberOfActiveOverlays(layer)
   *  @param new_layer index of the graphic layer to which the curve is moved,
   *    must be < getNumberOfGraphicLayers()
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition moveOverlay(size_t old_layer, size_t idx, size_t new_layer);

  /* attached image */

   /* Display transform */

  /* print related methods */

private:

  /** private undefined copy constructor
   */
  DcmPresentationState(const DcmPresentationState& copy);

  /** private undefined assignment operator
   */
  DcmPresentationState& operator=(const DcmPresentationState& obj);

  /** create dummy values for all missing type 1 elements.
   *  Called before a presentation state is written to make sure
   *  that the presentation state is complete.
   *  @param replaceSOPInstanceUID flag indicating whether the
   *    SOP Instance UID should be replaced by a new UID.
   *    If true, a new UID is always generated. If false, a new
   *    UID is generated only if no UID has been assigned before.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition createDummyValues(OFBool replaceSOPInstanceUID);

  /** removes and deletes all graphic layer for which
   *  no matching text, graphic, curve or overlay object exists.
   *  Also deletes all graphic annotation sequence items containing
   *  no text and no graphic object. Called before writing a presentation state.
   */
  void cleanupLayers();

protected:

  /** creates a default displayed area selection for the given dataset.
   *  Used in createFromImage().
   *  @param dset the DICOM dataset containing the image IOD
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition createDefaultDisplayedArea(DcmItem &dset);

  /* Module: Patient (M)
   */
  /// Module=Patient, VR=PN, VM=1, Type 1
  DcmPersonName            patientName;
  /// Module=Patient, VR=LO, VM=1, Type 2
  DcmLongString            patientID;
  /// Module=Patient, VR=DA, VM=1, Type 2
  DcmDate                  patientBirthDate;
  /// Module=Patient, VR=CS, VM=1, Type 2
  DcmCodeString            patientSex;

  /* Module: General Study (M)
   */
  /// Module=General_Study, VR=UI, VM=1, Type 1
  DcmUniqueIdentifier      studyInstanceUID;
  /// Module=General_Study, VR=DA, VM=1, Type 2
  DcmDate                  studyDate;
  /// Module=General_Study, VR=TM, VM=1, Type 2
  DcmTime                  studyTime;
  /// Module=General_Study, VR=PN, VM=1, Type 2
  DcmPersonName            referringPhysicianName;
  /// Module=General_Study, VR=SH, VM=1, Type 2
  DcmShortString           studyID;
  /// Module=General_Study, VR=SH, VM=1, Type 2
  DcmShortString           accessionNumber;

  /* Module: General Series (M)
   */
  /// Module=General_Series, VR=UI, VM=1, Type 1
  DcmUniqueIdentifier      seriesInstanceUID;
  /// Module=General_Series, VR=IS, VM=1, Type 2
  DcmIntegerString         seriesNumber;

  /* Module: Presentation Series (M) - specializes general series
   */
  // modality; see General Series

  /* Module: General Equipment (M)
   */
  /// Module=General_Equipment, VR=LO, VM=1, Type 2
  DcmLongString            manufacturer;

  /* Module: Displayed Area (M)
   */
  /// Module=Displayed_Area, VR=SQ, Card=1-n, Type 1
  DVPSDisplayedArea_PList  displayedAreaSelectionList;

  /* Module: Softcopy Presentation LUT (M)
   * There must never be more that one Presentation LUT for one Presentation State,
   * therefore we need not save a list of LUTs.
   */
  /// Module=Softcopy_Presentation_LUT, VR=SQ, Card=1, Type 1C
  DVPSPresentationLUT      presentationLUT;

  /* Module: Presentation State (M)
   * specializes mask and display shutter
   */
  /// Module=Presentation_State, VR=IS, VM=1, Type 1
  DcmIntegerString         imageNumber;
  /// Module=Presentation_State, VR=CS, VM=1, Type 1
  DcmCodeString            presentationLabel;
  /// Module=Presentation_State, VR=LO, VM=1, Type 2
  DcmLongString            presentationDescription;
  /// Module=Presentation_State, VR=DA, VM=1, Type 1
  DcmDate                  presentationCreationDate;
  /// Module=Presentation_State, VR=TM, VM=1, Type 1
  DcmTime                  presentationCreationTime;
  /// Module=Presentation_State, VR=PN, VM=1, Type 2
  DcmPersonName            presentationCreatorsName;
  /// ReferencedSeriesSequence, Module=Presentation_State
  DVPSReferencedSeries_PList referencedSeriesList;
  // shutterPresentationValue; Type 1c. See Display Shutter module

  /* Module: SOP Common (M)
   * we don't store the SOP Class UID because it is well known.
   */
  /// Module=SOP_Common, VR=UI, VM=1, Type 1
  DcmUniqueIdentifier      sOPInstanceUID;
  /// Module=SOP_Common, VR=CS, VM=1-n, Type 1C
  DcmCodeString            specificCharacterSet;
  /// Module=SOP_Common, VR=DA, VM=1, Type 3
  DcmDate                  instanceCreationDate;
  /// Module=SOP_Common, VR=TM, VM=1, Type 3
  DcmTime                  instanceCreationTime;
  /// Module=SOP_Common, VR=UI, VM=1, Type 3
  DcmUniqueIdentifier      instanceCreatorUID;

  /* Module: Display Shutter (C)
   * "required if display shutter to be applied and BitmapDispShutter not present"
   */
  /// if true, a rectangular shutter is in use
  OFBool                   useShutterRectangular;
  /// if true, a circular shutter is in use
  OFBool                   useShutterCircular;
  /// if true, a polygonal shutter is in use
  OFBool                   useShutterPolygonal;
  /// if true, a bitmap shutter is in use
  OFBool                   useShutterBitmap;
  /// Module=Display_Shutter, VM=CS, VR=1-3, Type 1
  DcmCodeString            shutterShape;
  /// Module=Display_Shutter, VR=IS, VM=1, Type 1C
  DcmIntegerString         shutterLeftVerticalEdge;
  /// Module=Display_Shutter, VR=IS, VM=1, Type 1C
  DcmIntegerString         shutterRightVerticalEdge;
  /// Module=Display_Shutter, VR=IS, VM=1, Type 1C
  DcmIntegerString         shutterUpperHorizontalEdge;
  /// Module=Display_Shutter, VR=IS, VM=1, Type 1C
  DcmIntegerString         shutterLowerHorizontalEdge;
  /// Module=Display_Shutter, VR=IS, VM=2, Type 1C
  DcmIntegerString         centerOfCircularShutter;
  /// Module=Display_Shutter, VR=IS, VM=1, Type 1C
  DcmIntegerString         radiusOfCircularShutter;
  /// Module=Display_Shutter, VR=IS, VM=2-2n, Type 1C
  DcmIntegerString         verticesOfThePolygonalShutter;
  /// Module=Display_Shutter, VR=US, VM=1, Type 3 (1c in other modules)
  DcmUnsignedShort         shutterPresentationValue;

  /* Module: Bitmap Display Shutter (C)
   * "required if display shutter to be applied and DispShutter not present"
   */
  /// Module=Bitmap_Display_Shutter, VR=US, VM=1, Type 1
  DcmUnsignedShort         shutterOverlayGroup;
  // shutterPresentationValue already defined in Display Shutter module
  // shutterShape already defined in Display Shutter module

  /* Module: Overlay Plane (C)
   * "required if overlay to be applied or BitmapDispShutter present"
   */
  /// Overlay(s), Module=Overlay_Plane
  DVPSOverlay_PList        overlayList;

  /* Module: Overlay/Curve Activation (C)
   * "required if ref. image contains overlay or curve to be displayed"
   */
  /// Overlay/Curve Activation Layer(s), Module=Overlay_Activation/Curve_Activation
  DVPSOverlayCurveActivationLayer_PList activationLayerList;

  /* Module: Graphic Annotation (C)
   * "required if graphical annotation to be applied"
   */
  /// GraphicAnnotationSequence, Module=Graphic_Annotation
  DVPSGraphicAnnotation_PList graphicAnnotationList;

  /* Module: Spatial Transformation (C)
   * "required if rotation/flipping/magnification to be applied"
   */
  /// Module=Spatial_Transform, VR=US, VM=1, Type 1
  DcmUnsignedShort         imageRotation;
  /// Module=Spatial_Transform, VR=CS, VM=1, Type 1
  DcmCodeString            imageHorizontalFlip;

  /* Module: Graphic Layer (C)
   * "required if graphic annotation, overlays or curves to be applied"
   */
  /// GraphicLayerSequence, Module=Graphic_Layer
  DVPSGraphicLayer_PList graphicLayerList;

  /* Module: Modality LUT (C)
   * "required if modality LUT to be applied"
   * There must never be more that one Modality LUT for one Presentation State,
   * therefore we need not save a list of LUTs.
   */
  /// if true, a modality rescale slope/intercept is set
  OFBool                   useModalityRescale;
  /// if true, a modality LUT is set
  OFBool                   useModalityLUT;
  /// Module=Modality_LUT, VR=xs, VM=3, Type 1c
  DcmUnsignedShort         modalityLUTDescriptor;
  /// Module=Modality_LUT, VR=LO, VM=1, Type 3
  DcmLongString            modalityLUTExplanation;
  /// Module=Modality_LUT, VR=LO, VM=1, Type 3
  DcmLongString            modalityLUTType;
  /// Module=Modality_LUT, VR=xs, VM=1-n, Type 1c
  DcmUnsignedShort         modalityLUTData;
  /// Module=Modality_LUT, VR=DS, VM=1, Type 1c
  DcmDecimalString         rescaleIntercept;
  /// Module=Modality_LUT, VR=DS, VM=1, Type 1c
  DcmDecimalString         rescaleSlope;
  /// Module=Modality_LUT, VR=LO, VM=1, Type 1c
  DcmLongString            rescaleType;

  /* Module: Softcopy VOI LUT (C)
   * "required if VOI LUT to be applied"
   */
  /// Module=Softcopy_VOI_LUT, VR=SQ, Card=1-n, Type 1
  DVPSSoftcopyVOI_PList    softcopyVOIList;

};

#endif
