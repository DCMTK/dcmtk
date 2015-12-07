/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTImageIOD
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTIMAGE_H
#define DRTIMAGE_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtadcs.h"   // for AdmittingDiagnosesCodeSequence
#include "dcmtk/dcmrt/seq/drtbrs.h"    // for BreedRegistrationSequence
#include "dcmtk/dcmrt/seq/drtcsis.h"   // for CodingSchemeIdentificationSequence
#include "dcmtk/dcmrt/seq/drtcctus.h"  // for ConsentForClinicalTrialUseSequence
#include "dcmtk/dcmrt/seq/drtcpis.h"   // for ConsultingPhysicianIdentificationSequence
#include "dcmtk/dcmrt/seq/drtcbars.h"  // for ContrastBolusAdministrationRouteSequence
#include "dcmtk/dcmrt/seq/drtbas.h"    // for ContrastBolusAgentSequence
#include "dcmtk/dcmrt/seq/drtces.h"    // for ContributingEquipmentSequence
#include "dcmtk/dcmrt/seq/drtcsas.h"   // for ConversionSourceAttributesSequence
#include "dcmtk/dcmrt/seq/drtdimcs.h"  // for DeidentificationMethodCodeSequence
#include "dcmtk/dcmrt/seq/drtdcs.h"    // for DerivationCodeSequence
#include "dcmtk/dcmrt/seq/drtds.h"     // for DeviceSequence
#include "dcmtk/dcmrt/seq/drtdss.h"    // for DigitalSignaturesSequence
#include "dcmtk/dcmrt/seq/drteas.h"    // for EncryptedAttributesSequence
#include "dcmtk/dcmrt/seq/drtes.h"     // for ExposureSequence
#include "dcmtk/dcmrt/seq/drtfms.h"    // for FluenceMapSequence
#include "dcmtk/dcmrt/seq/drtfes.h"    // for FrameExtractionSequence
#include "dcmtk/dcmrt/seq/drthsdrs.h"  // for HL7StructuredDocumentReferenceSequence
#include "dcmtk/dcmrt/seq/drtiis.h"    // for IconImageSequence
#include "dcmtk/dcmrt/seq/drtians.h"   // for IssuerOfAccessionNumberSequence
#include "dcmtk/dcmrt/seq/drtiais.h"   // for IssuerOfAdmissionIDSequence
#include "dcmtk/dcmrt/seq/drtipiqs.h"  // for IssuerOfPatientIDQualifiersSequence
#include "dcmtk/dcmrt/seq/drtiseis.h"  // for IssuerOfServiceEpisodeIDSequence
#include "dcmtk/dcmrt/seq/drtmps.h"    // for MACParametersSequence
#include "dcmtk/dcmrt/seq/drtmls.h"    // for ModalityLUTSequence
#include "dcmtk/dcmrt/seq/drtmacds.h"  // for MultiplexedAudioChannelsDescriptionCodeSequence
#include "dcmtk/dcmrt/seq/drtoas.h"    // for OriginalAttributesSequence
#include "dcmtk/dcmrt/seq/drtopis.h"   // for OtherPatientIDsSequence
#include "dcmtk/dcmrt/seq/drtpbcs.h"   // for PatientBreedCodeSequence
#include "dcmtk/dcmrt/seq/drtpsics.h"  // for PatientSizeCodeSequence
#include "dcmtk/dcmrt/seq/drtpscs.h"   // for PatientSpeciesCodeSequence
#include "dcmtk/dcmrt/seq/drtppcs.h"   // for PerformedProtocolCodeSequence
#include "dcmtk/dcmrt/seq/drtporis.h"  // for PhysiciansOfRecordIdentificationSequence
#include "dcmtk/dcmrt/seq/drtprsis.h"  // for PhysiciansReadingStudyIdentificationSequence
#include "dcmtk/dcmrt/seq/drtpdecs.h"  // for PrivateDataElementCharacteristicsSequence
#include "dcmtk/dcmrt/seq/drtpcs.h"    // for ProcedureCodeSequence
#include "dcmtk/dcmrt/seq/drtrwvms.h"  // for RealWorldValueMappingSequence
#include "dcmtk/dcmrt/seq/drtrppcs.h"  // for ReasonForPerformedProcedureCodeSequence
#include "dcmtk/dcmrt/seq/drtrims.h"   // for ReferencedImageSequence
#include "dcmtk/dcmrt/seq/drtris.h"    // for ReferencedInstanceSequence
#include "dcmtk/dcmrt/seq/drtrpphs.h"  // for ReferencedPatientPhotoSequence
#include "dcmtk/dcmrt/seq/drtrps.h"    // for ReferencedPatientSequence
#include "dcmtk/dcmrt/seq/drtrppss.h"  // for ReferencedPerformedProcedureStepSequence
#include "dcmtk/dcmrt/seq/drtrrtps5.h" // for ReferencedRTPlanSequence
#include "dcmtk/dcmrt/seq/drtrsers.h"  // for ReferencedSeriesSequence
#include "dcmtk/dcmrt/seq/drtrss.h"    // for ReferencedStudySequence
#include "dcmtk/dcmrt/seq/drtrpis.h"   // for ReferringPhysicianIdentificationSequence
#include "dcmtk/dcmrt/seq/drtras.h"    // for RequestAttributesSequence
#include "dcmtk/dcmrt/seq/drtrscs.h"   // for RequestingServiceCodeSequence
#include "dcmtk/dcmrt/seq/drtsdcs.h"   // for SeriesDescriptionCodeSequence
#include "dcmtk/dcmrt/seq/drtsis.h"    // for SourceImageSequence
#include "dcmtk/dcmrt/seq/drtscris.h"  // for StudiesContainingOtherReferencedInstancesSequence
#include "dcmtk/dcmrt/seq/drtvls.h"    // for VOILUTSequence


/** Interface class for RTImageIOD
 */
class DCMTK_DCMRT_EXPORT DRTImageIOD
  : protected DRTTypes
{

  public:

  // --- constructors, destructor and operators ---

    /** default constructor
     */
    DRTImageIOD();

    /** copy constructor
     *  @param copy IOD object to be copied
     */
    DRTImageIOD(const DRTImageIOD &copy);

    /** destructor
     */
    virtual ~DRTImageIOD();

    /** assigment operator
     *  @param copy IOD object to be copied
     */
    DRTImageIOD &operator=(const DRTImageIOD &copy);

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

    /** check whether FrameOfReferenceModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isFrameOfReferenceModulePresent(const OFBool complete = OFFalse);

    /** check whether ContrastBolusModule (C) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isContrastBolusModulePresent(const OFBool complete = OFFalse);

    /** check whether CineModule (C) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isCineModulePresent(const OFBool complete = OFFalse);

    /** check whether MultiFrameModule (C) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isMultiFrameModulePresent(const OFBool complete = OFFalse);

    /** check whether DeviceModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isDeviceModulePresent(const OFBool complete = OFFalse);

    /** check whether ModalityLUTModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isModalityLUTModulePresent(const OFBool complete = OFFalse);

    /** check whether VOILUTModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isVOILUTModulePresent(const OFBool complete = OFFalse);

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

    /** check whether FrameExtractionModule (C) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isFrameExtractionModulePresent(const OFBool complete = OFFalse);

  // --- get DICOM attribute values ---

    /** get AccessionNumber (0008,0050)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getAccessionNumber(OFString &value, const signed long pos = 0) const;

    /** get AcquisitionDate (0008,0022)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getAcquisitionDate(OFString &value, const signed long pos = 0) const;

    /** get AcquisitionDateTime (0008,002a)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getAcquisitionDateTime(OFString &value, const signed long pos = 0) const;

    /** get AcquisitionNumber (0020,0012)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getAcquisitionNumber(OFString &value, const signed long pos = 0) const;

    /** get AcquisitionNumber (0020,0012)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getAcquisitionNumber(Sint32 &value, const unsigned long pos = 0) const;

    /** get AcquisitionTime (0008,0032)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getAcquisitionTime(OFString &value, const signed long pos = 0) const;

    /** get ActualFrameDuration (0018,1242)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getActualFrameDuration(OFString &value, const signed long pos = 0) const;

    /** get ActualFrameDuration (0018,1242)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getActualFrameDuration(Sint32 &value, const unsigned long pos = 0) const;

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

    /** get BeamLimitingDeviceAngle (300a,0120)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getBeamLimitingDeviceAngle(OFString &value, const signed long pos = 0) const;

    /** get BeamLimitingDeviceAngle (300a,0120)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getBeamLimitingDeviceAngle(Float64 &value, const unsigned long pos = 0) const;

    /** get BitsAllocated (0028,0100)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getBitsAllocated(Uint16 &value, const unsigned long pos = 0) const;

    /** get BitsStored (0028,0101)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getBitsStored(Uint16 &value, const unsigned long pos = 0) const;

    /** get BluePaletteColorLookupTableData (0028,1203)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  count  stores number of items in the result array (if not NULL)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getBluePaletteColorLookupTableData(Uint16 *&value, unsigned long *count = NULL) const;

    /** get BluePaletteColorLookupTableDescriptor (0028,1103)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getBluePaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos = 0) const;

    /** get BurnedInAnnotation (0028,0301)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getBurnedInAnnotation(OFString &value, const signed long pos = 0) const;

    /** get CineRate (0018,0040)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getCineRate(OFString &value, const signed long pos = 0) const;

    /** get CineRate (0018,0040)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getCineRate(Sint32 &value, const unsigned long pos = 0) const;

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

    /** get Columns (0028,0011)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getColumns(Uint16 &value, const unsigned long pos = 0) const;

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

    /** get ContentDate (0008,0023)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContentDate(OFString &value, const signed long pos = 0) const;

    /** get ContentQualification (0018,9004)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContentQualification(OFString &value, const signed long pos = 0) const;

    /** get ContentTime (0008,0033)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContentTime(OFString &value, const signed long pos = 0) const;

    /** get ContrastBolusAgent (0018,0010)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastBolusAgent(OFString &value, const signed long pos = 0) const;

    /** get ContrastBolusIngredient (0018,1048)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastBolusIngredient(OFString &value, const signed long pos = 0) const;

    /** get ContrastBolusIngredientConcentration (0018,1049)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastBolusIngredientConcentration(OFString &value, const signed long pos = 0) const;

    /** get ContrastBolusIngredientConcentration (0018,1049)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastBolusIngredientConcentration(Float64 &value, const unsigned long pos = 0) const;

    /** get ContrastBolusRoute (0018,1040)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastBolusRoute(OFString &value, const signed long pos = 0) const;

    /** get ContrastBolusStartTime (0018,1042)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastBolusStartTime(OFString &value, const signed long pos = 0) const;

    /** get ContrastBolusStopTime (0018,1043)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastBolusStopTime(OFString &value, const signed long pos = 0) const;

    /** get ContrastBolusTotalDose (0018,1044)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastBolusTotalDose(OFString &value, const signed long pos = 0) const;

    /** get ContrastBolusTotalDose (0018,1044)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastBolusTotalDose(Float64 &value, const unsigned long pos = 0) const;

    /** get ContrastBolusVolume (0018,1041)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastBolusVolume(OFString &value, const signed long pos = 0) const;

    /** get ContrastBolusVolume (0018,1041)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastBolusVolume(Float64 &value, const unsigned long pos = 0) const;

    /** get ContrastFlowDuration (0018,1047)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastFlowDuration(OFString &value, const signed long pos = 0) const;

    /** get ContrastFlowDuration (0018,1047)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastFlowDuration(Float64 &value, const unsigned long pos = 0) const;

    /** get ContrastFlowDuration (0018,1047)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastFlowDuration(OFVector<Float64> &value) const;

    /** get ContrastFlowRate (0018,1046)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastFlowRate(OFString &value, const signed long pos = 0) const;

    /** get ContrastFlowRate (0018,1046)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastFlowRate(Float64 &value, const unsigned long pos = 0) const;

    /** get ContrastFlowRate (0018,1046)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContrastFlowRate(OFVector<Float64> &value) const;

    /** get ConversionType (0008,0064)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getConversionType(OFString &value, const signed long pos = 0) const;

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

    /** get DerivationDescription (0008,2111)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDerivationDescription(OFString &value, const signed long pos = 0) const;

    /** get DeviceSerialNumber (0018,1000)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDeviceSerialNumber(OFString &value, const signed long pos = 0) const;

    /** get EffectiveDuration (0018,0072)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getEffectiveDuration(OFString &value, const signed long pos = 0) const;

    /** get EffectiveDuration (0018,0072)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getEffectiveDuration(Float64 &value, const unsigned long pos = 0) const;

    /** get EndCumulativeMetersetWeight (300c,0009)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getEndCumulativeMetersetWeight(OFString &value, const signed long pos = 0) const;

    /** get EndCumulativeMetersetWeight (300c,0009)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getEndCumulativeMetersetWeight(Float64 &value, const unsigned long pos = 0) const;

    /** get EthnicGroup (0010,2160)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getEthnicGroup(OFString &value, const signed long pos = 0) const;

    /** get FractionNumber (3002,0029)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFractionNumber(OFString &value, const signed long pos = 0) const;

    /** get FractionNumber (3002,0029)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFractionNumber(Sint32 &value, const unsigned long pos = 0) const;

    /** get FrameDelay (0018,1066)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFrameDelay(OFString &value, const signed long pos = 0) const;

    /** get FrameDelay (0018,1066)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFrameDelay(Float64 &value, const unsigned long pos = 0) const;

    /** get FrameIncrementPointer (0028,0009)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFrameIncrementPointer(OFString &value, const signed long pos = 0) const;

    /** get FrameOfReferenceUID (0020,0052)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFrameOfReferenceUID(OFString &value, const signed long pos = 0) const;

    /** get FrameTime (0018,1063)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFrameTime(OFString &value, const signed long pos = 0) const;

    /** get FrameTime (0018,1063)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFrameTime(Float64 &value, const unsigned long pos = 0) const;

    /** get FrameTimeVector (0018,1065)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFrameTimeVector(OFString &value, const signed long pos = 0) const;

    /** get FrameTimeVector (0018,1065)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFrameTimeVector(Float64 &value, const unsigned long pos = 0) const;

    /** get FrameTimeVector (0018,1065)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFrameTimeVector(OFVector<Float64> &value) const;

    /** get GantryAngle (300a,011e)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getGantryAngle(OFString &value, const signed long pos = 0) const;

    /** get GantryAngle (300a,011e)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getGantryAngle(Float64 &value, const unsigned long pos = 0) const;

    /** get GantryID (0018,1008)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getGantryID(OFString &value, const signed long pos = 0) const;

    /** get GantryPitchAngle (300a,014a)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getGantryPitchAngle(Float32 &value, const unsigned long pos = 0) const;

    /** get GreenPaletteColorLookupTableData (0028,1202)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  count  stores number of items in the result array (if not NULL)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getGreenPaletteColorLookupTableData(Uint16 *&value, unsigned long *count = NULL) const;

    /** get GreenPaletteColorLookupTableDescriptor (0028,1102)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getGreenPaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos = 0) const;

    /** get HighBit (0028,0102)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getHighBit(Uint16 &value, const unsigned long pos = 0) const;

    /** get ICCProfile (0028,2000)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  count  stores number of items in the result array (if not NULL)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getICCProfile(Uint8 *&value, unsigned long *count = NULL) const;

    /** get ImageComments (0020,4000)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImageComments(OFString &value, const signed long pos = 0) const;

    /** get ImagePlanePixelSpacing (3002,0011)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImagePlanePixelSpacing(OFString &value, const signed long pos = 0) const;

    /** get ImagePlanePixelSpacing (3002,0011)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImagePlanePixelSpacing(Float64 &value, const unsigned long pos = 0) const;

    /** get ImagePlanePixelSpacing (3002,0011)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImagePlanePixelSpacing(OFVector<Float64> &value) const;

    /** get ImageTriggerDelay (0018,1067)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImageTriggerDelay(OFString &value, const signed long pos = 0) const;

    /** get ImageTriggerDelay (0018,1067)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImageTriggerDelay(Float64 &value, const unsigned long pos = 0) const;

    /** get ImageType (0008,0008)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImageType(OFString &value, const signed long pos = 0) const;

    /** get ImagesInAcquisition (0020,1002)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImagesInAcquisition(OFString &value, const signed long pos = 0) const;

    /** get ImagesInAcquisition (0020,1002)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImagesInAcquisition(Sint32 &value, const unsigned long pos = 0) const;

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

    /** get IrradiationEventUID (0008,3010)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getIrradiationEventUID(OFString &value, const signed long pos = 0) const;

    /** get IsocenterPosition (300a,012c)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getIsocenterPosition(OFString &value, const signed long pos = 0) const;

    /** get IsocenterPosition (300a,012c)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getIsocenterPosition(Float64 &value, const unsigned long pos = 0) const;

    /** get IsocenterPosition (300a,012c)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getIsocenterPosition(OFVector<Float64> &value) const;

    /** get IssuerOfPatientID (0010,0021)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getIssuerOfPatientID(OFString &value, const signed long pos = 0) const;

    /** get LargestImagePixelValue (0028,0107)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getLargestImagePixelValue(Uint16 &value, const unsigned long pos = 0) const;

    /** get LongitudinalTemporalInformationModified (0028,0303)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getLongitudinalTemporalInformationModified(OFString &value, const signed long pos = 0) const;

    /** get LossyImageCompression (0028,2110)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getLossyImageCompression(OFString &value, const signed long pos = 0) const;

    /** get LossyImageCompressionMethod (0028,2114)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getLossyImageCompressionMethod(OFString &value, const signed long pos = 0) const;

    /** get LossyImageCompressionRatio (0028,2112)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getLossyImageCompressionRatio(OFString &value, const signed long pos = 0) const;

    /** get LossyImageCompressionRatio (0028,2112)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getLossyImageCompressionRatio(Float64 &value, const unsigned long pos = 0) const;

    /** get LossyImageCompressionRatio (0028,2112)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getLossyImageCompressionRatio(OFVector<Float64> &value) const;

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

    /** get NumberOfFrames (0028,0008)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getNumberOfFrames(OFString &value, const signed long pos = 0) const;

    /** get NumberOfFrames (0028,0008)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getNumberOfFrames(Sint32 &value, const unsigned long pos = 0) const;

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

    /** get PatientOrientation (0020,0020)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientOrientation(OFString &value, const signed long pos = 0) const;

    /** get PatientPosition (0018,5100)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientPosition(OFString &value, const signed long pos = 0) const;

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

    /** get PatientSupportAngle (300a,0122)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientSupportAngle(OFString &value, const signed long pos = 0) const;

    /** get PatientSupportAngle (300a,0122)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientSupportAngle(Float64 &value, const unsigned long pos = 0) const;

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

    /** get PhotometricInterpretation (0028,0004)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPhotometricInterpretation(OFString &value, const signed long pos = 0) const;

    /** get PhysiciansOfRecord (0008,1048)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPhysiciansOfRecord(OFString &value, const signed long pos = 0) const;

    /** get PixelAspectRatio (0028,0034)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelAspectRatio(OFString &value, const signed long pos = 0) const;

    /** get PixelAspectRatio (0028,0034)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelAspectRatio(Sint32 &value, const unsigned long pos = 0) const;

    /** get PixelDataProviderURL (0028,7fe0)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelDataProviderURL(OFString &value, const signed long pos = 0) const;

    /** get PixelIntensityRelationship (0028,1040)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelIntensityRelationship(OFString &value, const signed long pos = 0) const;

    /** get PixelIntensityRelationshipSign (0028,1041)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelIntensityRelationshipSign(Sint16 &value, const unsigned long pos = 0) const;

    /** get PixelPaddingRangeLimit (0028,0121)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelPaddingRangeLimit(Uint16 &value, const unsigned long pos = 0) const;

    /** get PixelPaddingValue (0028,0120)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelPaddingValue(Uint16 &value, const unsigned long pos = 0) const;

    /** get PixelRepresentation (0028,0103)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelRepresentation(Uint16 &value, const unsigned long pos = 0) const;

    /** get PlanarConfiguration (0028,0006)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPlanarConfiguration(Uint16 &value, const unsigned long pos = 0) const;

    /** get PositionReferenceIndicator (0020,1040)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPositionReferenceIndicator(OFString &value, const signed long pos = 0) const;

    /** get PreferredPlaybackSequencing (0018,1244)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPreferredPlaybackSequencing(Uint16 &value, const unsigned long pos = 0) const;

    /** get PresentationLUTShape (2050,0020)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPresentationLUTShape(OFString &value, const signed long pos = 0) const;

    /** get PrimaryDosimeterUnit (300a,00b3)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPrimaryDosimeterUnit(OFString &value, const signed long pos = 0) const;

    /** get QualityControlImage (0028,0300)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getQualityControlImage(OFString &value, const signed long pos = 0) const;

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

    /** get RTImageDescription (3002,0004)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTImageDescription(OFString &value, const signed long pos = 0) const;

    /** get RTImageLabel (3002,0002)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTImageLabel(OFString &value, const signed long pos = 0) const;

    /** get RTImageName (3002,0003)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTImageName(OFString &value, const signed long pos = 0) const;

    /** get RTImageOrientation (3002,0010)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTImageOrientation(OFString &value, const signed long pos = 0) const;

    /** get RTImageOrientation (3002,0010)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTImageOrientation(Float64 &value, const unsigned long pos = 0) const;

    /** get RTImageOrientation (3002,0010)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTImageOrientation(OFVector<Float64> &value) const;

    /** get RTImagePlane (3002,000c)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTImagePlane(OFString &value, const signed long pos = 0) const;

    /** get RTImagePosition (3002,0012)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTImagePosition(OFString &value, const signed long pos = 0) const;

    /** get RTImagePosition (3002,0012)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTImagePosition(Float64 &value, const unsigned long pos = 0) const;

    /** get RTImagePosition (3002,0012)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTImagePosition(OFVector<Float64> &value) const;

    /** get RTImageSID (3002,0026)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTImageSID(OFString &value, const signed long pos = 0) const;

    /** get RTImageSID (3002,0026)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRTImageSID(Float64 &value, const unsigned long pos = 0) const;

    /** get RadiationMachineName (3002,0020)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRadiationMachineName(OFString &value, const signed long pos = 0) const;

    /** get RadiationMachineSAD (3002,0022)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRadiationMachineSAD(OFString &value, const signed long pos = 0) const;

    /** get RadiationMachineSAD (3002,0022)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRadiationMachineSAD(Float64 &value, const unsigned long pos = 0) const;

    /** get RadiationMachineSSD (3002,0024)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRadiationMachineSSD(OFString &value, const signed long pos = 0) const;

    /** get RadiationMachineSSD (3002,0024)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRadiationMachineSSD(Float64 &value, const unsigned long pos = 0) const;

    /** get RecognizableVisualFeatures (0028,0302)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRecognizableVisualFeatures(OFString &value, const signed long pos = 0) const;

    /** get RecommendedDisplayFrameRate (0008,2144)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRecommendedDisplayFrameRate(OFString &value, const signed long pos = 0) const;

    /** get RecommendedDisplayFrameRate (0008,2144)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRecommendedDisplayFrameRate(Sint32 &value, const unsigned long pos = 0) const;

    /** get RedPaletteColorLookupTableData (0028,1201)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  count  stores number of items in the result array (if not NULL)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRedPaletteColorLookupTableData(Uint16 *&value, unsigned long *count = NULL) const;

    /** get RedPaletteColorLookupTableDescriptor (0028,1101)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRedPaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos = 0) const;

    /** get ReferencedBeamNumber (300c,0006)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getReferencedBeamNumber(OFString &value, const signed long pos = 0) const;

    /** get ReferencedBeamNumber (300c,0006)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getReferencedBeamNumber(Sint32 &value, const unsigned long pos = 0) const;

    /** get ReferencedFractionGroupNumber (300c,0022)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getReferencedFractionGroupNumber(OFString &value, const signed long pos = 0) const;

    /** get ReferencedFractionGroupNumber (300c,0022)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getReferencedFractionGroupNumber(Sint32 &value, const unsigned long pos = 0) const;

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

    /** get ReportedValuesOrigin (3002,000a)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getReportedValuesOrigin(OFString &value, const signed long pos = 0) const;

    /** get RescaleIntercept (0028,1052)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRescaleIntercept(OFString &value, const signed long pos = 0) const;

    /** get RescaleIntercept (0028,1052)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRescaleIntercept(Float64 &value, const unsigned long pos = 0) const;

    /** get RescaleSlope (0028,1053)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRescaleSlope(OFString &value, const signed long pos = 0) const;

    /** get RescaleSlope (0028,1053)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRescaleSlope(Float64 &value, const unsigned long pos = 0) const;

    /** get RescaleType (0028,1054)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRescaleType(OFString &value, const signed long pos = 0) const;

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

    /** get Rows (0028,0010)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRows(Uint16 &value, const unsigned long pos = 0) const;

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

    /** get SamplesPerPixel (0028,0002)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSamplesPerPixel(Uint16 &value, const unsigned long pos = 0) const;

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

    /** get SmallestImagePixelValue (0028,0106)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSmallestImagePixelValue(Uint16 &value, const unsigned long pos = 0) const;

    /** get SoftwareVersions (0018,1020)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSoftwareVersions(OFString &value, const signed long pos = 0) const;

    /** get SourceToReferenceObjectDistance (3002,0028)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSourceToReferenceObjectDistance(OFString &value, const signed long pos = 0) const;

    /** get SourceToReferenceObjectDistance (3002,0028)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSourceToReferenceObjectDistance(Float64 &value, const unsigned long pos = 0) const;

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

    /** get StartCumulativeMetersetWeight (300c,0008)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStartCumulativeMetersetWeight(OFString &value, const signed long pos = 0) const;

    /** get StartCumulativeMetersetWeight (300c,0008)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStartCumulativeMetersetWeight(Float64 &value, const unsigned long pos = 0) const;

    /** get StartTrim (0008,2142)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStartTrim(OFString &value, const signed long pos = 0) const;

    /** get StartTrim (0008,2142)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStartTrim(Sint32 &value, const unsigned long pos = 0) const;

    /** get StationName (0008,1010)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStationName(OFString &value, const signed long pos = 0) const;

    /** get StereoPairsPresent (0022,0028)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStereoPairsPresent(OFString &value, const signed long pos = 0) const;

    /** get StopTrim (0008,2143)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStopTrim(OFString &value, const signed long pos = 0) const;

    /** get StopTrim (0008,2143)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStopTrim(Sint32 &value, const unsigned long pos = 0) const;

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

    /** get TableTopEccentricAngle (300a,0125)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTableTopEccentricAngle(OFString &value, const signed long pos = 0) const;

    /** get TableTopEccentricAngle (300a,0125)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTableTopEccentricAngle(Float64 &value, const unsigned long pos = 0) const;

    /** get TableTopEccentricAxisDistance (300a,0124)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTableTopEccentricAxisDistance(OFString &value, const signed long pos = 0) const;

    /** get TableTopEccentricAxisDistance (300a,0124)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTableTopEccentricAxisDistance(Float64 &value, const unsigned long pos = 0) const;

    /** get TableTopLateralPosition (300a,012a)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTableTopLateralPosition(OFString &value, const signed long pos = 0) const;

    /** get TableTopLateralPosition (300a,012a)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTableTopLateralPosition(Float64 &value, const unsigned long pos = 0) const;

    /** get TableTopLongitudinalPosition (300a,0129)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTableTopLongitudinalPosition(OFString &value, const signed long pos = 0) const;

    /** get TableTopLongitudinalPosition (300a,0129)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTableTopLongitudinalPosition(Float64 &value, const unsigned long pos = 0) const;

    /** get TableTopPitchAngle (300a,0140)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTableTopPitchAngle(Float32 &value, const unsigned long pos = 0) const;

    /** get TableTopRollAngle (300a,0144)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTableTopRollAngle(Float32 &value, const unsigned long pos = 0) const;

    /** get TableTopVerticalPosition (300a,0128)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTableTopVerticalPosition(OFString &value, const signed long pos = 0) const;

    /** get TableTopVerticalPosition (300a,0128)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTableTopVerticalPosition(Float64 &value, const unsigned long pos = 0) const;

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

    /** get VOILUTFunction (0028,1056)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getVOILUTFunction(OFString &value, const signed long pos = 0) const;

    /** get WindowCenter (0028,1050)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getWindowCenter(OFString &value, const signed long pos = 0) const;

    /** get WindowCenter (0028,1050)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getWindowCenter(Float64 &value, const unsigned long pos = 0) const;

    /** get WindowCenter (0028,1050)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getWindowCenter(OFVector<Float64> &value) const;

    /** get WindowCenterWidthExplanation (0028,1055)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getWindowCenterWidthExplanation(OFString &value, const signed long pos = 0) const;

    /** get WindowWidth (0028,1051)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getWindowWidth(OFString &value, const signed long pos = 0) const;

    /** get WindowWidth (0028,1051)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getWindowWidth(Float64 &value, const unsigned long pos = 0) const;

    /** get WindowWidth (0028,1051)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getWindowWidth(OFVector<Float64> &value) const;

    /** get XRayImageReceptorAngle (3002,000e)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getXRayImageReceptorAngle(OFString &value, const signed long pos = 0) const;

    /** get XRayImageReceptorAngle (3002,000e)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getXRayImageReceptorAngle(Float64 &value, const unsigned long pos = 0) const;

    /** get XRayImageReceptorTranslation (3002,000d)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getXRayImageReceptorTranslation(OFString &value, const signed long pos = 0) const;

    /** get XRayImageReceptorTranslation (3002,000d)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getXRayImageReceptorTranslation(Float64 &value, const unsigned long pos = 0) const;

    /** get XRayImageReceptorTranslation (3002,000d)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getXRayImageReceptorTranslation(OFVector<Float64> &value) const;

  // --- get pixel data element ---

    /** get PixelData (7fe0,0010)
     *  @return reference to pixel data element
     */
    DcmPixelData &getPixelData()
        { return PixelData; }

    /** get PixelData (7fe0,0010)
     *  @return const reference to pixel data element
     */
    const DcmPixelData &getPixelData() const
        { return PixelData; }

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

    /** get ContrastBolusAdministrationRouteSequence (0018,0014)
     *  @return reference to sequence element
     */
    DRTContrastBolusAdministrationRouteSequence &getContrastBolusAdministrationRouteSequence()
        { return ContrastBolusAdministrationRouteSequence; }

    /** get ContrastBolusAdministrationRouteSequence (0018,0014)
     *  @return const reference to sequence element
     */
    const DRTContrastBolusAdministrationRouteSequence &getContrastBolusAdministrationRouteSequence() const
        { return ContrastBolusAdministrationRouteSequence; }

    /** get ContrastBolusAgentSequence (0018,0012)
     *  @return reference to sequence element
     */
    DRTContrastBolusAgentSequence &getContrastBolusAgentSequence()
        { return ContrastBolusAgentSequence; }

    /** get ContrastBolusAgentSequence (0018,0012)
     *  @return const reference to sequence element
     */
    const DRTContrastBolusAgentSequence &getContrastBolusAgentSequence() const
        { return ContrastBolusAgentSequence; }

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

    /** get DerivationCodeSequence (0008,9215)
     *  @return reference to sequence element
     */
    DRTDerivationCodeSequence &getDerivationCodeSequence()
        { return DerivationCodeSequence; }

    /** get DerivationCodeSequence (0008,9215)
     *  @return const reference to sequence element
     */
    const DRTDerivationCodeSequence &getDerivationCodeSequence() const
        { return DerivationCodeSequence; }

    /** get DeviceSequence (0050,0010)
     *  @return reference to sequence element
     */
    DRTDeviceSequence &getDeviceSequence()
        { return DeviceSequence; }

    /** get DeviceSequence (0050,0010)
     *  @return const reference to sequence element
     */
    const DRTDeviceSequence &getDeviceSequence() const
        { return DeviceSequence; }

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

    /** get ExposureSequence (3002,0030)
     *  @return reference to sequence element
     */
    DRTExposureSequence &getExposureSequence()
        { return ExposureSequence; }

    /** get ExposureSequence (3002,0030)
     *  @return const reference to sequence element
     */
    const DRTExposureSequence &getExposureSequence() const
        { return ExposureSequence; }

    /** get FluenceMapSequence (3002,0040)
     *  @return reference to sequence element
     */
    DRTFluenceMapSequence &getFluenceMapSequence()
        { return FluenceMapSequence; }

    /** get FluenceMapSequence (3002,0040)
     *  @return const reference to sequence element
     */
    const DRTFluenceMapSequence &getFluenceMapSequence() const
        { return FluenceMapSequence; }

    /** get FrameExtractionSequence (0008,1164)
     *  @return reference to sequence element
     */
    DRTFrameExtractionSequence &getFrameExtractionSequence()
        { return FrameExtractionSequence; }

    /** get FrameExtractionSequence (0008,1164)
     *  @return const reference to sequence element
     */
    const DRTFrameExtractionSequence &getFrameExtractionSequence() const
        { return FrameExtractionSequence; }

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

    /** get IconImageSequence (0088,0200)
     *  @return reference to sequence element
     */
    DRTIconImageSequence &getIconImageSequence()
        { return IconImageSequence; }

    /** get IconImageSequence (0088,0200)
     *  @return const reference to sequence element
     */
    const DRTIconImageSequence &getIconImageSequence() const
        { return IconImageSequence; }

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

    /** get ModalityLUTSequence (0028,3000)
     *  @return reference to sequence element
     */
    DRTModalityLUTSequence &getModalityLUTSequence()
        { return ModalityLUTSequence; }

    /** get ModalityLUTSequence (0028,3000)
     *  @return const reference to sequence element
     */
    const DRTModalityLUTSequence &getModalityLUTSequence() const
        { return ModalityLUTSequence; }

    /** get MultiplexedAudioChannelsDescriptionCodeSequence (003a,0300)
     *  @return reference to sequence element
     */
    DRTMultiplexedAudioChannelsDescriptionCodeSequence &getMultiplexedAudioChannelsDescriptionCodeSequence()
        { return MultiplexedAudioChannelsDescriptionCodeSequence; }

    /** get MultiplexedAudioChannelsDescriptionCodeSequence (003a,0300)
     *  @return const reference to sequence element
     */
    const DRTMultiplexedAudioChannelsDescriptionCodeSequence &getMultiplexedAudioChannelsDescriptionCodeSequence() const
        { return MultiplexedAudioChannelsDescriptionCodeSequence; }

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

    /** get RealWorldValueMappingSequence (0040,9096)
     *  @return reference to sequence element
     */
    DRTRealWorldValueMappingSequence &getRealWorldValueMappingSequence()
        { return RealWorldValueMappingSequence; }

    /** get RealWorldValueMappingSequence (0040,9096)
     *  @return const reference to sequence element
     */
    const DRTRealWorldValueMappingSequence &getRealWorldValueMappingSequence() const
        { return RealWorldValueMappingSequence; }

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

    /** get ReferencedImageSequence (0008,1140)
     *  @return reference to sequence element
     */
    DRTReferencedImageSequence &getReferencedImageSequence()
        { return ReferencedImageSequence; }

    /** get ReferencedImageSequence (0008,1140)
     *  @return const reference to sequence element
     */
    const DRTReferencedImageSequence &getReferencedImageSequence() const
        { return ReferencedImageSequence; }

    /** get ReferencedInstanceSequence (0008,114a)
     *  @return reference to sequence element
     */
    DRTReferencedInstanceSequence &getReferencedInstanceSequence()
        { return ReferencedInstanceSequence; }

    /** get ReferencedInstanceSequence (0008,114a)
     *  @return const reference to sequence element
     */
    const DRTReferencedInstanceSequence &getReferencedInstanceSequence() const
        { return ReferencedInstanceSequence; }

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
    DRTReferencedRTPlanSequenceInRTImageModule &getReferencedRTPlanSequence()
        { return ReferencedRTPlanSequence; }

    /** get ReferencedRTPlanSequence (300c,0002)
     *  @return const reference to sequence element
     */
    const DRTReferencedRTPlanSequenceInRTImageModule &getReferencedRTPlanSequence() const
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

    /** get SourceImageSequence (0008,2112)
     *  @return reference to sequence element
     */
    DRTSourceImageSequence &getSourceImageSequence()
        { return SourceImageSequence; }

    /** get SourceImageSequence (0008,2112)
     *  @return const reference to sequence element
     */
    const DRTSourceImageSequence &getSourceImageSequence() const
        { return SourceImageSequence; }

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

    /** get VOILUTSequence (0028,3010)
     *  @return reference to sequence element
     */
    DRTVOILUTSequence &getVOILUTSequence()
        { return VOILUTSequence; }

    /** get VOILUTSequence (0028,3010)
     *  @return const reference to sequence element
     */
    const DRTVOILUTSequence &getVOILUTSequence() const
        { return VOILUTSequence; }

  // --- set DICOM attribute values ---

    /** set AccessionNumber (0008,0050)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setAccessionNumber(const OFString &value, const OFBool check = OFTrue);

    /** set AcquisitionDate (0008,0022)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setAcquisitionDate(const OFString &value, const OFBool check = OFTrue);

    /** set AcquisitionDateTime (0008,002a)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DT) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setAcquisitionDateTime(const OFString &value, const OFBool check = OFTrue);

    /** set AcquisitionNumber (0020,0012)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setAcquisitionNumber(const OFString &value, const OFBool check = OFTrue);

    /** set AcquisitionTime (0008,0032)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setAcquisitionTime(const OFString &value, const OFBool check = OFTrue);

    /** set ActualFrameDuration (0018,1242)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setActualFrameDuration(const OFString &value, const OFBool check = OFTrue);

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

    /** set BeamLimitingDeviceAngle (300a,0120)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setBeamLimitingDeviceAngle(const OFString &value, const OFBool check = OFTrue);

    /** set BitsAllocated (0028,0100)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setBitsAllocated(const Uint16 value, const unsigned long pos = 0);

    /** set BitsStored (0028,0101)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setBitsStored(const Uint16 value, const unsigned long pos = 0);

    /** set BluePaletteColorLookupTableData (0028,1203)
     *  @param  value  array value to be set (data is copied)
     *  @param  count  number of items in the array
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setBluePaletteColorLookupTableData(const Uint16 *value, const unsigned long count);

    /** set BluePaletteColorLookupTableDescriptor (0028,1103)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=3
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setBluePaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos = 0);

    /** set BurnedInAnnotation (0028,0301)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setBurnedInAnnotation(const OFString &value, const OFBool check = OFTrue);

    /** set CineRate (0018,0040)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setCineRate(const OFString &value, const OFBool check = OFTrue);

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

    /** set Columns (0028,0011)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setColumns(const Uint16 value, const unsigned long pos = 0);

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

    /** set ContentDate (0008,0023)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContentDate(const OFString &value, const OFBool check = OFTrue);

    /** set ContentQualification (0018,9004)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContentQualification(const OFString &value, const OFBool check = OFTrue);

    /** set ContentTime (0008,0033)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContentTime(const OFString &value, const OFBool check = OFTrue);

    /** set ContrastBolusAgent (0018,0010)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContrastBolusAgent(const OFString &value, const OFBool check = OFTrue);

    /** set ContrastBolusIngredient (0018,1048)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContrastBolusIngredient(const OFString &value, const OFBool check = OFTrue);

    /** set ContrastBolusIngredientConcentration (0018,1049)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContrastBolusIngredientConcentration(const OFString &value, const OFBool check = OFTrue);

    /** set ContrastBolusRoute (0018,1040)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContrastBolusRoute(const OFString &value, const OFBool check = OFTrue);

    /** set ContrastBolusStartTime (0018,1042)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContrastBolusStartTime(const OFString &value, const OFBool check = OFTrue);

    /** set ContrastBolusStopTime (0018,1043)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContrastBolusStopTime(const OFString &value, const OFBool check = OFTrue);

    /** set ContrastBolusTotalDose (0018,1044)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContrastBolusTotalDose(const OFString &value, const OFBool check = OFTrue);

    /** set ContrastBolusVolume (0018,1041)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContrastBolusVolume(const OFString &value, const OFBool check = OFTrue);

    /** set ContrastFlowDuration (0018,1047)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContrastFlowDuration(const OFString &value, const OFBool check = OFTrue);

    /** set ContrastFlowRate (0018,1046)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContrastFlowRate(const OFString &value, const OFBool check = OFTrue);

    /** set ConversionType (0008,0064)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setConversionType(const OFString &value, const OFBool check = OFTrue);

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

    /** set DerivationDescription (0008,2111)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDerivationDescription(const OFString &value, const OFBool check = OFTrue);

    /** set DeviceSerialNumber (0018,1000)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDeviceSerialNumber(const OFString &value, const OFBool check = OFTrue);

    /** set EffectiveDuration (0018,0072)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setEffectiveDuration(const OFString &value, const OFBool check = OFTrue);

    /** set EndCumulativeMetersetWeight (300c,0009)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setEndCumulativeMetersetWeight(const OFString &value, const OFBool check = OFTrue);

    /** set EthnicGroup (0010,2160)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setEthnicGroup(const OFString &value, const OFBool check = OFTrue);

    /** set FractionNumber (3002,0029)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setFractionNumber(const OFString &value, const OFBool check = OFTrue);

    /** set FrameDelay (0018,1066)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setFrameDelay(const OFString &value, const OFBool check = OFTrue);

    /** set FrameIncrementPointer (0028,0009)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (AT) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setFrameIncrementPointer(const OFString &value, const OFBool check = OFTrue);

    /** set FrameOfReferenceUID (0020,0052)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setFrameOfReferenceUID(const OFString &value, const OFBool check = OFTrue);

    /** set FrameTime (0018,1063)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setFrameTime(const OFString &value, const OFBool check = OFTrue);

    /** set FrameTimeVector (0018,1065)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setFrameTimeVector(const OFString &value, const OFBool check = OFTrue);

    /** set GantryAngle (300a,011e)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setGantryAngle(const OFString &value, const OFBool check = OFTrue);

    /** set GantryID (0018,1008)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setGantryID(const OFString &value, const OFBool check = OFTrue);

    /** set GantryPitchAngle (300a,014a)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setGantryPitchAngle(const Float32 value, const unsigned long pos = 0);

    /** set GreenPaletteColorLookupTableData (0028,1202)
     *  @param  value  array value to be set (data is copied)
     *  @param  count  number of items in the array
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setGreenPaletteColorLookupTableData(const Uint16 *value, const unsigned long count);

    /** set GreenPaletteColorLookupTableDescriptor (0028,1102)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=3
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setGreenPaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos = 0);

    /** set HighBit (0028,0102)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setHighBit(const Uint16 value, const unsigned long pos = 0);

    /** set ICCProfile (0028,2000)
     *  @param  value  array value to be set (data is copied)
     *  @param  count  number of items in the array
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setICCProfile(const Uint8 *value, const unsigned long count);

    /** set ImageComments (0020,4000)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LT) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setImageComments(const OFString &value, const OFBool check = OFTrue);

    /** set ImagePlanePixelSpacing (3002,0011)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (2) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setImagePlanePixelSpacing(const OFString &value, const OFBool check = OFTrue);

    /** set ImageTriggerDelay (0018,1067)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setImageTriggerDelay(const OFString &value, const OFBool check = OFTrue);

    /** set ImageType (0008,0008)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (2-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setImageType(const OFString &value, const OFBool check = OFTrue);

    /** set ImagesInAcquisition (0020,1002)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setImagesInAcquisition(const OFString &value, const OFBool check = OFTrue);

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

    /** set IrradiationEventUID (0008,3010)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UI) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setIrradiationEventUID(const OFString &value, const OFBool check = OFTrue);

    /** set IsocenterPosition (300a,012c)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (3) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setIsocenterPosition(const OFString &value, const OFBool check = OFTrue);

    /** set IssuerOfPatientID (0010,0021)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setIssuerOfPatientID(const OFString &value, const OFBool check = OFTrue);

    /** set LargestImagePixelValue (0028,0107)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setLargestImagePixelValue(const Uint16 value, const unsigned long pos = 0);

    /** set LongitudinalTemporalInformationModified (0028,0303)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setLongitudinalTemporalInformationModified(const OFString &value, const OFBool check = OFTrue);

    /** set LossyImageCompression (0028,2110)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setLossyImageCompression(const OFString &value, const OFBool check = OFTrue);

    /** set LossyImageCompressionMethod (0028,2114)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setLossyImageCompressionMethod(const OFString &value, const OFBool check = OFTrue);

    /** set LossyImageCompressionRatio (0028,2112)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setLossyImageCompressionRatio(const OFString &value, const OFBool check = OFTrue);

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

    /** set NumberOfFrames (0028,0008)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setNumberOfFrames(const OFString &value, const OFBool check = OFTrue);

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

    /** set PatientOrientation (0020,0020)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (2) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientOrientation(const OFString &value, const OFBool check = OFTrue);

    /** set PatientPosition (0018,5100)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientPosition(const OFString &value, const OFBool check = OFTrue);

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

    /** set PatientSupportAngle (300a,0122)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientSupportAngle(const OFString &value, const OFBool check = OFTrue);

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

    /** set PhotometricInterpretation (0028,0004)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPhotometricInterpretation(const OFString &value, const OFBool check = OFTrue);

    /** set PhysiciansOfRecord (0008,1048)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (PN) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPhysiciansOfRecord(const OFString &value, const OFBool check = OFTrue);

    /** set PixelAspectRatio (0028,0034)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (2) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPixelAspectRatio(const OFString &value, const OFBool check = OFTrue);

    /** set PixelDataProviderURL (0028,7fe0)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UR) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPixelDataProviderURL(const OFString &value, const OFBool check = OFTrue);

    /** set PixelIntensityRelationship (0028,1040)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPixelIntensityRelationship(const OFString &value, const OFBool check = OFTrue);

    /** set PixelIntensityRelationshipSign (0028,1041)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPixelIntensityRelationshipSign(const Sint16 value, const unsigned long pos = 0);

    /** set PixelPaddingRangeLimit (0028,0121)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPixelPaddingRangeLimit(const Uint16 value, const unsigned long pos = 0);

    /** set PixelPaddingValue (0028,0120)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPixelPaddingValue(const Uint16 value, const unsigned long pos = 0);

    /** set PixelRepresentation (0028,0103)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPixelRepresentation(const Uint16 value, const unsigned long pos = 0);

    /** set PlanarConfiguration (0028,0006)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPlanarConfiguration(const Uint16 value, const unsigned long pos = 0);

    /** set PositionReferenceIndicator (0020,1040)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPositionReferenceIndicator(const OFString &value, const OFBool check = OFTrue);

    /** set PreferredPlaybackSequencing (0018,1244)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPreferredPlaybackSequencing(const Uint16 value, const unsigned long pos = 0);

    /** set PresentationLUTShape (2050,0020)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPresentationLUTShape(const OFString &value, const OFBool check = OFTrue);

    /** set PrimaryDosimeterUnit (300a,00b3)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPrimaryDosimeterUnit(const OFString &value, const OFBool check = OFTrue);

    /** set QualityControlImage (0028,0300)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setQualityControlImage(const OFString &value, const OFBool check = OFTrue);

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

    /** set RTImageDescription (3002,0004)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRTImageDescription(const OFString &value, const OFBool check = OFTrue);

    /** set RTImageLabel (3002,0002)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRTImageLabel(const OFString &value, const OFBool check = OFTrue);

    /** set RTImageName (3002,0003)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRTImageName(const OFString &value, const OFBool check = OFTrue);

    /** set RTImageOrientation (3002,0010)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (6) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRTImageOrientation(const OFString &value, const OFBool check = OFTrue);

    /** set RTImagePlane (3002,000c)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRTImagePlane(const OFString &value, const OFBool check = OFTrue);

    /** set RTImagePosition (3002,0012)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (2) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRTImagePosition(const OFString &value, const OFBool check = OFTrue);

    /** set RTImageSID (3002,0026)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRTImageSID(const OFString &value, const OFBool check = OFTrue);

    /** set RadiationMachineName (3002,0020)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRadiationMachineName(const OFString &value, const OFBool check = OFTrue);

    /** set RadiationMachineSAD (3002,0022)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRadiationMachineSAD(const OFString &value, const OFBool check = OFTrue);

    /** set RadiationMachineSSD (3002,0024)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRadiationMachineSSD(const OFString &value, const OFBool check = OFTrue);

    /** set RecognizableVisualFeatures (0028,0302)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRecognizableVisualFeatures(const OFString &value, const OFBool check = OFTrue);

    /** set RecommendedDisplayFrameRate (0008,2144)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRecommendedDisplayFrameRate(const OFString &value, const OFBool check = OFTrue);

    /** set RedPaletteColorLookupTableData (0028,1201)
     *  @param  value  array value to be set (data is copied)
     *  @param  count  number of items in the array
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRedPaletteColorLookupTableData(const Uint16 *value, const unsigned long count);

    /** set RedPaletteColorLookupTableDescriptor (0028,1101)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=3
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRedPaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos = 0);

    /** set ReferencedBeamNumber (300c,0006)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setReferencedBeamNumber(const OFString &value, const OFBool check = OFTrue);

    /** set ReferencedFractionGroupNumber (300c,0022)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setReferencedFractionGroupNumber(const OFString &value, const OFBool check = OFTrue);

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

    /** set ReportedValuesOrigin (3002,000a)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setReportedValuesOrigin(const OFString &value, const OFBool check = OFTrue);

    /** set RescaleIntercept (0028,1052)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRescaleIntercept(const OFString &value, const OFBool check = OFTrue);

    /** set RescaleSlope (0028,1053)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRescaleSlope(const OFString &value, const OFBool check = OFTrue);

    /** set RescaleType (0028,1054)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRescaleType(const OFString &value, const OFBool check = OFTrue);

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

    /** set Rows (0028,0010)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRows(const Uint16 value, const unsigned long pos = 0);

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

    /** set SamplesPerPixel (0028,0002)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSamplesPerPixel(const Uint16 value, const unsigned long pos = 0);

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

    /** set SmallestImagePixelValue (0028,0106)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSmallestImagePixelValue(const Uint16 value, const unsigned long pos = 0);

    /** set SoftwareVersions (0018,1020)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSoftwareVersions(const OFString &value, const OFBool check = OFTrue);

    /** set SourceToReferenceObjectDistance (3002,0028)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSourceToReferenceObjectDistance(const OFString &value, const OFBool check = OFTrue);

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

    /** set StartCumulativeMetersetWeight (300c,0008)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStartCumulativeMetersetWeight(const OFString &value, const OFBool check = OFTrue);

    /** set StartTrim (0008,2142)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStartTrim(const OFString &value, const OFBool check = OFTrue);

    /** set StationName (0008,1010)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStationName(const OFString &value, const OFBool check = OFTrue);

    /** set StereoPairsPresent (0022,0028)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStereoPairsPresent(const OFString &value, const OFBool check = OFTrue);

    /** set StopTrim (0008,2143)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStopTrim(const OFString &value, const OFBool check = OFTrue);

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

    /** set TableTopEccentricAngle (300a,0125)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTableTopEccentricAngle(const OFString &value, const OFBool check = OFTrue);

    /** set TableTopEccentricAxisDistance (300a,0124)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTableTopEccentricAxisDistance(const OFString &value, const OFBool check = OFTrue);

    /** set TableTopLateralPosition (300a,012a)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTableTopLateralPosition(const OFString &value, const OFBool check = OFTrue);

    /** set TableTopLongitudinalPosition (300a,0129)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTableTopLongitudinalPosition(const OFString &value, const OFBool check = OFTrue);

    /** set TableTopPitchAngle (300a,0140)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTableTopPitchAngle(const Float32 value, const unsigned long pos = 0);

    /** set TableTopRollAngle (300a,0144)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTableTopRollAngle(const Float32 value, const unsigned long pos = 0);

    /** set TableTopVerticalPosition (300a,0128)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTableTopVerticalPosition(const OFString &value, const OFBool check = OFTrue);

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

    /** set VOILUTFunction (0028,1056)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setVOILUTFunction(const OFString &value, const OFBool check = OFTrue);

    /** set WindowCenter (0028,1050)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setWindowCenter(const OFString &value, const OFBool check = OFTrue);

    /** set WindowCenterWidthExplanation (0028,1055)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setWindowCenterWidthExplanation(const OFString &value, const OFBool check = OFTrue);

    /** set WindowWidth (0028,1051)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setWindowWidth(const OFString &value, const OFBool check = OFTrue);

    /** set XRayImageReceptorAngle (3002,000e)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setXRayImageReceptorAngle(const OFString &value, const OFBool check = OFTrue);

    /** set XRayImageReceptorTranslation (3002,000d)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (3) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setXRayImageReceptorTranslation(const OFString &value, const OFBool check = OFTrue);


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

    // --- FrameOfReferenceModule (U) ---

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

    // --- GeneralImageModule (M) ---

    /// InstanceNumber (0020,0013) vr=IS, vm=1, type=2
    /// - also defined in: SOPCommonModule
    DcmIntegerString InstanceNumber;
    /// PatientOrientation (0020,0020) vr=CS, vm=2, type=2C
    DcmCodeString PatientOrientation;
    /// ContentDate (0008,0023) vr=DA, vm=1, type=2C
    DcmDate ContentDate;
    /// ContentTime (0008,0033) vr=TM, vm=1, type=2C
    DcmTime ContentTime;
    // ImageType (0008,0008) vr=CS, vm=2-n, type=3
    // - also defined in: RTImageModule
    // DcmCodeString ImageType;
    /// AcquisitionNumber (0020,0012) vr=IS, vm=1, type=3
    DcmIntegerString AcquisitionNumber;
    /// AcquisitionDate (0008,0022) vr=DA, vm=1, type=3
    DcmDate AcquisitionDate;
    /// AcquisitionTime (0008,0032) vr=TM, vm=1, type=3
    DcmTime AcquisitionTime;
    /// AcquisitionDateTime (0008,002a) vr=DT, vm=1, type=3
    DcmDateTime AcquisitionDateTime;
    /// ReferencedImageSequence (0008,1140) vr=SQ, vm=1, type=3
    DRTReferencedImageSequence ReferencedImageSequence;
    /// DerivationDescription (0008,2111) vr=ST, vm=1, type=3
    DcmShortText DerivationDescription;
    /// DerivationCodeSequence (0008,9215) vr=SQ, vm=1, type=3
    DRTDerivationCodeSequence DerivationCodeSequence;
    /// SourceImageSequence (0008,2112) vr=SQ, vm=1, type=3
    DRTSourceImageSequence SourceImageSequence;
    /// ReferencedInstanceSequence (0008,114a) vr=SQ, vm=1, type=3
    DRTReferencedInstanceSequence ReferencedInstanceSequence;
    /// ImagesInAcquisition (0020,1002) vr=IS, vm=1, type=3
    DcmIntegerString ImagesInAcquisition;
    /// ImageComments (0020,4000) vr=LT, vm=1, type=3
    DcmLongText ImageComments;
    /// QualityControlImage (0028,0300) vr=CS, vm=1, type=3
    DcmCodeString QualityControlImage;
    /// BurnedInAnnotation (0028,0301) vr=CS, vm=1, type=3
    DcmCodeString BurnedInAnnotation;
    /// RecognizableVisualFeatures (0028,0302) vr=CS, vm=1, type=3
    DcmCodeString RecognizableVisualFeatures;
    /// LossyImageCompression (0028,2110) vr=CS, vm=1, type=3
    DcmCodeString LossyImageCompression;
    /// LossyImageCompressionRatio (0028,2112) vr=DS, vm=1-n, type=3
    DcmDecimalString LossyImageCompressionRatio;
    /// LossyImageCompressionMethod (0028,2114) vr=CS, vm=1-n, type=3
    DcmCodeString LossyImageCompressionMethod;
    /// IconImageSequence (0088,0200) vr=SQ, vm=1, type=3
    DRTIconImageSequence IconImageSequence;
    /// PresentationLUTShape (2050,0020) vr=CS, vm=1, type=3
    DcmCodeString PresentationLUTShape;
    /// IrradiationEventUID (0008,3010) vr=UI, vm=1-n, type=3
    DcmUniqueIdentifier IrradiationEventUID;
    /// RealWorldValueMappingSequence (0040,9096) vr=SQ, vm=1, type=3
    DRTRealWorldValueMappingSequence RealWorldValueMappingSequence;

    // --- ImagePixelModule (M) ---

    // SamplesPerPixel (0028,0002) vr=US, vm=1, type=1
    // - also defined in: RTImageModule
    // DcmUnsignedShort SamplesPerPixel;
    // PhotometricInterpretation (0028,0004) vr=CS, vm=1, type=1
    // - also defined in: RTImageModule
    // DcmCodeString PhotometricInterpretation;
    /// Rows (0028,0010) vr=US, vm=1, type=1
    DcmUnsignedShort Rows;
    /// Columns (0028,0011) vr=US, vm=1, type=1
    DcmUnsignedShort Columns;
    // BitsAllocated (0028,0100) vr=US, vm=1, type=1
    // - also defined in: RTImageModule
    // DcmUnsignedShort BitsAllocated;
    // BitsStored (0028,0101) vr=US, vm=1, type=1
    // - also defined in: RTImageModule
    // DcmUnsignedShort BitsStored;
    // HighBit (0028,0102) vr=US, vm=1, type=1
    // - also defined in: RTImageModule
    // DcmUnsignedShort HighBit;
    // PixelRepresentation (0028,0103) vr=US, vm=1, type=1
    // - also defined in: RTImageModule
    // DcmUnsignedShort PixelRepresentation;
    /// PixelData (7fe0,0010) vr=OB/OW, vm=1, type=1C
    DcmPixelData PixelData;
    /// PlanarConfiguration (0028,0006) vr=US, vm=1, type=1C
    DcmUnsignedShort PlanarConfiguration;
    /// PixelAspectRatio (0028,0034) vr=IS, vm=2, type=1C
    DcmIntegerString PixelAspectRatio;
    /// SmallestImagePixelValue (0028,0106) vr=US/SS, vm=1, type=3
    DcmUnsignedShort SmallestImagePixelValue;
    /// LargestImagePixelValue (0028,0107) vr=US/SS, vm=1, type=3
    DcmUnsignedShort LargestImagePixelValue;
    /// RedPaletteColorLookupTableDescriptor (0028,1101) vr=US/SS, vm=3, type=1C
    DcmUnsignedShort RedPaletteColorLookupTableDescriptor;
    /// GreenPaletteColorLookupTableDescriptor (0028,1102) vr=US/SS, vm=3, type=1C
    DcmUnsignedShort GreenPaletteColorLookupTableDescriptor;
    /// BluePaletteColorLookupTableDescriptor (0028,1103) vr=US/SS, vm=3, type=1C
    DcmUnsignedShort BluePaletteColorLookupTableDescriptor;
    /// RedPaletteColorLookupTableData (0028,1201) vr=OW, vm=1, type=1C
    DcmOtherByteOtherWord RedPaletteColorLookupTableData;
    /// GreenPaletteColorLookupTableData (0028,1202) vr=OW, vm=1, type=1C
    DcmOtherByteOtherWord GreenPaletteColorLookupTableData;
    /// BluePaletteColorLookupTableData (0028,1203) vr=OW, vm=1, type=1C
    DcmOtherByteOtherWord BluePaletteColorLookupTableData;
    /// ICCProfile (0028,2000) vr=OB, vm=1, type=3
    DcmOtherByteOtherWord ICCProfile;
    /// PixelDataProviderURL (0028,7fe0) vr=UR, vm=1, type=1C
    DcmUniversalResourceIdentifierOrLocator PixelDataProviderURL;
    /// PixelPaddingRangeLimit (0028,0121) vr=US/SS, vm=1, type=1C
    DcmUnsignedShort PixelPaddingRangeLimit;

    // --- ContrastBolusModule (C) ---

    /// ContrastBolusAgent (0018,0010) vr=LO, vm=1, type=2
    DcmLongString ContrastBolusAgent;
    /// ContrastBolusAgentSequence (0018,0012) vr=SQ, vm=1, type=3
    DRTContrastBolusAgentSequence ContrastBolusAgentSequence;
    /// ContrastBolusRoute (0018,1040) vr=LO, vm=1, type=3
    DcmLongString ContrastBolusRoute;
    /// ContrastBolusAdministrationRouteSequence (0018,0014) vr=SQ, vm=1, type=3
    DRTContrastBolusAdministrationRouteSequence ContrastBolusAdministrationRouteSequence;
    /// ContrastBolusVolume (0018,1041) vr=DS, vm=1, type=3
    DcmDecimalString ContrastBolusVolume;
    /// ContrastBolusStartTime (0018,1042) vr=TM, vm=1, type=3
    DcmTime ContrastBolusStartTime;
    /// ContrastBolusStopTime (0018,1043) vr=TM, vm=1, type=3
    DcmTime ContrastBolusStopTime;
    /// ContrastBolusTotalDose (0018,1044) vr=DS, vm=1, type=3
    DcmDecimalString ContrastBolusTotalDose;
    /// ContrastFlowRate (0018,1046) vr=DS, vm=1-n, type=3
    DcmDecimalString ContrastFlowRate;
    /// ContrastFlowDuration (0018,1047) vr=DS, vm=1-n, type=3
    DcmDecimalString ContrastFlowDuration;
    /// ContrastBolusIngredient (0018,1048) vr=CS, vm=1, type=3
    DcmCodeString ContrastBolusIngredient;
    /// ContrastBolusIngredientConcentration (0018,1049) vr=DS, vm=1, type=3
    DcmDecimalString ContrastBolusIngredientConcentration;

    // --- CineModule (C) ---

    /// PreferredPlaybackSequencing (0018,1244) vr=US, vm=1, type=3
    DcmUnsignedShort PreferredPlaybackSequencing;
    /// FrameTime (0018,1063) vr=DS, vm=1, type=1C
    DcmDecimalString FrameTime;
    /// FrameTimeVector (0018,1065) vr=DS, vm=1-n, type=1C
    DcmDecimalString FrameTimeVector;
    /// StartTrim (0008,2142) vr=IS, vm=1, type=3
    DcmIntegerString StartTrim;
    /// StopTrim (0008,2143) vr=IS, vm=1, type=3
    DcmIntegerString StopTrim;
    /// RecommendedDisplayFrameRate (0008,2144) vr=IS, vm=1, type=3
    DcmIntegerString RecommendedDisplayFrameRate;
    /// CineRate (0018,0040) vr=IS, vm=1, type=3
    DcmIntegerString CineRate;
    /// FrameDelay (0018,1066) vr=DS, vm=1, type=3
    DcmDecimalString FrameDelay;
    /// ImageTriggerDelay (0018,1067) vr=DS, vm=1, type=3
    DcmDecimalString ImageTriggerDelay;
    /// EffectiveDuration (0018,0072) vr=DS, vm=1, type=3
    DcmDecimalString EffectiveDuration;
    /// ActualFrameDuration (0018,1242) vr=IS, vm=1, type=3
    DcmIntegerString ActualFrameDuration;
    /// MultiplexedAudioChannelsDescriptionCodeSequence (003a,0300) vr=SQ, vm=1, type=2C
    DRTMultiplexedAudioChannelsDescriptionCodeSequence MultiplexedAudioChannelsDescriptionCodeSequence;

    // --- MultiFrameModule (C) ---

    /// NumberOfFrames (0028,0008) vr=IS, vm=1, type=1
    DcmIntegerString NumberOfFrames;
    /// FrameIncrementPointer (0028,0009) vr=AT, vm=1-n, type=1
    DcmAttributeTag FrameIncrementPointer;
    /// StereoPairsPresent (0022,0028) vr=CS, vm=1, type=3
    DcmCodeString StereoPairsPresent;

    // --- DeviceModule (U) ---

    /// DeviceSequence (0050,0010) vr=SQ, vm=1, type=1
    DRTDeviceSequence DeviceSequence;

    // --- RTImageModule (M) ---

    /// SamplesPerPixel (0028,0002) vr=US, vm=1, type=1
    /// - also defined in: ImagePixelModule
    DcmUnsignedShort SamplesPerPixel;
    /// PhotometricInterpretation (0028,0004) vr=CS, vm=1, type=1
    /// - also defined in: ImagePixelModule
    DcmCodeString PhotometricInterpretation;
    /// BitsAllocated (0028,0100) vr=US, vm=1, type=1
    /// - also defined in: ImagePixelModule
    DcmUnsignedShort BitsAllocated;
    /// BitsStored (0028,0101) vr=US, vm=1, type=1
    /// - also defined in: ImagePixelModule
    DcmUnsignedShort BitsStored;
    /// HighBit (0028,0102) vr=US, vm=1, type=1
    /// - also defined in: ImagePixelModule
    DcmUnsignedShort HighBit;
    /// PixelRepresentation (0028,0103) vr=US, vm=1, type=1
    /// - also defined in: ImagePixelModule
    DcmUnsignedShort PixelRepresentation;
    /// PixelIntensityRelationship (0028,1040) vr=CS, vm=1, type=3
    DcmCodeString PixelIntensityRelationship;
    /// PixelIntensityRelationshipSign (0028,1041) vr=SS, vm=1, type=1C
    DcmSignedShort PixelIntensityRelationshipSign;
    /// RTImageLabel (3002,0002) vr=SH, vm=1, type=1
    DcmShortString RTImageLabel;
    /// RTImageName (3002,0003) vr=LO, vm=1, type=3
    DcmLongString RTImageName;
    /// RTImageDescription (3002,0004) vr=ST, vm=1, type=3
    DcmShortText RTImageDescription;
    /// ImageType (0008,0008) vr=CS, vm=2-n, type=1
    /// - also defined in: GeneralImageModule
    DcmCodeString ImageType;
    /// ConversionType (0008,0064) vr=CS, vm=1, type=2
    DcmCodeString ConversionType;
    /// ReportedValuesOrigin (3002,000a) vr=CS, vm=1, type=2C
    DcmCodeString ReportedValuesOrigin;
    /// RTImagePlane (3002,000c) vr=CS, vm=1, type=1
    DcmCodeString RTImagePlane;
    /// XRayImageReceptorTranslation (3002,000d) vr=DS, vm=3, type=3
    DcmDecimalString XRayImageReceptorTranslation;
    /// XRayImageReceptorAngle (3002,000e) vr=DS, vm=1, type=2
    DcmDecimalString XRayImageReceptorAngle;
    /// RTImageOrientation (3002,0010) vr=DS, vm=6, type=2C
    DcmDecimalString RTImageOrientation;
    /// ImagePlanePixelSpacing (3002,0011) vr=DS, vm=2, type=2
    DcmDecimalString ImagePlanePixelSpacing;
    /// RTImagePosition (3002,0012) vr=DS, vm=2, type=2
    DcmDecimalString RTImagePosition;
    /// RadiationMachineName (3002,0020) vr=SH, vm=1, type=2
    DcmShortString RadiationMachineName;
    /// PrimaryDosimeterUnit (300a,00b3) vr=CS, vm=1, type=2
    DcmCodeString PrimaryDosimeterUnit;
    /// RadiationMachineSAD (3002,0022) vr=DS, vm=1, type=2
    DcmDecimalString RadiationMachineSAD;
    /// RadiationMachineSSD (3002,0024) vr=DS, vm=1, type=3
    DcmDecimalString RadiationMachineSSD;
    /// RTImageSID (3002,0026) vr=DS, vm=1, type=2
    DcmDecimalString RTImageSID;
    /// SourceToReferenceObjectDistance (3002,0028) vr=DS, vm=1, type=3
    DcmDecimalString SourceToReferenceObjectDistance;
    /// ReferencedRTPlanSequence (300c,0002) vr=SQ, vm=1, type=3
    DRTReferencedRTPlanSequenceInRTImageModule ReferencedRTPlanSequence;
    /// ReferencedBeamNumber (300c,0006) vr=IS, vm=1, type=3
    DcmIntegerString ReferencedBeamNumber;
    /// ReferencedFractionGroupNumber (300c,0022) vr=IS, vm=1, type=3
    DcmIntegerString ReferencedFractionGroupNumber;
    /// FractionNumber (3002,0029) vr=IS, vm=1, type=3
    DcmIntegerString FractionNumber;
    /// StartCumulativeMetersetWeight (300c,0008) vr=DS, vm=1, type=3
    DcmDecimalString StartCumulativeMetersetWeight;
    /// EndCumulativeMetersetWeight (300c,0009) vr=DS, vm=1, type=3
    DcmDecimalString EndCumulativeMetersetWeight;
    /// ExposureSequence (3002,0030) vr=SQ, vm=1, type=3
    DRTExposureSequence ExposureSequence;
    /// FluenceMapSequence (3002,0040) vr=SQ, vm=1, type=1C
    DRTFluenceMapSequence FluenceMapSequence;
    /// GantryAngle (300a,011e) vr=DS, vm=1, type=3
    DcmDecimalString GantryAngle;
    /// GantryPitchAngle (300a,014a) vr=FL, vm=1, type=3
    DcmFloatingPointSingle GantryPitchAngle;
    /// BeamLimitingDeviceAngle (300a,0120) vr=DS, vm=1, type=3
    DcmDecimalString BeamLimitingDeviceAngle;
    /// PatientSupportAngle (300a,0122) vr=DS, vm=1, type=3
    DcmDecimalString PatientSupportAngle;
    /// TableTopEccentricAxisDistance (300a,0124) vr=DS, vm=1, type=3
    DcmDecimalString TableTopEccentricAxisDistance;
    /// TableTopEccentricAngle (300a,0125) vr=DS, vm=1, type=3
    DcmDecimalString TableTopEccentricAngle;
    /// TableTopPitchAngle (300a,0140) vr=FL, vm=1, type=3
    DcmFloatingPointSingle TableTopPitchAngle;
    /// TableTopRollAngle (300a,0144) vr=FL, vm=1, type=3
    DcmFloatingPointSingle TableTopRollAngle;
    /// TableTopVerticalPosition (300a,0128) vr=DS, vm=1, type=3
    DcmDecimalString TableTopVerticalPosition;
    /// TableTopLongitudinalPosition (300a,0129) vr=DS, vm=1, type=3
    DcmDecimalString TableTopLongitudinalPosition;
    /// TableTopLateralPosition (300a,012a) vr=DS, vm=1, type=3
    DcmDecimalString TableTopLateralPosition;
    /// IsocenterPosition (300a,012c) vr=DS, vm=3, type=3
    DcmDecimalString IsocenterPosition;
    /// PatientPosition (0018,5100) vr=CS, vm=1, type=1C
    DcmCodeString PatientPosition;

    // --- ModalityLUTModule (U) ---

    /// ModalityLUTSequence (0028,3000) vr=SQ, vm=1, type=1C
    DRTModalityLUTSequence ModalityLUTSequence;
    /// RescaleIntercept (0028,1052) vr=DS, vm=1, type=1C
    DcmDecimalString RescaleIntercept;
    /// RescaleSlope (0028,1053) vr=DS, vm=1, type=1C
    DcmDecimalString RescaleSlope;
    /// RescaleType (0028,1054) vr=LO, vm=1, type=1C
    DcmLongString RescaleType;

    // --- VOILUTModule (U) ---

    /// VOILUTSequence (0028,3010) vr=SQ, vm=1, type=1C
    DRTVOILUTSequence VOILUTSequence;
    /// WindowCenter (0028,1050) vr=DS, vm=1-n, type=1C
    DcmDecimalString WindowCenter;
    /// WindowWidth (0028,1051) vr=DS, vm=1-n, type=1C
    DcmDecimalString WindowWidth;
    /// WindowCenterWidthExplanation (0028,1055) vr=LO, vm=1-n, type=3
    DcmLongString WindowCenterWidthExplanation;
    /// VOILUTFunction (0028,1056) vr=CS, vm=1, type=3
    DcmCodeString VOILUTFunction;

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
    // - also defined in: GeneralImageModule
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

    // --- FrameExtractionModule (C) ---

    /// FrameExtractionSequence (0008,1164) vr=SQ, vm=1, type=1
    DRTFrameExtractionSequence FrameExtractionSequence;

};


#endif
