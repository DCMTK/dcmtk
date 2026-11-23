/*
 *
 *  Copyright (C) 2015-2026, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID7181_AbstractMultiDimensionalImageModelComponentUnit
 *
 *  Generated automatically from DICOM PS 3.16-2026e
 *  File created on 2026-11-23 13:08:44 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid7181.h"


// general information on CID 7181 (Abstract Multi-dimensional Image Model Component Unit)
#define CONTEXT_GROUP_NUMBER  "7181"
#define CONTEXT_GROUP_KEYWORD "AbstractMultiDimensionalImageModelComponentUnit"
#define CONTEXT_GROUP_VERSION "20261115"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.918"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID7181_AbstractMultiDimensionalImageModelComponentUnit::CodeList *CID7181_AbstractMultiDimensionalImageModelComponentUnit::Codes = NULL;


CID7181_AbstractMultiDimensionalImageModelComponentUnit::CID7181_AbstractMultiDimensionalImageModelComponentUnit(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_KEYWORD, CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID7181_AbstractMultiDimensionalImageModelComponentUnit::CID7181_AbstractMultiDimensionalImageModelComponentUnit(const EnumType selectedValue,
                                                                                                                 const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_KEYWORD, CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID7181_AbstractMultiDimensionalImageModelComponentUnit::selectValue(const EnumType selectedValue,
                                                                                 const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID7181_AbstractMultiDimensionalImageModelComponentUnit::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID7181_AbstractMultiDimensionalImageModelComponentUnit::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID7181_AbstractMultiDimensionalImageModelComponentUnit::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID7181_AbstractMultiDimensionalImageModelComponentUnit::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID7181_AbstractMultiDimensionalImageModelComponentUnit::getCodedEntry(const EnumType value,
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


CID7181_AbstractMultiDimensionalImageModelComponentUnit::CodeList &CID7181_AbstractMultiDimensionalImageModelComponentUnit::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(mmHg, DSRBasicCodedEntry("mm[Hg]", "UCUM", "mmHg")));
        Codes->insert(OFMake_pair(kPa, DSRBasicCodedEntry("kPa", "UCUM", "kPa")));
        Codes->insert(OFMake_pair(PRU, DSRBasicCodedEntry("[PRU]", "UCUM", "P.R.U.")));
        Codes->insert(OFMake_pair(WoodU, DSRBasicCodedEntry("[wood'U]", "UCUM", "Wood U")));
        Codes->insert(OFMake_pair(dynscm5, DSRBasicCodedEntry("dyn.s.cm-5", "UCUM", "dyn.s.cm-5")));
        Codes->insert(OFMake_pair(PRUPerM2, DSRBasicCodedEntry("[PRU]/m2", "UCUM", "P.R.U./m2")));
        Codes->insert(OFMake_pair(WoodUPerM2, DSRBasicCodedEntry("[wood'U]/m2", "UCUM", "Wood U/m2")));
        Codes->insert(OFMake_pair(dynscm5PerM2, DSRBasicCodedEntry("dyn.s.cm-5/m2", "UCUM", "dyn.s.cm-5/m2")));
        Codes->insert(OFMake_pair(cm, DSRBasicCodedEntry("cm", "UCUM", "cm")));
        Codes->insert(OFMake_pair(mm, DSRBasicCodedEntry("mm", "UCUM", "mm")));
        Codes->insert(OFMake_pair(um, DSRBasicCodedEntry("um", "UCUM", "um")));
        Codes->insert(OFMake_pair(cm2, DSRBasicCodedEntry("cm2", "UCUM", "cm2")));
        Codes->insert(OFMake_pair(mm2, DSRBasicCodedEntry("mm2", "UCUM", "mm2")));
        Codes->insert(OFMake_pair(um2, DSRBasicCodedEntry("um2", "UCUM", "um2")));
        Codes->insert(OFMake_pair(dm3, DSRBasicCodedEntry("dm3", "UCUM", "dm3")));
        Codes->insert(OFMake_pair(cm3, DSRBasicCodedEntry("cm3", "UCUM", "cm3")));
        Codes->insert(OFMake_pair(mm3, DSRBasicCodedEntry("mm3", "UCUM", "mm3")));
        Codes->insert(OFMake_pair(um3, DSRBasicCodedEntry("um3", "UCUM", "um3")));
        Codes->insert(OFMake_pair(StandardizedUptakeValueBodyWeight, DSRBasicCodedEntry("g/ml{SUVbw}", "UCUM", "Standardized Uptake Value body weight")));
        Codes->insert(OFMake_pair(StandardizedUptakeValueLeanBodyMassJames, DSRBasicCodedEntry("g/ml{SUVlbm}", "UCUM", "Standardized Uptake Value lean body mass (James)")));
        Codes->insert(OFMake_pair(StandardizedUptakeValueLeanBodyMassJames128Multiplier, DSRBasicCodedEntry("g/ml{SUVlbm(James128)}", "UCUM", "Standardized Uptake Value lean body mass (James 128 multiplier)")));
        Codes->insert(OFMake_pair(StandardizedUptakeValueLeanBodyMassJanma, DSRBasicCodedEntry("g/ml{SUVlbm(Janma)}", "UCUM", "Standardized Uptake Value lean body mass (Janma)")));
        Codes->insert(OFMake_pair(StandardizedUptakeValueBodySurfaceArea, DSRBasicCodedEntry("cm2/ml{SUVbsa}", "UCUM", "Standardized Uptake Value body surface area")));
        Codes->insert(OFMake_pair(StandardizedUptakeValueIdealBodyWeight, DSRBasicCodedEntry("g/ml{SUVibw}", "UCUM", "Standardized Uptake Value ideal body weight")));
        Codes->insert(OFMake_pair(counts, DSRBasicCodedEntry("{counts}", "UCUM", "counts")));
        Codes->insert(OFMake_pair(countsPerS, DSRBasicCodedEntry("{counts}/s", "UCUM", "counts/s")));
        Codes->insert(OFMake_pair(ProportionalToCounts, DSRBasicCodedEntry("{propcounts}", "UCUM", "Proportional to counts")));
        Codes->insert(OFMake_pair(ProportionalToCountsPerSecond, DSRBasicCodedEntry("{propcounts}/s", "UCUM", "Proportional to counts per second")));
        Codes->insert(OFMake_pair(cm2PerMl, DSRBasicCodedEntry("cm2/ml", "UCUM", "cm2/ml")));
        Codes->insert(OFMake_pair(Percent, DSRBasicCodedEntry("%", "UCUM", "%")));
        Codes->insert(OFMake_pair(BqPerMl, DSRBasicCodedEntry("Bq/ml", "UCUM", "Bq/ml")));
        Codes->insert(OFMake_pair(mgPerMinPerMl, DSRBasicCodedEntry("mg/min/ml", "UCUM", "mg/min/ml")));
        Codes->insert(OFMake_pair(umolPerMinPerMl, DSRBasicCodedEntry("umol/min/ml", "UCUM", "umol/min/ml")));
        Codes->insert(OFMake_pair(mlPerMinPerG, DSRBasicCodedEntry("ml/min/g", "UCUM", "ml/min/g")));
        Codes->insert(OFMake_pair(mlPerG, DSRBasicCodedEntry("ml/g", "UCUM", "ml/g")));
        Codes->insert(OFMake_pair(PerCm, DSRBasicCodedEntry("/cm", "UCUM", "/cm")));
        Codes->insert(OFMake_pair(umolPerMl, DSRBasicCodedEntry("umol/ml", "UCUM", "umol/ml")));
        Codes->insert(OFMake_pair(mm2PerS, DSRBasicCodedEntry("mm2/s", "UCUM", "mm2/s")));
        Codes->insert(OFMake_pair(um2PerMs, DSRBasicCodedEntry("um2/ms", "UCUM", "um2/ms")));
        Codes->insert(OFMake_pair(um2PerS, DSRBasicCodedEntry("um2/s", "UCUM", "um2/s")));
        Codes->insert(OFMake_pair(_106mm2PerS, DSRBasicCodedEntry("10-6.mm2/s", "UCUM", "10-6.mm2/s")));
        Codes->insert(OFMake_pair(Gy, DSRBasicCodedEntry("Gy", "UCUM", "Gy")));
        Codes->insert(OFMake_pair(Sv, DSRBasicCodedEntry("Sv", "UCUM", "Sv")));
        Codes->insert(OFMake_pair(noUnits, DSRBasicCodedEntry("1", "UCUM", "no units")));
        Codes->insert(OFMake_pair(ratio, DSRBasicCodedEntry("{ratio}", "UCUM", "ratio")));
        Codes->insert(OFMake_pair(HounsfieldUnit, DSRBasicCodedEntry("[hnsf'U]", "UCUM", "Hounsfield unit")));
        Codes->insert(OFMake_pair(Counts, DSRBasicCodedEntry("{counts}", "UCUM", "Counts")));
        Codes->insert(OFMake_pair(arbitraryUnit, DSRBasicCodedEntry("[arb'U]", "UCUM", "arbitrary unit")));
        Codes->insert(OFMake_pair(ppm, DSRBasicCodedEntry("[ppm]", "UCUM", "ppm")));
        Codes->insert(OFMake_pair(cmPerS, DSRBasicCodedEntry("cm/s", "UCUM", "cm/s")));
        Codes->insert(OFMake_pair(mmPerS, DSRBasicCodedEntry("mm/s", "UCUM", "mm/s")));
        Codes->insert(OFMake_pair(dB, DSRBasicCodedEntry("dB", "UCUM", "dB")));
        Codes->insert(OFMake_pair(Cel, DSRBasicCodedEntry("Cel", "UCUM", "Cel")));
        Codes->insert(OFMake_pair(mlPerMin, DSRBasicCodedEntry("ml/min", "UCUM", "ml/min")));
        Codes->insert(OFMake_pair(mlPerS, DSRBasicCodedEntry("ml/s", "UCUM", "ml/s")));
        Codes->insert(OFMake_pair(ms, DSRBasicCodedEntry("ms", "UCUM", "ms")));
        Codes->insert(OFMake_pair(s, DSRBasicCodedEntry("s", "UCUM", "s")));
        Codes->insert(OFMake_pair(Hz, DSRBasicCodedEntry("Hz", "UCUM", "Hz")));
        Codes->insert(OFMake_pair(mT, DSRBasicCodedEntry("mT", "UCUM", "mT")));
        Codes->insert(OFMake_pair(ParticlesPer100gTissue, DSRBasicCodedEntry("{Particles}/(100)g{Tissue}", "UCUM", "Particles/100g tissue")));
        Codes->insert(OFMake_pair(sPerMm2, DSRBasicCodedEntry("s/mm2", "UCUM", "s/mm2")));
        Codes->insert(OFMake_pair(mlPer100gPerMin, DSRBasicCodedEntry("ml/(100)g/min", "UCUM", "ml/(100)g/min")));
        Codes->insert(OFMake_pair(mlPer100ml, DSRBasicCodedEntry("ml/(100)ml", "UCUM", "ml/(100)ml")));
        Codes->insert(OFMake_pair(mmolPerKgWetWeight, DSRBasicCodedEntry("mmol/kg{WetWeight}", "UCUM", "mmol/kg wet weight")));
        Codes->insert(OFMake_pair(PerMin, DSRBasicCodedEntry("/min", "UCUM", "/min")));
        Codes->insert(OFMake_pair(PerS, DSRBasicCodedEntry("/s", "UCUM", "/s")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID7181_AbstractMultiDimensionalImageModelComponentUnit::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
