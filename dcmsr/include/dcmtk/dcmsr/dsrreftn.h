/*
 *
 *  Copyright (C) 2000-2015, OFFIS e.V.
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
 *    classes: DSRByReferenceTreeNode
 *
 */


#ifndef DSRREFTN_H
#define DSRREFTN_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoctn.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for by-reference relationships
 */
class DCMTK_DCMSR_EXPORT DSRByReferenceTreeNode
  : public DSRDocumentTreeNode
{

  public:

    /** constructor
     ** @param  relationshipType  type of relationship to the parent/source tree node.
     *                            Should not be DSRTypes::RT_invalid or DSRTypes::RT_isRoot.
     */
    DSRByReferenceTreeNode(const E_RelationshipType relationshipType);

    /** constructor
     ** @param  relationshipType  type of relationship to the parent/source tree node.
     *                            Should not be DSRTypes::RT_invalid or DSRTypes::RT_isRoot.
     *  @param  referencedNodeID  ID of the node to be referenced (target content item)
     *  @param  targetValueType   value type of the node to be referenced, i.e.\ the target
     */
    DSRByReferenceTreeNode(const E_RelationshipType relationshipType,
                           const size_t referencedNodeID,
                           const E_ValueType targetValueType);

    /** copy constructor.
     *  Please note that the comments on the copy constructor of the base class
     *  DSRDocumentTreeNode apply.  Furthermore, the following member variables of
     *  this class are also not copied but initialized with their default values:
     *  - ValidReference
     *  - ReferencedNodeID
     *  - TargetValueType
     *
     *  As a result, the contained by-reference relationship becomes invalid and should
     *  be updated by updateReference() after the node has been added to a document tree.
     ** @param  node  tree node to be copied
     */
    DSRByReferenceTreeNode(const DSRByReferenceTreeNode &node);

    /** destructor
     */
    virtual ~DSRByReferenceTreeNode();

    /** clone this tree node.
     *  Internally, the copy constructor is used, so the corresponding comments apply.
     ** @return copy of this tree node
     */
    virtual DSRByReferenceTreeNode *clone() const;

    /** clear all member variables.
     *  Please note that the content item becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether the content item is valid.
     *  The content item is valid if the base class is valid, the concept name is empty and
     *  the reference (see hasValidValue()) is valid.
     ** @return OFTrue if tree node is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the value of the content item, i.e.\ the reference (which has been
     *  checked from outside of this class), is valid
     ** @return OFTrue if the value is valid, OFFalse otherwise
     */
    virtual OFBool hasValidValue() const;

    /** print content item.
     *  A typical output looks like this: inferred from 1.2.3.  If the position string
     *  of the referenced target content item is empty, a "?" (question mark) is printed.
     ** @param  stream  output stream to which the content item should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags) const;

    /** write content item in XML format
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &stream,
                                 const size_t flags) const;

    /** get ID of the referenced node (target content item)
     ** @return ID of the referenced node if valid, 0 otherwise
     */
    size_t getReferencedNodeID() const
    {
        return ReferencedNodeID;
    }

    /** get position string of the referenced node (target content item)
     ** @return position string of the referenced node if valid, an empty string otherwise
     */
    const OFString &getReferencedContentItem() const
    {
        return ReferencedContentItem;
    }

    /** get value type of the referenced node (target content item)
     ** @return value type of the referenced node if valid, DSRTypes::VT_invalid otherwise
     */
    E_ValueType getTargetValueType() const
    {
        return TargetValueType;
    }

    /** invalidate reference to the target content item.
     *  Sets the internal flag accordingly, see isValid() method.
     */
    void invalidateReference();

    /** update reference to the target content item (using the node ID).
     *  Also sets the internal flag accordingly, see isValid() method.  Please note, however,
     *  that it is not checked whether the referenced content item really exists.  This is
     *  done later on by DSRDocumentSubTree::checkByReferenceRelationships().
     ** @param  referencedNodeID  ID of the node to be referenced (target content item)
     *  @param  targetValueType   value type of the node to be referenced, i.e.\ the target
     ** @return OFTrue if the reference is valid, OFFalse otherwise
     */
    OFBool updateReference(const size_t referencedNodeID,
                           const E_ValueType targetValueType);

    /** update reference the target content item (using the position string).
     *  Also sets the internal flag accordingly, see isValid() method.  Please note, however,
     *  that it is not checked whether the referenced content item really exists.  This is
     *  done later on by DSRDocumentSubTree::checkByReferenceRelationships().
     ** @param  referencedContentItem  position string of the node to be referenced
     ** @return OFTrue if the reference is valid, OFFalse otherwise
     */
    OFBool updateReference(const OFString &referencedContentItem);


  protected:

    /** read content item (value) from dataset
     ** @param  dataset  DICOM dataset from which the content item should be read
     *  @param  flags    flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readContentItem(DcmItem &dataset,
                                        const size_t flags);

    /** write content item (value) to dataset
     ** @param  dataset  DICOM dataset to which the content item should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeContentItem(DcmItem &dataset) const;

    /** read content item specific XML data
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXMLContentItem(const DSRXMLDocument &doc,
                                           DSRXMLCursor cursor,
                                           const size_t flags);

    /** render content item (value) in HTML/XHTML format
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

    /** set the concept name
     ** @param  conceptName  dummy parameter
     *  @param  check        dummy parameter
     ** @return always returns EC_IllegalCall, since this content item has no concept name
     */
    virtual OFCondition setConceptName(const DSRCodedEntryValue &conceptName,
                                       const OFBool check = OFTrue);

    /** set observation date/time
     ** @param  observationDateTime  dummy parameter
     *  @param  check                dummy parameter
     ** @return always returns EC_IllegalCall, since this content item has no observation
     *          date and time (part of Document Relationship Macro)
     */
    virtual OFCondition setObservationDateTime(const OFString &observationDateTime,
                                               const OFBool check = OFTrue);

    /** set observation date/time from element
     ** @param  delem  dummy parameter
     *  @param  pos    dummy parameter
     *  @param  check  dummy parameter
     ** @return always returns EC_IllegalCall, since this content item has no observation
     *          date and time (part of Document Relationship Macro)
     */
    virtual OFCondition setObservationDateTime(const DcmElement &delem,
                                               const unsigned long pos = 0,
                                               const OFBool check = OFTrue);

    /** set observation date/time from dataset
     ** @param  dataset  dummy parameter
     *  @param  tagKey   dummy parameter
     *  @param  pos      dummy parameter
     *  @param  check    dummy parameter
     ** @return always returns EC_IllegalCall, since this content item has no observation
     *          date and time (part of Document Relationship Macro)
     */
    virtual OFCondition setObservationDateTime(DcmItem &dataset,
                                               const DcmTagKey &tagKey,
                                               const unsigned long pos = 0,
                                               const OFBool check = OFTrue);

    /** set observation unique identifier
     ** @param  observationUID  dummy parameter
     *  @param  check           dummy parameter
     ** @return always returns EC_IllegalCall, since this content item has no observation
     *          unique identifier (part of Document Relationship Macro)
     */
    virtual OFCondition setObservationUID(const OFString &observationUID,
                                          const OFBool check = OFTrue);

    /** set template identifier and mapping resource
     ** @param  templateIdentifier  dummy parameter
     *  @param  mappingResource     dummy parameter
     *  @param  mappingResourceUID  dummy parameter
     *  @param  check               dummy parameter
     ** @return always returns EC_IllegalCall, since this content item has no template
     *          identification (part of Document Relationship Macro)
     */
    virtual OFCondition setTemplateIdentification(const OFString &templateIdentifier,
                                                  const OFString &mappingResource,
                                                  const OFString &mappingResourceUID = "",
                                                  const OFBool check = OFTrue);


  private:

    /// flag indicating whether the reference is valid or not (i.e. checked).
    /// The default value is OFFalse.
    OFBool      ValidReference;
    /// position string of the referenced nodes (target content item).
    /// The default value is en empty string.
    OFString    ReferencedContentItem;
    /// node ID of the referenced node (target content item).
    /// The default value is 0.
    size_t      ReferencedNodeID;
    /// value type of the referenced node (target content item).
    /// The default value is DSRTypes::VT_invalid.
    E_ValueType TargetValueType;


 // --- declaration of default constructor and assignment operator

    DSRByReferenceTreeNode();
    DSRByReferenceTreeNode &operator=(const DSRByReferenceTreeNode &);
};


#endif
