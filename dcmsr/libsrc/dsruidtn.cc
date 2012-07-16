/*
 *
 *  Copyright (C) 2000-2012, OFFIS e.V.
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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRUIDRefTreeNode
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsruidtn.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRUIDRefTreeNode::DSRUIDRefTreeNode(const E_RelationshipType relationshipType)
  : DSRDocumentTreeNode(relationshipType, VT_UIDRef),
    DSRStringValue()
{
}


DSRUIDRefTreeNode::DSRUIDRefTreeNode(const E_RelationshipType relationshipType,
                                     const OFString &uidValue,
                                     const OFBool check)
  : DSRDocumentTreeNode(relationshipType, VT_UIDRef),
    DSRStringValue(uidValue, check)
{
}


DSRUIDRefTreeNode::~DSRUIDRefTreeNode()
{
}


void DSRUIDRefTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRStringValue::clear();
}


OFBool DSRUIDRefTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && getConceptName().isValid() && checkCurrentValue().good();
}


OFCondition DSRUIDRefTreeNode::print(STD_NAMESPACE ostream &stream,
                                     const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result ==EC_Normal)
    {
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
        stream << "=";
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_ITEM_VALUE)
        DSRStringValue::print(stream);
    }
    return result;
}


OFCondition DSRUIDRefTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                        const size_t flags) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    writeStringValueToXML(stream, getValue(), "value", (flags & XF_writeEmptyTags) > 0);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRUIDRefTreeNode::readContentItem(DcmItem &dataset)
{
    /* read UID */
    return DSRStringValue::read(dataset, DCM_UID);
}


OFCondition DSRUIDRefTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write UID */
    return DSRStringValue::write(dataset, DCM_UID);
}


OFCondition DSRUIDRefTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                  DSRXMLCursor cursor)
{
    /* retrieve value from XML element "value" */
    return DSRStringValue::readXML(doc, doc.getNamedNode(cursor.gotoChild(), "value"));
}


OFCondition DSRUIDRefTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                     STD_NAMESPACE ostream & /*annexStream*/,
                                                     const size_t /*nestingLevel*/,
                                                     size_t & /*annexNumber*/,
                                                     const size_t flags) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags);
    /* render UID */
    if (result.good())
    {
        result = DSRStringValue::renderHTML(docStream, flags);
        docStream << OFendl;
    }
    return result;
}


OFCondition DSRUIDRefTreeNode::checkValue(const OFString &uidValue) const
{
    /* first, make sure that the mandatory value is non-empty */
    OFCondition result = DSRStringValue::checkValue(uidValue);
    /* then, check whether the passed value is valid */
    if (result.good())
        result = DcmUniqueIdentifier::checkStringValue(uidValue, "1");
    return result;
}
