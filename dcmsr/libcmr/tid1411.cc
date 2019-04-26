/*
 *
 *  Copyright (C) 2016-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class TID1411_VolumetricROIMeasurements
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/tid1411.h"
#include "dcmtk/dcmsr/cmr/tid15def.h"
#include "dcmtk/dcmsr/cmr/logger.h"
#include "dcmtk/dcmsr/codes/dcm.h"
#include "dcmtk/dcmsr/codes/ncit.h"
#include "dcmtk/dcmsr/codes/sct.h"
#include "dcmtk/dcmsr/codes/umls.h"
#include "dcmtk/dcmsr/dsrtpltn.h"

#include "dcmtk/dcmdata/dcdeftag.h"
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
#define REFERENCED_SEGMENT              6
#define SOURCE_SERIES_FOR_SEGMENTATION  7
#define REAL_WORLD_VALUE_MAP            8
#define MEASUREMENT_METHOD              9
#define LAST_FINDING_SITE              10
#define LAST_MEASUREMENT               11
#define LAST_QUALITATIVE_EVALUATION    12
#define NUMBER_OF_LIST_ENTRIES         13

// general information on TID 1411 (Volumetric ROI Measurements)
#define TEMPLATE_NUMBER      "1411"
#define MAPPING_RESOURCE     "DCMR"
#define MAPPING_RESOURCE_UID UID_DICOMContentMappingResource
#define TEMPLATE_TYPE        OFTrue   /* extensible */
#define TEMPLATE_ORDER       OFFalse  /* non-significant */


template<typename T1, typename T2, typename T3, typename T4>
TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::TID1411_VolumetricROIMeasurements(const OFBool createGroup)
  : DSRSubTemplate(TEMPLATE_NUMBER, MAPPING_RESOURCE, MAPPING_RESOURCE_UID),
    Measurement(new TID1419_Measurement())
{
    setExtensible(TEMPLATE_TYPE);
    setOrderSignificant(TEMPLATE_ORDER);
    /* need to store position of various content items */
    reserveEntriesInNodeList(NUMBER_OF_LIST_ENTRIES, OFTrue /*initialize*/);
    /* TID 1411 (Volumetric ROI Measurements) Row 1 */
    if (createGroup)
        createMeasurementGroup();
}


template<typename T1, typename T2, typename T3, typename T4>
void TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::clear()
{
    DSRSubTemplate::clear();
    Measurement->clear();
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::isValid() const
{
    /* check whether base class is valid and all required content items are present */
    return DSRSubTemplate::isValid() &&
        hasMeasurementGroup() && hasTrackingIdentifier() && hasTrackingUniqueIdentifier() &&
        hasReferencedSegment() && hasSourceSeriesForSegmentation() && hasMeasurements(OFTrue /*checkChildren*/);
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::hasMeasurementGroup(const OFBool checkChildren) const
{
    OFBool result = OFFalse;
    /* need to check for child nodes? */
    if (checkChildren)
    {
        DSRDocumentTreeNodeCursor cursor(getRoot());
        /* go to content item at TID 1411 (Volumetric ROI Measurements) Row 1 */
        if (gotoEntryFromNodeList(cursor, MEASUREMENT_GROUP) > 0)
            result = cursor.hasChildNodes();
    } else {
        /* check for content item at TID 1411 (Volumetric ROI Measurements) Row 1 */
        result = (getEntryFromNodeList(MEASUREMENT_GROUP) > 0);
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::hasTrackingIdentifier() const
{
    /* check for content item at TID 1411 (Volumetric ROI Measurements) Row 2 */
    return (getEntryFromNodeList(TRACKING_IDENTIFIER) > 0);
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::hasTrackingUniqueIdentifier() const
{
    /* check for content item at TID 1411 (Volumetric ROI Measurements) Row 3 */
    return (getEntryFromNodeList(TRACKING_UNIQUE_IDENTIFIER) > 0);
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::hasReferencedSegment() const
{
    /* check for content item at TID 1411 (Volumetric ROI Measurements) Row 7 */
    return (getEntryFromNodeList(REFERENCED_SEGMENT) > 0);
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::hasSourceSeriesForSegmentation() const
{
    /* check for content item at TID 1411 (Volumetric ROI Measurements) Row 12 */
    return (getEntryFromNodeList(SOURCE_SERIES_FOR_SEGMENTATION) > 0);
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::hasMeasurements(const OFBool checkChildren) const
{
    OFBool result = OFFalse;
    /* need to check for child nodes? */
    if (checkChildren)
    {
        DSRDocumentTreeNodeCursor cursor(getRoot());
        /* go to content item at TID 1411 (Volumetric ROI Measurements) Row 13 */
        if (gotoEntryFromNodeList(cursor, LAST_MEASUREMENT) > 0)
        {
            /* check whether any of the "included TID 1419 templates" is non-empty */
            while (cursor.isValid() && (cursor.getNode()->getValueType() == VT_includedTemplate))
            {
                const DSRSubTemplate *subTempl = OFstatic_cast(const DSRIncludedTemplateTreeNode *, cursor.getNode())->getValue().get();
                if (subTempl != NULL)
                {
                    if (subTempl->compareTemplateIdentication("1419", "DCMR"))
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
        /* check for content item at TID 1411 (Volumetric ROI Measurements) Row 13 */
        result = (getEntryFromNodeList(LAST_MEASUREMENT) > 0);
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setActivitySession(const OFString &session,
                                                                                  const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!session.empty())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1411 (Volumetric ROI Measurements) Row 1b */
        CHECK_RESULT(addOrReplaceContentItem(ACTIVITY_SESSION, RT_hasObsContext, VT_Text, CODE_NCIt_ActivitySession, "TID 1411 - Row 1b", check));
        CHECK_RESULT(getCurrentContentItem().setStringValue(session, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setTrackingIdentifier(const OFString &trackingID,
                                                                                     const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!trackingID.empty())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1411 (Volumetric ROI Measurements) Row 2 */
        CHECK_RESULT(addOrReplaceContentItem(TRACKING_IDENTIFIER, RT_hasObsContext, VT_Text, CODE_DCM_TrackingIdentifier, "TID 1411 - Row 2", check));
        CHECK_RESULT(getCurrentContentItem().setStringValue(trackingID, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setTrackingUniqueIdentifier(const OFString &trackingUID,
                                                                                           const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!trackingUID.empty())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1411 (Volumetric ROI Measurements) Row 3 */
        CHECK_RESULT(addOrReplaceContentItem(TRACKING_UNIQUE_IDENTIFIER, RT_hasObsContext, VT_UIDRef, CODE_DCM_TrackingUniqueIdentifier, "TID 1411 - Row 3", check));
        CHECK_RESULT(getCurrentContentItem().setStringValue(trackingUID, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setFinding(const DSRCodedEntryValue &finding,
                                                                          const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (finding.isComplete())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1411 (Volumetric ROI Measurements) Row 3b */
        CHECK_RESULT(addOrReplaceContentItem(FINDING, RT_contains, VT_Code, CODE_DCM_Finding, "TID 1411 - Row 3b", check));
        CHECK_RESULT(getCurrentContentItem().setCodeValue(finding, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setTimePoint(const OFString &timePoint,
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


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setReferencedSegment(const DSRImageReferenceValue &segment,
                                                                                    const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (segment.isComplete())
    {
        const char *annotationText = "TID 1411 - Row 7";
        const DSRBasicCodedEntry conceptName(CODE_DCM_ReferencedSegment);
        /* check for supported segmentation SOP classes */
        if ((segment.getSOPClassUID() != UID_SegmentationStorage) && (segment.getSOPClassUID() != UID_SurfaceSegmentationStorage))
        {
            DCMSR_CMR_WARN("Cannot set value of '" << conceptName.CodeMeaning << "' content item (" << annotationText << ") ... wrong SOP Class");
            DCMSR_CMR_DEBUG("SOP Class UID \"" << segment.getSOPClassUID() << "\" does not match one of the known Segmentation objects");
            result = CMR_EC_InvalidSegmentationObject;
        }
        /*  ... and number of referenced segments */
        else if ((segment.getSegmentList().getNumberOfItems() != 1))
        {
            DCMSR_CMR_WARN("Cannot set value of '" << conceptName.CodeMeaning << "' content item (" << annotationText << ") ... wrong number of segments");
            result = CMR_EC_InvalidSegmentationObject;
        } else {
            /* check whether measurement group already exists */
            if (!hasMeasurementGroup())
                result = createMeasurementGroup();
            /* TID 1411 (Volumetric ROI Measurements) Row 7 */
            CHECK_RESULT(addOrReplaceContentItem(REFERENCED_SEGMENT, RT_contains, VT_Image, conceptName, annotationText, check));
            CHECK_RESULT(getCurrentContentItem().setImageReference(segment, check));
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setReferencedSegment(DcmItem &dataset,
                                                                                    const Uint16 segmentNumber,
                                                                                    const OFBool copyTracking,
                                                                                    const OFBool check)
{
    DSRImageReferenceValue segment;
    /* first, create the referenced image/segment object */
    OFCondition result = segment.setReference(dataset, check);
    segment.getSegmentList().addItem(segmentNumber);
    /* then, add/set the corresponding content item */
    CHECK_RESULT(setReferencedSegment(segment, check));
    /* need to copy tracking information? (introduced with CP-1496) */
    if (copyTracking && result.good())
    {
        DcmSequenceOfItems *dseq = NULL;
        /* get SegmentSequence (should always be present) */
        result = dataset.findAndGetSequence(DCM_SegmentSequence, dseq);
        checkElementValue(dseq, DCM_SegmentSequence, "1-n", "1", result, "SegmentDescriptionMacro");
        if (result.good())
        {
            DcmObject *dobj = NULL;
            OFBool segmentFound = OFFalse;
            /* iterate over all items in this sequence */
            while (((dobj = dseq->nextInContainer(dobj)) != NULL) && !segmentFound)
            {
                Uint16 number = 0;
                DcmItem *ditem = OFstatic_cast(DcmItem *, dobj);
                /* search for given segment number */
                if (ditem->findAndGetUint16(DCM_SegmentNumber, number).good())
                {
                    if (segmentNumber == number)
                    {
                        OFString trackingID, trackingUID;
                        /* get tracking ID and UID from current item (if present) and add/set content item */
                        getAndCheckStringValueFromDataset(*ditem, DCM_TrackingID, trackingID, "1", "1C", "SegmentSequence");
                        getAndCheckStringValueFromDataset(*ditem, DCM_TrackingUID, trackingUID, "1", "1C", "SegmentSequence");
                        if (!trackingID.empty() && !trackingUID.empty())
                        {
                            CHECK_RESULT(setTrackingIdentifier(trackingID, check));
                            CHECK_RESULT(setTrackingUniqueIdentifier(trackingUID, check));
                        }
                        else if (trackingID.empty() != trackingUID.empty())
                        {
                            /* report a violation of the type 1C conditions */
                            DCMSR_CMR_WARN("Either Tracking ID or Tracking UID is absent/empty in referenced segmentation object");
                        }
                        /* given segment number found */
                        segmentFound = OFTrue;
                    }
                }
            }
            /* report a warning if referenced segment could not be found */
            if (!segmentFound)
            {
                DCMSR_CMR_WARN("Cannot copy tracking information for '" << CODE_DCM_ReferencedSegment.CodeMeaning << "' content item (TID 1411 - Row 7) ... segment not found");
                DCMSR_CMR_DEBUG("Cannot find given Segment Number (" << segmentNumber << ") in Segment Sequence of referenced segmentation object");
            }
        } else {
            /* report a warning if referenced segment could not be found */
            DCMSR_CMR_WARN("Cannot copy tracking information for '" << CODE_DCM_ReferencedSegment.CodeMeaning << "' content item (TID 1411 - Row 7) ... segment not found");
        }
        /* tbc: return with an error in case the tracking information could not be copied? */
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setSourceSeriesForSegmentation(const OFString &seriesUID,
                                                                                              const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!seriesUID.empty())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1411 (Volumetric ROI Measurements) Row 12 */
        CHECK_RESULT(addOrReplaceContentItem(SOURCE_SERIES_FOR_SEGMENTATION, RT_contains, VT_UIDRef, CODE_DCM_SourceSeriesForSegmentation, "TID 1411 - Row 12", check));
        CHECK_RESULT(getCurrentContentItem().setStringValue(seriesUID, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setRealWorldValueMap(const DSRCompositeReferenceValue &valueMap,
                                                                                    const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (valueMap.isComplete())
    {
        const char *annotationText = "TID 1411 - Row 14";
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
            /* TID 1411 (Volumetric ROI Measurements) Row 14 */
            CHECK_RESULT(addOrReplaceContentItem(REAL_WORLD_VALUE_MAP, RT_contains, VT_Composite, conceptName, annotationText, check));
            CHECK_RESULT(getCurrentContentItem().setCompositeReference(valueMap, check));
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setRealWorldValueMap(DcmItem &dataset,
                                                                                    const OFBool check)
{
    DSRCompositeReferenceValue valueMap;
    /* first, create the referenced composite object */
    OFCondition result = valueMap.setReference(dataset, check);
    /* then, add/set the corresponding content item */
    CHECK_RESULT(setRealWorldValueMap(valueMap, check));
    return result;
}


template<typename T1, typename T2, typename T_Method, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T_Method, T4>::setMeasurementMethod(const T_Method &method,
                                                                                          const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (method.hasSelectedValue())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1419 (ROI Measurements) Row 1 */
        CHECK_RESULT(addOrReplaceContentItem(MEASUREMENT_METHOD, RT_hasConceptMod, VT_Code, CODE_SCT_MeasurementMethod, "TID 1419 - Row 1", check));
        CHECK_RESULT(getCurrentContentItem().setCodeValue(method, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::addFindingSite(const DSRCodedEntryValue &site,
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
                /* TID 1419 (ROI Measurements) Row 2 */
                CHECK_RESULT(subTree->addContentItem(RT_hasConceptMod, VT_Code, CODE_SCT_FindingSite, check));
                CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(site, check));
                CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1419 - Row 2"));
                const size_t lastNode = subTree->getNodeID();
                /* TID 1419 (ROI Measurements) Row 3 - optional */
                if (laterality.hasSelectedValue())
                {
                    CHECK_RESULT(subTree->addChildContentItem(RT_hasConceptMod, VT_Code, CODE_SCT_Laterality, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(laterality, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1419 - Row 3"));
                    GOOD_RESULT(subTree->gotoParent());
                }
                /* TID 1419 (ROI Measurements) Row 4 - optional */
                if (siteModifier.isComplete())
                {
                    CHECK_RESULT(subTree->addChildContentItem(RT_hasConceptMod, VT_Code, CODE_SCT_TopographicalModifier, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(siteModifier, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1419 - Row 4"));
                    GOOD_RESULT(subTree->gotoParent());
                }
                /* if everything was OK, insert new subtree into the template */
                if (result.good() && !subTree->isEmpty())
                {
                    /* go to last measurement (if any) */
                    if (gotoLastEntryFromNodeList(this, LAST_FINDING_SITE) == getEntryFromNodeList(MEASUREMENT_GROUP))
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


template<typename T_Measurement, typename T_Units, typename T_Method, typename T_Derivation>
OFCondition TID1411_VolumetricROIMeasurements<T_Measurement, T_Units, T_Method, T_Derivation>::addMeasurement(const T_Measurement &conceptName,
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
            /* go to content item at TID 1411 (Volumetric ROI Measurements) Row 15 */
            if (gotoEntryFromNodeList(this, LAST_MEASUREMENT) > 0)
            {
                /* check whether the current instance of TID 1419 is non-empty (if needed) */
                if (checkEmpty && Measurement->isEmpty())
                    result = getMeasurement().createNewMeasurement(conceptName, numericValue, checkValue);
                else {
                    /* create new instance of TID 1419 (ROI Measurements) */
                    TID1419_Measurement *subTempl = new TID1419_Measurement(conceptName, numericValue, checkValue);
                    if (subTempl != NULL)
                    {
                        /* store (shared) reference to new instance */
                        Measurement.reset(subTempl);
                        /* and add it to the current template (TID 1411 - Row 15) */
                        STORE_RESULT(includeTemplate(Measurement, AM_afterCurrent, RT_contains));
                        CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1411 - Row 15"));
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
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::addQualitativeEvaluation(const DSRCodedEntryValue &conceptName,
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
                /* insert TID 1411 (Volumetric ROI Measurements) Row 16 below root */
                STORE_RESULT(addChildContentItem(RT_contains, VT_Code, conceptName, check));
            } else {
               /* insert TID 1411 (Volumetric ROI Measurements) Row 16 after current position */
                STORE_RESULT(addContentItem(RT_contains, VT_Code, conceptName, check));
            }
            CHECK_RESULT(getCurrentContentItem().setCodeValue(codeValue, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1411 - Row 16"));
            /* store ID of recently added node for later use */
            GOOD_RESULT(storeEntryInNodeList(LAST_QUALITATIVE_EVALUATION, getNodeID()));
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::addQualitativeEvaluation(const DSRCodedEntryValue &conceptName,
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
                /* insert TID 1411 (Volumetric ROI Measurements) Row 17 below root */
                STORE_RESULT(addChildContentItem(RT_contains, VT_Text, conceptName, check));
            } else {
               /* insert TID 1411 (Volumetric ROI Measurements) Row 17 after current position */
                STORE_RESULT(addContentItem(RT_contains, VT_Text, conceptName, check));
            }
            CHECK_RESULT(getCurrentContentItem().setStringValue(stringValue, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1411 - Row 17"));
            /* store ID of recently added node for later use */
            GOOD_RESULT(storeEntryInNodeList(LAST_QUALITATIVE_EVALUATION, getNodeID()));
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


// protected methods

template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::createMeasurementGroup()
{
    OFCondition result = SR_EC_InvalidTemplateStructure;
    if (isEmpty())
    {
        /* TID 1411 (Volumetric ROI Measurements) Row 1 */
        STORE_RESULT(addContentItem(RT_unknown, VT_Container, CODE_DCM_MeasurementGroup));
        CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1411 - Row 1"));
        GOOD_RESULT(storeEntryInNodeList(MEASUREMENT_GROUP, getNodeID()));
        /* TID 1411 (Volumetric ROI Measurements) Row 15 */
        CHECK_RESULT(includeTemplate(Measurement, AM_belowCurrent, RT_contains));
        CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1411 - Row 15"));
        GOOD_RESULT(storeEntryInNodeList(LAST_MEASUREMENT, getNodeID()));
        /* if anything went wrong, clear the report */
        BAD_RESULT(clear());
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::addOrReplaceContentItem(const size_t nodePos,
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
template class TID1411_VolumetricROIMeasurements<CID7469_GenericIntensityAndSizeMeasurements,
                                                 CID7181_AbstractMultiDimensionalImageModelComponentUnits,
                                                 CID6147_ResponseCriteria,
                                                 CID7464_GeneralRegionOfInterestMeasurementModifiers>;
