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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRTreeNode, DSRTree
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:33 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DSRTreeNode
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
class DSRTree
  : public DSRTreeNodeCursor,
    protected DSRTypes
{

  public:

    /** default constructor
     */
    DSRTree();

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
     *                   (AM_afterCurrent, AM_beforeCurrent, AM_belowCurrent)
     ** @return ID of the new added node if successful, 0 otherwise
     */
    virtual size_t addNode(DSRTreeNode *node,
                           const E_AddMode addMode = AM_afterCurrent);

    /** remove current node from tree.
     *  Please note that not only the specified node but also all of its child nodes are
     *  removed from the tree and then deleted.  The cursor is set automatically to a new
     *  valid position.
     ** @return ID of the node which became the current one after deletion, 0 if an error
     *          occured or the tree is now empty.
     */
    virtual size_t removeNode();


  protected:

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


 // --- declaration of copy constructor and assignment operator

    DSRTree(const DSRTree &);
    DSRTree &operator=(const DSRTree &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrtree.h,v $
 *  Revision 1.10  2010-10-14 13:16:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.9  2005-12-08 16:05:29  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.8  2003/12/17 17:40:17  meichel
 *  Changed order of inheritance to avoid internal compiler error on Borland Builder.
 *
 *  Revision 1.7  2003/08/07 12:56:22  joergr
 *  Updated documentation to get rid of doxygen warnings.
 *
 *  Revision 1.6  2002/04/11 13:02:34  joergr
 *  Corrected typo and/or enhanced documentation.
 *
 *  Revision 1.5  2001/06/01 15:51:06  meichel
 *  Updated copyright header
 *
 *  Revision 1.4  2000/11/01 16:22:11  joergr
 *  Now derive "protected" from base class DSRTypes instead of "public".
 *
 *  Revision 1.3  2000/10/18 17:09:29  joergr
 *  Made some functions inline.
 *
 *  Revision 1.2  2000/10/16 11:56:10  joergr
 *  Added doc++ comments.
 *
 *  Revision 1.1  2000/10/13 07:49:35  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
