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
 *    classes: DSRTreeNodeCursor
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:49:34 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRTNCSR_H
#define DSRTNCSR_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "ofstring.h"
#include "ofstack.h"


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

class DSRTreeNode;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class implementing a tree node cursor
 */
class DSRTreeNodeCursor
{

  public:

    DSRTreeNodeCursor();

    DSRTreeNodeCursor(const DSRTreeNodeCursor &cursor);

    DSRTreeNodeCursor(DSRTreeNode *node);

    virtual ~DSRTreeNodeCursor();

    DSRTreeNodeCursor &operator=(const DSRTreeNodeCursor &cursor);

    DSRTreeNodeCursor &operator=(DSRTreeNode *node);

    virtual void clear();

    OFBool isValid() const
    {
        return (NodeCursor != NULL);
    }

    DSRTreeNode *getNode() const
    {
        return NodeCursor;
    }

    const DSRTreeNode *getNextNode() const;

    size_t gotoPrevious();

    size_t gotoNext();

    size_t goUp();

    size_t goDown();

    size_t iterate();

    size_t gotoNode(const size_t searchID);

    size_t gotoNode(const OFString &reference);

    size_t getNodeID() const;

    size_t getLevel() const;

 
  protected:
  
    void clearNodeCursorStack();

    const DSRTreeNode *getParentNode();

    DSRTreeNode *NodeCursor;
    OFStack<DSRTreeNode *> NodeCursorStack;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrtncsr.h,v $
 *  Revision 1.1  2000-10-13 07:49:34  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
