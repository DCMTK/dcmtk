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
 *  Update Date:      $Date: 2000-10-13 07:49:26 $
 *  CVS/RCS Revision: $Revision: 1.1 $
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

    OFBool canAddContentItem(const E_RelationshipType relationshipType,
                             const E_ValueType valueType,
                             const E_AddMode addMode = AM_afterCurrent);

    size_t addContentItem(const E_RelationshipType relationshipType,
                          const E_ValueType valueType,
                          const E_AddMode addMode = AM_afterCurrent);

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
 *  Revision 1.1  2000-10-13 07:49:26  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
