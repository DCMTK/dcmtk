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
 *    classes: CID5000_Languages
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid5000.h"


// general information on CID 5000 (Languages)
#define CONTEXT_GROUP_NUMBER  "5000"
#define CONTEXT_GROUP_VERSION ""      /* unknown */
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.328"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible? */


CID5000_Languages::CID5000_Languages(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID5000_Languages::CID5000_Languages(const EnumType selectedValue,
                                     const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID5000_Languages::selectValue(const EnumType selectedValue,
                                           const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID5000_Languages::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
                                              DSRCodedEntryValue *foundCodedEntry,
                                              const OFBool enhancedEncodingMode) const
{
    OFCondition result = SR_EC_CodedEntryNotInContextGroup;
    /* first, search for standard codes */
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator iter = getCodes().begin();
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator last = getCodes().end();
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
                    foundCodedEntry->setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
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


void CID5000_Languages::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID5000_Languages::initialize()
{
    /* create and initialize code list */
    getCodes();
}


DSRCodedEntryValue CID5000_Languages::getCodedEntry(const EnumType value,
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


CID5000_Languages::CodeList &CID5000_Languages::getCodes()
{
    /* use a static variable for singleton pattern */
    static CodeList *codes = NULL;
    /* check whether code list has already been created and initialized */
    if (codes == NULL)
    {
        /* create a new code list (should never fail) */
        codes = new CodeList();
        /* and initialize it by adding the coded entries */
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(English, DSRBasicCodedEntry("eng", "IETF4646", "English")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(English_CA, DSRBasicCodedEntry("en-CA", "IETF4646", "English (CA)")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(English_GB, DSRBasicCodedEntry("en-GB", "IETF4646", "English (GB)")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(English_US, DSRBasicCodedEntry("en-US", "IETF4646", "English (US)")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(French, DSRBasicCodedEntry("fra", "IETF4646", "French")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(French_CA, DSRBasicCodedEntry("fr-CA", "IETF4646", "French (CA)")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(French_CH, DSRBasicCodedEntry("fr-CH", "IETF4646", "French (CH)")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(French_FR, DSRBasicCodedEntry("fr-FR", "IETF4646", "French (FR)")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(German, DSRBasicCodedEntry("deu", "IETF4646", "German")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(German_AT, DSRBasicCodedEntry("de-AT", "IETF4646", "German (AT)")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(German_CH, DSRBasicCodedEntry("de-CH", "IETF4646", "German (CH)")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(German_DE, DSRBasicCodedEntry("de-DE", "IETF4646", "German (DE)")));
    }
    /* should never be NULL */
    return *codes;
}
