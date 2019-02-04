/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID7181_AbstractMultiDimensionalImageModelComponentUnits
 *
 *  Generated automatically from DICOM PS 3.16-2019a
 *  File created on 2019-02-04 12:56:31 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid7181.h"


// general information on CID 7181 (Abstract Multi-dimensional Image Model Component Units)
#define CONTEXT_GROUP_NUMBER  "7181"
#define CONTEXT_GROUP_VERSION "20180605"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.918"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID7181_AbstractMultiDimensionalImageModelComponentUnits::CodeList *CID7181_AbstractMultiDimensionalImageModelComponentUnits::Codes = NULL;


CID7181_AbstractMultiDimensionalImageModelComponentUnits::CID7181_AbstractMultiDimensionalImageModelComponentUnits(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID7181_AbstractMultiDimensionalImageModelComponentUnits::CID7181_AbstractMultiDimensionalImageModelComponentUnits(const EnumType selectedValue,
                                                                                                                   const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID7181_AbstractMultiDimensionalImageModelComponentUnits::selectValue(const EnumType selectedValue,
                                                                                  const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID7181_AbstractMultiDimensionalImageModelComponentUnits::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID7181_AbstractMultiDimensionalImageModelComponentUnits::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID7181_AbstractMultiDimensionalImageModelComponentUnits::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID7181_AbstractMultiDimensionalImageModelComponentUnits::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID7181_AbstractMultiDimensionalImageModelComponentUnits::getCodedEntry(const EnumType value,
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


CID7181_AbstractMultiDimensionalImageModelComponentUnits::CodeList &CID7181_AbstractMultiDimensionalImageModelComponentUnits::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(MmHg, DSRBasicCodedEntry("mm[Hg]", "UCUM", "mmHg")));
        Codes->insert(OFMake_pair(KPa, DSRBasicCodedEntry("kPa", "UCUM", "kPa")));
        Codes->insert(OFMake_pair(PRU, DSRBasicCodedEntry("[PRU]", "UCUM", "P.R.U.")));
        Codes->insert(OFMake_pair(WoodU, DSRBasicCodedEntry("[wood'U]", "UCUM", "Wood U")));
        Codes->insert(OFMake_pair(Dynscm5, DSRBasicCodedEntry("dyn.s.cm-5", "UCUM", "dyn.s.cm-5")));
        Codes->insert(OFMake_pair(PRUPerM2, DSRBasicCodedEntry("[PRU]/m2", "UCUM", "P.R.U./m2")));
        Codes->insert(OFMake_pair(WoodUPerM2, DSRBasicCodedEntry("[wood'U]/m2", "UCUM", "Wood U/m2")));
        Codes->insert(OFMake_pair(Dynscm5PerM2, DSRBasicCodedEntry("dyn.s.cm-5/m2", "UCUM", "dyn.s.cm-5/m2")));
        Codes->insert(OFMake_pair(Centimeter, DSRBasicCodedEntry("cm", "UCUM", "centimeter")));
        Codes->insert(OFMake_pair(Millimeter, DSRBasicCodedEntry("mm", "UCUM", "millimeter")));
        Codes->insert(OFMake_pair(Micrometer, DSRBasicCodedEntry("um", "UCUM", "micrometer")));
        Codes->insert(OFMake_pair(SquareCentimeter, DSRBasicCodedEntry("cm2", "UCUM", "square centimeter")));
        Codes->insert(OFMake_pair(SquareMillimeter, DSRBasicCodedEntry("mm2", "UCUM", "square millimeter")));
        Codes->insert(OFMake_pair(SquareMicrometer, DSRBasicCodedEntry("um2", "UCUM", "square micrometer")));
        Codes->insert(OFMake_pair(CubicDecimeter, DSRBasicCodedEntry("dm3", "UCUM", "cubic decimeter")));
        Codes->insert(OFMake_pair(CubicCentimeter, DSRBasicCodedEntry("cm3", "UCUM", "cubic centimeter")));
        Codes->insert(OFMake_pair(CubicMillimeter, DSRBasicCodedEntry("mm3", "UCUM", "cubic millimeter")));
        Codes->insert(OFMake_pair(CubicMicrometer, DSRBasicCodedEntry("um3", "UCUM", "cubic micrometer")));
        Codes->insert(OFMake_pair(StandardizedUptakeValueBodyWeight, DSRBasicCodedEntry("g/ml{SUVbw}", "UCUM", "Standardized Uptake Value body weight")));
        Codes->insert(OFMake_pair(StandardizedUptakeValueLeanBodyMassJames, DSRBasicCodedEntry("g/ml{SUVlbm}", "UCUM", "Standardized Uptake Value lean body mass (James)")));
        Codes->insert(OFMake_pair(StandardizedUptakeValueLeanBodyMassJames128Multiplier, DSRBasicCodedEntry("g/ml{SUVlbm(James128)}", "UCUM", "Standardized Uptake Value lean body mass (James 128 multiplier)")));
        Codes->insert(OFMake_pair(StandardizedUptakeValueLeanBodyMassJanma, DSRBasicCodedEntry("g/ml{SUVlbm(Janma)}", "UCUM", "Standardized Uptake Value lean body mass (Janma)")));
        Codes->insert(OFMake_pair(StandardizedUptakeValueBodySurfaceArea, DSRBasicCodedEntry("cm2/ml{SUVbsa}", "UCUM", "Standardized Uptake Value body surface area")));
        Codes->insert(OFMake_pair(StandardizedUptakeValueIdealBodyWeight, DSRBasicCodedEntry("g/ml{SUVibw}", "UCUM", "Standardized Uptake Value ideal body weight")));
        Codes->insert(OFMake_pair(Counts, DSRBasicCodedEntry("{counts}", "UCUM", "Counts")));
        Codes->insert(OFMake_pair(CountsPerSecond, DSRBasicCodedEntry("{counts}/s", "UCUM", "Counts per second")));
        Codes->insert(OFMake_pair(ProportionalToCounts, DSRBasicCodedEntry("{propcounts}", "UCUM", "Proportional to counts")));
        Codes->insert(OFMake_pair(ProportionalToCountsPerSecond, DSRBasicCodedEntry("{propcounts}/s", "UCUM", "Proportional to counts per second")));
        Codes->insert(OFMake_pair(SquareCentimeterPerMilliliter, DSRBasicCodedEntry("cm2/ml", "UCUM", "Centimeter**2/milliliter")));
        Codes->insert(OFMake_pair(Percent, DSRBasicCodedEntry("%", "UCUM", "Percent")));
        Codes->insert(OFMake_pair(BecquerelsPerMilliliter, DSRBasicCodedEntry("Bq/ml", "UCUM", "Becquerels/milliliter")));
        Codes->insert(OFMake_pair(MilligramsPerMinutePerMilliliter, DSRBasicCodedEntry("mg/min/ml", "UCUM", "Milligrams/minute/milliliter")));
        Codes->insert(OFMake_pair(MicromolePerMinutePerMilliliter, DSRBasicCodedEntry("umol/min/ml", "UCUM", "Micromole/minute/milliliter")));
        Codes->insert(OFMake_pair(MilliliterPerMinutePerGram, DSRBasicCodedEntry("ml/min/g", "UCUM", "Milliliter/minute/gram")));
        Codes->insert(OFMake_pair(MilliliterPerGram, DSRBasicCodedEntry("ml/g", "UCUM", "Milliliter/gram")));
        Codes->insert(OFMake_pair(PerCentimeter, DSRBasicCodedEntry("/cm", "UCUM", "/Centimeter")));
        Codes->insert(OFMake_pair(MicromolePerMilliliter, DSRBasicCodedEntry("umol/ml", "UCUM", "Micromole/milliliter")));
        Codes->insert(OFMake_pair(Mm2PerS, DSRBasicCodedEntry("mm2/s", "UCUM", "mm2/s")));
        Codes->insert(OFMake_pair(Um2PerMs, DSRBasicCodedEntry("um2/ms", "UCUM", "um2/ms")));
        Codes->insert(OFMake_pair(Um2PerS, DSRBasicCodedEntry("um2/s", "UCUM", "um2/s")));
        Codes->insert(OFMake_pair(_106mm2PerS, DSRBasicCodedEntry("10-6.mm2/s", "UCUM", "10-6.mm2/s")));
        Codes->insert(OFMake_pair(Gy, DSRBasicCodedEntry("Gy", "UCUM", "Gy")));
        Codes->insert(OFMake_pair(Sv, DSRBasicCodedEntry("Sv", "UCUM", "Sv")));
        Codes->insert(OFMake_pair(NoUnits, DSRBasicCodedEntry("1", "UCUM", "no units")));
        Codes->insert(OFMake_pair(Ratio, DSRBasicCodedEntry("{ratio}", "UCUM", "ratio")));
        Codes->insert(OFMake_pair(HounsfieldUnit, DSRBasicCodedEntry("[hnsf'U]", "UCUM", "Hounsfield Unit")));
        Codes->insert(OFMake_pair(ArbitraryUnit, DSRBasicCodedEntry("[arb'U]", "UCUM", "arbitrary unit")));
        Codes->insert(OFMake_pair(Ppm, DSRBasicCodedEntry("ppm", "UCUM", "ppm")));
        Codes->insert(OFMake_pair(CentimeterPerSecond, DSRBasicCodedEntry("cm/s", "UCUM", "centimeter/second")));
        Codes->insert(OFMake_pair(MillimeterPerSecond, DSRBasicCodedEntry("mm/s", "UCUM", "millimeter/second")));
        Codes->insert(OFMake_pair(Decibel, DSRBasicCodedEntry("dB", "UCUM", "decibel")));
        Codes->insert(OFMake_pair(DegreesCelsius, DSRBasicCodedEntry("Cel", "UCUM", "degrees Celsius")));
        Codes->insert(OFMake_pair(MilliliterPerMinute, DSRBasicCodedEntry("ml/min", "UCUM", "milliliter per minute")));
        Codes->insert(OFMake_pair(MilliliterPerSecond, DSRBasicCodedEntry("ml/s", "UCUM", "milliliter per second")));
        Codes->insert(OFMake_pair(Millisecond, DSRBasicCodedEntry("ms", "UCUM", "millisecond")));
        Codes->insert(OFMake_pair(Second, DSRBasicCodedEntry("s", "UCUM", "second")));
        Codes->insert(OFMake_pair(Hertz, DSRBasicCodedEntry("Hz", "UCUM", "Hertz")));
        Codes->insert(OFMake_pair(MilliTesla, DSRBasicCodedEntry("mT", "UCUM", "milliTesla")));
        Codes->insert(OFMake_pair(NumberParticlesPer100GramOfTissue, DSRBasicCodedEntry("{Particles}/[100]g{Tissue}", "UCUM", "number particles per 100 gram of tissue")));
        Codes->insert(OFMake_pair(SecondPerSquareMillimeter, DSRBasicCodedEntry("s/mm2", "UCUM", "second per square millimeter")));
        Codes->insert(OFMake_pair(MilliliterPer100GramPerMinute, DSRBasicCodedEntry("ml/[100]g/min", "UCUM", "milliliter per 100 gram per minute")));
        Codes->insert(OFMake_pair(MilliliterPer100Milliliter, DSRBasicCodedEntry("ml/[100]ml", "UCUM", "milliliter per 100 milliliter")));
        Codes->insert(OFMake_pair(MillimolesPerKgWetWeight, DSRBasicCodedEntry("mmol/kg{WetWeight}", "UCUM", "millimoles per kg wet weight")));
        Codes->insert(OFMake_pair(PerMin, DSRBasicCodedEntry("/min", "UCUM", "/min")));
        Codes->insert(OFMake_pair(PerS, DSRBasicCodedEntry("/s", "UCUM", "/s")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID7181_AbstractMultiDimensionalImageModelComponentUnits::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
