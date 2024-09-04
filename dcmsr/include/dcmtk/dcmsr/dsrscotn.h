/*
 *
 *  Copyright (C) 2000-2024, OFFIS e.V.
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
 *    classes: DSRSCoordTreeNode
 *
 */


#ifndef DSRSCOTN_H
#define DSRSCOTN_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoctn.h"
#include "dcmtk/dcmsr/dsrscovl.h"


// include this file in doxygen documentation

/** @file dsrscotn.h
 *  @brief definitions for class DSRSCoordTreeNode
 */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for content item SCOORD
 */
class DCMTK_DCMSR_EXPORT DSRSCoordTreeNode
  : public DSRDocumentTreeNode,
    public DSRSpatialCoordinatesValue
{

  public:

    /** constructor
     ** @param  relationshipType  type of relationship to the parent tree node.  Should
     *                            not be DSRTypes::RT_invalid orDSRTypes:: RT_isRoot.
     */
    DSRSCoordTreeNode(const E_RelationshipType relationshipType);

    /** copy constructor.
     *  Please note that the comments on the copy constructor of the base class
     *  DSRDocumentTreeNode apply.
     ** @param  node  tree node to be copied
     */
    DSRSCoordTreeNode(const DSRSCoordTreeNode &node);

    /** destructor
     */
    virtual ~DSRSCoordTreeNode();

    /** clone this tree node.
     *  Internally, the copy constructor is used, so the corresponding comments apply.
     ** @return copy of this tree node
     */
    virtual DSRSCoordTreeNode *clone() const;

    /** clear all member variables.
     *  Please note that the content item might become invalid afterwards.
     */
    virtual void clear();

    /** check whether nodes are "equal".
     *  Two tree nodes are equal if the comparison operator of the base class DSRDocumentTreeNode
     *  regards them as "equal" (same types and concept names) and the stored values are equal.
     ** @param  node  tree node that should be compared to the current one
     ** @return OFTrue if both tree nodes are equal, OFFalse otherwise
     */
    virtual OFBool isEqual(const DSRDocumentTreeNode &node) const;

    /** check whether nodes are "not equal".
     *  Two tree nodes are not equal if either the comparison operator of the base class
     *  DSRDocumentTreeNode regards them as "not equal" (different types or concept names) or
     *  the stored values are not equal.
     ** @param  node  tree node that should be compared to the current one
     ** @return OFTrue if both tree nodes are not equal, OFFalse otherwise
     */
    virtual OFBool isNotEqual(const DSRDocumentTreeNode &node) const;

    /** check whether the content item is valid.
     *  The content item is valid if the two base classes are valid.  This check includes the value
     *  of the content item, which can also be checked separately with hasValidValue().
     ** @return OFTrue if tree node is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the value of the content item, i.e.\ the spatial coordinates value, is valid
     ** @return OFTrue if the value is valid, OFFalse otherwise
     */
    virtual OFBool hasValidValue() const;

    /** check whether the content is short.
     *  The method isShort() from the base class DSRSpatialCoordinatesValue is called.
     ** @param  flags  flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return OFTrue if the content is short, OFFalse otherwise
     */
    virtual OFBool isShort(const size_t flags) const;

    /** print content item.
     *  A typical output looks like this: has properties SCOORD:(,,"SCoord Code")=
     *  (CIRCLE,0/0,255/255)
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

  // --- reintroduce methods from base class

     using DSRDocumentTreeNode::renderHTMLContentItem;


  private:

// --- declaration of default constructor and assignment operator

    DSRSCoordTreeNode();
    DSRSCoordTreeNode &operator=(const DSRSCoordTreeNode &);
};


/*------------------------*
 *  comparison operators  *
 *------------------------*/

/** equality operator.
 *  Internally, the DSRSCoordTreeNode::isEqual() method is used.
 *  @param  lhs  left-hand side
 *  @param  rhs  right-hand side
 *  @return OFTrue if 'lhs' and 'rhs' are equal, OFFalse otherwise
 */
DCMTK_DCMSR_EXPORT OFBool operator==(const DSRSCoordTreeNode &lhs,
                                     const DSRSCoordTreeNode &rhs);

/** inequality operator.
 *  Internally, the DSRSCoordTreeNode::isNotEqual() method is used.
 *  @param  lhs  left-hand side
 *  @param  rhs  right-hand side
 *  @return OFTrue if 'lhs' and 'rhs' are not equal, OFFalse otherwise
 */
DCMTK_DCMSR_EXPORT OFBool operator!=(const DSRSCoordTreeNode &lhs,
                                     const DSRSCoordTreeNode &rhs);


#endif
