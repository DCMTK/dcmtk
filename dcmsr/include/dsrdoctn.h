/*
 *
 *  Copyright (C) 2000-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRDocumentTreeNode
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-09-26 13:04:07 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRDOCTN_H
#define DSRDOCTN_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtree.h"
#include "dsrcodvl.h"

#include "dcitem.h"


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
     *  has been created (therefore, the corresponding memeber variables are declared
     *  as "const").
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
     *  The output of a content item depends on its value type.  This general method prints only
     *  those parts which all derived classes (= value types) do have in common, i.e. the type of
     *  relationship, the value type and the (optional) concept name.
     *  A typical output looks like this: has concept mod CODE: (,,"Concept")
     ** @param  stream  output stream to which the content item should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(ostream &stream,
                              const size_t flags) const;

    /** read content item from dataset.
     *  A number of read...() methods are called (see "protected" part) in order to retrieve all
     *  possibly nested content items from the dataset.
     ** @param  dataset       DICOM dataset from which the content item should be read
     *  @param  documentType  type of the document to be read (used for checking purposes)
     *  @param  flags         flag used to customize the reading process (see DSRTypes::RF_xxx)
     *  @param  logStream     pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmItem &dataset,
                             const E_DocumentType documentType,
                             const size_t flags,
                             OFConsole *logStream = NULL);

    /** write content item to dataset.
     *  A number of write...() methods are called (see "protected" part) in order to write all
     *  possibly nested content items to the dataset.
     ** @param  dataset      DICOM dataset to which the content item should be written
     *  @param  markedItems  optional stack where pointers to all 'marked' content items
     *                       (DICOM datasets/items) are added to during the write process.
     *                       Can be used to digitally sign parts of the document tree.
     *  @param  logStream    pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmItem &dataset,
                              DcmStack *markedItems = NULL,
                              OFConsole *logStream = NULL);

    /** write content item in XML format
     ** @param  stream     output stream to which the XML document is written
     *  @param  flags      flag used to customize the output (see DSRTypes::XF_xxx)
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(ostream &stream,
                                 const size_t flags,
                                 OFConsole *logStream = NULL) const;

    /** render content item in HTML format.
     *  After rendering the current content item all child nodes (if any) are also rendered (see
     *  renderHTMLChildNodes() for details).
     ** @param  docStream     output stream to which the main HTML document is written
     *  @param  annexStream   output stream to which the HTML document annex is written
     *  @param  nestingLevel  current nesting level.  Used to render section headings.
     *  @param  annexNumber   reference to the variable where the current annex number is stored.
     *                        Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags         flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  logStream     pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition renderHTML(ostream &docStream,
                                   ostream &annexStream,
                                   const size_t nestingLevel,
                                   size_t &annexNumber,
                                   const size_t flags,
                                   OFConsole *logStream = NULL) const;

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

    /** check whether the current content item is target of an by-reference relationship
     ** @return OFTrue if the content item is target, OFFalse otherwise
     */
    inline OFBool isReferenceTarget() const
    {
        return ReferenceTarget;
    }

    /** specify whether the current content item is target of an by-reference relationship
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
     *  the current value.  Currently no check is performed on the parameter value.
     ** @param  observationDateTime  value to be set (might be an empty string)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setObservationDateTime(const OFString &observationDateTime);

    /** check whether a node could be added as a child node.
     *  This method checks whether a content item as specified could be added as a child
     *  node to the current one (without really adding the node).  For this base class the
     *  return value is always OFTrue, derived classes typically overwrite this method.
     ** @param  documentType      type of document to which the content item belongs.
     *                            The document type has an impact on the relationship
     *                            constraints.
     *  @param  relationshipType  relationship type of the new node with regard to the
     *                            current one
     *  @param  valueType         value type of node to be checked/added
     *  @param  byReference       optional flag indicating whether the node/relationship
     *                            should be added by-value (default) or by-reference
     *                            (only for Comprehensive SR)
     ** @return OFTrue if specified node can be added, OFFalse otherwise
     */
    virtual OFBool canAddNode(const E_DocumentType documentType,
                              const E_RelationshipType relationshipType,
                              const E_ValueType valueType,
                              const OFBool byReference = OFFalse) const;

    /** remove digital signatures from content item.
     *  This method clears the MACParametersSequence and the DigitalSignaturesSequence for the
     *  current content item which have been filled during reading.
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
     ** @param  previousNode      reference to the pointer to the previous node (sibling).
     *                            Used to decide whether the new node is a child (value=NULL)
     *                            or a sibling (!=NULL).  NB: The value might be modified
     *                            inside this method (to store a reference to the previous node).
     *  @param  documentType      type of document to which this and the new content item belong
     *  @param  relationshipType  relationship type of the new node with regard to the
     *                            current one
     *  @param  valueType         value type of node to be added
     *  @param  checkConstraints  optional flag indicating whether to check the relationship
     *                            constraints (as specified for the current document class/type)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createAndAppendNewNode(DSRDocumentTreeNode *&previousNode,
                                       const E_DocumentType documentType,
                                       const E_RelationshipType relationshipType,
                                       const E_ValueType valueType,
                                       const OFBool checkConstraints = OFTrue);

    /** read content item (value) from dataset.
     *  This method does nothing for this base class, but derived classes overwrite it to read
     *  the contents according to their value type.
     ** @param  dataset    DICOM dataset from which the content item should be read
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readContentItem(DcmItem &dataset,
                                        OFConsole *logStream);

    /** write content item (value) to dataset.
     *  This method does nothing for this base class, but derived classes overwrite it to write
     *  the contents according to their value type.
     ** @param  dataset    DICOM dataset to which the content item should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeContentItem(DcmItem &dataset,
                                         OFConsole *logStream) const;

    /** render content item (value) in HTML format.
     *  This method does nothing for this base class, but derived classes overwrite it to render
     *  the contents according to their value type.
     ** @param  docStream     output stream to which the main HTML document is written
     *  @param  annexStream   output stream to which the HTML document annex is written
     *  @param  nestingLevel  current nesting level.  Used to render section headings.
     *  @param  annexNumber   reference to the variable where the current annex number is stored.
     *                        Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags         flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  logStream     pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition renderHTMLContentItem(ostream &docStream,
                                              ostream &annexStream,
                                              const size_t nestingLevel,
                                              size_t &annexNumber,
                                              const size_t flags,
                                              OFConsole *logStream) const;

    /** write common item start (XML tag)
     ** @param  stream          output stream to which the XML document is written
     *  @param  flags           flag used to customize the output (see DSRTypes::XF_xxx)
     *  @param  closingBracket  write closing bracket of XML start tag if OFTrue, otherwise the
     *                          bracket has to be closed in the calling method
     */
    void writeXMLItemStart(ostream &stream,
                           const size_t flags,
                           const OFBool closingBracket = OFTrue) const;

    /** write common item start (XML tag)
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     */
    void writeXMLItemEnd(ostream &stream,
                         const size_t flags) const;

    /** read SR document content module
     ** @param  dataset       DICOM dataset from which the data should be read
     *  @param  documentType  type of the document to be read (used for checking purposes)
     *  @param  flags         flag used to customize the reading process (see DSRTypes::RF_xxx)
     *  @param  logStream     pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readSRDocumentContentModule(DcmItem &dataset,
                                            const E_DocumentType documentType,
                                            const size_t flags,
                                            OFConsole *logStream);

    /** write SR document content module
     ** @param  dataset      DICOM dataset to which the data should be written
     *  @param  markedItems  optional stack where pointers to all 'marked' content items
     *                       (DICOM datasets/items) are added to during the write process.
     *  @param  logStream    pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeSRDocumentContentModule(DcmItem &dataset,
                                             DcmStack *markedItems,
                                             OFConsole *logStream);

    /** read document relationship macro
     ** @param  dataset       DICOM dataset from which the data should be read
     *  @param  documentType  type of the document to be read (used for checking purposes)
     *  @param  flags         flag used to customize the reading process (see DSRTypes::RF_xxx)
     *  @param  logStream     pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readDocumentRelationshipMacro(DcmItem &dataset,
                                              const E_DocumentType documentType,
                                              const size_t flags,
                                              OFConsole *logStream);

    /** write document relationship macro
     ** @param  dataset      DICOM dataset to which the data should be written
     *  @param  markedItems  optional stack where pointers to all 'marked' content items
     *                       (DICOM datasets/items) are added to during the write process.
     *  @param  logStream    pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeDocumentRelationshipMacro(DcmItem &dataset,
                                               DcmStack *markedItems,
                                               OFConsole *logStream);

    /** read document content macro
     ** @param  dataset    DICOM dataset from which the data should be read
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readDocumentContentMacro(DcmItem &dataset,
                                         OFConsole *logStream);

    /** write document content macro
     ** @param  dataset    DICOM dataset to which the data should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeDocumentContentMacro(DcmItem &dataset,
                                          OFConsole *logStream) const;

    /** read content sequence
     ** @param  dataset       DICOM dataset from which the data should be read
     *  @param  documentType  type of the document to be read (used for checking purposes)
     *  @param  flags         flag used to customize the reading process (see DSRTypes::RF_xxx)
     *  @param  logStream     pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readContentSequence(DcmItem &dataset,
                                    const E_DocumentType documentType,
                                    const size_t flags,
                                    OFConsole *logStream);

    /** write content sequence
     ** @param  dataset       DICOM dataset to which the data should be written
     *  @param  documentType  type of the document to be read (used for checking purposes)
     *  @param  markedItems   optional stack where pointers to all 'marked' content items
     *                        (DICOM datasets/items) are added to during the write process.
     *  @param  logStream     pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeContentSequence(DcmItem &dataset,
                                     DcmStack *markedItems,
                                     OFConsole *logStream) const;

    /** render concept name in HTML format.
     *  If the optional observation datetime field is valid (not empty) it is also rendered.
     ** @param  docStream    output stream to which the main HTML document is written
     *  @param  flags        flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  logStream    pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition renderHTMLConceptName(ostream &docStream,
                                      const size_t flags,
                                      OFConsole *logStream) const;

    /** render child nodes in HTML format
     ** @param  docStream     output stream to which the main HTML document is written
     *  @param  annexStream   output stream to which the HTML document annex is written
     *  @param  nestingLevel  current nesting level.  Used to render section headings.
     *  @param  annexNumber   reference to the variable where the current annex number is stored.
     *                        Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags         flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  logStream     pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition renderHTMLChildNodes(ostream &docStream,
                                     ostream &annexStream,
                                     const size_t nestingLevel,
                                     size_t &annexNumber,
                                     const size_t flags,
                                     OFConsole *logStream) const;

    /** convert relationship type into a text used for HTML rendering
     ** @param  relationshipType  type of relationship to be converted
     *  @param  relationshipText  reference to string variable where the resulting text should be
     *                            stored.  Value is cleared if 'relationshipType' is invalid or not
     *                            supported.
     *  @param  flags             flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return reference to the 'relationshipText' string
     */
    const OFString &getRelationshipText(const E_RelationshipType relationshipType,
                                        OFString &relationshipText,
                                        const size_t flags) const;


  private:

    /// flag indicating whether the content item is marked (e.g. used for digital signatures)
    OFBool                   MarkFlag;
    /// flag indicating whether the content item is referenced (by-reference relationship)
    OFBool                   ReferenceTarget;

    /// relationship type to the parent node (VR=CS, mandatory)
    const E_RelationshipType RelationshipType;
    /// value type (VR=CS, mandatory)
    const E_ValueType        ValueType;

    /// concept name (VR=SQ, conditional)
    DSRCodedEntryValue       ConceptName;
    /// observation date and time (VR=DT, conditional)
    OFString                 ObservationDateTime;

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
 *  Revision 1.11  2001-09-26 13:04:07  meichel
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
