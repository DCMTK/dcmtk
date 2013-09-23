/*
 *
 *  Copyright (C) 2000-2013, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRNumericMeasurementValue
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrnumvl.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRNumericMeasurementValue::DSRNumericMeasurementValue()
  : NumericValue(),
    MeasurementUnit(),
    ValueQualifier(),
    FloatingPointValue(DCM_FloatingPointValue),
    RationalNumeratorValue(DCM_RationalNumeratorValue),
    RationalDenominatorValue(DCM_RationalDenominatorValue)
{
}


DSRNumericMeasurementValue::DSRNumericMeasurementValue(const OFString &numericValue,
                                                       const DSRCodedEntryValue &measurementUnit,
                                                       const OFBool check)
  : NumericValue(),
    MeasurementUnit(),
    ValueQualifier(),
    FloatingPointValue(DCM_FloatingPointValue),
    RationalNumeratorValue(DCM_RationalNumeratorValue),
    RationalDenominatorValue(DCM_RationalDenominatorValue)
{
    /* use the set method for checking purposes */
    setValue(numericValue, measurementUnit, check);
}


DSRNumericMeasurementValue::DSRNumericMeasurementValue(const OFString &numericValue,
                                                       const DSRCodedEntryValue &measurementUnit,
                                                       const DSRCodedEntryValue &valueQualifier,
                                                       const OFBool check)
  : NumericValue(),
    MeasurementUnit(),
    ValueQualifier(),
    FloatingPointValue(DCM_FloatingPointValue),
    RationalNumeratorValue(DCM_RationalNumeratorValue),
    RationalDenominatorValue(DCM_RationalDenominatorValue)
{
    /* use the set method for checking purposes */
    setValue(numericValue, measurementUnit, valueQualifier, check);
}


DSRNumericMeasurementValue::DSRNumericMeasurementValue(const DSRNumericMeasurementValue &numericMeasurement)
  : NumericValue(numericMeasurement.NumericValue),
    MeasurementUnit(numericMeasurement.MeasurementUnit),
    ValueQualifier(numericMeasurement.ValueQualifier),
    FloatingPointValue(numericMeasurement.FloatingPointValue),
    RationalNumeratorValue(numericMeasurement.RationalNumeratorValue),
    RationalDenominatorValue(numericMeasurement.RationalDenominatorValue)
{
    /* do not check since this would be unexpected to the user */
}


DSRNumericMeasurementValue::~DSRNumericMeasurementValue()
{
}


DSRNumericMeasurementValue &DSRNumericMeasurementValue::operator=(const DSRNumericMeasurementValue &numericMeasurement)
{
    /* do not check since this would be unexpected to the user */
    NumericValue = numericMeasurement.NumericValue;
    MeasurementUnit = numericMeasurement.MeasurementUnit;
    ValueQualifier = numericMeasurement.ValueQualifier;
    FloatingPointValue = numericMeasurement.FloatingPointValue;
    RationalNumeratorValue = numericMeasurement.RationalNumeratorValue;
    RationalDenominatorValue = numericMeasurement.RationalDenominatorValue;
    return *this;
}


void DSRNumericMeasurementValue::clear()
{
    NumericValue.clear();
    MeasurementUnit.clear();
    ValueQualifier.clear();
    FloatingPointValue.clear();
    RationalNumeratorValue.clear();
    RationalDenominatorValue.clear();
}


OFBool DSRNumericMeasurementValue::isValid() const
{
    /* the MeasuredValueSequence can be empty (type 2) */
    return isEmpty() || (checkNumericValue(NumericValue).good() &&
                         checkMeasurementUnit(MeasurementUnit).good() &&
                         checkNumericValueQualifier(ValueQualifier).good());
}


OFBool DSRNumericMeasurementValue::isEmpty() const
{
    return NumericValue.empty() && MeasurementUnit.isEmpty();
}


OFCondition DSRNumericMeasurementValue::print(STD_NAMESPACE ostream &stream,
                                              const size_t flags) const
{
    if (isEmpty())
    {
        /* empty value */
        stream << "empty";
    } else {
        OFString printString;
        stream << "\"" << DSRTypes::convertToPrintString(NumericValue, printString) << "\" ";
        MeasurementUnit.print(stream, OFTrue /*printCodeValue*/, (flags & DSRTypes::PF_printInvalidCodes) > 0 /*printInvalid*/);
    }
    return EC_Normal;
}


OFCondition DSRNumericMeasurementValue::readXML(const DSRXMLDocument &doc,
                                                DSRXMLCursor cursor)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        cursor.gotoChild();
        /* get "value" element (might be absent since "Measured Value Sequence" is type 2) */
        if (!doc.getStringFromNodeContent(doc.getNamedNode(cursor, "value", OFFalse /*required*/), NumericValue).empty())
        {
            /* get additional representations of the numeric value (if any) */
            doc.getElementFromNodeContent(doc.getNamedNode(cursor, "float", OFFalse /*required*/), FloatingPointValue);
            const DSRXMLCursor childNode = doc.getNamedNode(cursor, "rational", OFFalse /*required*/).getChild();
            if (childNode.valid())
            {
                doc.getElementFromNodeContent(doc.getNamedNode(childNode, "numerator"), RationalNumeratorValue);
                doc.getElementFromNodeContent(doc.getNamedNode(childNode, "denominator"), RationalDenominatorValue);
            }
            /* get "unit" element (only if "value" present) */
            result = MeasurementUnit.readXML(doc, doc.getNamedNode(cursor, "unit"));
        } else
            result = EC_Normal;
        if (result.good())
        {
            /* get "qualifier" element (optional, do not report if absent or erroneous) */
            ValueQualifier.readXML(doc, doc.getNamedNode(cursor, "qualifier", OFFalse /*required*/));
        }
        if (!isValid())
            result = SR_EC_InvalidValue;
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::writeXML(STD_NAMESPACE ostream &stream,
                                                 const size_t flags) const
{
    /* write numeric value */
    DSRTypes::writeStringValueToXML(stream, NumericValue, "value", (flags & DSRTypes::XF_writeEmptyTags) > 0);
    /* write floating point representation */
    Float64 floatValue;
    const OFBool hasFloating = getFloatingPointRepresentation(floatValue).good();
    if (hasFloating || (flags & DSRTypes::XF_writeEmptyTags))
    {
        stream << "<float>";
        if (hasFloating)
        {
            /* increase default precision */
            const STD_NAMESPACE streamsize oldPrecision = stream.precision(8);
            stream << floatValue;
            /* reset i/o manipulators */
            stream.precision(oldPrecision);
        }
        stream << "</float>" << OFendl;
    }
    /* write rational representation */
    Sint32 numeratorValue;
    Uint32 denominatorValue;
    const OFBool hasRational = getRationalRepresentation(numeratorValue, denominatorValue).good();
    if (hasRational || (flags & DSRTypes::XF_writeEmptyTags))
    {
        stream << "<rational>" << OFendl;
        if (hasRational)
        {
            stream << "<numerator>" << numeratorValue << "</numerator>" << OFendl;
            stream << "<denominator>" << denominatorValue << "</denominator>" << OFendl;
        }
        stream << "</rational>" << OFendl;
    }
    /* write measurement unit */
    if (!MeasurementUnit.isEmpty() || (flags & DSRTypes::XF_writeEmptyTags))
    {
        if (flags & DSRTypes::XF_codeComponentsAsAttribute)
            stream << "<unit";     // bracket ">" is closed in the next writeXML() routine
        else
            stream << "<unit>" << OFendl;
        MeasurementUnit.writeXML(stream, flags);
        stream << "</unit>" << OFendl;
    }
    /* write value qualifier */
    if (!ValueQualifier.isEmpty() || (flags & DSRTypes::XF_writeEmptyTags))
    {
        if (flags & DSRTypes::XF_codeComponentsAsAttribute)
            stream << "<qualifier";     // bracket ">" is closed in the next writeXML() routine
        else
            stream << "<qualifier>" << OFendl;
        ValueQualifier.writeXML(stream, flags);
        stream << "</qualifier>" << OFendl;
    }
    return EC_Normal;
}


OFCondition DSRNumericMeasurementValue::readItem(DcmItem &dataset)
{
    /* read NumericValue */
    OFCondition result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_NumericValue, NumericValue, "1", "1", "MeasuredValueSequence");
    if (result.good())
    {
        /* read some optional attributes */
        DSRTypes::getAndCheckElementFromDataset(dataset, FloatingPointValue, "1", "1C", "MeasuredValueSequence");
        if (DSRTypes::getAndCheckElementFromDataset(dataset, RationalNumeratorValue, "1", "1C", "MeasuredValueSequence").good())
            DSRTypes::getAndCheckElementFromDataset(dataset, RationalDenominatorValue, "1", "1" /* was 1C */, "MeasuredValueSequence");
        /* read MeasurementUnitsCodeSequence */
        result = MeasurementUnit.readSequence(dataset, DCM_MeasurementUnitsCodeSequence, "1" /*type*/);
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::writeItem(DcmItem &dataset) const
{
    /* write NumericValue */
    OFCondition result = DSRTypes::putStringValueToDataset(dataset, DCM_NumericValue, NumericValue);
    /* write some optional attributes */
    DSRTypes::addElementToDataset(result, dataset, new DcmFloatingPointDouble(FloatingPointValue), "1", "1C", "MeasuredValueSequence");
    DSRTypes::addElementToDataset(result, dataset, new DcmSignedLong(RationalNumeratorValue), "1", "1C", "MeasuredValueSequence");
    DSRTypes::addElementToDataset(result, dataset, new DcmUnsignedLong(RationalDenominatorValue), "1", "1C", "MeasuredValueSequence");
    /* write MeasurementUnitsCodeSequence */
    if (result.good())
        result = MeasurementUnit.writeSequence(dataset, DCM_MeasurementUnitsCodeSequence);
    return result;
}


OFCondition DSRNumericMeasurementValue::readSequence(DcmItem &dataset)
{
    /* read MeasuredValueSequence */
    DcmSequenceOfItems dseq(DCM_MeasuredValueSequence);
    OFCondition result = DSRTypes::getElementFromDataset(dataset, dseq);
    DSRTypes::checkElementValue(dseq, "1", "2", result, "NUM content item");
    if (result.good())
    {
        /* check for empty sequence (allowed!) */
        if (!dseq.isEmpty())
        {
            /* read first item */
            DcmItem *ditem = dseq.getItem(0);
            if (ditem != NULL)
                result = readItem(*ditem);
            else
                result = SR_EC_InvalidDocumentTree;
        }
    }
    if (result.good())
    {
        /* read NumericValueQualifierCodeSequence (optional) */
        ValueQualifier.readSequence(dataset, DCM_NumericValueQualifierCodeSequence, "3" /*type*/);
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::writeSequence(DcmItem &dataset) const
{
    OFCondition result = EC_MemoryExhausted;
    /* write MeasuredValueSequence */
    DcmSequenceOfItems *dseq = new DcmSequenceOfItems(DCM_MeasuredValueSequence);
    if (dseq != NULL)
    {
        /* check for empty value */
        if (isEmpty())
            result = EC_Normal;
        else
        {
            DcmItem *ditem = new DcmItem();
            if (ditem != NULL)
            {
                /* write item */
                result = writeItem(*ditem);
                if (result.good())
                    dseq->insert(ditem);
                else
                    delete ditem;
            } else
                result = EC_MemoryExhausted;
        }
        /* write sequence (might be empty) */
        if (result.good())
            result = dataset.insert(dseq, OFTrue /*replaceOld*/);
        if (result.bad())
            delete dseq;
    }
    if (result.good())
    {
        /* write NumericValueQualifierCodeSequence (optional) */
        if (!ValueQualifier.isEmpty())
            ValueQualifier.writeSequence(dataset, DCM_NumericValueQualifierCodeSequence);
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::renderHTML(STD_NAMESPACE ostream &docStream,
                                                   STD_NAMESPACE ostream & /*annexStream*/,
                                                   size_t & /*annexNumber*/,
                                                   const size_t flags) const
{
    if (isEmpty())
    {
        /* empty value */
        docStream << "<i>empty</i>";
    } else {
        OFString htmlString;
        const OFBool fullCode = (flags & DSRTypes::HF_renderNumericUnitCodes) &&
            ((flags & DSRTypes::HF_renderInlineCodes) || (flags & DSRTypes::HF_renderItemsSeparately));
        if (!fullCode || (flags & DSRTypes::HF_useCodeDetailsTooltip))
        {
            if (flags & DSRTypes::HF_XHTML11Compatibility)
                docStream << "<span class=\"num\">";
            else if (flags & DSRTypes::HF_HTML32Compatibility)
                docStream << "<u>";
            else /* HTML 4.01 */
                docStream << "<span class=\"under\">";
        }
        docStream << DSRTypes::convertToHTMLString(NumericValue, htmlString, flags) << " ";
        /* render full code of the measurement unit (value first?) or code value only */
        MeasurementUnit.renderHTML(docStream, flags, fullCode, (flags & DSRTypes::HF_useCodeMeaningAsUnit) == 0 /*valueFirst*/);
        if (!fullCode || (flags & DSRTypes::HF_useCodeDetailsTooltip))
        {
            if (flags & DSRTypes::HF_HTML32Compatibility)
                docStream << "</u>";
            else
                docStream << "</span>";
        }
    }
    if (!ValueQualifier.isEmpty())
    {
        /* render optional numeric value qualifier */
        docStream << " [";
        ValueQualifier.renderHTML(docStream, flags, (flags & DSRTypes::HF_renderInlineCodes) > 0 /*fullCode*/);
        docStream << "]";
    }
    return EC_Normal;
}


OFCondition DSRNumericMeasurementValue::getValue(DSRNumericMeasurementValue &numericMeasurement) const
{
    numericMeasurement = *this;
    return EC_Normal;
}


OFCondition DSRNumericMeasurementValue::getMeasurementUnit(DSRCodedEntryValue &measurementUnit) const
{
    measurementUnit = MeasurementUnit;
    return EC_Normal;
}


OFCondition DSRNumericMeasurementValue::getNumericValueQualifier(DSRCodedEntryValue &valueQualifier) const
{
    valueQualifier = ValueQualifier;
    return EC_Normal;
}


OFCondition DSRNumericMeasurementValue::getFloatingPointRepresentation(Float64 &floatingPoint) const
{
    OFCondition result = SR_EC_RepresentationNotAvailable;
    /* cast away the const specifier (yes, this is ugly) */
    DcmFloatingPointDouble &floatElement = OFconst_cast(DcmFloatingPointDouble &, FloatingPointValue);
    if (!floatElement.isEmpty())
        result = floatElement.getFloat64(floatingPoint);
    return result;
}


OFCondition DSRNumericMeasurementValue::getRationalRepresentation(Sint32 &rationalNumerator,
                                                                  Uint32 &rationalDenominator) const
{
    OFCondition result = SR_EC_RepresentationNotAvailable;
    /* cast away the const specifier (yes, this is ugly) */
    DcmSignedLong &signedElement = OFconst_cast(DcmSignedLong &, RationalNumeratorValue);
    DcmUnsignedLong &unsignedElement = OFconst_cast(DcmUnsignedLong &, RationalDenominatorValue);
    /* check whether both values are present or absent */
    if (signedElement.isEmpty() != unsignedElement.isEmpty())
        result = EC_CorruptedData;
    /* determine rational numerator and denominator (if present) */
    else if (!signedElement.isEmpty() && !unsignedElement.isEmpty())
    {
        result = signedElement.getSint32(rationalNumerator);
        if (result.good())
            result = unsignedElement.getUint32(rationalDenominator);
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::setValue(const DSRNumericMeasurementValue &numericMeasurement,
                                                 const OFBool check)
{
    /* first set the basic parameters */
    OFCondition result = setValue(numericMeasurement.NumericValue,
                                  numericMeasurement.MeasurementUnit,
                                  numericMeasurement.ValueQualifier,
                                  check);
    /* then the additional representations */
    if (result.good())
    {
        Float64 floatValue;
        if (numericMeasurement.getFloatingPointRepresentation(floatValue).good())
            result = setFloatingPointRepresentation(floatValue, check);
        if (result.good())
        {
            Sint32 numeratorValue;
            Uint32 denominatorValue;
            if (numericMeasurement.getRationalRepresentation(numeratorValue, denominatorValue).good())
                result = setRationalRepresentation(numeratorValue, denominatorValue, check);
        }
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::setValue(const OFString &numericValue,
                                                 const DSRCodedEntryValue &measurementUnit,
                                                 const OFBool check)
{
    OFCondition result = EC_Normal;
    if (check)
    {
        /* check whether the passed values are valid */
        result = checkNumericValue(numericValue);
        if (result.good())
            result = checkMeasurementUnit(measurementUnit);
    } else {
        /* make sure that the mandatory values are non-empty */
        if (measurementUnit.isEmpty())
            result = EC_IllegalParameter;
    }
    if (result.good())
    {
        NumericValue = numericValue;
        MeasurementUnit = measurementUnit;
        /* clear additional representations */
        FloatingPointValue.clear();
        RationalNumeratorValue.clear();
        RationalDenominatorValue.clear();
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::setValue(const OFString &numericValue,
                                                 const DSRCodedEntryValue &measurementUnit,
                                                 const DSRCodedEntryValue &valueQualifier,
                                                 const OFBool check)
{
    OFCondition result = EC_Normal;
    if (check)
    {
        /* check whether the passed values are valid */
        result = checkNumericValue(numericValue);
        if (result.good())
            result = checkMeasurementUnit(measurementUnit);
        if (result.good())
            result = checkNumericValueQualifier(valueQualifier);
    } else {
        /* make sure that the mandatory values are non-empty */
        if (numericValue.empty() || measurementUnit.isEmpty())
            result = EC_IllegalParameter;
    }
    if (result.good())
    {
        NumericValue = numericValue;
        MeasurementUnit = measurementUnit;
        ValueQualifier = valueQualifier;
        /* clear additional representations */
        FloatingPointValue.clear();
        RationalNumeratorValue.clear();
        RationalDenominatorValue.clear();
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::setNumericValue(const OFString &numericValue,
                                                        const OFBool check)
{
    OFCondition result = EC_Normal;
    if (check)
    {
        /* check whether the passed value is valid */
        result = checkNumericValue(numericValue);
    } else {
        /* make sure that the mandatory values are non-empty */
        if (numericValue.empty())
            result = EC_IllegalParameter;
    }
    if (result.good())
    {
        NumericValue = numericValue;
        /* clear additional representations */
        FloatingPointValue.clear();
        RationalNumeratorValue.clear();
        RationalDenominatorValue.clear();
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::setMeasurementUnit(const DSRCodedEntryValue &measurementUnit,
                                                           const OFBool check)
{
    OFCondition result = EC_Normal;
    if (check)
    {
        /* check whether the passed value is valid */
        result = checkMeasurementUnit(measurementUnit);
    } else {
        /* make sure that the mandatory values are non-empty */
        if (measurementUnit.isEmpty())
            result = EC_IllegalParameter;
    }
    if (result.good())
        MeasurementUnit = measurementUnit;
    return result;
}


OFCondition DSRNumericMeasurementValue::setNumericValueQualifier(const DSRCodedEntryValue &valueQualifier,
                                                                 const OFBool check)
{
    OFCondition result = EC_Normal;
    /* check whether the passed value is valid */
    if (check)
        result = checkNumericValueQualifier(valueQualifier);
    if (result.good())
        ValueQualifier = valueQualifier;
    return result;
}


OFCondition DSRNumericMeasurementValue::setFloatingPointRepresentation(const Float64 floatingPoint,
                                                                       const OFBool /*check*/)
{
    /* make sure that only a single value is stored */
    return FloatingPointValue.putFloat64Array(&floatingPoint, 1);
}


OFCondition DSRNumericMeasurementValue::setRationalRepresentation(const Sint32 rationalNumerator,
                                                                  const Uint32 rationalDenominator,
                                                                  const OFBool check)
{
    OFCondition result = EC_Normal;
    /* check whether the passed values are valid */
    if (check)
        result = checkRationalRepresentation(rationalNumerator, rationalDenominator);
    if (result.good())
    {
        /* make sure that only a single value is stored */
        RationalNumeratorValue.putSint32Array(&rationalNumerator, 1);
        RationalDenominatorValue.putUint32Array(&rationalDenominator, 1);
    }
    return result;
}


void DSRNumericMeasurementValue::removeFloatingPointRepresentation()
{
    FloatingPointValue.clear();
}


void DSRNumericMeasurementValue::removeRationalRepresentation()
{
    RationalNumeratorValue.clear();
    RationalDenominatorValue.clear();
}


OFCondition DSRNumericMeasurementValue::checkNumericValue(const OFString &numericValue) const
{
    /* numeric measurement value should never be empty */
    return numericValue.empty() ? SR_EC_InvalidValue
                                : DcmDecimalString::checkStringValue(numericValue, "1");
}


OFCondition DSRNumericMeasurementValue::checkMeasurementUnit(const DSRCodedEntryValue &measurementUnit) const
{
    /* measurement unit should never be empty */
    return measurementUnit.isEmpty() ? SR_EC_InvalidValue
                                     : measurementUnit.checkCurrentValue();
}


OFCondition DSRNumericMeasurementValue::checkNumericValueQualifier(const DSRCodedEntryValue &valueQualifier) const
{
    /* numeric value qualifier might be empty */
    return valueQualifier.isEmpty() ? EC_Normal
                                    : valueQualifier.checkCurrentValue();
}


OFCondition DSRNumericMeasurementValue::checkRationalRepresentation(const Sint32 /*rationalNumerator*/,
                                                                    const Uint32 rationalDenominator) const
{
    /* avoid "division by zero" */
    return (rationalDenominator == 0) ? SR_EC_InvalidValue
                                      : EC_Normal;
}


OFCondition DSRNumericMeasurementValue::checkCurrentValue() const
{
    OFCondition result = checkNumericValue(NumericValue);
    if (result.good())
        result = checkMeasurementUnit(MeasurementUnit);
    if (result.good())
        result = checkNumericValueQualifier(ValueQualifier);
    if (result.good())
    {
        Sint32 numeratorValue;
        Uint32 denominatorValue;
        result = getRationalRepresentation(numeratorValue, denominatorValue);
        if (result.good())
            result = checkRationalRepresentation(numeratorValue, denominatorValue);
        else if (result == SR_EC_RepresentationNotAvailable)
            result = EC_Normal;
    }
    return result;
}
