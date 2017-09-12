/*
 *
 *  Copyright (C) 2000-2017, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRSpatialCoordinatesValue
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrscovl.h"
#include "dcmtk/dcmsr/dsrxmld.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrui.h"


DSRSpatialCoordinatesValue::DSRSpatialCoordinatesValue()
  : GraphicType(DSRTypes::GT_invalid),
    GraphicDataList(),
    FiducialUID()
{
}


DSRSpatialCoordinatesValue::DSRSpatialCoordinatesValue(const DSRTypes::E_GraphicType graphicType)
  : GraphicType(graphicType),
    GraphicDataList(),
    FiducialUID()
{
}


DSRSpatialCoordinatesValue::DSRSpatialCoordinatesValue(const DSRSpatialCoordinatesValue &coordinatesValue)
  : GraphicType(coordinatesValue.GraphicType),
    GraphicDataList(coordinatesValue.GraphicDataList),
    FiducialUID(coordinatesValue.FiducialUID)
{
}


DSRSpatialCoordinatesValue::~DSRSpatialCoordinatesValue()
{
}


DSRSpatialCoordinatesValue &DSRSpatialCoordinatesValue::operator=(const DSRSpatialCoordinatesValue &coordinatesValue)
{
    GraphicType = coordinatesValue.GraphicType;
    GraphicDataList = coordinatesValue.GraphicDataList;
    FiducialUID = coordinatesValue.FiducialUID;
    return *this;
}


OFBool DSRSpatialCoordinatesValue::operator==(const DSRSpatialCoordinatesValue &coordinatesValue) const
{
    return (GraphicType == coordinatesValue.GraphicType) &&
           (GraphicDataList == coordinatesValue.GraphicDataList) &&
           (FiducialUID == coordinatesValue.FiducialUID);
}


OFBool DSRSpatialCoordinatesValue::operator!=(const DSRSpatialCoordinatesValue &coordinatesValue) const
{
    return (GraphicType != coordinatesValue.GraphicType) ||
           (GraphicDataList != coordinatesValue.GraphicDataList) ||
           (FiducialUID != coordinatesValue.FiducialUID);
}


void DSRSpatialCoordinatesValue::clear()
{
    GraphicType = DSRTypes::GT_invalid;
    GraphicDataList.clear();
    FiducialUID.clear();
}


OFBool DSRSpatialCoordinatesValue::isValid() const
{
    return checkGraphicData(GraphicType, GraphicDataList).good();
}


OFBool DSRSpatialCoordinatesValue::isShort(const size_t flags) const
{
    return GraphicDataList.isEmpty() || ((flags & DSRTypes::HF_renderFullData) == 0);
}


OFCondition DSRSpatialCoordinatesValue::print(STD_NAMESPACE ostream &stream,
                                              const size_t flags) const
{
    /* GraphicType */
    stream << "(" << DSRTypes::graphicTypeToEnumeratedValue(GraphicType);
    /* GraphicData */
    if (!GraphicDataList.isEmpty())
    {
        stream << ",";
        GraphicDataList.print(stream, flags);
    }
    stream << ")";
    return EC_Normal;
}


OFCondition DSRSpatialCoordinatesValue::readXML(const DSRXMLDocument &doc,
                                                DSRXMLCursor cursor,
                                                const size_t /*flags*/)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        cursor.gotoChild();
        /* GraphicData (required) */
        const DSRXMLCursor dataNode = doc.getNamedNode(cursor, "data");
        if (dataNode.valid())
        {
            OFString tmpString;
            /* put value to the graphic data list */
            result = GraphicDataList.putString(doc.getStringFromNodeContent(dataNode, tmpString).c_str());
        }
        /* FiducialUID (optional) */
        const DSRXMLCursor fiducialNode = doc.getNamedNode(cursor, "fiducial", OFFalse /*required*/);
        if (fiducialNode.valid())
            doc.getStringFromAttribute(fiducialNode, FiducialUID, "uid");
    }
    return result;
}


OFCondition DSRSpatialCoordinatesValue::writeXML(STD_NAMESPACE ostream &stream,
                                                 const size_t flags) const
{
    /* GraphicType is written in TreeNode class */
    if ((flags & DSRTypes::XF_writeEmptyTags) || !GraphicDataList.isEmpty())
    {
        stream << "<data>";
        GraphicDataList.print(stream);
        stream << "</data>" << OFendl;
    }
    if ((flags & DSRTypes::XF_writeEmptyTags) || !FiducialUID.empty())
        stream << "<fiducial uid=\"" << FiducialUID << "\"/>" << OFendl;
    return EC_Normal;
}


OFCondition DSRSpatialCoordinatesValue::read(DcmItem &dataset,
                                             const size_t flags)
{
    /* read GraphicType */
    OFString tmpString;
    OFCondition result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_GraphicType, tmpString, "1", "1", "SCOORD content item");
    if (result.good())
    {
        GraphicType = DSRTypes::enumeratedValueToGraphicType(tmpString);
        /* check GraphicType */
        if (GraphicType == DSRTypes::GT_invalid)
            DSRTypes::printUnknownValueWarningMessage("GraphicType", tmpString.c_str());
        /* read GraphicData */
        result = GraphicDataList.read(dataset, flags);
        /* read optional attributes */
        if (result.good())
            DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_FiducialUID, FiducialUID, "1", "3", "SCOORD content item");
        /* check GraphicData and report warnings if any */
        checkGraphicData(GraphicType, GraphicDataList, OFTrue /*reportWarnings*/);
    }
    return result;
}


OFCondition DSRSpatialCoordinatesValue::write(DcmItem &dataset) const
{
    /* write GraphicType */
    OFCondition result = DSRTypes::putStringValueToDataset(dataset, DCM_GraphicType, DSRTypes::graphicTypeToEnumeratedValue(GraphicType));
    /* write GraphicData */
    if (result.good())
    {
        if (!GraphicDataList.isEmpty())
            result = GraphicDataList.write(dataset);
    }
    /* write optional attributes */
    if (result.good())
        DSRTypes::putStringValueToDataset(dataset, DCM_FiducialUID, FiducialUID, OFFalse /*allowEmpty*/);
    /* check GraphicData and report warnings if any */
    checkGraphicData(GraphicType, GraphicDataList, OFTrue /*reportWarnings*/);
    return result;
}


OFCondition DSRSpatialCoordinatesValue::renderHTML(STD_NAMESPACE ostream &docStream,
                                                   STD_NAMESPACE ostream &annexStream,
                                                   size_t &annexNumber,
                                                   const size_t flags) const
{
    /* render GraphicType */
    docStream << DSRTypes::graphicTypeToReadableName(GraphicType);
    /* render GraphicData */
    if (!isShort(flags))
    {
        const char *lineBreak = (flags & DSRTypes::HF_renderSectionTitlesInline) ? " " :
                                (flags & DSRTypes::HF_XHTML11Compatibility) ? "<br />" : "<br>";
        if (flags & DSRTypes::HF_currentlyInsideAnnex)
        {
            docStream << OFendl << "<p>" << OFendl;
            /* render graphic data list (= print)*/
            docStream << "<b>Graphic Data:</b>" << lineBreak;
            GraphicDataList.print(docStream);
            docStream << "</p>";
        } else {
            DSRTypes::createHTMLAnnexEntry(docStream, annexStream, "for more details see", annexNumber, flags);
            annexStream << "<p>" << OFendl;
            /* render graphic data list (= print)*/
            annexStream << "<b>Graphic Data:</b>" << lineBreak;
            GraphicDataList.print(annexStream);
            annexStream << "</p>" << OFendl;
        }
    }
    return EC_Normal;
}


OFCondition DSRSpatialCoordinatesValue::getValue(DSRSpatialCoordinatesValue &coordinatesValue) const
{
    coordinatesValue = *this;
    return EC_Normal;
}


OFCondition DSRSpatialCoordinatesValue::setValue(const DSRSpatialCoordinatesValue &coordinatesValue,
                                                 const OFBool check)
{
    OFCondition result = EC_Normal;
    if (check)
    {
        /* check whether the passed value is valid */
        result = checkGraphicData(coordinatesValue.GraphicType, coordinatesValue.GraphicDataList);
        if (result.good())
            result = checkFiducialUID(coordinatesValue.FiducialUID);
    } else {
        /* make sure that the mandatory values are non-empty/invalid */
        if ((coordinatesValue.GraphicType == DSRTypes::GT_invalid) ||
            coordinatesValue.GraphicDataList.isEmpty())
        {
            result = EC_IllegalParameter;
        }
    }
    if (result.good())
    {
        GraphicType = coordinatesValue.GraphicType;
        GraphicDataList = coordinatesValue.GraphicDataList;
        FiducialUID = coordinatesValue.FiducialUID;
    }
    return result;
}


OFCondition DSRSpatialCoordinatesValue::setGraphicType(const DSRTypes::E_GraphicType graphicType,
                                                       const OFBool /*check*/)
{
    OFCondition result = EC_IllegalParameter;
    /* check whether the passed value is valid */
    if (graphicType != DSRTypes::GT_invalid)
    {
        GraphicType = graphicType;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRSpatialCoordinatesValue::setFiducialUID(const OFString &fiducialUID,
                                                         const OFBool check)
{
    OFCondition result = EC_Normal;
    /* check whether the passed value is valid */
    if (check)
        result = checkFiducialUID(fiducialUID);
    if (result.good())
        FiducialUID = fiducialUID;
    return result;
}


// helper macro to avoid annoying check of boolean flag
#define REPORT_WARNING(msg) { if (reportWarnings) DCMSR_WARN(msg); }

OFCondition DSRSpatialCoordinatesValue::checkGraphicData(const DSRTypes::E_GraphicType graphicType,
                                                         const DSRGraphicDataList &graphicDataList,
                                                         const OFBool reportWarnings) const
{
    OFCondition result = SR_EC_InvalidValue;
    // check graphic type and data
    if (graphicType == DSRTypes::GT_invalid)
        REPORT_WARNING("Invalid Graphic Type for SCOORD content item")
    else if (graphicDataList.isEmpty())
        REPORT_WARNING("No Graphic Data for SCOORD content item")
    else
    {
        const size_t count = graphicDataList.getNumberOfItems();
        switch (graphicType)
        {
            case DSRTypes::GT_Point:
                if (count > 1)
                    REPORT_WARNING("Graphic Data has too many entries, only a single entry expected")
                result = EC_Normal;
                break;
            case DSRTypes::GT_Multipoint:
                if (count < 1)
                    REPORT_WARNING("Graphic Data has too few entries, at least one entry expected")
                result = EC_Normal;
                break;
            case DSRTypes::GT_Polyline:
/*
                // not required any more according to CP-233
                if (graphicDataList.getItem(1) != graphicDataList.getItem(count))
                    REPORT_WARNING("First and last entry in Graphic Data are not equal (POLYLINE)")
*/
                result = EC_Normal;
                break;
            case DSRTypes::GT_Circle:
                if (count < 2)
                    REPORT_WARNING("Graphic Data has too few entries, exactly two entries expected")
                else
                {
                    if (count > 2)
                        REPORT_WARNING("Graphic Data has too many entries, exactly two entries expected")
                    result = EC_Normal;
                }
                break;
            case DSRTypes::GT_Ellipse:
                if (count < 4)
                    REPORT_WARNING("Graphic Data has too few entries, exactly four entries expected")
                else
                {
                    if (count > 4)
                        REPORT_WARNING("Graphic Data has too many entries, exactly four entries expected")
                    result = EC_Normal;
                }
                break;
            default:
                /* GT_invalid */
                break;
        }
    }
    return result;
}


OFCondition DSRSpatialCoordinatesValue::checkFiducialUID(const OFString &fiducialUID) const
{
    /* fiducial UID might be empty */
    return fiducialUID.empty() ? EC_Normal
                               : DcmUniqueIdentifier::checkStringValue(fiducialUID, "1");
}
