/*
 *
 *  Copyright (C) 2017-2018, J. Riesmeier, Oldenburg, Germany
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
 *    classes: DSRDocumentTreeNodeFilter and derived implementations
 *
 */


#ifndef DSRDNFLT_H
#define DSRDNFLT_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsdefine.h"
#include "dcmtk/dcmsr/dsrcodvl.h"
#include "dcmtk/dcmsr/dsrtnant.h"
#include "dcmtk/dcmsr/dsrtypes.h"

#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofdatime.h"


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

class DSRDocumentTreeNode;



/*----------------------*
 *  class declarations  *
 *----------------------*/

/** Base class for a single document tree node filter
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeFilter
{

  public:

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeFilter();

    /** check whether given node matches the filter criterion (abstract)
     ** @param  node  pointer to the node to be checked
     ** @return OFTrue if 'node' matches the filter criterion, OFFalse otherwise
     */
    virtual OFBool matches(const DSRDocumentTreeNode *node) const = 0;
};


/** Base class for a list of document tree node filters
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeFilterList
  : public DSRDocumentTreeNodeFilter
{

  public:

    /** default constructor
     */
    DSRDocumentTreeNodeFilterList();

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeFilterList();

    /** add a document tree node filter to the list
     ** @param  filter  pointer to a single filter or to a list of filters to be added.
     *                  Ownership is passed to this list, i.e. memory is deleted by the
     *                  destructor.  Therefore, the instance has to be created with new().
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addFilter(DSRDocumentTreeNodeFilter *filter);

    /** check whether given node matches the filter criteria in the list (abstract)
     ** @param  node  pointer to the node to be checked
     ** @return OFTrue if 'node' matches the filter criteria, OFFalse otherwise
     */
    virtual OFBool matches(const DSRDocumentTreeNode *node) const = 0;


  protected:

    /// list of pointers to filter criteria
    OFList<DSRDocumentTreeNodeFilter *> FilterList;


  private:

 // --- declaration of copy constructor and assignment operator

    DSRDocumentTreeNodeFilterList(const DSRDocumentTreeNodeFilterList &);
    DSRDocumentTreeNodeFilterList &operator=(const DSRDocumentTreeNodeFilterList &);
};


/** Class implementing a list of document tree node filters that are combined with AND
 *  (conjunction)
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeAndFilter
  : public DSRDocumentTreeNodeFilterList
{

  public:

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeAndFilter();

    /** check whether given node matches all filter criteria in the list
     ** @param  node  pointer to the node to be checked
     ** @return OFTrue if 'node' matches the filter criteria, OFFalse otherwise
     */
    virtual OFBool matches(const DSRDocumentTreeNode *node) const;
};


/** Class implementing a list of document tree node filters that are combined with OR
 *  (disjunction)
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeOrFilter
  : public DSRDocumentTreeNodeFilterList
{

  public:

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeOrFilter();

    /** check whether given node matches at least a single filter criterion in the list
     ** @param  node  pointer to the node to be checked
     ** @return OFTrue if 'node' matches the filter criteria, OFFalse otherwise
     */
    virtual OFBool matches(const DSRDocumentTreeNode *node) const;
};


/** Class implementing a document tree node filter that checks for the presence (or
 *  absence) of child nodes
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeHasChildrenFilter
  : public DSRDocumentTreeNodeFilter
{

  public:

    /** (default) constructor
     ** @param  hasChildren  mode specifying whether the filter matches on the presence
     *                       (default) or absence of child nodes
     */
    DSRDocumentTreeNodeHasChildrenFilter(const OFBool hasChildren = OFTrue);

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeHasChildrenFilter();

    /** check whether given node matches the filter criterion
     ** @param  node  pointer to the node to be checked
     ** @return OFTrue if 'node' matches the filter criterion, OFFalse otherwise
     */
    virtual OFBool matches(const DSRDocumentTreeNode *node) const;


  private:

    /// mode specifying whether the filter matches on the presence or absence of child
    /// nodes
    const OFBool HasChildren;
};


/** Class implementing a document tree node filter that checks for the presence (or
 *  absence) of sibling nodes
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeHasSiblingsFilter
  : public DSRDocumentTreeNodeFilter
{

  public:

    /** (default) constructor
     ** @param  hasSiblings  mode specifying whether the filter matches on the presence
     *                       (default) or absence of sibling nodes
     */
    DSRDocumentTreeNodeHasSiblingsFilter(const OFBool hasSiblings = OFTrue);

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeHasSiblingsFilter();

    /** check whether given node matches the filter criterion
     ** @param  node  pointer to the node to be checked
     ** @return OFTrue if 'node' matches the filter criterion, OFFalse otherwise
     */
    virtual OFBool matches(const DSRDocumentTreeNode *node) const;


  private:

    /// mode specifying whether the filter matches on the presence or absence of sibling
    /// nodes
    const OFBool HasSiblings;
};


/** Class implementing a document tree node filter that checks for the presence (or
 *  absence) of a concept name.  An empty concept name is treated as absent.
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeHasConceptNameFilter
  : public DSRDocumentTreeNodeFilter
{

  public:

    /** constructor
     ** @param  hasConceptName  mode specifying whether the filter matches on the
     *                          presence (default) or absence of a concept name
     */
    DSRDocumentTreeNodeHasConceptNameFilter(const OFBool hasConceptName = OFTrue);

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeHasConceptNameFilter();

    /** check whether given node matches the filter criterion
     ** @param  node  pointer to the node to be checked
     ** @return OFTrue if 'node' matches the filter criterion, OFFalse otherwise
     */
    virtual OFBool matches(const DSRDocumentTreeNode *node) const;


  private:

    /// mode specifying whether the filter matches on the presence or absence of a
    /// concept name
    const OFBool HasConceptName;
};


/** Class implementing a document tree node filter that checks for a given concept name
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeConceptNameFilter
  : public DSRDocumentTreeNodeFilter
{

  public:

    /** constructor
     ** @param  conceptName  concept name to check for
     */
    DSRDocumentTreeNodeConceptNameFilter(const DSRCodedEntryValue &conceptName);

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeConceptNameFilter();

    /** check whether given node matches the filter criterion (concept name)
     ** @param  node  pointer to the node to be checked
     ** @return OFTrue if 'node' matches the filter criterion, OFFalse otherwise
     */
    virtual OFBool matches(const DSRDocumentTreeNode *node) const;


  private:

    /// concept name to check for
    const DSRCodedEntryValue ConceptName;
};


/** Class implementing a document tree node filter that checks for a given value type
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeValueTypeFilter
  : public DSRDocumentTreeNodeFilter
{

  public:

    /** constructor
     ** @param  valueType  value type to check for
     */
    DSRDocumentTreeNodeValueTypeFilter(const DSRTypes::E_ValueType &valueType);

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeValueTypeFilter();

    /** check whether given node matches the filter criterion (value type)
     ** @param  node  pointer to the node to be checked
     ** @return OFTrue if 'node' matches the filter criterion, OFFalse otherwise
     */
    virtual OFBool matches(const DSRDocumentTreeNode *node) const;


  private:

    /// value type to check for
    const DSRTypes::E_ValueType ValueType;
};


/** Class implementing a document tree node filter that checks for a given relationship
 *  type
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeRelationshipTypeFilter
  : public DSRDocumentTreeNodeFilter
{

  public:

    /** constructor
     ** @param  relationshipType  relationship type to check for
     */
    DSRDocumentTreeNodeRelationshipTypeFilter(const DSRTypes::E_RelationshipType &relationshipType);

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeRelationshipTypeFilter();

    /** check whether given node matches the filter criterion (relationship type)
     ** @param  node  pointer to the node to be checked
     ** @return OFTrue if 'node' matches the filter criterion, OFFalse otherwise
     */
    virtual OFBool matches(const DSRDocumentTreeNode *node) const;


  private:

    /// relationship type to check for
    const DSRTypes::E_RelationshipType RelationshipType;
};


/** Class implementing a document tree node filter that checks for a given annotation
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeAnnotationFilter
  : public DSRDocumentTreeNodeFilter
{

  public:

    /** constructor
     ** @param  annotation  annotation to check for
     */
    DSRDocumentTreeNodeAnnotationFilter(const DSRTreeNodeAnnotation &annotation);

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeAnnotationFilter();

    /** check whether given node matches the filter criterion (annotation)
     ** @param  node  pointer to the node to be checked
     ** @return OFTrue if 'node' matches the filter criterion, OFFalse otherwise
     */
    virtual OFBool matches(const DSRDocumentTreeNode *node) const;


  private:

    /// annotation to check for
    const DSRTreeNodeAnnotation Annotation;
};


/** Class implementing a document tree node filter that checks for a given range of
 *  observation date/time values
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeObservationDateTimeFilter
  : public DSRDocumentTreeNodeFilter
{

  public:

    /** constructor.
     *  Empty date/time values are not used for checking.  E.g. if only 'fromDateTime' is
     *  specified (non-empty) all moments in time subsequent to and including it match.
     *  If both date/time values are empty, only an empty observation date/time of the
     *  document tree node matches.
     ** @param  fromDateTime  start observation date/time to check for (might be empty)
     *  @param  toDateTime    end observation date/time to check for (might be empty)
     */
    DSRDocumentTreeNodeObservationDateTimeFilter(const OFString &fromDateTime,
                                                 const OFString &toDateTime);

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeObservationDateTimeFilter();

    /** check whether given node matches the filter criterion (observation date/time)
     ** @param  node  pointer to the node to be checked
     ** @return OFTrue if 'node' matches the filter criterion, OFFalse otherwise
     */
    virtual OFBool matches(const DSRDocumentTreeNode *node) const;


  private:

    /// start observation date/time to check for (as a string, might be empty)
    const OFString FromDateTime;
    /// end observation date/time to check for (as a string, might be empty)
    const OFString ToDateTime;
    /// start observation date/time to check for (as a converted OFDateTime instance)
    OFDateTime FromDateTimeValue;
    /// end observation date/time to check for (as a converted OFDateTime instance)
    OFDateTime ToDateTimeValue;
};


/** Class implementing a document tree node filter that checks for a given observation
 *  unique identifier
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeObservationUIDFilter
  : public DSRDocumentTreeNodeFilter
{

  public:

    /** constructor
     ** @param  observationUID  observation unique identifier to check for
     */
    DSRDocumentTreeNodeObservationUIDFilter(const OFString &observationUID);

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeObservationUIDFilter();

    /** check whether given node matches the filter criterion (observation unique
     *  identifier)
     ** @param  node  pointer to the node to be checked
     ** @return OFTrue if 'node' matches the filter criterion, OFFalse otherwise
     */
    virtual OFBool matches(const DSRDocumentTreeNode *node) const;


  private:

    /// observation unique identifier to check for
    const OFString ObservationUID;
};


/** Class implementing a document tree node filter that checks for a given template
 *  identification
 */
class DCMTK_DCMSR_EXPORT DSRDocumentTreeNodeTemplateIdentificationFilter
  : public DSRDocumentTreeNodeFilter
{

  public:

    /** constructor
     ** @param  templateIdentifier  template identifier to check for
     *  @param  mappingResource     mapping resource that defines the template
     *  @param  mappingResourceUID  uniquely identifies the mapping resource (optional).
     *                              Not used for comparison if the value is empty.
     */
    DSRDocumentTreeNodeTemplateIdentificationFilter(const OFString &templateIdentifier,
                                                    const OFString &mappingResource,
                                                    const OFString &mappingResourceUID = "");

    /** destructor
     */
    virtual ~DSRDocumentTreeNodeTemplateIdentificationFilter();

    /** check whether given node matches the filter criterion (template identification)
     ** @param  node  pointer to the node to be checked
     ** @return OFTrue if 'node' matches the filter criterion, OFFalse otherwise
     */
    virtual OFBool matches(const DSRDocumentTreeNode *node) const;


  private:

    /// template identifier to check for
    const OFString TemplateIdentifier;
    /// mapping resource to check for
    const OFString MappingResource;
    /// mapping resource unique identifier to check for (if not empty)
    const OFString MappingResourceUID;
};


#endif
