/*
 *
 *  Copyright (C) 2019, Open Connections GmbH
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
 *  Module:  dcmect
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for managing common Enhanced CT-specific types
 *
 */

#ifndef DCMECT_TYPES_H
#define DCMECT_TYPES_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmect/def.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/oflog/oflog.h"

/*-----------------------*
 * Logging               *
 *-----------------------*/

extern DCMTK_DCMECT_EXPORT OFLogger DCM_dcmectLogger;

#define DCMECT_TRACE(msg) OFLOG_TRACE(DCM_dcmectLogger, msg)
#define DCMECT_DEBUG(msg) OFLOG_DEBUG(DCM_dcmectLogger, msg)
#define DCMECT_INFO(msg) OFLOG_INFO(DCM_dcmectLogger, msg)
#define DCMECT_WARN(msg) OFLOG_WARN(DCM_dcmectLogger, msg)
#define DCMECT_ERROR(msg) OFLOG_ERROR(DCM_dcmectLogger, msg)
#define DCMECT_FATAL(msg) OFLOG_FATAL(DCM_dcmectLogger, msg)

// include this file in doxygen documentation

/** @file types.h
 *  @brief type definitions, constants and helper classes for the dcmect module
 */

/*----------------------*
 * Constant definitions *
 *----------------------*/

extern DCMTK_DCMECT_EXPORT const OFConditionConst ECT_InvalidDimensions;
extern DCMTK_DCMECT_EXPORT const OFConditionConst ECT_InvalidAttributeValue;
extern DCMTK_DCMECT_EXPORT const OFConditionConst ECT_InvalidPixelInfo;
extern DCMTK_DCMECT_EXPORT const OFConditionConst ECT_InvalidPixelData;
extern DCMTK_DCMECT_EXPORT const OFConditionConst ECT_NoPixelData;
extern DCMTK_DCMECT_EXPORT const OFConditionConst ECT_InvalidSOPClass;

/** General purpose class hiding global functions, constants and types in the
 *  Enhanced CT context from the global namespace.
 */
class DCMTK_DCMECT_EXPORT EctTypes
{

public:
    /// Enum containing the Enumerated Values for the first value of tag Image Type (0008,0008)
    enum E_ImageType1
    {
        /// Denotes an empty attribute value (not permitted but may be found when reading
        /// an invalid object)
        E_ImageType1_Empty,
        /// Denotes any invalid attribute value (not permitted but may be found when reading
        /// an invalid object)
        E_ImageType1_Invalid,
        /// Reflects the value "ORIGINAL"
        E_ImageType1_Original,
        /// Reflects the value "DERIVED"
        E_ImageType1_Derived,
        /// Reflects the value "MIXED"
        E_ImageType1_Mixed
    };

    /// Enum containing the Enumerated Values for the 2nd value of tag Image Type (0008,0008)
    enum E_ImageType2
    {
        /// Denotes an empty attribute value (not permitted but may be found when reading
        /// an invalid object)
        E_ImageType2_Empty,
        /// Denotes any invalid attribute value (not permitted but may be found when reading
        /// an invalid object)
        E_ImageType2_Invalid,
        /// Reflects the value "PRIMARY" which is the only valid value permitted
        E_ImageType2_Primary
    };

    /// Defined Term "ANGIO" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_Angio;
    /// Defined Term "CARDIAC" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_Cardiac;
    /// Defined Term "CARDIAC_GATED" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_CardiacGated;
    /// Defined Term "CARDRESP_GATED" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_CardRespGated;
    /// Defined Term "DYNAMIC" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_Dynamic;
    /// Defined Term "FLUOROSCOPY" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_Fluoroscopy;
    /// Defined Term "LOCALIZER" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_Localizer;
    /// Defined Term "MOTION" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_Motion;
    /// Defined Term "PERFUSION" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_Perfusion;
    /// Defined Term "PRE_CONTRAST" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_PreContrast;
    /// Defined Term "POST_CONTRAST" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_PostContrast;
    /// Defined Term "RESP_GATED" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_RespGated;
    /// Defined Term "REST" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_Rest;
    /// Defined Term "STATIC" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_Static;
    /// Defined Term "STRESS" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_Stress;
    /// Defined Term "VOLUME" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_Volume;
    /// Defined Term "NON_PARALLEL" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_NonParallel;
    /// Defined Term "WHOLE_BODY" for 3rd value of tag Image Type (0008,0008)
    static const OFString DT_ImageType3_WholeBody;

    /// Defined Term "ADDITION" for 4th value of tag Image Type (0008,0008)
    static const OFString DT_ImageType4_Addition;
    /// Defined Term "DIVISION" for 4th value of tag Image Type (0008,0008)
    static const OFString DT_ImageType4_Division;
    /// Defined Term "MASKED" for 4th value of tag Image Type (0008,0008)
    static const OFString DT_ImageType4_Masked;
    /// Defined Term "MAXIMUM" for 4th value of tag Image Type (0008,0008)
    static const OFString DT_ImageType4_Maximum;
    /// Defined Term "MEAN" for 4th value of tag Image Type (0008,0008)
    static const OFString DT_ImageType4_Mean;
    /// Defined Term "MINIMUM" for 4th value of tag Image Type (0008,0008)
    static const OFString DT_ImageType4_Minimum;
    /// Defined Term "MULTIPLICATION" for 4th value of tag Image Type (0008,0008)
    static const OFString DT_ImageType4_Multiplication;
    /// Defined Term "RESAMPLED" for 4th value of tag Image Type (0008,0008)
    static const OFString DT_ImageType4_Resampled;
    /// Defined Term "STD_DEVIATION" for 4th value of tag Image Type (0008,0008)
    static const OFString DT_ImageType4_Std_Deviation;
    /// Defined Term "SUBTRACTION" for 4th value of tag Image Type (0008,0008)
    static const OFString DT_ImageType4_Subtraction;
    /// Defined Term "NONE" for 4th value of tag Image Type (0008,0008)
    static const OFString DT_ImageType4_None;
    /// Defined Term "QUANTITY" for 4th value of tag Image Type (0008,0008)
    static const OFString DT_ImageType4_Quantity;
    /// Defined Term "MIXED" for 4th value of tag Image Type (0008,0008)
    static const OFString DT_ImageType4_Mixed;

    /// Enum containing the Enumerated Values of tag Content Qualification (0018,9004)
    enum E_ContentQualification
    {
        /// Denotes an empty attribute value (not permitted but may be found when reading
        /// an invalid object)
        E_ContentQuali_Empty,
        /// Denotes an invalid attribute value (not permitted but may be found when reading
        /// an invalid object)
        E_ContentQuali_Invalid,
        /// Reflects the value "PRODUCT" which is the only valid value permitted
        E_ContQuali_Product,
        /// Reflects the value "RESEARCH" which is the only valid value permitted
        E_ContQuali_Research,
        /// Reflects the value "SERVICE" which is the only valid value permitted
        E_ContQuali_Service
    };

    /// Enum containing the Enumerated Values of tag Burned In Annotation (0028,0301)
    enum E_BurnedInAnnotation
    {
        /// Denotes an empty attribute value (not permitted but may be found when reading
        /// an invalid object)
        E_BurnedInAnno_Empty,
        /// Denotes an invalid attribute value (not permitted but may be found when reading
        /// an invalid object)
        E_BurnedInAnno_Invalid,
        /// Reflects the value "NO" which is the only valid value permitted
        E_BurnedInAnno_No
    };

    /// Enum containing the Enumerated Values of tag Recognizable Visual Features (0028,0302)
    enum E_RecognizableVisualFeatures
    {
        /// Denotes an empty attribute value (not permitted but may be found when reading
        /// an invalid object)
        E_RecoVisFeatures_Empty,
        /// Denotes an invalid attribute value (not permitted but may be found when reading
        /// an invalid object)
        E_RecoVisFeatures_Invalid,
        /// Reflects the value "YES"
        E_RecoVisFeatures_Yes,
        /// Reflects the value "NO"
        E_RecoVisFeatures_No
    };

    /// Enum containing the Enumerated Values of tag Pixel Presentation (0008,9205)
    enum E_PixelPresentation
    {
        /// Denotes an empty attribute value (not permitted but may be found when reading
        /// an invalid object)
        E_PixelPres_Empty,
        /// Denotes an invalid attribute value (not permitted but may be found when reading
        /// an invalid object)
        E_PixelPres_Invalid,
        /// Reflects the value "COLOR"
        E_PixelPres_Color,
        /// Reflects the value "MONOCHROME"
        E_PixelPres_Monochrome,
        /// Reflects the value "MIXED"
        E_PixelPres_Mixed,
        /// Reflects the value "TRUE_COLOR"
        E_PixelPres_TrueColor
    };

    /// Enum containing the Enumerated Values of tag Volumetric Properties (0008,9206)
    enum E_VolumetricProperties
    {
        /// Denotes an empty attribute value (not permitted but may be found when reading
        /// an invalid object)
        E_VolProps_Empty,
        /// Denotes an invalid attribute value (not permitted but may be found when reading
        /// an invalid object)
        E_VolProps_Invalid,
        /// Reflects the value "VOLUME"
        E_VolProps_Volume,
        /// Reflects the value "SAMPLED"
        E_VolProps_Sampled,
        /// Reflects the value "DISTORTED"
        E_VolProps_Distorted,
        /// Reflects the value "MIXED"
        E_VolProps_Mixed
    };

    /// Defined Term "MAX_IP" for tag Volume Based Calculation Technique
    static const OFString DT_VolBasedCalcTechnique_MaxIp;
    /// Defined Term "MIN_IP" for tag Volume Based Calculation Technique
    static const OFString DT_VolBasedCalcTechnique_MinIp;
    /// Defined Term "VOLUME_RENDER" for tag Volume Based Calculation Technique
    static const OFString DT_VolBasedCalcTechnique_VolumeRender;
    /// Defined Term "SURFACE_RENDER" for tag Volume Based Calculation Technique
    static const OFString DT_VolBasedCalcTechnique_SurfaceRender;
    /// Defined Term "MPR" for tag Volume Based Calculation Technique
    static const OFString DT_VolBasedCalcTechnique_Mpr;
    /// Defined Term "CURVED_MPR" for tag Volume Based Calculation Technique
    static const OFString DT_VolBasedCalcTechnique_CurvedMpr;
    /// Defined Term "NONE" for tag Volume Based Calculation Technique
    static const OFString DT_VolBasedCalcTechnique_None;
    /// Defined Term "MIXED" for tag Volume Based Calculation Technique
    static const OFString DT_VolBasedCalcTechnique_Mixed;

    /** Returns related string for given enum value. Returns empty string for
     *  invalid or empty values.
     *  @param  value The enum value
     *  @return The related string value
     */
    static OFString imageType1ToStr(const E_ImageType1 value);

    /** Returns related string for given enum value. Returns empty string for
     *  invalid or empty values.
     *  @param  value The enum value
     *  @return The related string value
     */
    static OFString imageType2ToStr(const E_ImageType2 value);

    /** Returns related string for given enum value. Returns empty string for
     *  invalid or empty values.
     *  @param  value The enum value
     *  @return The related string value
     */
    static OFString contentQualiToStr(const E_ContentQualification value);

    /** Returns related string for given enum value. Returns empty string for
     *  invalid or empty values.
     *  @param  value The enum value
     *  @return The related string value
     */
    static OFString burnedAnnoToStr(const E_BurnedInAnnotation value);

    /** Returns related string for given enum value. Returns empty string for
     *  invalid or empty values.
     *  @param  value The enum value
     *  @return The related string value
     */
    static OFString pixelPresToStr(const E_PixelPresentation value);

    /** Returns related string for given enum value. Returns empty string for
     *  invalid or empty values.
     *  @param  value The enum value
     *  @return The related string value
     */
    static OFString volPropsToStr(const E_VolumetricProperties value);

    /** Returns related string for given enum value. Returns empty string for
     *  invalid or empty values.
     *  @param  value The enum value
     *  @return The related string value
     */
    static OFString recoVisFeaturesToStr(const E_RecognizableVisualFeatures value);
};

#endif // DCMECT_TYPES_H
