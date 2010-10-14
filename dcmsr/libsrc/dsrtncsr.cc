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
 *    classes: DSRTreeNodeCursor
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:42 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtncsr.h"
#include "dcmtk/dcmsr/dsrtree.h"


DSRTreeNodeCursor::DSRTreeNodeCursor()
  : NodeCursor(NULL),
    NodeCursorStack(),
    Position(0),
    PositionList()
{
}


DSRTreeNodeCursor::DSRTreeNodeCursor(const DSRTreeNodeCursor &cursor)
  : NodeCursor(cursor.NodeCursor),
    NodeCursorStack(cursor.NodeCursorStack),
    Position(cursor.Position),
    PositionList(cursor.PositionList)
{
}


DSRTreeNodeCursor::DSRTreeNodeCursor(DSRTreeNode *node)
  : NodeCursor(node),
    NodeCursorStack(),
    Position((node != NULL) ? 1 : 0),
    PositionList()
{
}


DSRTreeNodeCursor::~DSRTreeNodeCursor()
{
}


DSRTreeNodeCursor &DSRTreeNodeCursor::operator=(const DSRTreeNodeCursor &cursor)
{
    NodeCursor = cursor.NodeCursor;
    NodeCursorStack = cursor.NodeCursorStack;
    Position = cursor.Position;
    /* copy position list (operator= is private in class OFList) */
    PositionList.clear();
    const OFListConstIterator(size_t) endPos = cursor.PositionList.end();
    OFListConstIterator(size_t) iterator = cursor.PositionList.begin();
    while (iterator != endPos)
    {
        PositionList.push_back(*iterator);
        iterator++;
    }
    return *this;
}


DSRTreeNodeCursor &DSRTreeNodeCursor::operator=(DSRTreeNode *node)
{
    setCursor(node);
    return *this;
}


void DSRTreeNodeCursor::clear()
{
    NodeCursor = NULL;
    clearNodeCursorStack();
    Position = 0;
    PositionList.clear();
}


OFBool DSRTreeNodeCursor::isValid() const
{
    return (NodeCursor != NULL);
}


void DSRTreeNodeCursor::clearNodeCursorStack()
{
    while (!NodeCursorStack.empty())
        NodeCursorStack.pop();
}


const DSRTreeNode *DSRTreeNodeCursor::getParentNode()
{
    DSRTreeNode *node = NULL;
    if (!NodeCursorStack.empty())
        node = NodeCursorStack.top();
    return node;
}


const DSRTreeNode *DSRTreeNodeCursor::getNextNode() const
{
    DSRTreeNode *node = NULL;
    if (NodeCursor != NULL)
        return NodeCursor->Next;
    return node;
}


size_t DSRTreeNodeCursor::setCursor(DSRTreeNode *node)
{
    size_t nodeID = 0;
    NodeCursor = node;
    clearNodeCursorStack();
    PositionList.clear();
    if (NodeCursor != NULL)
    {
        nodeID = NodeCursor->Ident;
        Position = 1;
    } else
        Position = 0;
    return nodeID;
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
            --Position;
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
            ++Position;
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
                if (!PositionList.empty())
                {
                    Position = PositionList.back();
                    PositionList.pop_back();
                }
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
            if (Position > 0)
            {
                PositionList.push_back(Position);
                Position = 1;
            }
        }
    }
    return nodeID;
}


size_t DSRTreeNodeCursor::iterate(const OFBool searchIntoSub)
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        /* perform "deep search", if specified */
        if (searchIntoSub && (NodeCursor->Down != NULL))
        {
            NodeCursorStack.push(NodeCursor);
            NodeCursor = NodeCursor->Down;
            nodeID = NodeCursor->Ident;
            if (Position > 0)
            {
                PositionList.push_back(Position);
                Position = 1;
            }
        }
        else if (NodeCursor->Next != NULL)
        {
            NodeCursor = NodeCursor->Next;
            nodeID = NodeCursor->Ident;
            ++Position;
        }
        else if (searchIntoSub && !NodeCursorStack.empty())
        {
            do {
                if (!NodeCursorStack.empty())
                {
                    NodeCursor = NodeCursorStack.top();
                    NodeCursorStack.pop();
                    if (!PositionList.empty())
                    {
                        Position = PositionList.back();
                        PositionList.pop_back();
                    }
                } else
                    NodeCursor = NULL;
            } while ((NodeCursor != NULL) && (NodeCursor->Next == NULL));
            if (NodeCursor != NULL)
            {
                if (NodeCursor->Next != NULL)
                {
                    NodeCursor = NodeCursor->Next;
                    nodeID = NodeCursor->Ident;
                    ++Position;
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


size_t DSRTreeNodeCursor::gotoNode(const OFString &position,
                                   const char separator)
{
    size_t nodeID = 0;
    if (!position.empty())
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
                    posEnd = position.find(separator, posStart);
                    /* is last segment? */
                    if (posEnd == OFString_npos)
                        goCount = DSRTypes::stringToNumber(position.substr(posStart).c_str());
                    else {
                        goCount = DSRTypes::stringToNumber(position.substr(posStart, posEnd - posStart).c_str());
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
    size_t level = 0;
    if (NodeCursor != NULL)
        level = NodeCursorStack.size() + 1;
    return level;
}


const OFString &DSRTreeNodeCursor::getPosition(OFString &position,
                                               const char separator) const
{
    position.clear();
    if (Position > 0)
    {
        char stringBuf[20];
        const OFListConstIterator(size_t) endPos = PositionList.end();
        OFListConstIterator(size_t) iterator = PositionList.begin();
        while (iterator != endPos)
        {
            if (!position.empty())
                position += separator;
            position += DSRTypes::numberToString(*iterator, stringBuf);
            iterator++;
        }
        if (!position.empty())
            position += separator;
        position += DSRTypes::numberToString(Position, stringBuf);
    }
    return position;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtncsr.cc,v $
 *  Revision 1.10  2010-10-14 13:14:42  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.9  2005-12-08 15:48:17  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.8  2005/07/27 16:38:57  joergr
 *  Added flag to iterate() method indicating whether to perform a "deep search".
 *
 *  Revision 1.7  2003/08/07 14:11:20  joergr
 *  Renamed parameters/variables "string" to avoid name clash with STL class.
 *  Adapted for use of OFListConstIterator, needed for compiling with HAVE_STL.
 *
 *  Revision 1.6  2001/03/28 09:07:42  joergr
 *  Fixed bug in cycle/loop detection "algorithm".
 *
 *  Revision 1.5  2001/02/13 16:34:35  joergr
 *  Corrected wrong implementation of getLevel() - started from 0 instead of 1.
 *
 *  Revision 1.4  2000/11/07 18:29:45  joergr
 *  Moved some protected method to public part.
 *
 *  Revision 1.3  2000/10/26 14:35:39  joergr
 *  Generalized routine to get and search for position strings ("1.2.3").
 *
 *  Revision 1.2  2000/10/16 12:09:55  joergr
 *  Added new options: number nested items instead of indenting them, print SOP
 *  instance UID of referenced composite objects.
 *
 *  Revision 1.1  2000/10/13 07:52:27  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
