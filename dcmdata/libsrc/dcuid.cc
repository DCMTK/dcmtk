/*
 *
 *  Copyright (C) 1994-2023, OFFIS e.V.
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
// on Windows, we need Winsock2 for getting network adapter information
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#endif

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

#ifdef __MINGW32__
#include <winerror.h> /* needed for NO_ERROR constant on MinGW */
#endif

END_EXTERN_C

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/ofstd/ofthread.h"
#include "dcmtk/ofstd/ofcrc32.h"
#include "dcmtk/ofstd/ofdefine.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/ofstd/ofsockad.h"

struct UIDNameMap {
    const char* uid;
    const char* keyword;
    const char* name;
};

//
// It is very important that the names of the UIDs (third value) may not use
// the following characters: space  (  )  [  ], =  <  >

static const UIDNameMap uidNameMap[] = {

    // Application Context
    { UID_StandardApplicationContext,                                            "DICOMApplicationContext",                                            "StandardApplicationContext" },

    // Transfer Syntax
    { UID_LittleEndianImplicitTransferSyntax,                                    "ImplicitVRLittleEndian",                                             "LittleEndianImplicit" },
    { UID_LittleEndianExplicitTransferSyntax,                                    "ExplicitVRLittleEndian",                                             "LittleEndianExplicit" },
    { UID_BigEndianExplicitTransferSyntax,                                       "ExplicitVRBigEndian",                                                "BigEndianExplicit" },
    { UID_JPEGProcess1TransferSyntax,                                            "JPEGBaseline8Bit",                                                   "JPEGBaseline" },
    { UID_JPEGProcess2_4TransferSyntax,                                          "JPEGExtended12Bit",                                                  "JPEGExtended:Process2+4" },
    { UID_JPEGProcess3_5TransferSyntax,                                          "JPEGExtended35",                                                     "JPEGExtended:Process3+5" },
    { UID_JPEGProcess6_8TransferSyntax,                                          "JPEGSpectralSelectionNonHierarchical68",                             "JPEGSpectralSelection:Non-hierarchical:Process6+8" },
    { UID_JPEGProcess7_9TransferSyntax,                                          "JPEGSpectralSelectionNonHierarchical79",                             "JPEGSpectralSelection:Non-hierarchical:Process7+9" },
    { UID_JPEGProcess10_12TransferSyntax,                                        "JPEGFullProgressionNonHierarchical1012",                             "JPEGFullProgression:Non-hierarchical:Process10+12" },
    { UID_JPEGProcess11_13TransferSyntax,                                        "JPEGFullProgressionNonHierarchical1113",                             "JPEGFullProgression:Non-hierarchical:Process11+13" },
    { UID_JPEGProcess14TransferSyntax,                                           "JPEGLossless",                                                       "JPEGLossless:Non-hierarchical:Process14" },
    { UID_JPEGProcess15TransferSyntax,                                           "JPEGLosslessNonHierarchical15",                                      "JPEGLossless:Non-hierarchical:Process15" },
    { UID_JPEGProcess16_18TransferSyntax,                                        "JPEGExtendedHierarchical1618",                                       "JPEGExtended:Hierarchical:Process16+18" },
    { UID_JPEGProcess17_19TransferSyntax,                                        "JPEGExtendedHierarchical1719",                                       "JPEGExtended:Hierarchical:Process17+19" },
    { UID_JPEGProcess20_22TransferSyntax,                                        "JPEGSpectralSelectionHierarchical2022",                              "JPEGSpectralSelection:Hierarchical:Process20+22" },
    { UID_JPEGProcess21_23TransferSyntax,                                        "JPEGSpectralSelectionHierarchical2123",                              "JPEGSpectralSelection:Hierarchical:Process21+23" },
    { UID_JPEGProcess24_26TransferSyntax,                                        "JPEGFullProgressionHierarchical2426",                                "JPEGFullProgression:Hierarchical:Process24+26" },
    { UID_JPEGProcess25_27TransferSyntax,                                        "JPEGFullProgressionHierarchical2527",                                "JPEGFullProgression:Hierarchical:Process25+27" },
    { UID_JPEGProcess28TransferSyntax,                                           "JPEGLosslessHierarchical28",                                         "JPEGLossless:Hierarchical:Process28" },
    { UID_JPEGProcess29TransferSyntax,                                           "JPEGLosslessHierarchical29",                                         "JPEGLossless:Hierarchical:Process29" },
    { UID_JPEGProcess14SV1TransferSyntax,                                        "JPEGLosslessSV1",                                                    "JPEGLossless:Non-hierarchical-1stOrderPrediction" },
    { UID_JPEGLSLosslessTransferSyntax,                                          "JPEGLSLossless",                                                     "JPEGLSLossless" },
    { UID_JPEGLSLossyTransferSyntax,                                             "JPEGLSNearLossless",                                                 "JPEGLSLossy" },
    { UID_RLELosslessTransferSyntax,                                             "RLELossless",                                                        "RLELossless" },
    { UID_DeflatedExplicitVRLittleEndianTransferSyntax,                          "DeflatedExplicitVRLittleEndian",                                     "DeflatedLittleEndianExplicit" },
    { UID_JPEG2000LosslessOnlyTransferSyntax,                                    "JPEG2000Lossless",                                                   "JPEG2000LosslessOnly" },
    { UID_JPEG2000TransferSyntax,                                                "JPEG2000",                                                           "JPEG2000" },
    { UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnlyTransferSyntax, "JPEG2000MCLossless",                                                 "JPEG2000MulticomponentLosslessOnly" },
    { UID_JPEG2000Part2MulticomponentImageCompressionTransferSyntax,             "JPEG2000MC",                                                         "JPEG2000Multicomponent" },
    { UID_JPIPReferencedTransferSyntax,                                          "JPIPReferenced",                                                     "JPIPReferenced" },
    { UID_JPIPReferencedDeflateTransferSyntax,                                   "JPIPReferencedDeflate",                                              "JPIPReferencedDeflate" },
    { UID_MPEG2MainProfileAtMainLevelTransferSyntax,                             "MPEG2MPML",                                                          "MPEG2MainProfile@MainLevel" },
    { UID_FragmentableMPEG2MainProfileMainLevelTransferSyntax,                   "MPEG2MPMLF",                                                         "FragmentableMPEG2MainProfile/MainLevel" },
    { UID_MPEG2MainProfileAtHighLevelTransferSyntax,                             "MPEG2MPHL",                                                          "MPEG2MainProfile@HighLevel" },
    { UID_FragmentableMPEG2MainProfileHighLevelTransferSyntax,                   "MPEG2MPHLF",                                                         "FragmentableMPEG2MainProfile/HighLevel" },
    { UID_MPEG4HighProfileLevel4_1TransferSyntax,                                "MPEG4HP41",                                                          "MPEG4HighProfile/Level4.1" },
    { UID_FragmentableMPEG4HighProfileLevel4_1TransferSyntax,                    "MPEG4HP41F",                                                         "FragmentableMPEG4HighProfile/Level4.1" },
    { UID_MPEG4BDcompatibleHighProfileLevel4_1TransferSyntax,                    "MPEG4HP41BD",                                                        "MPEG4BDcompatibleHighProfile/Level4.1" },
    { UID_FragmentableMPEG4BDcompatibleHighProfileLevel4_1TransferSyntax,        "MPEG4HP41BDF",                                                       "FragmentableMPEG4BDcompatibleHighProfile/Level4.1" },
    { UID_MPEG4HighProfileLevel4_2_For2DVideoTransferSyntax,                     "MPEG4HP422D",                                                        "MPEG4HighProfile/Level4.2For2DVideo" },
    { UID_FragmentableMPEG4HighProfileLevel4_2_For2DVideoTransferSyntax,         "MPEG4HP422DF",                                                       "FragmentableMPEG4HighProfile/Level4.2For2DVideo" },
    { UID_MPEG4HighProfileLevel4_2_For3DVideoTransferSyntax,                     "MPEG4HP423D",                                                        "MPEG4HighProfile/Level4.2For3DVideo" },
    { UID_FragmentableMPEG4HighProfileLevel4_2_For3DVideoTransferSyntax,         "MPEG4HP423DF",                                                       "FragmentableMPEG4HighProfile/Level4.2For3DVideo" },
    { UID_MPEG4StereoHighProfileLevel4_2TransferSyntax,                          "MPEG4HP42STEREO",                                                    "MPEG4StereoHighProfile/Level4.2" },
    { UID_FragmentableMPEG4StereoHighProfileLevel4_2TransferSyntax,              "MPEG4HP42STEREOF",                                                   "FragmentableMPEG4StereoHighProfile/Level4.2" },
    { UID_HEVCMainProfileLevel5_1TransferSyntax,                                 "HEVCMP51",                                                           "HEVCMainProfile/Level5.1" },
    { UID_HEVCMain10ProfileLevel5_1TransferSyntax,                               "HEVCM10P51",                                                         "HEVCMain10Profile/Level5.1" },
    { UID_SMPTEST2110_20_UncompressedProgressiveActiveVideoTransferSyntax,       "SMPTEST211020UncompressedProgressiveActiveVideo",                    "SMPTEST2110-20:UncompressedProgressiveActiveVideo" },
    { UID_SMPTEST2110_20_UncompressedInterlacedActiveVideoTransferSyntax,        "SMPTEST211020UncompressedInterlacedActiveVideo",                     "SMPTEST2110-20:UncompressedInterlacedActiveVideo" },
    { UID_SMPTEST2110_30_PCMDigitalAudioTransferSyntax,                          "SMPTEST211030PCMDigitalAudio",                                       "SMPTEST2110-30:PCMDigitalAudio" },
    { UID_RETIRED_RFC2557MIMEEncapsulationTransferSyntax,                        "RFC2557MIMEEncapsulation",                                           "RETIRED_RFC2557MIMEEncapsulation" },
    { UID_RETIRED_XMLEncodingTransferSyntax,                                     "XMLEncoding",                                                        "RETIRED_XMLEncoding" },
    { UID_PrivateGE_LEI_WithBigEndianPixelDataTransferSyntax,                    NULL /* no official keyword */,                                       "PrivateGELittleEndianImplicitWithBigEndianPixelData" },

    // Storage (DICOM)
    { UID_AcquisitionContextSRStorage,                                           "AcquisitionContextSRStorage",                                        "AcquisitionContextSRStorage" },
    { UID_AdvancedBlendingPresentationStateStorage,                              "AdvancedBlendingPresentationStateStorage",                           "AdvancedBlendingPresentationStateStorage" },
    { UID_AmbulatoryECGWaveformStorage,                                          "AmbulatoryECGWaveformStorage",                                       "AmbulatoryECGWaveformStorage" },
    { UID_ArterialPulseWaveformStorage,                                          "ArterialPulseWaveformStorage",                                       "ArterialPulseWaveformStorage" },
    { UID_AutorefractionMeasurementsStorage,                                     "AutorefractionMeasurementsStorage",                                  "AutorefractionMeasurementsStorage" },
    { UID_BasicStructuredDisplayStorage,                                         "BasicStructuredDisplayStorage",                                      "BasicStructuredDisplayStorage" },
    { UID_BasicTextSRStorage,                                                    "BasicTextSRStorage",                                                 "BasicTextSRStorage" },
    { UID_BasicVoiceAudioWaveformStorage,                                        "BasicVoiceAudioWaveformStorage",                                     "BasicVoiceAudioWaveformStorage" },
    { UID_BlendingSoftcopyPresentationStateStorage,                              "BlendingSoftcopyPresentationStateStorage",                           "BlendingSoftcopyPresentationStateStorage" },
    { UID_BodyPositionWaveformStorage,                                           "BodyPositionWaveformStorage",                                        "BodyPositionWaveformStorage" },
    { UID_BreastProjectionXRayImageStorageForPresentation,                       "BreastProjectionXRayImageStorageForPresentation",                    "BreastProjectionXRayImageStorageForPresentation" },
    { UID_BreastProjectionXRayImageStorageForProcessing,                         "BreastProjectionXRayImageStorageForProcessing",                      "BreastProjectionXRayImageStorageForProcessing" },
    { UID_BreastTomosynthesisImageStorage,                                       "BreastTomosynthesisImageStorage",                                    "BreastTomosynthesisImageStorage" },
    { UID_CardiacElectrophysiologyWaveformStorage,                               "CardiacElectrophysiologyWaveformStorage",                            "CardiacElectrophysiologyWaveformStorage" },
    { UID_CArmPhotonElectronRadiationRecordStorage,                              "CArmPhotonElectronRadiationRecordStorage",                           "CArmPhotonElectronRadiationRecordStorage" },
    { UID_CArmPhotonElectronRadiationStorage,                                    "CArmPhotonElectronRadiationStorage",                                 "CArmPhotonElectronRadiationStorage" },
    { UID_ChestCADSRStorage,                                                     "ChestCADSRStorage",                                                  "ChestCADSRStorage" },
    { UID_ColonCADSRStorage,                                                     "ColonCADSRStorage",                                                  "ColonCADSRStorage" },
    { UID_ColorSoftcopyPresentationStateStorage,                                 "ColorSoftcopyPresentationStateStorage",                              "ColorSoftcopyPresentationStateStorage" },
    { UID_CompositingPlanarMPRVolumetricPresentationStateStorage,                "CompositingPlanarMPRVolumetricPresentationStateStorage",             "CompositingPlanarMPRVolumetricPresentationStateStorage" },
    { UID_Comprehensive3DSRStorage,                                              "Comprehensive3DSRStorage",                                           "Comprehensive3DSRStorage" },
    { UID_ComprehensiveSRStorage,                                                "ComprehensiveSRStorage",                                             "ComprehensiveSRStorage" },
    { UID_ComputedRadiographyImageStorage,                                       "ComputedRadiographyImageStorage",                                    "ComputedRadiographyImageStorage" },
    { UID_ContentAssessmentResultsStorage,                                       "ContentAssessmentResultsStorage",                                    "ContentAssessmentResultsStorage" },
    { UID_CornealTopographyMapStorage,                                           "CornealTopographyMapStorage",                                        "CornealTopographyMapStorage" },
    { UID_CTDefinedProcedureProtocolStorage,                                     "CTDefinedProcedureProtocolStorage",                                  "CTDefinedProcedureProtocolStorage" },
    { UID_CTImageStorage,                                                        "CTImageStorage",                                                     "CTImageStorage" },
    { UID_CTPerformedProcedureProtocolStorage,                                   "CTPerformedProcedureProtocolStorage",                                "CTPerformedProcedureProtocolStorage" },
    { UID_DeformableSpatialRegistrationStorage,                                  "DeformableSpatialRegistrationStorage",                               "DeformableSpatialRegistrationStorage" },
    { UID_DermoscopicPhotographyImageStorage,                                    "DermoscopicPhotographyImageStorage",                                 "DermoscopicPhotographyImageStorage" },
    { UID_DigitalIntraOralXRayImageStorageForPresentation,                       "DigitalIntraOralXRayImageStorageForPresentation",                    "DigitalIntraOralXRayImageStorageForPresentation" },
    { UID_DigitalIntraOralXRayImageStorageForProcessing,                         "DigitalIntraOralXRayImageStorageForProcessing",                      "DigitalIntraOralXRayImageStorageForProcessing" },
    { UID_DigitalMammographyXRayImageStorageForPresentation,                     "DigitalMammographyXRayImageStorageForPresentation",                  "DigitalMammographyXRayImageStorageForPresentation" },
    { UID_DigitalMammographyXRayImageStorageForProcessing,                       "DigitalMammographyXRayImageStorageForProcessing",                    "DigitalMammographyXRayImageStorageForProcessing" },
    { UID_DigitalXRayImageStorageForPresentation,                                "DigitalXRayImageStorageForPresentation",                             "DigitalXRayImageStorageForPresentation" },
    { UID_DigitalXRayImageStorageForProcessing,                                  "DigitalXRayImageStorageForProcessing",                               "DigitalXRayImageStorageForProcessing" },
    { UID_ElectromyogramWaveformStorage,                                         "ElectromyogramWaveformStorage",                                      "ElectromyogramWaveformStorage" },
    { UID_ElectrooculogramWaveformStorage,                                       "ElectrooculogramWaveformStorage",                                    "ElectrooculogramWaveformStorage" },
    { UID_EncapsulatedCDAStorage,                                                "EncapsulatedCDAStorage",                                             "EncapsulatedCDAStorage" },
    { UID_EncapsulatedMTLStorage,                                                "EncapsulatedMTLStorage",                                             "EncapsulatedMTLStorage" },
    { UID_EncapsulatedOBJStorage,                                                "EncapsulatedOBJStorage",                                             "EncapsulatedOBJStorage" },
    { UID_EncapsulatedPDFStorage,                                                "EncapsulatedPDFStorage",                                             "EncapsulatedPDFStorage" },
    { UID_EncapsulatedSTLStorage,                                                "EncapsulatedSTLStorage",                                             "EncapsulatedSTLStorage" },
    { UID_EnhancedContinuousRTImageStorage,                                      "EnhancedContinuousRTImageStorage",                                   "EnhancedContinuousRTImageStorage" },
    { UID_EnhancedCTImageStorage,                                                "EnhancedCTImageStorage",                                             "EnhancedCTImageStorage" },
    { UID_EnhancedMRColorImageStorage,                                           "EnhancedMRColorImageStorage",                                        "EnhancedMRColorImageStorage" },
    { UID_EnhancedMRImageStorage,                                                "EnhancedMRImageStorage",                                             "EnhancedMRImageStorage" },
    { UID_EnhancedPETImageStorage,                                               "EnhancedPETImageStorage",                                            "EnhancedPETImageStorage" },
    { UID_EnhancedRTImageStorage,                                                "EnhancedRTImageStorage",                                             "EnhancedRTImageStorage" },
    { UID_EnhancedSRStorage,                                                     "EnhancedSRStorage",                                                  "EnhancedSRStorage" },
    { UID_EnhancedUSVolumeStorage,                                               "EnhancedUSVolumeStorage",                                            "EnhancedUSVolumeStorage" },
    { UID_EnhancedXAImageStorage,                                                "EnhancedXAImageStorage",                                             "EnhancedXAImageStorage" },
    { UID_EnhancedXRayRadiationDoseSRStorage,                                    "EnhancedXRayRadiationDoseSRStorage",                                 "EnhancedXRayRadiationDoseSRStorage" },
    { UID_EnhancedXRFImageStorage,                                               "EnhancedXRFImageStorage",                                            "EnhancedXRFImageStorage" },
    { UID_ExtensibleSRStorage,                                                   "ExtensibleSRStorage",                                                "ExtensibleSRStorage" },
    { UID_General32BitECGWaveformStorage,                                        "General32bitECGWaveformStorage",                                     "General32BitECGWaveformStorage" },
    { UID_GeneralAudioWaveformStorage,                                           "GeneralAudioWaveformStorage",                                        "GeneralAudioWaveformStorage" },
    { UID_GeneralECGWaveformStorage,                                             "GeneralECGWaveformStorage",                                          "GeneralECGWaveformStorage" },
    { UID_GrayscalePlanarMPRVolumetricPresentationStateStorage,                  "GrayscalePlanarMPRVolumetricPresentationStateStorage",               "GrayscalePlanarMPRVolumetricPresentationStateStorage" },
    { UID_GrayscaleSoftcopyPresentationStateStorage,                             "GrayscaleSoftcopyPresentationStateStorage",                          "GrayscaleSoftcopyPresentationStateStorage" },
    { UID_HemodynamicWaveformStorage,                                            "HemodynamicWaveformStorage",                                         "HemodynamicWaveformStorage" },
    { UID_ImplantationPlanSRStorage,                                             "ImplantationPlanSRStorage",                                          "ImplantationPlanSRStorage" },
    // the following line is needed for reasons of backward compatibility (name of SOP Class changed, see the previous line)
    { UID_ImplantationPlanSRStorage,                                             NULL /* see above */,                                                 "ImplantationPlanSRDocumentStorage" },
    { UID_IntraocularLensCalculationsStorage,                                    "IntraocularLensCalculationsStorage",                                 "IntraocularLensCalculationsStorage" },
    { UID_IntravascularOpticalCoherenceTomographyImageStorageForPresentation,    "IntravascularOpticalCoherenceTomographyImageStorageForPresentation", "IntravascularOpticalCoherenceTomographyImageStorageForPresentation" },
    { UID_IntravascularOpticalCoherenceTomographyImageStorageForProcessing,      "IntravascularOpticalCoherenceTomographyImageStorageForProcessing",   "IntravascularOpticalCoherenceTomographyImageStorageForProcessing" },
    { UID_KeratometryMeasurementsStorage,                                        "KeratometryMeasurementsStorage",                                     "KeratometryMeasurementsStorage" },
    { UID_KeyObjectSelectionDocumentStorage,                                     "KeyObjectSelectionDocumentStorage",                                  "KeyObjectSelectionDocumentStorage" },
    { UID_LegacyConvertedEnhancedCTImageStorage,                                 "LegacyConvertedEnhancedCTImageStorage",                              "LegacyConvertedEnhancedCTImageStorage" },
    { UID_LegacyConvertedEnhancedMRImageStorage,                                 "LegacyConvertedEnhancedMRImageStorage",                              "LegacyConvertedEnhancedMRImageStorage" },
    { UID_LegacyConvertedEnhancedPETImageStorage,                                "LegacyConvertedEnhancedPETImageStorage",                             "LegacyConvertedEnhancedPETImageStorage" },
    { UID_LensometryMeasurementsStorage,                                         "LensometryMeasurementsStorage",                                      "LensometryMeasurementsStorage" },
    { UID_MacularGridThicknessAndVolumeReportStorage,                            "MacularGridThicknessAndVolumeReportStorage",                         "MacularGridThicknessAndVolumeReportStorage" },
    { UID_MammographyCADSRStorage,                                               "MammographyCADSRStorage",                                            "MammographyCADSRStorage" },
    { UID_MediaStorageDirectoryStorage,                                          "MediaStorageDirectoryStorage",                                       "MediaStorageDirectoryStorage" },
    { UID_MicroscopyBulkSimpleAnnotationsStorage,                                "MicroscopyBulkSimpleAnnotationsStorage",                             "MicroscopyBulkSimpleAnnotationsStorage" },
    { UID_MRImageStorage,                                                        "MRImageStorage",                                                     "MRImageStorage" },
    { UID_MRSpectroscopyStorage,                                                 "MRSpectroscopyStorage",                                              "MRSpectroscopyStorage" },
    { UID_MultichannelRespiratoryWaveformStorage,                                "MultichannelRespiratoryWaveformStorage",                             "MultichannelRespiratoryWaveformStorage" },
    { UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage,                   "MultiFrameGrayscaleByteSecondaryCaptureImageStorage",                "MultiframeGrayscaleByteSecondaryCaptureImageStorage" },
    { UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage,                   "MultiFrameGrayscaleWordSecondaryCaptureImageStorage",                "MultiframeGrayscaleWordSecondaryCaptureImageStorage" },
    { UID_MultiframeSingleBitSecondaryCaptureImageStorage,                       "MultiFrameSingleBitSecondaryCaptureImageStorage",                    "MultiframeSingleBitSecondaryCaptureImageStorage" },
    { UID_MultiframeTrueColorSecondaryCaptureImageStorage,                       "MultiFrameTrueColorSecondaryCaptureImageStorage",                    "MultiframeTrueColorSecondaryCaptureImageStorage" },
    { UID_MultipleVolumeRenderingVolumetricPresentationStateStorage,             "MultipleVolumeRenderingVolumetricPresentationStateStorage",          "MultipleVolumeRenderingVolumetricPresentationStateStorage" },
    { UID_NuclearMedicineImageStorage,                                           "NuclearMedicineImageStorage",                                        "NuclearMedicineImageStorage" },
    { UID_OphthalmicAxialMeasurementsStorage,                                    "OphthalmicAxialMeasurementsStorage",                                 "OphthalmicAxialMeasurementsStorage" },
    { UID_OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage,        "OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage",     "OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage" },
    { UID_OphthalmicOpticalCoherenceTomographyEnFaceImageStorage,                "OphthalmicOpticalCoherenceTomographyEnFaceImageStorage",             "OphthalmicOpticalCoherenceTomographyEnFaceImageStorage" },
    { UID_OphthalmicPhotography16BitImageStorage,                                "OphthalmicPhotography16BitImageStorage",                             "OphthalmicPhotography16BitImageStorage" },
    { UID_OphthalmicPhotography8BitImageStorage,                                 "OphthalmicPhotography8BitImageStorage",                              "OphthalmicPhotography8BitImageStorage" },
    { UID_OphthalmicThicknessMapStorage,                                         "OphthalmicThicknessMapStorage",                                      "OphthalmicThicknessMapStorage" },
    { UID_OphthalmicTomographyImageStorage,                                      "OphthalmicTomographyImageStorage",                                   "OphthalmicTomographyImageStorage" },
    { UID_OphthalmicVisualFieldStaticPerimetryMeasurementsStorage,               "OphthalmicVisualFieldStaticPerimetryMeasurementsStorage",            "OphthalmicVisualFieldStaticPerimetryMeasurementsStorage" },
    { UID_ParametricMapStorage,                                                  "ParametricMapStorage",                                               "ParametricMapStorage" },
    { UID_PatientRadiationDoseSRStorage,                                         "PatientRadiationDoseSRStorage",                                      "PatientRadiationDoseSRStorage" },
    { UID_PerformedImagingAgentAdministrationSRStorage,                          "PerformedImagingAgentAdministrationSRStorage",                       "PerformedImagingAgentAdministrationSRStorage" },
    { UID_PhotoacousticImageStorage,                                             "PhotoacousticImageStorage",                                          "PhotoacousticImageStorage" },
    { UID_PlannedImagingAgentAdministrationSRStorage,                            "PlannedImagingAgentAdministrationSRStorage",                         "PlannedImagingAgentAdministrationSRStorage" },
    { UID_PositronEmissionTomographyImageStorage,                                "PositronEmissionTomographyImageStorage",                             "PositronEmissionTomographyImageStorage" },
    { UID_ProcedureLogStorage,                                                   "ProcedureLogStorage",                                                "ProcedureLogStorage" },
    { UID_ProtocolApprovalStorage,                                               "ProtocolApprovalStorage",                                            "ProtocolApprovalStorage" },
    { UID_PseudoColorSoftcopyPresentationStateStorage,                           "PseudoColorSoftcopyPresentationStateStorage",                        "PseudoColorSoftcopyPresentationStateStorage" },
    { UID_RadiopharmaceuticalRadiationDoseSRStorage,                             "RadiopharmaceuticalRadiationDoseSRStorage",                          "RadiopharmaceuticalRadiationDoseSRStorage" },
    { UID_RawDataStorage,                                                        "RawDataStorage",                                                     "RawDataStorage" },
    { UID_RealWorldValueMappingStorage,                                          "RealWorldValueMappingStorage",                                       "RealWorldValueMappingStorage" },
    { UID_RespiratoryWaveformStorage,                                            "RespiratoryWaveformStorage",                                         "RespiratoryWaveformStorage" },
    { UID_RoboticArmRadiationStorage,                                            "RoboticArmRadiationStorage",                                         "RoboticArmRadiationStorage" },
    { UID_RoboticRadiationRecordStorage,                                         "RoboticRadiationRecordStorage",                                      "RoboticRadiationRecordStorage" },
    { UID_RoutineScalpElectroencephalogramWaveformStorage,                       "RoutineScalpElectroencephalogramWaveformStorage",                    "RoutineScalpElectroencephalogramWaveformStorage" },
    { UID_RTBeamsDeliveryInstructionStorage,                                     "RTBeamsDeliveryInstructionStorage",                                  "RTBeamsDeliveryInstructionStorage" },
    { UID_RTBeamsTreatmentRecordStorage,                                         "RTBeamsTreatmentRecordStorage",                                      "RTBeamsTreatmentRecordStorage" },
    { UID_RTBrachyApplicationSetupDeliveryInstructionStorage,                    "RTBrachyApplicationSetupDeliveryInstructionStorage",                 "RTBrachyApplicationSetupDeliveryInstructionStorage" },
    { UID_RTBrachyTreatmentRecordStorage,                                        "RTBrachyTreatmentRecordStorage",                                     "RTBrachyTreatmentRecordStorage" },
    { UID_RTDoseStorage,                                                         "RTDoseStorage",                                                      "RTDoseStorage" },
    { UID_RTImageStorage,                                                        "RTImageStorage",                                                     "RTImageStorage" },
    { UID_RTIonBeamsTreatmentRecordStorage,                                      "RTIonBeamsTreatmentRecordStorage",                                   "RTIonBeamsTreatmentRecordStorage" },
    { UID_RTIonPlanStorage,                                                      "RTIonPlanStorage",                                                   "RTIonPlanStorage" },
    { UID_RTPatientPositionAcquisitionInstructionStorage,                        "RTPatientPositionAcquisitionInstructionStorage",                     "RTPatientPositionAcquisitionInstructionStorage" },
    { UID_RTPhysicianIntentStorage,                                              "RTPhysicianIntentStorage",                                           "RTPhysicianIntentStorage" },
    { UID_RTPlanStorage,                                                         "RTPlanStorage",                                                      "RTPlanStorage" },
    { UID_RTRadiationRecordSetStorage,                                           "RTRadiationRecordSetStorage",                                        "RTRadiationRecordSetStorage" },
    { UID_RTRadiationSalvageRecordStorage,                                       "RTRadiationSalvageRecordStorage",                                    "RTRadiationSalvageRecordStorage" },
    { UID_RTRadiationSetDeliveryInstructionStorage,                              "RTRadiationSetDeliveryInstructionStorage",                           "RTRadiationSetDeliveryInstructionStorage" },
    { UID_RTRadiationSetStorage,                                                 "RTRadiationSetStorage",                                              "RTRadiationSetStorage" },
    { UID_RTSegmentAnnotationStorage,                                            "RTSegmentAnnotationStorage",                                         "RTSegmentAnnotationStorage" },
    { UID_RTStructureSetStorage,                                                 "RTStructureSetStorage",                                              "RTStructureSetStorage" },
    { UID_RTTreatmentPreparationStorage,                                         "RTTreatmentPreparationStorage",                                      "RTTreatmentPreparationStorage" },
    { UID_RTTreatmentSummaryRecordStorage,                                       "RTTreatmentSummaryRecordStorage",                                    "RTTreatmentSummaryRecordStorage" },
    { UID_SecondaryCaptureImageStorage,                                          "SecondaryCaptureImageStorage",                                       "SecondaryCaptureImageStorage" },
    { UID_SegmentationStorage,                                                   "SegmentationStorage",                                                "SegmentationStorage" },
    { UID_SegmentedVolumeRenderingVolumetricPresentationStateStorage,            "SegmentedVolumeRenderingVolumetricPresentationStateStorage",         "SegmentedVolumeRenderingVolumetricPresentationStateStorage" },
    { UID_SimplifiedAdultEchoSRStorage,                                          "SimplifiedAdultEchoSRStorage",                                       "SimplifiedAdultEchoSRStorage" },
    { UID_SleepElectroencephalogramWaveformStorage,                              "SleepElectroencephalogramWaveformStorage",                           "SleepElectroencephalogramWaveformStorage" },
    { UID_SpatialFiducialsStorage,                                               "SpatialFiducialsStorage",                                            "SpatialFiducialsStorage" },
    { UID_SpatialRegistrationStorage,                                            "SpatialRegistrationStorage",                                         "SpatialRegistrationStorage" },
    { UID_SpectaclePrescriptionReportStorage,                                    "SpectaclePrescriptionReportStorage",                                 "SpectaclePrescriptionReportStorage" },
    { UID_StereometricRelationshipStorage,                                       "StereometricRelationshipStorage",                                    "StereometricRelationshipStorage" },
    { UID_SubjectiveRefractionMeasurementsStorage,                               "SubjectiveRefractionMeasurementsStorage",                            "SubjectiveRefractionMeasurementsStorage" },
    { UID_SurfaceScanMeshStorage,                                                "SurfaceScanMeshStorage",                                             "SurfaceScanMeshStorage" },
    { UID_SurfaceScanPointCloudStorage,                                          "SurfaceScanPointCloudStorage",                                       "SurfaceScanPointCloudStorage" },
    { UID_SurfaceSegmentationStorage,                                            "SurfaceSegmentationStorage",                                         "SurfaceSegmentationStorage" },
    { UID_TomotherapeuticRadiationRecordStorage,                                 "TomotherapeuticRadiationRecordStorage",                              "TomotherapeuticRadiationRecordStorage" },
    { UID_TomotherapeuticRadiationStorage,                                       "TomotherapeuticRadiationStorage",                                    "TomotherapeuticRadiationStorage" },
    { UID_TractographyResultsStorage,                                            "TractographyResultsStorage",                                         "TractographyResultsStorage" },
    { UID_TwelveLeadECGWaveformStorage,                                          "TwelveLeadECGWaveformStorage",                                       "TwelveLeadECGWaveformStorage" },
    { UID_UltrasoundImageStorage,                                                "UltrasoundImageStorage",                                             "UltrasoundImageStorage" },
    { UID_UltrasoundMultiframeImageStorage,                                      "UltrasoundMultiFrameImageStorage",                                   "UltrasoundMultiframeImageStorage" },
    { UID_VariableModalityLUTSoftcopyPresentationStateStorage,                   "VariableModalityLUTSoftcopyPresentationStateStorage",                "VariableModalityLUTSoftcopyPresentationStateStorage" },
    { UID_VideoEndoscopicImageStorage,                                           "VideoEndoscopicImageStorage",                                        "VideoEndoscopicImageStorage" },
    { UID_VideoMicroscopicImageStorage,                                          "VideoMicroscopicImageStorage",                                       "VideoMicroscopicImageStorage" },
    { UID_VideoPhotographicImageStorage,                                         "VideoPhotographicImageStorage",                                      "VideoPhotographicImageStorage" },
    { UID_VisualAcuityMeasurementsStorage,                                       "VisualAcuityMeasurementsStorage",                                    "VisualAcuityMeasurementsStorage" },
    { UID_VLEndoscopicImageStorage,                                              "VLEndoscopicImageStorage",                                           "VLEndoscopicImageStorage" },
    { UID_VLMicroscopicImageStorage,                                             "VLMicroscopicImageStorage",                                          "VLMicroscopicImageStorage" },
    { UID_VLPhotographicImageStorage,                                            "VLPhotographicImageStorage",                                         "VLPhotographicImageStorage" },
    { UID_VLSlideCoordinatesMicroscopicImageStorage,                             "VLSlideCoordinatesMicroscopicImageStorage",                          "VLSlideCoordinatesMicroscopicImageStorage" },
    { UID_VLWholeSlideMicroscopyImageStorage,                                    "VLWholeSlideMicroscopyImageStorage",                                 "VLWholeSlideMicroscopyImageStorage" },
    { UID_VolumeRenderingVolumetricPresentationStateStorage,                     "VolumeRenderingVolumetricPresentationStateStorage",                  "VolumeRenderingVolumetricPresentationStateStorage" },
    { UID_WideFieldOphthalmicPhotographyStereographicProjectionImageStorage,     "WideFieldOphthalmicPhotographyStereographicProjectionImageStorage",  "WideFieldOphthalmicPhotographyStereographicProjectionImageStorage" },
    { UID_WideFieldOphthalmicPhotography3DCoordinatesImageStorage,               "WideFieldOphthalmicPhotography3DCoordinatesImageStorage",            "WideFieldOphthalmicPhotography3DCoordinatesImageStorage" },
    { UID_XADefinedProcedureProtocolStorage,                                     "XADefinedProcedureProtocolStorage",                                  "XADefinedProcedureProtocolStorage" },
    { UID_XAPerformedProcedureProtocolStorage,                                   "XAPerformedProcedureProtocolStorage",                                "XAPerformedProcedureProtocolStorage" },
    { UID_XAXRFGrayscaleSoftcopyPresentationStateStorage,                        "XAXRFGrayscaleSoftcopyPresentationStateStorage",                     "XAXRFGrayscaleSoftcopyPresentationStateStorage" },
    { UID_XRay3DAngiographicImageStorage,                                        "XRay3DAngiographicImageStorage",                                     "XRay3DAngiographicImageStorage" },
    { UID_XRay3DCraniofacialImageStorage,                                        "XRay3DCraniofacialImageStorage",                                     "XRay3DCraniofacialImageStorage" },
    { UID_XRayAngiographicImageStorage,                                          "XRayAngiographicImageStorage",                                       "XRayAngiographicImageStorage" },
    { UID_XRayRadiationDoseSRStorage,                                            "XRayRadiationDoseSRStorage",                                         "XRayRadiationDoseSRStorage" },
    { UID_XRayRadiofluoroscopicImageStorage,                                     "XRayRadiofluoroscopicImageStorage",                                  "XRayRadiofluoroscopicImageStorage" },
    // Storage (retired)
    { UID_RETIRED_HardcopyColorImageStorage,                                     "HardcopyColorImageStorage",                                          "RETIRED_HardcopyColorImageStorage" },
    { UID_RETIRED_HardcopyGrayscaleImageStorage,                                 "HardcopyGrayscaleImageStorage",                                      "RETIRED_HardcopyGrayscaleImageStorage" },
    { UID_RETIRED_NuclearMedicineImageStorage,                                   "NuclearMedicineImageStorageRetired",                                 "RETIRED_NuclearMedicineImageStorage" },
    { UID_RETIRED_StandaloneCurveStorage,                                        "StandaloneCurveStorage",                                             "RETIRED_StandaloneCurveStorage" },
    { UID_RETIRED_StandaloneModalityLUTStorage,                                  "StandaloneModalityLUTStorage",                                       "RETIRED_StandaloneModalityLUTStorage" },
    { UID_RETIRED_StandaloneOverlayStorage,                                      "StandaloneOverlayStorage",                                           "RETIRED_StandaloneOverlayStorage" },
    { UID_RETIRED_StandalonePETCurveStorage,                                     "StandalonePETCurveStorage",                                          "RETIRED_StandalonePETCurveStorage" },
    { UID_RETIRED_StandaloneVOILUTStorage,                                       "StandaloneVOILUTStorage",                                            "RETIRED_StandaloneVOILUTStorage" },
    { UID_RETIRED_StoredPrintStorage,                                            "StoredPrintStorage",                                                 "RETIRED_StoredPrintStorage" },
    { UID_RETIRED_UltrasoundImageStorage,                                        "UltrasoundImageStorageRetired",                                      "RETIRED_UltrasoundImageStorage" },
    { UID_RETIRED_UltrasoundMultiframeImageStorage,                              "UltrasoundMultiFrameImageStorageRetired",                            "RETIRED_UltrasoundMultiframeImageStorage" },
    { UID_RETIRED_VLImageStorage,                                                "VLImageStorageTrial",                                                "RETIRED_VLImageStorage" },
    { UID_RETIRED_VLMultiframeImageStorage,                                      "VLMultiFrameImageStorageTrial",                                      "RETIRED_VLMultiframeImageStorage" },
    { UID_RETIRED_XRayAngiographicBiPlaneImageStorage,                           "XRayAngiographicBiPlaneImageStorage",                                "RETIRED_XRayAngiographicBiPlaneImageStorage" },
    // Storage (DICOS)
    { UID_DICOS_CTImageStorage,                                                  "DICOSCTImageStorage",                                                "DICOS_CTImageStorage" },
    { UID_DICOS_DigitalXRayImageStorageForPresentation,                          "DICOSDigitalXRayImageStorageForPresentation",                        "DICOS_DigitalXRayImageStorageForPresentation" },
    { UID_DICOS_DigitalXRayImageStorageForProcessing,                            "DICOSDigitalXRayImageStorageForProcessing",                          "DICOS_DigitalXRayImageStorageForProcessing" },
    { UID_DICOS_ThreatDetectionReportStorage,                                    "DICOSThreatDetectionReportStorage",                                  "DICOS_ThreatDetectionReportStorage" },
    { UID_DICOS_2DAITStorage,                                                    "DICOS2DAITStorage",                                                  "DICOS_2DAITStorage" },
    { UID_DICOS_3DAITStorage,                                                    "DICOS3DAITStorage",                                                  "DICOS_3DAITStorage" },
    { UID_DICOS_QuadrupoleResonanceStorage,                                      "DICOSQuadrupoleResonanceStorage",                                    "DICOS_QuadrupoleResonanceStorage" },
    // Storage (DICONDE)
    { UID_DICONDE_EddyCurrentImageStorage,                                       "EddyCurrentImageStorage",                                            "DICONDE_EddyCurrentImageStorage" },
    { UID_DICONDE_EddyCurrentMultiframeImageStorage,                             "EddyCurrentMultiFrameImageStorage",                                  "DICONDE_EddyCurrentMultiframeImageStorage" },

    // Query/Retrieve
    { UID_FINDPatientRootQueryRetrieveInformationModel,                          "PatientRootQueryRetrieveInformationModelFind",                       "FINDPatientRootQueryRetrieveInformationModel" },
    { UID_FINDStudyRootQueryRetrieveInformationModel,                            "StudyRootQueryRetrieveInformationModelFind",                         "FINDStudyRootQueryRetrieveInformationModel" },
    { UID_GETPatientRootQueryRetrieveInformationModel,                           "PatientRootQueryRetrieveInformationModelGet",                        "GETPatientRootQueryRetrieveInformationModel" },
    { UID_GETStudyRootQueryRetrieveInformationModel,                             "StudyRootQueryRetrieveInformationModelGet",                          "GETStudyRootQueryRetrieveInformationModel" },
    { UID_MOVEPatientRootQueryRetrieveInformationModel,                          "PatientRootQueryRetrieveInformationModelMove",                       "MOVEPatientRootQueryRetrieveInformationModel" },
    { UID_MOVEStudyRootQueryRetrieveInformationModel,                            "StudyRootQueryRetrieveInformationModelMove",                         "MOVEStudyRootQueryRetrieveInformationModel" },
    { UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel,             "PatientStudyOnlyQueryRetrieveInformationModelFind",                  "RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel" },
    { UID_GETCompositeInstanceRetrieveWithoutBulkData,                           "CompositeInstanceRetrieveWithoutBulkDataGet",                        "GETCompositeInstanceRetrieveWithoutBulkData" },
    { UID_GETCompositeInstanceRootRetrieve,                                      "CompositeInstanceRootRetrieveGet",                                   "GETCompositeInstanceRootRetrieve" },
    { UID_RETIRED_GETPatientStudyOnlyQueryRetrieveInformationModel,              "PatientStudyOnlyQueryRetrieveInformationModelGet",                   "RETIRED_GETPatientStudyOnlyQueryRetrieveInformationModel" },
    { UID_MOVECompositeInstanceRootRetrieve,                                     "CompositeInstanceRootRetrieveMove",                                  "MOVECompositeInstanceRootRetrieve" },
    { UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel,             "PatientStudyOnlyQueryRetrieveInformationModelMove",                  "RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel" },

    // Modality Worklist
    { UID_FINDModalityWorklistInformationModel,                                  "ModalityWorklistInformationModelFind",                               "FINDModalityWorklistInformationModel" },

    // General Purpose Worklist (retired)
    { UID_RETIRED_FINDGeneralPurposeWorklistInformationModel,                    "GeneralPurposeWorklistInformationModelFind",                         "RETIRED_FINDGeneralPurposeWorklistInformationModel" },
    { UID_RETIRED_GeneralPurposePerformedProcedureStepSOPClass,                  "GeneralPurposePerformedProcedureStep",                               "RETIRED_GeneralPurposePerformedProcedureStepSOPClass" },
    { UID_RETIRED_GeneralPurposeScheduledProcedureStepSOPClass,                  "GeneralPurposeScheduledProcedureStep",                               "RETIRED_GeneralPurposeScheduledProcedureStepSOPClass" },
    { UID_RETIRED_GeneralPurposeWorklistManagementMetaSOPClass,                  "GeneralPurposeWorklistManagementMeta",                               "RETIRED_GeneralPurposeWorklistManagementMetaSOPClass" },

    // MPPS
    { UID_ModalityPerformedProcedureStepNotificationSOPClass,                    "ModalityPerformedProcedureStepNotification",                         "ModalityPerformedProcedureStepNotificationSOPClass" },
    { UID_ModalityPerformedProcedureStepRetrieveSOPClass,                        "ModalityPerformedProcedureStepRetrieve",                             "ModalityPerformedProcedureStepRetrieveSOPClass" },
    { UID_ModalityPerformedProcedureStepSOPClass,                                "ModalityPerformedProcedureStep",                                     "ModalityPerformedProcedureStepSOPClass" },

    // Radiotherapy
    { UID_RTConventionalMachineVerification,                                     "RTConventionalMachineVerification",                                  "RTConventionalMachineVerification" },
    { UID_RTIonMachineVerification,                                              "RTIonMachineVerification",                                           "RTIonMachineVerification" },

    // Unified Worklist and Procedure Step
    { UID_UnifiedWorklistAndProcedureStepServiceClass,                           "UnifiedWorklistAndProcedureStep",                                    "UnifiedWorklistAndProcedureStepServiceClass" },
    { UID_UnifiedProcedureStepPushSOPClass,                                      "UnifiedProcedureStepPush",                                           "UnifiedProcedureStepPushSOPClass" },
    { UID_UnifiedProcedureStepWatchSOPClass,                                     "UnifiedProcedureStepWatch",                                          "UnifiedProcedureStepWatchSOPClass" },
    { UID_UnifiedProcedureStepPullSOPClass,                                      "UnifiedProcedureStepPull",                                           "UnifiedProcedureStepPullSOPClass" },
    { UID_UnifiedProcedureStepEventSOPClass,                                     "UnifiedProcedureStepEvent",                                          "UnifiedProcedureStepEventSOPClass" },
    { UID_UnifiedProcedureStepQuerySOPClass,                                     "UnifiedProcedureStepQuery",                                          "UnifiedProcedureStepQuerySOPClass" },
    { UID_UPSGlobalSubscriptionSOPInstance,                                      "UPSGlobalSubscriptionInstance",                                      "UPSGlobalSubscriptionSOPInstance" },
    { UID_UPSFilteredGlobalSubscriptionSOPInstance,                              "UPSFilteredGlobalSubscriptionInstance",                              "UPSFilteredGlobalSubscriptionSOPInstance" },

    // Storage Commitment
    { UID_RETIRED_StorageCommitmentPullModelSOPClass,                            "StorageCommitmentPullModel",                                         "RETIRED_StorageCommitmentPullModelSOPClass" },
    { UID_RETIRED_StorageCommitmentPullModelSOPInstance,                         "StorageCommitmentPullModelInstance",                                 "RETIRED_StorageCommitmentPullModelSOPInstance" },
    { UID_StorageCommitmentPushModelSOPClass,                                    "StorageCommitmentPushModel",                                         "StorageCommitmentPushModelSOPClass" },
    { UID_StorageCommitmentPushModelSOPInstance,                                 "StorageCommitmentPushModelInstance",                                 "StorageCommitmentPushModelSOPInstance" },

    // Hanging Protocol Storage and Query/Retrieve
    { UID_HangingProtocolStorage,                                                "HangingProtocolStorage",                                             "HangingProtocolStorage" },
    { UID_FINDHangingProtocolInformationModel,                                   "HangingProtocolInformationModelFind",                                "FINDHangingProtocolInformationModel" },
    { UID_MOVEHangingProtocolInformationModel,                                   "HangingProtocolInformationModelMove",                                "MOVEHangingProtocolInformationModel" },
    { UID_GETHangingProtocolInformationModel,                                    "HangingProtocolInformationModelGet",                                 "GETHangingProtocolInformationModel" },

    // Relevant Patient Information Query
    { UID_BreastImagingRelevantPatientInformationQuery,                          "BreastImagingRelevantPatientInformationQuery",                       "BreastImagingRelevantPatientInformationQuery" },
    { UID_CardiacRelevantPatientInformationQuery,                                "CardiacRelevantPatientInformationQuery",                             "CardiacRelevantPatientInformationQuery" },
    { UID_GeneralRelevantPatientInformationQuery,                                "GeneralRelevantPatientInformationQuery",                             "GeneralRelevantPatientInformationQuery" },

    // Color Palette Storage and Query/Retrieve
    { UID_ColorPaletteStorage,                                                   "ColorPaletteStorage",                                                "ColorPaletteStorage" },
    { UID_FINDColorPaletteInformationModel,                                      "ColorPaletteQueryRetrieveInformationModelFind",                      "FINDColorPaletteInformationModel" },
    { UID_MOVEColorPaletteInformationModel,                                      "ColorPaletteQueryRetrieveInformationModelMove",                      "MOVEColorPaletteInformationModel" },
    { UID_GETColorPaletteInformationModel,                                       "ColorPaletteQueryRetrieveInformationModelGet",                       "GETColorPaletteInformationModel" },

    // Implant Template Storage and Query/Retrieve
    { UID_GenericImplantTemplateStorage,                                         "GenericImplantTemplateStorage",                                      "GenericImplantTemplateStorage" },
    { UID_FINDGenericImplantTemplateInformationModel,                            "GenericImplantTemplateInformationModelFind",                         "FINDGenericImplantTemplateInformationModel" },
    { UID_MOVEGenericImplantTemplateInformationModel,                            "GenericImplantTemplateInformationModelMove",                         "MOVEGenericImplantTemplateInformationModel" },
    { UID_GETGenericImplantTemplateInformationModel,                             "GenericImplantTemplateInformationModelGet",                          "GETGenericImplantTemplateInformationModel" },
    { UID_ImplantAssemblyTemplateStorage,                                        "ImplantAssemblyTemplateStorage",                                     "ImplantAssemblyTemplateStorage" },
    { UID_FINDImplantAssemblyTemplateInformationModel,                           "ImplantAssemblyTemplateInformationModelFind",                        "FINDImplantAssemblyTemplateInformationModel" },
    { UID_MOVEImplantAssemblyTemplateInformationModel,                           "ImplantAssemblyTemplateInformationModelMove",                        "MOVEImplantAssemblyTemplateInformationModel" },
    { UID_GETImplantAssemblyTemplateInformationModel,                            "ImplantAssemblyTemplateInformationModelGet",                         "GETImplantAssemblyTemplateInformationModel" },
    { UID_ImplantTemplateGroupStorage,                                           "ImplantTemplateGroupStorage",                                        "ImplantTemplateGroupStorage" },
    { UID_FINDImplantTemplateGroupInformationModel,                              "ImplantTemplateGroupInformationModelFind",                           "FINDImplantTemplateGroupInformationModel" },
    { UID_MOVEImplantTemplateGroupInformationModel,                              "ImplantTemplateGroupInformationModelMove",                           "MOVEImplantTemplateGroupInformationModel" },
    { UID_GETImplantTemplateGroupInformationModel,                               "ImplantTemplateGroupInformationModelGet",                            "GETImplantTemplateGroupInformationModel" },

    // Defined Procedure Protocol Query/Retrieve
    { UID_FINDDefinedProcedureProtocolInformationModel,                          "DefinedProcedureProtocolInformationModelFind",                       "FINDDefinedProcedureProtocolInformationModel" },
    { UID_MOVEDefinedProcedureProtocolInformationModel,                          "DefinedProcedureProtocolInformationModelMove",                       "MOVEDefinedProcedureProtocolInformationModel" },
    { UID_GETDefinedProcedureProtocolInformationModel,                           "DefinedProcedureProtocolInformationModelGet",                        "GETDefinedProcedureProtocolInformationModel" },

    // Protocol Approval Query/Retrieve
    { UID_FINDProtocolApprovalInformationModel,                                  "ProtocolApprovalInformationModelFind",                               "FINDProtocolApprovalInformationModel" },
    { UID_MOVEProtocolApprovalInformationModel,                                  "ProtocolApprovalInformationModelMove",                               "MOVEProtocolApprovalInformationModel" },
    { UID_GETProtocolApprovalInformationModel,                                   "ProtocolApprovalInformationModelGet",                                "GETProtocolApprovalInformationModel" },

    // Inventory Storage, Query/Retrieve, and related Services
    { UID_InventoryStorage,                                                      "InventoryStorage",                                                   "InventoryStorage" },
    { UID_FINDInventory,                                                         "InventoryFind",                                                      "FINDInventory" },
    { UID_MOVEInventory,                                                         "InventoryMove",                                                      "MOVEInventory" },
    { UID_GETInventory,                                                          "InventoryGet",                                                       "GETInventory" },
    { UID_InventoryCreation,                                                     "InventoryCreation",                                                  "InventoryCreation" },
    { UID_RepositoryQuery,                                                       "RepositoryQuery",                                                    "RepositoryQuery" },
    { UID_StorageManagementSOPInstance,                                          "StorageManagementInstance",                                          "StorageManagementSOPInstance" },

    // Print
    { UID_BasicAnnotationBoxSOPClass,                                            "BasicAnnotationBox",                                                 "BasicAnnotationBoxSOPClass" },
    { UID_BasicColorImageBoxSOPClass,                                            "BasicColorImageBox",                                                 "BasicColorImageBoxSOPClass" },
    { UID_BasicColorPrintManagementMetaSOPClass,                                 "BasicColorPrintManagementMeta",                                      "BasicColorPrintManagementMetaSOPClass" },
    { UID_BasicFilmBoxSOPClass,                                                  "BasicFilmBox",                                                       "BasicFilmBoxSOPClass" },
    { UID_BasicFilmSessionSOPClass,                                              "BasicFilmSession",                                                   "BasicFilmSessionSOPClass" },
    { UID_BasicGrayscaleImageBoxSOPClass,                                        "BasicGrayscaleImageBox",                                             "BasicGrayscaleImageBoxSOPClass" },
    { UID_BasicGrayscalePrintManagementMetaSOPClass,                             "BasicGrayscalePrintManagementMeta",                                  "BasicGrayscalePrintManagementMetaSOPClass" },
    { UID_PresentationLUTSOPClass,                                               "PresentationLUT",                                                    "PresentationLUTSOPClass" },
    { UID_PrintJobSOPClass,                                                      "PrintJob",                                                           "PrintJobSOPClass" },
    { UID_PrinterConfigurationRetrievalSOPClass,                                 "PrinterConfigurationRetrieval",                                      "PrinterConfigurationRetrievalSOPClass" },
    { UID_PrinterConfigurationRetrievalSOPInstance,                              "PrinterConfigurationRetrievalInstance",                              "PrinterConfigurationRetrievalSOPInstance" },
    { UID_PrinterSOPClass,                                                       "Printer",                                                            "PrinterSOPClass" },
    { UID_PrinterSOPInstance,                                                    "PrinterInstance",                                                    "PrinterSOPInstance" },
    { UID_RETIRED_BasicPrintImageOverlayBoxSOPClass,                             "BasicPrintImageOverlayBox",                                          "RETIRED_BasicPrintImageOverlayBoxSOPClass" },
    { UID_RETIRED_ImageOverlayBoxSOPClass,                                       "ImageOverlayBox",                                                    "RETIRED_ImageOverlayBoxSOPClass" },
    { UID_RETIRED_PrintQueueManagementSOPClass,                                  "PrintQueueManagement",                                               "RETIRED_PrintQueueManagementSOPClass" },
    { UID_RETIRED_PrintQueueSOPInstance,                                         "PrintQueue",                                                         "RETIRED_PrintQueueSOPInstance" },
    { UID_RETIRED_PullPrintRequestSOPClass,                                      "PullPrintRequest",                                                   "RETIRED_PullPrintRequestSOPClass" },
    { UID_RETIRED_PullStoredPrintManagementMetaSOPClass,                         "PullStoredPrintManagementMeta",                                      "RETIRED_PullStoredPrintManagementMetaSOPClass" },
    { UID_RETIRED_ReferencedColorPrintManagementMetaSOPClass,                    "ReferencedColorPrintManagementMeta",                                 "RETIRED_ReferencedColorPrintManagementMetaSOPClass" },
    { UID_RETIRED_ReferencedGrayscalePrintManagementMetaSOPClass,                "ReferencedGrayscalePrintManagementMeta",                             "RETIRED_ReferencedGrayscalePrintManagementMetaSOPClass" },
    { UID_RETIRED_ReferencedImageBoxSOPClass,                                    "ReferencedImageBox",                                                 "RETIRED_ReferencedImageBoxSOPClass" },
    { UID_VOILUTBoxSOPClass,                                                     "VOILUTBox",                                                          "VOILUTBoxSOPClass" },

    // Detached Management (retired)
    { UID_RETIRED_DetachedInterpretationManagementSOPClass,                      "DetachedInterpretationManagement",                                   "RETIRED_DetachedInterpretationManagementSOPClass" },
    { UID_RETIRED_DetachedPatientManagementMetaSOPClass,                         "DetachedPatientManagementMeta",                                      "RETIRED_DetachedPatientManagementMetaSOPClass" },
    { UID_RETIRED_DetachedPatientManagementSOPClass,                             "DetachedPatientManagement",                                          "RETIRED_DetachedPatientManagementSOPClass" },
    { UID_RETIRED_DetachedResultsManagementMetaSOPClass,                         "DetachedResultsManagementMeta",                                      "RETIRED_DetachedResultsManagementMetaSOPClass" },
    { UID_RETIRED_DetachedResultsManagementSOPClass,                             "DetachedResultsManagement",                                          "RETIRED_DetachedResultsManagementSOPClass" },
    { UID_RETIRED_DetachedStudyManagementMetaSOPClass,                           "DetachedStudyManagementMeta",                                        "RETIRED_DetachedStudyManagementMetaSOPClass" },
    { UID_RETIRED_DetachedStudyManagementSOPClass,                               "DetachedStudyManagement",                                            "RETIRED_DetachedStudyManagementSOPClass" },
    { UID_RETIRED_DetachedVisitManagementSOPClass,                               "DetachedVisitManagement",                                            "RETIRED_DetachedVisitManagementSOPClass" },

    // Procedure Log
    { UID_ProceduralEventLoggingSOPClass,                                        "ProceduralEventLogging",                                             "ProceduralEventLoggingSOPClass" },
    { UID_ProceduralEventLoggingSOPInstance,                                     "ProceduralEventLoggingInstance",                                     "ProceduralEventLoggingSOPInstance" },

    // Substance Administration
    { UID_SubstanceAdministrationLoggingSOPClass,                                "SubstanceAdministrationLogging",                                     "SubstanceAdministrationLoggingSOPClass" },
    { UID_SubstanceAdministrationLoggingSOPInstance,                             "SubstanceAdministrationLoggingInstance",                             "SubstanceAdministrationLoggingSOPInstance" },
    { UID_ProductCharacteristicsQuerySOPClass,                                   "ProductCharacteristicsQuery",                                        "ProductCharacteristicsQuerySOPClass" },
    { UID_SubstanceApprovalQuerySOPClass,                                        "SubstanceApprovalQuery",                                             "SubstanceApprovalQuerySOPClass" },

    // Media Creation
    { UID_MediaCreationManagementSOPClass,                                       "MediaCreationManagement",                                            "MediaCreationManagementSOPClass" },

    // SOP Class Relationship Negotiation
    { UID_StorageServiceClass,                                                   "Storage",                                                            "StorageServiceClass" },

    // Instance Availability Notification
    { UID_InstanceAvailabilityNotificationSOPClass,                              "InstanceAvailabilityNotification",                                   "InstanceAvailabilityNotificationSOPClass" },

    // Application Hosting
    { UID_NativeDICOMModel,                                                      "NativeDICOMModel",                                                   "NativeDICOMModel" },
    { UID_AbstractMultiDimensionalImageModel,                                    "AbstractMultiDimensionalImageModel",                                 "AbstractMultiDimensionalImageModel" },

    // Communication of Display Parameters
    { UID_DisplaySystemSOPClass,                                                 "DisplaySystem",                                                      "DisplaySystemSOPClass" },
    { UID_DisplaySystemSOPInstance,                                              "DisplaySystemInstance",                                              "DisplaySystemSOPInstance" },

    // Real-Time Video
    { UID_VideoEndoscopicImageRealTimeCommunication,                             "VideoEndoscopicImageRealTimeCommunication",                          "VideoEndoscopicImageRealTimeCommunication" },
    { UID_VideoPhotographicImageRealTimeCommunication,                           "VideoPhotographicImageRealTimeCommunication",                        "VideoPhotographicImageRealTimeCommunication" },
    { UID_AudioWaveformRealTimeCommunication,                                    "AudioWaveformRealTimeCommunication",                                 "AudioWaveformRealTimeCommunication" },
    { UID_RenditionSelectionDocumentRealTimeCommunication,                       "RenditionSelectionDocumentRealTimeCommunication",                    "RenditionSelectionDocumentRealTimeCommunication" },

    // Other
    { UID_RETIRED_BasicStudyContentNotificationSOPClass,                         "BasicStudyContentNotification",                                      "RETIRED_BasicStudyContentNotificationSOPClass" },
    { UID_RETIRED_StudyComponentManagementSOPClass,                              "StudyComponentManagement",                                           "RETIRED_StudyComponentManagementSOPClass" },
    { UID_VerificationSOPClass,                                                  "Verification",                                                       "VerificationSOPClass" },

    // Mapping Resources
    { UID_DICOMContentMappingResource,                                           "DICOMContentMappingResource",                                        "DICOMContentMappingResource" },

    // Coding Schemes
    { UID_DICOMControlledTerminologyCodingScheme,                                "DCM",                                                                "DICOMControlledTerminologyCodingScheme" },
    { UID_DICOMUIDRegistryCodingScheme,                                          "DCMUID",                                                             "DICOMUIDRegistryCodingScheme" },

    // Configuration Management LDAP UIDs
    { UID_LDAP_dicomAETitle,                                                     "dicomAETitle",                                                       "LDAP_dicomAETitle" },
    { UID_LDAP_dicomApplicationCluster,                                          "dicomApplicationCluster",                                            "LDAP_dicomApplicationCluster" },
    { UID_LDAP_dicomAssociationAcceptor,                                         "dicomAssociationAcceptor",                                           "LDAP_dicomAssociationAcceptor" },
    { UID_LDAP_dicomAssociationInitiator,                                        "dicomAssociationInitiator",                                          "LDAP_dicomAssociationInitiator" },
    { UID_LDAP_dicomAuthorizedNodeCertificateReference,                          "dicomAuthorizedNodeCertificateReference",                            "LDAP_dicomAuthorizedNodeCertificateReference" },
    { UID_LDAP_dicomConfigurationRoot,                                           "dicomConfigurationRoot",                                             "LDAP_dicomConfigurationRoot" },
    { UID_LDAP_dicomDescription,                                                 "dicomDescription",                                                   "LDAP_dicomDescription" },
    { UID_LDAP_dicomDevice,                                                      "dicomDevice",                                                        "LDAP_dicomDevice" },
    { UID_LDAP_dicomDeviceName,                                                  "dicomDeviceName",                                                    "LDAP_dicomDeviceName" },
    { UID_LDAP_dicomDeviceSerialNumber,                                          "dicomDeviceSerialNumber",                                            "LDAP_dicomDeviceSerialNumber" },
    { UID_LDAP_dicomDevicesRoot,                                                 "dicomDevicesRoot",                                                   "LDAP_dicomDevicesRoot" },
    { UID_LDAP_dicomHostname,                                                    "dicomHostname",                                                      "LDAP_dicomHostname" },
    { UID_LDAP_dicomInstalled,                                                   "dicomInstalled",                                                     "LDAP_dicomInstalled" },
    { UID_LDAP_dicomInstitutionAddress,                                          "dicomInstitutionAddress",                                            "LDAP_dicomInstitutionAddress" },
    { UID_LDAP_dicomInstitutionDepartmentName,                                   "dicomInstitutionDepartmentName",                                     "LDAP_dicomInstitutionDepartmentName" },
    { UID_LDAP_dicomInstitutionName,                                             "dicomInstitutionName",                                               "LDAP_dicomInstitutionName" },
    { UID_LDAP_dicomIssuerOfPatientID,                                           "dicomIssuerOfPatientID",                                             "LDAP_dicomIssuerOfPatientID" },
    { UID_LDAP_dicomManufacturer,                                                "dicomManufacturer",                                                  "LDAP_dicomManufacturer" },
    { UID_LDAP_dicomManufacturerModelName,                                       "dicomManufacturerModelName",                                         "LDAP_dicomManufacturerModelName" },
    { UID_LDAP_dicomNetworkAE,                                                   "dicomNetworkAE",                                                     "LDAP_dicomNetworkAE" },
    { UID_LDAP_dicomNetworkConnection,                                           "dicomNetworkConnection",                                             "LDAP_dicomNetworkConnection" },
    { UID_LDAP_dicomNetworkConnectionReference,                                  "dicomNetworkConnectionReference",                                    "LDAP_dicomNetworkConnectionReference" },
    { UID_LDAP_dicomPort,                                                        "dicomPort",                                                          "LDAP_dicomPort" },
    { UID_LDAP_dicomPreferredCalledAETitle,                                      "dicomPreferredCalledAETitle",                                        "LDAP_dicomPreferredCalledAETitle" },
    { UID_LDAP_dicomPreferredCallingAETitle,                                     "dicomPreferredCallingAETitle",                                       "LDAP_dicomPreferredCallingAETitle" },
    { UID_LDAP_dicomPrimaryDeviceType,                                           "dicomPrimaryDeviceType",                                             "LDAP_dicomPrimaryDeviceType" },
    { UID_LDAP_dicomRelatedDeviceReference,                                      "dicomRelatedDeviceReference",                                        "LDAP_dicomRelatedDeviceReference" },
    { UID_LDAP_dicomSOPClass,                                                    "dicomSOPClass",                                                      "LDAP_dicomSOPClass" },
    { UID_LDAP_dicomSoftwareVersion,                                             "dicomSoftwareVersion",                                               "LDAP_dicomSoftwareVersion" },
    { UID_LDAP_dicomStationName,                                                 "dicomStationName",                                                   "LDAP_dicomStationName" },
    { UID_LDAP_dicomSupportedCharacterSet,                                       "dicomSupportedCharacterSet",                                         "LDAP_dicomSupportedCharacterSet" },
    { UID_LDAP_dicomTLSCyphersuite,                                              "dicomTLSCyphersuite",                                                "LDAP_dicomTLSCyphersuite" },
    { UID_LDAP_dicomThisNodeCertificateReference,                                "dicomThisNodeCertificateReference",                                  "LDAP_dicomThisNodeCertificateReference" },
    { UID_LDAP_dicomTransferCapability,                                          "dicomTransferCapability",                                            "LDAP_dicomTransferCapability" },
    { UID_LDAP_dicomTransferRole,                                                "dicomTransferRole",                                                  "LDAP_dicomTransferRole" },
    { UID_LDAP_dicomTransferSyntax,                                              "dicomTransferSyntax",                                                "LDAP_dicomTransferSyntax" },
    { UID_LDAP_dicomUniqueAETitle,                                               "dicomUniqueAETitle",                                                 "LDAP_dicomUniqueAETitle" },
    { UID_LDAP_dicomUniqueAETitlesRegistryRoot,                                  "dicomUniqueAETitlesRegistryRoot",                                    "LDAP_dicomUniqueAETitlesRegistryRoot" },
    { UID_LDAP_dicomVendorData,                                                  "dicomVendorData",                                                    "LDAP_dicomVendorData" },

    // Well-known Frame of References
    { UID_ICBM452T1FrameOfReference,                                             "ICBM452T1",                                                          "ICBM452T1FrameOfReference" },
    { UID_ICBMSingleSubjectMRIFrameOfReference,                                  "ICBMSingleSubjectMRI",                                               "ICBMSingleSubjectMRIFrameOfReference" },
    { UID_IEC61217FixedCoordinateSystemFrameOfReference,                         "IEC61217FixedCoordinateSystem",                                      "IEC61217FixedCoordinateSystemFrameOfReference" },
    { UID_IEC61217TableTopCoordinateSystemFrameOfReference,                      "IEC61217TableTopCoordinateSystem",                                   "IEC61217TableTopCoordinateSystemFrameOfReference" },
    { UID_SPM2AVG152PDFrameOfReference,                                          "SPM2AVG152PD",                                                       "SPM2AVG152PDFrameOfReference" },
    { UID_SPM2AVG152T1FrameOfReference,                                          "SPM2AVG152T1",                                                       "SPM2AVG152T1FrameOfReference" },
    { UID_SPM2AVG152T2FrameOfReference,                                          "SPM2AVG152T2",                                                       "SPM2AVG152T2FrameOfReference" },
    { UID_SPM2AVG305T1FrameOfReference,                                          "SPM2AVG305T1",                                                       "SPM2AVG305T1FrameOfReference" },
    { UID_SPM2BRAINMASKFrameOfReference,                                         "SPM2BRAINMASK",                                                      "SPM2BRAINMASKFrameOfReference" },
    { UID_SPM2CSFFrameOfReference,                                               "SPM2CSF",                                                            "SPM2CSFFrameOfReference" },
    { UID_SPM2EPIFrameOfReference,                                               "SPM2EPI",                                                            "SPM2EPIFrameOfReference" },
    { UID_SPM2FILT1FrameOfReference,                                             "SPM2FILT1",                                                          "SPM2FILT1FrameOfReference" },
    { UID_SPM2GRAYFrameOfReference,                                              "SPM2GRAY",                                                           "SPM2GRAYFrameOfReference" },
    { UID_SPM2PDFrameOfReference,                                                "SPM2PD",                                                             "SPM2PDFrameOfReference" },
    { UID_SPM2PETFrameOfReference,                                               "SPM2PET",                                                            "SPM2PETFrameOfReference" },
    { UID_SPM2SINGLESUBJT1FrameOfReference,                                      "SINGLESUBJT1",                                                       "SPM2SINGLESUBJT1FrameOfReference" },
    { UID_SPM2SPECTFrameOfReference,                                             "SPM2SPECT",                                                          "SPM2SPECTFrameOfReference" },
    { UID_SPM2T1FrameOfReference,                                                "SPM2T1",                                                             "SPM2T1FrameOfReference" },
    { UID_SPM2T2FrameOfReference,                                                "SPM2T2",                                                             "SPM2T2FrameOfReference" },
    { UID_SPM2TRANSMFrameOfReference,                                            "SPM2TRANSM",                                                         "SPM2TRANSMFrameOfReference" },
    { UID_SPM2WHITEFrameOfReference,                                             "SPM2WHITE",                                                          "SPM2WHITEFrameOfReference" },
    { UID_StandardRoboticArmCoordinateSystemFrameOfReference,                    "StandardRoboticArmCoordinateSystem",                                 "StandardRoboticArmCoordinateSystemFrameOfReference" },
    { UID_TalairachBrainAtlasFrameOfReference,                                   "TalairachBrainAtlas",                                                "TalairachBrainAtlasFrameOfReference" },
    { UID_SRI24FrameOfReference,                                                 "SRI24",                                                              "SRI24FrameOfReference" },
    { UID_Colin27FrameOfReference,                                               "Colin27",                                                            "Colin27FrameOfReference" },
    { UID_LPBA40AIRFrameOfReference,                                             "LPBA40AIR",                                                          "LPBA40AIRFrameOfReference" },
    { UID_LPBA40FLIRTFrameOfReference,                                           "LPBA40FLIRT",                                                        "LPBA40FLIRTFrameOfReference" },
    { UID_LPBA40SPM5FrameOfReference,                                            "LPBA40SPM5",                                                         "LPBA40SPM5FrameOfReference" },

    // UTC Synchronization Frame of Reference
    { UID_UniversalCoordinatedTimeSynchronizationFrameOfReference,               "UTC",                                                                "UniversalCoordinatedTimeSynchronizationFrameOfReference" },

    // Well-known SOP Instances for Color Palettes
    { UID_HotIronColorPaletteSOPInstance,                                        "HotIronPalette",                                                     "HotIronColorPaletteSOPInstance" },
    { UID_HotMetalBlueColorPaletteSOPInstance,                                   "HotMetalBluePalette",                                                "HotMetalBlueColorPaletteSOPInstance" },
    { UID_PET20StepColorPaletteSOPInstance,                                      "PET20StepPalette",                                                   "PET20StepColorPaletteSOPInstance" },
    { UID_PETColorPaletteSOPInstance,                                            "PETPalette",                                                         "PETColorPaletteSOPInstance" },
    { UID_SpringColorPaletteSOPInstance,                                         "SpringPalette",                                                      "SpringColorPaletteSOPInstance" },
    { UID_SummerColorPaletteSOPInstance,                                         "SummerPalette",                                                      "SummerColorPaletteSOPInstance" },
    { UID_FallColorPaletteSOPInstance,                                           "FallPalette",                                                        "FallColorPaletteSOPInstance" },
    { UID_WinterColorPaletteSOPInstance,                                         "WinterPalette",                                                      "WinterColorPaletteSOPInstance" },

    // Draft Supplements
    { UID_DRAFT_RTBeamsDeliveryInstructionStorage,                               "RTBeamsDeliveryInstructionStorageTrial",                             "DRAFT_RTBeamsDeliveryInstructionStorage" },
    { UID_DRAFT_RTConventionalMachineVerification,                               "RTConventionalMachineVerificationTrial",                             "DRAFT_RTConventionalMachineVerification" },
    { UID_DRAFT_RTIonMachineVerification,                                        "RTIonMachineVerificationTrial",                                      "DRAFT_RTIonMachineVerification" },
    { UID_DRAFT_SRAudioStorage,                                                  "AudioSRStorageTrial",                                                "DRAFT_SRAudioStorage" },
    { UID_DRAFT_SRComprehensiveStorage,                                          "ComprehensiveSRStorageTrial",                                        "DRAFT_SRComprehensiveStorage" },
    { UID_DRAFT_SRDetailStorage,                                                 "DetailSRStorageTrial",                                               "DRAFT_SRDetailStorage" },
    { UID_DRAFT_SRTextStorage,                                                   "TextSRStorageTrial",                                                 "DRAFT_SRTextStorage" },
    { UID_DRAFT_WaveformStorage,                                                 "WaveformStorageTrial",                                               "DRAFT_WaveformStorage" },
    { UID_DRAFT_UnifiedWorklistAndProcedureStepServiceClass,                     "UnifiedWorklistAndProcedureStepTrial",                               "DRAFT_UnifiedWorklistAndProcedureStepServiceClass" },
    { UID_DRAFT_UnifiedProcedureStepPushSOPClass,                                "UnifiedProcedureStepPushTrial",                                      "DRAFT_UnifiedProcedureStepPushSOPClass" },
    { UID_DRAFT_UnifiedProcedureStepWatchSOPClass,                               "UnifiedProcedureStepWatchTrial",                                     "DRAFT_UnifiedProcedureStepWatchSOPClass" },
    { UID_DRAFT_UnifiedProcedureStepPullSOPClass,                                "UnifiedProcedureStepPullTrial",                                      "DRAFT_UnifiedProcedureStepPullSOPClass" },
    { UID_DRAFT_UnifiedProcedureStepEventSOPClass,                               "UnifiedProcedureStepEventTrial",                                     "DRAFT_UnifiedProcedureStepEventSOPClass" },

    { NULL, NULL, NULL}
};

static const int uidNameMap_size = OFstatic_cast(int, sizeof(uidNameMap) / sizeof(UIDNameMap));


/** an array of const strings containing all known Storage SOP Classes
 *  that fit into the conventional PATIENT-STUDY-SERIES-INSTANCE information
 *  model, i.e. everything a Storage SCP might want to store in a PACS.
 *  Special cases such as the Hanging Protocol or Color Palette Storage SOP
 *  Class are not included in this list but in dcmNonPatientStorageSOPClassUIDs.
 *
 *  THIS LIST CONTAINS ALL STORAGE SOP CLASSES INCLUDING RETIRED ONES
 *  AND IS (MUCH) LARGER THAN 64 ENTRIES.
 */

const char* dcmAllStorageSOPClassUIDs[] = {
    UID_AcquisitionContextSRStorage,
    UID_AdvancedBlendingPresentationStateStorage,
    UID_AmbulatoryECGWaveformStorage,
    UID_ArterialPulseWaveformStorage,
    UID_AutorefractionMeasurementsStorage,
    UID_BasicStructuredDisplayStorage,
    UID_BasicTextSRStorage,
    UID_BasicVoiceAudioWaveformStorage,
    UID_BlendingSoftcopyPresentationStateStorage,
    UID_BodyPositionWaveformStorage,
    UID_BreastProjectionXRayImageStorageForPresentation,
    UID_BreastProjectionXRayImageStorageForProcessing,
    UID_BreastTomosynthesisImageStorage,
    UID_CardiacElectrophysiologyWaveformStorage,
    UID_CArmPhotonElectronRadiationRecordStorage,
    UID_CArmPhotonElectronRadiationStorage,
    UID_ChestCADSRStorage,
    UID_ColonCADSRStorage,
    UID_ColorSoftcopyPresentationStateStorage,
    UID_CompositingPlanarMPRVolumetricPresentationStateStorage,
    UID_Comprehensive3DSRStorage,
    UID_ComprehensiveSRStorage,
    UID_ComputedRadiographyImageStorage,
    UID_ContentAssessmentResultsStorage,
    UID_CornealTopographyMapStorage,
    UID_CTImageStorage,
    UID_CTPerformedProcedureProtocolStorage,
    UID_DeformableSpatialRegistrationStorage,
    UID_DermoscopicPhotographyImageStorage,
    UID_DigitalIntraOralXRayImageStorageForPresentation,
    UID_DigitalIntraOralXRayImageStorageForProcessing,
    UID_DigitalMammographyXRayImageStorageForPresentation,
    UID_DigitalMammographyXRayImageStorageForProcessing,
    UID_DigitalXRayImageStorageForPresentation,
    UID_DigitalXRayImageStorageForProcessing,
    UID_ElectromyogramWaveformStorage,
    UID_ElectrooculogramWaveformStorage,
    UID_EncapsulatedCDAStorage,
    UID_EncapsulatedMTLStorage,
    UID_EncapsulatedOBJStorage,
    UID_EncapsulatedPDFStorage,
    UID_EncapsulatedSTLStorage,
    UID_EnhancedContinuousRTImageStorage,
    UID_EnhancedCTImageStorage,
    UID_EnhancedMRColorImageStorage,
    UID_EnhancedMRImageStorage,
    UID_EnhancedPETImageStorage,
    UID_EnhancedRTImageStorage,
    UID_EnhancedSRStorage,
    UID_EnhancedUSVolumeStorage,
    UID_EnhancedXAImageStorage,
    UID_EnhancedXRayRadiationDoseSRStorage,
    UID_EnhancedXRFImageStorage,
    UID_ExtensibleSRStorage,
    UID_General32BitECGWaveformStorage,
    UID_GeneralAudioWaveformStorage,
    UID_GeneralECGWaveformStorage,
    UID_GrayscalePlanarMPRVolumetricPresentationStateStorage,
    UID_GrayscaleSoftcopyPresentationStateStorage,
    UID_HemodynamicWaveformStorage,
    UID_ImplantationPlanSRStorage,
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
    UID_MicroscopyBulkSimpleAnnotationsStorage,
    UID_MRImageStorage,
    UID_MRSpectroscopyStorage,
    UID_MultichannelRespiratoryWaveformStorage,
    UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage,
    UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage,
    UID_MultiframeSingleBitSecondaryCaptureImageStorage,
    UID_MultiframeTrueColorSecondaryCaptureImageStorage,
    UID_MultipleVolumeRenderingVolumetricPresentationStateStorage,
    UID_NuclearMedicineImageStorage,
    UID_OphthalmicAxialMeasurementsStorage,
    UID_OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage,
    UID_OphthalmicOpticalCoherenceTomographyEnFaceImageStorage,
    UID_OphthalmicPhotography16BitImageStorage,
    UID_OphthalmicPhotography8BitImageStorage,
    UID_OphthalmicThicknessMapStorage,
    UID_OphthalmicTomographyImageStorage,
    UID_OphthalmicVisualFieldStaticPerimetryMeasurementsStorage,
    UID_ParametricMapStorage,
    UID_PatientRadiationDoseSRStorage,
    UID_PerformedImagingAgentAdministrationSRStorage,
    UID_PlannedImagingAgentAdministrationSRStorage,
    UID_PhotoacousticImageStorage,
    UID_PositronEmissionTomographyImageStorage,
    UID_ProcedureLogStorage,
    UID_PseudoColorSoftcopyPresentationStateStorage,
    UID_RadiopharmaceuticalRadiationDoseSRStorage,
    UID_RawDataStorage,
    UID_RealWorldValueMappingStorage,
    UID_RespiratoryWaveformStorage,
    UID_RoboticArmRadiationStorage,
    UID_RoboticRadiationRecordStorage,
    UID_RoutineScalpElectroencephalogramWaveformStorage,
    UID_RTBeamsDeliveryInstructionStorage,
    UID_RTBeamsTreatmentRecordStorage,
    UID_RTBrachyApplicationSetupDeliveryInstructionStorage,
    UID_RTBrachyTreatmentRecordStorage,
    UID_RTDoseStorage,
    UID_RTImageStorage,
    UID_RTIonBeamsTreatmentRecordStorage,
    UID_RTIonPlanStorage,
    UID_RTPatientPositionAcquisitionInstructionStorage,
    UID_RTPhysicianIntentStorage,
    UID_RTPlanStorage,
    UID_RTRadiationRecordSetStorage,
    UID_RTRadiationSalvageRecordStorage,
    UID_RTRadiationSetDeliveryInstructionStorage,
    UID_RTRadiationSetStorage,
    UID_RTSegmentAnnotationStorage,
    UID_RTStructureSetStorage,
    UID_RTTreatmentPreparationStorage,
    UID_RTTreatmentSummaryRecordStorage,
    UID_SecondaryCaptureImageStorage,
    UID_SegmentationStorage,
    UID_SegmentedVolumeRenderingVolumetricPresentationStateStorage,
    UID_SimplifiedAdultEchoSRStorage,
    UID_SleepElectroencephalogramWaveformStorage,
    UID_SpatialFiducialsStorage,
    UID_SpatialRegistrationStorage,
    UID_SpectaclePrescriptionReportStorage,
    UID_StereometricRelationshipStorage,
    UID_SubjectiveRefractionMeasurementsStorage,
    UID_SurfaceScanMeshStorage,
    UID_SurfaceScanPointCloudStorage,
    UID_SurfaceSegmentationStorage,
    UID_TomotherapeuticRadiationRecordStorage,
    UID_TomotherapeuticRadiationStorage,
    UID_TractographyResultsStorage,
    UID_TwelveLeadECGWaveformStorage,
    UID_UltrasoundImageStorage,
    UID_UltrasoundMultiframeImageStorage,
    UID_VariableModalityLUTSoftcopyPresentationStateStorage,
    UID_VideoEndoscopicImageStorage,
    UID_VideoMicroscopicImageStorage,
    UID_VideoPhotographicImageStorage,
    UID_VisualAcuityMeasurementsStorage,
    UID_VLEndoscopicImageStorage,
    UID_VLMicroscopicImageStorage,
    UID_VLPhotographicImageStorage,
    UID_VLSlideCoordinatesMicroscopicImageStorage,
    UID_VLWholeSlideMicroscopyImageStorage,
    UID_VolumeRenderingVolumetricPresentationStateStorage,
    UID_WideFieldOphthalmicPhotographyStereographicProjectionImageStorage,
    UID_WideFieldOphthalmicPhotography3DCoordinatesImageStorage,
    UID_XAPerformedProcedureProtocolStorage,
    UID_XAXRFGrayscaleSoftcopyPresentationStateStorage,
    UID_XRay3DAngiographicImageStorage,
    UID_XRay3DCraniofacialImageStorage,
    UID_XRayAngiographicImageStorage,
    UID_XRayRadiationDoseSRStorage,
    UID_XRayRadiofluoroscopicImageStorage,
    // non-patient DICOM objects:
    // - do not add them here but in dcmNonPatientStorageSOPClassUIDs
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
    UID_RETIRED_VLMultiframeImageStorage,
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
    UID_DICOS_2DAITStorage,
    UID_DICOS_3DAITStorage,
    UID_DICOS_QuadrupoleResonanceStorage,
    // DICONDE
    UID_DICONDE_EddyCurrentImageStorage,
    UID_DICONDE_EddyCurrentMultiframeImageStorage,
    NULL
};

const int numberOfDcmAllStorageSOPClassUIDs = OFstatic_cast(int, sizeof(dcmAllStorageSOPClassUIDs) / sizeof(const char*) - 1);


/** an array of const strings containing all known Storage SOP Classes
 *  that do not fit into the conventional PATIENT-STUDY-SERIES-INSTANCE
 *  information model. See function dcmIsaStorageSOPClassUID().
 */

const char* dcmNonPatientStorageSOPClassUIDs[] = {
    UID_ColorPaletteStorage,
    UID_CTDefinedProcedureProtocolStorage,
    UID_GenericImplantTemplateStorage,
    UID_HangingProtocolStorage,
    UID_ImplantAssemblyTemplateStorage,
    UID_ImplantTemplateGroupStorage,
    UID_InventoryStorage,
    UID_ProtocolApprovalStorage,
    UID_XADefinedProcedureProtocolStorage,
    // end marker (important!)
    NULL
};


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
    UID_GrayscaleSoftcopyPresentationStateStorage,
    UID_HemodynamicWaveformStorage,
    UID_ImplantationPlanSRStorage,
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
    // recently approved
//  UID_AcquisitionContextSRStorage,
//  UID_AdvancedBlendingPresentationStateStorage,
//  UID_BodyPositionWaveformStorage,
//  UID_BreastProjectionXRayImageStorageForPresentation,
//  UID_BreastProjectionXRayImageStorageForProcessing,
//  UID_CArmPhotonElectronRadiationRecordStorage,
//  UID_CArmPhotonElectronRadiationStorage,
//  UID_CompositingPlanarMPRVolumetricPresentationStateStorage,
//  UID_ContentAssessmentResultsStorage,
//  UID_CornealTopographyMapStorage,
//  UID_CTPerformedProcedureProtocolStorage,
//  UID_DermoscopicPhotographyImageStorage,
//  UID_ElectromyogramWaveformStorage,
//  UID_ElectrooculogramWaveformStorage,
//  UID_EncapsulatedMTLStorage,
//  UID_EncapsulatedOBJStorage,
//  UID_EncapsulatedSTLStorage,
//  UID_EnhancedContinuousRTImageStorage,
//  UID_EnhancedRTImageStorage,
//  UID_EnhancedXRayRadiationDoseSRStorage,
//  UID_ExtensibleSRStorage,
//  UID_General32BitECGWaveformStorage,
//  UID_GrayscalePlanarMPRVolumetricPresentationStateStorage,
//  UID_MicroscopyBulkSimpleAnnotationsStorage,
//  UID_MultichannelRespiratoryWaveformStorage,
//  UID_MultipleVolumeRenderingVolumetricPresentationStateStorage,
//  UID_OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage,
//  UID_OphthalmicOpticalCoherenceTomographyEnFaceImageStorage,
//  UID_ParametricMapStorage,
//  UID_PatientRadiationDoseSRStorage,
//  UID_PerformedImagingAgentAdministrationSRStorage,
//  UID_PhotoacousticImageStorage,
//  UID_PlannedImagingAgentAdministrationSRStorage,
//  UID_RadiopharmaceuticalRadiationDoseSRStorage,
//  UID_RoboticArmRadiationStorage,
//  UID_RoboticRadiationRecordStorage,
//  UID_RoutineScalpElectroencephalogramWaveformStorage,
//  UID_RTBrachyApplicationSetupDeliveryInstructionStorage,
//  UID_RTPatientPositionAcquisitionInstructionStorage,
//  UID_RTPhysicianIntentStorage,
//  UID_RTRadiationRecordSetStorage,
//  UID_RTRadiationSalvageRecordStorage,
//  UID_RTRadiationSetDeliveryInstructionStorage,
//  UID_RTRadiationSetStorage,
//  UID_RTSegmentAnnotationStorage,
//  UID_RTTreatmentPreparationStorage,
//  UID_SegmentedVolumeRenderingVolumetricPresentationStateStorage,
//  UID_SimplifiedAdultEchoSRStorage,
//  UID_SleepElectroencephalogramWaveformStorage,
//  UID_TomotherapeuticRadiationRecordStorage,
//  UID_TomotherapeuticRadiationStorage,
//  UID_TractographyResultsStorage,
//  UID_VariableModalityLUTSoftcopyPresentationStateStorage,
//  UID_VolumeRenderingVolumetricPresentationStateStorage,
//  UID_WideFieldOphthalmicPhotographyStereographicProjectionImageStorage,
//  UID_WideFieldOphthalmicPhotography3DCoordinatesImageStorage,
//  UID_XAPerformedProcedureProtocolStorage,
    // non-patient
//  UID_ColorPaletteStorage,
//  UID_CTDefinedProcedureProtocolStorage,
//  UID_GenericImplantTemplateStorage,
//  UID_HangingProtocolStorage,
//  UID_ImplantAssemblyTemplateStorage,
//  UID_ImplantTemplateGroupStorage,
//  UID_InventoryStorage,
//  UID_ProtocolApprovalStorage,
//  UID_XADefinedProcedureProtocolStorage,
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
    UID_RETIRED_VLMultiframeImageStorage,
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
//  UID_DICOS_2DAITStorage,
//  UID_DICOS_3DAITStorage,
//  UID_DICOS_QuadrupoleResonanceStorage,
    // DICONDE
//  UID_DICONDE_EddyCurrentImageStorage,
//  UID_DICONDE_EddyCurrentMultiframeImageStorage,
    NULL
};

const int numberOfDcmLongSCUStorageSOPClassUIDs = OFstatic_cast(int, sizeof(dcmLongSCUStorageSOPClassUIDs) / sizeof(const char*) - 1);


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

const int numberOfDcmShortSCUStorageSOPClassUIDs = OFstatic_cast(int, sizeof(dcmShortSCUStorageSOPClassUIDs) / sizeof(const char*) - 1);


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
    UID_DermoscopicPhotographyImageStorage,
    UID_DigitalIntraOralXRayImageStorageForPresentation,
    UID_DigitalIntraOralXRayImageStorageForProcessing,
    UID_DigitalMammographyXRayImageStorageForPresentation,
    UID_DigitalMammographyXRayImageStorageForProcessing,
    UID_DigitalXRayImageStorageForPresentation,
    UID_DigitalXRayImageStorageForProcessing,
    UID_EnhancedContinuousRTImageStorage,
    UID_EnhancedCTImageStorage,
    UID_EnhancedMRColorImageStorage,
    UID_EnhancedMRImageStorage,
    UID_EnhancedPETImageStorage,
    UID_EnhancedRTImageStorage,
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
    UID_OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage,
    UID_OphthalmicOpticalCoherenceTomographyEnFaceImageStorage,
    UID_OphthalmicPhotography16BitImageStorage,
    UID_OphthalmicPhotography8BitImageStorage,
    UID_OphthalmicThicknessMapStorage,
    UID_OphthalmicTomographyImageStorage,
    UID_ParametricMapStorage,
    UID_PhotoacousticImageStorage,
    UID_PositronEmissionTomographyImageStorage,
    UID_RTImageStorage,
    UID_SecondaryCaptureImageStorage,
    UID_SegmentationStorage,
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
    UID_WideFieldOphthalmicPhotographyStereographicProjectionImageStorage,
    UID_WideFieldOphthalmicPhotography3DCoordinatesImageStorage,
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
    UID_RETIRED_VLMultiframeImageStorage,
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

const int numberOfDcmImageSOPClassUIDs = OFstatic_cast(int, sizeof(dcmImageSOPClassUIDs) / sizeof(const char*) - 1);


typedef struct {
    const char *sopClass;       /* official UID value */
    const char *modality;       /* short character code */
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
    { UID_AcquisitionContextSRStorage,                             "SRa",  4096 },
    { UID_AdvancedBlendingPresentationStateStorage,                "PSa",  4096 },
    { UID_AmbulatoryECGWaveformStorage,                            "ECA",  4096 },
    { UID_ArterialPulseWaveformStorage,                            "WVa",  4096 },
    { UID_AutorefractionMeasurementsStorage,                       "OPa",  4096 },
    { UID_BasicStructuredDisplayStorage,                           "SD",   4096 },
    { UID_BasicTextSRStorage,                                      "SRt",  4096 },
    { UID_BasicVoiceAudioWaveformStorage,                          "AUV",  4096 },
    { UID_BlendingSoftcopyPresentationStateStorage,                "PSb",  4096 },
    { UID_BodyPositionWaveformStorage,                             "WVb",  4096 },
    { UID_BreastProjectionXRayImageStorageForPresentation,         "BX",   4096 * 4096 * 2 },
    { UID_BreastProjectionXRayImageStorageForProcessing,           "BP",   4096 * 4096 * 2 },
    { UID_BreastTomosynthesisImageStorage,                         "BT",   4096 * 4096 * 2 },
    { UID_CardiacElectrophysiologyWaveformStorage,                 "WVc",  4096 },
    { UID_CArmPhotonElectronRadiationRecordStorage,                "RRc",  4096 },
    { UID_CArmPhotonElectronRadiationStorage,                      "Rca",  4096 },
    { UID_ChestCADSRStorage,                                       "SRh",  4096 },
    { UID_ColonCADSRStorage,                                       "SRo",  4096 },
    { UID_ColorPaletteStorage,                                     "CP",   4096 },
    { UID_ColorSoftcopyPresentationStateStorage,                   "PSc",  4096 },
    { UID_CompositingPlanarMPRVolumetricPresentationStateStorage,  "VPc",  4096 },
    { UID_Comprehensive3DSRStorage,                                "SR3",  4096 },
    { UID_ComprehensiveSRStorage,                                  "SRc",  4096 },
    { UID_ComputedRadiographyImageStorage,                         "CR",   2048 * 2048 * 2 },
    { UID_ContentAssessmentResultsStorage,                         "AS",   4096 },
    { UID_CornealTopographyMapStorage,                             "CM",   512 * 512 },
    { UID_CTDefinedProcedureProtocolStorage,                       "PPcd", 4096 },  /* was PPd */
    { UID_CTImageStorage,                                          "CT",   512 * 512 * 2 },
    { UID_CTPerformedProcedureProtocolStorage,                     "PPcp", 4096 },  /* was PPp */
    { UID_DeformableSpatialRegistrationStorage,                    "RGd",  4096 },
    { UID_DermoscopicPhotographyImageStorage,                      "VLd",  768 * 576 * 3 },
    { UID_DigitalIntraOralXRayImageStorageForPresentation,         "DXo",  1024 * 1024 * 2 },
    { UID_DigitalIntraOralXRayImageStorageForProcessing,           "DPo",  1024 * 1024 * 2 },
    { UID_DigitalMammographyXRayImageStorageForPresentation,       "DXm",  4096 * 4096 * 2 },
    { UID_DigitalMammographyXRayImageStorageForProcessing,         "DPm",  4096 * 4096 * 2 },
    { UID_DigitalXRayImageStorageForPresentation,                  "DX",   2048 * 2048 * 2 },
    { UID_DigitalXRayImageStorageForProcessing,                    "DP",   2048 * 2048 * 2 },
    { UID_ElectromyogramWaveformStorage,                           "EMG",  4096 },
    { UID_ElectrooculogramWaveformStorage,                         "EOG",  4096 },
    { UID_EncapsulatedCDAStorage,                                  "CDA",  4096 },
    { UID_EncapsulatedMTLStorage,                                  "MTL",  4096 },
    { UID_EncapsulatedOBJStorage,                                  "OBJ",  4096 },
    { UID_EncapsulatedPDFStorage,                                  "PDF",  1024 * 1024 },
    { UID_EncapsulatedSTLStorage,                                  "STL",  4096 },
    { UID_EnhancedContinuousRTImageStorage,                        "RIc",  4096 },
    { UID_EnhancedCTImageStorage,                                  "CTe",  256 * 512 * 512 },
    { UID_EnhancedMRColorImageStorage,                             "MRc",  256 * 512 * 512 * 3 },
    { UID_EnhancedMRImageStorage,                                  "MRe",  256 * 512 * 512 },
    { UID_EnhancedPETImageStorage,                                 "PIe",  512 * 512 * 2 },
    { UID_EnhancedRTImageStorage,                                  "RIe",  4096 },
    { UID_EnhancedSRStorage,                                       "SRe",  4096 },
    { UID_EnhancedUSVolumeStorage,                                 "USe",  512 * 512 },
    { UID_EnhancedXAImageStorage,                                  "XAe",  256 * 512 * 512 },
    { UID_EnhancedXRayRadiationDoseSRStorage,                      "SRde", 4096 },
    { UID_EnhancedXRFImageStorage,                                 "RFe",  256 * 512 * 512 },
    { UID_ExtensibleSRStorage,                                     "SRx",  4096 },
    { UID_General32BitECGWaveformStorage,                          "ECGh", 4096 },
    { UID_GeneralAudioWaveformStorage,                             "AUG",  4096 },
    { UID_GeneralECGWaveformStorage,                               "ECG",  4096 },
    { UID_GenericImplantTemplateStorage,                           "IT",   4096 },
    { UID_GrayscalePlanarMPRVolumetricPresentationStateStorage,    "VPg",  4096 },
    { UID_GrayscaleSoftcopyPresentationStateStorage,               "PSg",  4096 },
    { UID_HangingProtocolStorage,                                  "HP",   4096 },
    { UID_HemodynamicWaveformStorage,                              "WVh",  4096 },
    { UID_ImplantAssemblyTemplateStorage,                          "ITa",  4096 },
    { UID_ImplantationPlanSRStorage,                               "SRi",  4096 },
    { UID_ImplantTemplateGroupStorage,                             "ITg",  4096 },
    { UID_IntraocularLensCalculationsStorage,                      "OPc",  4096 },
    { UID_IntravascularOpticalCoherenceTomographyImageStorageForPresentation, "OCt", 512 * 512 },
    { UID_IntravascularOpticalCoherenceTomographyImageStorageForProcessing, "OCp", 512 * 512 },
    { UID_InventoryStorage,                                        "INV",  4096 },
    { UID_KeratometryMeasurementsStorage,                          "OPk",  4096 },
    { UID_KeyObjectSelectionDocumentStorage,                       "KO",   4096 },
    { UID_LegacyConvertedEnhancedCTImageStorage,                   "CTl",  512 * 512 * 2 },
    { UID_LegacyConvertedEnhancedMRImageStorage,                   "MRl",  256 * 256 * 2 },
    { UID_LegacyConvertedEnhancedPETImageStorage,                  "PIl",  512 * 512 * 2 },
    { UID_LensometryMeasurementsStorage,                           "OPl",  4096 },
    { UID_MacularGridThicknessAndVolumeReportStorage,              "SRg",  4096 },
    { UID_MammographyCADSRStorage,                                 "SRm",  4096 },
    { UID_MicroscopyBulkSimpleAnnotationsStorage,                  "MAs",  4096 },
    { UID_MRImageStorage,                                          "MR",   256 * 256 * 2 },
    { UID_MRSpectroscopyStorage,                                   "MRs",  256 * 512 * 512 },
    { UID_MultichannelRespiratoryWaveformStorage,                  "WVm",  4096 },
    { UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage,     "SCb",  512 * 512 },
    { UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage,     "SCw",  512 * 512 * 2 },
    { UID_MultiframeSingleBitSecondaryCaptureImageStorage,         "SCs",  1024 * 1024 },  /* roughly an A4 300dpi scan */
    { UID_MultiframeTrueColorSecondaryCaptureImageStorage,         "SCc",  512 * 512 * 3 },
    { UID_MultipleVolumeRenderingVolumetricPresentationStateStorage, "VPm" , 4096 },
    { UID_NuclearMedicineImageStorage,                             "NM",   64 * 64 * 2 },
    { UID_OphthalmicAxialMeasurementsStorage,                      "OPx",  4096 },
    { UID_OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage, "OCb" , 768 * 576 },
    { UID_OphthalmicOpticalCoherenceTomographyEnFaceImageStorage,  "OCe",  768 * 576  * 2 },
    { UID_OphthalmicPhotography16BitImageStorage,                  "OPw",  768 * 576 * 6 },
    { UID_OphthalmicPhotography8BitImageStorage,                   "OPb",  768 * 576 * 3 },
    { UID_OphthalmicThicknessMapStorage,                           "OPm",  768 * 576 },
    { UID_OphthalmicTomographyImageStorage,                        "OPt",  768 * 576 * 3 },
    { UID_OphthalmicVisualFieldStaticPerimetryMeasurementsStorage, "OPp",  4096 },
    { UID_ParametricMapStorage,                                    "PM",   256 * 256 * 4 },
    { UID_PatientRadiationDoseSRStorage,                           "SRq",  4096 },
    { UID_PerformedImagingAgentAdministrationSRStorage,            "SRi",  4096 },
    { UID_PhotoacousticImageStorage,                               "PA",   256 * 512 * 512 },
    { UID_PlannedImagingAgentAdministrationSRStorage,              "SRj",  4096 },
    { UID_PositronEmissionTomographyImageStorage,                  "PI",   512 * 512 * 2 },
    { UID_ProcedureLogStorage,                                     "SRp",  4096 },
    { UID_ProtocolApprovalStorage,                                 "PAp",  4096 },  /* was PA */
    { UID_PseudoColorSoftcopyPresentationStateStorage,             "PSp",  4096 },
    { UID_RadiopharmaceuticalRadiationDoseSRStorage,               "SRr",  4096 },
    { UID_RawDataStorage,                                          "RAW",  256 * 512 * 512 },
    { UID_RealWorldValueMappingStorage,                            "RWM",  4096 },
    { UID_RespiratoryWaveformStorage,                              "WVr",  4096 },
    { UID_RoboticArmRadiationStorage,                              "Rra",  4096 },  /* was RRr */
    { UID_RoboticRadiationRecordStorage,                           "RRr",  4096 },
    { UID_RoutineScalpElectroencephalogramWaveformStorage,         "EEG",  4096 },
    { UID_RTBeamsDeliveryInstructionStorage,                       "RTd",  4096 },
    { UID_RTBeamsTreatmentRecordStorage,                           "RTb",  4096 },
    { UID_RTBrachyApplicationSetupDeliveryInstructionStorage,      "RTa",  4096 },
    { UID_RTBrachyTreatmentRecordStorage,                          "RTr",  4096 },
    { UID_RTDoseStorage,                                           "RD",   4096 },
    { UID_RTImageStorage,                                          "RI",   4096 },
    { UID_RTIonBeamsTreatmentRecordStorage,                        "RTi",  4096 },
    { UID_RTIonPlanStorage,                                        "RPi",  4096 },
    { UID_RTPatientPositionAcquisitionInstructionStorage,          "RPp",  4096 },
    { UID_RTPlanStorage,                                           "RP" ,  4096 },
    { UID_RTPhysicianIntentStorage,                                "RIp",  4096 },
    { UID_RTRadiationRecordSetStorage,                             "RSr",  4096 },
    { UID_RTRadiationSalvageRecordStorage,                         "RRs",  4096 },
    { UID_RTRadiationSetDeliveryInstructionStorage,                "RSd",  4096 },
    { UID_RTRadiationSetStorage,                                   "RSe",  4096 },  /* was RRs */
    { UID_RTSegmentAnnotationStorage,                              "RAs",  4096 },  /* was RRs */
    { UID_RTStructureSetStorage,                                   "RS",   4096 },
    { UID_RTTreatmentPreparationStorage,                           "RTp",  4096 },
    { UID_RTTreatmentSummaryRecordStorage,                         "RTs",  4096 },
    { UID_SecondaryCaptureImageStorage,                            "SC",   512 * 512 * 2 },
    { UID_SegmentationStorage,                                     "SG",   512 * 512 },
    { UID_SegmentedVolumeRenderingVolumetricPresentationStateStorage, "VPs", 4096 },
    { UID_SimplifiedAdultEchoSRStorage,                            "SRu",  4096 },
    { UID_SleepElectroencephalogramWaveformStorage,                "WVs",  4096 },
    { UID_SpatialFiducialsStorage,                                 "FID",  4096 },
    { UID_SpatialRegistrationStorage,                              "RGs",  4096 },
    { UID_SpectaclePrescriptionReportStorage,                      "SRs",  4096 },
    { UID_StereometricRelationshipStorage,                         "OPr",  4096 },
    { UID_SubjectiveRefractionMeasurementsStorage,                 "OPs",  4096 },
    { UID_SurfaceScanMeshStorage,                                  "SSm",  4096 },
    { UID_SurfaceScanPointCloudStorage,                            "SSp",  4096 },
    { UID_SurfaceSegmentationStorage,                              "SGs",  4096 },
    { UID_TomotherapeuticRadiationRecordStorage,                   "RRt",  4096 },
    { UID_TomotherapeuticRadiationStorage,                         "Rto",  4096 },  /* was RRt */
    { UID_TractographyResultsStorage,                              "TR",   4096 },
    { UID_TwelveLeadECGWaveformStorage,                            "TLE",  4096 },
    { UID_UltrasoundImageStorage,                                  "US",   512 * 512 },
    { UID_UltrasoundMultiframeImageStorage,                        "USm",  512 * 512 },
    { UID_VariableModalityLUTSoftcopyPresentationStateStorage,     "PSv",  4096 },
    { UID_VideoEndoscopicImageStorage,                             "VVe",  768 * 576 * 3 },
    { UID_VideoMicroscopicImageStorage,                            "VVm",  768 * 576 * 3 },
    { UID_VideoPhotographicImageStorage,                           "VVp",  768 * 576 * 3 },
    { UID_VisualAcuityMeasurementsStorage,                         "OPv",  4096 },
    { UID_VLEndoscopicImageStorage,                                "VLe",  768 * 576 * 3 },
    { UID_VLMicroscopicImageStorage,                               "VLm",  768 * 576 * 3 },
    { UID_VLPhotographicImageStorage,                              "VLp",  768 * 576 * 3 },
    { UID_VLSlideCoordinatesMicroscopicImageStorage,               "VLs",  768 * 576 * 3 },
    { UID_VLWholeSlideMicroscopyImageStorage,                      "VLw",  10000 * 10000 * 3 },
    { UID_VolumeRenderingVolumetricPresentationStateStorage,       "VPv",  4096 },
    { UID_WideFieldOphthalmicPhotographyStereographicProjectionImageStorage, "OWs", 768 * 576 * 3 },
    { UID_WideFieldOphthalmicPhotography3DCoordinatesImageStorage, "OW3",  768 * 576 * 3 },
    { UID_XADefinedProcedureProtocolStorage,                       "PPxd", 4096 },
    { UID_XAPerformedProcedureProtocolStorage,                     "PPxp", 4096 },
    { UID_XAXRFGrayscaleSoftcopyPresentationStateStorage,          "PSx",  4096 },
    { UID_XRay3DAngiographicImageStorage,                          "XA3",  256 * 512 * 512 },
    { UID_XRay3DCraniofacialImageStorage,                          "XC3",  256 * 512 * 512 },
    { UID_XRayAngiographicImageStorage,                            "XA",   256 * 512 * 512 },
    { UID_XRayRadiationDoseSRStorage,                              "SRd",  4096 },
    { UID_XRayRadiofluoroscopicImageStorage,                       "RF",   256 * 512 * 512 },
    // retired
    { UID_RETIRED_HardcopyColorImageStorage,                       "HC",   4096 },
    { UID_RETIRED_HardcopyGrayscaleImageStorage,                   "HG",   4096 },
    { UID_RETIRED_NuclearMedicineImageStorage,                     "NMr",  64 * 64 * 2 },
    { UID_RETIRED_StandaloneCurveStorage,                          "CV",   4096 },
    { UID_RETIRED_StandaloneModalityLUTStorage,                    "ML",   4096 * 2 },
    { UID_RETIRED_StandaloneOverlayStorage,                        "OV",   512 * 512 },
    { UID_RETIRED_StandalonePETCurveStorage,                       "PC",   4096 },
    { UID_RETIRED_StandaloneVOILUTStorage,                         "VO",   4096 * 2 },
    { UID_RETIRED_StoredPrintStorage,                              "SP",   4096 },
    { UID_RETIRED_UltrasoundImageStorage,                          "USr",  512 * 512 },
    { UID_RETIRED_UltrasoundMultiframeImageStorage,                "USf",  512 * 512 },
    { UID_RETIRED_VLImageStorage,                                  "VLr",  768 * 576 * 3 },
    { UID_RETIRED_VLMultiframeImageStorage,                        "VMr",  768 * 576 * 3 },
    { UID_RETIRED_XRayAngiographicBiPlaneImageStorage,             "XB",   512 * 512 * 2 },
    // draft
    { UID_DRAFT_RTBeamsDeliveryInstructionStorage,                 "RB_d", 4096 },  /* was "RBd" */
    { UID_DRAFT_SRAudioStorage,                                    "SR_a", 4096 },  /* was "SRw" */
    { UID_DRAFT_SRComprehensiveStorage,                            "SR_c", 4096 },  /* was "SRx" */
    { UID_DRAFT_SRDetailStorage,                                   "SR_d", 4096 },  /* was "SRy" */
    { UID_DRAFT_SRTextStorage,                                     "SR_t", 4096 },  /* was "SRz" */
    { UID_DRAFT_WaveformStorage,                                   "WV_d", 4096 },  /* was "WVd" */
    // DICOS
    { UID_DICOS_CTImageStorage,                                    "CTs",  512 * 512 * 2 },
    { UID_DICOS_DigitalXRayImageStorageForPresentation,            "DXs",  2048 * 2048 * 2 },
    { UID_DICOS_DigitalXRayImageStorageForProcessing,              "DPs",  2048 * 2048 * 2 },
    { UID_DICOS_ThreatDetectionReportStorage,                      "TDR",  4096 },
    { UID_DICOS_2DAITStorage,                                      "AI2",  4096 },
    { UID_DICOS_3DAITStorage,                                      "AI3",  4096 },
    { UID_DICOS_QuadrupoleResonanceStorage,                        "QR",   4096 },
    // DICONDE
    { UID_DICONDE_EddyCurrentImageStorage,                         "EC",   512 * 512 },
    { UID_DICONDE_EddyCurrentMultiframeImageStorage,               "ECm",  512 * 512 }
};

static const int numberOfDcmModalityTableEntries = OFstatic_cast(int, sizeof(modalities) / sizeof(DcmModalityTable));


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
** dcmFindNameOfUID(const char* uid, const char* defaultValue)
** Return the name of a UID.
** Performs a table lookup and returns a pointer to a read-only string.
** Returns defaultValue if the UID is not known.
*/

const char*
dcmFindNameOfUID(const char* uid, const char* defaultValue)
{
    if (uid == NULL) return defaultValue;
    for (int i = 0; i < uidNameMap_size; i++) {
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
// Returns NULL if the name is not known.
//

const char*
dcmFindUIDFromName(const char* name)
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
** dcmFindKeywordOfUID(const char* uid, const char* defaultValue)
** Return the keyword of a UID.
** Performs a table lookup and returns a pointer to a read-only string.
** Returns defaultValue if the UID is not known.
*/

const char*
dcmFindKeywordOfUID(const char* uid, const char* defaultValue)
{
    if (uid == NULL) return defaultValue;
    for (int i = 0; i < uidNameMap_size; i++) {
      if (uidNameMap[i].uid != NULL && strcmp(uid, uidNameMap[i].uid) == 0) {
        return uidNameMap[i].keyword;
      }
    }
    return defaultValue;
}

//
// dcmFindUIDFromKeyword(const char* keyword)
// Return the UID of a keyword.
// Performs a table lookup and returns a pointer to a read-only string.
// Returns NULL if the keyword is not known.
//

const char*
dcmFindUIDFromKeyword(const char* keyword)
{
    if (keyword == NULL) return NULL;
    for(int i = 0; i < uidNameMap_size; i++)
    {
      if (uidNameMap[i].keyword != NULL && strcmp(keyword, uidNameMap[i].keyword) == 0)
        return uidNameMap[i].uid;
    }
    return NULL;
}


/*
** dcmIsaStorageSOPClassUID(const char* uid)
** Returns true if the uid is one of the Storage SOP Classes.
** Performs a table lookup in the dcmAllStorageSOPClassUIDs,
** dcmNonPatientStorageSOPClassUIDs and/or dcmImageSOPClassUIDs table.
*/
OFBool
dcmIsaStorageSOPClassUID(const char* uid, const E_StorageSOPClassType type)
{
    if (uid == NULL) return OFFalse;
    /* check for patient object */
    if (type & ESSC_Patient) {
      for (int i = 0; i < numberOfDcmAllStorageSOPClassUIDs; i++) {
        if (dcmAllStorageSOPClassUIDs[i] != NULL && strcmp(uid, dcmAllStorageSOPClassUIDs[i]) == 0) {
          return OFTrue;
        }
      }
    }
    /* check for non-patient object */
    if (type & ESSC_NonPatient) {
      for (int i = 0; dcmNonPatientStorageSOPClassUIDs[i] != NULL; i++) {
        if (strcmp(uid, dcmNonPatientStorageSOPClassUIDs[i]) == 0) {
          return OFTrue;
        }
      }
    }
    /* check for image object */
    if (type & ESSC_Image) {
      for (int i = 0; i < numberOfDcmImageSOPClassUIDs; i++) {
        if (dcmImageSOPClassUIDs[i] != NULL && strcmp(uid, dcmImageSOPClassUIDs[i]) == 0) {
          return OFTrue;
        }
      }
    }
    return OFFalse;
}


/*
** dcmIsImageStorageSOPClassUID(const char* uid)
** Returns true if the uid is one of the Image Storage SOP Classes.
*/
OFBool
dcmIsImageStorageSOPClassUID(const char* uid)
{
    return dcmIsaStorageSOPClassUID(uid, ESSC_Image);
}

// ********************************

#ifndef HAVE_GETHOSTID
#if defined(HAVE_SYSINFO) && defined(HAVE_SYS_SYSTEMINFO_H)

/* Implementation of gethostid() based on sysinfo().
 * This implementation is used, for example, on Solaris.
 */
#include <sys/systeminfo.h>
static long gethostid(void)
{
    if (sysinfo(SI_HW_SERIAL, buf, 128) == -1) {
      DCMDATA_FATAL("sysinfo: " << OFStandard::getLastSystemErrorCode().message());
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

#ifdef _WIN32

/* On Windows systems specify a routine to determine the MAC address of the Ethernet adapter.
 * This implementation uses Win32 system calls available on Windows 2000 and newer.
 * A different implementation supporting Windows 95 was used in DCMTK releases up to 3.6.0,
 * but that implementation does not work reliably anymore on Windows 10.
 */
static IP_ADAPTER_INFO* getMACAddresses(OFVector<Uint8>& buffer)
{
  // get adapter info using OFVector to allocate memory
  DWORD bufferSize = 0;
  DWORD status = GetAdaptersInfo(OFnullptr, &bufferSize);
  if (status == ERROR_BUFFER_OVERFLOW)
  {
    buffer.resize(bufferSize);
    status = GetAdaptersInfo(OFreinterpret_cast(IP_ADAPTER_INFO*, &*buffer.begin()), &bufferSize);
  }
  if (status == ERROR_SUCCESS)
    return OFreinterpret_cast(IP_ADAPTER_INFO*, &*buffer.begin());
  // print out the error message for debugging
  OFString message;
  LPVOID errBuf = OFnullptr;
  if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      OFnullptr, status, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), OFreinterpret_cast(LPTSTR, &errBuf), 0, OFnullptr) > 0)
  {
    message = OFstatic_cast(const char*, errBuf);
    // remove "\r\n"
    if (message.size() >= 2 && message.substr(message.length() - 2) == "\r\n")
      message = message.substr(0, message.length() - 2);
  }
  LocalFree(errBuf);
  if (!message.empty())
  {
    DCMDATA_WARN("GetAdaptersInfo() failed with the error: '" << message << '\'');
  }
  else
  {
    DCMDATA_WARN("GetAdaptersInfo() failed with an unknown error");
  }
  // a null pointer indicates that no adapter is available
  return OFnullptr;
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
#if defined(HAVE_GETHOSTNAME) || defined(HAVE_WINDOWS_H)
    char name[1024];

    /*
    ** Define the hostid to be the system's main IP address.
    ** This is not perfect but it is better than nothing (i.e. using zero)
    */
    OFStandard::initializeNetwork();
    if (gethostname(name, 1024) == 0)
    {
        struct sockaddr_in *sin = NULL;
        struct sockaddr_in6 *sin6 = NULL;
        long *lp = NULL;
        OFSockAddr sa;
        OFStandard::getAddressByHostname(name, sa);
        switch (sa.getFamily())
        {
          case AF_INET:
            sin = sa. getSockaddr_in();
            result = OFstatic_cast(long, sin->sin_addr.s_addr);
            break;
          case AF_INET6:
            sin6 = sa. getSockaddr_in6();
            // interpret the first four bytes of the IPv6 address as a long
            lp = OFreinterpret_cast(long *, &sin6->sin6_addr);
            result = *lp;
            break;
          default:
            // unknown protocol family. Do nothing.
            break;
        }
    }

    OFStandard::shutdownNetwork();
#endif /* defined(HAVE_GETHOSTNAME) */
/* on Windows systems determine some system specific information (e.g. MAC address) */
#ifdef HAVE_WINDOWS_H
    OFCRC32 crc;
    /* get some processor specific information in addition to the MAC address */
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    /* get volume information of the system drive */
    char systemDrive[MAX_PATH];
    DWORD serialNumber = 0;
    if (GetSystemDirectoryA(systemDrive, OFstatic_cast(UINT, sizeof(systemDrive))) >= 0)
    {
        /* check for proper pathname */
        if ((strlen(systemDrive) >= 3) && (systemDrive[1] == ':') && (systemDrive[2] == '\\'))
        {
            /* truncate the pathname directly after the drive specification */
            systemDrive[3] = 0;
            if (!GetVolumeInformationA(systemDrive, NULL, 0, &serialNumber, NULL, NULL, NULL, 0))
                serialNumber = 0;
        }
    }
    /* concatenate the host specific elements and compute a 32-bit checksum */
    crc.addBlock(&result /*ip address*/, OFstatic_cast(unsigned long, sizeof(result)));
    // add the MAC addresses of all adapters
    {
      OFVector<Uint8> buffer;
      for (IP_ADAPTER_INFO* adapterInfo = getMACAddresses(buffer); adapterInfo; adapterInfo = adapterInfo->Next)
        crc.addBlock(adapterInfo->Address, adapterInfo->AddressLength);
    }
    crc.addBlock(&serialNumber, OFstatic_cast(unsigned long, sizeof(serialNumber)));
    crc.addBlock(&systemInfo.wProcessorLevel, OFstatic_cast(unsigned long, sizeof(systemInfo.wProcessorLevel)));
    crc.addBlock(&systemInfo.wProcessorRevision, OFstatic_cast(unsigned long, sizeof(systemInfo.wProcessorRevision)));
    crc.addBlock(&systemInfo.dwProcessorType, OFstatic_cast(unsigned long, sizeof(systemInfo.dwProcessorType)));
    result = OFstatic_cast(long, crc.getCRC32());
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
        counterOfCurrentUID = OFstatic_cast(Uint32, tv.tv_usec); /* This is in the range 0 - 999999 */
#endif
    /* Do not ever use "0" for the counter */
    counterOfCurrentUID++;
}


static char*
stripTrailing(char* s, char c)
{
    if (s == NULL) return s;
    for
    (
        char* it = s + strlen(s) - 1;
        it >= s && *it == c;
        *it-- = '\0'
    );
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

    uid[0] = '\0'; /* initialize */

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
