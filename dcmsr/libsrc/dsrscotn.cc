/*
 *
 *  Copyright (C) 2000-2003, OFFIS
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
 *    classes: DSRSCoordTreeNode
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-06-04 14:26:54 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrscotn.h"


DSRSCoordTreeNode::DSRSCoordTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_SCoord),
   DSRSpatialCoordinatesValue()
{
}


DSRSCoordTreeNode::~DSRSCoordTreeNode()
{
}


void DSRSCoordTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRSpatialCoordinatesValue::clear();
}


OFBool DSRSCoordTreeNode::isValid() const
{
    return DSRDocumentTreeNode::isValid() && DSRSpatialCoordinatesValue::isValid();
}


OFBool DSRSCoordTreeNode::isShort(const size_t flags) const
{
    return DSRSpatialCoordinatesValue::isShort(flags);
}


OFCondition DSRSCoordTreeNode::print(ostream &stream,
                                     const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        stream << "=";
        result = DSRSpatialCoordinatesValue::print(stream, flags);
    }
    return result;
}


OFCondition DSRSCoordTreeNode::writeXML(ostream &stream,
                                        const size_t flags,
                                        OFConsole *logStream) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags, OFFalse /* closingBracket */);
    stream << " type=\"" << graphicTypeToEnumeratedValue(getGraphicType()) << "\"";
    stream << ">" << endl;
    result = DSRDocumentTreeNode::writeXML(stream, flags, logStream);
    DSRSpatialCoordinatesValue::writeXML(stream, flags, logStream);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRSCoordTreeNode::readContentItem(DcmItem &dataset,
                                               OFConsole *logStream)
{
    /* read SpatialCoordinates */
    return DSRSpatialCoordinatesValue::read(dataset, logStream);
}


OFCondition DSRSCoordTreeNode::writeContentItem(DcmItem &dataset,
                                                OFConsole *logStream) const
{
    /* write SpatialCoordinates */
    return DSRSpatialCoordinatesValue::write(dataset, logStream);
}


OFCondition DSRSCoordTreeNode::renderHTMLContentItem(ostream &docStream,
                                                     ostream &annexStream,
                                                      const size_t /* nestingLevel */,
                                                      size_t &annexNumber,
                                                      const size_t flags,
                                                      OFConsole *logStream) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags, logStream);
    /* render SpatialCoordinates */
    if (result.good())
    {
        result = DSRSpatialCoordinatesValue::renderHTML(docStream, annexStream, annexNumber, flags, logStream);
        docStream << endl;
    }
    return result;
}


OFBool DSRSCoordTreeNode::canAddNode(const E_DocumentType documentType,
                                     const E_RelationshipType relationshipType,
                                     const E_ValueType valueType,
                                     const OFBool byReference) const
{
    OFBool result = OFFalse;
    if ((!byReference && (documentType != DT_KeyObjectDoc)) || (documentType == DT_ComprehensiveSR))
    {
        switch (relationshipType)
        {
            case RT_hasConceptMod:
                result = (valueType == VT_Text) || (valueType == VT_Code);
                break;
            case RT_selectedFrom:
                if (valueType == VT_Image)
                    result = (documentType == DT_EnhancedSR) || (documentType == DT_ComprehensiveSR);
                break;
            default:
                break;
        }
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrscotn.cc,v $
 *  Revision 1.11  2003-06-04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.10  2001/11/09 16:17:53  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.9  2001/10/10 15:30:00  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.8  2001/09/26 13:04:24  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.7  2001/05/07 16:14:25  joergr
 *  Updated CVS header.
 *
 *  Revision 1.6  2001/02/02 14:41:52  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.5  2000/11/07 18:33:31  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.4  2000/11/01 16:37:03  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.3  2000/10/26 14:33:53  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.2  2000/10/16 12:07:29  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:24  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
