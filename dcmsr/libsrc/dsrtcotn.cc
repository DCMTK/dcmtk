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
 *    classes: DSRTCoordTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-08-07 17:29:13 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrtcotn.h"
#include "dsrxmld.h"


DSRTCoordTreeNode::DSRTCoordTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_TCoord),
   DSRTemporalCoordinatesValue()
{
}


DSRTCoordTreeNode::~DSRTCoordTreeNode()
{
}


void DSRTCoordTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRTemporalCoordinatesValue::clear();
}


OFBool DSRTCoordTreeNode::isValid() const
{
    return DSRDocumentTreeNode::isValid() && DSRTemporalCoordinatesValue::isValid();
}


OFBool DSRTCoordTreeNode::isShort(const size_t flags) const
{
    return DSRTemporalCoordinatesValue::isShort(flags);
}


OFCondition DSRTCoordTreeNode::print(ostream &stream,
                                     const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        stream << "=";
        result = DSRTemporalCoordinatesValue::print(stream, flags);
    }
    return result;
}


OFCondition DSRTCoordTreeNode::writeXML(ostream &stream,
                                        const size_t flags,
                                        OFConsole *logStream) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags, OFFalse /*closingBracket*/);
    stream << " type=\"" << temporalRangeTypeToEnumeratedValue(getTemporalRangeType()) << "\"";
    stream << ">" << endl;
    result = DSRDocumentTreeNode::writeXML(stream, flags, logStream);
    DSRTemporalCoordinatesValue::writeXML(stream, flags, logStream);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRTCoordTreeNode::readContentItem(DcmItem &dataset,
                                               OFConsole *logStream)
{
    /* read TemporalCoordinates */
    return DSRTemporalCoordinatesValue::read(dataset, logStream);
}


OFCondition DSRTCoordTreeNode::writeContentItem(DcmItem &dataset,
                                                OFConsole *logStream) const
{
    /* write TemporalCoordinates */
    return DSRTemporalCoordinatesValue::write(dataset, logStream);
}


OFCondition DSRTCoordTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                  DSRXMLCursor cursor)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        OFString tmpString;
        /* read 'type' and check validity */
        result = setTemporalRangeType(enumeratedValueToTemporalRangeType(doc.getStringFromAttribute(cursor, tmpString, "type")));
        if (result.good())
        {
            /* proceed reading the temporal coordinates */
            result = DSRTemporalCoordinatesValue::readXML(doc, cursor);
        } else
            printUnknownValueWarningMessage(doc.getLogStream(), "TCOORD type", tmpString.c_str());
    }
    return result;
}


OFCondition DSRTCoordTreeNode::renderHTMLContentItem(ostream &docStream,
                                                     ostream &annexStream,
                                                     const size_t /*nestingLevel*/,
                                                     size_t &annexNumber,
                                                     const size_t flags,
                                                     OFConsole *logStream) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags, logStream);
    /* render TemporalCoordinates */
    if (result.good())
    {
        result = DSRTemporalCoordinatesValue::renderHTML(docStream, annexStream, annexNumber, flags, logStream);
        docStream << endl;
    }
    return result;
}


OFBool DSRTCoordTreeNode::canAddNode(const E_DocumentType documentType,
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
                switch (valueType)
                {
                    case VT_Image:
                    case VT_Waveform:
                    case VT_SCoord:
                        result = (documentType == DT_EnhancedSR) || (documentType == DT_ComprehensiveSR);
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtcotn.cc,v $
 *  Revision 1.11  2003-08-07 17:29:13  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.10  2003/08/07 13:55:43  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.9  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.8  2001/11/09 16:18:26  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.7  2001/10/10 15:30:03  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.6  2001/09/26 13:04:25  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.5  2001/05/07 16:14:25  joergr
 *  Updated CVS header.
 *
 *  Revision 1.4  2001/02/02 14:41:51  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.3  2000/11/07 18:33:31  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.2  2000/11/01 16:37:04  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.1  2000/10/26 14:40:28  joergr
 *  Added support for TCOORD content item.
 *
 *
 *
 */
