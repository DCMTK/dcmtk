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
 *    classes: DSRCompositeTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-16 12:01:55 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrcomtn.h"

#include "dcuid.h"


DSRCompositeTreeNode::DSRCompositeTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_Composite),
   DSRReferenceValue()
{
}


DSRCompositeTreeNode::~DSRCompositeTreeNode()
{
}


void DSRCompositeTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRReferenceValue::clear();
}


OFBool DSRCompositeTreeNode::isValid() const
{
    return DSRDocumentTreeNode::isValid() && DSRReferenceValue::isValid();
}


E_Condition DSRCompositeTreeNode::print(ostream &stream,
                                        const size_t flags) const
{
    E_Condition result = DSRDocumentTreeNode::print(stream, flags);
    if (result == EC_Normal)
    {
        stream << "=";
        result = DSRReferenceValue::print(stream, flags);
    }
    return result;
}


E_Condition DSRCompositeTreeNode::readContentItem(DcmItem &dataset,
                                                  OFConsole *logStream)
{
    /* read ReferencedSOPSequence */
    return DSRReferenceValue::readSequence(dataset, logStream);
}


E_Condition DSRCompositeTreeNode::writeContentItem(DcmItem &dataset,
                                                   OFConsole *logStream) const
{
    /* write ReferencedSOPSequence */
    return DSRReferenceValue::writeSequence(dataset, logStream);
}


E_Condition DSRCompositeTreeNode::renderHTMLContentItem(ostream &docStream,
                                                        ostream &annexStream,
                                                        const size_t /* nestingLevel */,
                                                        size_t &annexNumber,
                                                        const size_t flags,
                                                        OFConsole *logStream) const
{
    /* render ConceptName */
    E_Condition result = renderHTMLConceptName(docStream, flags, logStream);
    /* render Reference */
    if (result == EC_Normal)
        result = DSRReferenceValue::renderHTML(docStream, annexStream, annexNumber, flags, logStream);
    return result;
}


OFBool DSRCompositeTreeNode::canAddNode(const E_DocumentType documentType,
                                        const E_RelationshipType relationshipType,
                                        const E_ValueType valueType) const
{
    OFBool result = OFFalse;
    switch (relationshipType)
    {
        case RT_hasAcqContext:
            switch (valueType)
            {
                case VT_Text:
                case VT_Code:
                case VT_DateTime:
                case VT_Date:
                case VT_Time:
                case VT_UIDRef:
                case VT_PName:
                    result = OFTrue;
                    break;
                case VT_Num:
                    result = (documentType == DT_EnhancedSR) || (documentType == DT_ComprehensiveSR);
                case VT_Container:
                    result = (documentType == DT_ComprehensiveSR);  /* only by-reference - to be checked ! */
                    break;
                default:
                    break;
            }
            break;
        case RT_hasConceptMod:
            switch (valueType)
            {
                case VT_Text:
                case VT_Code:
                    result = OFTrue;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrcomtn.cc,v $
 *  Revision 1.2  2000-10-16 12:01:55  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:17  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
