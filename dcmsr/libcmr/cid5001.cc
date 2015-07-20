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
 *    classes: CID5001_Countries
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid5001.h"


// general information on CID 5001 (Countries)
#define CONTEXT_GROUP_NUMBER  "5001"
#define CONTEXT_GROUP_VERSION ""      /* unknown */
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.329"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible? */


CID5001_Countries::CID5001_Countries(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID5001_Countries::CID5001_Countries(const EnumType selectedValue,
                                     const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID5001_Countries::selectValue(const EnumType selectedValue,
                                           const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID5001_Countries::findCodedEntry(const DSRCodedEntryValue &codedEntryValue) const
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


void CID5001_Countries::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID5001_Countries::initialize()
{
    /* create and initialize code list */
    getCodes();
}


DSRCodedEntryValue CID5001_Countries::getCodedEntry(const EnumType value,
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


CID5001_Countries::CodeList &CID5001_Countries::getCodes()
{
    /* use a static variable for singleton pattern */
    static CodeList *codes = NULL;
    /* check whether code list has already been created and initialized */
    if (codes == NULL)
    {
        /* create a new code list (should never fail) */
        codes = new CodeList();
        /* and initialize it by adding the coded entries */
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Austria, DSRBasicCodedEntry("AT", "ISO3166_1", "Austria")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Canada, DSRBasicCodedEntry("CA", "ISO3166_1", "Canada")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(France, DSRBasicCodedEntry("FR", "ISO3166_1", "France")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Germany, DSRBasicCodedEntry("DE", "ISO3166_1", "Germany")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Switzerland, DSRBasicCodedEntry("CH", "ISO3166_1", "Switzerland")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(UnitedKingdom, DSRBasicCodedEntry("GB", "ISO3166_1", "United Kingdom of Great Britain and Northern Ireland")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(UnitedStates, DSRBasicCodedEntry("US", "ISO3166_1", "United States of America")));
    }
    /* should never be NULL */
    return *codes;
}
