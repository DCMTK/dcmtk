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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRTreeNode, DSRTree
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:49:35 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRTREE_H
#define DSRTREE_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrtncsr.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for tree nodes
 */
class DSRTreeNode
  : public DSRTypes
{

    friend class DSRTreeNodeCursor;
    friend class DSRTree;

  public:

    DSRTreeNode()
      : Prev(NULL),
        Next(NULL),
        Down(NULL),
        Ident(IdentCounter++)   // MT-safe?
    {
    }

    virtual ~DSRTreeNode()
    {
    }


  protected:

    DSRTreeNode *Prev;
    DSRTreeNode *Next;
    DSRTreeNode *Down;
  
    const size_t Ident;
    

  private:

    static size_t IdentCounter;

 
 // --- declaration of copy constructor and assignment operator

    DSRTreeNode(const DSRTreeNode &);
    DSRTreeNode &operator=(const DSRTreeNode &);
};


/** Class managing a tree of nodes.
 *  This is in fact a directed acyclic graph (DAG).
 */
class DSRTree
  : public DSRTypes,
    public DSRTreeNodeCursor
{

  public:

    DSRTree();

    virtual ~DSRTree();

    virtual void clear();
    
    OFBool isEmpty() const;

    size_t gotoRoot();

    size_t gotoNode(const size_t searchID,
                    const OFBool startFromRoot = OFTrue);

    size_t gotoNode(const OFString &reference,
                    const OFBool startFromRoot = OFTrue);

    virtual size_t addNode(DSRTreeNode *node,
                           const E_AddMode addMode = AM_afterCurrent);

    virtual size_t removeNode();


  protected:
  
    DSRTreeNode *getRoot() const
    {
        return RootNode;
    }
  

  private:

    DSRTreeNode *RootNode;


 // --- declaration of copy constructor and assignment operator

    DSRTree(const DSRTree &);
    DSRTree &operator=(const DSRTree &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrtree.h,v $
 *  Revision 1.1  2000-10-13 07:49:35  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
