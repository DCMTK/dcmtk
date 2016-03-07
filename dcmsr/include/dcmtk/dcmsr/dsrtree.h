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
 *    classes: DSRTreeNode, DSRTree
 *
 */


#ifndef DSRTREE_H
#define DSRTREE_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrtncsr.h"

#include "dcmtk/ofstd/ofutil.h"


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

template <typename T> class DSRTree;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for tree nodes
 */
class DCMTK_DCMSR_EXPORT DSRTreeNode
  : protected DSRTypes
{
    // allow direct access to member variables
    friend class DSRTree<DSRTreeNode>;
    friend class DSRTree<DSRDocumentTreeNode>;

  public:

    /** (default) constructor
     ** @param  annotation  optional annotation that should be set as the initial value
     */
    DSRTreeNode(const DSRTreeNodeAnnotation &annotation = DSRTreeNodeAnnotation())
      : Prev(NULL),
        Next(NULL),
        Down(NULL),
        Annotation(annotation),
        Ident(IdentCounter++)   // tbc: is this MT-safe?
    {
    }

    /** destructor
     */
    virtual ~DSRTreeNode()
    {
    }

    /** clone this tree node.
     *  Actually, a new tree node is created since the pointer members are not needed.
     *  Only the optional annotation is copied.
     ** @return copy of this tree node
     */
    virtual DSRTreeNode *clone() const
    {
        return new DSRTreeNode(Annotation);
    }

    /** get pointer to previous tree node
     ** @return pointer to previous tree node (might be NULL)
     */
    inline DSRTreeNode *getPrev() const
    {
        return Prev;
    }

    /** get pointer to next tree node
     ** @return pointer to next tree node (might be NULL)
     */
    inline DSRTreeNode *getNext() const
    {
        return Next;
    }

    /** get pointer to first child node
     ** @return pointer to first child node (might be NULL)
     */
    inline DSRTreeNode *getDown() const
    {
        return Down;
    }

    /** get unique identifier of this node
     ** @return unique identifier of this node
     */
    inline size_t getIdent() const
    {
        return Ident;
    }

    /** check whether this node has a (non-empty) annotation
     ** @return OFTrue if this node has a (non-empty) annotation, OFFalse otherwise
     */
    inline OFBool hasAnnotation() const
    {
        return !Annotation.isEmpty();
    }

    /** clear annotation of this node
     */
    inline void clearAnnotation()
    {
        Annotation.clear();
    }

    /** get annotation of this node (optional)
     ** @return annotation of this node (might be empty)
     */
    inline const DSRTreeNodeAnnotation &getAnnotation() const
    {
        return Annotation;
    }

    /** set annotation of this node (optional)
     ** @param  annotation  annotation to be set
     */
    inline void setAnnotation(const DSRTreeNodeAnnotation &annotation)
    {
        Annotation = annotation;
    }


  protected:

    /** get current value of global node counter
     ** @return current value of global node counter
     */
    inline static size_t getIdentCounter()
    {
        return IdentCounter;
    }

    /// pointer to previous tree node (if any)
    DSRTreeNode *Prev;
    /// pointer to next tree node (if any)
    DSRTreeNode *Next;
    /// pointer to first child node (if any)
    DSRTreeNode *Down;

    /// annotation of the tree node (optional)
    DSRTreeNodeAnnotation Annotation;


  private:

    /// unique identifier (created automatically)
    const size_t Ident;

    /// global counter used to create the unique identifiers
    static size_t IdentCounter;


 // --- declaration of copy constructor and assignment operator

    DSRTreeNode(const DSRTreeNode &);
    DSRTreeNode &operator=(const DSRTreeNode &);
};


/** Class managing a tree of nodes
 */
template<typename T = DSRTreeNode> class DSRTree
  : public DSRTreeNodeCursor<T>,
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
    DSRTree(const DSRTree<T> &tree);

    /** destructor
     */
    virtual ~DSRTree();

    /** assignment operator.
     *  Please note that internally the copy constructor is used, so the same comments
     *  apply.
     ** @param  tree  tree to be copied
     ** @return reference to this tree after copying
     */
    DSRTree &operator=(DSRTree<T> tree);

    /** clear all member variables, i.e.\ the tree with all nodes
     */
    virtual void clear();

    /** clear annotations of all tree nodes
     */
    void clearAnnotations();

    /** check whether tree has any nodes
     ** @return OFTrue if tree is empty, OFFalse otherwise
     */
    inline OFBool isEmpty() const;

    /** count number of nodes in the tree.
     *  This method iterates over all nodes that are stored in the tree.
     ** @return number of nodes, 0 if tree is empty
     */
    size_t countNodes() const;

    /** get ID of the next node to be created.
     *  The node ID uniquely identifies a content item in the document tree.
     ** @return ID of the next node to be created (should never be 0)
     */
    inline size_t getNextNodeID() const;

    /** set internal cursor to root node
     ** @return ID of root node if successful, 0 otherwise
     */
    inline size_t gotoRoot();

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

    /** set cursor to specified node. Starts from current position!
     ** @param  annotation     annotation of the node to set the cursor to
     *  @param  startFromRoot  flag indicating whether to start from the root node
     *                         or the current one
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t gotoNode(const DSRTreeNodeAnnotation &annotation,
                    const OFBool startFromRoot = OFTrue);

    /** add new node to the current one.
     *  Please note that no copy of the given node is created.  Therefore, the node
     *  should be created with new() - do not use a reference to a local variable.
     *  If the node could be added successfully, the cursor is set to it automatically.
     ** @param  node     pointer to the new node to be added
     *  @param  addMode  flag specifying at which position to add the new node
     ** @return ID of the new node if successful, 0 otherwise
     */
    virtual size_t addNode(T *node,
                           const E_AddMode addMode = AM_afterCurrent);

    /** replace current node by the given one.
     *  Please note that no copy of the given node is created.  Therefore, the node
     *  should be created with new() - do not use a reference to a local variable.  If
     *  the node could be replaced successfully, the "old" node (and all of its child
     *  nodes) are deleted, and the cursor is set to the new one.
     ** @param  node  pointer to the new node to replace the current one
     ** @return ID of the new node if successful, 0 otherwise
     */
    virtual size_t replaceNode(T *node);

    /** extract current node from tree.
     *  Please note that not only the specified node but also all of its child nodes are
     *  extracted from the tree.  The cursor is set automatically to a new valid position.
     ** @return pointer to extracted node, NULL in case of error (or the tree was empty)
     */
    virtual T *extractNode();

    /** get pointer to root node and "forget" the internal reference to this node.
     *  In other words: after calling this method, the stored tree will be empty.
     *  This also means that the caller is responsible for deleting the allocated memory.
     ** @return pointer to root node, might be NULL (empty tree)
     */
    virtual T *getAndRemoveRootNode();

    /** remove current node from tree.
     *  Please note that not only the specified node but also all of its child nodes are
     *  removed from the tree and then deleted.  The cursor is set automatically to a new
     *  valid position.
     ** @return ID of the node which became the current one after deletion, 0 if an error
     *          occurred or the tree is now empty.
     */
    virtual size_t removeNode();

    /** extract a subtree i.e.\ a fragment from this tree.
     *  The subtree is specified by the current node, which becomes the root of the subtree.
     ** @return pointer to the extracted subtree, NULL in case of error
     */
    virtual DSRTree<T> *extractSubTree();

    /** clone a subtree i.e.\ a fragment of this tree.
     *  The cloning starts with the current node and ends with the given node.
     ** @param  stopAfterNodeID  ID of the node after which the cloning should stop.
     *                           By default (0), the process ends after cloning the
     *                           current node with all of its child nodes (if any).
     ** @return pointer to a copy of the specified subtree, NULL in case of error
     */
    virtual DSRTree<T> *cloneSubTree(const size_t stopAfterNodeID = 0) const;


  protected:

    /** special constructor that receives a pointer to the root node.
     *  Please note that the 'rootNode' and the associated tree is not copied!
     ** @param  rootNode  pointer to the root node of the "new" tree
     */
    DSRTree(T *rootNode);

    /** special copy constructor that clones a particular subtree only
     ** @param  startCursor      first node of the subtree to be copied
     *  @param  stopAfterNodeID  ID of the node after which the cloning should stop
     */
    DSRTree(const DSRTreeNodeCursor<T> &startCursor,
            size_t stopAfterNodeID);

    /** fast, non-throwing swap function.
     *  The time complexity of this function is constant.
     ** @param  tree  tree to swap with
     */
    void swap(DSRTree<T> &tree);

    /** get pointer to root node
     ** @return pointer to root node, might be NULL (empty tree)
     */
    virtual T *getRoot() const;

    /** delete a tree given by its root node.
     *  Please note that the given 'rootNode' pointer becomes invalid afterwards.
     ** @param  rootNode  pointer to the root node of the tree to be deleted
     */
    virtual void deleteTreeFromRootNode(T *rootNode);


  private:

    /// pointer to the root tree node
    T *RootNode;
};


/*------------------*
 *  implementation  *
 *------------------*/

template<typename T>
DSRTree<T>::DSRTree()
  : DSRTreeNodeCursor<T>(),
    RootNode(NULL)
{
}


template<typename T>
DSRTree<T>::DSRTree(const DSRTree<T> &tree)
  : DSRTreeNodeCursor<T>(),
    RootNode(NULL)
{
    if (!tree.isEmpty())
    {
        E_AddMode addMode = AM_afterCurrent;
        T *newNode = NULL;
        T *nodeCursor = tree.getRoot();
        OFStack<T *> nodeCursorStack;
        /* perform a "deep search", just like DSRTreeNodeCursor<>::iterate() */
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
                nodeCursor = nodeCursor->getDown();
                addMode = AM_belowCurrent;
            }
            else if (nodeCursor->Next != NULL)
            {
                /* proceed with the following sibling */
                nodeCursor = nodeCursor->getNext();
                addMode = AM_afterCurrent;
            } else {
                /* check whether there are any siblings on higher levels */
                do {
                    if (!nodeCursorStack.empty())
                    {
                        nodeCursor = nodeCursorStack.top();
                        nodeCursorStack.pop();
                        this->goUp();
                    } else
                        nodeCursor = NULL;
                } while ((nodeCursor != NULL) && (nodeCursor->Next == NULL));
                if (nodeCursor != NULL)
                {
                    nodeCursor = nodeCursor->getNext();
                    addMode = AM_afterCurrent;
                }
            }
        }
        /* initialize the cursor */
        gotoRoot();
    }
}


template<typename T>
DSRTree<T>::DSRTree(T *rootNode)
  : DSRTreeNodeCursor<T>(),
    RootNode(rootNode)
{
    /* initialize the cursor */
    gotoRoot();
}


template<typename T>
DSRTree<T>::DSRTree(const DSRTreeNodeCursor<T> &startCursor,
                    size_t stopAfterNodeID)
  : DSRTreeNodeCursor<T>(),
    RootNode(NULL)
{
    T *nodeCursor = startCursor.getNode();
    /* since we start from a particular node, we need to check it first */
    if (nodeCursor != NULL)
    {
        E_AddMode addMode = AM_afterCurrent;
        T *newNode = NULL;
        OFStack<T *> nodeCursorStack;
        /* use current node if none was specified */
        if (stopAfterNodeID == 0)
            stopAfterNodeID = nodeCursor->getIdent();
        /* perform a "deep search", just like DSRTreeNodeCursor<>::iterate() */
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
                nodeCursor = nodeCursor->getDown();
                addMode = AM_belowCurrent;
            }
            else if (nodeCursor->Next != NULL)
            {
                /* check whether the last node has been processed */
                if (nodeCursor->getIdent() == stopAfterNodeID)
                {
                    /* exit the loop */
                    nodeCursor = NULL;
                } else {
                    /* proceed with the following sibling */
                    nodeCursor = nodeCursor->getNext();
                    addMode = AM_afterCurrent;
                }
            } else {
                /* check whether there are any siblings on higher levels */
                do {
                    if (!nodeCursorStack.empty())
                    {
                        nodeCursor = nodeCursorStack.top();
                        nodeCursorStack.pop();
                        this->goUp();
                        /* check whether the last node has been processed */
                        if ((nodeCursor != NULL) && (nodeCursor->getIdent() == stopAfterNodeID))
                        {
                            /* exit the loop */
                            nodeCursor = NULL;
                        }
                    } else
                        nodeCursor = NULL;
                } while ((nodeCursor != NULL) && (nodeCursor->Next == NULL));
                if (nodeCursor != NULL)
                {
                    nodeCursor = nodeCursor->getNext();
                    addMode = AM_afterCurrent;
                }
            }
        }
        /* initialize the cursor */
        gotoRoot();
    }
}


template<typename T>
DSRTree<T>::~DSRTree()
{
    clear();
}


template<typename T>
DSRTree<T> &DSRTree<T>::operator=(DSRTree<T> tree)
{
    /* by-value parameter serves as a temporary */
    swap(tree);
    return *this;
}


template<typename T>
void DSRTree<T>::clear()
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


template<typename T>
void DSRTree<T>::clearAnnotations()
{
    DSRTreeNodeCursor<T> cursor(RootNode);
    if (cursor.isValid())
    {
        /* iterate over all nodes */
        do {
            cursor.getNode()->clearAnnotation();
        } while (cursor.iterate());
    }
}


template<typename T>
OFBool DSRTree<T>::isEmpty() const
{
    return (RootNode == NULL);
}


template<typename T>
size_t DSRTree<T>::countNodes() const
{
    size_t count = 0;
    DSRTreeNodeCursor<T> cursor(RootNode);
    if (cursor.isValid())
    {
        /* iterate over all nodes */
        do {
            ++count;
        } while (cursor.iterate());
    }
    return count;
}


template<typename T>
size_t DSRTree<T>::getNextNodeID() const
{
    /* current value of the counter used to create unique identifiers */
    return DSRTreeNode::getIdentCounter();
}


template<typename T>
size_t DSRTree<T>::gotoRoot()
{
    return this->setCursor(RootNode);
}


template<typename T>
size_t DSRTree<T>::gotoNode(const size_t searchID,
                            const OFBool startFromRoot)
{
    size_t nodeID = 0;
    if (searchID > 0)
    {
        if (startFromRoot)
            gotoRoot();
        /* call the real function */
        nodeID = DSRTreeNodeCursor<T>::gotoNode(searchID);
    }
    return nodeID;
}


template<typename T>
size_t DSRTree<T>::gotoNode(const OFString &reference,
                            const OFBool startFromRoot)
{
    size_t nodeID = 0;
    if (!reference.empty())
    {
        if (startFromRoot)
            gotoRoot();
        /* call the real function */
        nodeID = DSRTreeNodeCursor<T>::gotoNode(reference);
    }
    return nodeID;
}


template<typename T>
size_t DSRTree<T>::gotoNode(const DSRTreeNodeAnnotation &annotation,
                            const OFBool startFromRoot)
{
    size_t nodeID = 0;
    if (!annotation.isEmpty())
    {
        if (startFromRoot)
            gotoRoot();
        /* call the real function */
        nodeID = DSRTreeNodeCursor<T>::gotoNode(annotation);
    }
    return nodeID;
}


template<typename T>
size_t DSRTree<T>::addNode(T *node,
                           const E_AddMode addMode)
{
    size_t nodeID = 0;
    /* make sure that 'node' points to a single node or to the "root" of a subtree */
    if ((node != NULL) && (node->Prev == NULL))
    {
        if (this->NodeCursor != NULL)
        {
            DSRTreeNode *lastNode = node;
            /* update references based on 'addMode' */
            switch (addMode)
            {
                case AM_afterCurrent:
                    node->Prev = this->NodeCursor;
                    /* goto last node (sibling), if any */
                    while (lastNode->Next != NULL)
                        lastNode = lastNode->Next;
                    lastNode->Next = this->NodeCursor->Next;
                    /* connect to current node */
                    if (this->NodeCursor->Next != NULL)
                        (this->NodeCursor->Next)->Prev = lastNode;
                    this->NodeCursor->Next = node;
                    ++this->Position;
                    break;
                case AM_beforeCurrent:
                    node->Prev = this->NodeCursor->Prev;
                    /* goto last node (sibling), if any */
                    while (lastNode->Next != NULL)
                        lastNode = lastNode->Next;
                    lastNode->Next = this->NodeCursor;
                    /* connect to current node */
                    if ((this->NodeCursor->Prev != NULL) && (this->Position > 1))
                        (this->NodeCursor->Prev)->Next = node;
                    else if (!this->NodeCursorStack.empty() && (this->Position == 1))
                        this->NodeCursorStack.top()->Down = node;
                    this->NodeCursor->Prev = lastNode;
                    /* check whether root node has been replaced */
                    if (this->NodeCursor == this->RootNode)
                        this->RootNode = node;
                    break;
                case AM_belowCurrent:
                    /* store old position */
                    this->Position.goDown();
                    this->NodeCursorStack.push(this->NodeCursor);
                    /* parent node has already child nodes */
                    if (this->NodeCursor->Down != NULL)
                    {
                        DSRTreeNode *tempNode = this->NodeCursor->Down;
                        /* goto last node (sibling) */
                        while (tempNode->Next != NULL)
                        {
                            tempNode = tempNode->Next;
                            ++this->Position;
                        }
                        /* connect to last child */
                        tempNode->Next = node;
                        node->Prev = tempNode;
                        ++this->Position;
                    } else
                        this->NodeCursor->Down = node;
                    break;
                case AM_belowCurrentBeforeFirstChild:
                    /* store old position */
                    this->Position.goDown();
                    this->NodeCursorStack.push(this->NodeCursor);
                    /* parent node has already child nodes */
                    if (this->NodeCursor->Down != NULL)
                    {
                        /* goto last node (sibling), if any */
                        while (lastNode->Next != NULL)
                            lastNode = lastNode->Next;
                        /* connect to (current) first child */
                        lastNode->Next = this->NodeCursor->Down;
                        (this->NodeCursor->Down)->Prev = lastNode;
                    }
                    this->NodeCursor->Down = node;
                    break;
            }
            this->NodeCursor = node;
        } else {
            /* originally, the tree was empty */
            this->RootNode = this->NodeCursor = node;
            this->Position.initialize();
        }
        nodeID = this->NodeCursor->getIdent();
    }
    return nodeID;
}


template<typename T>
size_t DSRTree<T>::replaceNode(T *node)
{
    size_t nodeID = 0;
    /* make sure that 'node' points to a single node or to the "root" of a subtree */
    if ((node != NULL) && (node->Prev == NULL))
    {
        if (this->NodeCursor != NULL)
        {
            /* connect to previous node */
            if (this->NodeCursor->Prev != NULL)
            {
                (this->NodeCursor->Prev)->Next = node;
                /* remove reference to former sibling */
                this->NodeCursor->Prev = NULL;
            } else {
                /* is there any direct parent node? */
                if (!this->NodeCursorStack.empty())
                {
                    DSRTreeNode *parent = this->NodeCursorStack.top();
                    if (parent != NULL)
                        parent->Down = node;
                }
            }
            /* connect to next node */
            if (this->NodeCursor->Next != NULL)
            {
                DSRTreeNode *lastNode = node;
                /* goto last node (sibling), if any */
                while (lastNode->Next != NULL)
                    lastNode = lastNode->Next;
                (this->NodeCursor->Next)->Prev = lastNode;
                lastNode->Next = this->NodeCursor->Next;
                /* remove reference to former sibling */
                this->NodeCursor->Next = NULL;
            }
            /* check whether root node has been replaced */
            if (this->NodeCursor == this->RootNode)
                this->RootNode = node;
            /* free memory of old (now replaced) node */
            deleteTreeFromRootNode(this->NodeCursor);
            /* set cursor to new node */
            this->NodeCursor = node;
            nodeID = node->getIdent();
        }
    }
    return nodeID;
}


template<typename T>
T *DSRTree<T>::extractNode()
{
    T *cursor = this->NodeCursor;
    /* extract current node (incl. subtree) from tree */
    if (cursor != NULL)
    {
        /* are there any siblings? */
        if ((cursor->Prev != NULL) || (cursor->Next != NULL))
        {
            /* connect to previous node */
            if (cursor->Prev != NULL)
                (cursor->Prev)->Next = cursor->Next;
            else
            {
                /* is there any direct parent node? */
                if (!this->NodeCursorStack.empty())
                {
                    DSRTreeNode *parent = this->NodeCursorStack.top();
                    if (parent != NULL)
                        parent->Down = cursor->Next;
                }
            }
            /* connect to next node */
            if (cursor->Next != NULL)
            {
                (cursor->Next)->Prev = cursor->Prev;
                if (this->NodeCursor == this->RootNode)
                    this->RootNode = cursor->getNext();     // old root node deleted
                this->NodeCursor = cursor->getNext();
            } else {
                /* set cursor to previous node since there is no next node */
                this->NodeCursor = cursor->getPrev();
                --this->Position;
            }
        } else {
            /* no siblings: check for child nodes */
            if (!this->NodeCursorStack.empty())
            {
                this->NodeCursor = this->NodeCursorStack.top();
                this->NodeCursorStack.pop();
                this->Position.goUp();
                /* should never be NULL, but ... */
                if (this->NodeCursor != NULL)
                    this->NodeCursor->Down = NULL;
                else
                {
                    this->RootNode = NULL;                  // tree is now empty
                    this->Position.clear();
                }
            } else {
                this->RootNode = this->NodeCursor = NULL;   // tree is now empty
                this->Position.clear();
            }
        }
        /* remove references to former siblings */
        cursor->Prev = NULL;
        cursor->Next = NULL;
    }
    return cursor;
}


template<typename T>
T *DSRTree<T>::getAndRemoveRootNode()
{
    T *root = RootNode;
    /* "forget" reference to root node */
    RootNode = NULL;
    return root;
}


template<typename T>
size_t DSRTree<T>::removeNode()
{
    size_t nodeID = 0;
    /* extract current node (incl. subtree) from tree */
    T *cursor = extractNode();
    if (cursor != NULL)
    {
        /* delete all nodes from extracted subtree */
        /* (could also use the "new" DSRTreeNodeCursor class) */
        T *delNode = NULL;
        OFStack<T *> cursorStack;
        while (cursor != NULL)
        {
            delNode = cursor;
            if (cursor->Down != NULL)
            {
                if (cursor->Next != NULL)
                    cursorStack.push(cursor->getNext());
                cursor = cursor->getDown();
            }
            else if (cursor->Next != NULL)
                cursor = cursor->getNext();
            else if (!cursorStack.empty())
            {
                cursor = cursorStack.top();
                cursorStack.pop();
            } else
                cursor = NULL;
            delete delNode;
        }
        /* return identifier of (new) current node */
        if (this->NodeCursor != NULL)
            nodeID = this->NodeCursor->getIdent();
    }
    return nodeID;
}


template<typename T>
DSRTree<T> *DSRTree<T>::extractSubTree()
{
    DSRTree<T> *tree = NULL;
    /* extract current node from tree and create a new tree object (with this root) */
    T *node = extractNode();
    if (node != NULL)
        tree = new DSRTree<T>(node);
    return tree;
}


template<typename T>
DSRTree<T> *DSRTree<T>::cloneSubTree(const size_t stopAfterNodeID) const
{
    /* create a copy of the specified subtree */
    return new DSRTree<T>(this->NodeCursor, stopAfterNodeID);
}


// protected methods

template<typename T>
void DSRTree<T>::swap(DSRTree<T> &tree)
{
    /* swap pointer to the root tree node */
    OFswap(RootNode, tree.RootNode);
}


template<typename T>
T *DSRTree<T>::getRoot() const
{
    return RootNode;
}


template<typename T>
void DSRTree<T>::deleteTreeFromRootNode(T *rootNode)
{
    /* create a temporary tree object from the given node, */
    /* the content will be deleted during destruction */
    DSRTree<T> tree(rootNode);
}


#endif
