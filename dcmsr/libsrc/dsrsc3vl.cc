/*
 *
 *  Copyright (C) 2010, OFFIS e.V.
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
 *    classes: DSRSpatialCoordinates3DValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:41 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrsc3vl.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRSpatialCoordinates3DValue::DSRSpatialCoordinates3DValue()
  : GraphicType(DSRTypes::GT3_invalid),
    GraphicDataList(),
    FrameOfReferenceUID()
{
}


DSRSpatialCoordinates3DValue::DSRSpatialCoordinates3DValue(const DSRTypes::E_GraphicType3D graphicType)
  : GraphicType(graphicType),
    GraphicDataList(),
    FrameOfReferenceUID()
{
}


DSRSpatialCoordinates3DValue::DSRSpatialCoordinates3DValue(const DSRSpatialCoordinates3DValue &coordinatesValue)
  : GraphicType(coordinatesValue.GraphicType),
    GraphicDataList(coordinatesValue.GraphicDataList),
    FrameOfReferenceUID(coordinatesValue.FrameOfReferenceUID)
{
}


DSRSpatialCoordinates3DValue::~DSRSpatialCoordinates3DValue()
{
}


DSRSpatialCoordinates3DValue &DSRSpatialCoordinates3DValue::operator=(const DSRSpatialCoordinates3DValue &coordinatesValue)
{
    GraphicType = coordinatesValue.GraphicType;
    GraphicDataList = coordinatesValue.GraphicDataList;
    FrameOfReferenceUID = coordinatesValue.FrameOfReferenceUID;
    return *this;
}


void DSRSpatialCoordinates3DValue::clear()
{
    GraphicType = DSRTypes::GT3_invalid;
    GraphicDataList.clear();
    FrameOfReferenceUID.clear();
}


OFBool DSRSpatialCoordinates3DValue::isValid() const
{
    return checkData(GraphicType, GraphicDataList, FrameOfReferenceUID);
}


OFBool DSRSpatialCoordinates3DValue::isShort(const size_t flags) const
{
    return GraphicDataList.isEmpty() || ((flags & DSRTypes::HF_renderFullData) == 0);
}


OFCondition DSRSpatialCoordinates3DValue::print(STD_NAMESPACE ostream &stream,
                                                const size_t flags) const
{
    /* GraphicType */
    stream << "(" << DSRTypes::graphicType3DToEnumeratedValue(GraphicType);
    /* ReferencedFrameOfReferenceUID */
    stream << ",";
    if (flags & DSRTypes::PF_printSOPInstanceUID)
        stream << "\"" << FrameOfReferenceUID << "\"";
    /* GraphicData */
    if (!GraphicDataList.isEmpty())
    {
        stream << ",";
        GraphicDataList.print(stream, flags);
    }
    stream << ")";
    return EC_Normal;
}


OFCondition DSRSpatialCoordinates3DValue::readXML(const DSRXMLDocument &doc,
                                                  DSRXMLCursor cursor)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        /* graphic data (required) */
        cursor = doc.getNamedNode(cursor.getChild(), "data");
        if (cursor.valid())
        {
            OFString tmpString;
            /* referenced frame of reference UID (required) */
            doc.getStringFromAttribute(cursor, FrameOfReferenceUID, "uid");
            /* put value to the graphic data list */
            result = GraphicDataList.putString(doc.getStringFromNodeContent(cursor, tmpString).c_str());
        }
    }
    return result;
}


OFCondition DSRSpatialCoordinates3DValue::writeXML(STD_NAMESPACE ostream &stream,
                                                   const size_t flags) const
{
    /* GraphicType is written in TreeNode class */
    if ((flags & DSRTypes::XF_writeEmptyTags) || !GraphicDataList.isEmpty())
    {
        stream << "<data uid=\"" << FrameOfReferenceUID << "\">" << OFendl;
        GraphicDataList.print(stream);
        stream << "</data>" << OFendl;
    }
    return EC_Normal;
}


OFCondition DSRSpatialCoordinates3DValue::read(DcmItem &dataset)
{
    /* read ReferencedFrameOfReferenceUID */
    OFString tmpString;
    OFCondition result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_ReferencedFrameOfReferenceUID, FrameOfReferenceUID, "1", "1", "SCOORD3D content item");
    /* read GraphicType */
    if (result.good())
        result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_GraphicType, tmpString, "1", "1", "SCOORD3D content item");
    if (result.good())
    {
        GraphicType = DSRTypes::enumeratedValueToGraphicType3D(tmpString);
        /* check GraphicType */
        if (GraphicType == DSRTypes::GT3_invalid)
            DSRTypes::printUnknownValueWarningMessage("GraphicType", tmpString.c_str());
        /* read GraphicData */
        result = GraphicDataList.read(dataset);
        /* check GraphicData and report warnings if any */
        checkData(GraphicType, GraphicDataList, FrameOfReferenceUID);
    }
    return result;
}


OFCondition DSRSpatialCoordinates3DValue::write(DcmItem &dataset) const
{
    /* write ReferencedFrameOfReferenceUID */
    OFCondition result = DSRTypes::putStringValueToDataset(dataset, DCM_ReferencedFrameOfReferenceUID, FrameOfReferenceUID);
    /* write GraphicType */
    if (result.good())
        DSRTypes::putStringValueToDataset(dataset, DCM_GraphicType, DSRTypes::graphicType3DToEnumeratedValue(GraphicType));
    /* write GraphicData */
    if (result.good())
    {
        if (!GraphicDataList.isEmpty())
            result = GraphicDataList.write(dataset);
    }
    /* check GraphicData and report warnings if any */
    checkData(GraphicType, GraphicDataList, FrameOfReferenceUID);
    return result;
}


OFCondition DSRSpatialCoordinates3DValue::renderHTML(STD_NAMESPACE ostream &docStream,
                                                     STD_NAMESPACE ostream &annexStream,
                                                     size_t &annexNumber,
                                                     const size_t flags) const
{
    /* render GraphicType */
    docStream << DSRTypes::graphicType3DToReadableName(GraphicType);
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


OFCondition DSRSpatialCoordinates3DValue::setGraphicType(const DSRTypes::E_GraphicType3D graphicType)
{
    OFCondition result = EC_IllegalParameter;
    if (graphicType != DSRTypes::GT3_invalid)
    {
        GraphicType = graphicType;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRSpatialCoordinates3DValue::setFrameOfReferenceUID(const OFString &frameOfReferenceUID)
{
    OFCondition result = EC_IllegalParameter;
    // do further checks?
    if (!frameOfReferenceUID.empty())
    {
        FrameOfReferenceUID = frameOfReferenceUID;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRSpatialCoordinates3DValue::getValue(DSRSpatialCoordinates3DValue &coordinatesValue) const
{
    coordinatesValue = *this;
    return EC_Normal;
}


OFCondition DSRSpatialCoordinates3DValue::setValue(const DSRSpatialCoordinates3DValue &coordinatesValue)
{
    OFCondition result = EC_IllegalParameter;
    if (checkData(coordinatesValue.GraphicType, coordinatesValue.GraphicDataList, coordinatesValue.FrameOfReferenceUID))
    {
        GraphicType = coordinatesValue.GraphicType;
        GraphicDataList = coordinatesValue.GraphicDataList;
        FrameOfReferenceUID = coordinatesValue.FrameOfReferenceUID;
        result = EC_Normal;
    }
    return result;
}


OFBool DSRSpatialCoordinates3DValue::checkData(const DSRTypes::E_GraphicType3D graphicType,
                                               const DSRGraphicData3DList &graphicDataList,
                                               const OFString &frameOfReferenceUID) const
{
    OFBool result = OFFalse;
    // check graphic type and data
    if (graphicType == DSRTypes::GT3_invalid)
        DCMSR_WARN("Invalid GraphicType for SCOORD3D content item");
    else if (graphicDataList.isEmpty())
        DCMSR_WARN("No GraphicData for SCOORD3D content item");
    else
    {
        const size_t count = graphicDataList.getNumberOfItems();
        switch (graphicType)
        {
            case DSRTypes::GT3_Point:
                if (count > 1)
                    DCMSR_WARN("GraphicData has too many entries, only a single entry expected");
                result = OFTrue;
                break;
            case DSRTypes::GT3_Multipoint:
                if (count < 1)
                    DCMSR_WARN("GraphicData has too few entries, at least one entry expected");
                result = OFTrue;
                break;
            case DSRTypes::GT3_Polyline:
                if (count < 1)
                    DCMSR_WARN("GraphicData has too few entries, at least one entry expected");
                result = OFTrue;
                break;
            case DSRTypes::GT3_Polygon:
                if (count < 1)
                    DCMSR_WARN("GraphicData has too few entries, at least one entry expected");
                else if (graphicDataList.getItem(1) != graphicDataList.getItem(count))
                {
                    DCMSR_WARN("First and last entry in GraphicData are not equal (POLYGON)");
                    result = OFTrue;
                }
                break;
            case DSRTypes::GT3_Ellipse:
                if (count < 4)
                    DCMSR_WARN("GraphicData has too few entries, exactly four entries expected");
                else
                {
                    if (count > 4)
                        DCMSR_WARN("GraphicData has too many entries, exactly four entries expected");
                    result = OFTrue;
                }
                break;
            case DSRTypes::GT3_Ellipsoid:
                if (count < 6)
                    DCMSR_WARN("GraphicData has too few entries, exactly six entries expected");
                else
                {
                    if (count > 6)
                        DCMSR_WARN("GraphicData has too many entries, exactly six entries expected");
                    result = OFTrue;
                }
                break;
            default:
                /* GT3_invalid */
                break;
        }
    }
    // check referenced frame of reference UID
    if (frameOfReferenceUID.empty())
    {
        DCMSR_WARN("Empty ReferencedFrameOfReferenceUID for SCOORD3D content item");
        result = OFFalse;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrsc3vl.cc,v $
 *  Revision 1.3  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.2  2010-09-28 14:15:20  joergr
 *  Removed old CVS log entries (copied from master file).
 *
 *  Revision 1.1  2010-09-28 14:07:29  joergr
 *  Added support for Colon CAD SR which requires a new value type (SCOORD3D).
 *
 *
 */
