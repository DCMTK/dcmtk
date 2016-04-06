/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID7469_GenericIntensityAndSizeMeasurements
 *
 *  Generated automatically from DICOM PS 3.16-2016b
 *  File created on 2016-04-06 16:26:59 by J. Riesmeier
 *
 */


#ifndef CMR_CID7469_H
#define CMR_CID7469_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"
#include "dcmtk/dcmsr/cmr/define.h"

#include "dcmtk/ofstd/ofmap.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Context Group:
 *  CID 7469 - Generic Intensity and Size Measurements.
 *  (type: extensible, version: 20141110)
 */
class DCMTK_CMR_EXPORT CID7469_GenericIntensityAndSizeMeasurements
  : public DSRContextGroup
{

  public:

    /** types of generic intensity and size measurements
     */
    enum EnumType
    {
        /// (F-65C50,SRT,"N-acetylaspartate"), included from CID 4033
        NAcetylaspartate,
        /// (F-61080,SRT,"Citrate"), included from CID 4033
        Citrate,
        /// (F-61620,SRT,"Choline"), included from CID 4033
        Choline,
        /// (F-61380,SRT,"Creatine"), included from CID 4033
        Creatine,
        /// (113094,DCM,"Creatine and Choline"), included from CID 4033
        CreatineAndCholine,
        /// (F-61760,SRT,"Lactate"), included from CID 4033
        Lactate,
        /// (F-63600,SRT,"Lipid"), included from CID 4033
        Lipid,
        /// (113095,DCM,"Lipid and Lactate"), included from CID 4033
        LipidAndLactate,
        /// (113080,DCM,"Glutamate and glutamine"), included from CID 4033
        GlutamateAndGlutamine,
        /// (F-64210,SRT,"Glutamine"), included from CID 4033
        Glutamine,
        /// (F-64460,SRT,"Tuarine"), included from CID 4033
        Tuarine,
        /// (F-61A90,SRT,"Inositol"), included from CID 4033
        Inositol,
        /// (113081,DCM,"Choline/Creatine Ratio"), included from CID 4033
        CholinePerCreatineRatio,
        /// (113082,DCM,"N-acetylaspartate/Creatine Ratio"), included from CID 4033
        NAcetylaspartatePerCreatineRatio,
        /// (113083,DCM,"N-acetylaspartate/Choline Ratio"), included from CID 4033
        NAcetylaspartatePerCholineRatio,
        /// (113096,DCM,"Creatine+Choline/Citrate Ratio"), included from CID 4033
        CreatinePlusCholinePerCitrateRatio,
        /// (113063,DCM,"T1"), included from CID 7180
        T1,
        /// (113065,DCM,"T2"), included from CID 7180
        T2,
        /// (113064,DCM,"T2*"), included from CID 7180
        T2Star,
        /// (113058,DCM,"Proton Density"), included from CID 7180
        ProtonDensity,
        /// (110800,DCM,"Spin Tagging Perfusion MR Signal Intensity"), included from CID 7180
        SpinTaggingPerfusionMRSignalIntensity,
        /// (113070,DCM,"Velocity encoded"), included from CID 7180
        VelocityEncoded,
        /// (113067,DCM,"Temperature encoded"), included from CID 7180
        TemperatureEncoded,
        /// (110801,DCM,"Contrast Agent Angio MR Signal Intensity"), included from CID 7180
        ContrastAgentAngioMRSignalIntensity,
        /// (110802,DCM,"Time Of Flight Angio MR Signal Intensity"), included from CID 7180
        TimeOfFlightAngioMRSignalIntensity,
        /// (110803,DCM,"Proton Density Weighted MR Signal Intensity"), included from CID 7180
        ProtonDensityWeightedMRSignalIntensity,
        /// (110804,DCM,"T1 Weighted MR Signal Intensity"), included from CID 7180
        T1WeightedMRSignalIntensity,
        /// (110805,DCM,"T2 Weighted MR Signal Intensity"), included from CID 7180
        T2WeightedMRSignalIntensity,
        /// (110806,DCM,"T2* Weighted MR Signal Intensity"), included from CID 7180
        T2StarWeightedMRSignalIntensity,
        /// (113043,DCM,"Diffusion weighted"), included from CID 7180
        DiffusionWeighted,
        /// (110807,DCM,"Field Map MR Signal Intensity"), included from CID 7180
        FieldMapMRSignalIntensity,
        /// (110808,DCM,"Fractional Anisotropy"), included from CID 7180
        FractionalAnisotropy,
        /// (110809,DCM,"Relative Anisotropy"), included from CID 7180
        RelativeAnisotropy,
        /// (113041,DCM,"Apparent Diffusion Coefficient"), included from CID 7180
        ApparentDiffusionCoefficient,
        /// (110810,DCM,"Volumetric Diffusion Dxx Component"), included from CID 7180
        VolumetricDiffusionDxxComponent,
        /// (110811,DCM,"Volumetric Diffusion Dxy Component"), included from CID 7180
        VolumetricDiffusionDxyComponent,
        /// (110812,DCM,"Volumetric Diffusion Dxz Component"), included from CID 7180
        VolumetricDiffusionDxzComponent,
        /// (110813,DCM,"Volumetric Diffusion Dyy Component"), included from CID 7180
        VolumetricDiffusionDyyComponent,
        /// (110814,DCM,"Volumetric Diffusion Dyz Component"), included from CID 7180
        VolumetricDiffusionDyzComponent,
        /// (110815,DCM,"Volumetric Diffusion Dzz Component"), included from CID 7180
        VolumetricDiffusionDzzComponent,
        /// (110816,DCM,"T1 Weighted Dynamic Contrast Enhanced MR Signal Intensity"), included from CID 7180
        T1WeightedDynamicContrastEnhancedMRSignalIntensity,
        /// (110817,DCM,"T2 Weighted Dynamic Contrast Enhanced MR Signal Intensity"), included from CID 7180
        T2WeightedDynamicContrastEnhancedMRSignalIntensity,
        /// (110818,DCM,"T2* Weighted Dynamic Contrast Enhanced MR Signal Intensity"), included from CID 7180
        T2StarWeightedDynamicContrastEnhancedMRSignalIntensity,
        /// (110819,DCM,"Blood Oxygenation Level"), included from CID 7180
        BloodOxygenationLevel,
        /// (110820,DCM,"Nuclear Medicine Projection Activity"), included from CID 7180
        NuclearMedicineProjectionActivity,
        /// (110821,DCM,"Nuclear Medicine Tomographic Activity"), included from CID 7180
        NuclearMedicineTomographicActivity,
        /// (110822,DCM,"Spatial Displacement X Component"), included from CID 7180
        SpatialDisplacementXComponent,
        /// (110823,DCM,"Spatial Displacement Y Component"), included from CID 7180
        SpatialDisplacementYComponent,
        /// (110824,DCM,"Spatial Displacement Z Component"), included from CID 7180
        SpatialDisplacementZComponent,
        /// (110825,DCM,"Hemodynamic Resistance"), included from CID 7180
        HemodynamicResistance,
        /// (110826,DCM,"Indexed Hemodynamic Resistance"), included from CID 7180
        IndexedHemodynamicResistance,
        /// (112031,DCM,"Attenuation Coefficient"), included from CID 7180
        AttenuationCoefficient,
        /// (110827,DCM,"Tissue Velocity"), included from CID 7180
        TissueVelocity,
        /// (110828,DCM,"Flow Velocity"), included from CID 7180
        FlowVelocity,
        /// (P0-02241,SRT,"Power Doppler"), included from CID 7180
        PowerDoppler,
        /// (110829,DCM,"Flow Variance"), included from CID 7180
        FlowVariance,
        /// (110830,DCM,"Elasticity"), included from CID 7180
        Elasticity,
        /// (110831,DCM,"Perfusion"), included from CID 7180
        Perfusion,
        /// (110832,DCM,"Speed of sound"), included from CID 7180
        SpeedOfSound,
        /// (110833,DCM,"Ultrasound Attenuation"), included from CID 7180
        UltrasoundAttenuation,
        /// (113068,DCM,"Student's T-test"), included from CID 7180
        StudentsTTest,
        /// (113071,DCM,"Z-score"), included from CID 7180
        ZScore,
        /// (113057,DCM,"R-Coefficient"), included from CID 7180
        RCoefficient,
        /// (126220,DCM,"R2-Coefficient"), included from CID 7180
        R2Coefficient,
        /// (110834,DCM,"RGB R Component"), included from CID 7180
        RGBRComponent,
        /// (110835,DCM,"RGB G Component"), included from CID 7180
        RGBGComponent,
        /// (110836,DCM,"RGB B Component"), included from CID 7180
        RGBBComponent,
        /// (110837,DCM,"YBR FULL Y Component"), included from CID 7180
        YBR_FULLYComponent,
        /// (110838,DCM,"YBR FULL CB Component"), included from CID 7180
        YBR_FULL_CBComponent,
        /// (110839,DCM,"YBR FULL CR Component"), included from CID 7180
        YBR_FULL_CRComponent,
        /// (110840,DCM,"YBR PARTIAL Y Component"), included from CID 7180
        YBR_PARTIALYComponent,
        /// (110841,DCM,"YBR PARTIAL CB Component"), included from CID 7180
        YBR_PARTIAL_CBComponent,
        /// (110842,DCM,"YBR PARTIAL CR Component"), included from CID 7180
        YBR_PARTIAL_CRComponent,
        /// (110843,DCM,"YBR ICT Y Component"), included from CID 7180
        YBR_ICTYComponent,
        /// (110844,DCM,"YBR ICT CB Component"), included from CID 7180
        YBR_ICT_CBComponent,
        /// (110845,DCM,"YBR ICT CR Component"), included from CID 7180
        YBR_ICT_CRComponent,
        /// (110846,DCM,"YBR RCT Y Component"), included from CID 7180
        YBR_RCTYComponent,
        /// (110847,DCM,"YBR RCT CB Component"), included from CID 7180
        YBR_RCT_CBComponent,
        /// (110848,DCM,"YBR RCT CR Component"), included from CID 7180
        YBR_RCT_CRComponent,
        /// (110849,DCM,"Echogenicity"), included from CID 7180
        Echogenicity,
        /// (110850,DCM,"X-Ray Attenuation"), included from CID 7180
        XRayAttenuation,
        /// (110852,DCM,"MR signal intensity"), included from CID 7180
        MRSignalIntensity,
        /// (110853,DCM,"Binary Segmentation"), included from CID 7180
        BinarySegmentation,
        /// (110854,DCM,"Fractional Probabilistic Segmentation"), included from CID 7180
        FractionalProbabilisticSegmentation,
        /// (110855,DCM,"Fractional Occupancy Segmentation"), included from CID 7180
        FractionalOccupancySegmentation,
        /// (126393,DCM,"R1"), included from CID 7180
        R1,
        /// (126394,DCM,"R2"), included from CID 7180
        R2,
        /// (113098,DCM,"Magnetization Transfer Ratio"), included from CID 7180
        MagnetizationTransferRatio,
        /// (126312,DCM,"Ktrans"), included from CID 4107
        Ktrans,
        /// (126313,DCM,"kep"), included from CID 4107
        Kep,
        /// (126314,DCM,"ve"), included from CID 4107
        Ve,
        /// (126330,DCM,"tau_m"), included from CID 4107
        Tau_m,
        /// (126331,DCM,"vp"), included from CID 4107
        Vp,
        /// (113055,DCM,"Regional Cerebral Blood Flow"), included from CID 4108
        RegionalCerebralBloodFlow,
        /// (126390,DCM,"Regional Blood Flow"), included from CID 4108
        RegionalBloodFlow,
        /// (113056,DCM,"Regional Cerebral Blood Volume"), included from CID 4108
        RegionalCerebralBloodVolume,
        /// (126391,DCM,"Regional Blood Volume"), included from CID 4108
        RegionalBloodVolume,
        /// (113052,DCM,"Mean Transit Time"), included from CID 4108
        MeanTransitTime,
        /// (113069,DCM,"Time To Peak"), included from CID 4108
        TimeToPeak,
        /// (126392,DCM,"Oxygen Extraction Fraction"), included from CID 4108
        OxygenExtractionFraction,
        /// (126320,DCM,"IAUC"), included from CID 4109
        IAUC,
        /// (126321,DCM,"IAUC60"), included from CID 4109
        IAUC60,
        /// (126322,DCM,"IAUC90"), included from CID 4109
        IAUC90,
        /// (126323,DCM,"IAUC180"), included from CID 4109
        IAUC180,
        /// (126324,DCM,"IAUCBN"), included from CID 4109
        IAUCBN,
        /// (126325,DCM,"IAUC60BN"), included from CID 4109
        IAUC60BN,
        /// (126326,DCM,"IAUC90BN"), included from CID 4109
        IAUC90BN,
        /// (126327,DCM,"IAUC180BN"), included from CID 4109
        IAUC180BN,
        /// (126370,DCM,"Time of Peak Concentration"), included from CID 4109
        TimeOfPeakConcentration,
        /// (126372,DCM,"Time of Leading Half-Peak Concentration"), included from CID 4109
        TimeOfLeadingHalfPeakConcentration,
        /// (126371,DCM,"Bolus Arrival Time"), included from CID 4109
        BolusArrivalTime,
        /// (126374,DCM,"Temporal Derivative Threshold"), included from CID 4109
        TemporalDerivativeThreshold,
        /// (126375,DCM,"Maximum Slope"), included from CID 4109
        MaximumSlope,
        /// (126376,DCM,"Maximum Difference"), included from CID 4109
        MaximumDifference,
        /// (126377,DCM,"Tracer Concentration"), included from CID 4109
        TracerConcentration,
        /// (126400,DCM,"Standardized Uptake Value"), included from CID 7180
        StandardizedUptakeValue,
        /// (126401,DCM,"SUVbw"), included from CID 7180
        SUVbw,
        /// (126402,DCM,"SUVlbm"), included from CID 7180
        SUVlbm,
        /// (126405,DCM,"SUVlbm(Janma)"), included from CID 7180
        SUVlbmJanma,
        /// (126403,DCM,"SUVbsa"), included from CID 7180
        SUVbsa,
        /// (126404,DCM,"SUVibw"), included from CID 7180
        SUVibw,
        /// (G-D7FE,SRT,"Length"), included from CID 7470
        Length,
        /// (121211,DCM,"Path length"), included from CID 7470
        PathLength,
        /// (121206,DCM,"Distance"), included from CID 7470
        Distance,
        /// (G-A220,SRT,"Width"), included from CID 7470
        Width,
        /// (G-D785,SRT,"Depth"), included from CID 7470
        Depth,
        /// (M-02550,SRT,"Diameter"), included from CID 7470
        Diameter,
        /// (G-A185,SRT,"Long Axis"), included from CID 7470
        LongAxis,
        /// (G-A186,SRT,"Short Axis"), included from CID 7470
        ShortAxis,
        /// (G-A193,SRT,"Major Axis"), included from CID 7470
        MajorAxis,
        /// (G-A194,SRT,"Minor Axis"), included from CID 7470
        MinorAxis,
        /// (G-A195,SRT,"Perpendicular Axis"), included from CID 7470
        PerpendicularAxis,
        /// (G-A196,SRT,"Radius"), included from CID 7470
        Radius,
        /// (G-A197,SRT,"Perimeter"), included from CID 7470
        Perimeter,
        /// (M-02560,SRT,"Circumference"), included from CID 7470
        Circumference,
        /// (G-A198,SRT,"Diameter of circumscribed circle"), included from CID 7470
        DiameterOfCircumscribedCircle,
        /// (121207,DCM,"Height"), included from CID 7470
        Height,
        /// (G-A166,SRT,"Area"), included from CID 7471
        Area,
        /// (G-A16A,SRT,"Area of defined region"), included from CID 7471
        AreaOfDefinedRegion,
        /// (G-D705,SRT,"Volume"), included from CID 7472
        Volume,
        /// (121216,DCM,"Volume estimated from single 2D region"), included from CID 7472
        VolumeEstimatedFromSingle2DRegion,
        /// (121218,DCM,"Volume estimated from two non-coplanar 2D regions"), included from CID 7472
        VolumeEstimatedFromTwoNonCoplanar2DRegions,
        /// (121217,DCM,"Volume estimated from three or more non-coplanar 2D regions"), included from CID 7472
        VolumeEstimatedFromThreeOrMoreNonCoplanar2DRegions,
        /// (121222,DCM,"Volume of sphere"), included from CID 7472
        VolumeOfSphere,
        /// (121221,DCM,"Volume of ellipsoid"), included from CID 7472
        VolumeOfEllipsoid,
        /// (121220,DCM,"Volume of circumscribed sphere"), included from CID 7472
        VolumeOfCircumscribedSphere,
        /// (121219,DCM,"Volume of bounding three dimensional region"), included from CID 7472
        VolumeOfBoundingThreeDimensionalRegion
    };

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID7469_GenericIntensityAndSizeMeasurements(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID7469_GenericIntensityAndSizeMeasurements(const EnumType selectedValue,
                                                const OFBool enhancedEncodingMode = OFFalse);

    /** select a coded entry given by its type as the current value
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition selectValue(const EnumType selectedValue,
                                    const OFBool enhancedEncodingMode = OFFalse);

  // --- static helper functions ---

    /** initialize this context group explicitly.  Internally, the list of standard coded
     *  entries is created and initialized by calling getCodes().
     ** @note This function can be called multiple times but in case of multi-threaded
     *        applications should be called at least once before any instance of this class
     *        is actually used.  For single-threaded applications, there is no need to call
     *        it since the initialization is done implicitly.
     */
    static void initialize();

    /** cleanup this context group, i.e.\ delete the internal list of standard coded entries.
     *  Usually, there is no need to call this method, but it might be useful in order to
     *  explicitly free the associated memory, e.g. when checking for memory leaks.  The list
     *  will be recreated automatically when needed (or when initialize() is called).
     */
    static void cleanup();

    /** map a given type to the associated coded entry
     ** @param  value                 type that should be mapped to a coded entry
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return coded entry that is associated with the given type
     */
    static DSRCodedEntryValue getCodedEntry(const EnumType value,
                                            const OFBool enhancedEncodingMode = OFFalse);

  // --- reintroduce methods from base class

     using DSRContextGroup::selectValue;
     using DSRContextGroup::findCodedEntry;


  protected:

    /// type used for storing and managing the coded entries
    typedef OFMap<EnumType, DSRBasicCodedEntry> CodeList;

    /** print details on coded entries that are contained in this context group.
     *  See DSRCodedEntryValue::print() for details of the coded entry output.
     ** @param  stream  stream to which the output should be printed
     */
    virtual void printCodes(STD_NAMESPACE ostream &stream) const;

    /** search for a given coded entry in this context group
     ** @param  searchForCodedEntry   coded entry to be searched for
     *  @param  foundCodedEntry       pointer to variable that will store the coded entry
     *                                (if found and pointer is not NULL)
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return result of the search process, also defines the type of the entry
     */
    virtual OFCondition findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
                                       DSRCodedEntryValue *foundCodedEntry,
                                       const OFBool enhancedEncodingMode = OFFalse) const;

  // --- static helper function ---

    /** get list of standard coded entries that are managed by this context group.
     *  Internally, the singleton pattern is used, so the list is initialized only once.
     *  Please note that this function is not thread-safe.  Therefore, the initialize()
     *  function can be called before any instance of this class is actually used.
     ** @return reference to list of coded entries managed by this context group
     */
    static CodeList &getCodes();

    /** set the "Enhanced Encoding Mode" for a given coded entry
     ** @param  codedEntryValue  coded entry for which the enhanced encoding mode is set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue);


  private:

    /// pointer to internal code list (use a static variable for singleton pattern)
    static CodeList *Codes;
};


/*-------------------*
 *  type definition  *
 *-------------------*/

// define short name for the context group class
typedef CID7469_GenericIntensityAndSizeMeasurements CMR_CID7469;


#endif
