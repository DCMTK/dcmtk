/*
 *
 *  Copyright (C) 2000-2003, OFFIS
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
 *  Update Date:      $Date: 2003-08-07 13:29:43 $
 *  CVS/RCS Revision: $Revision: 1.24 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrdoctn.h"

#include "ofstring.h"
#include "ofstream.h"


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


OFBool DSRDocumentTreeNode::isShort(const size_t /*flags*/) const
{
    return OFTrue;
}


OFCondition DSRDocumentTreeNode::print(ostream &stream,
                                       const size_t flags) const
{
    if (RelationshipType != RT_isRoot)
        stream << relationshipTypeToReadableName(RelationshipType) << " ";
    stream << valueTypeToDefinedTerm(ValueType) << ":";
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


OFCondition DSRDocumentTreeNode::readXML(const DSRXMLDocument &doc,
                                         DSRXMLCursor cursor,
                                         const E_DocumentType documentType,
                                         const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (cursor.valid())
    {
        OFString idAttr;
        /* important: NULL indicates first child node */
        DSRDocumentTreeNode *node = NULL;
        /* read "id" attribute (optional) and compare with expected value */
        if (!doc.getStringFromAttribute(cursor, idAttr, "id", OFFalse /*encoding*/, OFFalse /*required*/).empty() &&
            (stringToNumber(idAttr.c_str()) != Ident))
        {
            /* create warning message */
            OFOStringStream oss;
            oss << "XML attribute 'id' (" << idAttr << ") deviates from current node number (" << Ident << ")";
            OFSTRINGSTREAM_GETSTR(oss, tmpString)
            printWarningMessage(doc.getLogStream(), tmpString);
            OFSTRINGSTREAM_FREESTR(tmpString)
        }
        /* read concept name (not required in some cases) */
        ConceptName.readXML(doc, doc.getNamedNode(cursor.getChild(), "concept", OFFalse /*required*/));
        /* read observation datetime (optional) */
        const DSRXMLCursor childCursor = doc.getNamedNode(cursor.getChild(), "observation", OFFalse /*required*/);
        if (childCursor.valid())
            doc.getStringFromNodeContent(doc.getNamedNode(childCursor.getChild(), "datetime"), ObservationDateTime);
        /* read node content (depends on value type) */
        result = readXMLContentItem(doc, cursor);
        /* goto first child node */
        cursor.gotoChild();
        /* iterate over all child content items */
        while (cursor.valid() && result.good())
        {
            /* get SR value type from current XML node (optional) */
            E_ValueType valueType = doc.getValueTypeFromNode(cursor);
            /* invalid types are silently ignored */
            if ((valueType != VT_invalid) || doc.matchNode(cursor, "reference"))
            {
                /* get SR relationship type */
                E_RelationshipType relationshipType = doc.getRelationshipTypeFromNode(cursor);
                if (valueType != VT_invalid)
                {
                    /* create new node (by-value) */
                    result = createAndAppendNewNode(node, documentType, relationshipType, valueType, OFFalse /*checkConstraints*/);
                } else {
                    /* create new node (by-reference) */
                    result = createAndAppendNewNode(node, documentType, relationshipType, VT_byReference);
                }
                if (result.good())
                {
                    /* proceed with reading child nodes */
                    result = node->readXML(doc, cursor, documentType, flags);
                    /* print node error message (if any) */
                    doc.printGeneralNodeError(cursor, result);
                } else {
                    /* create new node failed */
                    OFString message = "Cannot add \"";
                    message += relationshipTypeToReadableName(relationshipType);
                    message += " ";
                    if (valueType != VT_invalid)
                        message += valueTypeToDefinedTerm(valueType /*target item*/);
                    else
                        message += "(by-reference)";
                    message += "\" to ";
                    message += valueTypeToDefinedTerm(ValueType /*source item*/);
                    message += " in ";
                    message += documentTypeToReadableName(documentType);
                    printErrorMessage(doc.getLogStream(), message.c_str());
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
        writeStringValueToXML(stream, relationshipTypeToDefinedTerm(RelationshipType), "relationship", flags & XF_writeEmptyTags > 0);
    /* concept name */
    if (ConceptName.isValid())
    {
        if (flags & DSRTypes::XF_codeComponentsAsAttribute)
            stream << "<concept";     // bracket ">" is closed in the next writeXML() routine
        else
            stream << "<concept>" << endl;
        ConceptName.writeXML(stream, flags, logStream);
        stream << "</concept>" << endl;
    }
    /* observation datetime (optional) */
    if (!ObservationDateTime.empty())
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
            node = OFstatic_cast(DSRDocumentTreeNode *, cursor.getNode());
            if (node != NULL)
                result = node->writeXML(stream, flags, logStream);
            else
                result = SR_EC_InvalidDocumentTree;
        } while (result.good() && cursor.gotoNext());
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
    if (result.good())
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
    OFCondition result = EC_IllegalParameter;
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


OFBool DSRDocumentTreeNode::canAddNode(const E_DocumentType /*documentType*/,
                                       const E_RelationshipType /*relationshipType*/,
                                       const E_ValueType /*valueType*/,
                                       const OFBool /*byReference*/) const
{
    /* no restrictions */
    return OFTrue;
}


OFCondition DSRDocumentTreeNode::readContentItem(DcmItem & /*dataset*/,
                                                 OFConsole * /*logStream*/)
{
    /* no content to read */
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::writeContentItem(DcmItem & /*dataset*/,
                                                  OFConsole * /*logStream*/) const
{
    /* no content to insert */
    return EC_Normal;
}


OFCondition DSRDocumentTreeNode::renderHTMLContentItem(ostream & /*docStream*/,
                                                       ostream & /*annexStream*/,
                                                       const size_t /*nestingLevel*/,
                                                       size_t & /*annexNumber*/,
                                                       const size_t /*flags*/,
                                                       OFConsole * /*logStream*/) const
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
    result = readDocumentRelationshipMacro(dataset, documentType, "1" /*posString*/, flags, logStream);
    /* read DocumentContentMacro */
    if (result.good())
        result= readDocumentContentMacro(dataset, "1" /*posString*/, logStream);
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
    if (result.good())
        result= writeDocumentContentMacro(dataset, logStream);
    return result;
}


OFCondition DSRDocumentTreeNode::readDocumentRelationshipMacro(DcmItem &dataset,
                                                               const E_DocumentType documentType,
                                                               const OFString &posString,
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
    if (dataset.tagExists(DCM_ContentTemplateSequence))
        printWarningMessage(logStream, "ContentTemplateSequence detected - template identification not yet supported");
    /* read ContentSequence */
    if (result.good())
        result = readContentSequence(dataset, documentType, posString, flags, logStream);
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
    if (!ObservationDateTime.empty())
        result = putStringValueToDataset(dataset, DCM_ObservationDateTime, ObservationDateTime);
    /* tbd: write ContentTemplateSequence */

    /* write ContentSequence */
    if (result.good())
        result = writeContentSequence(dataset, markedItems, logStream);
    return result;
}


OFCondition DSRDocumentTreeNode::readDocumentContentMacro(DcmItem &dataset,
                                                          const OFString &posString,
                                                          OFConsole *logStream)
{
    OFCondition result = EC_Normal;
    /* skip: read ValueType, already done somewhere else */

    /* read ConceptNameCodeSequence (might be empty) */
    if (result.good())
        ConceptName.readSequence(dataset, DCM_ConceptNameCodeSequence, "1C" /*type*/, logStream);
    if (result.good())
    {
        /* read ContentItem (depending on ValueType) */
        result = readContentItem(dataset, logStream);
        /* check for validity, after reading */
        if (!isValid())
            printInvalidContentItemMessage(logStream, "Reading", this, posString.c_str());
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
    if (result.good())
    {
        if (ConceptName.isValid())
            result = ConceptName.writeSequence(dataset, DCM_ConceptNameCodeSequence, logStream);
    }
    if (result.good())
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
    OFCondition result = EC_Normal;
    /* do not check by-reference relationships here, will be done later (after complete reading) */
    if (((valueType == VT_byReference) && (relationshipType != RT_unknown)) ||
        !checkConstraints || !isConstraintCheckingSupported(documentType) ||
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
        } else {
            if (valueType == VT_unknown)
                result = SR_EC_UnknownValueType;
            else
                result = EC_MemoryExhausted;
        }
    } else {
        /* summarize what went wrong */
        if (valueType == VT_unknown)
            result = SR_EC_UnknownValueType;
        else if (relationshipType == RT_unknown)
            result = SR_EC_UnknownRelationshipType;
        else
            result = SR_EC_InvalidByValueRelationship;
    }
    return result;
}


OFCondition DSRDocumentTreeNode::readContentSequence(DcmItem &dataset,
                                                     const E_DocumentType documentType,
                                                     const OFString &posString,
                                                     const size_t flags,
                                                     OFConsole *logStream)
{
    OFCondition result = EC_Normal;
    DcmStack stack;
    /* read ContentSequence (might be absent or empty */
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
                    /* create current location string */
                    char buffer[20];
                    OFString location = posString;
                    if (!location.empty())
                        location += ".";
                    location += numberToString(OFstatic_cast(size_t, i + 1), buffer);
                    if ((logStream != NULL) && (flags & RF_showCurrentlyProcessedItem))
                    {
                        logStream->lockCerr() << "Processing content item " << location << endl;
                        logStream->unlockCerr();
                    }
                    /* read RelationshipType */
                    result = getAndCheckStringValueFromDataset(*ditem, DCM_RelationshipType, tmpString, "1", "1", logStream);
                    if (result.good())
                    {
                        relationshipType = definedTermToRelationshipType(tmpString);
                        /* check relationship type */
                        if (relationshipType == RT_unknown)
                            printUnknownValueWarningMessage(logStream, "RelationshipType", tmpString.c_str());
                        /* check for by-reference relationship */
                        DcmUnsignedLong referencedContentItemIdentifier(DCM_ReferencedContentItemIdentifier);
                        if (getAndCheckElementFromDataset(*ditem, referencedContentItemIdentifier, "1-n", "1C", logStream).good())
                        {
                            /* create new node (by-reference) */
                            result = createAndAppendNewNode(node, documentType, relationshipType, VT_byReference);
                            /* read ReferencedContentItemIdentifier (again) */
                            if (result.good())
                                result = node->readContentItem(*ditem, logStream);
                        } else {
                            /* read ValueType (from DocumentContentMacro) - required to create new node */
                            result = getAndCheckStringValueFromDataset(*ditem, DCM_ValueType, tmpString, "1", "1", logStream);
                            if (result.good())
                            {
                                /* read by-value relationship */
                                valueType = definedTermToValueType(tmpString);
                                /* check value type */
                                if (valueType == VT_unknown)
                                    printUnknownValueWarningMessage(logStream, "ValueType", tmpString.c_str());
                                /* create new node (by-value) */
                                result = createAndAppendNewNode(node, documentType, relationshipType, valueType, flags & RF_ignoreRelationshipConstraints == 0);
                                /* read RelationshipMacro */
                                if (result.good())
                                    result = node->readDocumentRelationshipMacro(*ditem, documentType, location, flags, logStream);
                                else
                                {
                                    /* create new node failed */
                                    OFString message = "Cannot add \"";
                                    message += relationshipTypeToReadableName(relationshipType);
                                    message += " ";
                                    message += valueTypeToDefinedTerm(valueType /*target item*/);
                                    message += "\" to ";
                                    message += valueTypeToDefinedTerm(ValueType /*source item*/);
                                    message += " in ";
                                    message += documentTypeToReadableName(documentType);
                                    printErrorMessage(logStream, message.c_str());
                                }
                                /* read DocumentContentMacro (might be empty) */
                                if (result.good())
                                    node->readDocumentContentMacro(*ditem, location.c_str(), logStream);
                            }
                        }
                    }
                    /* check for any errors */
                    if (result.bad() /*&& (node != NULL)*/)
                    {
                        printContentItemErrorMessage(logStream, "Reading", result, node, location.c_str());
                        /* print current data set (item) that caused the error */
                        if ((logStream != NULL) && (flags & RF_verboseDebugMode))
                        {
                            logStream->lockCerr() << OFString(31, '-') << " DICOM DATA SET " << OFString(31, '-') << endl;
                            ditem->print(logStream->getCerr(), OFFalse /*showFullData*/, 1 /*level*/);
                            logStream->getCerr() << OFString(78, '-') << endl;
                            logStream->unlockCerr();
                        }
                    }
                } else
                    result = SR_EC_InvalidDocumentTree;
                i++;
            }
            /* skipping complete sub-tree if flag is set */
            if (result.bad() && (flags & RF_skipInvalidContentItems))
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
                                result = node->writeContentItem(*ditem, logStream);
                        } else {    // by-value
                            /* write RelationshipMacro */
                            if (result.good())
                                result = node->writeDocumentRelationshipMacro(*ditem, markedItems, logStream);
                            /* write DocumentContentMacro */
                            if (result.good())
                                node->writeDocumentContentMacro(*ditem, logStream);
                        }
                        /* check for any errors */
                        if (result.bad())
                            printContentItemErrorMessage(logStream, "Writing", result, node);
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


OFCondition DSRDocumentTreeNode::renderHTMLConceptName(ostream &docStream,
                                                       const size_t flags,
                                                       OFConsole *logStream) const
{
    if (!(flags & HF_renderItemInline) && (flags & HF_renderItemsSeparately))
    {
        /* flag indicating whether line is empty or not */
        OFBool writeLine = OFFalse;
        if (!ConceptName.getCodeMeaning().empty())
        {
            docStream << "<b>";
            /* render ConceptName & Code (if valid) */
            ConceptName.renderHTML(docStream, flags, logStream, (flags & HF_renderConceptNameCodes) && ConceptName.isValid() /*fullCode*/);
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
            docStream << "<small>(observed: " << dicomToReadableDateTime(ObservationDateTime, tmpString) << ")</small>";
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
        OFBool paragraphFlag = (flags & HF_createFootnoteReferences > 0);
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
                            if (node->getConceptName().getCodeMeaning().empty())
                                docStream << valueTypeToReadableName(node->getValueType());
                            else
                                node->getConceptName().renderHTML(docStream, flags, logStream, (flags & HF_renderConceptNameCodes) && ConceptName.isValid() /*fullCode*/);
                            docStream << " = ";
                        }
                        /* render HTML code (directly to the reference text) */
                        result = node->renderHTML(docStream, annexStream, 0 /*nesting level*/, annexNumber, newFlags | HF_renderItemInline, logStream);
                    } else {
                        /* render concept name or value type */
                        if (node->getConceptName().getCodeMeaning().empty())
                            docStream << valueTypeToReadableName(node->getValueType()) << " ";
                        else
                            docStream << node->getConceptName().getCodeMeaning() << " ";
                        /* render annex heading and reference */
                        createHTMLAnnexEntry(docStream, annexStream, "" /*referenceText*/, annexNumber);
                        /* create memory output stream for the temporal annex */
                        OFOStringStream tempAnnexStream;
                        /* render HTML code (directly to the annex) */
                        result = node->renderHTML(annexStream, tempAnnexStream, 0 /*nesting level*/, annexNumber, newFlags | HF_currentlyInsideAnnex, logStream);
                        /* append temporary stream to main stream */
                        if (result.good())
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
                        result = node->renderHTMLContentItem(docStream, annexStream, 0 /*nestingLevel*/, annexNumber, newFlags, logStream);
                        /* create footnote numbers (individually for each child?) */
                        if (result.good())
                        {
                            /* tags are closed automatically in 'node->renderHTMLChildNodes()' */
                            tempDocStream << "<p><small>" << endl;
                            /* render footnote text and reference */
                            createHTMLFootnote(docStream, tempDocStream, footnoteNumber, node->getNodeID());
                            /* render child nodes to temporary stream */
                            result = node->renderHTMLChildNodes(tempDocStream, annexStream, 0 /*nestingLevel*/, annexNumber, newFlags, logStream);
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
                result = SR_EC_InvalidDocumentTree;
        } while (result.good() && cursor.gotoNext());
        /* close last open paragraph (if any) */
        if (paragraphFlag)
            docStream << "</small></p>" << endl;
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
 *  Revision 1.24  2003-08-07 13:29:43  joergr
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
 *  Thanks again to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 *  contribution.
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
