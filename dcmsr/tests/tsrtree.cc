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
    DSRTree<> tree;
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
    OFCHECK_EQUAL(tree.getLevel(), 1);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 1);
    OFCHECK_EQUAL(tree.getLevel(), 2);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 2);
    OFCHECK_EQUAL(tree.getLevel(), 2);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 4);
    OFCHECK_EQUAL(tree.getLevel(), 3);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 5);
    OFCHECK_EQUAL(tree.getLevel(), 3);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 3);
    OFCHECK_EQUAL(tree.getLevel(), 2);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 7);
    OFCHECK_EQUAL(tree.getLevel(), 2);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 8);
    OFCHECK_EQUAL(tree.getLevel(), 2);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 6);
    OFCHECK_EQUAL(tree.getLevel(), 2);
    OFCHECK_EQUAL(tree.iterate(), 0);
    /* check the goto node methods */
    OFCHECK_EQUAL(tree.gotoNode(nodeID + 3), nodeID + 3);
    OFCHECK_EQUAL(tree.gotoFirst(), nodeID + 1);
    OFCHECK_EQUAL(tree.gotoNext(), nodeID + 2);
    OFCHECK_EQUAL(tree.gotoLast(), nodeID + 6);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 8);
    /* and finally, delete them all */
    tree.clear();
    OFCHECK(tree.isEmpty());
    OFCHECK_EQUAL(tree.countNodes(), 0);
}


OFTEST(dcmsr_treeWithoutRoot)
{
    DSRTree<> tree;
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


OFTEST(dcmsr_copyTree)
{
    DSRTree<> tree;
    const size_t nodeID = tree.getNextNodeID();
    /* first, create a simple tree of 6 nodes */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 3);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 4);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 5);
    /* then, copy this tree and check the nodes */
    OFString posString;
    DSRTree<> newTree(tree);
    OFCHECK_EQUAL(newTree.getPosition(posString), "1");
    OFCHECK(!newTree.hasParentNode());
    OFCHECK(!newTree.hasPreviousNode());
    OFCHECK(!newTree.hasNextNode());
    OFCHECK(newTree.hasChildNodes());
    newTree.iterate();
    OFCHECK_EQUAL(newTree.getPosition(posString), "1.1");
    OFCHECK(newTree.hasParentNode());
    OFCHECK(!newTree.hasPreviousNode());
    OFCHECK(newTree.hasNextNode());
    OFCHECK(!newTree.hasChildNodes());
    newTree.iterate();
    OFCHECK_EQUAL(newTree.getPosition(posString), "1.2");
    OFCHECK(newTree.hasParentNode());
    OFCHECK(newTree.hasPreviousNode());
    OFCHECK(newTree.hasNextNode());
    OFCHECK(newTree.hasChildNodes());
    newTree.iterate();
    OFCHECK_EQUAL(newTree.getPosition(posString), "1.2.1");
    OFCHECK(newTree.hasParentNode());
    OFCHECK(!newTree.hasPreviousNode());
    OFCHECK(newTree.hasNextNode());
    OFCHECK(!newTree.hasChildNodes());
    newTree.iterate();
    OFCHECK_EQUAL(newTree.getPosition(posString), "1.2.2");
    OFCHECK(newTree.hasParentNode());
    OFCHECK(newTree.hasPreviousNode());
    OFCHECK(!newTree.hasNextNode());
    OFCHECK(!newTree.hasChildNodes());
    newTree.iterate();
    OFCHECK_EQUAL(newTree.getPosition(posString), "1.3");
    OFCHECK(newTree.hasParentNode());
    OFCHECK(newTree.hasPreviousNode());
    OFCHECK(!newTree.hasNextNode());
    OFCHECK(!newTree.hasChildNodes());
}


OFTEST(dcmsr_cloneSubTree_1)
{
    DSRTree<> tree;
    const size_t nodeID = tree.getNextNodeID();
    /* first, create a simple tree of 6 nodes */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 3);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 4);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 5);
    /* then, clone a subtree and check its nodes */
    tree.gotoNode(nodeID + 1);
    OFString posString;
    DSRTree<> *newTree = tree.cloneSubTree(nodeID + 2);
    if (newTree != NULL)
    {
        OFCHECK_EQUAL(newTree->countNodes(), 4);
        OFCHECK_EQUAL(newTree->getPosition(posString), "1");
        OFCHECK(!newTree->hasParentNode());
        OFCHECK(!newTree->hasPreviousNode());
        OFCHECK(newTree->hasNextNode());
        OFCHECK(!newTree->hasChildNodes());
        newTree->iterate();
        OFCHECK_EQUAL(newTree->getPosition(posString), "2");
        OFCHECK(!newTree->hasParentNode());
        OFCHECK(newTree->hasPreviousNode());
        OFCHECK(!newTree->hasNextNode());
        OFCHECK(newTree->hasChildNodes());
        newTree->iterate();
        OFCHECK_EQUAL(newTree->getPosition(posString), "2.1");
        OFCHECK(newTree->hasParentNode());
        OFCHECK(!newTree->hasPreviousNode());
        OFCHECK(newTree->hasNextNode());
        OFCHECK(!newTree->hasChildNodes());
        newTree->iterate();
        OFCHECK_EQUAL(newTree->getPosition(posString), "2.2");
        OFCHECK(newTree->hasParentNode());
        OFCHECK(newTree->hasPreviousNode());
        OFCHECK(!newTree->hasNextNode());
        OFCHECK(!newTree->hasChildNodes());
        delete newTree;
    } else
        OFCHECK_FAIL("could not create clone of subtree");
}


OFTEST(dcmsr_cloneSubTree_2)
{
    DSRTree<> tree;
    const size_t nodeID = tree.getNextNodeID();
    /* first, create a simple tree of 6 nodes */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 3);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 4);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 5);
    /* then, clone a subtree and check its nodes */
    tree.gotoNode(nodeID + 2);
    OFString posString;
    DSRTree<> *newTree = tree.cloneSubTree();
    if (newTree != NULL)
    {
        OFCHECK_EQUAL(newTree->countNodes(), 3);
        OFCHECK_EQUAL(newTree->getPosition(posString), "1");
        OFCHECK(!newTree->hasParentNode());
        OFCHECK(!newTree->hasPreviousNode());
        OFCHECK(!newTree->hasNextNode());
        OFCHECK(newTree->hasChildNodes());
        newTree->iterate();
        OFCHECK_EQUAL(newTree->getPosition(posString), "1.1");
        OFCHECK(newTree->hasParentNode());
        OFCHECK(!newTree->hasPreviousNode());
        OFCHECK(newTree->hasNextNode());
        OFCHECK(!newTree->hasChildNodes());
        newTree->iterate();
        OFCHECK_EQUAL(newTree->getPosition(posString), "1.2");
        OFCHECK(newTree->hasParentNode());
        OFCHECK(newTree->hasPreviousNode());
        OFCHECK(!newTree->hasNextNode());
        OFCHECK(!newTree->hasChildNodes());
        delete newTree;
    } else
        OFCHECK_FAIL("could not create clone of subtree");
}


OFTEST(dcmsr_extractSubTree)
{
    DSRTree<> tree;
    const size_t nodeID = tree.getNextNodeID();
    /* first, create a simple tree of 6 nodes */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 3);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 4);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 5);
    /* then, extract a subtree and check its nodes */
    tree.gotoNode(nodeID + 2);
    DSRTree<> *subTree = tree.extractSubTree();
    if (subTree != NULL)
    {
        OFCHECK_EQUAL(subTree->countNodes(), 3);
        OFCHECK_EQUAL(subTree->getNodeID(), nodeID + 2);
        OFCHECK_EQUAL(subTree->iterate(), nodeID + 4);
        OFCHECK_EQUAL(subTree->iterate(), nodeID + 5);
        delete subTree;
    } else
        OFCHECK_FAIL("could not extract subtree");
    /* and check the original tree (again) */
    OFCHECK_EQUAL(tree.countNodes(), 3);
    OFCHECK_EQUAL(tree.gotoRoot(), nodeID + 0);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 1);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 3);
}
