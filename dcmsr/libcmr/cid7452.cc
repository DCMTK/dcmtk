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
 *    classes: CID7452_OrganizationalRoles
 *
 *    Generated automatically from DICOM PS 3.16-2015c
 *    File created on 2015-07-20 09:43:08 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid7452.h"


// general information on CID 7452 (Organizational Roles)
#define CONTEXT_GROUP_NUMBER  "7452"
#define CONTEXT_GROUP_VERSION "20150602"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.516"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */


CID7452_OrganizationalRoles::CID7452_OrganizationalRoles(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID7452_OrganizationalRoles::CID7452_OrganizationalRoles(const EnumType selectedValue,
                                                         const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID7452_OrganizationalRoles::selectValue(const EnumType selectedValue,
                                                     const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID7452_OrganizationalRoles::findCodedEntry(const DSRCodedEntryValue &codedEntryValue) const
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


void CID7452_OrganizationalRoles::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID7452_OrganizationalRoles::initialize()
{
    /* create and initialize code list */
    getCodes();
}


DSRCodedEntryValue CID7452_OrganizationalRoles::getCodedEntry(const EnumType value,
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


CID7452_OrganizationalRoles::CodeList &CID7452_OrganizationalRoles::getCodes()
{
    /* use a static variable for singleton pattern */
    static CodeList *codes = NULL;
    /* check whether code list has already been created and initialized */
    if (codes == NULL)
    {
        /* create a new code list (should never fail) */
        codes = new CodeList();
        /* and initialize it by adding the coded entries */
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(MedicalPractitioner, DSRBasicCodedEntry("J-0016E", "SRT", "Medical Practitioner")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Physician, DSRBasicCodedEntry("J-004E8", "SRT", "Physician")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Nurse, DSRBasicCodedEntry("J-07100", "SRT", "Nurse")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(RadiologicTechnologist, DSRBasicCodedEntry("J-00187", "SRT", "Radiologic Technologist")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Radiographer, DSRBasicCodedEntry("J-00187", "SRT", "Radiographer")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Intern, DSRBasicCodedEntry("C1144859", "UMLS", "Intern")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Resident, DSRBasicCodedEntry("J-005E6", "SRT", "Resident")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Registrar, DSRBasicCodedEntry("J-00172", "SRT", "Registrar")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Fellow, DSRBasicCodedEntry("121088", "DCM", "Fellow")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Attending, DSRBasicCodedEntry("J-005E8", "SRT", "Attending")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Consultant, DSRBasicCodedEntry("J-0050A", "SRT", "Consultant")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ConsultingPhysician, DSRBasicCodedEntry("C1441532", "UMLS", "Consulting Physician")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ScrubNurse, DSRBasicCodedEntry("J-0714A", "SRT", "Scrub nurse")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Surgeon, DSRBasicCodedEntry("J-00556", "SRT", "Surgeon")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Sonologist, DSRBasicCodedEntry("121092", "DCM", "Sonologist")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Sonographer, DSRBasicCodedEntry("C1954848", "UMLS", "Sonographer")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(RadiationPhysicist, DSRBasicCodedEntry("C2985483", "UMLS", "Radiation Physicist")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(MedicalPhysicist, DSRBasicCodedEntry("C1708969", "UMLS", "Medical Physicist")));
    }
    /* should never be NULL */
    return *codes;
}
