/*
 *
 *  Copyright (C) 2015-2021, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID7452_OrganizationalRoles
 *
 *  Generated automatically from DICOM PS 3.16-2021a
 *  File created on 2021-02-03 21:06:29 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid7452.h"


// general information on CID 7452 (Organizational Roles)
#define CONTEXT_GROUP_NUMBER  "7452"
#define CONTEXT_GROUP_VERSION "20170626"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.516"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID7452_OrganizationalRoles::CodeList *CID7452_OrganizationalRoles::Codes = NULL;


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


OFCondition CID7452_OrganizationalRoles::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID7452_OrganizationalRoles::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID7452_OrganizationalRoles::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID7452_OrganizationalRoles::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
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
            setEnhancedEncodingMode(codedEntry);
    }
    return codedEntry;
}


CID7452_OrganizationalRoles::CodeList &CID7452_OrganizationalRoles::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(MedicalPractitioner, DSRBasicCodedEntry("158965000", "SCT", "Medical Practitioner")));
        Codes->insert(OFMake_pair(Physician, DSRBasicCodedEntry("309343006", "SCT", "Physician")));
        Codes->insert(OFMake_pair(HeadOfRadiology, DSRBasicCodedEntry("128670", "DCM", "Head of Radiology")));
        Codes->insert(OFMake_pair(ChairOfProtocolCommittee, DSRBasicCodedEntry("128671", "DCM", "Chair of Protocol Committee")));
        Codes->insert(OFMake_pair(RepresentativeOfProtocolCommittee, DSRBasicCodedEntry("128676", "DCM", "Representative of Protocol Committee")));
        Codes->insert(OFMake_pair(RepresentativeOfEthicsCommittee, DSRBasicCodedEntry("128677", "DCM", "Representative of Ethics Committee")));
        Codes->insert(OFMake_pair(HeadOfCardiology, DSRBasicCodedEntry("128675", "DCM", "Head of Cardiology")));
        Codes->insert(OFMake_pair(AdministratorOfRadiologyDepartment, DSRBasicCodedEntry("128673", "DCM", "Administrator of Radiology Department")));
        Codes->insert(OFMake_pair(Nurse, DSRBasicCodedEntry("106292003", "SCT", "Nurse")));
        Codes->insert(OFMake_pair(RadiologicTechnologist, DSRBasicCodedEntry("159016003", "SCT", "Radiologic Technologist")));
        Codes->insert(OFMake_pair(LeadRadiologicTechnologist, DSRBasicCodedEntry("128674", "DCM", "Lead Radiologic Technologist")));
        Codes->insert(OFMake_pair(RadiationTherapist, DSRBasicCodedEntry("3430008", "SCT", "Radiation Therapist")));
        Codes->insert(OFMake_pair(Radiographer, DSRBasicCodedEntry("159016003", "SCT", "Radiographer")));
        Codes->insert(OFMake_pair(Intern, DSRBasicCodedEntry("C1144859", "UMLS", "Intern")));
        Codes->insert(OFMake_pair(Resident, DSRBasicCodedEntry("405277009", "SCT", "Resident")));
        Codes->insert(OFMake_pair(Registrar, DSRBasicCodedEntry("158971006", "SCT", "Registrar")));
        Codes->insert(OFMake_pair(Fellow, DSRBasicCodedEntry("121088", "DCM", "Fellow")));
        Codes->insert(OFMake_pair(Attending, DSRBasicCodedEntry("405279007", "SCT", "Attending")));
        Codes->insert(OFMake_pair(Consultant, DSRBasicCodedEntry("309390008", "SCT", "Consultant")));
        Codes->insert(OFMake_pair(ConsultingPhysician, DSRBasicCodedEntry("C1441532", "UMLS", "Consulting Physician")));
        Codes->insert(OFMake_pair(ScrubNurse, DSRBasicCodedEntry("415506007", "SCT", "Scrub nurse")));
        Codes->insert(OFMake_pair(Surgeon, DSRBasicCodedEntry("304292004", "SCT", "Surgeon")));
        Codes->insert(OFMake_pair(Sonologist, DSRBasicCodedEntry("121092", "DCM", "Sonologist")));
        Codes->insert(OFMake_pair(Sonographer, DSRBasicCodedEntry("C1954848", "UMLS", "Sonographer")));
        Codes->insert(OFMake_pair(RadiationPhysicist, DSRBasicCodedEntry("C2985483", "UMLS", "Radiation Physicist")));
        Codes->insert(OFMake_pair(MedicalPhysicist, DSRBasicCodedEntry("C1708969", "UMLS", "Medical Physicist")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID7452_OrganizationalRoles::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
