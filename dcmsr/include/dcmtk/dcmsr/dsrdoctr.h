/*
 *
 *  Copyright (C) 2000-2012, OFFIS e.V.
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
 *    classes: DSRDocumentTree
 *
 */


#ifndef DSRDOCTR_H
#define DSRDOCTR_H

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

/** Class managing the SR document tree
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTree
  : public DSRTree
{

  public:

    /** constructor
     ** @param  documentType  document type of the associated document
     */
    DSRDocumentTree(const E_DocumentType documentType);

    /** destructor
     */
    virtual ~DSRDocumentTree();

    /** clear internal member variables.
     *  The document type is not changed (e.g. set to DT_invalid).
     */
    virtual void clear();

    /** check whether the current internal state is valid.
     *  The SR document is valid if the document type is supported, the tree is not
     *  empty the root item is a container and has the internal relationship type
     *  RT_isRoot.
     ** @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** print current SR document tree to specified output stream
     ** @param  stream  output stream
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition print(STD_NAMESPACE ostream &stream,
                      const size_t flags = 0);

    /** read SR document tree from DICOM dataset.
     *  Please note that the current document tree is also deleted if the reading fails.
     *  If the log stream is set and valid the reason for any error might be obtained
     *  from the error/warning output.
     ** @param  dataset       reference to DICOM dataset where the tree should be read from
     *  @param  documentType  document type of the SR document from which the tree is read
     *  @param  flags         flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset,
                     const E_DocumentType documentType,
                     const size_t flags = 0);

    /** write current SR document tree to DICOM dataset
     ** @param  dataset      reference to DICOM dataset where the current tree should be
     *                       written to.  The 'dataset' is not cleared before writing to it!
     *  @param  markedItems  optional stack where pointers to all 'marked' content items
     *                       (DICOM datasets/items) are added to during the write process.
     *                       Can be used to digitally sign parts of the document tree.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition write(DcmItem &dataset,
                      DcmStack *markedItems = NULL);

    /** read XML document tree
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   optional flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXML(const DSRXMLDocument &doc,
                        DSRXMLCursor cursor,
                        const size_t flags);

    /** write current SR document tree in XML format
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeXML(STD_NAMESPACE ostream &stream,
                         const size_t flags);

    /** render current SR document tree in HTML/XHTML format
     ** @param  docStream    output stream to which the main HTML/XHTML document is written
     *  @param  annexStream  output stream to which the HTML/XHTML document annex is written
     *  @param  flags        flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition renderHTML(STD_NAMESPACE ostream &docStream,
                           STD_NAMESPACE ostream &annexStream,
                           const size_t flags = 0);

    /** get document type
     ** return current document type (might be DT_invalid)
     */
    E_DocumentType getDocumentType() const
    {
        return DocumentType;
    }

    /** change document type.
     *  Please note that the document tree is deleted if the specified 'documentType'
     *  is supported.  Otherwise the current document remains in force.
     ** @param  documentType  new document type to be set (should be != DT_invalid)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition changeDocumentType(const E_DocumentType documentType);

    /** check whether specified content item can be added to the current one.
     *  If the tree is currently empty only a CONTAINER with the internal relationship
     *  type RT_isRoot is allowed (as the new root node).  Always returns true if no
     *  constraint checker is available.  This method can be used to decide which type
     *  of content items can be added prior to really do so.
     ** @param  relationshipType  relationship type of node to be checked with regard
     *                            to the current one
     *  @param  valueType         value type of node to be checked
     *  @param  addMode           flag specifying at which position the new node would
     *                            be added.
     *                            (AM_afterCurrent, AM_beforeCurrent, AM_belowCurrent)
     ** @return OFTrue if specified node can be added, OFFalse otherwise
     */
    OFBool canAddContentItem(const E_RelationshipType relationshipType,
                             const E_ValueType valueType,
                             const E_AddMode addMode = AM_afterCurrent);

    /** check whether specified by-reference relationship can be added to the current
     *  content item.  Always returns true if no constraint checker is available.
     ** @param  relationshipType  type of relationship between current and target node
     *  @param  targetValueType   value type of the referenced target node
     ** @return OFTrue if specified by-reference relationship can be added, OFFalse otherwise
     */
    OFBool canAddByReferenceRelationship(const E_RelationshipType relationshipType,
                                         const E_ValueType targetValueType);

    /** add specified content item to the current one.
     *  If possible this method creates a new node as specified and adds it to the current
     *  one.  The method canAddContentItem() is called internally to check parameters first.
     ** @param  relationshipType  relationship type of node to be added with regard
     *                            to the current one
     *  @param  valueType         value type of node to be added
     *  @param  addMode           flag specifying at which position to add the new node.
     *                            (AM_afterCurrent, AM_beforeCurrent, AM_belowCurrent)
     ** @return ID of new node if successful, 0 otherwise
     */
    size_t addContentItem(const E_RelationshipType relationshipType,
                          const E_ValueType valueType,
                          const E_AddMode addMode = AM_afterCurrent);

    /** add specified by-reference relationship to the current content item.
     *  If possible this method creates a new pseudo-node (relationship) and adds it to the
     *  current one.  The method canAddByReferenceRelationship() is called internally to check
     *  parameters first.  The internal cursor is automatically re-set to the current node.
     ** @param  relationshipType  relationship type between current and referenced node
     *  @param  referencedNodeID  node ID of the referenced content item
     ** @return ID of new pseudo-node if successful, 0 otherwise
     */
    size_t addByReferenceRelationship(const E_RelationshipType relationshipType,
                                      const size_t referencedNodeID);

    /** remove current content item from tree.
     *  Please note that not only the specified node but also all of its child nodes are
     *  removed from the tree and then deleted.  The internal cursor is set automatically
     *  to a new valid position.
     ** @return ID of the node which became the current one after deletion, 0 if an error
     *    occured or the tree is now empty.
     */
    size_t removeCurrentContentItem();

    /** get reference to current content item.
     *  This mechanism allows to access all content items without using pointers.
     ** @return reference to current content item (might be invalid)
     */
    DSRContentItem &getCurrentContentItem();

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
    size_t gotoNamedNode(const DSRCodedEntryValue &conceptName,
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
    size_t gotoNextNamedNode(const DSRCodedEntryValue &conceptName,
                             const OFBool searchIntoSub = OFTrue);

    /** unmark all content items in the document tree.
     *  Use method 'setMark' on node-level to mark and unmark a single content item.
     *  Pointers to the DICOM dataset/item of marked content items are added to the optional
     *  stack when calling the 'write' method.  This mechanism can e.g. be used to digitally
     *  sign particular content items.
     */
    void unmarkAllContentItems();

    /** remove digital signatures from the document tree.
     *  This method clears the MACParametersSequence and the DigitalSignaturesSequence for
     *  all content items which have been filled during reading.
     */
    void removeSignatures();


  protected:

    /** add new node to the current one.
     *  Please note that no copy of the given node is created.  Therefore, the node
     *  should be created with new() - do not use a reference to a local variable.
     *  If the node could be added successfully the cursor is set to it automatically.
     ** @param  node     pointer to the new node to be added
     *  @param  addMode  flag specifying at which position to add the new node.
     *                   (AM_afterCurrent, AM_beforeCurrent, AM_belowCurrent)
     ** @return ID of the new added node if successful, 0 otherwise
     */
    virtual size_t addNode(DSRDocumentTreeNode *node,
                           const E_AddMode addMode = AM_afterCurrent);

    /** remove current node from tree.
     *  Please note that not only the specified node but also all of his child nodes are
     *  removed from the tree and deleted afterwards.  The cursor is set automatically to
     *  a new valid position.
     ** @return ID of the node which became the current one after deletion, 0 if an error
     *    occured or the tree is now empty.
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
     *  identify nodes) can be updated.  Please note that the modes 'CM_updatePositionString'
     *  and 'CM_updateNodeID' are mutually exclusive.
     ** @param  mode   mode used to customize the checking process (see DSRTypes::CM_xxx)
     *  @param  flags  flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition checkByReferenceRelationships(const size_t mode = 0,
                                              const size_t flags = 0);

    /** reset flag for all content items whether they are target of a by-reference relationship.
     *  This function calls 'setReferenceTarget(OFFalse)' for all content items.
     */
    void resetReferenceTargetFlag();


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

    /// document type of the associated SR document
    E_DocumentType DocumentType;
    /// current content item.  Introduced to avoid the external use of pointers.
    DSRContentItem CurrentContentItem;
    /// check relationship content constraints of the associated IOD
    DSRIODConstraintChecker *ConstraintChecker;


 // --- declaration of default/copy constructor and assignment operator

    DSRDocumentTree();
    DSRDocumentTree(const DSRDocumentTree &);
    DSRDocumentTree &operator=(const DSRDocumentTree &);
};


#endif
