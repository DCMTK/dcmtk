/*
 *
 *  Copyright (C) 2016-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
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
 *    classes: DSRIncludedTemplateNodeCursor
 *
 */


#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsritcsr.h"
#include "dcmtk/dcmsr/dsrdoctn.h"
#include "dcmtk/dcmsr/dsrtpltn.h"
#include "dcmtk/dcmsr/dsrstpl.h"


DSRIncludedTemplateNodeCursor::DSRIncludedTemplateNodeCursor()
  : DSRTreeNodeCursor<DSRDocumentTreeNode>()
{
}


DSRIncludedTemplateNodeCursor::DSRIncludedTemplateNodeCursor(const DSRIncludedTemplateNodeCursor &cursor)
  : DSRTreeNodeCursor<DSRDocumentTreeNode>(cursor)
{
}


DSRIncludedTemplateNodeCursor::DSRIncludedTemplateNodeCursor(DSRDocumentTreeNode *node,
                                                             const DSRPositionCounter *position)
  : DSRTreeNodeCursor<DSRDocumentTreeNode>(node, position)
{
}


DSRIncludedTemplateNodeCursor::~DSRIncludedTemplateNodeCursor()
{
}


DSRIncludedTemplateNodeCursor &DSRIncludedTemplateNodeCursor::operator=(const DSRIncludedTemplateNodeCursor &cursor)
{
    DSRTreeNodeCursor<DSRDocumentTreeNode>::operator=(cursor);
    return *this;
}


DSRIncludedTemplateNodeCursor &DSRIncludedTemplateNodeCursor::operator=(DSRDocumentTreeNode *node)
{
    DSRTreeNodeCursor<DSRDocumentTreeNode>::operator=(node);
    return *this;
}


const DSRDocumentTreeNode *DSRIncludedTemplateNodeCursor::getChildNode() const
{
    DSRDocumentTreeNode *node = NULL;
    if (NodeCursor != NULL)
    {
        /* check for special case "included template" */
        if (NodeCursor->getValueType() == DSRTypes::VT_includedTemplate)
        {
            DSRSubTemplate *subTempl = OFstatic_cast(DSRIncludedTemplateTreeNode *, NodeCursor)->getValue().get();
            if (subTempl != NULL)
            {
                /* get root node of referenced subtree */
                node = subTempl->getRoot();
            }
        } else {
            /* standard case */
            node = NodeCursor->getDown();
        }
    }
    return node;
}


size_t DSRIncludedTemplateNodeCursor::countChildNodes(const OFBool searchIntoSub) const
{
    size_t count = 0;
    if (NodeCursor != NULL)
    {
        /* do we have any children at all? */
        DSRDocumentTreeNodeCursor cursor(*this);
        if (cursor.goDown())
        {
            /* iterate over all child nodes */
            do {
                ++count;
            } while (cursor.iterate(searchIntoSub));
        }
    }
    return count;
}


size_t DSRIncludedTemplateNodeCursor::goDown()
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        /* check for special case "included template" */
        if (NodeCursor->getValueType() == DSRTypes::VT_includedTemplate)
        {
            DSRSubTemplate *subTempl = OFstatic_cast(DSRIncludedTemplateTreeNode *, NodeCursor)->getValue().get();
            if (subTempl != NULL)
            {
                NodeCursorStack.push(NodeCursor);
                /* go to root node of referenced subtree */
                NodeCursor = subTempl->getRoot();
                nodeID = NodeCursor->getIdent();
                Position.goDown();
            }
        }
        /* standard case */
        else if (NodeCursor->getDown() != NULL)
        {
            NodeCursorStack.push(NodeCursor);
            NodeCursor = NodeCursor->getDown();
            nodeID = NodeCursor->getIdent();
            Position.goDown();
        }
    }
    return nodeID;
}


size_t DSRIncludedTemplateNodeCursor::iterate(const OFBool searchIntoSub)
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        /* perform "deep search", if specified */
        if (searchIntoSub && hasChildNodes())
            nodeID = goDown();
        else if (NodeCursor->getNext() != NULL)
        {
            NodeCursor = NodeCursor->getNext();
            nodeID = NodeCursor->getIdent();
            /* check for special case: do not count "included template" nodes? */
            if ((NodeCursor->getValueType() != DSRTypes::VT_includedTemplate) || !(Position.getFlags() & DSRTypes::PF_dontCountIncludedTemplateNodes))
                ++Position;
        }
        else if (searchIntoSub && !NodeCursorStack.empty())
        {
            do {
                if (!NodeCursorStack.empty())
                {
                    NodeCursor = NodeCursorStack.top();
                    NodeCursorStack.pop();
                    Position.goUp();
                } else
                    NodeCursor = NULL;
            } while ((NodeCursor != NULL) && (NodeCursor->getNext() == NULL));
            if (NodeCursor != NULL)
            {
                if (NodeCursor->getNext() != NULL)
                {
                    NodeCursor = NodeCursor->getNext();
                    nodeID = NodeCursor->getIdent();
                    /* check for special case: do not count "included template" nodes? */
                    if ((NodeCursor->getValueType() != DSRTypes::VT_includedTemplate) || !(Position.getFlags() & DSRTypes::PF_dontCountIncludedTemplateNodes))
                        ++Position;
                }
            }
        }
    }
    return nodeID;
}
