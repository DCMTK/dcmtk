/*
 *
 *  Copyright (C) 2017-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class TID300_Measurement
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/tid300.h"
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
#define LAST_IMAGE_OR_SCOORD      6
#define EQUIVALENT_MEANING        7
#define REAL_WORLD_VALUE_MAP      8
#define NUMBER_OF_LIST_ENTRIES    9

// general information on TID 300 (Measurement)
#define TEMPLATE_NUMBER      "300"
#define MAPPING_RESOURCE     "DCMR"
#define MAPPING_RESOURCE_UID UID_DICOMContentMappingResource
#define TEMPLATE_TYPE        OFTrue  /* extensible */
#define TEMPLATE_ORDER       OFTrue  /* significant */


template<typename T_Measurement, typename T2, typename T3, typename T4>
TID300_Measurement<T_Measurement, T2, T3, T4>::TID300_Measurement()
  : DSRSubTemplate(TEMPLATE_NUMBER, MAPPING_RESOURCE, MAPPING_RESOURCE_UID)
{
    setExtensible(TEMPLATE_TYPE);
    setOrderSignificant(TEMPLATE_ORDER);
    /* need to store position of various content items */
    reserveEntriesInNodeList(NUMBER_OF_LIST_ENTRIES, OFTrue /*initialize*/);
}


template<typename T_Measurement, typename T2, typename T3, typename T4>
TID300_Measurement<T_Measurement, T2, T3, T4>::TID300_Measurement(const T_Measurement &conceptName,
                                                                  const MeasurementValue &numericValue,
                                                                  const OFBool check)
  : DSRSubTemplate(TEMPLATE_NUMBER, MAPPING_RESOURCE, MAPPING_RESOURCE_UID)
{
    setExtensible(TEMPLATE_TYPE);
    setOrderSignificant(TEMPLATE_ORDER);
    /* need to store position of various content items */
    reserveEntriesInNodeList(NUMBER_OF_LIST_ENTRIES, OFTrue /*initialize*/);
    /* create the mandatory "root" content item */
    createMeasurement(conceptName, numericValue, check);
}


template<typename T1, typename T2, typename T3, typename T4>
void TID300_Measurement<T1, T2, T3, T4>::clear()
{
    DSRSubTemplate::clear();
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID300_Measurement<T1, T2, T3, T4>::isValid() const
{
    /* check whether base class is valid and all required content items are present */
    return DSRSubTemplate::isValid() && hasMeasurement();
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID300_Measurement<T1, T2, T3, T4>::hasMeasurement() const
{
    /* check for content item at TID 300 (Measurement) Row 1 */
    return (getEntryFromNodeList(MEASUREMENT) > 0);
}


template<typename T_Measurement, typename T2, typename T3, typename T4>
OFCondition TID300_Measurement<T_Measurement, T2, T3, T4>::createNewMeasurement(const T_Measurement &conceptName,
                                                                                const MeasurementValue &numericValue,
                                                                                const OFBool check)
{
    clear();
    /* TID 300 (Measurement) Row 1 */
    return createMeasurement(conceptName, numericValue, check);
}


template<typename T1, typename T2, typename T_Method, typename T4>
OFCondition TID300_Measurement<T1, T2, T_Method, T4>::addModifier(const DSRCodedEntryValue &conceptName,
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
            /* go to last instance of TID 300 (Measurement) Row 2 (if any) */
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
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 300 - Row 2"));
            /* store ID of recently added node for later use */
            GOOD_RESULT(storeEntryInNodeList(LAST_MODIFIER, getNodeID()));
        } else
            result = CMR_EC_NoMeasurement;
    }
    return result;
}


template<typename T1, typename T2, typename T_Method, typename T4>
OFCondition TID300_Measurement<T1, T2, T_Method, T4>::setMeasurementMethod(const T_Method &method,
                                                                           const OFBool check)
{
    OFCondition result = EC_IllegalParameter;
    /* basic check of parameter */
    if (method.hasSelectedValue())
    {
        /* check whether measurement exists */
        if (hasMeasurement())
        {
            /* TID 300 (Measurement) Row 3 */
            STORE_RESULT(addOrReplaceContentItem(MEASUREMENT_METHOD, RT_hasConceptMod, VT_Code, CODE_SCT_MeasurementMethod, "TID 300 - Row 3", check));
            CHECK_RESULT(getCurrentContentItem().setCodeValue(method, check));
        } else
            result = CMR_EC_NoMeasurement;
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T_Derivation>
OFCondition TID300_Measurement<T1, T2, T3, T_Derivation>::setDerivation(const T_Derivation &derivation,
                                                                        const OFBool check)
{
    OFCondition result = EC_IllegalParameter;
    /* basic check of parameter */
    if (derivation.hasSelectedValue())
    {
        /* check whether measurement exists */
        if (hasMeasurement())
        {
            /* TID 300 (Measurement) Row 4 */
            STORE_RESULT(addOrReplaceContentItem(DERIVATION, RT_hasConceptMod, VT_Code, CODE_DCM_Derivation, "TID 300 - Row 4", check));
            CHECK_RESULT(getCurrentContentItem().setCodeValue(derivation, check));
        } else
            result = CMR_EC_NoMeasurement;
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID300_Measurement<T1, T2, T3, T4>::addFindingSite(const DSRCodedEntryValue &site,
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
                /* TID 300 (Measurement) Row 5 */
                STORE_RESULT(subTree->addContentItem(RT_hasConceptMod, VT_Code, CODE_SCT_FindingSite, check));
                CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(site, check));
                CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 300 - Row 5"));
                const size_t lastNode = subTree->getNodeID();
                /* TID 300 (Measurement) Row 6 - optional */
                if (laterality.hasSelectedValue())
                {
                    CHECK_RESULT(subTree->addChildContentItem(RT_hasConceptMod, VT_Code, CODE_SCT_Laterality, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(laterality, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 300 - Row 6"));
                    GOOD_RESULT(subTree->gotoParent());
                }
                /* TID 300 (Measurement) Row 7 - optional */
                if (siteModifier.isComplete())
                {
                    CHECK_RESULT(subTree->addChildContentItem(RT_hasConceptMod, VT_Code, CODE_SCT_TopographicalModifier, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(siteModifier, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 300 - Row 7"));
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
OFCondition TID300_Measurement<T1, T2, T3, T4>::addDerivationParameter(const DSRCodedEntryValue &conceptName,
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
                /* insert TID 300 (Measurement) Row 9 below root */
                STORE_RESULT(addChildContentItem(RT_inferredFrom, VT_Num, conceptName, check));
            } else {
               /* insert TID 300 (Measurement) Row 9 after current position */
                STORE_RESULT(addContentItem(RT_inferredFrom, VT_Num, conceptName, check));
            }
            CHECK_RESULT(getCurrentContentItem().setNumericValue(numericValue, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 300 - Row 9"));
            /* store ID of recently added node for later use */
            GOOD_RESULT(storeEntryInNodeList(LAST_DERIVATION_PARAMETER, getNodeID()));
        } else
            result = CMR_EC_NoMeasurement;
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID300_Measurement<T1, T2, T3, T4>::addImage(const DSRCodedEntryValue &conceptName,
                                                         const DSRImageReferenceValue &imageReference,
                                                         const OFBool check)
{
    OFCondition result = EC_IllegalParameter;
    /* basic check of parameters */
    if (conceptName.isComplete() && imageReference.isComplete())
    {
        /* check whether measurement exists */
        if (hasMeasurement())
        {
            /* go to last image or spatial coordinates (if any) */
            if (gotoLastEntryFromNodeList(this, LAST_IMAGE_OR_SCOORD) == getEntryFromNodeList(MEASUREMENT))
            {
                /* insert TID 320 (Image or Spatial Coordinates) Row 1 below root */
                STORE_RESULT(addChildContentItem(RT_inferredFrom, VT_Image, conceptName, check));
            } else {
               /* insert TID 320 (Image or Spatial Coordinates) Row 1 after current position */
                STORE_RESULT(addContentItem(RT_inferredFrom, VT_Image, conceptName, check));
            }
            CHECK_RESULT(getCurrentContentItem().setImageReference(imageReference, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 320 - Row 1"));
            /* store ID of recently added node for later use */
            GOOD_RESULT(storeEntryInNodeList(LAST_IMAGE_OR_SCOORD, getNodeID()));
        } else
            result = CMR_EC_NoMeasurement;
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID300_Measurement<T1, T2, T3, T4>::addImage(const DSRCodedEntryValue &conceptName,
                                                         DcmItem &dataset,
                                                         const OFBool check)
{
    DSRImageReferenceValue imageReference;
    /* first, create the referenced image object */
    OFCondition result = imageReference.setReference(dataset, check);
    /* then, add/set the corresponding content item */
    CHECK_RESULT(addImage(conceptName, imageReference, check));
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID300_Measurement<T1, T2, T3, T4>::addSpatialCoordinates(const DSRCodedEntryValue &conceptName,
                                                                      const DSRSpatialCoordinatesValue &coordinatesValue,
                                                                      const DSRImageReferenceValue &imageReference,
                                                                      const OFBool check)
{
    OFCondition result = EC_IllegalParameter;
    /* basic check of parameters */
    if (conceptName.isComplete() && imageReference.isComplete())
    {
        /* check whether measurement exists */
        if (hasMeasurement())
        {
            /* create a new subtree in order to "rollback" in case of error */
            DSRDocumentSubTree *subTree = new DSRDocumentSubTree;
            if (subTree != NULL)
            {
                /* TID 320 (Image or Spatial Coordinates) Row 3 */
                STORE_RESULT(subTree->addContentItem(RT_inferredFrom, VT_SCoord, conceptName, check));
                CHECK_RESULT(subTree->getCurrentContentItem().setSpatialCoordinates(coordinatesValue, check));
                CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 320 - Row 3"));
                const size_t lastNode = subTree->getNodeID();
                /* TID 320 (Image or Spatial Coordinates) Row 4 */
                CHECK_RESULT(subTree->addChildContentItem(RT_selectedFrom, VT_Image, DSRCodedEntryValue() /* no concept name */));
                CHECK_RESULT(subTree->getCurrentContentItem().setImageReference(imageReference, check));
                CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 300 - Row 4"));
                /* if everything was OK, insert new subtree into the template */
                if (result.good() && !subTree->isEmpty())
                {
                    /* go to last measurement (if any) */
                    if (gotoLastEntryFromNodeList(this, LAST_IMAGE_OR_SCOORD) == getEntryFromNodeList(MEASUREMENT))
                    {
                        /* insert subtree below root */
                        STORE_RESULT(insertSubTree(subTree, AM_belowCurrent));
                    } else  {
                        /* insert subtree after current position */
                        STORE_RESULT(insertSubTree(subTree, AM_afterCurrent));
                    }
                    /* store ID of recently added node for later use */
                    GOOD_RESULT(storeEntryInNodeList(LAST_IMAGE_OR_SCOORD, lastNode));
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
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID300_Measurement<T1, T2, T3, T4>::setEquivalentMeaningOfConceptName(const OFString &meaning,
                                                                                  const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!meaning.empty())
    {
        /* check whether measurement exists */
        if (hasMeasurement())
        {
            /* TID 300 (Measurement) Row 16 */
            CHECK_RESULT(addOrReplaceContentItem(EQUIVALENT_MEANING, RT_hasConceptMod, VT_Text, CODE_DCM_EquivalentMeaningOfConceptName, "TID 300 - Row 16", check));
            CHECK_RESULT(getCurrentContentItem().setStringValue(meaning, check));
        } else
            result = CMR_EC_NoMeasurement;
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID300_Measurement<T1, T2, T3, T4>::setRealWorldValueMap(const DSRCompositeReferenceValue &valueMap,
                                                                     const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (valueMap.isComplete())
    {
        const char *annotationText = "TID 300 - Row 18";
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
                /* TID 300 (Measurement) Row 18 */
                CHECK_RESULT(addOrReplaceContentItem(REAL_WORLD_VALUE_MAP, RT_inferredFrom, VT_Composite, conceptName, annotationText, check));
                CHECK_RESULT(getCurrentContentItem().setCompositeReference(valueMap, check));
            } else
                result = CMR_EC_NoMeasurement;
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID300_Measurement<T1, T2, T3, T4>::setRealWorldValueMap(DcmItem &dataset,
                                                                     const OFBool check)
{
    DSRCompositeReferenceValue valueMap;
    /* first, create the referenced composite object */
    OFCondition result = valueMap.setReference(dataset, check);
    /* then, add/set the corresponding content item */
    CHECK_RESULT(setRealWorldValueMap(valueMap, check));
    return result;
}


// protected methods

template<typename T_Measurement, typename T2, typename T3, typename T4>
OFCondition TID300_Measurement<T_Measurement, T2, T3, T4>::createMeasurement(const T_Measurement &conceptName,
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
            /* TID 300 (Measurement) Row 1 */
            STORE_RESULT(addContentItem(RT_unknown, VT_Num, conceptName, check));
            CHECK_RESULT(getCurrentContentItem().setNumericValue(numericValue, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 300 - Row 1"));
            GOOD_RESULT(storeEntryInNodeList(MEASUREMENT, getNodeID()));
        } else
            result = SR_EC_InvalidTemplateStructure;
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID300_Measurement<T1, T2, T3, T4>::addOrReplaceContentItem(const size_t nodePos,
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


// explicit template instantiation (needed for use in TID 1501)
template class TID300_Measurement<CID7469_GenericIntensityAndSizeMeasurements,
                                  CID7181_AbstractMultiDimensionalImageModelComponentUnits,
                                  CID6147_ResponseCriteria,
                                  CID7464_GeneralRegionOfInterestMeasurementModifiers>;
