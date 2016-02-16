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
 *    classes: DSRRootTemplate
 *
 */


#ifndef DSRRTPL_H
#define DSRRTPL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoctr.h"
#include "dcmtk/dcmsr/dsrctpl.h"


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

class DSRSubTemplate;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class managing an SR document tree that is based on an SR root template.
 *  This class provides a more template-specific API than the general document tree class
 *  and should be used for root templates only.
 ** @note The main focus of this class currently lies in the creation of SR documents that
 *        are based on an SR root template.  Support for reading SR documents into the
 *        underlying structures might be added in the future.
 */
class DCMTK_DCMSR_EXPORT DSRRootTemplate
  : protected DSRDocumentTree,
    public DSRTemplateCommon
{

  public:

    /** constructor
     ** @param  documentType        document type of the associated document
     *  @param  templateIdentifier  identifier of the template
     *  @param  mappingResource     mapping resource that defines the template
     *  @param  mappingResourceUID  uniquely identifies the mapping resource (optional)
     */
    DSRRootTemplate(const E_DocumentType documentType,
                    const OFString &templateIdentifier,
                    const OFString &mappingResource,
                    const OFString &mappingResourceUID = "");

    /** copy constructor.
     *  Also see notes on DSRDocumentTree's copy constructor.
     ** @param  rootTemplate  template to be copied
     */
    DSRRootTemplate(const DSRRootTemplate &rootTemplate);

    /** destructor
     */
    virtual ~DSRRootTemplate();

    /** clone this template.
     *  Internally, the copy constructor is used, so the corresponding comments apply.
     ** @return copy of this template
     */
    virtual DSRRootTemplate *clone() const;

    /** clone the internally stored document tree of this template.
     *  Internally, the copy constructor of DSRDocumentTree is used, so the corresponding
     *  comments apply.  In addition, the template identification is set for the root node
     *  of the cloned tree (if applicable).
     ** @return copy of the internally stored document tree
     */
    virtual DSRDocumentTree *cloneTree() const;

    /** clear internal member variables.
     *  Also see notes on the clear() method of the base classes.
     */
    virtual void clear();

    /** check whether the current internal state is valid.
     *  That means, whether both the internally stored document tree and the template
     *  identification are valid.
     ** @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether this template is a root template, i.e.\ describes a complete SR
     *  document tree starting from the root node
     ** @return always returns OFTrue since this class handles root templates
     */
    inline OFBool isRootTemplate() const
    {
        return OFTrue;
    }

    /** get read-only access to internally stored document tree.
     *  This method is not "const" because the template identification is set/updated
     *  automatically for the root node of the document tree (if applicable).
     ** @return constant reference to internally stored document tree
     */
    virtual const DSRDocumentTree &getTree();

    /** insert tree from given template to internally stored document tree.
     *  If possible, this method adds a copy of the given tree to the current content item.
     *  However, in case this template is non-extensible, an error code will be returned.
     ** @param  subTemplate     template that contains the tree that should be inserted
     *  @param  addMode         flag specifying at which position to add the new subtree
     *                          (e.g. after or below the current node)
     *  @param  defaultRelType  default relationship type between the top-level nodes of
     *                          the given subtree and the current node.  This relationship
     *                          type is used if the one of a top-level node is "unknown".
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition insertTemplate(const DSRSubTemplate &subTemplate,
                                       const E_AddMode addMode = AM_belowCurrent,
                                       const E_RelationshipType defaultRelType = RT_unknown);


  // --- introduce some methods from base class to public API

    using DSRDocumentTree::isEmpty;
    using DSRDocumentTree::isCursorValid;
    using DSRDocumentTree::getDocumentType;
    using DSRDocumentTree::print;
    using DSRDocumentTree::writeXML;
    using DSRDocumentTree::countNodes;
    using DSRDocumentTree::countChildNodes;
    using DSRDocumentTree::hasParentNode;
    using DSRDocumentTree::hasChildNodes;
    using DSRDocumentTree::hasPreviousNode;
    using DSRDocumentTree::hasNextNode;
    using DSRDocumentTree::hasSiblingNodes;
    using DSRDocumentTree::iterate;
    using DSRDocumentTree::gotoRoot;
    using DSRDocumentTree::gotoFirst;
    using DSRDocumentTree::gotoLast;
    using DSRDocumentTree::gotoPrevious;
    using DSRDocumentTree::gotoNext;
    using DSRDocumentTree::gotoParent;
    using DSRDocumentTree::gotoChild;
    using DSRDocumentTree::gotoNamedNode;
    using DSRDocumentTree::gotoNextNamedNode;
    using DSRDocumentTree::gotoAnnotatedNode;
    using DSRDocumentTree::gotoNextAnnotatedNode;
    using DSRDocumentTree::getCurrentContentItem;
    using DSRDocumentTree::updateByReferenceRelationships;


  protected:

    /** read SR document tree from DICOM dataset
     ** @param  dataset       dummy parameter
     *  @param  documentType  dummy parameter
     *  @param  flags         dummy parameter
     ** @return always returns EC_IllegalCall since this method is not yet implemented and
     *          should, therefore, never be called for this class
     */
    virtual OFCondition read(DcmItem &dataset,
                             const E_DocumentType documentType,
                             const size_t flags = 0);

    /** read XML document tree
     ** @param  doc     dummy parameter
     *  @param  cursor  dummy parameter
     *  @param  flags   dummy parameter
     ** @return always returns EC_IllegalCall since this method is not yet implemented and
     *          should, therefore, never be called for this class
     */
    virtual OFCondition readXML(const DSRXMLDocument &doc,
                                DSRXMLCursor cursor,
                                const size_t flags);

    /** update the tree for subsequent output, e.g.\ for being printed or added to an SR
     *  document.  This function is called automatically by the affected output methods.
     */
    virtual void updateTreeForOutput();


  private:

 // --- declaration of default constructor and assignment operator

    DSRRootTemplate();
    DSRRootTemplate &operator=(const DSRRootTemplate &);
};


#endif
