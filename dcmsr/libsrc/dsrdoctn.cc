/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *    classes: DSRDocumentTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:41 $
 *  CVS/RCS Revision: $Revision: 1.55 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoctn.h"
#include "dcmtk/dcmsr/dsrdtitn.h"
#include "dcmtk/dcmsr/dsrxmld.h"
#include "dcmtk/dcmsr/dsriodcc.h"

#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstream.h"


DSRDocumentTreeNode::DSRDocumentTreeNode(const E_RelationshipType relationshipType,
                                         const E_ValueType valueType)
  : DSRTreeNode(),
    MarkFlag(OFFalse),
    ReferenceTarget(OFFalse),
    RelationshipType(relationshipType),
    ValueType(valueType),
    ConceptName(),
    ObservationDateTime(),
    TemplateIdentifier(),
    MappingResource(),
    MACParameters(DCM_MACParametersSequence),
    DigitalSignatures(DCM_DigitalSignaturesSequence)
{
}


DSRDocumentTreeNode::~DSRDocumentTreeNode()
{
}


void DSRDocumentTreeNode::clear()
{
    MarkFlag = OFFalse;
    ReferenceTarget = OFFalse;
    ConceptName.clear();
    ObservationDateTime.clear();
    TemplateIdentifier.clear();
    MappingResource.clear();
    MACParameters.clear();
    DigitalSignatures.clear();
}


OFBool DSRDocumentTreeNode::isValid() const
{
    return (RelationshipType != RT_invalid) && (ValueType != VT_invalid);
}


OFBool DSRDocumentTreeNode::isShort(const size_t /*flags*/) const
{
    return OFTrue;
}


OFCondition DSRDocumentTreeNode::print(STD_NAMESPACE ostream &stream,
                                       const size_t flags) const
{
    if (RelationshipType != RT_isRoot)
        stream << relationshipTypeToReadableName(RelationshipType) << " ";
    stream << valueTypeToDefinedTerm(ValueType) << ":";
    /* only print valid concept name codes */
    if (ConceptName.isValid())
        ConceptName.print(stream, (flags & PF_printConceptNameCodes) > 0);
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::read(DcmItem &dataset,
                                      const DSRIODConstraintChecker *constraintChecker,
                                      const size_t flags)
{
    return readSRDocumentContentModule(dataset, constraintChecker, flags);
}


OFCondition DSRDocumentTreeNode::write(DcmItem &dataset,
                                       DcmStack *markedItems)
{
    return writeSRDocumentContentModule(dataset, markedItems);
}


OFCondition DSRDocumentTreeNode::readXML(const DSRXMLDocument &doc,
                                         DSRXMLCursor cursor,
                                         const E_DocumentType documentType,
                                         const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (cursor.valid())
    {
        OFString idAttr;
        OFString templateIdentifier, mappingResource;
        /* important: NULL indicates first child node */
        DSRDocumentTreeNode *node = NULL;
        /* read "id" attribute (optional) and compare with expected value */
        if (!doc.getStringFromAttribute(cursor, idAttr, "id", OFFalse /*encoding*/, OFFalse /*required*/).empty() &&
            (stringToNumber(idAttr.c_str()) != Ident))
        {
            /* create warning message */
            DCMSR_WARN("XML attribute 'id' (" << idAttr << ") deviates from current node number (" << Ident << ")");
        }
        /* template identification information expected "inside" content item */
        if (!(flags & XF_templateElementEnclosesItems))
        {
            /* check for optional template identification */
            const DSRXMLCursor childCursor = doc.getNamedNode(cursor.getChild(), "template", OFFalse /*required*/);
            if (childCursor.valid())
            {
                /* check whether information is stored as XML attributes */
                if (doc.hasAttribute(childCursor, "tid"))
                {
                    doc.getStringFromAttribute(childCursor, mappingResource, "resource");
                    doc.getStringFromAttribute(childCursor, templateIdentifier, "tid");
                } else {
                    doc.getStringFromNodeContent(doc.getNamedNode(childCursor.getChild(), "resource"), mappingResource);
                    doc.getStringFromNodeContent(doc.getNamedNode(childCursor.getChild(), "id"), templateIdentifier);
                }
                if (setTemplateIdentification(templateIdentifier, mappingResource).bad())
                    DCMSR_WARN("Content item has invalid/incomplete template identification");
            }
        }
        /* read concept name (not required in some cases) */
        ConceptName.readXML(doc, doc.getNamedNode(cursor.getChild(), "concept", OFFalse /*required*/));
        /* read observation datetime (optional) */
        const DSRXMLCursor childCursor = doc.getNamedNode(cursor.getChild(), "observation", OFFalse /*required*/);
        if (childCursor.valid())
            DSRDateTimeTreeNode::getValueFromXMLNodeContent(doc, doc.getNamedNode(childCursor.getChild(), "datetime"), ObservationDateTime);
        /* read node content (depends on value type) */
        result = readXMLContentItem(doc, cursor);
        /* goto first child node */
        cursor.gotoChild();
        /* iterate over all child content items */
        while (cursor.valid() && result.good())
        {
            /* template identification information expected "outside" content item */
            if (flags & XF_templateElementEnclosesItems)
            {
                /* check for optional template identification */
                if (doc.matchNode(cursor, "template"))
                {
                    doc.getStringFromAttribute(cursor, mappingResource, "resource");
                    doc.getStringFromAttribute(cursor, templateIdentifier, "tid");
                    /* goto first child of the "template" element */
                    cursor.gotoChild();
                }
            }
            /* get SR value type from current XML node, also supports "by-reference" detection */
            E_ValueType valueType = doc.getValueTypeFromNode(cursor);
            /* invalid types are silently ignored */
            if (valueType != VT_invalid)
            {
                /* get SR relationship type */
                E_RelationshipType relationshipType = doc.getRelationshipTypeFromNode(cursor);
                /* create new node (by-value or by-reference), do not check constraints */
                result = createAndAppendNewNode(node, relationshipType, valueType);
                if (result.good())
                {
                    if ((flags & XF_templateElementEnclosesItems) && (valueType != VT_byReference))
                    {
                        /* set template identification (if any) */
                        if (node->setTemplateIdentification(templateIdentifier, mappingResource).bad())
                            DCMSR_WARN("Content item has invalid/incomplete template identification");
                    }
                    /* proceed with reading child nodes */
                    result = node->readXML(doc, cursor, documentType, flags);
                    /* print node error message (if any) */
                    doc.printGeneralNodeError(cursor, result);
                } else {
                    /* create new node failed */
                    DCMSR_ERROR("Cannot add \"" << relationshipTypeToReadableName(relationshipType) << " "
                        << valueTypeToDefinedTerm(valueType /*target item*/) << "\" to "
                        << valueTypeToDefinedTerm(ValueType /*source item*/) << " in "
                        << documentTypeToReadableName(documentType));
                }
            }
            /* proceed with next node */
            cursor.gotoNext();
        }
    }
    return result;
}


OFCondition DSRDocumentTreeNode::readXMLContentItem(const DSRXMLDocument & /*doc*/,
                                                    DSRXMLCursor /*cursor*/)
{
    return EC_IllegalCall;
}


OFCondition DSRDocumentTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                          const size_t flags) const
{
    OFCondition result = EC_Normal;
    /* check for validity */
    if (!isValid())
        printInvalidContentItemMessage("Writing to XML", this);
    /* write optional template identification */
    if ((flags & XF_writeTemplateIdentification) && !(flags & XF_templateElementEnclosesItems))
    {
        if (!TemplateIdentifier.empty() && !MappingResource.empty())
        {
            if (flags & XF_templateIdentifierAsAttribute)
                stream << "<template resource=\"" << MappingResource << "\" tid=\"" << TemplateIdentifier << "\"/>" << OFendl;
            else
            {
                stream << "<template>" << OFendl;
                writeStringValueToXML(stream, MappingResource, "resource");
                writeStringValueToXML(stream, TemplateIdentifier, "id");
                stream << "</template>" << OFendl;
            }
        }
    }
    /* relationship type */
    if ((RelationshipType != RT_isRoot) && !(flags & XF_relationshipTypeAsAttribute))
        writeStringValueToXML(stream, relationshipTypeToDefinedTerm(RelationshipType), "relationship", (flags & XF_writeEmptyTags) > 0);
    /* concept name */
    if (ConceptName.isValid())
    {
        if (flags & XF_codeComponentsAsAttribute)
            stream << "<concept";     // bracket ">" is closed in the next writeXML() routine
        else
            stream << "<concept>" << OFendl;
        ConceptName.writeXML(stream, flags);
        stream << "</concept>" << OFendl;
    }
    /* observation datetime (optional) */
    if (!ObservationDateTime.empty())
    {
        OFString tmpString;
        stream << "<observation>" << OFendl;
        DcmDateTime::getISOFormattedDateTimeFromString(ObservationDateTime, tmpString, OFTrue /*seconds*/,
            OFFalse /*fraction*/, OFFalse /*timeZone*/, OFFalse /*createMissingPart*/, "T" /*dateTimeSeparator*/);
        writeStringValueToXML(stream, tmpString, "datetime");
        stream << "</observation>" << OFendl;
    }
    /* write child nodes (if any) */
    DSRTreeNodeCursor cursor(Down);
    if (cursor.isValid())
    {
        DSRDocumentTreeNode *node = NULL;
        do {    /* for all child nodes */
            node = OFstatic_cast(DSRDocumentTreeNode *, cursor.getNode());
            if (node != NULL)
                result = node->writeXML(stream, flags);
            else
                result = SR_EC_InvalidDocumentTree;
        } while (result.good() && cursor.gotoNext());
    }
    return result;
}


void DSRDocumentTreeNode::writeXMLItemStart(STD_NAMESPACE ostream &stream,
                                            const size_t flags,
                                            const OFBool closingBracket) const
{
    /* write optional template identification */
    if ((flags & XF_writeTemplateIdentification) && (flags & XF_templateElementEnclosesItems))
    {
        if (!TemplateIdentifier.empty() && !MappingResource.empty())
            stream << "<template resource=\"" << MappingResource << "\" tid=\"" << TemplateIdentifier << "\">" << OFendl;
    }
    /* write content item */
    if (flags & XF_valueTypeAsAttribute)
    {
        stream << "<item";
        if (ValueType != VT_byReference)
            stream << " valType=\"" << valueTypeToDefinedTerm(ValueType) << "\"";
    } else
        stream << "<" << valueTypeToXMLTagName(ValueType);
    if ((RelationshipType != RT_isRoot) && (flags & XF_relationshipTypeAsAttribute))
        stream << " relType=\"" << relationshipTypeToDefinedTerm(RelationshipType) << "\"";
    if (ReferenceTarget || (flags & XF_alwaysWriteItemIdentifier))
        stream << " id=\"" << getNodeID() << "\"";
    if (closingBracket)
        stream << ">" << OFendl;
}


void DSRDocumentTreeNode::writeXMLItemEnd(STD_NAMESPACE ostream &stream,
                                          const size_t flags) const
{
    /* close content item */
    if (flags & XF_valueTypeAsAttribute)
        stream << "</item>" << OFendl;
    else
        stream << "</" << valueTypeToXMLTagName(ValueType) << ">" << OFendl;
    /* close optional template identification */
    if ((flags & XF_writeTemplateIdentification) && (flags & XF_templateElementEnclosesItems))
    {
        if (!TemplateIdentifier.empty() && !MappingResource.empty())
            stream << "</template>" << OFendl;
    }
}


OFCondition DSRDocumentTreeNode::renderHTML(STD_NAMESPACE ostream &docStream,
                                            STD_NAMESPACE ostream &annexStream,
                                            const size_t nestingLevel,
                                            size_t &annexNumber,
                                            const size_t flags) const
{
    /* check for validity */
    if (!isValid())
        printInvalidContentItemMessage("Rendering", this);
    /* declare hyperlink target */
    if (ReferenceTarget)
    {
        const char *attrName = (flags & DSRTypes::HF_XHTML11Compatibility) ? "id" : "name";
        const char *closeElm = (flags & DSRTypes::HF_XHTML11Compatibility) ? " /" : "></a";
        docStream << "<a " << attrName << "=\"content_item_" << getNodeID() << "\"" << closeElm << ">" << OFendl;
    }
    /* render content item */
    OFCondition result = renderHTMLContentItem(docStream, annexStream, nestingLevel, annexNumber, flags);
    /* render child nodes */
    if (result.good())
        result = renderHTMLChildNodes(docStream, annexStream, nestingLevel, annexNumber, flags | HF_renderItemsSeparately);
    else
        printContentItemErrorMessage("Rendering", result, this);
    return result;
}


OFCondition DSRDocumentTreeNode::getConceptName(DSRCodedEntryValue &conceptName) const
{
    conceptName = ConceptName;
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::setConceptName(const DSRCodedEntryValue &conceptName)
{
    OFCondition result = EC_IllegalParameter;
    /* check for valid code */
    if (conceptName.isValid() || conceptName.isEmpty())
    {
        ConceptName = conceptName;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRDocumentTreeNode::setObservationDateTime(const OFString &observationDateTime)
{
    /* might add a check for proper DateTime format */
    ObservationDateTime = observationDateTime;
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::getTemplateIdentification(OFString &templateIdentifier,
                                                           OFString &mappingResource) const
{
    OFCondition result = SR_EC_InvalidValue;
    /* check for valid value pair */
    if ((TemplateIdentifier.empty() && MappingResource.empty()) ||
        (!TemplateIdentifier.empty() && !MappingResource.empty()))
    {
        templateIdentifier = TemplateIdentifier;
        mappingResource = MappingResource;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRDocumentTreeNode::setTemplateIdentification(const OFString &templateIdentifier,
                                                           const OFString &mappingResource)
{
    OFCondition result = EC_IllegalParameter;
    /* check for valid value pair */
    if ((templateIdentifier.empty() && mappingResource.empty()) ||
        (!templateIdentifier.empty() && !mappingResource.empty()))
    {
        TemplateIdentifier = templateIdentifier;
        MappingResource = mappingResource;
        result = EC_Normal;
    }
    return result;
}


void DSRDocumentTreeNode::removeSignatures()
{
    MACParameters.clear();
    DigitalSignatures.clear();
}


OFCondition DSRDocumentTreeNode::readContentItem(DcmItem & /*dataset*/)
{
    /* no content to read */
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::writeContentItem(DcmItem & /*dataset*/) const
{
    /* no content to insert */
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream & /*docStream*/,
                                                       STD_NAMESPACE ostream & /*annexStream*/,
                                                       const size_t /*nestingLevel*/,
                                                       size_t & /*annexNumber*/,
                                                       const size_t /*flags*/) const
{
    /* no content to render */
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::readSRDocumentContentModule(DcmItem &dataset,
                                                             const DSRIODConstraintChecker *constraintChecker,
                                                             const size_t flags)
{
    OFCondition result = EC_Normal;
    /* read DocumentRelationshipMacro */
    result = readDocumentRelationshipMacro(dataset, constraintChecker, "1" /*posString*/, flags);
    /* read DocumentContentMacro */
    if (result.good())
        result = readDocumentContentMacro(dataset, "1" /*posString*/, flags);
    return result;
}


OFCondition DSRDocumentTreeNode::writeSRDocumentContentModule(DcmItem &dataset,
                                                              DcmStack *markedItems)
{
    OFCondition result = EC_Normal;
    /* write DocumentRelationshipMacro */
    result = writeDocumentRelationshipMacro(dataset, markedItems);
    /* write DocumentContentMacro */
    if (result.good())
        result = writeDocumentContentMacro(dataset);
    return result;
}


OFCondition DSRDocumentTreeNode::readDocumentRelationshipMacro(DcmItem &dataset,
                                                               const DSRIODConstraintChecker *constraintChecker,
                                                               const OFString &posString,
                                                               const size_t flags)
{
    OFCondition result = EC_Normal;
    /* read digital signatures sequences (optional) */
    if (flags & RF_readDigitalSignatures)
    {
        getElementFromDataset(dataset, MACParameters);
        getElementFromDataset(dataset, DigitalSignatures);
    }
    /* read ObservationDateTime (conditional) */
    getAndCheckStringValueFromDataset(dataset, DCM_ObservationDateTime, ObservationDateTime, "1", "1C");
    /* determine template identifier expected for this document */
    const OFString expectedTemplateIdentifier = (constraintChecker != NULL) ? OFSTRING_GUARD(constraintChecker->getRootTemplateIdentifier()) : "";
    /* read ContentTemplateSequence (conditional) */
    DcmItem *ditem = NULL;
    if (dataset.findAndGetSequenceItem(DCM_ContentTemplateSequence, ditem, 0 /*itemNum*/).good())
    {
        getAndCheckStringValueFromDataset(*ditem, DCM_MappingResource, MappingResource, "1", "1", "ContentTemplateSequence");
        getAndCheckStringValueFromDataset(*ditem, DCM_TemplateIdentifier, TemplateIdentifier, "1", "1", "ContentTemplateSequence");
        if (!expectedTemplateIdentifier.empty())
        {
            /* check for DICOM Content Mapping Resource */
            if (MappingResource == "DCMR")
            {
                /* compare with expected TID */
                if (TemplateIdentifier != expectedTemplateIdentifier)
                {
                    DCMSR_WARN("Incorrect value for TemplateIdentifier ("
                        << ((TemplateIdentifier.empty()) ? "<empty>" : TemplateIdentifier) << "), "
                        << expectedTemplateIdentifier << " expected");
                }
            } else if (!MappingResource.empty())
                printUnknownValueWarningMessage("MappingResource", MappingResource.c_str());
        }
    }
    /* only check template identifier on dataset level (root node) */
    else if ((dataset.ident() == EVR_dataset) && !expectedTemplateIdentifier.empty())
    {
        DCMSR_WARN("ContentTemplateSequence missing or empty, TemplateIdentifier "
            << expectedTemplateIdentifier
            /* DICOM Content Mapping Resource is currently hard-coded (see above) */
            <<  " (DCMR) expected");
    }
    /* read ContentSequence */
    if (result.good())
        result = readContentSequence(dataset, constraintChecker, posString, flags);
    return result;
}


OFCondition DSRDocumentTreeNode::writeDocumentRelationshipMacro(DcmItem &dataset,
                                                                DcmStack *markedItems)
{
    OFCondition result = EC_Normal;
    /* write digital signatures sequences (optional) */
    if (MACParameters.card() > 0)
        addElementToDataset(result, dataset, new DcmSequenceOfItems(MACParameters), "1-n", "3", "SOPCommonModule");
    if (DigitalSignatures.card() > 0)
    {
        addElementToDataset(result, dataset, new DcmSequenceOfItems(DigitalSignatures), "1-n", "3", "SOPCommonModule");
        DCMSR_WARN("Writing possibly incorrect digital signature - same as read from dataset");
    }
    /* add to mark stack */
    if (MarkFlag && (markedItems != NULL))
        markedItems->push(&dataset);
    /* write ObservationDateTime (conditional) */
    result = putStringValueToDataset(dataset, DCM_ObservationDateTime, ObservationDateTime, OFFalse /*allowEmpty*/);
    /* write ContentTemplateSequence (conditional) */
    if (result.good())
    {
        if (!TemplateIdentifier.empty() && !MappingResource.empty())
        {
            DcmItem *ditem = NULL;
            /* create sequence with a single item */
            result = dataset.findOrCreateSequenceItem(DCM_ContentTemplateSequence, ditem, 0 /*position*/);
            if (result.good())
            {
                /* write item data */
                putStringValueToDataset(*ditem, DCM_TemplateIdentifier, TemplateIdentifier);
                putStringValueToDataset(*ditem, DCM_MappingResource, MappingResource);
            }
        }
    }
    /* write ContentSequence */
    if (result.good())
        result = writeContentSequence(dataset, markedItems);
    return result;
}


OFCondition DSRDocumentTreeNode::readDocumentContentMacro(DcmItem &dataset,
                                                          const OFString &posString,
                                                          const size_t flags)
{
    OFCondition result = EC_Normal;
    /* skip reading ValueType, already done somewhere else */

    /* read ConceptNameCodeSequence */
    if (RelationshipType == RT_isRoot)
    {
        /* the concept name is required for the root container */
        result = ConceptName.readSequence(dataset, DCM_ConceptNameCodeSequence, "1" /*type*/);
    } else {
        /* the concept name might be empty for all other content items */
        ConceptName.readSequence(dataset, DCM_ConceptNameCodeSequence, "1C" /*type*/);
    }
    if (result.good() || (flags & RF_ignoreContentItemErrors))
    {
        /* read ContentItem (depending on ValueType) */
        result = readContentItem(dataset);
    }
    /* check for validity, after reading */
    if (result.bad() || !isValid())
    {
        printInvalidContentItemMessage("Reading", this, posString.c_str());
        /* ignore content item reading/parsing error if flag is set */
        if (flags & RF_ignoreContentItemErrors)
           result = EC_Normal;
        /* content item is not valid */
        else if (result.good())
           result = SR_EC_InvalidValue;
    }
    return result;
}


OFCondition DSRDocumentTreeNode::writeDocumentContentMacro(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    /* write ValueType */
    result = putStringValueToDataset(dataset, DCM_ValueType, valueTypeToDefinedTerm(ValueType));
    /* write ConceptNameCodeSequence */
    if (result.good())
    {
        if (ConceptName.isValid())
            result = ConceptName.writeSequence(dataset, DCM_ConceptNameCodeSequence);
    }
    if (result.good())
    {
        /* check for validity, before writing */
        if (!isValid())
            printInvalidContentItemMessage("Writing", this);
        /* write ContentItem (depending on ValueType) */
        result = writeContentItem(dataset);
    }
    return result;
}


OFCondition DSRDocumentTreeNode::createAndAppendNewNode(DSRDocumentTreeNode *&previousNode,
                                                        const E_RelationshipType relationshipType,
                                                        const E_ValueType valueType,
                                                        const DSRIODConstraintChecker *constraintChecker)
{
    OFCondition result = EC_Normal;
    /* do not check by-reference relationships here, will be done later (after complete reading) */
    if ((relationshipType == RT_unknown) || ((relationshipType != RT_invalid) && ((valueType == VT_byReference) ||
        (constraintChecker == NULL) || constraintChecker->checkContentRelationship(ValueType, relationshipType, valueType))))
    {
        DSRDocumentTreeNode *node = createDocumentTreeNode(relationshipType, valueType);
        if (node != NULL)
        {
            /* first child node */
            if (previousNode == NULL)
                Down = node;
            else
            {
                /* new sibling */
                previousNode->Next = node;
                node->Prev = previousNode;
            }
            /* store new node for the next time */
            previousNode = node;
        } else {
            if (valueType == VT_invalid)
                result = SR_EC_UnknownValueType;
            else
                result = EC_MemoryExhausted;
        }
    } else {
        /* summarize what went wrong */
        if (valueType == VT_invalid)
            result = SR_EC_UnknownValueType;
        else if (relationshipType == RT_invalid)
            result = SR_EC_UnknownRelationshipType;
        else
            result = SR_EC_InvalidByValueRelationship;
    }
    return result;
}


OFCondition DSRDocumentTreeNode::readContentSequence(DcmItem &dataset,
                                                     const DSRIODConstraintChecker *constraintChecker,
                                                     const OFString &posString,
                                                     const size_t flags)
{
    OFCondition result = EC_Normal;
    DcmStack stack;
    /* read ContentSequence (might be absent or empty) */
    if (dataset.search(DCM_ContentSequence, stack, ESM_fromHere, OFFalse).good())
    {
        DcmSequenceOfItems *dseq = OFstatic_cast(DcmSequenceOfItems *, stack.top());
        if (dseq != NULL)
        {
            OFString tmpString;
            E_ValueType valueType = VT_invalid;
            E_RelationshipType relationshipType = RT_invalid;
            /* important: NULL indicates first child node */
            DSRDocumentTreeNode *node = NULL;
            DcmItem *ditem = NULL;
            const unsigned long count = dseq->card();
            /* for all items in the sequence */
            unsigned long i = 0;
            while ((i < count) && result.good())
            {
                ditem = dseq->getItem(i);
                if (ditem != NULL)
                {
                    DSRDocumentTreeNode *newNode = NULL;
                    /* create current location string */
                    char buffer[20];
                    OFString location = posString;
                    if (!location.empty())
                        location += ".";
                    location += numberToString(OFstatic_cast(size_t, i + 1), buffer);
                    if (flags & RF_showCurrentlyProcessedItem)
                        DCMSR_INFO("Processing content item " << location);
                    /* read RelationshipType */
                    result = getAndCheckStringValueFromDataset(*ditem, DCM_RelationshipType, tmpString, "1", "1", "content item");
                    if (result.good() || (flags & RF_acceptUnknownRelationshipType))
                    {
                        relationshipType = definedTermToRelationshipType(tmpString);
                        /* check relationship type */
                        if (relationshipType == RT_invalid)
                        {
                            printUnknownValueWarningMessage("RelationshipType", tmpString.c_str());
                            if (flags & RF_acceptUnknownRelationshipType)
                                relationshipType = RT_unknown;
                        }
                        /* check for by-reference relationship */
                        DcmUnsignedLong referencedContentItemIdentifier(DCM_ReferencedContentItemIdentifier);
                        if (getAndCheckElementFromDataset(*ditem, referencedContentItemIdentifier, "1-n", "1C", "content item").good())
                        {
                            /* create new node (by-reference, no constraint checker required) */
                            result = createAndAppendNewNode(node, relationshipType, VT_byReference);
                            /* read ReferencedContentItemIdentifier (again) */
                            if (result.good())
                            {
                                newNode = node;
                                result = node->readContentItem(*ditem);
                            }
                        } else {
                            /* read ValueType (from DocumentContentMacro) - required to create new node */
                            result = getAndCheckStringValueFromDataset(*ditem, DCM_ValueType, tmpString, "1", "1", "content item");
                            if (result.good())
                            {
                                /* read by-value relationship */
                                valueType = definedTermToValueType(tmpString);
                                /* check value type */
                                if (valueType != VT_invalid)
                                {
                                    /* create new node (by-value) */
                                    result = createAndAppendNewNode(node, relationshipType, valueType, (flags & RF_ignoreRelationshipConstraints) ? NULL : constraintChecker);
                                    /* read RelationshipMacro */
                                    if (result.good())
                                    {
                                        newNode = node;
                                        result = node->readDocumentRelationshipMacro(*ditem, constraintChecker, location, flags);
                                        /* read DocumentContentMacro */
                                        if (result.good())
                                            result = node->readDocumentContentMacro(*ditem, location.c_str(), flags);
                                    } else {
                                        /* create new node failed */

                                        /* determine document type */
                                        const E_DocumentType documentType = (constraintChecker != NULL) ? constraintChecker->getDocumentType() : DT_invalid;
                                        DCMSR_ERROR("Cannot add \"" << relationshipTypeToReadableName(relationshipType) << " "
                                            << valueTypeToDefinedTerm(valueType /*target item*/) << "\" to "
                                            << valueTypeToDefinedTerm(ValueType /*source item*/) << " in "
                                            << documentTypeToReadableName(documentType));
                                    }
                                } else {
                                    /* unknown/unsupported value type */
                                    printUnknownValueWarningMessage("ValueType", tmpString.c_str());
                                    result = SR_EC_UnknownValueType;
                                }
                            }
                        }
                    }
                    /* check for any errors */
                    if (result.bad())
                    {
                        printContentItemErrorMessage("Reading", result, newNode, location.c_str());
                        /* print current data set (item) that caused the error */
                        DCMSR_DEBUG(OFString(31, '-') << " DICOM DATA SET " << OFString(31, '-') << OFendl
                            << DcmObject::PrintHelper(*ditem, 0, 1) << OFString(78, '-'));
                    }
                } else
                    result = SR_EC_InvalidDocumentTree;
                i++;
            }
            /* skipping complete sub-tree if flag is set */
            if (result.bad() && (flags & RF_skipInvalidContentItems))
            {
                printInvalidContentItemMessage("Skipping", node);
                result = EC_Normal;
            }
        }
    }
    return result;
}


OFCondition DSRDocumentTreeNode::writeContentSequence(DcmItem &dataset,
                                                      DcmStack *markedItems) const
{
    OFCondition result = EC_Normal;
    /* goto first child of current node */
    DSRTreeNodeCursor cursor(Down);
    if (cursor.isValid())
    {
        /* write ContentSequence */
        DcmSequenceOfItems *dseq = new DcmSequenceOfItems(DCM_ContentSequence);
        if (dseq != NULL)
        {
            DcmItem *ditem = NULL;
            DSRDocumentTreeNode *node = NULL;
            do {        /* for all child nodes */
                node = OFstatic_cast(DSRDocumentTreeNode *, cursor.getNode());
                if (node != NULL)
                {
                    ditem = new DcmItem();
                    if (ditem != NULL)
                    {
                        /* write RelationshipType */
                        result = putStringValueToDataset(*ditem, DCM_RelationshipType, relationshipTypeToDefinedTerm(node->getRelationshipType()));
                        /* check for by-reference relationship */
                        if (node->getValueType() == VT_byReference)
                        {
                            /* write ReferencedContentItemIdentifier */
                            if (result.good())
                                result = node->writeContentItem(*ditem);
                        } else {    // by-value
                            /* write RelationshipMacro */
                            if (result.good())
                                result = node->writeDocumentRelationshipMacro(*ditem, markedItems);
                            /* write DocumentContentMacro */
                            if (result.good())
                                node->writeDocumentContentMacro(*ditem);
                        }
                        /* check for any errors */
                        if (result.bad())
                            printContentItemErrorMessage("Writing", result, node);
                        /* insert item into sequence */
                        if (result.good())
                            dseq->insert(ditem);
                        else
                            delete ditem;
                    } else
                        result = EC_MemoryExhausted;
                } else
                    result = SR_EC_InvalidDocumentTree;
            } while (result.good() && cursor.gotoNext());
            if (result.good())
                result = dataset.insert(dseq, OFTrue /*replaceOld*/);
            if (result.bad())
                delete dseq;
        } else
            result = EC_MemoryExhausted;
    }
    return result;
}


OFCondition DSRDocumentTreeNode::renderHTMLConceptName(STD_NAMESPACE ostream &docStream,
                                                       const size_t flags) const
{
    if (!(flags & HF_renderItemInline) && (flags & HF_renderItemsSeparately))
    {
        const char *lineBreak = (flags & DSRTypes::HF_renderSectionTitlesInline) ? " " :
                                (flags & DSRTypes::HF_XHTML11Compatibility) ? "<br />" : "<br>";
        /* flag indicating whether line is empty or not */
        OFBool writeLine = OFFalse;
        if (!ConceptName.getCodeMeaning().empty())
        {
            docStream << "<b>";
            /* render ConceptName & Code (if valid) */
            ConceptName.renderHTML(docStream, flags, (flags & HF_renderConceptNameCodes) && ConceptName.isValid() /*fullCode*/);
            docStream << ":</b>";
            writeLine = OFTrue;
        }
        else if (flags & HF_currentlyInsideAnnex)
        {
            docStream << "<b>";
            /* render ValueType only */
            docStream << valueTypeToReadableName(ValueType);
            docStream << ":</b>";
            writeLine = OFTrue;
        }
        /* render optional observation datetime */
        if (!ObservationDateTime.empty())
        {
            if (writeLine)
                docStream << " ";
            OFString tmpString;
            if (flags & HF_XHTML11Compatibility)
                docStream << "<span class=\"observe\">";
            else
                docStream << "<small>";
            docStream << "(observed: " << dicomToReadableDateTime(ObservationDateTime, tmpString) << ")";
            if (flags & HF_XHTML11Compatibility)
                docStream << "</span>";
            else
                docStream << "</small>";
            writeLine = OFTrue;
        }
        if (writeLine)
            docStream << lineBreak << OFendl;
    }
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::renderHTMLChildNodes(STD_NAMESPACE ostream &docStream,
                                                      STD_NAMESPACE ostream &annexStream,
                                                      const size_t nestingLevel,
                                                      size_t &annexNumber,
                                                      const size_t flags) const
{
    OFCondition result = EC_Normal;
    /* goto first child of current node */
    DSRTreeNodeCursor cursor(Down);
    if (cursor.isValid())
    {
        /* flag used to format the relationship reference texts */
        OFBool paragraphFlag = (flags & HF_createFootnoteReferences) > 0;
        /* local version of flags */
        size_t newFlags = flags;
        /* footnote counter */
        size_t footnoteNumber = 1;
        /* create memory output stream for the temporal document */
        OFOStringStream tempDocStream;
        DSRDocumentTreeNode *node = NULL;
        do {        /* for all child nodes */
            node = OFstatic_cast(DSRDocumentTreeNode *, cursor.getNode());
            if (node != NULL)
            {
                /* set/reset flag for footnote creation*/
                newFlags &= ~HF_createFootnoteReferences;
                if (!(flags & HF_renderItemsSeparately) && node->hasChildNodes() && (node->getValueType() != VT_Container))
                    newFlags |= HF_createFootnoteReferences;
                /* render (optional) reference to annex */
                OFString relationshipText;
                if (!getRelationshipText(node->getRelationshipType(), relationshipText, flags).empty())
                {
                    if (paragraphFlag)
                    {
                        /* inside paragraph: line break */
                        if (flags & HF_XHTML11Compatibility)
                            docStream << "<br />" << OFendl;
                        else
                            docStream << "<br>" << OFendl;
                    } else {
                        /* open paragraph */
                        if (flags & HF_XHTML11Compatibility)
                        {
                            docStream << "<div class=\"small\">" << OFendl;
                            docStream << "<p>" << OFendl;
                        } else {
                            docStream << "<p>" << OFendl;
                            docStream << "<small>" << OFendl;
                        }
                        paragraphFlag = OFTrue;
                    }
                    if (newFlags & HF_XHTML11Compatibility)
                        docStream << "<span class=\"relation\">" << relationshipText << "</span>: ";
                    else if (flags & DSRTypes::HF_HTML32Compatibility)
                        docStream << "<u>" << relationshipText << "</u>: ";
                    else /* HTML 4.01 */
                        docStream << "<span class=\"under\">" << relationshipText << "</span>: ";
                    /* expand short nodes with no children inline (or depending on 'flags' all nodes) */
                    if ((flags & HF_alwaysExpandChildrenInline) ||
                        (!(flags & HF_neverExpandChildrenInline) && !node->hasChildNodes() && node->isShort(flags)))
                    {
                        if (node->getValueType() != VT_byReference)
                        {
                            /* render concept name/code or value type */
                            if (node->getConceptName().getCodeMeaning().empty())
                                docStream << valueTypeToReadableName(node->getValueType());
                            else
                                node->getConceptName().renderHTML(docStream, flags, (flags & HF_renderConceptNameCodes) && ConceptName.isValid() /*fullCode*/);
                            docStream << " = ";
                        }
                        /* render HTML code (directly to the reference text) */
                        result = node->renderHTML(docStream, annexStream, 0 /*nesting level*/, annexNumber, newFlags | HF_renderItemInline);
                    } else {
                        /* render concept name or value type */
                        if (node->getConceptName().getCodeMeaning().empty())
                            docStream << valueTypeToReadableName(node->getValueType()) << " ";
                        else
                            docStream << node->getConceptName().getCodeMeaning() << " ";
                        /* render annex heading and reference */
                        createHTMLAnnexEntry(docStream, annexStream, "" /*referenceText*/, annexNumber, newFlags);
                        if (flags & HF_XHTML11Compatibility)
                            annexStream << "<div class=\"para\">" << OFendl;
                        else
                            annexStream << "<div>" << OFendl;
                        /* create memory output stream for the temporal annex */
                        OFOStringStream tempAnnexStream;
                        /* render HTML code (directly to the annex) */
                        result = node->renderHTML(annexStream, tempAnnexStream, 0 /*nesting level*/, annexNumber, newFlags | HF_currentlyInsideAnnex);
                        annexStream << "</div>" << OFendl;
                        /* use empty paragraph for bottom margin */
                        if (!(flags & HF_XHTML11Compatibility))
                            annexStream << "<p>" << OFendl;
                        /* append temporary stream to main stream */
                        if (result.good())
                            result = appendStream(annexStream, tempAnnexStream);
                    }
                } else {
                    /* close paragraph */
                    if (paragraphFlag)
                    {
                        if (flags & HF_XHTML11Compatibility)
                        {
                            docStream << "</p>" << OFendl;
                            docStream << "</div>" << OFendl;
                        } else {
                            docStream << "</small>" << OFendl;
                            docStream << "</p>" << OFendl;
                        }
                        paragraphFlag = OFFalse;
                    }
                    /* begin new paragraph */
                    if (flags & HF_renderItemsSeparately)
                    {
                        if (flags & HF_XHTML11Compatibility)
                            docStream << "<div class=\"para\">" << OFendl;
                        else
                            docStream << "<div>" << OFendl;
                    }
                    /* write footnote text to temporary stream */
                    if (newFlags & HF_createFootnoteReferences)
                    {
                        /* render HTML code (without child nodes) */
                        result = node->renderHTMLContentItem(docStream, annexStream, 0 /*nestingLevel*/, annexNumber, newFlags);
                        /* create footnote numbers (individually for each child?) */
                        if (result.good())
                        {
                            /* tags are closed automatically in 'node->renderHTMLChildNodes()' */
                            if (flags & HF_XHTML11Compatibility)
                            {
                                tempDocStream << "<div class=\"small\">" << OFendl;
                                tempDocStream << "<p>" << OFendl;
                            } else {
                                tempDocStream << "<p>" << OFendl;
                                tempDocStream << "<small>" << OFendl;
                            }
                            /* render footnote text and reference */
                            createHTMLFootnote(docStream, tempDocStream, footnoteNumber, node->getNodeID(), flags);
                            /* render child nodes to temporary stream */
                            result = node->renderHTMLChildNodes(tempDocStream, annexStream, 0 /*nestingLevel*/, annexNumber, newFlags);
                        }
                    } else {
                        /* render HTML code (incl. child nodes)*/
                        result = node->renderHTML(docStream, annexStream, nestingLevel + 1, annexNumber, newFlags);
                    }
                    /* end paragraph */
                    if (flags & HF_renderItemsSeparately)
                    {
                        docStream << "</div>" << OFendl;
                        /* use empty paragraph for bottom margin */
                        if (!(flags & HF_XHTML11Compatibility))
                            docStream << "<p>" << OFendl;
                    }
                }
            } else
                result = SR_EC_InvalidDocumentTree;
        } while (result.good() && cursor.gotoNext());
        /* close last open paragraph (if any) */
        if (paragraphFlag)
        {
            if (flags & HF_XHTML11Compatibility)
            {
                docStream << "</p>" << OFendl;
                docStream << "</div>" << OFendl;
            } else {
                docStream << "</small>" << OFendl;
                docStream << "</p>" << OFendl;
            }
        }
        /* append temporary stream to main stream */
        if (result.good())
            result = appendStream(docStream, tempDocStream);
    }
    return result;
}


const OFString &DSRDocumentTreeNode::getRelationshipText(const E_RelationshipType relationshipType,
                                                         OFString &relationshipText,
                                                         const size_t flags)
{
    switch (relationshipType)
    {
        case RT_contains:
            if (flags & HF_createFootnoteReferences)
                relationshipText = "Contains";
            else
                relationshipText.clear();
            break;
        case RT_hasObsContext:
            relationshipText = "Observation Context";
            break;
        case RT_hasAcqContext:
            relationshipText = "Acquisition Context";
            break;
        case RT_hasConceptMod:
            relationshipText = "Concept Modifier";
            break;
        case RT_hasProperties:
            relationshipText = "Properties";
            break;
        case RT_inferredFrom:
            relationshipText = "Inferred from";
            break;
        case RT_selectedFrom:
            relationshipText = "Selected from";
            break;
        default:
            relationshipText.clear();
            break;
    }
    return relationshipText;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrdoctn.cc,v $
 *  Revision 1.55  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.54  2010-09-29 15:16:50  joergr
 *  Enhanced checking and reporting of standard violations in write() methods.
 *
 *  Revision 1.53  2010-09-29 08:32:26  joergr
 *  Used more specific "moduleName" for getAndCheckElementFromDataset() and
 *  checkElementValue().
 *
 *  Revision 1.52  2010-08-09 15:29:12  joergr
 *  Made sure that no NULL pointer is passed to the OFString constructor.
 *
 *  Revision 1.51  2009-10-30 10:09:50  joergr
 *  Added check on the presence of the Concept Name Code Sequence for the root
 *  CONTAINER.
 *
 *  Revision 1.50  2009-10-14 10:49:32  joergr
 *  Fixed minor issues in log output. Also updated copyright date (if required).
 *
 *  Revision 1.49  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.48  2009-04-21 08:31:29  joergr
 *  Fixed typo.
 *
 *  Revision 1.47  2008-07-17 12:00:09  joergr
 *  Replaced call to getSequenceFromDataset() by getElementFromDataset().
 *
 *  Revision 1.46  2008-05-19 09:53:02  joergr
 *  Fixed issue with wrong reference to a content item in an error message.
 *  Added new flag that enables reading of SR documents with unknown/missing
 *  relationship type(s).
 *
 *  Revision 1.45  2007/11/15 16:45:26  joergr
 *  Added support for output in XHTML 1.1 format.
 *  Enhanced support for output in valid HTML 3.2 format. Migrated support for
 *  standard HTML from version 4.0 to 4.01 (strict).
 *
 *  Revision 1.44  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.43  2006/07/25 13:36:17  joergr
 *  Added new optional flags for the HTML rendering of SR documents:
 *  HF_alwaysExpandChildrenInline, HF_useCodeDetailsTooltip and
 *  HF_renderSectionTitlesInline.
 *
 *  Revision 1.42  2006/05/11 09:16:49  joergr
 *  Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 *  Revision 1.41  2005/12/08 15:47:49  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.40  2004/11/22 16:39:12  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *    in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.39  2004/09/09 14:03:19  joergr
 *  Added flags to control the way the template identification is encoded in
 *  writeXML() and expected in readXML().
 *
 *  Revision 1.38  2004/01/20 15:37:39  joergr
 *  Added new command line option which allows to write the item identifier "id"
 *  (XML attribute) even if it is not required (because the item is not referenced
 *  by any other item). Useful for debugging purposes.
 *
 *  Revision 1.37  2004/01/16 10:17:04  joergr
 *  Adapted XML output format of Date, Time and Datetime to XML Schema (ISO
 *  8601) requirements.
 *
 *  Revision 1.36  2004/01/05 14:37:23  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.35  2003/12/05 14:02:36  joergr
 *  Only report warning on incorrect template identifier when actually expecting
 *  one (i.e. only for particular SOP classes).
 *
 *  Revision 1.34  2003/12/01 15:47:28  joergr
 *  Changed XML encoding of by-reference relationships if flag
 *  XF_valueTypeAsAttribute is set.
 *
 *  Revision 1.33  2003/10/30 17:58:58  joergr
 *  Added full support for the ContentTemplateSequence (read/write, get/set
 *  template identification). Template constraints are not checked yet.
 *  Fixed bug in setConceptName() that caused to return EC_IllegalParameter
 *  even for valid parameters.
 *
 *  Revision 1.32  2003/10/09 17:48:16  joergr
 *  Slightly modified warning message text in readDocumentRelationshipMacro().
 *
 *  Revision 1.31  2003/10/09 14:17:59  joergr
 *  Changed message type for incorrect/missing TemplateIdentifier from error to
 *  warning.
 *
 *  Revision 1.30  2003/10/09 13:00:41  joergr
 *  Added check for root template identifier when reading an SR document.
 *
 *  Revision 1.29  2003/10/06 09:55:35  joergr
 *  Added new flag which allows to ignore content item errors when reading an SR
 *  document (e.g. missing value type specific attributes).
 *
 *  Revision 1.28  2003/09/15 14:13:42  joergr
 *  Introduced new class to facilitate checking of SR IOD relationship content
 *  constraints. Replaced old implementation distributed over numerous classes.
 *
 *  Revision 1.27  2003/09/10 13:18:43  joergr
 *  Replaced PrivateCodingSchemeUID by new CodingSchemeIdenticationSequence as
 *  required by CP 324.
 *
 *  Revision 1.26  2003/08/07 17:29:13  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.25  2003/08/07 15:21:53  joergr
 *  Added brackets around "bitwise and" operator/operands to avoid warnings
 *  reported by MSVC5.
 *
 *  Revision 1.24  2003/08/07 13:29:43  joergr
 *  Added readXML functionality.
 *  Adapted type casts to new-style typecast operators defined in ofcast.h.
 *  Distinguish more strictly between OFBool and int (required when HAVE_CXX_BOOL
 *  is defined).
 *
 *  Revision 1.23  2002/10/23 13:21:03  joergr
 *  Fixed bug in debug output of read() routines.
 *
 *  Revision 1.22  2002/08/02 15:06:02  joergr
 *  Fixed problems reported by Sun CC 2.0.1.
 *
 *  Revision 1.21  2002/08/02 12:39:06  joergr
 *  Enhanced debug output of dcmsr::read() routines (e.g. add position string
 *  of invalid content items to error messages).
 *
 *  Revision 1.20  2002/05/02 14:08:35  joergr
 *  Added support for standard and non-standard string streams (which one is
 *  supported is detected automatically via the configure mechanism).
 *
 *  Revision 1.19  2001/11/09 16:15:19  joergr
 *  Added new command line option allowing to encode codes as XML attributes
 *  (instead of tags).
 *
 *  Revision 1.18  2001/10/10 15:29:53  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.17  2001/10/02 12:07:08  joergr
 *  Adapted module "dcmsr" to the new class OFCondition. Introduced module
 *  specific error codes.
 *
 *  Revision 1.16  2001/09/28 14:10:29  joergr
 *  Check return value of DcmItem::insert() statements to avoid memory leaks
 *  when insert procedure failes.
 *
 *  Revision 1.15  2001/09/26 13:04:20  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.14  2001/06/20 15:04:25  joergr
 *  Added minimal support for new SOP class Key Object Selection Document
 *  (suppl. 59).
 *  Added new debugging features (additional flags) to examine "corrupted" SR
 *  documents.
 *
 *  Revision 1.13  2001/04/03 08:25:19  joergr
 *  Added new command line option: ignore relationship content constraints
 *  specified for each SR document class.
 *
 *  Revision 1.12  2001/02/02 14:41:53  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.11  2001/01/18 15:55:20  joergr
 *  Added support for digital signatures.
 *
 *  Revision 1.10  2000/11/13 10:27:00  joergr
 *  Added output of optional observation datetime to rendered HTML page.
 *
 *  Revision 1.9  2000/11/09 20:34:00  joergr
 *  Added support for non-ASCII characters in HTML 3.2 (use numeric value).
 *
 *  Revision 1.8  2000/11/09 11:32:45  joergr
 *  Minor HTML code purifications.
 *
 *  Revision 1.7  2000/11/07 18:33:29  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.6  2000/11/01 16:33:38  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.5  2000/10/26 14:29:20  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.4  2000/10/23 15:03:36  joergr
 *  Allow to set empty concept name code (= clear).
 *
 *  Revision 1.3  2000/10/18 17:16:08  joergr
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.2  2000/10/16 12:03:29  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:19  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
