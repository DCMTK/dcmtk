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
 *    classes: DSRImageTreeNode
 *
 */


#ifndef DSRIMGTN_H
#define DSRIMGTN_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoctn.h"
#include "dcmtk/dcmsr/dsrimgvl.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for content item IMAGE
 */
class DCMTK_DCMSR_EXPORT DSRImageTreeNode
  : public DSRDocumentTreeNode,
    public DSRImageReferenceValue
{

  public:

    /** constructor
     ** @param  relationshipType  type of relationship to the parent tree node.  Should
     *                            not be DSRTypes::RT_invalid or DSRTypes::RT_isRoot.
     */
    DSRImageTreeNode(const E_RelationshipType relationshipType);

    /** copy constructor.
     *  Please note that the comments on the copy constructor of the base class
     *  DSRDocumentTreeNode apply.
     ** @param node tree node to be copied
     */
    DSRImageTreeNode(const DSRImageTreeNode &node);

    /** destructor
     */
    virtual ~DSRImageTreeNode();

    /** clone this tree node.
     *  Internally, the copy constructor is used, so the corresponding comments apply.
     ** @return copy of this tree node
     */
    virtual DSRImageTreeNode *clone() const;

    /** clear all member variables.
     *  Please note that the content item might become invalid afterwards.
     */
    virtual void clear();

    /** check whether the content item is valid.
     *  The content item is valid if the two base classes are valid.  This check includes the value
     *  of the content item, which can also be checked separately with hasValidValue().
     ** @return OFTrue if tree node is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the value of the content item, i.e.\ the image reference value, is valid
     ** @return OFTrue if the value is valid, OFFalse otherwise
     */
    virtual OFBool hasValidValue() const;

    /** check whether the content is short.
     *  The method isShort() from the base class DSRImageReferenceValue is called.
     ** @param  flags  flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return OFTrue if the content is short, OFFalse otherwise
     */
    virtual OFBool isShort(const size_t flags) const;

    /** print content item.
     *  A typical output looks like this: contains IMAGE:=(CT image,"1.2.3") or
     *  (CT image,"1.2.3"),(GSPS,"1.2.3.4") if a presentation state is present.
     *  If the SOP class UID is unknown, the UID is printed instead of the related name.
     *  Also, the list of referenced frame numbers is shown, but not the UIDs of the
     *  real world value mapping object (if referenced).
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


  private:

 // --- declaration of default constructor and assignment operator

    DSRImageTreeNode();
    DSRImageTreeNode &operator=(const DSRImageTreeNode &);
};


#endif
