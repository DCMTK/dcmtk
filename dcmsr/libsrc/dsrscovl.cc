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
 *    classes: DSRSpatialCoordinatesValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-02-13 16:35:28 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrscovl.h"


DSRSpatialCoordinatesValue::DSRSpatialCoordinatesValue()
  : GraphicType(DSRTypes::GT_invalid),
    GraphicDataList()
{
}


DSRSpatialCoordinatesValue::DSRSpatialCoordinatesValue(const DSRTypes::E_GraphicType graphicType)
  : GraphicType(graphicType),
    GraphicDataList()
{
}


DSRSpatialCoordinatesValue::DSRSpatialCoordinatesValue(const DSRSpatialCoordinatesValue &coordinatesValue)
  : GraphicType(coordinatesValue.GraphicType),
    GraphicDataList(coordinatesValue.GraphicDataList)
{
}


DSRSpatialCoordinatesValue::~DSRSpatialCoordinatesValue()
{
}


DSRSpatialCoordinatesValue &DSRSpatialCoordinatesValue::operator=(const DSRSpatialCoordinatesValue &coordinatesValue)
{
    GraphicType = coordinatesValue.GraphicType;
    GraphicDataList = coordinatesValue.GraphicDataList;
    return *this;
}


void DSRSpatialCoordinatesValue::clear()
{
    GraphicType = DSRTypes::GT_invalid;
    GraphicDataList.clear();
}


OFBool DSRSpatialCoordinatesValue::isValid() const
{
    return checkData(GraphicType, GraphicDataList);
}


OFBool DSRSpatialCoordinatesValue::isShort(const size_t flags) const
{
    return GraphicDataList.isEmpty() || !(flags & DSRTypes::HF_renderFullData);
}


E_Condition DSRSpatialCoordinatesValue::print(ostream &stream,
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


E_Condition DSRSpatialCoordinatesValue::writeXML(ostream &stream,
                                                 const size_t flags,
                                                 OFConsole * /* logStream */) const
{
    /* GraphicType is written in TreeNode class */
    if ((flags & DSRTypes::XF_writeEmptyTags) || !GraphicDataList.isEmpty())
    {
        stream << "<data>";
        GraphicDataList.print(stream);        
        stream << "</data>" << endl;        
    }
    return EC_Normal;
}


E_Condition DSRSpatialCoordinatesValue::read(DcmItem &dataset,
                                             OFConsole *logStream)
{
    /* read GraphicType */
    OFString string;
    E_Condition result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_GraphicType, string, "1", "1", logStream, "SCOORD content item");
    if (result == EC_Normal)
    {
        GraphicType = DSRTypes::enumeratedValueToGraphicType(string);
        /* read GraphicData */
        result = GraphicDataList.read(dataset, logStream);
        /* check GraphicData and report warnings if any */
        checkData(GraphicType, GraphicDataList, logStream);
    }
    return result;
}


E_Condition DSRSpatialCoordinatesValue::write(DcmItem &dataset,
                                              OFConsole *logStream) const
{
    /* write GraphicType */
    E_Condition result = DSRTypes::putStringValueToDataset(dataset, DCM_GraphicType, DSRTypes::graphicTypeToEnumeratedValue(GraphicType));
    /* write GraphicData */
    if (result == EC_Normal)
    {
        if (!GraphicDataList.isEmpty())
            result = GraphicDataList.write(dataset, logStream);
    }
    /* check GraphicData and report warnings if any */
    checkData(GraphicType, GraphicDataList, logStream);
    return result;
}


E_Condition DSRSpatialCoordinatesValue::renderHTML(ostream &docStream,
                                                   ostream &annexStream,
                                                   size_t &annexNumber,
                                                   const size_t flags,
                                                   OFConsole * /* logStream */) const
{
    /* render GraphicType */
    docStream << DSRTypes::graphicTypeToReadableName(GraphicType);
    /* render GraphicData */
    if (!isShort(flags))
    {
        if (flags & DSRTypes::HF_currentlyInsideAnnex)
        {
            docStream << endl << "<p>" << endl;
            /* render graphic data list (= print)*/
            docStream << "<b>Graphic Data:</b><br>";
            GraphicDataList.print(docStream);
            docStream << "</p>";
        } else {
            DSRTypes::createHTMLAnnexEntry(docStream, annexStream, "for more details see", annexNumber);
            annexStream << "<p>" << endl;
            /* render graphic data list (= print)*/
            annexStream << "<b>Graphic Data:</b><br>";
            GraphicDataList.print(annexStream);
            annexStream << "</p>" << endl;
        }
    }
    return EC_Normal;
}


E_Condition DSRSpatialCoordinatesValue::setGraphicType(const DSRTypes::E_GraphicType graphicType)
{
    E_Condition result = EC_IllegalCall;
    if (graphicType != DSRTypes::GT_invalid)
    {
        GraphicType = graphicType;
        result = EC_Normal;
    }
    return result;
}


E_Condition DSRSpatialCoordinatesValue::getValue(DSRSpatialCoordinatesValue &coordinatesValue) const
{
    coordinatesValue = *this;
    return EC_Normal;
}


E_Condition DSRSpatialCoordinatesValue::setValue(const DSRSpatialCoordinatesValue &coordinatesValue)
{
    E_Condition result = EC_IllegalCall;
    if (checkData(coordinatesValue.GraphicType, coordinatesValue.GraphicDataList))
    {
        GraphicType = coordinatesValue.GraphicType;
        GraphicDataList = coordinatesValue.GraphicDataList;
        result = EC_Normal;
    }
    return result;
}


OFBool DSRSpatialCoordinatesValue::checkData(const DSRTypes::E_GraphicType graphicType,
                                             const DSRGraphicDataList &graphicDataList,
                                             OFConsole *logStream) const
{
    OFBool result = OFFalse;
    if (graphicType == DSRTypes::GT_invalid)
        DSRTypes::printWarningMessage(logStream, "Invalid GraphicType for SCOORD content item");
    else if (graphicDataList.isEmpty())
        DSRTypes::printWarningMessage(logStream, "No GraphicData for SCOORD content item");
    else
    {
        const size_t count = graphicDataList.getNumberOfItems();
        switch (graphicType)
        {
            case DSRTypes::GT_Point:
                if (count > 1)
                    DSRTypes::printWarningMessage(logStream, "GraphicData has too many entries, only a single entry expected");
                result = OFTrue;
                break;
            case DSRTypes::GT_Multipoint:
                if (count < 1)
                    DSRTypes::printWarningMessage(logStream, "GraphicData has too less entries, at least one entry expected");
                result = OFTrue;
                break;
            case DSRTypes::GT_Polyline:
/*              // not required any more according to CP-233
                if (!(graphicDataList.getItem(1) == graphicDataList.getItem(count)))
                    DSRTypes::printWarningMessage(logStream, "First and last entry in GraphicData are not equal (POLYLINE)");
*/
                result = OFTrue;
                break;
            case DSRTypes::GT_Circle:
                if (count < 2)
                    DSRTypes::printWarningMessage(logStream, "GraphicData has too less entries, exactly two entries expected");
                else
                {
                    if (count > 2)
                        DSRTypes::printWarningMessage(logStream, "GraphicData has too many entries, exactly two entries expected");
                    result = OFTrue;
                }
                break;
            case DSRTypes::GT_Ellipse:
                if (count < 4)
                    DSRTypes::printWarningMessage(logStream, "GraphicData has too less entries, exactly four entries expected");
                else
                {
                    if (count > 4)
                        DSRTypes::printWarningMessage(logStream, "GraphicData has too many entries, exactly four entries expected");
                    result = OFTrue;
                }
                break;
            default:
                /* GT_invalid */
                break;
        }
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrscovl.cc,v $
 *  Revision 1.8  2001-02-13 16:35:28  joergr
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
