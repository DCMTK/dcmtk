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
 *    classes: DSRWaveformTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-23 15:13:42 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRWAVTN_H
#define DSRWAVTN_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrdoctr.h"
#include "dsrwavvl.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for content item WAVEFORM
 */
class DSRWaveformTreeNode
  : public DSRDocumentTreeNode,
    public DSRWaveformReferenceValue
{

  public:

    /** constructor.
     ** @param  relationshipType  type of relationship to the parent tree node.
     *                            Should not be RT_invalid or RT_isRoot.
     */
    DSRWaveformTreeNode(const E_RelationshipType relationshipType);

    /** destructor
     */
    virtual ~DSRWaveformTreeNode();

    /** clear all member variables.
     *  Please note that the content item might become invalid afterwards.
     */
    virtual void clear();

    /** check whether the content item is valid.
     *  The content item is valid if the two base classes are valid.
     ** @return OFTrue if tree node is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;
    
    /** check whether the content is short.
     *  The method isShort() from the base class DSRWaveformReferenceValue is called.
     ** @param  flags  flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return OFTrue if the content is short, OFFalse otherwise
     */
    virtual OFBool isShort(const size_t flags) const;

    /** print content item.
     *  A typical output looks like this: has properties WAVEFORM:=(HemodynamicWaveform
     *  Storage,"1.2.3")
     ** @param  stream  output stream to which the content item should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition print(ostream &stream,
                              const size_t flags) const;

    /** check whether a node could be added as a child node.
     *  This method checks whether a content item as specified could be added as a child
     *  node to the current one (without really adding the node).
     ** @param  documentType      type of document to which the content item belongs.
     *                            The document type has an impact on the relationship
     *                            contraints. 
     *  @param  relationshipType  relationship type of the new node with regard to the
     *                            current one
     *  @param  valueType         value type of node to be checked/added
     ** @return OFTrue if specified node can be added, OFFalse otherwise
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

  
  private:

// --- declaration of default/copy constructor and assignment operator

    DSRWaveformTreeNode();
    DSRWaveformTreeNode(const DSRWaveformTreeNode &);
    DSRWaveformTreeNode &operator=(const DSRWaveformTreeNode &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrwavtn.h,v $
 *  Revision 1.2  2000-10-23 15:13:42  joergr
 *  Added/updated doc++ comments.
 *
 *  Revision 1.1  2000/10/13 07:49:37  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
