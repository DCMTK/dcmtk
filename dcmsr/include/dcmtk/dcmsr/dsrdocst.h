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
 *    classes: DSRDocumentSubTree
 *
 */


#ifndef DSRDOCST_H
#define DSRDOCST_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtree.h"
#include "dcmtk/dcmsr/dsrdoctn.h"
#include "dcmtk/dcmsr/dsrcitem.h"

#include "dcmtk/dcmdata/dcitem.h"


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

class DSRIODConstraintChecker;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class managing a SR document subtree.
 *  A subtree represents an extract of an SR document tree.  Compared to the "SR Document
 *  Content Tree" that is defined in the DICOM standard, there are almost no restrictions
 *  regarding the value and relationship types.  It is also possible to have multiple nodes
 *  on the top-level, i.e. no dedicated root, or to use "unknown" relationship types.
 */
class DCMTK_DCMSR_EXPORT DSRDocumentSubTree
  : public DSRTree
{

  public:

    /** default constructor
     */
    DSRDocumentSubTree();

    /** copy constructor.
     *  Please note that the internal cursor is not copied but reset, i.e. set to the root
     *  node.  Also the IOD constraint checker is not copied by this class but recreated
     *  by the derived class DSRDocumentTree (based on the corresponding document type).
     ** @param  tree  subtree to be copied
     */
    DSRDocumentSubTree(const DSRDocumentSubTree &tree);

    /** destructor
     */
    virtual ~DSRDocumentSubTree();

    /** clone this subtree.
     *  Internally, the copy constructor is used, so the corresponding comments apply.
     ** @return copy of this subtree
     */
    virtual DSRDocumentSubTree *clone() const;

    /** clear internal member variables
     */
    virtual void clear();

    /** check whether the current internal state is valid.
     *  A subtree is valid if it is not empty.
     ** @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** print current SR document tree to specified output stream
     ** @param  stream  output stream
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags = 0);

    /** check whether specified content item can be added to the current one.
     *  This method can be used to decide which type of content items can be added prior
     *  to really doing so.
     *  Always returns true if no constraint checker is available but 'relationshipType'
     *  and 'valueType' have valid values.
     ** @param  relationshipType  relationship type of node to be checked with regard to
     *                            the current one
     *  @param  valueType         value type of node to be checked
     *  @param  addMode           flag specifying at which position the new node would
     *                            be added.  Possible values: DSRTypes::AM_afterCurrent,
     *                            DSRTypes::AM_beforeCurrent, DSRTypes::AM_belowCurrent
     ** @return OFTrue if specified node can be added, OFFalse otherwise
     */
    virtual OFBool canAddContentItem(const E_RelationshipType relationshipType,
                                     const E_ValueType valueType,
                                     const E_AddMode addMode = AM_afterCurrent);

    /** check whether specified by-reference relationship can be added to the current
     *  content item.
     *  Always returns true if no constraint checker is available but 'relationshipType' and
     *  'targetValueType' have valid values.
     ** @param  relationshipType  type of relationship between current and target node
     *  @param  targetValueType   value type of the referenced target node
     ** @return OFTrue if specified by-reference relationship can be added, OFFalse otherwise
     */
    virtual OFBool canAddByReferenceRelationship(const E_RelationshipType relationshipType,
                                                 const E_ValueType targetValueType);

    /** add specified content item to the current one.
     *  If possible this method creates a new node as specified and adds it to the current
     *  one.  The method canAddContentItem() is called internally to check parameters first.
     *  If the node could be added successfully, the cursor is set to it automatically.
     ** @param  relationshipType  relationship type of node to be added with regard to
     *                            the current one
     *  @param  valueType         value type of node to be added
     *  @param  addMode           flag specifying at which position to add the new node.
     *                            Possible values: DSRTypes::AM_afterCurrent,
     *                            DSRTypes::AM_beforeCurrent, DSRTypes::AM_belowCurrent
     ** @return ID of new node if successful, 0 otherwise
     */
    virtual size_t addContentItem(const E_RelationshipType relationshipType,
                                  const E_ValueType valueType,
                                  const E_AddMode addMode = AM_afterCurrent);

    /** add specified content item to the current one.
     *  If possible this method adds a given new node to the current one.  The method
     *  canAddContentItem() is called internally to check parameters first.  If the
     *  node could be added successfully, the cursor is set to it automatically.
     *  Please note that no copy of the given node is created.  Therefore, the node
     *  has to be created with new() or with DSRTypes::createDocumentTreeNode() - do
     *  not use a reference to a local variable and do not delete it a second time.
     ** @param  node          pointer to the new node to be added
     *  @param  addMode       flag specifying at which position to add the new node.
     *                        Possible values: DSRTypes::AM_afterCurrent,
     *                        DSRTypes::AM_beforeCurrent, DSRTypes::AM_belowCurrent
     *  @param  deleteIfFail  flag specifying whether to delete the given 'node' if
     *                        adding fails.  By default the item is not deleted, i.e.
     *                        in case of error it has to be deleted by the caller.
     ** @return pointer to new node if successful, NULL otherwise
     */
    virtual DSRDocumentTreeNode *addContentItem(DSRDocumentTreeNode *node,
                                                const E_AddMode addMode = AM_afterCurrent,
                                                const OFBool deleteIfFail = OFFalse);

    /** add specified content item after the current one.
     *  If possible this method creates a new node as specified and adds it after the current
     *  one, i.e. on the same level.  Internally, the first variant of the addContentItem()
     *  method is called with the third parameter being DSRTypes::AM_afterCurrent.  If
     *  successful, the given concept name is set for the new node, and the cursor is updated.
     *  @note This is a convenience function that avoids calling several other functions.
     ** @param  relationshipType  relationship type of node to be added with regard
     *                            to the current one
     *  @param  valueType         value type of node to be added
     *  @param  conceptName       concept name of the node to be added
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition addContentItem(const E_RelationshipType relationshipType,
                                       const E_ValueType valueType,
                                       const DSRCodedEntryValue &conceptName);

    /** add specified content item below the current one.
     *  If possible this method creates a new node as specified and adds it below the current
     *  one, i.e. as a child.  Internally, the first variant of the addContentItem() method
     *  is called with the third parameter being DSRTypes::AM_belowCurrent.  If successful,
     *  the given concept name is set for the new node, and the cursor is updated.
     *  @note This is a convenience function that avoids calling several other functions.
     ** @param  relationshipType  relationship type of node to be added with regard
     *                            to the current one
     *  @param  valueType         value type of node to be added
     *  @param  conceptName       concept name of the node to be added
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition addChildContentItem(const E_RelationshipType relationshipType,
                                            const E_ValueType valueType,
                                            const DSRCodedEntryValue &conceptName);

    /** add specified by-reference relationship to the current content item.
     *  If possible this method creates a new pseudo-node (relationship) and adds it to the
     *  current one.  The method canAddByReferenceRelationship() is called internally to check
     *  parameters first.  The internal cursor is automatically re-set to the current node.
     ** @param  relationshipType  relationship type between current and referenced node
     *  @param  referencedNodeID  node ID of the referenced content item
     ** @return ID of new pseudo-node if successful, 0 otherwise
     */
    virtual size_t addByReferenceRelationship(const E_RelationshipType relationshipType,
                                              const size_t referencedNodeID);

    /** remove current content item from tree.
     *  Please note that not only the specified node but also all of its child nodes are
     *  removed from the tree and then deleted.  The internal cursor is set automatically
     *  to a new valid position.
     ** @return ID of the node which became the current one after deletion, 0 if an error
     *    occurred or the tree is now empty.
     */
    virtual size_t removeCurrentContentItem();

    /** get reference to current content item.
     *  This mechanism allows to access all content items without using pointers.
     ** @return reference to current content item (might be invalid)
     */
    virtual DSRContentItem &getCurrentContentItem();

    /** clone the current tree node.
     *  Internally, the copy constructor of the respective tree node class is used, so the
     *  corresponding comments apply.  Please note that the new node has to deleted by the
     *  caller if it is not added to the document tree using addContentItem().
     ** @return pointer to a copy of the current tree node (might be NULL)
     */
    virtual DSRDocumentTreeNode *cloneCurrentTreeNode() const;

    /** set internal cursor to the named node.
     *  If more than one node exists with the given concept name the first one will
     *  be selected.  Use gotoNextNamedNode() in order to go to the next matching node.
     ** @param  conceptName    concept name of the node to be searched for
     *  @param  startFromRoot  flag indicating whether to start from the root node
     *                         or the current one
     *  @param  searchIntoSub  flag indicating whether to search into sub-trees
     *                         ("deep search") or on the current level only
     ** @return ID of the new current node if successful, 0 otherwise
     */
    virtual size_t gotoNamedNode(const DSRCodedEntryValue &conceptName,
                                 const OFBool startFromRoot = OFTrue,
                                 const OFBool searchIntoSub = OFTrue);

    /** set internal cursor to the next named node.
     *  Starts from "next" node, i.e. either the first children of the current node
     *  or the first sibling following the current node.
     ** @param  conceptName    concept name of the node to be searched for
     *  @param  searchIntoSub  flag indicating whether to search into sub-trees
     *                         ("deep search") or on the current level only
     ** @return ID of the new current node if successful, 0 otherwise
     */
    virtual size_t gotoNextNamedNode(const DSRCodedEntryValue &conceptName,
                                     const OFBool searchIntoSub = OFTrue);


  protected:

    /** add new node to the current one.
     *  Please note that no copy of the given node is created.  Therefore, the node
     *  has to be created with new() - do not use a reference to a local variable.
     *  If the node could be added successfully, the cursor is set to it automatically.
     ** @param  node     pointer to the new node to be added
     *  @param  addMode  flag specifying at which position to add the new node.
     *                   Possible values: DSRTypes::AM_afterCurrent,
     *                   DSRTypes::AM_beforeCurrent, DSRTypes::AM_belowCurrent
     ** @return ID of the new added node if successful, 0 otherwise
     */
    virtual size_t addNode(DSRDocumentTreeNode *node,
                           const E_AddMode addMode = AM_afterCurrent);

    /** remove current node from tree.
     *  Please note that not only the specified node but also all of his child nodes are
     *  removed from the tree and deleted afterwards.  The cursor is set automatically to
     *  a new valid position.
     ** @return ID of the node which became the current one after deletion, 0 if an error
     *          occurred or the tree is now empty.
     */
    virtual size_t removeNode();

    /** check the by-reference relationships (if any) for validity.
     *  This function checks whether all by-reference relationships possibly contained
     *  in the document tree are valid according to the following restrictions: source
     *  and target node are not identical and the target node is not an ancestor of the
     *  source node (requirement from the DICOM standard to prevent loops -> directed
     *  acyclic graph, though this is not 100% true - see "reportlp.dcm" example).
     *  In addition, the position strings (used to encode by-reference relationships
     *  according to the DICOM standard) OR the node IDs (used internally to uniquely
     *  identify nodes) can be updated.
     *  Please note that the checking modes 'DSRTypes::CM_updatePositionString' and
     *  'DSRTypes::CM_updateNodeID' are mutually exclusive.
     ** @param  mode   mode used to customize the checking process (see DSRTypes::CM_xxx)
     *  @param  flags  flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition checkByReferenceRelationships(const size_t mode = 0,
                                                      const size_t flags = 0);

    /** reset flag for all content items whether they are target of a by-reference relationship.
     *  This function calls 'setReferenceTarget(OFFalse)' for all content items.
     */
    virtual void resetReferenceTargetFlag();

    /// check relationship content constraints of the associated IOD
    DSRIODConstraintChecker *ConstraintChecker;


  private:

    /** add new node to the current one.
     *  This method just overwrites the method from the base class DSRTree.  Use the
     *  above addNode() method instead.
     ** @param  node     dummy parameter
     *  @param  addMode  dummy parameter
     ** @return always 0 (invalid)
     */
    virtual size_t addNode(DSRTreeNode *node,
                           const E_AddMode addMode = AM_afterCurrent);

    /// current content item.  Introduced to avoid the external use of pointers.
    DSRContentItem CurrentContentItem;


 // --- declaration of assignment operator

    DSRDocumentSubTree &operator=(const DSRDocumentSubTree &);
};


#endif
