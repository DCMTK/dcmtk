/*
 *
 *  Copyright (C) 2016-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
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
 *    classes: DSRDocumentTreeNodeCursor
 *
 */


#ifndef DSRDNCSR_H
#define DSRDNCSR_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsdefine.h"
#include "dcmtk/dcmsr/dsrtncsr.h"
#include "dcmtk/dcmsr/dsrdnflt.h"


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

class DSRDocumentTreeNode;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class implementing a document tree node cursor.
 *  Included sub-templates are not supported by this type of cursor, i.e. the subtree
 *  that is managed by an instance of DSRIncludedTemplateTreeNode is not iterated.
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeCursor
  : public DSRTreeNodeCursor<DSRDocumentTreeNode>
{

  public:

    /** default constructor
     */
    DSRDocumentTreeNodeCursor();

    /** copy constructor
     ** @param  cursor  object to be copied
     */
    DSRDocumentTreeNodeCursor(const DSRDocumentTreeNodeCursor &cursor);

    /** copy constructor (for base class)
     ** @param  cursor  object to be copied
     */
    DSRDocumentTreeNodeCursor(const DSRTreeNodeCursor<DSRDocumentTreeNode> &cursor);

    /** constructor.
     *  See comments on DSRTreeNodeCursor<>::setCursor() method.
     ** @param  node      pointer to tree node used to initialize the cursor
     *  @param  position  optional pointer to position counter that should be used to
     *                    initialize the internal counter
     */
    DSRDocumentTreeNodeCursor(DSRDocumentTreeNode *node,
                              const DSRPositionCounter *position = NULL);

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeCursor();

    /** assignment operator
     ** @param  cursor  object to be copied
     ** @return reference to modified cursor (this object)
     */
    DSRDocumentTreeNodeCursor &operator=(const DSRDocumentTreeNodeCursor &cursor);

    /** assignment operator.
     *  See comments on DSRTreeNodeCursor<>::setCursor() method.
     ** @param  node  node to which the cursor should be set
     ** @return reference to modified cursor (this object)
     */
    DSRDocumentTreeNodeCursor &operator=(DSRDocumentTreeNode *node);

    /** set internal cursor to a matching node (starting from current position).
     *  If more than one node exists matching the given filter, the first one will be
     *  selected.  Use gotoNextMatchingNode() in order to go to the next matching node.
     ** @param  filter         matching criterion based on a single or multiple filters
     *  @param  searchIntoSub  flag indicating whether to search into sub-trees
     *                         ("deep search") or on the current level only
     ** @return ID of the new current node if successful, 0 otherwise
     */
    virtual size_t gotoMatchingNode(const DSRDocumentTreeNodeFilter &filter,
                                    const OFBool searchIntoSub = OFTrue);

    /** set internal cursor to the next matching node.
     *  Starts from "next" node, i.e. either the first child of the current node or the
     *  first sibling following the current node.
     ** @param  filter         matching criterion based on a single or multiple filters
     *  @param  searchIntoSub  flag indicating whether to search into sub-trees
     *                         ("deep search") or on the current level only
     ** @return ID of the new current node if successful, 0 otherwise
     */
    virtual size_t gotoNextMatchingNode(const DSRDocumentTreeNodeFilter &filter,
                                        const OFBool searchIntoSub = OFTrue);
};


#endif
