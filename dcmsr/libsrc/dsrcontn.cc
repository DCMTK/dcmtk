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
 *    classes: DSRContainerTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-26 14:27:47 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrcontn.h"


DSRContainerTreeNode::DSRContainerTreeNode(const E_RelationshipType relationshipType,
                                           const E_ContinuityOfContent continuityOfContent)
 : DSRDocumentTreeNode(relationshipType, VT_Container),
   ContinuityOfContent(continuityOfContent)
{
}


DSRContainerTreeNode::~DSRContainerTreeNode()
{
}


void DSRContainerTreeNode::clear()
{
   ContinuityOfContent = COC_Separate;      // this is more useful that COC_invalid
}


OFBool DSRContainerTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required for root node container */
    return DSRDocumentTreeNode::isValid() && (ContinuityOfContent != COC_invalid) &&
        ((getRelationshipType() != RT_isRoot) || getConceptName().isValid());
}


OFBool DSRContainerTreeNode::isShort(const size_t /* flags */) const
{
    return OFFalse;
}


E_Condition DSRContainerTreeNode::print(ostream &stream,
                                        const size_t flags) const
{
    E_Condition result = DSRDocumentTreeNode::print(stream, flags);
    if (result == EC_Normal)
        stream << "=" << continuityOfContentToEnumeratedValue(ContinuityOfContent);
    return result;
}


E_Condition DSRContainerTreeNode::readContentItem(DcmItem &dataset,
                                                  OFConsole *logStream)
{
    OFString string;
    /* read ContinuityOfContent */
    E_Condition result = getAndCheckStringValueFromDataset(dataset, DCM_ContinuityOfContent, string, "1", "1", logStream, "CONTAINER content item");
    if (result == EC_Normal)
        result = setContinuityOfContent(enumeratedValueToContinuityOfContent(string));
    return result;
}


E_Condition DSRContainerTreeNode::writeContentItem(DcmItem &dataset,
                                                   OFConsole * /* logStream */) const
{
    /* write ContinuityOfContent */
    return putStringValueToDataset(dataset, DCM_ContinuityOfContent, continuityOfContentToEnumeratedValue(ContinuityOfContent));
}


E_Condition DSRContainerTreeNode::renderHTMLContentItem(ostream &docStream,
                                                        ostream & /* annexStream */,
                                                        const size_t nestingLevel,
                                                        size_t & /* annexNumber */,
                                                        const size_t flags,
                                                        OFConsole *logStream) const
{
    /* section heading (optional) */
    if (nestingLevel > 0)
    {
        if (flags & HF_renderAllCodes)
        {
            /* render ConceptName & Code (if valid) */
            if (getConceptName().isValid())
            {
                docStream << "<h" << nestingLevel << ">";
                getConceptName().renderHTML(docStream, logStream);
                docStream << "</h" << nestingLevel << ">" << endl;
            }
        } else {
            /* render ConceptName only */
            if (getConceptName().getCodeMeaning().length() > 0)
            {
                docStream << "<h" << nestingLevel << ">";
                docStream << getConceptName().getCodeMeaning();
                docStream << "</h" << nestingLevel << ">" << endl;
            }
        }
    }
    return EC_Normal;
}


E_Condition DSRContainerTreeNode::renderHTML(ostream &docStream,
                                             ostream &annexStream,
                                             const size_t nestingLevel,
                                             size_t &annexNumber,
                                             const size_t flags,
                                             OFConsole *logStream) const
{
    /* check for validity */
    if (!isValid())
        printInvalidContentItemMessage(logStream, "Rendering", this);
    /* render content item */
    E_Condition result = renderHTMLContentItem(docStream, annexStream, nestingLevel, annexNumber, flags, logStream);
    if (result == EC_Normal)
    {
        /* section body: render child nodes */
        if (ContinuityOfContent == COC_Continuous)
            result = renderHTMLChildNodes(docStream, annexStream, nestingLevel, annexNumber, flags & ~HF_renderItemsSeparately, logStream);
        else
            result = renderHTMLChildNodes(docStream, annexStream, nestingLevel, annexNumber, flags | HF_renderItemsSeparately, logStream);
    } else
        printContentItemErrorMessage(logStream, "Rendering", result, this);
    return result;
}


OFBool DSRContainerTreeNode::canAddNode(const E_DocumentType documentType,
                                        const E_RelationshipType relationshipType,
                                        const E_ValueType valueType) const
{
    OFBool result = OFFalse;
    switch (relationshipType)
    {
        case RT_contains:
            switch (valueType)
            {
                case VT_Text:                
                case VT_Code:
                case VT_DateTime:
                case VT_Date:
                case VT_Time:
                case VT_UIDRef:
                case VT_PName:
                case VT_Composite:
                case VT_Image:
                case VT_Waveform:
                case VT_Container:
                    result = OFTrue;
                    break;
                case VT_Num:
                case VT_SCoord:
                case VT_TCoord:
                    result = (documentType == DT_EnhancedSR) || (documentType == DT_ComprehensiveSR);
                    break;
                default:
                    break;
            }
            break;
        case RT_hasObsContext:
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
                default:
                    break;
            }
            break;
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
/*
                case VT_Container:
                    result = (documentType == DT_ComprehensiveSR);  // only by-reference - to be checked !
                    break;
*/
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
    return result;
}


E_Condition DSRContainerTreeNode::setContinuityOfContent(const E_ContinuityOfContent continuityOfContent)
{
    E_Condition result = EC_IllegalCall;
    if (continuityOfContent != COC_invalid)
    {
        ContinuityOfContent = continuityOfContent;
        result = EC_Normal;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrcontn.cc,v $
 *  Revision 1.6  2000-10-26 14:27:47  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.5  2000/10/23 15:02:49  joergr
 *  Enhanced implementation of method isValid().
 *
 *  Revision 1.4  2000/10/19 16:03:48  joergr
 *  Added optional module name to read method to provide more detailed warning
 *  messages.
 *
 *  Revision 1.3  2000/10/18 17:14:30  joergr
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.2  2000/10/16 12:01:56  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:18  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
