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
 *    classes: CID42_NumericValueQualifier
 *
 *    Generated automatically from DICOM PS 3.16-2015c
 *    File created on 2015-07-20 09:43:00 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid42.h"


// general information on CID 42 (Numeric Value Qualifier)
#define CONTEXT_GROUP_NUMBER  "42"
#define CONTEXT_GROUP_VERSION "20020114"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.22"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */


CID42_NumericValueQualifier::CID42_NumericValueQualifier(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID42_NumericValueQualifier::CID42_NumericValueQualifier(const EnumType selectedValue,
                                                         const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID42_NumericValueQualifier::selectValue(const EnumType selectedValue,
                                                     const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID42_NumericValueQualifier::findCodedEntry(const DSRCodedEntryValue &codedEntryValue) const
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


void CID42_NumericValueQualifier::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID42_NumericValueQualifier::initialize()
{
    /* create and initialize code list */
    getCodes();
}


DSRCodedEntryValue CID42_NumericValueQualifier::getCodedEntry(const EnumType value,
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


CID42_NumericValueQualifier::CodeList &CID42_NumericValueQualifier::getCodes()
{
    /* use a static variable for singleton pattern */
    static CodeList *codes = NULL;
    /* check whether code list has already been created and initialized */
    if (codes == NULL)
    {
        /* create a new code list (should never fail) */
        codes = new CodeList();
        /* and initialize it by adding the coded entries */
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(NotANumber, DSRBasicCodedEntry("114000", "DCM", "Not a number")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(NegativeInfinity, DSRBasicCodedEntry("114001", "DCM", "Negative Infinity")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(PositiveInfinity, DSRBasicCodedEntry("114002", "DCM", "Positive Infinity")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(DivideByZero, DSRBasicCodedEntry("114003", "DCM", "Divide by zero")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Underflow, DSRBasicCodedEntry("114004", "DCM", "Underflow")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Overflow, DSRBasicCodedEntry("114005", "DCM", "Overflow")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(MeasurementFailure, DSRBasicCodedEntry("114006", "DCM", "Measurement failure")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(MeasurementNotAttempted, DSRBasicCodedEntry("114007", "DCM", "Measurement not attempted")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(CalculationFailure, DSRBasicCodedEntry("114008", "DCM", "Calculation failure")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ValueOutOfRange, DSRBasicCodedEntry("114009", "DCM", "Value out of range")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ValueUnknown, DSRBasicCodedEntry("114010", "DCM", "Value unknown")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ValueIndeterminate, DSRBasicCodedEntry("114011", "DCM", "Value indeterminate")));
    }
    /* should never be NULL */
    return *codes;
}
