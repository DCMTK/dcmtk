/*
 *
 *  Copyright (C) 2016, J. Riesmeier, Oldenburg, Germany
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
 *    classes: specialization of DSRTreeNodeCursor (= DSRIncludedTemplateNodeCursor)
 *
 */


#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdncsr.h"
#include "dcmtk/dcmsr/dsrdoctn.h"
#include "dcmtk/dcmsr/dsrtpltn.h"
#include "dcmtk/dcmsr/dsrstpl.h"


DCMTK_EXPLICIT_SPECIALIZATION
const DSRDocumentTreeNode *DSRTreeNodeCursor<DSRDocumentTreeNode, OFTrue>::getChildNode() const
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


DCMTK_EXPLICIT_SPECIALIZATION
size_t DSRTreeNodeCursor<DSRDocumentTreeNode, OFTrue>::countChildNodes(const OFBool searchIntoSub) const
{
    size_t count = 0;
    if (NodeCursor != NULL)
    {
        /* do we have any children at all? */
        DSRTreeNodeCursor<DSRDocumentTreeNode, OFTrue> cursor(*this);
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


DCMTK_EXPLICIT_SPECIALIZATION
size_t DSRTreeNodeCursor<DSRDocumentTreeNode, OFTrue>::goDown()
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


DCMTK_EXPLICIT_SPECIALIZATION
size_t DSRTreeNodeCursor<DSRDocumentTreeNode, OFTrue>::iterate(const OFBool searchIntoSub)
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


// explicit template instantiation
template class DSRTreeNodeCursor<DSRDocumentTreeNode, OFFalse>;
template class DSRTreeNodeCursor<DSRDocumentTreeNode, OFTrue>;
