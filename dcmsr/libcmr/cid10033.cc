/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID10033_CTReconstructionAlgorithm
 *
 *  Generated automatically from DICOM PS 3.16-2019a
 *  File created on 2019-02-04 12:56:45 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid10033.h"


// general information on CID 10033 (CT Reconstruction Algorithm)
#define CONTEXT_GROUP_NUMBER  "10033"
#define CONTEXT_GROUP_VERSION "20130207"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.958"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID10033_CTReconstructionAlgorithm::CodeList *CID10033_CTReconstructionAlgorithm::Codes = NULL;


CID10033_CTReconstructionAlgorithm::CID10033_CTReconstructionAlgorithm(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID10033_CTReconstructionAlgorithm::CID10033_CTReconstructionAlgorithm(const EnumType selectedValue,
                                                                       const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID10033_CTReconstructionAlgorithm::selectValue(const EnumType selectedValue,
                                                            const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID10033_CTReconstructionAlgorithm::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID10033_CTReconstructionAlgorithm::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID10033_CTReconstructionAlgorithm::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID10033_CTReconstructionAlgorithm::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID10033_CTReconstructionAlgorithm::getCodedEntry(const EnumType value,
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


CID10033_CTReconstructionAlgorithm::CodeList &CID10033_CTReconstructionAlgorithm::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(FilteredBackProjection, DSRBasicCodedEntry("113962", "DCM", "Filtered Back Projection")));
        Codes->insert(OFMake_pair(IterativeReconstruction, DSRBasicCodedEntry("113963", "DCM", "Iterative Reconstruction")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID10033_CTReconstructionAlgorithm::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
