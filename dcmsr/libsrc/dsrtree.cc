/*
 *
 *  Copyright (C) 2000, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRTree
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:52:27 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtree.h"


size_t DSRTreeNode::IdentCounter = 1;


DSRTree::DSRTree()
  : DSRTreeNodeCursor(),
    RootNode(NULL)
{
}


DSRTree::~DSRTree()
{
    clear();
}


OFBool DSRTree::isEmpty() const
{
    return (RootNode == NULL);
}


void DSRTree::clear()
{
    if (gotoRoot())
        removeNode();
}


size_t DSRTree::gotoRoot()
{
    size_t nodeID = 0;
    NodeCursor = RootNode;
    clearNodeCursorStack();
    if (NodeCursor != NULL)
        nodeID = NodeCursor->Ident;
    return nodeID;
}


size_t DSRTree::gotoNode(const size_t searchID,
                         const OFBool startFromRoot)
{
    size_t nodeID = 0;
    if (searchID > 0)
    {
        if (startFromRoot)
            gotoRoot();
        nodeID = DSRTreeNodeCursor::gotoNode(searchID);
    }
    return nodeID;
}


size_t DSRTree::gotoNode(const OFString &reference,
                         const OFBool startFromRoot)
{
    size_t nodeID = 0;
    if (reference.length() > 0)
    {
        if (startFromRoot)
            gotoRoot();
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
            switch (addMode)
            {
                case AM_afterCurrent:
                    node->Prev = NodeCursor;
                    node->Next = NodeCursor->Next;
                    NodeCursor->Next = node;
                    break;
                case AM_beforeCurrent:
                    node->Prev = NodeCursor->Prev;
                    node->Next = NodeCursor;
                    NodeCursor->Prev = node;
                    break;
                case AM_belowCurrent:
                    node->Down = NodeCursor->Down;
                    NodeCursor->Down = node;
                    NodeCursorStack.push(NodeCursor);
                    break;
            }
            NodeCursor = node;
        } else {
            RootNode = NodeCursor = node;
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
        
        if ((cursor->Prev != NULL) || (cursor->Next != NULL))
        {
            if (cursor->Prev != NULL)
            {
                (cursor->Prev)->Next = cursor->Next;
                NodeCursor = cursor->Prev;
            }
            if (cursor->Next != NULL)
            {
                (cursor->Next)->Prev = cursor->Prev;
                if (NodeCursor == RootNode)
                    RootNode = cursor->Next;        // old root node deleted
                NodeCursor = cursor->Next;
            }
        } else {
            if (!NodeCursorStack.empty())
            {
                NodeCursor = NodeCursorStack.top();
                NodeCursorStack.pop();
                if (NodeCursor != NULL)
                    NodeCursor->Down = NULL;
                else
                    RootNode = NULL;                // tree is now empty
            } else {
                RootNode = NodeCursor = NULL;       // tree is now empty
            }
        }

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


/*
 *  CVS/RCS Log:
 *  $Log: dsrtree.cc,v $
 *  Revision 1.1  2000-10-13 07:52:27  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
