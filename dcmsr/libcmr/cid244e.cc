/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID244e_Laterality
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid244e.h"


CID244e_Laterality::CID244e_Laterality(const DSRCodedEntryValue &selectedValue)
  : CID244_Laterality(selectedValue)
{
}


CID244e_Laterality::CID244e_Laterality(const EnumType selectedValue,
                                     const OFBool enhancedEncodingMode)
  : CID244_Laterality(selectedValue, enhancedEncodingMode)
{
}


CID244e_Laterality::CID244e_Laterality(const OFString &selectedValue,
                                       const OFBool enhancedEncodingMode)
  : CID244_Laterality(mapImageLaterality(selectedValue, enhancedEncodingMode))
{
}


OFCondition CID244e_Laterality::selectValue(const OFString &selectedValue,
                                            const OFBool enhancedEncodingMode)
{
    DSRCodedEntryValue codedEntry;
    /* map enumerated value to coded entry */
    OFCondition result = mapImageLaterality(selectedValue, codedEntry, enhancedEncodingMode);
    if (result.good())
    {
        /* select but never check the coded entry */
        result = selectValue(codedEntry, OFFalse /*check*/, OFFalse /*definedContextGroup*/);
    }
    return result;
}


// static functions

DSRCodedEntryValue CID244e_Laterality::mapImageLaterality(const OFString &enumeratedValue,
                                                          const OFBool enhancedEncodingMode)
{
    DSRCodedEntryValue codedEntry;
    /* call the real function (no need to check the returned status) */
    mapImageLaterality(enumeratedValue, codedEntry, enhancedEncodingMode);
    /* return coded entry (by-value) */
    return codedEntry;
}


OFCondition CID244e_Laterality::mapImageLaterality(const OFString &enumeratedValue,
                                                   DSRCodedEntryValue &codedEntryValue,
                                                   const OFBool enhancedEncodingMode)
{
    OFCondition result = EC_Normal;
    /* determine image laterality code from CID 244 */
    if (enumeratedValue == "R")       // right
        codedEntryValue = getCodedEntry(Right);
    else if (enumeratedValue == "L")  // left
        codedEntryValue = getCodedEntry(Left);
    else if (enumeratedValue == "B")  // both left and right
        codedEntryValue = getCodedEntry(Bilateral);
    else if (enumeratedValue == "U")  // unpaired
        codedEntryValue = getCodedEntry(Unilateral);
    else
        result = SR_EC_InvalidValue;
    /* set enhanced encoding mode (if enabled) */
    if (result.good() && enhancedEncodingMode)
        setEnhancedEncodingMode(codedEntryValue);
    return result;
}
