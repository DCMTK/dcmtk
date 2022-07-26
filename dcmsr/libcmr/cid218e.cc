/*
 *
 *  Copyright (C) 2022, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID218e_QuantitativeImageFeature
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid218e.h"


CID218e_QuantitativeImageFeature::CID218e_QuantitativeImageFeature(const DSRCodedEntryValue &selectedValue)
  : CID218_QuantitativeImageFeature(selectedValue)
{
}


CID218e_QuantitativeImageFeature::CID218e_QuantitativeImageFeature(const EnumType selectedValue,
                                                                   const OFBool enhancedEncodingMode)
  : CID218_QuantitativeImageFeature(selectedValue, enhancedEncodingMode)
{
}


CID218e_QuantitativeImageFeature::CID218e_QuantitativeImageFeature(const CMR_CID7469::EnumType selectedValue,
                                                                   const OFBool enhancedEncodingMode)
  : CID218_QuantitativeImageFeature(CMR_CID7469::getCodedEntry(selectedValue, enhancedEncodingMode))
{
}


OFCondition CID218e_QuantitativeImageFeature::selectValue(const CMR_CID7469::EnumType selectedValue,
                                                          const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(CMR_CID7469::getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}
