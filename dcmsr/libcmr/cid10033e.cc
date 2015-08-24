/*
 *
 *  Copyright (C) 2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID10033e_CTReconstructionAlgorithm
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid10033e.h"


CID10033e_CTReconstructionAlgorithm::CID10033e_CTReconstructionAlgorithm(const DSRCodedEntryValue &selectedValue)
  : CID10033_CTReconstructionAlgorithm(selectedValue)
{
}


CID10033e_CTReconstructionAlgorithm::CID10033e_CTReconstructionAlgorithm(const EnumType selectedValue,
                                                                         const OFBool enhancedEncodingMode)
  : CID10033_CTReconstructionAlgorithm(selectedValue, enhancedEncodingMode)
{
}


CID10033e_CTReconstructionAlgorithm::CID10033e_CTReconstructionAlgorithm(const OFString &selectedValue,
                                                                         const OFBool enhancedEncodingMode)
  : CID10033_CTReconstructionAlgorithm(mapReconstructionAlgorithm(selectedValue, enhancedEncodingMode))
{
}


OFCondition CID10033e_CTReconstructionAlgorithm::selectValue(const OFString &selectedValue,
                                                             const OFBool enhancedEncodingMode)
{
    DSRCodedEntryValue codedEntry;
    /* map enumerated value to coded entry */
    OFCondition result = mapReconstructionAlgorithm(selectedValue, codedEntry, enhancedEncodingMode);
    if (result.good())
    {
        /* select but never check the coded entry */
        result = selectValue(codedEntry, OFFalse /*check*/, OFFalse /*definedContextGroup*/);
    }
    return result;
}


// static functions

DSRCodedEntryValue CID10033e_CTReconstructionAlgorithm::mapReconstructionAlgorithm(const OFString &definedTerm,
                                                                                   const OFBool enhancedEncodingMode)
{
    DSRCodedEntryValue codedEntry;
    /* call the real function (no need to check the returned status) */
    mapReconstructionAlgorithm(definedTerm, codedEntry, enhancedEncodingMode);
    /* return coded entry (by-value) */
    return codedEntry;
}


OFCondition CID10033e_CTReconstructionAlgorithm::mapReconstructionAlgorithm(const OFString &definedTerm,
                                                                            DSRCodedEntryValue &codedEntryValue,
                                                                            const OFBool enhancedEncodingMode)
{
    OFCondition result = EC_Normal;
    /* determine reconstruction algorithm code from CID 10033 */
    if (definedTerm == "FILTER_BACK_PROJ")
        codedEntryValue = getCodedEntry(FilteredBackProjection);
    else if (definedTerm == "ITERATIVE")
        codedEntryValue = getCodedEntry(IterativeReconstruction);
    else
        result = SR_EC_UnsupportedValue;
    /* set enhanced encoding mode (if enabled) */
    if (result.good() && enhancedEncodingMode)
        setEnhancedEncodingMode(codedEntryValue);
    return result;
}
