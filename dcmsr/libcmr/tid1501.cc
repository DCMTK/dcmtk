/*
 *
 *  Copyright (C) 2017-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class TID1501_MeasurementGroup
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/tid1501.h"
#include "dcmtk/dcmsr/cmr/tid15def.h"
#include "dcmtk/dcmsr/cmr/logger.h"
#include "dcmtk/dcmsr/codes/dcm.h"
#include "dcmtk/dcmsr/codes/ncit.h"
#include "dcmtk/dcmsr/codes/sct.h"
#include "dcmtk/dcmsr/codes/umls.h"
#include "dcmtk/dcmsr/dsrtpltn.h"

#include "dcmtk/dcmdata/dcuid.h"


// helper macros for checking the return value of API calls
#define CHECK_RESULT(call) if (result.good()) result = call
#define STORE_RESULT(call) result = call
#define GOOD_RESULT(call) if (result.good()) call
#define BAD_RESULT(call) if (result.bad()) call

// index positions in node list (makes source code more readable)
#define MEASUREMENT_GROUP               0
#define ACTIVITY_SESSION                1
#define TRACKING_IDENTIFIER             2
#define TRACKING_UNIQUE_IDENTIFIER      3
#define FINDING                         4
#define TIME_POINT                      5
#define MEASUREMENT_METHOD              6
#define LAST_FINDING_SITE               7
#define REAL_WORLD_VALUE_MAP            8
#define LAST_MEASUREMENT                9
#define LAST_QUALITATIVE_EVALUATION    10
#define NUMBER_OF_LIST_ENTRIES         11

// general information on TID 1501 (Measurement Group)
#define TEMPLATE_NUMBER      "1501"
#define MAPPING_RESOURCE     "DCMR"
#define MAPPING_RESOURCE_UID UID_DICOMContentMappingResource
#define TEMPLATE_TYPE        OFTrue   /* extensible */
#define TEMPLATE_ORDER       OFFalse  /* non-significant */


template<typename T1, typename T2, typename T3, typename T4>
TID1501_MeasurementGroup<T1, T2, T3, T4>::TID1501_MeasurementGroup(const OFBool createGroup)
  : DSRSubTemplate(TEMPLATE_NUMBER, MAPPING_RESOURCE, MAPPING_RESOURCE_UID),
    Measurement(new TID300_Measurement())
{
    setExtensible(TEMPLATE_TYPE);
    setOrderSignificant(TEMPLATE_ORDER);
    /* need to store position of various content items */
    reserveEntriesInNodeList(NUMBER_OF_LIST_ENTRIES, OFTrue /*initialize*/);
    /* TID 1501 (Measurement Group) Row 1 */
    if (createGroup)
        createMeasurementGroup();
}


template<typename T1, typename T2, typename T3, typename T4>
void TID1501_MeasurementGroup<T1, T2, T3, T4>::clear()
{
    DSRSubTemplate::clear();
    Measurement->clear();
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1501_MeasurementGroup<T1, T2, T3, T4>::isValid() const
{
    /* check whether base class is valid and all required content items are present */
    return DSRSubTemplate::isValid() &&
        hasMeasurementGroup() && hasTrackingIdentifier() && hasTrackingUniqueIdentifier() &&
        hasMeasurements(OFTrue /*checkChildren*/);
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1501_MeasurementGroup<T1, T2, T3, T4>::hasMeasurementGroup(const OFBool checkChildren) const
{
    OFBool result = OFFalse;
    /* need to check for child nodes? */
    if (checkChildren)
    {
        DSRDocumentTreeNodeCursor cursor(getRoot());
        /* go to content item at TID 1501 (Measurement Group) Row 1 */
        if (gotoEntryFromNodeList(cursor, MEASUREMENT_GROUP) > 0)
            result = cursor.hasChildNodes();
    } else {
        /* check for content item at TID 1501 (Measurement Group) Row 1 */
        result = (getEntryFromNodeList(MEASUREMENT_GROUP) > 0);
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1501_MeasurementGroup<T1, T2, T3, T4>::hasTrackingIdentifier() const
{
    /* check for content item at TID 1501 (Measurement Group) Row 2 */
    return (getEntryFromNodeList(TRACKING_IDENTIFIER) > 0);
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1501_MeasurementGroup<T1, T2, T3, T4>::hasTrackingUniqueIdentifier() const
{
    /* check for content item at TID 1501 (Measurement Group) Row 3 */
    return (getEntryFromNodeList(TRACKING_UNIQUE_IDENTIFIER) > 0);
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1501_MeasurementGroup<T1, T2, T3, T4>::hasMeasurements(const OFBool checkChildren) const
{
    OFBool result = OFFalse;
    /* need to check for child nodes? */
    if (checkChildren)
    {
        DSRDocumentTreeNodeCursor cursor(getRoot());
        /* go to content item at TID 1501 (Measurement Group) Row 10 */
        if (gotoEntryFromNodeList(cursor, LAST_MEASUREMENT) > 0)
        {
            /* check whether any of the "included TID 300 templates" is non-empty */
            while (cursor.isValid() && (cursor.getNode()->getValueType() == VT_includedTemplate))
            {
                const DSRSubTemplate *subTempl = OFstatic_cast(const DSRIncludedTemplateTreeNode *, cursor.getNode())->getValue().get();
                if (subTempl != NULL)
                {
                    if (subTempl->compareTemplateIdentication("300", "DCMR"))
                    {
                        result = !subTempl->isEmpty();
                        if (result) break;
                    } else {
                        /* exit loop */
                        break;
                    }
                }
                if (cursor.gotoPrevious() == 0)
                {
                    /* invalidate cursor */
                    cursor.clear();
                }
            }
        }
    } else {
        /* check for content item at TID 1501 (Measurement Group) Row 10 */
        result = (getEntryFromNodeList(LAST_MEASUREMENT) > 0);
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1501_MeasurementGroup<T1, T2, T3, T4>::setActivitySession(const OFString &session,
                                                                         const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!session.empty())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1501 (Measurement Group) Row 1b */
        CHECK_RESULT(addOrReplaceContentItem(ACTIVITY_SESSION, RT_hasObsContext, VT_Text, CODE_NCIt_ActivitySession, "TID 1501 - Row 1b", check));
        CHECK_RESULT(getCurrentContentItem().setStringValue(session, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1501_MeasurementGroup<T1, T2, T3, T4>::setTrackingIdentifier(const OFString &trackingID,
                                                                            const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!trackingID.empty())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1501 (Measurement Group) Row 2 */
        CHECK_RESULT(addOrReplaceContentItem(TRACKING_IDENTIFIER, RT_hasObsContext, VT_Text, CODE_DCM_TrackingIdentifier, "TID 1501 - Row 2", check));
        CHECK_RESULT(getCurrentContentItem().setStringValue(trackingID, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1501_MeasurementGroup<T1, T2, T3, T4>::setTrackingUniqueIdentifier(const OFString &trackingUID,
                                                                                  const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!trackingUID.empty())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1501 (Measurement Group) Row 3 */
        CHECK_RESULT(addOrReplaceContentItem(TRACKING_UNIQUE_IDENTIFIER, RT_hasObsContext, VT_UIDRef, CODE_DCM_TrackingUniqueIdentifier, "TID 1501 - Row 3", check));
        CHECK_RESULT(getCurrentContentItem().setStringValue(trackingUID, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1501_MeasurementGroup<T1, T2, T3, T4>::setFinding(const DSRCodedEntryValue &finding,
                                                                 const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (finding.isComplete())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1501 (Measurement Group) Row 3b */
        CHECK_RESULT(addOrReplaceContentItem(FINDING, RT_contains, VT_Code, CODE_DCM_Finding, "TID 1501 - Row 3b", check));
        CHECK_RESULT(getCurrentContentItem().setCodeValue(finding, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1501_MeasurementGroup<T1, T2, T3, T4>::setTimePoint(const OFString &timePoint,
                                                                   const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!timePoint.empty())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1502 (Time Point Context) Row 3 */
        CHECK_RESULT(addOrReplaceContentItem(TIME_POINT, RT_hasObsContext, VT_Text, CODE_UMLS_TimePoint, "TID 1502 - Row 3", check));
        CHECK_RESULT(getCurrentContentItem().setStringValue(timePoint, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T_Method, typename T4>
OFCondition TID1501_MeasurementGroup<T1, T2, T_Method, T4>::setMeasurementMethod(const T_Method &method,
                                                                                 const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (method.hasSelectedValue())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1501 (Measurement Group) Row 5 */
        CHECK_RESULT(addOrReplaceContentItem(MEASUREMENT_METHOD, RT_hasConceptMod, VT_Code, CODE_SCT_MeasurementMethod, "TID 1501 - Row 5", check));
        CHECK_RESULT(getCurrentContentItem().setCodeValue(method, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1501_MeasurementGroup<T1, T2, T3, T4>::addFindingSite(const DSRCodedEntryValue &site,
                                                                     const CID244e_Laterality &laterality,
                                                                     const DSRCodedEntryValue &siteModifier,
                                                                     const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of mandatory parameter */
    if (site.isComplete())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        if (result.good())
        {
            /* create a new subtree in order to "rollback" in case of error */
            DSRDocumentSubTree *subTree = new DSRDocumentSubTree;
            if (subTree != NULL)
            {
                /* 1501 (Measurement Group) Row 6 */
                CHECK_RESULT(subTree->addContentItem(RT_hasConceptMod, VT_Code, CODE_SCT_FindingSite, check));
                CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(site, check));
                CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1501 - Row 6"));
                const size_t lastNode = subTree->getNodeID();
                /* 1501 (Measurement Group) Row 7 - optional */
                if (laterality.hasSelectedValue())
                {
                    CHECK_RESULT(subTree->addChildContentItem(RT_hasConceptMod, VT_Code, CODE_SCT_Laterality, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(laterality, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1501 - Row 7"));
                    GOOD_RESULT(subTree->gotoParent());
                }
                /* 1501 (Measurement Group) Row 8 - optional */
                if (siteModifier.isComplete())
                {
                    CHECK_RESULT(subTree->addChildContentItem(RT_hasConceptMod, VT_Code, CODE_SCT_TopographicalModifier, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(siteModifier, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1501 - Row 8"));
                    GOOD_RESULT(subTree->gotoParent());
                }
                /* if everything was OK, insert new subtree into the template */
                if (result.good() && !subTree->isEmpty())
                {
                    /* go to last measurement (if any) */
                    if (gotoLastEntryFromNodeList(this, LAST_FINDING_SITE) > 0)
                    {
                        /* insert subtree at current position */
                        STORE_RESULT(insertSubTree(subTree, AM_afterCurrent));
                        /* store ID of recently added node for later use */
                        GOOD_RESULT(storeEntryInNodeList(LAST_FINDING_SITE, lastNode));
                    } else {
                        /* should never happen but ... */
                        result = CMR_EC_NoMeasurementGroup;
                    }
                    /* in case of error, make sure that memory is freed */
                    BAD_RESULT(delete subTree);
                } else {
                    /* delete the new subtree since it has not been inserted */
                    delete subTree;
                }
            } else
                result = EC_MemoryExhausted;
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1501_MeasurementGroup<T1, T2, T3, T4>::setRealWorldValueMap(const DSRCompositeReferenceValue &valueMap,
                                                                           const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (valueMap.isComplete())
    {
        const char *annotationText = "TID 1501 - Row 9";
        const DSRBasicCodedEntry conceptName(CODE_DCM_RealWorldValueMapUsedForMeasurement);
        /* check for real world value mapping SOP classes */
        if (valueMap.getSOPClassUID() != UID_RealWorldValueMappingStorage)
        {
            DCMSR_CMR_WARN("Cannot set value of '" << conceptName.CodeMeaning << "' content item (" << annotationText << ") ... wrong SOP Class");
            DCMSR_CMR_DEBUG("SOP Class UID \"" << valueMap.getSOPClassUID() << "\" does not match the one of the Real World Value Mapping object");
            result = CMR_EC_InvalidRealWorldValueMappingObject;
        } else {
            /* check whether measurement group already exists */
            if (!hasMeasurementGroup())
                result = createMeasurementGroup();
            /* TID 15011 (Measurement Group) Row 9 */
            CHECK_RESULT(addOrReplaceContentItem(REAL_WORLD_VALUE_MAP, RT_contains, VT_Composite, conceptName, annotationText, check));
            CHECK_RESULT(getCurrentContentItem().setCompositeReference(valueMap, check));
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1501_MeasurementGroup<T1, T2, T3, T4>::setRealWorldValueMap(DcmItem &dataset,
                                                                           const OFBool check)
{
    DSRCompositeReferenceValue valueMap;
    /* first, create the referenced composite object */
    OFCondition result = valueMap.setReference(dataset, check);
    /* then, add/set the corresponding content item */
    CHECK_RESULT(setRealWorldValueMap(valueMap, check));
    return result;
}


template<typename T_Measurement, typename T2, typename T3, typename T4>
OFCondition TID1501_MeasurementGroup<T_Measurement, T2, T3, T4>::addMeasurement(const T_Measurement &conceptName,
                                                                                const MeasurementValue &numericValue,
                                                                                const OFBool checkEmpty,
                                                                                const OFBool checkValue)
{
    OFCondition result = EC_Normal;
    /* basic check of mandatory parameters */
    if (conceptName.hasSelectedValue() && numericValue.isComplete())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        if (result.good())
        {
            /* go to content item at TID 1501 (Measurement Report) Row 10 */
            if (gotoEntryFromNodeList(this, LAST_MEASUREMENT) > 0)
            {
                /* check whether the current instance of TID 300 is non-empty (if needed) */
                if (checkEmpty && Measurement->isEmpty())
                    result = getMeasurement().createNewMeasurement(conceptName, numericValue, checkValue);
                else {
                    /* create new instance of TID 300 (Measurement) */
                    TID300_Measurement *subTempl = new TID300_Measurement(conceptName, numericValue, checkValue);
                    if (subTempl != NULL)
                    {
                        /* store (shared) reference to new instance */
                        Measurement.reset(subTempl);
                        /* and add it to the current template (TID 1501 - Row 10) */
                        STORE_RESULT(includeTemplate(Measurement, AM_afterCurrent, RT_contains));
                        CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1501 - Row 10"));
                        GOOD_RESULT(storeEntryInNodeList(LAST_MEASUREMENT, getNodeID()));
                        /* tbc: what if the call of includeTemplate() fails? */
                    } else
                        result = EC_MemoryExhausted;
                }
            } else
                result = CMR_EC_NoMeasurementGroup;
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1501_MeasurementGroup<T1, T2, T3, T4>::addQualitativeEvaluation(const DSRCodedEntryValue &conceptName,
                                                                               const DSRCodedEntryValue &codeValue,
                                                                               const OFBool check)
{
    OFCondition result = EC_Normal;
    /* make sure that the parameters are non-empty */
    if (conceptName.isComplete() && codeValue.isComplete())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        if (result.good())
        {
            /* go to last qualitative evaluation (if any) */
            if (gotoLastEntryFromNodeList(this, LAST_QUALITATIVE_EVALUATION) == getEntryFromNodeList(MEASUREMENT_GROUP))
            {
                /* insert TID 1501 (Measurement Group) Row 11 below root */
                STORE_RESULT(addChildContentItem(RT_contains, VT_Code, conceptName, check));
            } else {
               /* insert TID 1501 (Measurement Group) Row 11 after current position */
                STORE_RESULT(addContentItem(RT_contains, VT_Code, conceptName, check));
            }
            CHECK_RESULT(getCurrentContentItem().setCodeValue(codeValue, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1501 - Row 11"));
            /* store ID of recently added node for later use */
            GOOD_RESULT(storeEntryInNodeList(LAST_QUALITATIVE_EVALUATION, getNodeID()));
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1501_MeasurementGroup<T1, T2, T3, T4>::addQualitativeEvaluation(const DSRCodedEntryValue &conceptName,
                                                                               const OFString &stringValue,
                                                                               const OFBool check)
{
    OFCondition result = EC_Normal;
    /* make sure that the parameters are non-empty */
    if (conceptName.isComplete() && !stringValue.empty())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        if (result.good())
        {
            /* go to last qualitative evaluation (if any) */
            if (gotoLastEntryFromNodeList(this, LAST_QUALITATIVE_EVALUATION) == getEntryFromNodeList(MEASUREMENT_GROUP))
            {
                /* insert TID 1501 (Measurement Group) Row 12 below root */
                STORE_RESULT(addChildContentItem(RT_contains, VT_Text, conceptName, check));
            } else {
               /* insert TID 1501 (Measurement Group) Row 12 after current position */
                STORE_RESULT(addContentItem(RT_contains, VT_Text, conceptName, check));
            }
            CHECK_RESULT(getCurrentContentItem().setStringValue(stringValue, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1501 - Row 12"));
            /* store ID of recently added node for later use */
            GOOD_RESULT(storeEntryInNodeList(LAST_QUALITATIVE_EVALUATION, getNodeID()));
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


// protected methods

template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1501_MeasurementGroup<T1, T2, T3, T4>::createMeasurementGroup()
{
    OFCondition result = SR_EC_InvalidTemplateStructure;
    /* reassure that the tree is definitely empty */
    if (isEmpty())
    {
        /* TID 1501 (Measurement Group) Row 1 */
        STORE_RESULT(addContentItem(RT_unknown, VT_Container, CODE_DCM_MeasurementGroup));
        CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1501 - Row 1"));
        GOOD_RESULT(storeEntryInNodeList(MEASUREMENT_GROUP, getNodeID()));
        /* TID 1501 (Measurement) Row 10 */
        CHECK_RESULT(includeTemplate(Measurement, AM_belowCurrent, RT_contains));
        CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1501 - Row 10"));
        GOOD_RESULT(storeEntryInNodeList(LAST_MEASUREMENT, getNodeID()));
        /* if anything went wrong, clear the report */
        BAD_RESULT(clear());
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1501_MeasurementGroup<T1, T2, T3, T4>::addOrReplaceContentItem(const size_t nodePos,
                                                                              const E_RelationshipType relationshipType,
                                                                              const E_ValueType valueType,
                                                                              const DSRCodedEntryValue &conceptName,
                                                                              const OFString &annotationText,
                                                                              const OFBool check)
{
    OFCondition result = EC_Normal;
    /* check concept name and coded entry value */
    if (conceptName.isComplete())
    {
        /* check whether content item already exists */
        if (getEntryFromNodeList(nodePos) == 0)
        {
            /* if not, create the content item (at correct position) */
            if (gotoLastEntryFromNodeList(this, nodePos) == getEntryFromNodeList(MEASUREMENT_GROUP))
            {
                /* need to add the new content item as the first child */
                if (addContentItem(relationshipType, valueType, AM_belowCurrentBeforeFirstChild) > 0)
                {
                    if (getCurrentContentItem().setConceptName(conceptName, check).bad())
                        result = SR_EC_InvalidConceptName;
                } else
                    result = SR_EC_CannotAddContentItem;

            } else {
                /* add new content item as a sibling (after the current one) */
                STORE_RESULT(addContentItem(relationshipType, valueType, conceptName));
            }
            /* store ID of added node for later use */
            GOOD_RESULT(storeEntryInNodeList(nodePos, getNodeID()));
        }
        else if (gotoEntryFromNodeList(this, nodePos) > 0)
        {
            /* make sure that the value type of the existing content item is correct */
            if (getCurrentContentItem().getValueType() != valueType)
            {
                DCMSR_CMR_WARN("Cannot replace value of '" << conceptName.getCodeMeaning()
                    << "' content item (" << annotationText << ") ... wrong value type");
                result = SR_EC_InvalidContentItem;
            }
            else if (getCurrentContentItem().getConceptName() != conceptName)
            {
                DCMSR_CMR_WARN("Cannot replace value of '" << conceptName.getCodeMeaning()
                    << "' content item (" << annotationText << ") ... wrong concept name");
                result = SR_EC_InvalidConceptName;
            } else {
                DCMSR_CMR_DEBUG("Replacing value of '" << conceptName.getCodeMeaning()
                    << "' content item (" << annotationText << ")");
                /* the actual replacing of the value is done by the caller of this method */
            }
        } else
            result = SR_EC_InvalidTemplateStructure;
        /* finally, set annotation */
        CHECK_RESULT(getCurrentContentItem().setAnnotationText(annotationText));
    } else
        result = SR_EC_InvalidConceptName;
    return result;
}


// explicit template instantiation (needed for use in TID 1500)
template class TID1501_MeasurementGroup<CID7469_GenericIntensityAndSizeMeasurements,
                                        CID7181_AbstractMultiDimensionalImageModelComponentUnits,
                                        CID6147_ResponseCriteria,
                                        CID7464_GeneralRegionOfInterestMeasurementModifiers>;
