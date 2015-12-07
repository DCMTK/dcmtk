/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTDoseIOD
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTDOSE_H
#define DRTDOSE_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtadcs.h"   // for AdmittingDiagnosesCodeSequence
#include "dcmtk/dcmrt/seq/drtbrs.h"    // for BreedRegistrationSequence
#include "dcmtk/dcmrt/seq/drtcsis.h"   // for CodingSchemeIdentificationSequence
#include "dcmtk/dcmrt/seq/drtcctus.h"  // for ConsentForClinicalTrialUseSequence
#include "dcmtk/dcmrt/seq/drtcpis.h"   // for ConsultingPhysicianIdentificationSequence
#include "dcmtk/dcmrt/seq/drtces.h"    // for ContributingEquipmentSequence
#include "dcmtk/dcmrt/seq/drtcsas.h"   // for ConversionSourceAttributesSequence
#include "dcmtk/dcmrt/seq/drtdvhs.h"   // for DVHSequence
#include "dcmtk/dcmrt/seq/drtdimcs.h"  // for DeidentificationMethodCodeSequence
#include "dcmtk/dcmrt/seq/drtdcs.h"    // for DerivationCodeSequence
#include "dcmtk/dcmrt/seq/drtdss.h"    // for DigitalSignaturesSequence
#include "dcmtk/dcmrt/seq/drteas.h"    // for EncryptedAttributesSequence
#include "dcmtk/dcmrt/seq/drtfes.h"    // for FrameExtractionSequence
#include "dcmtk/dcmrt/seq/drthsdrs.h"  // for HL7StructuredDocumentReferenceSequence
#include "dcmtk/dcmrt/seq/drtiis.h"    // for IconImageSequence
#include "dcmtk/dcmrt/seq/drtians.h"   // for IssuerOfAccessionNumberSequence
#include "dcmtk/dcmrt/seq/drtiais.h"   // for IssuerOfAdmissionIDSequence
#include "dcmtk/dcmrt/seq/drtipiqs.h"  // for IssuerOfPatientIDQualifiersSequence
#include "dcmtk/dcmrt/seq/drtiseis.h"  // for IssuerOfServiceEpisodeIDSequence
#include "dcmtk/dcmrt/seq/drtmps.h"    // for MACParametersSequence
#include "dcmtk/dcmrt/seq/drtmls.h"    // for ModalityLUTSequence
#include "dcmtk/dcmrt/seq/drtoas.h"    // for OriginalAttributesSequence
#include "dcmtk/dcmrt/seq/drtopis.h"   // for OtherPatientIDsSequence
#include "dcmtk/dcmrt/seq/drtpbcs.h"   // for PatientBreedCodeSequence
#include "dcmtk/dcmrt/seq/drtpsics.h"  // for PatientSizeCodeSequence
#include "dcmtk/dcmrt/seq/drtpscs.h"   // for PatientSpeciesCodeSequence
#include "dcmtk/dcmrt/seq/drtppcs.h"   // for PerformedProtocolCodeSequence
#include "dcmtk/dcmrt/seq/drtporis.h"  // for PhysiciansOfRecordIdentificationSequence
#include "dcmtk/dcmrt/seq/drtprsis.h"  // for PhysiciansReadingStudyIdentificationSequence
#include "dcmtk/dcmrt/seq/drtpsss.h"   // for PredecessorStructureSetSequence
#include "dcmtk/dcmrt/seq/drtpdecs.h"  // for PrivateDataElementCharacteristicsSequence
#include "dcmtk/dcmrt/seq/drtpcs.h"    // for ProcedureCodeSequence
#include "dcmtk/dcmrt/seq/drtrcs.h"    // for ROIContourSequence
#include "dcmtk/dcmrt/seq/drtrdros.h"  // for RTDoseROISequence
#include "dcmtk/dcmrt/seq/drtrwvms.h"  // for RealWorldValueMappingSequence
#include "dcmtk/dcmrt/seq/drtrppcs.h"  // for ReasonForPerformedProcedureCodeSequence
#include "dcmtk/dcmrt/seq/drtrfors.h"  // for ReferencedFrameOfReferenceSequence
#include "dcmtk/dcmrt/seq/drtrims.h"   // for ReferencedImageSequence
#include "dcmtk/dcmrt/seq/drtris.h"    // for ReferencedInstanceSequence
#include "dcmtk/dcmrt/seq/drtrpphs.h"  // for ReferencedPatientPhotoSequence
#include "dcmtk/dcmrt/seq/drtrps.h"    // for ReferencedPatientSequence
#include "dcmtk/dcmrt/seq/drtrppss.h"  // for ReferencedPerformedProcedureStepSequence
#include "dcmtk/dcmrt/seq/drtrrtps.h"  // for ReferencedRTPlanSequence
#include "dcmtk/dcmrt/seq/drtrsers.h"  // for ReferencedSeriesSequence
#include "dcmtk/dcmrt/seq/drtrsrs.h"   // for ReferencedSpatialRegistrationSequence
#include "dcmtk/dcmrt/seq/drtrsss.h"   // for ReferencedStructureSetSequence
#include "dcmtk/dcmrt/seq/drtrss.h"    // for ReferencedStudySequence
#include "dcmtk/dcmrt/seq/drtrtrs2.h"  // for ReferencedTreatmentRecordSequence
#include "dcmtk/dcmrt/seq/drtrpis.h"   // for ReferringPhysicianIdentificationSequence
#include "dcmtk/dcmrt/seq/drtras.h"    // for RequestAttributesSequence
#include "dcmtk/dcmrt/seq/drtrscs.h"   // for RequestingServiceCodeSequence
#include "dcmtk/dcmrt/seq/drtsdcs.h"   // for SeriesDescriptionCodeSequence
#include "dcmtk/dcmrt/seq/drtsis.h"    // for SourceImageSequence
#include "dcmtk/dcmrt/seq/drtssrs.h"   // for StructureSetROISequence
#include "dcmtk/dcmrt/seq/drtscris.h"  // for StudiesContainingOtherReferencedInstancesSequence


/** Interface class for RTDoseIOD
 */
class DCMTK_DCMRT_EXPORT DRTDoseIOD
  : protected DRTTypes
{

  public:

  // --- constructors, destructor and operators ---

    /** default constructor
     */
    DRTDoseIOD();

    /** copy constructor
     *  @param copy IOD object to be copied
     */
    DRTDoseIOD(const DRTDoseIOD &copy);

    /** destructor
     */
    virtual ~DRTDoseIOD();

    /** assigment operator
     *  @param copy IOD object to be copied
     */
    DRTDoseIOD &operator=(const DRTDoseIOD &copy);

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

    /** check whether GeneralImageModule (C) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isGeneralImageModulePresent(const OFBool complete = OFFalse);

    /** check whether ImagePlaneModule (C) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isImagePlaneModulePresent(const OFBool complete = OFFalse);

    /** check whether ImagePixelModule (C) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isImagePixelModulePresent(const OFBool complete = OFFalse);

    /** check whether MultiFrameModule (C) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isMultiFrameModulePresent(const OFBool complete = OFFalse);

    /** check whether OverlayPlaneModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isOverlayPlaneModulePresent(const OFBool complete = OFFalse);

    /** check whether MultiFrameOverlayModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isMultiFrameOverlayModulePresent(const OFBool complete = OFFalse);

    /** check whether ModalityLUTModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isModalityLUTModulePresent(const OFBool complete = OFFalse);

    /** check whether RTDVHModule (U) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isRTDVHModulePresent(const OFBool complete = OFFalse);

    /** check whether StructureSetModule (C) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isStructureSetModulePresent(const OFBool complete = OFFalse);

    /** check whether ROIContourModule (C) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isROIContourModulePresent(const OFBool complete = OFFalse);

    /** check whether RTDoseROIModule (C) is present.
     *  By default, a module is reported as being present if at least one attribute
     *  from this module has a non-empty value.
     *  @param  complete  check for all mandatory attributes of this module if enabled
     *  @return OFTrue if module is present, OFFalse otherwise
     */
    virtual OFBool isRTDoseROIModulePresent(const OFBool complete = OFFalse);

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

    /** get AuthorizationEquipmentCertificationNumber (0100,0426)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getAuthorizationEquipmentCertificationNumber(OFString &value, const signed long pos = 0) const;

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

    /** get DVHNormalizationDoseValue (3004,0042)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDVHNormalizationDoseValue(OFString &value, const signed long pos = 0) const;

    /** get DVHNormalizationDoseValue (3004,0042)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDVHNormalizationDoseValue(Float64 &value, const unsigned long pos = 0) const;

    /** get DVHNormalizationPoint (3004,0040)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDVHNormalizationPoint(OFString &value, const signed long pos = 0) const;

    /** get DVHNormalizationPoint (3004,0040)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDVHNormalizationPoint(Float64 &value, const unsigned long pos = 0) const;

    /** get DVHNormalizationPoint (3004,0040)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDVHNormalizationPoint(OFVector<Float64> &value) const;

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

    /** get DoseComment (3004,0006)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDoseComment(OFString &value, const signed long pos = 0) const;

    /** get DoseGridScaling (3004,000e)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDoseGridScaling(OFString &value, const signed long pos = 0) const;

    /** get DoseGridScaling (3004,000e)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDoseGridScaling(Float64 &value, const unsigned long pos = 0) const;

    /** get DoseSummationType (3004,000a)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDoseSummationType(OFString &value, const signed long pos = 0) const;

    /** get DoseType (3004,0004)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDoseType(OFString &value, const signed long pos = 0) const;

    /** get DoseUnits (3004,0002)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDoseUnits(OFString &value, const signed long pos = 0) const;

    /** get EthnicGroup (0010,2160)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getEthnicGroup(OFString &value, const signed long pos = 0) const;

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

    /** get GantryID (0018,1008)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getGantryID(OFString &value, const signed long pos = 0) const;

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

    /** get GridFrameOffsetVector (3004,000c)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getGridFrameOffsetVector(OFString &value, const signed long pos = 0) const;

    /** get GridFrameOffsetVector (3004,000c)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getGridFrameOffsetVector(Float64 &value, const unsigned long pos = 0) const;

    /** get GridFrameOffsetVector (3004,000c)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getGridFrameOffsetVector(OFVector<Float64> &value) const;

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

    /** get ImageFrameOrigin (60xx,0051)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImageFrameOrigin(Uint16 &value, const unsigned long pos = 0) const;

    /** get ImageOrientationPatient (0020,0037)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImageOrientationPatient(OFString &value, const signed long pos = 0) const;

    /** get ImageOrientationPatient (0020,0037)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImageOrientationPatient(Float64 &value, const unsigned long pos = 0) const;

    /** get ImageOrientationPatient (0020,0037)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImageOrientationPatient(OFVector<Float64> &value) const;

    /** get ImagePositionPatient (0020,0032)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImagePositionPatient(OFString &value, const signed long pos = 0) const;

    /** get ImagePositionPatient (0020,0032)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImagePositionPatient(Float64 &value, const unsigned long pos = 0) const;

    /** get ImagePositionPatient (0020,0032)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getImagePositionPatient(OFVector<Float64> &value) const;

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

    /** get NormalizationPoint (3004,0008)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getNormalizationPoint(OFString &value, const signed long pos = 0) const;

    /** get NormalizationPoint (3004,0008)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getNormalizationPoint(Float64 &value, const unsigned long pos = 0) const;

    /** get NormalizationPoint (3004,0008)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getNormalizationPoint(OFVector<Float64> &value) const;

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

    /** get NumberOfFramesInOverlay (60xx,0015)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getNumberOfFramesInOverlay(OFString &value, const signed long pos = 0) const;

    /** get NumberOfFramesInOverlay (60xx,0015)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getNumberOfFramesInOverlay(Sint32 &value, const unsigned long pos = 0) const;

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

    /** get OverlayBitPosition (60xx,0102)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOverlayBitPosition(Uint16 &value, const unsigned long pos = 0) const;

    /** get OverlayBitsAllocated (60xx,0100)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOverlayBitsAllocated(Uint16 &value, const unsigned long pos = 0) const;

    /** get OverlayColumns (60xx,0011)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOverlayColumns(Uint16 &value, const unsigned long pos = 0) const;

    /** get OverlayDescription (60xx,0022)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOverlayDescription(OFString &value, const signed long pos = 0) const;

    /** get OverlayLabel (60xx,1500)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOverlayLabel(OFString &value, const signed long pos = 0) const;

    /** get OverlayOrigin (60xx,0050)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOverlayOrigin(Sint16 &value, const unsigned long pos = 0) const;

    /** get OverlayRows (60xx,0010)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOverlayRows(Uint16 &value, const unsigned long pos = 0) const;

    /** get OverlaySubtype (60xx,0045)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOverlaySubtype(OFString &value, const signed long pos = 0) const;

    /** get OverlayType (60xx,0040)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOverlayType(OFString &value, const signed long pos = 0) const;

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

    /** get PixelSpacing (0028,0030)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelSpacing(OFString &value, const signed long pos = 0) const;

    /** get PixelSpacing (0028,0030)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelSpacing(Float64 &value, const unsigned long pos = 0) const;

    /** get PixelSpacing (0028,0030)
     *  @param  value  reference to variable in which the value(s) should be stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPixelSpacing(OFVector<Float64> &value) const;

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

    /** get PresentationLUTShape (2050,0020)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPresentationLUTShape(OFString &value, const signed long pos = 0) const;

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

    /** get ROIArea (60xx,1301)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getROIArea(OFString &value, const signed long pos = 0) const;

    /** get ROIArea (60xx,1301)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getROIArea(Sint32 &value, const unsigned long pos = 0) const;

    /** get ROIMean (60xx,1302)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getROIMean(OFString &value, const signed long pos = 0) const;

    /** get ROIMean (60xx,1302)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getROIMean(Float64 &value, const unsigned long pos = 0) const;

    /** get ROIStandardDeviation (60xx,1303)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getROIStandardDeviation(OFString &value, const signed long pos = 0) const;

    /** get ROIStandardDeviation (60xx,1303)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getROIStandardDeviation(Float64 &value, const unsigned long pos = 0) const;

    /** get RecognizableVisualFeatures (0028,0302)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRecognizableVisualFeatures(OFString &value, const signed long pos = 0) const;

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

    /** get SliceLocation (0020,1041)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSliceLocation(OFString &value, const signed long pos = 0) const;

    /** get SliceLocation (0020,1041)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSliceLocation(Float64 &value, const unsigned long pos = 0) const;

    /** get SliceThickness (0018,0050)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSliceThickness(OFString &value, const signed long pos = 0) const;

    /** get SliceThickness (0018,0050)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSliceThickness(Float64 &value, const unsigned long pos = 0) const;

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

    /** get SpatialTransformOfDose (3004,0005)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSpatialTransformOfDose(OFString &value, const signed long pos = 0) const;

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

    /** get StereoPairsPresent (0022,0028)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStereoPairsPresent(OFString &value, const signed long pos = 0) const;

    /** get StructureSetDate (3006,0008)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStructureSetDate(OFString &value, const signed long pos = 0) const;

    /** get StructureSetDescription (3006,0006)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStructureSetDescription(OFString &value, const signed long pos = 0) const;

    /** get StructureSetLabel (3006,0002)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStructureSetLabel(OFString &value, const signed long pos = 0) const;

    /** get StructureSetName (3006,0004)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStructureSetName(OFString &value, const signed long pos = 0) const;

    /** get StructureSetTime (3006,0009)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStructureSetTime(OFString &value, const signed long pos = 0) const;

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

    /** get TissueHeterogeneityCorrection (3004,0014)
     *  @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTissueHeterogeneityCorrection(OFString &value, const signed long pos = 0) const;

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

    /** get DVHSequence (3004,0050)
     *  @return reference to sequence element
     */
    DRTDVHSequence &getDVHSequence()
        { return DVHSequence; }

    /** get DVHSequence (3004,0050)
     *  @return const reference to sequence element
     */
    const DRTDVHSequence &getDVHSequence() const
        { return DVHSequence; }

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

    /** get PredecessorStructureSetSequence (3006,0018)
     *  @return reference to sequence element
     */
    DRTPredecessorStructureSetSequence &getPredecessorStructureSetSequence()
        { return PredecessorStructureSetSequence; }

    /** get PredecessorStructureSetSequence (3006,0018)
     *  @return const reference to sequence element
     */
    const DRTPredecessorStructureSetSequence &getPredecessorStructureSetSequence() const
        { return PredecessorStructureSetSequence; }

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

    /** get ROIContourSequence (3006,0039)
     *  @return reference to sequence element
     */
    DRTROIContourSequence &getROIContourSequence()
        { return ROIContourSequence; }

    /** get ROIContourSequence (3006,0039)
     *  @return const reference to sequence element
     */
    const DRTROIContourSequence &getROIContourSequence() const
        { return ROIContourSequence; }

    /** get RTDoseROISequence (3004,0010)
     *  @return reference to sequence element
     */
    DRTRTDoseROISequence &getRTDoseROISequence()
        { return RTDoseROISequence; }

    /** get RTDoseROISequence (3004,0010)
     *  @return const reference to sequence element
     */
    const DRTRTDoseROISequence &getRTDoseROISequence() const
        { return RTDoseROISequence; }

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

    /** get ReferencedFrameOfReferenceSequence (3006,0010)
     *  @return reference to sequence element
     */
    DRTReferencedFrameOfReferenceSequence &getReferencedFrameOfReferenceSequence()
        { return ReferencedFrameOfReferenceSequence; }

    /** get ReferencedFrameOfReferenceSequence (3006,0010)
     *  @return const reference to sequence element
     */
    const DRTReferencedFrameOfReferenceSequence &getReferencedFrameOfReferenceSequence() const
        { return ReferencedFrameOfReferenceSequence; }

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
    DRTReferencedRTPlanSequence &getReferencedRTPlanSequence()
        { return ReferencedRTPlanSequence; }

    /** get ReferencedRTPlanSequence (300c,0002)
     *  @return const reference to sequence element
     */
    const DRTReferencedRTPlanSequence &getReferencedRTPlanSequence() const
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

    /** get ReferencedSpatialRegistrationSequence (0070,0404)
     *  @return reference to sequence element
     */
    DRTReferencedSpatialRegistrationSequence &getReferencedSpatialRegistrationSequence()
        { return ReferencedSpatialRegistrationSequence; }

    /** get ReferencedSpatialRegistrationSequence (0070,0404)
     *  @return const reference to sequence element
     */
    const DRTReferencedSpatialRegistrationSequence &getReferencedSpatialRegistrationSequence() const
        { return ReferencedSpatialRegistrationSequence; }

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

    /** get ReferencedTreatmentRecordSequence (3008,0030)
     *  @return reference to sequence element
     */
    DRTReferencedTreatmentRecordSequenceInRTDoseModule &getReferencedTreatmentRecordSequence()
        { return ReferencedTreatmentRecordSequence; }

    /** get ReferencedTreatmentRecordSequence (3008,0030)
     *  @return const reference to sequence element
     */
    const DRTReferencedTreatmentRecordSequenceInRTDoseModule &getReferencedTreatmentRecordSequence() const
        { return ReferencedTreatmentRecordSequence; }

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

    /** get StructureSetROISequence (3006,0020)
     *  @return reference to sequence element
     */
    DRTStructureSetROISequence &getStructureSetROISequence()
        { return StructureSetROISequence; }

    /** get StructureSetROISequence (3006,0020)
     *  @return const reference to sequence element
     */
    const DRTStructureSetROISequence &getStructureSetROISequence() const
        { return StructureSetROISequence; }

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

    /** set AuthorizationEquipmentCertificationNumber (0100,0426)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setAuthorizationEquipmentCertificationNumber(const OFString &value, const OFBool check = OFTrue);

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

    /** set DVHNormalizationDoseValue (3004,0042)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDVHNormalizationDoseValue(const OFString &value, const OFBool check = OFTrue);

    /** set DVHNormalizationPoint (3004,0040)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (3) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDVHNormalizationPoint(const OFString &value, const OFBool check = OFTrue);

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

    /** set DoseComment (3004,0006)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDoseComment(const OFString &value, const OFBool check = OFTrue);

    /** set DoseGridScaling (3004,000e)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDoseGridScaling(const OFString &value, const OFBool check = OFTrue);

    /** set DoseSummationType (3004,000a)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDoseSummationType(const OFString &value, const OFBool check = OFTrue);

    /** set DoseType (3004,0004)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDoseType(const OFString &value, const OFBool check = OFTrue);

    /** set DoseUnits (3004,0002)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDoseUnits(const OFString &value, const OFBool check = OFTrue);

    /** set EthnicGroup (0010,2160)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setEthnicGroup(const OFString &value, const OFBool check = OFTrue);

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

    /** set GantryID (0018,1008)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setGantryID(const OFString &value, const OFBool check = OFTrue);

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

    /** set GridFrameOffsetVector (3004,000c)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (2-n) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setGridFrameOffsetVector(const OFString &value, const OFBool check = OFTrue);

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

    /** set ImageFrameOrigin (60xx,0051)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setImageFrameOrigin(const Uint16 value, const unsigned long pos = 0);

    /** set ImageOrientationPatient (0020,0037)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (6) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setImageOrientationPatient(const OFString &value, const OFBool check = OFTrue);

    /** set ImagePositionPatient (0020,0032)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (3) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setImagePositionPatient(const OFString &value, const OFBool check = OFTrue);

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

    /** set NormalizationPoint (3004,0008)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (3) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setNormalizationPoint(const OFString &value, const OFBool check = OFTrue);

    /** set NumberOfFrames (0028,0008)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setNumberOfFrames(const OFString &value, const OFBool check = OFTrue);

    /** set NumberOfFramesInOverlay (60xx,0015)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setNumberOfFramesInOverlay(const OFString &value, const OFBool check = OFTrue);

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

    /** set OverlayBitPosition (60xx,0102)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setOverlayBitPosition(const Uint16 value, const unsigned long pos = 0);

    /** set OverlayBitsAllocated (60xx,0100)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setOverlayBitsAllocated(const Uint16 value, const unsigned long pos = 0);

    /** set OverlayColumns (60xx,0011)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setOverlayColumns(const Uint16 value, const unsigned long pos = 0);

    /** set OverlayDescription (60xx,0022)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setOverlayDescription(const OFString &value, const OFBool check = OFTrue);

    /** set OverlayLabel (60xx,1500)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setOverlayLabel(const OFString &value, const OFBool check = OFTrue);

    /** set OverlayOrigin (60xx,0050)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=2
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setOverlayOrigin(const Sint16 value, const unsigned long pos = 0);

    /** set OverlayRows (60xx,0010)
     *  @param  value  value to be set (should be valid for this VR)
     *  @param  pos    index of the value to be set (0..vm-1), vm=1
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setOverlayRows(const Uint16 value, const unsigned long pos = 0);

    /** set OverlaySubtype (60xx,0045)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setOverlaySubtype(const OFString &value, const OFBool check = OFTrue);

    /** set OverlayType (60xx,0040)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setOverlayType(const OFString &value, const OFBool check = OFTrue);

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

    /** set PixelSpacing (0028,0030)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (2) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPixelSpacing(const OFString &value, const OFBool check = OFTrue);

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

    /** set PresentationLUTShape (2050,0020)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPresentationLUTShape(const OFString &value, const OFBool check = OFTrue);

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

    /** set ROIArea (60xx,1301)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setROIArea(const OFString &value, const OFBool check = OFTrue);

    /** set ROIMean (60xx,1302)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setROIMean(const OFString &value, const OFBool check = OFTrue);

    /** set ROIStandardDeviation (60xx,1303)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setROIStandardDeviation(const OFString &value, const OFBool check = OFTrue);

    /** set RecognizableVisualFeatures (0028,0302)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setRecognizableVisualFeatures(const OFString &value, const OFBool check = OFTrue);

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

    /** set SliceLocation (0020,1041)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSliceLocation(const OFString &value, const OFBool check = OFTrue);

    /** set SliceThickness (0018,0050)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSliceThickness(const OFString &value, const OFBool check = OFTrue);

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

    /** set SpatialResolution (0018,1050)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSpatialResolution(const OFString &value, const OFBool check = OFTrue);

    /** set SpatialTransformOfDose (3004,0005)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSpatialTransformOfDose(const OFString &value, const OFBool check = OFTrue);

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

    /** set StereoPairsPresent (0022,0028)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStereoPairsPresent(const OFString &value, const OFBool check = OFTrue);

    /** set StructureSetDate (3006,0008)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStructureSetDate(const OFString &value, const OFBool check = OFTrue);

    /** set StructureSetDescription (3006,0006)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStructureSetDescription(const OFString &value, const OFBool check = OFTrue);

    /** set StructureSetLabel (3006,0002)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStructureSetLabel(const OFString &value, const OFBool check = OFTrue);

    /** set StructureSetName (3006,0004)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStructureSetName(const OFString &value, const OFBool check = OFTrue);

    /** set StructureSetTime (3006,0009)
     *  @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStructureSetTime(const OFString &value, const OFBool check = OFTrue);

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

    /** set TissueHeterogeneityCorrection (3004,0014)
     *  @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1-3) if enabled
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTissueHeterogeneityCorrection(const OFString &value, const OFBool check = OFTrue);

  // --- all other attributes (without get/set methods) ---

    // - OverlayData (60xx,3000)


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

    // --- GeneralImageModule (C) ---

    // InstanceNumber (0020,0013) vr=IS, vm=1, type=2
    // - also defined in: RTDoseModule, StructureSetModule, SOPCommonModule
    // DcmIntegerString InstanceNumber;
    /// PatientOrientation (0020,0020) vr=CS, vm=2, type=2C
    DcmCodeString PatientOrientation;
    /// ContentDate (0008,0023) vr=DA, vm=1, type=2C
    DcmDate ContentDate;
    /// ContentTime (0008,0033) vr=TM, vm=1, type=2C
    DcmTime ContentTime;
    /// ImageType (0008,0008) vr=CS, vm=2-n, type=3
    DcmCodeString ImageType;
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
    // DerivationCodeSequence (0008,9215) vr=SQ, vm=1, type=3
    // - also defined in: RTDoseModule
    // DRTDerivationCodeSequence DerivationCodeSequence;
    /// SourceImageSequence (0008,2112) vr=SQ, vm=1, type=3
    DRTSourceImageSequence SourceImageSequence;
    // ReferencedInstanceSequence (0008,114a) vr=SQ, vm=1, type=3
    // - also defined in: RTDoseModule
    // DRTReferencedInstanceSequence ReferencedInstanceSequence;
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

    // --- ImagePlaneModule (C) ---

    /// PixelSpacing (0028,0030) vr=DS, vm=2, type=1
    DcmDecimalString PixelSpacing;
    /// ImageOrientationPatient (0020,0037) vr=DS, vm=6, type=1
    DcmDecimalString ImageOrientationPatient;
    /// ImagePositionPatient (0020,0032) vr=DS, vm=3, type=1
    DcmDecimalString ImagePositionPatient;
    /// SliceThickness (0018,0050) vr=DS, vm=1, type=2
    DcmDecimalString SliceThickness;
    /// SliceLocation (0020,1041) vr=DS, vm=1, type=3
    DcmDecimalString SliceLocation;

    // --- ImagePixelModule (C) ---

    // SamplesPerPixel (0028,0002) vr=US, vm=1, type=1
    // - also defined in: RTDoseModule
    // DcmUnsignedShort SamplesPerPixel;
    // PhotometricInterpretation (0028,0004) vr=CS, vm=1, type=1
    // - also defined in: RTDoseModule
    // DcmCodeString PhotometricInterpretation;
    /// Rows (0028,0010) vr=US, vm=1, type=1
    DcmUnsignedShort Rows;
    /// Columns (0028,0011) vr=US, vm=1, type=1
    DcmUnsignedShort Columns;
    // BitsAllocated (0028,0100) vr=US, vm=1, type=1
    // - also defined in: RTDoseModule
    // DcmUnsignedShort BitsAllocated;
    // BitsStored (0028,0101) vr=US, vm=1, type=1
    // - also defined in: RTDoseModule
    // DcmUnsignedShort BitsStored;
    // HighBit (0028,0102) vr=US, vm=1, type=1
    // - also defined in: RTDoseModule
    // DcmUnsignedShort HighBit;
    // PixelRepresentation (0028,0103) vr=US, vm=1, type=1
    // - also defined in: RTDoseModule
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

    // --- MultiFrameModule (C) ---

    /// NumberOfFrames (0028,0008) vr=IS, vm=1, type=1
    DcmIntegerString NumberOfFrames;
    /// FrameIncrementPointer (0028,0009) vr=AT, vm=1-n, type=1
    DcmAttributeTag FrameIncrementPointer;
    /// StereoPairsPresent (0022,0028) vr=CS, vm=1, type=3
    DcmCodeString StereoPairsPresent;

    // --- OverlayPlaneModule (U) ---

    /// OverlayRows (60xx,0010) vr=US, vm=1, type=1
    DcmUnsignedShort OverlayRows;
    /// OverlayColumns (60xx,0011) vr=US, vm=1, type=1
    DcmUnsignedShort OverlayColumns;
    /// OverlayType (60xx,0040) vr=CS, vm=1, type=1
    DcmCodeString OverlayType;
    /// OverlayOrigin (60xx,0050) vr=SS, vm=2, type=1
    DcmSignedShort OverlayOrigin;
    /// OverlayBitsAllocated (60xx,0100) vr=US, vm=1, type=1
    DcmUnsignedShort OverlayBitsAllocated;
    /// OverlayBitPosition (60xx,0102) vr=US, vm=1, type=1
    DcmUnsignedShort OverlayBitPosition;
    /// OverlayData (60xx,3000) vr=OB/OW, vm=1, type=1
    DcmOtherByteOtherWord OverlayData;
    /// OverlayDescription (60xx,0022) vr=LO, vm=1, type=3
    DcmLongString OverlayDescription;
    /// OverlaySubtype (60xx,0045) vr=LO, vm=1, type=3
    DcmLongString OverlaySubtype;
    /// OverlayLabel (60xx,1500) vr=LO, vm=1, type=3
    DcmLongString OverlayLabel;
    /// ROIArea (60xx,1301) vr=IS, vm=1, type=3
    DcmIntegerString ROIArea;
    /// ROIMean (60xx,1302) vr=DS, vm=1, type=3
    DcmDecimalString ROIMean;
    /// ROIStandardDeviation (60xx,1303) vr=DS, vm=1, type=3
    DcmDecimalString ROIStandardDeviation;

    // --- MultiFrameOverlayModule (U) ---

    /// NumberOfFramesInOverlay (60xx,0015) vr=IS, vm=1, type=1
    DcmIntegerString NumberOfFramesInOverlay;
    /// ImageFrameOrigin (60xx,0051) vr=US, vm=1, type=3
    DcmUnsignedShort ImageFrameOrigin;

    // --- ModalityLUTModule (U) ---

    /// ModalityLUTSequence (0028,3000) vr=SQ, vm=1, type=1C
    DRTModalityLUTSequence ModalityLUTSequence;
    /// RescaleIntercept (0028,1052) vr=DS, vm=1, type=1C
    DcmDecimalString RescaleIntercept;
    /// RescaleSlope (0028,1053) vr=DS, vm=1, type=1C
    DcmDecimalString RescaleSlope;
    /// RescaleType (0028,1054) vr=LO, vm=1, type=1C
    DcmLongString RescaleType;

    // --- RTDoseModule (M) ---

    /// SamplesPerPixel (0028,0002) vr=US, vm=1, type=1C
    /// - also defined in: ImagePixelModule
    DcmUnsignedShort SamplesPerPixel;
    /// PhotometricInterpretation (0028,0004) vr=CS, vm=1, type=1C
    /// - also defined in: ImagePixelModule
    DcmCodeString PhotometricInterpretation;
    /// BitsAllocated (0028,0100) vr=US, vm=1, type=1C
    /// - also defined in: ImagePixelModule
    DcmUnsignedShort BitsAllocated;
    /// BitsStored (0028,0101) vr=US, vm=1, type=1C
    /// - also defined in: ImagePixelModule
    DcmUnsignedShort BitsStored;
    /// HighBit (0028,0102) vr=US, vm=1, type=1C
    /// - also defined in: ImagePixelModule
    DcmUnsignedShort HighBit;
    /// PixelRepresentation (0028,0103) vr=US, vm=1, type=1C
    /// - also defined in: ImagePixelModule
    DcmUnsignedShort PixelRepresentation;
    /// DoseUnits (3004,0002) vr=CS, vm=1, type=1
    DcmCodeString DoseUnits;
    /// DoseType (3004,0004) vr=CS, vm=1, type=1
    DcmCodeString DoseType;
    /// SpatialTransformOfDose (3004,0005) vr=CS, vm=1, type=3
    DcmCodeString SpatialTransformOfDose;
    /// ReferencedSpatialRegistrationSequence (0070,0404) vr=SQ, vm=1, type=2C
    DRTReferencedSpatialRegistrationSequence ReferencedSpatialRegistrationSequence;
    /// InstanceNumber (0020,0013) vr=IS, vm=1, type=3
    /// - also defined in: GeneralImageModule, StructureSetModule, SOPCommonModule
    DcmIntegerString InstanceNumber;
    /// DoseComment (3004,0006) vr=LO, vm=1, type=3
    DcmLongString DoseComment;
    /// NormalizationPoint (3004,0008) vr=DS, vm=3, type=3
    DcmDecimalString NormalizationPoint;
    /// DoseSummationType (3004,000a) vr=CS, vm=1, type=1
    DcmCodeString DoseSummationType;
    /// ReferencedRTPlanSequence (300c,0002) vr=SQ, vm=1, type=1C
    DRTReferencedRTPlanSequence ReferencedRTPlanSequence;
    /// ReferencedTreatmentRecordSequence (3008,0030) vr=SQ, vm=1, type=1C
    DRTReferencedTreatmentRecordSequenceInRTDoseModule ReferencedTreatmentRecordSequence;
    /// GridFrameOffsetVector (3004,000c) vr=DS, vm=2-n, type=1C
    DcmDecimalString GridFrameOffsetVector;
    /// DoseGridScaling (3004,000e) vr=DS, vm=1, type=1C
    DcmDecimalString DoseGridScaling;
    /// TissueHeterogeneityCorrection (3004,0014) vr=CS, vm=1-3, type=3
    DcmCodeString TissueHeterogeneityCorrection;
    /// DerivationCodeSequence (0008,9215) vr=SQ, vm=1, type=3
    /// - also defined in: GeneralImageModule
    DRTDerivationCodeSequence DerivationCodeSequence;
    /// ReferencedInstanceSequence (0008,114a) vr=SQ, vm=1, type=3
    /// - also defined in: GeneralImageModule
    DRTReferencedInstanceSequence ReferencedInstanceSequence;

    // --- RTDVHModule (U) ---

    /// ReferencedStructureSetSequence (300c,0060) vr=SQ, vm=1, type=1
    DRTReferencedStructureSetSequence ReferencedStructureSetSequence;
    /// DVHNormalizationPoint (3004,0040) vr=DS, vm=3, type=3
    DcmDecimalString DVHNormalizationPoint;
    /// DVHNormalizationDoseValue (3004,0042) vr=DS, vm=1, type=3
    DcmDecimalString DVHNormalizationDoseValue;
    /// DVHSequence (3004,0050) vr=SQ, vm=1, type=1
    DRTDVHSequence DVHSequence;

    // --- StructureSetModule (C) ---

    /// StructureSetLabel (3006,0002) vr=SH, vm=1, type=1
    DcmShortString StructureSetLabel;
    /// StructureSetName (3006,0004) vr=LO, vm=1, type=3
    DcmLongString StructureSetName;
    /// StructureSetDescription (3006,0006) vr=ST, vm=1, type=3
    DcmShortText StructureSetDescription;
    // InstanceNumber (0020,0013) vr=IS, vm=1, type=3
    // - also defined in: GeneralImageModule, RTDoseModule, SOPCommonModule
    // DcmIntegerString InstanceNumber;
    /// StructureSetDate (3006,0008) vr=DA, vm=1, type=2
    DcmDate StructureSetDate;
    /// StructureSetTime (3006,0009) vr=TM, vm=1, type=2
    DcmTime StructureSetTime;
    /// ReferencedFrameOfReferenceSequence (3006,0010) vr=SQ, vm=1, type=3
    DRTReferencedFrameOfReferenceSequence ReferencedFrameOfReferenceSequence;
    /// StructureSetROISequence (3006,0020) vr=SQ, vm=1, type=1
    DRTStructureSetROISequence StructureSetROISequence;
    /// PredecessorStructureSetSequence (3006,0018) vr=SQ, vm=1, type=3
    DRTPredecessorStructureSetSequence PredecessorStructureSetSequence;

    // --- ROIContourModule (C) ---

    /// ROIContourSequence (3006,0039) vr=SQ, vm=1, type=1
    DRTROIContourSequence ROIContourSequence;

    // --- RTDoseROIModule (C) ---

    /// RTDoseROISequence (3004,0010) vr=SQ, vm=1, type=1
    DRTRTDoseROISequence RTDoseROISequence;

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
    // - also defined in: GeneralImageModule, RTDoseModule, StructureSetModule
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
