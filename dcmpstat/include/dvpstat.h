/*
 *
 *  Copyright (C) 1998-2002, OFFIS
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-12-09 13:29:49 $
 *  CVS/RCS Revision: $Revision: 1.41 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSTAT_H__
#define __DVPSTAT_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctk.h"

#include "ofstring.h"    /* for class OFString */
#include "dvpstyp.h"     /* for enum types */
#include "dvpsovl.h"     /* for DVPSOverlay_PList */
#include "dvpsgll.h"     /* for DVPSGraphicLayer_PList */
#include "dvpsrsl.h"     /* for DVPSReferencedSeries_PList */
#include "dvpsall.h"     /* for DVPSOverlayCurveActivationLayer_PList */
#include "dvpsgal.h"     /* for DVPSGraphicObject_PList */
#include "dvpscul.h"     /* for DVPSCurve_PList */
#include "dvpsvll.h"     /* for DVPSVOILUT_PList */
#include "dvpsvwl.h"     /* for DVPSVOIWindow_PList */
#include "dvpsdal.h"     /* for DVPSDisplayedArea_PList */
#include "dvpssvl.h"     /* for DVPSSoftcopyVOI_PList */
#include "dvpspl.h"      /* for DVPSPresentationLUT */

class DVPSTextObject;
class DVPSGraphicObject;
class DVPSCurve;
class DicomImage;
class DiDisplayFunction;
class DVPSDisplayedArea;
class DVPSSoftcopyVOI;

/** a Grayscale Softcopy Presentation State.
 *  This class manages the data structures comprising a Presentation State object.
 *  Presentation states can be created, read, written, and modified.
 */

class DVPresentationState
{
public:
  /** default constructor
   *  @param displayFunction list of objects describing the display
   *    characteristics of the monitor. Used to implement the standard display function.
   *    The parameter should be an array with DVPSD_max entries (see DVInterface).
   *    If absent, no display transform is performed.
   *  @param minPrintBitmapX default value for minimum print bitmap size X
   *  @param minPrintBitmapY default value for minimum print bitmap size Y
   *  @param maxPrintBitmapX default value for maximum print bitmap size X
   *  @param maxPrintBitmapY default value for maximum print bitmap size Y
   *  @param maxPreviewImageX default value for maximum preview image size X
   *  @param maxPreviewImageY default value for maximum preview image size Y
   */
  DVPresentationState(
    DiDisplayFunction **dispFunction=NULL,
    unsigned long minPrintBitmapX=0,
    unsigned long minPrintBitmapY=0,
    unsigned long maxPrintBitmapX=0,
    unsigned long maxPrintBitmapY=0,
    unsigned long maxPreviewImageX=0,
    unsigned long maxPreviewImageY=0);

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

  /** adds a reference to the currently attached image to this
   *  presentation state. This method checks if the given image
   *  is not yet referenced and if its Study UID and SOP class
   *  match for this presentation state and returns an error code otherwise.
   *  @param aetitle the series retrieveAETitle. Must be a valid DICOM 'AE' value. Default: value absent.
   *  @param filesetID the series storageMediaFileSetID. Must be a valid DICOM 'SH' value. Default: value absent.
   *  @param filesetUID the series storageMediaFileSetUID. Must be a valid DICOM UID. Default: value absent.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addImageReferenceAttached(
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

  /** removes a reference to the currently attached image from this presentation state.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition removeImageReferenceAttached();

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

  /** sets the current Presentation LUT type.
   *  DVPSP_table can only be used if the presentation state
   *  contains a lookup table, i.e. if havePresentationLookupTable() returns OFTrue.
   *  @param newType the new presentation LUT type.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setCurrentPresentationLUT(DVPSPresentationLUTType newType);

  /** resets the Presentation LUT to the default LUT shape
   *  which is DVPSP_identity for MONOCHROME2 images and DVPSP_inverse for MONOCHROME1.
   *  DVPSP_table can only be used if the presentation state
   *  contains a lookup table, i.e. if havePresentationLookupTable() returns OFTrue.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setDefaultPresentationLUTShape();

  /** stores a presentation lookup table in the presentation state.
   *  This method stores a presentation lookup table in the
   *  presentation state and activates it. The LUT is copied to
   *  the presentation state. If the method returns EC_Normal,
   *  any old presentation LUT in the presentation state is overwritten.
   *  This method keeps the inverse/not inverse status intact,
   *  i.e. inverses the LUT if necessary.
   *  @param lutDescriptor the LUT Descriptor in DICOM format (VM=3)
   *  @param lutData the LUT Data in DICOM format
   *  @param lutExplanation the LUT Explanation in DICOM format, may be empty.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setPresentationLookupTable(
    DcmUnsignedShort& lutDescriptor,
    DcmUnsignedShort& lutData,
    DcmLongString& lutExplanation);

  /** stores a presentation lookup table in the presentation state.
   *  This method stores a presentation lookup table in the
   *  presentation state and activates it. The LUT is copied to
   *  the presentation state. Overwrites old LUT. If unsuccessful,
   *  LUT is set to DVPSP_identity.
   *  This method keeps the inverse/not inverse status intact,
   *  i.e. inverses the LUT if necessary.
   *  @param dset dataset from which the Presentation LUT SQ or Shape is read.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setPresentationLookupTable(DcmItem &dset);

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

  /** writes the current Presentation LUT to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset. In the case of a MONOCHROME1 image an inverted
   *  LUT is written to dataset because the print bitmap is always MONOCHROME2.
   *  @param dset the dataset to which the Presentation LUT SQ/Shape is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition writePresentationLUTforPrint(DcmItem &dset);

  /** gets the current Presentation LUT object.
   *  @return the current presentation LUT object
   */
  DVPSPresentationLUT *getPresentationLUTData() { return &presentationLUT; }

  /** checks whether the attached image is MONOCHROME1. In this case, the
   *  presentation LUT must be reversed when applied to the print bitmap
   *  which is always MONOCHROME2.
   *  @return OFTrue if attached image is MONOCHROME1, OFFalse otherwise.
   */
  OFBool isMonochrome1Image() { return currentImageMonochrome1; }

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

  /** check if a VOI window is currently active
   *  @return OFTrue if a VOI window is active
   */
  OFBool haveActiveVOIWindow();

  /** check if a VOI LUT is currently active
   *  @return OFTrue if a VOI LUT is active
   */
  OFBool haveActiveVOILUT();

  /** returns a description string for a currently active VOI transform.
   *  If no description is available, NULL is returned.
   *  @return a pointer to a string or NULL.
   */
  const char *getCurrentVOIDescription();

  /** gets the width of the current VOI window.
   *  May only be called if haveActiveVOIWindow() is OFTrue.
   *  @param w the window width is returned in this parameter
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition getCurrentWindowWidth(double &w);

  /** get the center of the current VOI window.
   *  May only be called if haveActiveVOIWindow() is OFTrue.
   *  @param c the window center is returned in this parameter
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition getCurrentWindowCenter(double &c);

  /** gets the number of VOI LUTs available in the attached image.
   */
  size_t getNumberOfVOILUTsInImage();

  /** gets the number of VOI Windows available in the attached image.
   */
  size_t getNumberOfVOIWindowsInImage();

  /** returns a description string for the given VOI LUT in the attached
   *  image.
   *  If no description for the given index is available, NULL is returned.
   *  @param idx index, must be < getNumberOfVOILUTsInImage()
   *  @return a pointer to a string or NULL.
   */
  const char *getDescriptionOfVOILUTsInImage(size_t idx);

  /** returns a description string for the given VOI Window
   *  in the attached image.
   *  If no description for the given index is available, NULL is returned.
   *  @param idx index, must be < getNumberOfVOIWindowsInImage()
   *  @return a pointer to a string or NULL.
   */
  const char *getDescriptionOfVOIWindowsInImage(size_t idx);

  /** activates one of the VOI LUTs from the attached image.
   *  The applicability of the activation is controlled by the applicability parameter.
   *  @param idx index of the VOI transform, must be < getNumberOfVOILUTsInImage().
   *  @param applicability defines the applicability of the new VOI transform.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition setVOILUTFromImage(size_t idx,
    DVPSObjectApplicability applicability=DVPSB_currentImage);

  /** activates one of the VOI Windows from the attached image.
   *  The applicability of the activation is controlled by the applicability parameter.
   *  @param idx index of the VOI transform, must be < getNumberOfVOIWindowsInImage().
   *  @param applicability defines the applicability of the new VOI transform.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition setVOIWindowFromImage(size_t idx,
    DVPSObjectApplicability applicability=DVPSB_currentImage);

  /** sets a user defined VOI window center and width.
   *  The applicability of the VOI window is controlled by the applicability parameter.
   *  @param wCenter the window center
   *  @param wWidth  the window width
   *  @param description an optional description. Default: absent.
   *  @param applicability defines the applicability of the new VOI transform.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition setVOIWindow(double wCenter, double wWidth, const char *description=NULL,
    DVPSObjectApplicability applicability=DVPSB_currentImage);

  /** stores a VOI lookup table in the presentation state.
   *  This method stores a VOI lookup table in the
   *  presentation state and activates it. The LUT is copied to
   *  the presentation state.
   *  If the method returns an error code, an old LUT is left unchanged.
   *  The applicability of the VOI LUT is controlled by the applicability parameter.
   *  @param lutDescriptor the LUT Descriptor in DICOM format (VM=3)
   *  @param lutData the LUT Data in DICOM format
   *  @param lutExplanation the LUT Explanation in DICOM format, may be empty.
   *  @param applicability defines the applicability of the new VOI transform.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setVOILUT(
    DcmUnsignedShort& lutDescriptor,
    DcmUnsignedShort& lutData,
    DcmLongString& lutExplanation,
    DVPSObjectApplicability applicability=DVPSB_currentImage);

  /** deactivates the current VOI transformation.
   *  The applicability of the deactivation is controlled by the applicability parameter.
   *  After a call to this method, no VOI transform is active for the current image and frame.
   *  @param applicability defines the applicability of the new VOI transform.
   */
  void deactivateVOI(DVPSObjectApplicability applicability=DVPSB_currentImage);

  /** stores VOI lookup table with a gamma curve shape in the presentation state.
   *  If a VOI window is currently active the center and width values are used to specify
   *  the number of LUT entries and the first value mapped, otherwise the full pixel range
   *  is used. The output range of the LUT is always 16 bit (data is stored as OW).
   *  This method stores a VOI lookup table in the presentation state and activates it.
   *  The LUT is copied to the presentation state.
   *  If the method returns an error code, an old LUT is left unchanged.
   *  The applicability of the VOI LUT is controlled by the applicability parameter.
   *  @param gammaValue gamma value used to create the VOI LUT data
   *  @param applicability defines the applicability of the new VOI transform.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setGammaVOILUT(double gammaValue, DVPSObjectApplicability applicability=DVPSB_currentImage);

  /* Displayed Area Interface */

  /** gets the presentation size mode for the current image and frame.
   *  This method may only be called when an image is attached to the presentation state.
   *  @return presentation size mode
   */
  DVPSPresentationSizeMode getDisplayedAreaPresentationSizeMode();

  /** gets the presentation pixel aspect ratio for for the current image and frame.
   *  Pixel aspect ratio is defined here as the width of a pixel divided
   *  by the height of a pixel (x/y).
   *  This method may only be called when an image is attached to the presentation state.
   *  @return pixel aspect ratio
   */
  double getDisplayedAreaPresentationPixelAspectRatio();

  /** gets the displayed area top lefthand corner and
   *  bottom righthand corner for the current image and frame.
   *  This method may only be called when an image is attached to the presentation state.
   *  @param tlhcX the displayed area top lefthand corner X value is returned in this parameter
   *  @param tlhcY the displayed area top lefthand corner Y value is returned in this parameter
   *  @param brhcX the displayed area bottom righthand corner X value is returned in this parameter
   *  @param brhcY the displayed area bottom righthand corner Y value is returned in this parameter
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition getDisplayedArea(Sint32& tlhcX, Sint32& tlhcY, Sint32& brhcX, Sint32& brhcY);

  /** gets the presentation pixel spacing for the current image and frame if it is known.
   *  @param x the horizontal pixel spacing (mm) is returned in this parameter upon success
   *  @param y the vertical pixel spacing (mm) is returned in this parameter upon success
   *  @return EC_Normal if successful, an error code if no presentation pixel spacing is available.
   */
  OFCondition getDisplayedAreaPresentationPixelSpacing(double& x, double& y);

  /** gets the presentation pixel magnification ratio for the current image and frame if it is present.
   *  @param magnification the magnification ratio is returned in this parameter upon success
   *  @return EC_Normal if successful, an error code if no magnification ratio is available.
   */
  OFCondition getDisplayedAreaPresentationPixelMagnificationRatio(double& magnification);

  /** checks if "TRUE SIZE" can be used as presentation size mode for the current image and frame
   *  (i.e. pixel spacing is known).
   *  @return OFTrue if TRUE SIZE mode is available, OFFalse otherwise.
   */
  OFBool canUseDisplayedAreaTrueSize();

  /** sets the displayed area and size mode (for the current frame, the current image
   *  or all images referenced by the presentation state object).
   *  @param sizeMode presentation size mode.
   *  @param tlhcX displayed area top lefthand corner X
   *  @param tlhcY displayed area top lefthand corner Y
   *  @param brhcX displayed area bottom righthand corner X
   *  @param brhcY displayed area bottom righthand corner Y
   *  @param magnification magnification factor - ignored unless sizeMode==DVPSD_magnify.
   *  @param applicability defines the applicability of the new displayed area definition.
   *    Possible choices are: DVPSB_currentFrame - current frame only,
   *    DVPSB_currentImage - all frames of current image (default),
   *    and DVPSB_allImages -  all images referenced by this presentation state.
   *    The last choice should be used with care
   *    because it will also cause the pixel spacing or pixel aspect ratio of the current image
   *    to be applied to all images referenced by the presentation state.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setDisplayedArea(
    DVPSPresentationSizeMode sizeMode,
    Sint32 tlhcX, Sint32 tlhcY,
    Sint32 brhcX, Sint32 brhcY,
    double magnification=1.0,
    DVPSObjectApplicability applicability=DVPSB_currentImage);


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
  OFCondition setRectShutter(Sint32 lv, Sint32 rv, Sint32 uh, Sint32 lh);

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
  OFCondition setCircularShutter(Sint32 centerX, Sint32 centerY, Sint32 radius);

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
  OFCondition addPolyShutterVertex(Sint32 x, Sint32 y);

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

  /** removes and deletes a graphic layer. All text, graphic, curve
   *  and overlay objects on this graphic layer are also deleted or deactivated, respectively.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition removeGraphicLayer(size_t idx);


  /* text objects */

  /** returns the number of text objects for the given
   *  graphic layer.
   *  Only the objects that are applicable to the current (attached) image
   *  and the selected frame number are used by this method.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return number of text objects
   */
  size_t getNumberOfTextObjects(size_t layer);

  /** gets the text object with the given index
   *  on the given layer. If the text object or the graphic layer does
   *  not exist, NULL is returned.
   *  Only the objects that are applicable to the current (attached) image
   *  and the selected frame number are used by this method.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the text object, must be < getNumberOfTextObjects(layer)
   *  @return a pointer to the text object
   */
  DVPSTextObject *getTextObject(size_t layer, size_t idx);

  /** creates a new text object on the given layer.
   *  Returns a pointer to the new text object.
   *  If the creation of the text object fails or if the graphic layer
   *  does not exist, NULL is returned.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param applicability defines to which images/frames the new object applies.
   *   Default: all images referenced by the presentation state.
   *  @return a pointer to the new text object
   */
  DVPSTextObject *addTextObject(size_t layer,
    DVPSObjectApplicability applicability=DVPSB_allImages);

  /** deletes the text object with the given index
   *  on the given layer.
   *  Only the objects that are applicable to the current (attached) image
   *  and the selected frame number are used by this method.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the text object, must be < getNumberOfTextObjects(layer)
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition removeTextObject(size_t layer, size_t idx);

  /** moves the text object with the given index on the given
   *  layer to a different layer.
   *  Only the objects that are applicable to the current (attached) image
   *  and the selected frame number are used by this method.
   *  @param old_layer index of the graphic layer on which the text object is,
   *    must be < getNumberOfGraphicLayers()
   *  @param idx index of the text object, must be < getNumberOfTextObjects(layer)
   *  @param new_layer index of the graphic layer to which the text object is moved,
   *    must be < getNumberOfGraphicLayers()
   *  @param applicability defines to which images/frames the new object applies from now on.
   *   Default: all images referenced by the presentation state.
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition moveTextObject(size_t old_layer, size_t idx, size_t new_layer,
    DVPSObjectApplicability applicability=DVPSB_allImages);

  /* graphic objects */

  /** returns the number of graphic objects for the given
   *  graphic layer.
   *  Only the objects that are applicable to the current (attached) image
   *  and the selected frame number are used by this method.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return number of graphic objects
   */
  size_t getNumberOfGraphicObjects(size_t layer);

  /** gets the graphic object with the given index
   *  on the given layer. If the graphic object or the graphic layer does
   *  not exist, NULL is returned.
   *  Only the objects that are applicable to the current (attached) image
   *  and the selected frame number are used by this method.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the graphic object, must be < getNumberOfGraphicObjects(layer)
   *  @return a pointer to the graphic object
   */
  DVPSGraphicObject *getGraphicObject(size_t layer, size_t idx);

  /** creates a new graphic object on the given layer.
   *  Returns a pointer to the new graphic object.
   *  If the creation of the graphic object fails or if the graphic layer
   *  does not exist, NULL is returned.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param applicability defines to which images/frames the new object applies.
   *   Default: all images referenced by the presentation state.
   *  @return a pointer to the new graphic object
   */
  DVPSGraphicObject *addGraphicObject(size_t layer,
    DVPSObjectApplicability applicability=DVPSB_allImages);

  /** deletes the graphic object with the given index
   *  on the given layer.
   *  Only the objects that are applicable to the current (attached) image
   *  and the selected frame number are used by this method.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the graphic object, must be < getNumberOfGraphicObjects(layer)
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition removeGraphicObject(size_t layer, size_t idx);

  /** moves the graphic object with the given index on the given
   *  layer to a different layer.
   *  Only the objects that are applicable to the current (attached) image
   *  and the selected frame number are used by this method.
   *  @param old_layer index of the graphic layer on which the graphic object is,
   *    must be < getNumberOfGraphicLayers()
   *  @param idx index of the graphic object, must be < getNumberOfGraphicObjects(layer)
   *  @param new_layer index of the graphic layer to which the graphic object is moved,
   *    must be < getNumberOfGraphicLayers()
   *  @param applicability defines to which images/frames the new object applies from now on.
   *   Default: all images referenced by the presentation state.
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition moveGraphicObject(size_t old_layer, size_t idx, size_t new_layer,
    DVPSObjectApplicability applicability=DVPSB_allImages);

  /* curves */

  /** returns the number of curve activations for the given
   *  graphic layer.
   *  @param idx index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @return number of curves
   */
  size_t getNumberOfCurves(size_t layer);

  /** gets the curve with the given index
   *  on the given layer. If the curve or the graphic layer does
   *  not exist, NULL is returned.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the curve, must be < getNumberOfCurves(layer)
   *  @return a pointer to the curve
   */
  DVPSCurve *getCurve(size_t layer, size_t idx);

  /** returns the number of curves in the attached image
   *  that could be activated in the presentation state.
   *  @return number of available curves
   */
  size_t getNumberOfCurvesInImage();

  /** gets the curve with the given index
   *  from the attached image. If the curve does
   *  not exist, NULL is returned.
   *  @param idx index of the curve, must be < getNumberOfCurvesInImage()
   *  @return a pointer to the curve
   */
  DVPSCurve *getCurveInImage(size_t idx);

  /** activates curve in presentation state.
   *  This method adds an activation for the given curve from the
   *  attached image to the given graphic layer in the presentation state.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param curveidxinimage index of the curve in the attached image,
   *    must be < getNumberOfCurvesInImage()
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition addCurve(size_t layer, size_t curveidxinimage);

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

  /** gets the overlay label of the given activated overlay.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the overlay, must be < getNumberOfActiveOverlays().
   *  @return label string if it exists, NULL otherwise.
   */
  const char *getActiveOverlayLabel(size_t layer, size_t idx);

  /** gets the overlay description of the given activated overlay.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the overlay, must be < getNumberOfActiveOverlays().
   *  @return description string if it exists, NULL otherwise.
   */
  const char *getActiveOverlayDescription(size_t layer, size_t idx);

  /** checks whether the given activated overlay is a ROI
   *  (region of interest) overlay.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the overlay, must be < getNumberOfActiveOverlays().
   *  @return OFTrue if overlay exists and is ROI, OFFalse otherwise.
   */
  OFBool activeOverlayIsROI(size_t layer, size_t idx);

  /** gets one overlay bitmap.
   *  @param layer index of the graphic layer on which this overlay is
   *    activated, must be < getNumberOfGraphicLayers().
   *  @param idx index of the overlay activation on the given layer,
   *    must be < getNumberOfActiveOverlays(layer).
   *  @param overlayData upon success a pointer to the overlay plane is passed back
   *    in this parameter. The overlay plane is organized as one byte or one word per pixel.
   *    The byte/word values are already transformed from pvalues to DDLs.
   *  @param width upon success the width of the overlay bitmap in pixels is returned in this parameter.
   *  @param height upon success the height of the overlay bitmap in pixels is returned in this parameter.
   *  @param left_pos upon success the horizontal position of the overlay relative to the image
   *   is returned. 0 means that the overlay is left aligned with the image.
   *   Since the overlay is cropped at the borders of the image, values < 0 are impossible.
   *  @param top_pos upon success the vertical position of the overlay relative to the image
   *   is returned.
   *  @param isROI returns OFTrue if the overlay is ROI, OFFalse if the overlay is Graphic.
   *  @param fore returns value of foreground color (pvalue), all other values are transparent
   *  @param bits number of bits used for overlayData (valid: 8 or 12, default: 8). If bits is less than or
   *   equal to 8 the resulting overlayData is an array of 8 bit values, an array 16 bit values otherwise.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition getOverlayData(
     size_t layer,
     size_t idx,
     const void *&overlayData,
     unsigned int &width,
     unsigned int &height,
     unsigned int &left_pos,
     unsigned int &top_pos,
     OFBool &isROI,
     Uint16 &fore,
     unsigned int bits = 8);

  /** gets the number of overlays which are embedded in the
   *  image currently attached to the presentation state. Overlays in the image are counted only
   *  if they are not shadowed by overlays that are embedded in the presentation state
   *  and use the same repeating group number.
   *  @return number of overlays in attached image
   */
  size_t getNumberOfOverlaysInImage();

  /** gets the repeating group number of the given overlay in the attached image.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInImage().
   *  @return repeating group number if found, 0 otherwise.
   */
  Uint16 getOverlayInImageGroup(size_t idx);

  /** gets the overlay label of the given overlay in the attached image.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInImage().
   *  @return label string if it exists, NULL otherwise.
   */
  const char *getOverlayInImageLabel(size_t idx);

  /** gets the overlay description of the given overlay in the attached image.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInImage().
   *  @return description string if it exists, NULL otherwise.
   */
  const char *getOverlayInImageDescription(size_t idx);

  /** gets the index of the activation layer on which the given
   *  overlay from the attached image is activated.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInImage().
   *  @return layer index (which is < getNumberOfGraphicLayers()) if overlay exists
   *     and is activated, DVPS_IDX_NONE otherwise.
   */
  size_t getOverlayInImageActivationLayer(size_t idx);

  /** checks whether the given overlay in the attached image is a ROI
   *  (region of interest) overlay.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInImage().
   *  @return OFTrue if overlay exists and is ROI, OFFalse otherwise.
   */
  OFBool overlayInImageIsROI(size_t idx);

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

  /** removes an overlay from the presentation state.
   *  If the overlay is activated, the activation is also removed.
   *  Since overlays in the presentation state can shadow overlays in the attached image,
   *  execution of this method may change the number of overlays reported in the attached image.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInPresentationState().
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition removeOverlayFromPresentationState(size_t idx);

  /** changes the repeating group used for an overlay in the presentation state.
   *  Since overlays in the presentation state can shadow overlays in the attached image,
   *  execution of this method may change the number of overlays reported in the attached image.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInPresentationState().
   *  @param newGroup new repeating group number 0x6000-0x601F (even). If this optional parameter is omitted,
   *    the method attemps to automatically determine a new group number so that no overlay in the
   *    attached image is shadowed any more. If this is impossible, the method fails and leaves
   *    the overlay repeating group unchanged.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition changeOverlayGroupInPresentationState(size_t idx, Uint16 newGroup=0);

  /** adds a new overlay bitmap to the presentation state.
   *  The overlay is read from a DICOM dataset which must contain the
   *  attributes required for a graphic or ROI overlay, see class DVPSOverlay.
   *  The dataset can be an image or standalone overlay IOD.
   *  The overlay data is copied into the presentation state, i.e. the DICOM dataset
   *  can be deleted after execution of this method.
   *  @param overlayIOD the DICOM dataset from which the overlay is to be read
   *  @groupInItem the repeating group 0x6000..0x61F (even) of the overlay to be read
   *  @param newGroup repeating group number 0x6000-0x601F (even) to be used for
   *    the overlay in the presentation state. If this optional parameter is omitted,
   *    the method attemps to automatically determine a new group number so that no overlay in the
   *    attached image is shadowed any more. If this is impossible, a group number so far unused
   *    in the presentation state is taken.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition addOverlayToPresentationState(DcmItem& overlayIOD, Uint16 groupInItem, Uint16 newGroup=0);

  /** checks if an overlay from the presentation state is suitable
   *  for use as a bitmap shutter. An overlay is suitable if it is a graphic overlay
   *  with the same size as the attached image and with the origin 1\1.
   *  This method does not check wether the overlay is already activated as overlay
   *  or bitmap shutter.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInPresentationState().
   *  @return OFTrue if overlay can be used as display shutter.
   */
  OFBool overlayIsSuitableAsBitmapShutter(size_t idx);

  /** activates the given overlay from the attached image
   *  on the given graphic layer.
   *  If the overlay is already activated (i.e.
   *  getOverlayInImageActivationLayer(idx) != DVPS_IDX_NONE) this method fails.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInImage().
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition activateOverlayInImage(size_t layer, size_t idx);

  /** activates the given overlay from the presentation state
   *  on the given graphic layer.
   *  If the overlay is already activated or used as a bitmap overlay (i.e.
   *  getOverlayInPresentationStateActivationLayer(idx) != DVPS_IDX_NONE or
   *  overlayIsBitmapShutter(idx) == OFTrue) this method fails.
   *  @param layer index of the graphic layer, must be < getNumberOfGraphicLayers()
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInPresentationState().
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition activateOverlayInPresentationState(size_t layer, size_t idx);

  /** activates an overlay as bitmap shutter.
   *  The overlay must not be activated on a graphic layer (i.e.
   *  getOverlayInPresentationStateActivationLayer(idx) != DVPS_IDX_NONE,
   *  otherwise this method fails.
   *  @param idx index of the overlay, must be < getNumberOfOverlaysInPresentationState().
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition activateOverlayAsBitmapShutter(size_t idx);

  /** removes activation for an overlay which may be
   *  embedded in the attached image or part of the presentation state.
   *  @param layer index of the graphic layer on which this overlay is
   *    activated, must be < getNumberOfGraphicLayers().
   *  @param idx index of the overlay activation on the given layer,
   *    must be < getNumberOfActiveOverlays(layer).
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition deactivateOverlay(size_t layer, size_t idx);

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

  /** attaches an image to the presentation state.
   *  If an image is already attached to the presentation state,
   *  the old image is detached (and freed if necessary) and the new image is attached.
   *  A preview image is created automatically (if values for preview resolution are valid).
   *  @param dataset pointer to the DICOM image as DcmDataset.
   *  @param transferOwnership if true, the presentation state assumes ownership
   *    of the passed DcmDataset, which is freed when a different image is attached
   *    or the presentation state is deleted.
   *  @return EC_Normal if successful, an error code otherwise. If an error code
   *    is returned, the presentation state has the same state as before the call
   *    to this method.
   */
  OFCondition attachImage(DcmDataset *dataset, OFBool transferOwnership);

  /** attaches an image to the presentation state.
   *  If an image is already attached to the presentation state,
   *  the old image is detached (and freed if necessary) and the new image is attached.
   *  A preview image is created automatically (if values for preview resolution are valid).
   *  @param fileformat pointer to the DICOM image as DcmFileFormat.
   *  @param transferOwnership if true, the presentation state assumes ownership
   *    of the passed DcmFileFormat, which is freed when a different image is attached
   *    or the presentation state is deleted.
   *  @return EC_Normal if successful, an error code otherwise. If an error code
   *    is returned, the presentation state has the same state as before the call
   *    to this method.
   */
  OFCondition attachImage(DcmFileFormat *fileformat, OFBool transferOwnership);

  /** detaches and frees the image (incl. preview) attached to the presentation state.
   */
  void detachImage();

  /** checks whether image is inverse (shape, plut or mono1).
   *  @return OFTrue if image is inverse, OFFalse otherwise.
   */
  OFBool isInverse();

  /** inverts image by changing presentation state LUT or presentation state LUT shape.
   *  Pixel data has to be re-get after this transformation.
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition invertImage();

  /** applies presentation state to attached image and returns image bitmap.
   *  This method sets all parameter required to correctly render the pixel data
   *  in the image attached to the presentation state and then creates the
   *  required pixel data which contains all grayscale transformations but none
   *  of the none-grayscale transformations of the presentation state "burned in"
   *  into the pixel data. The pixel data returned is already corrected by a
   *  display transform for the current display device and can be mapped directly
   *  to digital driving levels of the graphics board. The pointer to the pixel
   *  data remains valid until the next call to this function, or until the
   *  image is detached or the presentation state is deleted.
   *  @param pixelData in this parameter a pointer to the pixel data is returned
   *    (array of 8 bit values). The storage area allocated for this pointer must
   *    not be freed by the caller. If the return value is an error code,
   *    no valid pixel data pointer may be assumed.
   *  @param width returns the width of the bitmap in pixels
   *  @param height returns the height of the bitmap in pixels
   *  @return EC_Normal upon success, an error code otherwise.
   */
   OFCondition getPixelData(
     const void *&pixelData,
     unsigned long &width,
     unsigned long &height);

  /** same as method above apart from the fact that the storage area is handled
   *  externally.
   *  @param pixelData pointer to storage area where the pixel data is copied to
   *    (array of 8 bit values). The storage area must be allocated and deleted
   *    from the calling method.
   *  @param size specifies size of the storage area in bytes.
   *  @return EC_Normal upon success, an error code otherwise.
   */
   OFCondition getPixelData(
     void *pixelData,
     unsigned long size);

   /** returns the SOP Class UID of the currently attached image.
    *  @return SOP class UID of current image, NULL if absent
    */
   const char *getAttachedImageSOPClassUID();

   /** returns the SOP Instance UID of the currently attached image.
    *  @return SOP instance UID of current image, NULL if absent
    */
   const char *getAttachedImageSOPInstanceUID();

   /** gets the width of the attached image.
    *  The rotation status of the presentation state is not taken
    *  into account, i.e. the width of an unrotated image is returned.
    *  This method may only be called when an image is attached to the
    *  presentation state.
    *  @param width upon success, the image width (pixels) is returned in this parameter.
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition getImageWidth(unsigned long &width);

   /** gets the height of the attached image.
    *  The rotation status of the presentation state is not taken
    *  into account, i.e. the height of an unrotated image is returned.
    *  This method may only be called when an image is attached to the
    *  presentation state.
    *  @param height upon success, the image height (pixels) is returned in this parameter.
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition getImageHeight(unsigned long &height);

   /** gets number of bytes used for the print bitmap.
    *  (depends on width, height and depth)
    *  @return number of bytes used for the print bitmap
    */
   unsigned long getPrintBitmapSize();

   /** sets the minimum print bitmap width and height.
    *  Smaller images are scaled up by an appropriate integer factor. Both maximum
    *  values need to be twice greater than the maximum of the minimum values.
    *  @param width minimum width of print bitmap (in pixels)
    *  @param height minimum height of print bitmap (in pixels)
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition setMinimumPrintBitmapWidthHeight(unsigned long width,
                                                unsigned long height);

   /** sets the maximum print bitmap width and height.
    *  Larger images are scaled down by an appropriate integer factor. Both maximum
    *  values need to be twice greater than the maximum of the minimum values.
    *  @param width maximum width of print bitmap (in pixels)
    *  @param height maximum height of print bitmap (in pixels)
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition setMaximumPrintBitmapWidthHeight(unsigned long width,
                                                unsigned long height);

   /** gets width and height of print bitmap.
    *  Bitmap size depends on implicit scaling, a heuristic is used for very small images.
    *  The return values depend on the current minimum/maximum print bitmap width/height values!
    *  @param width upon success, the bitmap width (in pixels) is returned in this parameter
    *  @param height upon success, the bitmap height (in pixels) is returned in this parameter
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition getPrintBitmapWidthHeight(unsigned long &width,
                                         unsigned long &height);

   /** gets width of print bitmap.
    *  Bitmap size depends on implicit scaling, a heuristic is used for very small images.
    *  The return value depends on the current minimum/maximum print bitmaps width/height values!
    *  @param width upon success, the image width (in pixels) is returned in this parameter
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition getPrintBitmapWidth(unsigned long &width);

   /** gets height of print bitmap.
    *  bitmap size depends on implicit scaling, a heuristic is used for very small images
    *  The return value depends on the current minimum/maximum print bitmaps width/height values!
    *  @param height upon success, the image height (in pixels) is returned in this parameter
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition getPrintBitmapHeight(unsigned long &height);

   /** gets the presentation pixel aspect ratio for the print bitmap.
    *  Pixel aspect ratio is defined here as the width of a pixel divided
    *  by the height of a pixel (x/y). The related image is already rotated and flipped!
    *  @return pixel aspect ratio
    */
   double getPrintBitmapPixelAspectRatio();

   /** gets requested image size for print bitmap.
    *  If the presentation state mode is DVPSD_trueSize, this method computes
    *  the true physical width (in mm) of the print image (under consideration of the
    *  rotation status) and writes it to the requestedImageSize string.
    *  @param requestedImageSize requested image size is written to this parameter upon
    *    successful return. Otherwise string is empty upon return.
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition getPrintBitmapRequestedImageSize(OFString& requestedImageSize);

   /** writes the bitmap data into the given buffer.
    *  The bitmap has the format: 12 bits stored and 16 bits allocated. This method is used
    *  to create the preformatted bitmap where the annotations are later burned in.
    *  Implicit scaling is performed if the bitmap is too small (see minimum bitmap size).
    *  The storage area must be allocated and deleted from the calling method.
    *  @param bitmap pointer to storage area where the pixel data is copied to.
    *  @param size specifies size of the storage area in bytes
    *  @param inversePLUT render inverse PLUT into bitmap if OFTrue (optional)
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition getPrintBitmap(void *bitmap,
                              unsigned long size,
                              OFBool inversePLUT = OFFalse);

   /** creates a new preview image based on the current image and pstate.
    *  The maximum size of this image is specified by the two parameters maxWidth and maxHeight.
    *  The actual size should be determined using one of the following appropriate methods (e.g.
    *  getPreviewImageWidthHeight) since the original pixel aspect ratio is alsways considered.
    *  The preview image includes all grayscale and spatial transformations performed on the
    *  current image so far. The method renderPixelData also renders the preview image (if existing).
    *  Therefore the preview image is always held consistent with the current image.
    *  Overlays, bitmapped shutters and any other annotations are not rendered into the preview image.
    *  @param width the maximum width used to create the preview image
    *  @param height the maximum height used to create the preview image
    *  @param clipMode specifies whether to clip the preview image to the displayed area (not implemented!)
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition createPreviewImage(unsigned long maxWidth,
                                  unsigned long maxHeight,
                                  OFBool clipMode = OFFalse);

   /** deletes and disables the current preview image.
    */
   void deletePreviewImage();

   /** gets number of bytes used for the preview image bitmap.
    *  (depends on width and height)
    *  @return number of bytes used for the preview image bitmap
    */
   unsigned long getPreviewImageSize();

   /** gets current width and height of the preview image.
    *  @param width upon success, the image width (in pixels) is returned in this parameter
    *  @param height upon success, the image height (in pixels) is returned in this parameter
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition getPreviewImageWidthHeight(unsigned long &width,
                                          unsigned long &height);

   /** gets current width of the preview image.
    *  @param width upon success, the image width (in pixels) is returned in this parameter
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition getPreviewImageWidth(unsigned long &width);

   /** gets current height of the preview image.
    *  @param height upon success, the image height (in pixels) is returned in this parameter
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition getPreviewImageHeight(unsigned long &height);

   /** writes the bitmap data of the preview image into the given buffer.
    *  The storage area must be allocated and deleted from the calling method.
    *  @param bitmap pointer to storage area where the pixel data is copied to (array of 8 bit values)
    *  @param size specifies size of the storage area in bytes
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition getPreviewImageBitmap(void *bitmap,
                                     unsigned long size);

   /** gets smallest and biggest possible pixel value in the attached image.
    *  These values are defined as the smallest and biggest number that
    *  could possibly be contained in the image after application of the Modality transform,
    *  but before any VOI, Presentation or display transform.
    *  This method may only be called when an image is attached to the
    *  presentation state.
    *  @param minValue upon success, the smallest value is returned in this parameter.
    *  @param maxValue upon success, the biggest value is returned in this parameter.
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition getImageMinMaxPixelRange(double &minValue, double& maxValue);

   /** gets smallest and biggest occuring pixel value in the attached image.
    *  These values are defined as the smallest and biggest number that
    *  are actually contained in the image after application of the Modality transform,
    *  but before any VOI, Presentation or display transform.
    *  This method may only be called when an image is attached to the
    *  presentation state.
    *  @param minValue upon success, the smallest value is returned in this parameter.
    *  @param maxValue upon success, the biggest value is returned in this parameter.
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition getImageMinMaxPixelValue(double &minValue, double& maxValue);

   /** gets the number of frames of the current (attached) image.
    *  This method may only be called when an image is attached to the
    *  presentation state.
    *  @param frames upon success, the number of frames is returned in this parameter.
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition getImageNumberOfFrames(unsigned long &frames);

   /** selects one frame of a multiframe image. This affects the image bitmap
    *  that is rendered, the overlay bitmaps and the visibility of graphic and text objects.
    *  This method may only be called when an image is attached to the
    *  presentation state.
    *  @param frame frame number in the range [1..getImageNumberOfFrames()]
    *  @return EC_Normal upon success, an error code otherwise
    */
   OFCondition selectImageFrameNumber(unsigned long frame);

  /** gets the index of the currently selected frame in a multi-frame image.
   *  @return index of the currently selected frame, 0 if an error occurred
   */
   unsigned long getSelectedImageFrameNumber();

   /* Display transform */

   /** gets the currently selected display transform.
    *  Display transform will only be performed if switched on _and_
    *  a valid monitor characteristics description exists.
    *  Default after creation of a presentation state is "on".
    *  @return current display transform if on, DVPSD_none if off.
    */
   DVPSDisplayTransform getDisplayTransform() { return displayTransform; }

   /** activates or deactivates display correction.
    *  Display transform will only be performed if switched on
    *  _and_ a valid display function object exists.
    *  @param transform display transform to be set, DVPSD_none to switch off.
    */
   void setDisplayTransform(DVPSDisplayTransform transform) { displayTransform = transform; }

   /** converts a 16-bit P-Value to an 8-bit DDL value for on-sceen display.
    *  If a display function is set and enabled (see setDisplayTransform()),
    *  the DDL is corrected for the nonlinearity of the display, otherwise
    *  a simple linear mapping is performed. For 12-bit DDL values (hardcopy)
    *  the display function is automatically disabled.
    *  @param pvalue P-Value 0..0xFFFF
    *  @param bits number of bits used for the output DDL value (8 = softcopy, 12 = hardcopy)
    *  @return display driving level (DDL), 0..0xFF (8 bit) / 0..0xFFF (12 bit)
    */
   Uint16 convertPValueToDDL(Uint16 pvalue, unsigned int bits = 8);

  /* print related methods */

  /** writes the patient module attributes and a source image sequence
   *  for a grayscale hardcopy image.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition writeHardcopyImageAttributes(DcmItem &dset);

  /** gets the modality of the attached image.
   *  @return modality string if it exists, NULL or empty string otherwise.
   */
  const char *getCurrentImageModality();

  /** sets a new log stream
   *  @param stream new log stream, NULL for default logstream
   *  @param verbMode verbose mode flag
   *  @param dbgMode debug mode flag
   */
  void setLog(OFConsole *stream, OFBool verbMode, OFBool dbgMode);

private:

  /** private undefined copy constructor
   */
  DVPresentationState(const DVPresentationState& copy);

  /** private undefined assignment operator
   */
  DVPresentationState& operator=(const DVPresentationState& obj);

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

  /** tries to find an overlay repeating group that is not used in the
   *  presentation state or the attached image. If that does not exist,
   *  it returns a group number that is unused in the presentation state
   *  but used in the image.
   *  @param currentGroup the current group number of the overlay
   *    to be changed (if any)
   *  @return group number 0x6000..0x601F if found, 0 otherwise.
   */
  Uint16 findOverlayGroup(Uint16 currentGroup=0);

  /** prepares pixel data for image and overlays for access.
   *  This method is called internally before any image pixel data
   *  or overlay bitmaps is created. It makes sure that the following settings
   *  of the presentation state are reflected in the dcmimage-based
   *  image processing routines: VOI transformation, Presentation LUT,
   *  rotation, flip, overlay activation.
   *  @param display use current display function if OFTrue, don't use otherwise
   */
  void renderPixelData(OFBool display = OFTrue);

  /** creates a default displayed area selection for the given dataset.
   *  Used in createFromImage().
   *  @param dset the DICOM dataset containing the image IOD
   *  @return EC_Normal upon success, an error code otherwise.
   */
  OFCondition createDefaultDisplayedArea(DcmItem &dset);

  /** attempts to find the displayed area selection for the
   *  current image and frame. If not found, creates a default.
   *  @return displayed area selection item. If creation of a default failed
   *    or no image is attached to the presentation state, returns NULL.
   */
  DVPSDisplayedArea *getDisplayedAreaSelection();

  /** attempts to find the Softcopy VOI LUT item for the
   *  current image and frame. If not found, returns NULL.
   *  @return softcopy VOI LUT item for current image and frame if found, NULL otherwise
   */
  DVPSSoftcopyVOI *getCurrentSoftcopyVOI();

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
  DVPSDisplayedArea_PList  displayedAreaSelectionList;

  /* Module: Softcopy Presentation LUT (M)
   * There must never be more that one Presentation LUT for one Presentation State,
   * therefore we need not save a list of LUTs.
   */
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
  DVPSSoftcopyVOI_PList    softcopyVOIList;

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
  /** a pointer to the dcmimage representation of the (smaller) preview
   *  image to which the presentation state is currently applied
   */
  DicomImage *previewImage;
  /** contains the width of the attached image without consideration of rotation.
   */
  unsigned long currentImageWidth;
  /** contains the height of the attached image without consideration of rotation.
   */
  unsigned long currentImageHeight;
  /** contains the width of the attached image after pixel data have been rendered (w/o clipping).
   */
  unsigned long renderedImageWidth;
  /** contains the height of the attached image after pixel data have been rendered (w/o clipping).
   */
  unsigned long renderedImageHeight;
  /** contains the top hand corner of the attached image after pixel data have been rendered.
   */
  signed long renderedImageTop;
  /** contains the left hand corner of the attached image after pixel data have been rendered.
   *  (the following equation is always true: renderedImageTop <= renderedImageBottom)
   */
  signed long renderedImageLeft;
  /** contains the bottom hand corner of the attached image after pixel data have been rendered.
   *  (the following equation is always true: renderedImageLeft <= renderedImageRight)
   */
  signed long renderedImageBottom;
  /** contains the right hand corner of the attached image after pixel data have been rendered.
   */
  signed long renderedImageRight;
  /** contains the SOP Class UID of the attached image
   */
  char *currentImageSOPClassUID;
  /** contains the SOP Instance UID of the attached image
   */
  char *currentImageSOPInstanceUID;
  /** contains the current selected frame number of the attached image.
   *  Frame numbers are counted from 1 (like in DICOM, unlike dcmimgle).
   */
  unsigned long currentImageSelectedFrame;
  /** a flag describing whether the presentation state is owner of
   *  the DICOM dataset in currentImageDataset/currentImageFileformat.
   *  If the presentation state is owner, it will delete the dataset
   *  upon its own destruction or attachment of a different image.
   */
  OFBool currentImageOwned;
  /** a flag describing whether the VOI settings in currentImage
   *  match the ones in the presentation state.
   */
  OFBool currentImageVOIValid;
  /** a flag describing whether the presentation LUT settings
   *  in currentImage match the ones in the presentation state.
   */
  OFBool currentImagePLUTValid;
  /** a flag describing whether currentImage has been flipped
   */
  OFBool currentImageFlip;
  /** a flag describing the current rotation angle of currentImage
   */
  DVPSRotationType currentImageRotation;
  /** a flag describing whether the Overlay settings in
   *  currentImage match the ones in the presentation state.
   *  values: 0=invalid, 1=invalid, but no external overlay added, 2=valid
   */
  int currentImageOverlaysValid;

  /** list of curves of the currently attached image
   */
  DVPSCurve_PList currentImageCurveList;
  /** list of VOI LUTs of the currently attached image
   */
  DVPSVOILUT_PList currentImageVOILUTList;
  /** list of VOI Windows of the currently attached image
   */
  DVPSVOIWindow_PList currentImageVOIWindowList;

  /** modality of currently attached image (if any)
   */
  DcmCodeString currentImageModality;

  /** true if attached image is MONOCHROME1
   */
  OFBool currentImageMonochrome1;

  /** flag indicating the currently selected display transform
   *  DVPSD_none if switched off
   */
  DVPSDisplayTransform displayTransform;

  /** a flag describing whether current image is inverse
   */
  OFBool imageInverse;

  /** reference to list of display functions if existing
   */
  DiDisplayFunction **displayFunction;

  /** minimum width of print bitmap (used for implicit scaling)
   */
  unsigned long minimumPrintBitmapWidth;

  /** minimum height of print bitmap (used for implicit scaling)
   */
  unsigned long minimumPrintBitmapHeight;

  /** maximum width of print bitmap (used for implicit scaling)
   */
  unsigned long maximumPrintBitmapWidth;

  /** maximum height of print bitmap (used for implicit scaling)
   */
  unsigned long maximumPrintBitmapHeight;

  /** maximum width of (optional) preview image
   */
  unsigned long maximumPreviewImageWidth;

  /** maximum height of (optional) preview image
   */
  unsigned long maximumPreviewImageHeight;

  /** output stream for error messages, never NULL
   */
  OFConsole *logstream;

  /** flag indicating whether we're operating in verbose mode
   */
  OFBool verboseMode;

  /** flag indicating whether we're operating in debug mode
   */
  OFBool debugMode;

};

#endif

/*
 *  $Log: dvpstat.h,v $
 *  Revision 1.41  2002-12-09 13:29:49  joergr
 *  Renamed parameter/local variable to avoid name clashes with global
 *  declaration left and/or right (used for as iostream manipulators).
 *
 *  Revision 1.40  2001/09/26 15:36:17  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.39  2001/06/01 15:50:23  meichel
 *  Updated copyright header
 *
 *  Revision 1.38  2000/11/13 15:50:43  meichel
 *  Added dcmpstat support methods for creating image references
 *    in SR documents.
 *
 *  Revision 1.37  2000/07/04 15:58:03  joergr
 *  Added support for overriding the presentation LUT settings made for the
 *  image boxes.
 *
 *  Revision 1.36  2000/06/09 10:14:12  joergr
 *  Added support for rendering inverse presentation LUT into print bitmaps.
 *
 *  Revision 1.35  2000/06/08 17:36:51  joergr
 *  Corrected bug in addImageReferenceToPState().
 *
 *  Revision 1.34  2000/06/02 16:00:53  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.33  2000/05/30 14:21:24  joergr
 *  Renamed some variables to avoid compiler warnings (reported by gcc 2.9x with
 *  additional compiler flags).
 *
 *  Revision 1.32  2000/05/30 13:47:03  joergr
 *  Added support for multi-frame images and multiple references from a single
 *  presentation to a number of images.
 *  Removed methods which were already marked as "retired".
 *  Added new function allowing to set a VOILUT created from a given gamma
 *  value.
 *
 *  Revision 1.31  2000/03/08 16:28:57  meichel
 *  Updated copyright header.
 *
 *  Revision 1.30  2000/02/29 12:16:16  meichel
 *  Fixed bug in dcmpstat library that caused Monochrome1 images
 *    to be printed inverse if a Presentation LUT was applied.
 *
 *  Revision 1.29  1999/11/15 19:02:16  joergr
 *  Changed behaviour of method getOverlayData(): parameter 'transp' replaced by
 *  'fore' to specify the foreground color used for the overlay plane.
 *
 *  Revision 1.28  1999/11/03 10:36:32  joergr
 *  Enhanced comments for methods dealing with arrays of pixel data.
 *
 *  Revision 1.27  1999/10/20 10:49:20  joergr
 *  Enhanced method getOverlayData to support 12 bit data for print.
 *  Enhanced method convertPValueToDDL to support 12 bit data for print.
 *  Added support for a down-scaled preview image of the current DICOM image
 *  (e.g. useful for online-windowing or print preview).
 *
 *  Revision 1.26  1999/10/19 16:24:50  meichel
 *  Corrected handling of MONOCHROME1 images when used with P-LUTs
 *
 *  Revision 1.25  1999/10/13 14:11:57  meichel
 *  Added config file entries and access methods
 *    for user-defined VOI presets, log directory, verbatim logging
 *    and an explicit list of image display formats for each printer.
 *
 *  Revision 1.24  1999/10/07 17:21:51  meichel
 *  Reworked management of Presentation LUTs in order to create tighter
 *    coupling between Softcopy and Print.
 *
 *  Revision 1.23  1999/09/10 12:46:48  meichel
 *  Added implementations for a number of print API methods.
 *
 *  Revision 1.22  1999/09/10 09:02:32  joergr
 *  Added support for CIELAB display function. New methods to handle display
 *  functions. Old methods are marked as retired and should be removed asap.
 *
 *  Revision 1.21  1999/09/07 09:04:37  joergr
 *  Completed support for getting a print bitmap out of a pstate object.
 *
 *  Revision 1.20  1999/09/01 16:14:42  meichel
 *  Added support for requested image size to print routines
 *
 *  Revision 1.19  1999/08/27 15:57:58  meichel
 *  Added methods for saving hardcopy images and stored print objects
 *    either in file or in the local database.
 *
 *  Revision 1.18  1999/08/25 16:48:01  joergr
 *  Added minimal support to get a print bitmap out of a pstate object.
 *
 *  Revision 1.17  1999/07/30 13:34:51  meichel
 *  Added new classes managing Stored Print objects
 *
 *  Revision 1.16  1999/07/22 16:39:13  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.15  1999/05/04 12:28:11  meichel
 *  Removed carriage returns
 *
 *  Revision 1.14  1999/04/27 11:23:57  joergr
 *  Added method to check whether current image is inverse or not.
 *
 *  Revision 1.13  1999/03/22 09:05:35  joergr
 *  Added parameter to get value of (transparent) background color for method
 *  getOverlayData.
 *
 *  Revision 1.12  1999/03/03 13:26:06  joergr
 *  Added method to invert an image by changing the presentation state LUT or
 *  shape.
 *  Moved method 'isBartenTransformPossible()' from presentation state class to
 *  interface class.
 *
 *  Revision 1.11  1999/03/02 13:01:02  joergr
 *  Added method to presentation state class that checks whether Barten
 *  transformation is possible or not.
 *
 *  Revision 1.10  1999/02/25 18:40:08  joergr
 *  Added method to fill pixel data into an externally handled storage area.
 *
 *  Revision 1.9  1999/02/18 11:36:38  meichel
 *  Added new method convertPValueToDDL() to DVPresentationState
 *    that maps P-Values to DDLs.
 *
 *  Revision 1.8  1999/02/17 10:05:32  meichel
 *  Moved creation of Display Function object from DVPresentationState to
 *    DVInterface to avoid unnecessary re-reads.
 *
 *  Revision 1.7  1999/02/09 15:58:57  meichel
 *  Implemented bitmap shutter activation amd method for
 *    exchanging graphic layers.
 *
 *  Revision 1.6  1999/02/05 17:45:36  meichel
 *  Added config file entry for monitor characteristics file.  Monitor charac-
 *    teristics are passed to dcmimage if present to activate Barten transform.
 *
 *  Revision 1.5  1999/01/15 17:33:05  meichel
 *  added methods to DVPresentationState allowing to access the image
 *    references in the presentation state.  Also added methods allowing to
 *    get the width and height of the attached image.
 *
 *  Revision 1.4  1999/01/11 13:35:49  meichel
 *  added new methods getImageAspectRatio, getImageMinMaxPixelRange and
 *    getImageMinMaxPixelValue to class DVPresentationState.
 *
 *  Revision 1.3  1998/12/22 17:57:07  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *  Revision 1.2  1998/12/14 16:10:35  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:34  meichel
 *  Initial Release.
 *
 *
 */

