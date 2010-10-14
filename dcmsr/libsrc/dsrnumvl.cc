/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:41 $
 *  CVS/RCS Revision: $Revision: 1.27 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrnumvl.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRNumericMeasurementValue::DSRNumericMeasurementValue()
  : NumericValue(),
    MeasurementUnit(),
    ValueQualifier()
{
}


DSRNumericMeasurementValue::DSRNumericMeasurementValue(const OFString &numericValue,
                                                       const DSRCodedEntryValue &measurementUnit)
  : NumericValue(),
    MeasurementUnit(),
    ValueQualifier()
{
    /* use the set methods for checking purposes */
    setValue(numericValue, measurementUnit);
}


DSRNumericMeasurementValue::DSRNumericMeasurementValue(const OFString &numericValue,
                                                       const DSRCodedEntryValue &measurementUnit,
                                                       const DSRCodedEntryValue &valueQualifier)
  : NumericValue(),
    MeasurementUnit(),
    ValueQualifier()
{
    /* use the set methods for checking purposes */
    setValue(numericValue, measurementUnit, valueQualifier);
}


DSRNumericMeasurementValue::DSRNumericMeasurementValue(const DSRNumericMeasurementValue &numericMeasurement)
  : NumericValue(numericMeasurement.NumericValue),
    MeasurementUnit(numericMeasurement.MeasurementUnit),
    ValueQualifier(numericMeasurement.ValueQualifier)
{
    /* do not check since this would unexpected to the user */
}


DSRNumericMeasurementValue::~DSRNumericMeasurementValue()
{
}


DSRNumericMeasurementValue &DSRNumericMeasurementValue::operator=(const DSRNumericMeasurementValue &numericMeasurement)
{
    /* do not check since this would unexpected to the user */
    NumericValue = numericMeasurement.NumericValue;
    MeasurementUnit = numericMeasurement.MeasurementUnit;
    ValueQualifier = numericMeasurement.ValueQualifier;
    return *this;
}


void DSRNumericMeasurementValue::clear()
{
    NumericValue.clear();
    MeasurementUnit.clear();
    ValueQualifier.clear();
}


OFBool DSRNumericMeasurementValue::isValid() const
{
    return isEmpty() || (checkNumericValue(NumericValue) &&
                         checkMeasurementUnit(MeasurementUnit) &&
                         checkNumericValueQualifier(ValueQualifier));
}


OFBool DSRNumericMeasurementValue::isEmpty() const
{
    return NumericValue.empty() && MeasurementUnit.isEmpty();
}


OFCondition DSRNumericMeasurementValue::print(STD_NAMESPACE ostream &stream,
                                              const size_t /*flags*/) const
{
    if (isEmpty())
    {
        /* empty value */
        stream << "empty";
    } else {
        OFString printString;
        stream << "\"" << DSRTypes::convertToPrintString(NumericValue, printString) << "\" ";
        MeasurementUnit.print(stream, OFTrue /*printCodeValue*/, OFTrue /*printInvalid*/);
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
    DSRTypes::writeStringValueToXML(stream, NumericValue, "value", (flags & DSRTypes::XF_writeEmptyTags) > 0);
    if (!MeasurementUnit.isEmpty() || (flags & DSRTypes::XF_writeEmptyTags))
    {
        /* write measurement unit */
        if (flags & DSRTypes::XF_codeComponentsAsAttribute)
            stream << "<unit";     // bracket ">" is closed in the next writeXML() routine
        else
            stream << "<unit>" << OFendl;
        MeasurementUnit.writeXML(stream, flags);
        stream << "</unit>" << OFendl;
    }
    if (!ValueQualifier.isEmpty() || (flags & DSRTypes::XF_writeEmptyTags))
    {
        /* write value qualifier */
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
        /* read MeasurementUnitsCodeSequence */
        result = MeasurementUnit.readSequence(dataset, DCM_MeasurementUnitsCodeSequence, "1" /*type*/);
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::writeItem(DcmItem &dataset) const
{
    /* write NumericValue */
    OFCondition result = DSRTypes::putStringValueToDataset(dataset, DCM_NumericValue, NumericValue);
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
        if (dseq.card() > 0)
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


OFCondition DSRNumericMeasurementValue::setValue(const DSRNumericMeasurementValue &numericMeasurement)
{
    return setValue(numericMeasurement.NumericValue, numericMeasurement.MeasurementUnit, numericMeasurement.ValueQualifier);
}


OFCondition DSRNumericMeasurementValue::setValue(const OFString &numericValue,
                                                 const DSRCodedEntryValue &measurementUnit)
{
    OFCondition result = EC_IllegalParameter;
    /* check both values before setting them */
    if (checkNumericValue(numericValue) && checkMeasurementUnit(measurementUnit))
    {
        NumericValue = numericValue;
        MeasurementUnit = measurementUnit;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::setValue(const OFString &numericValue,
                                                 const DSRCodedEntryValue &measurementUnit,
                                                 const DSRCodedEntryValue &valueQualifier)
{
    OFCondition result = EC_IllegalParameter;
    /* check all three values before setting them */
    if (checkNumericValue(numericValue) && checkMeasurementUnit(measurementUnit) &&
        checkNumericValueQualifier(valueQualifier))
    {
        NumericValue = numericValue;
        MeasurementUnit = measurementUnit;
        ValueQualifier = valueQualifier;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::setNumericValue(const OFString &numericValue)
{
    OFCondition result = EC_IllegalParameter;
    if (checkNumericValue(numericValue))
    {
        NumericValue = numericValue;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::setMeasurementUnit(const DSRCodedEntryValue &measurementUnit)
{
    OFCondition result = EC_IllegalParameter;
    if (checkMeasurementUnit(measurementUnit))
    {
        MeasurementUnit = measurementUnit;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::setNumericValueQualifier(const DSRCodedEntryValue &valueQualifier)
{
    OFCondition result = EC_IllegalParameter;
    if (checkNumericValueQualifier(valueQualifier))
    {
        ValueQualifier = valueQualifier;
        result = EC_Normal;
    }
    return result;
}


OFBool DSRNumericMeasurementValue::checkNumericValue(const OFString &numericValue) const
{
    return !numericValue.empty();
}


OFBool DSRNumericMeasurementValue::checkMeasurementUnit(const DSRCodedEntryValue &measurementUnit) const
{
    return measurementUnit.isValid();
}


OFBool DSRNumericMeasurementValue::checkNumericValueQualifier(const DSRCodedEntryValue &valueQualifier) const
{
    return valueQualifier.isEmpty() || valueQualifier.isValid();
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrnumvl.cc,v $
 *  Revision 1.27  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.26  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.25  2008-07-17 12:00:09  joergr
 *  Replaced call to getSequenceFromDataset() by getElementFromDataset().
 *
 *  Revision 1.24  2007-11-15 16:45:26  joergr
 *  Added support for output in XHTML 1.1 format.
 *  Enhanced support for output in valid HTML 3.2 format. Migrated support for
 *  standard HTML from version 4.0 to 4.01 (strict).
 *
 *  Revision 1.23  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.22  2006/07/25 13:37:48  joergr
 *  Added new optional flags for the HTML rendering of SR documents:
 *  HF_alwaysExpandChildrenInline, HF_useCodeDetailsTooltip and
 *  HF_renderSectionTitlesInline.
 *
 *  Revision 1.21  2006/05/11 09:16:49  joergr
 *  Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 *  Revision 1.20  2005/12/08 15:48:00  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.19  2004/11/22 16:39:12  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *    in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.18  2004/09/03 09:07:53  joergr
 *  Fixed inconsistency in readXML() method that prevented support for empty
 *  MeasuredValueSequence.
 *  Removed output of empty measurement unit in writeXML() by default. Added
 *  output of empty value qualifier in writeXML() if flag is set accordingly.
 *
 *  Revision 1.17  2003/08/07 17:29:13  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.16  2003/08/07 15:21:53  joergr
 *  Added brackets around "bitwise and" operator/operands to avoid warnings
 *  reported by MSVC5.
 *
 *  Revision 1.15  2003/08/07 13:40:01  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.14  2002/12/10 13:20:37  joergr
 *  Added support for the Numeric Value Qualifier Code Sequence (introduced with
 *  CP 260).
 *
 *  Revision 1.13  2001/11/09 16:17:13  joergr
 *  Added new command line option allowing to encode codes as XML attributes
 *  (instead of tags).
 *
 *  Revision 1.12  2001/10/10 15:29:58  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.11  2001/10/02 12:07:10  joergr
 *  Adapted module "dcmsr" to the new class OFCondition. Introduced module
 *  specific error codes.
 *
 *  Revision 1.10  2001/09/28 14:11:09  joergr
 *  Check return value of DcmItem::insert() statements to avoid memory leaks
 *  when insert procedure failes.
 *
 *  Revision 1.9  2001/09/26 13:04:22  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.8  2001/06/01 15:51:09  meichel
 *  Updated copyright header
 *
 *  Revision 1.7  2000/11/09 20:34:01  joergr
 *  Added support for non-ASCII characters in HTML 3.2 (use numeric value).
 *
 *  Revision 1.6  2000/11/07 18:33:30  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.5  2000/11/01 16:37:01  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.4  2000/10/19 16:05:09  joergr
 *  Added optional module name to read method to provide more detailed warning
 *  messages.
 *
 *  Revision 1.3  2000/10/18 17:19:57  joergr
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.2  2000/10/16 12:06:58  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:22  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
