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
 *  Update Date:      $Date: 2000-10-13 07:52:25 $
 *  CVS/RCS Revision: $Revision: 1.1 $
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
    return (GraphicType != DSRTypes::GT_invalid) && checkData();
}


OFBool DSRSpatialCoordinatesValue::isShort(const size_t flags) const
{
    return (GraphicDataList.isEmpty()) || !(flags & DSRTypes::HF_renderFullData);
}


E_Condition DSRSpatialCoordinatesValue::print(ostream &stream,
                                              const size_t flags) const
{
    E_Condition result = EC_Normal;
    /* GraphicType */
    stream << " = " << DSRTypes::graphicTypeToEnumeratedValue(GraphicType);
    /* GraphicData */
    if (!GraphicDataList.isEmpty())
    {
        stream << ", \"";
        result = GraphicDataList.print(stream, flags);
        stream << "\"";
    }
    return EC_Normal;
}


E_Condition DSRSpatialCoordinatesValue::read(DcmItem &dataset,
                                             OFConsole *logStream)
{
    /* read GraphicType */
    OFString string;
    E_Condition result = DSRTypes::getStringValueFromDataset(dataset, DCM_GraphicType, string);
    if (result == EC_Normal)
    {
        GraphicType = DSRTypes::enumeratedValueToGraphicType(string);
        /* read GraphicData */
        result = GraphicDataList.read(dataset, logStream);
    }
    /* check GraphicData and report warnings if any */
    checkData(logStream);
    return result;
}


E_Condition DSRSpatialCoordinatesValue::write(DcmItem &dataset,
                                              OFConsole *logStream) const
{
    /* write GraphicType */
    E_Condition result = DSRTypes::putStringValueToDataset(dataset, DCM_GraphicType, DSRTypes::graphicTypeToEnumeratedValue(GraphicType));;
    /* write GraphicData */
    if (result == EC_Normal)
    {
        if (!GraphicDataList.isEmpty())
            result = GraphicDataList.write(dataset, logStream);
    }
    /* check GraphicData and report warnings if any */
    checkData(logStream);
    return result;
}


E_Condition DSRSpatialCoordinatesValue::renderHTML(ostream &docStream,
                                                   ostream &annexStream,
                                                   size_t &annexNumber,
                                                   const size_t flags,
                                                   OFConsole * /* logStream */) const
{
    /* render GraphicType */
    docStream << DSRTypes::graphicTypeToReadableName(GraphicType) << endl;
    /* render GraphicData */
    if (!GraphicDataList.isEmpty() && (flags & DSRTypes::HF_renderFullData))
    {
        if (flags & DSRTypes::HF_currentlyInsideAnnex)
        {
            docStream << "<p>" << endl;
            /* render graphic data list (= print)*/
            docStream << "<b>Graphic Data:</b><br>";
            GraphicDataList.print(docStream);
            docStream << "</p>" << endl;
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
    E_Condition result = setGraphicType(coordinatesValue.GraphicType);
    if (result == EC_Normal)
        GraphicDataList = coordinatesValue.GraphicDataList;
    return result;
}


OFBool DSRSpatialCoordinatesValue::checkData(OFConsole *logStream) const
{
    OFBool result = OFFalse;
    if (!GraphicDataList.isEmpty())
    {
        const size_t count = GraphicDataList.getNumberOfItems();
        switch (GraphicType)
        {
            case DSRTypes::GT_Point:
                if (count > 1)
                    DSRTypes::printWarningMessage(logStream, "GraphicData has too many entries, only a single entry expected");
                result = OFTrue;
                break;
            case DSRTypes::GT_Multipoint:
            case DSRTypes::GT_Polyline:
                result = OFTrue;
                break;
            case DSRTypes::GT_Circle:
            case DSRTypes::GT_Ellipse:
                if (count < 2)
                    DSRTypes::printWarningMessage(logStream, "GraphicData has too less entries, exactly two entries expected");
                else
                {
                    if (count > 2)
                        DSRTypes::printWarningMessage(logStream, "GraphicData has too many entries, exactly two entries expected");
                    result = OFTrue;
                }
            default:
                /* invalid */
                break;
        }
    } else
        DSRTypes::printWarningMessage(logStream, "No GraphicData for SCOORD content item");
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrscovl.cc,v $
 *  Revision 1.1  2000-10-13 07:52:25  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
