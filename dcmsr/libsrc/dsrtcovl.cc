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
 *    classes: DSRTemporalCoordinatesValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-11-01 16:37:05 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtcovl.h"


DSRTemporalCoordinatesValue::DSRTemporalCoordinatesValue()
  : TemporalRangeType(DSRTypes::TRT_invalid),
    SamplePositionList(),
    TimeOffsetList(),
    DatetimeList()
{
}


DSRTemporalCoordinatesValue::DSRTemporalCoordinatesValue(const DSRTypes::E_TemporalRangeType temporalRangeType)
  : TemporalRangeType(temporalRangeType),
    SamplePositionList(),
    TimeOffsetList(),
    DatetimeList()
{
}


DSRTemporalCoordinatesValue::DSRTemporalCoordinatesValue(const DSRTemporalCoordinatesValue &coordinatesValue)
  : TemporalRangeType(coordinatesValue.TemporalRangeType),
    SamplePositionList(coordinatesValue.SamplePositionList),
    TimeOffsetList(coordinatesValue.TimeOffsetList),
    DatetimeList(coordinatesValue.DatetimeList)
{
}


DSRTemporalCoordinatesValue::~DSRTemporalCoordinatesValue()
{
}


DSRTemporalCoordinatesValue &DSRTemporalCoordinatesValue::operator=(const DSRTemporalCoordinatesValue &coordinatesValue)
{
    TemporalRangeType = coordinatesValue.TemporalRangeType;
    SamplePositionList = coordinatesValue.SamplePositionList;
    TimeOffsetList = coordinatesValue.TimeOffsetList;
    DatetimeList = coordinatesValue.DatetimeList;
    return *this;
}


void DSRTemporalCoordinatesValue::clear()
{
    TemporalRangeType = DSRTypes::TRT_invalid;
    SamplePositionList.clear();
    TimeOffsetList.clear();
    DatetimeList.clear();
}


OFBool DSRTemporalCoordinatesValue::isValid() const
{
    return checkData(TemporalRangeType, SamplePositionList, TimeOffsetList, DatetimeList);
}


OFBool DSRTemporalCoordinatesValue::isShort(const size_t flags) const
{
    return (SamplePositionList.isEmpty() && TimeOffsetList.isEmpty() && DatetimeList.isEmpty()) ||
          !(flags & DSRTypes::HF_renderFullData);
}


E_Condition DSRTemporalCoordinatesValue::print(ostream &stream,
                                               const size_t flags) const
{
    /* TemporalRangeType */
    stream << "(" << DSRTypes::temporalRangeTypeToEnumeratedValue(TemporalRangeType);
    /* print data */
    OFString string;
    stream << ",";
    /* print only one list */
    if (!SamplePositionList.isEmpty())
        SamplePositionList.print(stream, flags);
    else if (!TimeOffsetList.isEmpty())
        TimeOffsetList.print(stream, flags);
    else
        DatetimeList.print(stream, flags);
    stream << ")";
    return EC_Normal;
}


E_Condition DSRTemporalCoordinatesValue::writeXML(ostream &stream,
                                                  const size_t flags,
                                                  OFConsole * /* logStream */) const
{
    /* TemporalRangeType is written in TreeNode class */
    if ((flags & DSRTypes::XF_writeEmptyTags) || !SamplePositionList.isEmpty() ||
         !TimeOffsetList.isEmpty() || !DatetimeList.isEmpty())
    {
        stream << "<data type =\"";
        /* print only one list */
        if (!SamplePositionList.isEmpty())
        {
            stream << "SAMPLE POSITION\">" << endl;
            SamplePositionList.print(stream);
        } else if (!TimeOffsetList.isEmpty())
        {
            stream << "TIME OFFSET\">" << endl;
            TimeOffsetList.print(stream);
        } else {
            stream << "DATETIME\">" << endl;
            DatetimeList.print(stream);
        }
        stream << "</data>" << endl;        
    }
    return EC_Normal;
}


E_Condition DSRTemporalCoordinatesValue::read(DcmItem &dataset,
                                              OFConsole *logStream)
{
    /* read TemporalRangeType */
    OFString string;
    E_Condition result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_TemporalRangeType, string, "1", "1", logStream, "TCOORD content item");
    if (result == EC_Normal)
    {
        TemporalRangeType = DSRTypes::enumeratedValueToTemporalRangeType(string);
        /* read data (all three lists) */
        SamplePositionList.read(dataset, logStream);
        TimeOffsetList.read(dataset, logStream);
        DatetimeList.read(dataset, logStream);
        /* check data and report warnings if any */
        checkData(TemporalRangeType, SamplePositionList, TimeOffsetList, DatetimeList, logStream);
    }
    return result;
}


E_Condition DSRTemporalCoordinatesValue::write(DcmItem &dataset,
                                               OFConsole *logStream) const
{
    /* write TemporalRangeType */
    E_Condition result = DSRTypes::putStringValueToDataset(dataset, DCM_TemporalRangeType, DSRTypes::temporalRangeTypeToEnumeratedValue(TemporalRangeType));
    if (result == EC_Normal)
    {
        /* write data (only one list) */
        if (!SamplePositionList.isEmpty())
            SamplePositionList.write(dataset, logStream);
        else if (!TimeOffsetList.isEmpty())
            TimeOffsetList.write(dataset, logStream);
        else
            DatetimeList.write(dataset, logStream);
    }
    /* check data and report warnings if any */
    checkData(TemporalRangeType, SamplePositionList, TimeOffsetList, DatetimeList, logStream);
    return result;
}


E_Condition DSRTemporalCoordinatesValue::renderHTML(ostream &docStream,
                                                    ostream &annexStream,
                                                    size_t &annexNumber,
                                                    const size_t flags,
                                                    OFConsole * /* logStream */) const
{
    /* render TemporalRangeType */
    docStream << DSRTypes::temporalRangeTypeToReadableName(TemporalRangeType);
    /* render data */
    if (!isShort(flags))
    {
        if (flags & DSRTypes::HF_currentlyInsideAnnex)
        {
            docStream << endl << "<p>" << endl;
            /* render data list (= print)*/
            if (!SamplePositionList.isEmpty())
            {
                docStream << "<b>Reference Sample Positions:</b><br>";
                SamplePositionList.print(docStream);
            }
            else if (!TimeOffsetList.isEmpty())
            {
                docStream << "<b>Referenced Time Offsets:</b><br>";
                TimeOffsetList.print(docStream);
            } else {
                docStream << "<b>Referenced Datetime:</b><br>";
                DatetimeList.print(docStream);
            }
            docStream << "</p>";
        } else {
            DSRTypes::createHTMLAnnexEntry(docStream, annexStream, "for more details see", annexNumber);
            annexStream << "<p>" << endl;
            /* render data list (= print)*/
            if (!SamplePositionList.isEmpty())
            {
                annexStream << "<b>Reference Sample Positions:</b><br>";
                SamplePositionList.print(annexStream);
            }
            else if (!TimeOffsetList.isEmpty())
            {
                annexStream << "<b>Referenced Time Offsets:</b><br>";
                TimeOffsetList.print(annexStream);
            } else {
                annexStream << "<b>Referenced Datetime:</b><br>";
                DatetimeList.print(annexStream);
            }
            annexStream << "</p>" << endl;
        }
    }
    return EC_Normal;
}


E_Condition DSRTemporalCoordinatesValue::setValue(const DSRTemporalCoordinatesValue &coordinatesValue)
{
    E_Condition result = EC_IllegalCall;
    if (checkData(coordinatesValue.TemporalRangeType, coordinatesValue.SamplePositionList,
                  coordinatesValue.TimeOffsetList, coordinatesValue.DatetimeList))
    {
        TemporalRangeType = coordinatesValue.TemporalRangeType;
        SamplePositionList = coordinatesValue.SamplePositionList;
        TimeOffsetList = coordinatesValue.TimeOffsetList;
        DatetimeList = coordinatesValue.DatetimeList;
        result = EC_Normal;
    }
    return result;
}


E_Condition DSRTemporalCoordinatesValue::getValue(DSRTemporalCoordinatesValue &coordinatesValue) const
{
    coordinatesValue = *this;
    return EC_Normal;
}


E_Condition DSRTemporalCoordinatesValue::setTemporalRangeType(const DSRTypes::E_TemporalRangeType temporalRangeType)
{
    E_Condition result = EC_IllegalCall;
    if (temporalRangeType != DSRTypes::TRT_invalid)
    {
        TemporalRangeType = temporalRangeType;
        result = EC_Normal;
    }
    return result;
}


OFBool DSRTemporalCoordinatesValue::checkData(const DSRTypes::E_TemporalRangeType temporalRangeType,
                                              const DSRReferencedSamplePositionList &samplePositionList,
                                              const DSRReferencedTimeOffsetList &timeOffsetList,
                                              const DSRReferencedDatetimeList &datetimeList,
                                              OFConsole *logStream) const
{
    OFBool result = OFTrue;
    if (temporalRangeType == DSRTypes::TRT_invalid)
        DSRTypes::printWarningMessage(logStream, "Invalid TemporalRangeType for TCOORD content item");
    const OFBool list1 = !samplePositionList.isEmpty();
    const OFBool list2 = !timeOffsetList.isEmpty();
    const OFBool list3 = !datetimeList.isEmpty();
    if (list1 && list2 && list3)
        DSRTypes::printWarningMessage(logStream, "ReferencedSamplePositions/TimeOffsets/Datetime present in TCOORD content item");
    else if (list1 && list2)
        DSRTypes::printWarningMessage(logStream, "ReferencedSamplePositions/TimeOffsets present in TCOORD content item");
    else if (list1 && list3)
        DSRTypes::printWarningMessage(logStream, "ReferencedSamplePositions/Datetime present in TCOORD content item");
    else if (list2 && list3)
        DSRTypes::printWarningMessage(logStream, "ReferencedTimeOffsets/Datetime present in TCOORD content item");
    else if (!list1 && !list2 && !list3)
    {
        DSRTypes::printWarningMessage(logStream, "ReferencedSamplePositions/TimeOffsets/Datetime empty in TCOORD content item");
        /* invalid: all lists are empty (type 1C) */
        result= OFFalse;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtcovl.cc,v $
 *  Revision 1.2  2000-11-01 16:37:05  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.1  2000/10/26 14:40:29  joergr
 *  Added support for TCOORD content item.
 *
 *
 *
 */
