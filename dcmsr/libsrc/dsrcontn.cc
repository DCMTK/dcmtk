/*
 *
 *  Copyright (C) 2000-2002, OFFIS
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
 *  Update Date:      $Date: 2002-12-05 13:52:52 $
 *  CVS/RCS Revision: $Revision: 1.19 $
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


OFCondition DSRContainerTreeNode::print(ostream &stream,
                                        const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
        stream << "=" << continuityOfContentToEnumeratedValue(ContinuityOfContent);
    return result;
}


OFCondition DSRContainerTreeNode::readContentItem(DcmItem &dataset,
                                                  OFConsole *logStream)
{
    OFString string;
    /* read ContinuityOfContent */
    OFCondition result = getAndCheckStringValueFromDataset(dataset, DCM_ContinuityOfContent, string, "1", "1", logStream, "CONTAINER content item");
    if (result.good())
    {
        ContinuityOfContent = enumeratedValueToContinuityOfContent(string);
        /* check ContinuityOfContent value */
        if (ContinuityOfContent == COC_invalid)
        {
            OFString message = "Reading unknown ContinuityOfContent value ";
            message += string;
            printWarningMessage(logStream, message.c_str());
        }
    }
    return result;
}


OFCondition DSRContainerTreeNode::writeContentItem(DcmItem &dataset,
                                                   OFConsole * /* logStream */) const
{
    /* write ContinuityOfContent */
    return putStringValueToDataset(dataset, DCM_ContinuityOfContent, continuityOfContentToEnumeratedValue(ContinuityOfContent));
}


OFCondition DSRContainerTreeNode::writeXML(ostream &stream,
                                           const size_t flags,
                                           OFConsole *logStream) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags, OFFalse /* closingBracket */);
    stream << " flag=\"" << continuityOfContentToEnumeratedValue(ContinuityOfContent) << "\"";
    stream << ">" << endl;
    result = DSRDocumentTreeNode::writeXML(stream, flags, logStream);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRContainerTreeNode::renderHTMLContentItem(ostream &docStream,
                                                        ostream & /* annexStream */,
                                                        const size_t nestingLevel,
                                                        size_t & /* annexNumber */,
                                                        const size_t flags,
                                                        OFConsole *logStream) const
{
    /* section heading (optional) */
    if (nestingLevel > 0)
    {
        /* render ConceptName & Code (if valid) */
        if (getConceptName().getCodeMeaning().length() > 0)
        {
            const size_t section = (nestingLevel > 6) ? 6 : nestingLevel;
            docStream << "<h" << section << ">";
            getConceptName().renderHTML(docStream, flags, logStream, (flags & HF_renderConceptNameCodes) && getConceptName().isValid() /* fullCode */);
            docStream << "</h" << section << ">" << endl;
        }
        /* render optional observation datetime */
        if (getObservationDateTime().length() > 0)
        {
            OFString string;
            docStream << "<small>(observed: " << dicomToReadableDateTime(getObservationDateTime(), string) << ")</small>" << endl;
        }
    }
    return EC_Normal;
}


OFCondition DSRContainerTreeNode::renderHTML(ostream &docStream,
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
    OFCondition result = renderHTMLContentItem(docStream, annexStream, nestingLevel, annexNumber, flags, logStream);
    if (result.good())
    {
        /* section body: render child nodes */
        if (ContinuityOfContent == COC_Continuous)
            result = renderHTMLChildNodes(docStream, annexStream, nestingLevel, annexNumber, flags & ~HF_renderItemsSeparately, logStream);
        else  // might be invalid
            result = renderHTMLChildNodes(docStream, annexStream, nestingLevel, annexNumber, flags | HF_renderItemsSeparately, logStream);
    } else
        printContentItemErrorMessage(logStream, "Rendering", result, this);
    return result;
}


OFBool DSRContainerTreeNode::canAddNode(const E_DocumentType documentType,
                                        const E_RelationshipType relationshipType,
                                        const E_ValueType valueType,
                                        const OFBool byReference) const
{
    OFBool result = OFFalse;
    if (!byReference || (documentType == DT_ComprehensiveSR))
    {
        switch (relationshipType)
        {
            case RT_contains:
                switch (valueType)
                {
                    case VT_Text:
                    case VT_Image:
                    case VT_Waveform:
                    case VT_Composite:
                        result = (!byReference);        /* only allowed for by-value relationships */;
                        break;
                    case VT_Code:
                    case VT_DateTime:
                    case VT_Date:
                    case VT_Time:
                    case VT_UIDRef:
                    case VT_PName:
                    case VT_Container:
                        result = (!byReference) && (documentType != DT_KeyObjectDoc);
                        break;
                    case VT_Num:
                    case VT_SCoord:
                    case VT_TCoord:
                        result = (documentType == DT_EnhancedSR) || ((documentType == DT_ComprehensiveSR) && !byReference);
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
                    case VT_UIDRef:
                    case VT_PName:
                        result = OFTrue;
                        break;
                    case VT_DateTime:
                    case VT_Date:
                    case VT_Time:
                    case VT_Composite:          /* allow COMPOSITE according to CP 286 */
                        result = (documentType != DT_KeyObjectDoc);
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
                        result = (documentType != DT_KeyObjectDoc);
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
                result = (valueType == VT_Code) || ((valueType == VT_Text) && (documentType != DT_KeyObjectDoc));
                break;
            default:
                break;
        }
    }
    return result;
}


OFCondition DSRContainerTreeNode::setContinuityOfContent(const E_ContinuityOfContent continuityOfContent)
{
    OFCondition result = EC_IllegalParameter;
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
 *  Revision 1.19  2002-12-05 13:52:52  joergr
 *  Added further checks when reading SR documents (e.g. value of VerificationFlag,
 *  CompletionsFlag, ContinuityOfContent and SpecificCharacterSet).
 *
 *  Revision 1.18  2002/11/25 12:00:34  joergr
 *  Adapted code according to CP 286, i.e. allow a COMPOSITE content item to be
 *  the target of a HAS OBS CONTEXT relationship with a CONTAINER item.
 *
 *  Revision 1.17  2001/11/09 16:14:10  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.16  2001/10/10 15:29:50  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.15  2001/09/26 13:04:18  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.14  2001/05/07 16:14:22  joergr
 *  Updated CVS header.
 *
 *  Revision 1.13  2001/02/02 14:41:54  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.12  2001/01/25 15:32:17  joergr
 *  Allow invalid continuity of content flag when reading SR datasets.
 *
 *  Revision 1.11  2000/11/14 11:18:59  joergr
 *  Added output of optional observation datetime to rendered HTML page.
 *
 *  Revision 1.10  2000/11/09 20:34:00  joergr
 *  Added support for non-ASCII characters in HTML 3.2 (use numeric value).
 *
 *  Revision 1.9  2000/11/09 11:32:12  joergr
 *  Minor HTML code purifications.
 *
 *  Revision 1.8  2000/11/07 18:33:29  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.7  2000/11/01 16:31:17  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.6  2000/10/26 14:27:47  joergr
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
