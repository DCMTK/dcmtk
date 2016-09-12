/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID5000_Languages
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid5000.h"


// general information on CID 5000 (Languages)
#define CONTEXT_GROUP_NUMBER  "5000"
#define CONTEXT_GROUP_VERSION ""      /* unknown */
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.328"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible? */

// initialize global/static variable
CID5000_Languages::CodeList *CID5000_Languages::Codes = NULL;


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


void CID5000_Languages::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID5000_Languages::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID5000_Languages::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
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
            setEnhancedEncodingMode(codedEntry);
    }
    return codedEntry;
}


CID5000_Languages::CodeList &CID5000_Languages::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(English, DSRBasicCodedEntry("eng", "RFC5646", "English")));
        Codes->insert(OFMake_pair(English_CA, DSRBasicCodedEntry("en-CA", "RFC5646", "English (CA)")));
        Codes->insert(OFMake_pair(English_GB, DSRBasicCodedEntry("en-GB", "RFC5646", "English (GB)")));
        Codes->insert(OFMake_pair(English_US, DSRBasicCodedEntry("en-US", "RFC5646", "English (US)")));
        Codes->insert(OFMake_pair(French, DSRBasicCodedEntry("fra", "RFC5646", "French")));
        Codes->insert(OFMake_pair(French_CA, DSRBasicCodedEntry("fr-CA", "RFC5646", "French (CA)")));
        Codes->insert(OFMake_pair(French_CH, DSRBasicCodedEntry("fr-CH", "RFC5646", "French (CH)")));
        Codes->insert(OFMake_pair(French_FR, DSRBasicCodedEntry("fr-FR", "RFC5646", "French (FR)")));
        Codes->insert(OFMake_pair(German, DSRBasicCodedEntry("deu", "RFC5646", "German")));
        Codes->insert(OFMake_pair(German_AT, DSRBasicCodedEntry("de-AT", "RFC5646", "German (AT)")));
        Codes->insert(OFMake_pair(German_CH, DSRBasicCodedEntry("de-CH", "RFC5646", "German (CH)")));
        Codes->insert(OFMake_pair(German_DE, DSRBasicCodedEntry("de-DE", "RFC5646", "German (DE)")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID5000_Languages::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
