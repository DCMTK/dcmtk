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
 *    classes: DVPresentationState
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-12-14 16:10:35 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSTAT_H__
#define __DVPSTAT_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctk.h"

#include "dvpstyp.h"     /* for enum types */
#include "dvpsovl.h"     /* for DVPSOverlay_PList */
#include "dvpsgll.h"     /* for DVPSGraphicLayer_PList */
#include "dvpsrsl.h"     /* for DVPSReferencedSeries_PList */
#include "dvpsall.h"     /* for DVPSOverlayCurveActivationLayer_PList */
#include "dvpsgal.h"     /* for DVPSGraphicObject_PList */

class DVPSTextObject;
class DVPSGraphicObject;
class DVPSCurveObject;
class DicomImage;

/** a Grayscale Softcopy Presentation State.
 *  This class manages the data structures comprising a Presentation State object.
 *  Presentation states can be created, read, written, and modified. 
 */

class DVPresentationState
{
public:
  /// default constructor
  DVPresentationState();
  
  /// copy constructor
  DVPresentationState(const DVPresentationState& copy);

  /** clone method.
   *  @return a pointer to a new DVPresentationState object containing
   *  a deep copy of this object.
   */
  DVPresentationState *clone() { return new DVPresentationState(*this); }  

  /// destructor
  virtual ~DVPresentationState();

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
  E_Condition read(DcmItem &dset);

  /** writes the list of graphic annotations managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the GraphicAnnotationSequence is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);
   	
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
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition addImageReference(
    const char *studyUID, 
    const char *seriesUID, 
    const char *sopclassUID, 
    const char *instanceUID,
    const char *frames=NULL);

  /** adds a reference to an image to this presentation state.
   *  This method checks if the given SOP class and Study UID match
   *  for this presentation state and returns an error code otherwise.
   *  @param dset the DICOM dataset containing the image IOD
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition addImageReference(DcmItem &dset);

  /** adds a reference to the currently attached image to this
   *  presentation state. This method checks if the given image
   *  is not yet referenced and if its Study UID and SOP class
   *  match for this presentation state and returns an error code otherwise.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition addImageReferenceAttached();
  
  /** removes a reference to an image from this presentation state.
   *  @param studyUID the Study Instance UID of the image reference to be removed.
   *  @param seriesUID the Series Instance UID of the image reference to be removed.
   *  @param instanceUID the SOP instance UID of the image reference to be removed.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition removeImageReference(
    const char *studyUID, 
    const char *seriesUID, 
    const char *instanceUID);
     
  /** removes a reference to an image from this presentation state.
   *  @param dset the DICOM dataset containing the image IOD
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition removeImageReference(DcmItem &dset);

  /** removes a reference to the currently attached image from this presentation state.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition removeImageReferenceAttached();
  
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
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition createFromImage(DcmItem &dset,
    DVPSoverlayActivation overlayActivation      = DVPSO_copyOverlays,
    DVPSVOIActivation     voiActivation          = DVPSV_preferVOILUT,
    OFBool                curveActivation        = OFTrue,
    OFBool                shutterActivation      = OFTrue,
    OFBool                presentationActivation = OFTrue,
    DVPSGraphicLayering   layering               = DVPSG_twoLayers);
  
  
  /* Presentation LUT Interface */
  
  /** gets the current Presentation LUT type.
   *  @return the current presentation LUT type
   */
  DVPSPresentationLUTType getPresentationLUT() { return presentationLUT; }
  
  /** checks if a real Presentation LUT (not shape)
   *  is available in the presentation state.
   *  @return OFTrue if the presentation state contains
   *    a presentation LUT, no matter if it is activated or not.
   *    Returns OFFalse otherwise.
   */
  OFBool havePresentationLookupTable();
  
  /** sets the current Presentation LUT type.
   *  DVPSP_table can only be used if the presentation state
   *  contains a lookup table, i.e. if havePresentationLookupTable() returns OFTrue.
   *  @param newType the new presentation LUT type.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setCurrentPresentationLUT(DVPSPresentationLUTType newType);
 
  /** stores a presentation lookup table in the presentation state.
   *  This method stores a presentation lookup table in the
   *  presentation state and activates it. The LUT is copied to
   *  the presentation state. If the method returns EC_Normal,
   *  any old presentation LUT in the presentation state is overwritten.
   *  If the method returns an error code, an old LUT is left unchanged.
   *  @param lutDescriptor the LUT Descriptor in DICOM format (VM=3)
   *  @param lutData the LUT Data in DICOM format
   *  @param lutExplanation the LUT Explanation in DICOM format, may be empty.
   *  @return EC_Normal if successful, an error code otherwise.
   */ 
  E_Condition setPresentationLookupTable(
    DcmUnsignedShort& lutDescriptor,
    DcmUnsignedShort& lutData,
    DcmLongString& lutExplanation);
    
  /** gets a description of the current presentation LUT.
   *  For well-known presentation LUT shapes, a standard text
   *  is returned. For presentation LUTs, the LUT explanation
   *  is returned if it exists and a standard text otherwise.
   *  This method never returns NULL.
   *  @return a pointer to a string describing the current presentation LUT.
   */
  const char *getCurrentPresentationLUTExplanation();

  /** returns the LUT explanation of the presentation LUT
   *  if it exists and is non-empty. 
   *  Otherwise returns NULL.
   *  @return a string pointer
   */
  const char *getPresentationLUTExplanation();
  
  
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
  E_Condition setRotation(DVPSRotationType rotation);

  /** sets horizontal flip status of the presentation state.
   *  @param isFlipped the flip status, OFTrue for on, OFFalse for off.
   *  @return EC_Normal if successful, an error code otherwise.
   */ 
  E_Condition setFlip(OFBool isFlipped);
  
  
  /* VOI Transform Interface */
  
  /** UNIMPLEMENTED gets the number of VOI transformations available.
   *  This number includes all VOI windows and VOI LUTs
   *  embedded in the attached image as well as a possible VOI LUT
   *  that was embedded in the presentation state upon loading
   *  or construction and the last user-selected VOI window.
   */
  size_t getNumberOfVOIs();
  
  /** UNIMPLEMENTED returns the index of the VOI transform that is currently
   *  being applied. A specia values can be returned:
   *  DVPS_IDX_NONE indicates that no VOI transform is currently being applied.
   *  @return index of the current VOI transform or special value.
   */
  size_t getCurrentVOIIndex();

  /** UNIMPLEMENTED returns a description string for a given VOI transform.
   *  If no description for the given index is available,
   *  NULL is returned.
   *  @return a pointer to a string or NULL.
   */
  const char *getVOIDescription(size_t idx);
 
  /** UNIMPLEMENTED returns a description string for a currently active VOI transform.
   *  If no description is available, NULL is returned.
   *  @return a pointer to a string or NULL.
   */
  const char *getCurrentVOIDescription(size_t idx);
 
  /** UNIMPLEMENTED returns the location of a given VOI transform.
   *  The transform can be located in the presentation state
   *  or in the attached image.
   *  @param idx index of the VOI transform
   *  @return location of the VOI transform.
   */
  DVPSSourceType getVOISource(size_t idx);
  
  /** UNIMPLEMENTED gets the width of the current VOI window.
   *  If no VOI transform is active, or if a VOI LUT is active,
   *  the method returns an error code and no window width.
   *  @param w the window width is returned in this parameter
   *  @return EC_Normal upon success, an error code otherwise.
   */  
  E_Condition getCurrentWindowWidth(double &w);
  
  /** UNIMPLEMENTED get the center of the current VOI window.
   *  If no VOI transform is active, or if a VOI LUT is active,
   *  the method returns an error code and no window center.
   *  @param c the window center is returned in this parameter
   *  @return EC_Normal upon success, an error code otherwise.
   */  
  E_Condition getCurrentWindowCenter(double &c);
  
  /** UNIMPLEMENTED activates one of the existing VOI transformations.
   *  @param idx index of the VOI transform, must be < getNumberOfVOIs().
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition setVOI(size_t idx);
 
  /** UNIMPLEMENTED sets a user defined VOI window center and width.
   *  @param wCenter the window center
   *  @param wWidth  the window width
   *  @param description an optional description. Default: absent.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition setVOI(double wCenter, double wWidth, const char *description=NULL);

  /** UNIMPLEMENTED stores a VOI lookup table in the presentation state.
   *  This method stores a VOI lookup table in the
   *  presentation state and activates it. The LUT is copied to
   *  the presentation state. If the method returns EC_Normal,
   *  any old VOI LUT in the presentation state is overwritten.
   *  If the method returns an error code, an old LUT is left unchanged.
   *  @param lutDescriptor the LUT Descriptor in DICOM format (VM=3)
   *  @param lutData the LUT Data in DICOM format
   *  @param lutExplanation the LUT Explanation in DICOM format, may be empty.
   *  @return EC_Normal if successful, an error code otherwise.
   */ 
  E_Condition setVOI( 
    const DcmUnsignedShort& lutDescriptor,
    const DcmUnsignedShort& lutData,
    const DcmLongString& lutExplanation);
 
  /** UNIMPLEMENTED deactivates the current VOI transformation.
   *  After a call to this method, no VOI transform is active.
   */
  void deactivateVOI();
   
   
  /* Displayed Area Interface */
  
  /** returns the X component of the displayed area
   *  top left hand corner.
   *  @return TLHC X component.
   */
  Uint16 getDisplayedAreaTLHC_x();
 
  /** returns the Y component of the displayed area
   *  top left hand corner.
   *  @return TLHC Y component.
   */
  Uint16 getDisplayedAreaTLHC_y();
 
  /** returns the X component of the displayed area
   *  bottom right hand corner.
   *  @return BRHC X component.
   */
  Uint16 getDisplayedAreaBRHC_x();
 
  /** returns the Y component of the displayed area
   *  bottom right hand corner.
   *  @return BRHC Y component.
   */
  Uint16 getDisplayedAreaBRHC_y();
  
  /** sets the displayed area top left hand corner.
   *  @param x TLHC X component
   *  @param y TLHC Y component
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition setDisplayedAreaTLHC(Uint16 x, Uint16 y);

  /** sets the displayed area bottom right hand corner.
   *  @param x BRHC X component
   *  @param y BRHC Y component
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition setDisplayedAreaBRHC(Uint16 x, Uint16 y);
  
  /* shutter Interface */
  
  /** checks if a display shutter of given type is active.
   *  @param type the shutter type
   *  @return OFTrue if this type of shutter is currently active.
   */
  OFBool haveShutter(DVPSShutterType type);
  
  /** deactivates display shutter of given type.
   *  After a call to this method haveShutter(type) will return OFFalse. 
   *  @param type the shutter type
   */
  void removeShutter(DVPSShutterType type);
 
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
  
  /** sets and activates rectangular display shutter.
   *  If a bitmap shutter is exists, it is deactivated if this
   *  method returns successfully. If no shutter display value exists,
   *  a default of 0 (black) is set.
   *  @param lv the left vertical edge
   *  @param rv the right vertical edge
   *  @param uh the upper horizontal edge
   *  @param lh the lower horizontal edge
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition setRectShutter(Sint32 lv, Sint32 rv, Sint32 uh, Sint32 lh);
  
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

  /** sets and activates circular display shutter.
   *  If a bitmap shutter is exists, it is deactivated if this
   *  method returns successfully. If no shutter display value exists,
   *  a default of 0 (black) is set.
   *  @param centerX the X component of the shutter center
   *  @param centerY the Y component of the shutter center
   *  @param radius the (horizontal) radius of the shutter
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition setCircularShutter(Sint32 centerX, Sint32 centerY, Sint32 radius);
  
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
  E_Condition getPolyShutterVertex(size_t idx, Sint32& x, Sint32& y);

  /** sets polygonal display shutter origin.
   *  This method creates a
   *  polygonal shutter consisting only of a single point.
   *  The polygonal display shutter is deactivated after this method.
   *  @param x the x component of the shutter origin
   *  @param y the x component of the shutter origin
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition setPolyShutterOrigin(Sint32 x, Sint32 y);

  /** sets polygonal display shutter point.
   *  This method adds a point to the polygonal display shutter,
   *  which must already have at least an origin.
   *  If the point set with this method is identical to the
   *  origin of the shutter, the shutter is activated and
   *  a possible bitmap shutter is deactivated. If no shutter display value exists,
   *  a default of 0 (black) is set.
   *  @param x the x component of the shutter origin
   *  @param y the x component of the shutter origin
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition addPolyShutterVertex(Sint32 x, Sint32 y);
  
  /* bitmap shutter Interface */ 

  /** gets bitmap display shutter overlay group.
   *  May be used only if bitmap shutter is present.
   *  @return the bitmap shutter repeating group.
   */
  Uint16 getBitmapShutterGroup();
  
  /** sets and activates bitmap display shutter.
   *  The bitmap display shutter is one overlay group
   *  which is embedded in the presentation state,
   *  has the same size as the attached image and which
   *  is not currently activated as an image overlay.
   *  Upon successful return, this method deactivates
   *  any rectangular, circular or polygonal shutter.
   *  If no shutter display value exists,
   *  a default of 0 (black) is set.
   *  @param group the repeating group number of the overlay to be used
   *    as bitmap shutter.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition setBitmapShutterGroup(Uint16 group);


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
  E_Condition setShutterPresentationValue(Uint16 pvalue);
  
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
  E_Condition setPresentationLabel(const char *label);
  
  /** sets the presentation state description.
   *  The passed string must be a valid DICOM Long String
   *  (i.e. max 64 characters, no backslash or control chars).
   *  @param descr the new presentation state description
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition setPresentationDescription(const char *descr);
  
  /** sets the presentation state creator's name.
   *  The passed string must be a valid DICOM Person Name String
   *  (see NEMA PS3.5:1998).
   *  @param name the new creator's name
   *  @return EC_Normal upon success, an error code otherwise.
   */
  E_Condition setPresentationCreatorsName(const char *name);

  /* specific character set */

  /** UNIMPLEMENTED sets the specific character set for this presentation state.
   *  @param charset the new character set for this text object
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setCharset(DVPScharacterSet charset);
  
  /** UNIMPLEMENTED gets the specific character set for this presentation state.
   *  @return character set identifier
   */
  DVPScharacterSet getCharset();

  /** UNIMPLEMENTED gets the specific character set string for this presentation state.
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

  /** checks whether a recommended display value
   *  for the given graphic layer exists.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return OFTrue if a recommended display value exists
   */
  OFBool haveGraphicLayerRecommendedDisplayValue(size_t idx);

  /** checks whether a recommended display value
   *  for the given graphic layer exists and is monochrome.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return OFTrue if a recommended display value exists and is monochrome
   */
  OFBool isGrayGraphicLayerRecommendedDisplayValue(size_t idx);

  /** gets the recommended display value for the
   *  given graphic layer (monochrome). If the recommended display value is a color,
   *  it is implicitly converted to grayscale.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param gray the recommended display value as an unsigned 16-bit P-value
   *    is returned in this parameter.
   *  @return EC_Normal upon success, an error code otherwise
   */
  E_Condition getGraphicLayerRecommendedDisplayValueGray(size_t idx, Uint16& gray);

  /** gets the recommended display value for the
   *  given graphic layer (color). If the recommended display value is monochrome,
   *  identical R, G and B components are passed back.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param r returns the R component of the recommended display value as unsigned 16-bit P-value
   *  @param g returns the G component of the recommended display value as unsigned 16-bit P-value
   *  @param b returns the B component of the recommended display value as unsigned 16-bit P-value
   *  @return EC_Normal upon success, an error code otherwise
   */
  E_Condition getGraphicLayerRecommendedDisplayValueRGB(size_t idx, Uint16& r, Uint16& g, Uint16& b);

  /** sets the recommended display value for the
   *  given graphic layer.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param gray the recommended display value as an unsigned 16-bit P-value
   *  @return EC_Normal upon success, an error code otherwise
   */
  E_Condition setGraphicLayerRecommendedDisplayValueGray(size_t idx, Uint16 gray);
 
  /** sets the recommended display value for the
   *  given graphic layer.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param r the R component of the recommended display value as unsigned 16-bit P-value
   *  @param g the G component of the recommended display value as unsigned 16-bit P-value
   *  @param b the B component of the recommended display value as unsigned 16-bit P-value
   *  @return EC_Normal upon success, an error code otherwise
   */
  E_Condition setGraphicLayerRecommendedDisplayValueRGB(size_t idx, Uint16 r, Uint16 g, Uint16 b);

  /** assigns a new unique name to the given graphic layer.
   *  The new name must be unique, otherwise an error code is returned.
   *  Upon success, all references (for graphic annotations, curves and overlays) to the given
   *  graphic layer are also renamed so that the presentation state remains
   *  consistent.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param name the new name of the graphic layer. Must be a valid DICOM Code String.
   *  @return EC_Normal upon success, an error code otherwise
   */
  E_Condition setGraphicLayerName(size_t idx, const char *name);
  
  /** sets a new description to the given graphic layer.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param descr description of the graphic layer. Must be a valid DICOM Long String.
   *  @return EC_Normal upon success, an error code otherwise
   */
  E_Condition setGraphicLayerDescription(size_t idx, const char *descr);
 
  /** makes a graphic layer the highest layer for display.
   *  This method assigns a graphic layer order higher than all
   *  existing graphic layer orders to the given graphic layer,
   *  sorts and renumbers the list of graphic layers. Upon success,
   *  the given graphic layer is guaranteed to have the new index 
   *  (getNumberOfGraphicLayers()-1).
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return EC_Normal upon success, an error code otherwise
   */
  E_Condition toFrontGraphicLayer(size_t idx);

  /** makes a graphic layer the lowest layer for display.
   *  This method assigns a graphic layer order lower than all
   *  existing graphic layer orders to the given graphic layer,
   *  sorts and renumbers the list of graphic layers. Upon success,
   *  the given graphic layer is guaranteed to have the new index 0.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return EC_Normal upon success, an error code otherwise
   */
  E_Condition toBackGraphicLayer(size_t idx);

  /** creates a new graphic layer with the given
   *  name and optional description.
   *  The new name must be unique, otherwise an error code is returned.
   *  The toFrontGraphicLayer() method is implicitly called for the new layer.
   *  @param gLayer the name of the graphic layer. Must be a valid DICOM Code String.
   *  @param gLayerDescription the optional description of the graphic layer.
   *    Must be a valid DICOM Long String.
   *  @return EC_Normal upon success, an error code otherwise
   */
  E_Condition addGraphicLayer(
     const char *gLayer, 
     const char *gLayerDescription=NULL);
 
  /** removes and deletes a graphic layer. All text, graphic, curve
   *  and overlay objects on this graphic layer are also deleted or deactivated, respectively.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return EC_Normal upon success, an error code otherwise
   */
  E_Condition removeGraphicLayer(size_t idx);
 

  /* text objects */
  
  /** returns the number of text objects for the given
   *  graphic layer. 
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return number of text objects
   */   
  size_t getNumberOfTextObjects(size_t layer);

  /** gets the text object with the given index
   *  on the given layer. If the text object or the graphic layer does
   *  not exist, NULL is returned.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the text object, must be < getNumberOfTextObjects(layer)
   *  @return a pointer to the text object
   */   
  DVPSTextObject *getTextObject(size_t layer, size_t idx);

  /** creates a new text object on the given layer. 
   *  Returns a pointer to the new text object. If the graphic layer
   *  does not exist or if the creation of the text object fails, NULL is returned.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return a pointer to the new text object
   */   
  DVPSTextObject *addTextObject(size_t layer);

  /** deletes the text object with the given index
   *  on the given layer. 
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the text object, must be < getNumberOfTextObjects(layer)
   *  @return EC_Normal upon success, an error code otherwise
   */   
  E_Condition removeTextObject(size_t layer, size_t idx);

  /** moves the text object with the given index on the given
   *  layer to a different layer. 
   *  @param old_layer index of the graphic layer on which the text object is, 
   *    must be < getNumberOfGraphicLayers()
   *  @param idx index of the text object, must be < getNumberOfTextObjects(layer)
   *  @param new_layer index of the graphic layer to which the text object is moved, 
   *    must be < getNumberOfGraphicLayers()
   *  @return EC_Normal upon success, an error code otherwise
   */   
  E_Condition moveTextObject(size_t old_layer, size_t idx, size_t new_layer);
  
  /* graphic objects */

  /** returns the number of graphic objects for the given
   *  graphic layer. 
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return number of graphic objects
   */   
  size_t getNumberOfGraphicObjects(size_t layer);

  /** gets the graphic object with the given index
   *  on the given layer. If the graphic object or the graphic layer does
   *  not exist, NULL is returned.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the graphic object, must be < getNumberOfGraphicObjects(layer)
   *  @return a pointer to the graphic object
   */   
  DVPSGraphicObject *getGraphicObject(size_t layer, size_t idx);

  /** creates a new graphic object on the given layer. 
   *  Returns a pointer to the new graphic object. If the graphic layer
   *  does not exist or if the creation of the graphic object fails, NULL is returned.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return a pointer to the new graphic object
   */   
  DVPSGraphicObject *addGraphicObject(size_t layer);

  /** deletes the graphic object with the given index
   *  on the given layer. 
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the graphic object, must be < getNumberOfGraphicObjects(layer)
   *  @return EC_Normal upon success, an error code otherwise
   */   
  E_Condition removeGraphicObject(size_t layer, size_t idx);
  
  /** moves the graphic object with the given index on the given
   *  layer to a different layer. 
   *  @param old_layer index of the graphic layer on which the graphic object is, 
   *    must be < getNumberOfGraphicLayers()
   *  @param idx index of the graphic object, must be < getNumberOfGraphicObjects(layer)
   *  @param new_layer index of the graphic layer to which the graphic object is moved, 
   *    must be < getNumberOfGraphicLayers()
   *  @return EC_Normal upon success, an error code otherwise
   */   
  E_Condition moveGraphicObject(size_t old_layer, size_t idx, size_t new_layer);
  
  /* curves */

  /** UNIMPLEMENTED returns the number of curve activations for the given
   *  graphic layer. 
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return number of curves
   */   
  size_t getNumberOfCurves(size_t layer);

  /** UNIMPLEMENTED gets the curve with the given index
   *  on the given layer. If the curve or the graphic layer does
   *  not exist, NULL is returned.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the curve, must be < getNumberOfCurves(layer)
   *  @return a pointer to the curve
   */   
  DVPSCurveObject *getCurve(size_t layer, size_t idx);

  /** UNIMPLEMENTED returns the number of curves in the attached image
   *  that could be activated in the presentation state.
   *  @return number of available curves
   */   
  size_t getNumberOfCurvesInImage();
  
  /** UNIMPLEMENTED gets the curve with the given index
   *  from the attached image. If the curve does
   *  not exist, NULL is returned.
   *  @param idx index of the curve, must be < getNumberOfCurvesInImage()
   *  @return a pointer to the curve
   */   
  DVPSCurveObject *getCurveInImage(size_t idx);

  /** UNIMPLEMENTED activates curve in presentation state.
   *  This method adds an activation for the given curve from the
   *  attached image to the given graphic layer in the presentation state.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param curveidxinimage index of the curve in the attached image,
   *    must be < getNumberOfCurvesInImage()
   *  @return EC_Normal upon success, an error code otherwise
   */   
  E_Condition addCurve(size_t layer, size_t curveidxinimage);
 
  /** UNIMPLEMENTED deletes the curve activation with the given index
   *  on the given layer. 
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the curve activation, must be < getNumberOfCurves(layer)
   *  @return EC_Normal upon success, an error code otherwise
   */   
  E_Condition removeCurve(size_t layer, size_t idx);

  /** UNIMPLEMENTED moves the curve activation with the given index on the given
   *  layer to a different layer. 
   *  @param old_layer index of the graphic layer on which the curve is, 
   *    must be < getNumberOfGraphicLayers()
   *  @param idx index of the curve activation, must be < getNumberOfCurves(layer)
   *  @param new_layer index of the graphic layer to which the curve is moved, 
   *    must be < getNumberOfGraphicLayers()
   *  @return EC_Normal upon success, an error code otherwise
   */   
  E_Condition moveCurve(size_t old_layer, size_t idx, size_t new_layer);

   
  /* UNDEFINED and UNIMPLEMENTED: Overlays - the overlay interface still needs to be defined. */
  /* get overlays for layer */
  /* get available overlays */
  /* deactivate overlay for layer */
  /* activate overlay for layer. Make sure that the same overlay is not used as Bitmap Shutter. */
  /* move overlay activation to different layer */
  /* add overlay to pstate */
  /* remove overlay from pstate */  
     
  /** attaches an image to the presentation state.
   *  If an image is already attached to the presentation state,
   *  the old image is detached and freed and the new image is attached.
   *  The presentation state assumes ownership of the passed DcmDataset,
   *  which is freed when a different image is attached or the presentation
   *  state is deleted.
   *  @param dataset pointer to the DICOM image as DcmDataset. 
   *  @return EC_Normal if successful, an error code otherwise. If an error code
   *    is returned, the presentation state has the same state as before the call
   *    to this method.
   */
  E_Condition attachImage(DcmDataset *dataset);
  
  /** attaches an image to the presentation state.
   *  If an image is already attached to the presentation state,
   *  the old image is detached and freed and the new image is attached.
   *  The presentation state assumes ownership of the passed DcmFileFormat,
   *  which is freed when a different image is attached or the presentation
   *  state is deleted.
   *  @param fileformat pointer to the DICOM image as DcmFileFormat. 
   *  @return EC_Normal if successful, an error code otherwise. If an error code
   *    is returned, the presentation state has the same state as before the call
   *    to this method.
   */
  E_Condition attachImage(DcmFileFormat *fileformat);

  /** detaches and frees the image attached to the presentation state.
   */
  void detachImage();
  	
  /** PARTIALLY IMPLEMENTED. apply presentation state to attached image and return image bitmap.
   *  This method sets all parameter required to correctly render the pixel data
   *  in the image attached to the presentation state and then creates the
   *  required pixel data which contains all grayscale transformations but none
   *  of the none-grayscale transformations of the presentation state "burned in"
   *  into the pixel data. The pixel data returned is already corrected by a
   *  Barten transform for the current display device and can be mapped directly
   *  to digital driving levels of the graphics board. The pointer to the pixel
   *  data remains valid until the next call to this function, or until the
   *  image is detached or the presentation state is deleted.
   *  @param pixelData in this parameter a pointer to the pixel data is
   *    returned. The storage area allocated for this pointer must not be
   *    freed by the caller. If the return value is an error code, 
   *    no valid pixel data pointer may be assumed.
   *  @param width returns the width of the bitmap in pixels
   *  @param height returns the height of the bitmap in pixels
   *  @return EC_Normal upon success, an error code otherwise.
   */
   E_Condition getPixelData(
     const void *&pixelData,
     unsigned long &width,
     unsigned long &height);
  

private:
  
  /** private undefined assignment operator
   */
  DVPresentationState& operator=(const DVPresentationState& obj);
  
  /** create dummy values for all missing type 1 elements.
   *  Called before a presentation state is written to make sure
   *  that the presentation state is complete.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition createDummyValues();

  /** removes and deletes all graphic layer for which
   *  no matching text, graphic, curve or overlay object exists.
   *  Also deletes all graphic annotation sequence items containing
   *  no text and no graphic object. Called before writing a presentation state.
   */
  void cleanupLayers();
  
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
  DcmPersonName            referringPhysiciansName;
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
  /// Module=Displayed_Area, VR=US, VM=2, Type 1 
  DcmUnsignedShort         displayedAreaTLHC;
  /// Module=Displayed_Area, VR=US, VM=2, Type 1 
  DcmUnsignedShort         displayedAreaBRHC;

  /* Module: Softcopy Presentation LUT (M)
   * There must never be more that one Presentation LUT for one Presentation State,
   * therefore we need not save a list of LUTs.
   */
  /// describes active type of presentation LUT.
  DVPSPresentationLUTType  presentationLUT;
  /// Module=Softcopy_Presentation_LUT, VR=xs, VM=3, Type 1c 
  DcmUnsignedShort         presentationLUTDescriptor;
  /// Module=Softcopy_Presentation_LUT, VR=LO, VM=1, Type 3 
  DcmLongString            presentationLUTExplanation;
  /// Module=Softcopy_Presentation_LUT, VR=xs, VM=1-n, Type 1c 
  DcmUnsignedShort         presentationLUTData;

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
  /// Module=Spatial_Transform, VR=IS, VM=1, Type 1 
  DcmIntegerString         imageRotation;
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
  
  /* Module: VOI LUT (C)
   * There must never be more that one VOI LUT for one Presentation State,
   * therefore we need not save a list of LUTs.
   */
  /// If true, a VOI window is set
  OFBool                   useVOIWindow;
  /// If true, a VOI LUT is set
  OFBool                   useVOILUT;
  /// Module=VOI_LUT, VR=xs, VM=3, Type 1c 
  DcmUnsignedShort         voiLUTDescriptor;
  /// Module=VOI_LUT, VR=LO, VM=1, Type 3 
  DcmLongString            voiLUTExplanation;
  /// Module=VOI_LUT, VR=xs, VM=1-n, Type 1c 
  DcmUnsignedShort         voiLUTData;
  /// Module=VOI_LUT, VR=DS, VM=1-n, Type 3 
  DcmDecimalString         windowCenter;
  /// Module=VOI_LUT, VR=DS, VM=1-n, Type 1c 
  DcmDecimalString         windowWidth;
  /// Module=VOI_LUT, VR=LO, VM=1-n, Type 3 
  DcmLongString            windowCenterWidthExplanation;

  /* connection with dcmimage */
  
  /** a pointer to the DICOM dataset comprising the image
   *  to which the presentation state is currently applied 
   */
  DcmDataset *currentImageDataset;
  /** a pointer to the fileformat (if it exists) of the
   *  DICOM dataset comprising the image to which the
   *  presentation state is currently applied
   */
  DcmFileFormat *currentImageFileformat;
  /** a pointer to the dcmimage representation of the
   *  image to which the presentation state is currently applied
   */
  DicomImage *currentImage;
  /** a pointer to the dcmimage representation of the
   *  result of the last bitmap output operation
   */
  DicomImage *currentImageOutput;
  
};

#endif

/*
 *  $Log: dvpstat.h,v $
 *  Revision 1.2  1998-12-14 16:10:35  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:34  meichel
 *  Initial Release.
 *
 *
 */

