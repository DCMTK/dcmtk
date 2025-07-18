/*
 *
 *  Copyright (C) 2015-2025, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID7445_DeviceParticipatingRole
 *
 *  Generated automatically from DICOM PS 3.16-2025c
 *  File created on 2025-07-16 12:39:07 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid7445.h"


// general information on CID 7445 (Device Participating Role)
#define CONTEXT_GROUP_NUMBER  "7445"
#define CONTEXT_GROUP_KEYWORD "DeviceParticipatingRole"
#define CONTEXT_GROUP_VERSION "20120406"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.1042"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID7445_DeviceParticipatingRole::CodeList *CID7445_DeviceParticipatingRole::Codes = NULL;


CID7445_DeviceParticipatingRole::CID7445_DeviceParticipatingRole(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_KEYWORD, CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID7445_DeviceParticipatingRole::CID7445_DeviceParticipatingRole(const EnumType selectedValue,
                                                                 const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_KEYWORD, CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID7445_DeviceParticipatingRole::selectValue(const EnumType selectedValue,
                                                         const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID7445_DeviceParticipatingRole::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID7445_DeviceParticipatingRole::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID7445_DeviceParticipatingRole::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID7445_DeviceParticipatingRole::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID7445_DeviceParticipatingRole::getCodedEntry(const EnumType value,
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


CID7445_DeviceParticipatingRole::CodeList &CID7445_DeviceParticipatingRole::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(IrradiatingDevice, DSRBasicCodedEntry("113859", "DCM", "Irradiating Device")));
        Codes->insert(OFMake_pair(Recording, DSRBasicCodedEntry("121097", "DCM", "Recording")));
        Codes->insert(OFMake_pair(XRayReadingDevice, DSRBasicCodedEntry("113942", "DCM", "X-Ray Reading Device")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID7445_DeviceParticipatingRole::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
