/*
 *
 *  Copyright (C) 2015-2016, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for managing common functional group types
 *
 */

#ifndef FGTYPES_H
#define FGTYPES_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmfg/fgdefine.h"

class FGBase;
class FunctionalGroups;

/*-----------------------*
 *  logging              *
 *-----------------------*/

extern DCMTK_DCMFG_EXPORT OFLogger DCM_dcmfgLogger;

#define DCMFG_TRACE(msg) OFLOG_TRACE(DCM_dcmfgLogger, msg)
#define DCMFG_DEBUG(msg) OFLOG_DEBUG(DCM_dcmfgLogger, msg)
#define DCMFG_INFO(msg)  OFLOG_INFO(DCM_dcmfgLogger, msg)
#define DCMFG_WARN(msg)  OFLOG_WARN(DCM_dcmfgLogger, msg)
#define DCMFG_ERROR(msg) OFLOG_ERROR(DCM_dcmfgLogger, msg)
#define DCMFG_FATAL(msg) OFLOG_FATAL(DCM_dcmfgLogger, msg)


/*-----------------------*
 *  constant definitions  *
 *-----------------------*/

/* Specific error conditions for module dcmfg.(TODO)
 * These error codes can be used in addition to the general purpose
 * codes defined in module dcmdata.
 */

/// Functional group already exists
extern DCMTK_DCMFG_EXPORT   const OFConditionConst     FG_EC_DoubledFG;
/// Specified functional group does not exist
extern DCMTK_DCMFG_EXPORT   const OFConditionConst     FG_EC_NoSuchGroup;
/// Not enough items
extern DCMTK_DCMFG_EXPORT   const OFConditionConst     FG_EC_NotEnoughItems;
/// Too many Items
extern DCMTK_DCMFG_EXPORT   const OFConditionConst     FG_EC_TooManyItems;
/// Invalid data
extern DCMTK_DCMFG_EXPORT   const OFConditionConst     FG_EC_InvalidData;
/// Could not write functional group
extern DCMTK_DCMFG_EXPORT   const OFConditionConst     FG_EC_CouldNotWriteFG;
/// Could not insert functional group
extern DCMTK_DCMFG_EXPORT   const OFConditionConst     FG_EC_CouldNotInsertFG;
/// No such shared functional group
extern DCMTK_DCMFG_EXPORT   const OFConditionConst     FG_EC_NoSharedFG;
/// No such per-frame functional group
extern DCMTK_DCMFG_EXPORT   const OFConditionConst     FG_EC_NoPerFrameFG;
/// Could not create functional group
extern DCMTK_DCMFG_EXPORT   const OFConditionConst     FG_EC_CouldNotCreateFG;
/// Could not create functional group
extern DCMTK_DCMFG_EXPORT   const OFConditionConst     FG_EC_CouldNotReadSourceImage;
/// Could not create functional group
extern DCMTK_DCMFG_EXPORT   const OFConditionConst     FG_EC_CouldNotAddFG;
/// Not enough frames found
extern DCMTK_DCMFG_EXPORT   const OFConditionConst     FG_EC_NotEnoughFrames;
/// No stacks specified (but expected)
extern DCMTK_DCMFG_EXPORT   const OFConditionConst     FG_EC_NoStacksFound;

/*---------------------*
 *  class declaration  *
 *---------------------*/

/** General purpose class hiding global functions, constants and types related
 *  to functional groups from the global namespace.
 */
class DCMTK_DCMFG_EXPORT DcmFGTypes
{

public:

  // --- Type definitions ---

  /** Functional group types
   */
  enum E_FGType
  {
    /// Undefined functional group
    EFG_UNDEFINED,
    /// Unknown functional group
    EFG_UNKNOWN,
    /// Cardiac Synchronization
    EFG_CARDIACSYNC,
    /// Contrast/Bolus Usage
    EFG_CONTRASTBOLUSUSAGE,
    /// Derivation Image
    EFG_DERIVATIONIMAGE,
    /// Frame Anatomy
    EFG_FRAMEANATOMY,
    /// Frame Content
    EFG_FRAMECONTENT,
    /// Frame Display Shutter
    EFG_FRAMEDISPLAYSHUTTER,
    /// Frame Pixel Shift
    EFG_FRAMEPIXELSHIFT,
    /// "Frame VOI LUT" or "Frame VOI LUT with LUT" Macro (share same starting sequence)
    EFG_FRAMEVOILUTMETA,
    /// Image Data Type
    EFG_IMAGEDATATYPE,
    /// Irradiation Event Identification
    EFG_IRRADIATIONEVENTIDENT,
    /// Patient Orientation in Frame
    EFG_PATIENTORIENTINFRAME,
    /// Patient Physiological State
    EFG_PATIENTPHYSIOSTATE,
    /// Pixel Intensity Relationship LUT
    EFG_PIXELINTENSITYRELLUT,
    /// Pixel Measures
    EFG_PIXELMEASURES,
    /// "Pixel Value Transformation" or "Identity Pixel Value Transformation" Macro (share same sequence and attributes)
    EFG_PIXELVALUETRANSMETA,
    /// Plane Orientation (Volume)
    EFG_PLANEORIENTVOLUME,
    /// Plane Position (Volume)
    EFG_PLANEPOSITIONVOLUME,
    /// Plane Position (Patient)
    EFG_PLANEPOSPATIENT,
    /// Plane Orientation (Patient)
    EFG_PLANEORIENTPATIENT,
    /// Radiopharmaceutical Usage
    EFG_RADIOPHARAMAUSAGE,
    /// Real World Value Mapping
    EFG_REALWORLDVALUEMAPPING,
    /// Respiratory Synchronization
    EFG_RESPIRATORYSYNC,
    /// Segmentation Macro
    EFG_SEGMENTATION,
    /// Temporal Position Macro
    EFG_TEMPORALPOSITION,
    /// Unassigned Shared Converted Attributes Macro
    EFG_UNASSIGNEDSHAREDCONVERTEDATTRIBUTES,
    /// US Image Description Macro
    EFG_USIMAGEDESCRIPTION
  };

  /** Functional group types
   */
  enum E_FGSharedType
  {
    /// Unknown
    EFGS_UNKNOWN,
    /// Functional Group can be shared or per-frame
    EFGS_BOTH,
    /// Functional Group can only be shared (not per-frame)
    EFGS_ONLYSHARED,
    /// Functional Group can only be per-frame (not shared)
    EFGS_ONLYPERFRAME
  };

  /** US Image Description Frame Type (first value)
   */
  enum E_FGUSFrameType
  {
    /// Unknown
    EFGFT_UNKNOWN,
    /// ORIGINAL
    EFGFT_ORIGINAL,
    /// DERIVED
    EFGFT_DERIVED
  };

  // -- static helpers --

  /** Returns functional group type based on tag key
   *  @param  key The key to get the group type for
   *  @return The functional group type
   */
  static E_FGType tagKey2FGType(const DcmTagKey& key);

  /** Returns the name of the functional group based on the group type
   *  @param  fgType The functional group type
   *  @return The name of the functional group
   */
  static OFString FGType2OFString(const E_FGType fgType);

  /** Returns the name of a functional group based on a given tag key
   *  @param  key The tag key to get the functional group type for
   *  @return The name of the functional group
   */
  static OFString tagKey2FGString(const DcmTagKey& key);

};

#endif // FGTYPES_H
