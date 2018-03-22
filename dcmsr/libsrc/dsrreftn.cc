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
 *    classes: DSRByReferenceTreeNode
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrreftn.h"
#include "dcmtk/dcmsr/dsrxmld.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrul.h"


DSRByReferenceTreeNode::DSRByReferenceTreeNode(const E_RelationshipType relationshipType)
  : DSRDocumentTreeNode(relationshipType, VT_byReference),
    ValidReference(OFFalse),
    ReferencedContentItem(),
    ReferencedNodeID(0),
    TargetValueType(VT_invalid)
{
}


DSRByReferenceTreeNode::DSRByReferenceTreeNode(const E_RelationshipType relationshipType,
                                               const size_t referencedNodeID,
                                               const E_ValueType targetValueType)
  : DSRDocumentTreeNode(relationshipType, VT_byReference),
    ValidReference(OFFalse),
    ReferencedContentItem(),
    ReferencedNodeID(referencedNodeID),
    TargetValueType(targetValueType)
{
}


DSRByReferenceTreeNode::DSRByReferenceTreeNode(const DSRByReferenceTreeNode &node)
  : DSRDocumentTreeNode(node),
    ValidReference(OFFalse),
    ReferencedContentItem(node.ReferencedContentItem),
    ReferencedNodeID(0),
    TargetValueType(VT_invalid)
{
}


DSRByReferenceTreeNode::~DSRByReferenceTreeNode()
{
}


OFBool DSRByReferenceTreeNode::operator==(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator==(node);
    if (result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        const DSRByReferenceTreeNode &byRefNode = OFstatic_cast(const DSRByReferenceTreeNode &, node);
        if (ValidReference && byRefNode.ValidReference)
        {
            /* check referenced node ID only */
            result = (ReferencedNodeID == byRefNode.ReferencedNodeID);
        } else {
            /* check whether both references are invalid */
            result = (ValidReference == byRefNode.ValidReference);
        }
    }
    return result;
}


OFBool DSRByReferenceTreeNode::operator!=(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator!=(node);
    if (!result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        const DSRByReferenceTreeNode &byRefNode = OFstatic_cast(const DSRByReferenceTreeNode &, node);
        if (ValidReference && byRefNode.ValidReference)
        {
            /* check referenced node ID only */
            result = (ReferencedNodeID != byRefNode.ReferencedNodeID);
        } else {
            /* check whether either of the references is invalid */
            result = (ValidReference != byRefNode.ValidReference);
        }
    }
    return result;
}


DSRByReferenceTreeNode *DSRByReferenceTreeNode::clone() const
{
    return new DSRByReferenceTreeNode(*this);
}


void DSRByReferenceTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    ValidReference = OFFalse;
    ReferencedContentItem.clear();
    ReferencedNodeID = 0;
    TargetValueType = VT_invalid;
}


OFBool DSRByReferenceTreeNode::isValid() const
{
    /* ConceptNameCodeSequence not allowed */
    return DSRDocumentTreeNode::isValid() && getConceptName().isEmpty() && hasValidValue();
}


OFBool DSRByReferenceTreeNode::hasValidValue() const
{
    return ValidReference;
}


OFCondition DSRByReferenceTreeNode::print(STD_NAMESPACE ostream &stream,
                                          const size_t flags) const
{
    DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_RELATIONSHIP_TYPE)
    stream << relationshipTypeToReadableName(getRelationshipType()) << " ";
    DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_ITEM_VALUE)
    if (ReferencedContentItem.empty())
        stream << "?";
    else
        stream << ReferencedContentItem;
    /* print node ID (might be useful for debugging purposes) */
    if (flags & PF_printNodeID)
    {
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_RESET)
        stream << " = id:" << ReferencedNodeID;
    }
    return EC_Normal;
}


OFCondition DSRByReferenceTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                             const size_t flags) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags, OFFalse /*closingBracket*/);
    stream << " ref=\"" << ReferencedNodeID << "\">" << OFendl;
    /* basically, there should be no child content items but ... */
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    writeXMLItemEnd(stream, flags);
    return result;
}


void DSRByReferenceTreeNode::invalidateReference()
{
    ValidReference = OFFalse;
}


OFBool DSRByReferenceTreeNode::updateReference(const size_t referencedNodeID,
                                               const E_ValueType targetValueType)
{
    ReferencedNodeID = referencedNodeID;
    TargetValueType = targetValueType;
    /* check whether the given reference is valid */
    ValidReference = (ReferencedNodeID > 0);
    return ValidReference;
}


OFBool DSRByReferenceTreeNode::updateReference(const OFString &referencedContentItem)
{

    ReferencedContentItem = referencedContentItem;
    /* tbd: check for valid reference could be more strict */
    ValidReference = checkForValidReference(ReferencedContentItem);
    return ValidReference;
}


// protected methods

OFCondition DSRByReferenceTreeNode::readContentItem(DcmItem &dataset,
                                                    const size_t /*flags*/)
{
    DcmUnsignedLong delem(DCM_ReferencedContentItemIdentifier);
    /* clear before reading */
    ReferencedContentItem.clear();
    updateReference(0, VT_invalid);
    /* read ReferencedContentItemIdentifier */
    OFCondition result = getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", "by-reference relationship");
    if (result.good())
    {
        /* create reference string from unsigned long values */
        Uint32 value = 0;
        char buffer[20];
        const unsigned long count = delem.getVM();
        for (unsigned long i = 0; i < count; i++)
        {
            if (i > 0)
                ReferencedContentItem += '.';
            if (delem.getUint32(value, i).good())
                ReferencedContentItem += numberToString(OFstatic_cast(size_t, value), buffer);
        }
    }
    return result;
}


OFCondition DSRByReferenceTreeNode::writeContentItem(DcmItem &dataset) const
{
    OFCondition result = SR_EC_InvalidValue;
    /* only write references with valid format */
    if (checkForValidReference(ReferencedContentItem))
    {
        result = EC_Normal;
        DcmUnsignedLong delem(DCM_ReferencedContentItemIdentifier);
        /* create unsigned long values from reference string */
        size_t posStart = 0;
        size_t posEnd = 0;
        unsigned long i = 0;
        do {
            /* search for next separator */
            posEnd = ReferencedContentItem.find('.', posStart);
            /* is last segment? */
            if (posEnd == OFString_npos)
                delem.putUint32(OFstatic_cast(Uint32, DSRTypes::stringToNumber(ReferencedContentItem.substr(posStart).c_str())), i);
            else {
                delem.putUint32(OFstatic_cast(Uint32, DSRTypes::stringToNumber(ReferencedContentItem.substr(posStart, posEnd - posStart).c_str())), i);
                posStart = posEnd + 1;
            }
            i++;
        } while (posEnd != OFString_npos);
        /* write ReferencedContentItemIdentifier */
        addElementToDataset(result, dataset, new DcmUnsignedLong(delem), "1-n", "1", "by-reference relationship");
    }
    return result;
}


OFCondition DSRByReferenceTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                       DSRXMLCursor cursor,
                                                       const size_t /*flags*/)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        OFString refID;
        /* get "ref" attribute */
        if (!doc.getStringFromAttribute(cursor, refID, "ref").empty())
        {
            ReferencedNodeID = stringToNumber(refID.c_str());
            /* this does not mean that the reference is really correct, this will be checked later */
            result = EC_Normal;
        } else
            result = SR_EC_InvalidValue;
    }
    return result;
}


OFCondition DSRByReferenceTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                          STD_NAMESPACE ostream & /*annexStream*/,
                                                          const size_t /*nestingLevel*/,
                                                          size_t & /*annexNumber*/,
                                                          const size_t /*flags*/) const
{
    /* render reference string */
    docStream << "Content Item <a href=\"#content_item_" << ReferencedNodeID << "\">by-reference</a>" << OFendl;
    return EC_Normal;
}


OFCondition DSRByReferenceTreeNode::setConceptName(const DSRCodedEntryValue & /*conceptName*/,
                                                   const OFBool /*check*/)
{
    /* invalid: no concept name allowed */
    return EC_IllegalCall;
}


OFCondition DSRByReferenceTreeNode::setObservationDateTime(const OFString & /*observationDateTime*/,
                                                           const OFBool /*check*/)
{
    /* invalid: no observation date/time allowed */
    return EC_IllegalCall;
}


OFCondition DSRByReferenceTreeNode::setObservationDateTime(const DcmElement & /*delem*/,
                                                           const unsigned long /*pos*/,
                                                           const OFBool /*check*/)
{
    /* invalid: no observation date/time allowed */
    return EC_IllegalCall;
}


OFCondition DSRByReferenceTreeNode::setObservationDateTime(DcmItem & /*dataset*/,
                                                           const DcmTagKey & /*tagKey*/,
                                                           const unsigned long /*pos*/,
                                                           const OFBool /*check*/)
{
    /* invalid: no observation date/time allowed */
    return EC_IllegalCall;
}


OFCondition DSRByReferenceTreeNode::setObservationUID(const OFString & /*observationUID*/,
                                                      const OFBool /*check*/)
{
    /* invalid: no observation unique identifier allowed */
    return EC_IllegalCall;
}


OFCondition DSRByReferenceTreeNode::setTemplateIdentification(const OFString & /*templateIdentifier*/,
                                                              const OFString & /*mappingResource*/,
                                                              const OFString & /*mappingResourceUID*/,
                                                              const OFBool /*check*/)
{
    /* invalid: no template identification allowed */
    return EC_IllegalCall;
}
