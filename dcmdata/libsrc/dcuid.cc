/*
 *
 *  Copyright (C) 1994-2014, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose:
 *  Definitions of "well known" DICOM Unique Identifiers,
 *  routines for finding and creating UIDs.
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_WINDOWS_H
#include <windows.h>     /* this includes either winsock.h or winsock2.h */
#else
#ifdef HAVE_WINSOCK_H
#include <winsock.h>     /* include winsock.h directly i.e. on MacOS */
#endif
#endif

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CTIME
#define INCLUDE_LIBC
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#ifndef DCOMPAT_SYS_SOCKET_H_
#define DCOMPAT_SYS_SOCKET_H_
/* some systems don't protect sys/socket.h (e.g. DEC Ultrix) */
#include <sys/socket.h>
#endif
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#ifndef HAVE_WINDOWS_H
#ifndef HAVE_PROTOTYPE_GETTIMEOFDAY
 /* Ultrix has gettimeofday() but no prototype in the header files */
 int gettimeofday(struct timeval *tp, void *);
#endif
#endif

END_EXTERN_C

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/ofstd/ofthread.h"
#include "dcmtk/ofstd/ofcrc32.h"
#include "dcmtk/ofstd/ofdefine.h"
#include "dcmtk/ofstd/ofstd.h"

struct UIDNameMap {
    const char* uid;
    const char* name;
};

//
// It is very important that the names of the UIDs may not use the following
// characters: space  (  )  [  ], =  <  >

static const UIDNameMap uidNameMap[] = {

    { UID_StandardApplicationContext,                          "StandardApplicationContext" },

    // Transfer Syntax
    { UID_LittleEndianImplicitTransferSyntax,                  "LittleEndianImplicit" },
    { UID_LittleEndianExplicitTransferSyntax,                  "LittleEndianExplicit" },
    { UID_BigEndianExplicitTransferSyntax,                     "BigEndianExplicit" },
    { UID_JPEGProcess1TransferSyntax,                          "JPEGBaseline" },
    { UID_JPEGProcess2_4TransferSyntax,                        "JPEGExtended:Process2+4" },
    { UID_JPEGProcess3_5TransferSyntax,                        "JPEGExtended:Process3+5" },
    { UID_JPEGProcess6_8TransferSyntax,                        "JPEGSpectralSelection:Non-hierarchical:Process6+8" },
    { UID_JPEGProcess7_9TransferSyntax,                        "JPEGSpectralSelection:Non-hierarchical:Process7+9" },
    { UID_JPEGProcess10_12TransferSyntax,                      "JPEGFullProgression:Non-hierarchical:Process10+12" },
    { UID_JPEGProcess11_13TransferSyntax,                      "JPEGFullProgression:Non-hierarchical:Process11+13" },
    { UID_JPEGProcess14TransferSyntax,                         "JPEGLossless:Non-hierarchical:Process14" },
    { UID_JPEGProcess15TransferSyntax,                         "JPEGLossless:Non-hierarchical:Process15" },
    { UID_JPEGProcess16_18TransferSyntax,                      "JPEGExtended:Hierarchical:Process16+18" },
    { UID_JPEGProcess17_19TransferSyntax,                      "JPEGExtended:Hierarchical:Process17+19" },
    { UID_JPEGProcess20_22TransferSyntax,                      "JPEGSpectralSelection:Hierarchical:Process20+22" },
    { UID_JPEGProcess21_23TransferSyntax,                      "JPEGSpectralSelection:Hierarchical:Process21+23" },
    { UID_JPEGProcess24_26TransferSyntax,                      "JPEGFullProgression:Hierarchical:Process24+26" },
    { UID_JPEGProcess25_27TransferSyntax,                      "JPEGFullProgression:Hierarchical:Process25+27" },
    { UID_JPEGProcess28TransferSyntax,                         "JPEGLossless:Hierarchical:Process28" },
    { UID_JPEGProcess29TransferSyntax,                         "JPEGLossless:Hierarchical:Process29" },
    { UID_JPEGProcess14SV1TransferSyntax,                      "JPEGLossless:Non-hierarchical-1stOrderPrediction" },
    { UID_JPEGLSLosslessTransferSyntax,                        "JPEGLSLossless" },
    { UID_JPEGLSLossyTransferSyntax,                           "JPEGLSLossy" },
    { UID_RLELosslessTransferSyntax,                           "RLELossless" },
    { UID_DeflatedExplicitVRLittleEndianTransferSyntax,        "DeflatedLittleEndianExplicit" },
    { UID_JPEG2000LosslessOnlyTransferSyntax,                  "JPEG2000LosslessOnly" },
    { UID_JPEG2000TransferSyntax,                              "JPEG2000" },
    { UID_MPEG2MainProfileAtMainLevelTransferSyntax,           "MPEG2MainProfile@MainLevel" },
    { UID_MPEG2MainProfileAtHighLevelTransferSyntax,           "MPEG2MainProfile@HighLevel" },
    { UID_MPEG4HighProfileLevel4_1TransferSyntax,              "MPEG4HighProfile/Level4.1" },
    { UID_MPEG4BDcompatibleHighProfileLevel4_1TransferSyntax,  "MPEG4BDcompatibleHighProfile/Level4.1" },
    { UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnlyTransferSyntax, "JPEG2000MulticomponentLosslessOnly" },
    { UID_JPEG2000Part2MulticomponentImageCompressionTransferSyntax,             "JPEG2000Multicomponent" },
    { UID_JPIPReferencedTransferSyntax,                        "JPIPReferenced" },
    { UID_JPIPReferencedDeflateTransferSyntax,                 "JPIPReferencedDeflate" },
    { UID_RFC2557MIMEEncapsulationTransferSyntax,              "RFC2557MIMEEncapsulation" },
    { UID_XMLEncodingTransferSyntax,                           "XMLEncoding" },

    // Storage
    { UID_AmbulatoryECGWaveformStorage,                        "AmbulatoryECGWaveformStorage" },
    { UID_ArterialPulseWaveformStorage,                        "ArterialPulseWaveformStorage" },
    { UID_AutorefractionMeasurementsStorage,                   "AutorefractionMeasurementsStorage" },
    { UID_BasicStructuredDisplayStorage,                       "BasicStructuredDisplayStorage" },
    { UID_BasicTextSRStorage,                                  "BasicTextSRStorage" },
    { UID_BasicVoiceAudioWaveformStorage,                      "BasicVoiceAudioWaveformStorage" },
    { UID_BlendingSoftcopyPresentationStateStorage,            "BlendingSoftcopyPresentationStateStorage" },
    { UID_BreastProjectionXRayImageStorageForPresentation,     "BreastProjectionXRayImageStorageForPresentation" },
    { UID_BreastProjectionXRayImageStorageForProcessing,       "BreastProjectionXRayImageStorageForProcessing" },
    { UID_BreastTomosynthesisImageStorage,                     "BreastTomosynthesisImageStorage" },
    { UID_CardiacElectrophysiologyWaveformStorage,             "CardiacElectrophysiologyWaveformStorage" },
    { UID_ChestCADSRStorage,                                   "ChestCADSRStorage" },
    { UID_ColonCADSRStorage,                                   "ColonCADSRStorage" },
    { UID_ColorSoftcopyPresentationStateStorage,               "ColorSoftcopyPresentationStateStorage" },
    { UID_Comprehensive3DSRStorage,                            "Comprehensive3DSRStorage" },
    { UID_ComprehensiveSRStorage,                              "ComprehensiveSRStorage" },
    { UID_ComputedRadiographyImageStorage,                     "ComputedRadiographyImageStorage" },
    { UID_CornealTopographyMapStorage,                         "CornealTopographyMapStorage" },
    { UID_CTImageStorage,                                      "CTImageStorage" },
    { UID_DeformableSpatialRegistrationStorage,                "DeformableSpatialRegistrationStorage" },
    { UID_DigitalIntraOralXRayImageStorageForPresentation,     "DigitalIntraOralXRayImageStorageForPresentation" },
    { UID_DigitalIntraOralXRayImageStorageForProcessing,       "DigitalIntraOralXRayImageStorageForProcessing" },
    { UID_DigitalMammographyXRayImageStorageForPresentation,   "DigitalMammographyXRayImageStorageForPresentation" },
    { UID_DigitalMammographyXRayImageStorageForProcessing,     "DigitalMammographyXRayImageStorageForProcessing" },
    { UID_DigitalXRayImageStorageForPresentation,              "DigitalXRayImageStorageForPresentation" },
    { UID_DigitalXRayImageStorageForProcessing,                "DigitalXRayImageStorageForProcessing" },
    { UID_EncapsulatedCDAStorage,                              "EncapsulatedCDAStorage" },
    { UID_EncapsulatedPDFStorage,                              "EncapsulatedPDFStorage" },
    { UID_EnhancedCTImageStorage,                              "EnhancedCTImageStorage" },
    { UID_EnhancedMRColorImageStorage,                         "EnhancedMRColorImageStorage" },
    { UID_EnhancedMRImageStorage,                              "EnhancedMRImageStorage" },
    { UID_EnhancedPETImageStorage,                             "EnhancedPETImageStorage" },
    { UID_EnhancedSRStorage,                                   "EnhancedSRStorage" },
    { UID_EnhancedUSVolumeStorage,                             "EnhancedUSVolumeStorage" },
    { UID_EnhancedXAImageStorage,                              "EnhancedXAImageStorage" },
    { UID_EnhancedXRFImageStorage,                             "EnhancedXRFImageStorage" },
    { UID_GeneralAudioWaveformStorage,                         "GeneralAudioWaveformStorage" },
    { UID_GeneralECGWaveformStorage,                           "GeneralECGWaveformStorage" },
    { UID_GenericImplantTemplateStorage,                       "GenericImplantTemplateStorage" },
    { UID_GrayscaleSoftcopyPresentationStateStorage,           "GrayscaleSoftcopyPresentationStateStorage" },
    { UID_HangingProtocolStorage,                              "HangingProtocolStorage" },
    { UID_HemodynamicWaveformStorage,                          "HemodynamicWaveformStorage" },
    { UID_ImplantAssemblyTemplateStorage,                      "ImplantAssemblyTemplateStorage" },
    { UID_ImplantationPlanSRDocumentStorage,                   "ImplantationPlanSRDocumentStorage" },
    { UID_ImplantTemplateGroupStorage,                         "ImplantTemplateGroupStorage" },
    { UID_IntraocularLensCalculationsStorage,                  "IntraocularLensCalculationsStorage" },
    { UID_IntravascularOpticalCoherenceTomographyImageStorageForPresentation, "IntravascularOpticalCoherenceTomographyImageStorageForPresentation" },
    { UID_IntravascularOpticalCoherenceTomographyImageStorageForProcessing, "IntravascularOpticalCoherenceTomographyImageStorageForProcessing" },
    { UID_KeratometryMeasurementsStorage,                      "KeratometryMeasurementsStorage" },
    { UID_KeyObjectSelectionDocumentStorage,                   "KeyObjectSelectionDocumentStorage" },
    { UID_LegacyConvertedEnhancedCTImageStorage,               "LegacyConvertedEnhancedCTImageStorage" },
    { UID_LegacyConvertedEnhancedMRImageStorage,               "LegacyConvertedEnhancedMRImageStorage" },
    { UID_LegacyConvertedEnhancedPETImageStorage,              "LegacyConvertedEnhancedPETImageStorage" },
    { UID_LensometryMeasurementsStorage,                       "LensometryMeasurementsStorage" },
    { UID_MacularGridThicknessAndVolumeReportStorage,          "MacularGridThicknessAndVolumeReportStorage" },
    { UID_MammographyCADSRStorage,                             "MammographyCADSRStorage" },
    { UID_MediaStorageDirectoryStorage,                        "MediaStorageDirectoryStorage" },
    { UID_MRImageStorage,                                      "MRImageStorage" },
    { UID_MRSpectroscopyStorage,                               "MRSpectroscopyStorage" },
    { UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage, "MultiframeGrayscaleByteSecondaryCaptureImageStorage" },
    { UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage, "MultiframeGrayscaleWordSecondaryCaptureImageStorage" },
    { UID_MultiframeSingleBitSecondaryCaptureImageStorage,     "MultiframeSingleBitSecondaryCaptureImageStorage" },
    { UID_MultiframeTrueColorSecondaryCaptureImageStorage,     "MultiframeTrueColorSecondaryCaptureImageStorage" },
    { UID_NuclearMedicineImageStorage,                         "NuclearMedicineImageStorage" },
    { UID_OphthalmicAxialMeasurementsStorage,                  "OphthalmicAxialMeasurementsStorage" },
    { UID_OphthalmicPhotography16BitImageStorage,              "OphthalmicPhotography16BitImageStorage" },
    { UID_OphthalmicPhotography8BitImageStorage,               "OphthalmicPhotography8BitImageStorage" },
    { UID_OphthalmicThicknessMapStorage,                       "OphthalmicThicknessMapStorage" },
    { UID_OphthalmicTomographyImageStorage,                    "OphthalmicTomographyImageStorage" },
    { UID_OphthalmicVisualFieldStaticPerimetryMeasurementsStorage, "OphthalmicVisualFieldStaticPerimetryMeasurementsStorage" },
    { UID_PositronEmissionTomographyImageStorage,              "PositronEmissionTomographyImageStorage" },
    { UID_ProcedureLogStorage,                                 "ProcedureLogStorage" },
    { UID_PseudoColorSoftcopyPresentationStateStorage,         "PseudoColorSoftcopyPresentationStateStorage" },
    { UID_RawDataStorage,                                      "RawDataStorage" },
    { UID_RealWorldValueMappingStorage,                        "RealWorldValueMappingStorage" },
    { UID_RespiratoryWaveformStorage,                          "RespiratoryWaveformStorage" },
    { UID_RTBeamsTreatmentRecordStorage,                       "RTBeamsTreatmentRecordStorage" },
    { UID_RTBrachyTreatmentRecordStorage,                      "RTBrachyTreatmentRecordStorage" },
    { UID_RTDoseStorage,                                       "RTDoseStorage" },
    { UID_RTImageStorage,                                      "RTImageStorage" },
    { UID_RTIonBeamsTreatmentRecordStorage,                    "RTIonBeamsTreatmentRecordStorage" },
    { UID_RTIonPlanStorage,                                    "RTIonPlanStorage" },
    { UID_RTPlanStorage,                                       "RTPlanStorage" },
    { UID_RTStructureSetStorage,                               "RTStructureSetStorage" },
    { UID_RTTreatmentSummaryRecordStorage,                     "RTTreatmentSummaryRecordStorage" },
    { UID_SecondaryCaptureImageStorage,                        "SecondaryCaptureImageStorage" },
    { UID_SegmentationStorage,                                 "SegmentationStorage" },
    { UID_SpatialFiducialsStorage,                             "SpatialFiducialsStorage" },
    { UID_SpatialRegistrationStorage,                          "SpatialRegistrationStorage" },
    { UID_SpectaclePrescriptionReportStorage,                  "SpectaclePrescriptionReportStorage" },
    { UID_StereometricRelationshipStorage,                     "StereometricRelationshipStorage" },
    { UID_SubjectiveRefractionMeasurementsStorage,             "SubjectiveRefractionMeasurementsStorage" },
    { UID_SurfaceScanMeshStorage,                              "SurfaceScanMeshStorage" },
    { UID_SurfaceScanPointCloudStorage,                        "SurfaceScanPointCloudStorage" },
    { UID_SurfaceSegmentationStorage,                          "SurfaceSegmentationStorage" },
    { UID_TwelveLeadECGWaveformStorage,                        "TwelveLeadECGWaveformStorage" },
    { UID_UltrasoundImageStorage,                              "UltrasoundImageStorage" },
    { UID_UltrasoundMultiframeImageStorage,                    "UltrasoundMultiframeImageStorage" },
    { UID_VideoEndoscopicImageStorage,                         "VideoEndoscopicImageStorage" },
    { UID_VideoMicroscopicImageStorage,                        "VideoMicroscopicImageStorage" },
    { UID_VideoPhotographicImageStorage,                       "VideoPhotographicImageStorage" },
    { UID_VisualAcuityMeasurementsStorage,                     "VisualAcuityMeasurementsStorage" },
    { UID_VLEndoscopicImageStorage,                            "VLEndoscopicImageStorage" },
    { UID_VLMicroscopicImageStorage,                           "VLMicroscopicImageStorage" },
    { UID_VLPhotographicImageStorage,                          "VLPhotographicImageStorage" },
    { UID_VLSlideCoordinatesMicroscopicImageStorage,           "VLSlideCoordinatesMicroscopicImageStorage" },
    { UID_VLWholeSlideMicroscopyImageStorage,                  "VLWholeSlideMicroscopyImageStorage" },
    { UID_XAXRFGrayscaleSoftcopyPresentationStateStorage,      "XAXRFGrayscaleSoftcopyPresentationStateStorage" },
    { UID_XRay3DAngiographicImageStorage,                      "XRay3DAngiographicImageStorage" },
    { UID_XRay3DCraniofacialImageStorage,                      "XRay3DCraniofacialImageStorage" },
    { UID_XRayAngiographicImageStorage,                        "XRayAngiographicImageStorage" },
    { UID_XRayRadiationDoseSRStorage,                          "XRayRadiationDoseSRStorage" },
    { UID_XRayRadiofluoroscopicImageStorage,                   "XRayRadiofluoroscopicImageStorage" },
    // Storage (retired)
    { UID_RETIRED_HardcopyColorImageStorage,                   "RETIRED_HardcopyColorImageStorage" },
    { UID_RETIRED_HardcopyGrayscaleImageStorage,               "RETIRED_HardcopyGrayscaleImageStorage" },
    { UID_RETIRED_NuclearMedicineImageStorage,                 "RETIRED_NuclearMedicineImageStorage" },
    { UID_RETIRED_StandaloneCurveStorage,                      "RETIRED_StandaloneCurveStorage" },
    { UID_RETIRED_StandaloneModalityLUTStorage,                "RETIRED_StandaloneModalityLUTStorage" },
    { UID_RETIRED_StandaloneOverlayStorage,                    "RETIRED_StandaloneOverlayStorage" },
    { UID_RETIRED_StandalonePETCurveStorage,                   "RETIRED_StandalonePETCurveStorage" },
    { UID_RETIRED_StandaloneVOILUTStorage,                     "RETIRED_StandaloneVOILUTStorage" },
    { UID_RETIRED_StoredPrintStorage,                          "RETIRED_StoredPrintStorage" },
    { UID_RETIRED_UltrasoundImageStorage,                      "RETIRED_UltrasoundImageStorage" },
    { UID_RETIRED_UltrasoundMultiframeImageStorage,            "RETIRED_UltrasoundMultiframeImageStorage" },
    { UID_RETIRED_VLImageStorage,                              "RETIRED_VLImageStorage" },
    { UID_RETIRED_VLMultiFrameImageStorage,                    "RETIRED_VLMultiFrameImageStorage" },
    { UID_RETIRED_XRayAngiographicBiPlaneImageStorage,         "RETIRED_XRayAngiographicBiPlaneImageStorage" },
    // Storage (DICOS)
    { UID_DICOS_CTImageStorage,                                "DICOS_CTImageStorage" },
    { UID_DICOS_DigitalXRayImageStorageForPresentation,        "DICOS_DigitalXRayImageStorageForPresentation" },
    { UID_DICOS_DigitalXRayImageStorageForProcessing,          "DICOS_DigitalXRayImageStorageForProcessing" },
    { UID_DICOS_ThreatDetectionReportStorage,                  "DICOS_ThreatDetectionReportStorage" },
    // Storage (DICONDE)
    { UID_DICONDE_EddyCurrentImageStorage,                     "DICONDE_EddyCurrentImageStorage" },
    { UID_DICONDE_EddyCurrentMultiframeImageStorage,           "DICONDE_EddyCurrentMultiframeImageStorage" },

    // Query/Retrieve
    { UID_FINDPatientRootQueryRetrieveInformationModel,              "FINDPatientRootQueryRetrieveInformationModel" },
    { UID_FINDStudyRootQueryRetrieveInformationModel,                "FINDStudyRootQueryRetrieveInformationModel" },
    { UID_GETPatientRootQueryRetrieveInformationModel,               "GETPatientRootQueryRetrieveInformationModel" },
    { UID_GETStudyRootQueryRetrieveInformationModel,                 "GETStudyRootQueryRetrieveInformationModel" },
    { UID_MOVEPatientRootQueryRetrieveInformationModel,              "MOVEPatientRootQueryRetrieveInformationModel" },
    { UID_MOVEStudyRootQueryRetrieveInformationModel,                "MOVEStudyRootQueryRetrieveInformationModel" },
    { UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel, "RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel" },
    { UID_RETIRED_GETCompositeInstanceRetrieveWithoutBulkData,       "RETIRED_GETCompositeInstanceRetrieveWithoutBulkData" },
    { UID_RETIRED_GETCompositeInstanceRootRetrieve,                  "RETIRED_GETCompositeInstanceRootRetrieve" },
    { UID_RETIRED_GETPatientStudyOnlyQueryRetrieveInformationModel,  "RETIRED_GETPatientStudyOnlyQueryRetrieveInformationModel" },
    { UID_RETIRED_MOVECompositeInstanceRootRetrieve,                 "RETIRED_MOVECompositeInstanceRootRetrieve" },
    { UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel, "RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel" },

    // Worklist
    { UID_FINDModalityWorklistInformationModel,                "FINDModalityWorklistInformationModel" },

    // General Purpose Worklist
    { UID_RETIRED_FINDGeneralPurposeWorklistInformationModel,   "RETIRED_FINDGeneralPurposeWorklistInformationModel" },
    { UID_RETIRED_GeneralPurposePerformedProcedureStepSOPClass, "RETIRED_GeneralPurposePerformedProcedureStepSOPClass" },
    { UID_RETIRED_GeneralPurposeScheduledProcedureStepSOPClass, "RETIRED_GeneralPurposeScheduledProcedureStepSOPClass" },
    { UID_RETIRED_GeneralPurposeWorklistManagementMetaSOPClass, "RETIRED_GeneralPurposeWorklistManagementMetaSOPClass" },

    // MPPS
    { UID_ModalityPerformedProcedureStepNotificationSOPClass,  "ModalityPerformedProcedureStepNotificationSOPClass" },
    { UID_ModalityPerformedProcedureStepRetrieveSOPClass,      "ModalityPerformedProcedureStepRetrieveSOPClass" },
    { UID_ModalityPerformedProcedureStepSOPClass,              "ModalityPerformedProcedureStepSOPClass" },

    // Radiotherapy Treatment Delivery
    { UID_RTBeamsDeliveryInstructionStorage,                   "RTBeamsDeliveryInstructionStorage" },
    { UID_RTConventionalMachineVerification,                   "RTConventionalMachineVerification" },
    { UID_RTIonMachineVerification,                            "RTIonMachineVerification" },

    // Unified Worklist and Procedure Step
    { UID_UnifiedWorklistAndProcedureStepServiceClass,         "UnifiedWorklistAndProcedureStepServiceClass" },
    { UID_UnifiedProcedureStepPushSOPClass,                    "UnifiedProcedureStepPushSOPClass" },
    { UID_UnifiedProcedureStepWatchSOPClass,                   "UnifiedProcedureStepWatchSOPClass" },
    { UID_UnifiedProcedureStepPullSOPClass,                    "UnifiedProcedureStepPullSOPClass" },
    { UID_UnifiedProcedureStepEventSOPClass,                   "UnifiedProcedureStepEventSOPClass" },
    { UID_UnifiedWorklistAndProcedureStepSOPInstance,          "UnifiedWorklistAndProcedureStepSOPInstance" },

    // Storage Commitment
    { UID_RETIRED_StorageCommitmentPullModelSOPClass,          "RETIRED_StorageCommitmentPullModelSOPClass" },
    { UID_RETIRED_StorageCommitmentPullModelSOPInstance,       "RETIRED_StorageCommitmentPullModelSOPInstance" },
    { UID_StorageCommitmentPushModelSOPClass,                  "StorageCommitmentPushModelSOPClass" },
    { UID_StorageCommitmentPushModelSOPInstance,               "StorageCommitmentPushModelSOPInstance" },

    // Hanging Protocols
    { UID_FINDHangingProtocolInformationModel,                 "FINDHangingProtocolInformationModel" },
    { UID_MOVEHangingProtocolInformationModel,                 "MOVEHangingProtocolInformationModel" },

    // Relevant Patient Information Query
    { UID_BreastImagingRelevantPatientInformationQuery,        "BreastImagingRelevantPatientInformationQuery" },
    { UID_CardiacRelevantPatientInformationQuery,              "CardiacRelevantPatientInformationQuery" },
    { UID_GeneralRelevantPatientInformationQuery,              "GeneralRelevantPatientInformationQuery" },

    // Color Palette Storage and Query/Retrieve
    { UID_ColorPaletteStorage,                                 "ColorPaletteStorage" },
    { UID_FINDColorPaletteInformationModel,                    "FINDColorPaletteInformationModel" },
    { UID_MOVEColorPaletteInformationModel,                    "MOVEColorPaletteInformationModel" },
    { UID_GETColorPaletteInformationModel,                     "GETColorPaletteInformationModel" },

    // Implant Template Query/Retrieve
    { UID_FINDGenericImplantTemplateInformationModel,          "FINDGenericImplantTemplateInformationModel" },
    { UID_MOVEGenericImplantTemplateInformationModel,          "MOVEGenericImplantTemplateInformationModel" },
    { UID_GETGenericImplantTemplateInformationModel,           "GETGenericImplantTemplateInformationModel" },
    { UID_FINDImplantAssemblyTemplateInformationModel,         "FINDImplantAssemblyTemplateInformationModel" },
    { UID_MOVEImplantAssemblyTemplateInformationModel,         "MOVEImplantAssemblyTemplateInformationModel" },
    { UID_GETImplantAssemblyTemplateInformationModel,          "GETImplantAssemblyTemplateInformationModel" },
    { UID_FINDImplantTemplateGroupInformationModel,            "FINDImplantTemplateGroupInformationModel" },
    { UID_MOVEImplantTemplateGroupInformationModel,            "MOVEImplantTemplateGroupInformationModel" },
    { UID_GETImplantTemplateGroupInformationModel,             "GETImplantTemplateGroupInformationModel" },

    // Print
    { UID_BasicAnnotationBoxSOPClass,                          "BasicAnnotationBoxSOPClass" },
    { UID_BasicColorImageBoxSOPClass,                          "BasicColorImageBoxSOPClass" },
    { UID_BasicColorPrintManagementMetaSOPClass,               "BasicColorPrintManagementMetaSOPClass" },
    { UID_BasicFilmBoxSOPClass,                                "BasicFilmBoxSOPClass" },
    { UID_BasicFilmSessionSOPClass,                            "BasicFilmSessionSOPClass" },
    { UID_BasicGrayscaleImageBoxSOPClass,                      "BasicGrayscaleImageBoxSOPClass" },
    { UID_BasicGrayscalePrintManagementMetaSOPClass,           "BasicGrayscalePrintManagementMetaSOPClass" },
    { UID_PresentationLUTSOPClass,                             "PresentationLUTSOPClass" },
    { UID_PrintJobSOPClass,                                    "PrintJobSOPClass" },
    { UID_PrinterConfigurationRetrievalSOPClass,               "PrinterConfigurationRetrievalSOPClass" },
    { UID_PrinterConfigurationRetrievalSOPInstance,            "PrinterConfigurationRetrievalSOPInstance" },
    { UID_PrinterSOPClass,                                     "PrinterSOPClass" },
    { UID_PrinterSOPInstance,                                  "PrinterSOPInstance" },
    { UID_RETIRED_BasicPrintImageOverlayBoxSOPClass,           "RETIRED_BasicPrintImageOverlayBoxSOPClass" },
    { UID_RETIRED_ImageOverlayBoxSOPClass,                     "RETIRED_ImageOverlayBoxSOPClass" },
    { UID_RETIRED_PrintQueueManagementSOPClass,                "RETIRED_PrintQueueManagementSOPClass" },
    { UID_RETIRED_PrintQueueSOPInstance,                       "RETIRED_PrintQueueSOPInstance" },
    { UID_RETIRED_PullPrintRequestSOPClass,                    "RETIRED_PullPrintRequestSOPClass" },
    { UID_RETIRED_PullStoredPrintManagementMetaSOPClass,       "RETIRED_PullStoredPrintManagementMetaSOPClass" },
    { UID_RETIRED_ReferencedColorPrintManagementMetaSOPClass,  "RETIRED_ReferencedColorPrintManagementMetaSOPClass" },
    { UID_RETIRED_ReferencedGrayscalePrintManagementMetaSOPClass, "RETIRED_ReferencedGrayscalePrintManagementMetaSOPClass" },
    { UID_RETIRED_ReferencedImageBoxSOPClass,                  "RETIRED_ReferencedImageBoxSOPClass" },
    { UID_VOILUTBoxSOPClass,                                   "VOILUTBoxSOPClass" },

    // Detached Management
    { UID_RETIRED_DetachedInterpretationManagementSOPClass,    "RETIRED_DetachedInterpretationManagementSOPClass" },
    { UID_RETIRED_DetachedPatientManagementMetaSOPClass,       "RETIRED_DetachedPatientManagementMetaSOPClass" },
    { UID_RETIRED_DetachedPatientManagementSOPClass,           "RETIRED_DetachedPatientManagementSOPClass" },
    { UID_RETIRED_DetachedResultsManagementMetaSOPClass,       "RETIRED_DetachedResultsManagementMetaSOPClass" },
    { UID_RETIRED_DetachedResultsManagementSOPClass,           "RETIRED_DetachedResultsManagementSOPClass" },
    { UID_RETIRED_DetachedStudyManagementMetaSOPClass,         "RETIRED_DetachedStudyManagementMetaSOPClass" },
    { UID_RETIRED_DetachedStudyManagementSOPClass,             "RETIRED_DetachedStudyManagementSOPClass" },
    { UID_RETIRED_DetachedVisitManagementSOPClass,             "RETIRED_DetachedVisitManagementSOPClass" },

    // Procedure Log
    { UID_ProceduralEventLoggingSOPClass,                      "ProceduralEventLoggingSOPClass" },
    { UID_ProceduralEventLoggingSOPInstance,                   "ProceduralEventLoggingSOPInstance" },

    // Substance Administration
    { UID_SubstanceAdministrationLoggingSOPClass,              "SubstanceAdministrationLoggingSOPClass" },
    { UID_SubstanceAdministrationLoggingSOPInstance,           "SubstanceAdministrationLoggingSOPInstance" },
    { UID_ProductCharacteristicsQuerySOPClass,                 "ProductCharacteristicsQuerySOPClass" },
    { UID_SubstanceApprovalQuerySOPClass,                      "SubstanceApprovalQuerySOPClass" },

    // Media Creation
    { UID_MediaCreationManagementSOPClass,                     "MediaCreationManagementSOPClass" },

    // SOP Class Relationship Negotiation
    { UID_StorageServiceClass,                                 "StorageServiceClass" },

    // Instance Availability Notification
    { UID_InstanceAvailabilityNotificationSOPClass,            "InstanceAvailabilityNotificationSOPClass" },

    // Application Hosting
    { UID_NativeDICOMModel,                                    "NativeDICOMModel" },
    { UID_AbstractMultiDimensionalImageModel,                  "AbstractMultiDimensionalImageModel" },

    // Other
    { UID_RETIRED_BasicStudyContentNotificationSOPClass,       "RETIRED_BasicStudyContentNotificationSOPClass" },
    { UID_RETIRED_StudyComponentManagementSOPClass,            "RETIRED_StudyComponentManagementSOPClass" },
    { UID_VerificationSOPClass,                                "VerificationSOPClass" },

    // Coding Schemes
    { UID_DICOMControlledTerminologyCodingScheme,              "DICOMControlledTerminologyCodingScheme" },
    { UID_DICOMUIDRegistryCodingScheme,                        "DICOMUIDRegistryCodingScheme" },

    // Configuration Management LDAP UIDs
    { UID_LDAP_dicomAETitle,                                   "LDAP_dicomAETitle" },
    { UID_LDAP_dicomApplicationCluster,                        "LDAP_dicomApplicationCluster" },
    { UID_LDAP_dicomAssociationAcceptor,                       "LDAP_dicomAssociationAcceptor" },
    { UID_LDAP_dicomAssociationInitiator,                      "LDAP_dicomAssociationInitiator" },
    { UID_LDAP_dicomAuthorizedNodeCertificateReference,        "LDAP_dicomAuthorizedNodeCertificateReference" },
    { UID_LDAP_dicomConfigurationRoot,                         "LDAP_dicomConfigurationRoot" },
    { UID_LDAP_dicomDescription,                               "LDAP_dicomDescription" },
    { UID_LDAP_dicomDevice,                                    "LDAP_dicomDevice" },
    { UID_LDAP_dicomDeviceName,                                "LDAP_dicomDeviceName" },
    { UID_LDAP_dicomDeviceSerialNumber,                        "LDAP_dicomDeviceSerialNumber" },
    { UID_LDAP_dicomDevicesRoot,                               "LDAP_dicomDevicesRoot" },
    { UID_LDAP_dicomHostname,                                  "LDAP_dicomHostname" },
    { UID_LDAP_dicomInstalled,                                 "LDAP_dicomInstalled" },
    { UID_LDAP_dicomInstitutionAddress,                        "LDAP_dicomInstitutionAddress" },
    { UID_LDAP_dicomInstitutionDepartmentName,                 "LDAP_dicomInstitutionDepartmentName" },
    { UID_LDAP_dicomInstitutionName,                           "LDAP_dicomInstitutionName" },
    { UID_LDAP_dicomIssuerOfPatientID,                         "LDAP_dicomIssuerOfPatientID" },
    { UID_LDAP_dicomManufacturer,                              "LDAP_dicomManufacturer" },
    { UID_LDAP_dicomManufacturerModelName,                     "LDAP_dicomManufacturerModelName" },
    { UID_LDAP_dicomNetworkAE,                                 "LDAP_dicomNetworkAE" },
    { UID_LDAP_dicomNetworkConnection,                         "LDAP_dicomNetworkConnection" },
    { UID_LDAP_dicomNetworkConnectionReference,                "LDAP_dicomNetworkConnectionReference" },
    { UID_LDAP_dicomPort,                                      "LDAP_dicomPort" },
    { UID_LDAP_dicomPreferredCalledAETitle,                    "LDAP_dicomPreferredCalledAETitle" },
    { UID_LDAP_dicomPreferredCallingAETitle,                   "LDAP_dicomPreferredCallingAETitle" },
    { UID_LDAP_dicomPrimaryDeviceType,                         "LDAP_dicomPrimaryDeviceType" },
    { UID_LDAP_dicomRelatedDeviceReference,                    "LDAP_dicomRelatedDeviceReference" },
    { UID_LDAP_dicomSOPClass,                                  "LDAP_dicomSOPClass" },
    { UID_LDAP_dicomSoftwareVersion,                           "LDAP_dicomSoftwareVersion" },
    { UID_LDAP_dicomStationName,                               "LDAP_dicomStationName" },
    { UID_LDAP_dicomSupportedCharacterSet,                     "LDAP_dicomSupportedCharacterSet" },
    { UID_LDAP_dicomTLSCyphersuite,                            "LDAP_dicomTLSCyphersuite" },
    { UID_LDAP_dicomThisNodeCertificateReference,              "LDAP_dicomThisNodeCertificateReference" },
    { UID_LDAP_dicomTransferCapability,                        "LDAP_dicomTransferCapability" },
    { UID_LDAP_dicomTransferRole,                              "LDAP_dicomTransferRole" },
    { UID_LDAP_dicomTransferSyntax,                            "LDAP_dicomTransferSyntax" },
    { UID_LDAP_dicomUniqueAETitle,                             "LDAP_dicomUniqueAETitle" },
    { UID_LDAP_dicomUniqueAETitlesRegistryRoot,                "LDAP_dicomUniqueAETitlesRegistryRoot" },
    { UID_LDAP_dicomVendorData,                                "LDAP_dicomVendorData" },

    // Well-known Frame of References
    { UID_ICBM452T1FrameOfReference,                           "ICBM452T1FrameOfReference" },
    { UID_ICBMSingleSubjectMRIFrameOfReference,                "ICBMSingleSubjectMRIFrameOfReference" },
    { UID_SPM2AVG152PDFrameOfReference,                        "SPM2AVG152PDFrameOfReference" },
    { UID_SPM2AVG152T1FrameOfReference,                        "SPM2AVG152T1FrameOfReference" },
    { UID_SPM2AVG152T2FrameOfReference,                        "SPM2AVG152T2FrameOfReference" },
    { UID_SPM2AVG305T1FrameOfReference,                        "SPM2AVG305T1FrameOfReference" },
    { UID_SPM2BRAINMASKFrameOfReference,                       "SPM2BRAINMASKFrameOfReference" },
    { UID_SPM2CSFFrameOfReference,                             "SPM2CSFFrameOfReference" },
    { UID_SPM2EPIFrameOfReference,                             "SPM2EPIFrameOfReference" },
    { UID_SPM2FILT1FrameOfReference,                           "SPM2FILT1FrameOfReference" },
    { UID_SPM2GRAYFrameOfReference,                            "SPM2GRAYFrameOfReference" },
    { UID_SPM2PDFrameOfReference,                              "SPM2PDFrameOfReference" },
    { UID_SPM2PETFrameOfReference,                             "SPM2PETFrameOfReference" },
    { UID_SPM2SINGLESUBJT1FrameOfReference,                    "SPM2SINGLESUBJT1FrameOfReference" },
    { UID_SPM2SPECTFrameOfReference,                           "SPM2SPECTFrameOfReference" },
    { UID_SPM2T1FrameOfReference,                              "SPM2T1FrameOfReference" },
    { UID_SPM2T2FrameOfReference,                              "SPM2T2FrameOfReference" },
    { UID_SPM2TRANSMFrameOfReference,                          "SPM2TRANSMFrameOfReference" },
    { UID_SPM2WHITEFrameOfReference,                           "SPM2WHITEFrameOfReference" },
    { UID_TalairachBrainAtlasFrameOfReference,                 "TalairachBrainAtlasFrameOfReference" },

    // UTC Synchronization Frame of Reference
    { UID_UniversalCoordinatedTimeSynchronizationFrameOfReference, "UniversalCoordinatedTimeSynchronizationFrameOfReference" },

    // Well-known SOP Instances for Color Palettes
    { UID_HotIronColorPaletteSOPInstance,                      "HotIronColorPaletteSOPInstance" },
    { UID_HotMetalBlueColorPaletteSOPInstance,                 "HotMetalBlueColorPaletteSOPInstance" },
    { UID_PET20StepColorPaletteSOPInstance,                    "PET20StepColorPaletteSOPInstance" },
    { UID_PETColorPaletteSOPInstance,                          "PETColorPaletteSOPInstance" },

    // Draft Supplements
    { UID_DRAFT_RTBeamsDeliveryInstructionStorage,             "DRAFT_RTBeamsDeliveryInstructionStorage" },
    { UID_DRAFT_RTConventionalMachineVerification,             "DRAFT_RTConventionalMachineVerification" },
    { UID_DRAFT_RTIonMachineVerification,                      "DRAFT_RTIonMachineVerification" },
    { UID_DRAFT_SRAudioStorage,                                "DRAFT_SRAudioStorage" },
    { UID_DRAFT_SRComprehensiveStorage,                        "DRAFT_SRComprehensiveStorage" },
    { UID_DRAFT_SRDetailStorage,                               "DRAFT_SRDetailStorage" },
    { UID_DRAFT_SRTextStorage,                                 "DRAFT_SRTextStorage" },
    { UID_DRAFT_WaveformStorage,                               "DRAFT_WaveformStorage" },
    { UID_DRAFT_UnifiedWorklistAndProcedureStepServiceClass,   "DRAFT_UnifiedWorklistAndProcedureStepServiceClass" },
    { UID_DRAFT_UnifiedProcedureStepPushSOPClass,              "DRAFT_UnifiedProcedureStepPushSOPClass" },
    { UID_DRAFT_UnifiedProcedureStepWatchSOPClass,             "DRAFT_UnifiedProcedureStepWatchSOPClass" },
    { UID_DRAFT_UnifiedProcedureStepPullSOPClass,              "DRAFT_UnifiedProcedureStepPullSOPClass" },
    { UID_DRAFT_UnifiedProcedureStepEventSOPClass,             "DRAFT_UnifiedProcedureStepEventSOPClass" },

    { NULL, NULL }
};

static const int uidNameMap_size = ( sizeof(uidNameMap) / sizeof(UIDNameMap) );


/** an array of const strings containing all known Storage SOP Classes
 *  that fit into the conventional PATIENT-STUDY-SERIES-INSTANCE information
 *  model, i.e. everything a Storage SCP might want to store in a PACS.
 *  Special cases such as hanging protocol storage or the Storage SOP Class
 *  are not included in this list.
 *
 *  THIS LIST CONTAINS ALL STORAGE SOP CLASSES INCLUDING RETIRED ONES
 *  AND IS LARGER THAN 64 ENTRIES.
 */

const char* dcmAllStorageSOPClassUIDs[] = {
    UID_AmbulatoryECGWaveformStorage,
    UID_ArterialPulseWaveformStorage,
    UID_AutorefractionMeasurementsStorage,
    UID_BasicStructuredDisplayStorage,
    UID_BasicTextSRStorage,
    UID_BasicVoiceAudioWaveformStorage,
    UID_BlendingSoftcopyPresentationStateStorage,
    UID_BreastProjectionXRayImageStorageForPresentation,
    UID_BreastProjectionXRayImageStorageForProcessing,
    UID_BreastTomosynthesisImageStorage,
    UID_CardiacElectrophysiologyWaveformStorage,
    UID_ChestCADSRStorage,
    UID_ColonCADSRStorage,
    UID_ColorSoftcopyPresentationStateStorage,
    UID_Comprehensive3DSRStorage,
    UID_ComprehensiveSRStorage,
    UID_ComputedRadiographyImageStorage,
    UID_CornealTopographyMapStorage,
    UID_CTImageStorage,
    UID_DeformableSpatialRegistrationStorage,
    UID_DigitalIntraOralXRayImageStorageForPresentation,
    UID_DigitalIntraOralXRayImageStorageForProcessing,
    UID_DigitalMammographyXRayImageStorageForPresentation,
    UID_DigitalMammographyXRayImageStorageForProcessing,
    UID_DigitalXRayImageStorageForPresentation,
    UID_DigitalXRayImageStorageForProcessing,
    UID_EncapsulatedCDAStorage,
    UID_EncapsulatedPDFStorage,
    UID_EnhancedCTImageStorage,
    UID_EnhancedMRColorImageStorage,
    UID_EnhancedMRImageStorage,
    UID_EnhancedPETImageStorage,
    UID_EnhancedSRStorage,
    UID_EnhancedUSVolumeStorage,
    UID_EnhancedXAImageStorage,
    UID_EnhancedXRFImageStorage,
    UID_GeneralAudioWaveformStorage,
    UID_GeneralECGWaveformStorage,
    UID_GenericImplantTemplateStorage,
    UID_GrayscaleSoftcopyPresentationStateStorage,
    UID_HemodynamicWaveformStorage,
    UID_ImplantAssemblyTemplateStorage,
    UID_ImplantationPlanSRDocumentStorage,
    UID_ImplantTemplateGroupStorage,
    UID_IntraocularLensCalculationsStorage,
    UID_IntravascularOpticalCoherenceTomographyImageStorageForPresentation,
    UID_IntravascularOpticalCoherenceTomographyImageStorageForProcessing,
    UID_KeratometryMeasurementsStorage,
    UID_KeyObjectSelectionDocumentStorage,
    UID_LegacyConvertedEnhancedCTImageStorage,
    UID_LegacyConvertedEnhancedMRImageStorage,
    UID_LegacyConvertedEnhancedPETImageStorage,
    UID_LensometryMeasurementsStorage,
    UID_MacularGridThicknessAndVolumeReportStorage,
    UID_MammographyCADSRStorage,
    UID_MRImageStorage,
    UID_MRSpectroscopyStorage,
    UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage,
    UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage,
    UID_MultiframeSingleBitSecondaryCaptureImageStorage,
    UID_MultiframeTrueColorSecondaryCaptureImageStorage,
    UID_NuclearMedicineImageStorage,
    UID_OphthalmicAxialMeasurementsStorage,
    UID_OphthalmicPhotography16BitImageStorage,
    UID_OphthalmicPhotography8BitImageStorage,
    UID_OphthalmicThicknessMapStorage,
    UID_OphthalmicTomographyImageStorage,
    UID_OphthalmicVisualFieldStaticPerimetryMeasurementsStorage,
    UID_PositronEmissionTomographyImageStorage,
    UID_ProcedureLogStorage,
    UID_PseudoColorSoftcopyPresentationStateStorage,
    UID_RawDataStorage,
    UID_RealWorldValueMappingStorage,
    UID_RespiratoryWaveformStorage,
    UID_RTBeamsDeliveryInstructionStorage,
    UID_RTBeamsTreatmentRecordStorage,
    UID_RTBrachyTreatmentRecordStorage,
    UID_RTDoseStorage,
    UID_RTImageStorage,
    UID_RTIonBeamsTreatmentRecordStorage,
    UID_RTIonPlanStorage,
    UID_RTPlanStorage,
    UID_RTStructureSetStorage,
    UID_RTTreatmentSummaryRecordStorage,
    UID_SecondaryCaptureImageStorage,
    UID_SegmentationStorage,
    UID_SpatialFiducialsStorage,
    UID_SpatialRegistrationStorage,
    UID_SpectaclePrescriptionReportStorage,
    UID_StereometricRelationshipStorage,
    UID_SubjectiveRefractionMeasurementsStorage,
    UID_SurfaceScanMeshStorage,
    UID_SurfaceScanPointCloudStorage,
    UID_SurfaceSegmentationStorage,
    UID_TwelveLeadECGWaveformStorage,
    UID_UltrasoundImageStorage,
    UID_UltrasoundMultiframeImageStorage,
    UID_VideoEndoscopicImageStorage,
    UID_VideoMicroscopicImageStorage,
    UID_VideoPhotographicImageStorage,
    UID_VisualAcuityMeasurementsStorage,
    UID_VLEndoscopicImageStorage,
    UID_VLMicroscopicImageStorage,
    UID_VLPhotographicImageStorage,
    UID_VLSlideCoordinatesMicroscopicImageStorage,
    UID_VLWholeSlideMicroscopyImageStorage,
    UID_XAXRFGrayscaleSoftcopyPresentationStateStorage,
    UID_XRay3DAngiographicImageStorage,
    UID_XRay3DCraniofacialImageStorage,
    UID_XRayAngiographicImageStorage,
    UID_XRayRadiationDoseSRStorage,
    UID_XRayRadiofluoroscopicImageStorage,
    // retired
    UID_RETIRED_HardcopyColorImageStorage,
    UID_RETIRED_HardcopyGrayscaleImageStorage,
    UID_RETIRED_NuclearMedicineImageStorage,
    UID_RETIRED_StandaloneCurveStorage,
    UID_RETIRED_StandaloneModalityLUTStorage,
    UID_RETIRED_StandaloneOverlayStorage,
    UID_RETIRED_StandalonePETCurveStorage,
    UID_RETIRED_StandaloneVOILUTStorage,
    UID_RETIRED_StoredPrintStorage,
    UID_RETIRED_UltrasoundImageStorage,
    UID_RETIRED_UltrasoundMultiframeImageStorage,
    UID_RETIRED_VLImageStorage,
    UID_RETIRED_VLMultiFrameImageStorage,
    UID_RETIRED_XRayAngiographicBiPlaneImageStorage,
    // draft
    UID_DRAFT_SRAudioStorage,
    UID_DRAFT_SRComprehensiveStorage,
    UID_DRAFT_SRDetailStorage,
    UID_DRAFT_SRTextStorage,
    UID_DRAFT_WaveformStorage,
    UID_DRAFT_RTBeamsDeliveryInstructionStorage,
    // DICOS
    UID_DICOS_CTImageStorage,
    UID_DICOS_DigitalXRayImageStorageForPresentation,
    UID_DICOS_DigitalXRayImageStorageForProcessing,
    UID_DICOS_ThreatDetectionReportStorage,
    // DICONDE
    UID_DICONDE_EddyCurrentImageStorage,
    UID_DICONDE_EddyCurrentMultiframeImageStorage,
    NULL
};

const int numberOfAllDcmStorageSOPClassUIDs = (sizeof(dcmAllStorageSOPClassUIDs) / sizeof(const char*)) - 1;


/*  an array of const strings containing all storage SOP classes that
 *  are proposed by default by those Storage SCU components in DCMTK
 *  that always propose one presentation context for each SOP class,
 *  e.g. movescu or dcmqrdb. This list is guaranteed to have at most
 *  120 entries (currently: 120) to leave enough room for FIND/MOVE
 *  presentation contexts.
 */
const char* dcmLongSCUStorageSOPClassUIDs[] = {
    UID_AmbulatoryECGWaveformStorage,
    UID_ArterialPulseWaveformStorage,
    UID_AutorefractionMeasurementsStorage,
    UID_BasicStructuredDisplayStorage,
    UID_BasicTextSRStorage,
    UID_BasicVoiceAudioWaveformStorage,
    UID_BlendingSoftcopyPresentationStateStorage,
    UID_BreastTomosynthesisImageStorage,
    UID_CardiacElectrophysiologyWaveformStorage,
    UID_ChestCADSRStorage,
    UID_ColonCADSRStorage,
    UID_ColorSoftcopyPresentationStateStorage,
    UID_Comprehensive3DSRStorage,
    UID_ComprehensiveSRStorage,
    UID_ComputedRadiographyImageStorage,
    UID_CTImageStorage,
    UID_DeformableSpatialRegistrationStorage,
    UID_DigitalIntraOralXRayImageStorageForPresentation,
    UID_DigitalIntraOralXRayImageStorageForProcessing,
    UID_DigitalMammographyXRayImageStorageForPresentation,
    UID_DigitalMammographyXRayImageStorageForProcessing,
    UID_DigitalXRayImageStorageForPresentation,
    UID_DigitalXRayImageStorageForProcessing,
    UID_EncapsulatedCDAStorage,
    UID_EncapsulatedPDFStorage,
    UID_EnhancedCTImageStorage,
    UID_EnhancedMRColorImageStorage,
    UID_EnhancedMRImageStorage,
    UID_EnhancedPETImageStorage,
    UID_EnhancedSRStorage,
    UID_EnhancedUSVolumeStorage,
    UID_EnhancedXAImageStorage,
    UID_EnhancedXRFImageStorage,
    UID_GeneralAudioWaveformStorage,
    UID_GeneralECGWaveformStorage,
    UID_GenericImplantTemplateStorage,
    UID_GrayscaleSoftcopyPresentationStateStorage,
    UID_HemodynamicWaveformStorage,
    UID_ImplantAssemblyTemplateStorage,
    UID_ImplantationPlanSRDocumentStorage,
    UID_ImplantTemplateGroupStorage,
    UID_IntraocularLensCalculationsStorage,
    UID_IntravascularOpticalCoherenceTomographyImageStorageForPresentation,
    UID_IntravascularOpticalCoherenceTomographyImageStorageForProcessing,
    UID_KeratometryMeasurementsStorage,
    UID_KeyObjectSelectionDocumentStorage,
    UID_LensometryMeasurementsStorage,
    UID_MacularGridThicknessAndVolumeReportStorage,
    UID_MammographyCADSRStorage,
    UID_MRImageStorage,
    UID_MRSpectroscopyStorage,
    UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage,
    UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage,
    UID_MultiframeSingleBitSecondaryCaptureImageStorage,
    UID_MultiframeTrueColorSecondaryCaptureImageStorage,
    UID_NuclearMedicineImageStorage,
    UID_OphthalmicAxialMeasurementsStorage,
    UID_OphthalmicPhotography16BitImageStorage,
    UID_OphthalmicPhotography8BitImageStorage,
    UID_OphthalmicThicknessMapStorage,
    UID_OphthalmicTomographyImageStorage,
    UID_OphthalmicVisualFieldStaticPerimetryMeasurementsStorage,
    UID_PositronEmissionTomographyImageStorage,
    UID_ProcedureLogStorage,
    UID_PseudoColorSoftcopyPresentationStateStorage,
    UID_RawDataStorage,
    UID_RealWorldValueMappingStorage,
    UID_RespiratoryWaveformStorage,
    UID_RTBeamsDeliveryInstructionStorage,
    UID_RTBeamsTreatmentRecordStorage,
    UID_RTBrachyTreatmentRecordStorage,
    UID_RTDoseStorage,
    UID_RTImageStorage,
    UID_RTIonBeamsTreatmentRecordStorage,
    UID_RTIonPlanStorage,
    UID_RTPlanStorage,
    UID_RTStructureSetStorage,
    UID_RTTreatmentSummaryRecordStorage,
    UID_SecondaryCaptureImageStorage,
    UID_SegmentationStorage,
    UID_SpatialFiducialsStorage,
    UID_SpatialRegistrationStorage,
    UID_SpectaclePrescriptionReportStorage,
    UID_StereometricRelationshipStorage,
    UID_SubjectiveRefractionMeasurementsStorage,
    UID_SurfaceScanMeshStorage,
    UID_SurfaceScanPointCloudStorage,
    UID_SurfaceSegmentationStorage,
    UID_TwelveLeadECGWaveformStorage,
    UID_UltrasoundImageStorage,
    UID_UltrasoundMultiframeImageStorage,
    UID_VideoEndoscopicImageStorage,
    UID_VideoMicroscopicImageStorage,
    UID_VideoPhotographicImageStorage,
    UID_VisualAcuityMeasurementsStorage,
    UID_VLEndoscopicImageStorage,
    UID_VLMicroscopicImageStorage,
    UID_VLPhotographicImageStorage,
    UID_VLSlideCoordinatesMicroscopicImageStorage,
    UID_VLWholeSlideMicroscopyImageStorage,
    UID_XAXRFGrayscaleSoftcopyPresentationStateStorage,
    UID_XRay3DAngiographicImageStorage,
    UID_XRay3DCraniofacialImageStorage,
    UID_XRayAngiographicImageStorage,
    UID_XRayRadiationDoseSRStorage,
    UID_XRayRadiofluoroscopicImageStorage,
    // recently approved
//  UID_BreastProjectionXRayImageStorageForPresentation,
//  UID_BreastProjectionXRayImageStorageForProcessing,
//  UID_CornealTopographyMapStorage,
//  UID_LegacyConvertedEnhancedCTImageStorage,
//  UID_LegacyConvertedEnhancedMRImageStorage,
//  UID_LegacyConvertedEnhancedPETImageStorage,
    // retired
    UID_RETIRED_HardcopyColorImageStorage,
    UID_RETIRED_HardcopyGrayscaleImageStorage,
    UID_RETIRED_NuclearMedicineImageStorage,
    UID_RETIRED_StandaloneCurveStorage,
    UID_RETIRED_StandaloneModalityLUTStorage,
    UID_RETIRED_StandaloneOverlayStorage,
    UID_RETIRED_StandalonePETCurveStorage,
    UID_RETIRED_StandaloneVOILUTStorage,
    UID_RETIRED_StoredPrintStorage,
    UID_RETIRED_UltrasoundImageStorage,
    UID_RETIRED_UltrasoundMultiframeImageStorage,
    UID_RETIRED_VLImageStorage,
    UID_RETIRED_VLMultiFrameImageStorage,
    UID_RETIRED_XRayAngiographicBiPlaneImageStorage,
    // draft
//  UID_DRAFT_RTBeamsDeliveryInstructionStorage,
//  UID_DRAFT_SRAudioStorage,
//  UID_DRAFT_SRComprehensiveStorage,
//  UID_DRAFT_SRDetailStorage,
//  UID_DRAFT_SRTextStorage,
//  UID_DRAFT_WaveformStorage,
    // DICOS
//  UID_DICOS_CTImageStorage,
//  UID_DICOS_DigitalXRayImageStorageForPresentation,
//  UID_DICOS_DigitalXRayImageStorageForProcessing,
//  UID_DICOS_ThreatDetectionReportStorage,
    // DICONDE
//  UID_DICONDE_EddyCurrentImageStorage,
//  UID_DICONDE_EddyCurrentMultiframeImageStorage,
    NULL
};

const int numberOfDcmLongSCUStorageSOPClassUIDs = (sizeof(dcmLongSCUStorageSOPClassUIDs) / sizeof(const char*)) - 1;


/** an array of const strings containing all storage SOP classes that
 *  are proposed by default by those Storage SCU components in DCMTK
 *  that always propose TWO presentation context for each SOP class,
 *  e.g. storescu. This list is guaranteed to have at most 64 entries.
 */
const char* dcmShortSCUStorageSOPClassUIDs[] = {
    /* This list *must* be limited to 64 SOP classes or less (currently: 64).
     * If we have more than 64 storage transfer syntaxes, tools such as
     * storescu will fail because they attempt to negotiate two presentation
     * contexts for each SOP class, and there is a total limit of 128 contexts
     * for one association.
     * Because of this limitation, all draft and retired storage SOP classes
     * are removed from this list. We have also omitted support for some
     * recently approved SOP classes.
     * UID_MediaStorageDirectoryStorage should not be present in this list.
     */
    UID_AmbulatoryECGWaveformStorage,
    UID_BasicTextSRStorage,
    UID_BasicVoiceAudioWaveformStorage,
    UID_BlendingSoftcopyPresentationStateStorage,
    UID_CardiacElectrophysiologyWaveformStorage,
    UID_ChestCADSRStorage,
    UID_ColonCADSRStorage,
    UID_ColorSoftcopyPresentationStateStorage,
    UID_ComprehensiveSRStorage,
    UID_ComputedRadiographyImageStorage,
    UID_CTImageStorage,
    UID_DigitalIntraOralXRayImageStorageForPresentation,
    UID_DigitalIntraOralXRayImageStorageForProcessing,
    UID_DigitalMammographyXRayImageStorageForPresentation,
    UID_DigitalMammographyXRayImageStorageForProcessing,
    UID_DigitalXRayImageStorageForPresentation,
    UID_DigitalXRayImageStorageForProcessing,
    UID_EncapsulatedPDFStorage,
    UID_EnhancedCTImageStorage,
    UID_EnhancedMRImageStorage,
    UID_EnhancedSRStorage,
    UID_EnhancedXAImageStorage,
    UID_EnhancedXRFImageStorage,
    UID_GeneralECGWaveformStorage,
    UID_GrayscaleSoftcopyPresentationStateStorage,
    UID_HemodynamicWaveformStorage,
    UID_KeyObjectSelectionDocumentStorage,
    UID_MammographyCADSRStorage,
    UID_MRImageStorage,
    UID_MRSpectroscopyStorage,
    UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage,
    UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage,
    UID_MultiframeSingleBitSecondaryCaptureImageStorage,
    UID_MultiframeTrueColorSecondaryCaptureImageStorage,
    UID_NuclearMedicineImageStorage,
    UID_OphthalmicPhotography16BitImageStorage,
    UID_OphthalmicPhotography8BitImageStorage,
    UID_OphthalmicTomographyImageStorage,
    UID_PositronEmissionTomographyImageStorage,
    UID_ProcedureLogStorage,
    UID_PseudoColorSoftcopyPresentationStateStorage,
    UID_RawDataStorage,
    UID_RealWorldValueMappingStorage,
    UID_RTBeamsTreatmentRecordStorage,
    UID_RTBrachyTreatmentRecordStorage,
    UID_RTDoseStorage,
    UID_RTImageStorage,
    UID_RTPlanStorage,
    UID_RTStructureSetStorage,
    UID_RTTreatmentSummaryRecordStorage,
    UID_SecondaryCaptureImageStorage,
    UID_SpatialFiducialsStorage,
    UID_SpatialRegistrationStorage,
    UID_StereometricRelationshipStorage,
    UID_TwelveLeadECGWaveformStorage,
    UID_UltrasoundImageStorage,
    UID_UltrasoundMultiframeImageStorage,
    UID_VLEndoscopicImageStorage,
    UID_VLMicroscopicImageStorage,
    UID_VLPhotographicImageStorage,
    UID_VLSlideCoordinatesMicroscopicImageStorage,
    UID_XRayAngiographicImageStorage,
    UID_XRayRadiationDoseSRStorage,
    UID_XRayRadiofluoroscopicImageStorage,
    NULL
};

const int numberOfDcmShortSCUStorageSOPClassUIDs = (sizeof(dcmShortSCUStorageSOPClassUIDs) / sizeof(const char*)) - 1;


/*
** The global variable dcmImageSOPClassUIDs is an array of
** string pointers containing the UIDs of all known Image SOP
** Classes.  The instances of SOP Classes in this list can be
** referenced from DICOMDIR IMAGE records.
**
** The dcmgpdir/dcmmkdir program uses this list to determine
** what kind of objects can be referenced from IMAGE records.
** Be _very_ careful when adding SOP Classes to this list!!
**
** The global variable numberOfDcmImageSOPClassUIDs defines the
** size of the array.
*/
const char* dcmImageSOPClassUIDs[] = {
    UID_BreastProjectionXRayImageStorageForPresentation,
    UID_BreastProjectionXRayImageStorageForProcessing,
    UID_BreastTomosynthesisImageStorage,
    UID_ComputedRadiographyImageStorage,
    UID_CTImageStorage,
    UID_CornealTopographyMapStorage,
    UID_DigitalIntraOralXRayImageStorageForPresentation,
    UID_DigitalIntraOralXRayImageStorageForProcessing,
    UID_DigitalMammographyXRayImageStorageForPresentation,
    UID_DigitalMammographyXRayImageStorageForProcessing,
    UID_DigitalXRayImageStorageForPresentation,
    UID_DigitalXRayImageStorageForProcessing,
    UID_EnhancedCTImageStorage,
    UID_EnhancedMRColorImageStorage,
    UID_EnhancedMRImageStorage,
    UID_EnhancedPETImageStorage,
    UID_EnhancedUSVolumeStorage,
    UID_EnhancedXAImageStorage,
    UID_EnhancedXRFImageStorage,
    UID_IntravascularOpticalCoherenceTomographyImageStorageForPresentation,
    UID_IntravascularOpticalCoherenceTomographyImageStorageForProcessing,
    UID_LegacyConvertedEnhancedCTImageStorage,
    UID_LegacyConvertedEnhancedMRImageStorage,
    UID_LegacyConvertedEnhancedPETImageStorage,
    UID_MRImageStorage,
    UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage,
    UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage,
    UID_MultiframeSingleBitSecondaryCaptureImageStorage,
    UID_MultiframeTrueColorSecondaryCaptureImageStorage,
    UID_NuclearMedicineImageStorage,
    UID_OphthalmicPhotography16BitImageStorage,
    UID_OphthalmicPhotography8BitImageStorage,
    UID_OphthalmicThicknessMapStorage,
    UID_OphthalmicTomographyImageStorage,
    UID_PositronEmissionTomographyImageStorage,
    UID_RTImageStorage,
    UID_SecondaryCaptureImageStorage,
    UID_UltrasoundImageStorage,
    UID_UltrasoundMultiframeImageStorage,
    UID_VideoEndoscopicImageStorage,
    UID_VideoMicroscopicImageStorage,
    UID_VideoPhotographicImageStorage,
    UID_VLEndoscopicImageStorage,
    UID_VLMicroscopicImageStorage,
    UID_VLPhotographicImageStorage,
    UID_VLSlideCoordinatesMicroscopicImageStorage,
    UID_VLWholeSlideMicroscopyImageStorage,
    UID_XRay3DAngiographicImageStorage,
    UID_XRay3DCraniofacialImageStorage,
    UID_XRayAngiographicImageStorage,
    UID_XRayRadiofluoroscopicImageStorage,
    // retired
    UID_RETIRED_HardcopyColorImageStorage,
    UID_RETIRED_HardcopyGrayscaleImageStorage,
    UID_RETIRED_NuclearMedicineImageStorage,
    UID_RETIRED_UltrasoundImageStorage,
    UID_RETIRED_UltrasoundMultiframeImageStorage,
    UID_RETIRED_VLImageStorage,
    UID_RETIRED_VLMultiFrameImageStorage,
    UID_RETIRED_XRayAngiographicBiPlaneImageStorage,
    // DICOS
    UID_DICOS_CTImageStorage,
    UID_DICOS_DigitalXRayImageStorageForPresentation,
    UID_DICOS_DigitalXRayImageStorageForProcessing,
    // DICONDE
    UID_DICONDE_EddyCurrentImageStorage,
    UID_DICONDE_EddyCurrentMultiframeImageStorage,
    NULL
};

const int numberOfDcmImageSOPClassUIDs = (sizeof(dcmImageSOPClassUIDs) / sizeof(const char*)) - 1;


typedef struct {
    const char *sopClass;
    const char *modality;
    unsigned long averageSize;  /* can be way, way out */
} DcmModalityTable;

/*
** The modalities table defines a short character code for each
** Storage SOP Class for use in filenames.
** It also gives a typical size for each SOP Instance.  This will
** usually be way out, but is useful in user interfaces to give an
** idea of progress when receiving an image (C-STORE does not indicate
** the size of an image being transmitted).
*/
static const DcmModalityTable modalities[] = {
    { UID_AmbulatoryECGWaveformStorage,                            "ECA", 4096 },
    { UID_ArterialPulseWaveformStorage,                            "WVa", 4096 },
    { UID_AutorefractionMeasurementsStorage,                       "OPa", 4096 },
    { UID_BasicStructuredDisplayStorage,                           "SD",  4096 },
    { UID_BasicTextSRStorage,                                      "SRt", 4096 },
    { UID_BasicVoiceAudioWaveformStorage,                          "AUV", 4096 },
    { UID_BlendingSoftcopyPresentationStateStorage,                "PSb", 4096 },
    { UID_BreastProjectionXRayImageStorageForPresentation,         "BX",  4096 * 4096 * 2 },
    { UID_BreastProjectionXRayImageStorageForProcessing,           "BP",  4096 * 4096 * 2 },
    { UID_BreastTomosynthesisImageStorage,                         "BT",  4096 * 4096 * 2 },
    { UID_CardiacElectrophysiologyWaveformStorage,                 "WVc", 4096 },
    { UID_ChestCADSRStorage,                                       "SRh", 4096 },
    { UID_ColonCADSRStorage,                                       "SRo", 4096 },
    { UID_ColorSoftcopyPresentationStateStorage,                   "PSc", 4096 },
    { UID_Comprehensive3DSRStorage,                                "SR3", 4096 },
    { UID_ComprehensiveSRStorage,                                  "SRc", 4096 },
    { UID_ComputedRadiographyImageStorage,                         "CR",  2048 * 2048 * 2 },
    { UID_CornealTopographyMapStorage,                             "CM",  512 * 512 },
    { UID_CTImageStorage,                                          "CT",  512 * 512 * 2 },
    { UID_DeformableSpatialRegistrationStorage,                    "RGd", 4096 },
    { UID_DigitalIntraOralXRayImageStorageForPresentation,         "DXo", 1024 * 1024 * 2 },
    { UID_DigitalIntraOralXRayImageStorageForProcessing,           "DPo", 1024 * 1024 * 2 },
    { UID_DigitalMammographyXRayImageStorageForPresentation,       "DXm", 4096 * 4096 * 2 },
    { UID_DigitalMammographyXRayImageStorageForProcessing,         "DPm", 4096 * 4096 * 2 },
    { UID_DigitalXRayImageStorageForPresentation,                  "DX",  2048 * 2048 * 2 },
    { UID_DigitalXRayImageStorageForProcessing,                    "DP",  2048 * 2048 * 2 },
    { UID_EncapsulatedCDAStorage,                                  "CDA", 4096 },
    { UID_EncapsulatedPDFStorage,                                  "PDF", 1024 * 1024 },
    { UID_EnhancedCTImageStorage,                                  "CTe", 256 * 512 * 512 },
    { UID_EnhancedMRColorImageStorage,                             "MRc", 256 * 512 * 512 * 3 },
    { UID_EnhancedMRImageStorage,                                  "MRe", 256 * 512 * 512 },
    { UID_EnhancedPETImageStorage,                                 "PIe", 512 * 512 * 2 },
    { UID_EnhancedSRStorage,                                       "SRe", 4096 },
    { UID_EnhancedUSVolumeStorage,                                 "USe", 512 * 512 },
    { UID_EnhancedXAImageStorage,                                  "XAe", 256 * 512 * 512 },
    { UID_EnhancedXRFImageStorage,                                 "RFe", 256 * 512 * 512 },
    { UID_GeneralAudioWaveformStorage,                             "AUG", 4096 },
    { UID_GeneralECGWaveformStorage,                               "ECG", 4096 },
    { UID_GenericImplantTemplateStorage,                           "IT",  4096 },
    { UID_GrayscaleSoftcopyPresentationStateStorage,               "PSg", 4096 },
    { UID_HemodynamicWaveformStorage,                              "WVh", 4096 },
    { UID_ImplantAssemblyTemplateStorage,                          "ITa", 4096 },
    { UID_ImplantationPlanSRDocumentStorage,                       "SRi", 4096 },
    { UID_ImplantTemplateGroupStorage,                             "ITg", 4096 },
    { UID_IntraocularLensCalculationsStorage,                      "OPc", 4096 },
    { UID_IntravascularOpticalCoherenceTomographyImageStorageForPresentation, "OCt", 512 * 512 },
    { UID_IntravascularOpticalCoherenceTomographyImageStorageForProcessing, "OCp", 512 * 512 },
    { UID_KeratometryMeasurementsStorage,                          "OPk", 4096 },
    { UID_KeyObjectSelectionDocumentStorage,                       "KO",  4096 },
    { UID_LegacyConvertedEnhancedCTImageStorage,                   "CTl", 512 * 512 * 2 },
    { UID_LegacyConvertedEnhancedMRImageStorage,                   "MRl", 256 * 256 * 2 },
    { UID_LegacyConvertedEnhancedPETImageStorage,                  "PIl", 512 * 512 * 2 },
    { UID_LensometryMeasurementsStorage,                           "OPl", 4096 },
    { UID_MacularGridThicknessAndVolumeReportStorage,              "SRg", 4096 },
    { UID_MammographyCADSRStorage,                                 "SRm", 4096 },
    { UID_MRImageStorage,                                          "MR",  256 * 256 * 2 },
    { UID_MRSpectroscopyStorage,                                   "MRs", 256 * 512 * 512 },
    { UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage,     "SCb", 512 * 512 },
    { UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage,     "SCw", 512 * 512 * 2 },
    { UID_MultiframeSingleBitSecondaryCaptureImageStorage,         "SCs", 1024 * 1024 },  /* roughly an A4 300dpi scan */
    { UID_MultiframeTrueColorSecondaryCaptureImageStorage,         "SCc", 512 * 512 * 3 },
    { UID_NuclearMedicineImageStorage,                             "NM",  64 * 64 * 2 },
    { UID_OphthalmicAxialMeasurementsStorage,                      "OPx", 4096 },
    { UID_OphthalmicPhotography16BitImageStorage,                  "OPw", 768 * 576 * 6 },
    { UID_OphthalmicPhotography8BitImageStorage,                   "OPb", 768 * 576 * 3 },
    { UID_OphthalmicThicknessMapStorage,                           "OPm", 768 * 576 },
    { UID_OphthalmicTomographyImageStorage,                        "OPt", 768 * 576 * 3 },
    { UID_OphthalmicVisualFieldStaticPerimetryMeasurementsStorage, "OPp", 4096 },
    { UID_PositronEmissionTomographyImageStorage,                  "PI",  512 * 512 * 2 },
    { UID_ProcedureLogStorage,                                     "SRp", 4096 },
    { UID_PseudoColorSoftcopyPresentationStateStorage,             "PSp", 4096 },
    { UID_RawDataStorage,                                          "RAW", 512 * 512 * 256 },
    { UID_RealWorldValueMappingStorage,                            "RWM", 4096 },
    { UID_RespiratoryWaveformStorage,                              "WVr", 4096 },
    { UID_RTBeamsDeliveryInstructionStorage,                       "RTd", 4096 },
    { UID_RTBeamsTreatmentRecordStorage,                           "RTb", 4096 },
    { UID_RTBrachyTreatmentRecordStorage,                          "RTr", 4096 },
    { UID_RTDoseStorage,                                           "RD",  4096 },
    { UID_RTImageStorage,                                          "RI",  4096 },
    { UID_RTIonBeamsTreatmentRecordStorage,                        "RTi", 4096 },
    { UID_RTIonPlanStorage,                                        "RPi", 4096 },
    { UID_RTPlanStorage,                                           "RP" , 4096 },
    { UID_RTStructureSetStorage,                                   "RS",  4096 },
    { UID_RTTreatmentSummaryRecordStorage,                         "RTs", 4096 },
    { UID_SecondaryCaptureImageStorage,                            "SC",  512 * 512 * 2 },
    { UID_SegmentationStorage,                                     "SG",  4096 },
    { UID_SpatialFiducialsStorage,                                 "FID", 4096 },
    { UID_SpatialRegistrationStorage,                              "RGs", 4096 },
    { UID_SpectaclePrescriptionReportStorage,                      "SRs", 4096 },
    { UID_StereometricRelationshipStorage,                         "OPr", 4096 },
    { UID_SubjectiveRefractionMeasurementsStorage,                 "OPs", 4096 },
    { UID_SurfaceScanMeshStorage,                                  "SSm", 4096 },
    { UID_SurfaceScanPointCloudStorage,                            "SSp", 4096 },
    { UID_SurfaceSegmentationStorage,                              "SGs", 4096 },
    { UID_TwelveLeadECGWaveformStorage,                            "TLE", 4096 },
    { UID_UltrasoundImageStorage,                                  "US",  512 * 512 },
    { UID_UltrasoundMultiframeImageStorage,                        "USm", 512 * 512 },
    { UID_VideoEndoscopicImageStorage,                             "VVe", 768 * 576 * 3 },
    { UID_VideoMicroscopicImageStorage,                            "VVm", 768 * 576 * 3 },
    { UID_VideoPhotographicImageStorage,                           "VVp", 768 * 576 * 3 },
    { UID_VisualAcuityMeasurementsStorage,                         "OPv", 4096 },
    { UID_VLEndoscopicImageStorage,                                "VLe", 768 * 576 * 3 },
    { UID_VLMicroscopicImageStorage,                               "VLm", 768 * 576 * 3 },
    { UID_VLPhotographicImageStorage,                              "VLp", 768 * 576 * 3 },
    { UID_VLSlideCoordinatesMicroscopicImageStorage,               "VLs", 768 * 576 * 3 },
    { UID_VLWholeSlideMicroscopyImageStorage,                      "VLw", 10000 * 10000 * 3},
    { UID_XAXRFGrayscaleSoftcopyPresentationStateStorage,          "PSx", 4096 },
    { UID_XRay3DAngiographicImageStorage,                          "XA3", 256 * 512 * 512 },
    { UID_XRay3DCraniofacialImageStorage,                          "XC3", 256 * 512 * 512 },
    { UID_XRayAngiographicImageStorage,                            "XA",  256 * 512 * 512 },
    { UID_XRayRadiationDoseSRStorage,                              "SRd", 4096 },
    { UID_XRayRadiofluoroscopicImageStorage,                       "RF",  256 * 512 * 512 },
    // retired
    { UID_RETIRED_HardcopyColorImageStorage,                       "HC",  4096 },
    { UID_RETIRED_HardcopyGrayscaleImageStorage,                   "HG",  4096 },
    { UID_RETIRED_NuclearMedicineImageStorage,                     "NMr", 64 * 64 * 2 },
    { UID_RETIRED_StandaloneCurveStorage,                          "CV",  4096 },
    { UID_RETIRED_StandaloneModalityLUTStorage,                    "ML",  4096 * 2 },
    { UID_RETIRED_StandaloneOverlayStorage,                        "OV",  512 * 512 },
    { UID_RETIRED_StandalonePETCurveStorage,                       "PC",  4096 },
    { UID_RETIRED_StandaloneVOILUTStorage,                         "VO",  4096 * 2 },
    { UID_RETIRED_StoredPrintStorage,                              "SP",  4096 },
    { UID_RETIRED_UltrasoundImageStorage,                          "USr", 512 * 512 },
    { UID_RETIRED_UltrasoundMultiframeImageStorage,                "USf", 512 * 512 },
    { UID_RETIRED_VLImageStorage,                                  "VLr", 768 * 576 * 3 },
    { UID_RETIRED_VLMultiFrameImageStorage,                        "VMr", 768 * 576 * 3 },
    { UID_RETIRED_XRayAngiographicBiPlaneImageStorage,             "XB",  512 * 512 * 2 },
    // draft
    { UID_DRAFT_RTBeamsDeliveryInstructionStorage,                 "RBd", 4096 },
    { UID_DRAFT_SRAudioStorage,                                    "SRw", 4096 },
    { UID_DRAFT_SRComprehensiveStorage,                            "SRx", 4096 },
    { UID_DRAFT_SRDetailStorage,                                   "SRy", 4096 },
    { UID_DRAFT_SRTextStorage,                                     "SRz", 4096 },
    { UID_DRAFT_WaveformStorage,                                   "WVd", 4096 },
    // DICOS
    { UID_DICOS_CTImageStorage,                                    "CTs", 512 * 512 * 2 },
    { UID_DICOS_DigitalXRayImageStorageForPresentation,            "DXs", 2048 * 2048 * 2 },
    { UID_DICOS_DigitalXRayImageStorageForProcessing,              "DPs", 2048 * 2048 * 2 },
    { UID_DICOS_ThreatDetectionReportStorage,                      "TDR", 4096 },
    // DICONDE
    { UID_DICONDE_EddyCurrentImageStorage,                         "EC",  512 * 512 },
    { UID_DICONDE_EddyCurrentMultiframeImageStorage,               "ECm", 512 * 512 }
};

static const int numberOfDcmModalityTableEntries = (sizeof(modalities) / sizeof(DcmModalityTable));


/*
 * Public Function Prototypes
 */


const char *dcmSOPClassUIDToModality(const char *sopClassUID,
                                     const char *defaultValue)
{
    if (sopClassUID == NULL) return NULL;
    /* check for known SOP class */
    for (int i = 0; i < numberOfDcmModalityTableEntries; i++)
    {
      if (strcmp(modalities[i].sopClass, sopClassUID) == 0) return modalities[i].modality;
    }
    /* SOP class not found */
    return defaultValue;
}

unsigned long dcmGuessModalityBytes(const char *sopClassUID)
{
    unsigned long nbytes = 1048576; /* default: 1 MByte */

    if (sopClassUID == NULL) return nbytes;

    int found=0;
    for (int i = 0; (!found && (i < numberOfDcmModalityTableEntries)); i++)
    {
      found = (strcmp(modalities[i].sopClass, sopClassUID) == 0);
      if (found) nbytes = modalities[i].averageSize;
    }

    return nbytes;
}


/*
** dcmFindNameOfUID(const char* uid)
** Return the name of a UID.
** Performs a table lookup and returns a pointer to a read-only string.
** Returns defaultValue of the UID is not known.
*/

const char*
dcmFindNameOfUID(const char* uid, const char* defaultValue)
{
    int i = 0;
    if (uid == NULL) return defaultValue;
    for (i=0; i<uidNameMap_size; i++) {
      if (uidNameMap[i].uid != NULL && strcmp(uid, uidNameMap[i].uid) == 0) {
        return uidNameMap[i].name;
      }
    }
    return defaultValue;
}

//
// dcmFindUIDFromName(const char* name)
// Return the UID of a name.
// Performs a table lookup and returns a pointer to a read-only string.
// Returns NULL of the name is not known.
//

const char *
dcmFindUIDFromName(const char * name)
{
    if (name == NULL) return NULL;
    for(int i = 0; i < uidNameMap_size; i++)
    {
      if (uidNameMap[i].name != NULL && strcmp(name, uidNameMap[i].name) == 0)
        return uidNameMap[i].uid;
    }
    return NULL;
}


/*
** dcmIsaStorageSOPClassUID(const char* uid)
** Returns true if the uid is one of the Storage SOP Classes.
** Performs a table lookup in the dcmAllStorageSOPClassUIDs table.
*/
OFBool
dcmIsaStorageSOPClassUID(const char* uid)
{
    int i = 0;
    if (uid == NULL) return OFFalse;
    for (i=0; i<numberOfAllDcmStorageSOPClassUIDs; i++) {
      if (dcmAllStorageSOPClassUIDs[i] != NULL && strcmp(uid, dcmAllStorageSOPClassUIDs[i]) == 0) {
        return OFTrue;
      }
    }
    return OFFalse;
}

// ********************************

#ifndef HAVE_GETHOSTID
#ifdef HAVE_SYSINFO

#include <sys/systeminfo.h>
static long gethostid(void)
{
    char buf[256];
    if (sysinfo(SI_HW_SERIAL, buf, 128) == -1) {
      DCMDATA_FATAL("sysinfo: " << OFStandard::strerror(errno, buf, sizeof(buf)));
      exit(1);
    }
#ifdef HAVE_STRTOUL
    return(strtoul(buf, NULL, 0));
#else
    long result;
    sscanf(buf, "%ld", &result);
    return result;
#endif
}

#else // !HAVE_SYSINFO

/*
** There is no implementation of gethostid() and we cannot implement it in
** terms of sysinfo() so define a workaround.
*/
#if (defined(HAVE_GETHOSTNAME) && defined(HAVE_GETHOSTBYNAME)) || defined(HAVE_WINDOWS_H)

// 16K should be large enough to handle everything pointed to by a struct hostent
#define GETHOSTBYNAME_R_BUFSIZE 16384

/* On Windows systems specify a routine to determine the MAC address of the Ethernet adapter */
/* Special handling for MinGW which does not yet (as of MinGW 2.0) support snmp.h */
#if defined(HAVE_WINDOWS_H) && !defined(__MINGW32__)

#include <snmp.h>

// Visual C++ prior to version 6 declared different type names
// in <snmp.h>, so we need to define a mapping
#if _MSC_VER < 1200
typedef RFC1157VarBind     SnmpVarBind;
typedef RFC1157VarBindList SnmpVarBindList;
typedef AsnInteger         AsnInteger32;
#define SNMP_PDU_GETNEXT   ASN_RFC1157_GETNEXTREQUEST
#endif


typedef int(WINAPI *pSnmpUtilOidCpy) (
        OUT AsnObjectIdentifier *pOidDst,
        IN AsnObjectIdentifier *pOidSrc);

typedef int(WINAPI *pSnmpUtilOidNCmp) (
        IN AsnObjectIdentifier *pOid1,
        IN AsnObjectIdentifier *pOid2,
        IN UINT nSubIds);

typedef void(WINAPI *pSnmpUtilVarBindFree) (
        IN OUT SnmpVarBind *pVb);

typedef bool(WINAPI *pSnmpExtensionInit) (
        IN DWORD dwTimeZeroReference,
        OUT HANDLE *hPollForTrapEvent,
        OUT AsnObjectIdentifier *supportedView);

typedef bool(WINAPI *pSnmpExtensionTrap) (
        OUT AsnObjectIdentifier *enterprise,
        OUT AsnInteger32 *genericTrap,
        OUT AsnInteger32 *specificTrap,
        OUT AsnTimeticks *timeStamp,
        OUT SnmpVarBindList *variableBindings);

typedef bool(WINAPI *pSnmpExtensionQuery) (
        IN BYTE requestType,
        IN OUT SnmpVarBindList *variableBindings,
        OUT AsnInteger32 *errorStatus,
        OUT AsnInteger32 *errorIndex);

typedef bool(WINAPI *pSnmpExtensionInitEx) (
        OUT AsnObjectIdentifier *supportedView);

typedef struct _ASTAT_
{
    ADAPTER_STATUS adapt;
    NAME_BUFFER    NameBuff[30];
} ASTAT, *PASTAT;

/* get the MAC address of the (first) Ethernet adapter (6 bytes) */
static unsigned char *getMACAddress(unsigned char buffer[6])
{
    OFBool success = OFFalse;
    /* init return variable */
    memzero(buffer, 6 * sizeof(unsigned char));
    NCB ncb;
    memzero(&ncb, sizeof(ncb));
    /* reset the LAN adapter */
    ncb.ncb_command = NCBRESET;
    /* it is considered bad practice to hardcode the LANA number (should enumerate
       adapters first), but at least this approach also works on Windows 9x */
    ncb.ncb_lana_num = 0;
    if (Netbios(&ncb) == NRC_GOODRET)
    {
        ASTAT Adapter;
        /* prepare to get the adapter status block */
        memzero(&ncb, sizeof(ncb));
        ncb.ncb_command = NCBASTAT;
        /* it is considered bad practice to hardcode the LANA number (should enumerate
           adapters first), but at least this approach also works on Windows 9x */
        ncb.ncb_lana_num = 0;
        strcpy((char *)ncb.ncb_callname, "*");
        ncb.ncb_buffer = (unsigned char *)&Adapter;
        ncb.ncb_length = sizeof(Adapter);
        /* get the adapter's info */
        if (Netbios(&ncb) == 0)
        {
            /* store the MAC address */
            buffer[0] = Adapter.adapt.adapter_address[0];
            buffer[1] = Adapter.adapt.adapter_address[1];
            buffer[2] = Adapter.adapt.adapter_address[2];
            buffer[3] = Adapter.adapt.adapter_address[3];
            buffer[4] = Adapter.adapt.adapter_address[4];
            buffer[5] = Adapter.adapt.adapter_address[5];
            success = OFTrue;
        }
    }
    /* check whether NetBIOS routines succeeded, if not try the SNMP approach */
    if (!success)
    {
        HINSTANCE m_hInst1, m_hInst2;
        /* load the "SNMP Utility Library" dll and get the addresses of the functions necessary */
        m_hInst1 = LoadLibrary("snmpapi.dll");
        if (m_hInst1 >= (HINSTANCE)HINSTANCE_ERROR)
        {
            pSnmpUtilOidCpy m_Copy = (pSnmpUtilOidCpy)GetProcAddress(m_hInst1, "SnmpUtilOidCpy");
            pSnmpUtilOidNCmp m_Compare = (pSnmpUtilOidNCmp)GetProcAddress(m_hInst1, "SnmpUtilOidNCmp");
            pSnmpUtilVarBindFree m_BindFree = (pSnmpUtilVarBindFree)GetProcAddress(m_hInst1, "SnmpUtilVarBindFree");
            /* load the "SNMP Internet MIB" dll and get the addresses of the functions necessary */
            m_hInst2 = LoadLibrary("inetmib1.dll");
            if (m_hInst2 >= (HINSTANCE)HINSTANCE_ERROR)
            {
                HANDLE PollForTrapEvent;
                AsnObjectIdentifier SupportedView;
                UINT OID_ifEntryType[] = {1, 3, 6, 1, 2, 1, 2, 2, 1, 3};
                UINT OID_ifEntryNum[] = {1, 3, 6, 1, 2, 1, 2, 1};
                UINT OID_ipMACEntAddr[] = {1, 3, 6, 1, 2, 1, 2, 2, 1, 6};
                AsnObjectIdentifier MIB_ifMACEntAddr = {sizeof(OID_ipMACEntAddr) / sizeof(UINT), OID_ipMACEntAddr};
                AsnObjectIdentifier MIB_ifEntryType = {sizeof(OID_ifEntryType) / sizeof(UINT), OID_ifEntryType};
                AsnObjectIdentifier MIB_ifEntryNum = {sizeof(OID_ifEntryNum) / sizeof(UINT), OID_ifEntryNum};
                SnmpVarBindList varBindList;
                SnmpVarBind varBind[2];
                AsnInteger32 errorStatus;
                AsnInteger32 errorIndex;
                AsnObjectIdentifier MIB_NULL = {0, 0};
                int ret;
                int dtmp;
                int j = 0;
                pSnmpExtensionInit m_Init = (pSnmpExtensionInit)GetProcAddress(m_hInst2, "SnmpExtensionInit");
                /* pSnmpExtensionInitEx m_InitEx = (pSnmpExtensionInitEx)GetProcAddress(m_hInst2, "SnmpExtensionInitEx"); */
                pSnmpExtensionQuery m_Query = (pSnmpExtensionQuery)GetProcAddress(m_hInst2, "SnmpExtensionQuery");
                /* pSnmpExtensionTrap m_Trap = (pSnmpExtensionTrap)GetProcAddress(m_hInst2, "SnmpExtensionTrap"); */
                m_Init(GetTickCount(), &PollForTrapEvent, &SupportedView);
                /* initialize the variable list to be retrieved by m_Query */
                varBindList.list = varBind;
                varBind[0].name = MIB_NULL;
                varBind[1].name = MIB_NULL;
                /* copy in the OID to find the number of entries in the inteface table */
                varBindList.len = 1;        /* only retrieving one item */
                m_Copy(&varBind[0].name, &MIB_ifEntryNum);
                ret = m_Query(SNMP_PDU_GETNEXT, &varBindList, &errorStatus, &errorIndex);
                varBindList.len = 2;
                /* copy in the OID of ifType, the type of interface */
                m_Copy(&varBind[0].name, &MIB_ifEntryType);
                /* copy in the OID of ifPhysAddress, the address */
                m_Copy(&varBind[1].name, &MIB_ifMACEntAddr);
                do {
                    /* Submit the query.  Responses will be loaded into varBindList.
                       We can expect this call to succeed a # of times corresponding
                       to the # of adapters reported to be in the system */
                    ret = m_Query(SNMP_PDU_GETNEXT, &varBindList, &errorStatus, &errorIndex);
                    if (!ret)
                        ret = 1;
                    else
                    {
                        /* confirm that the proper type has been returned */
                        ret = m_Compare(&varBind[0].name, &MIB_ifEntryType, MIB_ifEntryType.idLength);
                    }
                    if (!ret)
                    {
                        j++;
                        dtmp = varBind[0].value.asnValue.number;
                        /* type 6 describes ethernet interfaces */
                        if (dtmp == 6)
                        {
                            /* confirm that we have an address here */
                            ret = m_Compare(&varBind[1].name, &MIB_ifMACEntAddr,MIB_ifMACEntAddr.idLength);
                            if ((!ret) && (varBind[1].value.asnValue.address.stream != NULL))
                            {
                                if ((varBind[1].value.asnValue.address.stream[0] == 0x44) &&
                                    (varBind[1].value.asnValue.address.stream[1] == 0x45) &&
                                    (varBind[1].value.asnValue.address.stream[2] == 0x53) &&
                                    (varBind[1].value.asnValue.address.stream[3] == 0x54) &&
                                    (varBind[1].value.asnValue.address.stream[4] == 0x00))
                                {
                                    /* ignore all dial-up networking adapters */
                                    continue;
                                }
                                if ((varBind[1].value.asnValue.address.stream[0] == 0x00) &&
                                    (varBind[1].value.asnValue.address.stream[1] == 0x00) &&
                                    (varBind[1].value.asnValue.address.stream[2] == 0x00) &&
                                    (varBind[1].value.asnValue.address.stream[3] == 0x00) &&
                                    (varBind[1].value.asnValue.address.stream[4] == 0x00) &&
                                    (varBind[1].value.asnValue.address.stream[5] == 0x00))
                                {
                                    /* ignore NULL addresses returned by other network interfaces */
                                    continue;
                                }
                                /* store the MAC address */
                                buffer[0] = varBind[1].value.asnValue.address.stream[0];
                                buffer[1] = varBind[1].value.asnValue.address.stream[1];
                                buffer[2] = varBind[1].value.asnValue.address.stream[2];
                                buffer[3] = varBind[1].value.asnValue.address.stream[3];
                                buffer[4] = varBind[1].value.asnValue.address.stream[4];
                                buffer[5] = varBind[1].value.asnValue.address.stream[5];
                                ret = 1;    // we found an address -> exit
                            }
                        }
                    }
                } while (!ret);  /* Stop only on an error. An error will occur when we
                                    go exhaust the list of interfaces to be examined */
                FreeLibrary(m_hInst2);
                /* free the bindings */
                m_BindFree(&varBind[0]);
                m_BindFree(&varBind[1]);
            }
            FreeLibrary(m_hInst1);
        }
    }
    return buffer;
}
#endif

#ifdef HAVE_PROTOTYPE_GETHOSTID
/* CW10 has a prototype but no implementation (gethostid() is already declared extern */
long gethostid(void)
#else
static long gethostid(void)
#endif
{
    long result = 0;
#if (defined(HAVE_GETHOSTNAME) && defined(HAVE_GETHOSTBYNAME)) || defined(HAVE_WINDOWS_H)
    char name[1024];
    struct hostent *hent = NULL;
    char **p = NULL;
    struct in_addr in;
#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.1 */
    WORD winSockVersionNeeded = MAKEWORD(1, 1);
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif
    /*
    ** Define the hostid to be the system's main TCP/IP address.
    ** This is not perfect but it is better than nothing (i.e. using zero)
    */
    if (gethostname(name, 1024) == 0)
    {
#if defined(HAVE_GETHOSTBYNAME_R)
        // use gethostbyname_r instead of gethostbyname
        int h_errnop=0;
        struct hostent theHostent;
        char buffer[GETHOSTBYNAME_R_BUFSIZE];
        if ((hent = gethostbyname_r(name, &theHostent, buffer, GETHOSTBYNAME_R_BUFSIZE, &h_errnop)) != NULL)
#else
        if ((hent = gethostbyname(name)) != NULL)
#endif
        {
            p = hent->h_addr_list;
            if (p && *p)
            {
                (void) memcpy(&in.s_addr, *p, sizeof(in.s_addr));
                result = (long)in.s_addr;
            }
        }
    }
#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif
#endif /* defined(HAVE_GETHOSTNAME) && defined(HAVE_GETHOSTBYNAME) */
/* on Windows systems determine some system specific information (e.g. MAC address) */
#ifdef HAVE_WINDOWS_H
    OFCRC32 crc;
    /* get some processor specific information in addition to the MAC address */
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    /* get volume information of the system drive */
    char systemDrive[MAX_PATH];
    DWORD serialNumber = 0;
    if (GetSystemDirectory(systemDrive, sizeof(systemDrive)) >= 0)
    {
        /* check for proper pathname */
        if ((strlen(systemDrive) >= 3) && (systemDrive[1] == ':') && (systemDrive[2] == '\\'))
        {
            /* truncate the pathname directly after the drive specification */
            systemDrive[3] = 0;
            if (!GetVolumeInformation(systemDrive, NULL, 0, &serialNumber, NULL, NULL, NULL, 0))
                serialNumber = 0;
        }
    }
    /* concatenate the host specific elements and compute a 32-bit checksum */
    crc.addBlock(&result /*ip address*/, sizeof(result));
#ifndef __MINGW32__
    /* on MinGW, getMacAddress() is not yet available. */
    unsigned char buffer[6];
    crc.addBlock(getMACAddress(buffer), sizeof(buffer));
#endif
    crc.addBlock(&serialNumber, sizeof(serialNumber));
    crc.addBlock(&systemInfo.wProcessorLevel, sizeof(systemInfo.wProcessorLevel));
    crc.addBlock(&systemInfo.wProcessorRevision, sizeof(systemInfo.wProcessorRevision));
    crc.addBlock(&systemInfo.dwProcessorType, sizeof(systemInfo.dwProcessorType));
    result = (long)crc.getCRC32();
#endif
    /* 'artificial' hostid: on Windows system a CRC32 checksum over some host specific
       information (e.g. MAC address), the 4 bytes TCP/IP address otherwise.
    */
    return result;
}

#else // !(defined(HAVE_GETHOSTNAME) && defined(HAVE_GETHOSTBYNAME))
/*
** last chance workaround if there is no other way
*/
#ifdef HAVE_PROTOTYPE_GETHOSTID
/* CW10 has a prototype but no implementation (gethostid() is already declared extern */
long gethostid(void) { return 0; }
#else
static long gethostid(void) { return 0; }
#endif
#endif // !(defined(HAVE_GETHOSTNAME) && defined(HAVE_GETHOSTBYNAME))

#endif // !HAVE_SYSINFO
#else // HAVE_GETHOSTID
#ifndef HAVE_PROTOTYPE_GETHOSTID
extern "C" {
long gethostid(void);
}
#endif // !HAVE_PROTOTYPE_GETHOSTID
#endif // HAVE_GETHOSTID

// ********************************

/*
 * Global variable storing the return value of gethostid().
 * Since the variable is not declared in the header file it can only be used
 * within this source file. Any access to or modification of its value is
 * protected by a mutex (see dcmGenerateUniqueIdentifier()).
 */

static unsigned long hostIdentifier = 0;


/*
** char* generateUniqueIdentifier(char* buf)
** Creates a Unique Identifier in buf and returns buf.
** buf must be at least 65 bytes.
*/


#ifdef WITH_THREADS
static OFMutex uidCounterMutex;  // mutex protecting access to counterOfCurrentUID and hostIdentifier
#endif

static unsigned int counterOfCurrentUID = 0;

static const unsigned int maxUIDLen = 64;    /* A UID may be 64 chars or less */

static void
initCounterOfCurrentUID()
{
    /* Code taken from oftime.cc */
#ifdef HAVE_WINDOWS_H
    /* Windows: no microseconds available, use milliseconds instead */
    SYSTEMTIME timebuf;
    GetSystemTime(&timebuf);
    counterOfCurrentUID = timebuf.wMilliseconds; /* This is in the range 0 - 999 */
#else /* Unix */
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == 0)
        counterOfCurrentUID = tv.tv_usec; /* This is in the range 0 - 999999 */
#endif
    /* Do not ever use "0" for the counter */
    counterOfCurrentUID++;
}


static char*
stripTrailing(char* s, char c)
{
    int i, n;

    if (s == NULL) return s;

    n = strlen(s);
    for (i = n - 1; (i >= 0) && (s[i] == c); i--)
        s[i] = '\0';
    return s;
}

static void
addUIDComponent(char* uid, const char* s)
{
    /* copy into UID as much of the contents of s as possible */
    if (OFStandard::strlcat(uid, s, maxUIDLen + 1) >= maxUIDLen + 1) // maxUIDLen+1 because strlcat() wants the size of the buffer, not the permitted number of characters.
    {
        DCMDATA_WARN("Truncated UID in dcmGenerateUniqueIdentifier(), SITE_UID_ROOT too long?");
    }
    stripTrailing(uid, '.');
}

inline static unsigned long
forcePositive(long i)
{
    return (i < 0) ? OFstatic_cast(unsigned long, -i) : OFstatic_cast(unsigned long, i);
}

char* dcmGenerateUniqueIdentifier(char* uid, const char* prefix)
{
    char buf[128]; /* be very safe */

    uid[0] = '\0'; /* initialise */

#ifdef WITH_THREADS
    uidCounterMutex.lock();
#endif
    if (hostIdentifier == 0)
    {
        /* On 64-bit Linux, the "32-bit identifier" returned by gethostid() is
           sign-extended to a 64-bit long, so we need to blank the upper 32 bits */
        hostIdentifier = OFstatic_cast(unsigned long, gethostid() & 0xffffffff);
    }
    if (counterOfCurrentUID == 0)
        initCounterOfCurrentUID();

    unsigned int counter = counterOfCurrentUID++;
#ifdef WITH_THREADS
    uidCounterMutex.unlock();
#endif

    if (prefix != NULL ) {
        addUIDComponent(uid, prefix);
    } else {
        addUIDComponent(uid, SITE_INSTANCE_UID_ROOT);
    }

    sprintf(buf, ".%lu", hostIdentifier);
    addUIDComponent(uid, buf);

    sprintf(buf, ".%lu", forcePositive(OFStandard::getProcessID()));
    addUIDComponent(uid, buf);

    sprintf(buf, ".%lu", forcePositive(OFstatic_cast(long, time(NULL))));
    addUIDComponent(uid, buf);

    sprintf(buf, ".%u", counter);

    addUIDComponent(uid, buf);

    return uid;
}
