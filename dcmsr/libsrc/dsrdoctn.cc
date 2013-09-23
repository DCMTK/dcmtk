/*
 *
 *  Copyright (C) 2000-2013, OFFIS e.V.
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
    ObservationUID(),
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
    ObservationUID.clear();
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
    {
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_RELATIONSHIP_TYPE)
        stream << relationshipTypeToReadableName(RelationshipType) << " ";
    }
    DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_VALUE_TYPE)
    stream << valueTypeToDefinedTerm(ValueType);
    DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
    stream << ":";
    /* only print valid concept name codes */
    if (ConceptName.isValid() || (flags & PF_printInvalidCodes))
    {
        DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_CONCEPT_NAME)
        ConceptName.print(stream, (flags & PF_printConceptNameCodes) > 0 /*printCodeValue*/,
                                  (flags & PF_printInvalidCodes) > 0 /*printInvalid*/);
    }
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
        /* read observation UID and date/time (optional) */
        const DSRXMLCursor childCursor = doc.getNamedNode(cursor.getChild(), "observation", OFFalse /*required*/);
        if (childCursor.valid())
        {
            doc.getStringFromAttribute(childCursor, ObservationUID, "uid", OFFalse /*encoding*/, OFFalse /*required*/);
            DSRDateTimeTreeNode::getValueFromXMLNodeContent(doc, doc.getNamedNode(childCursor.getChild(), "datetime", OFFalse /*required*/), ObservationDateTime);
        }
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
    if (!(ObservationDateTime.empty() && ObservationUID.empty()))
    {
        /* observation UID (optional) */
        OFString tmpString;
        stream << "<observation";
        if (!ObservationUID.empty())
            stream << " uid=\"" << ObservationUID << "\"";
        stream << ">" << OFendl;
        /* observation date/time (optional) */
        if (!ObservationDateTime.empty())
        {
            /* output time in ISO 8601 format */
            DcmDateTime::getISOFormattedDateTimeFromString(ObservationDateTime, tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/,
                OFTrue /*timeZone*/, OFFalse /*createMissingPart*/, "T" /*dateTimeSeparator*/, "" /*timeZoneSeparator*/);
            writeStringValueToXML(stream, tmpString, "datetime");
        }
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


OFCondition DSRDocumentTreeNode::setConceptName(const DSRCodedEntryValue &conceptName,
                                                const OFBool check)
{
    OFCondition result = EC_Normal;
    /* check for valid code (if not disabled) */
    if (check && !conceptName.isEmpty())
        result = conceptName.checkCurrentValue();
    if (result.good())
        ConceptName = conceptName;
    return result;
}


OFCondition DSRDocumentTreeNode::setObservationDateTime(const OFString &observationDateTime,
                                                        const OFBool check)
{
    OFCondition result = (check) ? DcmDateTime::checkStringValue(observationDateTime, "1") : EC_Normal;
    if (result.good())
        ObservationDateTime = observationDateTime;
    return result;
}


OFCondition DSRDocumentTreeNode::setObservationUID(const OFString &observationUID,
                                                   const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(observationUID, "1") : EC_Normal;
    if (result.good())
        ObservationUID = observationUID;
    return result;
}


OFCondition DSRDocumentTreeNode::getTemplateIdentification(OFString &templateIdentifier,
                                                           OFString &mappingResource) const
{
    OFCondition result = SR_EC_InvalidValue;
    /* check for valid value pair */
    if (TemplateIdentifier.empty() == MappingResource.empty())
    {
        templateIdentifier = TemplateIdentifier;
        mappingResource = MappingResource;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRDocumentTreeNode::setTemplateIdentification(const OFString &templateIdentifier,
                                                           const OFString &mappingResource,
                                                           const OFBool check)
{
    OFCondition result = EC_Normal;
    /* check for valid value pair */
    if (templateIdentifier.empty() != mappingResource.empty())
        result = EC_IllegalParameter;
    /* check whether the passed value is valid */
    else if (check)
    {
        result = DcmCodeString::checkStringValue(templateIdentifier, "1");
        if (result.good())
            result = DcmCodeString::checkStringValue(mappingResource, "1");
    }
    if (result.good())
    {
        TemplateIdentifier = templateIdentifier;
        MappingResource = mappingResource;
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
    /* read ObservationUID (optional) */
    getAndCheckStringValueFromDataset(dataset, DCM_ObservationUID, ObservationUID, "1", "3");
    /* determine template identifier expected for this document */
    const OFString expectedTemplateIdentifier = (constraintChecker != NULL) ? OFSTRING_GUARD(constraintChecker->getRootTemplateIdentifier()) : "";
    /* read ContentTemplateSequence (conditional) */
    DcmItem *ditem = NULL;
    if (dataset.findAndGetSequenceItem(DCM_ContentTemplateSequence, ditem, 0 /*itemNum*/).good())
    {
        getAndCheckStringValueFromDataset(*ditem, DCM_MappingResource, MappingResource, "1", "1", "ContentTemplateSequence");
        getAndCheckStringValueFromDataset(*ditem, DCM_TemplateIdentifier, TemplateIdentifier, "1", "1", "ContentTemplateSequence");
        /* check for a common error: Template Identifier includes "TID" prefix */
        if ((MappingResource == "DCMR") && !TemplateIdentifier.empty())
        {
            if ((TemplateIdentifier.find_first_not_of("0123456789") != OFString_npos) || (TemplateIdentifier.at(0) == '0'))
                DCMSR_WARN("TemplateIdentifier shall be a string of digits without leading zeros");
        }
        /* check whether the expected template (if known) has been used */
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
    if (!MACParameters.isEmpty())
        addElementToDataset(result, dataset, new DcmSequenceOfItems(MACParameters), "1-n", "3", "SOPCommonModule");
    if (!DigitalSignatures.isEmpty())
    {
        addElementToDataset(result, dataset, new DcmSequenceOfItems(DigitalSignatures), "1-n", "3", "SOPCommonModule");
        DCMSR_WARN("Writing possibly incorrect digital signature - same as read from dataset");
    }
    /* add to mark stack */
    if (MarkFlag && (markedItems != NULL))
        markedItems->push(&dataset);
    /* write ObservationDateTime (conditional) */
    result = putStringValueToDataset(dataset, DCM_ObservationDateTime, ObservationDateTime, OFFalse /*allowEmpty*/);
    /* write ObservationUID (optional) */
    if (result.good())
        result = putStringValueToDataset(dataset, DCM_ObservationUID, ObservationUID, OFFalse /*allowEmpty*/);
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
        if (result.bad())
            DCMSR_DEBUG("Ignoring content item error because of read flag");
        /* read ContentItem (depending on ValueType) */
        result = readContentItem(dataset);
    }
    /* check for validity, after reading */
    if (result.bad() || !isValid())
    {
        printInvalidContentItemMessage("Reading", this, posString.c_str());
        /* ignore content item reading/parsing error if flag is set */
        if (flags & RF_ignoreContentItemErrors)
        {
            DCMSR_DEBUG("Ignoring content item error because of read flag");
            result = EC_Normal;
        }
        /* content item is not valid */
        else if (result.good())
        {
            result = SR_EC_InvalidValue;
        }
        /* accept invalid content item value if flag is set */
        else if ((result == SR_EC_InvalidValue) && (flags & RF_acceptInvalidContentItemValue))
        {
            DCMSR_DEBUG("Ignoring invalid content item value because of read flag");
            result = EC_Normal;
        }
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
                            << DcmObject::PrintHelper(*ditem, DCMTypes::PF_convertToOctalNumbers, 1) << OFString(78, '-'));
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
        /* render optional observation date/time */
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
