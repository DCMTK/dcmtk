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
    OFCHECK_EQUAL(tree.countNodes(), 0);
    /* first, create a simple tree of 6 nodes and check the references */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), 3);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), 4);
    OFCHECK_EQUAL(tree.gotoPrevious(), 3);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), 5);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), 6);
    OFCHECK_EQUAL(tree.gotoPrevious(), 5);
    OFCHECK_EQUAL(tree.goUp(), 3);
    OFCHECK_EQUAL(tree.gotoPrevious(), 2);
    OFCHECK_EQUAL(tree.goUp(), 1);
    OFCHECK_EQUAL(tree.countNodes(), 6);
    /* then, insert new nodes into the tree and check the references */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), 7);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_beforeCurrent), 8);
    OFCHECK_EQUAL(tree.gotoNext(), 7);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_beforeCurrent), 9);
    OFCHECK_EQUAL(tree.gotoPrevious(), 8);
    OFCHECK_EQUAL(tree.goUp(), 1);
    OFCHECK_EQUAL(tree.countNodes(), 9);
    /* iterate over all tree nodes ... */
    OFCHECK_EQUAL(tree.getLevel(), 1);
    OFCHECK_EQUAL(tree.iterate(), 2);
    OFCHECK_EQUAL(tree.getLevel(), 2);
    OFCHECK_EQUAL(tree.iterate(), 3);
    OFCHECK_EQUAL(tree.getLevel(), 2);
    OFCHECK_EQUAL(tree.iterate(), 5);
    OFCHECK_EQUAL(tree.getLevel(), 3);
    OFCHECK_EQUAL(tree.iterate(), 6);
    OFCHECK_EQUAL(tree.getLevel(), 3);
    OFCHECK_EQUAL(tree.iterate(), 4);
    OFCHECK_EQUAL(tree.getLevel(), 2);
    OFCHECK_EQUAL(tree.iterate(), 8);
    OFCHECK_EQUAL(tree.getLevel(), 2);
    OFCHECK_EQUAL(tree.iterate(), 9);
    OFCHECK_EQUAL(tree.getLevel(), 2);
    OFCHECK_EQUAL(tree.iterate(), 7);
    OFCHECK_EQUAL(tree.getLevel(), 2);
    OFCHECK_EQUAL(tree.iterate(), 0);
    /* and finally, delete them all */
    tree.clear();
    OFCHECK(tree.isEmpty());
    OFCHECK_EQUAL(tree.countNodes(), 0);
}


OFTEST(dcmsr_treeWithoutRoot)
{
    DSRTree tree;
    OFCHECK_EQUAL(tree.countNodes(), 0);
    /* first, create a simple tree of 6 nodes (with 3 nodes on top-level!) */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), 10);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), 11);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), 12);
    OFCHECK_EQUAL(tree.gotoPrevious(), 11);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), 13);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), 14);
    OFCHECK_EQUAL(tree.gotoPrevious(), 13);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), 15);
    OFCHECK_EQUAL(tree.countNodes(), 6);
    /* then clear the tree; should be empty now */
    tree.clear();
    OFCHECK(tree.isEmpty());
    OFCHECK_EQUAL(tree.countNodes(), 0);
}
