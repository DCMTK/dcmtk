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
 *    classes: DSRTimeTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:49:33 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRTIMTN_H
#define DSRTIMTN_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrdoctn.h"
#include "dsrstrvl.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for content item TIME
 */
class DSRTimeTreeNode
  : public DSRDocumentTreeNode,
    public DSRStringValue
{

  public:
 
    DSRTimeTreeNode(const E_RelationshipType relationshipType);

    DSRTimeTreeNode(const E_RelationshipType relationshipType,
                    const OFString &stringValue);

    virtual ~DSRTimeTreeNode();

    virtual OFBool isValid() const;

    virtual E_Condition print(ostream &stream,
                              const size_t flags) const;

    virtual OFBool canAddNode(const E_DocumentType documentType,
                              const E_RelationshipType relationshipType,
                              const E_ValueType valueType) const;
    
    
  protected:

    virtual E_Condition readContentItem(DcmItem &dataset,
                                        OFConsole *logStream);

    virtual E_Condition writeContentItem(DcmItem &dataset,
                                         OFConsole *logStream) const;

    virtual E_Condition renderHTMLContentItem(ostream &docStream,
                                              ostream &annexStream,
                                              const size_t nestingLevel,
                                              size_t &annexNumber,
                                              const size_t flags,
                                              OFConsole *logStream) const;


  private:
    
 // --- declaration of default/copy constructor and assignment operator

    DSRTimeTreeNode();
    DSRTimeTreeNode(const DSRTimeTreeNode &);
    DSRTimeTreeNode &operator=(const DSRTimeTreeNode &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrtimtn.h,v $
 *  Revision 1.1  2000-10-13 07:49:33  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
