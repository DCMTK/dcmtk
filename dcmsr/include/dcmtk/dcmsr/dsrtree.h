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
 *    classes: DSRTreeNode, DSRTree
 *
 */


#ifndef DSRTREE_H
#define DSRTREE_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrtncsr.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for tree nodes
 */
class DCMTK_DCMSR_EXPORT DSRTreeNode
  : protected DSRTypes
{
    // allow direct access to member variables
    friend class DSRTreeNodeCursor;
    friend class DSRTree;

  public:

    /** default constructor
     */
    DSRTreeNode()
      : Prev(NULL),
        Next(NULL),
        Down(NULL),
        Ident(IdentCounter++)   // MT-safe?
    {
    }

    /** destructor
     */
    virtual ~DSRTreeNode()
    {
    }

    /** clone this tree node.
     *  Actually, a new tree node is created since the pointer members are not needed.
     ** @return copy of this tree node
     */
    virtual DSRTreeNode *clone() const
    {
        return new DSRTreeNode;
    }


  protected:

    /// pointer to previous tree node (if any)
    DSRTreeNode *Prev;
    /// pointer to next tree node (if any)
    DSRTreeNode *Next;
    /// pointer to first child node (if any)
    DSRTreeNode *Down;

    /// unique identifier (created automatically)
    const size_t Ident;


  private:

    /// global counter used to create the unique identifiers
    static size_t IdentCounter;


 // --- declaration of copy constructor and assignment operator

    DSRTreeNode(const DSRTreeNode &);
    DSRTreeNode &operator=(const DSRTreeNode &);
};


/** Class managing a tree of nodes
 */
class DCMTK_DCMSR_EXPORT DSRTree
  : public DSRTreeNodeCursor,
    protected DSRTypes
{

  public:

    /** default constructor
     */
    DSRTree();

    /** copy constructor.
     *  Please note that the internal cursor is not copied but reset, i.e. set to the
     *  root node.
     ** @param  tree  tree to be copied
     */
    DSRTree(const DSRTree &tree);

    /** destructor
     */
    virtual ~DSRTree();

    /** clear all member variables
     */
    virtual void clear();

    /** check whether tree has any nodes
     ** @return OFTrue if tree is empty, OFFalse otherwise
     */
    OFBool isEmpty() const;

    /** count number of nodes in the tree.
     *  This method iterates over all nodes that are stored in the tree.
     *  @return number of nodes, 0 if tree is empty
     */
    size_t countNodes() const;

    /** get ID of the next node to be created.
     *  The node ID uniquely identifies a content item in the document tree.
     ** @return ID of the next node to be created (should never be 0)
     */
    size_t getNextNodeID() const;

    /** set internal cursor to root node
     ** @return ID of root node if successful, 0 otherwise
     */
    size_t gotoRoot();

    /** set internal cursor to specified node
     ** @param  searchID       ID of the node to set the cursor to
     *  @param  startFromRoot  flag indicating whether to start from the root node
     *                         or the current one
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t gotoNode(const size_t searchID,
                    const OFBool startFromRoot = OFTrue);

    /** set internal cursor to specified node
     ** @param  reference      position string of the node to set the cursor to.
     *                         (the format is e.g. "1.2.3" for the third child of the
     *                         second child of the first node - see DSRTreeNodeCursor).
     *  @param  startFromRoot  flag indicating whether to start from the root node
     *                         or the current one
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t gotoNode(const OFString &reference,
                    const OFBool startFromRoot = OFTrue);

    /** add new node to the current one.
     *  Please note that no copy of the given node is created.  Therefore, the node
     *  should be created with new() - do not use a reference to a local variable.
     *  If the node could be added successfully the cursor is set to it automatically.
     ** @param  node     pointer to the new node to be added
     *  @param  addMode  flag specifying at which position to add the new node.
     *                   Possible values: DSRTypes::AM_afterCurrent,
     *                   DSRTypes::AM_beforeCurrent, DSRTypes::AM_belowCurrent)
     ** @return ID of the new added node if successful, 0 otherwise
     */
    virtual size_t addNode(DSRTreeNode *node,
                           const E_AddMode addMode = AM_afterCurrent);

    /** remove current node from tree.
     *  Please note that not only the specified node but also all of its child nodes are
     *  removed from the tree and then deleted.  The cursor is set automatically to a new
     *  valid position.
     ** @return ID of the node which became the current one after deletion, 0 if an error
     *          occurred or the tree is now empty.
     */
    virtual size_t removeNode();

    /** clone a subtree i.e.\ a fragment of this tree.
     *  The cloning starts with the current node and ends with the given node.
     ** @param  stopAfterNodeID  ID of the node after which the cloning should stop.
     *                           By default (0), the process ends after cloning the
     *                           current node with all of its child nodes (if any).
     ** @return pointer to a copy of the specified subtree, NULL in case of error
     */
    virtual DSRTree *cloneSubTree(const size_t stopAfterNodeID = 0) const;


  protected:

    /** special copy constructor that clones a particular subtree only
     ** @param  startCursor      first node of the subtree to be copied
     *  @param  stopAfterNodeID  ID of the node after which the cloning should stop
     */
    DSRTree(const DSRTreeNodeCursor &startCursor,
            size_t stopAfterNodeID);

    /** get pointer to root node
     ** @return pointer to root node, might be NULL (empty tree)
     */
    inline DSRTreeNode *getRoot() const
    {
        return RootNode;
    }


  private:

    /// pointer to the root tree node
    DSRTreeNode *RootNode;


 // --- declaration of assignment operator

    DSRTree &operator=(const DSRTree &);
};


#endif
