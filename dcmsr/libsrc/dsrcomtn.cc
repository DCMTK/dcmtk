/*
 *
 *  Copyright (C) 2000-2001, OFFIS
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
 *  Update Date:      $Date: 2001-05-07 16:14:22 $
 *  CVS/RCS Revision: $Revision: 1.9 $
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
   DSRCompositeReferenceValue()
{
}


DSRCompositeTreeNode::~DSRCompositeTreeNode()
{
}


void DSRCompositeTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRCompositeReferenceValue::clear();
}


OFBool DSRCompositeTreeNode::isValid() const
{
    return DSRDocumentTreeNode::isValid() && DSRCompositeReferenceValue::isValid();
}


E_Condition DSRCompositeTreeNode::print(ostream &stream,
                                        const size_t flags) const
{
    E_Condition result = DSRDocumentTreeNode::print(stream, flags);
    if (result == EC_Normal)
    {
        stream << "=";
        result = DSRCompositeReferenceValue::print(stream, flags);
    }
    return result;
}


E_Condition DSRCompositeTreeNode::writeXML(ostream &stream,
                                           const size_t flags,
                                           OFConsole *logStream) const
{
    E_Condition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags, logStream);
    DSRCompositeReferenceValue::writeXML(stream, flags, logStream);
    writeXMLItemEnd(stream, flags);
    return result;
}


E_Condition DSRCompositeTreeNode::readContentItem(DcmItem &dataset,
                                                  OFConsole *logStream)
{
    /* read ReferencedSOPSequence */
    return DSRCompositeReferenceValue::readSequence(dataset, "1" /* type */, logStream);
}


E_Condition DSRCompositeTreeNode::writeContentItem(DcmItem &dataset,
                                                   OFConsole *logStream) const
{
    /* write ReferencedSOPSequence */
    return DSRCompositeReferenceValue::writeSequence(dataset, logStream);
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
    {
        result = DSRCompositeReferenceValue::renderHTML(docStream, annexStream, annexNumber, flags, logStream);
        docStream << endl;
    }
    return result;
}


OFBool DSRCompositeTreeNode::canAddNode(const E_DocumentType documentType,
                                        const E_RelationshipType relationshipType,
                                        const E_ValueType valueType,
                                        const OFBool byReference) const
{
    OFBool result = OFFalse;
    if (!byReference || (documentType == DT_ComprehensiveSR))
    {
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
                        break;
                    case VT_Container:
                        result = byReference;       /* documentType is already checked */
                        break;
                    default:
                        break;
                }
                break;
            case RT_hasConceptMod:
                result = (valueType == VT_Text) || (valueType == VT_Code);
                break;
            default:
                break;
        }
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrcomtn.cc,v $
 *  Revision 1.9  2001-05-07 16:14:22  joergr
 *  Updated CVS header.
 *
 *  Revision 1.8  2001/02/02 14:41:54  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.7  2000/11/07 18:33:28  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.6  2000/11/01 16:30:08  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.5  2000/10/26 14:26:54  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.4  2000/10/20 10:14:57  joergr
 *  Renamed class DSRReferenceValue to DSRCompositeReferenceValue.
 *
 *  Revision 1.3  2000/10/18 17:13:58  joergr
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.2  2000/10/16 12:01:55  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:17  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
