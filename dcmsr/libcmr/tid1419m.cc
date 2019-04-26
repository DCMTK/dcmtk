/*
 *
 *  Copyright (C) 2017-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class TID1419_ROIMeasurements_Measurement
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/tid1419m.h"
#include "dcmtk/dcmsr/cmr/tid15def.h"
#include "dcmtk/dcmsr/cmr/cid6147.h"
#include "dcmtk/dcmsr/cmr/cid7181.h"
#include "dcmtk/dcmsr/cmr/cid7464.h"
#include "dcmtk/dcmsr/cmr/cid7469.h"
#include "dcmtk/dcmsr/cmr/logger.h"
#include "dcmtk/dcmsr/codes/dcm.h"
#include "dcmtk/dcmsr/codes/sct.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"


// helper macros for checking the return value of API calls
#define CHECK_RESULT(call) if (result.good()) result = call
#define STORE_RESULT(call) result = call
#define GOOD_RESULT(call) if (result.good()) call
#define BAD_RESULT(call) if (result.bad()) call

// index positions in node list (makes source code more readable)
#define MEASUREMENT               0
#define LAST_MODIFIER             1
#define MEASUREMENT_METHOD        2
#define DERIVATION                3
#define LAST_FINDING_SITE         4
#define LAST_DERIVATION_PARAMETER 5
#define EQUIVALENT_MEANING        6
#define REAL_WORLD_VALUE_MAP      7
#define ALGORITHM_IDENTIFICATION  8
#define NUMBER_OF_LIST_ENTRIES    9

// general information on TID 1419 (ROI Measurements)
#define TEMPLATE_NUMBER      "1419"
#define MAPPING_RESOURCE     "DCMR"
#define MAPPING_RESOURCE_UID UID_DICOMContentMappingResource
#define TEMPLATE_TYPE        OFTrue   /* extensible */
#define TEMPLATE_ORDER       OFFalse  /* non-significant */


template<typename T_Measurement, typename T2, typename T3, typename T4>
TID1419_ROIMeasurements_Measurement<T_Measurement, T2, T3, T4>::TID1419_ROIMeasurements_Measurement()
  : DSRSubTemplate(TEMPLATE_NUMBER, MAPPING_RESOURCE, MAPPING_RESOURCE_UID),
    AlgorithmIdentification(new TID4019_AlgorithmIdentification())
{
    setExtensible(TEMPLATE_TYPE);
    setOrderSignificant(TEMPLATE_ORDER);
    /* need to store position of various content items */
    reserveEntriesInNodeList(NUMBER_OF_LIST_ENTRIES, OFTrue /*initialize*/);
}


template<typename T_Measurement, typename T2, typename T3, typename T4>
TID1419_ROIMeasurements_Measurement<T_Measurement, T2, T3, T4>::TID1419_ROIMeasurements_Measurement(const T_Measurement &conceptName,
                                                                                                    const MeasurementValue &numericValue,
                                                                                                    const OFBool check)
  : DSRSubTemplate(TEMPLATE_NUMBER, MAPPING_RESOURCE, MAPPING_RESOURCE_UID),
    AlgorithmIdentification(new TID4019_AlgorithmIdentification())
{
    setExtensible(TEMPLATE_TYPE);
    setOrderSignificant(TEMPLATE_ORDER);
    /* need to store position of various content items */
    reserveEntriesInNodeList(NUMBER_OF_LIST_ENTRIES, OFTrue /*initialize*/);
    /* create the mandatory "root" content item */
    createMeasurement(conceptName, numericValue, check);
}


template<typename T1, typename T2, typename T3, typename T4>
void TID1419_ROIMeasurements_Measurement<T1, T2, T3, T4>::clear()
{
    DSRSubTemplate::clear();
    AlgorithmIdentification->clear();
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1419_ROIMeasurements_Measurement<T1, T2, T3, T4>::isValid() const
{
    /* check whether base class is valid and all required content items are present */
    return DSRSubTemplate::isValid() && hasMeasurement();
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1419_ROIMeasurements_Measurement<T1, T2, T3, T4>::hasMeasurement() const
{
    /* check for content item at TID 1419 (Measurement) Row 5 */
    return (getEntryFromNodeList(MEASUREMENT) > 0);
}


template<typename T_Measurement, typename T2, typename T3, typename T4>
OFCondition TID1419_ROIMeasurements_Measurement<T_Measurement, T2, T3, T4>::createNewMeasurement(const T_Measurement &conceptName,
                                                                                                 const MeasurementValue &numericValue,
                                                                                                 const OFBool check)
{
    clear();
    /* TID 1419 (Measurement) Row 5 */
    return createMeasurement(conceptName, numericValue, check);
}


template<typename T1, typename T2, typename T_Method, typename T4>
OFCondition TID1419_ROIMeasurements_Measurement<T1, T2, T_Method, T4>::addModifier(const DSRCodedEntryValue &conceptName,
                                                                                   const DSRCodedEntryValue &modifier,
                                                                                   const OFBool check)
{
    OFCondition result = EC_IllegalParameter;
    /* basic check of parameters */
    if (conceptName.isComplete() && modifier.isComplete())
    {
        /* check whether measurement exists */
        if (hasMeasurement())
        {
            /* go to last instance of TID 1419 (ROI Measurements) Row 6 (if any) */
            if (gotoLastEntryFromNodeList(this, LAST_MODIFIER) == getEntryFromNodeList(MEASUREMENT))
            {
                /* need to add the new content item as the first child */
                if (addContentItem(RT_hasConceptMod, VT_Code, AM_belowCurrentBeforeFirstChild) > 0)
                {
                    if (getCurrentContentItem().setConceptName(conceptName, check).good())
                        result = EC_Normal;
                    else
                        result = SR_EC_InvalidConceptName;
                } else
                    result = SR_EC_CannotAddContentItem;
            } else
                STORE_RESULT(addContentItem(RT_hasConceptMod, VT_Code, conceptName, check));
            CHECK_RESULT(getCurrentContentItem().setCodeValue(modifier, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1419 - Row 6"));
            /* store ID of recently added node for later use */
            GOOD_RESULT(storeEntryInNodeList(LAST_MODIFIER, getNodeID()));
        } else
            result = CMR_EC_NoMeasurement;
    }
    return result;
}


template<typename T1, typename T2, typename T_Method, typename T4>
OFCondition TID1419_ROIMeasurements_Measurement<T1, T2, T_Method, T4>::setMeasurementMethod(const T_Method &method,
                                                                                            const OFBool check)
{
    OFCondition result = EC_IllegalParameter;
    /* basic check of parameter */
    if (method.hasSelectedValue())
    {
        /* check whether measurement exists */
        if (hasMeasurement())
        {
            /* TID 1419 (ROI Measurements) Row 7 */
            STORE_RESULT(addOrReplaceContentItem(MEASUREMENT_METHOD, RT_hasConceptMod, VT_Code, CODE_SCT_MeasurementMethod, "TID 1419 - Row 7", check));
            CHECK_RESULT(getCurrentContentItem().setCodeValue(method, check));
        } else
            result = CMR_EC_NoMeasurement;
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T_Derivation>
OFCondition TID1419_ROIMeasurements_Measurement<T1, T2, T3, T_Derivation>::setDerivation(const T_Derivation &derivation,
                                                                                         const OFBool check)
{
    OFCondition result = EC_IllegalParameter;
    /* basic check of parameter */
    if (derivation.hasSelectedValue())
    {
        /* check whether measurement exists */
        if (hasMeasurement())
        {
            /* TID 1419 (ROI Measurements) Row 8 */
            STORE_RESULT(addOrReplaceContentItem(DERIVATION, RT_hasConceptMod, VT_Code, CODE_DCM_Derivation, "TID 1419 - Row 8", check));
            CHECK_RESULT(getCurrentContentItem().setCodeValue(derivation, check));
        } else
            result = CMR_EC_NoMeasurement;
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1419_ROIMeasurements_Measurement<T1, T2, T3, T4>::addFindingSite(const DSRCodedEntryValue &site,
                                                                                const CID244e_Laterality &laterality,
                                                                                const DSRCodedEntryValue &siteModifier,
                                                                                const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of mandatory parameter */
    if (site.isComplete())
    {
        /* check whether measurement exists */
        if (hasMeasurement())
        {
            /* create a new subtree in order to "rollback" in case of error */
            DSRDocumentSubTree *subTree = new DSRDocumentSubTree;
            if (subTree != NULL)
            {
                /* TID 1419 (ROI Measurements) Row 9 */
                STORE_RESULT(subTree->addContentItem(RT_hasConceptMod, VT_Code, CODE_SCT_FindingSite, check));
                CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(site, check));
                CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1419 - Row 9"));
                const size_t lastNode = subTree->getNodeID();
                /* TID 1419 (ROI Measurements) Row 10 - optional */
                if (laterality.hasSelectedValue())
                {
                    CHECK_RESULT(subTree->addChildContentItem(RT_hasConceptMod, VT_Code, CODE_SCT_Laterality, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(laterality, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1419 - Row 10"));
                    GOOD_RESULT(subTree->gotoParent());
                }
                /* TID 1419 (ROI Measurements) Row 11 - optional */
                if (siteModifier.isComplete())
                {
                    CHECK_RESULT(subTree->addChildContentItem(RT_hasConceptMod, VT_Code, CODE_SCT_TopographicalModifier, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(siteModifier, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1419 - Row 11"));
                    GOOD_RESULT(subTree->gotoParent());
                }
                /* if everything was OK, insert new subtree into the template */
                if (result.good() && !subTree->isEmpty())
                {
                    /* go to last measurement (if any) */
                    if (gotoLastEntryFromNodeList(this, LAST_FINDING_SITE) == getEntryFromNodeList(MEASUREMENT))
                    {
                        /* insert subtree below root */
                        STORE_RESULT(insertSubTree(subTree, AM_belowCurrent));
                    } else  {
                        /* insert subtree after current position */
                        STORE_RESULT(insertSubTree(subTree, AM_afterCurrent));
                    }
                    /* store ID of recently added node for later use */
                    GOOD_RESULT(storeEntryInNodeList(LAST_FINDING_SITE, lastNode));
                    /* in case of error, make sure that memory is freed */
                    BAD_RESULT(delete subTree);
                } else {
                    /* delete the new subtree since it has not been inserted */
                    delete subTree;
                }
            } else
                result = EC_MemoryExhausted;
        } else
            result = CMR_EC_NoMeasurement;
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1419_ROIMeasurements_Measurement<T1, T2, T3, T4>::addDerivationParameter(const DSRCodedEntryValue &conceptName,
                                                                                        const CMR_SRNumericMeasurementValue &numericValue,
                                                                                        const OFBool check)
{
    OFCondition result = EC_IllegalParameter;
    /* basic check of parameters */
    if (conceptName.isComplete() && numericValue.isComplete())
    {
        /* check whether measurement exists */
        if (hasMeasurement())
        {
            /* go to last derivation parameter (if any) */
            if (gotoLastEntryFromNodeList(this, LAST_DERIVATION_PARAMETER) == getEntryFromNodeList(MEASUREMENT))
            {
                /* insert TID 1419 (ROI Measurements) Row 13 below root */
                STORE_RESULT(addChildContentItem(RT_inferredFrom, VT_Num, conceptName, check));
            } else {
               /* insert TID 1419 (ROI Measurements) Row 13 after current position */
                STORE_RESULT(addContentItem(RT_inferredFrom, VT_Num, conceptName, check));
            }
            CHECK_RESULT(getCurrentContentItem().setNumericValue(numericValue, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1419 - Row 13"));
            /* store ID of recently added node for later use */
            GOOD_RESULT(storeEntryInNodeList(LAST_DERIVATION_PARAMETER, getNodeID()));
        } else
            result = CMR_EC_NoMeasurement;
    }
    return result;
}


template<typename T_Measurement, typename T2, typename T3, typename T4>
OFCondition TID1419_ROIMeasurements_Measurement<T_Measurement, T2, T3, T4>::setEquivalentMeaningOfConceptName(const OFString &meaning,
                                                                                                              const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!meaning.empty())
    {
        /* check whether measurement exists */
        if (hasMeasurement())
        {
            /* TID 1419 (ROI Measurements) Row 18 */
            CHECK_RESULT(addOrReplaceContentItem(EQUIVALENT_MEANING, RT_hasConceptMod, VT_Text, CODE_DCM_EquivalentMeaningOfConceptName, "TID 1419 - Row 18", check));
            CHECK_RESULT(getCurrentContentItem().setStringValue(meaning, check));
        } else
            result = CMR_EC_NoMeasurement;
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T_Measurement, typename T2, typename T3, typename T4>
OFCondition TID1419_ROIMeasurements_Measurement<T_Measurement, T2, T3, T4>::setRealWorldValueMap(const DSRCompositeReferenceValue &valueMap,
                                                                                                 const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (valueMap.isComplete())
    {
        const char *annotationText = "TID 1419 - Row 19";
        const DSRBasicCodedEntry conceptName(CODE_DCM_RealWorldValueMapUsedForMeasurement);
        /* check for real world value mapping SOP classes */
        if (valueMap.getSOPClassUID() != UID_RealWorldValueMappingStorage)
        {
            DCMSR_CMR_WARN("Cannot set value of '" << conceptName.CodeMeaning << "' content item (" << annotationText << ") ... wrong SOP Class");
            DCMSR_CMR_DEBUG("SOP Class UID \"" << valueMap.getSOPClassUID() << "\" does not match the one of the Real World Value Mapping object");
            result = CMR_EC_InvalidRealWorldValueMappingObject;
        } else {
            /* check whether measurement exists */
            if (hasMeasurement())
            {
                /* TID 1419 (ROI Measurements) Row 19 */
                CHECK_RESULT(addOrReplaceContentItem(REAL_WORLD_VALUE_MAP, RT_inferredFrom, VT_Composite, conceptName, annotationText, check));
                CHECK_RESULT(getCurrentContentItem().setCompositeReference(valueMap, check));
            } else
                result = CMR_EC_NoMeasurement;
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T_Measurement, typename T2, typename T3, typename T4>
OFCondition TID1419_ROIMeasurements_Measurement<T_Measurement, T2, T3, T4>::setRealWorldValueMap(DcmItem &dataset,
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
OFCondition TID1419_ROIMeasurements_Measurement<T_Measurement, T2, T3, T4>::setAlgorithmIdentification(const OFString &algorithmName,
                                                                                                       const OFString &algorithmVersion,
                                                                                                       const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameters */
    if (!algorithmName.empty() && !algorithmVersion.empty())
    {
        /* check whether measurement exists */
        if (hasMeasurement())
        {
            /* TID 1419 (ROI Measurements) Row 20 */
            result = getAlgorithmIdentification().setIdentification(algorithmName, algorithmVersion, check);
        } else
            result = CMR_EC_NoMeasurement;
    } else
        result = EC_IllegalParameter;
    return result;
}


// protected methods

template<typename T_Measurement, typename T2, typename T3, typename T4>
OFCondition TID1419_ROIMeasurements_Measurement<T_Measurement, T2, T3, T4>::createMeasurement(const T_Measurement &conceptName,
                                                                                              const MeasurementValue &numericValue,
                                                                                              const OFBool check)
{
    OFCondition result = EC_IllegalParameter;
    /* basic check of parameters */
    if (conceptName.hasSelectedValue() && numericValue.isComplete())
    {
        /* reassure that the tree is definitely empty */
        if (isEmpty())
        {
            /* TID 1419 (ROI Measurements) Row 5 */
            STORE_RESULT(addContentItem(RT_unknown, VT_Num, conceptName, check));
            CHECK_RESULT(getCurrentContentItem().setNumericValue(numericValue, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1419 - Row 5"));
            GOOD_RESULT(storeEntryInNodeList(MEASUREMENT, getNodeID()));
            /* TID 1419 (ROI Measurements) Row 20 */
            CHECK_RESULT(includeTemplate(AlgorithmIdentification, AM_belowCurrent, RT_hasConceptMod));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1419 - Row 20"));
            GOOD_RESULT(storeEntryInNodeList(ALGORITHM_IDENTIFICATION, getNodeID()));
        } else
            result = SR_EC_InvalidTemplateStructure;
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1419_ROIMeasurements_Measurement<T1, T2, T3, T4>::addOrReplaceContentItem(const size_t nodePos,
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
            if (gotoLastEntryFromNodeList(this, nodePos) == getEntryFromNodeList(MEASUREMENT))
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


// explicit template instantiation (needed for use in TID 1411)
template class TID1419_ROIMeasurements_Measurement<CID7469_GenericIntensityAndSizeMeasurements,
                                                   CID7181_AbstractMultiDimensionalImageModelComponentUnits,
                                                   CID6147_ResponseCriteria,
                                                   CID7464_GeneralRegionOfInterestMeasurementModifiers>;
