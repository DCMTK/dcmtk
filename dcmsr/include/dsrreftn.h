/*
 *
 *  Copyright (C) 2000, OFFIS
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
 *    classes: DSRByReferenceTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-11-01 16:23:23 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRREFTN_H
#define DSRREFTN_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrdoctn.h"
#include "dsrstrvl.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for by-reference relationships
 */
class DSRByReferenceTreeNode
  : public DSRDocumentTreeNode,
    public DSRStringValue
{

  public:
 
    /** constructor.
     ** @param  relationshipType  type of relationship to the parent/source tree node.
     *                            Should not be RT_invalid or RT_isRoot.
     */
    DSRByReferenceTreeNode(const E_RelationshipType relationshipType);

    /** constructor.
     ** @param  relationshipType  type of relationship to the parent/source tree node.
     *                            Should not be RT_invalid or RT_isRoot.
     *  @param  stringValue       initial string value to be set
     */
    DSRByReferenceTreeNode(const E_RelationshipType relationshipType,
                           const OFString &stringValue);

    /** destructor
     */
    virtual ~DSRByReferenceTreeNode();

    /** clear all member variables.
     *  Please note that the content item becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether the content item is valid.
     *  The content item is valid if the two base classes are valid and the concept
     *  name is empty.
     ** @return OFTrue if tree node is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** print content item.
     *  A typical output looks like this: inferred from 1.2.3
     ** @param  stream  output stream to which the content item should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition print(ostream &stream,
                              const size_t flags) const;

    /** write content item in XML format
     ** @param  stream     output stream to which the XML document is written
     *  @param  flags      flag used to customize the output (see DSRTypes::XF_xxx)
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition writeXML(ostream &stream,
                                 const size_t flags,
                                 OFConsole *logStream) const;

    /** set the concept name
     ** @param  conceptName  dummy parameter
     ** @return always returns EC_IllegalCall, since this content item has no concept name
     */
    virtual E_Condition setConceptName(const DSRCodedEntryValue &conceptName);

    /** set observation date time
     ** @param  observationDateTime  dummy parameter
     ** @return always returns EC_IllegalCall, since this content item has no observation
     *          date and time
     */
    virtual E_Condition setObservationDateTime(const OFString &observationDateTime);

    /** check whether a node could be added as a child node
     ** @param  documentType      dummy parameter 
     *  @param  relationshipType  dummy parameter
     *  @param  valueType         dummy parameter
     ** @return always returns OFFalse, since this content item is a leaf (has no children)
     */
    virtual OFBool canAddNode(const E_DocumentType documentType,
                              const E_RelationshipType relationshipType,
                              const E_ValueType valueType) const;
    

  protected:
  
    /** read content item (value) from dataset
     ** @param  dataset    DICOM dataset from which the content item should be read
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition readContentItem(DcmItem &dataset,
                                        OFConsole *logStream);

    /** write content item (value) to dataset
     ** @param  dataset    DICOM dataset to which the content item should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition writeContentItem(DcmItem &dataset,
                                         OFConsole *logStream) const;

    /** render content item (value) in HTML format
     ** @param  docStream     output stream to which the main HTML document is written
     *  @param  annexStream   output stream to which the HTML document annex is written
     *  @param  nestingLevel  current nesting level.  Used to render section headings.
     *  @param  annexNumber   reference to the variable where the current annex number is stored.
     *                        Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags         flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  logStream     pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition renderHTMLContentItem(ostream &docStream,
                                              ostream &annexStream,
                                              const size_t nestingLevel,
                                              size_t &annexNumber,
                                              const size_t flags,
                                              OFConsole *logStream) const;

    /** check the specified string value for validity.
     *  A valid format consists of '.' separated interger number, e.g. "1", "1.2" or "1.2.3".
     ** @param  stringValue  string value to be checked
     ** @return OFTrue if code is valid, OFFalse otherwise
     */
    virtual OFBool checkValue(const OFString &stringValue) const;

      
  private:

 // --- declaration of default/copy constructor and assignment operator

    DSRByReferenceTreeNode();
    DSRByReferenceTreeNode(const DSRByReferenceTreeNode &);
    DSRByReferenceTreeNode &operator=(const DSRByReferenceTreeNode &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrreftn.h,v $
 *  Revision 1.2  2000-11-01 16:23:23  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.1  2000/10/26 14:22:42  joergr
 *  Added support for "Comprehensive SR".
 *
 *
 *
 */
