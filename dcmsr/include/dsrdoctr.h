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
 *    classes: DSRDocumentTree
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-11-01 16:23:20 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRDOCTR_H
#define DSRDOCTR_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtree.h"
#include "dsrdoctn.h"
#include "dsrcitem.h"

#include "dcitem.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class managing the document tree
 */
class DSRDocumentTree
  : public DSRTree
{

  public:

    /** constructor 
     ** @param  documentType  document type of the associated document
     */
    DSRDocumentTree(const E_DocumentType documentType);

    /** destructor 
     */
    virtual ~DSRDocumentTree();

    /** clear internal member variables.
     *  The document type is not changed (e.g. set to DT_invalid).
     */
    virtual void clear();

    /** check whether the current internal state is valid.
     *  The SR document is valid if the document type is supported, the tree is not
     *  empty the root item is a container and has the internal relationship type
     *  RT_isRoot.
     ** @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** set the log stream.
     *  The log stream is used to report any warnings and error messages.
     ** @param  stream  pointer to the log stream (might be NULL = no messages)
     */
    void setLogStream(OFConsole *stream);

    /** print current SR document tree to specified output stream
     ** @param  stream  output stream
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition print(ostream &stream,
                      const size_t flags = 0) const;

    /** read SR document tree from DICOM dataset.
     *  Please note that the current document tree is also deleted if the reading fails.
     *  If the log stream is set and valid the reason for any error might be obtained
     *  from the error/warning output.
     ** @param  dataset       reference to DICOM dataset where the tree should be read from
     *  @param  documentType  document type of the SR document from which the tree is read
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition read(DcmItem &dataset,
                     const E_DocumentType documentType);

    /** write current SR document tree to DICOM dataset
     ** @param  dataset  reference to DICOM dataset where the current tree should be
     *                   written to
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition write(DcmItem &dataset) const;

    /** write current SR document tree in XML format
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition writeXML(ostream &stream,
                         const size_t flags) const;

    /** render current SR document tree in HTML format
     ** @param  stream  output stream to which the HTML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition renderHTML(ostream &docStream,
                           ostream &annexStream,
                           const size_t flags = 0) const;

    /** get document type
     ** return current document type (might be DT_invalid)
     */
    E_DocumentType getDocumentType() const
    {
        return DocumentType;
    }

    /** change document type.
     *  Please note that the document tree is deleted if the specified 'documentType'
     *  is supported.
     ** @param  documentType  new document type to be set (should be != DT_invalid)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition changeDocumentType(const E_DocumentType documentType);

    /** check whether specified content item can be added to the current one.
     *  If the tree is currently empty only a CONTAINER with the internal relationship
     *  type RT_isRoot is allowed (as the new root node).  This method can be used
     *  to decide which type of content items can be added prior to really do so.
     ** @param  relationshipType  relationship type of node to be checked with regard
     *                            to the current one
     *  @param  valueType         value type of node to be checked
     *  @param  addMode           flag specifying at which position the new node would
     *                            be added.
     *                            (AM_afterCurrent, AM_beforeCurrent, AM_belowCurrent)
     ** @return OFTrue if specified node can be added, OFFalse otherwise
     */
    OFBool canAddContentItem(const E_RelationshipType relationshipType,
                             const E_ValueType valueType,
                             const E_AddMode addMode = AM_afterCurrent);

    /** add specified content item to the current one.
     *  If possible this method creates a new node as specified and adds it to the current
     *  one.  The method canAddContentItem() is called internally to check parameters first.
     ** @param  relationshipType  relationship type of node to be added with regard
     *                            to the current one
     *  @param  valueType         value type of node to be added
     *  @param  addMode           flag specifying at which position to add the new node.
     *                            (AM_afterCurrent, AM_beforeCurrent, AM_belowCurrent)
     ** @return ID of new node if successful, 0 otherwise
     */
    size_t addContentItem(const E_RelationshipType relationshipType,
                          const E_ValueType valueType,
                          const E_AddMode addMode = AM_afterCurrent);

    /** remove current content item from tree.
     *  Please note that not only the specified node but also all of his child nodes are
     *  removed from the tree and then deleted.  The internal cursor is set automatically
     *  to a new valid position.
     ** @return ID of the node which became the current one after deletion, 0 if an error
     *          occured or the tree is now empty.
     */
    size_t removeCurrentContentItem();

    /** get reference to current content item.
     *  This mechanism allows to access all content items without using pointers.
     ** @return reference to current content item (might be invalid)
     */
    DSRContentItem &getCurrentContentItem();


  protected:

    /** add new node to the current one.
     *  Please note that no copy of the given node is created.  Therefore, the node
     *  should be created with new() - do not use a reference to a local variable.
     *  If the node could be added successfully the cursor is set to it automatically.
     ** @param  node     pointer to the new node to be added
     *  @param  addMode  flag specifying at which position to add the new node.
     *                   (AM_afterCurrent, AM_beforeCurrent, AM_belowCurrent)
     ** @return ID of the new added node if successful, 0 otherwise
     */
    virtual size_t addNode(DSRDocumentTreeNode *node,
                           const E_AddMode addMode = AM_afterCurrent);

    /** remove current node from tree.
     *  Please note that not only the specified node but also all of his child nodes are
     *  removed from the tree and then deleted.  The cursor is set automatically to a new
     *  valid position.
     ** @return ID of the node which became the current one after deletion, 0 if an error
     *          occured or the tree is now empty.
     */
    virtual size_t removeNode();


  private:

    /** add new node to the current one.
     *  This method just overwrites the method from the base class DSRTree.  Use the above
     *  addNode() method instead.
     ** @param  node     dummy parameter
     *  @param  addMode  dummy parameter
     ** @return always 0 (invalid)
     */
    virtual size_t addNode(DSRTreeNode *node,
                           const E_AddMode addMode = AM_afterCurrent);

    /// document type of the associated SR document
    E_DocumentType DocumentType;
    /// output stream for error messages, NULL for no messages
    OFConsole *LogStream;
    /// current content item.  Introduced to avoid the use of pointers.
    DSRContentItem CurrentContentItem;


 // --- declaration of default/copy constructor and assignment operator

    DSRDocumentTree();
    DSRDocumentTree(const DSRDocumentTree &);
    DSRDocumentTree &operator=(const DSRDocumentTree &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrdoctr.h,v $
 *  Revision 1.4  2000-11-01 16:23:20  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.3  2000/10/18 17:02:57  joergr
 *  Added doc++ comments.
 *
 *  Revision 1.2  2000/10/16 16:31:45  joergr
 *  Added doc++ comments.
 *
 *  Revision 1.1  2000/10/13 07:49:26  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
