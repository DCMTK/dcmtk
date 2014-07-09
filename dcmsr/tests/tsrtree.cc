/*
 *
 *  Copyright (C) 2012-2014, OFFIS e.V.
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
 *    test program for classes DSRTree and DSRTreeNodeCursor
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmsr/dsrtree.h"


OFTEST(dcmsr_addTreeNode)
{
    DSRTree tree;
    const size_t nodeID = tree.getNextNodeID();
    OFCHECK_EQUAL(tree.countNodes(), 0);
    /* first, create a simple tree of 6 nodes and check the references */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 3);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 4);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 5);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 4);
    OFCHECK_EQUAL(tree.goUp(), nodeID + 2);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 1);
    OFCHECK_EQUAL(tree.goUp(), nodeID + 0);
    OFCHECK_EQUAL(tree.countNodes(), 6);
    /* then, insert new nodes into the tree and check the references */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 6);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_beforeCurrent), nodeID + 7);
    OFCHECK_EQUAL(tree.gotoNext(), nodeID + 6);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_beforeCurrent), nodeID + 8);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 7);
    OFCHECK_EQUAL(tree.goUp(), nodeID + 0);
    OFCHECK_EQUAL(tree.countNodes(), 9);
    /* iterate over all tree nodes ... */
    OFCHECK_EQUAL(tree.getLevel(), nodeID + 0);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 1);
    OFCHECK_EQUAL(tree.getLevel(), nodeID + 1);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 2);
    OFCHECK_EQUAL(tree.getLevel(), nodeID + 1);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 4);
    OFCHECK_EQUAL(tree.getLevel(), nodeID + 2);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 5);
    OFCHECK_EQUAL(tree.getLevel(), nodeID + 2);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 3);
    OFCHECK_EQUAL(tree.getLevel(), nodeID + 1);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 7);
    OFCHECK_EQUAL(tree.getLevel(), nodeID + 1);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 8);
    OFCHECK_EQUAL(tree.getLevel(), nodeID + 1);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 6);
    OFCHECK_EQUAL(tree.getLevel(), nodeID + 1);
    OFCHECK_EQUAL(tree.iterate(), 0);
    /* and finally, delete them all */
    tree.clear();
    OFCHECK(tree.isEmpty());
    OFCHECK_EQUAL(tree.countNodes(), 0);
}


OFTEST(dcmsr_treeWithoutRoot)
{
    DSRTree tree;
    const size_t nodeID = tree.getNextNodeID();
    OFCHECK_EQUAL(tree.countNodes(), 0);
    /* first, create a simple tree of 6 nodes (with 3 nodes on top-level!) */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 3);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 4);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 3);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 5);
    OFCHECK_EQUAL(tree.countNodes(), 6);
    /* then clear the tree; should be empty now */
    tree.clear();
    OFCHECK(tree.isEmpty());
    OFCHECK_EQUAL(tree.countNodes(), 0);
}
