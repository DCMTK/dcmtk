/*
 *
 *  Copyright (C) 2000-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRNumericMeasurementValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-10-02 12:07:10 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrnumvl.h"


DSRNumericMeasurementValue::DSRNumericMeasurementValue()
  : NumericValue(),
    MeasurementUnit()
{
}


DSRNumericMeasurementValue::DSRNumericMeasurementValue(const OFString &numericValue,
                                                       const DSRCodedEntryValue &measurementUnit)
  : NumericValue(),
    MeasurementUnit()
{
    /* use the set methods for checking purposes */
    setValue(numericValue, measurementUnit);
}


DSRNumericMeasurementValue::DSRNumericMeasurementValue(const DSRNumericMeasurementValue &numericMeasurement)
  : NumericValue(numericMeasurement.NumericValue),
    MeasurementUnit(numericMeasurement.MeasurementUnit)
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
    return *this;
}


void DSRNumericMeasurementValue::clear()
{
    NumericValue.clear();
    MeasurementUnit.clear();
}


OFBool DSRNumericMeasurementValue::isValid() const
{
    return isEmpty() || (checkNumericValue(NumericValue) && checkMeasurementUnit(MeasurementUnit));
}


OFBool DSRNumericMeasurementValue::isEmpty() const
{
    return (NumericValue.length() == 0) && MeasurementUnit.isEmpty();
}


OFCondition DSRNumericMeasurementValue::print(ostream &stream,
                                              const size_t /* flags */) const
{
    if (isEmpty())
    {
        /* empty value */
        stream << "empty";
    } else {
        OFString printString;
        stream << "\"" << DSRTypes::convertToPrintString(NumericValue, printString) << "\" ";
        MeasurementUnit.print(stream, OFTrue /* printCodeValue */, OFTrue /* printInvalid */);
    }
    return EC_Normal;
}


OFCondition DSRNumericMeasurementValue::writeXML(ostream &stream,
                                                 const size_t flags,
                                                 OFConsole *logStream) const
{
    DSRTypes::writeStringValueToXML(stream, NumericValue, "value", flags & DSRTypes::XF_writeEmptyTags);
    stream << "<unit>" << endl;
    MeasurementUnit.writeXML(stream, flags, logStream);
    stream << "</unit>" << endl;
    return EC_Normal;
}


OFCondition DSRNumericMeasurementValue::readItem(DcmItem &dataset,
                                                 OFConsole *logStream)
{
    /* read NumericValue */
    OFCondition result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_NumericValue, NumericValue, "1", "1", logStream, "MeasuredValueSequence");
    if (result == EC_Normal)
    {
        /* read MeasurementUnitsCodeSequence */
        result = MeasurementUnit.readSequence(dataset, DCM_MeasurementUnitsCodeSequence, "1" /* type */, logStream);
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::writeItem(DcmItem &dataset,
                                                  OFConsole *logStream) const
{
    /* write NumericValue */
    OFCondition result = DSRTypes::putStringValueToDataset(dataset, DCM_NumericValue, NumericValue);
    /* write MeasurementUnitsCodeSequence */
    if (result == EC_Normal)
        result = MeasurementUnit.writeSequence(dataset, DCM_MeasurementUnitsCodeSequence, logStream);
    return result;
}


OFCondition DSRNumericMeasurementValue::readSequence(DcmItem &dataset,
                                                     OFConsole *logStream)
{
    /* read MeasuredValueSequence */
    DcmSequenceOfItems dseq(DCM_MeasuredValueSequence);
    OFCondition result = DSRTypes::getSequenceFromDataset(dataset, dseq);
    DSRTypes::checkElementValue(dseq, "1", "2", logStream, result, "NUM content item");
    if (result == EC_Normal)
    {
        /* check for empty sequence (allowed!) */
        if (dseq.card() > 0)
        {
            /* read first item */
            DcmItem *ditem = dseq.getItem(0);
            if (ditem != NULL)
                result = readItem(*ditem, logStream);
            else
                result = SR_EC_InvalidDocumentTree;
        }
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::writeSequence(DcmItem &dataset,
                                                      OFConsole *logStream) const
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
                result = writeItem(*ditem, logStream);
                if (result == EC_Normal)
                    dseq->insert(ditem);
                else
                    delete ditem;
            } else
                result = EC_MemoryExhausted;
        }
        /* write sequence (might be empty) */
        if (result == EC_Normal)
            result = dataset.insert(dseq, OFTrue /* replaceOld */);
        if (result != EC_Normal)
            delete dseq;
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::renderHTML(ostream &docStream,
                                                   ostream & /* annexStream */,
                                                   size_t & /* annexNumber */,
                                                   const size_t flags,
                                                   OFConsole *logStream) const
{
    if (isEmpty())
    {
        /* empty value */
        docStream << "<i>empty</i>";
    } else {
        OFString htmlString;
        const OFBool fullCode = (flags & DSRTypes::HF_renderNumericUnitCodes) &&
            ((flags & DSRTypes::HF_renderInlineCodes) || (flags & DSRTypes::HF_renderItemsSeparately));
        if (!fullCode)
            docStream << "<u>";
        docStream << DSRTypes::convertToMarkupString(NumericValue, htmlString, flags & DSRTypes::HF_convertNonASCIICharacters) << " ";
        /* render full code of the measurement unit (value first?) or code value only */
        MeasurementUnit.renderHTML(docStream, flags, logStream, fullCode, !(flags & DSRTypes::HF_useCodeMeaningAsUnit) /* valueFirst */);
        if (!fullCode)
            docStream << "</u>";
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
    return setValue(numericMeasurement.NumericValue, numericMeasurement.MeasurementUnit);
}


OFCondition DSRNumericMeasurementValue::setValue(const OFString &numericValue,
                                                 const DSRCodedEntryValue &measurementUnit)
{
    OFCondition result = EC_IllegalCall;
    /* check both values before setting them */
    if (checkNumericValue(numericValue) && checkMeasurementUnit(measurementUnit))
    {
        NumericValue = numericValue;
        MeasurementUnit = measurementUnit;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::setNumericValue(const OFString &numericValue)
{
    OFCondition result = EC_IllegalCall;
    if (checkNumericValue(numericValue))
    {
        NumericValue = numericValue;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRNumericMeasurementValue::setMeasurementUnit(const DSRCodedEntryValue &measurementUnit)
{
    OFCondition result = EC_IllegalCall;
    if (checkMeasurementUnit(measurementUnit))
    {
        MeasurementUnit = measurementUnit;
        result = EC_Normal;
    }
    return result;    
}


OFBool DSRNumericMeasurementValue::checkNumericValue(const OFString &numericValue) const
{
    return (numericValue.length() > 0);
}


OFBool DSRNumericMeasurementValue::checkMeasurementUnit(const DSRCodedEntryValue &measurementUnit) const
{
    return measurementUnit.isValid();
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrnumvl.cc,v $
 *  Revision 1.11  2001-10-02 12:07:10  joergr
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
