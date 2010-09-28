/*
 *
 *  Copyright (C) 2010, OFFIS
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
 *    classes: DSRSpatialCoordinates3DValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-09-28 14:07:29 $
 *  CVS/RCS Revision: $Revision: 1.1 $
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
 *  Revision 1.1  2010-09-28 14:07:29  joergr
 *  Added support for Colon CAD SR which requires a new value type (SCOORD3D).
 *
 *  Revision 1.20  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.19  2007-11-15 16:45:42  joergr
 *  Added support for output in XHTML 1.1 format.
 *
 *  Revision 1.18  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.17  2006/07/25 13:37:48  joergr
 *  Added new optional flags for the HTML rendering of SR documents:
 *  HF_alwaysExpandChildrenInline, HF_useCodeDetailsTooltip and
 *  HF_renderSectionTitlesInline.
 *
 *  Revision 1.16  2005/12/08 15:48:06  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.15  2003/10/17 16:10:43  joergr
 *  Fixed wrong wording in a warning message ("too less" -> "too few").
 *
 *  Revision 1.14  2003/08/07 15:21:53  joergr
 *  Added brackets around "bitwise and" operator/operands to avoid warnings
 *  reported by MSVC5.
 *
 *  Revision 1.13  2003/08/07 13:46:04  joergr
 *  Added readXML functionality.
 *  Distinguish more strictly between OFBool and int (required when HAVE_CXX_BOOL
 *  is defined).
 *
 *  Revision 1.12  2002/12/05 13:53:30  joergr
 *  Added further checks when reading SR documents (e.g. value of VerificationFlag,
 *  CompletionsFlag, ContinuityOfContent and SpecificCharacterSet).
 *
 *  Revision 1.11  2001/10/10 15:30:01  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.10  2001/09/26 13:04:24  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.9  2001/05/07 16:14:25  joergr
 *  Updated CVS header.
 *
 *  Revision 1.8  2001/02/13 16:35:28  joergr
 *  Minor corrections in XML output (newlines, etc.).
 *
 *  Revision 1.7  2000/11/06 11:33:45  joergr
 *  Removed additional check (according to CP).
 *
 *  Revision 1.6  2000/11/01 16:37:03  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.5  2000/10/26 14:34:39  joergr
 *  Use method isShort() to decide whether a content item can be rendered
 *  "inline" or not.
 *
 *  Revision 1.4  2000/10/19 16:06:42  joergr
 *  Added optional module name to read method to provide more detailed warning
 *  messages.
 *
 *  Revision 1.3  2000/10/18 17:22:09  joergr
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.2  2000/10/16 12:08:29  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:25  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
