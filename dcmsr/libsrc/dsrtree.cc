/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRTree
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:42 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
                    ++Position;
                    break;
                case AM_beforeCurrent:
                    node->Prev = NodeCursor->Prev;
                    node->Next = NodeCursor;
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


/*
 *  CVS/RCS Log:
 *  $Log: dsrtree.cc,v $
 *  Revision 1.8  2010-10-14 13:14:42  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.7  2005-12-08 15:48:18  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.6  2003/08/07 14:11:57  joergr
 *  Added comment.
 *
 *  Revision 1.5  2001/03/28 09:07:42  joergr
 *  Fixed bug in cycle/loop detection "algorithm".
 *
 *  Revision 1.4  2001/02/16 16:58:50  joergr
 *  Fixed another small bug in method removeNode().
 *
 *  Revision 1.3  2001/01/25 18:46:37  joergr
 *  Fixed bug in method removeNode().
 *
 *  Revision 1.2  2000/11/07 18:33:32  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.1  2000/10/13 07:52:27  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
