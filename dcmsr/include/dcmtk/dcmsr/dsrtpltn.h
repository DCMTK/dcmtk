/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
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
 *    classes: DSRIncludedTemplateTreeNode
 *
 */


#ifndef DSRTPLTN_H
#define DSRTPLTN_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoctn.h"
#include "dcmtk/dcmsr/dsrstpl.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for managing included templates (internal)
 */
class DCMTK_DCMSR_EXPORT DSRIncludedTemplateTreeNode
  : public DSRDocumentTreeNode
{

  public:

    /** constructor.
     *  Also sets the template identification if available from 'referencedTemplate'.
     ** @param  referencedTemplate  shared pointer to template that should be managed
     *  @param  defaultRelType      default relationship type of the included template
     */
    DSRIncludedTemplateTreeNode(const DSRSharedSubTemplate &referencedTemplate,
                                const E_RelationshipType defaultRelType);

    /** copy constructor.
     *  Please note that the comments on the copy constructor of the base class
     *  DSRDocumentTreeNode apply.
     ** @param  node  tree node to be copied
     */
    DSRIncludedTemplateTreeNode(const DSRIncludedTemplateTreeNode &node);

    /** destructor
     */
    virtual ~DSRIncludedTemplateTreeNode();

    /** clone this tree node.
     *  Internally, the copy constructor is used, so the corresponding comments apply.
     ** @return copy of this tree node
     */
    virtual DSRIncludedTemplateTreeNode *clone() const;

    /** clear all member variables.
     *  Please note that the content item becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether the content item is valid.
     *  The content item is valid if the base class is valid and the reference to the
     *  included template is valid.
     ** @return OFTrue if tree node is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the value of the content item, i.e.\ the reference to the included
     *  template is valid.  See getValue().
     ** @return OFTrue if the value is valid, OFFalse otherwise
     */
    virtual OFBool hasValidValue() const;

    /** check whether the content is short.
     *  This method is used to check whether the rendered output of this content item can
     *  be expanded inline or not (used for renderHTML()).
     ** @param  flags  flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return OFTrue if the content is short, OFFalse otherwise
     */
    virtual OFBool isShort(const size_t flags) const;

    /** print content item.
     *  A typical output looks like this: # INCLUDE TID 1600 (DCMR)
     ** @param  stream  output stream to which the content item should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags) const;

    /** write content of included template in XML format
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful (or included template is empty, so there
     *          is nothing to do), an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &stream,
                                 const size_t flags) const;

    /** get reference to included template, i.e.\ the value of this content item
     ** @return reference to included template.  The managed pointer might be NULL.
     */
    inline const DSRSharedSubTemplate &getValue() const
    {
        return ReferencedTemplate;
    }

    /** set reference to included template, i.e.\ the value of this content item.
     *  Currently, no checks are performed on the passed 'referencedTemplate' parameter.
     ** @param  referencedTemplate  shared pointer to template that should be managed
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setValue(const DSRSharedSubTemplate &referencedTemplate);


  protected:

    /** read content item from dataset
     ** @param  dataset            dummy parameter
     *  @param  constraintChecker  dummy parameter
     *  @param  flags              dummy parameter
     ** @return always returns SR_EC_CannotProcessIncludedTemplates
     */
    virtual OFCondition read(DcmItem &dataset,
                             const DSRIODConstraintChecker *constraintChecker,
                             const size_t flags);

    /** write content item to dataset
     ** @param  dataset      dummy parameter
     *  @param  markedItems  dummy parameter
     ** @return always returns SR_EC_CannotProcessIncludedTemplates
     */
    virtual OFCondition write(DcmItem &dataset,
                              DcmStack *markedItems = NULL);

    /** read general XML document tree node data
     ** @param  doc           dummy parameter
     *  @param  cursor        dummy parameter
     *  @param  documentType  dummy parameter
     *  @param  flags         dummy parameter
     ** @return always returns SR_EC_CannotProcessIncludedTemplates
     */
    virtual OFCondition readXML(const DSRXMLDocument &doc,
                                DSRXMLCursor cursor,
                                const E_DocumentType documentType,
                                const size_t flags);

    /** render content item in HTML/XHTML format
     ** @param  docStream     dummy parameter
     *  @param  annexStream   dummy parameter
     *  @param  nestingLevel  dummy parameter
     *  @param  annexNumber   dummy parameter
     *  @param  flags         dummy parameter
     ** @return always returns SR_EC_CannotProcessIncludedTemplates
     */
    virtual OFCondition renderHTML(STD_NAMESPACE ostream &docStream,
                                   STD_NAMESPACE ostream &annexStream,
                                   const size_t nestingLevel,
                                   size_t &annexNumber,
                                   const size_t flags) const;

    /** set the concept name
     ** @param  conceptName  dummy parameter
     *  @param  check        dummy parameter
     ** @return always returns EC_IllegalCall, since this method should not be called
     */
    virtual OFCondition setConceptName(const DSRCodedEntryValue &conceptName,
                                       const OFBool check = OFTrue);

    /** set observation date/time
     ** @param  observationDateTime  dummy parameter
     *  @param  check                dummy parameter
     ** @return always returns EC_IllegalCall, since this method should not be called
     */
    virtual OFCondition setObservationDateTime(const OFString &observationDateTime,
                                               const OFBool check = OFTrue);

    /** set observation date/time from element
     ** @param  delem  dummy parameter
     *  @param  pos    dummy parameter
     *  @param  check  dummy parameter
     ** @return always returns EC_IllegalCall, since this method should not be called
     */
    virtual OFCondition setObservationDateTime(const DcmElement &delem,
                                               const unsigned long pos = 0,
                                               const OFBool check = OFTrue);

    /** set observation date/time from dataset
     ** @param  dataset  dummy parameter
     *  @param  tagKey   dummy parameter
     *  @param  pos      dummy parameter
     *  @param  check    dummy parameter
     ** @return always returns EC_IllegalCall, since this method should not be called
     */
    virtual OFCondition setObservationDateTime(DcmItem &dataset,
                                               const DcmTagKey &tagKey,
                                               const unsigned long pos = 0,
                                               const OFBool check = OFTrue);

    /** set observation unique identifier
     ** @param  observationUID  dummy parameter
     *  @param  check           dummy parameter
     ** @return always returns EC_IllegalCall, since this method should not be called
     */
    virtual OFCondition setObservationUID(const OFString &observationUID,
                                          const OFBool check = OFTrue);

    /** set template identifier and mapping resource
     ** @param  templateIdentifier  dummy parameter
     *  @param  mappingResource     dummy parameter
     *  @param  mappingResourceUID  dummy parameter
     *  @param  check               dummy parameter
     ** @return always returns EC_IllegalCall, since this method should not be called
     */
    virtual OFCondition setTemplateIdentification(const OFString &templateIdentifier,
                                                  const OFString &mappingResource,
                                                  const OFString &mappingResourceUID = "",
                                                  const OFBool check = OFTrue);


  private:

    /// shared pointer to included template that is managed by this class
    DSRSharedSubTemplate ReferencedTemplate;


 // --- declaration of default constructor and assignment operator

    DSRIncludedTemplateTreeNode();
    DSRIncludedTemplateTreeNode &operator=(const DSRIncludedTemplateTreeNode &);
};


#endif
