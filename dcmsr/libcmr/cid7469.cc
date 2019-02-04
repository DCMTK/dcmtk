/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID7469_GenericIntensityAndSizeMeasurements
 *
 *  Generated automatically from DICOM PS 3.16-2019a
 *  File created on 2019-02-04 12:56:41 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid7469.h"


// general information on CID 7469 (Generic Intensity and Size Measurements)
#define CONTEXT_GROUP_NUMBER  "7469"
#define CONTEXT_GROUP_VERSION "20141110"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.1003"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID7469_GenericIntensityAndSizeMeasurements::CodeList *CID7469_GenericIntensityAndSizeMeasurements::Codes = NULL;


CID7469_GenericIntensityAndSizeMeasurements::CID7469_GenericIntensityAndSizeMeasurements(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID7469_GenericIntensityAndSizeMeasurements::CID7469_GenericIntensityAndSizeMeasurements(const EnumType selectedValue,
                                                                                         const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID7469_GenericIntensityAndSizeMeasurements::selectValue(const EnumType selectedValue,
                                                                     const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID7469_GenericIntensityAndSizeMeasurements::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
                                                                        DSRCodedEntryValue *foundCodedEntry,
                                                                        const OFBool enhancedEncodingMode) const
{
    OFCondition result = SR_EC_CodedEntryNotInContextGroup;
    /* first, search for standard codes */
    CodeList::const_iterator iter = getCodes().begin();
    CodeList::const_iterator last = getCodes().end();
    /* iterate over coded entry list */
    while (iter != last)
    {
        /* if found, exit loop */
        if (searchForCodedEntry == iter->second)
        {
            /* return coded entry (if requested) */
            if (foundCodedEntry != NULL)
            {
                *foundCodedEntry = iter->second;
                /* also set enhanced encoding mode (if enabled) */
                if (!foundCodedEntry->isEmpty() && enhancedEncodingMode)
                    setEnhancedEncodingMode(*foundCodedEntry);
            }
            result = SR_EC_CodedEntryInStandardContextGroup;
            break;
        }
        ++iter;
    }
    /* if not, continue with extended codes */
    if (result.bad())
    {
        result = DSRContextGroup::findCodedEntry(searchForCodedEntry, foundCodedEntry);
        /* tbd: set "enhanced encoding mode" to mark a local/extended version? */
    }
    return result;
}


void CID7469_GenericIntensityAndSizeMeasurements::printCodes(STD_NAMESPACE ostream &stream) const
{
    /* print standard codes */
    stream << "Standard codes:" << OFendl;
    CodeList::const_iterator iter = getCodes().begin();
    CodeList::const_iterator last = getCodes().end();
    /* iterate over coded entry list */
    while (iter != last)
    {
        stream << "  ";
        /* print coded entry */
        DSRCodedEntryValue(iter->second).print(stream);
        stream << OFendl;
        ++iter;
    }
    /* print extended codes */
    DSRContextGroup::printCodes(stream);
}


// static functions

void CID7469_GenericIntensityAndSizeMeasurements::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID7469_GenericIntensityAndSizeMeasurements::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID7469_GenericIntensityAndSizeMeasurements::getCodedEntry(const EnumType value,
                                                                              const OFBool enhancedEncodingMode)
{
    DSRCodedEntryValue codedEntry;
    /* search for given enumerated value */
    CodeList::iterator iter = getCodes().find(value);
    /* if found, set the coded entry */
    if (iter != getCodes().end())
    {
        codedEntry = iter->second;
        /* also set enhanced encoding mode (if enabled) */
        if (!codedEntry.isEmpty() && enhancedEncodingMode)
            setEnhancedEncodingMode(codedEntry);
    }
    return codedEntry;
}


CID7469_GenericIntensityAndSizeMeasurements::CodeList &CID7469_GenericIntensityAndSizeMeasurements::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(NAcetylaspartate, DSRBasicCodedEntry("F-65C50", "SRT", "N-acetylaspartate")));
        Codes->insert(OFMake_pair(Citrate, DSRBasicCodedEntry("F-61080", "SRT", "Citrate")));
        Codes->insert(OFMake_pair(Choline, DSRBasicCodedEntry("F-61620", "SRT", "Choline")));
        Codes->insert(OFMake_pair(Creatine, DSRBasicCodedEntry("F-61380", "SRT", "Creatine")));
        Codes->insert(OFMake_pair(CreatineAndCholine, DSRBasicCodedEntry("113094", "DCM", "Creatine and Choline")));
        Codes->insert(OFMake_pair(Lactate, DSRBasicCodedEntry("F-61760", "SRT", "Lactate")));
        Codes->insert(OFMake_pair(Lipid, DSRBasicCodedEntry("F-63600", "SRT", "Lipid")));
        Codes->insert(OFMake_pair(LipidAndLactate, DSRBasicCodedEntry("113095", "DCM", "Lipid and Lactate")));
        Codes->insert(OFMake_pair(GlutamateAndGlutamine, DSRBasicCodedEntry("113080", "DCM", "Glutamate and glutamine")));
        Codes->insert(OFMake_pair(Glutamine, DSRBasicCodedEntry("F-64210", "SRT", "Glutamine")));
        Codes->insert(OFMake_pair(Tuarine, DSRBasicCodedEntry("F-64460", "SRT", "Tuarine")));
        Codes->insert(OFMake_pair(Inositol, DSRBasicCodedEntry("F-61A90", "SRT", "Inositol")));
        Codes->insert(OFMake_pair(CholinePerCreatineRatio, DSRBasicCodedEntry("113081", "DCM", "Choline/Creatine Ratio")));
        Codes->insert(OFMake_pair(NAcetylaspartatePerCreatineRatio, DSRBasicCodedEntry("113082", "DCM", "N-acetylaspartate/Creatine Ratio")));
        Codes->insert(OFMake_pair(NAcetylaspartatePerCholineRatio, DSRBasicCodedEntry("113083", "DCM", "N-acetylaspartate/Choline Ratio")));
        Codes->insert(OFMake_pair(CreatinePlusCholinePerCitrateRatio, DSRBasicCodedEntry("113096", "DCM", "Creatine+Choline/Citrate Ratio")));
        Codes->insert(OFMake_pair(T1, DSRBasicCodedEntry("113063", "DCM", "T1")));
        Codes->insert(OFMake_pair(T2, DSRBasicCodedEntry("113065", "DCM", "T2")));
        Codes->insert(OFMake_pair(T2Star, DSRBasicCodedEntry("113064", "DCM", "T2*")));
        Codes->insert(OFMake_pair(ProtonDensity, DSRBasicCodedEntry("113058", "DCM", "Proton Density")));
        Codes->insert(OFMake_pair(SpinTaggingPerfusionMRSignalIntensity, DSRBasicCodedEntry("110800", "DCM", "Spin Tagging Perfusion MR Signal Intensity")));
        Codes->insert(OFMake_pair(VelocityEncoded, DSRBasicCodedEntry("113070", "DCM", "Velocity encoded")));
        Codes->insert(OFMake_pair(TemperatureEncoded, DSRBasicCodedEntry("113067", "DCM", "Temperature encoded")));
        Codes->insert(OFMake_pair(ContrastAgentAngioMRSignalIntensity, DSRBasicCodedEntry("110801", "DCM", "Contrast Agent Angio MR Signal Intensity")));
        Codes->insert(OFMake_pair(TimeOfFlightAngioMRSignalIntensity, DSRBasicCodedEntry("110802", "DCM", "Time Of Flight Angio MR Signal Intensity")));
        Codes->insert(OFMake_pair(ProtonDensityWeightedMRSignalIntensity, DSRBasicCodedEntry("110803", "DCM", "Proton Density Weighted MR Signal Intensity")));
        Codes->insert(OFMake_pair(T1WeightedMRSignalIntensity, DSRBasicCodedEntry("110804", "DCM", "T1 Weighted MR Signal Intensity")));
        Codes->insert(OFMake_pair(T2WeightedMRSignalIntensity, DSRBasicCodedEntry("110805", "DCM", "T2 Weighted MR Signal Intensity")));
        Codes->insert(OFMake_pair(T2StarWeightedMRSignalIntensity, DSRBasicCodedEntry("110806", "DCM", "T2* Weighted MR Signal Intensity")));
        Codes->insert(OFMake_pair(DiffusionWeighted, DSRBasicCodedEntry("113043", "DCM", "Diffusion weighted")));
        Codes->insert(OFMake_pair(VolumetricDiffusionDxxComponent, DSRBasicCodedEntry("110810", "DCM", "Volumetric Diffusion Dxx Component")));
        Codes->insert(OFMake_pair(VolumetricDiffusionDxyComponent, DSRBasicCodedEntry("110811", "DCM", "Volumetric Diffusion Dxy Component")));
        Codes->insert(OFMake_pair(VolumetricDiffusionDxzComponent, DSRBasicCodedEntry("110812", "DCM", "Volumetric Diffusion Dxz Component")));
        Codes->insert(OFMake_pair(VolumetricDiffusionDyyComponent, DSRBasicCodedEntry("110813", "DCM", "Volumetric Diffusion Dyy Component")));
        Codes->insert(OFMake_pair(VolumetricDiffusionDyzComponent, DSRBasicCodedEntry("110814", "DCM", "Volumetric Diffusion Dyz Component")));
        Codes->insert(OFMake_pair(VolumetricDiffusionDzzComponent, DSRBasicCodedEntry("110815", "DCM", "Volumetric Diffusion Dzz Component")));
        Codes->insert(OFMake_pair(FractionalAnisotropy, DSRBasicCodedEntry("110808", "DCM", "Fractional Anisotropy")));
        Codes->insert(OFMake_pair(RelativeAnisotropy, DSRBasicCodedEntry("110809", "DCM", "Relative Anisotropy")));
        Codes->insert(OFMake_pair(VolumeRatio, DSRBasicCodedEntry("113288", "DCM", "Volume Ratio")));
        Codes->insert(OFMake_pair(ApparentDiffusionCoefficient, DSRBasicCodedEntry("113041", "DCM", "Apparent Diffusion Coefficient")));
        Codes->insert(OFMake_pair(DiffusionCoefficient, DSRBasicCodedEntry("113289", "DCM", "Diffusion Coefficient")));
        Codes->insert(OFMake_pair(MonoExponentialApparentDiffusionCoefficient, DSRBasicCodedEntry("113290", "DCM", "Mono-exponential Apparent Diffusion Coefficient")));
        Codes->insert(OFMake_pair(SlowDiffusionCoefficient, DSRBasicCodedEntry("113291", "DCM", "Slow Diffusion Coefficient")));
        Codes->insert(OFMake_pair(FastDiffusionCoefficient, DSRBasicCodedEntry("113292", "DCM", "Fast Diffusion Coefficient")));
        Codes->insert(OFMake_pair(FastDiffusionCoefficientFraction, DSRBasicCodedEntry("113293", "DCM", "Fast Diffusion Coefficient Fraction")));
        Codes->insert(OFMake_pair(KurtosisDiffusionCoefficient, DSRBasicCodedEntry("113294", "DCM", "Kurtosis Diffusion Coefficient")));
        Codes->insert(OFMake_pair(GammaDistributionScaleParameter, DSRBasicCodedEntry("113295", "DCM", "Gamma Distribution Scale Parameter")));
        Codes->insert(OFMake_pair(GammaDistributionShapeParameter, DSRBasicCodedEntry("113296", "DCM", "Gamma Distribution Shape Parameter")));
        Codes->insert(OFMake_pair(GammaDistributionMode, DSRBasicCodedEntry("113297", "DCM", "Gamma Distribution Mode")));
        Codes->insert(OFMake_pair(DistributedDiffusionCoefficient, DSRBasicCodedEntry("113298", "DCM", "Distributed Diffusion Coefficient")));
        Codes->insert(OFMake_pair(AnomalousExponentParameter, DSRBasicCodedEntry("113299", "DCM", "Anomalous Exponent Parameter")));
        Codes->insert(OFMake_pair(FieldMapMRSignalIntensity, DSRBasicCodedEntry("110807", "DCM", "Field Map MR Signal Intensity")));
        Codes->insert(OFMake_pair(T1WeightedDynamicContrastEnhancedMRSignalIntensity, DSRBasicCodedEntry("110816", "DCM", "T1 Weighted Dynamic Contrast Enhanced MR Signal Intensity")));
        Codes->insert(OFMake_pair(T2WeightedDynamicContrastEnhancedMRSignalIntensity, DSRBasicCodedEntry("110817", "DCM", "T2 Weighted Dynamic Contrast Enhanced MR Signal Intensity")));
        Codes->insert(OFMake_pair(T2StarWeightedDynamicContrastEnhancedMRSignalIntensity, DSRBasicCodedEntry("110818", "DCM", "T2* Weighted Dynamic Contrast Enhanced MR Signal Intensity")));
        Codes->insert(OFMake_pair(BloodOxygenationLevel, DSRBasicCodedEntry("110819", "DCM", "Blood Oxygenation Level")));
        Codes->insert(OFMake_pair(NuclearMedicineProjectionActivity, DSRBasicCodedEntry("110820", "DCM", "Nuclear Medicine Projection Activity")));
        Codes->insert(OFMake_pair(NuclearMedicineTomographicActivity, DSRBasicCodedEntry("110821", "DCM", "Nuclear Medicine Tomographic Activity")));
        Codes->insert(OFMake_pair(SpatialDisplacementXComponent, DSRBasicCodedEntry("110822", "DCM", "Spatial Displacement X Component")));
        Codes->insert(OFMake_pair(SpatialDisplacementYComponent, DSRBasicCodedEntry("110823", "DCM", "Spatial Displacement Y Component")));
        Codes->insert(OFMake_pair(SpatialDisplacementZComponent, DSRBasicCodedEntry("110824", "DCM", "Spatial Displacement Z Component")));
        Codes->insert(OFMake_pair(HemodynamicResistance, DSRBasicCodedEntry("110825", "DCM", "Hemodynamic Resistance")));
        Codes->insert(OFMake_pair(IndexedHemodynamicResistance, DSRBasicCodedEntry("110826", "DCM", "Indexed Hemodynamic Resistance")));
        Codes->insert(OFMake_pair(AttenuationCoefficient, DSRBasicCodedEntry("112031", "DCM", "Attenuation Coefficient")));
        Codes->insert(OFMake_pair(TissueVelocity, DSRBasicCodedEntry("110827", "DCM", "Tissue Velocity")));
        Codes->insert(OFMake_pair(FlowVelocity, DSRBasicCodedEntry("110828", "DCM", "Flow Velocity")));
        Codes->insert(OFMake_pair(PowerDoppler, DSRBasicCodedEntry("P0-02241", "SRT", "Power Doppler")));
        Codes->insert(OFMake_pair(FlowVariance, DSRBasicCodedEntry("110829", "DCM", "Flow Variance")));
        Codes->insert(OFMake_pair(Elasticity, DSRBasicCodedEntry("110830", "DCM", "Elasticity")));
        Codes->insert(OFMake_pair(Perfusion, DSRBasicCodedEntry("110831", "DCM", "Perfusion")));
        Codes->insert(OFMake_pair(SpeedOfSound, DSRBasicCodedEntry("110832", "DCM", "Speed of sound")));
        Codes->insert(OFMake_pair(UltrasoundAttenuation, DSRBasicCodedEntry("110833", "DCM", "Ultrasound Attenuation")));
        Codes->insert(OFMake_pair(StudentsTTest, DSRBasicCodedEntry("113068", "DCM", "Student's T-test")));
        Codes->insert(OFMake_pair(ZScore, DSRBasicCodedEntry("113071", "DCM", "Z-score")));
        Codes->insert(OFMake_pair(RCoefficient, DSRBasicCodedEntry("113057", "DCM", "R-Coefficient")));
        Codes->insert(OFMake_pair(R2Coefficient, DSRBasicCodedEntry("126220", "DCM", "R2-Coefficient")));
        Codes->insert(OFMake_pair(ChiSquare, DSRBasicCodedEntry("126221", "DCM", "Chi-square")));
        Codes->insert(OFMake_pair(DW, DSRBasicCodedEntry("126222", "DCM", "D-W")));
        Codes->insert(OFMake_pair(AIC, DSRBasicCodedEntry("126223", "DCM", "AIC")));
        Codes->insert(OFMake_pair(BIC, DSRBasicCodedEntry("126224", "DCM", "BIC")));
        Codes->insert(OFMake_pair(RGBRComponent, DSRBasicCodedEntry("110834", "DCM", "RGB R Component")));
        Codes->insert(OFMake_pair(RGBGComponent, DSRBasicCodedEntry("110835", "DCM", "RGB G Component")));
        Codes->insert(OFMake_pair(RGBBComponent, DSRBasicCodedEntry("110836", "DCM", "RGB B Component")));
        Codes->insert(OFMake_pair(YBR_FULLYComponent, DSRBasicCodedEntry("110837", "DCM", "YBR FULL Y Component")));
        Codes->insert(OFMake_pair(YBR_FULL_CBComponent, DSRBasicCodedEntry("110838", "DCM", "YBR FULL CB Component")));
        Codes->insert(OFMake_pair(YBR_FULL_CRComponent, DSRBasicCodedEntry("110839", "DCM", "YBR FULL CR Component")));
        Codes->insert(OFMake_pair(YBR_PARTIALYComponent, DSRBasicCodedEntry("110840", "DCM", "YBR PARTIAL Y Component")));
        Codes->insert(OFMake_pair(YBR_PARTIAL_CBComponent, DSRBasicCodedEntry("110841", "DCM", "YBR PARTIAL CB Component")));
        Codes->insert(OFMake_pair(YBR_PARTIAL_CRComponent, DSRBasicCodedEntry("110842", "DCM", "YBR PARTIAL CR Component")));
        Codes->insert(OFMake_pair(YBR_ICTYComponent, DSRBasicCodedEntry("110843", "DCM", "YBR ICT Y Component")));
        Codes->insert(OFMake_pair(YBR_ICT_CBComponent, DSRBasicCodedEntry("110844", "DCM", "YBR ICT CB Component")));
        Codes->insert(OFMake_pair(YBR_ICT_CRComponent, DSRBasicCodedEntry("110845", "DCM", "YBR ICT CR Component")));
        Codes->insert(OFMake_pair(YBR_RCTYComponent, DSRBasicCodedEntry("110846", "DCM", "YBR RCT Y Component")));
        Codes->insert(OFMake_pair(YBR_RCT_CBComponent, DSRBasicCodedEntry("110847", "DCM", "YBR RCT CB Component")));
        Codes->insert(OFMake_pair(YBR_RCT_CRComponent, DSRBasicCodedEntry("110848", "DCM", "YBR RCT CR Component")));
        Codes->insert(OFMake_pair(Echogenicity, DSRBasicCodedEntry("110849", "DCM", "Echogenicity")));
        Codes->insert(OFMake_pair(XRayAttenuation, DSRBasicCodedEntry("110850", "DCM", "X-Ray Attenuation")));
        Codes->insert(OFMake_pair(MRSignalIntensity, DSRBasicCodedEntry("110852", "DCM", "MR signal intensity")));
        Codes->insert(OFMake_pair(BinarySegmentation, DSRBasicCodedEntry("110853", "DCM", "Binary Segmentation")));
        Codes->insert(OFMake_pair(FractionalProbabilisticSegmentation, DSRBasicCodedEntry("110854", "DCM", "Fractional Probabilistic Segmentation")));
        Codes->insert(OFMake_pair(FractionalOccupancySegmentation, DSRBasicCodedEntry("110855", "DCM", "Fractional Occupancy Segmentation")));
        Codes->insert(OFMake_pair(R1, DSRBasicCodedEntry("126393", "DCM", "R1")));
        Codes->insert(OFMake_pair(R2, DSRBasicCodedEntry("126394", "DCM", "R2")));
        Codes->insert(OFMake_pair(R2Star, DSRBasicCodedEntry("126395", "DCM", "R2*")));
        Codes->insert(OFMake_pair(MagnetizationTransferRatio, DSRBasicCodedEntry("113098", "DCM", "Magnetization Transfer Ratio")));
        Codes->insert(OFMake_pair(MagneticSusceptibility, DSRBasicCodedEntry("126396", "DCM", "Magnetic Susceptibility")));
        Codes->insert(OFMake_pair(Ktrans, DSRBasicCodedEntry("126312", "DCM", "Ktrans")));
        Codes->insert(OFMake_pair(Kep, DSRBasicCodedEntry("126313", "DCM", "kep")));
        Codes->insert(OFMake_pair(Ve, DSRBasicCodedEntry("126314", "DCM", "ve")));
        Codes->insert(OFMake_pair(Tau_m, DSRBasicCodedEntry("126330", "DCM", "tau_m")));
        Codes->insert(OFMake_pair(Vp, DSRBasicCodedEntry("126331", "DCM", "vp")));
        Codes->insert(OFMake_pair(RegionalCerebralBloodFlow, DSRBasicCodedEntry("113055", "DCM", "Regional Cerebral Blood Flow")));
        Codes->insert(OFMake_pair(RegionalBloodFlow, DSRBasicCodedEntry("126390", "DCM", "Regional Blood Flow")));
        Codes->insert(OFMake_pair(RegionalCerebralBloodVolume, DSRBasicCodedEntry("113056", "DCM", "Regional Cerebral Blood Volume")));
        Codes->insert(OFMake_pair(RegionalBloodVolume, DSRBasicCodedEntry("126391", "DCM", "Regional Blood Volume")));
        Codes->insert(OFMake_pair(MeanTransitTime, DSRBasicCodedEntry("113052", "DCM", "Mean Transit Time")));
        Codes->insert(OFMake_pair(TimeToPeak, DSRBasicCodedEntry("113069", "DCM", "Time To Peak")));
        Codes->insert(OFMake_pair(OxygenExtractionFraction, DSRBasicCodedEntry("126392", "DCM", "Oxygen Extraction Fraction")));
        Codes->insert(OFMake_pair(Tmax, DSRBasicCodedEntry("113084", "DCM", "Tmax")));
        Codes->insert(OFMake_pair(IAUC, DSRBasicCodedEntry("126320", "DCM", "IAUC")));
        Codes->insert(OFMake_pair(IAUC60, DSRBasicCodedEntry("126321", "DCM", "IAUC60")));
        Codes->insert(OFMake_pair(IAUC90, DSRBasicCodedEntry("126322", "DCM", "IAUC90")));
        Codes->insert(OFMake_pair(IAUC180, DSRBasicCodedEntry("126323", "DCM", "IAUC180")));
        Codes->insert(OFMake_pair(IAUCBN, DSRBasicCodedEntry("126324", "DCM", "IAUCBN")));
        Codes->insert(OFMake_pair(IAUC60BN, DSRBasicCodedEntry("126325", "DCM", "IAUC60BN")));
        Codes->insert(OFMake_pair(IAUC90BN, DSRBasicCodedEntry("126326", "DCM", "IAUC90BN")));
        Codes->insert(OFMake_pair(IAUC180BN, DSRBasicCodedEntry("126327", "DCM", "IAUC180BN")));
        Codes->insert(OFMake_pair(TimeOfPeakConcentration, DSRBasicCodedEntry("126370", "DCM", "Time of Peak Concentration")));
        Codes->insert(OFMake_pair(TimeOfLeadingHalfPeakConcentration, DSRBasicCodedEntry("126372", "DCM", "Time of Leading Half-Peak Concentration")));
        Codes->insert(OFMake_pair(BolusArrivalTime, DSRBasicCodedEntry("126371", "DCM", "Bolus Arrival Time")));
        Codes->insert(OFMake_pair(TemporalDerivativeThreshold, DSRBasicCodedEntry("126374", "DCM", "Temporal Derivative Threshold")));
        Codes->insert(OFMake_pair(MaximumSlope, DSRBasicCodedEntry("126375", "DCM", "Maximum Slope")));
        Codes->insert(OFMake_pair(MaximumDifference, DSRBasicCodedEntry("126376", "DCM", "Maximum Difference")));
        Codes->insert(OFMake_pair(TracerConcentration, DSRBasicCodedEntry("126377", "DCM", "Tracer Concentration")));
        Codes->insert(OFMake_pair(StandardizedUptakeValue, DSRBasicCodedEntry("126400", "DCM", "Standardized Uptake Value")));
        Codes->insert(OFMake_pair(SUVbw, DSRBasicCodedEntry("126401", "DCM", "SUVbw")));
        Codes->insert(OFMake_pair(SUVlbm, DSRBasicCodedEntry("126402", "DCM", "SUVlbm")));
        Codes->insert(OFMake_pair(SUVlbmJames128, DSRBasicCodedEntry("126406", "DCM", "SUVlbm(James128)")));
        Codes->insert(OFMake_pair(SUVlbmJanma, DSRBasicCodedEntry("126405", "DCM", "SUVlbm(Janma)")));
        Codes->insert(OFMake_pair(SUVbsa, DSRBasicCodedEntry("126403", "DCM", "SUVbsa")));
        Codes->insert(OFMake_pair(SUVibw, DSRBasicCodedEntry("126404", "DCM", "SUVibw")));
        Codes->insert(OFMake_pair(AbsorbedDose, DSRBasicCodedEntry("128513", "DCM", "Absorbed Dose")));
        Codes->insert(OFMake_pair(EquivalentDose, DSRBasicCodedEntry("128512", "DCM", "Equivalent Dose")));
        Codes->insert(OFMake_pair(Fat, DSRBasicCodedEntry("T-D008A", "SRT", "Fat")));
        Codes->insert(OFMake_pair(FatFraction, DSRBasicCodedEntry("129100", "DCM", "Fat fraction")));
        Codes->insert(OFMake_pair(WaterPerFatInPhase, DSRBasicCodedEntry("129101", "DCM", "Water/fat in phase")));
        Codes->insert(OFMake_pair(WaterPerFatOutOfPhase, DSRBasicCodedEntry("129102", "DCM", "Water/fat out of phase")));
        Codes->insert(OFMake_pair(NegativeEnhancementIntegral, DSRBasicCodedEntry("113054", "DCM", "Negative enhancement integral")));
        Codes->insert(OFMake_pair(SignalChange, DSRBasicCodedEntry("113059", "DCM", "Signal change")));
        Codes->insert(OFMake_pair(SignalToNoise, DSRBasicCodedEntry("113060", "DCM", "Signal to noise")));
        Codes->insert(OFMake_pair(TimeCourseOfSignal, DSRBasicCodedEntry("113066", "DCM", "Time course of signal")));
        Codes->insert(OFMake_pair(Water, DSRBasicCodedEntry("C-10120", "SRT", "Water")));
        Codes->insert(OFMake_pair(WaterFraction, DSRBasicCodedEntry("129103", "DCM", "Water fraction")));
        Codes->insert(OFMake_pair(Length, DSRBasicCodedEntry("G-D7FE", "SRT", "Length")));
        Codes->insert(OFMake_pair(PathLength, DSRBasicCodedEntry("121211", "DCM", "Path length")));
        Codes->insert(OFMake_pair(Distance, DSRBasicCodedEntry("121206", "DCM", "Distance")));
        Codes->insert(OFMake_pair(Width, DSRBasicCodedEntry("G-A220", "SRT", "Width")));
        Codes->insert(OFMake_pair(Depth, DSRBasicCodedEntry("G-D785", "SRT", "Depth")));
        Codes->insert(OFMake_pair(Diameter, DSRBasicCodedEntry("M-02550", "SRT", "Diameter")));
        Codes->insert(OFMake_pair(LongAxis, DSRBasicCodedEntry("G-A185", "SRT", "Long Axis")));
        Codes->insert(OFMake_pair(ShortAxis, DSRBasicCodedEntry("G-A186", "SRT", "Short Axis")));
        Codes->insert(OFMake_pair(MajorAxis, DSRBasicCodedEntry("G-A193", "SRT", "Major Axis")));
        Codes->insert(OFMake_pair(MinorAxis, DSRBasicCodedEntry("G-A194", "SRT", "Minor Axis")));
        Codes->insert(OFMake_pair(PerpendicularAxis, DSRBasicCodedEntry("G-A195", "SRT", "Perpendicular Axis")));
        Codes->insert(OFMake_pair(Radius, DSRBasicCodedEntry("G-A196", "SRT", "Radius")));
        Codes->insert(OFMake_pair(Perimeter, DSRBasicCodedEntry("G-A197", "SRT", "Perimeter")));
        Codes->insert(OFMake_pair(Circumference, DSRBasicCodedEntry("M-02560", "SRT", "Circumference")));
        Codes->insert(OFMake_pair(DiameterOfCircumscribedCircle, DSRBasicCodedEntry("G-A198", "SRT", "Diameter of circumscribed circle")));
        Codes->insert(OFMake_pair(Height, DSRBasicCodedEntry("121207", "DCM", "Height")));
        Codes->insert(OFMake_pair(Maximum3DDiameterOfAMesh, DSRBasicCodedEntry("L0JK", "IBSI", "Maximum 3D Diameter of a Mesh")));
        Codes->insert(OFMake_pair(MajorAxisIn3DLength, DSRBasicCodedEntry("TDIC", "IBSI", "Major Axis in 3D Length")));
        Codes->insert(OFMake_pair(MinorAxisIn3DLength, DSRBasicCodedEntry("P9VJ", "IBSI", "Minor Axis in 3D Length")));
        Codes->insert(OFMake_pair(LeastAxisIn3DLength, DSRBasicCodedEntry("7J51", "IBSI", "Least Axis in 3D Length")));
        Codes->insert(OFMake_pair(Area, DSRBasicCodedEntry("G-A166", "SRT", "Area")));
        Codes->insert(OFMake_pair(AreaOfDefinedRegion, DSRBasicCodedEntry("G-A16A", "SRT", "Area of defined region")));
        Codes->insert(OFMake_pair(SurfaceAreaOfMesh, DSRBasicCodedEntry("C0JK", "IBSI", "Surface Area of Mesh")));
        Codes->insert(OFMake_pair(Volume, DSRBasicCodedEntry("G-D705", "SRT", "Volume")));
        Codes->insert(OFMake_pair(VolumeEstimatedFromSingle2DRegion, DSRBasicCodedEntry("121216", "DCM", "Volume estimated from single 2D region")));
        Codes->insert(OFMake_pair(VolumeEstimatedFromTwoNonCoplanar2DRegions, DSRBasicCodedEntry("121218", "DCM", "Volume estimated from two non-coplanar 2D regions")));
        Codes->insert(OFMake_pair(VolumeEstimatedFromThreeOrMoreNonCoplanar2DRegions, DSRBasicCodedEntry("121217", "DCM", "Volume estimated from three or more non-coplanar 2D regions")));
        Codes->insert(OFMake_pair(VolumeOfSphere, DSRBasicCodedEntry("121222", "DCM", "Volume of sphere")));
        Codes->insert(OFMake_pair(VolumeOfEllipsoid, DSRBasicCodedEntry("121221", "DCM", "Volume of ellipsoid")));
        Codes->insert(OFMake_pair(VolumeOfCircumscribedSphere, DSRBasicCodedEntry("121220", "DCM", "Volume of circumscribed sphere")));
        Codes->insert(OFMake_pair(VolumeOfBoundingThreeDimensionalRegion, DSRBasicCodedEntry("121219", "DCM", "Volume of bounding three dimensional region")));
        Codes->insert(OFMake_pair(VolumeOfMesh, DSRBasicCodedEntry("RNU0", "IBSI", "Volume of Mesh")));
        Codes->insert(OFMake_pair(VolumeFromVoxelSummation, DSRBasicCodedEntry("YEKZ", "IBSI", "Volume from Voxel Summation")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID7469_GenericIntensityAndSizeMeasurements::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
