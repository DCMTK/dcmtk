/*
 *
 *  Copyright (C) 2022, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID218e_QuantitativeImageFeatures
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid218e.h"


CID218e_QuantitativeImageFeatures::CID218e_QuantitativeImageFeatures(const DSRCodedEntryValue &selectedValue)
  : CID218_QuantitativeImageFeatures(selectedValue)
{
}


CID218e_QuantitativeImageFeatures::CID218e_QuantitativeImageFeatures(const EnumType selectedValue,
                                                                     const OFBool enhancedEncodingMode)
  : CID218_QuantitativeImageFeatures(selectedValue, enhancedEncodingMode)
{
}


CID218e_QuantitativeImageFeatures::CID218e_QuantitativeImageFeatures(const CMR_CID7469::EnumType selectedValue,
                                                                     const OFBool enhancedEncodingMode)
  : CID218_QuantitativeImageFeatures(CMR_CID7469::getCodedEntry(selectedValue, enhancedEncodingMode))
{
}


OFCondition CID218e_QuantitativeImageFeatures::selectValue(const CMR_CID7469::EnumType selectedValue,
                                                           const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(CMR_CID7469::getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}
