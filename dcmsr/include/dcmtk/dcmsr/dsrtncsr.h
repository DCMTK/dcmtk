/*
 *
 *  Copyright (C) 2000-2016, OFFIS e.V.
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
 *    classes: DSRTreeNodeCursor
 *
 */


#ifndef DSRTNCSR_H
#define DSRTNCSR_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrposcn.h"
#include "dcmtk/dcmsr/dsrtnant.h"

#include "dcmtk/ofstd/ofstack.h"


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

class DSRTreeNode;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class implementing a tree node cursor.
 ** @tparam  T  template type used for the tree node pointers
 *  @tparam  B  boolean flag that indicates whether to treat certain tree nodes
 *              differently (depends on the particular implementation)
 */
template<typename T = DSRTreeNode, OFBool B = OFFalse>
class DSRTreeNodeCursor
{

  public:

    /** default constructor
     */
    DSRTreeNodeCursor();

    /** copy constructor
     ** @param  cursor  object to be copied
     */
    DSRTreeNodeCursor(const DSRTreeNodeCursor<T, B> &cursor);

    /** constructor.
     *  See comments on setCursor(T*) method.
     ** @param  node      pointer to tree node used to initialize the cursor
     *  @param  position  optional pointer to position counter that should be used to
     *                    initialize the internal counter
     */
    DSRTreeNodeCursor(T *node,
                      const DSRPositionCounter *position = NULL);

    /** destructor
     */
    virtual ~DSRTreeNodeCursor();

    /** assignment operator
     ** @param  cursor  object to be copied
     ** @return reference to modified cursor (this object)
     */
    DSRTreeNodeCursor<T, B> &operator=(const DSRTreeNodeCursor<T, B> &cursor);

    /** assignment operator.
     *  See comments on setCursor(T*) method.
     ** @param  node  node to which the cursor should be set
     ** @return reference to modified cursor (this object)
     */
    DSRTreeNodeCursor<T, B> &operator=(T *node);

    /** clear all member variables.
     *  The cursor becomes invalid afterwards (same state as after default construction).
     */
    virtual void clear();

    /** check whether cursor currently points to a valid node
     ** @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** count number of children of the current node.
     *  This method iterates over all children of the current node, either on all
     *  sub-levels or on the first child level only.
     ** @param  searchIntoSub  flag indicating whether to search into sub-trees
     *                         ("deep search") or on the first child level only
     ** @return number of children of the current node, 0 if none
     */
    size_t countChildNodes(const OFBool searchIntoSub = OFTrue) const;

    /** check whether the current node has a parent
     ** @return OFTrue if the current node has a parent, OFFalse otherwise
     */
    inline OFBool hasParentNode() const;

    /** check whether the current node has any children
     ** @return OFTrue if the current node has any children, OFFalse otherwise
     */
    inline OFBool hasChildNodes() const;

    /** check whether the current node has a preceding sibling
     ** @return OFTrue if the current node has a preceding sibling, OFFalse otherwise
     */
    inline OFBool hasPreviousNode() const;

    /** check whether the current node has a following sibling
     ** @return OFTrue if the current node has a following sibling, OFFalse otherwise
     */
    inline OFBool hasNextNode() const;

    /** check whether the current node has any siblings
     ** @return OFTrue if the current node has any siblings, OFFalse otherwise
     */
    inline OFBool hasSiblingNodes() const;

    /** get pointer to current node
     ** @return pointer to current node (might be NULL)
     */
    virtual T *getNode() const;

    /** get pointer to parent node.
     *  Can be used to have a lookup to the parent node without changing the cursor.
     ** @return pointer to parent node (if any), NULL otherwise
     */
    virtual const T *getParentNode() const;

    /** get pointer to first child node.
     *  Can be used to have a lookup to the first child node without changing the cursor.
     ** @return pointer to first child node (if any), NULL otherwise
     */
    virtual const T *getChildNode() const;

    /** get pointer to previous node.
     *  Can be used to have a lookup to the previous node without changing the cursor.
     ** @return pointer to previous node (if any), NULL otherwise
     */
    virtual const T *getPreviousNode() const;

    /** get pointer to next node.
     *  Can be used to have a lookup to the next node without changing the cursor.
     ** @return pointer to next node (if any), NULL otherwise
     */
    virtual const T *getNextNode() const;

    /** goto first node on the same level (first sibling).
     *  Please note that the first node might be identical to the current node.
     ** @return ID of the first node if successful, 0 otherwise
     */
    size_t gotoFirst();

    /** goto last node on the same level (last sibling).
     *  Please note that the last node might be identical to the current node.
     ** @return ID of the last node if successful, 0 otherwise
     */
    size_t gotoLast();

    /** goto previous node on the same level (preceding sibling)
     ** @return ID of the previous node if successful, 0 otherwise
     */
    size_t gotoPrevious();

    /** goto next node on the same level (following sibling)
     ** @return ID of the next node if successful, 0 otherwise
     */
    size_t gotoNext();

    /** goto parent node (one level up)
     ** @return ID of the parent node if successful, 0 otherwise
     */
    size_t goUp();

    /** goto first child node (one level down)
     ** @return ID of the first child node if successful, 0 otherwise
     */
    size_t goDown();

    /** @copydoc goUp()
     */
    inline size_t gotoParent();

    /** @copydoc goDown()
     */
    inline size_t gotoChild();

    /** iterate over all nodes. Starts from current position!
     ** @param  searchIntoSub  flag indicating whether to search into sub-trees
     *                         ("deep search") or on the current level only
     ** @return ID of the next node if successful, 0 otherwise
     */
    size_t iterate(const OFBool searchIntoSub = OFTrue);

    /** set cursor to specified node. Starts from current position!
     ** @param  searchID  ID of the node to set the cursor to
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t gotoNode(const size_t searchID);

    /** set cursor to specified node. Starts from current position!
     ** @param  position   position string of the node to set the cursor to.
     *                     (the format is e.g. "1.2.3" for the third child of the
     *                     second child of the first node - see getPosition()).
     *  @param  separator  character used to separate the figures (default: '.')
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t gotoNode(const OFString &position,
                    const char separator = '.');

    /** set cursor to specified node. Starts from current position!
     ** @param  annotation  annotation of the node to set the cursor to
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t gotoNode(const DSRTreeNodeAnnotation &annotation);

    /** get current node ID.
     *  The node ID uniquely identifies a content item in the document tree.  Most of
     *  the navigation methods above do return this ID too.
     ** @return ID of the current node if valid, 0 otherwise
     */
    inline size_t getNodeID() const;

    /** get current level.
     *  The level starts with 1 for the root node, then 2 for its child nodes, etc.
     ** @return number of the current level if valid, 0 otherwise
     */
    inline size_t getLevel() const;

    /** get reference to internal position counter.
     *  Please note that this method allows for manipulating the internal position
     *  counter, so handle with care!
     ** @return reference to internal position counter
     */
    inline DSRPositionCounter &getPositionCounter();

    /** get position string of the current node.
     *  Specifies the position of each node by means of a dot separated string of
     *  position counters.  The first figure of this string specifies the position
     *  within the first level (e.g. "1"), the second one the position in the second
     *  level (e.g. "2"), the third one the position in the third level (e.g. "3"),
     *  etc.  A position string of "1.2.3" would, therefore, point to the third child
     *  of the second child of the first node.
     ** @param  position   variable where the position string should be stored
     *  @param  separator  character used to separate the figures (default: '.')
     ** @return reference to the resulting position string (empty if invalid)
     */
    inline const OFString &getPosition(OFString &position,
                                       const char separator = '.') const;


  protected:

    /** clear the internal node cursor stack
     */
    void clearNodeCursorStack();

    /** get cursor
     ** @return reference to cursor (this object)
     */
    inline const DSRTreeNodeCursor<T, B> &getCursor() const;

    /** set cursor to specified object
     ** @param  cursor  object to set this cursor to
     */
    inline void setCursor(const DSRTreeNodeCursor<T, B> &cursor);

    /** set cursor to specified node.
     *  Clears the internal position counter and sets the position of the current level
     *  to 1 (if the passed 'node' is valid) or 0 (if the 'node' is invalid).
     ** @param  node  node to which the cursor should be set
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t setCursor(T *node);

    /// pointer to current node
    T *NodeCursor;
    /// stack of node pointers. Used to store the cursor position of upper levels.
    OFStack<T *> NodeCursorStack;

    /// counter for the current position within the current level and on upper levels
    DSRPositionCounter Position;
};


/*------------------*
 *  implementation  *
 *------------------*/

template<typename T, OFBool B>
DSRTreeNodeCursor<T, B>::DSRTreeNodeCursor()
  : NodeCursor(NULL),
    NodeCursorStack(),
    Position()
{
}


template<typename T, OFBool B>
DSRTreeNodeCursor<T, B>::DSRTreeNodeCursor(const DSRTreeNodeCursor<T, B> &cursor)
  : NodeCursor(cursor.NodeCursor),
    NodeCursorStack(cursor.NodeCursorStack),
    Position(cursor.Position)
{
}


template<typename T, OFBool B>
DSRTreeNodeCursor<T, B>::DSRTreeNodeCursor(T *node,
                                           const DSRPositionCounter *position)
  : NodeCursor(node),
    NodeCursorStack(),
    Position()
{
    /* check whether a valid position counter is given */
    if (position != NULL)
    {
        if (position->isValid())
            Position = *position;
        else
            Position.initialize(NodeCursor != NULL, position->getFlags());
    } else
        Position.initialize(NodeCursor != NULL);
}


template<typename T, OFBool B>
DSRTreeNodeCursor<T, B>::~DSRTreeNodeCursor()
{
}


template<typename T, OFBool B>
DSRTreeNodeCursor<T, B> &DSRTreeNodeCursor<T, B>::operator=(const DSRTreeNodeCursor<T, B> &cursor)
{
    setCursor(cursor);
    return *this;
}


template<typename T, OFBool B>
DSRTreeNodeCursor<T, B> &DSRTreeNodeCursor<T, B>::operator=(T *node)
{
    setCursor(node);
    return *this;
}


template<typename T, OFBool B>
void DSRTreeNodeCursor<T, B>::clear()
{
    NodeCursor = NULL;
    clearNodeCursorStack();
    Position.clear();
}


template<typename T, OFBool B>
OFBool DSRTreeNodeCursor<T, B>::isValid() const
{
    return (NodeCursor != NULL);
}


template<typename T, OFBool B>
void DSRTreeNodeCursor<T, B>::clearNodeCursorStack()
{
    while (!NodeCursorStack.empty())
        NodeCursorStack.pop();
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::countChildNodes(const OFBool searchIntoSub) const
{
    size_t count = 0;
    if (NodeCursor != NULL)
    {
        /* do we have any children at all? */
        DSRTreeNodeCursor<T, B> cursor(NodeCursor->getDown());
        if (cursor.isValid())
        {
            /* iterate over all child nodes */
            do {
                ++count;
            } while (cursor.iterate(searchIntoSub));
        }
    }
    return count;
}


template<typename T, OFBool B>
OFBool DSRTreeNodeCursor<T, B>::hasParentNode() const
{
    return ((NodeCursor != NULL) && !NodeCursorStack.empty());
}


template<typename T, OFBool B>
OFBool DSRTreeNodeCursor<T, B>::hasChildNodes() const
{
    return (getChildNode() != NULL);
}


template<typename T, OFBool B>
OFBool DSRTreeNodeCursor<T, B>::hasPreviousNode() const
{
    return (getPreviousNode() != NULL);
}


template<typename T, OFBool B>
OFBool DSRTreeNodeCursor<T, B>::hasNextNode() const
{
    return (getNextNode() != NULL);
}


template<typename T, OFBool B>
OFBool DSRTreeNodeCursor<T, B>::hasSiblingNodes() const
{
    return (getPreviousNode() != NULL) || (getNextNode() != NULL);
}


template<typename T, OFBool B>
T *DSRTreeNodeCursor<T, B>::getNode() const
{
    return NodeCursor;
}


template<typename T, OFBool B>
const T *DSRTreeNodeCursor<T, B>::getParentNode() const
{
    T *node = NULL;
    if (hasParentNode())
        node = NodeCursorStack.top();
    return node;
}


template<typename T, OFBool B>
const T *DSRTreeNodeCursor<T, B>::getChildNode() const
{
    T *node = NULL;
    if (NodeCursor != NULL)
        node = NodeCursor->getDown();
    return node;
}


template<typename T, OFBool B>
const T *DSRTreeNodeCursor<T, B>::getPreviousNode() const
{
    T *node = NULL;
    if (NodeCursor != NULL)
        node = NodeCursor->getPrev();
    return node;
}


template<typename T, OFBool B>
const T *DSRTreeNodeCursor<T, B>::getNextNode() const
{
    T *node = NULL;
    if (NodeCursor != NULL)
        node = NodeCursor->getNext();
    return node;
}


template<typename T, OFBool B>
const DSRTreeNodeCursor<T, B> &DSRTreeNodeCursor<T, B>::getCursor() const
{
    return *this;
}


template<typename T, OFBool B>
void DSRTreeNodeCursor<T, B>::setCursor(const DSRTreeNodeCursor<T, B> &cursor)
{
    NodeCursor = cursor.NodeCursor;
    NodeCursorStack = cursor.NodeCursorStack;
    Position = cursor.Position;
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::setCursor(T *node)
{
    size_t nodeID = 0;
    NodeCursor = node;
    if (NodeCursor != NULL)
        nodeID = NodeCursor->getIdent();
    clearNodeCursorStack();
    Position.initialize(NodeCursor != NULL);
    return nodeID;
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::gotoFirst()
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        while (NodeCursor->getPrev() != NULL)
        {
            NodeCursor = NodeCursor->getPrev();
            --Position;
        }
        nodeID = NodeCursor->getIdent();
    }
    return nodeID;
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::gotoLast()
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        while (NodeCursor->getNext() != NULL)
        {
            NodeCursor = NodeCursor->getNext();
            ++Position;
        }
        nodeID = NodeCursor->getIdent();
    }
    return nodeID;
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::gotoPrevious()
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        if (NodeCursor->getPrev() != NULL)
        {
            NodeCursor = NodeCursor->getPrev();
            nodeID = NodeCursor->getIdent();
            --Position;
        }
    }
    return nodeID;
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::gotoNext()
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        if (NodeCursor->getNext() != NULL)
        {
            NodeCursor = NodeCursor->getNext();
            nodeID = NodeCursor->getIdent();
            ++Position;
        }
    }
    return nodeID;
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::goUp()
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        if (!NodeCursorStack.empty())
        {
            T *cursor = NodeCursorStack.top();
            NodeCursorStack.pop();
            if (cursor != NULL)
            {
                NodeCursor = cursor;
                nodeID = NodeCursor->getIdent();
                Position.goUp();
            }
        }
    }
    return nodeID;
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::goDown()
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        if (NodeCursor->getDown() != NULL)
        {
            NodeCursorStack.push(NodeCursor);
            NodeCursor = NodeCursor->getDown();
            nodeID = NodeCursor->getIdent();
            Position.goDown();
        }
    }
    return nodeID;
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::gotoParent()
{
    return goUp();
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::gotoChild()
{
    return goDown();
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::iterate(const OFBool searchIntoSub)
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        /* perform "deep search", if specified */
        if (searchIntoSub && (NodeCursor->getDown() != NULL))
        {
            NodeCursorStack.push(NodeCursor);
            NodeCursor = NodeCursor->getDown();
            nodeID = NodeCursor->getIdent();
            Position.goDown();
        }
        else if (NodeCursor->getNext() != NULL)
        {
            NodeCursor = NodeCursor->getNext();
            nodeID = NodeCursor->getIdent();
            ++Position;
        }
        else if (searchIntoSub && !NodeCursorStack.empty())
        {
            do {
                if (!NodeCursorStack.empty())
                {
                    NodeCursor = NodeCursorStack.top();
                    NodeCursorStack.pop();
                    Position.goUp();
                } else
                    NodeCursor = NULL;
            } while ((NodeCursor != NULL) && (NodeCursor->getNext() == NULL));
            if (NodeCursor != NULL)
            {
                if (NodeCursor->getNext() != NULL)
                {
                    NodeCursor = NodeCursor->getNext();
                    nodeID = NodeCursor->getIdent();
                    ++Position;
                }
            }
        }
    }
    return nodeID;
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::gotoNode(const size_t searchID)
{
    size_t nodeID = 0;
    if (searchID > 0)
    {
        if (NodeCursor != NULL)
        {
            nodeID = NodeCursor->getIdent();
            while ((nodeID > 0) && (nodeID != searchID))
                nodeID = iterate();
        }
    }
    return nodeID;
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::gotoNode(const OFString &position,
                                         const char separator)
{
    size_t nodeID = 0;
    if (!position.empty())
    {
        if (NodeCursor != NULL)
        {
            nodeID = NodeCursor->getIdent();
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


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::gotoNode(const DSRTreeNodeAnnotation &annotation)
{
    size_t nodeID = 0;
    if (!annotation.isEmpty())
    {
        if (NodeCursor != NULL)
        {
            nodeID = NodeCursor->getIdent();
            while ((nodeID > 0) && (NodeCursor->getAnnotation() != annotation))
                nodeID = iterate();
        }
    }
    return nodeID;
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::getNodeID() const
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
        nodeID = NodeCursor->getIdent();
    return nodeID;
}


template<typename T, OFBool B>
size_t DSRTreeNodeCursor<T, B>::getLevel() const
{
    size_t level = 0;
    if (NodeCursor != NULL)
        level = NodeCursorStack.size() + 1;
    return level;
}


template<typename T, OFBool B>
DSRPositionCounter &DSRTreeNodeCursor<T, B>::getPositionCounter()
{
    return Position;
}


template<typename T, OFBool B>
const OFString &DSRTreeNodeCursor<T, B>::getPosition(OFString &position,
                                                     const char separator) const
{
    return Position.getString(position, separator);
}


#endif
