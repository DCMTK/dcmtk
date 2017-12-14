/*
 *
 *  Copyright (C) 2012-2017, OFFIS e.V.
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


OFTEST(dcmsr_addTreeNode_1)
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
    /* also try new "add mode" */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrentBeforeFirstChild), nodeID + 9);
    OFCHECK_EQUAL(tree.goUp(), nodeID + 0);
    OFCHECK_EQUAL(tree.countNodes(), 10);
    /* iterate over all tree nodes ... */
    OFCHECK_EQUAL(tree.getLevel(), 1);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 9);
    OFCHECK_EQUAL(tree.getLevel(), 2);
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
    OFCHECK_EQUAL(tree.gotoFirst(), nodeID + 9);
    OFCHECK_EQUAL(tree.gotoNext(), nodeID + 1);
    OFCHECK_EQUAL(tree.gotoLast(), nodeID + 6);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 8);
    /* and finally, delete them all */
    tree.clear();
    OFCHECK(tree.isEmpty());
    OFCHECK_EQUAL(tree.countNodes(), 0);
}


OFTEST(dcmsr_addTreeNode_2)
{
    DSRTree<> tree;
    const size_t nodeID = tree.getNextNodeID();
    OFCHECK_EQUAL(tree.countNodes(), 0);
    /* first, create a simple tree of 3 nodes and check the references */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.countNodes(), 3);
    /* then, create another tree with 3 node on the same level */
    DSRTree<> newTree;
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode()), nodeID + 3);
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 4);
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 5);
    OFCHECK_EQUAL(newTree.countNodes(), 3);
    /* and, insert the new tree (i.e. add its first node) into the other one */
    OFCHECK_EQUAL(tree.gotoNode(nodeID + 1), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(newTree.getAndRemoveRootNode(), DSRTypes::AM_afterCurrent), nodeID + 3);
    OFCHECK_EQUAL(tree.countNodes(), 6);
    /* check resulting tree */
    OFCHECK_EQUAL(tree.gotoRoot(), nodeID + 0);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 1);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 3);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 4);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 5);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 2);
}


OFTEST(dcmsr_addTreeNode_3)
{
    DSRTree<> tree;
    const size_t nodeID = tree.getNextNodeID();
    OFCHECK_EQUAL(tree.countNodes(), 0);
    /* first, create a simple tree of 3 nodes and check the references */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.countNodes(), 3);
    /* then, create another tree with 3 node on the same level */
    DSRTree<> newTree;
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode()), nodeID + 3);
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 4);
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 5);
    OFCHECK_EQUAL(newTree.countNodes(), 3);
    /* and, insert the new tree (i.e. add its first node) into the other one */
    OFCHECK_EQUAL(tree.gotoNode(nodeID + 1), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(newTree.getAndRemoveRootNode(), DSRTypes::AM_beforeCurrent), nodeID + 3);
    OFCHECK_EQUAL(tree.countNodes(), 6);
    /* check resulting tree */
    OFCHECK_EQUAL(tree.gotoRoot(), nodeID + 0);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 3);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 4);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 5);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 1);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 2);
}


OFTEST(dcmsr_addTreeNode_4)
{
    DSRTree<> tree;
    const size_t nodeID = tree.getNextNodeID();
    OFCHECK_EQUAL(tree.countNodes(), 0);
    /* first, create a simple tree of 3 nodes and check the references */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.countNodes(), 3);
    /* then, create another tree with 3 node on the same level */
    DSRTree<> newTree;
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode()), nodeID + 3);
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 4);
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 5);
    OFCHECK_EQUAL(newTree.countNodes(), 3);
    /* and, insert the new tree (i.e. add its first node) into the other one */
    OFCHECK_EQUAL(tree.gotoNode(nodeID + 0), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(newTree.getAndRemoveRootNode(), DSRTypes::AM_belowCurrent), nodeID + 3);
    OFCHECK_EQUAL(tree.countNodes(), 6);
    /* check resulting tree */
    OFCHECK_EQUAL(tree.gotoRoot(), nodeID + 0);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 1);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 2);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 3);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 4);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 5);
}


OFTEST(dcmsr_addTreeNode_5)
{
    DSRTree<> tree;
    const size_t nodeID = tree.getNextNodeID();
    OFCHECK_EQUAL(tree.countNodes(), 0);
    /* first, create a simple tree of 3 nodes and check the references */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.countNodes(), 3);
    /* then, create another tree with 3 node on the same level */
    DSRTree<> newTree;
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode()), nodeID + 3);
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 4);
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 5);
    OFCHECK_EQUAL(newTree.countNodes(), 3);
    /* and, insert the new tree (i.e. add its first node) into the other one */
    OFCHECK_EQUAL(tree.gotoNode(nodeID + 0), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(newTree.getAndRemoveRootNode(), DSRTypes::AM_belowCurrentBeforeFirstChild), nodeID + 3);
    OFCHECK_EQUAL(tree.countNodes(), 6);
    /* check resulting tree */
    OFCHECK_EQUAL(tree.gotoRoot(), nodeID + 0);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 3);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 4);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 5);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 1);
    OFCHECK_EQUAL(tree.iterate(), nodeID + 2);
}


OFTEST(dcmsr_addTreeNode_6)
{
    DSRTree<> tree;
    const size_t nodeID = tree.getNextNodeID();
    OFCHECK_EQUAL(tree.countNodes(), 0);
    /* first, create a simple tree of 3 nodes and check the references */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.countNodes(), 3);
    /* then, create another tree with 3 node on the same level */
    DSRTree<> newTree;
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode()), nodeID + 3);
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 4);
    OFCHECK_EQUAL(newTree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 5);
    OFCHECK_EQUAL(newTree.countNodes(), 3);
    /* and, insert the new tree before the root node of the other one */
    OFCHECK_EQUAL(tree.gotoRoot(), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(newTree.getAndRemoveRootNode(), DSRTypes::AM_beforeCurrent), nodeID + 3);
    OFCHECK_EQUAL(tree.countNodes(), 6);
    /* check resulting tree (should have a new root node now) */
    OFCHECK_EQUAL(tree.gotoRoot(), nodeID + 3);
}


OFTEST(dcmsr_replaceTreeNode)
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
    /* then, replace one of the nodes */
    OFCHECK_EQUAL(tree.gotoNode(nodeID + 2), nodeID + 2);
    OFCHECK_EQUAL(tree.replaceNode(new DSRTreeNode()), nodeID + 6);
    /* a subtree of 3 nodes has been replaced by 1 node */
    OFCHECK_EQUAL(tree.countNodes(), 4);
    /* finally, replace the root node */
    OFCHECK_EQUAL(tree.gotoRoot(), nodeID + 0);
    OFCHECK_EQUAL(tree.replaceNode(new DSRTreeNode()), nodeID + 7);
    OFCHECK_EQUAL(tree.countNodes(), 1);
}


OFTEST(dcmsr_getPosition)
{
    DSRTree<> tree;
    OFString posString;
    const size_t nodeID = tree.getNextNodeID();
    /* create a simple tree of 4 nodes and directly check the position strings */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), nodeID + 0);
    OFCHECK_EQUAL(tree.getPosition(posString), "1");
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.getPosition(posString), "1.1");
    OFCHECK_EQUAL(tree.goUp(), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.getPosition(posString), "1.2");
    OFCHECK_EQUAL(tree.goUp(), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 3);
    OFCHECK_EQUAL(tree.getPosition(posString), "1.3");
    /* also try to go to a node based on its position */
    OFCHECK_EQUAL(tree.gotoNode("1.1"), nodeID + 1);
    OFCHECK_EQUAL(tree.gotoNode("1"), nodeID + 0);
    OFCHECK_EQUAL(tree.gotoNode("1.3", OFFalse /*startFromRoot*/), nodeID + 3);
    OFCHECK_EQUAL(tree.gotoNode("2"), 0 /* not found */);
}


OFTEST(dcmsr_countChildNodes)
{
    DSRTree<> tree;
    const size_t nodeID = tree.getNextNodeID();
    OFCHECK_EQUAL(tree.countNodes(), 0);
    /* first, create a simple tree of 7 nodes (with a single root node) */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 3);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 4);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 5);
    OFCHECK_EQUAL(tree.goUp(), nodeID + 2);
    OFCHECK_EQUAL(tree.gotoNext(), nodeID + 3);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 6);
    OFCHECK_EQUAL(tree.countNodes(), 7);
    /* then, check the number of children for all level-1 and -2 nodes */
    OFCHECK_EQUAL(tree.gotoRoot(), nodeID + 0);
    OFCHECK_EQUAL(tree.countChildNodes(), 6);
    OFCHECK_EQUAL(tree.countChildNodes(OFFalse /*searchIntoSub*/), 3);
    tree.gotoNode(nodeID + 1);
    OFCHECK_EQUAL(tree.countChildNodes(), 0);
    tree.gotoNode(nodeID + 2);
    OFCHECK_EQUAL(tree.countChildNodes(), 2);
    tree.gotoNode(nodeID + 3);
    OFCHECK_EQUAL(tree.countChildNodes(), 1);
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
    /* first, create a simple tree of 6 nodes (with a single root node) */
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


OFTEST(dcmsr_assignTree)
{
    DSRTree<> tree;
    const size_t nodeID = tree.getNextNodeID();
    /* first, create a simple tree of 6 nodes (with a single root node) */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 3);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 4);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), nodeID + 5);
    OFCHECK_EQUAL(tree.countNodes(), 6);
    /* then, assign this tree to a new variable and check the nodes */
    DSRTree<> newTree = tree;
    OFCHECK_EQUAL(tree.countNodes(), 6);
    OFCHECK_EQUAL(newTree.countNodes(), 6);
}


OFTEST(dcmsr_cloneSubTree_1)
{
    DSRTree<> tree;
    const size_t nodeID = tree.getNextNodeID();
    /* first, create a simple tree of 6 nodes (with a single root node) */
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
    /* first, create a simple tree of 6 nodes (with a single root node) */
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
    /* first, create a simple tree of 6 nodes (with a single root node) */
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


OFTEST(dcmsr_gotoParentUntilRoot)
{
    DSRTree<> tree;
    const size_t rootID = tree.getNextNodeID();
    /* first, create a simple tree of 8 nodes and check the references */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode()), rootID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), rootID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), rootID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), rootID + 3);
    OFCHECK_EQUAL(tree.gotoPrevious(), rootID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), rootID + 4);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_afterCurrent), rootID + 5);
    OFCHECK_EQUAL(tree.gotoPrevious(), rootID + 4);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), rootID + 6);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), rootID + 7);
    /* then, go one level up until the root node is reached */
    size_t prevID;
    size_t nodeID = tree.getNodeID();
    do {
        prevID = nodeID;
        nodeID = tree.gotoParent();
    } while (nodeID != 0);
    /* check whether it is really the root node */
    OFCHECK_EQUAL(prevID, rootID);
    OFCHECK_EQUAL(tree.getNodeID(), rootID);
    /* try again starting at another child node */
    nodeID = rootID + 5;
    OFCHECK_EQUAL(tree.gotoNode(nodeID), nodeID);
    do {
        prevID = nodeID;
        nodeID = tree.gotoParent();
    } while (nodeID != 0);
    /* check whether it is really the root node */
    OFCHECK_EQUAL(prevID, rootID);
    OFCHECK_EQUAL(tree.getNodeID(), rootID);
}


OFTEST(dcmsr_gotoAnnotatedTreeNode)
{
    DSRTree<> tree;
    const size_t nodeID = tree.getNextNodeID();
    /* first, create a simple tree of 6 nodes (5 of them are annotated) */
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(DSRTreeNodeAnnotation("root"))), nodeID + 0);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(DSRTreeNodeAnnotation("first child")), DSRTypes::AM_belowCurrent), nodeID + 1);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(DSRTreeNodeAnnotation("second child")), DSRTypes::AM_afterCurrent), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(DSRTreeNodeAnnotation("third child")), DSRTypes::AM_afterCurrent), nodeID + 3);
    OFCHECK_EQUAL(tree.gotoPrevious(), nodeID + 2);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(), DSRTypes::AM_belowCurrent), nodeID + 4);
    OFCHECK_EQUAL(tree.addNode(new DSRTreeNode(DSRTreeNodeAnnotation("grandchild")), DSRTypes::AM_afterCurrent), nodeID + 5);
    /* check whether the annotations are correct */
    OFCHECK_EQUAL(tree.gotoNode(DSRTreeNodeAnnotation("second child")), nodeID + 2);
    OFCHECK_EQUAL(tree.gotoNode(DSRTreeNodeAnnotation("grandchild"), OFFalse /*startFromRoot*/), nodeID + 5);
    OFCHECK_EQUAL(tree.gotoNode(DSRTreeNodeAnnotation("bastard")), 0 /* not found */);
    OFCHECK_EQUAL(tree.gotoNode(DSRTreeNodeAnnotation("first child")), nodeID + 1);
    /* clear annotation of current node and try again */
    if (tree.isValid())
        tree.getNode()->clearAnnotation();
    else
        OFCHECK_FAIL("could not get current node");
    OFCHECK_EQUAL(tree.gotoNode(DSRTreeNodeAnnotation("first child")), 0 /* not found */);
    /* clear all annotations and check one again */
    tree.clearAnnotations();
    OFCHECK_EQUAL(tree.gotoNode(DSRTreeNodeAnnotation("root")), 0 /* not found */);
    OFCHECK_EQUAL(tree.gotoNode(DSRTreeNodeAnnotation("third child")), 0 /* not found */);
}
