/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID7464_GeneralRegionOfInterestMeasurementModifiers
 *
 *  Generated automatically from DICOM PS 3.16-2019a
 *  File created on 2019-02-04 12:56:39 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid7464.h"


// general information on CID 7464 (General Region of Interest Measurement Modifiers)
#define CONTEXT_GROUP_NUMBER  "7464"
#define CONTEXT_GROUP_VERSION "20121101"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.951"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID7464_GeneralRegionOfInterestMeasurementModifiers::CodeList *CID7464_GeneralRegionOfInterestMeasurementModifiers::Codes = NULL;


CID7464_GeneralRegionOfInterestMeasurementModifiers::CID7464_GeneralRegionOfInterestMeasurementModifiers(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID7464_GeneralRegionOfInterestMeasurementModifiers::CID7464_GeneralRegionOfInterestMeasurementModifiers(const EnumType selectedValue,
                                                                                                         const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID7464_GeneralRegionOfInterestMeasurementModifiers::selectValue(const EnumType selectedValue,
                                                                             const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID7464_GeneralRegionOfInterestMeasurementModifiers::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID7464_GeneralRegionOfInterestMeasurementModifiers::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID7464_GeneralRegionOfInterestMeasurementModifiers::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID7464_GeneralRegionOfInterestMeasurementModifiers::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID7464_GeneralRegionOfInterestMeasurementModifiers::getCodedEntry(const EnumType value,
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


CID7464_GeneralRegionOfInterestMeasurementModifiers::CodeList &CID7464_GeneralRegionOfInterestMeasurementModifiers::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(Maximum, DSRBasicCodedEntry("G-A437", "SRT", "Maximum")));
        Codes->insert(OFMake_pair(Minimum, DSRBasicCodedEntry("R-404FB", "SRT", "Minimum")));
        Codes->insert(OFMake_pair(Mean, DSRBasicCodedEntry("R-00317", "SRT", "Mean")));
        Codes->insert(OFMake_pair(StandardDeviation, DSRBasicCodedEntry("R-10047", "SRT", "Standard Deviation")));
        Codes->insert(OFMake_pair(Total, DSRBasicCodedEntry("R-40507", "SRT", "Total")));
        Codes->insert(OFMake_pair(Median, DSRBasicCodedEntry("R-00319", "SRT", "Median")));
        Codes->insert(OFMake_pair(Mode, DSRBasicCodedEntry("R-0032E", "SRT", "Mode")));
        Codes->insert(OFMake_pair(PeakValueWithinROI, DSRBasicCodedEntry("126031", "DCM", "Peak Value Within ROI")));
        Codes->insert(OFMake_pair(CoefficientOfVariance, DSRBasicCodedEntry("C0681921", "UMLS", "Coefficient of Variance")));
        Codes->insert(OFMake_pair(Skewness, DSRBasicCodedEntry("126051", "DCM", "Skewness")));
        Codes->insert(OFMake_pair(Kurtosis, DSRBasicCodedEntry("126052", "DCM", "Kurtosis")));
        Codes->insert(OFMake_pair(Variance, DSRBasicCodedEntry("C1711260", "UMLS", "Variance")));
        Codes->insert(OFMake_pair(RootMeanSquare, DSRBasicCodedEntry("C2347976", "UMLS", "Root Mean Square")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID7464_GeneralRegionOfInterestMeasurementModifiers::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
