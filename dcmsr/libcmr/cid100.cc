/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID100_QuantitativeDiagnosticImagingProcedures
 *
 *  Generated automatically from DICOM PS 3.16-2019a
 *  File created on 2019-02-04 12:56:18 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid100.h"


// general information on CID 100 (Quantitative Diagnostic Imaging Procedures)
#define CONTEXT_GROUP_NUMBER  "100"
#define CONTEXT_GROUP_VERSION "20190121"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.998"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID100_QuantitativeDiagnosticImagingProcedures::CodeList *CID100_QuantitativeDiagnosticImagingProcedures::Codes = NULL;


CID100_QuantitativeDiagnosticImagingProcedures::CID100_QuantitativeDiagnosticImagingProcedures(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID100_QuantitativeDiagnosticImagingProcedures::CID100_QuantitativeDiagnosticImagingProcedures(const EnumType selectedValue,
                                                                                               const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID100_QuantitativeDiagnosticImagingProcedures::selectValue(const EnumType selectedValue,
                                                                        const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID100_QuantitativeDiagnosticImagingProcedures::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID100_QuantitativeDiagnosticImagingProcedures::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID100_QuantitativeDiagnosticImagingProcedures::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID100_QuantitativeDiagnosticImagingProcedures::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID100_QuantitativeDiagnosticImagingProcedures::getCodedEntry(const EnumType value,
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


CID100_QuantitativeDiagnosticImagingProcedures::CodeList &CID100_QuantitativeDiagnosticImagingProcedures::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(ImagingProcedure, DSRBasicCodedEntry("P0-0099A", "SRT", "Imaging procedure")));
        Codes->insert(OFMake_pair(MagneticResonanceImagingGuidance, DSRBasicCodedEntry("P5-09051", "SRT", "Magnetic resonance imaging guidance")));
        Codes->insert(OFMake_pair(MultiparametricMRI, DSRBasicCodedEntry("126020", "DCM", "Multiparametric MRI")));
        Codes->insert(OFMake_pair(MultiparametricMRIOfProstate, DSRBasicCodedEntry("126021", "DCM", "Multiparametric MRI of prostate")));
        Codes->insert(OFMake_pair(MultiparametricMRIOfWholeBody, DSRBasicCodedEntry("126022", "DCM", "Multiparametric MRI of whole body")));
        Codes->insert(OFMake_pair(DynamicMagneticResonanceImagingOfKnee, DSRBasicCodedEntry("P5-0907F", "SRT", "Dynamic magnetic resonance imaging of knee")));
        Codes->insert(OFMake_pair(DynamicMagneticResonanceImagingOfPelvis, DSRBasicCodedEntry("P5-70694", "SRT", "Dynamic magnetic resonance imaging of pelvis")));
        Codes->insert(OFMake_pair(CTUnspecifiedBodyRegion, DSRBasicCodedEntry("25045-6", "LN", "CT unspecified body region")));
        Codes->insert(OFMake_pair(MRIUnspecifiedBodyRegion, DSRBasicCodedEntry("25056-3", "LN", "MRI unspecified body region")));
        Codes->insert(OFMake_pair(NMUnspecifiedBodyRegion, DSRBasicCodedEntry("49118-3", "LN", "NM unspecified body region")));
        Codes->insert(OFMake_pair(PETUnspecifiedBodyRegion, DSRBasicCodedEntry("44136-0", "LN", "PET unspecified body region")));
        Codes->insert(OFMake_pair(PETWholeBody, DSRBasicCodedEntry("44139-4", "LN", "PET whole body")));
        Codes->insert(OFMake_pair(PETCT_FDGImagingOfWholeBody, DSRBasicCodedEntry("P5-080FF", "SRT", "PET/CT FDG imaging of whole body")));
        Codes->insert(OFMake_pair(PETCT_METImagingOfWholeBody, DSRBasicCodedEntry("P5-08118", "SRT", "PET/CT MET imaging of whole body")));
        Codes->insert(OFMake_pair(CTPerfusionHeadWithContrastIV, DSRBasicCodedEntry("39142-5", "LN", "CT perfusion head with contrast IV")));
        Codes->insert(OFMake_pair(SPECTBrain, DSRBasicCodedEntry("39632-5", "LN", "SPECT brain")));
        Codes->insert(OFMake_pair(NMHeadPerfusionBrainPET_CT_AV45, DSRBasicCodedEntry("RPID5427", "RADLEX", "NM head perfusion brain PET-CT AV-45")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID100_QuantitativeDiagnosticImagingProcedures::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
