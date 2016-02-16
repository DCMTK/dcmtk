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
 *    classes: DSRSubTemplate
 *
 */


#ifndef DSRSTPL_H
#define DSRSTPL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoctr.h"
#include "dcmtk/dcmsr/dsrctpl.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class managing an SR document tree that is based on an arbitrary SR template.
 *  This class provides a more template-specific API than the general subtree class and
 *  should be used for non-root templates only.
 ** @note The main focus of this class currently lies in the creation of SR documents that
 *        are based on an SR non-root template.  Support for reading SR documents into the
 *        underlying structures might be added in the future.
 */
class DCMTK_DCMSR_EXPORT DSRSubTemplate
  : protected DSRDocumentSubTree,
    public DSRTemplateCommon
{
    // allow direct access to inherited getRoot() method
    friend class DSRTreeNodeCursor<DSRDocumentTreeNode, OFTrue>;

  public:

    /** constructor
     ** @param  templateIdentifier  identifier of the template
     *  @param  mappingResource     mapping resource that defines the template
     *  @param  mappingResourceUID  uniquely identifies the mapping resource (optional)
     */
    DSRSubTemplate(const OFString &templateIdentifier,
                   const OFString &mappingResource,
                   const OFString &mappingResourceUID = "");

    /** copy constructor.
     *  Also see notes on DSRDocumentSubTree's copy constructor.
     ** @param  subTemplate  template to be copied
     */
    DSRSubTemplate(const DSRSubTemplate &subTemplate);

    /** destructor
     */
    virtual ~DSRSubTemplate();

    /** clone this template.
     *  Internally, the copy constructor is used, so the corresponding comments apply.
     ** @return copy of this template
     */
    virtual DSRSubTemplate *clone() const;

    /** clone the internally stored subtree of this template.
     *  Internally, the copy constructor of DSRDocumentSubTree is used, so the corresponding
     *  comments apply.  In addition, the template identification is set for the root node
     *  of the cloned subtree (if applicable).
     ** @return copy of the internally stored subtree
     */
    virtual DSRDocumentSubTree *cloneTree() const;

    /** clear internal member variables.
     *  Also see notes on the clear() method of the base classes.
     */
    virtual void clear();

    /** check whether the current internal state is valid.
     *  That means, whether both the internally stored subtree and the template
     *  identification are valid.
     ** @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether this template is a root template, i.e.\ describes a complete SR
     *  document tree starting from the root node
     ** @return always returns OFFalse since this class handles non-root templates only
     */
    inline OFBool isRootTemplate() const
    {
        return OFFalse;
    }

    /** get read-only access to internally stored subtree.
     *  This method is not "const" because the template identification is set/updated
     *  automatically for the root node of the subtree (if applicable).
     ** @return constant reference to internally stored subtree
     */
    virtual const DSRDocumentSubTree &getTree();

    /** insert tree from given template to internally stored subtree.
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

    using DSRDocumentSubTree::isEmpty;
    using DSRDocumentSubTree::isCursorValid;
    using DSRDocumentSubTree::print;
    using DSRDocumentSubTree::writeXML;
    using DSRDocumentSubTree::countNodes;
    using DSRDocumentSubTree::countChildNodes;
    using DSRDocumentSubTree::hasParentNode;
    using DSRDocumentSubTree::hasChildNodes;
    using DSRDocumentSubTree::hasPreviousNode;
    using DSRDocumentSubTree::hasNextNode;
    using DSRDocumentSubTree::hasSiblingNodes;
    using DSRDocumentSubTree::iterate;
    using DSRDocumentSubTree::gotoRoot;
    using DSRDocumentSubTree::gotoFirst;
    using DSRDocumentSubTree::gotoLast;
    using DSRDocumentSubTree::gotoPrevious;
    using DSRDocumentSubTree::gotoNext;
    using DSRDocumentSubTree::gotoParent;
    using DSRDocumentSubTree::gotoChild;
    using DSRDocumentSubTree::gotoNamedNode;
    using DSRDocumentSubTree::gotoNextNamedNode;
    using DSRDocumentSubTree::gotoAnnotatedNode;
    using DSRDocumentSubTree::gotoNextAnnotatedNode;
    using DSRDocumentSubTree::getCurrentContentItem;
    using DSRDocumentSubTree::updateByReferenceRelationships;


  protected:

    /** update the tree for subsequent output, e.g.\ for being printed or added to an SR
     *  document.  This function is called automatically by the affected output methods.
     */
    virtual void updateTreeForOutput();


  private:

 // --- declaration of default constructor and assignment operator

    DSRSubTemplate();
    DSRSubTemplate &operator=(const DSRSubTemplate &);
};


#endif
