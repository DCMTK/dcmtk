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
 *    classes: DSRDocumentTree
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoctr.h"
#include "dcmtk/dcmsr/dsrcontn.h"
#include "dcmtk/dcmsr/dsrreftn.h"
#include "dcmtk/dcmsr/dsrxmld.h"
#include "dcmtk/dcmsr/dsriodcc.h"


DSRDocumentTree::DSRDocumentTree(const E_DocumentType documentType)
  : DSRTree(),
    DocumentType(DT_invalid),
    CurrentContentItem(),
    ConstraintChecker(NULL)
{
    /* check & set document type, create constraint checker object */
    changeDocumentType(documentType);
}


DSRDocumentTree::~DSRDocumentTree()
{
    delete ConstraintChecker;
}


void DSRDocumentTree::clear()
{
    DSRTree::clear();
}


OFBool DSRDocumentTree::isValid() const
{
    OFBool result = OFFalse;
    if (isDocumentTypeSupported(DocumentType))
    {
        /* check root node */
        const DSRDocumentTreeNode *node = OFstatic_cast(DSRDocumentTreeNode *, getRoot());
        if (node != NULL)
        {
            if ((node->getRelationshipType() == RT_isRoot) && (node->getValueType() == VT_Container))
                result = OFTrue;
        }
    }
    return result;
}


OFCondition DSRDocumentTree::print(STD_NAMESPACE ostream &stream,
                                   const size_t flags)
{
    OFCondition result = EC_Normal;
    DSRTreeNodeCursor cursor(getRoot());
    if (cursor.isValid())
    {
        /* check and update by-reference relationships (if applicable) */
        checkByReferenceRelationships(CM_updatePositionString);
        OFString tmpString;
        size_t level = 0;
        const DSRDocumentTreeNode *node = NULL;
        /* iterate over all nodes */
        do {
            node = OFstatic_cast(DSRDocumentTreeNode *, cursor.getNode());
            if (node != NULL)
            {
                /* print node position */
                if (flags & PF_printItemPosition)
                {
                    DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_ITEM_POSITION)
                    stream << cursor.getPosition(tmpString) << "  ";
                } else {
                    /* use line identation */
                    level = cursor.getLevel();
                    if (level > 0)  // valid ?
                        stream << OFString((level - 1) * 2, ' ');
                }
                /* print node content */
                DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
                stream << "<";
                result = node->print(stream, flags);
                DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
                stream << ">";
                /* print observation date/time (optional) */
                if (!node->getObservationDateTime().empty())
                {
                    stream << " {" << dicomToReadableDateTime(node->getObservationDateTime(), tmpString) << "}";
                }
                if (flags & PF_printTemplateIdentification)
                {
                    /* check for template identification */
                    OFString templateIdentifier, mappingResource;
                    if (node->getTemplateIdentification(templateIdentifier, mappingResource).good())
                    {
                        if (!templateIdentifier.empty() && !mappingResource.empty())
                        {
                            DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
                            stream << "  # ";
                            DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_TEMPLATE_ID)
                            stream << "TID " << templateIdentifier << " (" << mappingResource << ")";
                        }
                    }
                }
                DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_RESET)
                stream << OFendl;
            } else
                result = SR_EC_InvalidDocumentTree;
        } while (result.good() && cursor.iterate());
    }
    return result;
}


OFCondition DSRDocumentTree::read(DcmItem &dataset,
                                  const E_DocumentType documentType,
                                  const size_t flags)
{
    /* clear current document tree, check & change document type */
    OFCondition result = changeDocumentType(documentType);
    if (result.good())
    {
        if (ConstraintChecker == NULL)
            DCMSR_WARN("Check for relationship content constraints not yet supported");
        else if (ConstraintChecker->isTemplateSupportRequired())
            DCMSR_WARN("Check for template constraints not yet supported");
        if (flags & RF_showCurrentlyProcessedItem)
            DCMSR_INFO("Processing content item 1");
        /* first try to read value type */
        OFString tmpString;
        if (getAndCheckStringValueFromDataset(dataset, DCM_ValueType, tmpString, "1", "1").good() ||
            (flags & RF_ignoreContentItemErrors))
        {
            /* root node should always be a container */
            if (definedTermToValueType(tmpString) != VT_Container)
            {
                if (flags & RF_ignoreContentItemErrors)
                    DCMSR_WARN("Root content item should always be a CONTAINER");
                else {
                    DCMSR_ERROR("Root content item should always be a CONTAINER");
                    result = SR_EC_InvalidDocumentTree;
                }
            }
            if (result.good())
            {
                /* ... then create corresponding document tree node */
                DSRDocumentTreeNode *node = new DSRContainerTreeNode(RT_isRoot);
                if (node != NULL)
                {
                    /* ... insert it into the (empty) tree - checking is not required here */
                    if (addNode(node))
                    {
                        /* ... and let the node read the rest of the document */
                        result = node->read(dataset, ConstraintChecker, flags);
                        /* check and update by-reference relationships (if applicable) */
                        checkByReferenceRelationships(CM_updateNodeID, flags);
                    } else
                        result = SR_EC_InvalidDocumentTree;
                } else
                    result = EC_MemoryExhausted;
            }
        } else {
            DCMSR_ERROR("ValueType attribute for root content item is missing");
            result = SR_EC_MandatoryAttributeMissing;
        }
    }
    return result;
}


OFCondition DSRDocumentTree::readXML(const DSRXMLDocument &doc,
                                     DSRXMLCursor cursor,
                                     const size_t flags)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (ConstraintChecker == NULL)
        DCMSR_WARN("Check for relationship content constraints not yet supported");
    else if (ConstraintChecker->isTemplateSupportRequired())
        DCMSR_WARN("Check for template constraints not yet supported");
    /* we assume that 'cursor' points to the "content" element */
    if (cursor.valid())
    {
        OFString templateIdentifier, mappingResource;
        /* template identification information expected "outside" content item */
        if (flags & XF_templateElementEnclosesItems)
        {
            /* check for optional root template identification */
            const DSRXMLCursor childCursor = doc.getNamedNode(cursor, "template", OFFalse /*required*/);
            if (childCursor.valid())
            {
                doc.getStringFromAttribute(childCursor, mappingResource, "resource");
                doc.getStringFromAttribute(childCursor, templateIdentifier, "tid");
                /* get first child of the "template" element */
                cursor = childCursor.getChild();
            }
        }
        E_ValueType valueType = doc.getValueTypeFromNode(cursor);
        /* proceed to first valid container (if any) */
        while (cursor.getNext().valid() && (valueType != VT_Container))
            valueType = doc.getValueTypeFromNode(cursor.gotoNext());
        /* root node should always be a container */
        if (valueType == VT_Container)
        {
            /* ... then create corresponding document tree node */
            DSRDocumentTreeNode *node = new DSRContainerTreeNode(RT_isRoot);
            if (node != NULL)
            {
                /* ... insert it into the (empty) tree - checking is not required here */
                if (addNode(node))
                {
                    if (flags & XF_templateElementEnclosesItems)
                    {
                        /* set template identification (if any) */
                        if (node->setTemplateIdentification(templateIdentifier, mappingResource).bad())
                            DCMSR_WARN("Root content item has invalid/incomplete template identification");
                    }
                    /* ... and let the node read the rest of the document */
                    result = node->readXML(doc, cursor, DocumentType, flags);
                    /* check and update by-reference relationships (if applicable) */
                    checkByReferenceRelationships(CM_updatePositionString);
                } else
                    result = SR_EC_InvalidDocumentTree;
            } else
                result = EC_MemoryExhausted;
        } else {
            DCMSR_ERROR("Root content item should always be a CONTAINER");
            result = SR_EC_InvalidDocumentTree;
        }
    }
    return result;
}


OFCondition DSRDocumentTree::write(DcmItem &dataset,
                                   DcmStack *markedItems)
{
    OFCondition result = SR_EC_InvalidDocumentTree;
    /* check whether root node has correct relationship and value type */
    if (isValid())
    {
        DSRDocumentTreeNode *node = OFstatic_cast(DSRDocumentTreeNode *, getRoot());
        if (node != NULL)
        {
            /* check and update by-reference relationships (if applicable) */
            checkByReferenceRelationships(CM_updatePositionString);
            /* start writing from root node */
            result = node->write(dataset, markedItems);
        }
    }
    return result;
}


OFCondition DSRDocumentTree::writeXML(STD_NAMESPACE ostream &stream,
                                      const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocumentTree;
    /* check whether root node has correct relationship and value type */
    if (isValid())
    {
        DSRDocumentTreeNode *node = OFstatic_cast(DSRDocumentTreeNode *, getRoot());
        /* start writing from root node */
        if (node != NULL)
        {
            /* check by-reference relationships (if applicable) */
            checkByReferenceRelationships(CM_resetReferenceTargetFlag);
            /* start writing from root node */
            result = node->writeXML(stream, flags);
        }
    }
    return result;
}


OFCondition DSRDocumentTree::renderHTML(STD_NAMESPACE ostream &docStream,
                                        STD_NAMESPACE ostream &annexStream,
                                        const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocumentTree;
    /* check whether root node has correct relationship and value type */
    if (isValid())
    {
        DSRDocumentTreeNode *node = OFstatic_cast(DSRDocumentTreeNode *, getRoot());
        /* start rendering from root node */
        if (node != NULL)
        {
            /* check by-reference relationships (if applicable) */
            checkByReferenceRelationships(CM_resetReferenceTargetFlag);
            size_t annexNumber = 1;
            /* start rendering from root node */
            result = node->renderHTML(docStream, annexStream, 1 /*nestingLevel*/, annexNumber, flags & ~HF_internalUseOnly);
        }
    }
    return result;
}


OFCondition DSRDocumentTree::changeDocumentType(const E_DocumentType documentType)
{
    OFCondition result = SR_EC_UnsupportedValue;
    /* first, check whether new document type is supported at all */
    if (isDocumentTypeSupported(documentType))
    {
        /* clear object */
        clear();
        /* store new document type */
        DocumentType = documentType;
        /* create appropriate IOD constraint checker */
        delete ConstraintChecker;
        ConstraintChecker = createIODConstraintChecker(documentType);
        result = EC_Normal;
    }
    return result;
}


OFBool DSRDocumentTree::canAddContentItem(const E_RelationshipType relationshipType,
                                          const E_ValueType valueType,
                                          const E_AddMode addMode)
{
    OFBool result = OFFalse;
    const DSRDocumentTreeNode *node = OFstatic_cast(const DSRDocumentTreeNode *, getNode());
    if (node != NULL)
    {
        if (ConstraintChecker != NULL)
        {
            if ((addMode == AM_beforeCurrent) || (addMode == AM_afterCurrent))
            {
                /* check parent node */
                node = OFstatic_cast(const DSRDocumentTreeNode *, getParentNode());
                if (node != NULL)
                    result = ConstraintChecker->checkContentRelationship(node->getValueType(), relationshipType, valueType);
            } else
                result = ConstraintChecker->checkContentRelationship(node->getValueType(), relationshipType, valueType);
        } else
            result = OFTrue;    /* cannot check, therefore, allow everything */
    } else {
        /* root node has to be a Container */
        result = (relationshipType == RT_isRoot) && (valueType == VT_Container);
    }
    return result;
}


OFBool DSRDocumentTree::canAddByReferenceRelationship(const E_RelationshipType relationshipType,
                                                      const E_ValueType targetValueType)
{
    OFBool result = OFFalse;
    if (ConstraintChecker != NULL)
    {
        const DSRDocumentTreeNode *node = OFstatic_cast(const DSRDocumentTreeNode *, getNode());
        if (node != NULL)
            result = ConstraintChecker->checkContentRelationship(node->getValueType(), relationshipType, targetValueType, OFTrue /*byReference*/);
    } else
        result = OFTrue;    /* cannot check, therefore, allow everything */
    return result;
}


size_t DSRDocumentTree::addContentItem(const E_RelationshipType relationshipType,
                                       const E_ValueType valueType,
                                       const E_AddMode addMode)
{
    size_t nodeID = 0;
    if (canAddContentItem(relationshipType, valueType, addMode))
    {
        /* create a new node ... */
        DSRDocumentTreeNode *node = createDocumentTreeNode(relationshipType, valueType);
        if (node != NULL)
        {
            /* ... and add it to the current node */
            nodeID = addNode(node, addMode);
        }
    }
    return nodeID;
}


size_t DSRDocumentTree::addByReferenceRelationship(const E_RelationshipType relationshipType,
                                                   const size_t referencedNodeID)
{
    size_t nodeID = 0;
    if (referencedNodeID > 0)
    {
        DSRTreeNodeCursor cursor(getRoot());
        if (cursor.isValid())
        {
            /* goto specified target node (might be improved later on) */
            if (cursor.gotoNode(referencedNodeID))
            {
                OFString sourceString;
                OFString targetString;
                getPosition(sourceString);
                cursor.getPosition(targetString);
                /* check whether target node is an ancestor of source node (prevent loops) */
                if (sourceString.substr(0, targetString.length()) != targetString)
                {
                    const DSRDocumentTreeNode *targetNode = OFstatic_cast(DSRDocumentTreeNode *, cursor.getNode());
                    if (targetNode != NULL)
                    {
                        /* check whether relationship is valid/allowed */
                        if (canAddByReferenceRelationship(relationshipType, targetNode->getValueType()))
                        {
                            DSRDocumentTreeNode *node = new DSRByReferenceTreeNode(relationshipType, referencedNodeID);
                            if (node != NULL)
                            {
                                nodeID = addNode(node, AM_belowCurrent);
                                /* go back to current node */
                                if (nodeID > 0)
                                    goUp();
                            }
                        }
                    }
                }
            }
        }
    }
    return nodeID;
}


size_t DSRDocumentTree::removeCurrentContentItem()
{
    return removeNode();
}


DSRContentItem &DSRDocumentTree::getCurrentContentItem()
{
    CurrentContentItem.setTreeNode(OFstatic_cast(DSRDocumentTreeNode *, getNode()));
    return CurrentContentItem;
}


size_t DSRDocumentTree::gotoNamedNode(const DSRCodedEntryValue &conceptName,
                                      const OFBool startFromRoot,
                                      const OFBool searchIntoSub)
{
    size_t nodeID = 0;
    if (conceptName.isValid())
    {
        if (startFromRoot)
            gotoRoot();
        DSRDocumentTreeNode *node = NULL;
        /* iterate over all nodes */
        do {
            node = OFstatic_cast(DSRDocumentTreeNode *, getNode());
            if ((node != NULL) && (node->getConceptName() == conceptName))
                nodeID = node->getNodeID();
        } while ((nodeID == 0) && iterate(searchIntoSub));
    }
    return nodeID;
}


size_t DSRDocumentTree::gotoNextNamedNode(const DSRCodedEntryValue &conceptName,
                                          const OFBool searchIntoSub)
{
    /* first, goto "next" node */
    size_t nodeID = iterate(searchIntoSub);
    if (nodeID > 0)
        nodeID = gotoNamedNode(conceptName, OFFalse /*startFromRoot*/, searchIntoSub);
    return nodeID;
}


void DSRDocumentTree::unmarkAllContentItems()
{
    DSRTreeNodeCursor cursor(getRoot());
    if (cursor.isValid())
    {
        DSRDocumentTreeNode *node = NULL;
        /* iterate over all nodes */
        do {
            node = OFstatic_cast(DSRDocumentTreeNode *, cursor.getNode());
            if (node != NULL)
                node->setMark(OFFalse);
        } while (cursor.iterate());
    }
}


void DSRDocumentTree::removeSignatures()
{
    DSRTreeNodeCursor cursor(getRoot());
    if (cursor.isValid())
    {
        DSRDocumentTreeNode *node = NULL;
        /* iterate over all nodes */
        do {
            node = OFstatic_cast(DSRDocumentTreeNode *, cursor.getNode());
            if (node != NULL)
                node->removeSignatures();
        } while (cursor.iterate());
    }
}


size_t DSRDocumentTree::addNode(DSRTreeNode * /*node*/,
                                const E_AddMode /*addMode*/)
{
    /* invalid for this class */
    return 0;
}


size_t DSRDocumentTree::addNode(DSRDocumentTreeNode *node,
                                const E_AddMode addMode)
{
    /* might add a check for templates later on */
    return DSRTree::addNode(node, addMode);
}


size_t DSRDocumentTree::removeNode()
{
    /* might add a check for templates later on */
    return DSRTree::removeNode();
}


OFCondition DSRDocumentTree::checkByReferenceRelationships(const size_t mode,
                                                           const size_t flags)
{
    OFCondition result = EC_IllegalParameter;
    /* the update flags are mutually exclusive */
    if (!((mode & CM_updatePositionString) && (mode & CM_updateNodeID)))
    {
        result = EC_Normal;
        /* by-reference relationships are only allowed for particular IODs */
        if ((ConstraintChecker != NULL) && ConstraintChecker->isByReferenceAllowed())
        {
            /* specify for all content items not to be the target of a by-reference relationship */
            if (mode & CM_resetReferenceTargetFlag)
                resetReferenceTargetFlag();
            /* start at the root of the document tree */
            DSRTreeNodeCursor cursor(getRoot());
            if (cursor.isValid())
            {
                const DSRDocumentTreeNode *node = NULL;
                do {    /* for all content items */
                    node = OFstatic_cast(DSRDocumentTreeNode *, cursor.getNode());
                    if (node != NULL)
                    {
                        /* only check/update by-reference relationships */
                        if (node->getValueType() == VT_byReference)
                        {
                            size_t refNodeID = 0;
                            /* type cast to directly access member variables of by-reference class */
                            DSRByReferenceTreeNode *refNode = OFconst_cast(DSRByReferenceTreeNode *, OFstatic_cast(const DSRByReferenceTreeNode *, node));
                            if (flags & RF_showCurrentlyProcessedItem)
                            {
                                OFString posString;
                                DCMSR_WARN("Updating by-reference relationship in content item " << cursor.getPosition(posString));
                            }
                            /* start searching from root node (be careful with large trees, might be improved later on) */
                            DSRTreeNodeCursor refCursor(getRoot());
                            if (mode & CM_updateNodeID)
                            {
                                /* update node ID */
                                refNodeID = refCursor.gotoNode(refNode->ReferencedContentItem);
                                if (refNodeID > 0)
                                    refNode->ReferencedNodeID = refCursor.getNodeID();
                                else
                                    refNode->ReferencedNodeID = 0;
                                refNode->ValidReference = (refNode->ReferencedNodeID > 0);
                            } else {
                                /* ReferenceNodeID contains a valid value */
                                refNodeID = refCursor.gotoNode(refNode->ReferencedNodeID);
                                if (mode & CM_updatePositionString)
                                {
                                    /* update position string */
                                    if (refNodeID > 0)
                                        refCursor.getPosition(refNode->ReferencedContentItem);
                                    else
                                        refNode->ReferencedContentItem.clear();
                                    /* tbd: check for valid reference could be more strict */
                                    refNode->ValidReference = checkForValidReference(refNode->ReferencedContentItem);
                                } else if (refNodeID == 0)
                                    refNode->ValidReference = OFFalse;
                            }
                            if (refNodeID > 0)
                            {
                                /* source and target content items should not be identical */
                                if (refNodeID != cursor.getNodeID())
                                {
                                    OFString posString;
                                    cursor.getPosition(posString);
                                    /* check whether target node is an ancestor of source node (prevent loops) */
                                    if (posString.substr(0, refNode->ReferencedContentItem.length()) != refNode->ReferencedContentItem)
                                    {
                                        /* refCursor should now point to the reference target (refNodeID > 0) */
                                        const DSRDocumentTreeNode *parentNode = OFstatic_cast(const DSRDocumentTreeNode *, cursor.getParentNode());
                                        DSRDocumentTreeNode *targetNode = OFstatic_cast(DSRDocumentTreeNode *, refCursor.getNode());
                                        if ((parentNode != NULL) && (targetNode != NULL))
                                        {
                                            /* specify that this content item is target of an by-reference relationship */
                                            targetNode->setReferenceTarget();
                                            /* do we really need to check the constraints? */
                                            E_RelationshipType relationshipType = refNode->getRelationshipType();
                                            if (!(flags & RF_ignoreRelationshipConstraints) &&
                                                (!(flags & RF_acceptUnknownRelationshipType) || (relationshipType != RT_unknown)))
                                            {
                                                /* check whether relationship is valid */
                                                if ((ConstraintChecker != NULL) && !ConstraintChecker->checkContentRelationship(parentNode->getValueType(),
                                                    relationshipType, targetNode->getValueType(), OFTrue /*byReference*/))
                                                {
                                                    DCMSR_WARN("Invalid by-reference relationship between item \"" << posString
                                                        << "\" and \"" << refNode->ReferencedContentItem << "\"");
                                                }
                                            }
                                        } else
                                            DCMSR_WARN("Corrupted data structures while checking by-reference relationships");
                                    } else
                                        DCMSR_WARN("By-reference relationship to ancestor content item (loop check)");
                                } else
                                    DCMSR_WARN("Source and target content item of by-reference relationship are identical");
                            } else {
                                if (mode & CM_updateNodeID)
                                {
                                    DCMSR_WARN("Target content item of by-reference relationship ("
                                        << refNode->ReferencedContentItem << ") does not exist");
                                } else
                                    DCMSR_WARN("Target content item of by-reference relationship does not exist");
                            }
                        }
                    } else
                        result = SR_EC_InvalidDocumentTree;
                } while (result.good() && cursor.iterate());
            }
        }
    }
    return result;
}


void DSRDocumentTree::resetReferenceTargetFlag()
{
    DSRTreeNodeCursor cursor(getRoot());
    if (cursor.isValid())
    {
        DSRDocumentTreeNode *node = NULL;
        /* iterate over all nodes */
        do {
            node = OFstatic_cast(DSRDocumentTreeNode *, cursor.getNode());
            if (node != NULL)
                node->setReferenceTarget(OFFalse);
        } while (cursor.iterate());
    }
}
