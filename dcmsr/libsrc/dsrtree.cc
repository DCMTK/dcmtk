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
 *    classes: DSRTree
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtree.h"


// initialize tree node counter (ID)
size_t DSRTreeNode::IdentCounter = 1;


DSRTree::DSRTree()
  : DSRTreeNodeCursor(),
    RootNode(NULL)
{
}


DSRTree::DSRTree(const DSRTree &tree)
  : DSRTreeNodeCursor(),
    RootNode(NULL)
{
    if (!tree.isEmpty())
    {
        E_AddMode addMode = AM_afterCurrent;
        DSRTreeNode *newNode = NULL;
        DSRTreeNode *nodeCursor = tree.getRoot();
        OFStack<DSRTreeNode *> nodeCursorStack;
        /* perform a "deep search", just like DSRTreeNodeCursor::iterate() */
        while (nodeCursor != NULL)
        {
            /* clone current node and add it to the tree */
            if (addNode(newNode = nodeCursor->clone(), addMode) == 0)
            {
                /* failed to add node, so delete it and exit the loop */
                delete newNode;
                break;
            }
            /* then goto to the next node to be copied */
            if (nodeCursor->Down != NULL)
            {
                /* go one level down to the first child node */
                nodeCursorStack.push(nodeCursor);
                nodeCursor = nodeCursor->Down;
                addMode = AM_belowCurrent;
            }
            else if (nodeCursor->Next != NULL)
            {
                /* proceed with the following sibling */
                nodeCursor = nodeCursor->Next;
                addMode = AM_afterCurrent;
            } else {
                /* check whether there are any siblings on higher levels */
                do {
                    if (!nodeCursorStack.empty())
                    {
                        nodeCursor = nodeCursorStack.top();
                        nodeCursorStack.pop();
                        goUp();
                    } else
                        nodeCursor = NULL;
                } while ((nodeCursor != NULL) && (nodeCursor->Next == NULL));
                if (nodeCursor != NULL)
                {
                    nodeCursor = nodeCursor->Next;
                    addMode = AM_afterCurrent;
                }
            }
        }
        /* initialize the cursor */
        gotoRoot();
    }
}


DSRTree::DSRTree(const DSRTreeNodeCursor &startCursor,
                 size_t stopAfterNodeID)
  : DSRTreeNodeCursor(),
    RootNode(NULL)
{
    DSRTreeNode *nodeCursor = startCursor.getNode();
    /* since we start from a particular node, we need to check it first */
    if (nodeCursor != NULL)
    {
        E_AddMode addMode = AM_afterCurrent;
        DSRTreeNode *newNode = NULL;
        OFStack<DSRTreeNode *> nodeCursorStack;
        /* use current node if none was specified */
        if (stopAfterNodeID == 0)
            stopAfterNodeID = nodeCursor->Ident;
        /* perform a "deep search", just like DSRTreeNodeCursor::iterate() */
        while (nodeCursor != NULL)
        {
            /* clone current node and add it to the tree */
            if (addNode(newNode = nodeCursor->clone(), addMode) == 0)
            {
                /* failed to add node, so delete it and exit the loop */
                delete newNode;
                break;
            }
            /* then goto to the next node to be copied */
            if (nodeCursor->Down != NULL)
            {
                /* go one level down to the first child node */
                nodeCursorStack.push(nodeCursor);
                nodeCursor = nodeCursor->Down;
                addMode = AM_belowCurrent;
            }
            else if (nodeCursor->Next != NULL)
            {
                /* check whether the last node has been processed */
                if (nodeCursor->Ident == stopAfterNodeID)
                {
                    /* exit the loop */
                    nodeCursor = NULL;
                } else {
                    /* proceed with the following sibling */
                    nodeCursor = nodeCursor->Next;
                    addMode = AM_afterCurrent;
                }
            } else {
                /* check whether there are any siblings on higher levels */
                do {
                    if (!nodeCursorStack.empty())
                    {
                        nodeCursor = nodeCursorStack.top();
                        nodeCursorStack.pop();
                        goUp();
                        /* check whether the last node has been processed */
                        if ((nodeCursor != NULL) && (nodeCursor->Ident == stopAfterNodeID))
                        {
                            /* exit the loop */
                            nodeCursor = NULL;
                        }
                    } else
                        nodeCursor = NULL;
                } while ((nodeCursor != NULL) && (nodeCursor->Next == NULL));
                if (nodeCursor != NULL)
                {
                    nodeCursor = nodeCursor->Next;
                    addMode = AM_afterCurrent;
                }
            }
        }
        /* initialize the cursor */
        gotoRoot();
    }
}


DSRTree::~DSRTree()
{
    clear();
}


void DSRTree::clear()
{
    if (gotoRoot())
    {
        size_t nodeID = 0;
        /* there might be more than one node at top-level */
        do {
            /* so delete them all */
            nodeID = removeNode();
        } while (nodeID > 0);
    }
}


OFBool DSRTree::isEmpty() const
{
    return (RootNode == NULL);
}


size_t DSRTree::countNodes() const
{
    size_t count = 0;
    DSRTreeNodeCursor cursor(RootNode);
    if (cursor.isValid())
    {
        /* iterate over all nodes */
        do {
            ++count;
        } while (cursor.iterate());
    }
    return count;
}


size_t DSRTree::getNextNodeID() const
{
    /* current value of the counter used to create unique identifiers */
    return DSRTreeNode::IdentCounter;
}


size_t DSRTree::gotoRoot()
{
    return setCursor(RootNode);
}


size_t DSRTree::gotoNode(const size_t searchID,
                         const OFBool startFromRoot)
{
    size_t nodeID = 0;
    if (searchID > 0)
    {
        if (startFromRoot)
            gotoRoot();
        /* call the real function */
        nodeID = DSRTreeNodeCursor::gotoNode(searchID);
    }
    return nodeID;
}


size_t DSRTree::gotoNode(const OFString &reference,
                         const OFBool startFromRoot)
{
    size_t nodeID = 0;
    if (!reference.empty())
    {
        if (startFromRoot)
            gotoRoot();
        /* call the real function */
        nodeID = DSRTreeNodeCursor::gotoNode(reference);
    }
    return nodeID;
}


size_t DSRTree::addNode(DSRTreeNode *node,
                        const E_AddMode addMode)
{
    size_t nodeID = 0;
    if (node != NULL)
    {
        if (NodeCursor != NULL)
        {
            /* update references based on 'addMode' */
            switch (addMode)
            {
                case AM_afterCurrent:
                    node->Prev = NodeCursor;
                    node->Next = NodeCursor->Next;
                    /* connect to current node */
                    if (NodeCursor->Next != NULL)
                        (NodeCursor->Next)->Prev = node;
                    NodeCursor->Next = node;
                    ++Position;
                    break;
                case AM_beforeCurrent:
                    node->Prev = NodeCursor->Prev;
                    node->Next = NodeCursor;
                    /* connect to current node */
                    if ((NodeCursor->Prev != NULL) && (Position > 1))
                        (NodeCursor->Prev)->Next = node;
                    else if (!NodeCursorStack.empty() && (Position == 1))
                        NodeCursorStack.top()->Down = node;
                    NodeCursor->Prev = node;
                    break;
                case AM_belowCurrent:
                    /* store old position */
                    if (Position > 0)
                    {
                        PositionList.push_back(Position);
                        Position = 1;
                    }
                    NodeCursorStack.push(NodeCursor);
                    /* parent node has already child nodes */
                    if (NodeCursor->Down != NULL)
                    {
                        DSRTreeNode *tempNode = NodeCursor->Down;
                        /* goto last node (sibling) */
                        while (tempNode->Next != NULL)
                        {
                            tempNode = tempNode->Next;
                            ++Position;
                        }
                        tempNode->Next = node;
                        node->Prev = tempNode;
                    } else
                        NodeCursor->Down = node;
                    break;
            }
            NodeCursor = node;
        } else {
            RootNode = NodeCursor = node;
            Position = 1;
        }
        nodeID = NodeCursor->Ident;
    }
    return nodeID;
}


size_t DSRTree::removeNode()
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        DSRTreeNode *cursor = NodeCursor;

        /* extract current node (incl. subtree) from tree */

        /* are there any siblings? */
        if ((cursor->Prev != NULL) || (cursor->Next != NULL))
        {
            /* connect to previous node */
            if (cursor->Prev != NULL)
            {
                (cursor->Prev)->Next = cursor->Next;
            } else {
                /* is there any direct parent node? */
                if (!NodeCursorStack.empty())
                {
                    DSRTreeNode *parent = NodeCursorStack.top();
                    if (parent != NULL)
                        parent->Down = cursor->Next;
                }
            }
            /* connect to next node */
            if (cursor->Next != NULL)
            {
                (cursor->Next)->Prev = cursor->Prev;
                if (NodeCursor == RootNode)
                    RootNode = cursor->Next;        // old root node deleted
                NodeCursor = cursor->Next;
            } else {
                /* set cursor to previous node since there is no next node */
                NodeCursor = cursor->Prev;
                --Position;
            }
        } else {
            /* no siblings: check for child nodes */
            if (!NodeCursorStack.empty())
            {
                NodeCursor = NodeCursorStack.top();
                NodeCursorStack.pop();
                Position = PositionList.back();
                PositionList.pop_back();
                /* should never be NULL, but ... */
                if (NodeCursor != NULL)
                    NodeCursor->Down = NULL;
                else
                {
                    RootNode = NULL;                // tree is now empty
                    Position = 0;
                }
            } else {
                RootNode = NodeCursor = NULL;       // tree is now empty
                Position = 0;
                PositionList.clear();
            }
        }

        /* remove references to former siblings */
        cursor->Prev = NULL;
        cursor->Next = NULL;

        /* delete all nodes from extracted subtree */
        /* (this routine might also use the "new" DSRTreeNodeCursor class) */

        DSRTreeNode *delNode = NULL;
        OFStack<DSRTreeNode *> cursorStack;
        while (cursor != NULL)
        {
            delNode = cursor;
            if (cursor->Down != NULL)
            {
                if (cursor->Next != NULL)
                    cursorStack.push(cursor->Next);
                cursor = cursor->Down;
            } else if (cursor->Next != NULL)
                cursor = cursor->Next;
            else if (!cursorStack.empty())
            {
                cursor = cursorStack.top();
                cursorStack.pop();
            } else
                cursor = NULL;
            delete delNode;
        }

        if (NodeCursor != NULL)
            nodeID = NodeCursor->Ident;
    }
    return nodeID;
}


DSRTree *DSRTree::cloneSubTree(const size_t stopAfterNodeID) const
{
    /* create a copy of the specified subtree */
    return new DSRTree(NodeCursor, stopAfterNodeID);
}
