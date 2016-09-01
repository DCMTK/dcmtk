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
 *  Module:  dcmpmap
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class managing various parametric myps-typical types
 *
 */

#ifndef DPMTYPES_H
#define DPMTYPES_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmpmap/dpmdef.h"

// ----------------------------------------------------------------------------
// Define the loggers for this module
// ----------------------------------------------------------------------------

extern DCMTK_DCMPMAP_EXPORT OFLogger DCM_dcmpmapLogger;

#define DCMPMAP_TRACE(msg) OFLOG_TRACE(DCM_dcmpmapLogger, msg)
#define DCMPMAP_DEBUG(msg) OFLOG_DEBUG(DCM_dcmpmapLogger, msg)
#define DCMPMAP_INFO(msg)  OFLOG_INFO(DCM_dcmpmapLogger, msg)
#define DCMPMAP_WARN(msg)  OFLOG_WARN(DCM_dcmpmapLogger, msg)
#define DCMPMAP_ERROR(msg) OFLOG_ERROR(DCM_dcmpmapLogger, msg)
#define DCMPMAP_FATAL(msg) OFLOG_FATAL(DCM_dcmpmapLogger, msg)

// ----------------------------------------------------------------------------
// Error constants
// ----------------------------------------------------------------------------

extern DCMTK_DCMPMAP_EXPORT   const OFConditionConst     DPM_InvalidDimensions;
extern DCMTK_DCMPMAP_EXPORT   const OFConditionConst     DPM_InvalidAttributeValue;
extern DCMTK_DCMPMAP_EXPORT   const OFConditionConst     DPM_InvalidPixelInfo;
extern DCMTK_DCMPMAP_EXPORT   const OFConditionConst     DPM_InvalidPixelData;
extern DCMTK_DCMPMAP_EXPORT   const OFConditionConst     DPM_NoPixelData;
extern DCMTK_DCMPMAP_EXPORT   const OFConditionConst     DPM_InvalidSOPClass;

/** General purpose class hiding global functions, constants and types in the
 *  parametric map context from the global namespace.
 */
class DCMTK_DCMPMAP_EXPORT DPMTypes
{
  public:

    /// Enum for attribute Content Qualification
    enum ContentQualification
    {
      /// Unkonwn (e.g. not set)
      CQ_UNKNOWN,
      /// PRODUCT
      CQ_PRODUCT,
      /// RESEARCH
      CQ_RESEARCH,
      /// SERVICE
      CQ_SERVICE
    };

    /// Enum for attribute Lossy Image Compression
    enum LossyImageCompression
    {
      /// Unknown (e.g. not set)
      COMP_UNKNOWN,
      /// YES (lossy image compression used)
      COMP_YES,
      /// NO (no lossy image compression used)
      COMP_NO
    };


    /// Enum for attribute Recognizable Visual Features
    enum RecognizableVisibleFeatures
    {
      /// Unknown (e.g. not set)
      RVF_UNKNOWN,
      /// YES (recognizable visual features present)
      RVF_YES,
      /// NO (recognizable visual features not present)
      RVF_NO
    };

    /** Convert Recognizable Visual Features enum to OFString
     *  @param  rvf The enum to convert
     *  @return String version of enum value
     */
    static OFString rvf2Str(const RecognizableVisibleFeatures rvf);

    /** Convert OFString version of Recognizable Visual Features attribute
     *  value to enum value
     *  @param  rvf The OFString value to convert
     *  @return The enum value of the string
     */
    static RecognizableVisibleFeatures str2Rvf(const OFString& rvf);

    /** Convert Content Qualification enum to OFString
     *  @param  cq The enum to convert
     *  @return String version of enum value
     */
    static OFString cq2Str(const ContentQualification cq);

    /** Convert OFString version of Content Qualification attribute
     *  value to enum value
     *  @param  cq The OFString value to convert
     *  @return The enum value of the string
     */
    static ContentQualification str2Cq(const OFString& cq);

    /** Check whether Recognizable Visual Features enum value is valid
     *  @param  rvf Value to check
     *  @return OFTrue if valid, OFFalse otherwise
     */
    static OFBool rvfValid(const RecognizableVisibleFeatures rvf);

    /** Check whether Content Qualification enum value is valid
     *  @param  cq Value to check
     *  @return OFTrue if valid, OFFalse otherwise
     */
    static OFBool cqValid(const ContentQualification cq);

};

#endif // DPMTYPES_H
