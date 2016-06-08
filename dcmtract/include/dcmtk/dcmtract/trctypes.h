/*
 *
 *  Copyright (C) 2016, Open Connections GmbH
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
 *  Module:  dcmtract
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class and definitions managing various tractography-typical types
 *
 */

#ifndef TRCTYPES_H
#define TRCTYPES_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmtract/trcdef.h"

/*
 * Logging
 */

extern DCMTK_DCMTRACT_EXPORT OFLogger DCM_dcmtractLogger;

#define DCMTRACT_TRACE(msg) OFLOG_TRACE(DCM_dcmtractLogger, msg)
#define DCMTRACT_DEBUG(msg) OFLOG_DEBUG(DCM_dcmtractLogger, msg)
#define DCMTRACT_INFO(msg)  OFLOG_INFO(DCM_dcmtractLogger, msg)
#define DCMTRACT_WARN(msg)  OFLOG_WARN(DCM_dcmtractLogger, msg)
#define DCMTRACT_ERROR(msg) OFLOG_ERROR(DCM_dcmtractLogger, msg)
#define DCMTRACT_FATAL(msg) OFLOG_FATAL(DCM_dcmtractLogger, msg)


/*
 * Definition of error codes
 */

/// Invalid Point Coordinates Data
extern DCMTK_DCMTRACT_EXPORT   const OFConditionConst     TRC_EC_InvalidPointCoordinatesData;
/// Invalid Color Information
extern DCMTK_DCMTRACT_EXPORT   const OFConditionConst     TRC_EC_InvalidColorInformation;
/// No such Track
extern DCMTK_DCMTRACT_EXPORT   const OFConditionConst     TRC_EC_NoSuchTrack;
/// No such Measurement
extern DCMTK_DCMTRACT_EXPORT   const OFConditionConst     TRC_EC_NoSuchMeasurement;
/// Measurement Data Missing
extern DCMTK_DCMTRACT_EXPORT   const OFConditionConst     TRC_EC_MeasurementDataMissing;
/// Invalid Statistic Data
extern DCMTK_DCMTRACT_EXPORT   const OFConditionConst     TRC_EC_InvalidStatisticData;
/// Invalid Track Data
extern DCMTK_DCMTRACT_EXPORT   const OFConditionConst     TRC_EC_InvalidTrackData;

/**
 * Types specific to this module
 */

class TrcTypes
{
public:

  /** Tractography coloring modes
   */
  enum E_TrackColorMode
  {
    /// Unknown (default value if not initialized)
    CM_UNKNOWN,
    /// Recommended CIELab value is provided as one color per track
    CM_TRACK,
    /// Recommended CIELab values are provided as separate colors for each point
    /// within track
    CM_POINTS,
    /// Recommended CIELab value is provided per track set and not for single tracks.
    /// Is returned from Trackset if corresponding CIELab attribute is set, and by Tracks
    /// whose CIELab attributes are not provided at all
    CM_TRACKSET,
    /// Erroneous setting of color mode, i.e. Recommended CIELab Value or
    /// Recommended CIELab Value List are both set or wrongly set on a track
    CM_ERROR
  };

  /** Tractography Track Set laterality types.
   *  Laterality of (paired) body part examined.
   */
  enum E_TrackSetLaterality
  {
    /// Unknown (default value if not initialized)
    LAT_UNKNOWN,
    /// Error: In case an invalid value is used
    LAT_ERROR,
    /// Left body part
    LAT_LEFT,
    /// Right body part
    LAT_RIGHT,
    /// Left and right body part
    LAT_RIGHT_AND_LEFT,
    /// Either left or right body part
    LAT_UNILATERAL
  };
};

#endif // TRCTYPES_H
