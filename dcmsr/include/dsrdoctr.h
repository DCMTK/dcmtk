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
 *  Update Date:      $Date: 2000-10-16 16:31:45 $
 *  CVS/RCS Revision: $Revision: 1.2 $
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

    DSRDocumentTree(const E_DocumentType documentType);

    virtual ~DSRDocumentTree();

    virtual void clear();

    virtual OFBool isValid() const;

    void setLogStream(OFConsole *stream);

    E_Condition print(ostream &stream,
                      const size_t flags = 0) const;

    E_Condition read(DcmItem &dataset,
                     const E_DocumentType documentType);

    E_Condition write(DcmItem &dataset);

    E_Condition renderHTML(ostream &docStream,
                           ostream &annexStream,
                           const size_t flags = 0) const;

    E_DocumentType getDocumentType() const
    {
        return DocumentType;
    }

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

    DSRContentItem &getCurrentContentItem();


  protected:

    virtual size_t addNode(DSRDocumentTreeNode *node,
                           const E_AddMode addMode = AM_afterCurrent);

    virtual size_t removeNode();


  private:

    virtual size_t addNode(DSRTreeNode *node,
                           const E_AddMode addMode = AM_afterCurrent);

    E_DocumentType DocumentType;

    /// output stream for error messages, NULL for no messages
    OFConsole *LogStream;

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
 *  Revision 1.2  2000-10-16 16:31:45  joergr
 *  Added doc++ comments.
 *
 *  Revision 1.1  2000/10/13 07:49:26  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
