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
 *    classes: DSRSCoordTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-16 12:07:29 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

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


E_Condition DSRSCoordTreeNode::print(ostream &stream,
                                     const size_t flags) const
{
    E_Condition result = DSRDocumentTreeNode::print(stream, flags);
    if (result == EC_Normal)
    {
        stream << "=";
        result = DSRSpatialCoordinatesValue::print(stream, flags);
    }
    return result;
}


E_Condition DSRSCoordTreeNode::readContentItem(DcmItem &dataset,
                                               OFConsole *logStream)
{
    /* read SpatialCoordinates */
    return DSRSpatialCoordinatesValue::read(dataset, logStream);
}


E_Condition DSRSCoordTreeNode::writeContentItem(DcmItem &dataset,
                                                OFConsole *logStream) const
{
    /* write SpatialCoordinates */
    return DSRSpatialCoordinatesValue::write(dataset, logStream);
}


E_Condition DSRSCoordTreeNode::renderHTMLContentItem(ostream &docStream,
                                                     ostream &annexStream,
                                                      const size_t /* nestingLevel */,
                                                      size_t &annexNumber,
                                                      const size_t flags,
                                                      OFConsole *logStream) const
{
    /* render ConceptName */
    E_Condition result = renderHTMLConceptName(docStream, flags, logStream);
    /* render SpatialCoordinates */
    if (result == EC_Normal)
        result = DSRSpatialCoordinatesValue::renderHTML(docStream, annexStream, annexNumber, flags, logStream);
    return result;
}


OFBool DSRSCoordTreeNode::canAddNode(const E_DocumentType documentType,
                                     const E_RelationshipType relationshipType,
                                     const E_ValueType valueType) const
{
    OFBool result = OFFalse;
    if (relationshipType == RT_selectedFrom)
    {
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
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrscotn.cc,v $
 *  Revision 1.2  2000-10-16 12:07:29  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:24  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
