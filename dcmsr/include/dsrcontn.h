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
 *    classes: DSRContainerTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-11-09 16:10:47 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRCONTN_H
#define DSRCONTN_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrdoctr.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for content item CONTAINER
 */
class DSRContainerTreeNode
  : public DSRDocumentTreeNode
{

  public:

    /** constructor.
     ** @param  relationshipType  type of relationship to the parent tree node.
     *                            Should not be RT_invalid or RT_isRoot.
     */
    DSRContainerTreeNode(const E_RelationshipType relationshipType,
                         const E_ContinuityOfContent continuityOfContent = COC_Separate);

    /** destructor
     */
    virtual ~DSRContainerTreeNode();

    /** clear all member variables.
     *  Please note that the content item might become invalid afterwards.
     */
    virtual void clear();

    /** check whether the content item is valid.
     *  The content item is valid if the base class is valid, the continuity of content
     *  flag is valid, and the concept name is valid or the content item is not the root item.
     ** @return OFTrue if tree node is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the content is short.
     *  A container content item is defined to be never short (return always OFFalse).
     ** @param  flags  flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return OFTrue if the content is short, OFFalse otherwise
     */
    virtual OFBool isShort(const size_t flags) const;

    /** print content item.
     *  A typical output looks like this: CONTAINER:(,,"Diagnosis")=SEPARATE for the root node
     *  and contains CONTAINER:=CONTINUOUS for a "normal" content item.
     ** @param  stream  output stream to which the content item should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(ostream &stream,
                              const size_t flags) const;

    /** write content item in XML format
     ** @param  stream     output stream to which the XML document is written
     *  @param  flags      flag used to customize the output (see DSRTypes::XF_xxx)
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(ostream &stream,
                                 const size_t flags,
                                 OFConsole *logStream) const;

    /** render content item in HTML format.
     *  After rendering the current content item all child nodes (if any) are also rendered (see
     *  renderHTMLChildNodes() for details).  This method overwrites the one specified in base class
     *  DSRDocumentTree since the rendering of the child nodes depends on the value of the flag
     *  'ContinuityOfContent'.
     ** @param  docStream    output stream to which the main HTML document is written
     *  @param  annexStream  output stream to which the HTML document annex is written
     *  @param  annexNumber  reference to the variable where the current annex number is stored.
     *                       Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags        flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  logStream    pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition renderHTML(ostream &docStream,
                                   ostream &annexStream,
                                   const size_t nestingLevel,
                                   size_t &annexNumber,
                                   const size_t flags,
                                   OFConsole *logStream) const;

    /** check whether a node could be added as a child node.
     *  This method checks whether a content item as specified could be added as a child
     *  node to the current one (without really adding the node).
     ** @param  documentType      type of document to which the content item belongs.
     *                            The document type has an impact on the relationship
     *                            contraints. 
     *  @param  relationshipType  relationship type of the new node with regard to the
     *                            current one
     *  @param  valueType         value type of node to be checked/added
     *  @param  byReference       optional flag indicating whether the node/relationship
     *                            should be added by-value (default) or by-reference
     *                            (only for Comprehensive SR and Mammography CAD SR)
     ** @return OFTrue if specified node can be added, OFFalse otherwise
     */
    virtual OFBool canAddNode(const E_DocumentType documentType,
                              const E_RelationshipType relationshipType,
                              const E_ValueType valueType,
                              const OFBool byReference = OFFalse) const;

    /** get continuity of content flag.
     *  This flag specifies whether or not its contained content items (child nodes) are
     *  logically linked in a continuous textual flow, or are sparate items.
     ** @return continuity of content flag if successful, COC_invalid otherwise
     */
    inline E_ContinuityOfContent getContinuityOfContent() const
    {
        return ContinuityOfContent;
    }

    /** set continuity of content flag.
     *  This flag specifies whether or not its contained content items (child nodes) are
     *  logically linked in a continuous textual flow, or are sparate items.
     ** @param  continuityOfContent  value to be set (should be different from COC_onvalid)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setContinuityOfContent(const E_ContinuityOfContent continuityOfContent);


  protected:
  
    /** read content item (value) from dataset
     ** @param  dataset    DICOM dataset from which the content item should be read
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readContentItem(DcmItem &dataset,
                                        OFConsole *logStream);

    /** write content item (value) to dataset
     ** @param  dataset    DICOM dataset to which the content item should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeContentItem(DcmItem &dataset,
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
    virtual OFCondition renderHTMLContentItem(ostream &docStream,
                                              ostream &annexStream,
                                              const size_t nestingLevel,
                                              size_t &annexNumber,
                                              const size_t flags,
                                              OFConsole *logStream) const;

  
  private:

    /// continuity of content flag (VR=CS, mandatory)
    E_ContinuityOfContent ContinuityOfContent;


 // --- declaration of default/copy constructor and assignment operator

    DSRContainerTreeNode();
    DSRContainerTreeNode(const DSRContainerTreeNode &);
    DSRContainerTreeNode &operator=(const DSRContainerTreeNode &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrcontn.h,v $
 *  Revision 1.8  2001-11-09 16:10:47  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.7  2001/09/26 13:04:05  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.6  2001/06/01 15:51:00  meichel
 *  Updated copyright header
 *
 *  Revision 1.5  2000/11/07 18:14:28  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.4  2000/11/01 16:14:26  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.3  2000/10/23 15:09:27  joergr
 *  Added/updated doc++ comments.
 *
 *  Revision 1.2  2000/10/18 17:01:17  joergr
 *  Made some functions inline.
 *
 *  Revision 1.1  2000/10/13 07:49:24  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
