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
 *  Update Date:      $Date: 2000-10-16 16:31:08 $
 *  CVS/RCS Revision: $Revision: 1.3 $
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
#include "oflist.h"


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

    /** default constructor
     */
    DSRTreeNodeCursor();

    /** copy constructor
     ** @param  cursor  object to be copied
     */
    DSRTreeNodeCursor(const DSRTreeNodeCursor &cursor);

    /** constructor
     ** @param  node  pointer tree node used to initialize the cursor (set to this node)
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
     */
    virtual OFBool isValid() const;

    /** get pointer to current node
     ** @return pointer to current node (might be NULL)
     */
    DSRTreeNode *getNode() const
    {
        return NodeCursor;
    }

    /** get pointer to next node.
     *  Used to have a lookup to the next node without changing the cursor.
     ** @return pointer to next node (if any), NULL otherwise
     */
    const DSRTreeNode *getNextNode() const;

    /** goto previous node on the same level
     ** @return ID of the previous node if successful, 0 otherwise
     */
    size_t gotoPrevious();

    /** goto next node on the same level
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

    /** iterate over all nodes.
     *  This method visits first the children (if any) and then the siblings ("deep search").
     ** @return ID of the next node if successful, 0 otherwise
     */
    size_t iterate();

    /** set cursor to specified node
     ** @param  searchID  ID of the node to set the cursor to
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t gotoNode(const size_t searchID);

    /** set cursor to specified node
     ** @param  reference  position string of the node to set the cursor to.
     *                     (the format is e.g. "1.2.3" for the third child of the
     *                     second child of the first node - see getPosition()).
     ** @return ID of the new current node if successful, 0 otherwise
     */
    size_t gotoNode(const OFString &position);

    /** get current node ID
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
     ** @param  position  variable where the position string should be stored
     ** @return reference to the resulting position string (empty if invalid)
     */
    const OFString &getPosition(OFString &position) const;


  protected:

    /** clear the internal node cursor stack
     */
    void clearNodeCursorStack();

    /** get pointer to parent node
     *  Used to have a lookup to the parent node without changing the cursor.
     ** @return pointer to parent node (if any), NULL otherwise
     */
    const DSRTreeNode *getParentNode();

    /// pointer current node
    DSRTreeNode *NodeCursor;
    /// stack of node pointers. Used to store the cursor position of upper levels.
    OFStack<DSRTreeNode *> NodeCursorStack;

    /// current position within the current level
    size_t Position;
    /// list of position counters in upper levels
    OFList<size_t> PositionList;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrtncsr.h,v $
 *  Revision 1.3  2000-10-16 16:31:08  joergr
 *  Updated comments.
 *
 *  Revision 1.2  2000/10/16 11:57:00  joergr
 *  Added doc++ comments.
 *
 *  Revision 1.1  2000/10/13 07:49:34  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
