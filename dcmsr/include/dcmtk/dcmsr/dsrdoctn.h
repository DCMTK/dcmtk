/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:32 $
 *  CVS/RCS Revision: $Revision: 1.27 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRDOCTN_H
#define DSRDOCTN_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtree.h"
#include "dcmtk/dcmsr/dsrcodvl.h"
#include "dcmtk/dcmdata/dcitem.h"


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

class DSRIODConstraintChecker;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Base class for content items
 */
class DSRDocumentTreeNode
  : public DSRTreeNode
{
    // allow access to getConceptNamePtr()
    friend class DSRContentItem;

  public:

    /** constructor.
     *  The 'relationshipType' and 'valueType' can never be changed after the tree node
     *  has been created (therefore, the corresponding member variables are declared
     *  "const").
     ** @param  relationshipType  type of relationship to the parent tree node.
     *                            Should not be RT_invalid and RT_isRoot only for the
     *                            root node.
     *  @param  valueType         value type of the associated content item.
     *                            Should not be VT_invalid.
     */
    DSRDocumentTreeNode(const E_RelationshipType relationshipType,
                        const E_ValueType valueType);

    /** destructor
     */
    virtual ~DSRDocumentTreeNode();

    /** clear all member variables.
     *  This does not apply to the relationship and value type since they are never changed.
     */
    virtual void clear();

    /** check whether the content item is valid.
     *  The content item is valid if the relationship type and the value type are both not
     *  invalid.
     ** @return OFTrue if tree node is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the content is short.
     *  This method is used to check whether the rendered output of this content item can be
     *  expanded inline or not (used for renderHTML()).  This base class always returns OFTrue.
     ** @param  flags  flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return OFTrue if the content is short, OFFalse otherwise
     */
    virtual OFBool isShort(const size_t flags) const;

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
     *  @param  flags         optional flag used to customize the reading process
     *                        (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXML(const DSRXMLDocument &doc,
                                DSRXMLCursor cursor,
                                const E_DocumentType documentType,
                                const size_t flags);

    /** write content item in XML format
     ** @param  stream     output stream to which the XML document is written
     *  @param  flags      flag used to customize the output (see DSRTypes::XF_xxx)
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
        return (DigitalSignatures.card() > 0);
    }

    /** check whether content item is marked.
     *  Use method 'setMark' to mark and unmark the current content item.
     *  Pointers to the DICOM dataset/item of marked content items are added to the optional
     *  stack when calling the 'write' method.  This mechanism can e.g. be used to digitally
     *  sign particular content items.
     ** @return OFTrue if content item is marked, OFFalse otherwise
     */
    inline OFBool isMarked() const
    {
        return MarkFlag;
    }

    /** mark/unmark the current content item.
     *  See explanation for method 'isMarked' for details.
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
        return (Down != NULL);
    }

    /** check whether the current content item has any siblings
     ** @return OFTrue if there are any sibling nodes, OFFalse otherwise
     */
    inline OFBool hasSiblingNodes() const
    {
        return (Prev != NULL) || (Next != NULL);
    }

    /** get ID of the current tree node
     ** @return ID of the current tree node (should never be 0)
     */
    inline size_t getNodeID() const
    {
        return Ident;
    }

    /** get relationship type of the current content item
     ** @return relationship type of the current content item (might be RT_invalid)
     */
    inline E_RelationshipType getRelationshipType() const
    {
        return RelationshipType;
    }

    /** get value type of the current content item
     ** @return value type of the current content item (might be VT_invalid)
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
    OFCondition getConceptName(DSRCodedEntryValue &conceptName) const;

    /** set the concept name.
     *  Code describing the concept represented by this content item.  Also conveys the value
     *  of document title and section headings in documents.
     *  If the new code is invalid the current one is not replaced.  An empty code can
     *  be used to clear the current concept name.
     ** @param  conceptName  code to be set as the new concept name (checked before set)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setConceptName(const DSRCodedEntryValue &conceptName);

    /** get observation date time.
     *  This is the date and time on which this content item was completed.  Might be empty
     *  if the date and time do not differ from the content date and time, see DSRDocument.
     ** @return observation date and time of current content item (might be empty/invalid)
     */
    inline const OFString &getObservationDateTime() const
    {
        return ObservationDateTime;
    }

    /** set observation date time.
     *  This is the date and time on which this content item was completed.  Might be empty
     *  if the date and time do not differ from the content date and time, see DSRDocument.
     *  Please use the correct DICOM format (YYYYMMDDHHMMSS) or an empty string to clear
     *  the current value.  Currently no check is performed on the parameter value!
     ** @param  observationDateTime  value to be set (might be an empty string)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setObservationDateTime(const OFString &observationDateTime);

    /** get template identifier and mapping resource.
     *  This value pair identifies the template that was used to create this content item
     *  (and its children).  According to the DICOM standard it is "required if a template
     *  was used to define the content of this Item, and the template consists of a single
     *  CONTAINER with nested content, and it is the outermost invocation of a set of
     *  nested templates that start with the same CONTAINER."  However, this condition is
     *  currently not checked.  The identification is valid if both values are either present
     *  (non-empty) or absent (empty).
     ** @param  templateIdentifier  identifier of the template (might be empty)
     *  @param  mappingResource     mapping resource that defines the template (might be empty)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getTemplateIdentification(OFString &templateIdentifier,
                                          OFString &mappingResource) const;

    /** set template identifier and mapping resource.
     *  The identification is valid if both values are either present (non-empty) or absent
     *  (empty).  See getTemplateIdentification() for details.
     ** @param  templateIdentifier  identifier of the template to be set (VR=CS)
     *  @param  mappingResource     mapping resource that defines the template (VR=CS)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTemplateIdentification(const OFString &templateIdentifier,
                                                  const OFString &mappingResource);

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
     ** @param  dataset    DICOM dataset from which the content item should be read
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readContentItem(DcmItem &dataset);

    /** write content item (value) to dataset.
     *  This method does nothing for this base class, but derived classes overwrite it to write
     *  the contents according to their value type.
     ** @param  dataset    DICOM dataset to which the content item should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeContentItem(DcmItem &dataset) const;

    /** read content item specific XML data.
     *  This method does nothing for this base class, but derived classes overwrite it to read
     *  the contents according to their value type.
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXMLContentItem(const DSRXMLDocument &doc,
                                           DSRXMLCursor cursor);

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
     *  @param  posString          location of the current content item (e.g. "1.2.3")
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
     *  @param  posString  location of the current content item (e.g. "1.2.3")
     *  @param  flags      flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readDocumentContentMacro(DcmItem &dataset,
                                         const OFString &posString,
                                         const size_t flags);

    /** write document content macro
     ** @param  dataset    DICOM dataset to which the data should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeDocumentContentMacro(DcmItem &dataset) const;

    /** read content sequence
     ** @param  dataset            DICOM dataset from which the data should be read
     *  @param  constraintChecker  checks relationship content constraints of the associated IOD
     *  @param  posString          location of the current content item (e.g. "1.2.3")
     *  @param  flags              flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readContentSequence(DcmItem &dataset,
                                    const DSRIODConstraintChecker *constraintChecker,
                                    const OFString &posString,
                                    const size_t flags);

    /** write content sequence
     ** @param  dataset       DICOM dataset to which the data should be written
     *  @param  markedItems   optional stack where pointers to all 'marked' content items
     *                        (DICOM datasets/items) are added to during the write process.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeContentSequence(DcmItem &dataset,
                                     DcmStack *markedItems) const;

    /** render concept name in HTML/XHTML format.
     *  If the optional observation datetime field is valid (not empty) it is also rendered.
     ** @param  docStream    output stream to which the main HTML/XHTML document is written
     *  @param  flags        flag used to customize the output (see DSRTypes::HF_xxx)
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


  private:

    /// flag indicating whether the content item is marked (e.g. used for digital signatures)
    OFBool                   MarkFlag;
    /// flag indicating whether the content item is referenced (by-reference relationship)
    OFBool                   ReferenceTarget;

    /// relationship type to the parent node (associated DICOM VR=CS, mandatory)
    const E_RelationshipType RelationshipType;
    /// value type (associated DICOM VR=CS, mandatory)
    const E_ValueType        ValueType;

    /// concept name (VR=SQ, conditional)
    DSRCodedEntryValue       ConceptName;
    /// observation date and time (VR=DT, conditional)
    OFString                 ObservationDateTime;

    /// template identifier (VR=CS, mandatory in ContentTemplateSequence)
    OFString                 TemplateIdentifier;
    /// mapping resource (VR=CS, mandatory in ContentTemplateSequence)
    OFString                 MappingResource;

    /// MAC parameters sequence (VR=SQ, optional)
    DcmSequenceOfItems       MACParameters;
    /// digital signatures sequence (VR=SQ, optional)
    DcmSequenceOfItems       DigitalSignatures;


 // --- declaration of default/copy constructor and assignment operator

    DSRDocumentTreeNode();
    DSRDocumentTreeNode(const DSRDocumentTreeNode &);
    DSRDocumentTreeNode &operator=(const DSRDocumentTreeNode &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrdoctn.h,v $
 *  Revision 1.27  2010-10-14 13:16:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.26  2009-10-13 14:57:50  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.25  2008-05-19 09:48:32  joergr
 *  Fixed typo.
 *
 *  Revision 1.24  2007-11-15 16:33:30  joergr
 *  Added support for output in XHTML 1.1 format.
 *
 *  Revision 1.23  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.22  2006/05/11 09:18:21  joergr
 *  Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 *  Revision 1.21  2005/12/08 16:05:00  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.20  2004/11/22 16:39:09  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *    in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.19  2003/12/16 15:57:51  joergr
 *  Added note that the condition for the Content Template Sequence is currently
 *  not checked.
 *
 *  Revision 1.18  2003/10/30 17:53:02  joergr
 *  Added full support for the ContentTemplateSequence (read/write, get/set
 *  template identification). Template constraints are not checked yet.
 *
 *  Revision 1.17  2003/10/06 09:52:58  joergr
 *  Added new flag which allows to ignore content item errors when reading an SR
 *  document (e.g. missing value type specific attributes).
 *
 *  Revision 1.16  2003/09/15 14:18:54  joergr
 *  Introduced new class to facilitate checking of SR IOD relationship content
 *  constraints. Replaced old implementation distributed over numerous classes.
 *
 *  Revision 1.15  2003/08/07 17:31:00  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.14  2003/08/07 12:34:06  joergr
 *  Added readXML functionality.
 *  Updated documentation to get rid of doxygen warnings.
 *
 *  Revision 1.13  2002/08/02 12:38:31  joergr
 *  Enhanced debug output of dcmsr::read() routines (e.g. add position string
 *  of invalid content items to error messages).
 *
 *  Revision 1.12  2001/11/09 16:10:49  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.11  2001/09/26 13:04:07  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.10  2001/04/03 08:24:01  joergr
 *  Added new command line option: ignore relationship content constraints
 *  specified for each SR document class.
 *
 *  Revision 1.9  2001/02/02 14:37:33  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.8  2001/01/18 15:53:34  joergr
 *  Added support for digital signatures.
 *
 *  Revision 1.7  2000/11/13 10:26:21  joergr
 *  Added output of optional observation datetime to rendered HTML page.
 *
 *  Revision 1.6  2000/11/07 18:14:28  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.5  2000/11/01 16:23:19  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.4  2000/10/26 14:17:38  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.3  2000/10/23 15:10:29  joergr
 *  Added/updated doc++ comments.
 *
 *  Revision 1.2  2000/10/18 17:02:27  joergr
 *  Added methods allowing direct access to certain content item values.
 *  Made some functions inline.
 *
 *  Revision 1.1  2000/10/13 07:49:26  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
