/*
 *
 *  Copyright (C) 1994-2024, OFFIS e.V.
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
 *  Author:  Andrew Hewett, Joerg Riesmeier
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

/** struct for mapping UID values to keywords, names and properties
 */
struct UIDNameMap {
    const char* uid;
    const char* keyword;
    const char* name;
    DcmUIDProperties properties;
};

/** UID mapping table.
 *  It is very important that the DCMTK-specific names of the UIDs (column #3)
 *  may not use the following characters: space ( ) [ ] , = < >
 *  For the official keywords (column #2) according to DICOM PS3.6, the DICOM
 *  standard already ensures this.
 */
static const UIDNameMap uidNameMap[] = {
    // column #1: UID                                                            #2: DICOM keyword                                                     #3: DCMTK-specific name                                               #4: properties

    // Application Context
    { UID_StandardApplicationContext,                                            "DICOMApplicationContext",                                            "StandardApplicationContext",                                         { EUS_DICOM, EUV_Standard, EUT_ApplicationContextName, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Transfer Syntax
    { UID_LittleEndianImplicitTransferSyntax,                                    "ImplicitVRLittleEndian",                                             "LittleEndianImplicit",                                               { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LittleEndianExplicitTransferSyntax,                                    "ExplicitVRLittleEndian",                                             "LittleEndianExplicit",                                               { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_BigEndianExplicitTransferSyntax,                                       "ExplicitVRBigEndian",                                                "BigEndianExplicit",                                                  { EUS_DICOM, EUV_Retired,  EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess1TransferSyntax,                                            "JPEGBaseline8Bit",                                                   "JPEGBaseline",                                                       { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess2_4TransferSyntax,                                          "JPEGExtended12Bit",                                                  "JPEGExtended:Process2+4",                                            { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess3_5TransferSyntax,                                          "JPEGExtended35",                                                     "JPEGExtended:Process3+5",                                            { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess6_8TransferSyntax,                                          "JPEGSpectralSelectionNonHierarchical68",                             "JPEGSpectralSelection:Non-hierarchical:Process6+8",                  { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess7_9TransferSyntax,                                          "JPEGSpectralSelectionNonHierarchical79",                             "JPEGSpectralSelection:Non-hierarchical:Process7+9",                  { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess10_12TransferSyntax,                                        "JPEGFullProgressionNonHierarchical1012",                             "JPEGFullProgression:Non-hierarchical:Process10+12",                  { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess11_13TransferSyntax,                                        "JPEGFullProgressionNonHierarchical1113",                             "JPEGFullProgression:Non-hierarchical:Process11+13",                  { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess14TransferSyntax,                                           "JPEGLossless",                                                       "JPEGLossless:Non-hierarchical:Process14",                            { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess15TransferSyntax,                                           "JPEGLosslessNonHierarchical15",                                      "JPEGLossless:Non-hierarchical:Process15",                            { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess16_18TransferSyntax,                                        "JPEGExtendedHierarchical1618",                                       "JPEGExtended:Hierarchical:Process16+18",                             { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess17_19TransferSyntax,                                        "JPEGExtendedHierarchical1719",                                       "JPEGExtended:Hierarchical:Process17+19",                             { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess20_22TransferSyntax,                                        "JPEGSpectralSelectionHierarchical2022",                              "JPEGSpectralSelection:Hierarchical:Process20+22",                    { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess21_23TransferSyntax,                                        "JPEGSpectralSelectionHierarchical2123",                              "JPEGSpectralSelection:Hierarchical:Process21+23",                    { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess24_26TransferSyntax,                                        "JPEGFullProgressionHierarchical2426",                                "JPEGFullProgression:Hierarchical:Process24+26",                      { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess25_27TransferSyntax,                                        "JPEGFullProgressionHierarchical2527",                                "JPEGFullProgression:Hierarchical:Process25+27",                      { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess28TransferSyntax,                                           "JPEGLosslessHierarchical28",                                         "JPEGLossless:Hierarchical:Process28",                                { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess29TransferSyntax,                                           "JPEGLosslessHierarchical29",                                         "JPEGLossless:Hierarchical:Process29",                                { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGProcess14SV1TransferSyntax,                                        "JPEGLosslessSV1",                                                    "JPEGLossless:Non-hierarchical-1stOrderPrediction",                   { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGLSLosslessTransferSyntax,                                          "JPEGLSLossless",                                                     "JPEGLSLossless",                                                     { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEGLSLossyTransferSyntax,                                             "JPEGLSNearLossless",                                                 "JPEGLSLossy",                                                        { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_RLELosslessTransferSyntax,                                             "RLELossless",                                                        "RLELossless",                                                        { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_DeflatedExplicitVRLittleEndianTransferSyntax,                          "DeflatedExplicitVRLittleEndian",                                     "DeflatedLittleEndianExplicit",                                       { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEG2000LosslessOnlyTransferSyntax,                                    "JPEG2000Lossless",                                                   "JPEG2000LosslessOnly",                                               { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEG2000TransferSyntax,                                                "JPEG2000",                                                           "JPEG2000",                                                           { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnlyTransferSyntax, "JPEG2000MCLossless",                                                 "JPEG2000MulticomponentLosslessOnly",                                 { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPEG2000Part2MulticomponentImageCompressionTransferSyntax,             "JPEG2000MC",                                                         "JPEG2000Multicomponent",                                             { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPIPReferencedTransferSyntax,                                          "JPIPReferenced",                                                     "JPIPReferenced",                                                     { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_JPIPReferencedDeflateTransferSyntax,                                   "JPIPReferencedDeflate",                                              "JPIPReferencedDeflate",                                              { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_MPEG2MainProfileAtMainLevelTransferSyntax,                             "MPEG2MPML",                                                          "MPEG2MainProfile@MainLevel",                                         { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_FragmentableMPEG2MainProfileMainLevelTransferSyntax,                   "MPEG2MPMLF",                                                         "FragmentableMPEG2MainProfile/MainLevel",                             { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_MPEG2MainProfileAtHighLevelTransferSyntax,                             "MPEG2MPHL",                                                          "MPEG2MainProfile@HighLevel",                                         { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_FragmentableMPEG2MainProfileHighLevelTransferSyntax,                   "MPEG2MPHLF",                                                         "FragmentableMPEG2MainProfile/HighLevel",                             { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_MPEG4HighProfileLevel4_1TransferSyntax,                                "MPEG4HP41",                                                          "MPEG4HighProfile/Level4.1",                                          { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_FragmentableMPEG4HighProfileLevel4_1TransferSyntax,                    "MPEG4HP41F",                                                         "FragmentableMPEG4HighProfile/Level4.1",                              { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_MPEG4BDcompatibleHighProfileLevel4_1TransferSyntax,                    "MPEG4HP41BD",                                                        "MPEG4BDcompatibleHighProfile/Level4.1",                              { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_FragmentableMPEG4BDcompatibleHighProfileLevel4_1TransferSyntax,        "MPEG4HP41BDF",                                                       "FragmentableMPEG4BDcompatibleHighProfile/Level4.1",                  { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_MPEG4HighProfileLevel4_2_For2DVideoTransferSyntax,                     "MPEG4HP422D",                                                        "MPEG4HighProfile/Level4.2For2DVideo",                                { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_FragmentableMPEG4HighProfileLevel4_2_For2DVideoTransferSyntax,         "MPEG4HP422DF",                                                       "FragmentableMPEG4HighProfile/Level4.2For2DVideo",                    { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_MPEG4HighProfileLevel4_2_For3DVideoTransferSyntax,                     "MPEG4HP423D",                                                        "MPEG4HighProfile/Level4.2For3DVideo",                                { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_FragmentableMPEG4HighProfileLevel4_2_For3DVideoTransferSyntax,         "MPEG4HP423DF",                                                       "FragmentableMPEG4HighProfile/Level4.2For3DVideo",                    { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_MPEG4StereoHighProfileLevel4_2TransferSyntax,                          "MPEG4HP42STEREO",                                                    "MPEG4StereoHighProfile/Level4.2",                                    { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_FragmentableMPEG4StereoHighProfileLevel4_2TransferSyntax,              "MPEG4HP42STEREOF",                                                   "FragmentableMPEG4StereoHighProfile/Level4.2",                        { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_HEVCMainProfileLevel5_1TransferSyntax,                                 "HEVCMP51",                                                           "HEVCMainProfile/Level5.1",                                           { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_HEVCMain10ProfileLevel5_1TransferSyntax,                               "HEVCM10P51",                                                         "HEVCMain10Profile/Level5.1",                                         { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SMPTEST2110_20_UncompressedProgressiveActiveVideoTransferSyntax,       "SMPTEST211020UncompressedProgressiveActiveVideo",                    "SMPTEST2110-20:UncompressedProgressiveActiveVideo",                  { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SMPTEST2110_20_UncompressedInterlacedActiveVideoTransferSyntax,        "SMPTEST211020UncompressedInterlacedActiveVideo",                     "SMPTEST2110-20:UncompressedInterlacedActiveVideo",                   { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SMPTEST2110_30_PCMDigitalAudioTransferSyntax,                          "SMPTEST211030PCMDigitalAudio",                                       "SMPTEST2110-30:PCMDigitalAudio",                                     { EUS_DICOM, EUV_Standard, EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_RFC2557MIMEEncapsulationTransferSyntax,                        "RFC2557MIMEEncapsulation",                                           "RETIRED_RFC2557MIMEEncapsulation",                                   { EUS_DICOM, EUV_Retired,  EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_XMLEncodingTransferSyntax,                                     "XMLEncoding",                                                        "RETIRED_XMLEncoding",                                                { EUS_DICOM, EUV_Retired,  EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_PrivateGE_LEI_WithBigEndianPixelDataTransferSyntax,                    NULL /* no official keyword */,                                       "PrivateGELittleEndianImplicitWithBigEndianPixelData",                { EUS_DICOM, EUV_Private,  EUT_TransferSyntax, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Storage (DICOM)
    { UID_AcquisitionContextSRStorage,                                           "AcquisitionContextSRStorage",                                        "AcquisitionContextSRStorage",                                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_AdvancedBlendingPresentationStateStorage,                              "AdvancedBlendingPresentationStateStorage",                           "AdvancedBlendingPresentationStateStorage",                           { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_PresentationState, UID_PROP_NONE } },
    { UID_AmbulatoryECGWaveformStorage,                                          "AmbulatoryECGWaveformStorage",                                       "AmbulatoryECGWaveformStorage",                                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_ArterialPulseWaveformStorage,                                          "ArterialPulseWaveformStorage",                                       "ArterialPulseWaveformStorage",                                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_AutorefractionMeasurementsStorage,                                     "AutorefractionMeasurementsStorage",                                  "AutorefractionMeasurementsStorage",                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_BasicStructuredDisplayStorage,                                         "BasicStructuredDisplayStorage",                                      "BasicStructuredDisplayStorage",                                      { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_BasicTextSRStorage,                                                    "BasicTextSRStorage",                                                 "BasicTextSRStorage",                                                 { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_BasicVoiceAudioWaveformStorage,                                        "BasicVoiceAudioWaveformStorage",                                     "BasicVoiceAudioWaveformStorage",                                     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_BlendingSoftcopyPresentationStateStorage,                              "BlendingSoftcopyPresentationStateStorage",                           "BlendingSoftcopyPresentationStateStorage",                           { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_PresentationState, UID_PROP_NONE } },
    { UID_BodyPositionWaveformStorage,                                           "BodyPositionWaveformStorage",                                        "BodyPositionWaveformStorage",                                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_BreastProjectionXRayImageStorageForPresentation,                       "BreastProjectionXRayImageStorageForPresentation",                    "BreastProjectionXRayImageStorageForPresentation",                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_BreastProjectionXRayImageStorageForProcessing,                         "BreastProjectionXRayImageStorageForProcessing",                      "BreastProjectionXRayImageStorageForProcessing",                      { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_BreastTomosynthesisImageStorage,                                       "BreastTomosynthesisImageStorage",                                    "BreastTomosynthesisImageStorage",                                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_CardiacElectrophysiologyWaveformStorage,                               "CardiacElectrophysiologyWaveformStorage",                            "CardiacElectrophysiologyWaveformStorage",                            { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_CArmPhotonElectronRadiationRecordStorage,                              "CArmPhotonElectronRadiationRecordStorage",                           "CArmPhotonElectronRadiationRecordStorage",                           { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_CArmPhotonElectronRadiationStorage,                                    "CArmPhotonElectronRadiationStorage",                                 "CArmPhotonElectronRadiationStorage",                                 { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_ChestCADSRStorage,                                                     "ChestCADSRStorage",                                                  "ChestCADSRStorage",                                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_ColonCADSRStorage,                                                     "ColonCADSRStorage",                                                  "ColonCADSRStorage",                                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_ColorSoftcopyPresentationStateStorage,                                 "ColorSoftcopyPresentationStateStorage",                              "ColorSoftcopyPresentationStateStorage",                              { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_PresentationState, UID_PROP_NONE } },
    { UID_CompositingPlanarMPRVolumetricPresentationStateStorage,                "CompositingPlanarMPRVolumetricPresentationStateStorage",             "CompositingPlanarMPRVolumetricPresentationStateStorage",             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_PresentationState, UID_PROP_NONE } },
    { UID_Comprehensive3DSRStorage,                                              "Comprehensive3DSRStorage",                                           "Comprehensive3DSRStorage",                                           { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_ComprehensiveSRStorage,                                                "ComprehensiveSRStorage",                                             "ComprehensiveSRStorage",                                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_ComputedRadiographyImageStorage,                                       "ComputedRadiographyImageStorage",                                    "ComputedRadiographyImageStorage",                                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_ContentAssessmentResultsStorage,                                       "ContentAssessmentResultsStorage",                                    "ContentAssessmentResultsStorage",                                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_CornealTopographyMapStorage,                                           "CornealTopographyMapStorage",                                        "CornealTopographyMapStorage",                                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_CTDefinedProcedureProtocolStorage,                                     "CTDefinedProcedureProtocolStorage",                                  "CTDefinedProcedureProtocolStorage",                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NON_PATIENT | UID_PROP_NO_DIR_RECORD } },
    { UID_CTImageStorage,                                                        "CTImageStorage",                                                     "CTImageStorage",                                                     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_CTPerformedProcedureProtocolStorage,                                   "CTPerformedProcedureProtocolStorage",                                "CTPerformedProcedureProtocolStorage",                                { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_DeformableSpatialRegistrationStorage,                                  "DeformableSpatialRegistrationStorage",                               "DeformableSpatialRegistrationStorage",                               { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_DermoscopicPhotographyImageStorage,                                    "DermoscopicPhotographyImageStorage",                                 "DermoscopicPhotographyImageStorage",                                 { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_DigitalIntraOralXRayImageStorageForPresentation,                       "DigitalIntraOralXRayImageStorageForPresentation",                    "DigitalIntraOralXRayImageStorageForPresentation",                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_DigitalIntraOralXRayImageStorageForProcessing,                         "DigitalIntraOralXRayImageStorageForProcessing",                      "DigitalIntraOralXRayImageStorageForProcessing",                      { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_DigitalMammographyXRayImageStorageForPresentation,                     "DigitalMammographyXRayImageStorageForPresentation",                  "DigitalMammographyXRayImageStorageForPresentation",                  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_DigitalMammographyXRayImageStorageForProcessing,                       "DigitalMammographyXRayImageStorageForProcessing",                    "DigitalMammographyXRayImageStorageForProcessing",                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_DigitalXRayImageStorageForPresentation,                                "DigitalXRayImageStorageForPresentation",                             "DigitalXRayImageStorageForPresentation",                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_DigitalXRayImageStorageForProcessing,                                  "DigitalXRayImageStorageForProcessing",                               "DigitalXRayImageStorageForProcessing",                               { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_ElectromyogramWaveformStorage,                                         "ElectromyogramWaveformStorage",                                      "ElectromyogramWaveformStorage",                                      { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_ElectrooculogramWaveformStorage,                                       "ElectrooculogramWaveformStorage",                                    "ElectrooculogramWaveformStorage",                                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_EncapsulatedCDAStorage,                                                "EncapsulatedCDAStorage",                                             "EncapsulatedCDAStorage",                                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Encapsulated,      UID_PROP_NONE } },
    { UID_EncapsulatedMTLStorage,                                                "EncapsulatedMTLStorage",                                             "EncapsulatedMTLStorage",                                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Encapsulated,      UID_PROP_NONE } },
    { UID_EncapsulatedOBJStorage,                                                "EncapsulatedOBJStorage",                                             "EncapsulatedOBJStorage",                                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Encapsulated,      UID_PROP_NONE } },
    { UID_EncapsulatedPDFStorage,                                                "EncapsulatedPDFStorage",                                             "EncapsulatedPDFStorage",                                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Encapsulated,      UID_PROP_NONE } },
    { UID_EncapsulatedSTLStorage,                                                "EncapsulatedSTLStorage",                                             "EncapsulatedSTLStorage",                                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Encapsulated,      UID_PROP_NONE } },
    { UID_EnhancedContinuousRTImageStorage,                                      "EnhancedContinuousRTImageStorage",                                   "EnhancedContinuousRTImageStorage",                                   { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_EnhancedCTImageStorage,                                                "EnhancedCTImageStorage",                                             "EnhancedCTImageStorage",                                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_EnhancedMRColorImageStorage,                                           "EnhancedMRColorImageStorage",                                        "EnhancedMRColorImageStorage",                                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_EnhancedMRImageStorage,                                                "EnhancedMRImageStorage",                                             "EnhancedMRImageStorage",                                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_EnhancedPETImageStorage,                                               "EnhancedPETImageStorage",                                            "EnhancedPETImageStorage",                                            { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_EnhancedRTImageStorage,                                                "EnhancedRTImageStorage",                                             "EnhancedRTImageStorage",                                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_EnhancedSRStorage,                                                     "EnhancedSRStorage",                                                  "EnhancedSRStorage",                                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_EnhancedUSVolumeStorage,                                               "EnhancedUSVolumeStorage",                                            "EnhancedUSVolumeStorage",                                            { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_EnhancedXAImageStorage,                                                "EnhancedXAImageStorage",                                             "EnhancedXAImageStorage",                                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_EnhancedXRayRadiationDoseSRStorage,                                    "EnhancedXRayRadiationDoseSRStorage",                                 "EnhancedXRayRadiationDoseSRStorage",                                 { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_EnhancedXRFImageStorage,                                               "EnhancedXRFImageStorage",                                            "EnhancedXRFImageStorage",                                            { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_ExtensibleSRStorage,                                                   "ExtensibleSRStorage",                                                "ExtensibleSRStorage",                                                { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_General32BitECGWaveformStorage,                                        "General32bitECGWaveformStorage",                                     "General32BitECGWaveformStorage",                                     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_GeneralAudioWaveformStorage,                                           "GeneralAudioWaveformStorage",                                        "GeneralAudioWaveformStorage",                                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_GeneralECGWaveformStorage,                                             "GeneralECGWaveformStorage",                                          "GeneralECGWaveformStorage",                                          { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_GrayscalePlanarMPRVolumetricPresentationStateStorage,                  "GrayscalePlanarMPRVolumetricPresentationStateStorage",               "GrayscalePlanarMPRVolumetricPresentationStateStorage",               { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_PresentationState, UID_PROP_NONE } },
    { UID_GrayscaleSoftcopyPresentationStateStorage,                             "GrayscaleSoftcopyPresentationStateStorage",                          "GrayscaleSoftcopyPresentationStateStorage",                          { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_PresentationState, UID_PROP_NONE } },
    { UID_HemodynamicWaveformStorage,                                            "HemodynamicWaveformStorage",                                         "HemodynamicWaveformStorage",                                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_ImplantationPlanSRStorage,                                             "ImplantationPlanSRStorage",                                          "ImplantationPlanSRStorage",                                          { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    // the following line is needed for reasons of backward compatibility (name of SOP Class changed, see the previous line)
    { UID_ImplantationPlanSRStorage,                                             NULL /* see above */,                                                 "ImplantationPlanSRDocumentStorage",                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_IntraocularLensCalculationsStorage,                                    "IntraocularLensCalculationsStorage",                                 "IntraocularLensCalculationsStorage",                                 { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_IntravascularOpticalCoherenceTomographyImageStorageForPresentation,    "IntravascularOpticalCoherenceTomographyImageStorageForPresentation", "IntravascularOpticalCoherenceTomographyImageStorageForPresentation", { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_IntravascularOpticalCoherenceTomographyImageStorageForProcessing,      "IntravascularOpticalCoherenceTomographyImageStorageForProcessing",   "IntravascularOpticalCoherenceTomographyImageStorageForProcessing",   { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_KeratometryMeasurementsStorage,                                        "KeratometryMeasurementsStorage",                                     "KeratometryMeasurementsStorage",                                     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_KeyObjectSelectionDocumentStorage,                                     "KeyObjectSelectionDocumentStorage",                                  "KeyObjectSelectionDocumentStorage",                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_LegacyConvertedEnhancedCTImageStorage,                                 "LegacyConvertedEnhancedCTImageStorage",                              "LegacyConvertedEnhancedCTImageStorage",                              { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_LegacyConvertedEnhancedMRImageStorage,                                 "LegacyConvertedEnhancedMRImageStorage",                              "LegacyConvertedEnhancedMRImageStorage",                              { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_LegacyConvertedEnhancedPETImageStorage,                                "LegacyConvertedEnhancedPETImageStorage",                             "LegacyConvertedEnhancedPETImageStorage",                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_LensometryMeasurementsStorage,                                         "LensometryMeasurementsStorage",                                      "LensometryMeasurementsStorage",                                      { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_MacularGridThicknessAndVolumeReportStorage,                            "MacularGridThicknessAndVolumeReportStorage",                         "MacularGridThicknessAndVolumeReportStorage",                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_MammographyCADSRStorage,                                               "MammographyCADSRStorage",                                            "MammographyCADSRStorage",                                            { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_MediaStorageDirectoryStorage,                                          "MediaStorageDirectoryStorage",                                       "MediaStorageDirectoryStorage",                                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_MicroscopyBulkSimpleAnnotationsStorage,                                "MicroscopyBulkSimpleAnnotationsStorage",                             "MicroscopyBulkSimpleAnnotationsStorage",                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_MRImageStorage,                                                        "MRImageStorage",                                                     "MRImageStorage",                                                     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_MRSpectroscopyStorage,                                                 "MRSpectroscopyStorage",                                              "MRSpectroscopyStorage",                                              { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_MultichannelRespiratoryWaveformStorage,                                "MultichannelRespiratoryWaveformStorage",                             "MultichannelRespiratoryWaveformStorage",                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage,                   "MultiFrameGrayscaleByteSecondaryCaptureImageStorage",                "MultiframeGrayscaleByteSecondaryCaptureImageStorage",                { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage,                   "MultiFrameGrayscaleWordSecondaryCaptureImageStorage",                "MultiframeGrayscaleWordSecondaryCaptureImageStorage",                { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_MultiframeSingleBitSecondaryCaptureImageStorage,                       "MultiFrameSingleBitSecondaryCaptureImageStorage",                    "MultiframeSingleBitSecondaryCaptureImageStorage",                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_MultiframeTrueColorSecondaryCaptureImageStorage,                       "MultiFrameTrueColorSecondaryCaptureImageStorage",                    "MultiframeTrueColorSecondaryCaptureImageStorage",                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_MultipleVolumeRenderingVolumetricPresentationStateStorage,             "MultipleVolumeRenderingVolumetricPresentationStateStorage",          "MultipleVolumeRenderingVolumetricPresentationStateStorage",          { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_PresentationState, UID_PROP_NONE } },
    { UID_NuclearMedicineImageStorage,                                           "NuclearMedicineImageStorage",                                        "NuclearMedicineImageStorage",                                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_OphthalmicAxialMeasurementsStorage,                                    "OphthalmicAxialMeasurementsStorage",                                 "OphthalmicAxialMeasurementsStorage",                                 { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage,        "OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage",     "OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage",     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_OphthalmicOpticalCoherenceTomographyEnFaceImageStorage,                "OphthalmicOpticalCoherenceTomographyEnFaceImageStorage",             "OphthalmicOpticalCoherenceTomographyEnFaceImageStorage",             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_OphthalmicPhotography16BitImageStorage,                                "OphthalmicPhotography16BitImageStorage",                             "OphthalmicPhotography16BitImageStorage",                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_OphthalmicPhotography8BitImageStorage,                                 "OphthalmicPhotography8BitImageStorage",                              "OphthalmicPhotography8BitImageStorage",                              { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_OphthalmicThicknessMapStorage,                                         "OphthalmicThicknessMapStorage",                                      "OphthalmicThicknessMapStorage",                                      { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_OphthalmicTomographyImageStorage,                                      "OphthalmicTomographyImageStorage",                                   "OphthalmicTomographyImageStorage",                                   { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_OphthalmicVisualFieldStaticPerimetryMeasurementsStorage,               "OphthalmicVisualFieldStaticPerimetryMeasurementsStorage",            "OphthalmicVisualFieldStaticPerimetryMeasurementsStorage",            { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_ParametricMapStorage,                                                  "ParametricMapStorage",                                               "ParametricMapStorage",                                               { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_ENHANCED_MF } },
    { UID_PatientRadiationDoseSRStorage,                                         "PatientRadiationDoseSRStorage",                                      "PatientRadiationDoseSRStorage",                                      { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_PerformedImagingAgentAdministrationSRStorage,                          "PerformedImagingAgentAdministrationSRStorage",                       "PerformedImagingAgentAdministrationSRStorage",                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_PhotoacousticImageStorage,                                             "PhotoacousticImageStorage",                                          "PhotoacousticImageStorage",                                          { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_PlannedImagingAgentAdministrationSRStorage,                            "PlannedImagingAgentAdministrationSRStorage",                         "PlannedImagingAgentAdministrationSRStorage",                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_PositronEmissionTomographyImageStorage,                                "PositronEmissionTomographyImageStorage",                             "PositronEmissionTomographyImageStorage",                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_ProcedureLogStorage,                                                   "ProcedureLogStorage",                                                "ProcedureLogStorage",                                                { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_ProtocolApprovalStorage,                                               "ProtocolApprovalStorage",                                            "ProtocolApprovalStorage",                                            { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NON_PATIENT | UID_PROP_NO_DIR_RECORD } },
    { UID_PseudoColorSoftcopyPresentationStateStorage,                           "PseudoColorSoftcopyPresentationStateStorage",                        "PseudoColorSoftcopyPresentationStateStorage",                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_PresentationState, UID_PROP_NONE } },
    { UID_RadiopharmaceuticalRadiationDoseSRStorage,                             "RadiopharmaceuticalRadiationDoseSRStorage",                          "RadiopharmaceuticalRadiationDoseSRStorage",                          { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_RawDataStorage,                                                        "RawDataStorage",                                                     "RawDataStorage",                                                     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RealWorldValueMappingStorage,                                          "RealWorldValueMappingStorage",                                       "RealWorldValueMappingStorage",                                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RespiratoryWaveformStorage,                                            "RespiratoryWaveformStorage",                                         "RespiratoryWaveformStorage",                                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_RoboticArmRadiationStorage,                                            "RoboticArmRadiationStorage",                                         "RoboticArmRadiationStorage",                                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RoboticRadiationRecordStorage,                                         "RoboticRadiationRecordStorage",                                      "RoboticRadiationRecordStorage",                                      { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RoutineScalpElectroencephalogramWaveformStorage,                       "RoutineScalpElectroencephalogramWaveformStorage",                    "RoutineScalpElectroencephalogramWaveformStorage",                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_RTBeamsDeliveryInstructionStorage,                                     "RTBeamsDeliveryInstructionStorage",                                  "RTBeamsDeliveryInstructionStorage",                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTBeamsTreatmentRecordStorage,                                         "RTBeamsTreatmentRecordStorage",                                      "RTBeamsTreatmentRecordStorage",                                      { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTBrachyApplicationSetupDeliveryInstructionStorage,                    "RTBrachyApplicationSetupDeliveryInstructionStorage",                 "RTBrachyApplicationSetupDeliveryInstructionStorage",                 { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTBrachyTreatmentRecordStorage,                                        "RTBrachyTreatmentRecordStorage",                                     "RTBrachyTreatmentRecordStorage",                                     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTDoseStorage,                                                         "RTDoseStorage",                                                      "RTDoseStorage",                                                      { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTImageStorage,                                                        "RTImageStorage",                                                     "RTImageStorage",                                                     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_RTIonBeamsTreatmentRecordStorage,                                      "RTIonBeamsTreatmentRecordStorage",                                   "RTIonBeamsTreatmentRecordStorage",                                   { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTIonPlanStorage,                                                      "RTIonPlanStorage",                                                   "RTIonPlanStorage",                                                   { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTPatientPositionAcquisitionInstructionStorage,                        "RTPatientPositionAcquisitionInstructionStorage",                     "RTPatientPositionAcquisitionInstructionStorage",                     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTPhysicianIntentStorage,                                              "RTPhysicianIntentStorage",                                           "RTPhysicianIntentStorage",                                           { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTPlanStorage,                                                         "RTPlanStorage",                                                      "RTPlanStorage",                                                      { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTRadiationRecordSetStorage,                                           "RTRadiationRecordSetStorage",                                        "RTRadiationRecordSetStorage",                                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTRadiationSalvageRecordStorage,                                       "RTRadiationSalvageRecordStorage",                                    "RTRadiationSalvageRecordStorage",                                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTRadiationSetDeliveryInstructionStorage,                              "RTRadiationSetDeliveryInstructionStorage",                           "RTRadiationSetDeliveryInstructionStorage",                           { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTRadiationSetStorage,                                                 "RTRadiationSetStorage",                                              "RTRadiationSetStorage",                                              { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTSegmentAnnotationStorage,                                            "RTSegmentAnnotationStorage",                                         "RTSegmentAnnotationStorage",                                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTStructureSetStorage,                                                 "RTStructureSetStorage",                                              "RTStructureSetStorage",                                              { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTTreatmentPreparationStorage,                                         "RTTreatmentPreparationStorage",                                      "RTTreatmentPreparationStorage",                                      { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_RTTreatmentSummaryRecordStorage,                                       "RTTreatmentSummaryRecordStorage",                                    "RTTreatmentSummaryRecordStorage",                                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_SecondaryCaptureImageStorage,                                          "SecondaryCaptureImageStorage",                                       "SecondaryCaptureImageStorage",                                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_SegmentationStorage,                                                   "SegmentationStorage",                                                "SegmentationStorage",                                                { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_SegmentedVolumeRenderingVolumetricPresentationStateStorage,            "SegmentedVolumeRenderingVolumetricPresentationStateStorage",         "SegmentedVolumeRenderingVolumetricPresentationStateStorage",         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_PresentationState, UID_PROP_NONE } },
    { UID_SimplifiedAdultEchoSRStorage,                                          "SimplifiedAdultEchoSRStorage",                                       "SimplifiedAdultEchoSRStorage",                                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_SleepElectroencephalogramWaveformStorage,                              "SleepElectroencephalogramWaveformStorage",                           "SleepElectroencephalogramWaveformStorage",                           { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_SpatialFiducialsStorage,                                               "SpatialFiducialsStorage",                                            "SpatialFiducialsStorage",                                            { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_SpatialRegistrationStorage,                                            "SpatialRegistrationStorage",                                         "SpatialRegistrationStorage",                                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_SpectaclePrescriptionReportStorage,                                    "SpectaclePrescriptionReportStorage",                                 "SpectaclePrescriptionReportStorage",                                 { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_StereometricRelationshipStorage,                                       "StereometricRelationshipStorage",                                    "StereometricRelationshipStorage",                                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_SubjectiveRefractionMeasurementsStorage,                               "SubjectiveRefractionMeasurementsStorage",                            "SubjectiveRefractionMeasurementsStorage",                            { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_SurfaceScanMeshStorage,                                                "SurfaceScanMeshStorage",                                             "SurfaceScanMeshStorage",                                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_SurfaceScanPointCloudStorage,                                          "SurfaceScanPointCloudStorage",                                       "SurfaceScanPointCloudStorage",                                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_SurfaceSegmentationStorage,                                            "SurfaceSegmentationStorage",                                         "SurfaceSegmentationStorage",                                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_TomotherapeuticRadiationRecordStorage,                                 "TomotherapeuticRadiationRecordStorage",                              "TomotherapeuticRadiationRecordStorage",                              { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_TomotherapeuticRadiationStorage,                                       "TomotherapeuticRadiationStorage",                                    "TomotherapeuticRadiationStorage",                                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_TractographyResultsStorage,                                            "TractographyResultsStorage",                                         "TractographyResultsStorage",                                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_TwelveLeadECGWaveformStorage,                                          "TwelveLeadECGWaveformStorage",                                       "TwelveLeadECGWaveformStorage",                                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Waveform,          UID_PROP_NONE } },
    { UID_UltrasoundImageStorage,                                                "UltrasoundImageStorage",                                             "UltrasoundImageStorage",                                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_UltrasoundMultiframeImageStorage,                                      "UltrasoundMultiFrameImageStorage",                                   "UltrasoundMultiframeImageStorage",                                   { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_VariableModalityLUTSoftcopyPresentationStateStorage,                   "VariableModalityLUTSoftcopyPresentationStateStorage",                "VariableModalityLUTSoftcopyPresentationStateStorage",                { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_PresentationState, UID_PROP_NONE } },
    { UID_VideoEndoscopicImageStorage,                                           "VideoEndoscopicImageStorage",                                        "VideoEndoscopicImageStorage",                                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_VideoMicroscopicImageStorage,                                          "VideoMicroscopicImageStorage",                                       "VideoMicroscopicImageStorage",                                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_VideoPhotographicImageStorage,                                         "VideoPhotographicImageStorage",                                      "VideoPhotographicImageStorage",                                      { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_VisualAcuityMeasurementsStorage,                                       "VisualAcuityMeasurementsStorage",                                    "VisualAcuityMeasurementsStorage",                                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_VLEndoscopicImageStorage,                                              "VLEndoscopicImageStorage",                                           "VLEndoscopicImageStorage",                                           { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_VLMicroscopicImageStorage,                                             "VLMicroscopicImageStorage",                                          "VLMicroscopicImageStorage",                                          { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_VLPhotographicImageStorage,                                            "VLPhotographicImageStorage",                                         "VLPhotographicImageStorage",                                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_VLSlideCoordinatesMicroscopicImageStorage,                             "VLSlideCoordinatesMicroscopicImageStorage",                          "VLSlideCoordinatesMicroscopicImageStorage",                          { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_VLWholeSlideMicroscopyImageStorage,                                    "VLWholeSlideMicroscopyImageStorage",                                 "VLWholeSlideMicroscopyImageStorage",                                 { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_VolumeRenderingVolumetricPresentationStateStorage,                     "VolumeRenderingVolumetricPresentationStateStorage",                  "VolumeRenderingVolumetricPresentationStateStorage",                  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_PresentationState, UID_PROP_NONE } },
    { UID_WideFieldOphthalmicPhotographyStereographicProjectionImageStorage,     "WideFieldOphthalmicPhotographyStereographicProjectionImageStorage",  "WideFieldOphthalmicPhotographyStereographicProjectionImageStorage",  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_WideFieldOphthalmicPhotography3DCoordinatesImageStorage,               "WideFieldOphthalmicPhotography3DCoordinatesImageStorage",            "WideFieldOphthalmicPhotography3DCoordinatesImageStorage",            { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_XADefinedProcedureProtocolStorage,                                     "XADefinedProcedureProtocolStorage",                                  "XADefinedProcedureProtocolStorage",                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NON_PATIENT | UID_PROP_NO_DIR_RECORD } },
    { UID_XAPerformedProcedureProtocolStorage,                                   "XAPerformedProcedureProtocolStorage",                                "XAPerformedProcedureProtocolStorage",                                { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other,             UID_PROP_NONE } },
    { UID_XAXRFGrayscaleSoftcopyPresentationStateStorage,                        "XAXRFGrayscaleSoftcopyPresentationStateStorage",                     "XAXRFGrayscaleSoftcopyPresentationStateStorage",                     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_PresentationState, UID_PROP_NONE } },
    { UID_XRay3DAngiographicImageStorage,                                        "XRay3DAngiographicImageStorage",                                     "XRay3DAngiographicImageStorage",                                     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_XRay3DCraniofacialImageStorage,                                        "XRay3DCraniofacialImageStorage",                                     "XRay3DCraniofacialImageStorage",                                     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_ENHANCED_MF } },
    { UID_XRayAngiographicImageStorage,                                          "XRayAngiographicImageStorage",                                       "XRayAngiographicImageStorage",                                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    { UID_XRayRadiationDoseSRStorage,                                            "XRayRadiationDoseSRStorage",                                         "XRayRadiationDoseSRStorage",                                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_StructuredReport,  UID_PROP_NONE } },
    { UID_XRayRadiofluoroscopicImageStorage,                                     "XRayRadiofluoroscopicImageStorage",                                  "XRayRadiofluoroscopicImageStorage",                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image,             UID_PROP_NONE } },
    // Storage (retired)
    { UID_RETIRED_HardcopyColorImageStorage,                                     "HardcopyColorImageStorage",                                          "RETIRED_HardcopyColorImageStorage",                                  { EUS_DICOM, EUV_Retired, EUT_SOPClass, EUST_Storage, EUIT_Image, UID_PROP_NONE } },
    { UID_RETIRED_HardcopyGrayscaleImageStorage,                                 "HardcopyGrayscaleImageStorage",                                      "RETIRED_HardcopyGrayscaleImageStorage",                              { EUS_DICOM, EUV_Retired, EUT_SOPClass, EUST_Storage, EUIT_Image, UID_PROP_NONE } },
    { UID_RETIRED_NuclearMedicineImageStorage,                                   "NuclearMedicineImageStorageRetired",                                 "RETIRED_NuclearMedicineImageStorage",                                { EUS_DICOM, EUV_Retired, EUT_SOPClass, EUST_Storage, EUIT_Image, UID_PROP_NONE } },
    { UID_RETIRED_StandaloneCurveStorage,                                        "StandaloneCurveStorage",                                             "RETIRED_StandaloneCurveStorage",                                     { EUS_DICOM, EUV_Retired, EUT_SOPClass, EUST_Storage, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_StandaloneModalityLUTStorage,                                  "StandaloneModalityLUTStorage",                                       "RETIRED_StandaloneModalityLUTStorage",                               { EUS_DICOM, EUV_Retired, EUT_SOPClass, EUST_Storage, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_StandaloneOverlayStorage,                                      "StandaloneOverlayStorage",                                           "RETIRED_StandaloneOverlayStorage",                                   { EUS_DICOM, EUV_Retired, EUT_SOPClass, EUST_Storage, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_StandalonePETCurveStorage,                                     "StandalonePETCurveStorage",                                          "RETIRED_StandalonePETCurveStorage",                                  { EUS_DICOM, EUV_Retired, EUT_SOPClass, EUST_Storage, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_StandaloneVOILUTStorage,                                       "StandaloneVOILUTStorage",                                            "RETIRED_StandaloneVOILUTStorage",                                    { EUS_DICOM, EUV_Retired, EUT_SOPClass, EUST_Storage, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_StoredPrintStorage,                                            "StoredPrintStorage",                                                 "RETIRED_StoredPrintStorage",                                         { EUS_DICOM, EUV_Retired, EUT_SOPClass, EUST_Storage, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_UltrasoundImageStorage,                                        "UltrasoundImageStorageRetired",                                      "RETIRED_UltrasoundImageStorage",                                     { EUS_DICOM, EUV_Retired, EUT_SOPClass, EUST_Storage, EUIT_Image, UID_PROP_NONE } },
    { UID_RETIRED_UltrasoundMultiframeImageStorage,                              "UltrasoundMultiFrameImageStorageRetired",                            "RETIRED_UltrasoundMultiframeImageStorage",                           { EUS_DICOM, EUV_Retired, EUT_SOPClass, EUST_Storage, EUIT_Image, UID_PROP_NONE } },
    { UID_RETIRED_VLImageStorage,                                                "VLImageStorageTrial",                                                "RETIRED_VLImageStorage",                                             { EUS_DICOM, EUV_Retired, EUT_SOPClass, EUST_Storage, EUIT_Image, UID_PROP_NONE } },
    { UID_RETIRED_VLMultiframeImageStorage,                                      "VLMultiFrameImageStorageTrial",                                      "RETIRED_VLMultiframeImageStorage",                                   { EUS_DICOM, EUV_Retired, EUT_SOPClass, EUST_Storage, EUIT_Image, UID_PROP_NONE } },
    { UID_RETIRED_XRayAngiographicBiPlaneImageStorage,                           "XRayAngiographicBiPlaneImageStorage",                                "RETIRED_XRayAngiographicBiPlaneImageStorage",                        { EUS_DICOM, EUV_Retired, EUT_SOPClass, EUST_Storage, EUIT_Image, UID_PROP_NONE } },
    // Storage (DICOS)
    { UID_DICOS_CTImageStorage,                                                  "DICOSCTImageStorage",                                                "DICOS_CTImageStorage",                                               { EUS_DICOS, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image, UID_PROP_NONE } },
    { UID_DICOS_DigitalXRayImageStorageForPresentation,                          "DICOSDigitalXRayImageStorageForPresentation",                        "DICOS_DigitalXRayImageStorageForPresentation",                       { EUS_DICOS, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image, UID_PROP_NONE } },
    { UID_DICOS_DigitalXRayImageStorageForProcessing,                            "DICOSDigitalXRayImageStorageForProcessing",                          "DICOS_DigitalXRayImageStorageForProcessing",                         { EUS_DICOS, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image, UID_PROP_NONE } },
    { UID_DICOS_ThreatDetectionReportStorage,                                    "DICOSThreatDetectionReportStorage",                                  "DICOS_ThreatDetectionReportStorage",                                 { EUS_DICOS, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other, UID_PROP_NO_DIR_RECORD } },
    { UID_DICOS_2DAITStorage,                                                    "DICOS2DAITStorage",                                                  "DICOS_2DAITStorage",                                                 { EUS_DICOS, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other, UID_PROP_NO_DIR_RECORD } },
    { UID_DICOS_3DAITStorage,                                                    "DICOS3DAITStorage",                                                  "DICOS_3DAITStorage",                                                 { EUS_DICOS, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other, UID_PROP_NO_DIR_RECORD } },
    { UID_DICOS_QuadrupoleResonanceStorage,                                      "DICOSQuadrupoleResonanceStorage",                                    "DICOS_QuadrupoleResonanceStorage",                                   { EUS_DICOS, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_other, UID_PROP_NO_DIR_RECORD } },
    // Storage (DICONDE)
    { UID_DICONDE_EddyCurrentImageStorage,                                       "EddyCurrentImageStorage",                                            "DICONDE_EddyCurrentImageStorage",                                    { EUS_DICONDE, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image, UID_PROP_NONE } },
    { UID_DICONDE_EddyCurrentMultiframeImageStorage,                             "EddyCurrentMultiFrameImageStorage",                                  "DICONDE_EddyCurrentMultiframeImageStorage",                          { EUS_DICONDE, EUV_Standard, EUT_SOPClass, EUST_Storage, EUIT_Image, UID_PROP_NONE } },

    // Query/Retrieve
    { UID_FINDPatientRootQueryRetrieveInformationModel,                          "PatientRootQueryRetrieveInformationModelFind",                       "FINDPatientRootQueryRetrieveInformationModel",                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_FINDStudyRootQueryRetrieveInformationModel,                            "StudyRootQueryRetrieveInformationModelFind",                         "FINDStudyRootQueryRetrieveInformationModel",                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_GETPatientRootQueryRetrieveInformationModel,                           "PatientRootQueryRetrieveInformationModelGet",                        "GETPatientRootQueryRetrieveInformationModel",                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_GETStudyRootQueryRetrieveInformationModel,                             "StudyRootQueryRetrieveInformationModelGet",                          "GETStudyRootQueryRetrieveInformationModel",                          { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_MOVEPatientRootQueryRetrieveInformationModel,                          "PatientRootQueryRetrieveInformationModelMove",                       "MOVEPatientRootQueryRetrieveInformationModel",                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_MOVEStudyRootQueryRetrieveInformationModel,                            "StudyRootQueryRetrieveInformationModelMove",                         "MOVEStudyRootQueryRetrieveInformationModel",                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel,             "PatientStudyOnlyQueryRetrieveInformationModelFind",                  "RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel",          { EUS_DICOM, EUV_Retired,  EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_GETCompositeInstanceRetrieveWithoutBulkData,                           "CompositeInstanceRetrieveWithoutBulkDataGet",                        "GETCompositeInstanceRetrieveWithoutBulkData",                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_GETCompositeInstanceRootRetrieve,                                      "CompositeInstanceRootRetrieveGet",                                   "GETCompositeInstanceRootRetrieve",                                   { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_GETPatientStudyOnlyQueryRetrieveInformationModel,              "PatientStudyOnlyQueryRetrieveInformationModelGet",                   "RETIRED_GETPatientStudyOnlyQueryRetrieveInformationModel",           { EUS_DICOM, EUV_Retired,  EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_MOVECompositeInstanceRootRetrieve,                                     "CompositeInstanceRootRetrieveMove",                                  "MOVECompositeInstanceRootRetrieve",                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel,             "PatientStudyOnlyQueryRetrieveInformationModelMove",                  "RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel",          { EUS_DICOM, EUV_Retired,  EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },

    // Modality Worklist
    { UID_FINDModalityWorklistInformationModel,                                  "ModalityWorklistInformationModelFind",                               "FINDModalityWorklistInformationModel",                               { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Worklist, EUIT_other, UID_PROP_NONE } },

    // General Purpose Worklist (retired)
    { UID_RETIRED_FINDGeneralPurposeWorklistInformationModel,                    "GeneralPurposeWorklistInformationModelFind",                         "RETIRED_FINDGeneralPurposeWorklistInformationModel",                 { EUS_DICOM, EUV_Retired, EUT_SOPClass,     EUST_Worklist, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_GeneralPurposePerformedProcedureStepSOPClass,                  "GeneralPurposePerformedProcedureStep",                               "RETIRED_GeneralPurposePerformedProcedureStepSOPClass",               { EUS_DICOM, EUV_Retired, EUT_SOPClass,     EUST_Worklist, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_GeneralPurposeScheduledProcedureStepSOPClass,                  "GeneralPurposeScheduledProcedureStep",                               "RETIRED_GeneralPurposeScheduledProcedureStepSOPClass",               { EUS_DICOM, EUV_Retired, EUT_SOPClass,     EUST_Worklist, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_GeneralPurposeWorklistManagementMetaSOPClass,                  "GeneralPurposeWorklistManagementMeta",                               "RETIRED_GeneralPurposeWorklistManagementMetaSOPClass",               { EUS_DICOM, EUV_Retired, EUT_MetaSOPClass, EUST_Worklist, EUIT_other, UID_PROP_NONE } },

    // MPPS
    { UID_ModalityPerformedProcedureStepNotificationSOPClass,                    "ModalityPerformedProcedureStepNotification",                         "ModalityPerformedProcedureStepNotificationSOPClass",                 { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_ModalityPerformedProcedureStepRetrieveSOPClass,                        "ModalityPerformedProcedureStepRetrieve",                             "ModalityPerformedProcedureStepRetrieveSOPClass",                     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_ModalityPerformedProcedureStepSOPClass,                                "ModalityPerformedProcedureStep",                                     "ModalityPerformedProcedureStepSOPClass",                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Radiotherapy
    { UID_RTConventionalMachineVerification,                                     "RTConventionalMachineVerification",                                  "RTConventionalMachineVerification",                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_RTIonMachineVerification,                                              "RTIonMachineVerification",                                           "RTIonMachineVerification",                                           { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Unified Worklist and Procedure Step
    { UID_UnifiedWorklistAndProcedureStepServiceClass,                           "UnifiedWorklistAndProcedureStep",                                    "UnifiedWorklistAndProcedureStepServiceClass",                        { EUS_DICOM, EUV_Standard, EUT_ServiceClass, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_UnifiedProcedureStepPushSOPClass,                                      "UnifiedProcedureStepPush",                                           "UnifiedProcedureStepPushSOPClass",                                   { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_UnifiedProcedureStepWatchSOPClass,                                     "UnifiedProcedureStepWatch",                                          "UnifiedProcedureStepWatchSOPClass",                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_UnifiedProcedureStepPullSOPClass,                                      "UnifiedProcedureStepPull",                                           "UnifiedProcedureStepPullSOPClass",                                   { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_UnifiedProcedureStepEventSOPClass,                                     "UnifiedProcedureStepEvent",                                          "UnifiedProcedureStepEventSOPClass",                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_UnifiedProcedureStepQuerySOPClass,                                     "UnifiedProcedureStepQuery",                                          "UnifiedProcedureStepQuerySOPClass",                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_UPSGlobalSubscriptionSOPInstance,                                      "UPSGlobalSubscriptionInstance",                                      "UPSGlobalSubscriptionSOPInstance",                                   { EUS_DICOM, EUV_Standard, EUT_SOPInstance,  EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_UPSFilteredGlobalSubscriptionSOPInstance,                              "UPSFilteredGlobalSubscriptionInstance",                              "UPSFilteredGlobalSubscriptionSOPInstance",                           { EUS_DICOM, EUV_Standard, EUT_SOPInstance,  EUST_other, EUIT_other, UID_PROP_NONE } },

    // Storage Commitment
    { UID_RETIRED_StorageCommitmentPullModelSOPClass,                            "StorageCommitmentPullModel",                                         "RETIRED_StorageCommitmentPullModelSOPClass",                         { EUS_DICOM, EUV_Retired,  EUT_SOPClass,    EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_StorageCommitmentPullModelSOPInstance,                         "StorageCommitmentPullModelInstance",                                 "RETIRED_StorageCommitmentPullModelSOPInstance",                      { EUS_DICOM, EUV_Retired,  EUT_SOPInstance, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_StorageCommitmentPushModelSOPClass,                                    "StorageCommitmentPushModel",                                         "StorageCommitmentPushModelSOPClass",                                 { EUS_DICOM, EUV_Standard, EUT_SOPClass,    EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_StorageCommitmentPushModelSOPInstance,                                 "StorageCommitmentPushModelInstance",                                 "StorageCommitmentPushModelSOPInstance",                              { EUS_DICOM, EUV_Standard, EUT_SOPInstance, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Hanging Protocol Storage and Query/Retrieve
    { UID_HangingProtocolStorage,                                                "HangingProtocolStorage",                                             "HangingProtocolStorage",                                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage,       EUIT_other, UID_PROP_NON_PATIENT } },
    { UID_FINDHangingProtocolInformationModel,                                   "HangingProtocolInformationModelFind",                                "FINDHangingProtocolInformationModel",                                { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_MOVEHangingProtocolInformationModel,                                   "HangingProtocolInformationModelMove",                                "MOVEHangingProtocolInformationModel",                                { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_GETHangingProtocolInformationModel,                                    "HangingProtocolInformationModelGet",                                 "GETHangingProtocolInformationModel",                                 { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },

    // Relevant Patient Information Query
    { UID_BreastImagingRelevantPatientInformationQuery,                          "BreastImagingRelevantPatientInformationQuery",                       "BreastImagingRelevantPatientInformationQuery",                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_CardiacRelevantPatientInformationQuery,                                "CardiacRelevantPatientInformationQuery",                             "CardiacRelevantPatientInformationQuery",                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_GeneralRelevantPatientInformationQuery,                                "GeneralRelevantPatientInformationQuery",                             "GeneralRelevantPatientInformationQuery",                             { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },

    // Color Palette Storage and Query/Retrieve
    { UID_ColorPaletteStorage,                                                   "ColorPaletteStorage",                                                "ColorPaletteStorage",                                                { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage,       EUIT_other, UID_PROP_NON_PATIENT } },
    { UID_FINDColorPaletteInformationModel,                                      "ColorPaletteQueryRetrieveInformationModelFind",                      "FINDColorPaletteInformationModel",                                   { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_MOVEColorPaletteInformationModel,                                      "ColorPaletteQueryRetrieveInformationModelMove",                      "MOVEColorPaletteInformationModel",                                   { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_GETColorPaletteInformationModel,                                       "ColorPaletteQueryRetrieveInformationModelGet",                       "GETColorPaletteInformationModel",                                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },

    // Implant Template Storage and Query/Retrieve
    { UID_GenericImplantTemplateStorage,                                         "GenericImplantTemplateStorage",                                      "GenericImplantTemplateStorage",                                      { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage,       EUIT_other, UID_PROP_NON_PATIENT } },
    { UID_FINDGenericImplantTemplateInformationModel,                            "GenericImplantTemplateInformationModelFind",                         "FINDGenericImplantTemplateInformationModel",                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_MOVEGenericImplantTemplateInformationModel,                            "GenericImplantTemplateInformationModelMove",                         "MOVEGenericImplantTemplateInformationModel",                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_GETGenericImplantTemplateInformationModel,                             "GenericImplantTemplateInformationModelGet",                          "GETGenericImplantTemplateInformationModel",                          { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_ImplantAssemblyTemplateStorage,                                        "ImplantAssemblyTemplateStorage",                                     "ImplantAssemblyTemplateStorage",                                     { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage,       EUIT_other, UID_PROP_NON_PATIENT } },
    { UID_FINDImplantAssemblyTemplateInformationModel,                           "ImplantAssemblyTemplateInformationModelFind",                        "FINDImplantAssemblyTemplateInformationModel",                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_MOVEImplantAssemblyTemplateInformationModel,                           "ImplantAssemblyTemplateInformationModelMove",                        "MOVEImplantAssemblyTemplateInformationModel",                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_GETImplantAssemblyTemplateInformationModel,                            "ImplantAssemblyTemplateInformationModelGet",                         "GETImplantAssemblyTemplateInformationModel",                         { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_ImplantTemplateGroupStorage,                                           "ImplantTemplateGroupStorage",                                        "ImplantTemplateGroupStorage",                                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_Storage,       EUIT_other, UID_PROP_NON_PATIENT } },
    { UID_FINDImplantTemplateGroupInformationModel,                              "ImplantTemplateGroupInformationModelFind",                           "FINDImplantTemplateGroupInformationModel",                           { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_MOVEImplantTemplateGroupInformationModel,                              "ImplantTemplateGroupInformationModelMove",                           "MOVEImplantTemplateGroupInformationModel",                           { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_GETImplantTemplateGroupInformationModel,                               "ImplantTemplateGroupInformationModelGet",                            "GETImplantTemplateGroupInformationModel",                            { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },

    // Defined Procedure Protocol Query/Retrieve
    { UID_FINDDefinedProcedureProtocolInformationModel,                          "DefinedProcedureProtocolInformationModelFind",                       "FINDDefinedProcedureProtocolInformationModel",                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_MOVEDefinedProcedureProtocolInformationModel,                          "DefinedProcedureProtocolInformationModelMove",                       "MOVEDefinedProcedureProtocolInformationModel",                       { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_GETDefinedProcedureProtocolInformationModel,                           "DefinedProcedureProtocolInformationModelGet",                        "GETDefinedProcedureProtocolInformationModel",                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },

    // Protocol Approval Query/Retrieve
    { UID_FINDProtocolApprovalInformationModel,                                  "ProtocolApprovalInformationModelFind",                               "FINDProtocolApprovalInformationModel",                               { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_MOVEProtocolApprovalInformationModel,                                  "ProtocolApprovalInformationModelMove",                               "MOVEProtocolApprovalInformationModel",                               { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_GETProtocolApprovalInformationModel,                                   "ProtocolApprovalInformationModelGet",                                "GETProtocolApprovalInformationModel",                                { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },

    // Inventory Storage, Query/Retrieve, and related Services
    { UID_InventoryStorage,                                                      "InventoryStorage",                                                   "InventoryStorage",                                                   { EUS_DICOM, EUV_Standard, EUT_SOPClass,    EUST_Storage,       EUIT_other, UID_PROP_NON_PATIENT } },
    { UID_FINDInventory,                                                         "InventoryFind",                                                      "FINDInventory",                                                      { EUS_DICOM, EUV_Standard, EUT_SOPClass,    EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_MOVEInventory,                                                         "InventoryMove",                                                      "MOVEInventory",                                                      { EUS_DICOM, EUV_Standard, EUT_SOPClass,    EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_GETInventory,                                                          "InventoryGet",                                                       "GETInventory",                                                       { EUS_DICOM, EUV_Standard, EUT_SOPClass,    EUST_QueryRetrieve, EUIT_other, UID_PROP_NONE } },
    { UID_InventoryCreation,                                                     "InventoryCreation",                                                  "InventoryCreation",                                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass,    EUST_other,         EUIT_other, UID_PROP_NONE } },
    { UID_RepositoryQuery,                                                       "RepositoryQuery",                                                    "RepositoryQuery",                                                    { EUS_DICOM, EUV_Standard, EUT_SOPClass,    EUST_other,         EUIT_other, UID_PROP_NONE } },
    { UID_StorageManagementSOPInstance,                                          "StorageManagementInstance",                                          "StorageManagementSOPInstance",                                       { EUS_DICOM, EUV_Standard, EUT_SOPInstance, EUST_other,         EUIT_other, UID_PROP_NONE } },

    // Print
    { UID_BasicAnnotationBoxSOPClass,                                            "BasicAnnotationBox",                                                 "BasicAnnotationBoxSOPClass",                                         { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_BasicColorImageBoxSOPClass,                                            "BasicColorImageBox",                                                 "BasicColorImageBoxSOPClass",                                         { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_BasicColorPrintManagementMetaSOPClass,                                 "BasicColorPrintManagementMeta",                                      "BasicColorPrintManagementMetaSOPClass",                              { EUS_DICOM, EUV_Standard, EUT_MetaSOPClass, EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_BasicFilmBoxSOPClass,                                                  "BasicFilmBox",                                                       "BasicFilmBoxSOPClass",                                               { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_BasicFilmSessionSOPClass,                                              "BasicFilmSession",                                                   "BasicFilmSessionSOPClass",                                           { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_BasicGrayscaleImageBoxSOPClass,                                        "BasicGrayscaleImageBox",                                             "BasicGrayscaleImageBoxSOPClass",                                     { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_BasicGrayscalePrintManagementMetaSOPClass,                             "BasicGrayscalePrintManagementMeta",                                  "BasicGrayscalePrintManagementMetaSOPClass",                          { EUS_DICOM, EUV_Standard, EUT_MetaSOPClass, EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_PresentationLUTSOPClass,                                               "PresentationLUT",                                                    "PresentationLUTSOPClass",                                            { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_PrintJobSOPClass,                                                      "PrintJob",                                                           "PrintJobSOPClass",                                                   { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_PrinterConfigurationRetrievalSOPClass,                                 "PrinterConfigurationRetrieval",                                      "PrinterConfigurationRetrievalSOPClass",                              { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_PrinterConfigurationRetrievalSOPInstance,                              "PrinterConfigurationRetrievalInstance",                              "PrinterConfigurationRetrievalSOPInstance",                           { EUS_DICOM, EUV_Standard, EUT_SOPInstance,  EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_PrinterSOPClass,                                                       "Printer",                                                            "PrinterSOPClass",                                                    { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_PrinterSOPInstance,                                                    "PrinterInstance",                                                    "PrinterSOPInstance",                                                 { EUS_DICOM, EUV_Standard, EUT_SOPInstance,  EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_BasicPrintImageOverlayBoxSOPClass,                             "BasicPrintImageOverlayBox",                                          "RETIRED_BasicPrintImageOverlayBoxSOPClass",                          { EUS_DICOM, EUV_Retired,  EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_ImageOverlayBoxSOPClass,                                       "ImageOverlayBox",                                                    "RETIRED_ImageOverlayBoxSOPClass",                                    { EUS_DICOM, EUV_Retired,  EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_PrintQueueManagementSOPClass,                                  "PrintQueueManagement",                                               "RETIRED_PrintQueueManagementSOPClass",                               { EUS_DICOM, EUV_Retired,  EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_PrintQueueSOPInstance,                                         "PrintQueue",                                                         "RETIRED_PrintQueueSOPInstance",                                      { EUS_DICOM, EUV_Retired,  EUT_SOPInstance,  EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_PullPrintRequestSOPClass,                                      "PullPrintRequest",                                                   "RETIRED_PullPrintRequestSOPClass",                                   { EUS_DICOM, EUV_Retired,  EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_PullStoredPrintManagementMetaSOPClass,                         "PullStoredPrintManagementMeta",                                      "RETIRED_PullStoredPrintManagementMetaSOPClass",                      { EUS_DICOM, EUV_Retired,  EUT_MetaSOPClass, EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_ReferencedColorPrintManagementMetaSOPClass,                    "ReferencedColorPrintManagementMeta",                                 "RETIRED_ReferencedColorPrintManagementMetaSOPClass",                 { EUS_DICOM, EUV_Retired,  EUT_MetaSOPClass, EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_ReferencedGrayscalePrintManagementMetaSOPClass,                "ReferencedGrayscalePrintManagementMeta",                             "RETIRED_ReferencedGrayscalePrintManagementMetaSOPClass",             { EUS_DICOM, EUV_Retired,  EUT_MetaSOPClass, EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_ReferencedImageBoxSOPClass,                                    "ReferencedImageBox",                                                 "RETIRED_ReferencedImageBoxSOPClass",                                 { EUS_DICOM, EUV_Retired,  EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },
    { UID_VOILUTBoxSOPClass,                                                     "VOILUTBox",                                                          "VOILUTBoxSOPClass",                                                  { EUS_DICOM, EUV_Standard, EUT_SOPClass,     EUST_PrintManagement, EUIT_other, UID_PROP_NONE } },

    // Detached Management (retired)
    { UID_RETIRED_DetachedInterpretationManagementSOPClass,                      "DetachedInterpretationManagement",                                   "RETIRED_DetachedInterpretationManagementSOPClass",                   { EUS_DICOM, EUV_Retired, EUT_SOPClass,     EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_DetachedPatientManagementMetaSOPClass,                         "DetachedPatientManagementMeta",                                      "RETIRED_DetachedPatientManagementMetaSOPClass",                      { EUS_DICOM, EUV_Retired, EUT_MetaSOPClass, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_DetachedPatientManagementSOPClass,                             "DetachedPatientManagement",                                          "RETIRED_DetachedPatientManagementSOPClass",                          { EUS_DICOM, EUV_Retired, EUT_SOPClass,     EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_DetachedResultsManagementMetaSOPClass,                         "DetachedResultsManagementMeta",                                      "RETIRED_DetachedResultsManagementMetaSOPClass",                      { EUS_DICOM, EUV_Retired, EUT_MetaSOPClass, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_DetachedResultsManagementSOPClass,                             "DetachedResultsManagement",                                          "RETIRED_DetachedResultsManagementSOPClass",                          { EUS_DICOM, EUV_Retired, EUT_SOPClass,     EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_DetachedStudyManagementMetaSOPClass,                           "DetachedStudyManagementMeta",                                        "RETIRED_DetachedStudyManagementMetaSOPClass",                        { EUS_DICOM, EUV_Retired, EUT_MetaSOPClass, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_DetachedStudyManagementSOPClass,                               "DetachedStudyManagement",                                            "RETIRED_DetachedStudyManagementSOPClass",                            { EUS_DICOM, EUV_Retired, EUT_SOPClass,     EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_DetachedVisitManagementSOPClass,                               "DetachedVisitManagement",                                            "RETIRED_DetachedVisitManagementSOPClass",                            { EUS_DICOM, EUV_Retired, EUT_SOPClass,     EUST_other, EUIT_other, UID_PROP_NONE } },

    // Procedure Log
    { UID_ProceduralEventLoggingSOPClass,                                        "ProceduralEventLogging",                                             "ProceduralEventLoggingSOPClass",                                     { EUS_DICOM, EUV_Standard, EUT_SOPClass,    EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_ProceduralEventLoggingSOPInstance,                                     "ProceduralEventLoggingInstance",                                     "ProceduralEventLoggingSOPInstance",                                  { EUS_DICOM, EUV_Standard, EUT_SOPInstance, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Substance Administration
    { UID_SubstanceAdministrationLoggingSOPClass,                                "SubstanceAdministrationLogging",                                     "SubstanceAdministrationLoggingSOPClass",                             { EUS_DICOM, EUV_Standard, EUT_SOPClass,    EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SubstanceAdministrationLoggingSOPInstance,                             "SubstanceAdministrationLoggingInstance",                             "SubstanceAdministrationLoggingSOPInstance",                          { EUS_DICOM, EUV_Standard, EUT_SOPInstance, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_ProductCharacteristicsQuerySOPClass,                                   "ProductCharacteristicsQuery",                                        "ProductCharacteristicsQuerySOPClass",                                { EUS_DICOM, EUV_Standard, EUT_SOPClass,    EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SubstanceApprovalQuerySOPClass,                                        "SubstanceApprovalQuery",                                             "SubstanceApprovalQuerySOPClass",                                     { EUS_DICOM, EUV_Standard, EUT_SOPClass,    EUST_other, EUIT_other, UID_PROP_NONE } },

    // Media Creation
    { UID_MediaCreationManagementSOPClass,                                       "MediaCreationManagement",                                            "MediaCreationManagementSOPClass",                                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_other, EUIT_other, UID_PROP_NONE } },

    // SOP Class Relationship Negotiation
    { UID_StorageServiceClass,                                                   "Storage",                                                            "StorageServiceClass",                                                { EUS_DICOM, EUV_Standard, EUT_ServiceClass, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Instance Availability Notification
    { UID_InstanceAvailabilityNotificationSOPClass,                              "InstanceAvailabilityNotification",                                   "InstanceAvailabilityNotificationSOPClass",                           { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Application Hosting
    { UID_NativeDICOMModel,                                                      "NativeDICOMModel",                                                   "NativeDICOMModel",                                                   { EUS_DICOM, EUV_Standard, EUT_ApplicationHosting, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_AbstractMultiDimensionalImageModel,                                    "AbstractMultiDimensionalImageModel",                                 "AbstractMultiDimensionalImageModel",                                 { EUS_DICOM, EUV_Standard, EUT_ApplicationHosting, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Communication of Display Parameters
    { UID_DisplaySystemSOPClass,                                                 "DisplaySystem",                                                      "DisplaySystemSOPClass",                                              { EUS_DICOM, EUV_Standard, EUT_SOPClass,    EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_DisplaySystemSOPInstance,                                              "DisplaySystemInstance",                                              "DisplaySystemSOPInstance",                                           { EUS_DICOM, EUV_Standard, EUT_SOPInstance, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Real-Time Video
    { UID_VideoEndoscopicImageRealTimeCommunication,                             "VideoEndoscopicImageRealTimeCommunication",                          "VideoEndoscopicImageRealTimeCommunication",                          { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_other, EUIT_other,            UID_PROP_NONE } },
    { UID_VideoPhotographicImageRealTimeCommunication,                           "VideoPhotographicImageRealTimeCommunication",                        "VideoPhotographicImageRealTimeCommunication",                        { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_other, EUIT_other,            UID_PROP_NONE } },
    { UID_AudioWaveformRealTimeCommunication,                                    "AudioWaveformRealTimeCommunication",                                 "AudioWaveformRealTimeCommunication",                                 { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_other, EUIT_other,            UID_PROP_NONE } },
    { UID_RenditionSelectionDocumentRealTimeCommunication,                       "RenditionSelectionDocumentRealTimeCommunication",                    "RenditionSelectionDocumentRealTimeCommunication",                    { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_other, EUIT_StructuredReport, UID_PROP_NONE } },

    // Other
    { UID_RETIRED_BasicStudyContentNotificationSOPClass,                         "BasicStudyContentNotification",                                      "RETIRED_BasicStudyContentNotificationSOPClass",                      { EUS_DICOM, EUV_Retired,  EUT_SOPClass, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_RETIRED_StudyComponentManagementSOPClass,                              "StudyComponentManagement",                                           "RETIRED_StudyComponentManagementSOPClass",                           { EUS_DICOM, EUV_Retired,  EUT_SOPClass, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_VerificationSOPClass,                                                  "Verification",                                                       "VerificationSOPClass",                                               { EUS_DICOM, EUV_Standard, EUT_SOPClass, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Mapping Resources
    { UID_DICOMContentMappingResource,                                           "DICOMContentMappingResource",                                        "DICOMContentMappingResource",                                        { EUS_DICOM, EUV_Standard, EUT_MappingResource, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Coding Schemes
    { UID_DICOMControlledTerminologyCodingScheme,                                "DCM",                                                                "DICOMControlledTerminologyCodingScheme",                             { EUS_DICOM, EUV_Standard, EUT_CodingScheme, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_DICOMUIDRegistryCodingScheme,                                          "DCMUID",                                                             "DICOMUIDRegistryCodingScheme",                                       { EUS_DICOM, EUV_Standard, EUT_CodingScheme, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Configuration Management LDAP UIDs
    { UID_LDAP_dicomAETitle,                                                     "dicomAETitle",                                                       "LDAP_dicomAETitle",                                                  { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomApplicationCluster,                                          "dicomApplicationCluster",                                            "LDAP_dicomApplicationCluster",                                       { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomAssociationAcceptor,                                         "dicomAssociationAcceptor",                                           "LDAP_dicomAssociationAcceptor",                                      { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomAssociationInitiator,                                        "dicomAssociationInitiator",                                          "LDAP_dicomAssociationInitiator",                                     { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomAuthorizedNodeCertificateReference,                          "dicomAuthorizedNodeCertificateReference",                            "LDAP_dicomAuthorizedNodeCertificateReference",                       { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomConfigurationRoot,                                           "dicomConfigurationRoot",                                             "LDAP_dicomConfigurationRoot",                                        { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomDescription,                                                 "dicomDescription",                                                   "LDAP_dicomDescription",                                              { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomDevice,                                                      "dicomDevice",                                                        "LDAP_dicomDevice",                                                   { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomDeviceName,                                                  "dicomDeviceName",                                                    "LDAP_dicomDeviceName",                                               { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomDeviceSerialNumber,                                          "dicomDeviceSerialNumber",                                            "LDAP_dicomDeviceSerialNumber",                                       { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomDevicesRoot,                                                 "dicomDevicesRoot",                                                   "LDAP_dicomDevicesRoot",                                              { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomHostname,                                                    "dicomHostname",                                                      "LDAP_dicomHostname",                                                 { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomInstalled,                                                   "dicomInstalled",                                                     "LDAP_dicomInstalled",                                                { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomInstitutionAddress,                                          "dicomInstitutionAddress",                                            "LDAP_dicomInstitutionAddress",                                       { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomInstitutionDepartmentName,                                   "dicomInstitutionDepartmentName",                                     "LDAP_dicomInstitutionDepartmentName",                                { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomInstitutionName,                                             "dicomInstitutionName",                                               "LDAP_dicomInstitutionName",                                          { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomIssuerOfPatientID,                                           "dicomIssuerOfPatientID",                                             "LDAP_dicomIssuerOfPatientID",                                        { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomManufacturer,                                                "dicomManufacturer",                                                  "LDAP_dicomManufacturer",                                             { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomManufacturerModelName,                                       "dicomManufacturerModelName",                                         "LDAP_dicomManufacturerModelName",                                    { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomNetworkAE,                                                   "dicomNetworkAE",                                                     "LDAP_dicomNetworkAE",                                                { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomNetworkConnection,                                           "dicomNetworkConnection",                                             "LDAP_dicomNetworkConnection",                                        { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomNetworkConnectionReference,                                  "dicomNetworkConnectionReference",                                    "LDAP_dicomNetworkConnectionReference",                               { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomPort,                                                        "dicomPort",                                                          "LDAP_dicomPort",                                                     { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomPreferredCalledAETitle,                                      "dicomPreferredCalledAETitle",                                        "LDAP_dicomPreferredCalledAETitle",                                   { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomPreferredCallingAETitle,                                     "dicomPreferredCallingAETitle",                                       "LDAP_dicomPreferredCallingAETitle",                                  { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomPrimaryDeviceType,                                           "dicomPrimaryDeviceType",                                             "LDAP_dicomPrimaryDeviceType",                                        { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomRelatedDeviceReference,                                      "dicomRelatedDeviceReference",                                        "LDAP_dicomRelatedDeviceReference",                                   { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomSOPClass,                                                    "dicomSOPClass",                                                      "LDAP_dicomSOPClass",                                                 { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomSoftwareVersion,                                             "dicomSoftwareVersion",                                               "LDAP_dicomSoftwareVersion",                                          { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomStationName,                                                 "dicomStationName",                                                   "LDAP_dicomStationName",                                              { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomSupportedCharacterSet,                                       "dicomSupportedCharacterSet",                                         "LDAP_dicomSupportedCharacterSet",                                    { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomTLSCyphersuite,                                              "dicomTLSCyphersuite",                                                "LDAP_dicomTLSCyphersuite",                                           { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomThisNodeCertificateReference,                                "dicomThisNodeCertificateReference",                                  "LDAP_dicomThisNodeCertificateReference",                             { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomTransferCapability,                                          "dicomTransferCapability",                                            "LDAP_dicomTransferCapability",                                       { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomTransferRole,                                                "dicomTransferRole",                                                  "LDAP_dicomTransferRole",                                             { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomTransferSyntax,                                              "dicomTransferSyntax",                                                "LDAP_dicomTransferSyntax",                                           { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomUniqueAETitle,                                               "dicomUniqueAETitle",                                                 "LDAP_dicomUniqueAETitle",                                            { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomUniqueAETitlesRegistryRoot,                                  "dicomUniqueAETitlesRegistryRoot",                                    "LDAP_dicomUniqueAETitlesRegistryRoot",                               { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LDAP_dicomVendorData,                                                  "dicomVendorData",                                                    "LDAP_dicomVendorData",                                               { EUS_DICOM, EUV_Standard, EUT_LDAP, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Well-known Frame of References
    { UID_ICBM452T1FrameOfReference,                                             "ICBM452T1",                                                          "ICBM452T1FrameOfReference",                                          { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_ICBMSingleSubjectMRIFrameOfReference,                                  "ICBMSingleSubjectMRI",                                               "ICBMSingleSubjectMRIFrameOfReference",                               { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_IEC61217FixedCoordinateSystemFrameOfReference,                         "IEC61217FixedCoordinateSystem",                                      "IEC61217FixedCoordinateSystemFrameOfReference",                      { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_IEC61217TableTopCoordinateSystemFrameOfReference,                      "IEC61217TableTopCoordinateSystem",                                   "IEC61217TableTopCoordinateSystemFrameOfReference",                   { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2AVG152PDFrameOfReference,                                          "SPM2AVG152PD",                                                       "SPM2AVG152PDFrameOfReference",                                       { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2AVG152T1FrameOfReference,                                          "SPM2AVG152T1",                                                       "SPM2AVG152T1FrameOfReference",                                       { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2AVG152T2FrameOfReference,                                          "SPM2AVG152T2",                                                       "SPM2AVG152T2FrameOfReference",                                       { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2AVG305T1FrameOfReference,                                          "SPM2AVG305T1",                                                       "SPM2AVG305T1FrameOfReference",                                       { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2BRAINMASKFrameOfReference,                                         "SPM2BRAINMASK",                                                      "SPM2BRAINMASKFrameOfReference",                                      { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2CSFFrameOfReference,                                               "SPM2CSF",                                                            "SPM2CSFFrameOfReference",                                            { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2EPIFrameOfReference,                                               "SPM2EPI",                                                            "SPM2EPIFrameOfReference",                                            { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2FILT1FrameOfReference,                                             "SPM2FILT1",                                                          "SPM2FILT1FrameOfReference",                                          { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2GRAYFrameOfReference,                                              "SPM2GRAY",                                                           "SPM2GRAYFrameOfReference",                                           { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2PDFrameOfReference,                                                "SPM2PD",                                                             "SPM2PDFrameOfReference",                                             { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2PETFrameOfReference,                                               "SPM2PET",                                                            "SPM2PETFrameOfReference",                                            { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2SINGLESUBJT1FrameOfReference,                                      "SINGLESUBJT1",                                                       "SPM2SINGLESUBJT1FrameOfReference",                                   { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2SPECTFrameOfReference,                                             "SPM2SPECT",                                                          "SPM2SPECTFrameOfReference",                                          { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2T1FrameOfReference,                                                "SPM2T1",                                                             "SPM2T1FrameOfReference",                                             { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2T2FrameOfReference,                                                "SPM2T2",                                                             "SPM2T2FrameOfReference",                                             { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2TRANSMFrameOfReference,                                            "SPM2TRANSM",                                                         "SPM2TRANSMFrameOfReference",                                         { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SPM2WHITEFrameOfReference,                                             "SPM2WHITE",                                                          "SPM2WHITEFrameOfReference",                                          { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_StandardRoboticArmCoordinateSystemFrameOfReference,                    "StandardRoboticArmCoordinateSystem",                                 "StandardRoboticArmCoordinateSystemFrameOfReference",                 { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_TalairachBrainAtlasFrameOfReference,                                   "TalairachBrainAtlas",                                                "TalairachBrainAtlasFrameOfReference",                                { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_SRI24FrameOfReference,                                                 "SRI24",                                                              "SRI24FrameOfReference",                                              { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_Colin27FrameOfReference,                                               "Colin27",                                                            "Colin27FrameOfReference",                                            { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LPBA40AIRFrameOfReference,                                             "LPBA40AIR",                                                          "LPBA40AIRFrameOfReference",                                          { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LPBA40FLIRTFrameOfReference,                                           "LPBA40FLIRT",                                                        "LPBA40FLIRTFrameOfReference",                                        { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },
    { UID_LPBA40SPM5FrameOfReference,                                            "LPBA40SPM5",                                                         "LPBA40SPM5FrameOfReference",                                         { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },

    // UTC Synchronization Frame of Reference
    { UID_UniversalCoordinatedTimeSynchronizationFrameOfReference,               "UTC",                                                                "UniversalCoordinatedTimeSynchronizationFrameOfReference",            { EUS_DICOM, EUV_Standard, EUT_FrameOfReference, EUST_other, EUIT_other, UID_PROP_NONE } },

    // Well-known SOP Instances for Color Palettes
    { UID_HotIronColorPaletteSOPInstance,                                        "HotIronPalette",                                                     "HotIronColorPaletteSOPInstance",                                     { EUS_DICOM, EUV_Standard, EUT_SOPInstance, EUST_ColorPalette, EUIT_other, UID_PROP_NONE } },
    { UID_HotMetalBlueColorPaletteSOPInstance,                                   "HotMetalBluePalette",                                                "HotMetalBlueColorPaletteSOPInstance",                                { EUS_DICOM, EUV_Standard, EUT_SOPInstance, EUST_ColorPalette, EUIT_other, UID_PROP_NONE } },
    { UID_PET20StepColorPaletteSOPInstance,                                      "PET20StepPalette",                                                   "PET20StepColorPaletteSOPInstance",                                   { EUS_DICOM, EUV_Standard, EUT_SOPInstance, EUST_ColorPalette, EUIT_other, UID_PROP_NONE } },
    { UID_PETColorPaletteSOPInstance,                                            "PETPalette",                                                         "PETColorPaletteSOPInstance",                                         { EUS_DICOM, EUV_Standard, EUT_SOPInstance, EUST_ColorPalette, EUIT_other, UID_PROP_NONE } },
    { UID_SpringColorPaletteSOPInstance,                                         "SpringPalette",                                                      "SpringColorPaletteSOPInstance",                                      { EUS_DICOM, EUV_Standard, EUT_SOPInstance, EUST_ColorPalette, EUIT_other, UID_PROP_NONE } },
    { UID_SummerColorPaletteSOPInstance,                                         "SummerPalette",                                                      "SummerColorPaletteSOPInstance",                                      { EUS_DICOM, EUV_Standard, EUT_SOPInstance, EUST_ColorPalette, EUIT_other, UID_PROP_NONE } },
    { UID_FallColorPaletteSOPInstance,                                           "FallPalette",                                                        "FallColorPaletteSOPInstance",                                        { EUS_DICOM, EUV_Standard, EUT_SOPInstance, EUST_ColorPalette, EUIT_other, UID_PROP_NONE } },
    { UID_WinterColorPaletteSOPInstance,                                         "WinterPalette",                                                      "WinterColorPaletteSOPInstance",                                      { EUS_DICOM, EUV_Standard, EUT_SOPInstance, EUST_ColorPalette, EUIT_other, UID_PROP_NONE } },

    // Draft Supplements
    { UID_DRAFT_RTBeamsDeliveryInstructionStorage,                               "RTBeamsDeliveryInstructionStorageTrial",                             "DRAFT_RTBeamsDeliveryInstructionStorage",                            { EUS_DICOM, EUV_Draft, EUT_SOPClass,     EUST_Storage, EUIT_other,            UID_PROP_NONE } },
    { UID_DRAFT_RTConventionalMachineVerification,                               "RTConventionalMachineVerificationTrial",                             "DRAFT_RTConventionalMachineVerification",                            { EUS_DICOM, EUV_Draft, EUT_SOPClass,     EUST_other,   EUIT_other,            UID_PROP_NONE } },
    { UID_DRAFT_RTIonMachineVerification,                                        "RTIonMachineVerificationTrial",                                      "DRAFT_RTIonMachineVerification",                                     { EUS_DICOM, EUV_Draft, EUT_SOPClass,     EUST_other,   EUIT_other,            UID_PROP_NONE } },
    { UID_DRAFT_SRAudioStorage,                                                  "AudioSRStorageTrial",                                                "DRAFT_SRAudioStorage",                                               { EUS_DICOM, EUV_Draft, EUT_SOPClass,     EUST_Storage, EUIT_StructuredReport, UID_PROP_NONE } },
    { UID_DRAFT_SRComprehensiveStorage,                                          "ComprehensiveSRStorageTrial",                                        "DRAFT_SRComprehensiveStorage",                                       { EUS_DICOM, EUV_Draft, EUT_SOPClass,     EUST_Storage, EUIT_StructuredReport, UID_PROP_NONE } },
    { UID_DRAFT_SRDetailStorage,                                                 "DetailSRStorageTrial",                                               "DRAFT_SRDetailStorage",                                              { EUS_DICOM, EUV_Draft, EUT_SOPClass,     EUST_Storage, EUIT_StructuredReport, UID_PROP_NONE } },
    { UID_DRAFT_SRTextStorage,                                                   "TextSRStorageTrial",                                                 "DRAFT_SRTextStorage",                                                { EUS_DICOM, EUV_Draft, EUT_SOPClass,     EUST_Storage, EUIT_StructuredReport, UID_PROP_NONE } },
    { UID_DRAFT_WaveformStorage,                                                 "WaveformStorageTrial",                                               "DRAFT_WaveformStorage",                                              { EUS_DICOM, EUV_Draft, EUT_SOPClass,     EUST_Storage, EUIT_Waveform,         UID_PROP_NONE } },
    { UID_DRAFT_UnifiedWorklistAndProcedureStepServiceClass,                     "UnifiedWorklistAndProcedureStepTrial",                               "DRAFT_UnifiedWorklistAndProcedureStepServiceClass",                  { EUS_DICOM, EUV_Draft, EUT_ServiceClass, EUST_other,   EUIT_other,            UID_PROP_NONE } },
    { UID_DRAFT_UnifiedProcedureStepPushSOPClass,                                "UnifiedProcedureStepPushTrial",                                      "DRAFT_UnifiedProcedureStepPushSOPClass",                             { EUS_DICOM, EUV_Draft, EUT_SOPClass,     EUST_other,   EUIT_other,            UID_PROP_NONE } },
    { UID_DRAFT_UnifiedProcedureStepWatchSOPClass,                               "UnifiedProcedureStepWatchTrial",                                     "DRAFT_UnifiedProcedureStepWatchSOPClass",                            { EUS_DICOM, EUV_Draft, EUT_SOPClass,     EUST_other,   EUIT_other,            UID_PROP_NONE } },
    { UID_DRAFT_UnifiedProcedureStepPullSOPClass,                                "UnifiedProcedureStepPullTrial",                                      "DRAFT_UnifiedProcedureStepPullSOPClass",                             { EUS_DICOM, EUV_Draft, EUT_SOPClass,     EUST_other,   EUIT_other,            UID_PROP_NONE } },
    { UID_DRAFT_UnifiedProcedureStepEventSOPClass,                               "UnifiedProcedureStepEventTrial",                                     "DRAFT_UnifiedProcedureStepEventSOPClass",                            { EUS_DICOM, EUV_Draft, EUT_SOPClass,     EUST_other,   EUIT_other,            UID_PROP_NONE } },

    // end of the list
    { NULL, NULL, NULL, { EUS_other, EUV_other, EUT_other, EUST_other, EUIT_other, UID_PROP_NONE } }
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

/*
** dcmFindUIDFromName(const char* name)
** Return the UID of a name.
** Performs a table lookup and returns a pointer to a read-only string.
** Returns NULL if the name is not known.
*/
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

/*
** dcmFindUIDFromKeyword(const char* keyword)
** Return the UID of a keyword.
** Performs a table lookup and returns a pointer to a read-only string.
** Returns NULL if the keyword is not known.
*/
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
** dcmGetPropertiesOfUID(const char* uid, DcmUIDProperties &properties)
** Return the properties of a UID.
** Performs a table lookup and fills a given struct with the properties.
** Returns true if UID was found, false otherwise.
*/
OFBool
dcmGetPropertiesOfUID(const char* uid, DcmUIDProperties &properties)
{
    if (uid != NULL)
    {
      for (int i = 0; i < uidNameMap_size; i++) {
        if (uidNameMap[i].uid != NULL && strcmp(uid, uidNameMap[i].uid) == 0) {
          properties = uidNameMap[i].properties;
          return OFTrue;
        }
      }
    }
    return OFFalse;
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
