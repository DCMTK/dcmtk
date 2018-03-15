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


#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdnflt.h"
#include "dcmtk/dcmsr/dsrdoctn.h"

#include "dcmtk/dcmdata/dcvrdt.h"


// implementation of base filter

DSRDocumentTreeNodeFilter::~DSRDocumentTreeNodeFilter()
{
}


// implementation of filter list

DSRDocumentTreeNodeFilterList::DSRDocumentTreeNodeFilterList()
  : FilterList()
{
}


DSRDocumentTreeNodeFilterList::~DSRDocumentTreeNodeFilterList()
{
    OFListIterator(DSRDocumentTreeNodeFilter *) iter = FilterList.begin();
    const OFListConstIterator(DSRDocumentTreeNodeFilter *) last = FilterList.end();
    /* delete all filters and free memory */
    while (iter != last)
    {
        delete (*iter);
        iter = FilterList.erase(iter);
    }
}


OFCondition DSRDocumentTreeNodeFilterList::addFilter(DSRDocumentTreeNodeFilter *filter)
{
    OFCondition result = EC_IllegalParameter;
    /* basic check of parameter */
    if (filter != NULL)
    {
        FilterList.push_back(filter);
        result = EC_Normal;
    }
    return result;
}


// implementation of AND filter

DSRDocumentTreeNodeAndFilter::~DSRDocumentTreeNodeAndFilter()
{
}


OFBool DSRDocumentTreeNodeAndFilter::matches(const DSRDocumentTreeNode *node) const
{
    OFBool result = OFFalse;
    if (node != NULL)
    {
        result = OFTrue;
        /* check whether all filters match */
        OFListConstIterator(DSRDocumentTreeNodeFilter *) iter = FilterList.begin();
        const OFListConstIterator(DSRDocumentTreeNodeFilter *) last = FilterList.end();
        while ((iter != last) && result)
        {
            result = (*iter)->matches(node);
            ++iter;
        }
    }
    return result;
}


// implementation of OR filter

DSRDocumentTreeNodeOrFilter::~DSRDocumentTreeNodeOrFilter()
{
}


OFBool DSRDocumentTreeNodeOrFilter::matches(const DSRDocumentTreeNode *node) const
{
    OFBool result = OFFalse;
    if (node != NULL)
    {
        /* check whether one of the filters matches */
        OFListConstIterator(DSRDocumentTreeNodeFilter *) iter = FilterList.begin();
        const OFListConstIterator(DSRDocumentTreeNodeFilter *) last = FilterList.end();
        while ((iter != last) && !result)
        {
            result = (*iter)->matches(node);
            ++iter;
        }
    }
    return result;
}


// implementation of "has Children" filter

DSRDocumentTreeNodeHasChildrenFilter::DSRDocumentTreeNodeHasChildrenFilter(const OFBool hasChildren)
  : HasChildren(hasChildren)
{
}


DSRDocumentTreeNodeHasChildrenFilter::~DSRDocumentTreeNodeHasChildrenFilter()
{
}


OFBool DSRDocumentTreeNodeHasChildrenFilter::matches(const DSRDocumentTreeNode *node) const
{
    /* check whether the expected value matches */
    return (node != NULL) && (node->hasChildNodes() == HasChildren);
}


// implementation of "has Siblings" filter

DSRDocumentTreeNodeHasSiblingsFilter::DSRDocumentTreeNodeHasSiblingsFilter(const OFBool hasSiblings)
  : HasSiblings(hasSiblings)
{
}


DSRDocumentTreeNodeHasSiblingsFilter::~DSRDocumentTreeNodeHasSiblingsFilter()
{
}


OFBool DSRDocumentTreeNodeHasSiblingsFilter::matches(const DSRDocumentTreeNode *node) const
{
    /* check whether the expected value matches */
    return (node != NULL) && (node->hasSiblingNodes() == HasSiblings);
}


// implementation of "has Concept Name" filter

DSRDocumentTreeNodeHasConceptNameFilter::DSRDocumentTreeNodeHasConceptNameFilter(const OFBool hasConceptName)
  : HasConceptName(hasConceptName)
{
}


DSRDocumentTreeNodeHasConceptNameFilter::~DSRDocumentTreeNodeHasConceptNameFilter()
{
}


OFBool DSRDocumentTreeNodeHasConceptNameFilter::matches(const DSRDocumentTreeNode *node) const
{
    /* check whether the expected value matches */
    return (node != NULL) && (node->getConceptName().isEmpty() != HasConceptName);
}


// implementation of Concept Name filter

DSRDocumentTreeNodeConceptNameFilter::DSRDocumentTreeNodeConceptNameFilter(const DSRCodedEntryValue &conceptName)
  : ConceptName(conceptName)
{
}


DSRDocumentTreeNodeConceptNameFilter::~DSRDocumentTreeNodeConceptNameFilter()
{
}


OFBool DSRDocumentTreeNodeConceptNameFilter::matches(const DSRDocumentTreeNode *node) const
{
    /* check whether the concept name matches */
    return (node != NULL) && (node->getConceptName() == ConceptName);
}


// implementation of Value Type filter

DSRDocumentTreeNodeValueTypeFilter::DSRDocumentTreeNodeValueTypeFilter(const DSRTypes::E_ValueType &valueType)
  : ValueType(valueType)
{
}


DSRDocumentTreeNodeValueTypeFilter::~DSRDocumentTreeNodeValueTypeFilter()
{
}


OFBool DSRDocumentTreeNodeValueTypeFilter::matches(const DSRDocumentTreeNode *node) const
{
    /* check whether the value type matches */
    return (node != NULL) && (node->getValueType() == ValueType);
}


// implementation of Relationship Type filter

DSRDocumentTreeNodeRelationshipTypeFilter::DSRDocumentTreeNodeRelationshipTypeFilter(const DSRTypes::E_RelationshipType &relationshipType)
  : RelationshipType(relationshipType)
{
}


DSRDocumentTreeNodeRelationshipTypeFilter::~DSRDocumentTreeNodeRelationshipTypeFilter()
{
}


OFBool DSRDocumentTreeNodeRelationshipTypeFilter::matches(const DSRDocumentTreeNode *node) const
{
    /* check whether the relationship type matches */
    return (node != NULL) && (node->getRelationshipType() == RelationshipType);
}


// implementation of Annotation filter

DSRDocumentTreeNodeAnnotationFilter::DSRDocumentTreeNodeAnnotationFilter(const DSRTreeNodeAnnotation &annotation)
  : Annotation(annotation)
{
}


DSRDocumentTreeNodeAnnotationFilter::~DSRDocumentTreeNodeAnnotationFilter()
{
}


OFBool DSRDocumentTreeNodeAnnotationFilter::matches(const DSRDocumentTreeNode *node) const
{
    /* check whether the annotation matches */
    return (node != NULL) && (node->getAnnotation() == Annotation);
}


// implementation of Observation Date/Time filter

DSRDocumentTreeNodeObservationDateTimeFilter::DSRDocumentTreeNodeObservationDateTimeFilter(const OFString &fromDateTime,
                                                                                           const OFString &toDateTime)
  : FromDateTime(fromDateTime),
    ToDateTime(toDateTime),
    FromDateTimeValue(),
    ToDateTimeValue()
{
    /* convert to internal format only once */
    DcmDateTime::getOFDateTimeFromString(fromDateTime, FromDateTimeValue);
    DcmDateTime::getOFDateTimeFromString(toDateTime, ToDateTimeValue);
}


DSRDocumentTreeNodeObservationDateTimeFilter::~DSRDocumentTreeNodeObservationDateTimeFilter()
{
}


OFBool DSRDocumentTreeNodeObservationDateTimeFilter::matches(const DSRDocumentTreeNode *node) const
{
    OFBool result = OFFalse;
    /* check whether the observation date/time matches */
    if (node != NULL)
    {
        /* special case: empty value */
        if (node->getObservationDateTime().empty())
            result = FromDateTime.empty() && ToDateTime.empty();
        else {
            const OFBool fromDateTimeValid = FromDateTimeValue.isValid();
            const OFBool toDateTimeValid = ToDateTimeValue.isValid();
            /* check for valid date/time range */
            if (fromDateTimeValid || toDateTimeValid)
            {
                OFDateTime dateTimeValue;
                if (DcmDateTime::getOFDateTimeFromString(node->getObservationDateTime(), dateTimeValue).good())
                {
                    /* NB: see limitations of OFDateTime class regarding "<=" comparison */
                    if (fromDateTimeValid && toDateTimeValid)
                        result = (FromDateTimeValue <= dateTimeValue) && (dateTimeValue <= ToDateTimeValue);
                    else if (fromDateTimeValid)
                        result = (FromDateTimeValue <= dateTimeValue);
                    else /* toDateTimeValid */
                        result = (dateTimeValue <= ToDateTimeValue);
                }
            }
        }
    }
    return result;
}


// implementation of Observation UID filter

DSRDocumentTreeNodeObservationUIDFilter::DSRDocumentTreeNodeObservationUIDFilter(const OFString &observationUID)
  : ObservationUID(observationUID)
{
}


DSRDocumentTreeNodeObservationUIDFilter::~DSRDocumentTreeNodeObservationUIDFilter()
{
}


OFBool DSRDocumentTreeNodeObservationUIDFilter::matches(const DSRDocumentTreeNode *node) const
{
    /* check whether the observation UID matches */
    return (node != NULL) && (node->getObservationUID() == ObservationUID);
}


// implementation of Template Identification filter

DSRDocumentTreeNodeTemplateIdentificationFilter::DSRDocumentTreeNodeTemplateIdentificationFilter(const OFString &templateIdentifier,
                                                                                                 const OFString &mappingResource,
                                                                                                 const OFString &mappingResourceUID)
  : TemplateIdentifier(templateIdentifier),
    MappingResource(mappingResource),
    MappingResourceUID(mappingResourceUID)
{
}


DSRDocumentTreeNodeTemplateIdentificationFilter::~DSRDocumentTreeNodeTemplateIdentificationFilter()
{
}


OFBool DSRDocumentTreeNodeTemplateIdentificationFilter::matches(const DSRDocumentTreeNode *node) const
{
    /* check whether the template identification matches */
    return (node != NULL) && (node->compareTemplateIdentification(TemplateIdentifier, MappingResource, MappingResourceUID));
}
