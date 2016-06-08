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
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class for managing common types needed by dcmiod module
 *
 */

#ifndef IODTYPES_H
#define IODTYPES_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmiod/ioddef.h"
#include "dcmtk/dcmiod/cielabutil.h"

// ----------------------------------------------------------------------------
// Define the loggers for this module
// ----------------------------------------------------------------------------

extern DCMTK_DCMIOD_EXPORT OFLogger DCM_dcmiodLogger;

#define DCMIOD_TRACE(msg) OFLOG_TRACE(DCM_dcmiodLogger, msg)
#define DCMIOD_DEBUG(msg) OFLOG_DEBUG(DCM_dcmiodLogger, msg)
#define DCMIOD_INFO(msg)  OFLOG_INFO(DCM_dcmiodLogger, msg)
#define DCMIOD_WARN(msg)  OFLOG_WARN(DCM_dcmiodLogger, msg)
#define DCMIOD_ERROR(msg) OFLOG_ERROR(DCM_dcmiodLogger, msg)
#define DCMIOD_FATAL(msg) OFLOG_FATAL(DCM_dcmiodLogger, msg)


// ----------------------------------------------------------------------------
// Error constants
// ----------------------------------------------------------------------------

extern DCMTK_DCMIOD_EXPORT   const OFConditionConst     IOD_EC_WrongSOPClass;
extern DCMTK_DCMIOD_EXPORT   const OFConditionConst     IOD_EC_MissingAttribute;
extern DCMTK_DCMIOD_EXPORT   const OFConditionConst     IOD_EC_MissingSequenceData;
extern DCMTK_DCMIOD_EXPORT   const OFConditionConst     IOD_EC_InvalidDimensions;
extern DCMTK_DCMIOD_EXPORT   const OFConditionConst     IOD_EC_CannotInsertFrame;
extern DCMTK_DCMIOD_EXPORT   const OFConditionConst     IOD_EC_InvalidPixelData;
extern DCMTK_DCMIOD_EXPORT   const OFConditionConst     IOD_EC_InvalidObject;
extern DCMTK_DCMIOD_EXPORT   const OFConditionConst     IOD_EC_CannotDecompress;
extern DCMTK_DCMIOD_EXPORT   const OFConditionConst     IOD_EC_NoSuchRule;
extern DCMTK_DCMIOD_EXPORT   const OFConditionConst     IOD_EC_InvalidLaterality;
extern DCMTK_DCMIOD_EXPORT   const OFConditionConst     IOD_EC_InvalidElementValue;
extern DCMTK_DCMIOD_EXPORT   const OFConditionConst     IOD_EC_InvalidReference;
extern DCMTK_DCMIOD_EXPORT   const OFConditionConst     IOD_EC_ReferencesOmitted;


/** Class that wraps some constant definitions in the context of IODs
 */
class DCMTK_DCMIOD_EXPORT DcmIODTypes
{

public:

  /** Struct representing a single frame
   */
  struct Frame
  {
    /// Array for the pixel data bytes
    Uint8* pixData;
    /// Number of pixel data bytes (i.e.\ Bits Allocated)
    size_t length;
    /// Destructor, frees memory
    ~Frame() {delete[] pixData;pixData = NULL;}
  };

  /** IOD Information Entities (incomplete list, extended as needed)
   */
  enum IOD_IE
  {
    /// Undefined Information Entity (i.e.\ no value set)
    IE_UNDEFINED,
    /// Patient Entity
    IE_PATIENT,
    /// Study Entity
    IE_STUDY,
    /// Series Entity
    IE_SERIES,
    /// Frame of Reference Entity
    IE_FOR,
    /// Equipment Entity
    IE_EQUIPMENT,
    /// Image Entity
    IE_IMAGE,
    //// Meta Entity: Instance covering image, waveform, etc.
    IE_INSTANCE
  };

  /** Enumerated values for attribute "Laterality"
   */
  enum IOD_LATERALITY
  {
    /// Undefined (e.g.\ value not set)
    LATERALITY_UNDEFINED,
    /// Left body part
    LATERALITY_L,
    /// Right body part
    LATERALITY_R
  };

  /** Enhanced US Image Module: Image Type (first value)
   */
  enum IOD_ENHUSIMAGETYPE
  {
    /// Unknown
    IMAGETYPE_UNKNOWN,
    /// ORIGINAL
    IMAGETYPE_ORIGINAL,
    /// DERIVED
    IMAGETYPE_DERIVED
  };


private:

  /** Private undefined default constructor
   */
  DcmIODTypes() {};

  /** Private undefined destructor
   */
  ~DcmIODTypes() {};

};


#endif // IODTYPES_H
