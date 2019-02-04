/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID42_NumericValueQualifier
 *
 *  Generated automatically from DICOM PS 3.16-2019a
 *  File created on 2019-02-04 12:56:16 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid42.h"


// general information on CID 42 (Numeric Value Qualifier)
#define CONTEXT_GROUP_NUMBER  "42"
#define CONTEXT_GROUP_VERSION "20020114"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.22"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID42_NumericValueQualifier::CodeList *CID42_NumericValueQualifier::Codes = NULL;


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


OFCondition CID42_NumericValueQualifier::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID42_NumericValueQualifier::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID42_NumericValueQualifier::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID42_NumericValueQualifier::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
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
            setEnhancedEncodingMode(codedEntry);
    }
    return codedEntry;
}


CID42_NumericValueQualifier::CodeList &CID42_NumericValueQualifier::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(NotANumber, DSRBasicCodedEntry("114000", "DCM", "Not a number")));
        Codes->insert(OFMake_pair(NegativeInfinity, DSRBasicCodedEntry("114001", "DCM", "Negative Infinity")));
        Codes->insert(OFMake_pair(PositiveInfinity, DSRBasicCodedEntry("114002", "DCM", "Positive Infinity")));
        Codes->insert(OFMake_pair(DivideByZero, DSRBasicCodedEntry("114003", "DCM", "Divide by zero")));
        Codes->insert(OFMake_pair(Underflow, DSRBasicCodedEntry("114004", "DCM", "Underflow")));
        Codes->insert(OFMake_pair(Overflow, DSRBasicCodedEntry("114005", "DCM", "Overflow")));
        Codes->insert(OFMake_pair(MeasurementFailure, DSRBasicCodedEntry("114006", "DCM", "Measurement failure")));
        Codes->insert(OFMake_pair(MeasurementNotAttempted, DSRBasicCodedEntry("114007", "DCM", "Measurement not attempted")));
        Codes->insert(OFMake_pair(CalculationFailure, DSRBasicCodedEntry("114008", "DCM", "Calculation failure")));
        Codes->insert(OFMake_pair(ValueOutOfRange, DSRBasicCodedEntry("114009", "DCM", "Value out of range")));
        Codes->insert(OFMake_pair(ValueUnknown, DSRBasicCodedEntry("114010", "DCM", "Value unknown")));
        Codes->insert(OFMake_pair(ValueIndeterminate, DSRBasicCodedEntry("114011", "DCM", "Value indeterminate")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID42_NumericValueQualifier::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
