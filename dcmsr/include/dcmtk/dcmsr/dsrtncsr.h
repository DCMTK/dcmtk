/*
 *
 *  Copyright (C) 2000-2018, OFFIS e.V.
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
 */
template<typename T = DSRTreeNode>
class DSRTreeNodeCursor
{

  public:

    /** default constructor
     */
    DSRTreeNodeCursor();

    /** copy constructor
     ** @param  cursor  object to be copied
     */
    DSRTreeNodeCursor(const DSRTreeNodeCursor<T> &cursor);

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
    DSRTreeNodeCursor<T> &operator=(const DSRTreeNodeCursor<T> &cursor);

    /** assignment operator.
     *  See comments on setCursor(T*) method.
     ** @param  node  node to which the cursor should be set
     ** @return reference to modified cursor (this object)
     */
    DSRTreeNodeCursor<T> &operator=(T *node);

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
    virtual size_t countChildNodes(const OFBool searchIntoSub = OFTrue) const;

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
    virtual size_t gotoFirst();

    /** goto last node on the same level (last sibling).
     *  Please note that the last node might be identical to the current node.
     ** @return ID of the last node if successful, 0 otherwise
     */
    virtual size_t gotoLast();

    /** goto previous node on the same level (preceding sibling)
     ** @return ID of the previous node if successful, 0 otherwise
     */
    virtual size_t gotoPrevious();

    /** goto next node on the same level (following sibling)
     ** @return ID of the next node if successful, 0 otherwise
     */
    virtual size_t gotoNext();

    /** goto parent node (one level up)
     ** @return ID of the parent node if successful, 0 otherwise
     */
    virtual size_t goUp();

    /** goto first child node (one level down)
     ** @return ID of the first child node if successful, 0 otherwise
     */
    virtual size_t goDown();

    /** @copydoc goUp()
     */
    virtual size_t gotoParent();

    /** @copydoc goDown()
     */
    virtual size_t gotoChild();

    /** iterate over all nodes (starting from current position!)
     ** @param  searchIntoSub  flag indicating whether to search into sub-trees
     *                         ("deep search") or on the current level only
     ** @return ID of the next node if successful, 0 otherwise
     */
    virtual size_t iterate(const OFBool searchIntoSub = OFTrue);

    /** set cursor to specified node.  Starts search from current position.
     ** @param  searchID  ID of the node to set the cursor to
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t gotoNode(const size_t searchID);

    /** set cursor to specified node.  Starts search from current position.
     ** @param  position   position string of the node to set the cursor to.
     *                     (the format is e.g. "1.2.3" for the third child of the
     *                     second child of the first node - see getPosition()).
     *  @param  separator  character used to separate the figures (default: '.')
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t gotoNode(const OFString &position,
                    const char separator = '.');

    /** set cursor to specified node.  Starts search from current position.
     ** @param  annotation  annotation of the node to set the cursor to
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t gotoNode(const DSRTreeNodeAnnotation &annotation);

    /** set cursor to specified node.  Starts search from current position.
     *  This method requires that T implements the comparison operator "not equal".
     ** @param  nodeValue  value of the node to set the cursor to
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t gotoNode(const T &nodeValue);

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

    /** fast, non-throwing swap function.
     *  The time complexity of this function is constant.
     ** @param  cursor  cursor to swap with
     */
    void swap(DSRTreeNodeCursor<T> &cursor);

    /** clear the internal node cursor stack
     */
    void clearNodeCursorStack();

    /** get cursor
     ** @return reference to cursor (this object)
     */
    inline const DSRTreeNodeCursor<T> &getCursor() const;

    /** set cursor to specified object
     ** @param  cursor  object to set this cursor to
     */
    inline void setCursor(const DSRTreeNodeCursor<T> &cursor);

    /** set cursor to specified node.
     *  Clears the internal position counter and sets the position of the current level
     *  to 1 (if the passed 'node' is valid) or 0 (if the 'node' is invalid).
     ** @param  node  node to which the cursor should be set
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t setCursor(T *node);

    /** get pointer to first child node
     ** @return pointer to first child node (if any), NULL otherwise
     */
    T *getChild() const;

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

template<typename T>
DSRTreeNodeCursor<T>::DSRTreeNodeCursor()
  : NodeCursor(NULL),
    NodeCursorStack(),
    Position()
{
}


template<typename T>
DSRTreeNodeCursor<T>::DSRTreeNodeCursor(const DSRTreeNodeCursor<T> &cursor)
  : NodeCursor(cursor.NodeCursor),
    NodeCursorStack(cursor.NodeCursorStack),
    Position(cursor.Position)
{
}


template<typename T>
DSRTreeNodeCursor<T>::DSRTreeNodeCursor(T *node,
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


template<typename T>
DSRTreeNodeCursor<T>::~DSRTreeNodeCursor()
{
}


template<typename T>
DSRTreeNodeCursor<T> &DSRTreeNodeCursor<T>::operator=(const DSRTreeNodeCursor<T> &cursor)
{
    setCursor(cursor);
    return *this;
}


template<typename T>
DSRTreeNodeCursor<T> &DSRTreeNodeCursor<T>::operator=(T *node)
{
    setCursor(node);
    return *this;
}


template<typename T>
void DSRTreeNodeCursor<T>::clear()
{
    NodeCursor = NULL;
    clearNodeCursorStack();
    Position.clear();
}


template<typename T>
OFBool DSRTreeNodeCursor<T>::isValid() const
{
    return (NodeCursor != NULL);
}


template<typename T>
size_t DSRTreeNodeCursor<T>::countChildNodes(const OFBool searchIntoSub) const
{
    size_t count = 0;
    if (NodeCursor != NULL)
    {
        /* do we have any children at all? */
        DSRTreeNodeCursor<T> cursor(NodeCursor->getDown());
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


template<typename T>
OFBool DSRTreeNodeCursor<T>::hasParentNode() const
{
    return ((NodeCursor != NULL) && !NodeCursorStack.empty());
}


template<typename T>
OFBool DSRTreeNodeCursor<T>::hasChildNodes() const
{
    return (getChildNode() != NULL);
}


template<typename T>
OFBool DSRTreeNodeCursor<T>::hasPreviousNode() const
{
    return (getPreviousNode() != NULL);
}


template<typename T>
OFBool DSRTreeNodeCursor<T>::hasNextNode() const
{
    return (getNextNode() != NULL);
}


template<typename T>
OFBool DSRTreeNodeCursor<T>::hasSiblingNodes() const
{
    return (getPreviousNode() != NULL) || (getNextNode() != NULL);
}


template<typename T>
T *DSRTreeNodeCursor<T>::getNode() const
{
    return NodeCursor;
}


template<typename T>
const T *DSRTreeNodeCursor<T>::getParentNode() const
{
    T *node = NULL;
    if (hasParentNode())
        node = NodeCursorStack.top();
    return node;
}


template<typename T>
const T *DSRTreeNodeCursor<T>::getChildNode() const
{
    T *node = NULL;
    if (NodeCursor != NULL)
        node = NodeCursor->getDown();
    return node;
}


template<typename T>
const T *DSRTreeNodeCursor<T>::getPreviousNode() const
{
    T *node = NULL;
    if (NodeCursor != NULL)
        node = NodeCursor->getPrev();
    return node;
}


template<typename T>
const T *DSRTreeNodeCursor<T>::getNextNode() const
{
    T *node = NULL;
    if (NodeCursor != NULL)
        node = NodeCursor->getNext();
    return node;
}


template<typename T>
size_t DSRTreeNodeCursor<T>::gotoFirst()
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


template<typename T>
size_t DSRTreeNodeCursor<T>::gotoLast()
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


template<typename T>
size_t DSRTreeNodeCursor<T>::gotoPrevious()
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


template<typename T>
size_t DSRTreeNodeCursor<T>::gotoNext()
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


template<typename T>
size_t DSRTreeNodeCursor<T>::goUp()
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


template<typename T>
size_t DSRTreeNodeCursor<T>::goDown()
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


template<typename T>
size_t DSRTreeNodeCursor<T>::gotoParent()
{
    return goUp();
}


template<typename T>
size_t DSRTreeNodeCursor<T>::gotoChild()
{
    return goDown();
}


template<typename T>
size_t DSRTreeNodeCursor<T>::iterate(const OFBool searchIntoSub)
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


template<typename T>
size_t DSRTreeNodeCursor<T>::gotoNode(const size_t searchID)
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


template<typename T>
size_t DSRTreeNodeCursor<T>::gotoNode(const OFString &position,
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


template<typename T>
size_t DSRTreeNodeCursor<T>::gotoNode(const DSRTreeNodeAnnotation &annotation)
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


template<typename T>
size_t DSRTreeNodeCursor<T>::gotoNode(const T &nodeValue)
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
    {
        nodeID = NodeCursor->getIdent();
        while ((nodeID > 0) && (*NodeCursor != nodeValue))
            nodeID = iterate();
    }
    return nodeID;
}


template<typename T>
size_t DSRTreeNodeCursor<T>::getNodeID() const
{
    size_t nodeID = 0;
    if (NodeCursor != NULL)
        nodeID = NodeCursor->getIdent();
    return nodeID;
}


template<typename T>
size_t DSRTreeNodeCursor<T>::getLevel() const
{
    size_t level = 0;
    if (NodeCursor != NULL)
        level = NodeCursorStack.size() + 1;
    return level;
}


template<typename T>
DSRPositionCounter &DSRTreeNodeCursor<T>::getPositionCounter()
{
    return Position;
}


template<typename T>
const OFString &DSRTreeNodeCursor<T>::getPosition(OFString &position,
                                                  const char separator) const
{
    return Position.getString(position, separator);
}


// protected methods

template<typename T>
void DSRTreeNodeCursor<T>::swap(DSRTreeNodeCursor<T> &cursor)
{
    /* swap all members */
    OFswap(NodeCursor, cursor.NodeCursor);
    OFswap(NodeCursorStack, cursor.NodeCursorStack);
    OFswap(Position, cursor.Position);
}


template<typename T>
void DSRTreeNodeCursor<T>::clearNodeCursorStack()
{
    while (!NodeCursorStack.empty())
        NodeCursorStack.pop();
}


template<typename T>
const DSRTreeNodeCursor<T> &DSRTreeNodeCursor<T>::getCursor() const
{
    return *this;
}


template<typename T>
void DSRTreeNodeCursor<T>::setCursor(const DSRTreeNodeCursor<T> &cursor)
{
    NodeCursor = cursor.NodeCursor;
    NodeCursorStack = cursor.NodeCursorStack;
    Position = cursor.Position;
}


template<typename T>
size_t DSRTreeNodeCursor<T>::setCursor(T *node)
{
    size_t nodeID = 0;
    NodeCursor = node;
    if (NodeCursor != NULL)
        nodeID = NodeCursor->getIdent();
    clearNodeCursorStack();
    Position.initialize(NodeCursor != NULL);
    return nodeID;
}


template<typename T>
T *DSRTreeNodeCursor<T>::getChild() const
{
    T *node = NULL;
    if (NodeCursor != NULL)
        node = NodeCursor->getDown();
    return node;
}


#endif
