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
 *    classes: DSRTreeNodeCursor
 *
 */


#ifndef DSRTNCSR_H
#define DSRTNCSR_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstack.h"
#include "dcmtk/ofstd/oflist.h"

#include "dcmtk/dcmsr/dsdefine.h"


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

class DSRTreeNode;


/*-------------------*
 *  type definition  *
 *-------------------*/

// this typedef is needed to avoid a warning on Sun CC 2.0.1
typedef DSRTreeNode *DSRTreeNodePointer;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class implementing a tree node cursor
 */
class DCMTK_DCMSR_EXPORT DSRTreeNodeCursor
{

  public:

    /** default constructor
     */
    DSRTreeNodeCursor();

    /** copy constructor
     ** @param  cursor  object to be copied
     */
    DSRTreeNodeCursor(const DSRTreeNodeCursor &cursor);

    /** constructor
     ** @param  node  pointer to tree node used to initialize the cursor (set to this node)
     */
    DSRTreeNodeCursor(DSRTreeNode *node);

    /** destructor
     */
    virtual ~DSRTreeNodeCursor();

    /** assignment operator
     ** @param  cursor  object to be copied
     ** @return reference to modified cursor (this object)
     */
    DSRTreeNodeCursor &operator=(const DSRTreeNodeCursor &cursor);

    /** assignment operator
     ** @param  node  node to which the cursor should be set
     ** @return reference to modified cursor (this object)
     */
    DSRTreeNodeCursor &operator=(DSRTreeNode *node);

    /** clear all member variables
     */
    virtual void clear();

    /** check whether cursor currently points to a valid node
     ** @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the current node has a parent
     ** @return OFTrue if the current node has a parent, OFFalse otherwise
     */
    OFBool hasParentNode() const;

    /** check whether the current node has at least one child
     ** @return OFTrue if the current node has at least one child, OFFalse otherwise
     */
    OFBool hasChildNode() const;

    /** check whether the current node has a preceding sibling
     ** @return OFTrue if the current node has a preceding sibling, OFFalse otherwise
     */
    OFBool hasPreviousNode() const;

    /** check whether the current node has a following sibling
     ** @return OFTrue if the current node has a following sibling, OFFalse otherwise
     */
    OFBool hasNextNode() const;

    /** get pointer to current node
     ** @return pointer to current node (might be NULL)
     */
    inline DSRTreeNode *getNode() const
    {
        return NodeCursor;
    }

    /** get pointer to parent node.
     *  Can be used to have a lookup to the parent node without changing the cursor.
     ** @return pointer to parent node (if any), NULL otherwise
     */
    const DSRTreeNode *getParentNode();

    /** get pointer to first child node.
     *  Can be used to have a lookup to the first child node without changing the cursor.
     ** @return pointer to first child node (if any), NULL otherwise
     */
    const DSRTreeNode *getChildNode() const;

    /** get pointer to previous node.
     *  Can be used to have a lookup to the previous node without changing the cursor.
     ** @return pointer to previous node (if any), NULL otherwise
     */
    const DSRTreeNode *getPreviousNode() const;

    /** get pointer to next node.
     *  Can be used to have a lookup to the next node without changing the cursor.
     ** @return pointer to next node (if any), NULL otherwise
     */
    const DSRTreeNode *getNextNode() const;

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

    /** iterate over all nodes. Starts from current position!
     ** @param  searchIntoSub  flag indicating whether to search into sub-trees
                               ("deep search") or on the current level only
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

    /** get current node ID.
     *  The node ID uniquely identifies a content item in the document tree.  Most of
     *  the navigation methods above do return this ID too.
     ** @return ID of the current node if valid, 0 otherwise
     */
    size_t getNodeID() const;

    /** get current level.
     *  The level starts with 1 for the root node, then 2 for its child nodes, etc.
     ** @return number of the current level if valid, 0 otherwise
     */
    size_t getLevel() const;

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
    const OFString &getPosition(OFString &position,
                                const char separator = '.') const;


  protected:

    /** clear the internal node cursor stack
     */
    void clearNodeCursorStack();

    /** set cursor to specified node.
     *  Clears the internal position list/stack and sets the position counter to 1.
     ** @param  node  node to which the cursor should be set
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t setCursor(DSRTreeNode *node);

    /// pointer to current node
    DSRTreeNode *NodeCursor;
    /// stack of node pointers. Used to store the cursor position of upper levels.
    OFStack<DSRTreeNodePointer> NodeCursorStack;

    /// current position within the current level
    size_t Position;
    /// list of position counters in upper levels
    OFList<size_t> PositionList;
};


#endif
