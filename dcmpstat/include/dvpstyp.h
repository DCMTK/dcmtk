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
 *    enums: DVPSoverlayActivation, DVPSVOIActivation, DVPSGraphicLayering
 *           DVPSPresentationLUTType, DVPSRotationType, DVPSSourceType
 *           DVPSShutterType
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:36 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSTYP_H__
#define __DVPSTYP_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */

/** describes how to handle overlays when creating a default presentation state
 *  for an image. 
 */
enum DVPSoverlayActivation
{
  /** ignore overlays even if present in the image.
   *  A presentation state without any embedded or activated overlays
   *  is created.
   */
  DVPSO_ignoreOverlays,
  /** if overlays are present in the image, activate but do not copy them.
   *  If the image contains overlays, overlay activation elements are
   *  created in the presentation state object. The overlays remain in the image.
   */
  DVPSO_referenceOverlays,
  /** if overlays are present in the image, copy them.
   *  If the image contains overlays which are not embedded in the pixel
   *  data but use the OverlayData element, the overlays are copied to the
   *  presentation state and activated. 
   *  Overlays that are embedded in the image pixel data are not copied
   *  but also activated.
   */ 
  DVPSO_copyOverlays
};


/** describes how to handle VOI transformations when creating a default presentation state
 *  for an image. 
 */
enum DVPSVOIActivation
{
  /** ignore VOI transformations even if present in the image.
   *  The presentation state will not contain any VOI transformation.
   */
  DVPSV_ignoreVOI,
  /** select the first VOI window if present, VOI LUT alternatively.
   *  If the image contains one or more settings for window center and
   *  window width, the first window center/width is copied to the presentation
   *  state. If the image contains no window center/width but does contain
   *  one or more VOI LUTs, the first VOI LUT is copied to the presentation
   *  state.
   */
  DVPSV_preferVOIWindow,
  /** select the first VOI LUT if present, VOI Window alternatively.
   *  If the image contains one or more VOI LUTs, 
   *  the first VOI LUT is copied to the presentation
   *  state. If the image contains no VOI LUT but does contain
   *  one or more settings for window center/width, the first 
   *  window center and width are copied to the presentation state.
   */
  DVPSV_preferVOILUT
};

/** describes how to handle layering of curves and overlays 
 *  when creating a default presentation state for an image. 
 */
enum DVPSGraphicLayering
{
  /** put all overlays and curves into a single graphic layer.
   *  If curves and/or overlays are present, a single graphic layer is
   *  created and all curves and overlays are assigned to this single layer.
   */
  DVPSG_oneLayer,
  /** create one graphic layer for overlays and one graphic layer for curves on top of that.
   *  If overlays are present, one layer containing all overlays is created.
   *  If curves are present, a different layer containing all curves is created.
   *  If both curves and overlays are present, the curves are assigned the higher layer order.
   */
  DVPSG_twoLayers,
  /** create individual layers for each referenced element, curves on top of overlays.
   *  For each overlay and curve present, a separate graphic layer is created.
   *  The curve layers have higher order than the overlay layers.
   */
  DVPSG_separateLayers
};

/** describes a type of presentation LUT that is currently
 *  being used or set in a presentation state.
 */
enum DVPSPresentationLUTType
{
  /** Presentation LUT Shape with value 'IDENTITY'
   */
  DVPSP_identity,
  /** Presentation LUT Shape with value 'INVERSE'
   */
  DVPSP_inverse,
  /** Presentation LUT look up table
   */
  DVPSP_table
};

/** describes the rotation status of a presentation state.
 */
enum DVPSRotationType
{
  /** no rotation
   */
  DVPSR_0_deg,
  /** rotation of 90 degrees
   */
  DVPSR_90_deg,
  /** rotation of 180 degrees
   */
  DVPSR_180_deg,
  /** rotation of 270 degrees
   */
  DVPSR_270_deg
};

/** describes the origin (or place of storage) of data
 *  managed by a presentation state.
 */
enum DVPSSourceType
{
  /** data stems from presentation state
   */
  DVPSS_presentationState,
  /** data stems from image
   */
  DVPSS_image
};

/** describes the different types of display shutters
 */
enum DVPSShutterType
{
  /** rectangular shutter
   */
  DVPSU_rectangular,
  /** circular shutter
   */
  DVPSU_circular,
  /** polygonal shutter
   */
  DVPSU_polygonal,
  /** bitmap shutter
   */
  DVPSU_bitmap 
};

#endif

/*
 *  $Log: dvpstyp.h,v $
 *  Revision 1.1  1998-11-27 14:50:36  meichel
 *  Initial Release.
 *
 *
 */
