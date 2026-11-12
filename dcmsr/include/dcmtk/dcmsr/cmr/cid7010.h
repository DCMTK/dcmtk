/*
 *
 *  Copyright (C) 2015-2026, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID7010_KeyObjectSelectionDocumentTitle
 *
 *  Generated automatically from DICOM PS 3.16-2026d
 *  File created on 2026-11-12 15:49:15 by J. Riesmeier
 *
 */


#ifndef CMR_CID7010_H
#define CMR_CID7010_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"
#include "dcmtk/dcmsr/cmr/define.h"

#include "dcmtk/ofstd/ofmap.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Context Group:
 *  CID 7010 - Key Object Selection Document Title.
 *  (type: extensible, version: 20260603)
 */
class DCMTK_CMR_EXPORT CID7010_KeyObjectSelectionDocumentTitle
  : public DSRContextGroup
{

  public:

    /** types of key object selection document title
     */
    enum EnumType
    {
        /// (113000,DCM,"Of Interest")
        OfInterest,
        /// (113001,DCM,"Rejected for Quality Reasons")
        RejectedForQualityReasons,
        /// (113002,DCM,"For Referring Provider")
        ForReferringProvider,
        /// (113003,DCM,"For Surgery")
        ForSurgery,
        /// (113004,DCM,"For Teaching")
        ForTeaching,
        /// (113005,DCM,"For Conference")
        ForConference,
        /// (113006,DCM,"For Therapy")
        ForTherapy,
        /// (113007,DCM,"For Patient")
        ForPatient,
        /// (113008,DCM,"For Peer Review")
        ForPeerReview,
        /// (113009,DCM,"For Research")
        ForResearch,
        /// (113010,DCM,"Quality Issue")
        QualityIssue,
        /// (113013,DCM,"Best In Set")
        BestInSet,
        /// (113018,DCM,"For Printing")
        ForPrinting,
        /// (113020,DCM,"For Report Attachment")
        ForReportAttachment,
        /// (113021,DCM,"For Litigation")
        ForLitigation,
        /// (113030,DCM,"Manifest")
        Manifest,
        /// (113031,DCM,"Signed Manifest")
        SignedManifest,
        /// (131560,DCM,"Manifest with Description")
        ManifestWithDescription,
        /// (113032,DCM,"Complete Study Content")
        CompleteStudyContent,
        /// (113033,DCM,"Signed Complete Study Content")
        SignedCompleteStudyContent,
        /// (113034,DCM,"Complete Acquisition Content")
        CompleteAcquisitionContent,
        /// (113035,DCM,"Signed Complete Acquisition Content")
        SignedCompleteAcquisitionContent,
        /// (113036,DCM,"Group of Frames for Display")
        GroupOfFramesForDisplay,
        /// (113037,DCM,"Rejected for Patient Safety Reasons")
        RejectedForPatientSafetyReasons,
        /// (131360,DCM,"Rejection Withdrawn")
        RejectionWithdrawn,
        /// (113038,DCM,"Incorrect Modality Worklist Entry")
        IncorrectModalityWorklistEntry,
        /// (113039,DCM,"Data Retention Policy Expired")
        DataRetentionPolicyExpired,
        /// (113022,DCM,"Collection of Presentation States")
        CollectionOfPresentationStates,
        /// (128181,DCM,"Diagnostic Source Images")
        DiagnosticSourceImages,
        /// (128182,DCM,"Segmentation Result")
        SegmentationResult,
        /// (128183,DCM,"Registration Result")
        RegistrationResult,
        /// (128195,DCM,"For Diagnosis")
        ForDiagnosis,
        /// (128218,DCM,"Diagnosis Input Used")
        DiagnosisInputUsed,
        /// (128196,DCM,"For Segmentation")
        ForSegmentation,
        /// (128219,DCM,"Contouring Input Used")
        ContouringInputUsed,
        /// (128199,DCM,"For Plan Comparison")
        ForPlanComparison,
        /// (128220,DCM,"Plan Comparison Input Used")
        PlanComparisonInputUsed,
        /// (128203,DCM,"For Tumor Board")
        ForTumorBoard,
        /// (128221,DCM,"Tumor Board Input Used")
        TumorBoardInputUsed,
        /// (128208,DCM,"For Tumor Registry")
        ForTumorRegistry,
        /// (128222,DCM,"Tumor Registry Input Used")
        TumorRegistryInputUsed,
        /// (128207,DCM,"For Clinical Trial Submission")
        ForClinicalTrialSubmission,
        /// (128223,DCM,"Clinical Trial Submission Input Used")
        ClinicalTrialSubmissionInputUsed,
        /// (128184,DCM,"Pre-Planning Result"), included from CID 7023
        PrePlanningResult,
        /// (128185,DCM,"RT Prescription Result"), included from CID 7023
        RTPrescriptionResult,
        /// (128186,DCM,"Dose Calculation Image Series"), included from CID 7023
        DoseCalculationImageSeries,
        /// (128187,DCM,"Coordinate Alignment Image Series"), included from CID 7023
        CoordinateAlignmentImageSeries,
        /// (128188,DCM,"RT Treatment Simulation Result"), included from CID 7023
        RTTreatmentSimulationResult,
        /// (128189,DCM,"RT Planning Result"), included from CID 7023
        RTPlanningResult,
        /// (128190,DCM,"Dosimetric Result"), included from CID 7023
        DosimetricResult,
        /// (128191,DCM,"Patient Setup Verification Result"), included from CID 7023
        PatientSetupVerificationResult,
        /// (128192,DCM,"RT Treatment Session Result"), included from CID 7023
        RTTreatmentSessionResult,
        /// (128193,DCM,"RT Treatment Course Summary"), included from CID 7023
        RTTreatmentCourseSummary,
        /// (128194,DCM,"RT Treatment QA Result"), included from CID 7023
        RTTreatmentQAResult,
        /// (128180,DCM,"For RT Workflow"), included from CID 7024
        ForRTWorkflow,
        /// (128197,DCM,"For RT Prescription"), included from CID 7024
        ForRTPrescription,
        /// (128198,DCM,"For RT Treatment Planning"), included from CID 7024
        ForRTTreatmentPlanning,
        /// (128200,DCM,"For RT Plan Summation"), included from CID 7024
        ForRTPlanSummation,
        /// (128201,DCM,"For Physician Review"), included from CID 7024
        ForPhysicianReview,
        /// (128202,DCM,"For Physicist Review"), included from CID 7024
        ForPhysicistReview,
        /// (128204,DCM,"For Plan Quality Assurance"), included from CID 7024
        ForPlanQualityAssurance,
        /// (128205,DCM,"For Machine Quality Assurance"), included from CID 7024
        ForMachineQualityAssurance,
        /// (128206,DCM,"For Patient Setup Verification"), included from CID 7024
        ForPatientSetupVerification,
        /// (128209,DCM,"RT Workflow Input Used"), included from CID 7025
        RTWorkflowInputUsed,
        /// (128210,DCM,"RT Prescription Input Used"), included from CID 7025
        RTPrescriptionInputUsed,
        /// (128211,DCM,"RT Treatment Planning Input Used"), included from CID 7025
        RTTreatmentPlanningInputUsed,
        /// (128212,DCM,"RT Plan Summation Input Used"), included from CID 7025
        RTPlanSummationInputUsed,
        /// (128213,DCM,"Physician Review Input Used"), included from CID 7025
        PhysicianReviewInputUsed,
        /// (128214,DCM,"Physicist Review Input Used"), included from CID 7025
        PhysicistReviewInputUsed,
        /// (128215,DCM,"Plan Quality Assurance Input Used"), included from CID 7025
        PlanQualityAssuranceInputUsed,
        /// (128216,DCM,"Machine Quality Assurance Input Used"), included from CID 7025
        MachineQualityAssuranceInputUsed,
        /// (128217,DCM,"Patient Setup Verification Input Used"), included from CID 7025
        PatientSetupVerificationInputUsed,
        /// (128710,DCM,"For Teaching File Export"), included from CID 7014
        ForTeachingFileExport,
        /// (128711,DCM,"For Clinical Trial Export"), included from CID 7014
        ForClinicalTrialExport,
        /// (128713,DCM,"For Research Collection Export"), included from CID 7014
        ForResearchCollectionExport,
        /// (128714,DCM,"For Publication Export"), included from CID 7014
        ForPublicationExport,
        /// (130370,DCM,"RTV Rendition"), included from CID 7070
        RTVRendition,
        /// (130371,DCM,"RTV Audio and Video Rendition"), included from CID 7070
        RTVAudioAndVideoRendition,
        /// (130372,DCM,"RTV Stereo Video Rendition"), included from CID 7070
        RTVStereoVideoRendition,
        /// (130373,DCM,"RTV Audio and Stereo Video Rendition"), included from CID 7070
        RTVAudioAndStereoVideoRendition
    };

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID7010_KeyObjectSelectionDocumentTitle(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID7010_KeyObjectSelectionDocumentTitle(const EnumType selectedValue,
                                            const OFBool enhancedEncodingMode = OFFalse);

    /** select a coded entry given by its type as the current value
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition selectValue(const EnumType selectedValue,
                                    const OFBool enhancedEncodingMode = OFFalse);

  // --- static helper functions ---

    /** initialize this context group explicitly.  Internally, the list of standard coded
     *  entries is created and initialized by calling getCodes().
     ** @note This function can be called multiple times but in case of multi-threaded
     *        applications should be called at least once before any instance of this class
     *        is actually used.  For single-threaded applications, there is no need to call
     *        it since the initialization is done implicitly.
     */
    static void initialize();

    /** cleanup this context group, i.e.\ delete the internal list of standard coded entries.
     *  Usually, there is no need to call this method, but it might be useful in order to
     *  explicitly free the associated memory, e.g. when checking for memory leaks.  The list
     *  will be recreated automatically when needed (or when initialize() is called).
     */
    static void cleanup();

    /** map a given type to the associated coded entry
     ** @param  value                 type that should be mapped to a coded entry
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return coded entry that is associated with the given type
     */
    static DSRCodedEntryValue getCodedEntry(const EnumType value,
                                            const OFBool enhancedEncodingMode = OFFalse);

  // --- reintroduce methods from base class

     using DSRContextGroup::selectValue;
     using DSRContextGroup::findCodedEntry;


  protected:

    /// type used for storing and managing the coded entries
    typedef OFMap<EnumType, DSRBasicCodedEntry> CodeList;

    /** print details on coded entries that are contained in this context group.
     *  See DSRCodedEntryValue::print() for details of the coded entry output.
     ** @param  stream  stream to which the output should be printed
     */
    virtual void printCodes(STD_NAMESPACE ostream &stream) const;

    /** search for a given coded entry in this context group
     ** @param  searchForCodedEntry   coded entry to be searched for
     *  @param  foundCodedEntry       pointer to variable that will store the coded entry
     *                                (if found and pointer is not NULL)
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return result of the search process, also defines the type of the entry
     */
    virtual OFCondition findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
                                       DSRCodedEntryValue *foundCodedEntry,
                                       const OFBool enhancedEncodingMode = OFFalse) const;

  // --- static helper function ---

    /** get list of standard coded entries that are managed by this context group.
     *  Internally, the singleton pattern is used, so the list is initialized only once.
     *  Please note that this function is not thread-safe.  Therefore, the initialize()
     *  function can be called before any instance of this class is actually used.
     ** @return reference to list of coded entries managed by this context group
     */
    static CodeList &getCodes();

    /** set the "Enhanced Encoding Mode" for a given coded entry
     ** @param  codedEntryValue  coded entry for which the enhanced encoding mode is set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue);


  private:

    /// pointer to internal code list (use a static variable for singleton pattern)
    static CodeList *Codes;
};


/*-------------------*
 *  type definition  *
 *-------------------*/

// define short name for the context group class
typedef CID7010_KeyObjectSelectionDocumentTitle CMR_CID7010;


#endif
