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
 *    classes: DSRDocumentTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-23 15:03:36 $
 *  CVS/RCS Revision: $Revision: 1.4 $
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
 : RelationshipType(relationshipType),
   ValueType(valueType),
   ConceptName(),
   ObservationDateTime()
{
}


DSRDocumentTreeNode::~DSRDocumentTreeNode()
{
}


void DSRDocumentTreeNode::clear()
{
    ConceptName.clear();
    ObservationDateTime.clear();
}


OFBool DSRDocumentTreeNode::isValid() const
{
    return (RelationshipType != RT_invalid) && (ValueType != VT_invalid);
}


OFBool DSRDocumentTreeNode::isShort(const size_t /* flags */) const
{
    return OFTrue;
}


E_Condition DSRDocumentTreeNode::print(ostream &stream,
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


E_Condition DSRDocumentTreeNode::read(DcmItem &dataset,
                                      const E_DocumentType documentType,
                                      OFConsole *logStream)
{
    return readSRDocumentContentModule(dataset, documentType, logStream);
}


E_Condition DSRDocumentTreeNode::write(DcmItem &dataset,
                                       OFConsole *logStream) const
{
    return writeSRDocumentContentModule(dataset, logStream);
}


E_Condition DSRDocumentTreeNode::renderHTML(ostream &docStream,
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
    /* render child nodes */
    if (result == EC_Normal)
        result = renderHTMLChildNodes(docStream, annexStream, nestingLevel, annexNumber, flags | HF_renderItemsSeparately, logStream);
    else
        printContentItemErrorMessage(logStream, "Rendering", result, this);
    return result;
}


E_Condition DSRDocumentTreeNode::getConceptName(DSRCodedEntryValue &conceptName) const
{
    conceptName = ConceptName;
    return EC_Normal;
}


E_Condition DSRDocumentTreeNode::setConceptName(const DSRCodedEntryValue &conceptName)
{
    E_Condition result = EC_IllegalCall;
    /* check for valid code */
    if (conceptName.isValid() || conceptName.isEmpty())
        ConceptName = conceptName;
    return result;
}


E_Condition DSRDocumentTreeNode::setObservationDateTime(const OFString &observationDateTime)
{
    /* might add a check for proper DateTime format */
    ObservationDateTime = observationDateTime;
    return EC_Normal;
}


OFBool DSRDocumentTreeNode::canAddNode(const E_DocumentType /* documentType */,
                                       const E_RelationshipType /* relationshipType */,
                                       const E_ValueType /* valueType */) const
{
    /* no restrictions */
    return OFTrue;
}


E_Condition DSRDocumentTreeNode::readContentItem(DcmItem & /* dataset */,
                                                 OFConsole * /* logStream */)
{
    /* no content to read */
    return EC_Normal;
}


E_Condition DSRDocumentTreeNode::writeContentItem(DcmItem & /* dataset */,
                                                  OFConsole * /* logStream */) const
{
    /* no content to insert */
    return EC_Normal;
}


E_Condition DSRDocumentTreeNode::renderHTMLContentItem(ostream & /* docStream */,
                                                       ostream & /* annexStream */,
                                                       const size_t /* nestingLevel */,
                                                       size_t & /* annexNumber */,
                                                       const size_t /* flags */,
                                                       OFConsole * /* logStream */) const
{
    /* no content to render */
    return EC_Normal;
}


E_Condition DSRDocumentTreeNode::readSRDocumentContentModule(DcmItem &dataset,
                                                             const E_DocumentType documentType,
                                                             OFConsole *logStream)
{
    E_Condition result = EC_Normal;
    /* read DocumentRelationshipMacro */
    result = readDocumentRelationshipMacro(dataset, documentType, logStream);
    /* read DocumentContentMacro */
    if (result == EC_Normal)
        result= readDocumentContentMacro(dataset, logStream);
    return result;
}


E_Condition DSRDocumentTreeNode::writeSRDocumentContentModule(DcmItem &dataset,
                                                              OFConsole *logStream) const
{
    E_Condition result = EC_Normal;
    /* write DocumentRelationshipMacro */
    result = writeDocumentRelationshipMacro(dataset, logStream);
    /* write DocumentContentMacro */
    if (result == EC_Normal)
        result= writeDocumentContentMacro(dataset, logStream);
    return result;
}


E_Condition DSRDocumentTreeNode::readDocumentRelationshipMacro(DcmItem &dataset,
                                                               const E_DocumentType documentType,
                                                               OFConsole *logStream)
{
    E_Condition result = EC_Normal;
    /* read ObservationDateTime (conditional) */
    getAndCheckStringValueFromDataset(dataset, DCM_ObservationDateTime, ObservationDateTime, "1", "1C", logStream);
    /* tbd: read ContentTemplateSequence */

    /* read ContentSequence */
    if (result == EC_Normal)
        result = readContentSequence(dataset, documentType, logStream);
    return result;
}


E_Condition DSRDocumentTreeNode::writeDocumentRelationshipMacro(DcmItem &dataset,
                                                                OFConsole *logStream) const
{
    E_Condition result = EC_Normal;
    /* write ObservationDateTime (conditional) */
    if (ObservationDateTime.length() > 0)
        result = putStringValueToDataset(dataset, DCM_ObservationDateTime, ObservationDateTime);
    /* tbd: write ContentTemplateSequence */

    /* write ContentSequence */
    if (result == EC_Normal)
        result = writeContentSequence(dataset, logStream);
    return result;
}


E_Condition DSRDocumentTreeNode::readDocumentContentMacro(DcmItem &dataset,
                                                          OFConsole *logStream)
{
    E_Condition result = EC_Normal;
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


E_Condition DSRDocumentTreeNode::writeDocumentContentMacro(DcmItem &dataset,
                                                           OFConsole *logStream) const
{
    E_Condition result = EC_Normal;
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


E_Condition DSRDocumentTreeNode::createAndAppendNewNode(DSRDocumentTreeNode *&previousNode,
                                                        const E_DocumentType documentType,
                                                        const E_RelationshipType relationshipType,
                                                        const E_ValueType valueType)
{
    E_Condition result = EC_IllegalCall;
    if (canAddNode(documentType, relationshipType, valueType))
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
        } else
            result = EC_CorruptedData;
    }
    return result;
}


E_Condition DSRDocumentTreeNode::readContentSequence(DcmItem &dataset,
                                                     const E_DocumentType documentType,
                                                     OFConsole *logStream)
{
    E_Condition result = EC_Normal;
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
                            OFString message = "Reading unknown relationship type ";
                            message += string;
                            printWarningMessage(logStream, message.c_str());
                        }
                        /* read ValueType (from DocumentContentMacro) - required to create new node */
                        result = getAndCheckStringValueFromDataset(*ditem, DCM_ValueType, string, "1", "1", logStream);
                    }
                    if (result == EC_Normal)
                    {
                        valueType = definedTermToValueType(string);
                        /* check value type */
                        if (valueType == VT_unknown)
                        {
                            OFString message = "Reading unknown value type ";
                            message += string;
                            printWarningMessage(logStream, message.c_str());
                        }
                        /* create new node */
                        result = createAndAppendNewNode(node, documentType, relationshipType, valueType);
                    }
                    /* read RelationshipMacro (only for by-value references! to be checked) */
                    if (result == EC_Normal)
                        result = node->readDocumentRelationshipMacro(*ditem, documentType, logStream);
                    /* read DocumentContentMacro (might be empty) */
                    if (result == EC_Normal)
                        node->readDocumentContentMacro(*ditem, logStream);
                    /* tbd: read ReferencedContentItemIdentifier (conditional) */

                    /* check for any errors */
                    if (result != EC_Normal)
                        printContentItemErrorMessage(logStream, "Reading", result, node);
                } else
                    result = EC_CorruptedData;
                i++;
            }
        }
    }
    return result;
}


E_Condition DSRDocumentTreeNode::writeContentSequence(DcmItem &dataset,
                                                      OFConsole *logStream) const
{
    E_Condition result = EC_Normal;
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
                        /* write RelationshipMacro (only for by-value references !) */
                        if (result == EC_Normal)
                            result = node->writeDocumentRelationshipMacro(*ditem, logStream);
                        /* write DocumentContentMacro */
                        if (result == EC_Normal)
                            node->writeDocumentContentMacro(*ditem, logStream);
                        /* tbd: write ReferencedContentItemIdentifier (conditional) */

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


E_Condition DSRDocumentTreeNode::renderHTMLConceptName(ostream &docStream,
                                                       const size_t flags,
                                                       OFConsole *logStream) const
{
    if (!(flags & HF_renderItemInline) && (flags & HF_renderItemsSeparately))
    {
        if ((flags & HF_renderConceptNameCodes) && (ConceptName.isValid()))
        {
            docStream << "<b>";
            /* render ConceptName & Code */
            ConceptName.renderHTML(docStream, logStream);
            docStream << ":</b><br>" << endl;
        } else if (ConceptName.getCodeMeaning().length() > 0)
        {
            docStream << "<b>";
            /* render ConceptName only */
            docStream << ConceptName.getCodeMeaning();
            docStream << ":</b><br>" << endl;
        } else if (flags & HF_currentlyInsideAnnex)
        {
            docStream << "<b>";
            /* render ValueType only */
            docStream << valueTypeToReadableName(ValueType);
            docStream << ":</b><br>" << endl;
        }
    }
    return EC_Normal;
}


E_Condition DSRDocumentTreeNode::renderHTMLChildNodes(ostream &docStream,
                                                      ostream &annexStream,
                                                      const size_t nestingLevel,
                                                      size_t &annexNumber,
                                                      const size_t flags,
                                                      OFConsole *logStream) const
{
    E_Condition result = EC_Normal;
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
                        docStream << "<small><p>" << endl;
                        paragraphFlag = OFTrue;
                    }
                    docStream << "<u>" << relationshipText << "</u>: ";
                    /* expand short nodes with no childs inline */
                    if (!(flags & HF_neverExpandChildsInline) && !node->hasChildNodes() && node->isShort(flags))
                    {
                        /* render concept name or value type */
                        if (node->getConceptName().getCodeMeaning().length() > 0)
                        {
                            /* render full concept name (incl. code) if required */
                            if ((ConceptName.isValid()) && (flags & HF_renderConceptNameCodes))
                                ConceptName.renderHTML(docStream, logStream);
                            else
                                docStream << node->getConceptName().getCodeMeaning();
                            docStream << " ";
                        } else
                            docStream << valueTypeToReadableName(node->getValueType()) << " ";
                        /* render HTML code (directly to the reference text) */
                        result = node->renderHTML(docStream, annexStream, 0 /* nesting level */, annexNumber, newFlags | HF_renderItemInline, logStream);
//                        docStream << endl;
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
                        docStream << "</p></small>" << endl;
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
                            tempDocStream << "<small><p>" << endl;
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
            docStream << "</p></small>" << endl;
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
 *  Revision 1.4  2000-10-23 15:03:36  joergr
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
