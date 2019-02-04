/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID7453_PerformingRoles
 *
 *  Generated automatically from DICOM PS 3.16-2019a
 *  File created on 2019-02-04 12:56:37 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid7453.h"


// general information on CID 7453 (Performing Roles)
#define CONTEXT_GROUP_NUMBER  "7453"
#define CONTEXT_GROUP_VERSION "20180326"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.517"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID7453_PerformingRoles::CodeList *CID7453_PerformingRoles::Codes = NULL;


CID7453_PerformingRoles::CID7453_PerformingRoles(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID7453_PerformingRoles::CID7453_PerformingRoles(const EnumType selectedValue,
                                                 const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID7453_PerformingRoles::selectValue(const EnumType selectedValue,
                                                 const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID7453_PerformingRoles::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID7453_PerformingRoles::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID7453_PerformingRoles::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID7453_PerformingRoles::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID7453_PerformingRoles::getCodedEntry(const EnumType value,
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


CID7453_PerformingRoles::CodeList &CID7453_PerformingRoles::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(Performing, DSRBasicCodedEntry("121094", "DCM", "Performing")));
        Codes->insert(OFMake_pair(Referring, DSRBasicCodedEntry("C1709880", "UMLS", "Referring")));
        Codes->insert(OFMake_pair(Requesting, DSRBasicCodedEntry("121096", "DCM", "Requesting")));
        Codes->insert(OFMake_pair(Recording, DSRBasicCodedEntry("121097", "DCM", "Recording")));
        Codes->insert(OFMake_pair(Verifying, DSRBasicCodedEntry("121098", "DCM", "Verifying")));
        Codes->insert(OFMake_pair(Assisting, DSRBasicCodedEntry("121099", "DCM", "Assisting")));
        Codes->insert(OFMake_pair(CirculatingNurse, DSRBasicCodedEntry("J-0714B", "SRT", "Circulating Nurse")));
        Codes->insert(OFMake_pair(Standby, DSRBasicCodedEntry("121101", "DCM", "Standby")));
        Codes->insert(OFMake_pair(IrradiationAuthorizing, DSRBasicCodedEntry("113850", "DCM", "Irradiation Authorizing")));
        Codes->insert(OFMake_pair(IrradiationAdministering, DSRBasicCodedEntry("113851", "DCM", "Irradiation Administering")));
        Codes->insert(OFMake_pair(Reader, DSRBasicCodedEntry("C28747", "NCIt", "Reader")));
        Codes->insert(OFMake_pair(EligibilityReader, DSRBasicCodedEntry("129001", "DCM", "Eligibility Reader")));
        Codes->insert(OFMake_pair(Adjudicator, DSRBasicCodedEntry("C96561", "NCIt", "Adjudicator")));
        Codes->insert(OFMake_pair(Reviewer, DSRBasicCodedEntry("C54634", "NCIt", "Reviewer")));
        Codes->insert(OFMake_pair(Designator, DSRBasicCodedEntry("129002", "DCM", "Designator")));
        Codes->insert(OFMake_pair(ImageQualityController, DSRBasicCodedEntry("129003", "DCM", "Image Quality Controller")));
        Codes->insert(OFMake_pair(ResultsQualityController, DSRBasicCodedEntry("129004", "DCM", "Results Quality Controller")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID7453_PerformingRoles::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
