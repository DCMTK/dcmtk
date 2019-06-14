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
 *    classes: DSRDocumentSubTree
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdocst.h"
#include "dcmtk/dcmsr/dsrcontn.h"
#include "dcmtk/dcmsr/dsrreftn.h"
#include "dcmtk/dcmsr/dsrtpltn.h"
#include "dcmtk/dcmsr/dsrstpl.h"
#include "dcmtk/dcmsr/dsriodcc.h"

#include "dcmtk/dcmdata/dcvrdt.h"


DSRDocumentSubTree::DSRDocumentSubTree()
  : DSRTree<DSRDocumentTreeNode>(),
    ConstraintChecker(NULL),
    CurrentContentItem()
{
    /* by default, there is no IOD constraint checker */
}


DSRDocumentSubTree::DSRDocumentSubTree(const DSRDocumentSubTree &tree)
  : DSRTree<DSRDocumentTreeNode>(tree),
    ConstraintChecker(NULL),
    CurrentContentItem()
{
    /* the real work is done in the base class DSRTree,
     * so just update the by-reference relationships (if any) */
    checkByReferenceRelationships<DSRDocumentTreeNodeCursor>(CM_updateNodeID);
}


DSRDocumentSubTree::DSRDocumentSubTree(DSRDocumentTreeNode *rootNode)
  : DSRTree<DSRDocumentTreeNode>(rootNode),
    ConstraintChecker(NULL),
    CurrentContentItem()
{
    /* the real work is done in the base class DSRTree */
}


DSRDocumentSubTree::DSRDocumentSubTree(const DSRDocumentTreeNodeCursor &startCursor,
                                       size_t stopAfterNodeID)
  : DSRTree<DSRDocumentTreeNode>(startCursor, stopAfterNodeID),
    ConstraintChecker(NULL),
    CurrentContentItem()
{
    /* the real work is done in the base class DSRTree */
}


DSRDocumentSubTree::~DSRDocumentSubTree()
{
    delete ConstraintChecker;
}


DSRDocumentSubTree &DSRDocumentSubTree::operator=(DSRDocumentSubTree tree)
{
    /* by-value parameter serves as a temporary */
    swap(tree);
    return *this;
}


DSRDocumentSubTree *DSRDocumentSubTree::clone() const
{
    return new DSRDocumentSubTree(*this);
}


void DSRDocumentSubTree::clear()
{
    DSRTree<DSRDocumentTreeNode>::clear();
}


OFBool DSRDocumentSubTree::isValid() const
{
    /* an empty subtree is not valid */
    return !DSRTree<DSRDocumentTreeNode>::isEmpty();
}


OFBool DSRDocumentSubTree::isCursorValid() const
{
    /* check whether cursor points to an existing node */
    return DSRTreeNodeCursor<DSRDocumentTreeNode>::isValid();
}


OFBool DSRDocumentSubTree::isValidDocumentTree(const E_RelationshipType defaultRelType) const
{
    OFBool result = OFFalse;
    /* check root node */
    const DSRDocumentTreeNode *node = getRoot();
    if (node != NULL)
    {
        E_RelationshipType relationshipType;
        /* use default relationship type if "unknown" */
        relationshipType = node->getRelationshipType();
        if (relationshipType == RT_unknown)
            relationshipType = defaultRelType;
        /* make sure that there is a single root node only, with type "CONTAINER" */
        if ((relationshipType == RT_isRoot) && (node->getValueType() == VT_Container))
            result = !node->hasSiblingNodes();
    }
    return result;
}


OFBool DSRDocumentSubTree::isExpandedDocumentTree() const
{
    OFBool result = OFTrue;
    DSRDocumentTreeNodeCursor cursor(getRoot());
    if (cursor.isValid())
    {
        /* search for a single INCLUDE template node */
        do {
            result = (cursor.getNode()->getValueType() != VT_includedTemplate);
        } while (result && cursor.iterate());
    }
    return result;
}


OFBool DSRDocumentSubTree::hasTemplateIdentification() const
{
    OFBool result = OFFalse;
    /* check whether template identification is possible */
    if (canUseTemplateIdentification())
    {
        /* check whether template identification is set */
        result = getRoot()->hasTemplateIdentification();
    }
    return result;
}


OFBool DSRDocumentSubTree::canUseTemplateIdentification() const
{
    OFBool result = OFFalse;
    /* check root node */
    const DSRDocumentTreeNode *node = getRoot();
    if (node != NULL)
    {
        /* make sure that there is a single root node only, with type "CONTAINER" */
        result = (node->getValueType() == VT_Container) && !node->hasSiblingNodes();
    }
    return result;
}


OFCondition DSRDocumentSubTree::print(STD_NAMESPACE ostream &stream,
                                      const size_t flags,
                                      const DSRPositionCounter *posCounter)
{
    OFCondition result = EC_Normal;
    /* initialize cursor with root node (and optional position counter) */
    DSRIncludedTemplateNodeCursor cursor(getRoot(), posCounter);
    if (cursor.isValid())
    {
        /* check and update by-reference relationships */
        checkByReferenceRelationships<DSRIncludedTemplateNodeCursor>(CM_updatePositionString, flags & CB_maskPrintFlags);
        /* update the document tree for output (if needed) */
        updateTreeForOutput();
        OFString tmpString;
        DSRDocumentTreeNode *node;
        /* iterate over all nodes */
        do {
            node = cursor.getNode();
            /* check whether to print node ID, position, indentation at all */
            if ((node->getValueType() != VT_includedTemplate) || !(flags & PF_hideIncludedTemplateNodes))
            {
                /* print node ID (might be useful for debugging purposes) */
                if (flags & PF_printNodeID)
                    stream << "id:" << node->getNodeID() << " ";
                /* print node position */
                if (flags & PF_printItemPosition)
                {
                    DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_ITEM_POSITION)
                    stream << cursor.getPosition(tmpString) << "  ";
                } else {
                    /* use line indentation */
                    const size_t level = cursor.getPositionCounter().getLevel();
                    if (level > 0)  // valid ?
                        stream << OFString((level - 1) * 2, ' ');
                }
            }
            /* special handling for included templates */
            if (node->getValueType() == VT_includedTemplate)
            {
                /* print separate line for internal template node (if requested) */
                if (!(flags & PF_hideIncludedTemplateNodes))
                {
                    DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
                    /* print content of template node */
                    node->print(stream, flags);
                    DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_RESET)
                    stream << OFendl;
                }
                else if ((flags & PF_dontCountIncludedTemplateNodes) && !(cursor.getPositionCounter().getFlags() & PF_dontCountIncludedTemplateNodes))
                {
                    /* do not count internal "included template" nodes */
                    --cursor.getPositionCounter();
                }
            } else {
                /* print node content */
                DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
                stream << "<";
                result = node->print(stream, flags);
                DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_DELIMITER)
                stream << ">";
                /* print extended information on the node */
                if (result.good())
                    result = node->printExtended(stream, flags);
                DCMSR_PRINT_ANSI_ESCAPE_CODE(DCMSR_ANSI_ESCAPE_CODE_RESET)
                stream << OFendl;
            }
        } while (result.good() && cursor.iterate());
    }
    return result;
}


OFCondition DSRDocumentSubTree::writeXML(STD_NAMESPACE ostream &stream,
                                         const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocumentTree;
    /* check whether document tree is valid */
    if (isValid())
    {
        DSRDocumentTreeNodeCursor cursor(getRoot());
        /* start writing from root node */
        if (cursor.isValid())
        {
            /* check by-reference relationships (if applicable) */
            checkByReferenceRelationships<DSRDocumentTreeNodeCursor>(CM_resetReferenceTargetFlag);
            /* update the document tree for output (if needed) */
            updateTreeForOutput();
            /* write current node (and its siblings) */
            do {
                result = cursor.getNode()->writeXML(stream, flags);
            } while (result.good() && cursor.gotoNext());
        }
    }
    return result;
}


DSRContentItem &DSRDocumentSubTree::getCurrentContentItem()
{
    CurrentContentItem.setTreeNode(getNode());
    return CurrentContentItem;
}


const DSRDocumentTreeNode *DSRDocumentSubTree::getCurrentNode() const
{
    return getNode();
}


OFBool DSRDocumentSubTree::getCursorToRootNode(DSRDocumentTreeNodeCursor &cursor) const
{
    cursor = DSRDocumentTreeNodeCursor(getRoot());
    return cursor.isValid();
}


OFBool DSRDocumentSubTree::getCursorToRootNode(DSRIncludedTemplateNodeCursor &cursor) const
{
    cursor = DSRIncludedTemplateNodeCursor(getRoot());
    return cursor.isValid();
}


OFBool DSRDocumentSubTree::getCursorToCurrentNode(DSRDocumentTreeNodeCursor &cursor) const
{
    cursor = DSRDocumentTreeNodeCursor(getNode());
    return cursor.isValid();
}


OFBool DSRDocumentSubTree::getCursorToSubTree(DSRDocumentTreeNodeCursor &cursor) const
{
    cursor = DSRDocumentTreeNodeCursor(getChild());
    return cursor.isValid();
}


size_t DSRDocumentSubTree::countNodes(const OFBool searchIntoSubTemplates,
                                      const OFBool countIncludedTemplateNodes) const
{
    size_t count = 0;
    /* check whether special handling of included templates is needed */
    if (searchIntoSubTemplates || !countIncludedTemplateNodes)
    {
        DSRDocumentTreeNodeCursor cursor(getRoot());
        if (cursor.isValid())
        {
            /* iterate over all nodes */
            do {
                if (cursor.getNode()->getValueType() == VT_includedTemplate)
                {
                    /* special handling: count nodes of included subtree */
                    if (searchIntoSubTemplates)
                    {
                        const DSRSubTemplate *subTempl = OFstatic_cast(const DSRIncludedTemplateTreeNode *, cursor.getNode())->getValue().get();
                        if (subTempl != NULL)
                            count += subTempl->countNodes(searchIntoSubTemplates, countIncludedTemplateNodes);
                    }
                    /* also need to count "included template" node */
                    if (countIncludedTemplateNodes)
                        ++count;
                } else {
                  /* standard case */
                  ++count;
                }
            } while (cursor.iterate());
        }
    } else {
        /* call the inherited function */
        count = DSRTree<DSRDocumentTreeNode>::countNodes();
    }
    return count;
}


size_t DSRDocumentSubTree::gotoMatchingNode(const DSRDocumentTreeNodeFilter &filter,
                                            const OFBool startFromRoot,
                                            const OFBool searchIntoSub)
{
    size_t nodeID = 0;
    if (startFromRoot)
        gotoRoot();
    const DSRDocumentTreeNode *node;
    /* iterate over all nodes */
    do {
        node = getNode();
        /* and check whether it matches */
        if (filter.matches(node))
            nodeID = node->getNodeID();
    } while ((nodeID == 0) && iterate(searchIntoSub));
    return nodeID;
}


size_t DSRDocumentSubTree::gotoNextMatchingNode(const DSRDocumentTreeNodeFilter &filter,
                                                const OFBool searchIntoSub)
{
    /* first, goto "next" node */
    size_t nodeID = iterate(searchIntoSub);
    if (nodeID > 0)
        nodeID = gotoMatchingNode(filter, OFFalse /*startFromRoot*/, searchIntoSub);
    return nodeID;
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
        const DSRDocumentTreeNode *node;
        /* iterate over all nodes */
        do {
            node = getNode();
            /* and check for the desired concept name */
            if ((node != NULL) && (node->getConceptName() == conceptName))
                nodeID = node->getNodeID();
        } while ((nodeID == 0) && iterate(searchIntoSub));
    }
    return nodeID;
}


size_t DSRDocumentSubTree::gotoNamedChildNode(const DSRCodedEntryValue &conceptName)
{
    /* first, goto child node */
    size_t nodeID = gotoChild();
    if (nodeID > 0)
        nodeID = gotoNamedNode(conceptName, OFFalse /*startFromRoot*/, OFFalse /*searchIntoSub*/);
    return nodeID;
}


size_t DSRDocumentSubTree::gotoNamedNodeInSubTree(const DSRCodedEntryValue &conceptName,
                                                  const OFBool searchIntoSub)
{
    size_t nodeID = 0;
    if (hasChildNodes())
    {
        /* start from first child node */
        DSRDocumentTreeNodeCursor cursor(getChild());
        if (cursor.isValid())
        {
            const DSRDocumentTreeNode *node;
            /* iterate over all nodes in the subtree */
            do {
                node = cursor.getNode();
                /* and check for the desired concept name */
                if ((node != NULL) && (node->getConceptName() == conceptName))
                    nodeID = node->getNodeID();
            } while ((nodeID == 0) && cursor.iterate(searchIntoSub));
            /* if found, set internal cursor to this node */
            if (nodeID > 0)
                nodeID = gotoNode(nodeID, OFFalse /*startFromRoot*/);
        }
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


size_t DSRDocumentSubTree::gotoAnnotatedNode(const OFString &annotationText,
                                             const OFBool startFromRoot)
{
    /* call the inherited function */
    return gotoNode(DSRTreeNodeAnnotation(annotationText), startFromRoot);
}


size_t DSRDocumentSubTree::gotoNextAnnotatedNode(const OFString &annotationText)
{
    /* first, goto "next" node */
    size_t nodeID = iterate();
    if (nodeID > 0)
        nodeID = gotoAnnotatedNode(annotationText, OFFalse /*startFromRoot*/);
    return nodeID;
}


OFBool DSRDocumentSubTree::canAddContentItem(const E_RelationshipType relationshipType,
                                             const E_ValueType valueType,
                                             const E_AddMode addMode) const
{
    OFBool result = OFFalse;
    /* never accept invalid types */
    if ((relationshipType != RT_invalid) && (valueType != VT_invalid))
    {
        /* also do not accept internal types */
        if ((valueType != VT_byReference) && (valueType != VT_includedTemplate))
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
                }
                /* a root node can only be added to an empty tree */
                else if (relationshipType != RT_isRoot)
                {
                    /* "unknown" relationships are only allowed on top-level */
                    result = (!hasParentNode() && (addMode != AM_belowCurrent)) || (relationshipType != RT_unknown);
                }
            } else {
                /* no special rules for root node (at least in a subtree) */
                result = OFTrue;
            }
        }
    }
    return result;
}


OFBool DSRDocumentSubTree::canAddByReferenceRelationship(const E_RelationshipType relationshipType,
                                                         const E_ValueType targetValueType) const
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
            /* certain relationships are never allowed */
            result = (relationshipType != RT_isRoot) && (relationshipType != RT_unknown) && (targetValueType != VT_includedTemplate);
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
                                               const DSRCodedEntryValue &conceptName,
                                               const OFBool check)
{
    OFCondition result = EC_Normal;
    /* call the functions doing the real work */
    if (addContentItem(relationshipType, valueType, AM_afterCurrent) > 0)
    {
        if (!conceptName.isEmpty())
        {
            /* use a more appropriate error code than the one returned */
            if (getCurrentContentItem().setConceptName(conceptName, check).bad())
                result = SR_EC_InvalidConceptName;
        }
    } else
        result = SR_EC_CannotAddContentItem;
    return result;
}


OFCondition DSRDocumentSubTree::addChildContentItem(const E_RelationshipType relationshipType,
                                                    const E_ValueType valueType,
                                                    const DSRCodedEntryValue &conceptName,
                                                    const OFBool check)
{
    OFCondition result = EC_Normal;
    /* call the functions doing the real work */
    if (addContentItem(relationshipType, valueType, AM_belowCurrent) > 0)
    {
        if (!conceptName.isEmpty())
        {
            /* use a more appropriate error code than the one returned */
            if (getCurrentContentItem().setConceptName(conceptName, check).bad())
                result = SR_EC_InvalidConceptName;
        }
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
        DSRDocumentTreeNodeCursor cursor(getRoot());
        if (cursor.isValid())
        {
            /* goto specified target node (might be improved later on) */
            if (cursor.gotoNode(referencedNodeID) > 0)
            {
                OFString sourceString;
                OFString targetString;
                getPosition(sourceString);
                cursor.getPosition(targetString);
                /* check whether target node is an ancestor of source node (prevent loops) */
                if (validByReferenceRelationship(sourceString, targetString))
                {
                    const DSRDocumentTreeNode *targetNode = cursor.getNode();
                    const E_ValueType targetValueType = targetNode->getValueType();
                    /* check whether relationship is valid/allowed */
                    if (canAddByReferenceRelationship(relationshipType, targetValueType))
                    {
                        DSRDocumentTreeNode *node = new DSRByReferenceTreeNode(relationshipType, referencedNodeID, targetValueType);
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
                } else {
                    /* report a warning with some details to the debug logger */
                    DCMSR_DEBUG("Invalid by-reference relationship from content item \""
                        << sourceString << "\" to \"" << targetString << "\" (loop check)");
                }
            } else {
                /* report a warning with some details to the debug logger */
                DCMSR_DEBUG("Target content item of by-reference relationship does not exist");
            }
        }
    }
    return nodeID;
}


OFCondition DSRDocumentSubTree::updateByReferenceRelationships(const OFBool updateIncludedTemplates)
{
    OFCondition result = EC_Normal;
    /* update the position strings of by-reference relationships */
    if (updateIncludedTemplates)
        result = checkByReferenceRelationships<DSRIncludedTemplateNodeCursor>(CM_updatePositionString);
    else
        result = checkByReferenceRelationships<DSRDocumentTreeNodeCursor>(CM_updatePositionString);
    return result;
}


OFBool DSRDocumentSubTree::canInsertSubTree(const DSRDocumentSubTree *tree,
                                            const E_AddMode addMode,
                                            const E_RelationshipType defaultRelType) const
{
    OFBool result = OFFalse;
    if (tree != NULL)
    {
        if (getNode() != NULL)
        {
            /* check whether the top-level nodes of the subtree can be added */
            DSRDocumentTreeNodeCursor cursor(tree->getRoot());
            if (cursor.isValid())
            {
                DSRDocumentTreeNode *node;
                E_RelationshipType relationshipType;
                do {
                    node = cursor.getNode();
                    /* use default relationship type if "unknown" */
                    relationshipType = node->getRelationshipType();
                    if (relationshipType == RT_unknown)
                        relationshipType = defaultRelType;
                    result = canAddContentItem(relationshipType, node->getValueType(), addMode);
                    /* exit loop on first node that cannot be added */
                } while (cursor.gotoNext() && result);
            }
        } else {
            /* no special rules for root node (at least in a subtree) */
            result = OFTrue;
        }
        /* we also need to check all other nodes in the subtree */
        if (result && checkSubTreeConstraints(tree, ConstraintChecker).bad())
            result = OFFalse;
    }
    return result;
}


OFCondition DSRDocumentSubTree::insertSubTree(DSRDocumentSubTree *tree,
                                              const E_AddMode addMode,
                                              const E_RelationshipType defaultRelType,
                                              const OFBool deleteIfFail)
{
    OFCondition result = EC_Normal;
    if (tree != NULL)
    {
        /* check whether subtree can be inserted */
        if (canInsertSubTree(tree, addMode, defaultRelType))
        {
            /* replace "unknown" relationship type on top-level */
            if (defaultRelType != RT_unknown)
            {
                DSRDocumentTreeNodeCursor cursor(tree->getRoot());
                if (cursor.isValid())
                {
                    DSRDocumentTreeNode *node;
                    do {
                        node = cursor.getNode();
                        /* use default relationship type if "unknown" */
                        if (node->getRelationshipType() == RT_unknown)
                            result = node->setRelationshipType(defaultRelType);
                    } while (cursor.gotoNext() && result.good());
                    /* use a more appropriate error code than the one returned */
                    if (result == EC_IllegalParameter)
                        result = SR_EC_CannotChangeRelationshipType;
                }
            }
            /* finally, if everything worked as expected... */
            if (result.good())
            {
                /* try to add the root node of the given subtree */
                if (addNode(tree->getRoot(), addMode) > 0)
                {
                    /* "forget" reference to root node */
                    tree->getAndRemoveRootNode();
                } else
                    result = SR_EC_CannotInsertSubTree;
            }
        } else
            result = SR_EC_CannotInsertSubTree;
        /* delete given subtree (if needed) */
        if (result.good() || (deleteIfFail && result.bad()))
        {
            delete tree;
            tree = NULL;
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


DSRDocumentSubTree *DSRDocumentSubTree::extractSubTree()
{
    DSRDocumentSubTree *tree = NULL;
    /* extract current node from tree and create a new subtree object (with this root) */
    DSRDocumentTreeNode *node = extractNode();
    if (node != NULL)
        tree = new DSRDocumentSubTree(node);
    return tree;
}


size_t DSRDocumentSubTree::removeCurrentContentItem()
{
    /* remove the current node from the tree (including all child nodes) */
    return removeNode();
}


OFCondition DSRDocumentSubTree::removeSubTree(const size_t searchID)
{
    OFCondition result = EC_Normal;
    /* we cannot remove anything from an empty tree */
    if (!isEmpty())
    {
        /* goto the given content item (if specified) */
        if (searchID > 0)
        {
            if (gotoNode(searchID, OFTrue /*startFromRoot*/) == 0)
                result = SR_EC_ContentItemNotFound;
        }
        /* and, remove it, i.e. the current node, from the tree */
        if (result.good())
        {
            /* removing the root node deletes the complete subtree */
            if ((removeNode() == 0) && !isEmpty())
                result = SR_EC_CannotRemoveSubTree;
        }
    } else
        result = SR_EC_EmptyDocumentTree;
    return result;
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


OFCondition DSRDocumentSubTree::createExpandedSubTree(DSRDocumentSubTree *&tree) const
{
    OFCondition result = EC_Normal;
    if (!isEmpty())
    {
        /* first, create a clone of this tree */
        tree = clone();
        if (tree != NULL)
        {
            /* expand all "included template" nodes in the new tree */
            result = expandIncludedTemplates(tree);
            /* in case of error, free memory */
            if (result.bad())
            {
                delete tree;
                tree = NULL;
            }
        } else
            result = EC_MemoryExhausted;
    } else {
        tree = NULL;
        result = SR_EC_EmptyDocumentTree;
    }
    return result;
}


OFCondition DSRDocumentSubTree::setObservationDateTime(const OFString &observationDateTime,
                                                       const OFBool check)
{
    /* check parameter only once */
    OFCondition result = (check) ? DcmDateTime::checkStringValue(observationDateTime, "1") : EC_Normal;
    if (result.good())
    {
        DSRIncludedTemplateNodeCursor cursor(getRoot());
        if (cursor.isValid())
        {
            /* iterate over all nodes */
            do {
                /* and set the observation data/time */
                cursor.getNode()->setObservationDateTime(observationDateTime, OFFalse /*check*/);
            } while (cursor.iterate());
        }
    }
    return result;
}


OFBool DSRDocumentSubTree::compareTemplateIdentification(const OFString &templateIdentifier,
                                                         const OFString &mappingResource,
                                                         const OFString &mappingResourceUID) const
{
    OFBool result = OFFalse;
    /* check whether template identification is possible */
    if (canUseTemplateIdentification())
    {
        /* compare with template identification of root CONTAINER */
        result = getRoot()->compareTemplateIdentification(templateIdentifier, mappingResource, mappingResourceUID);
    }
    return result;
}


OFCondition DSRDocumentSubTree::getTemplateIdentification(OFString &templateIdentifier,
                                                          OFString &mappingResource) const
{
    OFCondition result = SR_EC_CannotUseTemplateIdentification;
    /* check whether template identification is possible */
    if (canUseTemplateIdentification())
    {
        /* get template identification from root CONTAINER */
        result = getRoot()->getTemplateIdentification(templateIdentifier, mappingResource);
    }
    return result;
}


OFCondition DSRDocumentSubTree::getTemplateIdentification(OFString &templateIdentifier,
                                                          OFString &mappingResource,
                                                          OFString &mappingResourceUID) const
{
    OFCondition result = SR_EC_CannotUseTemplateIdentification;
    /* check whether template identification is possible */
    if (canUseTemplateIdentification())
    {
        /* get template identification from root CONTAINER */
        result = getRoot()->getTemplateIdentification(templateIdentifier, mappingResource, mappingResourceUID);
    }
    return result;
}


OFCondition DSRDocumentSubTree::setTemplateIdentification(const OFString &templateIdentifier,
                                                          const OFString &mappingResource,
                                                          const OFString &mappingResourceUID,
                                                          const OFBool check)
{
    OFCondition result = SR_EC_CannotUseTemplateIdentification;
    /* check whether template identification is possible */
    if (canUseTemplateIdentification())
    {
        /* mark root CONTAINER with given template identification */
        result = getRoot()->setTemplateIdentification(templateIdentifier, mappingResource, mappingResourceUID, check);
    }
    return result;
}


// protected methods

void DSRDocumentSubTree::swap(DSRDocumentSubTree &tree)
{
    /* call inherited method */
    DSRTree<DSRDocumentTreeNode>::swap(tree);
    /* swap other members */
    CurrentContentItem.swap(tree.CurrentContentItem);
    OFswap(ConstraintChecker, tree.ConstraintChecker);
}


DSRDocumentTreeNode *DSRDocumentSubTree::getNode() const
{
    /* just call the inherited method */
    return DSRTree<DSRDocumentTreeNode>::getNode();
}


size_t DSRDocumentSubTree::addNode(DSRDocumentTreeNode *node,
                                   const E_AddMode addMode)
{
    /* might add further checks later on */
    return DSRTree<DSRDocumentTreeNode>::addNode(node, addMode);
}


size_t DSRDocumentSubTree::replaceNode(DSRDocumentTreeNode *node)
{
    /* might add further checks later on */
    return DSRTree<DSRDocumentTreeNode>::replaceNode(node);
}


DSRDocumentTreeNode *DSRDocumentSubTree::extractNode()
{
    /* might add further checks later on */
    return DSRTree<DSRDocumentTreeNode>::extractNode();
}


DSRDocumentTreeNode *DSRDocumentSubTree::getAndRemoveRootNode()
{
    /* might add further checks later on */
    return DSRTree<DSRDocumentTreeNode>::getAndRemoveRootNode();
}


size_t DSRDocumentSubTree::removeNode()
{
    /* might add further checks later on */
    return DSRTree<DSRDocumentTreeNode>::removeNode();
}


DSRDocumentSubTree *DSRDocumentSubTree::cloneSubTree(const DSRDocumentTreeNodeCursor &startCursor,
                                                     const size_t stopAfterNodeID)
{
    /* create a copy of the specified subtree */
    return new DSRDocumentSubTree(startCursor, stopAfterNodeID);
}


OFCondition DSRDocumentSubTree::includeTemplate(const DSRSharedSubTemplate &subTemplate,
                                                const E_AddMode addMode,
                                                const E_RelationshipType defaultRelType)
{
    OFCondition result = EC_Normal;
    /* make sure that managed object exists */
    if (subTemplate)
    {
        /* create node that stores the reference to the included template */
        DSRDocumentTreeNode *node = new DSRIncludedTemplateTreeNode(subTemplate, defaultRelType);
        if (node != NULL)
        {
            /* check whether adding the node actually works */
            if (addNode(node, addMode) == 0)
            {
                result = SR_EC_CannotAddContentItem;
                /* if not, delete node */
                delete node;
            }
        } else
            result = EC_MemoryExhausted;
    } else
        result = EC_IllegalParameter;
    return result;
}


OFCondition DSRDocumentSubTree::expandIncludedTemplates(DSRDocumentSubTree *tree) const
{
    OFCondition result = EC_Normal;
    /* make sure that the passed tree pointer is valid and the tree is not empty */
    if ((tree != NULL) && (tree->gotoRoot() > 0))
    {
        OFBool nodeDeleted;
        DSRDocumentTreeNode *node;
        /* iterate over all nodes */
        do {
            node = tree->getNode();
            nodeDeleted = OFFalse;
            if (node != NULL)
            {
                /* and expand the included templates (if any) */
                if (node->getValueType() == VT_includedTemplate)
                {
                    DSRSubTemplate *subTempl = OFstatic_cast(DSRIncludedTemplateTreeNode *, node)->getValue().get();
                    if (subTempl != NULL)
                    {
                        /* template has no content items */
                        if (subTempl->isEmpty())
                        {
                            /* just remove current node (included template) */
                            nodeDeleted = (tree->removeNode() > 0);
                        } else {
                            /* make sure that by-reference relationships are "up to date" */
                            subTempl->updateByReferenceRelationships(OFTrue /*updateIncludedTemplates*/);
                            /* clone the subtree managed by the template */
                            DSRDocumentSubTree *subTree = subTempl->cloneTree();
                            if (subTree != NULL)
                            {
                                /* check whether there are any "unknown" relationships on top level */
                                const E_RelationshipType defaultRelType = node->getRelationshipType();
                                DSRDocumentTreeNodeCursor cursor(subTree->getRoot());
                                do {
                                    DSRDocumentTreeNode *curNode = cursor.getNode();
                                    /* if so, replace them with the "default" relationship type */
                                    if ((curNode != NULL) && (curNode->getRelationshipType() == RT_unknown))
                                        curNode->setRelationshipType(defaultRelType);
                                } while (cursor.gotoNext());
                                /* replace the current node (and its children) with the cloned subtree */
                                if (tree->replaceNode(subTree->getRoot()) > 0)
                                {
                                    /* "forget" reference to root node */
                                    subTree->getAndRemoveRootNode();
                                } else
                                    result = SR_EC_CannotInsertSubTree;
                                /* free memory */
                                delete subTree;
                            } else
                                result = EC_MemoryExhausted;
                        }
                    }
                }
            } else
                result = SR_EC_InvalidDocumentTree;
        } while (result.good() && (nodeDeleted || tree->iterate()));
        /* finally, set cursor back to root node */
        if (result.good())
            tree->gotoRoot();
    } else
        result = EC_IllegalParameter;
    return result;
}


template <typename T_Cursor>
OFCondition DSRDocumentSubTree::checkByReferenceRelationships(const size_t mode,
                                                              const size_t flags)
{
    OFCondition result = EC_Normal;
    /* the update flags are mutually exclusive */
    if (!((mode & CM_updatePositionString) && (mode & CM_updateNodeID)))
    {
        /* by-reference relationships are only allowed for particular IODs */
        if ((ConstraintChecker == NULL) || ConstraintChecker->isByReferenceAllowed())
        {
            /* specify for all content items not to be the target of a by-reference relationship */
            if (mode & CM_resetReferenceTargetFlag)
                resetReferenceTargetFlag();
            /* pass flags to reference cursor (see below) */
            const DSRPositionCounter posCounter(flags);
            /* start at the root of the document tree */
            T_Cursor cursor(getRoot());
            if (cursor.isValid())
            {
                /* for all content items */
                do {
                    /* only check/update by-reference relationships */
                    if (cursor.getNode()->getValueType() == VT_byReference)
                    {
                        size_t refNodeID = 0;
                        OFString nodePosString;
                        cursor.getPosition(nodePosString);
                        /* type cast to access members of by-reference class */
                        DSRByReferenceTreeNode *byRefNode = OFstatic_cast(DSRByReferenceTreeNode *, cursor.getNode());
                        if (flags & RF_showCurrentlyProcessedItem)
                            DCMSR_INFO("Updating by-reference relationship in content item " << nodePosString);
                        /* start searching from root node, be careful with large trees! (tbd: might be improved later on) */
                        T_Cursor refCursor(getRoot(), &posCounter);
                        if (mode & CM_updateNodeID)
                        {
                            /* update node ID (based on position string) */
                            refNodeID = refCursor.gotoNode(byRefNode->getReferencedContentItem());
                            const DSRDocumentTreeNode *targetNode = (refNodeID > 0) ? refCursor.getNode() : NULL;
                            const E_ValueType targetValueType = (targetNode != NULL) ? targetNode->getValueType() : VT_invalid;
                            byRefNode->updateReference(refNodeID, targetValueType);
                        } else {
                            /* node ID is expected to be valid */
                            refNodeID = refCursor.gotoNode(byRefNode->getReferencedNodeID());
                            if (mode & CM_updatePositionString)
                            {
                                OFString refPosString;
                                /* update position string */
                                if (refNodeID > 0)
                                    refCursor.getPosition(refPosString);
                                byRefNode->updateReference(refPosString);
                            } else if (refNodeID == 0)
                                byRefNode->invalidateReference();
                        }
                        const OFString refContentItem(byRefNode->getReferencedContentItem());
                        if (refNodeID > 0)
                        {
                            /* source and target content items should not be identical */
                            if (refNodeID != cursor.getNodeID())
                            {
                                /* check whether target node is an ancestor of source node (prevent loops) */
                                if (refContentItem.empty() || validByReferenceRelationship(nodePosString, refContentItem))
                                {
                                    /* refCursor should now point to the reference target (refNodeID > 0) */
                                    const DSRDocumentTreeNode *parentNode = cursor.getParentNode();
                                    DSRDocumentTreeNode *targetNode = refCursor.getNode();
                                    if ((parentNode != NULL) && (targetNode != NULL))
                                    {
                                        /* specify that this content item is target of an by-reference relationship */
                                        targetNode->setReferenceTarget();
                                        /* do we really need to check the constraints? */
                                        E_RelationshipType relationshipType = byRefNode->getRelationshipType();
                                        if (!(flags & RF_ignoreRelationshipConstraints) &&
                                            (!(flags & RF_acceptUnknownRelationshipType) || (relationshipType != RT_unknown)))
                                        {
                                            /* check whether relationship is valid */
                                            if ((ConstraintChecker != NULL) &&
                                                !ConstraintChecker->checkContentRelationship(parentNode->getValueType(), relationshipType,
                                                                                             targetNode->getValueType(), OFTrue /*byReference*/))
                                            {
                                                if (refContentItem.empty())
                                                    DCMSR_WARN("Invalid by-reference relationship at content item \"" << nodePosString << "\"");
                                                else {
                                                    DCMSR_WARN("Invalid by-reference relationship between content item \""
                                                        << nodePosString << "\" and \"" << refContentItem << "\"");
                                                }
                                            }
                                        }
                                    } else
                                        DCMSR_WARN("Corrupted data structures while checking by-reference relationships");
                                } else {
                                    DCMSR_WARN("By-reference relationship from \"" << nodePosString << "\" to ancestor content item \""
                                        << refContentItem << "\" (loop check)");
                                }
                            } else
                                DCMSR_WARN("Source and target content item of by-reference relationship are identical");
                        } else {
                            if (refContentItem.empty())
                                DCMSR_WARN("Target content item of by-reference relationship does not exist");
                            else
                                DCMSR_WARN("Target content item \"" << refContentItem << "\" of by-reference relationship does not exist");
                        }
                    }
                } while (result.good() && cursor.iterate());
            }
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


void DSRDocumentSubTree::resetReferenceTargetFlag()
{
    DSRIncludedTemplateNodeCursor cursor(getRoot());
    if (cursor.isValid())
    {
        /* iterate over all nodes */
        do {
            /* and reset the flag */
            cursor.getNode()->setReferenceTarget(OFFalse);
        } while (cursor.iterate());
    }
}


void DSRDocumentSubTree::updateTreeForOutput()
{
    /* nothing to do for this class, might be overwritten in derived classes */
}


OFCondition DSRDocumentSubTree::checkSubTreeConstraints(const DSRDocumentSubTree *tree,
                                                        const DSRIODConstraintChecker *checker) const
{
    OFCondition result = EC_Normal;
    /* make sure that the passed tree pointer is valid */
    if (tree != NULL)
    {
        /* do we have an IOD constraint checker? */
        if (checker != NULL)
        {
            /* check whether the nodes of the subtree can be added to this tree */
            DSRDocumentTreeNodeCursor cursor(tree->getRoot());
            if (cursor.isValid())
            {
                OFBool check = OFTrue;
                do {
                    const DSRDocumentTreeNode *node = cursor.getNode();
                    const DSRDocumentTreeNode *parent = cursor.getParentNode();
                    if (parent != NULL)
                    {
                        /* is it a by-reference relationship? */
                        if (node->getValueType() == VT_byReference)
                        {
                            /* type cast to access members of by-reference class */
                            const E_ValueType targetValueType = OFstatic_cast(const DSRByReferenceTreeNode *, node)->getTargetValueType();
                            /* use 'target value type' that is stored within the node instance */
                            check = checker->checkContentRelationship(parent->getValueType(),
                                                                      node->getRelationshipType(), targetValueType,
                                                                      OFTrue /*byReference*/);
                        }
                        /* is it an included template? */
                        else if (node->getValueType() == VT_includedTemplate)
                        {
                            // tbd: need to implement a proper check for this case
                            DCMSR_TRACE("DSRDocumentSubTree::checkSubTreeConstraints() Need to check subtree of included template "
                                << "at node id:" << node->getNodeID() << " ... not yet implemented");
                        } else {
                            /* check whether the relationship with parent is allowed */
                            check = checker->checkContentRelationship(parent->getValueType(),
                                                                      node->getRelationshipType(), node->getValueType());
                        }
                    }
                    /* exit loop on first node that has a disallowed relationship */
                } while (cursor.iterate() && check);
                /* there has been at least one disallowed relationship */
                if (!check)
                    result = SR_EC_IncompatibleDocumentTree;
            }
        } else {
            /* if not, there is nothing we can do but it's no error */
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


// static functions

OFBool DSRDocumentSubTree::validByReferenceRelationship(const OFString &sourcePosition,
                                                        const OFString &targetPosition)
{
    /* check whether target node is an ancestor of source node (prevent loops) */
    OFBool result = (sourcePosition != targetPosition);
    if (result && (sourcePosition.length() > targetPosition.length()) && (sourcePosition.compare(0, targetPosition.length() + 1, targetPosition + ".") == 0))
        result = OFFalse;
    return result;
}
