/*
 *
 *  Copyright (C) 2000, OFFIS
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
 *  Update Date:      $Date: 2000-10-16 12:06:58 $
 *  CVS/RCS Revision: $Revision: 1.2 $
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
    return checkNumericValue(NumericValue) && checkMeasurementUnit(MeasurementUnit);
}


E_Condition DSRNumericMeasurementValue::print(ostream &stream,
                                              const size_t /* flags */) const
{
    stream << "\"" << NumericValue << "\" ";
    MeasurementUnit.print(stream, OFTrue /* printCodeValue */, OFTrue /* printInvalid */);
    return EC_Normal;
}


E_Condition DSRNumericMeasurementValue::readItem(DcmItem &dataset,
                                                 OFConsole *logStream)
{
    /* read NumericValue */
    E_Condition result = DSRTypes::getStringValueFromDataset(dataset, DCM_NumericValue, NumericValue);
    if (result != EC_Normal)
        DSRTypes::printWarningMessage(logStream, "NumericValue absent in MeasuredValueSequence");
    else {
        /* read MeasurementUnitsCodeSequence */
        result = MeasurementUnit.readSequence(dataset, DCM_MeasurementUnitsCodeSequence, logStream);
        if (result != EC_Normal)
            DSRTypes::printWarningMessage(logStream, "MeasurementUnitsCodeSequence absent in MeasuredValueSequence");
    }
    return result;
}


E_Condition DSRNumericMeasurementValue::writeItem(DcmItem &dataset,
                                                  OFConsole *logStream) const
{
    /* write NumericValue */
    E_Condition result = DSRTypes::putStringValueToDataset(dataset, DCM_NumericValue, NumericValue);
    /* write MeasurementUnitsCodeSequence */
    if (result == EC_Normal)
        result = MeasurementUnit.writeSequence(dataset, DCM_MeasurementUnitsCodeSequence, logStream);
    return result;
}


E_Condition DSRNumericMeasurementValue::readSequence(DcmItem &dataset,
                                                     OFConsole *logStream)
{
    DcmStack stack;
    /* read MeasuredValueSequence */
    E_Condition result = dataset.search(DCM_MeasuredValueSequence, stack, ESM_fromHere, OFFalse);
    if (result == EC_Normal)
    {
        DcmSequenceOfItems *dseq = (DcmSequenceOfItems *)stack.top();
        if (dseq != NULL)
        {
            /* check for more or less than 1 item */
            if (dseq->card() < 1)
                DSRTypes::printWarningMessage(logStream, "MeasuredValueSequence is empty");
            else if (dseq->card() > 1)
                DSRTypes::printWarningMessage(logStream, "MeasuredValueSequence contains more than 1 item");
            /* read first item */
            DcmItem *ditem = dseq->getItem(0);
            if (ditem != NULL)
                result = readItem(*ditem, logStream);
            else
                result = EC_CorruptedData;
        } else
            result = EC_CorruptedData;
    }
    return result;
}


E_Condition DSRNumericMeasurementValue::writeSequence(DcmItem &dataset,
                                                      OFConsole *logStream) const
{
    E_Condition result = EC_MemoryExhausted;
    /* write MeasuredValueSequence */
    DcmSequenceOfItems *dseq = new DcmSequenceOfItems(DCM_MeasuredValueSequence);
    if (dseq != NULL)
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
        /* write sequence */
        if (result == EC_Normal)
            dataset.insert(dseq);
        else
            delete dseq;
    }
    return result;
}


E_Condition DSRNumericMeasurementValue::renderHTML(ostream &docStream,
                                                   ostream & /* annexStream */,
                                                   size_t & /* annexNumber */,
                                                   OFConsole * /* logStream */) const
{
    /* CodeValue contains the name of the measurement unit */
    docStream << NumericValue << " " << MeasurementUnit.getCodeValue() << endl;
    return EC_Normal;
}


E_Condition DSRNumericMeasurementValue::getMeasurementUnit(DSRCodedEntryValue &measurementUnit) const
{
    measurementUnit = MeasurementUnit;
    return EC_Normal;
}


E_Condition DSRNumericMeasurementValue::getValue(DSRNumericMeasurementValue &numericMeasurement) const
{
    numericMeasurement = *this;
    return EC_Normal;
}


E_Condition DSRNumericMeasurementValue::setNumericValue(const OFString &numericValue)
{
    E_Condition result = EC_IllegalCall;
    if (checkNumericValue(numericValue))
    {
        NumericValue = numericValue;
        result = EC_Normal;
    }
    return result;
}


E_Condition DSRNumericMeasurementValue::setMeasurementUnit(const DSRCodedEntryValue &measurementUnit)
{
    E_Condition result = EC_IllegalCall;
    if (checkMeasurementUnit(measurementUnit))
    {
        MeasurementUnit = measurementUnit;
        result = EC_Normal;
    }
    return result;    
}


E_Condition DSRNumericMeasurementValue::setValue(const DSRNumericMeasurementValue &numericMeasurement)
{
    return setValue(numericMeasurement.NumericValue, numericMeasurement.MeasurementUnit);
}


E_Condition DSRNumericMeasurementValue::setValue(const OFString &numericValue,
                                                 const DSRCodedEntryValue &measurementUnit)
{
    E_Condition result = setNumericValue(numericValue);
    if (result == EC_Normal)
        result = setMeasurementUnit(measurementUnit);
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
 *  Revision 1.2  2000-10-16 12:06:58  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:22  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
