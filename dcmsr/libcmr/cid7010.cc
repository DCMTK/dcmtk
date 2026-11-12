/*
 *
 *  Copyright (C) 2015-2026, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID7010_KeyObjectSelectionDocumentTitle
 *
 *  Generated automatically from DICOM PS 3.16-2026d
 *  File created on 2026-11-12 15:49:15 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid7010.h"


// general information on CID 7010 (Key Object Selection Document Title)
#define CONTEXT_GROUP_NUMBER  "7010"
#define CONTEXT_GROUP_KEYWORD "KeyObjectSelectionDocumentTitle"
#define CONTEXT_GROUP_VERSION "20260603"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.490"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID7010_KeyObjectSelectionDocumentTitle::CodeList *CID7010_KeyObjectSelectionDocumentTitle::Codes = NULL;


CID7010_KeyObjectSelectionDocumentTitle::CID7010_KeyObjectSelectionDocumentTitle(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_KEYWORD, CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID7010_KeyObjectSelectionDocumentTitle::CID7010_KeyObjectSelectionDocumentTitle(const EnumType selectedValue,
                                                                                 const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_KEYWORD, CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID7010_KeyObjectSelectionDocumentTitle::selectValue(const EnumType selectedValue,
                                                                 const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID7010_KeyObjectSelectionDocumentTitle::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
                                                                    DSRCodedEntryValue *foundCodedEntry,
                                                                    const OFBool enhancedEncodingMode) const
{
    OFCondition result = SR_EC_CodedEntryNotInContextGroup;
    /* first, search for standard codes */
    CodeList::const_iterator iter = getCodes().begin();
    CodeList::const_iterator last = getCodes().end();
    /* iterate over coded entry list */
    while (iter != last)
    {
        /* if found, exit loop */
        if (searchForCodedEntry == iter->second)
        {
            /* return coded entry (if requested) */
            if (foundCodedEntry != NULL)
            {
                *foundCodedEntry = iter->second;
                /* also set enhanced encoding mode (if enabled) */
                if (!foundCodedEntry->isEmpty() && enhancedEncodingMode)
                    setEnhancedEncodingMode(*foundCodedEntry);
            }
            result = SR_EC_CodedEntryInStandardContextGroup;
            break;
        }
        ++iter;
    }
    /* if not, continue with extended codes */
    if (result.bad())
    {
        result = DSRContextGroup::findCodedEntry(searchForCodedEntry, foundCodedEntry);
        /* tbd: set "enhanced encoding mode" to mark a local/extended version? */
    }
    return result;
}


void CID7010_KeyObjectSelectionDocumentTitle::printCodes(STD_NAMESPACE ostream &stream) const
{
    /* print standard codes */
    stream << "Standard codes:" << OFendl;
    CodeList::const_iterator iter = getCodes().begin();
    CodeList::const_iterator last = getCodes().end();
    /* iterate over coded entry list */
    while (iter != last)
    {
        stream << "  ";
        /* print coded entry */
        DSRCodedEntryValue(iter->second).print(stream);
        stream << OFendl;
        ++iter;
    }
    /* print extended codes */
    DSRContextGroup::printCodes(stream);
}


// static functions

void CID7010_KeyObjectSelectionDocumentTitle::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID7010_KeyObjectSelectionDocumentTitle::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID7010_KeyObjectSelectionDocumentTitle::getCodedEntry(const EnumType value,
                                                                          const OFBool enhancedEncodingMode)
{
    DSRCodedEntryValue codedEntry;
    /* search for given enumerated value */
    CodeList::iterator iter = getCodes().find(value);
    /* if found, set the coded entry */
    if (iter != getCodes().end())
    {
        codedEntry = iter->second;
        /* also set enhanced encoding mode (if enabled) */
        if (!codedEntry.isEmpty() && enhancedEncodingMode)
            setEnhancedEncodingMode(codedEntry);
    }
    return codedEntry;
}


CID7010_KeyObjectSelectionDocumentTitle::CodeList &CID7010_KeyObjectSelectionDocumentTitle::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(OfInterest, DSRBasicCodedEntry("113000", "DCM", "Of Interest")));
        Codes->insert(OFMake_pair(RejectedForQualityReasons, DSRBasicCodedEntry("113001", "DCM", "Rejected for Quality Reasons")));
        Codes->insert(OFMake_pair(ForReferringProvider, DSRBasicCodedEntry("113002", "DCM", "For Referring Provider")));
        Codes->insert(OFMake_pair(ForSurgery, DSRBasicCodedEntry("113003", "DCM", "For Surgery")));
        Codes->insert(OFMake_pair(ForTeaching, DSRBasicCodedEntry("113004", "DCM", "For Teaching")));
        Codes->insert(OFMake_pair(ForConference, DSRBasicCodedEntry("113005", "DCM", "For Conference")));
        Codes->insert(OFMake_pair(ForTherapy, DSRBasicCodedEntry("113006", "DCM", "For Therapy")));
        Codes->insert(OFMake_pair(ForPatient, DSRBasicCodedEntry("113007", "DCM", "For Patient")));
        Codes->insert(OFMake_pair(ForPeerReview, DSRBasicCodedEntry("113008", "DCM", "For Peer Review")));
        Codes->insert(OFMake_pair(ForResearch, DSRBasicCodedEntry("113009", "DCM", "For Research")));
        Codes->insert(OFMake_pair(QualityIssue, DSRBasicCodedEntry("113010", "DCM", "Quality Issue")));
        Codes->insert(OFMake_pair(BestInSet, DSRBasicCodedEntry("113013", "DCM", "Best In Set")));
        Codes->insert(OFMake_pair(ForPrinting, DSRBasicCodedEntry("113018", "DCM", "For Printing")));
        Codes->insert(OFMake_pair(ForReportAttachment, DSRBasicCodedEntry("113020", "DCM", "For Report Attachment")));
        Codes->insert(OFMake_pair(ForLitigation, DSRBasicCodedEntry("113021", "DCM", "For Litigation")));
        Codes->insert(OFMake_pair(Manifest, DSRBasicCodedEntry("113030", "DCM", "Manifest")));
        Codes->insert(OFMake_pair(SignedManifest, DSRBasicCodedEntry("113031", "DCM", "Signed Manifest")));
        Codes->insert(OFMake_pair(ManifestWithDescription, DSRBasicCodedEntry("131560", "DCM", "Manifest with Description")));
        Codes->insert(OFMake_pair(CompleteStudyContent, DSRBasicCodedEntry("113032", "DCM", "Complete Study Content")));
        Codes->insert(OFMake_pair(SignedCompleteStudyContent, DSRBasicCodedEntry("113033", "DCM", "Signed Complete Study Content")));
        Codes->insert(OFMake_pair(CompleteAcquisitionContent, DSRBasicCodedEntry("113034", "DCM", "Complete Acquisition Content")));
        Codes->insert(OFMake_pair(SignedCompleteAcquisitionContent, DSRBasicCodedEntry("113035", "DCM", "Signed Complete Acquisition Content")));
        Codes->insert(OFMake_pair(GroupOfFramesForDisplay, DSRBasicCodedEntry("113036", "DCM", "Group of Frames for Display")));
        Codes->insert(OFMake_pair(RejectedForPatientSafetyReasons, DSRBasicCodedEntry("113037", "DCM", "Rejected for Patient Safety Reasons")));
        Codes->insert(OFMake_pair(RejectionWithdrawn, DSRBasicCodedEntry("131360", "DCM", "Rejection Withdrawn")));
        Codes->insert(OFMake_pair(IncorrectModalityWorklistEntry, DSRBasicCodedEntry("113038", "DCM", "Incorrect Modality Worklist Entry")));
        Codes->insert(OFMake_pair(DataRetentionPolicyExpired, DSRBasicCodedEntry("113039", "DCM", "Data Retention Policy Expired")));
        Codes->insert(OFMake_pair(CollectionOfPresentationStates, DSRBasicCodedEntry("113022", "DCM", "Collection of Presentation States")));
        Codes->insert(OFMake_pair(DiagnosticSourceImages, DSRBasicCodedEntry("128181", "DCM", "Diagnostic Source Images")));
        Codes->insert(OFMake_pair(SegmentationResult, DSRBasicCodedEntry("128182", "DCM", "Segmentation Result")));
        Codes->insert(OFMake_pair(RegistrationResult, DSRBasicCodedEntry("128183", "DCM", "Registration Result")));
        Codes->insert(OFMake_pair(ForDiagnosis, DSRBasicCodedEntry("128195", "DCM", "For Diagnosis")));
        Codes->insert(OFMake_pair(DiagnosisInputUsed, DSRBasicCodedEntry("128218", "DCM", "Diagnosis Input Used")));
        Codes->insert(OFMake_pair(ForSegmentation, DSRBasicCodedEntry("128196", "DCM", "For Segmentation")));
        Codes->insert(OFMake_pair(ContouringInputUsed, DSRBasicCodedEntry("128219", "DCM", "Contouring Input Used")));
        Codes->insert(OFMake_pair(ForPlanComparison, DSRBasicCodedEntry("128199", "DCM", "For Plan Comparison")));
        Codes->insert(OFMake_pair(PlanComparisonInputUsed, DSRBasicCodedEntry("128220", "DCM", "Plan Comparison Input Used")));
        Codes->insert(OFMake_pair(ForTumorBoard, DSRBasicCodedEntry("128203", "DCM", "For Tumor Board")));
        Codes->insert(OFMake_pair(TumorBoardInputUsed, DSRBasicCodedEntry("128221", "DCM", "Tumor Board Input Used")));
        Codes->insert(OFMake_pair(ForTumorRegistry, DSRBasicCodedEntry("128208", "DCM", "For Tumor Registry")));
        Codes->insert(OFMake_pair(TumorRegistryInputUsed, DSRBasicCodedEntry("128222", "DCM", "Tumor Registry Input Used")));
        Codes->insert(OFMake_pair(ForClinicalTrialSubmission, DSRBasicCodedEntry("128207", "DCM", "For Clinical Trial Submission")));
        Codes->insert(OFMake_pair(ClinicalTrialSubmissionInputUsed, DSRBasicCodedEntry("128223", "DCM", "Clinical Trial Submission Input Used")));
        Codes->insert(OFMake_pair(PrePlanningResult, DSRBasicCodedEntry("128184", "DCM", "Pre-Planning Result")));
        Codes->insert(OFMake_pair(RTPrescriptionResult, DSRBasicCodedEntry("128185", "DCM", "RT Prescription Result")));
        Codes->insert(OFMake_pair(DoseCalculationImageSeries, DSRBasicCodedEntry("128186", "DCM", "Dose Calculation Image Series")));
        Codes->insert(OFMake_pair(CoordinateAlignmentImageSeries, DSRBasicCodedEntry("128187", "DCM", "Coordinate Alignment Image Series")));
        Codes->insert(OFMake_pair(RTTreatmentSimulationResult, DSRBasicCodedEntry("128188", "DCM", "RT Treatment Simulation Result")));
        Codes->insert(OFMake_pair(RTPlanningResult, DSRBasicCodedEntry("128189", "DCM", "RT Planning Result")));
        Codes->insert(OFMake_pair(DosimetricResult, DSRBasicCodedEntry("128190", "DCM", "Dosimetric Result")));
        Codes->insert(OFMake_pair(PatientSetupVerificationResult, DSRBasicCodedEntry("128191", "DCM", "Patient Setup Verification Result")));
        Codes->insert(OFMake_pair(RTTreatmentSessionResult, DSRBasicCodedEntry("128192", "DCM", "RT Treatment Session Result")));
        Codes->insert(OFMake_pair(RTTreatmentCourseSummary, DSRBasicCodedEntry("128193", "DCM", "RT Treatment Course Summary")));
        Codes->insert(OFMake_pair(RTTreatmentQAResult, DSRBasicCodedEntry("128194", "DCM", "RT Treatment QA Result")));
        Codes->insert(OFMake_pair(ForRTWorkflow, DSRBasicCodedEntry("128180", "DCM", "For RT Workflow")));
        Codes->insert(OFMake_pair(ForRTPrescription, DSRBasicCodedEntry("128197", "DCM", "For RT Prescription")));
        Codes->insert(OFMake_pair(ForRTTreatmentPlanning, DSRBasicCodedEntry("128198", "DCM", "For RT Treatment Planning")));
        Codes->insert(OFMake_pair(ForRTPlanSummation, DSRBasicCodedEntry("128200", "DCM", "For RT Plan Summation")));
        Codes->insert(OFMake_pair(ForPhysicianReview, DSRBasicCodedEntry("128201", "DCM", "For Physician Review")));
        Codes->insert(OFMake_pair(ForPhysicistReview, DSRBasicCodedEntry("128202", "DCM", "For Physicist Review")));
        Codes->insert(OFMake_pair(ForPlanQualityAssurance, DSRBasicCodedEntry("128204", "DCM", "For Plan Quality Assurance")));
        Codes->insert(OFMake_pair(ForMachineQualityAssurance, DSRBasicCodedEntry("128205", "DCM", "For Machine Quality Assurance")));
        Codes->insert(OFMake_pair(ForPatientSetupVerification, DSRBasicCodedEntry("128206", "DCM", "For Patient Setup Verification")));
        Codes->insert(OFMake_pair(RTWorkflowInputUsed, DSRBasicCodedEntry("128209", "DCM", "RT Workflow Input Used")));
        Codes->insert(OFMake_pair(RTPrescriptionInputUsed, DSRBasicCodedEntry("128210", "DCM", "RT Prescription Input Used")));
        Codes->insert(OFMake_pair(RTTreatmentPlanningInputUsed, DSRBasicCodedEntry("128211", "DCM", "RT Treatment Planning Input Used")));
        Codes->insert(OFMake_pair(RTPlanSummationInputUsed, DSRBasicCodedEntry("128212", "DCM", "RT Plan Summation Input Used")));
        Codes->insert(OFMake_pair(PhysicianReviewInputUsed, DSRBasicCodedEntry("128213", "DCM", "Physician Review Input Used")));
        Codes->insert(OFMake_pair(PhysicistReviewInputUsed, DSRBasicCodedEntry("128214", "DCM", "Physicist Review Input Used")));
        Codes->insert(OFMake_pair(PlanQualityAssuranceInputUsed, DSRBasicCodedEntry("128215", "DCM", "Plan Quality Assurance Input Used")));
        Codes->insert(OFMake_pair(MachineQualityAssuranceInputUsed, DSRBasicCodedEntry("128216", "DCM", "Machine Quality Assurance Input Used")));
        Codes->insert(OFMake_pair(PatientSetupVerificationInputUsed, DSRBasicCodedEntry("128217", "DCM", "Patient Setup Verification Input Used")));
        Codes->insert(OFMake_pair(ForTeachingFileExport, DSRBasicCodedEntry("128710", "DCM", "For Teaching File Export")));
        Codes->insert(OFMake_pair(ForClinicalTrialExport, DSRBasicCodedEntry("128711", "DCM", "For Clinical Trial Export")));
        Codes->insert(OFMake_pair(ForResearchCollectionExport, DSRBasicCodedEntry("128713", "DCM", "For Research Collection Export")));
        Codes->insert(OFMake_pair(ForPublicationExport, DSRBasicCodedEntry("128714", "DCM", "For Publication Export")));
        Codes->insert(OFMake_pair(RTVRendition, DSRBasicCodedEntry("130370", "DCM", "RTV Rendition")));
        Codes->insert(OFMake_pair(RTVAudioAndVideoRendition, DSRBasicCodedEntry("130371", "DCM", "RTV Audio and Video Rendition")));
        Codes->insert(OFMake_pair(RTVStereoVideoRendition, DSRBasicCodedEntry("130372", "DCM", "RTV Stereo Video Rendition")));
        Codes->insert(OFMake_pair(RTVAudioAndStereoVideoRendition, DSRBasicCodedEntry("130373", "DCM", "RTV Audio and Stereo Video Rendition")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID7010_KeyObjectSelectionDocumentTitle::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
