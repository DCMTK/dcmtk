/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTIonPlanIOD
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTIONPL_H
#define DRTIONPL_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtadcs.h"   // for AdmittingDiagnosesCodeSequence
#include "dcmtk/dcmrt/seq/drtbrs.h"    // for BreedRegistrationSequence
#include "dcmtk/dcmrt/seq/drtcsis.h"   // for CodingSchemeIdentificationSequence
#include "dcmtk/dcmrt/seq/drtcctus.h"  // for ConsentForClinicalTrialUseSequence
#include "dcmtk/dcmrt/seq/drtcpis.h"   // for ConsultingPhysicianIdentificationSequence
#include "dcmtk/dcmrt/seq/drtces.h"    // for ContributingEquipmentSequence
#include "dcmtk/dcmrt/seq/drtcsas.h"   // for ConversionSourceAttributesSequence
#include "dcmtk/dcmrt/seq/drtdimcs.h"  // for DeidentificationMethodCodeSequence
#include "dcmtk/dcmrt/seq/drtdss.h"    // for DigitalSignaturesSequence
#include "dcmtk/dcmrt/seq/drtdrs.h"    // for DoseReferenceSequence
#include "dcmtk/dcmrt/seq/drteas.h"    // for EncryptedAttributesSequence
#include "dcmtk/dcmrt/seq/drtfgs.h"    // for FractionGroupSequence
#include "dcmtk/dcmrt/seq/drthsdrs.h"  // for HL7StructuredDocumentReferenceSequence
#include "dcmtk/dcmrt/seq/drtibs.h"    // for IonBeamSequence
#include "dcmtk/dcmrt/seq/drtitts.h"   // for IonToleranceTableSequence
#include "dcmtk/dcmrt/seq/drtians.h"   // for IssuerOfAccessionNumberSequence
#include "dcmtk/dcmrt/seq/drtiais.h"   // for IssuerOfAdmissionIDSequence
#include "dcmtk/dcmrt/seq/drtipiqs.h"  // for IssuerOfPatientIDQualifiersSequence
#include "dcmtk/dcmrt/seq/drtiseis.h"  // for IssuerOfServiceEpisodeIDSequence
#include "dcmtk/dcmrt/seq/drtmps.h"    // for MACParametersSequence
#include "dcmtk/dcmrt/seq/drtoas.h"    // for OriginalAttributesSequence
#include "dcmtk/dcmrt/seq/drtopis.h"   // for OtherPatientIDsSequence
#include "dcmtk/dcmrt/seq/drtpbcs.h"   // for PatientBreedCodeSequence
#include "dcmtk/dcmrt/seq/drtpss.h"    // for PatientSetupSequence
#include "dcmtk/dcmrt/seq/drtpsics.h"  // for PatientSizeCodeSequence
#include "dcmtk/dcmrt/seq/drtpscs.h"   // for PatientSpeciesCodeSequence
#include "dcmtk/dcmrt/seq/drtppcs.h"   // for PerformedProtocolCodeSequence
#include "dcmtk/dcmrt/seq/drtporis.h"  // for PhysiciansOfRecordIdentificationSequence
#include "dcmtk/dcmrt/seq/drtprsis.h"  // for PhysiciansReadingStudyIdentificationSequence
#include "dcmtk/dcmrt/seq/drtpdecs.h"  // for PrivateDataElementCharacteristicsSequence
#include "dcmtk/dcmrt/seq/drtpcs.h"    // for ProcedureCodeSequence
#include "dcmtk/dcmrt/seq/drtrppcs.h"  // for ReasonForPerformedProcedureCodeSequence
#include "dcmtk/dcmrt/seq/drtrds.h"    // for ReferencedDoseSequence
#include "dcmtk/dcmrt/seq/drtrpphs.h"  // for ReferencedPatientPhotoSequence
#include "dcmtk/dcmrt/seq/drtrps.h"    // for ReferencedPatientSequence
#include "dcmtk/dcmrt/seq/drtrppss.h"  // for ReferencedPerformedProcedureStepSequence
#include "dcmtk/dcmrt/seq/drtrrtps3.h" // for ReferencedRTPlanSequence
#include "dcmtk/dcmrt/seq/drtrsers.h"  // for ReferencedSeriesSequence
#include "dcmtk/dcmrt/seq/drtrsss.h"   // for ReferencedStructureSetSequence
#include "dcmtk/dcmrt/seq/drtrss.h"    // for ReferencedStudySequence
#include "dcmtk/dcmrt/seq/drtrpis.h"   // for ReferringPhysicianIdentificationSequence
#include "dcmtk/dcmrt/seq/drtras.h"    // for RequestAttributesSequence
#include "dcmtk/dcmrt/seq/drtrscs.h"   // for RequestingServiceCodeSequence
#include "dcmtk/dcmrt/seq/drtsdcs.h"   // for SeriesDescriptionCodeSequence
#include "dcmtk/dcmrt/seq/drtscris.h"  // for StudiesContainingOtherReferencedInstancesSequence


/** Interface class for RTIonPlanIOD
 */
class DCMTK_DCMRT_EXPORT DRTIonPlanIOD
  : protected DRTTypes
{

  public:

  // --- constructors, destructor and operators ---

    /** default constructor
     */
    DRTIonPlanIOD();

    /** copy constructor
     *  @param copy IOD object to be copied
     */
    DRTIonPlanIOD(const DRTIonPlanIOD &copy);

    /** destructor
     */
    virtual ~DRTIonPlanIOD();

    /** assigment operator
     *  @param copy IOD object to be copied
     */
    DRTIonPlanIOD &operator=(const DRTIonPlanIOD &copy);

  // --- general methods ---

    /** clear all internal member variables
     */
    virtual void clear();

    /** check whether the current internal state is valid
     *  @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isValid();

  // --- input/output methods ---

    /** read object from dataset
     *  @param  dataset  reference to DICOM dataset from which the object should be read
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmItem &dataset);

    /** read PatientData from dataset
     *  @param  dataset  reference to DICOM dataset from which the data should be read
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readPatientData(DcmItem &dataset);

    /** read StudyData from dataset. Also reads PatientData.
     *  @param  dataset  reference to DICOM dataset from which the data should be read
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readStudyData(DcmItem &dataset);

    /** read SeriesData from dataset. Also reads PatientData, StudyData.
     *  @param  dataset  reference to DICOM dataset from which the data should be read
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readSeriesData(DcmItem &dataset);

    /** write object to dataset
     *  @param  dataset  reference to DICOM dataset to which the object should be written
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmItem &dataset);

  // --- check presence of non-mandatory DICOM modules ---

    /** check whether ClinicalTrialSubjectModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isClinicalTrialSubjectModulePresent(const OFBool complete = OFFalse);

    /** check whether PatientStudyModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isPatientStudyModulePresent(const OFBool complete = OFFalse);

    /** check whether ClinicalTrialStudyModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isClinicalTrialStudyModulePresent(const OFBool complete = OFFalse);

    /** check whether ClinicalTrialSeriesModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isClinicalTrialSeriesModulePresent(const OFBool complete = OFFalse);

    /** check whether RTPrescriptionModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isRTPrescriptionModulePresent(const OFBool complete = OFFalse);

    /** check whether RTIonToleranceTablesModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isRTIonToleranceTablesModulePresent(const OFBool complete = OFFalse);

    /** check whether RTPatientSetupModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isRTPatientSetupModulePresent(const OFBool complete = OFFalse);

    /** check whether RTFractionSchemeModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isRTFractionSchemeModulePresent(const OFBool complete = OFFalse);

    /** check whether RTIonBeamsModule (C) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isRTIonBeamsModulePresent(const OFBool complete = OFFalse);

    /** check whether ApprovalModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isApprovalModulePresent(const OFBool complete = OFFalse);

    /** check whether CommonInstanceReferenceModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isCommonInstanceReferenceModulePresent(const OFBool complete = OFFalse);

  // --- get DICOM attribute values ---

    /** get AccessionNumber (0008,0050)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getAccessionNumber(OFString &value, const signed long pos = 0) const;

    /** get AdditionalPatientHistory (0010,21b0)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getAdditionalPatientHistory(OFString &value, const signed long pos = 0) const;

    /** get AdmissionID (0038,0010)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getAdmissionID(OFString &value, const signed long pos = 0) const;

    /** get AdmittingDiagnosesDescription (0008,1080)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getAdmittingDiagnosesDescription(OFString &value, const signed long pos = 0) const;

    /** get ApprovalStatus (300e,0002)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getApprovalStatus(OFString &value, const signed long pos = 0) const;

    /** get AuthorizationEquipmentCertificationNumber (0100,0426)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getAuthorizationEquipmentCertificationNumber(OFString &value, const signed long pos = 0) const;

    /** get ClinicalTrialCoordinatingCenterName (0012,0060)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getClinicalTrialCoordinatingCenterName(OFString &value, const signed long pos = 0) const;

    /** get ClinicalTrialProtocolEthicsCommitteeApprovalNumber (0012,0082)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getClinicalTrialProtocolEthicsCommitteeApprovalNumber(OFString &value, const signed long pos = 0) const;

    /** get ClinicalTrialProtocolEthicsCommitteeName (0012,0081)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getClinicalTrialProtocolEthicsCommitteeName(OFString &value, const signed long pos = 0) const;

    /** get ClinicalTrialProtocolID (0012,0020)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getClinicalTrialProtocolID(OFString &value, const signed long pos = 0) const;

    /** get ClinicalTrialProtocolName (0012,0021)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getClinicalTrialProtocolName(OFString &value, const signed long pos = 0) const;

    /** get ClinicalTrialSeriesDescription (0012,0072)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getClinicalTrialSeriesDescription(OFString &value, const signed long pos = 0) const;

    /** get ClinicalTrialSeriesID (0012,0071)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getClinicalTrialSeriesID(OFString &value, const signed long pos = 0) const;

    /** get ClinicalTrialSiteID (0012,0030)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getClinicalTrialSiteID(OFString &value, const signed long pos = 0) const;

    /** get ClinicalTrialSiteName (0012,0031)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getClinicalTrialSiteName(OFString &value, const signed long pos = 0) const;

    /** get ClinicalTrialSponsorName (0012,0010)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getClinicalTrialSponsorName(OFString &value, const signed long pos = 0) const;

    /** get ClinicalTrialSubjectID (0012,0040)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getClinicalTrialSubjectID(OFString &value, const signed long pos = 0) const;

    /** get ClinicalTrialSubjectReadingID (0012,0042)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getClinicalTrialSubjectReadingID(OFString &value, const signed long pos = 0) const;

    /** get ClinicalTrialTimePointDescription (0012,0051)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getClinicalTrialTimePointDescription(OFString &value, const signed long pos = 0) const;

    /** get ClinicalTrialTimePointID (0012,0050)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getClinicalTrialTimePointID(OFString &value, const signed long pos = 0) const;

    /** get CommentsOnThePerformedProcedureStep (0040,0280)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getCommentsOnThePerformedProcedureStep(OFString &value, const signed long pos = 0) const;

    /** get ConsultingPhysicianName (0008,009c)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getConsultingPhysicianName(OFString &value, const signed long pos = 0) const;

    /** get ContentQualification (0018,9004)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContentQualification(OFString &value, const signed long pos = 0) const;

    /** get DateOfLastCalibration (0018,1200)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDateOfLastCalibration(OFString &value, const signed long pos = 0) const;

    /** get DeidentificationMethod (0012,0063)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDeidentificationMethod(OFString &value, const signed long pos = 0) const;

    /** get DeviceSerialNumber (0018,1000)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDeviceSerialNumber(OFString &value, const signed long pos = 0) const;

    /** get EthnicGroup (0010,2160)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getEthnicGroup(OFString &value, const signed long pos = 0) const;

    /** get FrameOfReferenceUID (0020,0052)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFrameOfReferenceUID(OFString &value, const signed long pos = 0) const;

    /** get GantryID (0018,1008)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getGantryID(OFString &value, const signed long pos = 0) const;

    /** get InstanceCoercionDateTime (0008,0015)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstanceCoercionDateTime(OFString &value, const signed long pos = 0) const;

    /** get InstanceCreationDate (0008,0012)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstanceCreationDate(OFString &value, const signed long pos = 0) const;

    /** get InstanceCreationTime (0008,0013)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstanceCreationTime(OFString &value, const signed long pos = 0) const;

    /** get InstanceCreatorUID (0008,0014)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstanceCreatorUID(OFString &value, const signed long pos = 0) const;

    /** get InstanceNumber (0020,0013)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstanceNumber(OFString &value, const signed long pos = 0) const;

    /** get InstanceNumber (0020,0013)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstanceNumber(Sint32 &value, const unsigned long pos = 0) const;

    /** get InstitutionAddress (0008,0081)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstitutionAddress(OFString &value, const signed long pos = 0) const;

    /** get InstitutionName (0008,0080)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstitutionName(OFString &value, const signed long pos = 0) const;

    /** get InstitutionalDepartmentName (0008,1040)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstitutionalDepartmentName(OFString &value, const signed long pos = 0) const;

    /** get IssuerOfPatientID (0010,0021)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getIssuerOfPatientID(OFString &value, const signed long pos = 0) const;

    /** get LongitudinalTemporalInformationModified (0028,0303)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getLongitudinalTemporalInformationModified(OFString &value, const signed long pos = 0) const;

    /** get Manufacturer (0008,0070)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getManufacturer(OFString &value, const signed long pos = 0) const;

    /** get ManufacturerModelName (0008,1090)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getManufacturerModelName(OFString &value, const signed long pos = 0) const;

    /** get Modality (0008,0060)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getModality(OFString &value, const signed long pos = 0) const;

    /** get NameOfPhysiciansReadingStudy (0008,1060)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getNameOfPhysiciansReadingStudy(OFString &value, const signed long pos = 0) const;

    /** get Occupation (0010,2180)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOccupation(OFString &value, const signed long pos = 0) const;

    /** get OperatorsName (0008,1070)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOperatorsName(OFString &value, const signed long pos = 0) const;

    /** get OriginalSpecializedSOPClassUID (0008,001b)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOriginalSpecializedSOPClassUID(OFString &value, const signed long pos = 0) const;

    /** get OtherPatientIDs (0010,1000)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOtherPatientIDs(OFString &value, const signed long pos = 0) const;

    /** get OtherPatientNames (0010,1001)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOtherPatientNames(OFString &value, const signed long pos = 0) const;

    /** get PatientAge (0010,1010)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientAge(OFString &value, const signed long pos = 0) const;

    /** get PatientBirthDate (0010,0030)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientBirthDate(OFString &value, const signed long pos = 0) const;

    /** get PatientBirthTime (0010,0032)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientBirthTime(OFString &value, const signed long pos = 0) const;

    /** get PatientBreedDescription (0010,2292)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientBreedDescription(OFString &value, const signed long pos = 0) const;

    /** get PatientComments (0010,4000)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientComments(OFString &value, const signed long pos = 0) const;

    /** get PatientID (0010,0020)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientID(OFString &value, const signed long pos = 0) const;

    /** get PatientIdentityRemoved (0012,0062)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientIdentityRemoved(OFString &value, const signed long pos = 0) const;

    /** get PatientName (0010,0010)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientName(OFString &value, const signed long pos = 0) const;

    /** get PatientSex (0010,0040)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientSex(OFString &value, const signed long pos = 0) const;

    /** get PatientSexNeutered (0010,2203)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientSexNeutered(OFString &value, const signed long pos = 0) const;

    /** get PatientSize (0010,1020)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientSize(OFString &value, const signed long pos = 0) const;

    /** get PatientSize (0010,1020)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientSize(Float64 &value, const unsigned long pos = 0) const;

    /** get PatientSpeciesDescription (0010,2201)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientSpeciesDescription(OFString &value, const signed long pos = 0) const;

    /** get PatientWeight (0010,1030)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientWeight(OFString &value, const signed long pos = 0) const;

    /** get PatientWeight (0010,1030)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientWeight(Float64 &value, const unsigned long pos = 0) const;

    /** get PerformedProcedureStepDescription (0040,0254)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPerformedProcedureStepDescription(OFString &value, const signed long pos = 0) const;

    /** get PerformedProcedureStepEndDate (0040,0250)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPerformedProcedureStepEndDate(OFString &value, const signed long pos = 0) const;

    /** get PerformedProcedureStepEndTime (0040,0251)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPerformedProcedureStepEndTime(OFString &value, const signed long pos = 0) const;

    /** get PerformedProcedureStepID (0040,0253)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPerformedProcedureStepID(OFString &value, const signed long pos = 0) const;

    /** get PerformedProcedureStepStartDate (0040,0244)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPerformedProcedureStepStartDate(OFString &value, const signed long pos = 0) const;

    /** get PerformedProcedureStepStartTime (0040,0245)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPerformedProcedureStepStartTime(OFString &value, const signed long pos = 0) const;

    /** get PhysiciansOfRecord (0008,1048)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPhysiciansOfRecord(OFString &value, const signed long pos = 0) const;

    /** get PixelPaddingValue (0028,0120)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelPaddingValue(Uint16 &value, const unsigned long pos = 0) const;

    /** get PlanIntent (300a,000a)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPlanIntent(OFString &value, const signed long pos = 0) const;

    /** get PositionReferenceIndicator (0020,1040)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPositionReferenceIndicator(OFString &value, const signed long pos = 0) const;

    /** get PrescriptionDescription (300a,000e)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPrescriptionDescription(OFString &value, const signed long pos = 0) const;

    /** get QualityControlSubject (0010,0200)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getQualityControlSubject(OFString &value, const signed long pos = 0) const;

    /** get QueryRetrieveView (0008,0053)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getQueryRetrieveView(OFString &value, const signed long pos = 0) const;

    /** get RTPlanDate (300a,0006)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTPlanDate(OFString &value, const signed long pos = 0) const;

    /** get RTPlanDescription (300a,0004)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTPlanDescription(OFString &value, const signed long pos = 0) const;

    /** get RTPlanGeometry (300a,000c)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTPlanGeometry(OFString &value, const signed long pos = 0) const;

    /** get RTPlanLabel (300a,0002)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTPlanLabel(OFString &value, const signed long pos = 0) const;

    /** get RTPlanName (300a,0003)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTPlanName(OFString &value, const signed long pos = 0) const;

    /** get RTPlanTime (300a,0007)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTPlanTime(OFString &value, const signed long pos = 0) const;

    /** get ReferringPhysicianName (0008,0090)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getReferringPhysicianName(OFString &value, const signed long pos = 0) const;

    /** get RelatedGeneralSOPClassUID (0008,001a)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRelatedGeneralSOPClassUID(OFString &value, const signed long pos = 0) const;

    /** get ResponsibleOrganization (0010,2299)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getResponsibleOrganization(OFString &value, const signed long pos = 0) const;

    /** get ResponsiblePerson (0010,2297)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getResponsiblePerson(OFString &value, const signed long pos = 0) const;

    /** get ResponsiblePersonRole (0010,2298)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getResponsiblePersonRole(OFString &value, const signed long pos = 0) const;

    /** get ReviewDate (300e,0004)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getReviewDate(OFString &value, const signed long pos = 0) const;

    /** get ReviewTime (300e,0005)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getReviewTime(OFString &value, const signed long pos = 0) const;

    /** get ReviewerName (300e,0008)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getReviewerName(OFString &value, const signed long pos = 0) const;

    /** get SOPAuthorizationComment (0100,0424)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSOPAuthorizationComment(OFString &value, const signed long pos = 0) const;

    /** get SOPAuthorizationDateTime (0100,0420)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSOPAuthorizationDateTime(OFString &value, const signed long pos = 0) const;

    /** get SOPClassUID (0008,0016)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSOPClassUID(OFString &value, const signed long pos = 0) const;

    /** get SOPInstanceStatus (0100,0410)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSOPInstanceStatus(OFString &value, const signed long pos = 0) const;

    /** get SOPInstanceUID (0008,0018)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSOPInstanceUID(OFString &value, const signed long pos = 0) const;

    /** get SeriesDate (0008,0021)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSeriesDate(OFString &value, const signed long pos = 0) const;

    /** get SeriesDescription (0008,103e)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSeriesDescription(OFString &value, const signed long pos = 0) const;

    /** get SeriesInstanceUID (0020,000e)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSeriesInstanceUID(OFString &value, const signed long pos = 0) const;

    /** get SeriesNumber (0020,0011)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSeriesNumber(OFString &value, const signed long pos = 0) const;

    /** get SeriesNumber (0020,0011)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSeriesNumber(Sint32 &value, const unsigned long pos = 0) const;

    /** get SeriesTime (0008,0031)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSeriesTime(OFString &value, const signed long pos = 0) const;

    /** get ServiceEpisodeDescription (0038,0062)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getServiceEpisodeDescription(OFString &value, const signed long pos = 0) const;

    /** get ServiceEpisodeID (0038,0060)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getServiceEpisodeID(OFString &value, const signed long pos = 0) const;

    /** get SoftwareVersions (0018,1020)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSoftwareVersions(OFString &value, const signed long pos = 0) const;

    /** get SpatialResolution (0018,1050)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSpatialResolution(OFString &value, const signed long pos = 0) const;

    /** get SpatialResolution (0018,1050)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSpatialResolution(Float64 &value, const unsigned long pos = 0) const;

    /** get SpecificCharacterSet (0008,0005)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSpecificCharacterSet(OFString &value, const signed long pos = 0) const;

    /** get StationName (0008,1010)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStationName(OFString &value, const signed long pos = 0) const;

    /** get StudyDate (0008,0020)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStudyDate(OFString &value, const signed long pos = 0) const;

    /** get StudyDescription (0008,1030)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStudyDescription(OFString &value, const signed long pos = 0) const;

    /** get StudyID (0020,0010)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStudyID(OFString &value, const signed long pos = 0) const;

    /** get StudyInstanceUID (0020,000d)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStudyInstanceUID(OFString &value, const signed long pos = 0) const;

    /** get StudyTime (0008,0030)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStudyTime(OFString &value, const signed long pos = 0) const;

    /** get TimeOfLastCalibration (0018,1201)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTimeOfLastCalibration(OFString &value, const signed long pos = 0) const;

    /** get TimezoneOffsetFromUTC (0008,0201)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTimezoneOffsetFromUTC(OFString &value, const signed long pos = 0) const;

    /** get TreatmentProtocols (300a,0009)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTreatmentProtocols(OFString &value, const signed long pos = 0) const;

    /** get TreatmentSites (300a,000b)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTreatmentSites(OFString &value, const signed long pos = 0) const;

  // --- get DICOM sequence attributes ---

    /** get AdmittingDiagnosesCodeSequence (0008,1084)
     *  @return reference to sequence element
     */
    DRTAdmittingDiagnosesCodeSequence &getAdmittingDiagnosesCodeSequence()
        { return AdmittingDiagnosesCodeSequence; }

    /** get AdmittingDiagnosesCodeSequence (0008,1084)
     *  @return const reference to sequence element
     */
    const DRTAdmittingDiagnosesCodeSequence &getAdmittingDiagnosesCodeSequence() const
        { return AdmittingDiagnosesCodeSequence; }

    /** get BreedRegistrationSequence (0010,2294)
     *  @return reference to sequence element
     */
    DRTBreedRegistrationSequence &getBreedRegistrationSequence()
        { return BreedRegistrationSequence; }

    /** get BreedRegistrationSequence (0010,2294)
     *  @return const reference to sequence element
     */
    const DRTBreedRegistrationSequence &getBreedRegistrationSequence() const
        { return BreedRegistrationSequence; }

    /** get CodingSchemeIdentificationSequence (0008,0110)
     *  @return reference to sequence element
     */
    DRTCodingSchemeIdentificationSequence &getCodingSchemeIdentificationSequence()
        { return CodingSchemeIdentificationSequence; }

    /** get CodingSchemeIdentificationSequence (0008,0110)
     *  @return const reference to sequence element
     */
    const DRTCodingSchemeIdentificationSequence &getCodingSchemeIdentificationSequence() const
        { return CodingSchemeIdentificationSequence; }

    /** get ConsentForClinicalTrialUseSequence (0012,0083)
     *  @return reference to sequence element
     */
    DRTConsentForClinicalTrialUseSequence &getConsentForClinicalTrialUseSequence()
        { return ConsentForClinicalTrialUseSequence; }

    /** get ConsentForClinicalTrialUseSequence (0012,0083)
     *  @return const reference to sequence element
     */
    const DRTConsentForClinicalTrialUseSequence &getConsentForClinicalTrialUseSequence() const
        { return ConsentForClinicalTrialUseSequence; }

    /** get ConsultingPhysicianIdentificationSequence (0008,009d)
     *  @return reference to sequence element
     */
    DRTConsultingPhysicianIdentificationSequence &getConsultingPhysicianIdentificationSequence()
        { return ConsultingPhysicianIdentificationSequence; }

    /** get ConsultingPhysicianIdentificationSequence (0008,009d)
     *  @return const reference to sequence element
     */
    const DRTConsultingPhysicianIdentificationSequence &getConsultingPhysicianIdentificationSequence() const
        { return ConsultingPhysicianIdentificationSequence; }

    /** get ContributingEquipmentSequence (0018,a001)
     *  @return reference to sequence element
     */
    DRTContributingEquipmentSequence &getContributingEquipmentSequence()
        { return ContributingEquipmentSequence; }

    /** get ContributingEquipmentSequence (0018,a001)
     *  @return const reference to sequence element
     */
    const DRTContributingEquipmentSequence &getContributingEquipmentSequence() const
        { return ContributingEquipmentSequence; }

    /** get ConversionSourceAttributesSequence (0020,9172)
     *  @return reference to sequence element
     */
    DRTConversionSourceAttributesSequence &getConversionSourceAttributesSequence()
        { return ConversionSourceAttributesSequence; }

    /** get ConversionSourceAttributesSequence (0020,9172)
     *  @return const reference to sequence element
     */
    const DRTConversionSourceAttributesSequence &getConversionSourceAttributesSequence() const
        { return ConversionSourceAttributesSequence; }

    /** get DeidentificationMethodCodeSequence (0012,0064)
     *  @return reference to sequence element
     */
    DRTDeidentificationMethodCodeSequence &getDeidentificationMethodCodeSequence()
        { return DeidentificationMethodCodeSequence; }

    /** get DeidentificationMethodCodeSequence (0012,0064)
     *  @return const reference to sequence element
     */
    const DRTDeidentificationMethodCodeSequence &getDeidentificationMethodCodeSequence() const
        { return DeidentificationMethodCodeSequence; }

    /** get DigitalSignaturesSequence (fffa,fffa)
     *  @return reference to sequence element
     */
    DRTDigitalSignaturesSequence &getDigitalSignaturesSequence()
        { return DigitalSignaturesSequence; }

    /** get DigitalSignaturesSequence (fffa,fffa)
     *  @return const reference to sequence element
     */
    const DRTDigitalSignaturesSequence &getDigitalSignaturesSequence() const
        { return DigitalSignaturesSequence; }

    /** get DoseReferenceSequence (300a,0010)
     *  @return reference to sequence element
     */
    DRTDoseReferenceSequence &getDoseReferenceSequence()
        { return DoseReferenceSequence; }

    /** get DoseReferenceSequence (300a,0010)
     *  @return const reference to sequence element
     */
    const DRTDoseReferenceSequence &getDoseReferenceSequence() const
        { return DoseReferenceSequence; }

    /** get EncryptedAttributesSequence (0400,0500)
     *  @return reference to sequence element
     */
    DRTEncryptedAttributesSequence &getEncryptedAttributesSequence()
        { return EncryptedAttributesSequence; }

    /** get EncryptedAttributesSequence (0400,0500)
     *  @return const reference to sequence element
     */
    const DRTEncryptedAttributesSequence &getEncryptedAttributesSequence() const
        { return EncryptedAttributesSequence; }

    /** get FractionGroupSequence (300a,0070)
     *  @return reference to sequence element
     */
    DRTFractionGroupSequence &getFractionGroupSequence()
        { return FractionGroupSequence; }

    /** get FractionGroupSequence (300a,0070)
     *  @return const reference to sequence element
     */
    const DRTFractionGroupSequence &getFractionGroupSequence() const
        { return FractionGroupSequence; }

    /** get HL7StructuredDocumentReferenceSequence (0040,a390)
     *  @return reference to sequence element
     */
    DRTHL7StructuredDocumentReferenceSequence &getHL7StructuredDocumentReferenceSequence()
        { return HL7StructuredDocumentReferenceSequence; }

    /** get HL7StructuredDocumentReferenceSequence (0040,a390)
     *  @return const reference to sequence element
     */
    const DRTHL7StructuredDocumentReferenceSequence &getHL7StructuredDocumentReferenceSequence() const
        { return HL7StructuredDocumentReferenceSequence; }

    /** get IonBeamSequence (300a,03a2)
     *  @return reference to sequence element
     */
    DRTIonBeamSequence &getIonBeamSequence()
        { return IonBeamSequence; }

    /** get IonBeamSequence (300a,03a2)
     *  @return const reference to sequence element
     */
    const DRTIonBeamSequence &getIonBeamSequence() const
        { return IonBeamSequence; }

    /** get IonToleranceTableSequence (300a,03a0)
     *  @return reference to sequence element
     */
    DRTIonToleranceTableSequence &getIonToleranceTableSequence()
        { return IonToleranceTableSequence; }

    /** get IonToleranceTableSequence (300a,03a0)
     *  @return const reference to sequence element
     */
    const DRTIonToleranceTableSequence &getIonToleranceTableSequence() const
        { return IonToleranceTableSequence; }

    /** get IssuerOfAccessionNumberSequence (0008,0051)
     *  @return reference to sequence element
     */
    DRTIssuerOfAccessionNumberSequence &getIssuerOfAccessionNumberSequence()
        { return IssuerOfAccessionNumberSequence; }

    /** get IssuerOfAccessionNumberSequence (0008,0051)
     *  @return const reference to sequence element
     */
    const DRTIssuerOfAccessionNumberSequence &getIssuerOfAccessionNumberSequence() const
        { return IssuerOfAccessionNumberSequence; }

    /** get IssuerOfAdmissionIDSequence (0038,0014)
     *  @return reference to sequence element
     */
    DRTIssuerOfAdmissionIDSequence &getIssuerOfAdmissionIDSequence()
        { return IssuerOfAdmissionIDSequence; }

    /** get IssuerOfAdmissionIDSequence (0038,0014)
     *  @return const reference to sequence element
     */
    const DRTIssuerOfAdmissionIDSequence &getIssuerOfAdmissionIDSequence() const
        { return IssuerOfAdmissionIDSequence; }

    /** get IssuerOfPatientIDQualifiersSequence (0010,0024)
     *  @return reference to sequence element
     */
    DRTIssuerOfPatientIDQualifiersSequence &getIssuerOfPatientIDQualifiersSequence()
        { return IssuerOfPatientIDQualifiersSequence; }

    /** get IssuerOfPatientIDQualifiersSequence (0010,0024)
     *  @return const reference to sequence element
     */
    const DRTIssuerOfPatientIDQualifiersSequence &getIssuerOfPatientIDQualifiersSequence() const
        { return IssuerOfPatientIDQualifiersSequence; }

    /** get IssuerOfServiceEpisodeIDSequence (0038,0064)
     *  @return reference to sequence element
     */
    DRTIssuerOfServiceEpisodeIDSequence &getIssuerOfServiceEpisodeIDSequence()
        { return IssuerOfServiceEpisodeIDSequence; }

    /** get IssuerOfServiceEpisodeIDSequence (0038,0064)
     *  @return const reference to sequence element
     */
    const DRTIssuerOfServiceEpisodeIDSequence &getIssuerOfServiceEpisodeIDSequence() const
        { return IssuerOfServiceEpisodeIDSequence; }

    /** get MACParametersSequence (4ffe,0001)
     *  @return reference to sequence element
     */
    DRTMACParametersSequence &getMACParametersSequence()
        { return MACParametersSequence; }

    /** get MACParametersSequence (4ffe,0001)
     *  @return const reference to sequence element
     */
    const DRTMACParametersSequence &getMACParametersSequence() const
        { return MACParametersSequence; }

    /** get OriginalAttributesSequence (0400,0561)
     *  @return reference to sequence element
     */
    DRTOriginalAttributesSequence &getOriginalAttributesSequence()
        { return OriginalAttributesSequence; }

    /** get OriginalAttributesSequence (0400,0561)
     *  @return const reference to sequence element
     */
    const DRTOriginalAttributesSequence &getOriginalAttributesSequence() const
        { return OriginalAttributesSequence; }

    /** get OtherPatientIDsSequence (0010,1002)
     *  @return reference to sequence element
     */
    DRTOtherPatientIDsSequence &getOtherPatientIDsSequence()
        { return OtherPatientIDsSequence; }

    /** get OtherPatientIDsSequence (0010,1002)
     *  @return const reference to sequence element
     */
    const DRTOtherPatientIDsSequence &getOtherPatientIDsSequence() const
        { return OtherPatientIDsSequence; }

    /** get PatientBreedCodeSequence (0010,2293)
     *  @return reference to sequence element
     */
    DRTPatientBreedCodeSequence &getPatientBreedCodeSequence()
        { return PatientBreedCodeSequence; }

    /** get PatientBreedCodeSequence (0010,2293)
     *  @return const reference to sequence element
     */
    const DRTPatientBreedCodeSequence &getPatientBreedCodeSequence() const
        { return PatientBreedCodeSequence; }

    /** get PatientSetupSequence (300a,0180)
     *  @return reference to sequence element
     */
    DRTPatientSetupSequence &getPatientSetupSequence()
        { return PatientSetupSequence; }

    /** get PatientSetupSequence (300a,0180)
     *  @return const reference to sequence element
     */
    const DRTPatientSetupSequence &getPatientSetupSequence() const
        { return PatientSetupSequence; }

    /** get PatientSizeCodeSequence (0010,1021)
     *  @return reference to sequence element
     */
    DRTPatientSizeCodeSequence &getPatientSizeCodeSequence()
        { return PatientSizeCodeSequence; }

    /** get PatientSizeCodeSequence (0010,1021)
     *  @return const reference to sequence element
     */
    const DRTPatientSizeCodeSequence &getPatientSizeCodeSequence() const
        { return PatientSizeCodeSequence; }

    /** get PatientSpeciesCodeSequence (0010,2202)
     *  @return reference to sequence element
     */
    DRTPatientSpeciesCodeSequence &getPatientSpeciesCodeSequence()
        { return PatientSpeciesCodeSequence; }

    /** get PatientSpeciesCodeSequence (0010,2202)
     *  @return const reference to sequence element
     */
    const DRTPatientSpeciesCodeSequence &getPatientSpeciesCodeSequence() const
        { return PatientSpeciesCodeSequence; }

    /** get PerformedProtocolCodeSequence (0040,0260)
     *  @return reference to sequence element
     */
    DRTPerformedProtocolCodeSequence &getPerformedProtocolCodeSequence()
        { return PerformedProtocolCodeSequence; }

    /** get PerformedProtocolCodeSequence (0040,0260)
     *  @return const reference to sequence element
     */
    const DRTPerformedProtocolCodeSequence &getPerformedProtocolCodeSequence() const
        { return PerformedProtocolCodeSequence; }

    /** get PhysiciansOfRecordIdentificationSequence (0008,1049)
     *  @return reference to sequence element
     */
    DRTPhysiciansOfRecordIdentificationSequence &getPhysiciansOfRecordIdentificationSequence()
        { return PhysiciansOfRecordIdentificationSequence; }

    /** get PhysiciansOfRecordIdentificationSequence (0008,1049)
     *  @return const reference to sequence element
     */
    const DRTPhysiciansOfRecordIdentificationSequence &getPhysiciansOfRecordIdentificationSequence() const
        { return PhysiciansOfRecordIdentificationSequence; }

    /** get PhysiciansReadingStudyIdentificationSequence (0008,1062)
     *  @return reference to sequence element
     */
    DRTPhysiciansReadingStudyIdentificationSequence &getPhysiciansReadingStudyIdentificationSequence()
        { return PhysiciansReadingStudyIdentificationSequence; }

    /** get PhysiciansReadingStudyIdentificationSequence (0008,1062)
     *  @return const reference to sequence element
     */
    const DRTPhysiciansReadingStudyIdentificationSequence &getPhysiciansReadingStudyIdentificationSequence() const
        { return PhysiciansReadingStudyIdentificationSequence; }

    /** get PrivateDataElementCharacteristicsSequence (0008,0300)
     *  @return reference to sequence element
     */
    DRTPrivateDataElementCharacteristicsSequence &getPrivateDataElementCharacteristicsSequence()
        { return PrivateDataElementCharacteristicsSequence; }

    /** get PrivateDataElementCharacteristicsSequence (0008,0300)
     *  @return const reference to sequence element
     */
    const DRTPrivateDataElementCharacteristicsSequence &getPrivateDataElementCharacteristicsSequence() const
        { return PrivateDataElementCharacteristicsSequence; }

    /** get ProcedureCodeSequence (0008,1032)
     *  @return reference to sequence element
     */
    DRTProcedureCodeSequence &getProcedureCodeSequence()
        { return ProcedureCodeSequence; }

    /** get ProcedureCodeSequence (0008,1032)
     *  @return const reference to sequence element
     */
    const DRTProcedureCodeSequence &getProcedureCodeSequence() const
        { return ProcedureCodeSequence; }

    /** get ReasonForPerformedProcedureCodeSequence (0040,1012)
     *  @return reference to sequence element
     */
    DRTReasonForPerformedProcedureCodeSequence &getReasonForPerformedProcedureCodeSequence()
        { return ReasonForPerformedProcedureCodeSequence; }

    /** get ReasonForPerformedProcedureCodeSequence (0040,1012)
     *  @return const reference to sequence element
     */
    const DRTReasonForPerformedProcedureCodeSequence &getReasonForPerformedProcedureCodeSequence() const
        { return ReasonForPerformedProcedureCodeSequence; }

    /** get ReferencedDoseSequence (300c,0080)
     *  @return reference to sequence element
     */
    DRTReferencedDoseSequence &getReferencedDoseSequence()
        { return ReferencedDoseSequence; }

    /** get ReferencedDoseSequence (300c,0080)
     *  @return const reference to sequence element
     */
    const DRTReferencedDoseSequence &getReferencedDoseSequence() const
        { return ReferencedDoseSequence; }

    /** get ReferencedPatientPhotoSequence (0010,1100)
     *  @return reference to sequence element
     */
    DRTReferencedPatientPhotoSequence &getReferencedPatientPhotoSequence()
        { return ReferencedPatientPhotoSequence; }

    /** get ReferencedPatientPhotoSequence (0010,1100)
     *  @return const reference to sequence element
     */
    const DRTReferencedPatientPhotoSequence &getReferencedPatientPhotoSequence() const
        { return ReferencedPatientPhotoSequence; }

    /** get ReferencedPatientSequence (0008,1120)
     *  @return reference to sequence element
     */
    DRTReferencedPatientSequence &getReferencedPatientSequence()
        { return ReferencedPatientSequence; }

    /** get ReferencedPatientSequence (0008,1120)
     *  @return const reference to sequence element
     */
    const DRTReferencedPatientSequence &getReferencedPatientSequence() const
        { return ReferencedPatientSequence; }

    /** get ReferencedPerformedProcedureStepSequence (0008,1111)
     *  @return reference to sequence element
     */
    DRTReferencedPerformedProcedureStepSequence &getReferencedPerformedProcedureStepSequence()
        { return ReferencedPerformedProcedureStepSequence; }

    /** get ReferencedPerformedProcedureStepSequence (0008,1111)
     *  @return const reference to sequence element
     */
    const DRTReferencedPerformedProcedureStepSequence &getReferencedPerformedProcedureStepSequence() const
        { return ReferencedPerformedProcedureStepSequence; }

    /** get ReferencedRTPlanSequence (300c,0002)
     *  @return reference to sequence element
     */
    DRTReferencedRTPlanSequenceInRTGeneralPlanModule &getReferencedRTPlanSequence()
        { return ReferencedRTPlanSequence; }

    /** get ReferencedRTPlanSequence (300c,0002)
     *  @return const reference to sequence element
     */
    const DRTReferencedRTPlanSequenceInRTGeneralPlanModule &getReferencedRTPlanSequence() const
        { return ReferencedRTPlanSequence; }

    /** get ReferencedSeriesSequence (0008,1115)
     *  @return reference to sequence element
     */
    DRTReferencedSeriesSequence &getReferencedSeriesSequence()
        { return ReferencedSeriesSequence; }

    /** get ReferencedSeriesSequence (0008,1115)
     *  @return const reference to sequence element
     */
    const DRTReferencedSeriesSequence &getReferencedSeriesSequence() const
        { return ReferencedSeriesSequence; }

    /** get ReferencedStructureSetSequence (300c,0060)
     *  @return reference to sequence element
     */
    DRTReferencedStructureSetSequence &getReferencedStructureSetSequence()
        { return ReferencedStructureSetSequence; }

    /** get ReferencedStructureSetSequence (300c,0060)
     *  @return const reference to sequence element
     */
    const DRTReferencedStructureSetSequence &getReferencedStructureSetSequence() const
        { return ReferencedStructureSetSequence; }

    /** get ReferencedStudySequence (0008,1110)
     *  @return reference to sequence element
     */
    DRTReferencedStudySequence &getReferencedStudySequence()
        { return ReferencedStudySequence; }

    /** get ReferencedStudySequence (0008,1110)
     *  @return const reference to sequence element
     */
    const DRTReferencedStudySequence &getReferencedStudySequence() const
        { return ReferencedStudySequence; }

    /** get ReferringPhysicianIdentificationSequence (0008,0096)
     *  @return reference to sequence element
     */
    DRTReferringPhysicianIdentificationSequence &getReferringPhysicianIdentificationSequence()
        { return ReferringPhysicianIdentificationSequence; }

    /** get ReferringPhysicianIdentificationSequence (0008,0096)
     *  @return const reference to sequence element
     */
    const DRTReferringPhysicianIdentificationSequence &getReferringPhysicianIdentificationSequence() const
        { return ReferringPhysicianIdentificationSequence; }

    /** get RequestAttributesSequence (0040,0275)
     *  @return reference to sequence element
     */
    DRTRequestAttributesSequence &getRequestAttributesSequence()
        { return RequestAttributesSequence; }

    /** get RequestAttributesSequence (0040,0275)
     *  @return const reference to sequence element
     */
    const DRTRequestAttributesSequence &getRequestAttributesSequence() const
        { return RequestAttributesSequence; }

    /** get RequestingServiceCodeSequence (0032,1034)
     *  @return reference to sequence element
     */
    DRTRequestingServiceCodeSequence &getRequestingServiceCodeSequence()
        { return RequestingServiceCodeSequence; }

    /** get RequestingServiceCodeSequence (0032,1034)
     *  @return const reference to sequence element
     */
    const DRTRequestingServiceCodeSequence &getRequestingServiceCodeSequence() const
        { return RequestingServiceCodeSequence; }

    /** get SeriesDescriptionCodeSequence (0008,103f)
     *  @return reference to sequence element
     */
    DRTSeriesDescriptionCodeSequence &getSeriesDescriptionCodeSequence()
        { return SeriesDescriptionCodeSequence; }

    /** get SeriesDescriptionCodeSequence (0008,103f)
     *  @return const reference to sequence element
     */
    const DRTSeriesDescriptionCodeSequence &getSeriesDescriptionCodeSequence() const
        { return SeriesDescriptionCodeSequence; }

    /** get StudiesContainingOtherReferencedInstancesSequence (0008,1200)
     *  @return reference to sequence element
     */
    DRTStudiesContainingOtherReferencedInstancesSequence &getStudiesContainingOtherReferencedInstancesSequence()
        { return StudiesContainingOtherReferencedInstancesSequence; }

    /** get StudiesContainingOtherReferencedInstancesSequence (0008,1200)
     *  @return const reference to sequence element
     */
    const DRTStudiesContainingOtherReferencedInstancesSequence &getStudiesContainingOtherReferencedInstancesSequence() const
        { return StudiesContainingOtherReferencedInstancesSequence; }

  // --- set DICOM attribute values ---

    /** set AccessionNumber (0008,0050)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setAccessionNumber(const OFString &value, const OFBool check = OFTrue);

    /** set AdditionalPatientHistory (0010,21b0)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LT) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setAdditionalPatientHistory(const OFString &value, const OFBool check = OFTrue);

    /** set AdmissionID (0038,0010)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setAdmissionID(const OFString &value, const OFBool check = OFTrue);

    /** set AdmittingDiagnosesDescription (0008,1080)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setAdmittingDiagnosesDescription(const OFString &value, const OFBool check = OFTrue);

    /** set ApprovalStatus (300e,0002)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setApprovalStatus(const OFString &value, const OFBool check = OFTrue);

    /** set AuthorizationEquipmentCertificationNumber (0100,0426)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setAuthorizationEquipmentCertificationNumber(const OFString &value, const OFBool check = OFTrue);

    /** set ClinicalTrialCoordinatingCenterName (0012,0060)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setClinicalTrialCoordinatingCenterName(const OFString &value, const OFBool check = OFTrue);

    /** set ClinicalTrialProtocolEthicsCommitteeApprovalNumber (0012,0082)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setClinicalTrialProtocolEthicsCommitteeApprovalNumber(const OFString &value, const OFBool check = OFTrue);

    /** set ClinicalTrialProtocolEthicsCommitteeName (0012,0081)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setClinicalTrialProtocolEthicsCommitteeName(const OFString &value, const OFBool check = OFTrue);

    /** set ClinicalTrialProtocolID (0012,0020)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setClinicalTrialProtocolID(const OFString &value, const OFBool check = OFTrue);

    /** set ClinicalTrialProtocolName (0012,0021)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setClinicalTrialProtocolName(const OFString &value, const OFBool check = OFTrue);

    /** set ClinicalTrialSeriesDescription (0012,0072)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setClinicalTrialSeriesDescription(const OFString &value, const OFBool check = OFTrue);

    /** set ClinicalTrialSeriesID (0012,0071)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setClinicalTrialSeriesID(const OFString &value, const OFBool check = OFTrue);

    /** set ClinicalTrialSiteID (0012,0030)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setClinicalTrialSiteID(const OFString &value, const OFBool check = OFTrue);

    /** set ClinicalTrialSiteName (0012,0031)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setClinicalTrialSiteName(const OFString &value, const OFBool check = OFTrue);

    /** set ClinicalTrialSponsorName (0012,0010)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setClinicalTrialSponsorName(const OFString &value, const OFBool check = OFTrue);

    /** set ClinicalTrialSubjectID (0012,0040)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setClinicalTrialSubjectID(const OFString &value, const OFBool check = OFTrue);

    /** set ClinicalTrialSubjectReadingID (0012,0042)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setClinicalTrialSubjectReadingID(const OFString &value, const OFBool check = OFTrue);

    /** set ClinicalTrialTimePointDescription (0012,0051)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setClinicalTrialTimePointDescription(const OFString &value, const OFBool check = OFTrue);

    /** set ClinicalTrialTimePointID (0012,0050)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setClinicalTrialTimePointID(const OFString &value, const OFBool check = OFTrue);

    /** set CommentsOnThePerformedProcedureStep (0040,0280)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setCommentsOnThePerformedProcedureStep(const OFString &value, const OFBool check = OFTrue);

    /** set ConsultingPhysicianName (0008,009c)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (PN) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setConsultingPhysicianName(const OFString &value, const OFBool check = OFTrue);

    /** set ContentQualification (0018,9004)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContentQualification(const OFString &value, const OFBool check = OFTrue);

    /** set DateOfLastCalibration (0018,1200)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDateOfLastCalibration(const OFString &value, const OFBool check = OFTrue);

    /** set DeidentificationMethod (0012,0063)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDeidentificationMethod(const OFString &value, const OFBool check = OFTrue);

    /** set DeviceSerialNumber (0018,1000)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDeviceSerialNumber(const OFString &value, const OFBool check = OFTrue);

    /** set EthnicGroup (0010,2160)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setEthnicGroup(const OFString &value, const OFBool check = OFTrue);

    /** set FrameOfReferenceUID (0020,0052)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setFrameOfReferenceUID(const OFString &value, const OFBool check = OFTrue);

    /** set GantryID (0018,1008)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setGantryID(const OFString &value, const OFBool check = OFTrue);

    /** set InstanceCoercionDateTime (0008,0015)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DT) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstanceCoercionDateTime(const OFString &value, const OFBool check = OFTrue);

    /** set InstanceCreationDate (0008,0012)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstanceCreationDate(const OFString &value, const OFBool check = OFTrue);

    /** set InstanceCreationTime (0008,0013)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstanceCreationTime(const OFString &value, const OFBool check = OFTrue);

    /** set InstanceCreatorUID (0008,0014)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstanceCreatorUID(const OFString &value, const OFBool check = OFTrue);

    /** set InstanceNumber (0020,0013)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstanceNumber(const OFString &value, const OFBool check = OFTrue);

    /** set InstitutionAddress (0008,0081)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstitutionAddress(const OFString &value, const OFBool check = OFTrue);

    /** set InstitutionName (0008,0080)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstitutionName(const OFString &value, const OFBool check = OFTrue);

    /** set InstitutionalDepartmentName (0008,1040)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstitutionalDepartmentName(const OFString &value, const OFBool check = OFTrue);

    /** set IssuerOfPatientID (0010,0021)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setIssuerOfPatientID(const OFString &value, const OFBool check = OFTrue);

    /** set LongitudinalTemporalInformationModified (0028,0303)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setLongitudinalTemporalInformationModified(const OFString &value, const OFBool check = OFTrue);

    /** set Manufacturer (0008,0070)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setManufacturer(const OFString &value, const OFBool check = OFTrue);

    /** set ManufacturerModelName (0008,1090)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setManufacturerModelName(const OFString &value, const OFBool check = OFTrue);

    /** set Modality (0008,0060)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setModality(const OFString &value, const OFBool check = OFTrue);

    /** set NameOfPhysiciansReadingStudy (0008,1060)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (PN) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setNameOfPhysiciansReadingStudy(const OFString &value, const OFBool check = OFTrue);

    /** set Occupation (0010,2180)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setOccupation(const OFString &value, const OFBool check = OFTrue);

    /** set OperatorsName (0008,1070)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (PN) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setOperatorsName(const OFString &value, const OFBool check = OFTrue);

    /** set OriginalSpecializedSOPClassUID (0008,001b)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setOriginalSpecializedSOPClassUID(const OFString &value, const OFBool check = OFTrue);

    /** set OtherPatientIDs (0010,1000)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setOtherPatientIDs(const OFString &value, const OFBool check = OFTrue);

    /** set OtherPatientNames (0010,1001)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (PN) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setOtherPatientNames(const OFString &value, const OFBool check = OFTrue);

    /** set PatientAge (0010,1010)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (AS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientAge(const OFString &value, const OFBool check = OFTrue);

    /** set PatientBirthDate (0010,0030)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientBirthDate(const OFString &value, const OFBool check = OFTrue);

    /** set PatientBirthTime (0010,0032)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientBirthTime(const OFString &value, const OFBool check = OFTrue);

    /** set PatientBreedDescription (0010,2292)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientBreedDescription(const OFString &value, const OFBool check = OFTrue);

    /** set PatientComments (0010,4000)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LT) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientComments(const OFString &value, const OFBool check = OFTrue);

    /** set PatientID (0010,0020)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientID(const OFString &value, const OFBool check = OFTrue);

    /** set PatientIdentityRemoved (0012,0062)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientIdentityRemoved(const OFString &value, const OFBool check = OFTrue);

    /** set PatientName (0010,0010)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (PN) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientName(const OFString &value, const OFBool check = OFTrue);

    /** set PatientSex (0010,0040)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientSex(const OFString &value, const OFBool check = OFTrue);

    /** set PatientSexNeutered (0010,2203)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientSexNeutered(const OFString &value, const OFBool check = OFTrue);

    /** set PatientSize (0010,1020)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientSize(const OFString &value, const OFBool check = OFTrue);

    /** set PatientSpeciesDescription (0010,2201)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientSpeciesDescription(const OFString &value, const OFBool check = OFTrue);

    /** set PatientWeight (0010,1030)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientWeight(const OFString &value, const OFBool check = OFTrue);

    /** set PerformedProcedureStepDescription (0040,0254)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPerformedProcedureStepDescription(const OFString &value, const OFBool check = OFTrue);

    /** set PerformedProcedureStepEndDate (0040,0250)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPerformedProcedureStepEndDate(const OFString &value, const OFBool check = OFTrue);

    /** set PerformedProcedureStepEndTime (0040,0251)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPerformedProcedureStepEndTime(const OFString &value, const OFBool check = OFTrue);

    /** set PerformedProcedureStepID (0040,0253)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPerformedProcedureStepID(const OFString &value, const OFBool check = OFTrue);

    /** set PerformedProcedureStepStartDate (0040,0244)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPerformedProcedureStepStartDate(const OFString &value, const OFBool check = OFTrue);

    /** set PerformedProcedureStepStartTime (0040,0245)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPerformedProcedureStepStartTime(const OFString &value, const OFBool check = OFTrue);

    /** set PhysiciansOfRecord (0008,1048)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (PN) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPhysiciansOfRecord(const OFString &value, const OFBool check = OFTrue);

    /** set PixelPaddingValue (0028,0120)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPixelPaddingValue(const Uint16 value, const unsigned long pos = 0);

    /** set PlanIntent (300a,000a)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPlanIntent(const OFString &value, const OFBool check = OFTrue);

    /** set PositionReferenceIndicator (0020,1040)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPositionReferenceIndicator(const OFString &value, const OFBool check = OFTrue);

    /** set PrescriptionDescription (300a,000e)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPrescriptionDescription(const OFString &value, const OFBool check = OFTrue);

    /** set QualityControlSubject (0010,0200)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setQualityControlSubject(const OFString &value, const OFBool check = OFTrue);

    /** set QueryRetrieveView (0008,0053)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setQueryRetrieveView(const OFString &value, const OFBool check = OFTrue);

    /** set RTPlanDate (300a,0006)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRTPlanDate(const OFString &value, const OFBool check = OFTrue);

    /** set RTPlanDescription (300a,0004)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRTPlanDescription(const OFString &value, const OFBool check = OFTrue);

    /** set RTPlanGeometry (300a,000c)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRTPlanGeometry(const OFString &value, const OFBool check = OFTrue);

    /** set RTPlanLabel (300a,0002)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRTPlanLabel(const OFString &value, const OFBool check = OFTrue);

    /** set RTPlanName (300a,0003)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRTPlanName(const OFString &value, const OFBool check = OFTrue);

    /** set RTPlanTime (300a,0007)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRTPlanTime(const OFString &value, const OFBool check = OFTrue);

    /** set ReferringPhysicianName (0008,0090)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (PN) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setReferringPhysicianName(const OFString &value, const OFBool check = OFTrue);

    /** set RelatedGeneralSOPClassUID (0008,001a)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UI) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRelatedGeneralSOPClassUID(const OFString &value, const OFBool check = OFTrue);

    /** set ResponsibleOrganization (0010,2299)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setResponsibleOrganization(const OFString &value, const OFBool check = OFTrue);

    /** set ResponsiblePerson (0010,2297)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (PN) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setResponsiblePerson(const OFString &value, const OFBool check = OFTrue);

    /** set ResponsiblePersonRole (0010,2298)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setResponsiblePersonRole(const OFString &value, const OFBool check = OFTrue);

    /** set ReviewDate (300e,0004)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setReviewDate(const OFString &value, const OFBool check = OFTrue);

    /** set ReviewTime (300e,0005)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setReviewTime(const OFString &value, const OFBool check = OFTrue);

    /** set ReviewerName (300e,0008)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (PN) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setReviewerName(const OFString &value, const OFBool check = OFTrue);

    /** set SOPAuthorizationComment (0100,0424)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LT) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSOPAuthorizationComment(const OFString &value, const OFBool check = OFTrue);

    /** set SOPAuthorizationDateTime (0100,0420)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DT) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSOPAuthorizationDateTime(const OFString &value, const OFBool check = OFTrue);

    /** set SOPClassUID (0008,0016)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSOPClassUID(const OFString &value, const OFBool check = OFTrue);

    /** set SOPInstanceStatus (0100,0410)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSOPInstanceStatus(const OFString &value, const OFBool check = OFTrue);

    /** set SOPInstanceUID (0008,0018)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSOPInstanceUID(const OFString &value, const OFBool check = OFTrue);

    /** set SeriesDate (0008,0021)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSeriesDate(const OFString &value, const OFBool check = OFTrue);

    /** set SeriesDescription (0008,103e)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSeriesDescription(const OFString &value, const OFBool check = OFTrue);

    /** set SeriesInstanceUID (0020,000e)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSeriesInstanceUID(const OFString &value, const OFBool check = OFTrue);

    /** set SeriesNumber (0020,0011)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSeriesNumber(const OFString &value, const OFBool check = OFTrue);

    /** set SeriesTime (0008,0031)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSeriesTime(const OFString &value, const OFBool check = OFTrue);

    /** set ServiceEpisodeDescription (0038,0062)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setServiceEpisodeDescription(const OFString &value, const OFBool check = OFTrue);

    /** set ServiceEpisodeID (0038,0060)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setServiceEpisodeID(const OFString &value, const OFBool check = OFTrue);

    /** set SoftwareVersions (0018,1020)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSoftwareVersions(const OFString &value, const OFBool check = OFTrue);

    /** set SpatialResolution (0018,1050)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSpatialResolution(const OFString &value, const OFBool check = OFTrue);

    /** set SpecificCharacterSet (0008,0005)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSpecificCharacterSet(const OFString &value, const OFBool check = OFTrue);

    /** set StationName (0008,1010)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStationName(const OFString &value, const OFBool check = OFTrue);

    /** set StudyDate (0008,0020)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStudyDate(const OFString &value, const OFBool check = OFTrue);

    /** set StudyDescription (0008,1030)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStudyDescription(const OFString &value, const OFBool check = OFTrue);

    /** set StudyID (0020,0010)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStudyID(const OFString &value, const OFBool check = OFTrue);

    /** set StudyInstanceUID (0020,000d)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStudyInstanceUID(const OFString &value, const OFBool check = OFTrue);

    /** set StudyTime (0008,0030)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStudyTime(const OFString &value, const OFBool check = OFTrue);

    /** set TimeOfLastCalibration (0018,1201)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTimeOfLastCalibration(const OFString &value, const OFBool check = OFTrue);

    /** set TimezoneOffsetFromUTC (0008,0201)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTimezoneOffsetFromUTC(const OFString &value, const OFBool check = OFTrue);

    /** set TreatmentProtocols (300a,0009)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTreatmentProtocols(const OFString &value, const OFBool check = OFTrue);

    /** set TreatmentSites (300a,000b)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTreatmentSites(const OFString &value, const OFBool check = OFTrue);


  protected:

    /** check the given dataset before reading.
      * This methods checks whether the dataset contains at least the DICOM attribute
      * SOPClassUID (0008,0016) with the expected value.  Any incorrectness regarding
      * this attribute is reported to the log stream (if enabled for error messages).
      * @param  dataset  DICOM dataset to be checked
      * @return status, EC_Normal if successful, an error code otherwise
      */
    OFCondition checkDatasetForReading(DcmItem &dataset);

    /** update various DICOM attributes
     */
    void updateAttributes();

  private:

    // DICOM attributes are listed ordered by module

    // --- PatientModule (M) ---

    /// PatientName (0010,0010) vr=PN, vm=1, type=2
    DcmPersonName PatientName;
    /// PatientID (0010,0020) vr=LO, vm=1, type=2
    DcmLongString PatientID;
    /// IssuerOfPatientID (0010,0021) vr=LO, vm=1, type=3
    DcmLongString IssuerOfPatientID;
    /// IssuerOfPatientIDQualifiersSequence (0010,0024) vr=SQ, vm=1, type=3
    DRTIssuerOfPatientIDQualifiersSequence IssuerOfPatientIDQualifiersSequence;
    /// PatientBirthDate (0010,0030) vr=DA, vm=1, type=2
    DcmDate PatientBirthDate;
    /// PatientSex (0010,0040) vr=CS, vm=1, type=2
    DcmCodeString PatientSex;
    /// ReferencedPatientPhotoSequence (0010,1100) vr=SQ, vm=1, type=3
    DRTReferencedPatientPhotoSequence ReferencedPatientPhotoSequence;
    /// QualityControlSubject (0010,0200) vr=CS, vm=1, type=3
    DcmCodeString QualityControlSubject;
    /// ReferencedPatientSequence (0008,1120) vr=SQ, vm=1, type=3
    DRTReferencedPatientSequence ReferencedPatientSequence;
    /// PatientBirthTime (0010,0032) vr=TM, vm=1, type=3
    DcmTime PatientBirthTime;
    /// OtherPatientIDs (0010,1000) vr=LO, vm=1-n, type=3
    DcmLongString OtherPatientIDs;
    /// OtherPatientIDsSequence (0010,1002) vr=SQ, vm=1, type=3
    DRTOtherPatientIDsSequence OtherPatientIDsSequence;
    /// OtherPatientNames (0010,1001) vr=PN, vm=1-n, type=3
    DcmPersonName OtherPatientNames;
    /// EthnicGroup (0010,2160) vr=SH, vm=1, type=3
    DcmShortString EthnicGroup;
    /// PatientComments (0010,4000) vr=LT, vm=1, type=3
    DcmLongText PatientComments;
    /// PatientSpeciesDescription (0010,2201) vr=LO, vm=1, type=1C
    DcmLongString PatientSpeciesDescription;
    /// PatientSpeciesCodeSequence (0010,2202) vr=SQ, vm=1, type=1C
    DRTPatientSpeciesCodeSequence PatientSpeciesCodeSequence;
    /// PatientBreedDescription (0010,2292) vr=LO, vm=1, type=2C
    DcmLongString PatientBreedDescription;
    /// PatientBreedCodeSequence (0010,2293) vr=SQ, vm=1, type=2C
    DRTPatientBreedCodeSequence PatientBreedCodeSequence;
    /// BreedRegistrationSequence (0010,2294) vr=SQ, vm=1, type=2C
    DRTBreedRegistrationSequence BreedRegistrationSequence;
    /// ResponsiblePerson (0010,2297) vr=PN, vm=1, type=2C
    DcmPersonName ResponsiblePerson;
    /// ResponsiblePersonRole (0010,2298) vr=CS, vm=1, type=1C
    DcmCodeString ResponsiblePersonRole;
    /// ResponsibleOrganization (0010,2299) vr=LO, vm=1, type=2C
    DcmLongString ResponsibleOrganization;
    /// PatientIdentityRemoved (0012,0062) vr=CS, vm=1, type=3
    DcmCodeString PatientIdentityRemoved;
    /// DeidentificationMethod (0012,0063) vr=LO, vm=1-n, type=1C
    DcmLongString DeidentificationMethod;
    /// DeidentificationMethodCodeSequence (0012,0064) vr=SQ, vm=1, type=1C
    DRTDeidentificationMethodCodeSequence DeidentificationMethodCodeSequence;

    // --- ClinicalTrialSubjectModule (U) ---

    /// ClinicalTrialSponsorName (0012,0010) vr=LO, vm=1, type=1
    DcmLongString ClinicalTrialSponsorName;
    /// ClinicalTrialProtocolID (0012,0020) vr=LO, vm=1, type=1
    DcmLongString ClinicalTrialProtocolID;
    /// ClinicalTrialProtocolName (0012,0021) vr=LO, vm=1, type=2
    DcmLongString ClinicalTrialProtocolName;
    /// ClinicalTrialSiteID (0012,0030) vr=LO, vm=1, type=2
    DcmLongString ClinicalTrialSiteID;
    /// ClinicalTrialSiteName (0012,0031) vr=LO, vm=1, type=2
    DcmLongString ClinicalTrialSiteName;
    /// ClinicalTrialSubjectID (0012,0040) vr=LO, vm=1, type=1C
    DcmLongString ClinicalTrialSubjectID;
    /// ClinicalTrialSubjectReadingID (0012,0042) vr=LO, vm=1, type=1C
    DcmLongString ClinicalTrialSubjectReadingID;
    /// ClinicalTrialProtocolEthicsCommitteeName (0012,0081) vr=LO, vm=1, type=1C
    DcmLongString ClinicalTrialProtocolEthicsCommitteeName;
    /// ClinicalTrialProtocolEthicsCommitteeApprovalNumber (0012,0082) vr=LO, vm=1, type=3
    DcmLongString ClinicalTrialProtocolEthicsCommitteeApprovalNumber;

    // --- GeneralStudyModule (M) ---

    /// StudyInstanceUID (0020,000d) vr=UI, vm=1, type=1
    DcmUniqueIdentifier StudyInstanceUID;
    /// StudyDate (0008,0020) vr=DA, vm=1, type=2
    DcmDate StudyDate;
    /// StudyTime (0008,0030) vr=TM, vm=1, type=2
    DcmTime StudyTime;
    /// ReferringPhysicianName (0008,0090) vr=PN, vm=1, type=2
    DcmPersonName ReferringPhysicianName;
    /// ReferringPhysicianIdentificationSequence (0008,0096) vr=SQ, vm=1, type=3
    DRTReferringPhysicianIdentificationSequence ReferringPhysicianIdentificationSequence;
    /// ConsultingPhysicianName (0008,009c) vr=PN, vm=1-n, type=3
    DcmPersonName ConsultingPhysicianName;
    /// ConsultingPhysicianIdentificationSequence (0008,009d) vr=SQ, vm=1, type=3
    DRTConsultingPhysicianIdentificationSequence ConsultingPhysicianIdentificationSequence;
    /// StudyID (0020,0010) vr=SH, vm=1, type=2
    DcmShortString StudyID;
    /// AccessionNumber (0008,0050) vr=SH, vm=1, type=2
    DcmShortString AccessionNumber;
    /// IssuerOfAccessionNumberSequence (0008,0051) vr=SQ, vm=1, type=3
    DRTIssuerOfAccessionNumberSequence IssuerOfAccessionNumberSequence;
    /// StudyDescription (0008,1030) vr=LO, vm=1, type=3
    DcmLongString StudyDescription;
    /// PhysiciansOfRecord (0008,1048) vr=PN, vm=1-n, type=3
    DcmPersonName PhysiciansOfRecord;
    /// PhysiciansOfRecordIdentificationSequence (0008,1049) vr=SQ, vm=1, type=3
    DRTPhysiciansOfRecordIdentificationSequence PhysiciansOfRecordIdentificationSequence;
    /// NameOfPhysiciansReadingStudy (0008,1060) vr=PN, vm=1-n, type=3
    DcmPersonName NameOfPhysiciansReadingStudy;
    /// PhysiciansReadingStudyIdentificationSequence (0008,1062) vr=SQ, vm=1, type=3
    DRTPhysiciansReadingStudyIdentificationSequence PhysiciansReadingStudyIdentificationSequence;
    /// RequestingServiceCodeSequence (0032,1034) vr=SQ, vm=1, type=3
    DRTRequestingServiceCodeSequence RequestingServiceCodeSequence;
    /// ReferencedStudySequence (0008,1110) vr=SQ, vm=1, type=3
    DRTReferencedStudySequence ReferencedStudySequence;
    /// ProcedureCodeSequence (0008,1032) vr=SQ, vm=1, type=3
    DRTProcedureCodeSequence ProcedureCodeSequence;
    /// ReasonForPerformedProcedureCodeSequence (0040,1012) vr=SQ, vm=1, type=3
    DRTReasonForPerformedProcedureCodeSequence ReasonForPerformedProcedureCodeSequence;

    // --- PatientStudyModule (U) ---

    /// AdmittingDiagnosesDescription (0008,1080) vr=LO, vm=1-n, type=3
    DcmLongString AdmittingDiagnosesDescription;
    /// AdmittingDiagnosesCodeSequence (0008,1084) vr=SQ, vm=1, type=3
    DRTAdmittingDiagnosesCodeSequence AdmittingDiagnosesCodeSequence;
    /// PatientAge (0010,1010) vr=AS, vm=1, type=3
    DcmAgeString PatientAge;
    /// PatientSize (0010,1020) vr=DS, vm=1, type=3
    DcmDecimalString PatientSize;
    /// PatientWeight (0010,1030) vr=DS, vm=1, type=3
    DcmDecimalString PatientWeight;
    /// PatientSizeCodeSequence (0010,1021) vr=SQ, vm=1, type=3
    DRTPatientSizeCodeSequence PatientSizeCodeSequence;
    /// Occupation (0010,2180) vr=SH, vm=1, type=3
    DcmShortString Occupation;
    /// AdditionalPatientHistory (0010,21b0) vr=LT, vm=1, type=3
    DcmLongText AdditionalPatientHistory;
    /// AdmissionID (0038,0010) vr=LO, vm=1, type=3
    DcmLongString AdmissionID;
    /// IssuerOfAdmissionIDSequence (0038,0014) vr=SQ, vm=1, type=3
    DRTIssuerOfAdmissionIDSequence IssuerOfAdmissionIDSequence;
    /// ServiceEpisodeID (0038,0060) vr=LO, vm=1, type=3
    DcmLongString ServiceEpisodeID;
    /// IssuerOfServiceEpisodeIDSequence (0038,0064) vr=SQ, vm=1, type=3
    DRTIssuerOfServiceEpisodeIDSequence IssuerOfServiceEpisodeIDSequence;
    /// ServiceEpisodeDescription (0038,0062) vr=LO, vm=1, type=3
    DcmLongString ServiceEpisodeDescription;
    /// PatientSexNeutered (0010,2203) vr=CS, vm=1, type=2C
    DcmCodeString PatientSexNeutered;

    // --- ClinicalTrialStudyModule (U) ---

    /// ClinicalTrialTimePointID (0012,0050) vr=LO, vm=1, type=2
    DcmLongString ClinicalTrialTimePointID;
    /// ClinicalTrialTimePointDescription (0012,0051) vr=ST, vm=1, type=3
    DcmShortText ClinicalTrialTimePointDescription;
    /// ConsentForClinicalTrialUseSequence (0012,0083) vr=SQ, vm=1, type=3
    DRTConsentForClinicalTrialUseSequence ConsentForClinicalTrialUseSequence;

    // --- RTSeriesModule (M) ---

    /// Modality (0008,0060) vr=CS, vm=1, type=1
    DcmCodeString Modality;
    /// SeriesInstanceUID (0020,000e) vr=UI, vm=1, type=1
    DcmUniqueIdentifier SeriesInstanceUID;
    /// SeriesNumber (0020,0011) vr=IS, vm=1, type=2
    DcmIntegerString SeriesNumber;
    /// SeriesDate (0008,0021) vr=DA, vm=1, type=3
    DcmDate SeriesDate;
    /// SeriesTime (0008,0031) vr=TM, vm=1, type=3
    DcmTime SeriesTime;
    /// SeriesDescription (0008,103e) vr=LO, vm=1, type=3
    DcmLongString SeriesDescription;
    /// SeriesDescriptionCodeSequence (0008,103f) vr=SQ, vm=1, type=3
    DRTSeriesDescriptionCodeSequence SeriesDescriptionCodeSequence;
    /// OperatorsName (0008,1070) vr=PN, vm=1-n, type=2
    DcmPersonName OperatorsName;
    /// ReferencedPerformedProcedureStepSequence (0008,1111) vr=SQ, vm=1, type=3
    DRTReferencedPerformedProcedureStepSequence ReferencedPerformedProcedureStepSequence;
    /// RequestAttributesSequence (0040,0275) vr=SQ, vm=1, type=3
    DRTRequestAttributesSequence RequestAttributesSequence;
    /// PerformedProcedureStepID (0040,0253) vr=SH, vm=1, type=3
    DcmShortString PerformedProcedureStepID;
    /// PerformedProcedureStepStartDate (0040,0244) vr=DA, vm=1, type=3
    DcmDate PerformedProcedureStepStartDate;
    /// PerformedProcedureStepStartTime (0040,0245) vr=TM, vm=1, type=3
    DcmTime PerformedProcedureStepStartTime;
    /// PerformedProcedureStepEndDate (0040,0250) vr=DA, vm=1, type=3
    DcmDate PerformedProcedureStepEndDate;
    /// PerformedProcedureStepEndTime (0040,0251) vr=TM, vm=1, type=3
    DcmTime PerformedProcedureStepEndTime;
    /// PerformedProcedureStepDescription (0040,0254) vr=LO, vm=1, type=3
    DcmLongString PerformedProcedureStepDescription;
    /// PerformedProtocolCodeSequence (0040,0260) vr=SQ, vm=1, type=3
    DRTPerformedProtocolCodeSequence PerformedProtocolCodeSequence;
    /// CommentsOnThePerformedProcedureStep (0040,0280) vr=ST, vm=1, type=3
    DcmShortText CommentsOnThePerformedProcedureStep;

    // --- ClinicalTrialSeriesModule (U) ---

    /// ClinicalTrialCoordinatingCenterName (0012,0060) vr=LO, vm=1, type=2
    DcmLongString ClinicalTrialCoordinatingCenterName;
    /// ClinicalTrialSeriesID (0012,0071) vr=LO, vm=1, type=3
    DcmLongString ClinicalTrialSeriesID;
    /// ClinicalTrialSeriesDescription (0012,0072) vr=LO, vm=1, type=3
    DcmLongString ClinicalTrialSeriesDescription;

    // --- FrameOfReferenceModule (M) ---

    /// FrameOfReferenceUID (0020,0052) vr=UI, vm=1, type=1
    DcmUniqueIdentifier FrameOfReferenceUID;
    /// PositionReferenceIndicator (0020,1040) vr=LO, vm=1, type=2
    DcmLongString PositionReferenceIndicator;

    // --- GeneralEquipmentModule (M) ---

    /// Manufacturer (0008,0070) vr=LO, vm=1, type=2
    DcmLongString Manufacturer;
    /// InstitutionName (0008,0080) vr=LO, vm=1, type=3
    DcmLongString InstitutionName;
    /// InstitutionAddress (0008,0081) vr=ST, vm=1, type=3
    DcmShortText InstitutionAddress;
    /// StationName (0008,1010) vr=SH, vm=1, type=3
    DcmShortString StationName;
    /// InstitutionalDepartmentName (0008,1040) vr=LO, vm=1, type=3
    DcmLongString InstitutionalDepartmentName;
    /// ManufacturerModelName (0008,1090) vr=LO, vm=1, type=3
    DcmLongString ManufacturerModelName;
    /// DeviceSerialNumber (0018,1000) vr=LO, vm=1, type=3
    DcmLongString DeviceSerialNumber;
    /// SoftwareVersions (0018,1020) vr=LO, vm=1-n, type=3
    DcmLongString SoftwareVersions;
    /// GantryID (0018,1008) vr=LO, vm=1, type=3
    DcmLongString GantryID;
    /// SpatialResolution (0018,1050) vr=DS, vm=1, type=3
    DcmDecimalString SpatialResolution;
    /// DateOfLastCalibration (0018,1200) vr=DA, vm=1-n, type=3
    DcmDate DateOfLastCalibration;
    /// TimeOfLastCalibration (0018,1201) vr=TM, vm=1-n, type=3
    DcmTime TimeOfLastCalibration;
    /// PixelPaddingValue (0028,0120) vr=US/SS, vm=1, type=1C
    DcmUnsignedShort PixelPaddingValue;

    // --- RTGeneralPlanModule (M) ---

    /// RTPlanLabel (300a,0002) vr=SH, vm=1, type=1
    DcmShortString RTPlanLabel;
    /// RTPlanName (300a,0003) vr=LO, vm=1, type=3
    DcmLongString RTPlanName;
    /// RTPlanDescription (300a,0004) vr=ST, vm=1, type=3
    DcmShortText RTPlanDescription;
    /// InstanceNumber (0020,0013) vr=IS, vm=1, type=3
    /// - also defined in: SOPCommonModule
    DcmIntegerString InstanceNumber;
    /// RTPlanDate (300a,0006) vr=DA, vm=1, type=2
    DcmDate RTPlanDate;
    /// RTPlanTime (300a,0007) vr=TM, vm=1, type=2
    DcmTime RTPlanTime;
    /// TreatmentProtocols (300a,0009) vr=LO, vm=1-n, type=3
    DcmLongString TreatmentProtocols;
    /// PlanIntent (300a,000a) vr=CS, vm=1, type=3
    DcmCodeString PlanIntent;
    /// TreatmentSites (300a,000b) vr=LO, vm=1-n, type=3
    DcmLongString TreatmentSites;
    /// RTPlanGeometry (300a,000c) vr=CS, vm=1, type=1
    DcmCodeString RTPlanGeometry;
    /// ReferencedStructureSetSequence (300c,0060) vr=SQ, vm=1, type=1C
    DRTReferencedStructureSetSequence ReferencedStructureSetSequence;
    /// ReferencedDoseSequence (300c,0080) vr=SQ, vm=1, type=3
    DRTReferencedDoseSequence ReferencedDoseSequence;
    /// ReferencedRTPlanSequence (300c,0002) vr=SQ, vm=1, type=3
    DRTReferencedRTPlanSequenceInRTGeneralPlanModule ReferencedRTPlanSequence;

    // --- RTPrescriptionModule (U) ---

    /// PrescriptionDescription (300a,000e) vr=ST, vm=1, type=3
    DcmShortText PrescriptionDescription;
    /// DoseReferenceSequence (300a,0010) vr=SQ, vm=1, type=3
    DRTDoseReferenceSequence DoseReferenceSequence;

    // --- RTIonToleranceTablesModule (U) ---

    /// IonToleranceTableSequence (300a,03a0) vr=SQ, vm=1, type=1
    DRTIonToleranceTableSequence IonToleranceTableSequence;

    // --- RTPatientSetupModule (U) ---

    /// PatientSetupSequence (300a,0180) vr=SQ, vm=1, type=1
    DRTPatientSetupSequence PatientSetupSequence;

    // --- RTFractionSchemeModule (U) ---

    /// FractionGroupSequence (300a,0070) vr=SQ, vm=1, type=1
    DRTFractionGroupSequence FractionGroupSequence;

    // --- RTIonBeamsModule (C) ---

    /// IonBeamSequence (300a,03a2) vr=SQ, vm=1, type=1
    DRTIonBeamSequence IonBeamSequence;

    // --- ApprovalModule (U) ---

    /// ApprovalStatus (300e,0002) vr=CS, vm=1, type=1
    DcmCodeString ApprovalStatus;
    /// ReviewDate (300e,0004) vr=DA, vm=1, type=2C
    DcmDate ReviewDate;
    /// ReviewTime (300e,0005) vr=TM, vm=1, type=2C
    DcmTime ReviewTime;
    /// ReviewerName (300e,0008) vr=PN, vm=1, type=2C
    DcmPersonName ReviewerName;

    // --- SOPCommonModule (M) ---

    /// SOPClassUID (0008,0016) vr=UI, vm=1, type=1
    DcmUniqueIdentifier SOPClassUID;
    /// SOPInstanceUID (0008,0018) vr=UI, vm=1, type=1
    DcmUniqueIdentifier SOPInstanceUID;
    /// SpecificCharacterSet (0008,0005) vr=CS, vm=1-n, type=1C
    DcmCodeString SpecificCharacterSet;
    /// InstanceCreationDate (0008,0012) vr=DA, vm=1, type=3
    DcmDate InstanceCreationDate;
    /// InstanceCreationTime (0008,0013) vr=TM, vm=1, type=3
    DcmTime InstanceCreationTime;
    /// InstanceCoercionDateTime (0008,0015) vr=DT, vm=1, type=3
    DcmDateTime InstanceCoercionDateTime;
    /// InstanceCreatorUID (0008,0014) vr=UI, vm=1, type=3
    DcmUniqueIdentifier InstanceCreatorUID;
    /// RelatedGeneralSOPClassUID (0008,001a) vr=UI, vm=1-n, type=3
    DcmUniqueIdentifier RelatedGeneralSOPClassUID;
    /// OriginalSpecializedSOPClassUID (0008,001b) vr=UI, vm=1, type=3
    DcmUniqueIdentifier OriginalSpecializedSOPClassUID;
    /// CodingSchemeIdentificationSequence (0008,0110) vr=SQ, vm=1, type=3
    DRTCodingSchemeIdentificationSequence CodingSchemeIdentificationSequence;
    /// TimezoneOffsetFromUTC (0008,0201) vr=SH, vm=1, type=3
    DcmShortString TimezoneOffsetFromUTC;
    /// ContributingEquipmentSequence (0018,a001) vr=SQ, vm=1, type=3
    DRTContributingEquipmentSequence ContributingEquipmentSequence;
    // InstanceNumber (0020,0013) vr=IS, vm=1, type=3
    // - also defined in: RTGeneralPlanModule
    // DcmIntegerString InstanceNumber;
    /// SOPInstanceStatus (0100,0410) vr=CS, vm=1, type=3
    DcmCodeString SOPInstanceStatus;
    /// SOPAuthorizationDateTime (0100,0420) vr=DT, vm=1, type=3
    DcmDateTime SOPAuthorizationDateTime;
    /// SOPAuthorizationComment (0100,0424) vr=LT, vm=1, type=3
    DcmLongText SOPAuthorizationComment;
    /// AuthorizationEquipmentCertificationNumber (0100,0426) vr=LO, vm=1, type=3
    DcmLongString AuthorizationEquipmentCertificationNumber;
    /// MACParametersSequence (4ffe,0001) vr=SQ, vm=1, type=3
    DRTMACParametersSequence MACParametersSequence;
    /// DigitalSignaturesSequence (fffa,fffa) vr=SQ, vm=1, type=3
    DRTDigitalSignaturesSequence DigitalSignaturesSequence;
    /// EncryptedAttributesSequence (0400,0500) vr=SQ, vm=1, type=1C
    DRTEncryptedAttributesSequence EncryptedAttributesSequence;
    /// OriginalAttributesSequence (0400,0561) vr=SQ, vm=1, type=3
    DRTOriginalAttributesSequence OriginalAttributesSequence;
    /// HL7StructuredDocumentReferenceSequence (0040,a390) vr=SQ, vm=1, type=1C
    DRTHL7StructuredDocumentReferenceSequence HL7StructuredDocumentReferenceSequence;
    /// LongitudinalTemporalInformationModified (0028,0303) vr=CS, vm=1, type=3
    DcmCodeString LongitudinalTemporalInformationModified;
    /// QueryRetrieveView (0008,0053) vr=CS, vm=1, type=1C
    DcmCodeString QueryRetrieveView;
    /// ConversionSourceAttributesSequence (0020,9172) vr=SQ, vm=1, type=1C
    DRTConversionSourceAttributesSequence ConversionSourceAttributesSequence;
    /// ContentQualification (0018,9004) vr=CS, vm=1, type=3
    DcmCodeString ContentQualification;
    /// PrivateDataElementCharacteristicsSequence (0008,0300) vr=SQ, vm=1, type=3
    DRTPrivateDataElementCharacteristicsSequence PrivateDataElementCharacteristicsSequence;

    // --- CommonInstanceReferenceModule (U) ---

    /// ReferencedSeriesSequence (0008,1115) vr=SQ, vm=1, type=1C
    DRTReferencedSeriesSequence ReferencedSeriesSequence;
    /// StudiesContainingOtherReferencedInstancesSequence (0008,1200) vr=SQ, vm=1, type=1C
    DRTStudiesContainingOtherReferencedInstancesSequence StudiesContainingOtherReferencedInstancesSequence;

};


#endif
