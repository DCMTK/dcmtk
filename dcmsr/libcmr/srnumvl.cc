/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CMR_SRNumericMeasurementValue
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/srnumvl.h"


CMR_SRNumericMeasurementValue::CMR_SRNumericMeasurementValue()
  : DSRNumericMeasurementValue()
{
}


CMR_SRNumericMeasurementValue::CMR_SRNumericMeasurementValue(const OFString &numericValue,
                                                             const DSRCodedEntryValue &measurementUnit,
                                                             const OFBool check)
  : DSRNumericMeasurementValue(numericValue, measurementUnit, check)
{
}


CMR_SRNumericMeasurementValue::CMR_SRNumericMeasurementValue(const CID42_NumericValueQualifier &valueQualifier,
                                                             const OFBool check)
  : DSRNumericMeasurementValue(valueQualifier, check)
{
}


CMR_SRNumericMeasurementValue::CMR_SRNumericMeasurementValue(const OFString &numericValue,
                                                             const DSRCodedEntryValue &measurementUnit,
                                                             const CID42_NumericValueQualifier &valueQualifier,
                                                             const OFBool check)
  : DSRNumericMeasurementValue(numericValue, measurementUnit, valueQualifier, check)
{
}


CMR_SRNumericMeasurementValue::CMR_SRNumericMeasurementValue(const CMR_SRNumericMeasurementValue &numericMeasurement)
  : DSRNumericMeasurementValue(numericMeasurement)
{
}


CMR_SRNumericMeasurementValue::CMR_SRNumericMeasurementValue(const DSRNumericMeasurementValue &numericMeasurement)
  : DSRNumericMeasurementValue(numericMeasurement)
{
}


CMR_SRNumericMeasurementValue::~CMR_SRNumericMeasurementValue()
{
}


CMR_SRNumericMeasurementValue &CMR_SRNumericMeasurementValue::operator=(const CMR_SRNumericMeasurementValue &numericMeasurement)
{
    /* just call the inherited method */
    DSRNumericMeasurementValue::operator=(numericMeasurement);
    return *this;
}


OFCondition CMR_SRNumericMeasurementValue::setValue(const CID42_NumericValueQualifier &valueQualifier,
                                                    const OFBool check)
{
    /* map type to coded entry and call the method doing the real work */
    return DSRNumericMeasurementValue::setValue(valueQualifier, check);
}


OFCondition CMR_SRNumericMeasurementValue::setValue(const OFString &numericValue,
                                                    const DSRCodedEntryValue &measurementUnit,
                                                    const CID42_NumericValueQualifier &valueQualifier,
                                                    const OFBool check)
{
    /* map type to coded entry and call the method doing the real work */
    return DSRNumericMeasurementValue::setValue(numericValue, measurementUnit, valueQualifier, check);
}


OFCondition CMR_SRNumericMeasurementValue::setNumericValueQualifier(const CID42_NumericValueQualifier &valueQualifier,
                                                                    const OFBool check)
{
    /* map type to coded entry and call the method doing the real work */
    return DSRNumericMeasurementValue::setNumericValueQualifier(valueQualifier, check);
}


OFCondition CMR_SRNumericMeasurementValue::checkNumericValueQualifier(const DSRCodedEntryValue &valueQualifier) const
{
    /* first, perform some basic checks (done by the base class) */
    OFCondition result = DSRNumericMeasurementValue::checkNumericValueQualifier(valueQualifier);
    if (result.good() && !valueQualifier.isEmpty())
    {
        /* then, also check for conformance with CID 42 */
        static const CID42_NumericValueQualifier contextGroup;
        if (contextGroup.findCodedEntry(valueQualifier).bad())
            result = SR_EC_CodedEntryNotInContextGroup;
    }
    return result;
}
