/*
 *
 *  Copyright (C) 2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID10013e_CTAcquisitionType
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid10013e.h"


CID10013e_CTAcquisitionType::CID10013e_CTAcquisitionType(const DSRCodedEntryValue &selectedValue)
  : CID10013_CTAcquisitionType(selectedValue)
{
}


CID10013e_CTAcquisitionType::CID10013e_CTAcquisitionType(const EnumType selectedValue,
                                                         const OFBool enhancedEncodingMode)
  : CID10013_CTAcquisitionType(selectedValue, enhancedEncodingMode)
{
}


CID10013e_CTAcquisitionType::CID10013e_CTAcquisitionType(const OFString &selectedValue,
                                                         const OFBool enhancedEncodingMode)
  : CID10013_CTAcquisitionType(mapAcquisitionType(selectedValue, enhancedEncodingMode))
{
}


OFCondition CID10013e_CTAcquisitionType::selectValue(const OFString &selectedValue,
                                                     const OFBool enhancedEncodingMode)
{
    DSRCodedEntryValue codedEntry;
    /* map enumerated value to coded entry */
    OFCondition result = mapAcquisitionType(selectedValue, codedEntry, enhancedEncodingMode);
    if (result.good())
    {
        /* select but never check the coded entry */
        result = selectValue(codedEntry, OFFalse /*check*/, OFFalse /*definedContextGroup*/);
    }
    return result;
}


// static functions

DSRCodedEntryValue CID10013e_CTAcquisitionType::mapAcquisitionType(const OFString &definedTerm,
                                                                   const OFBool enhancedEncodingMode)
{
    DSRCodedEntryValue codedEntry;
    /* call the real function (no need to check the returned status) */
    mapAcquisitionType(definedTerm, codedEntry, enhancedEncodingMode);
    /* return coded entry (by-value) */
    return codedEntry;
}


OFCondition CID10013e_CTAcquisitionType::mapAcquisitionType(const OFString &definedTerm,
                                                            DSRCodedEntryValue &codedEntryValue,
                                                            const OFBool enhancedEncodingMode)
{
    OFCondition result = EC_Normal;
    /* determine acquisition type code from CID 10013 */
    if (definedTerm == "SEQUENCED")
        codedEntryValue = getCodedEntry(SequencedAcquisition);
    else if (definedTerm == "SPIRAL")
        codedEntryValue = getCodedEntry(SpiralAcquisition);
    else if (definedTerm == "CONSTANT_ANGLE")
        codedEntryValue = getCodedEntry(ConstantAngleAcquisition);
    else if (definedTerm == "STATIONARY")
        codedEntryValue = getCodedEntry(StationaryAcquisition);
    else if (definedTerm == "FREE")
        codedEntryValue = getCodedEntry(FreeAcquisition);
    else
        result = SR_EC_UnsupportedValue;
    /* set enhanced encoding mode (if enabled) */
    if (result.good() && enhancedEncodingMode)
        setEnhancedEncodingMode(codedEntryValue);
    return result;
}
