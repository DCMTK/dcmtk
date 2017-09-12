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
 *    classes: DSRIncludedTemplateNodeCursor
 *
 */


#ifndef DSRITCSR_H
#define DSRITCSR_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsdefine.h"
#include "dcmtk/dcmsr/dsrtncsr.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class implementing a document tree node cursor.
 *  This type of cursor also supports included sub-templates, i.e. the subtree that is
 *  managed by an instance of DSRIncludedTemplateTreeNode is also iterated.
 */
class DCMTK_DCMSR_EXPORT DSRIncludedTemplateNodeCursor
  : public DSRTreeNodeCursor<DSRDocumentTreeNode>
{

  public:

    /** default constructor
     */
    DSRIncludedTemplateNodeCursor();

    /** copy constructor
     ** @param  cursor  object to be copied
     */
    DSRIncludedTemplateNodeCursor(const DSRIncludedTemplateNodeCursor &cursor);

    /** constructor.
     *  See comments on DSRTreeNodeCursor<>::setCursor() method.
     ** @param  node      pointer to tree node used to initialize the cursor
     *  @param  position  optional pointer to position counter that should be used to
     *                    initialize the internal counter
     */
    DSRIncludedTemplateNodeCursor(DSRDocumentTreeNode *node,
                                  const DSRPositionCounter *position = NULL);

    /** destructor
     */
    virtual ~DSRIncludedTemplateNodeCursor();

    /** assignment operator
     ** @param  cursor  object to be copied
     ** @return reference to modified cursor (this object)
     */
    DSRIncludedTemplateNodeCursor &operator=(const DSRIncludedTemplateNodeCursor &cursor);

    /** assignment operator.
     *  See comments on DSRTreeNodeCursor<>::setCursor() method.
     ** @param  node  node to which the cursor should be set
     ** @return reference to modified cursor (this object)
     */
    DSRIncludedTemplateNodeCursor &operator=(DSRDocumentTreeNode *node);

    /** get pointer to first child node.
     *  Can be used to have a lookup to the first child node without changing the cursor.
     ** @return pointer to first child node (if any), NULL otherwise
     */
    virtual const DSRDocumentTreeNode *getChildNode() const;

    /** count number of children of the current node.
     *  This method iterates over all children of the current node, either on all
     *  sub-levels or on the first child level only.
     ** @param  searchIntoSub  flag indicating whether to search into sub-trees
     *                         ("deep search") or on the first child level only
     ** @return number of children of the current node, 0 if none
     */
    virtual size_t countChildNodes(const OFBool searchIntoSub = OFTrue) const;

    /** goto first child node (one level down)
     ** @return ID of the first child node if successful, 0 otherwise
     */
    virtual size_t goDown();

    /** iterate over all nodes (starting from current position)
     ** @param  searchIntoSub  flag indicating whether to search into sub-trees
     *                         ("deep search") or on the current level only
     ** @return ID of the next node if successful, 0 otherwise
     */
    virtual size_t iterate(const OFBool searchIntoSub = OFTrue);
};


#endif
