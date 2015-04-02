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
 *    classes: DSRDateTimeTreeNode
 *
 */


#ifndef DSRDTITN_H
#define DSRDTITN_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoctn.h"
#include "dcmtk/dcmsr/dsrstrvl.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for content item DATETIME
 */
class DCMTK_DCMSR_EXPORT DSRDateTimeTreeNode
  : public DSRDocumentTreeNode,
    public DSRStringValue
{

  public:

    /** constructor
     ** @param  relationshipType  type of relationship to the parent tree node.  Should
     *                            not be DSRTypes::RT_invalid or DSRTypes::RT_isRoot.
     */
    DSRDateTimeTreeNode(const E_RelationshipType relationshipType);

    /** constructor
     ** @param  relationshipType  type of relationship to the parent tree node.
     *                            Should not be DSRTypes::RT_invalid or DSRTypes::RT_isRoot.
     *  @param  dateTimeValue     initial value to be set (VR=DT, mandatory)
     *  @param  check             if enabled, check 'dateTimeValue' for validity before setting
     *                            it.  See checkValue() for details.  An empty value is never
     *                            accepted.
     */
    DSRDateTimeTreeNode(const E_RelationshipType relationshipType,
                        const OFString &dateTimeValue,
                        const OFBool check = OFTrue);

    /** copy constructor.
     *  Please note that the comments on the copy constructor of the base class
     *  DSRDocumentTreeNode apply.
     ** @param  node  tree node to be copied
     */
    DSRDateTimeTreeNode(const DSRDateTimeTreeNode &node);

    /** destructor
     */
    virtual ~DSRDateTimeTreeNode();

    /** clone this tree node.
     *  Internally, the copy constructor is used, so the corresponding comments apply.
     ** @return copy of this tree node
     */
    virtual DSRDateTimeTreeNode *clone() const;

    /** clear all member variables.
     *  Please note that the content item might become invalid afterwards.
     */
    virtual void clear();

    /** check whether the content item is valid.
     *  The content item is valid if the base classes, the concept name and the currently stored
     *  value (see hasValidValue()) are valid.
     ** @return OFTrue if tree node is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the value of the content item, i.e.\ the stored date/time value, is valid
     ** @return OFTrue if the value is valid, OFFalse otherwise
     */
    virtual OFBool hasValidValue() const;

    /** print content item.
     *  A typical output looks like this: contains DATETIME:(,,"Code")="2000101012000000"
     ** @param  stream  output stream to which the content item should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags) const;

    /** write content item in XML format. Uses ISO formatted date/time value.
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &stream,
                                 const size_t flags) const;

    // --- static helper function ---

    /** get DICOM date/time value from given XML element.
     *  The DICOM DateTime (DT) value is expected to be stored in ISO format as created by
     *  writeXML().
     ** @param  doc            document containing the XML file content
     *  @param  cursor         cursor pointing to the corresponding node
     *  @param  dateTimeValue  reference to string object in which the value should be stored
     *  @param  clearString    flag specifying whether to clear the 'dateTimeValue' or not
     ** @return reference to string object (might be empty)
     */
    static OFString &getValueFromXMLNodeContent(const DSRXMLDocument &doc,
                                                DSRXMLCursor cursor,
                                                OFString &dateTimeValue,
                                                const OFBool clearString = OFTrue);


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

    /** check the specified date/time value for validity.
     *  In addition to the base class check for a non-empty value, this method also checks
     *  whether the given value conforms to the corresponding VR (DT) and VM (1).
     ** @param  dateTimeValue  value to be checked
     ** @return status, EC_Normal if value is valid, an error code otherwise
     */
    virtual OFCondition checkValue(const OFString &dateTimeValue) const;


  private:

 // --- declaration of default constructor and assignment operator

    DSRDateTimeTreeNode();
    DSRDateTimeTreeNode &operator=(const DSRDateTimeTreeNode &);
};


#endif
