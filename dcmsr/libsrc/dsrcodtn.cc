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
 *    classes: DSRCodeTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-26 14:26:54 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrcodtn.h"


DSRCodeTreeNode::DSRCodeTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_Code),
   DSRCodedEntryValue()
{
}


DSRCodeTreeNode::~DSRCodeTreeNode()
{
}


void DSRCodeTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRCodedEntryValue::clear();
}


OFBool DSRCodeTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && DSRCodedEntryValue::isValid() && getConceptName().isValid();
}


E_Condition DSRCodeTreeNode::print(ostream &stream,
                                   const size_t flags) const
{
    E_Condition result = DSRDocumentTreeNode::print(stream, flags);
    if (result == EC_Normal)
    {
        stream << "=";
        DSRCodedEntryValue::print(stream, OFTrue /* printCodeValue */, OFTrue /* printInvalid */);
    }
    return result;
}


E_Condition DSRCodeTreeNode::readContentItem(DcmItem &dataset,
                                             OFConsole *logStream)
{    
    /* read ConceptCodeSequence */
    return DSRCodedEntryValue::readSequence(dataset, DCM_ConceptCodeSequence, "1" /* type */, logStream);
}


E_Condition DSRCodeTreeNode::writeContentItem(DcmItem &dataset,
                                              OFConsole *logStream) const
{
    /* write ConceptCodeSequence */
    return DSRCodedEntryValue::writeSequence(dataset, DCM_ConceptCodeSequence, logStream);
}


E_Condition DSRCodeTreeNode::renderHTMLContentItem(ostream &docStream,
                                                   ostream & /* annexStream */,
                                                   const size_t /* nestingLevel */,
                                                   size_t & /* annexNumber */,
                                                   const size_t flags,
                                                   OFConsole *logStream) const
{
    /* render ConceptName */
    E_Condition result = renderHTMLConceptName(docStream, flags, logStream);
    /* render Code */
    if (result == EC_Normal)
        result = DSRCodedEntryValue::renderHTML(docStream, logStream);
    return result;
}


OFBool DSRCodeTreeNode::canAddNode(const E_DocumentType documentType,
                                   const E_RelationshipType relationshipType,
                                   const E_ValueType valueType) const
{
    OFBool result = OFFalse;
    switch (relationshipType)
    {
        case RT_hasObsContext:
            switch (valueType)
            {
                case VT_Text:                
                case VT_Code:
                case VT_Num:
                case VT_DateTime:
                case VT_Date:
                case VT_Time:
                case VT_UIDRef:
                case VT_PName:
                    result = (documentType == DT_ComprehensiveSR);
                    break;
                default:
                    break;
            }
            break;
        case RT_hasConceptMod:
            result = (valueType == VT_Text) || (valueType == VT_Code);
            break;
        case RT_inferredFrom:
        case RT_hasProperties:
            switch (valueType)
            {
                case VT_Text:
                case VT_Code:
                case VT_Num:
                case VT_DateTime:
                case VT_Date:
                case VT_Time:
                case VT_UIDRef:
                case VT_PName:
                case VT_Composite:
                case VT_Image:
                case VT_Waveform:
                case VT_SCoord:
                case VT_TCoord:
                    result = (documentType == DT_ComprehensiveSR);
                    break;
/*                    
                case VT_Container:
                    result = (documentType == DT_ComprehensiveSR);  // only by-reference - to be checked !
                    break;
*/
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
 *  $Log: dsrcodtn.cc,v $
 *  Revision 1.5  2000-10-26 14:26:54  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.4  2000/10/23 15:04:45  joergr
 *  Added clear() method.
 *
 *  Revision 1.3  2000/10/18 17:12:53  joergr
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.2  2000/10/16 12:01:55  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:16  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
