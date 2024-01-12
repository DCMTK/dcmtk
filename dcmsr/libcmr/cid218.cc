/*
 *
 *  Copyright (C) 2015-2024, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID218_QuantitativeImageFeature
 *
 *  Generated automatically from DICOM PS 3.16-2023e
 *  File created on 2024-01-12 09:54:34 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid218.h"


// general information on CID 218 (Quantitative Image Feature)
#define CONTEXT_GROUP_NUMBER  "218"
#define CONTEXT_GROUP_VERSION "20200920"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.1269"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID218_QuantitativeImageFeature::CodeList *CID218_QuantitativeImageFeature::Codes = NULL;


CID218_QuantitativeImageFeature::CID218_QuantitativeImageFeature(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID218_QuantitativeImageFeature::CID218_QuantitativeImageFeature(const EnumType selectedValue,
                                                                 const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID218_QuantitativeImageFeature::selectValue(const EnumType selectedValue,
                                                         const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID218_QuantitativeImageFeature::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID218_QuantitativeImageFeature::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID218_QuantitativeImageFeature::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID218_QuantitativeImageFeature::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID218_QuantitativeImageFeature::getCodedEntry(const EnumType value,
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


CID218_QuantitativeImageFeature::CodeList &CID218_QuantitativeImageFeature::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(IntensityHistogramMean, DSRBasicCodedEntry("X6K6", "IBSI", "Intensity Histogram Mean")));
        Codes->insert(OFMake_pair(IntensityHistogramVariance, DSRBasicCodedEntry("CH89", "IBSI", "Intensity Histogram Variance")));
        Codes->insert(OFMake_pair(IntensityHistogramSkewness, DSRBasicCodedEntry("88K1", "IBSI", "Intensity Histogram Skewness")));
        Codes->insert(OFMake_pair(IntensityHistogramKurtosis, DSRBasicCodedEntry("C3I7", "IBSI", "Intensity Histogram Kurtosis")));
        Codes->insert(OFMake_pair(IntensityHistogramMedian, DSRBasicCodedEntry("WIFQ", "IBSI", "Intensity Histogram Median")));
        Codes->insert(OFMake_pair(IntensityHistogramMinimumGrayLevel, DSRBasicCodedEntry("1PR8", "IBSI", "Intensity Histogram Minimum Gray Level")));
        Codes->insert(OFMake_pair(IntensityHistogram10thPercentile, DSRBasicCodedEntry("GPMT", "IBSI", "Intensity Histogram 10th Percentile")));
        Codes->insert(OFMake_pair(IntensityHistogram90thPercentile, DSRBasicCodedEntry("OZ0C", "IBSI", "Intensity Histogram 90th Percentile")));
        Codes->insert(OFMake_pair(IntensityHistogramMaximumGrayLevel, DSRBasicCodedEntry("3NCY", "IBSI", "Intensity Histogram Maximum Gray Level")));
        Codes->insert(OFMake_pair(IntensityHistogramMode, DSRBasicCodedEntry("AMMC", "IBSI", "Intensity Histogram Mode")));
        Codes->insert(OFMake_pair(IntensityHistogramInterquartileRange, DSRBasicCodedEntry("WR0O", "IBSI", "Intensity Histogram Interquartile Range")));
        Codes->insert(OFMake_pair(IntensityHistogramRange, DSRBasicCodedEntry("5Z3W", "IBSI", "Intensity Histogram Range")));
        Codes->insert(OFMake_pair(IntensityHistogramMeanAbsoluteDeviation, DSRBasicCodedEntry("D2ZX", "IBSI", "Intensity Histogram Mean Absolute Deviation")));
        Codes->insert(OFMake_pair(IntensityHistogramRobustMeanAbsoluteDeviation, DSRBasicCodedEntry("WRZB", "IBSI", "Intensity Histogram Robust Mean Absolute Deviation")));
        Codes->insert(OFMake_pair(IntensityHistogramMedianAbsoluteDeviation, DSRBasicCodedEntry("4RNL", "IBSI", "Intensity Histogram Median Absolute Deviation")));
        Codes->insert(OFMake_pair(IntensityHistogramCoefficientOfVariation, DSRBasicCodedEntry("CWYJ", "IBSI", "Intensity Histogram Coefficient of Variation")));
        Codes->insert(OFMake_pair(IntensityHistogramQuartileCoefficientOfDispersion, DSRBasicCodedEntry("SLWD", "IBSI", "Intensity Histogram Quartile Coefficient of Dispersion")));
        Codes->insert(OFMake_pair(IntensityHistogramEntropy, DSRBasicCodedEntry("TLU2", "IBSI", "Intensity Histogram Entropy")));
        Codes->insert(OFMake_pair(IntensityHistogramUniformity, DSRBasicCodedEntry("BJ5W", "IBSI", "Intensity Histogram Uniformity")));
        Codes->insert(OFMake_pair(IntensityHistogramMaximumGradient, DSRBasicCodedEntry("12CE", "IBSI", "Intensity Histogram Maximum Gradient")));
        Codes->insert(OFMake_pair(IntensityHistogramMaximumGradientGrayLevel, DSRBasicCodedEntry("8E6O", "IBSI", "Intensity Histogram Maximum Gradient Gray Level")));
        Codes->insert(OFMake_pair(IntensityHistogramMinimumGradient, DSRBasicCodedEntry("VQB3", "IBSI", "Intensity Histogram Minimum Gradient")));
        Codes->insert(OFMake_pair(IntensityHistogramMinimumGradientGrayLevel, DSRBasicCodedEntry("RHQZ", "IBSI", "Intensity Histogram Minimum Gradient Gray Level")));
        Codes->insert(OFMake_pair(JointEntropyOfGLCM, DSRBasicCodedEntry("TU9B", "IBSI", "Joint Entropy of GLCM")));
        Codes->insert(OFMake_pair(RootAngularSecondMomentOfGLCM, DSRBasicCodedEntry("126061", "DCM", "Root Angular Second Moment of GLCM")));
        Codes->insert(OFMake_pair(InverseDifferenceMomentOfGLCM, DSRBasicCodedEntry("WF0Z", "IBSI", "Inverse Difference Moment of GLCM")));
        Codes->insert(OFMake_pair(ContrastOfGLCM, DSRBasicCodedEntry("ACUI", "IBSI", "Contrast of GLCM")));
        Codes->insert(OFMake_pair(DissimilarityOfGLCM, DSRBasicCodedEntry("8S9J", "IBSI", "Dissimilarity of GLCM")));
        Codes->insert(OFMake_pair(AngularSecondMomentOfGLCM, DSRBasicCodedEntry("8ZQL", "IBSI", "Angular Second Moment of GLCM")));
        Codes->insert(OFMake_pair(CorrelationOfGLCM, DSRBasicCodedEntry("NI2N", "IBSI", "Correlation of GLCM")));
        Codes->insert(OFMake_pair(JointMaximumOfGLCM, DSRBasicCodedEntry("GYBY", "IBSI", "Joint Maximum of GLCM")));
        Codes->insert(OFMake_pair(JointAverageOfGLCM, DSRBasicCodedEntry("60VM", "IBSI", "Joint Average of GLCM")));
        Codes->insert(OFMake_pair(JointVarianceOfGLCM, DSRBasicCodedEntry("UR99", "IBSI", "Joint Variance of GLCM")));
        Codes->insert(OFMake_pair(DifferenceAverageOfGLCM, DSRBasicCodedEntry("TF7R", "IBSI", "Difference Average of GLCM")));
        Codes->insert(OFMake_pair(DifferenceVarianceOfGLCM, DSRBasicCodedEntry("D3YU", "IBSI", "Difference Variance of GLCM")));
        Codes->insert(OFMake_pair(DifferenceEntropyOfGLCM, DSRBasicCodedEntry("NTRS", "IBSI", "Difference Entropy of GLCM")));
        Codes->insert(OFMake_pair(SumAverageOfGLCM, DSRBasicCodedEntry("ZGXS", "IBSI", "Sum Average of GLCM")));
        Codes->insert(OFMake_pair(SumVarianceOfGLCM, DSRBasicCodedEntry("OEEB", "IBSI", "Sum Variance of GLCM")));
        Codes->insert(OFMake_pair(SumEntropyOfGLCM, DSRBasicCodedEntry("P6QZ", "IBSI", "Sum Entropy of GLCM")));
        Codes->insert(OFMake_pair(InverseDifferenceOfGLCM, DSRBasicCodedEntry("IB1Z", "IBSI", "Inverse Difference of GLCM")));
        Codes->insert(OFMake_pair(NormalizedInverseDifferenceOfGLCM, DSRBasicCodedEntry("NDRX", "IBSI", "Normalized Inverse Difference of GLCM")));
        Codes->insert(OFMake_pair(NormalizedInverseDifferenceMomentOfGLCM, DSRBasicCodedEntry("1QCO", "IBSI", "Normalized Inverse Difference Moment of GLCM")));
        Codes->insert(OFMake_pair(InverseVarianceOfGLCM, DSRBasicCodedEntry("E8JP", "IBSI", "Inverse Variance of GLCM")));
        Codes->insert(OFMake_pair(AutocorrelationOfGLCM, DSRBasicCodedEntry("QWB0", "IBSI", "Autocorrelation of GLCM")));
        Codes->insert(OFMake_pair(ClusterTendencyOfGLCM, DSRBasicCodedEntry("DG8W", "IBSI", "Cluster Tendency of GLCM")));
        Codes->insert(OFMake_pair(ClusterShadeOfGLCM, DSRBasicCodedEntry("7NFM", "IBSI", "Cluster Shade of GLCM")));
        Codes->insert(OFMake_pair(ClusterProminenceOfGLCM, DSRBasicCodedEntry("AE86", "IBSI", "Cluster Prominence of GLCM")));
        Codes->insert(OFMake_pair(FirstMeasureOfInformationCorrelationOfGLCM, DSRBasicCodedEntry("R8DG", "IBSI", "First Measure of Information Correlation of GLCM")));
        Codes->insert(OFMake_pair(SecondMeasureOfInformationCorrelationOfGLCM, DSRBasicCodedEntry("JN9H", "IBSI", "Second Measure of Information Correlation of GLCM")));
        Codes->insert(OFMake_pair(ShortRunsEmphasis, DSRBasicCodedEntry("22OV", "IBSI", "Short Runs Emphasis")));
        Codes->insert(OFMake_pair(LongRunsEmphasis, DSRBasicCodedEntry("W4KF", "IBSI", "Long Runs Emphasis")));
        Codes->insert(OFMake_pair(LowGrayLevelRunEmphasis, DSRBasicCodedEntry("V3SW", "IBSI", "Low Gray Level Run Emphasis")));
        Codes->insert(OFMake_pair(HighGrayLevelRunEmphasis, DSRBasicCodedEntry("G3QZ", "IBSI", "High Gray Level Run Emphasis")));
        Codes->insert(OFMake_pair(ShortRunLowGrayLevelEmphasis, DSRBasicCodedEntry("HTZT", "IBSI", "Short Run Low Gray Level Emphasis")));
        Codes->insert(OFMake_pair(ShortRunHighGrayLevelEmphasis, DSRBasicCodedEntry("GD3A", "IBSI", "Short Run High Gray Level Emphasis")));
        Codes->insert(OFMake_pair(LongRunLowGrayLevelEmphasis, DSRBasicCodedEntry("IVPO", "IBSI", "Long Run Low Gray Level Emphasis")));
        Codes->insert(OFMake_pair(LongRunHighGrayLevelEmphasis, DSRBasicCodedEntry("3KUM", "IBSI", "Long Run High Gray Level Emphasis")));
        Codes->insert(OFMake_pair(GrayLevelNonuniformityInRuns, DSRBasicCodedEntry("R5YN", "IBSI", "Gray Level Nonuniformity in Runs")));
        Codes->insert(OFMake_pair(NormalizedGrayLevelNonuniformityInRuns, DSRBasicCodedEntry("OVBL", "IBSI", "Normalized Gray Level Nonuniformity in Runs")));
        Codes->insert(OFMake_pair(RunLengthNonuniformity, DSRBasicCodedEntry("W92Y", "IBSI", "Run Length Nonuniformity")));
        Codes->insert(OFMake_pair(NormalizedRunLengthNonuniformity, DSRBasicCodedEntry("IC23", "IBSI", "Normalized Run Length Nonuniformity")));
        Codes->insert(OFMake_pair(RunPercentage, DSRBasicCodedEntry("9ZK5", "IBSI", "Run Percentage")));
        Codes->insert(OFMake_pair(GrayLevelVarianceInRuns, DSRBasicCodedEntry("8CE5", "IBSI", "Gray Level Variance in Runs")));
        Codes->insert(OFMake_pair(RunLengthVariance, DSRBasicCodedEntry("SXLW", "IBSI", "Run Length Variance")));
        Codes->insert(OFMake_pair(RunEntropy, DSRBasicCodedEntry("HJ9O", "IBSI", "Run Entropy")));
        Codes->insert(OFMake_pair(SmallZoneEmphasis, DSRBasicCodedEntry("5QRC", "IBSI", "Small Zone Emphasis")));
        Codes->insert(OFMake_pair(LargeZoneEmphasis, DSRBasicCodedEntry("48P8", "IBSI", "Large Zone Emphasis")));
        Codes->insert(OFMake_pair(LowGrayLevelZoneEmphasis, DSRBasicCodedEntry("XMSY", "IBSI", "Low Gray Level Zone Emphasis")));
        Codes->insert(OFMake_pair(HighGrayLevelZoneEmphasis, DSRBasicCodedEntry("5GN9", "IBSI", "High Gray Level Zone Emphasis")));
        Codes->insert(OFMake_pair(SmallZoneLowGrayLevelEmphasis, DSRBasicCodedEntry("5RAI", "IBSI", "Small Zone Low Gray Level Emphasis")));
        Codes->insert(OFMake_pair(SmallZoneHighGrayLevelEmphasis, DSRBasicCodedEntry("HW1V", "IBSI", "Small Zone High Gray Level Emphasis")));
        Codes->insert(OFMake_pair(LargeZoneLowGrayLevelEmphasis, DSRBasicCodedEntry("YH51", "IBSI", "Large Zone Low Gray Level Emphasis")));
        Codes->insert(OFMake_pair(LargeZoneHighGrayLevelEmphasis, DSRBasicCodedEntry("J17V", "IBSI", "Large Zone High Gray Level Emphasis")));
        Codes->insert(OFMake_pair(GrayLevelNonuniformityOfSizeZoneCounts, DSRBasicCodedEntry("JNSA", "IBSI", "Gray Level Nonuniformity of Size Zone Counts")));
        Codes->insert(OFMake_pair(NormalizedGrayLevelNonuniformityOfSizeZoneCounts, DSRBasicCodedEntry("Y1RO", "IBSI", "Normalized Gray Level Nonuniformity of Size Zone Counts")));
        Codes->insert(OFMake_pair(ZoneSizeNonuniformity, DSRBasicCodedEntry("4JP3", "IBSI", "Zone Size Nonuniformity")));
        Codes->insert(OFMake_pair(NormalizedZoneSizeNonuniformity, DSRBasicCodedEntry("VB3A", "IBSI", "Normalized Zone Size Nonuniformity")));
        Codes->insert(OFMake_pair(SizeZonePercentage, DSRBasicCodedEntry("P30P", "IBSI", "Size Zone Percentage")));
        Codes->insert(OFMake_pair(GrayLevelVarianceInSizeZones, DSRBasicCodedEntry("BYLV", "IBSI", "Gray Level Variance in Size Zones")));
        Codes->insert(OFMake_pair(ZoneSizeVariance, DSRBasicCodedEntry("3NSA", "IBSI", "Zone Size Variance")));
        Codes->insert(OFMake_pair(ZoneSizeEntropy, DSRBasicCodedEntry("GU8N", "IBSI", "Zone Size Entropy")));
        Codes->insert(OFMake_pair(SmallDistanceEmphasis, DSRBasicCodedEntry("0GBI", "IBSI", "Small Distance Emphasis")));
        Codes->insert(OFMake_pair(LargeDistanceEmphasis, DSRBasicCodedEntry("MB4I", "IBSI", "Large Distance Emphasis")));
        Codes->insert(OFMake_pair(LowGreyLevelZoneEmphasis, DSRBasicCodedEntry("S1RA", "IBSI", "Low Grey Level Zone Emphasis")));
        Codes->insert(OFMake_pair(HighGreyLevelZoneEmphasis, DSRBasicCodedEntry("K26C", "IBSI", "High Grey Level Zone Emphasis")));
        Codes->insert(OFMake_pair(SmallDistanceLowGreyLevelEmphasis, DSRBasicCodedEntry("RUVG", "IBSI", "Small Distance Low Grey Level Emphasis")));
        Codes->insert(OFMake_pair(SmallDistanceHighGreyLevelEmphasis, DSRBasicCodedEntry("DKNJ", "IBSI", "Small Distance High Grey Level Emphasis")));
        Codes->insert(OFMake_pair(LargeDistanceLowGreyLevelEmphasis, DSRBasicCodedEntry("A7WM", "IBSI", "Large Distance Low Grey Level Emphasis")));
        Codes->insert(OFMake_pair(LargeDistanceHighGreyLevelEmphasis, DSRBasicCodedEntry("KLTH", "IBSI", "Large Distance High Grey Level Emphasis")));
        Codes->insert(OFMake_pair(GreyLevelNonUniformityOfDistanceZoneCounts, DSRBasicCodedEntry("VFT7", "IBSI", "Grey Level Non-uniformity of Distance Zone Counts")));
        Codes->insert(OFMake_pair(NormalizedGreyLevelNonUniformityOfDistanceZoneCounts, DSRBasicCodedEntry("7HP3", "IBSI", "Normalized Grey Level Non-uniformity of Distance Zone Counts")));
        Codes->insert(OFMake_pair(ZoneDistanceNonUniformity, DSRBasicCodedEntry("V294", "IBSI", "Zone Distance Non-uniformity")));
        Codes->insert(OFMake_pair(NormalizedZoneDistanceNonUniformity, DSRBasicCodedEntry("IATH", "IBSI", "Normalized Zone Distance Non-uniformity")));
        Codes->insert(OFMake_pair(DistanceZonePercentage, DSRBasicCodedEntry("VIWW", "IBSI", "Distance Zone Percentage")));
        Codes->insert(OFMake_pair(GreyLevelVarianceInDistanceZones, DSRBasicCodedEntry("QK93", "IBSI", "Grey Level Variance in Distance Zones")));
        Codes->insert(OFMake_pair(ZoneDistanceVariance, DSRBasicCodedEntry("7WT1", "IBSI", "Zone Distance Variance")));
        Codes->insert(OFMake_pair(ZoneDistanceEntropy, DSRBasicCodedEntry("GBDU", "IBSI", "Zone Distance Entropy")));
        Codes->insert(OFMake_pair(CoarsenessOfNGTDM, DSRBasicCodedEntry("QCDE", "IBSI", "Coarseness of NGTDM")));
        Codes->insert(OFMake_pair(ContrastOfNGTDM, DSRBasicCodedEntry("65HE", "IBSI", "Contrast of NGTDM")));
        Codes->insert(OFMake_pair(BusynessOfNGTDM, DSRBasicCodedEntry("NQ30", "IBSI", "Busyness of NGTDM")));
        Codes->insert(OFMake_pair(ComplexityOfNGTDM, DSRBasicCodedEntry("HDEZ", "IBSI", "Complexity of NGTDM")));
        Codes->insert(OFMake_pair(StrengthOfNGTDM, DSRBasicCodedEntry("1X9X", "IBSI", "Strength of NGTDM")));
        Codes->insert(OFMake_pair(LowDependenceEmphasis, DSRBasicCodedEntry("SODN", "IBSI", "Low dependence emphasis")));
        Codes->insert(OFMake_pair(HighDependenceEmphasis, DSRBasicCodedEntry("IMOQ", "IBSI", "High dependence emphasis")));
        Codes->insert(OFMake_pair(LowGreyLevelCountEmphasis, DSRBasicCodedEntry("TL9H", "IBSI", "Low grey level count emphasis")));
        Codes->insert(OFMake_pair(HighGreyLevelCountEmphasis, DSRBasicCodedEntry("OAE7", "IBSI", "High grey level count emphasis")));
        Codes->insert(OFMake_pair(LowDependenceLowGreyLevelEmphasis, DSRBasicCodedEntry("EQ3F", "IBSI", "Low dependence low grey level emphasis")));
        Codes->insert(OFMake_pair(LowDependenceHighGreyLevelEmphasis, DSRBasicCodedEntry("JA6D", "IBSI", "Low dependence high grey level emphasis")));
        Codes->insert(OFMake_pair(HighDependenceLowGreyLevelEmphasis, DSRBasicCodedEntry("NBZI", "IBSI", "High dependence low grey level emphasis")));
        Codes->insert(OFMake_pair(HighDependenceHighGreyLevelEmphasis, DSRBasicCodedEntry("9QMG", "IBSI", "High dependence high grey level emphasis")));
        Codes->insert(OFMake_pair(GreyLevelNonUniformityOfNGLDM, DSRBasicCodedEntry("FP8K", "IBSI", "Grey level non-uniformity of NGLDM")));
        Codes->insert(OFMake_pair(NormalizedGreyLevelNonUniformityOfNGLDM, DSRBasicCodedEntry("5SPA", "IBSI", "Normalized grey level non-uniformity of NGLDM")));
        Codes->insert(OFMake_pair(DependenceCountNonUniformity, DSRBasicCodedEntry("Z87G", "IBSI", "Dependence count non-uniformity")));
        Codes->insert(OFMake_pair(DependenceCountNonUniformityNormalized, DSRBasicCodedEntry("OKJI", "IBSI", "Dependence count non-uniformity normalized")));
        Codes->insert(OFMake_pair(DependenceCountPercentage, DSRBasicCodedEntry("6XV8", "IBSI", "Dependence count percentage")));
        Codes->insert(OFMake_pair(GreyLevelVarianceOfNGLDM, DSRBasicCodedEntry("1PFV", "IBSI", "Grey level variance of NGLDM")));
        Codes->insert(OFMake_pair(DependenceCountVariance, DSRBasicCodedEntry("DNX2", "IBSI", "Dependence count variance")));
        Codes->insert(OFMake_pair(DependenceCountEntropy, DSRBasicCodedEntry("FCBV", "IBSI", "Dependence count entropy")));
        Codes->insert(OFMake_pair(DependenceCountEnergy, DSRBasicCodedEntry("CAS9", "IBSI", "Dependence count energy")));
        Codes->insert(OFMake_pair(FractalDimension, DSRBasicCodedEntry("126050", "DCM", "Fractal Dimension")));
        Codes->insert(OFMake_pair(NAcetylaspartate, DSRBasicCodedEntry("115391007", "SCT", "N-acetylaspartate")));
        Codes->insert(OFMake_pair(Citrate, DSRBasicCodedEntry("59351004", "SCT", "Citrate")));
        Codes->insert(OFMake_pair(Choline, DSRBasicCodedEntry("65123005", "SCT", "Choline")));
        Codes->insert(OFMake_pair(Creatine, DSRBasicCodedEntry("14804005", "SCT", "Creatine")));
        Codes->insert(OFMake_pair(CreatineAndCholine, DSRBasicCodedEntry("113094", "DCM", "Creatine and Choline")));
        Codes->insert(OFMake_pair(Lactate, DSRBasicCodedEntry("83036002", "SCT", "Lactate")));
        Codes->insert(OFMake_pair(Lipid, DSRBasicCodedEntry("70106000", "SCT", "Lipid")));
        Codes->insert(OFMake_pair(LipidAndLactate, DSRBasicCodedEntry("113095", "DCM", "Lipid and Lactate")));
        Codes->insert(OFMake_pair(GlutamateAndGlutamine, DSRBasicCodedEntry("113080", "DCM", "Glutamate and glutamine")));
        Codes->insert(OFMake_pair(Glutamine, DSRBasicCodedEntry("25761002", "SCT", "Glutamine")));
        Codes->insert(OFMake_pair(Tuarine, DSRBasicCodedEntry("10944007", "SCT", "Tuarine")));
        Codes->insert(OFMake_pair(Inositol, DSRBasicCodedEntry("72164009", "SCT", "Inositol")));
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
        Codes->insert(OFMake_pair(PowerDoppler, DSRBasicCodedEntry("425704008", "SCT", "Power Doppler")));
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
        Codes->insert(OFMake_pair(AbsoluteRegionalBloodFlow, DSRBasicCodedEntry("126390", "DCM", "Absolute Regional Blood Flow")));
        Codes->insert(OFMake_pair(AbsoluteRegionalBloodVolume, DSRBasicCodedEntry("126391", "DCM", "Absolute Regional Blood Volume")));
        Codes->insert(OFMake_pair(RelativeRegionalBloodFlow, DSRBasicCodedEntry("126397", "DCM", "Relative Regional Blood Flow")));
        Codes->insert(OFMake_pair(RelativeRegionalBloodVolume, DSRBasicCodedEntry("126398", "DCM", "Relative Regional Blood Volume")));
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
        Codes->insert(OFMake_pair(Fat, DSRBasicCodedEntry("256674009", "SCT", "Fat")));
        Codes->insert(OFMake_pair(FatFraction, DSRBasicCodedEntry("129100", "DCM", "Fat fraction")));
        Codes->insert(OFMake_pair(WaterPerFatInPhase, DSRBasicCodedEntry("129101", "DCM", "Water/fat in phase")));
        Codes->insert(OFMake_pair(WaterPerFatOutOfPhase, DSRBasicCodedEntry("129102", "DCM", "Water/fat out of phase")));
        Codes->insert(OFMake_pair(NegativeEnhancementIntegral, DSRBasicCodedEntry("113054", "DCM", "Negative enhancement integral")));
        Codes->insert(OFMake_pair(SignalChange, DSRBasicCodedEntry("113059", "DCM", "Signal change")));
        Codes->insert(OFMake_pair(SignalToNoise, DSRBasicCodedEntry("113060", "DCM", "Signal to noise")));
        Codes->insert(OFMake_pair(TimeCourseOfSignal, DSRBasicCodedEntry("113066", "DCM", "Time course of signal")));
        Codes->insert(OFMake_pair(Water, DSRBasicCodedEntry("11713004", "SCT", "Water")));
        Codes->insert(OFMake_pair(WaterFraction, DSRBasicCodedEntry("129103", "DCM", "Water fraction")));
        Codes->insert(OFMake_pair(RelativeLinearStoppingPower, DSRBasicCodedEntry("130086", "DCM", "Relative Linear Stopping Power")));
        Codes->insert(OFMake_pair(ClassActivation, DSRBasicCodedEntry("130402", "DCM", "Class activation")));
        Codes->insert(OFMake_pair(GradientWeightedClassActivation, DSRBasicCodedEntry("130403", "DCM", "Gradient-weighted class activation")));
        Codes->insert(OFMake_pair(Saliency, DSRBasicCodedEntry("130404", "DCM", "Saliency")));
        Codes->insert(OFMake_pair(Length, DSRBasicCodedEntry("410668003", "SCT", "Length")));
        Codes->insert(OFMake_pair(PathLength, DSRBasicCodedEntry("121211", "DCM", "Path length")));
        Codes->insert(OFMake_pair(Distance, DSRBasicCodedEntry("121206", "DCM", "Distance")));
        Codes->insert(OFMake_pair(Width, DSRBasicCodedEntry("103355008", "SCT", "Width")));
        Codes->insert(OFMake_pair(Depth, DSRBasicCodedEntry("131197000", "SCT", "Depth")));
        Codes->insert(OFMake_pair(Diameter, DSRBasicCodedEntry("81827009", "SCT", "Diameter")));
        Codes->insert(OFMake_pair(LongAxis, DSRBasicCodedEntry("103339001", "SCT", "Long Axis")));
        Codes->insert(OFMake_pair(ShortAxis, DSRBasicCodedEntry("103340004", "SCT", "Short Axis")));
        Codes->insert(OFMake_pair(MajorAxis, DSRBasicCodedEntry("131187009", "SCT", "Major Axis")));
        Codes->insert(OFMake_pair(MinorAxis, DSRBasicCodedEntry("131188004", "SCT", "Minor Axis")));
        Codes->insert(OFMake_pair(PerpendicularAxis, DSRBasicCodedEntry("131189007", "SCT", "Perpendicular Axis")));
        Codes->insert(OFMake_pair(Radius, DSRBasicCodedEntry("131190003", "SCT", "Radius")));
        Codes->insert(OFMake_pair(Perimeter, DSRBasicCodedEntry("131191004", "SCT", "Perimeter")));
        Codes->insert(OFMake_pair(Circumference, DSRBasicCodedEntry("74551000", "SCT", "Circumference")));
        Codes->insert(OFMake_pair(DiameterOfCircumscribedCircle, DSRBasicCodedEntry("131192006", "SCT", "Diameter of circumscribed circle")));
        Codes->insert(OFMake_pair(Height, DSRBasicCodedEntry("121207", "DCM", "Height")));
        Codes->insert(OFMake_pair(LineSegmentLength, DSRBasicCodedEntry("121227", "DCM", "Line segment length")));
        Codes->insert(OFMake_pair(Maximum3DDiameterOfAMesh, DSRBasicCodedEntry("L0JK", "IBSI", "Maximum 3D Diameter of a Mesh")));
        Codes->insert(OFMake_pair(MajorAxisIn3DLength, DSRBasicCodedEntry("TDIC", "IBSI", "Major Axis in 3D Length")));
        Codes->insert(OFMake_pair(MinorAxisIn3DLength, DSRBasicCodedEntry("P9VJ", "IBSI", "Minor Axis in 3D Length")));
        Codes->insert(OFMake_pair(LeastAxisIn3DLength, DSRBasicCodedEntry("7J51", "IBSI", "Least Axis in 3D Length")));
        Codes->insert(OFMake_pair(Area, DSRBasicCodedEntry("42798000", "SCT", "Area")));
        Codes->insert(OFMake_pair(AreaOfDefinedRegion, DSRBasicCodedEntry("131184002", "SCT", "Area of defined region")));
        Codes->insert(OFMake_pair(SurfaceAreaOfMesh, DSRBasicCodedEntry("C0JK", "IBSI", "Surface Area of Mesh")));
        Codes->insert(OFMake_pair(Volume, DSRBasicCodedEntry("118565006", "SCT", "Volume")));
        Codes->insert(OFMake_pair(VolumeEstimatedFromSingle2DRegion, DSRBasicCodedEntry("121216", "DCM", "Volume estimated from single 2D region")));
        Codes->insert(OFMake_pair(VolumeEstimatedFromTwoNonCoplanar2DRegions, DSRBasicCodedEntry("121218", "DCM", "Volume estimated from two non-coplanar 2D regions")));
        Codes->insert(OFMake_pair(VolumeEstimatedFromThreeOrMoreNonCoplanar2DRegions, DSRBasicCodedEntry("121217", "DCM", "Volume estimated from three or more non-coplanar 2D regions")));
        Codes->insert(OFMake_pair(VolumeOfSphere, DSRBasicCodedEntry("121222", "DCM", "Volume of sphere")));
        Codes->insert(OFMake_pair(VolumeOfEllipsoid, DSRBasicCodedEntry("121221", "DCM", "Volume of ellipsoid")));
        Codes->insert(OFMake_pair(VolumeOfCircumscribedSphere, DSRBasicCodedEntry("121220", "DCM", "Volume of circumscribed sphere")));
        Codes->insert(OFMake_pair(VolumeOfBoundingThreeDimensionalRegion, DSRBasicCodedEntry("121219", "DCM", "Volume of bounding three dimensional region")));
        Codes->insert(OFMake_pair(VolumeOfMesh, DSRBasicCodedEntry("RNU0", "IBSI", "Volume of Mesh")));
        Codes->insert(OFMake_pair(VolumeFromVoxelSummation, DSRBasicCodedEntry("YEKZ", "IBSI", "Volume from Voxel Summation")));
        Codes->insert(OFMake_pair(SurfaceToVolumeRatio, DSRBasicCodedEntry("2PR5", "IBSI", "Surface to Volume Ratio")));
        Codes->insert(OFMake_pair(Compactness1, DSRBasicCodedEntry("SKGS", "IBSI", "Compactness 1")));
        Codes->insert(OFMake_pair(Compactness2, DSRBasicCodedEntry("BQWJ", "IBSI", "Compactness 2")));
        Codes->insert(OFMake_pair(SphericalDisproportion, DSRBasicCodedEntry("KRCK", "IBSI", "Spherical Disproportion")));
        Codes->insert(OFMake_pair(Sphericity, DSRBasicCodedEntry("QCFX", "IBSI", "Sphericity")));
        Codes->insert(OFMake_pair(Asphericity, DSRBasicCodedEntry("25C7", "IBSI", "Asphericity")));
        Codes->insert(OFMake_pair(CentreOfMassShift, DSRBasicCodedEntry("KLMA", "IBSI", "Centre of Mass Shift")));
        Codes->insert(OFMake_pair(Elongation, DSRBasicCodedEntry("Q3CK", "IBSI", "Elongation")));
        Codes->insert(OFMake_pair(Flatness, DSRBasicCodedEntry("N17B", "IBSI", "Flatness")));
        Codes->insert(OFMake_pair(VolumeDensityInFrameOfReferenceAxisAlignedBoundingBox, DSRBasicCodedEntry("PBX1", "IBSI", "Volume Density in Frame of Reference Axis Aligned Bounding Box")));
        Codes->insert(OFMake_pair(AreaDensityInFrameOfReferenceAxisAlignedBoundingBox, DSRBasicCodedEntry("R59B", "IBSI", "Area Density in Frame of Reference Axis Aligned Bounding Box")));
        Codes->insert(OFMake_pair(VolumeDensityInOrientedMinimumBoundingBox, DSRBasicCodedEntry("ZH1A", "IBSI", "Volume Density in Oriented Minimum Bounding Box")));
        Codes->insert(OFMake_pair(AreaDensityInOrientedMinimumBoundingBox, DSRBasicCodedEntry("IQYR", "IBSI", "Area Density in Oriented Minimum Bounding Box")));
        Codes->insert(OFMake_pair(VolumeDensityInApproximateEnclosingEllipsoid, DSRBasicCodedEntry("6BDE", "IBSI", "Volume Density in Approximate Enclosing Ellipsoid")));
        Codes->insert(OFMake_pair(AreaDensityInApproximateEnclosingEllipsoid, DSRBasicCodedEntry("RDD2", "IBSI", "Area Density in Approximate Enclosing Ellipsoid")));
        Codes->insert(OFMake_pair(VolumeDensityInMinimumVolumeEnclosingEllipsoid, DSRBasicCodedEntry("SWZ1", "IBSI", "Volume Density in Minimum Volume Enclosing Ellipsoid")));
        Codes->insert(OFMake_pair(AreaDensityInMinimumVolumeEnclosingEllipsoid, DSRBasicCodedEntry("BRI8", "IBSI", "Area Density in Minimum Volume Enclosing Ellipsoid")));
        Codes->insert(OFMake_pair(VolumeDensityInConvexHull, DSRBasicCodedEntry("R3ER", "IBSI", "Volume Density in Convex Hull")));
        Codes->insert(OFMake_pair(AreaDensityInConvexHull, DSRBasicCodedEntry("7T7F", "IBSI", "Area Density in Convex Hull")));
        Codes->insert(OFMake_pair(IntegratedIntensity, DSRBasicCodedEntry("99N0", "IBSI", "Integrated Intensity")));
        Codes->insert(OFMake_pair(MoransIIndex, DSRBasicCodedEntry("N365", "IBSI", "Moran's I Index")));
        Codes->insert(OFMake_pair(GearysCMeasure, DSRBasicCodedEntry("NPT7", "IBSI", "Geary's C Measure")));
        Codes->insert(OFMake_pair(CobbAngle, DSRBasicCodedEntry("285285000", "SCT", "Cobb angle")));
        Codes->insert(OFMake_pair(AcetabularAngle, DSRBasicCodedEntry("121224", "DCM", "Acetabular angle")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID218_QuantitativeImageFeature::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
