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
 *    classes: DSRTreeNodeCursor
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

#include "dsrtncsr.h"
#include "dsrtree.h"


DSRTreeNodeCursor::DSRTreeNodeCursor()
  : NodeCursor(NULL),
    NodeCursorStack()
{
}


DSRTreeNodeCursor::DSRTreeNodeCursor(const DSRTreeNodeCursor &cursor)
  : NodeCursor(cursor.NodeCursor),
    NodeCursorStack(cursor.NodeCursorStack)
{
}


DSRTreeNodeCursor::DSRTreeNodeCursor(DSRTreeNode *node)
  : NodeCursor(node),
    NodeCursorStack()
{
}


DSRTreeNodeCursor::~DSRTreeNodeCursor()
{
}


DSRTreeNodeCursor &DSRTreeNodeCursor::operator=(const DSRTreeNodeCursor &cursor)
{
    NodeCursor = cursor.NodeCursor;
    NodeCursorStack = cursor.NodeCursorStack;
    return *this;
}


DSRTreeNodeCursor &DSRTreeNodeCursor::operator=(DSRTreeNode *node)
{
    NodeCursor = node;
    clearNodeCursorStack();
    return *this;
}


void DSRTreeNodeCursor::clear()
{
    NodeCursor = NULL;
    clearNodeCursorStack();
}


void DSRTreeNodeCursor::clearNodeCursorStack()
{
    while (!NodeCursorStack.empty())
        NodeCursorStack.pop();
}


const DSRTreeNode *DSRTreeNodeCursor::getNextNode() const
{
    DSRTreeNode *node = NULL;
    if (NodeCursor != NULL)
        return NodeCursor->Next;
    return node;
}


size_t DSRTreeNodeCursor::gotoPrevious()
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        if (NodeCursor->Prev != NULL)
        {
            NodeCursor = NodeCursor->Prev;
            nodeID = NodeCursor->Ident;
        }
    }
    return nodeID;
}


size_t DSRTreeNodeCursor::gotoNext()
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        if (NodeCursor->Next != NULL)
        {
            NodeCursor = NodeCursor->Next;
            nodeID = NodeCursor->Ident;
        }
    }
    return nodeID;
}


size_t DSRTreeNodeCursor::goUp()
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        if (!NodeCursorStack.empty())
        {
            DSRTreeNode *cursor = NodeCursorStack.top();
            NodeCursorStack.pop();
            if (cursor != NULL)
            {
                NodeCursor = cursor;
                nodeID = NodeCursor->Ident;
            }
        }
    }
    return nodeID;
}


size_t DSRTreeNodeCursor::goDown()
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        if (NodeCursor->Down != NULL)
        {
            NodeCursorStack.push(NodeCursor);
            NodeCursor = NodeCursor->Down;
            nodeID = NodeCursor->Ident;
        }
    }
    return nodeID;
}


size_t DSRTreeNodeCursor::iterate()
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        if (NodeCursor->Down != NULL)
        {
            NodeCursorStack.push(NodeCursor);
            NodeCursor = NodeCursor->Down;
            nodeID = NodeCursor->Ident;
        } else if (NodeCursor->Next != NULL)
        {
            NodeCursor = NodeCursor->Next;
            nodeID = NodeCursor->Ident;
        }
        else if (!NodeCursorStack.empty())
        {
            do {
                if (!NodeCursorStack.empty())
                {
                    NodeCursor = NodeCursorStack.top();
                    NodeCursorStack.pop();
                } else
                    NodeCursor = NULL;
            } while ((NodeCursor != NULL) && (NodeCursor->Next == NULL));
            if (NodeCursor != NULL)
            {
                if (NodeCursor->Next != NULL)
                {
                    NodeCursor = NodeCursor->Next;
                    nodeID = NodeCursor->Ident;
                }
            }
        }
    }
    return nodeID;
}


size_t DSRTreeNodeCursor::gotoNode(const size_t searchID)
{
    size_t nodeID = 0;
    if (searchID > 0)
    {
        if (NodeCursor != NULL)
        {
            nodeID = NodeCursor->Ident;
            while ((nodeID > 0) && (nodeID != searchID))
                nodeID = iterate();
        }
    }
    return nodeID;
}


size_t DSRTreeNodeCursor::gotoNode(const OFString &reference)
{
    size_t nodeID = 0;
    if (reference.length() > 0)
    {
        if (NodeCursor != NULL)
        {
            nodeID = NodeCursor->Ident;
            size_t posStart = 0;
            size_t posEnd = 0;
            size_t goCount = 0;
            do {
                /* go down after first valid substring/segment */
                if (posStart > 0)
                    nodeID = goDown();
                /* current node still valid? */
                if (nodeID > 0)
                {
                    /* search for next separator */
                    posEnd = reference.find('.', posStart);
                    /* is last segment? */
                    if (posEnd == OFString_npos)
                        goCount = DSRTypes::stringToNumber(reference.substr(posStart).c_str());
                    else {
                        goCount = DSRTypes::stringToNumber(reference.substr(posStart, posEnd - posStart).c_str());
                        posStart = posEnd + 1;
                    }
                    /* is valid number? */
                    if (goCount > 0)
                    {
                        while ((nodeID > 0) && (goCount > 1))
                        {
                            nodeID = gotoNext();
                            goCount--;
                        }
                    } else
                        nodeID = 0;
                }
            } while ((nodeID > 0) && (posEnd != OFString_npos));
        }        
    }
    return nodeID;
}


size_t DSRTreeNodeCursor::getNodeID() const
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
        nodeID = NodeCursor->Ident;
    return nodeID;
}


size_t DSRTreeNodeCursor::getLevel() const
{
    return NodeCursorStack.size();
}


const DSRTreeNode *DSRTreeNodeCursor::getParentNode()
{
    DSRTreeNode *node = NULL;
    if (!NodeCursorStack.empty())
        node = NodeCursorStack.top();
    return node;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtncsr.cc,v $
 *  Revision 1.1  2000-10-13 07:52:27  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
