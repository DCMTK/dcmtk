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
 *    classes: DSRTCoordTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-26 14:40:28 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtcotn.h"


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


E_Condition DSRTCoordTreeNode::print(ostream &stream,
                                     const size_t flags) const
{
    E_Condition result = DSRDocumentTreeNode::print(stream, flags);
    if (result == EC_Normal)
    {
        stream << "=";
        result = DSRTemporalCoordinatesValue::print(stream, flags);
    }
    return result;
}


E_Condition DSRTCoordTreeNode::readContentItem(DcmItem &dataset,
                                               OFConsole *logStream)
{
    /* read TemporalCoordinates */
    return DSRTemporalCoordinatesValue::read(dataset, logStream);
}


E_Condition DSRTCoordTreeNode::writeContentItem(DcmItem &dataset,
                                                OFConsole *logStream) const
{
    /* write TemporalCoordinates */
    return DSRTemporalCoordinatesValue::write(dataset, logStream);
}


E_Condition DSRTCoordTreeNode::renderHTMLContentItem(ostream &docStream,
                                                     ostream &annexStream,
                                                      const size_t /* nestingLevel */,
                                                      size_t &annexNumber,
                                                      const size_t flags,
                                                      OFConsole *logStream) const
{
    /* render ConceptName */
    E_Condition result = renderHTMLConceptName(docStream, flags, logStream);
    /* render TemporalCoordinates */
    if (result == EC_Normal)
        result = DSRTemporalCoordinatesValue::renderHTML(docStream, annexStream, annexNumber, flags, logStream);
    return result;
}


OFBool DSRTCoordTreeNode::canAddNode(const E_DocumentType documentType,
                                     const E_RelationshipType relationshipType,
                                     const E_ValueType valueType) const
{
    OFBool result = OFFalse;
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
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtcotn.cc,v $
 *  Revision 1.1  2000-10-26 14:40:28  joergr
 *  Added support for TCOORD content item.
 *
 *
 *
 */
