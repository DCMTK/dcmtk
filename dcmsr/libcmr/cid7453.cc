/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: CID7453_PerformingRoles
 *
 *    Generated automatically from DICOM PS 3.16-2015c
 *    File created on 2015-07-20 09:43:09 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid7453.h"


// general information on CID 7453 (Performing Roles)
#define CONTEXT_GROUP_NUMBER  "7453"
#define CONTEXT_GROUP_VERSION "20141110"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.517"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */


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


OFCondition CID7453_PerformingRoles::findCodedEntry(const DSRCodedEntryValue &codedEntryValue) const
{
    OFCondition result = SR_EC_CodedEntryNotInContextGroup;
    /* first, search for standard codes */
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator iter = getCodes().begin();
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator last = getCodes().end();
    /* iterate over coded entry list */
    while (iter != last)
    {
        /* if found, exit loop */
        if (codedEntryValue == iter->second)
        {
            result = SR_EC_CodedEntryInStandardContextGroup;
            break;
        }
        ++iter;
    }
    /* if not, continue with extended codes */
    if (result.bad())
        result = DSRContextGroup::findCodedEntry(codedEntryValue);
    return result;
}


void CID7453_PerformingRoles::printCodes(STD_NAMESPACE ostream &stream) const
{
    /* print standard codes */
    stream << "Standard codes:" << OFendl;
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator iter = getCodes().begin();
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator last = getCodes().end();
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
            codedEntry.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
    }
    return codedEntry;
}


CID7453_PerformingRoles::CodeList &CID7453_PerformingRoles::getCodes()
{
    /* use a static variable for singleton pattern */
    static CodeList *codes = NULL;
    /* check whether code list has already been created and initialized */
    if (codes == NULL)
    {
        /* create a new code list (should never fail) */
        codes = new CodeList();
        /* and initialize it by adding the coded entries */
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Performing, DSRBasicCodedEntry("121094", "DCM", "Performing")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Referring, DSRBasicCodedEntry("C1709880", "UMLS", "Referring")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Requesting, DSRBasicCodedEntry("121096", "DCM", "Requesting")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Recording, DSRBasicCodedEntry("121097", "DCM", "Recording")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Verifying, DSRBasicCodedEntry("121098", "DCM", "Verifying")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Assisting, DSRBasicCodedEntry("121099", "DCM", "Assisting")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(CirculatingNurse, DSRBasicCodedEntry("J-0714B", "SRT", "Circulating Nurse")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Standby, DSRBasicCodedEntry("121101", "DCM", "Standby")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(IrradiationAuthorizing, DSRBasicCodedEntry("113850", "DCM", "Irradiation Authorizing")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(IrradiationAdministering, DSRBasicCodedEntry("113851", "DCM", "Irradiation Administering")));
    }
    /* should never be NULL */
    return *codes;
}
