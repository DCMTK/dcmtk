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
 *    classes: DSRDocumentTreeNode
 *
 */


#ifndef DSRDOCTN_H
#define DSRDOCTN_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtree.h"
#include "dcmtk/dcmsr/dsrcodvl.h"


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

class DSRIODConstraintChecker;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Base class for content items
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNode
  : public DSRTreeNode
{
    // allow direct access to protected methods
    friend class DSRTree<DSRDocumentTreeNode>;
    friend class DSRTreeNodeCursor<DSRDocumentTreeNode, OFFalse>;
    friend class DSRTreeNodeCursor<DSRDocumentTreeNode, OFTrue>;

    // allow access to getConceptNamePtr()
    friend class DSRContentItem;

  public:

    /** constructor.
     *  The 'valueType' can never be changed after the tree node has been created
     *  (therefore, the corresponding member variable is declared "const").
     ** @param  relationshipType  type of relationship to the parent tree node.  Should
     *                            not be DSRTypes::RT_invalid, and DSRTypes::RT_isRoot
     *                            only for the root node.
     *  @param  valueType         value type of the associated content item.  Should not
     *                            be DSRTypes::VT_invalid.
     */
    DSRDocumentTreeNode(const E_RelationshipType relationshipType,
                        const E_ValueType valueType);

    /** copy constructor.
     *  Please note that the member variables of the base class DSRTreeNode are not copied
     *  because the new tree node is not (yet) part of a document tree.  Furthermore, the
     *  following member variables of this class are also not copied but initialized with
     *  their respective default values:
     *  - ReferenceTarget
     *  - MACParameters
     *  - DigitalSignatures
     *
     ** @param  node  tree node to be copied
     */
    DSRDocumentTreeNode(const DSRDocumentTreeNode &node);

    /** destructor
     */
    virtual ~DSRDocumentTreeNode();

    /** clone this tree node (abstract).
     *  Internally, the copy constructor is used, so the corresponding comments apply.
     ** @return copy of this tree node
     */
    virtual DSRDocumentTreeNode *clone() const = 0;

    /** clear all member variables.
     *  This does not apply to the relationship and value type since they are never changed.
     */
    virtual void clear();

    /** check whether the content item is valid.
     *  The content item is valid if the relationship type and the value type are both not invalid.
     ** @return OFTrue if tree node is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the value of the content item is valid.  See derived classes for details.
     ** @return OFTrue if the value is valid, OFFalse otherwise
     */
    virtual OFBool hasValidValue() const;

    /** check whether the content is short.
     *  This method is used to check whether the rendered output of this content item can be
     *  expanded inline or not (used for renderHTML()).  This base class always returns OFTrue.
     ** @param  flags  flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return OFTrue if the content is short, OFFalse otherwise
     */
    virtual OFBool isShort(const size_t flags) const;

    /** check whether template identification is set
     ** @return OFTrue if template identification is set, OFFalse otherwise
     */
    virtual OFBool hasTemplateIdentification() const;

    /** print content item.
     *  The output of a content item depends on its value type.  This general method prints
     *  only those parts which all derived classes (= value types) do have in common, i.e. the
     *  type of relationship, the value type and the (optional) concept name.
     *  A typical output looks like this: has concept mod CODE: (,,"Concept")
     ** @param  stream  output stream to which the content item should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags) const;

    /** print extended information on the content item.
     *  The following details are printed (if present and output is enabled): observation
     *  date/time (in curly brackets), annotation text (in quotation marks) and template
     *  identification (after a hash mark).  This method is intended to be called after
     *  the general print() method, e.g. like it is done by DSRDocumentSubTree::print().
     ** @param  stream  output stream to which the extended information should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition printExtended(STD_NAMESPACE ostream &stream,
                                      const size_t flags) const;

    /** read content item from dataset.
     *  A number of readXXX() methods are called (see "protected" part) in order to retrieve all
     *  possibly nested content items from the dataset.
     ** @param  dataset            DICOM dataset from which the content item should be read
     *  @param  constraintChecker  checks relationship content constraints of the associated IOD
     *  @param  flags              flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmItem &dataset,
                             const DSRIODConstraintChecker *constraintChecker,
                             const size_t flags);

    /** write content item to dataset.
     *  A number of writeXXX() methods are called (see "protected" part) in order to write all
     *  possibly nested content items to the dataset.
     ** @param  dataset      DICOM dataset to which the content item should be written
     *  @param  markedItems  optional stack where pointers to all 'marked' content items
     *                       (DICOM datasets/items) are added to during the write process.
     *                       Can be used to digitally sign parts of the document tree.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmItem &dataset,
                              DcmStack *markedItems = NULL);

    /** read general XML document tree node data
     ** @param  doc           document containing the XML file content
     *  @param  cursor        cursor pointing to the starting node
     *  @param  documentType  type of the document to be read (used for debug output only)
     *  @param  flags         flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXML(const DSRXMLDocument &doc,
                                DSRXMLCursor cursor,
                                const E_DocumentType documentType,
                                const size_t flags);

    /** write content item in XML format
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &stream,
                                 const size_t flags) const;

    /** render content item in HTML/XHTML format.
     *  After rendering the current content item all child nodes (if any) are also rendered (see
     *  renderHTMLChildNodes() for details).
     ** @param  docStream     output stream to which the main HTML/XHTML document is written
     *  @param  annexStream   output stream to which the HTML/XHTML document annex is written
     *  @param  nestingLevel  current nesting level.  Used to render section headings.
     *  @param  annexNumber   reference to the variable where the current annex number is stored.
     *                        Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags         flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition renderHTML(STD_NAMESPACE ostream &docStream,
                                   STD_NAMESPACE ostream &annexStream,
                                   const size_t nestingLevel,
                                   size_t &annexNumber,
                                   const size_t flags) const;

    /** check whether content item is digitally signed.
     *  A content item is signed if the DigitalSignaturesSequence exists.  This sequence is read
     *  from the dataset if present and the 'signature' flag for the 'read' method is turned on.
     ** @return OFTrue if content item is signed, OFFalse otherwise
     */
    inline OFBool isSigned()
    {
        return !DigitalSignatures.isEmpty();
    }

    /** check whether content item is marked.
     *  Use method setMark() to mark and unmark the current content item.
     *  Pointers to the DICOM dataset/item of marked content items are added to the optional
     *  stack when calling the DSRDocument::write() method.  This mechanism can e.g. be used
     *  to digitally sign particular content items.
     ** @return OFTrue if content item is marked, OFFalse otherwise
     */
    inline OFBool isMarked() const
    {
        return MarkFlag;
    }

    /** mark/unmark the current content item.
     *  See explanation for method isMarked() for details.
     *  @param  flag  mark item if OFTrue, unmark otherwise
     */
    inline void setMark(const OFBool flag)
    {
        MarkFlag = flag;
    }

    /** check whether the current content item is target of a by-reference relationship
     ** @return OFTrue if the content item is target, OFFalse otherwise
     */
    inline OFBool isReferenceTarget() const
    {
        return ReferenceTarget;
    }

    /** specify whether the current content item is target of a by-reference relationship
     ** @param  isTarget  OFTrue if the content item is target (default), OFFalse otherwise
     */
    inline void setReferenceTarget(const OFBool isTarget = OFTrue)
    {
        ReferenceTarget = isTarget;
    }

    /** check whether the current content item has any children
     ** @return OFTrue if there are any child nodes, OFFalse otherwise
     */
    inline OFBool hasChildNodes() const
    {
        return (getDown() != NULL);
    }

    /** check whether the current content item has any siblings
     ** @return OFTrue if there are any sibling nodes, OFFalse otherwise
     */
    inline OFBool hasSiblingNodes() const
    {
        return (getPrev() != NULL) || (getNext() != NULL);
    }

    /** get ID of the current tree node
     ** @return ID of the current tree node (should never be 0)
     */
    inline size_t getNodeID() const
    {
        return getIdent();
    }

    /** get relationship type of the current content item
     ** @return relationship type of the current content item (might be DSRTypes::RT_invalid)
     */
    inline E_RelationshipType getRelationshipType() const
    {
        return RelationshipType;
    }

    /** set relationship type of the current content item (if previously unknown).
     *  Please note that changing the relationship type (which was originally passed to the
     *  constructor of this class) only works if the current value is DSRTypes::RT_unknown.
     *  This is needed for inserting document subtrees where the top-level nodes might have an
     *  "unknown" relationship to the parent node (see DSRDocumentSubTree::insertSubTree()).
     ** @param  relationshipType  type of relationship to the parent tree node.  Should not be
     *                            DSRTypes::RT_invalid or DSRTypes::RT_unknown.  Be careful
     *                            with DSRTypes::RT_isRoot, use it for root nodes only.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRelationshipType(const E_RelationshipType relationshipType);

    /** get value type of the current content item
     ** @return value type of the current content item (might be DSRTypes::VT_invalid)
     */
    inline E_ValueType getValueType() const
    {
        return ValueType;
    }

    /** get reference to the concept name
     ** @return reference to the concept name (code, might be empty/invalid)
     */
    inline const DSRCodedEntryValue &getConceptName() const
    {
        return ConceptName;
    }

    /** get copy of the concept name.
     *  Code describing the concept represented by this content item.  Also conveys the value
     *  of document title and section headings in documents.
     ** @param  conceptName  reference to a variable where the code should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getConceptName(DSRCodedEntryValue &conceptName) const;

    /** set the concept name.
     *  Code describing the concept represented by this content item.  Also conveys the value
     *  of document title and section headings in documents.
     *  If the new code is invalid the current one is not replaced.  An empty code can
     *  be used to clear the current concept name.
     ** @param  conceptName  code to be set as the new concept name (checked before set)
     *  @param  check        check 'conceptName' for validity if enabled.  See
     *                       DSRCodedEntryValue::checkCode() for details.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setConceptName(const DSRCodedEntryValue &conceptName,
                                       const OFBool check = OFTrue);

    /** get observation date/time.
     *  This is the date and time on which this content item was completed.  Might be empty
     *  if the date and time do not differ from the content date and time, see DSRDocument.
     ** @return observation date/time of current content item (might be empty/invalid)
     */
    inline const OFString &getObservationDateTime() const
    {
        return ObservationDateTime;
    }

    /** set observation date/time.
     *  This is the date and time on which this content item was completed.  Might be empty
     *  if the date and time do not differ from the content date and time, see DSRDocument.
     ** @param  observationDateTime  value to be set (might be an empty string)
     *  @param  check                check 'observationDateTime' for conformance with VR (DT)
     *                               and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setObservationDateTime(const OFString &observationDateTime,
                                               const OFBool check = OFTrue);

    /** set observation date/time from element.
     *  This is the date and time on which this content item was completed.  Might be empty
     *  if the date and time do not differ from the content date and time, see DSRDocument.
     ** @param  delem  DICOM element from which the date/time value should be retrieved
     *  @param  pos    index of the value in case of multi-valued elements (0..vm-1)
     *  @param  check  check date/time value for conformance with VR (DT) and VM (1) if
     *                 enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setObservationDateTime(const DcmElement &delem,
                                               const unsigned long pos = 0,
                                               const OFBool check = OFTrue);

    /** set observation date/time from dataset.
     *  This is the date and time on which this content item was completed.  Might be empty
     *  if the date and time do not differ from the content date and time, see DSRDocument.
     ** @param  dataset  DICOM dataset from which the date/time value should be retrieved
     *  @param  tagKey   DICOM tag specifying the attribute from which the value should be
     *                   retrieved.  The search is limited to the top-level of the dataset.
     *  @param  pos      index of the value in case of multi-valued elements (0..vm-1)
     *  @param  check    check date/time value for conformance with VR (DT) and VM (1) if
     *                   enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setObservationDateTime(DcmItem &dataset,
                                               const DcmTagKey &tagKey,
                                               const unsigned long pos = 0,
                                               const OFBool check = OFTrue);

    /** get observation unique identifier.
     *  The UID represents the semantic content of the observation; an encoding of the same
     *  observation with the same context into another representation may use the same UID.
     ** @return observation unique identifier of current content item (might be empty/invalid)
     */
    inline const OFString &getObservationUID() const
    {
        return ObservationUID;
    }

    /** set observation unique identifier.
     *  The UID represents the semantic content of the observation; an encoding of the same
     *  observation with the same context into another representation may use the same UID.
     ** @param  observationUID  value to be set (might be an empty string)
     *  @param  check           check 'observationUID' for conformance with VR (UI) and VM (1)
     *                          if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setObservationUID(const OFString &observationUID,
                                          const OFBool check = OFTrue);

    /** compare template identification with given values
     ** @param  templateIdentifier  template identifier to compare with
     *  @param  mappingResource     mapping resource that defines the template
     *  @param  mappingResourceUID  uniquely identifies the mapping resource (optional).
     *                              Not used for comparison if the value is empty.
     ** @result OFTrue if template identification is identical, OFFalse otherwise
     */
    virtual OFBool compareTemplateIdentification(const OFString &templateIdentifier,
                                                 const OFString &mappingResource,
                                                 const OFString &mappingResourceUID = "") const;

    /** get template identifier and mapping resource.
     *  This value pair identifies the template that was used to create this content item
     *  (and its children).  According to the DICOM standard, it is "required if a template
     *  was used to define the content of this Item, and the template consists of a single
     *  CONTAINER with nested content, and it is the outermost invocation of a set of
     *  nested templates that start with the same CONTAINER."  The identification is valid
     *  if both values are either present (non-empty) or absent (empty).
     ** @param  templateIdentifier  identifier of the template (might be empty)
     *  @param  mappingResource     mapping resource that defines the template (might be empty)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTemplateIdentification(OFString &templateIdentifier,
                                                  OFString &mappingResource) const;

    /** get template identifier, mapping resource and optional mapping resource UID.
     *  This value triple identifies the template that was used to create this content item
     *  (and its children).  According to the DICOM standard, it is "required if a template
     *  was used to define the content of this Item, and the template consists of a single
     *  CONTAINER with nested content, and it is the outermost invocation of a set of
     *  nested templates that start with the same CONTAINER."  The identification is valid
     *  if the first two values are either present (non-empty) or all three are absent (empty).
     ** @param  templateIdentifier  identifier of the template (might be empty)
     *  @param  mappingResource     mapping resource that defines the template (might be empty)
     *  @param  mappingResourceUID  uniquely identifies the mapping resource (might be empty)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTemplateIdentification(OFString &templateIdentifier,
                                                  OFString &mappingResource,
                                                  OFString &mappingResourceUID) const;

    /** set template identifier and mapping resource.
     *  The identification is valid if the first two values are either present (non-empty) or
     *  all three values are absent (empty).  See getTemplateIdentification() for details.
     *  A warning message is reported to the logger if a template identification is specified
     *  for a content item that is not a CONTAINER.
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

    /** remove digital signatures from content item.
     *  This method clears the MACParametersSequence and the DigitalSignaturesSequence for
     *  the current content item which have been filled during reading.
     */
    void removeSignatures();


  protected:

    /** get pointer to the concept name
     ** @return pointer to the concept name (never NULL)
     */
    inline DSRCodedEntryValue *getConceptNamePtr()
    {
        return &ConceptName;
    }

    /** get pointer to previous tree node
     ** @return pointer to previous tree node (might be NULL)
     */
    inline DSRDocumentTreeNode *getPrev() const
    {
        return OFstatic_cast(DSRDocumentTreeNode *, DSRTreeNode::getPrev());
    }

    /** get pointer to next tree node
     ** @return pointer to next tree node (might be NULL)
     */
    inline DSRDocumentTreeNode *getNext() const
    {
        return OFstatic_cast(DSRDocumentTreeNode *, DSRTreeNode::getNext());
    }

    /** get pointer to first child node
     ** @return pointer to first child node (might be NULL)
     */
    inline DSRDocumentTreeNode *getDown() const
    {
        return OFstatic_cast(DSRDocumentTreeNode *, DSRTreeNode::getDown());
    }

    /** get unique identifier of this node
     ** @return unique identifier of this node
     */
    inline size_t getIdent() const
    {
        return DSRTreeNode::getIdent();
    }

    /** create a new node and append it to the current one
     ** @param  previousNode       reference to the pointer to the previous node (sibling).
     *                             Used to decide whether the new node is a child (value=NULL)
     *                             or a sibling (!=NULL).  NB: The value might be modified
     *                             inside this method (to store a reference to the previous node).
     *  @param  relationshipType   relationship type of the new node with regard to the
     *                             current one
     *  @param  valueType          value type of node to be added
     *  @param  constraintChecker  checks relationship content constraints of the associated IOD
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createAndAppendNewNode(DSRDocumentTreeNode *&previousNode,
                                       const E_RelationshipType relationshipType,
                                       const E_ValueType valueType,
                                       const DSRIODConstraintChecker *constraintChecker = NULL);

    /** read content item (value) from dataset.
     *  This method does nothing for this base class, but derived classes overwrite it to read
     *  the contents according to their value type.
     ** @param  dataset  DICOM dataset from which the content item should be read
     *  @param  flags    flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readContentItem(DcmItem &dataset,
                                        const size_t flags);

    /** write content item (value) to dataset.
     *  This method does nothing for this base class, but derived classes overwrite it to write
     *  the contents according to their value type.
     ** @param  dataset  DICOM dataset to which the content item should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeContentItem(DcmItem &dataset) const;

    /** read content item specific XML data.
     *  This method does nothing for this base class, but derived classes overwrite it to read
     *  the contents according to their value type.
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXMLContentItem(const DSRXMLDocument &doc,
                                           DSRXMLCursor cursor,
                                           const size_t flags);

    /** render content item (value) in HTML/XHTML format.
     *  This method does nothing for this base class, but derived classes overwrite it to render
     *  the contents according to their value type.
     ** @param  docStream     output stream to which the main HTML/XHTML document is written
     *  @param  annexStream   output stream to which the HTML/XHTML document annex is written
     *  @param  nestingLevel  current nesting level.  Used to render section headings.
     *  @param  annexNumber   reference to the variable where the current annex number is stored.
     *                        Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags         flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                              STD_NAMESPACE ostream &annexStream,
                                              const size_t nestingLevel,
                                              size_t &annexNumber,
                                              const size_t flags) const;

    /** write common item start (XML tag)
     ** @param  stream          output stream to which the XML document is written
     *  @param  flags           flag used to customize the output (see DSRTypes::XF_xxx)
     *  @param  closingBracket  write closing bracket of XML start tag if OFTrue, otherwise the
     *                          bracket has to be closed in the calling method
     */
    void writeXMLItemStart(STD_NAMESPACE ostream &stream,
                           const size_t flags,
                           const OFBool closingBracket = OFTrue) const;

    /** write common item start (XML tag)
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     */
    void writeXMLItemEnd(STD_NAMESPACE ostream &stream,
                         const size_t flags) const;

    /** read SR document content module
     ** @param  dataset            DICOM dataset from which the data should be read
     *  @param  constraintChecker  checks relationship content constraints of the associated IOD
     *  @param  flags              flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readSRDocumentContentModule(DcmItem &dataset,
                                            const DSRIODConstraintChecker *constraintChecker,
                                            const size_t flags);

    /** write SR document content module
     ** @param  dataset      DICOM dataset to which the data should be written
     *  @param  markedItems  optional stack where pointers to all 'marked' content items
     *                       (DICOM datasets/items) are added to during the write process.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeSRDocumentContentModule(DcmItem &dataset,
                                             DcmStack *markedItems);

    /** read document relationship macro
     ** @param  dataset            DICOM dataset from which the data should be read
     *  @param  constraintChecker  checks relationship content constraints of the associated IOD
     *  @param  posString          location of the current content item (e.g.\ "1.2.3")
     *  @param  flags              flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readDocumentRelationshipMacro(DcmItem &dataset,
                                              const DSRIODConstraintChecker *constraintChecker,
                                              const OFString &posString,
                                              const size_t flags);

    /** write document relationship macro
     ** @param  dataset      DICOM dataset to which the data should be written
     *  @param  markedItems  optional stack where pointers to all 'marked' content items
     *                       (DICOM datasets/items) are added to during the write process.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeDocumentRelationshipMacro(DcmItem &dataset,
                                               DcmStack *markedItems);

    /** read document content macro
     ** @param  dataset    DICOM dataset from which the data should be read
     *  @param  posString  location of the current content item (e.g.\ "1.2.3")
     *  @param  flags      flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readDocumentContentMacro(DcmItem &dataset,
                                         const OFString &posString,
                                         const size_t flags);

    /** write document content macro
     ** @param  dataset  DICOM dataset to which the data should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeDocumentContentMacro(DcmItem &dataset) const;

    /** read content sequence
     ** @param  dataset            DICOM dataset from which the data should be read
     *  @param  constraintChecker  checks relationship content constraints of the associated IOD
     *  @param  posString          location of the current content item (e.g.\ "1.2.3")
     *  @param  flags              flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readContentSequence(DcmItem &dataset,
                                    const DSRIODConstraintChecker *constraintChecker,
                                    const OFString &posString,
                                    const size_t flags);

    /** write content sequence
     ** @param  dataset      DICOM dataset to which the data should be written
     *  @param  markedItems  optional stack where pointers to all 'marked' content items
     *                       (DICOM datasets/items) are added to during the write process.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeContentSequence(DcmItem &dataset,
                                     DcmStack *markedItems) const;

    /** render concept name in HTML/XHTML format.
     *  If the optional observation date/time field is non-empty, it is also rendered.
     ** @param  docStream  output stream to which the main HTML/XHTML document is written
     *  @param  flags      flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition renderHTMLConceptName(STD_NAMESPACE ostream &docStream,
                                      const size_t flags) const;

    /** render child nodes in HTML/XHTML format
     ** @param  docStream     output stream to which the main HTML/XHTML document is written
     *  @param  annexStream   output stream to which the HTML/XHTML document annex is written
     *  @param  nestingLevel  current nesting level.  Used to render section headings.
     *  @param  annexNumber   reference to the variable where the current annex number is stored.
     *                        Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags         flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition renderHTMLChildNodes(STD_NAMESPACE ostream &docStream,
                                     STD_NAMESPACE ostream &annexStream,
                                     const size_t nestingLevel,
                                     size_t &annexNumber,
                                     const size_t flags) const;

  // --- static function ---

    /** convert relationship type into a text used for HTML rendering
     ** @param  relationshipType  type of relationship to be converted
     *  @param  relationshipText  reference to string variable where the resulting text should be
     *                            stored.  Value is cleared if 'relationshipType' is invalid or not
     *                            supported.
     *  @param  flags             flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return reference to the 'relationshipText' string (might be empty)
     */
    static const OFString &getRelationshipText(const E_RelationshipType relationshipType,
                                               OFString &relationshipText,
                                               const size_t flags);

    /** check the specified template identification values for validity.
     *  The identification is valid if the first two values are either present (non-empty) or
     *  absent (empty).  In the latter case, the third (optional) parameter also has to be empty.
     ** @param  templateIdentifier  identifier of the template
     *  @param  mappingResource     mapping resource that defines the template
     *  @param  mappingResourceUID  uniquely identifies the mapping resource (optional)
     ** @return OFTrue if template identification is valid, OFFalse otherwise
     */
    static OFBool checkTemplateIdentification(const OFString &templateIdentifier,
                                              const OFString &mappingResource,
                                              const OFString &mappingResourceUID);


  private:

    /// flag indicating whether the content item is marked (e.g.\ used for digital signatures).
    /// The default value is OFFalse.
    OFBool             MarkFlag;
    /// flag indicating whether the content item is referenced (by-reference relationship).
    /// The default value is OFFalse.
    OFBool             ReferenceTarget;

    /// relationship type to the parent node (associated DICOM VR=CS, mandatory)
    E_RelationshipType RelationshipType;
    /// value type (associated DICOM VR=CS, mandatory)
    const E_ValueType  ValueType;

    /// concept name (VR=SQ, conditional)
    DSRCodedEntryValue ConceptName;
    /// observation date/time (VR=DT, conditional)
    OFString           ObservationDateTime;
    /// observation unique identifier (VR=UI, optional)
    OFString           ObservationUID;

    /// template identifier (VR=CS, mandatory in ContentTemplateSequence)
    OFString           TemplateIdentifier;
    /// mapping resource (VR=CS, mandatory in ContentTemplateSequence)
    OFString           MappingResource;
    /// mapping resource UID (VR=UI, optional in ContentTemplateSequence)
    OFString           MappingResourceUID;

    /// MAC parameters sequence (VR=SQ, optional)
    DcmSequenceOfItems MACParameters;
    /// digital signatures sequence (VR=SQ, optional)
    DcmSequenceOfItems DigitalSignatures;

 // --- declaration of default constructor and assignment operator

    DSRDocumentTreeNode();
    DSRDocumentTreeNode &operator=(const DSRDocumentTreeNode &);
};


#endif
