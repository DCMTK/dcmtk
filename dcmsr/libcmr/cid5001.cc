/*
 *
 *  Copyright (C) 2015-2022, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID5001_Country
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid5001.h"


// general information on CID 5001 (Country)
#define CONTEXT_GROUP_NUMBER  "5001"
#define CONTEXT_GROUP_VERSION ""      /* unknown */
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.329"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible? */

// initialize global/static variable
CID5001_Country::CodeList *CID5001_Country::Codes = NULL;


CID5001_Country::CID5001_Country(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID5001_Country::CID5001_Country(const EnumType selectedValue,
                                 const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID5001_Country::selectValue(const EnumType selectedValue,
                                         const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID5001_Country::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID5001_Country::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID5001_Country::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID5001_Country::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID5001_Country::getCodedEntry(const EnumType value,
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


CID5001_Country::CodeList &CID5001_Country::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(Austria, DSRBasicCodedEntry("AT", "ISO3166_1", "Austria")));
        Codes->insert(OFMake_pair(Canada, DSRBasicCodedEntry("CA", "ISO3166_1", "Canada")));
        Codes->insert(OFMake_pair(France, DSRBasicCodedEntry("FR", "ISO3166_1", "France")));
        Codes->insert(OFMake_pair(Germany, DSRBasicCodedEntry("DE", "ISO3166_1", "Germany")));
        Codes->insert(OFMake_pair(Switzerland, DSRBasicCodedEntry("CH", "ISO3166_1", "Switzerland")));
        Codes->insert(OFMake_pair(UnitedKingdom, DSRBasicCodedEntry("GB", "ISO3166_1", "United Kingdom of Great Britain and Northern Ireland")));
        Codes->insert(OFMake_pair(UnitedStates, DSRBasicCodedEntry("US", "ISO3166_1", "United States of America")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID5001_Country::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
