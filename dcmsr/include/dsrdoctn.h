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
 *    classes: DSRDocumentTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:49:26 $
 *  CVS/RCS Revision: $Revision: 1.1 $
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

  public:

    DSRDocumentTreeNode(const E_RelationshipType relationshipType,
                        const E_ValueType valueType);

    virtual ~DSRDocumentTreeNode();

    virtual void clear();

    virtual OFBool isValid() const;
    
    virtual OFBool isShort(const size_t flags) const;

    virtual E_Condition print(ostream &stream,
                              const size_t flags) const;

    virtual E_Condition read(DcmItem &dataset,
                             const E_DocumentType documentType,
                             OFConsole *logStream = NULL);

    virtual E_Condition write(DcmItem &dataset,
                              OFConsole *logStream = NULL) const;

    virtual E_Condition renderHTML(ostream &docStream,
                                   ostream &annexStream,
                                   const size_t nestingLevel,
                                   size_t &annexNumber,
                                   const size_t flags,
                                   OFConsole *logStream = NULL) const;

    OFBool hasChildNodes() const
    {
        return (Down != NULL);
    }

    size_t getNodeID() const
    {
        return Ident;
    }

    E_RelationshipType getRelationshipType() const
    {
        return RelationshipType;
    }

    E_ValueType getValueType() const
    {
        return ValueType;
    }

    const DSRCodedEntryValue &getConceptName() const
    {
        return ConceptName;
    }

    E_Condition getConceptName(DSRCodedEntryValue &conceptName) const;

    E_Condition setConceptName(const DSRCodedEntryValue &conceptName);

    const OFString &getObservationDateTime() const
    {
        return ObservationDateTime;
    }

    E_Condition setObservationDateTime(const OFString &observationDateTime);

    virtual OFBool canAddNode(const E_DocumentType documentType,
                              const E_RelationshipType relationshipType,
                              const E_ValueType valueType) const;


  protected:

    E_Condition createAndAppendNewNode(DSRDocumentTreeNode *&previousNode,
                                       const E_DocumentType documentType,
                                       const E_RelationshipType relationshipType,
                                       const E_ValueType valueType);

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

    E_Condition readSRDocumentContentModule(DcmItem &dataset,
                                            const E_DocumentType documentType,
                                            OFConsole *logStream);

    E_Condition writeSRDocumentContentModule(DcmItem &dataset,
                                             OFConsole *logStream) const;

    E_Condition readDocumentRelationshipMacro(DcmItem &dataset,
                                              const E_DocumentType documentType,
                                              OFConsole *logStream);

    E_Condition writeDocumentRelationshipMacro(DcmItem &dataset,
                                               OFConsole *logStream) const;

    E_Condition readDocumentContentMacro(DcmItem &dataset,
                                         OFConsole *logStream);

    E_Condition writeDocumentContentMacro(DcmItem &dataset,
                                          OFConsole *logStream) const;

    E_Condition readContentSequence(DcmItem &dataset,
                                    const E_DocumentType documentType,
                                    OFConsole *logStream);

    E_Condition writeContentSequence(DcmItem &dataset,
                                     OFConsole *logStream) const;

    E_Condition renderHTMLConceptName(ostream &docStream,
                                      const size_t flags,
                                      OFConsole *logStream) const;

    E_Condition renderHTMLChildNodes(ostream &docStream,
                                     ostream &annexStream,
                                     const size_t nestingLevel,
                                     size_t &annexNumber,
                                     const size_t flags,
                                     OFConsole *logStream) const;

    const OFString &getRelationshipText(const E_RelationshipType relationshipType,
                                        OFString &relationshipText,
                                        const size_t flags) const;
  
  
  private:

    const E_RelationshipType RelationshipType;
    const E_ValueType        ValueType;

    DSRCodedEntryValue ConceptName;
    OFString           ObservationDateTime;


 // --- declaration of default/copy constructor and assignment operator

    DSRDocumentTreeNode();
    DSRDocumentTreeNode(const DSRDocumentTreeNode &);
    DSRDocumentTreeNode &operator=(const DSRDocumentTreeNode &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrdoctn.h,v $
 *  Revision 1.1  2000-10-13 07:49:26  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
