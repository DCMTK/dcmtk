/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class TID1500_MeasurementReport
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/tid1500.h"
#include "dcmtk/dcmsr/cmr/logger.h"
#include "dcmtk/dcmsr/codes/dcm.h"
#include "dcmtk/dcmsr/codes/umls.h"

#include "dcmtk/dcmsr/dsrtpltn.h"


// helper macros for checking the return value of API calls
#define CHECK_RESULT(call) if (result.good()) result = call
#define STORE_RESULT(call) result = call
#define DELETE_ERROR(pointer) if (result.bad()) delete pointer

// index positions in node list (makes source code more readable)
#define MEASUREMENT_REPORT      0
#define OBSERVATION_CONTEXT     1
#define LAST_PROCEDURE_REPORTED 2
#define IMAGING_MEASUREMENTS    3
#define QUALITATIVE_EVALUATIONS 4

// general information on TID 1500 (Measurement Report)
#define TEMPLATE_NUMBER      "1500"
#define MAPPING_RESOURCE     "DCMR"
#define MAPPING_RESOURCE_UID UID_DICOMContentMappingResource
#define TEMPLATE_TYPE        OFTrue  /* extensible */

// conditions constants
makeOFConditionConst(CMR_EC_NoMeasurementReport, OFM_dcmsr, 1500, OF_error, "No Measurement Report");


// conditions constants


TID1500_MeasurementReport::TID1500_MeasurementReport(const CID7021_MeasurementReportDocumentTitles &title)
  : DSRRootTemplate(DT_EnhancedSR, TEMPLATE_NUMBER, MAPPING_RESOURCE, MAPPING_RESOURCE_UID),
    Language(new TID1204_LanguageOfContentItemAndDescendants),
    ObservationContext(new TID1001_ObservationContext),
    ImageLibrary(new TID1600_ImageLibrary)
{
    setExtensible(TEMPLATE_TYPE);
    /* need to store position of various content items */
    reserveEntriesInNodeList(5, OFTrue /*initialize*/);
    /* if specified, create an initial report */
    if (title.hasSelectedValue())
        createMeasurementReport(title);
}


void TID1500_MeasurementReport::clear()
{
    DSRRootTemplate::clear();
    Language->clear();
    ObservationContext->clear();
    ImageLibrary->clear();
}


OFBool TID1500_MeasurementReport::isValid() const
{
    /* check whether base class is valid and all members are valid */
    return DSRRootTemplate::isValid() &&
        Language->isValid() && ObservationContext->isValid() && ImageLibrary->isValid() &&
        hasProcedureReported() && hasImagingMeasurements() && hasQualitativeEvaluations();
}


OFBool TID1500_MeasurementReport::hasProcedureReported() const
{
    /* check for image library group (TID 1500 - Row 4) */
    return (getEntryFromNodeList(LAST_PROCEDURE_REPORTED) > 0);
}


OFBool TID1500_MeasurementReport::hasImagingMeasurements() const
{
    /* check for image library group (TID 1500 - Row 6) */
    return (getEntryFromNodeList(IMAGING_MEASUREMENTS) > 0);
}


OFBool TID1500_MeasurementReport::hasQualitativeEvaluations() const
{
    /* check for image library group (TID 1500 - Row 12) */
    return (getEntryFromNodeList(QUALITATIVE_EVALUATIONS) > 0);
}


OFCondition TID1500_MeasurementReport::getDocumentTitle(DSRCodedEntryValue &titleCode)
{
    OFCondition result = EC_Normal;
    /* go to content item at TID 1500 (Measurement Report) Row 1 */
    if (gotoEntryFromNodeList(this, MEASUREMENT_REPORT) > 0)
    {
        titleCode = getCurrentContentItem().getConceptName();
    } else {
        /* in case of error, clear the result variable */
        titleCode.clear();
        result = SR_EC_ContentItemNotFound;
    }
    return result;
}


OFCondition TID1500_MeasurementReport::createNewMeasurementReport(const CID7021_MeasurementReportDocumentTitles &title)
{
    clear();
    /* TID 1500 (Measurement Report) Row 1 */
    return createMeasurementReport(title);
}


OFCondition TID1500_MeasurementReport::setLanguage(const CID5000_Languages &language,
                                                   const CID5001_Countries &country,
                                                   const OFBool check)
{
    /* TID 1500 (Measurement Report) Row 2 */
    return getLanguage().setLanguage(language, country, check);
}


OFCondition TID1500_MeasurementReport::addProcedureReported(const CID100_QuantitativeDiagnosticImagingProcedures &procedure,
                                                            const OFBool check)
{
    OFCondition result = EC_IllegalParameter;
    /* make sure that there is a coded entry */
    if (procedure.hasSelectedValue())
    {
        /* go to content item at TID 1500 (Measurement Report) Row 3 or 4 */
        if (gotoLastEntryFromNodeList(this, LAST_PROCEDURE_REPORTED) > 0)
        {
            /* TID 1500 (Measurement Report) Row 4 */
            STORE_RESULT(addContentItem(RT_hasConceptMod, VT_Code, CODE_DCM_ProcedureReported));
            CHECK_RESULT(getCurrentContentItem().setCodeValue(procedure, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 4"));
            /* store ID of recently added node for later use */
            if (result.good())
                storeEntryInNodeList(LAST_PROCEDURE_REPORTED, getNodeID());
        } else
            result = CMR_EC_NoMeasurementReport;
    }
    return result;
}


// protected methods

OFCondition TID1500_MeasurementReport::createMeasurementReport(const CID7021_MeasurementReportDocumentTitles &title)
{
    OFCondition result = EC_IllegalParameter;
    /* make sure that there is a coded entry */
    if (title.hasSelectedValue())
    {
        /* reassure that the report is definitely empty */
        if (isEmpty())
        {
            /* TID 1500 (Measurement Report) Row 1 */
            STORE_RESULT(addChildContentItem(RT_isRoot, VT_Container, title));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 1"));
            /* store ID of root node for later use */
            if (result.good())
                storeEntryInNodeList(MEASUREMENT_REPORT, getNodeID());
            /* TID 1500 (Measurement Report) Row 2 */
            CHECK_RESULT(includeTemplate(Language, AM_belowCurrent, RT_hasConceptMod));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 2"));
            /* TID 1500 (Measurement Report) Row 3 */
            CHECK_RESULT(includeTemplate(ObservationContext, AM_afterCurrent, RT_hasObsContext));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 3"));
            /* store ID of current node for later use */
            if (result.good())
                storeEntryInNodeList(OBSERVATION_CONTEXT, getNodeID());
            /* TID 1500 (Measurement Report) Row 5 */
            CHECK_RESULT(includeTemplate(ImageLibrary, AM_afterCurrent, RT_contains));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 5"));
            /* TID 1500 (Measurement Report) Row 6 */
            CHECK_RESULT(addContentItem(RT_contains, VT_Container, CODE_DCM_ImagingMeasurements));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 6"));
            /* store ID of current node for later use */
            if (result.good())
                storeEntryInNodeList(IMAGING_MEASUREMENTS, getNodeID());
            /* TID 1500 (Measurement Report) Row 12 */
            CHECK_RESULT(addContentItem(RT_contains, VT_Container, CODE_UMLS_QualitativeEvaluations));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 12"));
            /* store ID of current node for later use */
            if (result.good())
                storeEntryInNodeList(QUALITATIVE_EVALUATIONS, getNodeID());
            /* if anything went wrong, clear the report */
            if (result.bad())
                clear();
        } else
            result = SR_EC_InvalidTemplateStructure;
    }
    return result;
}
