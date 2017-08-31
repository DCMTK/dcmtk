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
 *    classes: DSRDocumentTreeNodeCursor
 *
 */


#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdncsr.h"
#include "dcmtk/dcmsr/dsrdoctn.h"


DSRDocumentTreeNodeCursor::DSRDocumentTreeNodeCursor()
  : DSRTreeNodeCursor<DSRDocumentTreeNode>()
{
}


DSRDocumentTreeNodeCursor::DSRDocumentTreeNodeCursor(const DSRDocumentTreeNodeCursor &cursor)
  : DSRTreeNodeCursor<DSRDocumentTreeNode>(cursor)
{
}


DSRDocumentTreeNodeCursor::DSRDocumentTreeNodeCursor(const DSRTreeNodeCursor<DSRDocumentTreeNode> &cursor)
  : DSRTreeNodeCursor<DSRDocumentTreeNode>(cursor)
{
}


DSRDocumentTreeNodeCursor::DSRDocumentTreeNodeCursor(DSRDocumentTreeNode *node,
                                                     const DSRPositionCounter *position)
  : DSRTreeNodeCursor<DSRDocumentTreeNode>(node, position)
{
}


DSRDocumentTreeNodeCursor::~DSRDocumentTreeNodeCursor()
{
}


DSRDocumentTreeNodeCursor &DSRDocumentTreeNodeCursor::operator=(const DSRDocumentTreeNodeCursor &cursor)
{
    DSRTreeNodeCursor<DSRDocumentTreeNode>::operator=(cursor);
    return *this;
}


DSRDocumentTreeNodeCursor &DSRDocumentTreeNodeCursor::operator=(DSRDocumentTreeNode *node)
{
    DSRTreeNodeCursor<DSRDocumentTreeNode>::operator=(node);
    return *this;
}


size_t DSRDocumentTreeNodeCursor::gotoMatchingNode(const DSRDocumentTreeNodeFilter &filter,
                                                   const OFBool searchIntoSub)
{
    size_t nodeID = 0;
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


size_t DSRDocumentTreeNodeCursor::gotoNextMatchingNode(const DSRDocumentTreeNodeFilter &filter,
                                                       const OFBool searchIntoSub)
{
    /* first, goto "next" node */
    size_t nodeID = iterate(searchIntoSub);
    if (nodeID > 0)
        nodeID = gotoMatchingNode(filter, searchIntoSub);
    return nodeID;
}
