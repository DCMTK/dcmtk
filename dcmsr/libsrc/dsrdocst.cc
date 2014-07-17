/*
 *
 *  Copyright (C) 2000-2014, OFFIS e.V.
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
 *    classes: DSRDocumentSubTree
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdocst.h"
#include "dcmtk/dcmsr/dsrcontn.h"
#include "dcmtk/dcmsr/dsrreftn.h"
#include "dcmtk/dcmsr/dsriodcc.h"


DSRDocumentSubTree::DSRDocumentSubTree()
  : DSRTree(),
    ConstraintChecker(NULL),
    CurrentContentItem()
{
    /* by default, there is no IOD constraint checker */
}


DSRDocumentSubTree::DSRDocumentSubTree(const DSRDocumentSubTree &tree)
  : DSRTree(tree),
    ConstraintChecker(NULL),
    CurrentContentItem()
{
    /* the real work is done in the base class DSRTree */
}


DSRDocumentSubTree::DSRDocumentSubTree(const DSRTreeNodeCursor &startCursor,
                                       size_t stopAfterNodeID)
  : DSRTree(startCursor, stopAfterNodeID),
    ConstraintChecker(NULL),
    CurrentContentItem()
{
    /* the real work is done in the base class DSRTree */
}


DSRDocumentSubTree::~DSRDocumentSubTree()
{
    delete ConstraintChecker;
}


DSRDocumentSubTree *DSRDocumentSubTree::clone() const
{
    return new DSRDocumentSubTree(*this);
}


void DSRDocumentSubTree::clear()
{
    DSRTree::clear();
}


OFBool DSRDocumentSubTree::isValid() const
{
    /* an empty subtree is not valid */
    return !DSRTree::isEmpty();
}


OFCondition DSRDocumentSubTree::print(STD_NAMESPACE ostream &stream,
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
                    /* use line indentation */
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


DSRContentItem &DSRDocumentSubTree::getCurrentContentItem()
{
    CurrentContentItem.setTreeNode(getNode());
    return CurrentContentItem;
}


size_t DSRDocumentSubTree::gotoNamedNode(const DSRCodedEntryValue &conceptName,
                                         const OFBool startFromRoot,
                                         const OFBool searchIntoSub)
{
    size_t nodeID = 0;
    if (conceptName.isValid())
    {
        if (startFromRoot)
            gotoRoot();
        const DSRDocumentTreeNode *node = NULL;
        /* iterate over all nodes */
        do {
            node = getNode();
            if ((node != NULL) && (node->getConceptName() == conceptName))
                nodeID = node->getNodeID();
        } while ((nodeID == 0) && iterate(searchIntoSub));
    }
    return nodeID;
}


size_t DSRDocumentSubTree::gotoNextNamedNode(const DSRCodedEntryValue &conceptName,
                                             const OFBool searchIntoSub)
{
    /* first, goto "next" node */
    size_t nodeID = iterate(searchIntoSub);
    if (nodeID > 0)
        nodeID = gotoNamedNode(conceptName, OFFalse /*startFromRoot*/, searchIntoSub);
    return nodeID;
}


OFBool DSRDocumentSubTree::canAddContentItem(const E_RelationshipType relationshipType,
                                             const E_ValueType valueType,
                                             const E_AddMode addMode)
{
    OFBool result = OFFalse;
    /* never accept invalid types */
    if ((relationshipType != RT_invalid) && (valueType != VT_invalid))
    {
        const DSRDocumentTreeNode *node = getNode();
        if (node != NULL)
        {
            /* do we have an IOD constraint checker? */
            if (ConstraintChecker != NULL)
            {
                if ((addMode == AM_beforeCurrent) || (addMode == AM_afterCurrent))
                {
                    /* check parent node */
                    node = getParentNode();
                    if (node != NULL)
                        result = ConstraintChecker->checkContentRelationship(node->getValueType(), relationshipType, valueType);
                } else
                    result = ConstraintChecker->checkContentRelationship(node->getValueType(), relationshipType, valueType);
            } else {
                /* cannot check, therefore, allow everything */
                result = OFTrue;
            }
        } else {
            /* no special rules for root node (at least in a subtree) */
            result = OFTrue;
        }
    }
    return result;
}


OFBool DSRDocumentSubTree::canAddByReferenceRelationship(const E_RelationshipType relationshipType,
                                                         const E_ValueType targetValueType)
{
    OFBool result = OFFalse;
    /* never accept invalid types */
    if ((relationshipType != RT_invalid) && (targetValueType != VT_invalid))
    {
        /* do we have an IOD constraint checker? */
        if (ConstraintChecker != NULL)
        {
            const DSRDocumentTreeNode *node = getNode();
            if (node != NULL)
                result = ConstraintChecker->checkContentRelationship(node->getValueType(), relationshipType, targetValueType, OFTrue /*byReference*/);
        } else {
            /* cannot check, therefore, allow everything */
            result = OFTrue;
        }
    }
    return result;
}


size_t DSRDocumentSubTree::addContentItem(const E_RelationshipType relationshipType,
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
            /* in case of error, free allocated memory */
            if (nodeID == 0)
            {
                delete node;
                node = NULL;
            }
        }
    }
    return nodeID;
}


OFCondition DSRDocumentSubTree::addContentItem(DSRDocumentTreeNode *node,
                                               const E_AddMode addMode,
                                               const OFBool deleteIfFail)
{
    OFCondition result = EC_Normal;
    if (node != NULL)
    {
        /* check whether new node can be added */
        if (canAddContentItem(node->getRelationshipType(), node->getValueType(), addMode))
        {
            /* check whether adding the node actually works */
            if (addNode(node, addMode) == 0)
                result = SR_EC_CannotAddContentItem;
        } else
            result = SR_EC_CannotAddContentItem;
        /* if not, delete node (if needed) */
        if (deleteIfFail && result.bad())
        {
            delete node;
            node = NULL;
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


OFCondition DSRDocumentSubTree::addContentItem(const E_RelationshipType relationshipType,
                                               const E_ValueType valueType,
                                               const DSRCodedEntryValue &conceptName)
{
    OFCondition result = EC_Normal;
    /* call the functions doing the real work */
    if (addContentItem(relationshipType, valueType, AM_afterCurrent) > 0)
    {
        /* use a more appropriate code than the one returned */
        if (getCurrentContentItem().setConceptName(conceptName).bad())
            result = SR_EC_InvalidConceptName;
    } else
        result = SR_EC_CannotAddContentItem;
    return result;
}


OFCondition DSRDocumentSubTree::addChildContentItem(const E_RelationshipType relationshipType,
                                                    const E_ValueType valueType,
                                                    const DSRCodedEntryValue &conceptName)
{
    OFCondition result = EC_Normal;
    /* call the functions doing the real work */
    if (addContentItem(relationshipType, valueType, AM_belowCurrent) > 0)
    {
        /* use a more appropriate code than the one returned */
        if (getCurrentContentItem().setConceptName(conceptName).bad())
            result = SR_EC_InvalidConceptName;
    } else
        result = SR_EC_CannotAddContentItem;
    return result;
}


size_t DSRDocumentSubTree::addByReferenceRelationship(const E_RelationshipType relationshipType,
                                                      const size_t referencedNodeID)
{
    size_t nodeID = 0;
    if (referencedNodeID > 0)
    {
        /* always start from root node */
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
                                /* in case of error, free allocated memory */
                                if (nodeID == 0)
                                {
                                    delete node;
                                    node = NULL;
                                } else {
                                    /* otherwise, go back to current node */
                                    goUp();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return nodeID;
}


size_t DSRDocumentSubTree::removeCurrentContentItem()
{
    return removeNode();
}


DSRDocumentTreeNode *DSRDocumentSubTree::cloneCurrentTreeNode() const
{
    const DSRDocumentTreeNode *node = getNode();
    /* create a copy if the current node is valid */
    return (node != NULL) ? node->clone() : NULL;
}


DSRDocumentSubTree *DSRDocumentSubTree::cloneSubTree(const size_t stopAfterNodeID) const
{
    /* create a copy of the specified subtree */
    return new DSRDocumentSubTree(NodeCursor, stopAfterNodeID);
}


// protected methods

DSRDocumentTreeNode *DSRDocumentSubTree::getRoot() const
{
    return OFstatic_cast(DSRDocumentTreeNode *, DSRTree::getRoot());
}


DSRDocumentTreeNode *DSRDocumentSubTree::getNode() const
{
    return OFstatic_cast(DSRDocumentTreeNode *, DSRTreeNodeCursor::getNode());
}


const DSRDocumentTreeNode *DSRDocumentSubTree::getParentNode()
{
    return OFstatic_cast(const DSRDocumentTreeNode *, DSRTreeNodeCursor::getParentNode());
}


const DSRDocumentTreeNode *DSRDocumentSubTree::getChildNode() const
{
    return OFstatic_cast(const DSRDocumentTreeNode *, DSRTreeNodeCursor::getChildNode());
}


const DSRDocumentTreeNode *DSRDocumentSubTree::getPreviousNode() const
{
    return OFstatic_cast(const DSRDocumentTreeNode *, DSRTreeNodeCursor::getPreviousNode());
}


const DSRDocumentTreeNode *DSRDocumentSubTree::getNextNode() const
{
    return OFstatic_cast(const DSRDocumentTreeNode *, DSRTreeNodeCursor::getNextNode());
}


size_t DSRDocumentSubTree::addNode(DSRTreeNode * /*node*/,
                                   const E_AddMode /*addMode*/)
{
    /* invalid for this class */
    return 0;
}


size_t DSRDocumentSubTree::addNode(DSRDocumentTreeNode *node,
                                   const E_AddMode addMode)
{
    /* might add a check for templates later on */
    return DSRTree::addNode(node, addMode);
}


size_t DSRDocumentSubTree::removeNode()
{
    /* might add a check for templates later on */
    return DSRTree::removeNode();
}


OFCondition DSRDocumentSubTree::checkByReferenceRelationships(const size_t mode,
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
                /* for all content items */
                do {
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


void DSRDocumentSubTree::resetReferenceTargetFlag()
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
