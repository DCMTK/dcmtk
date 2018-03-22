/*
 *
 *  Copyright (C) 2000-2018, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRContainerTreeNode
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrcontn.h"
#include "dcmtk/dcmsr/dsrxmld.h"

#include "dcmtk/dcmdata/dcdeftag.h"


DSRContainerTreeNode::DSRContainerTreeNode(const E_RelationshipType relationshipType,
                                           const E_ContinuityOfContent continuityOfContent)
  : DSRDocumentTreeNode(relationshipType, VT_Container),
    ContinuityOfContent(continuityOfContent)
{
}


DSRContainerTreeNode::DSRContainerTreeNode(const DSRContainerTreeNode &node)
  : DSRDocumentTreeNode(node),
    ContinuityOfContent(node.ContinuityOfContent)
{
}


DSRContainerTreeNode::~DSRContainerTreeNode()
{
}


OFBool DSRContainerTreeNode::operator==(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator==(node);
    if (result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = (ContinuityOfContent == OFstatic_cast(const DSRContainerTreeNode &, node).ContinuityOfContent);
    }
    return result;
}


OFBool DSRContainerTreeNode::operator!=(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator!=(node);
    if (!result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = (ContinuityOfContent != OFstatic_cast(const DSRContainerTreeNode &, node).ContinuityOfContent);
    }
    return result;
}


DSRContainerTreeNode *DSRContainerTreeNode::clone() const
{
    return new DSRContainerTreeNode(*this);
}


void DSRContainerTreeNode::clear()
{
    ContinuityOfContent = COC_Separate;      // this is more useful that COC_invalid
}


OFBool DSRContainerTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required for root node container */
    return DSRDocumentTreeNode::isValid() && hasValidValue() &&
        ((getRelationshipType() != RT_isRoot) || getConceptName().isValid());
}


OFBool DSRContainerTreeNode::hasValidValue() const
{
    return (ContinuityOfContent != COC_invalid);
}


OFBool DSRContainerTreeNode::isShort(const size_t /*flags*/) const
{
    return OFFalse;
}


OFCondition DSRContainerTreeNode::print(STD_NAMESPACE ostream &stream,
                                        const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
        stream << "=";
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_ITEM_VALUE)
        stream << continuityOfContentToEnumeratedValue(ContinuityOfContent);
    }
    return result;
}


OFCondition DSRContainerTreeNode::readContentItem(DcmItem &dataset,
                                                  const size_t /*flags*/)
{
    OFString tmpString;
    /* read ContinuityOfContent */
    OFCondition result = getAndCheckStringValueFromDataset(dataset, DCM_ContinuityOfContent, tmpString, "1", "1", "CONTAINER content item");
    if (result.good())
    {
        ContinuityOfContent = enumeratedValueToContinuityOfContent(tmpString);
        /* check ContinuityOfContent value */
        if (ContinuityOfContent == COC_invalid)
        {
            printUnknownValueWarningMessage("ContinuityOfContent value", tmpString.c_str());
            result = SR_EC_InvalidValue;
        }
    }
    return result;
}


OFCondition DSRContainerTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write ContinuityOfContent */
    return putStringValueToDataset(dataset, DCM_ContinuityOfContent, continuityOfContentToEnumeratedValue(ContinuityOfContent));
}


OFCondition DSRContainerTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                     DSRXMLCursor cursor,
                                                     const size_t /*flags*/)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        OFString tmpString;
        /* read 'flag' and check validity */
        ContinuityOfContent = enumeratedValueToContinuityOfContent(doc.getStringFromAttribute(cursor, tmpString, "flag"));
        if (ContinuityOfContent == COC_invalid)
        {
            printUnknownValueWarningMessage("CONTAINER flag", tmpString.c_str());
            result = SR_EC_InvalidValue;
        } else
            result = EC_Normal;
    }
    return result;
}


OFCondition DSRContainerTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                           const size_t flags) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags, OFFalse /*closingBracket*/);
    stream << " flag=\"" << continuityOfContentToEnumeratedValue(ContinuityOfContent) << "\"";
    stream << ">" << OFendl;
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRContainerTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                        STD_NAMESPACE ostream & /*annexStream*/,
                                                        const size_t nestingLevel,
                                                        size_t & /*annexNumber*/,
                                                        const size_t flags) const
{
    /* section heading (optional) */
    if (nestingLevel > 0)
    {
        /* render ConceptName & Code (if valid) */
        if (!getConceptName().getCodeMeaning().empty())
        {
            const size_t section = (nestingLevel > 6) ? 6 : nestingLevel;
            docStream << "<h" << section << ">";
            getConceptName().renderHTML(docStream, flags, (flags & HF_renderConceptNameCodes) && getConceptName().isValid() /*fullCode*/);
            docStream << "</h" << section << ">" << OFendl;
        }
        /* render optional observation date/time */
        if (!getObservationDateTime().empty())
        {
            OFString tmpString;
            docStream << "<p>" << OFendl;
            if (flags & HF_XHTML11Compatibility)
                docStream << "<span class=\"observe\">";
            else
                docStream << "<small>";
            docStream << "(observed: " << dicomToReadableDateTime(getObservationDateTime(), tmpString) << ")";
            if (flags & HF_XHTML11Compatibility)
                docStream << "</span>" << OFendl;
            else
                docStream << "</small>" << OFendl;
            docStream << "</p>" << OFendl;
        }
    }
    return EC_Normal;
}


OFCondition DSRContainerTreeNode::renderHTML(STD_NAMESPACE ostream &docStream,
                                             STD_NAMESPACE ostream &annexStream,
                                             const size_t nestingLevel,
                                             size_t &annexNumber,
                                             const size_t flags) const
{
    /* check for validity */
    if (!isValid())
        printInvalidContentItemMessage("Rendering", this);
    /* render content item */
    OFCondition result = renderHTMLContentItem(docStream, annexStream, nestingLevel, annexNumber, flags);
    if (result.good())
    {
        /* section body: render child nodes */
        if (ContinuityOfContent == COC_Continuous)
            result = renderHTMLChildNodes(docStream, annexStream, nestingLevel, annexNumber, flags & ~HF_renderItemsSeparately);
        else  // might be invalid
            result = renderHTMLChildNodes(docStream, annexStream, nestingLevel, annexNumber, flags | HF_renderItemsSeparately);
    } else
        printContentItemErrorMessage("Rendering", result, this);
    return result;
}


OFCondition DSRContainerTreeNode::setContinuityOfContent(const E_ContinuityOfContent continuityOfContent,
                                                         const OFBool /*check*/)
{
    OFCondition result = EC_IllegalParameter;
    /* check whether the passed value is valid */
    if (continuityOfContent != COC_invalid)
    {
        ContinuityOfContent = continuityOfContent;
        result = EC_Normal;
    }
    return result;
}
