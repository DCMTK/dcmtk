/*
 *
 *  Copyright (C) 2000-2012, OFFIS e.V.
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
 *    classes: DSRTemporalCoordinatesValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2012-06-11 08:53:07 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtcovl.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRTemporalCoordinatesValue::DSRTemporalCoordinatesValue()
  : TemporalRangeType(DSRTypes::TRT_invalid),
    SamplePositionList(),
    TimeOffsetList(),
    DateTimeList()
{
}


DSRTemporalCoordinatesValue::DSRTemporalCoordinatesValue(const DSRTypes::E_TemporalRangeType temporalRangeType)
  : TemporalRangeType(temporalRangeType),
    SamplePositionList(),
    TimeOffsetList(),
    DateTimeList()
{
}


DSRTemporalCoordinatesValue::DSRTemporalCoordinatesValue(const DSRTemporalCoordinatesValue &coordinatesValue)
  : TemporalRangeType(coordinatesValue.TemporalRangeType),
    SamplePositionList(coordinatesValue.SamplePositionList),
    TimeOffsetList(coordinatesValue.TimeOffsetList),
    DateTimeList(coordinatesValue.DateTimeList)
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
    DateTimeList = coordinatesValue.DateTimeList;
    return *this;
}


void DSRTemporalCoordinatesValue::clear()
{
    TemporalRangeType = DSRTypes::TRT_invalid;
    SamplePositionList.clear();
    TimeOffsetList.clear();
    DateTimeList.clear();
}


OFBool DSRTemporalCoordinatesValue::isValid() const
{
    return checkData(TemporalRangeType, SamplePositionList, TimeOffsetList, DateTimeList).good();
}


OFBool DSRTemporalCoordinatesValue::isShort(const size_t flags) const
{
    return (SamplePositionList.isEmpty() && TimeOffsetList.isEmpty() && DateTimeList.isEmpty()) ||
          !(flags & DSRTypes::HF_renderFullData);
}


OFCondition DSRTemporalCoordinatesValue::print(STD_NAMESPACE ostream &stream,
                                               const size_t flags) const
{
    /* TemporalRangeType */
    stream << "(" << DSRTypes::temporalRangeTypeToEnumeratedValue(TemporalRangeType);
    /* print data */
    stream << ",";
    /* print only one list */
    if (!SamplePositionList.isEmpty())
        SamplePositionList.print(stream, flags);
    else if (!TimeOffsetList.isEmpty())
        TimeOffsetList.print(stream, flags);
    else
        DateTimeList.print(stream, flags);
    stream << ")";
    return EC_Normal;
}


OFCondition DSRTemporalCoordinatesValue::readXML(const DSRXMLDocument &doc,
                                                 DSRXMLCursor cursor)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        /* graphic data (required) */
        cursor = doc.getNamedNode(cursor.getChild(), "data");
        if (cursor.valid())
        {
            OFString tmpString, typeString;
            /* read 'type' and check validity */
            doc.getStringFromAttribute(cursor, typeString, "type");
            if (typeString == "SAMPLE POSITION")
            {
                /* put value to the sample position list */
                result = SamplePositionList.putString(doc.getStringFromNodeContent(cursor, tmpString).c_str());
            }
            else if (typeString == "TIME OFFSET")
            {
                /* put value to the time offset list */
                result = TimeOffsetList.putString(doc.getStringFromNodeContent(cursor, tmpString).c_str());
            }
            else if (typeString == "DATETIME")
            {
                /* put value to the date/time list (tbd: convert from ISO 8601 format?) */
                result = DateTimeList.putString(doc.getStringFromNodeContent(cursor, tmpString).c_str());
            } else {
                DSRTypes::printUnknownValueWarningMessage("TCOORD data type", typeString.c_str());
                result = SR_EC_InvalidValue;
            }
        }
    }
    return result;
}


OFCondition DSRTemporalCoordinatesValue::writeXML(STD_NAMESPACE ostream &stream,
                                                  const size_t flags) const
{
    /* TemporalRangeType is written in TreeNode class */
    if ((flags & DSRTypes::XF_writeEmptyTags) || !SamplePositionList.isEmpty() ||
         !TimeOffsetList.isEmpty() || !DateTimeList.isEmpty())
    {
        stream << "<data type=\"";
        /* print only one list */
        if (!SamplePositionList.isEmpty())
        {
            stream << "SAMPLE POSITION\">";
            SamplePositionList.print(stream);
        }
        else if (!TimeOffsetList.isEmpty())
        {
            stream << "TIME OFFSET\">";
            TimeOffsetList.print(stream);
        } else {
            /* tbd: convert output to ISO 8601 format? */
            stream << "DATETIME\">";
            DateTimeList.print(stream);
        }
        stream << "</data>" << OFendl;
    }
    return EC_Normal;
}


OFCondition DSRTemporalCoordinatesValue::read(DcmItem &dataset)
{
    /* read TemporalRangeType */
    OFString tmpString;
    OFCondition result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_TemporalRangeType, tmpString, "1", "1", "TCOORD content item");
    if (result.good())
    {
        TemporalRangeType = DSRTypes::enumeratedValueToTemporalRangeType(tmpString);
        /* check TemporalRangeType */
        if (TemporalRangeType == DSRTypes::TRT_invalid)
            DSRTypes::printUnknownValueWarningMessage("TemporalRangeType", tmpString.c_str());
        /* first read data (all three lists) */
        SamplePositionList.read(dataset);
        TimeOffsetList.read(dataset);
        DateTimeList.read(dataset);
        /* then check data and report warnings if any */
        checkData(TemporalRangeType, SamplePositionList, TimeOffsetList, DateTimeList, OFTrue /*reportWarnings*/);
    }
    return result;
}


OFCondition DSRTemporalCoordinatesValue::write(DcmItem &dataset) const
{
    /* write TemporalRangeType */
    OFCondition result = DSRTypes::putStringValueToDataset(dataset, DCM_TemporalRangeType, DSRTypes::temporalRangeTypeToEnumeratedValue(TemporalRangeType));
    if (result.good())
    {
        /* write data (only one list) */
        if (!SamplePositionList.isEmpty())
            SamplePositionList.write(dataset);
        else if (!TimeOffsetList.isEmpty())
            TimeOffsetList.write(dataset);
        else
            DateTimeList.write(dataset);
    }
    /* check data and report warnings if any */
    checkData(TemporalRangeType, SamplePositionList, TimeOffsetList, DateTimeList, OFTrue /*reportWarnings*/);
    return result;
}


OFCondition DSRTemporalCoordinatesValue::renderHTML(STD_NAMESPACE ostream &docStream,
                                                    STD_NAMESPACE ostream &annexStream,
                                                    size_t &annexNumber,
                                                    const size_t flags) const
{
    /* render TemporalRangeType */
    docStream << DSRTypes::temporalRangeTypeToReadableName(TemporalRangeType);
    /* render data */
    if (!isShort(flags))
    {
        const char *lineBreak = (flags & DSRTypes::HF_renderSectionTitlesInline) ? " " :
                                (flags & DSRTypes::HF_XHTML11Compatibility) ? "<br />" : "<br>";
        if (flags & DSRTypes::HF_currentlyInsideAnnex)
        {
            docStream << OFendl << "<p>" << OFendl;
            /* render data list (= print)*/
            if (!SamplePositionList.isEmpty())
            {
                docStream << "<b>Reference Sample Positions:</b>" << lineBreak;
                SamplePositionList.print(docStream);
            }
            else if (!TimeOffsetList.isEmpty())
            {
                docStream << "<b>Referenced Time Offsets:</b>" << lineBreak;
                TimeOffsetList.print(docStream);
            } else {
                docStream << "<b>Referenced Date/Time:</b>" << lineBreak;
                DateTimeList.print(docStream);
            }
            docStream << "</p>";
        } else {
            DSRTypes::createHTMLAnnexEntry(docStream, annexStream, "for more details see", annexNumber, flags);
            annexStream << "<p>" << OFendl;
            /* render data list (= print)*/
            if (!SamplePositionList.isEmpty())
            {
                annexStream << "<b>Reference Sample Positions:</b>" << lineBreak;
                SamplePositionList.print(annexStream);
            }
            else if (!TimeOffsetList.isEmpty())
            {
                annexStream << "<b>Referenced Time Offsets:</b>" << lineBreak;
                TimeOffsetList.print(annexStream);
            } else {
                annexStream << "<b>Referenced Date/Time:</b>" << lineBreak;
                DateTimeList.print(annexStream);
            }
            annexStream << "</p>" << OFendl;
        }
    }
    return EC_Normal;
}


OFCondition DSRTemporalCoordinatesValue::setValue(const DSRTemporalCoordinatesValue &coordinatesValue,
                                                  const OFBool check)
{
    OFCondition result = EC_Normal;
    if (check)
    {
        /* check whether the passed value is valid */
        result = checkData(coordinatesValue.TemporalRangeType, coordinatesValue.SamplePositionList,
                           coordinatesValue.TimeOffsetList, coordinatesValue.DateTimeList);
    } else {
        /* make sure that the mandatory values are non-empty/invalid */
        if ((coordinatesValue.TemporalRangeType == DSRTypes::TRT_invalid) ||
            (coordinatesValue.SamplePositionList.isEmpty() &&
             coordinatesValue.TimeOffsetList.isEmpty() &&
             coordinatesValue.DateTimeList.isEmpty()))
        {
            result = EC_IllegalParameter;
        }
    }
    if (result.good())
    {
        TemporalRangeType = coordinatesValue.TemporalRangeType;
        SamplePositionList = coordinatesValue.SamplePositionList;
        TimeOffsetList = coordinatesValue.TimeOffsetList;
        DateTimeList = coordinatesValue.DateTimeList;
    }
    return result;
}


OFCondition DSRTemporalCoordinatesValue::getValue(DSRTemporalCoordinatesValue &coordinatesValue) const
{
    coordinatesValue = *this;
    return EC_Normal;
}


OFCondition DSRTemporalCoordinatesValue::setTemporalRangeType(const DSRTypes::E_TemporalRangeType temporalRangeType,
                                                              const OFBool /*check*/)
{
    OFCondition result = EC_IllegalParameter;
    /* check whether the passed value is valid */
    if (temporalRangeType != DSRTypes::TRT_invalid)
    {
        TemporalRangeType = temporalRangeType;
        result = EC_Normal;
    }
    return result;
}


// helper macro to avoid annoying check of boolean flag
#define REPORT_WARNING(msg) { if (reportWarnings) DCMSR_WARN(msg); }

OFCondition DSRTemporalCoordinatesValue::checkData(const DSRTypes::E_TemporalRangeType temporalRangeType,
                                                   const DSRReferencedSamplePositionList &samplePositionList,
                                                   const DSRReferencedTimeOffsetList &timeOffsetList,
                                                   const DSRReferencedDateTimeList &dateTimeList,
                                                   const OFBool reportWarnings) const
{
    OFCondition result = EC_Normal;
    if (temporalRangeType == DSRTypes::TRT_invalid)
        REPORT_WARNING("Invalid TemporalRangeType for TCOORD content item")
    const OFBool list1 = !samplePositionList.isEmpty();
    const OFBool list2 = !timeOffsetList.isEmpty();
    const OFBool list3 = !dateTimeList.isEmpty();
    if (list1 && list2 && list3)
        REPORT_WARNING("ReferencedSamplePositions/TimeOffsets/DateTime present in TCOORD content item")
    else if (list1 && list2)
        REPORT_WARNING("ReferencedSamplePositions/TimeOffsets present in TCOORD content item")
    else if (list1 && list3)
        REPORT_WARNING("ReferencedSamplePositions/DateTime present in TCOORD content item")
    else if (list2 && list3)
        REPORT_WARNING("ReferencedTimeOffsets/DateTime present in TCOORD content item")
    else if (!list1 && !list2 && !list3)
    {
        REPORT_WARNING("ReferencedSamplePositions/TimeOffsets/DateTime empty in TCOORD content item")
        /* invalid: all lists are empty (type 1C) */
        result = SR_EC_InvalidValue;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtcovl.cc,v $
 *  Revision 1.18  2012-06-11 08:53:07  joergr
 *  Added optional "check" parameter to "set" methods and enhanced documentation.
 *
 *  Revision 1.17  2010-10-14 13:14:42  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.16  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.15  2007-11-15 16:45:42  joergr
 *  Added support for output in XHTML 1.1 format.
 *
 *  Revision 1.14  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.13  2006/07/25 13:37:48  joergr
 *  Added new optional flags for the HTML rendering of SR documents:
 *  HF_alwaysExpandChildrenInline, HF_useCodeDetailsTooltip and
 *  HF_renderSectionTitlesInline.
 *
 *  Revision 1.12  2005/12/08 15:48:14  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.11  2004/01/16 10:10:16  joergr
 *  Added comment regarding ISO datetime format.
 *
 *  Revision 1.10  2003/10/08 11:39:57  joergr
 *  Fixed incorrect output format in writeXML().
 *
 *  Revision 1.9  2003/08/07 14:08:24  joergr
 *  Added readXML functionality.
 *  Renamed parameters/variables "string" to avoid name clash with STL class.
 *
 *  Revision 1.8  2002/12/05 13:53:30  joergr
 *  Added further checks when reading SR documents (e.g. value of VerificationFlag,
 *  CompletionsFlag, ContinuityOfContent and SpecificCharacterSet).
 *
 *  Revision 1.7  2002/07/22 14:22:10  joergr
 *  Removed unused variable.
 *
 *  Revision 1.6  2001/11/09 16:19:03  joergr
 *  Adjusted formatting in XML output.
 *
 *  Revision 1.5  2001/10/10 15:30:04  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.4  2001/09/26 13:04:26  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.3  2001/06/01 15:51:11  meichel
 *  Updated copyright header
 *
 *  Revision 1.2  2000/11/01 16:37:05  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.1  2000/10/26 14:40:29  joergr
 *  Added support for TCOORD content item.
 *
 *
 *
 */
