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
 *    classes: DSRDocumentTreeNode
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-09-26 13:04:20 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrdoctn.h"

#include "ofstring.h"


DSRDocumentTreeNode::DSRDocumentTreeNode(const E_RelationshipType relationshipType,
                                         const E_ValueType valueType)
  : DSRTreeNode(),
    MarkFlag(OFFalse),
    ReferenceTarget(OFFalse),
    RelationshipType(relationshipType),
    ValueType(valueType),
    ConceptName(),
    ObservationDateTime(),
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
    MACParameters.clear();
    DigitalSignatures.clear();
}


OFBool DSRDocumentTreeNode::isValid() const
{
    return (RelationshipType != RT_invalid) && (ValueType != VT_invalid);
}


OFBool DSRDocumentTreeNode::isShort(const size_t /* flags */) const
{
    return OFTrue;
}


OFCondition DSRDocumentTreeNode::print(ostream &stream,
                                       const size_t flags) const
{
    if (RelationshipType != RT_isRoot)
        stream << relationshipTypeToReadableName(RelationshipType) << " ";
    stream << valueTypeToDefinedTerm(ValueType);
#ifdef DEBUG
    stream << " #" << Ident;
#endif
    stream << ":";
    if (ConceptName.isValid())
        ConceptName.print(stream, flags & PF_printConceptNameCodes);
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::read(DcmItem &dataset,
                                      const E_DocumentType documentType,
                                      const size_t flags,
                                      OFConsole *logStream)
{
    return readSRDocumentContentModule(dataset, documentType, flags, logStream);
}


OFCondition DSRDocumentTreeNode::write(DcmItem &dataset,
                                       DcmStack *markedItems,
                                       OFConsole *logStream)
{
    return writeSRDocumentContentModule(dataset, markedItems, logStream);
}


OFCondition DSRDocumentTreeNode::writeXML(ostream &stream,
                                          const size_t flags,
                                          OFConsole *logStream) const
{
    OFCondition result = EC_Normal;
    /* check for validity */
    if (!isValid())
        printInvalidContentItemMessage(logStream, "Writing to XML", this);
    /* relationship type */
    if ((RelationshipType != RT_isRoot) && !(flags & XF_relationshipTypeAsAttribute))
        writeStringValueToXML(stream, relationshipTypeToDefinedTerm(RelationshipType), "relationship", flags & XF_writeEmptyTags);
    /* concept name */
    if (ConceptName.isValid())
    {
        stream << "<concept>" << endl;
        ConceptName.writeXML(stream, flags, logStream);
        stream << "</concept>" << endl;
    }
    /* observation datetime (optional) */
    if (ObservationDateTime.length() > 0)
    {
        stream << "<observation>" << endl;
        writeStringValueToXML(stream, ObservationDateTime, "datetime");
        stream << "</observation>" << endl;
    }
    /* write child nodes (if any) */
    DSRTreeNodeCursor cursor(Down);
    if (cursor.isValid())
    {
        DSRDocumentTreeNode *node = NULL;
        do {        /* for all child nodes */
            node = (DSRDocumentTreeNode *)cursor.getNode();
            if (node != NULL)
                result = node->writeXML(stream, flags, logStream);
            else
                result = EC_IllegalCall;
        } while ((result == EC_Normal) && (cursor.gotoNext()));
    }
    return result;
}


void DSRDocumentTreeNode::writeXMLItemStart(ostream &stream,
                                            const size_t flags,
                                            const OFBool closingBracket) const
{
    if (flags & XF_valueTypeAsAttribute)
        stream << "<item valType=\"" << valueTypeToDefinedTerm(ValueType) << "\"";
    else
        stream << "<" << valueTypeToXMLTagName(ValueType);
    if ((RelationshipType != RT_isRoot) && (flags & XF_relationshipTypeAsAttribute))
        stream << " relType=\"" << relationshipTypeToDefinedTerm(RelationshipType) << "\"";
    if (ReferenceTarget)
        stream << " id=\"" << getNodeID() << "\"";
    if (closingBracket)
        stream << ">" << endl;
}


void DSRDocumentTreeNode::writeXMLItemEnd(ostream &stream,
                                          const size_t flags) const
{
    if (flags & XF_valueTypeAsAttribute)
        stream << "</item>" << endl;
    else
        stream << "</" << valueTypeToXMLTagName(ValueType) <<  ">" << endl;
}


OFCondition DSRDocumentTreeNode::renderHTML(ostream &docStream,
                                            ostream &annexStream,
                                            const size_t nestingLevel,
                                            size_t &annexNumber,
                                            const size_t flags,
                                            OFConsole *logStream) const
{
    /* check for validity */
    if (!isValid())
        printInvalidContentItemMessage(logStream, "Rendering", this);
    /* declare hyperlink target */
    if (ReferenceTarget)
        docStream << "<a name=\"content_item_" << getNodeID() << "\">" << endl;
    /* render content item */
    OFCondition result = renderHTMLContentItem(docStream, annexStream, nestingLevel, annexNumber, flags, logStream);
    if (ReferenceTarget)
        docStream << "</a>" << endl;
    /* render child nodes */
    if (result == EC_Normal)
        result = renderHTMLChildNodes(docStream, annexStream, nestingLevel, annexNumber, flags | HF_renderItemsSeparately, logStream);
    else
        printContentItemErrorMessage(logStream, "Rendering", result, this);
    return result;
}


OFCondition DSRDocumentTreeNode::getConceptName(DSRCodedEntryValue &conceptName) const
{
    conceptName = ConceptName;
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::setConceptName(const DSRCodedEntryValue &conceptName)
{
    OFCondition result = EC_IllegalCall;
    /* check for valid code */
    if (conceptName.isValid() || conceptName.isEmpty())
        ConceptName = conceptName;
    return result;
}


OFCondition DSRDocumentTreeNode::setObservationDateTime(const OFString &observationDateTime)
{
    /* might add a check for proper DateTime format */
    ObservationDateTime = observationDateTime;
    return EC_Normal;
}


void DSRDocumentTreeNode::removeSignatures()
{
    MACParameters.clear();
    DigitalSignatures.clear();
}


OFBool DSRDocumentTreeNode::canAddNode(const E_DocumentType /* documentType */,
                                       const E_RelationshipType /* relationshipType */,
                                       const E_ValueType /* valueType */,
                                       const OFBool /* byReference */) const
{
    /* no restrictions */
    return OFTrue;
}


OFCondition DSRDocumentTreeNode::readContentItem(DcmItem & /* dataset */,
                                                 OFConsole * /* logStream */)
{
    /* no content to read */
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::writeContentItem(DcmItem & /* dataset */,
                                                  OFConsole * /* logStream */) const
{
    /* no content to insert */
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::renderHTMLContentItem(ostream & /* docStream */,
                                                       ostream & /* annexStream */,
                                                       const size_t /* nestingLevel */,
                                                       size_t & /* annexNumber */,
                                                       const size_t /* flags */,
                                                       OFConsole * /* logStream */) const
{
    /* no content to render */
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::readSRDocumentContentModule(DcmItem &dataset,
                                                             const E_DocumentType documentType,
                                                             const size_t flags,
                                                             OFConsole *logStream)
{
    OFCondition result = EC_Normal;
    /* read DocumentRelationshipMacro */
    result = readDocumentRelationshipMacro(dataset, documentType, flags, logStream);
    /* read DocumentContentMacro */
    if (result == EC_Normal)
        result= readDocumentContentMacro(dataset, logStream);
    return result;
}


OFCondition DSRDocumentTreeNode::writeSRDocumentContentModule(DcmItem &dataset,
                                                              DcmStack *markedItems,
                                                              OFConsole *logStream)
{
    OFCondition result = EC_Normal;
    /* write DocumentRelationshipMacro */
    result = writeDocumentRelationshipMacro(dataset, markedItems, logStream);
    /* write DocumentContentMacro */
    if (result == EC_Normal)
        result= writeDocumentContentMacro(dataset, logStream);
    return result;
}


OFCondition DSRDocumentTreeNode::readDocumentRelationshipMacro(DcmItem &dataset,
                                                               const E_DocumentType documentType,
                                                               const size_t flags,
                                                               OFConsole *logStream)
{
    OFCondition result = EC_Normal;
    /* read digital signatures sequences (optional) */
    if (flags & RF_readDigitalSignatures)
    {
        getSequenceFromDataset(dataset, MACParameters);
        getSequenceFromDataset(dataset, DigitalSignatures);
    }
    /* read ObservationDateTime (conditional) */
    getAndCheckStringValueFromDataset(dataset, DCM_ObservationDateTime, ObservationDateTime, "1", "1C", logStream);
    /* tbd: read ContentTemplateSequence */

    /* read ContentSequence */
    if (result == EC_Normal)
        result = readContentSequence(dataset, documentType, flags, logStream);
    return result;
}


OFCondition DSRDocumentTreeNode::writeDocumentRelationshipMacro(DcmItem &dataset,
                                                                DcmStack *markedItems,
                                                                OFConsole *logStream)
{
    OFCondition result = EC_Normal;
    /* write digital signatures sequences (optional) */
    if (MACParameters.card() > 0)
        addElementToDataset(result, dataset, new DcmSequenceOfItems(MACParameters));
    if (DigitalSignatures.card() > 0)
    {
        addElementToDataset(result, dataset, new DcmSequenceOfItems(DigitalSignatures));
        printWarningMessage(logStream, "Writing possibly incorrect digital signature - same as read from dataset.");
    }
    /* add to mark stack */
    if (MarkFlag && (markedItems != NULL))
        markedItems->push(&dataset);
    /* write ObservationDateTime (conditional) */
    if (ObservationDateTime.length() > 0)
        result = putStringValueToDataset(dataset, DCM_ObservationDateTime, ObservationDateTime);
    /* tbd: write ContentTemplateSequence */

    /* write ContentSequence */
    if (result == EC_Normal)
        result = writeContentSequence(dataset, markedItems, logStream);
    return result;
}


OFCondition DSRDocumentTreeNode::readDocumentContentMacro(DcmItem &dataset,
                                                          OFConsole *logStream)
{
    OFCondition result = EC_Normal;
    /* skip: read ValueType, already done somewhere else */

    /* read ConceptNameCodeSequence (might be empty) */
    if (result == EC_Normal)
        ConceptName.readSequence(dataset, DCM_ConceptNameCodeSequence, "1C" /* type */, logStream);
    if (result == EC_Normal)
    {
        /* read ContentItem (depending on ValueType) */
        result = readContentItem(dataset, logStream);
        /* check for validity, after reading */
        if (!isValid())
            printInvalidContentItemMessage(logStream, "Reading", this);
    }
    return result;
}


OFCondition DSRDocumentTreeNode::writeDocumentContentMacro(DcmItem &dataset,
                                                           OFConsole *logStream) const
{
    OFCondition result = EC_Normal;
    /* write ValueType */
    result = putStringValueToDataset(dataset, DCM_ValueType, valueTypeToDefinedTerm(ValueType));
    /* write ConceptNameCodeSequence */
    if (result == EC_Normal)
    {
        if (ConceptName.isValid())
            result = ConceptName.writeSequence(dataset, DCM_ConceptNameCodeSequence, logStream);
    }
    if (result == EC_Normal)
    {
        /* check for validity, before writing */
        if (!isValid())
            printInvalidContentItemMessage(logStream, "Writing", this);
        /* write ContentItem (depending on ValueType) */
        result = writeContentItem(dataset, logStream);
    }
    return result;
}


OFCondition DSRDocumentTreeNode::createAndAppendNewNode(DSRDocumentTreeNode *&previousNode,
                                                        const E_DocumentType documentType,
                                                        const E_RelationshipType relationshipType,
                                                        const E_ValueType valueType,
                                                        const OFBool checkConstraints)
{
    OFCondition result = EC_CorruptedData;
    /* do not check by-reference relationships here, will be done later (after complete reading) */
    if ((valueType == VT_byReference) || !checkConstraints || !isConstraintCheckingSupported(documentType) ||
        canAddNode(documentType, relationshipType, valueType))
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
            result = EC_Normal;
        }
    }
    return result;
}


OFCondition DSRDocumentTreeNode::readContentSequence(DcmItem &dataset,
                                                     const E_DocumentType documentType,
                                                     const size_t flags,
                                                     OFConsole *logStream)
{
    OFCondition result = EC_Normal;
    DcmStack stack;
    /* read ContentSequence (might be absent or empty */
    if (dataset.search(DCM_ContentSequence, stack, ESM_fromHere, OFFalse) == EC_Normal)
    {
        DcmSequenceOfItems *dseq = (DcmSequenceOfItems *)stack.top();
        if (dseq != NULL)
        {
            OFString string;
            E_ValueType valueType = VT_invalid;
            E_RelationshipType relationshipType = RT_invalid;
            DSRDocumentTreeNode *node = NULL;  /* important: NULL indicates first child node */
            DcmItem *ditem = NULL;
            const unsigned long count = dseq->card();
            /* for all items in the sequence */
            unsigned long i = 0;
            while ((i < count) && (result == EC_Normal))
            {
                ditem = dseq->getItem(i);
                if (ditem != NULL)
                {
                    /* read RelationshipType */
                    result = getAndCheckStringValueFromDataset(*ditem, DCM_RelationshipType, string, "1", "1", logStream);
                    if (result == EC_Normal)
                    {
                        relationshipType = definedTermToRelationshipType(string);
                        /* check relationship type */
                        if (relationshipType == RT_unknown)
                        {
                            OFString message = "Reading unknown RelationshipType ";
                            message += string;
                            printWarningMessage(logStream, message.c_str());
                        }
                        /* check for by-reference relationship */
                        DcmUnsignedLong referencedContentItemIdentifier(DCM_ReferencedContentItemIdentifier);
                        if (getAndCheckElementFromDataset(*ditem, referencedContentItemIdentifier, "1-n", "1C", logStream) == EC_Normal)
                        {
                            /* create new node (by-reference) */
                            result = createAndAppendNewNode(node, documentType, relationshipType, VT_byReference);
                            /* read ReferencedContentItemIdentifier (again) */
                            if (result == EC_Normal)
                                result = node->readContentItem(*ditem, logStream);
                        } else {
                            /* read ValueType (from DocumentContentMacro) - required to create new node */
                            result = getAndCheckStringValueFromDataset(*ditem, DCM_ValueType, string, "1", "1", logStream);
                            if (result == EC_Normal)
                            {
                                /* read by-value relationship */
                                valueType = definedTermToValueType(string);
                                /* check value type */
                                if (valueType == VT_unknown)
                                {
                                    OFString message = "Reading unknown ValueType ";
                                    message += string;
                                    printWarningMessage(logStream, message.c_str());
                                }
                                /* create new node (by-value) */
                                result = createAndAppendNewNode(node, documentType, relationshipType, valueType, !(flags & RF_ignoreRelationshipConstraints));
                                /* read RelationshipMacro */
                                if (result == EC_Normal)
                                    result = node->readDocumentRelationshipMacro(*ditem, documentType, flags, logStream);
                                else
                                {
                                    /* create new node failed */
                                    OFString message = "Cannot add \"";
                                    message += relationshipTypeToReadableName(relationshipType);
                                    message += " ";
                                    message += valueTypeToDefinedTerm(valueType /* target item */);
                                    message += "\" to ";
                                    message += valueTypeToDefinedTerm(ValueType /* source item */);
                                    message += " in ";
                                    message += documentTypeToReadableName(documentType);
                                    printErrorMessage(logStream, message.c_str());
                                }
                                /* read DocumentContentMacro (might be empty) */
                                if (result == EC_Normal)
                                    node->readDocumentContentMacro(*ditem, logStream);
                            }
                        }
                    }
                    /* check for any errors */
                    if ((result != EC_Normal) /*&& (node != NULL)*/)
                    {
                        printContentItemErrorMessage(logStream, "Reading", result, node);
                        /* print current data set (item) that caused the error */
                        if ((logStream != NULL) && (flags & RF_verboseDebugMode))
                        {
                            logStream->lockCerr() << OFString(31, '-') << " DICOM DATA SET " << OFString(31, '-') << endl;
                            ditem->print(logStream->getCerr(), OFFalse /* showFullData */);
                            logStream->getCerr() << OFString(78, '-') << endl;
                            logStream->unlockCerr();
                        }
                    }
                } else
                    result = EC_CorruptedData;
                i++;
            }
            /* skipping complete sub-tree if flag is set */
            if ((result != EC_Normal) && (flags & RF_skipInvalidContentItems))
            {
                printInvalidContentItemMessage(logStream, "Skipping", node);
                result = EC_Normal;
            }
        }
    }
    return result;
}


OFCondition DSRDocumentTreeNode::writeContentSequence(DcmItem &dataset,
                                                      DcmStack *markedItems,
                                                      OFConsole *logStream) const
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
                node = (DSRDocumentTreeNode *)cursor.getNode();
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
                            if (result == EC_Normal)
                                result = node->writeContentItem(*ditem, logStream);
                        } else {    // by-value
                            /* write RelationshipMacro */
                            if (result == EC_Normal)
                                result = node->writeDocumentRelationshipMacro(*ditem, markedItems, logStream);
                            /* write DocumentContentMacro */
                            if (result == EC_Normal)
                                node->writeDocumentContentMacro(*ditem, logStream);
                        }
                        /* check for any errors */
                        if (result != EC_Normal)
                            printContentItemErrorMessage(logStream, "Writing", result, node);
                        /* insert item into sequence */
                        if (result == EC_Normal)
                            dseq->insert(ditem);
                        else
                            delete ditem;
                    } else
                        result = EC_MemoryExhausted;
                } else
                    result = EC_IllegalCall;
            } while ((result == EC_Normal) && (cursor.gotoNext()));
            if (result == EC_Normal)
                dataset.insert(dseq);
            else
                delete dseq;
        } else
            result = EC_MemoryExhausted;
    }
    return result;
}


OFCondition DSRDocumentTreeNode::renderHTMLConceptName(ostream &docStream,
                                                       const size_t flags,
                                                       OFConsole *logStream) const
{
    if (!(flags & HF_renderItemInline) && (flags & HF_renderItemsSeparately))
    {
        /* flag indicating whether line is empty or not */
        OFBool writeLine = OFFalse;
        if (ConceptName.getCodeMeaning().length() > 0)
        {
            docStream << "<b>";
            /* render ConceptName & Code (if valid) */
            ConceptName.renderHTML(docStream, flags, logStream, (flags & HF_renderConceptNameCodes) && ConceptName.isValid() /* fullCode */);
            docStream << ":</b>";
            writeLine = OFTrue;
        } else if (flags & HF_currentlyInsideAnnex)
        {
            docStream << "<b>";
            /* render ValueType only */
            docStream << valueTypeToReadableName(ValueType);
            docStream << ":</b>";
            writeLine = OFTrue;
        }
        /* render optional observation datetime */
        if (ObservationDateTime.length() > 0)
        {
            if (writeLine)
                docStream << " ";
            OFString string;
            docStream << "<small>(observed: " << dicomToReadableDateTime(ObservationDateTime, string) << ")</small>";
            writeLine = OFTrue;
        }
        if (writeLine)
            docStream << "<br>" << endl;
    }
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::renderHTMLChildNodes(ostream &docStream,
                                                      ostream &annexStream,
                                                      const size_t nestingLevel,
                                                      size_t &annexNumber,
                                                      const size_t flags,
                                                      OFConsole *logStream) const
{
    OFCondition result = EC_Normal;
    /* goto first child of current node */
    DSRTreeNodeCursor cursor(Down);
    if (cursor.isValid())
    {
        /* flag used to format the relationship reference texts */
        OFBool paragraphFlag = (flags & HF_createFootnoteReferences);
        /* local version of flags */
        size_t newFlags = flags;
        /* footnote counter */
        size_t footnoteNumber = 1;
        /* create memory output stream for the temporal document */
        ostrstream tempDocStream;
        DSRDocumentTreeNode *node = NULL;
        do {        /* for all child nodes */
            node = (DSRDocumentTreeNode *)cursor.getNode();
            if (node != NULL)
            {
                /* set/reset flag for footnote creation*/
                newFlags &= ~HF_createFootnoteReferences;
                if (!(flags & HF_renderItemsSeparately) && node->hasChildNodes() && (node->getValueType() != VT_Container))
                    newFlags |= HF_createFootnoteReferences;
                /* render (optional) reference to annex */
                OFString relationshipText;
                if (getRelationshipText(node->getRelationshipType(), relationshipText, flags).length() > 0)
                {
                    if (paragraphFlag)
                    {
                        /* inside paragraph: line break */
                        docStream << "<br>" << endl;
                    } else {
                        /* open paragraph */
                        docStream << "<p><small>" << endl;
                        paragraphFlag = OFTrue;
                    }
                    docStream << "<u>" << relationshipText << "</u>: ";
                    /* expand short nodes with no children inline */
                    if (!(flags & HF_neverExpandChildrenInline) && !node->hasChildNodes() && node->isShort(flags))
                    {
                        if (node->getValueType() != VT_byReference)
                        {
                            /* render concept name/code or value type */
                            if (node->getConceptName().getCodeMeaning().length() > 0)
                                node->getConceptName().renderHTML(docStream, flags, logStream, (flags & HF_renderConceptNameCodes) && ConceptName.isValid() /* fullCode */);
                            else
                                docStream << valueTypeToReadableName(node->getValueType());
                            docStream << " = ";
                        }
                        /* render HTML code (directly to the reference text) */
                        result = node->renderHTML(docStream, annexStream, 0 /* nesting level */, annexNumber, newFlags | HF_renderItemInline, logStream);
                    } else {
                        /* render concept name or value type */
                        if (node->getConceptName().getCodeMeaning().length() > 0)
                            docStream << node->getConceptName().getCodeMeaning() << " ";
                        else
                            docStream << valueTypeToReadableName(node->getValueType()) << " ";
                        /* render annex heading and reference */
                        createHTMLAnnexEntry(docStream, annexStream, "" /* referenceText */, annexNumber);
                        /* create memory output stream for the temporal annex */
                        ostrstream tempAnnexStream;
                        /* render HTML code (directly to the annex) */
                        result = node->renderHTML(annexStream, tempAnnexStream, 0 /* nesting level */, annexNumber, newFlags | HF_currentlyInsideAnnex, logStream);
                        /* append temporary stream to main stream */
                        if (result == EC_Normal)
                            result = appendStream(annexStream, tempAnnexStream);
                    }
                } else {
                    /* close paragraph */
                    if (paragraphFlag)
                    {
                        docStream << "</small></p>" << endl;
                        paragraphFlag = OFFalse;
                    }
                    /* begin new paragraph */
                    if (flags & HF_renderItemsSeparately)
                        docStream << "<p>" << endl;
                    /* write footnote text to temporary stream */
                    if (newFlags & HF_createFootnoteReferences)
                    {
                        /* render HTML code (without child nodes) */
                        result = node->renderHTMLContentItem(docStream, annexStream, 0 /* nestingLevel */, annexNumber, newFlags, logStream);
                        /* create footnote numbers (individually for each child?) */
                        if (result == EC_Normal)
                        {
                            /* tags are closed automatically in 'node->renderHTMLChildNodes()' */
                            tempDocStream << "<p><small>" << endl;
                            /* render footnote text and reference */
                            createHTMLFootnote(docStream, tempDocStream, footnoteNumber, node->getNodeID());
                            /* render child nodes to temporary stream */
                            result = node->renderHTMLChildNodes(tempDocStream, annexStream, 0 /* nestingLevel */, annexNumber, newFlags, logStream);
                        }
                    } else {
                        /* render HTML code (incl. child nodes)*/
                        result = node->renderHTML(docStream, annexStream, nestingLevel + 1, annexNumber, newFlags, logStream);
                    }
                    /* end paragraph */
                    if (flags & HF_renderItemsSeparately)
                        docStream << "</p>" << endl;
                }
            } else
                result = EC_IllegalCall;
        } while ((result == EC_Normal) && (cursor.gotoNext()));
        /* close last open paragraph (if any) */
        if (paragraphFlag)
            docStream << "</small></p>" << endl;
        /* append temporary stream to main stream */
        if (result == EC_Normal)
            result = appendStream(docStream, tempDocStream);
    }
    return result;
}


const OFString &DSRDocumentTreeNode::getRelationshipText(const E_RelationshipType relationshipType,
                                                         OFString &relationshipText,
                                                         const size_t flags) const
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
 *  Revision 1.15  2001-09-26 13:04:20  meichel
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
 *  dded output of optional observation datetime to rendered HTML page.
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
