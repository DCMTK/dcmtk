/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID10013_CTAcquisitionType
 *
 *  Generated automatically from DICOM PS 3.16-2019a
 *  File created on 2019-02-04 12:56:43 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid10013.h"


// general information on CID 10013 (CT Acquisition Type)
#define CONTEXT_GROUP_NUMBER  "10013"
#define CONTEXT_GROUP_VERSION "20160314"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.545"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID10013_CTAcquisitionType::CodeList *CID10013_CTAcquisitionType::Codes = NULL;


CID10013_CTAcquisitionType::CID10013_CTAcquisitionType(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID10013_CTAcquisitionType::CID10013_CTAcquisitionType(const EnumType selectedValue,
                                                       const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID10013_CTAcquisitionType::selectValue(const EnumType selectedValue,
                                                    const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID10013_CTAcquisitionType::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID10013_CTAcquisitionType::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID10013_CTAcquisitionType::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID10013_CTAcquisitionType::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID10013_CTAcquisitionType::getCodedEntry(const EnumType value,
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


CID10013_CTAcquisitionType::CodeList &CID10013_CTAcquisitionType::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(SequencedAcquisition, DSRBasicCodedEntry("113804", "DCM", "Sequenced Acquisition")));
        Codes->insert(OFMake_pair(SpiralAcquisition, DSRBasicCodedEntry("P5-08001", "SRT", "Spiral Acquisition")));
        Codes->insert(OFMake_pair(ConstantAngleAcquisition, DSRBasicCodedEntry("113805", "DCM", "Constant Angle Acquisition")));
        Codes->insert(OFMake_pair(StationaryAcquisition, DSRBasicCodedEntry("113806", "DCM", "Stationary Acquisition")));
        Codes->insert(OFMake_pair(FreeAcquisition, DSRBasicCodedEntry("113807", "DCM", "Free Acquisition")));
        Codes->insert(OFMake_pair(ConeBeamAcquisition, DSRBasicCodedEntry("R-FB8F1", "SRT", "Cone Beam Acquisition")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID10013_CTAcquisitionType::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
