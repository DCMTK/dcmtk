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
 *    classes: DSRDocumentSubTree
 *
 */


#ifndef DSRDOCST_H
#define DSRDOCST_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtree.h"
#include "dcmtk/dcmsr/dsrdoctn.h"
#include "dcmtk/dcmsr/dsrdncsr.h"
#include "dcmtk/dcmsr/dsrcitem.h"

#include "dcmtk/ofstd/ofmem.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DSRIODConstraintChecker;
class DSRSubTemplate;


/*-------------------*
 *  type definition  *
 *-------------------*/

typedef OFshared_ptr<DSRSubTemplate> DSRSharedSubTemplate;


/*--------------------------*
 *  template instantiation  *
 *--------------------------*/

// the following is required in order to help older Clang compilers,
// e.g. Clang 3.0 and 3.1 on Mac OS X when building with shared libs
#if defined(__clang__)
template class DSRTree<DSRDocumentTreeNode>;
#endif


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
  : public DSRTree<DSRDocumentTreeNode>
{

  public:

    /** default constructor
     */
    DSRDocumentSubTree();

    /** copy constructor.
     *  Please note that the internal cursor is not copied but reset, i.e. set to the root
     *  node.  Also the IOD constraint checker is not copied by this class but recreated
     *  by the derived class DSRDocumentTree (based on the corresponding document type).
     *  This constructor also updates any by-reference relationships, i.e. translates the
     *  references from the source 'tree' (based on the position string) to the IDs of the
     *  newly created nodes.
     ** @param  tree  subtree to be copied
     */
    DSRDocumentSubTree(const DSRDocumentSubTree &tree);

    /** destructor
     */
    virtual ~DSRDocumentSubTree();

    /** assignment operator.
     *  Please note that internally the copy constructor is used, so the same comments apply.
     ** @param  tree  subtree to be copied
     ** @return reference to this subtree after copying
     */
    DSRDocumentSubTree &operator=(DSRDocumentSubTree tree);

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

    /** check whether the internal cursor, which points to the current content item, is valid
     ** @return OFTrue if cursor is valid, OFFalse otherwise
     */
    virtual OFBool isCursorValid() const;

    /** check whether this subtree is a valid document tree.
     *  In order to be a valid document tree, there should be a single root node only, with
     *  the value type "CONTAINER", and the internal relationship type of this node should be
     *  DSRTypes::RT_isRoot.
     ** @param  defaultRelType  default relationship type that is used if the one of the
     *                          top-level node (root node) is "unknown"
     ** @return OFTrue if subtree is a valid document tree, OFFalse otherwise
     */
    virtual OFBool isValidDocumentTree(const E_RelationshipType defaultRelType = RT_unknown) const;

    /** check whether this subtree is an expanded document tree.
     *  Expanded means that no instances of DSRIncludedTemplateTreeNode exist in the document
     *  tree, i.e. no templates were included or all of them were replaced by their content
     *  (subtree).
     ** @return OFTrue if subtree is an expanded document tree, OFFalse otherwise
     */
    virtual OFBool isExpandedDocumentTree() const;

    /** check whether template identification is set
     ** @return OFTrue if template identification is set, OFFalse otherwise
     */
    virtual OFBool hasTemplateIdentification() const;

    /** check whether template identification is possible at all.
     *  According to the DICOM standard, it can be used if "the template consists of a single
     *  CONTAINER with nested content, and it is the outermost invocation of a set of nested
     *  templates that start with the same CONTAINER."
     *  With other words, the tree should have a single root node with value type "CONTAINER".
     ** @return OFTrue if template identification is possible, OFFalse otherwise
     */
    virtual OFBool canUseTemplateIdentification() const;

    /** print current SR document tree to specified output stream
     ** @param  stream      output stream
     *  @param  flags       optional flag used to customize the output (see DSRTypes::PF_xxx)
     *  @param  posCounter  optional pointer to position counter that should be used to
     *                      initialize the counter for line indentation or numbering of
     *                      nested content items
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags = 0,
                              const DSRPositionCounter *posCounter = NULL);

    /** write current SR document tree in XML format
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   optional flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &stream,
                                 const size_t flags = 0);

    /** get reference to current content item.
     *  This mechanism allows to access all content items without using pointers.
     ** @return reference to current content item (might be invalid)
     */
    virtual DSRContentItem &getCurrentContentItem();

    /** get read-only access to current node (content item)
     ** @return pointer to current node (might be NULL)
     */
    virtual const DSRDocumentTreeNode *getCurrentNode() const;

    /** get a cursor to the root node of this document tree.
     *  This cursor can be used to iterate over the nodes of the document tree without
     *  changing the internal cursor.  Please note that the cursor might become invalid,
     *  e.g. by pointing to a non-existing node if the content of the document tree is
     *  modified after the cursor has been retrieved.
     *  Included sub-templates are not supported by this type of cursor, i.e. the subtree
     *  that is managed by an instance of DSRIncludedTemplateTreeNode is not iterated.
     ** @param  cursor  reference to variable where the cursor is stored
     ** @return OFTrue is the returned 'cursor' is valid, OFFalse otherwise
     */
    virtual OFBool getCursorToRootNode(DSRDocumentTreeNodeCursor &cursor) const;

    /** get a cursor to the root node of this document tree.
     *  This cursor can be used to iterate over the nodes of the document tree without
     *  changing the internal cursor.  Please note that the cursor might become invalid,
     *  e.g. by pointing to a non-existing node if the content of the document tree is
     *  modified after the cursor has been retrieved.
     *  This type of cursor also supports included sub-templates, i.e. the subtree that
     *  is managed by an instance of DSRIncludedTemplateTreeNode is also iterated.
     ** @param  cursor  reference to variable where the cursor is stored
     ** @return OFTrue is the returned 'cursor' is valid, OFFalse otherwise
     */
    virtual OFBool getCursorToRootNode(DSRIncludedTemplateNodeCursor &cursor) const;

    /** count number of content items (nodes) in the document tree.
     *  This method iterates over all nodes that are stored in the document tree.
     *  By default, included sub-templates are counted as a single node (see options).
     ** @param  searchIntoSubTemplates      optional flag indicating whether to also
     *                                      count the content of included sub-templates
     *                                      (i.e.\ the nodes of the managed subtrees)
     *  @param  countIncludedTemplateNodes  optional flag indicating whether to count
     *                                      the DSRIncludedTemplateTreeNode instances
     *                                      as nodes.  See includeTemplate() for details.
     ** @return number of nodes, 0 if document tree is empty
     */
    size_t countNodes(const OFBool searchIntoSubTemplates = OFFalse,
                      const OFBool countIncludedTemplateNodes = OFTrue) const;

    /** set internal cursor to a named node.
     *  If more than one node exists with the given concept name, the first one will
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

    /** set internal cursor to a named node (starting from the first children of the
     *  current node).
     *  If more than one node exists with the given concept name, the first one will
     *  be selected.  Use gotoNextNamedNode() in order to go to the next matching node.
     ** @param  conceptName    concept name of the node to be searched for
     *  @param  searchIntoSub  flag indicating whether to search into sub-trees
     *                         ("deep search") or on the current level only
     ** @return ID of the new current node if successful, 0 otherwise
     */
    virtual size_t gotoNamedChildNode(const DSRCodedEntryValue &conceptName,
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

    /** set internal cursor to an annotated node.
     *  If more than one node exists with the given annotation text, the first one will
     *  be selected.  Use gotoNextAnnotatedNode() in order to go to the next matching
     *  node.  In contrast to gotoNamedNode(), a "deep search" is always performed.
     ** @param  annotationText  annotation text of the node to be searched for
     *  @param  startFromRoot   flag indicating whether to start from the root node
     *                          or the current one
     ** @return ID of the new current node if successful, 0 otherwise
     */
    virtual size_t gotoAnnotatedNode(const OFString &annotationText,
                                     const OFBool startFromRoot = OFTrue);

    /** set internal cursor to the next annotated node.
     *  Starts from "next" node, i.e. either the first children of the current node
     *  or the first sibling following the current node.
     ** @param  annotationText  annotation text of the node to be searched for
     ** @return ID of the new current node if successful, 0 otherwise
     */
    virtual size_t gotoNextAnnotatedNode(const OFString &annotationText);

    /** check whether specified content item can be added to the current one.
     *  This method can be used to decide which type of content items can be added prior
     *  to really doing so.  Please note that only by-value relationships are supported.
     *  Always returns true if no constraint checker is available but 'relationshipType'
     *  and 'valueType' have valid values.
     ** @param  relationshipType  relationship type of node to be checked with regard to
     *                            the current one
     *  @param  valueType         value type of node to be checked
     *  @param  addMode           flag specifying at which position the new node would
     *                            be added (e.g. after or below the current node)
     ** @return OFTrue if specified node can be added, OFFalse otherwise
     */
    virtual OFBool canAddContentItem(const E_RelationshipType relationshipType,
                                     const E_ValueType valueType,
                                     const E_AddMode addMode = AM_afterCurrent) const;

    /** check whether specified by-reference relationship can be added to the current
     *  content item.
     *  Always returns true if no constraint checker is available but 'relationshipType' and
     *  'targetValueType' have valid values.  The value type DSRTypes::VT_includedTemplate is
     *  never allowed for the target content item.
     ** @param  relationshipType  type of relationship between current and target node
     *  @param  targetValueType   value type of the referenced node (target content item)
     ** @return OFTrue if specified by-reference relationship can be added, OFFalse otherwise
     */
    virtual OFBool canAddByReferenceRelationship(const E_RelationshipType relationshipType,
                                                 const E_ValueType targetValueType) const;

    /** add specified content item to the current one.
     *  If possible, this method creates a new node as specified and adds it to the current
     *  one.  The method canAddContentItem() is called internally to check parameters first.
     *  If the node could be added successfully, the cursor is set to it automatically.
     ** @param  relationshipType  relationship type of node to be added with regard to
     *                            the current one
     *  @param  valueType         value type of node to be added
     *  @param  addMode           flag specifying at which position to add the new node
     *                            (e.g. after or below the current node)
     ** @return ID of new node if successful, 0 otherwise
     */
    virtual size_t addContentItem(const E_RelationshipType relationshipType,
                                  const E_ValueType valueType,
                                  const E_AddMode addMode = AM_afterCurrent);

    /** add specified content item to the current one.
     *  If possible, this method adds a given new node to the current one.  The method
     *  canAddContentItem() is called internally to check parameters first.  If the
     *  node could be added successfully, the cursor is set to it automatically.
     *  Please note that no copy of the given node is created.  Therefore, the node
     *  has to be created with new() or with DSRTypes::createDocumentTreeNode() - do
     *  not use a reference to a local variable and do not delete it a second time.
     ** @param  node          pointer to the new node to be added (should not be empty).
     *                        Reference remains valid after successful insertion.
     *  @param  addMode       flag specifying at which position to add the new node
     *                        (e.g. after or below the current node)
     *  @param  deleteIfFail  flag specifying whether to delete the given 'node' if
     *                        adding fails.  By default, the item is not deleted, i.e.
     *                        in case of error it has to be deleted by the caller.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition addContentItem(DSRDocumentTreeNode *node,
                                       const E_AddMode addMode = AM_afterCurrent,
                                       const OFBool deleteIfFail = OFFalse);

    /** add specified content item after the current one.
     *  If possible, this method creates a new node as specified and adds it after the current
     *  one, i.e. on the same level.  Internally, the first variant of the addContentItem()
     *  method is called with the third parameter being DSRTypes::AM_afterCurrent.  If
     *  successful, the given concept name is set for the new node, and the cursor is updated.
     *  @note This is a convenience function that avoids calling several other functions.
     ** @param  relationshipType  relationship type of node to be added with regard
     *                            to the current one
     *  @param  valueType         value type of node to be added
     *  @param  conceptName       concept name of the node to be added
     *  @param  check             if enabled, check 'conceptName for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition addContentItem(const E_RelationshipType relationshipType,
                                       const E_ValueType valueType,
                                       const DSRCodedEntryValue &conceptName,
                                       const OFBool check = OFTrue);

    /** add specified content item below the current one.
     *  If possible, this method creates a new node as specified and adds it below the current
     *  one, i.e. as a child.  Internally, the first variant of the addContentItem() method
     *  is called with the third parameter being DSRTypes::AM_belowCurrent.  If successful,
     *  the given concept name is set for the new node, and the cursor is updated.
     *  @note This is a convenience function that avoids calling several other functions.
     ** @param  relationshipType  relationship type of node to be added with regard
     *                            to the current one
     *  @param  valueType         value type of node to be added
     *  @param  conceptName       concept name of the node to be added
     *  @param  check             if enabled, check 'conceptName for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition addChildContentItem(const E_RelationshipType relationshipType,
                                            const E_ValueType valueType,
                                            const DSRCodedEntryValue &conceptName,
                                            const OFBool check = OFTrue);

    /** add specified by-reference relationship to the current content item.
     *  If possible, this method creates a new pseudo-node (relationship) and adds it to the
     *  current one.  The method canAddByReferenceRelationship() is called internally to check
     *  parameters first.  The internal cursor is automatically re-set to the current node.
     ** @param  relationshipType  relationship type between current and referenced node
     *  @param  referencedNodeID  ID of the referenced node (target content item)
     ** @return ID of new pseudo-node if successful, 0 otherwise
     */
    virtual size_t addByReferenceRelationship(const E_RelationshipType relationshipType,
                                              const size_t referencedNodeID);

    /** update the position strings used to encode by-reference relationships (if any).
     *  Internally, this method calls checkByReferenceRelationships() with the 'mode'
     *  parameter being DSRTypes::CM_updatePositionString.  It should be called before
     *  this subtree is cloned in order to make sure that the by-reference relationships
     *  (if any) still work on the cloned subtree.  This method should also be called
     *  before accessing the position string of a referenced content item, see
     *  DSRByReferenceTreeNode::getReferencedContentItem().
     ** @param  updateIncludedTemplates  optional flag indicating whether to also update
     *                                   the subtrees managed by included sub-templates
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition updateByReferenceRelationships(const OFBool updateIncludedTemplates = OFFalse);

    /** check whether specified subtree can be inserted at the current position, i.e.\ added
     *  to the current content item.  Internally, the method canAddContentItem() is used for
     *  all top-level nodes of the document subtree.  In addition, if a constraint checker
     *  is available, the remaining nodes of the given subtree are also checked for their
     *  compliance with the relationship content constraints of the underlying SR IOD.
     ** @param  tree            pointer to new subtree to be inserted (should not be empty)
     *  @param  addMode         flag specifying at which position the new subtree would
     *                          be added (e.g. after or below the current node)
     *  @param  defaultRelType  default relationship type between the top-level nodes of
     *                          the given subtree and the current node.  This relationship
     *                          type is used if the one of a top-level node is "unknown".
     ** @return OFTrue if specified subtree can be inserted, OFFalse otherwise
     */
    virtual OFBool canInsertSubTree(const DSRDocumentSubTree *tree,
                                    const E_AddMode addMode = AM_belowCurrent,
                                    const E_RelationshipType defaultRelType = RT_unknown) const;

    /** insert specified subtree to this tree, i.e.\ add it to the current content item.
     *  If possible, this method adds a given new subtree to the current content item.
     *  The method canInsertSubTree() is called internally to check the parameters first.
     *  Please note that no copy of the given subtree is created.  Therefore, the subtree
     *  has to be created with new() or with cloneSubTree() - do not use a reference to a
     *  local variable and do not delete it a second time.
     ** @param  tree            pointer to new subtree to be inserted (should not be empty).
     *                          Reference becomes invalid after successful insertion!
     *  @param  addMode         flag specifying at which position to add the new subtree
     *                          (e.g. after or below the current node)
     *  @param  defaultRelType  default relationship type between the top-level nodes of
     *                          the given subtree and the current node.  This relationship
     *                          type is used if the one of a top-level node is "unknown".
     *  @param  deleteIfFail    flag specifying whether to delete the given 'tree' if
     *                          adding fails.  By default, the tree is not deleted, i.e.
     *                          in case of error it has to be deleted by the caller.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition insertSubTree(DSRDocumentSubTree *tree,
                                      const E_AddMode addMode = AM_belowCurrent,
                                      const E_RelationshipType defaultRelType = RT_unknown,
                                      const OFBool deleteIfFail = OFFalse);

    /** extract a subtree i.e.\ a fragment from this tree.
     *  The subtree is specified by the current node, which becomes the root of the subtree.
     *  In contrast to cloneSubTree(), this method also makes sure that the by-reference
     *  relationships are preserved (as long as both source and target node are contained
     *  in the same tree).  Please note that the returned subtree has to be deleted by the
     *  caller if it is not inserted into the document tree using insertSubTree().
     ** @return pointer to the extracted subtree, NULL in case of error
     */
    virtual DSRDocumentSubTree *extractSubTree();

    /** remove current content item from tree.
     *  Please note that not only the specified node but also all of its child nodes are
     *  removed from the tree and then deleted.  The internal cursor is set automatically
     *  to a new valid position.
     ** @return ID of the node which became the current one after deletion, 0 if an error
     *    occurred or the tree is now empty.
     */
    virtual size_t removeCurrentContentItem();

    /** remove a subtree from this tree.
     *  The subtree to be removed (and deleted) is either specified by the current node or by
     *  the node with the given ID.  Afterwards, the internal cursor is set automatically to
     *  a new valid position.  It would be an error to remove a subtree from an empty tree.
     ** @param  searchID  ID of the root node specifying the subtree to be removed.
     *                    By default (0), the current node is used.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition removeSubTree(const size_t searchID = 0);

    /** clone the current tree node.
     *  Internally, the copy constructor of the respective tree node class is used, so the
     *  corresponding comments apply.  Please note that the returned tree node has to be
     *  deleted by the caller if it is not added to the document tree using addContentItem().
     ** @return pointer to a copy of the current tree node, NULL in case of error
     */
    virtual DSRDocumentTreeNode *cloneCurrentTreeNode() const;

    /** clone a subtree i.e.\ a fragment of this tree.
     *  The cloning starts with the current node and ends with the given node.
     *  Please note that the returned subtree has to be deleted by the caller if it is not
     *  inserted into the document tree using insertSubTree().
     ** @param  stopAfterNodeID  ID of the node after which the cloning should stop.
     *                           By default (0), the process ends after cloning the
     *                           current node with all of its child nodes (if any).
     ** @return pointer to a copy of the specified subtree, NULL in case of error
     */
    virtual DSRDocumentSubTree *cloneSubTree(const size_t stopAfterNodeID = 0) const;

    /** created an expanded version of this (sub)tree.
     *  Expanded means that no instance of DSRIncludedTemplateTreeNode will exist in the new
     *  document tree, i.e. all of them are replaced by their content (subtree).  Please note
     *  that the returned subtree has to be deleted by the caller if it is not inserted into
     *  the document tree using insertSubTree().
     ** @param  tree  variable that will store the pointer to the new subtree
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createExpandedSubTree(DSRDocumentSubTree *&tree) const;

    /** compare template identification of the root node with given values.
     *  Please note that the comparison only takes place if there is a single node at the
     *  root of the tree and its value type is CONTAINER.
     ** @param  templateIdentifier  template identifier to compare with
     *  @param  mappingResource     mapping resource that defines the template
     *  @param  mappingResourceUID  uniquely identifies the mapping resource (optional).
     *                              Not used for comparison if the value is empty.
     ** @result OFTrue if template identification can be compared and the values are
     *          identical, OFFalse otherwise
     */
    virtual OFBool compareTemplateIdentification(const OFString &templateIdentifier,
                                                 const OFString &mappingResource,
                                                 const OFString &mappingResourceUID = "") const;

    /** get template identifier and mapping resource from the root node of this tree.  See
     *  DSRDocumentTreeNode::getTemplateIdentification() for details on template identification.
     *  Please note that the template identification is only retrieved if there is a single node
     *  at the root of the tree and its value type is CONTAINER.
     ** @param  templateIdentifier  identifier of the template (might be empty)
     *  @param  mappingResource     mapping resource that defines the template (might be empty)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTemplateIdentification(OFString &templateIdentifier,
                                                  OFString &mappingResource) const;

    /** get template identifier, mapping resource and optional mapping resource UID from the
     *  root node of this tree.  See DSRDocumentTreeNode::getTemplateIdentification() for
     *  details on template identification.
     *  Please note that the template identification is only retrieved if there is a single node
     *  at the root of the tree and its value type is CONTAINER.
     ** @param  templateIdentifier  identifier of the template (might be empty)
     *  @param  mappingResource     mapping resource that defines the template (might be empty)
     *  @param  mappingResourceUID  uniquely identifies the mapping resource (might be empty)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTemplateIdentification(OFString &templateIdentifier,
                                                  OFString &mappingResource,
                                                  OFString &mappingResourceUID) const;

    /** set template identifier and mapping resource to the root node of this tree.
     *  The identification is valid if the first two values are either present (non-empty) or
     *  all three values are absent (empty).  See DSRDocumentTreeNode::getTemplateIdentification()
     *  for details.
     *  Please note that the template identification is only set if there is a single node at
     *  the root of the tree and its value type is CONTAINER.
     ** @param  templateIdentifier  identifier of the template to be set
     *  @param  mappingResource     mapping resource that defines the template
     *  @param  mappingResourceUID  uniquely identifies the mapping resource (optional)
     *  @param  check               check 'templateIdentifier', 'mappingResource' and
     *                              'mappingResourceUID' for conformance with VR (CS,UI) and
     *                              VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTemplateIdentification(const OFString &templateIdentifier,
                                                  const OFString &mappingResource,
                                                  const OFString &mappingResourceUID = "",
                                                  const OFBool check = OFTrue);


  protected:

    /** special constructor that receives a pointer to the root node.
     *  Please note that the 'rootNode' and the associated tree is not copied!
     ** @param  rootNode  pointer to the root node of the "new" tree
     */
    DSRDocumentSubTree(DSRDocumentTreeNode *rootNode);

    /** special copy constructor that clones a particular subtree only
     ** @param  startCursor      first node of the subtree to be copied
     *  @param  stopAfterNodeID  ID of the node after which the cloning should stop
     */
    DSRDocumentSubTree(const DSRDocumentTreeNodeCursor &startCursor,
                       size_t stopAfterNodeID);

    /** fast, non-throwing swap function.
     *  The time complexity of this function is constant.
     ** @param  tree  subtree to swap with
     */
    void swap(DSRDocumentSubTree &tree);

    /** get pointer to current node.
     *  Hide this inherited method from the public interface.
     ** @return pointer to current node (might be NULL)
     */
    virtual DSRDocumentTreeNode *getNode() const;

    /** add new node to the current one.
     *  Please note that no copy of the given node is created.  Therefore, the node
     *  has to be created with new() - do not use a reference to a local variable.
     *  If the node could be added successfully, the cursor is set to it automatically.
     ** @param  node     pointer to the new node to be added
     *  @param  addMode  flag specifying at which position to add the new node
     *                   (e.g. after or below the current node)
     ** @return ID of the new node if successful, 0 otherwise
     */
    virtual size_t addNode(DSRDocumentTreeNode *node,
                           const E_AddMode addMode = AM_afterCurrent);

    /** replace current node by the given one.
     *  Please note that no copy of the given node is created.  Therefore, the node
     *  should be created with new() - do not use a reference to a local variable.  If
     *  the node could be replaced successfully, the "old" node (and all of its child
     *  nodes) are deleted, and the cursor is set to the new one.
     ** @param  node  pointer to the new node to replace the current one
     ** @return ID of the new node if successful, 0 otherwise
     */
    virtual size_t replaceNode(DSRDocumentTreeNode *node);

    /** extract current node from tree.
     *  Please note that not only the specified node but also all of its child nodes are
     *  extracted from the tree.  The cursor is set automatically to a new valid position.
     ** @return pointer to extracted node, NULL in case of error (or the tree was empty)
     */
    virtual DSRDocumentTreeNode *extractNode();

    /** get pointer to root node and "forget" the internal reference to this node.
     *  In other words: after calling this method, the stored tree will be empty.
     *  This also means that the caller is responsible for deleting the allocated memory.
     ** @return pointer to root node, might be NULL (empty tree)
     */
    virtual DSRDocumentTreeNode *getAndRemoveRootNode();

    /** remove current node from tree.
     *  Please note that not only the specified node but also all of his child nodes are
     *  removed from the tree and deleted afterwards.  The cursor is set automatically to
     *  a new valid position.
     ** @return ID of the node which became the current one after deletion, 0 if an error
     *          occurred or the tree is now empty.
     */
    virtual size_t removeNode();

    /** include specified sub-template, i.e.\ add a new DSRIncludedTemplateTreeNode, which
     *  references this template, to the current content item.
     *  Please note that no checks are performed that would make sure that the template
     *  with its top-level nodes can actually be added, e.g. by using an IOD constraint
     *  checker.  This is also the reason why this method is "protected" and not "public",
     *  i.e. it can only be used by derived classes, e.g. DSRSubTemplate and its children.
     ** @param  subTemplate     shared pointer to a sub-template that should be included
     *                          into this tree (at the current position)
     *  @param  addMode         flag specifying at which position to add the 'subTemplate'
     *                          (e.g. after or below the current node)
     *  @param  defaultRelType  default relationship type that will be used when the
     *                          subtree that is managed by the 'subTemplate' is inserted
     *                          into this tree and the relationship type of one of the
     *                          top-level nodes is "unknown".  Also see documentation of
     *                          createExpandedSubTree().
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition includeTemplate(const DSRSharedSubTemplate &subTemplate,
                                        const E_AddMode addMode = AM_belowCurrent,
                                        const E_RelationshipType defaultRelType = RT_unknown);

    /** expand all "included template" content items in a given (sub)tree.
     *  Expanding means that all instances of DSRIncludedTemplateTreeNode are replaced by
     *  their content (subtree).
     *  Please note that the internal cursor of the given 'tree' is set to the root node
     *  if no error occurred.  Otherwise, the cursor points to the content item that
     *  caused the problem.
     ** @param  tree  pointer to the subtree that should be expanded
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition expandIncludedTemplates(DSRDocumentSubTree *tree) const;

    /** check the by-reference relationships (if any) for validity.
     *  This function checks whether all by-reference relationships possibly contained
     *  in the document tree are valid according to the following restrictions: source
     *  and target node are not identical and the target node is not an ancestor of the
     *  source node (requirement from the DICOM standard to prevent loops -> directed
     *  acyclic graph, though this is not 100% true - see "reportlp.dcm" example).
     *  In addition, the position strings (used to encode by-reference relationships
     *  according to the DICOM standard) OR the node IDs (used internally to uniquely
     *  identify nodes) can be updated.
     *  Please note that the checking modes DSRTypes::CM_updatePositionString and
     *  DSRTypes::CM_updateNodeID are mutually exclusive.
     ** @tparam  T_Cursor  template type used for the cursor iterating the document (sub)tree
     ** @param   mode      mode used to customize the checking process (see DSRTypes::CM_xxx)
     *  @param   flags     flag used to customize the reading (see DSRTypes::RF_xxx) and/or
     *                     printing process (see DSRTypes::PF_xxx).  Conflicting definitions
     *                     are avoided using the appropriate bit mask (see DSRTypes::CB_xxx).
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    template <typename T_Cursor>
    OFCondition checkByReferenceRelationships(const size_t mode = 0,
                                              const size_t flags = 0);

    /** reset flag for all content items whether they are target of a by-reference relationship.
     *  This function calls 'setReferenceTarget(OFFalse)' for all content items in the tree.
     */
    virtual void resetReferenceTargetFlag();

    /** update the tree for subsequent output, e.g.\ for being printed or added to an SR
     *  document.  By default, this virtual function does nothing but is called automatically
     *  by the affected output methods and should be overwritten in derived classes.
     */
    virtual void updateTreeForOutput();

    /** check whether the given subtree complies with the constraints of the given checker
     ** @param  tree     pointer to subtree that should be checked
     *  @param  checker  pointer to relationship content constraints checker to be used.
     *                   If NULL, no checks are performed by this method.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition checkSubTreeConstraints(const DSRDocumentSubTree *tree,
                                                const DSRIODConstraintChecker *checker) const;

    /// check relationship content constraints of the associated IOD
    DSRIODConstraintChecker *ConstraintChecker;


  private:

    /// current content item.  Introduced to avoid the external use of pointers.
    DSRContentItem CurrentContentItem;
};


#endif
