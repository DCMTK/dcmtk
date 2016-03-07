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
 *    classes: DSRDocumentTree
 *
 */


#ifndef DSRDOCTR_H
#define DSRDOCTR_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdocst.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class managing the SR document tree.
 *  This tree structure consists of content items (nodes) that are related to each other.
 *  It is called "SR Document Content Tree" in the DICOM standard.  There is always a
 *  single node at the root, which has the value type CONTAINER.  Depending on the
 *  document type, certain restrictions apply on which value and relationship types are
 *  allowed.  The rules are specified and checked by a so-called IOD constraint checker.
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTree
  : public DSRDocumentSubTree
{

  public:

    /** constructor
     ** @param  documentType  document type of the associated document
     */
    DSRDocumentTree(const E_DocumentType documentType);

    /** copy constructor.
     *  Please note that the internal cursor is not copied but reset, i.e. set to the
     *  root node.
     ** @param  tree  tree to be copied
     */
    DSRDocumentTree(const DSRDocumentTree &tree);

    /** destructor
     */
    virtual ~DSRDocumentTree();

    /** assignment operator.
     *  Please note that internally the copy constructor is used, so the same comments
     *  apply.
     ** @param  tree  tree to be copied
     ** @return reference to this tree after copying
     */
    DSRDocumentTree &operator=(DSRDocumentTree tree);

    /** clone this tree.
     *  Internally, the copy constructor is used, so the corresponding comments apply.
     ** @return copy of this tree
     */
    virtual DSRDocumentTree *clone() const;

    /** clear internal member variables.
     *  The document type is not changed (e.g. set to DSRTypes::DT_invalid).
     */
    virtual void clear();

    /** check whether the current internal state is valid.
     *  The SR document is valid if the document type is supported, the tree is not
     *  empty, the root item is a container and has the internal relationship type
     *  DSRTypes::RT_isRoot.  Also see DSRDocumentSubTree::isValidDocumentTree().
     ** @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** print current SR document tree to specified output stream
     ** @param  stream  output stream
     *  @param  flags   optional flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags = 0);

    /** read SR document tree from DICOM dataset.
     *  Please note that the current document tree is also deleted if the reading fails.
     *  If logging is enabled, the reason for any error might be obtained from the
     *  error/warning output.
     ** @param  dataset       reference to DICOM dataset where the tree should be read from
     *  @param  documentType  document type of the SR document from which the tree is read
     *  @param  flags         flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmItem &dataset,
                             const E_DocumentType documentType,
                             const size_t flags = 0);

    /** write current SR document tree to DICOM dataset.
     *  Please note that included (non-expanded) sub-templates are not supported.
     ** @param  dataset      reference to DICOM dataset where the current tree should be
     *                       written to.  The 'dataset' is not cleared before writing to it!
     *  @param  markedItems  optional stack where pointers to all 'marked' content items
     *                       (DICOM datasets/items) are added to during the write process.
     *                       Can be used to digitally sign parts of the document tree.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmItem &dataset,
                              DcmStack *markedItems = NULL);

    /** read XML document tree
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXML(const DSRXMLDocument &doc,
                                DSRXMLCursor cursor,
                                const size_t flags);

    /** render current SR document tree in HTML/XHTML format.
     *  Please note that included (non-expanded) sub-templates are not supported.
     ** @param  docStream    output stream to which the main HTML/XHTML document is written
     *  @param  annexStream  output stream to which the HTML/XHTML document annex is written
     *  @param  flags        optional flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition renderHTML(STD_NAMESPACE ostream &docStream,
                                   STD_NAMESPACE ostream &annexStream,
                                   const size_t flags = 0);

    /** get document type
     ** @return current document type (might be DSRTypes::DT_invalid)
     */
    inline E_DocumentType getDocumentType() const
    {
        return DocumentType;
    }

    /** change document type.
     *  First, it is checked whether the specified 'documentType' is supported at all.  If so,
     *  the currently stored document tree is either deleted (see 'deleteTree' parameter) or
     *  it is checked whether the tree also complies with the relationship content constraints
     *  of the new SR IOD.  Otherwise the current document remains in force.
     *  Please note that the 'documentType' is not compared with the type of the currently
     *  stored document tree, i.e. the above described process is always performed, even if
     *  the document type does not change.
     ** @param  documentType  new document type to be set (should not be DSRTypes::DT_invalid)
     *  @param  deleteTree    delete the currently stored document tree if OFTrue.
     *                        Otherwise, it is checked whether the tree can be preserved.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition changeDocumentType(const E_DocumentType documentType,
                                           const OFBool deleteTree);

    /** check whether specified content item can be added to the current one.
     *  This method can be used to decide which type of content items can be added prior
     *  to really doing so.
     *  If the tree is currently empty, only a CONTAINER with the internal relationship
     *  type DSRTypes::RT_isRoot is allowed (as the new root node).  Always returns true
     *  if no constraint checker is available but 'relationshipType' and 'valueType' have
     *  valid values.
     ** @param  relationshipType  relationship type of node to be checked with regard
     *                            to the current one
     *  @param  valueType         value type of node to be checked
     *  @param  addMode           flag specifying at which position the new node would
     *                            be added (e.g. after or below the current node)
     ** @return OFTrue if specified node can be added, OFFalse otherwise
     */
    virtual OFBool canAddContentItem(const E_RelationshipType relationshipType,
                                     const E_ValueType valueType,
                                     const E_AddMode addMode = AM_afterCurrent) const;

    /** check whether specified subtree can be inserted at the current position, i.e.\ added
     *  to the current content item.
     *  If this tree is currently empty, only a CONTAINER with the internal relationship
     *  type DSRTypes::RT_isRoot is allowed as the new root node (of the given subtree).
     *  In all other cases, the method DSRDocumentSubTree::canInsertSubTree() is called.
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

    /** check whether the document tree complies with the constraints of the given checker.
     *  This method also checks whether the currently stored document tree is either empty
     *  or valid, and whether the template identification of the root node (if any) is as
     *  expected.  However, the latter only results in a warning message to the logger.
     ** @param  checker  pointer to relationship content constraints checker to be used
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition checkDocumentTreeConstraints(DSRIODConstraintChecker *checker);

    /** unmark all content items in the document tree.
     *  Use method DSRDocumentTreeNode::setMark() to mark and unmark a single content item.
     *  Pointers to the DICOM dataset/item of marked content items are added to the optional
     *  stack when calling the DSRDocument::write() method.  This mechanism can e.g. be used
     *  to digitally sign particular content items.
     */
    void unmarkAllContentItems();

    /** remove digital signatures from the document tree.
     *  This method clears the MACParametersSequence and the DigitalSignaturesSequence for
     *  all content items which have been filled during reading.
     */
    void removeSignatures();


  protected:

    /** fast, non-throwing swap function.
     *  The time complexity of this function is constant.
     ** @param  tree  tree to swap with
     */
    void swap(DSRDocumentTree &tree);


  private:

    /// document type of the associated SR document
    E_DocumentType DocumentType;


 // --- declaration of default constructor

    DSRDocumentTree();
};


#endif
