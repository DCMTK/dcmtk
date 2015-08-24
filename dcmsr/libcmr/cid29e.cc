/*
 *
 *  Copyright (C) 2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID29e_AcquisitionModality
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid29e.h"
#include "dcmtk/dcmsr/codes/dcm.h"


CID29e_AcquisitionModality::CID29e_AcquisitionModality(const DSRCodedEntryValue &selectedValue)
  : CID29_AcquisitionModality(selectedValue)
{
}


CID29e_AcquisitionModality::CID29e_AcquisitionModality(const EnumType selectedValue,
                                                       const OFBool enhancedEncodingMode)
  : CID29_AcquisitionModality(selectedValue, enhancedEncodingMode)
{
}


OFCondition CID29e_AcquisitionModality::selectValue(const OFString &selectedValue,
                                                    const OFBool enhancedEncodingMode)
{
    DSRCodedEntryValue codedEntry;
    /* map defined term to coded entry */
    OFCondition result = mapModality(selectedValue, codedEntry, enhancedEncodingMode);
    if (result.good())
    {
        /* select but never check the coded entry */
        result = selectValue(codedEntry, OFFalse /*check*/, OFFalse /*definedContextGroup*/);
    }
    return result;
}


DSRCodedEntryValue CID29e_AcquisitionModality::mapModality(const OFString &definedTerm,
                                                           const OFBool enhancedEncodingMode) const
{
    DSRCodedEntryValue codedEntry;
    /* call the real function (no need to check the returned status) */
    mapModality(definedTerm, codedEntry, enhancedEncodingMode);
    /* return coded entry (by-value) */
    return codedEntry;
}


OFCondition CID29e_AcquisitionModality::mapModality(const OFString &definedTerm,
                                                    DSRCodedEntryValue &codedEntryValue,
                                                    const OFBool enhancedEncodingMode) const
{
    OFCondition result = EC_Normal;
    /* determine modality code from CID 29 */
    if (findCodedEntry(DSRCodedEntryValue(definedTerm /*codeValue*/, CODE_DCM_CodingSchemeDesignator, "-" /*codeMeaning*/), &codedEntryValue, enhancedEncodingMode).bad())
        result = SR_EC_UnsupportedValue;
    /* set enhanced encoding mode (if enabled) */
    else if (enhancedEncodingMode)
        setEnhancedEncodingMode(codedEntryValue);
    return result;
}
