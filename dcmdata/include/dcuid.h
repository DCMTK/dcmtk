/*
 *
 *  Copyright (C) 1994-2003, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose:
 *  Definitions of "well known" DICOM Unique Indentifiers,
 *  routines for finding and creating UIDs.
 *
 *  Last Update:      $Author: wilkens $
 *  Update Date:      $Date: 2004-02-11 15:53:08 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcuid.h,v $
 *  CVS/RCS Revision: $Revision: 1.58 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCUID_H
#define DCUID_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctypes.h"

#define INCLUDE_CSTDLIB
#include "ofstdinc.h"

#ifdef HAVE_UNISTD_H
BEGIN_EXTERN_C
#include <unistd.h> /* for NULL */
END_EXTERN_C
#endif


/*
** dcmFindNameOfUID(const char* uid)
** Return the name of a UID.
** Performs a table lookup and returns a pointer to a read-only string.
** Returns NULL if the UID is not known.
*/

const char* dcmFindNameOfUID(const char* uid);

//
// dcmFindUIDFromName(const char* name)
// Return the UID of a name.
// Performs a table lookup and returns a pointer to a read-only string.
// Returns NULL if the name is not known.
//

const char * dcmFindUIDFromName(const char * name);


/*
** The global variable dcmStorageSOPClassUIDs is an array of
** string pointers containing the UIDs of all known Storage SOP
** Classes.  The global variable numberOfDcmStorageStopClassUIDs
** defines the size of the array.
*/

extern const char* dcmStorageSOPClassUIDs[];
extern const int numberOfDcmStorageSOPClassUIDs;

/*
** dcmIsaStorageSOPClassUID(const char* uid)
** Returns true if the uid is one of the Storage SOP Classes.
** Performs a table lookup in the dcmStorageSOPClassUIDs table.
*/
OFBool dcmIsaStorageSOPClassUID(const char* uid);

/*
** The global variable dcmImageSOPClassUIDs is an array of
** string pointers containing the UIDs of all known Image SOP
** Classes.  The global variable numberOfDcmImageSOPClassUIDs
** defines the size of the array.
** NOTE: this list represets a subset of the dcmStorageSOPClassUIDs list
*/

extern const char* dcmImageSOPClassUIDs[];
extern const int numberOfDcmImageSOPClassUIDs;


/*
** char* generateUniqueIdentifier(char* uid)
** Creates a Unique Identifer in uid and returns uid.
** uid must be at least 65 bytes. Care is taken to make sure
** that the generated UID is 64 characters or less.
**
** If a prefix string is not passed as the second argument a
** default of SITE_INSTANCE_UID_ROOT (see below) will be used.
** Otherwise the supplied prefix string will appear at the beginning
** of uid.
**
** The UID is created by appending to the prefix the following:
**      the host id (if obtainable, zero otherwise)
**      the process id (if obtainable, zero otherwise)
**      the system calendar time
**      an accumulating counter for this process
*/
char* dcmGenerateUniqueIdentifier(char* uid, const char* prefix=NULL);

/*
 * dcmSOPClassUIDToModality
 * performs a table lookup and returns a short modality identifier
 * that can be used for building file names etc.
 * Identifiers are defined for all storage SOP classes.
 * Returns NULL if no modality identifier found or sopClassUID==NULL.
 */
const char *dcmSOPClassUIDToModality(const char *sopClassUID);

/*
 * dcmGuessModalityBytes
 * performs a table lookup and returns a guessed average
 * file size for the given SOP class.
 * Average sizes are defined for all storage SOP classes.
 */
unsigned long dcmGuessModalityBytes(const char *sopClassUID);

/*
** String Constants
*/

/*
** OFFIS UID is: 1.2.276.0.7230010
** UID Root for dcmtk DICOM project:   1.2.276.0.7230010.3
**          for OFFIS GO-Kard project: 1.2.276.0.7230010.8
*/

/* NOTE: Implementation version name VR=SH may not be longer than 16 chars
 *       The second name is used to identify files written without dcmdata
 *       (i.e. using the --bit-preserving switch in various tools)
 */
#define OFFIS_DTK_IMPLEMENTATION_VERSION_NAME   "OFFIS_DCMTK_352"
#define OFFIS_DTK_IMPLEMENTATION_VERSION_NAME2  "OFFIS_DCMBP_352"
#define OFFIS_DCMTK_RELEASEDATE                 "2003-11-07"

#define OFFIS_UID_ROOT                          "1.2.276.0.7230010.3"
#define OFFIS_DCMTK_VERSION_NUMBER              352
#define OFFIS_DCMTK_VERSION_STRING              "3.5.2"
#define OFFIS_DCMTK_VERSION_SUFFIX              "a"
#define OFFIS_DCMTK_VERSION                     OFFIS_DCMTK_VERSION_STRING OFFIS_DCMTK_VERSION_SUFFIX
#define OFFIS_IMPLEMENTATION_CLASS_UID          OFFIS_UID_ROOT ".0." OFFIS_DCMTK_VERSION_STRING
#define OFFIS_INSTANCE_CREATOR_UID              OFFIS_IMPLEMENTATION_CLASS_UID

#define OFFIS_CODING_SCHEME_UID_ROOT            OFFIS_UID_ROOT ".0.0"
#define OFFIS_CODING_SCHEME_VERSION             "1"
#define OFFIS_CODING_SCHEME_UID                 OFFIS_CODING_SCHEME_UID_ROOT "." OFFIS_CODING_SCHEME_VERSION

/*
** Each site should define its own SITE_UID_ROOT
*/
#ifndef SITE_UID_ROOT
#define SITE_UID_ROOT                           OFFIS_UID_ROOT  /* default */
#endif

/*
** Useful UID prefixes. These can be whatever you want.
**
** These site UIDs are arbitary, non-standard, with no meaning
** and can be changed at any time.  Do _not_ rely on these values.
** Do _not_ assume any semantics when using these suffixes.
**
*/

#define SITE_STUDY_UID_ROOT                     SITE_UID_ROOT ".1.2"
#define SITE_SERIES_UID_ROOT                    SITE_UID_ROOT ".1.3"
#define SITE_INSTANCE_UID_ROOT                  SITE_UID_ROOT ".1.4"

/*
** A private SOP Class UID which can be used in a file meta-header when
** no real SOP Class is stored in the file. -- NON-STANDARD
*/
#define UID_PrivateGenericFileSOPClass          SITE_UID_ROOT ".1.0.1"


/*
** DICOM Defined Standard Application Context UID
*/

#define UID_StandardApplicationContext          "1.2.840.10008.3.1.1.1"

/*
** Defined Transfer Syntax UIDs
*/

/* Implicit VR Little Endian: Default Transfer Syntax for DICOM */
#define UID_LittleEndianImplicitTransferSyntax  "1.2.840.10008.1.2"
/* Explicit VR Little Endian */
#define UID_LittleEndianExplicitTransferSyntax  "1.2.840.10008.1.2.1"
/* Explicit VR Big Endian */
#define UID_BigEndianExplicitTransferSyntax     "1.2.840.10008.1.2.2"
/* JPEG Baseline (Process 1): Default Transfer Syntax
   for Lossy JPEG 8 Bit Image Compression */
#define UID_JPEGProcess1TransferSyntax          "1.2.840.10008.1.2.4.50"
/* JPEG Extended (Process 2 & 4): Default Transfer Syntax
   for Lossy JPEG 12 Bit Image Compression (Process 4 only) */
#define UID_JPEGProcess2_4TransferSyntax        "1.2.840.10008.1.2.4.51"
/* JPEG Extended (Process 3 & 5) */
#define UID_JPEGProcess3_5TransferSyntax        "1.2.840.10008.1.2.4.52"
/* JPEG Spectral Selection, Non-Hierarchical (Process 6 & 8) */
#define UID_JPEGProcess6_8TransferSyntax        "1.2.840.10008.1.2.4.53"
/* JPEG Spectral Selection, Non-Hierarchical (Process 7 & 9) */
#define UID_JPEGProcess7_9TransferSyntax        "1.2.840.10008.1.2.4.54"
/* JPEG Full Progression, Non-Hierarchical (Process 10 & 12) */
#define UID_JPEGProcess10_12TransferSyntax      "1.2.840.10008.1.2.4.55"
/* JPEG Full Progression, Non-Hierarchical (Process 11 & 13) */
#define UID_JPEGProcess11_13TransferSyntax      "1.2.840.10008.1.2.4.56"
/* JPEG Lossless, Non-Hierarchical (Process 14) */
#define UID_JPEGProcess14TransferSyntax         "1.2.840.10008.1.2.4.57"
/* JPEG Lossless, Non-Hierarchical (Process 15) */
#define UID_JPEGProcess15TransferSyntax         "1.2.840.10008.1.2.4.58"
/* JPEG Extended, Hierarchical (Process 16 & 18) */
#define UID_JPEGProcess16_18TransferSyntax      "1.2.840.10008.1.2.4.59"
/* JPEG Extended, Hierarchical (Process 17 & 19) */
#define UID_JPEGProcess17_19TransferSyntax      "1.2.840.10008.1.2.4.60"
/* JPEG Spectral Selection, Hierarchical (Process 20 & 22) */
#define UID_JPEGProcess20_22TransferSyntax      "1.2.840.10008.1.2.4.61"
/* JPEG Spectral Selection, Hierarchical (Process 21 & 23) */
#define UID_JPEGProcess21_23TransferSyntax      "1.2.840.10008.1.2.4.62"
/* JPEG Full Progression, Hierarchical (Process 24 & 26) */
#define UID_JPEGProcess24_26TransferSyntax      "1.2.840.10008.1.2.4.63"
/* JPEG Full Progression, Hierarchical (Process 25 & 27) */
#define UID_JPEGProcess25_27TransferSyntax      "1.2.840.10008.1.2.4.64"
/* JPEG Lossless, Hierarchical (Process 28) */
#define UID_JPEGProcess28TransferSyntax         "1.2.840.10008.1.2.4.65"
/* JPEG Lossless, Hierarchical (Process 29) */
#define UID_JPEGProcess29TransferSyntax         "1.2.840.10008.1.2.4.66"
/* JPEG Lossless, Non-Hierarchical, First-Order Prediction (Process 14
   [Selection Value 1]): Default Transfer Syntax for Lossless JPEG Image Compression */
#define UID_JPEGProcess14SV1TransferSyntax      "1.2.840.10008.1.2.4.70"
/* JPEG-LS Lossless Image Compression */
#define UID_JPEGLSLossless                      "1.2.840.10008.1.2.4.80"
/* JPEG-LS Lossy (Near-Lossless) Image Compression */
#define UID_JPEGLSLossy                         "1.2.840.10008.1.2.4.81"
/* RLE Lossless */
#define UID_RLELossless                         "1.2.840.10008.1.2.5"
/* Deflated Explicit VR Little Endian */
#define UID_DeflatedExplicitVRLittleEndianTransferSyntax "1.2.840.10008.1.2.1.99"
/* JPEG 2000 Image Compression (Lossless Only) */
#define UID_JPEG2000LosslessOnlyTransferSyntax  "1.2.840.10008.1.2.4.90"
/* JPEG 2000 Image Compression (Lossless or Lossy) */
#define UID_JPEG2000TransferSyntax              "1.2.840.10008.1.2.4.91"


/*
** Defined SOP UIDs according to 2001 DICOM edition
*/

// Storage
#define UID_StoredPrintStorage                                     "1.2.840.10008.5.1.1.27"
#define UID_HardcopyGrayscaleImageStorage                          "1.2.840.10008.5.1.1.29"
#define UID_HardcopyColorImageStorage                              "1.2.840.10008.5.1.1.30"
#define UID_ComputedRadiographyImageStorage                        "1.2.840.10008.5.1.4.1.1.1"
#define UID_DigitalXRayImageStorageForPresentation                 "1.2.840.10008.5.1.4.1.1.1.1"
#define UID_DigitalXRayImageStorageForProcessing                   "1.2.840.10008.5.1.4.1.1.1.1.1"
#define UID_DigitalMammographyXRayImageStorageForPresentation      "1.2.840.10008.5.1.4.1.1.1.2"
#define UID_DigitalMammographyXRayImageStorageForProcessing        "1.2.840.10008.5.1.4.1.1.1.2.1"
#define UID_DigitalIntraOralXRayImageStorageForPresentation        "1.2.840.10008.5.1.4.1.1.1.3"
#define UID_DigitalIntraOralXRayImageStorageForProcessing          "1.2.840.10008.5.1.4.1.1.1.3.1"
#define UID_CTImageStorage                                         "1.2.840.10008.5.1.4.1.1.2"
#define UID_EnhancedCTImageStorage                                 "1.2.840.10008.5.1.4.1.1.2.1"
#define UID_RETIRED_UltrasoundMultiframeImageStorage               "1.2.840.10008.5.1.4.1.1.3"
#define UID_UltrasoundMultiframeImageStorage                       "1.2.840.10008.5.1.4.1.1.3.1"
#define UID_MRImageStorage                                         "1.2.840.10008.5.1.4.1.1.4"
#define UID_RETIRED_NuclearMedicineImageStorage                    "1.2.840.10008.5.1.4.1.1.5"
#define UID_RETIRED_UltrasoundImageStorage                         "1.2.840.10008.5.1.4.1.1.6"
#define UID_UltrasoundImageStorage                                 "1.2.840.10008.5.1.4.1.1.6.1"
#define UID_SecondaryCaptureImageStorage                           "1.2.840.10008.5.1.4.1.1.7"
#define UID_StandaloneOverlayStorage                               "1.2.840.10008.5.1.4.1.1.8"
#define UID_StandaloneCurveStorage                                 "1.2.840.10008.5.1.4.1.1.9"
#define UID_TwelveLeadECGWaveformStorage                           "1.2.840.10008.5.1.4.1.1.9.1.1"
#define UID_GeneralECGWaveformStorage                              "1.2.840.10008.5.1.4.1.1.9.1.2"
#define UID_AmbulatoryECGWaveformStorage                           "1.2.840.10008.5.1.4.1.1.9.1.3"
#define UID_HemodynamicWaveformStorage                             "1.2.840.10008.5.1.4.1.1.9.2.1"
#define UID_CardiacElectrophysiologyWaveformStorage                "1.2.840.10008.5.1.4.1.1.9.3.1"
#define UID_BasicVoiceAudioWaveformStorage                         "1.2.840.10008.5.1.4.1.1.9.4.1"
#define UID_StandaloneModalityLUTStorage                           "1.2.840.10008.5.1.4.1.1.10"
#define UID_StandaloneVOILUTStorage                                "1.2.840.10008.5.1.4.1.1.11"
#define UID_GrayscaleSoftcopyPresentationStateStorage              "1.2.840.10008.5.1.4.1.1.11.1"
#define UID_XRayAngiographicImageStorage                           "1.2.840.10008.5.1.4.1.1.12.1"
#define UID_XRayFluoroscopyImageStorage                            "1.2.840.10008.5.1.4.1.1.12.2"
#define UID_RETIRED_XRayAngiographicBiPlaneImageStorage            "1.2.840.10008.5.1.4.1.1.12.3"
#define UID_NuclearMedicineImageStorage                            "1.2.840.10008.5.1.4.1.1.20"
#define UID_RETIRED_VLImageStorage                                 "1.2.840.10008.5.1.4.1.1.77.1"
#define UID_VLEndoscopicImageStorage                               "1.2.840.10008.5.1.4.1.1.77.1.1"
#define UID_VLMicroscopicImageStorage                              "1.2.840.10008.5.1.4.1.1.77.1.2"
#define UID_VLSlideCoordinatesMicroscopicImageStorage              "1.2.840.10008.5.1.4.1.1.77.1.3"
#define UID_VLPhotographicImageStorage                             "1.2.840.10008.5.1.4.1.1.77.1.4"
#define UID_RETIRED_VLMultiFrameImageStorage                       "1.2.840.10008.5.1.4.1.1.77.2"
#define UID_BasicTextSR                                            "1.2.840.10008.5.1.4.1.1.88.11"
#define UID_EnhancedSR                                             "1.2.840.10008.5.1.4.1.1.88.22"
#define UID_ComprehensiveSR                                        "1.2.840.10008.5.1.4.1.1.88.33"
#define UID_PETImageStorage                                        "1.2.840.10008.5.1.4.1.1.128"
#define UID_PETCurveStorage                                        "1.2.840.10008.5.1.4.1.1.129"
#define UID_RTImageStorage                                         "1.2.840.10008.5.1.4.1.1.481.1"
#define UID_RTDoseStorage                                          "1.2.840.10008.5.1.4.1.1.481.2"
#define UID_RTStructureSetStorage                                  "1.2.840.10008.5.1.4.1.1.481.3"
#define UID_RTBeamsTreatmentRecordStorage                          "1.2.840.10008.5.1.4.1.1.481.4"
#define UID_RTPlanStorage                                          "1.2.840.10008.5.1.4.1.1.481.5"
#define UID_RTBrachyTreatmentRecordStorage                         "1.2.840.10008.5.1.4.1.1.481.6"
#define UID_RTTreatmentSummaryRecordStorage                        "1.2.840.10008.5.1.4.1.1.481.7"

// Storage SOP Classes from Supplement 49 (final text)
#define UID_EnhancedMRImageStorage                                  "1.2.840.10008.5.1.4.1.1.4.1"
#define UID_MRSpectroscopyStorage                                   "1.2.840.10008.5.1.4.1.1.4.2"
#define UID_RawDataStorage                                          "1.2.840.10008.5.1.4.1.1.66"

// Storage SOP Class from Supplement 50 (final text)
#define UID_MammographyCADSR                                       "1.2.840.10008.5.1.4.1.1.88.50"

// Storage SOP Classes from Supplement 57 (final text)
#define UID_MultiframeSingleBitSecondaryCaptureImageStorage        "1.2.840.10008.5.1.4.1.1.7.1"
#define UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage    "1.2.840.10008.5.1.4.1.1.7.2"
#define UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage    "1.2.840.10008.5.1.4.1.1.7.3"
#define UID_MultiframeTrueColorSecondaryCaptureImageStorage        "1.2.840.10008.5.1.4.1.1.7.4"

// Storage SOP Class from Supplement 59 (final text)
#define UID_KeyObjectSelectionDocument                             "1.2.840.10008.5.1.4.1.1.88.59"

// Storage SOP Class from Supplement 65 (final text)
#define UID_ChestCADSR                                             "1.2.840.10008.5.1.4.1.1.88.65"

// Storage SOP Class from Supplement 66 (final text)
#define UID_ProcedureLogStorage                                    "1.2.840.10008.5.1.4.1.1.88.40"

// Storage SOP Class from Supplement 73 (final text)
#define UID_SpatialRegistrationStorage                             "1.2.840.10008.5.1.4.1.1.66.1"
#define UID_SpatialFinducialsStorage                               "1.2.840.10008.5.1.4.1.1.66.2"

// Query/Retrieve
#define UID_FINDPatientRootQueryRetrieveInformationModel           "1.2.840.10008.5.1.4.1.2.1.1"
#define UID_MOVEPatientRootQueryRetrieveInformationModel           "1.2.840.10008.5.1.4.1.2.1.2"
#define UID_GETPatientRootQueryRetrieveInformationModel            "1.2.840.10008.5.1.4.1.2.1.3"
#define UID_FINDStudyRootQueryRetrieveInformationModel             "1.2.840.10008.5.1.4.1.2.2.1"
#define UID_MOVEStudyRootQueryRetrieveInformationModel             "1.2.840.10008.5.1.4.1.2.2.2"
#define UID_GETStudyRootQueryRetrieveInformationModel              "1.2.840.10008.5.1.4.1.2.2.3"
#define UID_FINDPatientStudyOnlyQueryRetrieveInformationModel      "1.2.840.10008.5.1.4.1.2.3.1"
#define UID_MOVEPatientStudyOnlyQueryRetrieveInformationModel      "1.2.840.10008.5.1.4.1.2.3.2"
#define UID_GETPatientStudyOnlyQueryRetrieveInformationModel       "1.2.840.10008.5.1.4.1.2.3.3"
#define UID_FINDModalityWorklistInformationModel                   "1.2.840.10008.5.1.4.31"
// Supplement 52 (final text)
#define UID_FINDGeneralPurposeWorklistInformationModel "1.2.840.10008.5.1.4.32.1"

// Print
#define UID_BasicFilmSessionSOPClass                               "1.2.840.10008.5.1.1.1"
#define UID_BasicFilmBoxSOPClass                                   "1.2.840.10008.5.1.1.2"
#define UID_BasicGrayscaleImageBoxSOPClass                         "1.2.840.10008.5.1.1.4"
#define UID_BasicColorImageBoxSOPClass                             "1.2.840.10008.5.1.1.4.1"
#define UID_RETIRED_ReferencedImageBoxSOPClass                     "1.2.840.10008.5.1.1.4.2"
#define UID_BasicGrayscalePrintManagementMetaSOPClass              "1.2.840.10008.5.1.1.9"
#define UID_RETIRED_ReferencedGrayscalePrintManagementMetaSOPClass "1.2.840.10008.5.1.1.9.1"
#define UID_PrintJobSOPClass                                       "1.2.840.10008.5.1.1.14"
#define UID_BasicAnnotationBoxSOPClass                             "1.2.840.10008.5.1.1.15"
#define UID_PrinterSOPClass                                        "1.2.840.10008.5.1.1.16"
#define UID_PrinterConfigurationRetrievalSOPClass                  "1.2.840.10008.5.1.1.16.376"
#define UID_PrinterSOPInstance                                     "1.2.840.10008.5.1.1.17"
#define UID_PrinterConfigurationRetrievalSOPInstance               "1.2.840.10008.5.1.1.17.376"
#define UID_BasicColorPrintManagementMetaSOPClass                  "1.2.840.10008.5.1.1.18"
#define UID_RETIRED_ReferencedColorPrintManagementMetaSOPClass     "1.2.840.10008.5.1.1.18.1"
#define UID_VOILUTBoxSOPClass                                      "1.2.840.10008.5.1.1.22"
#define UID_PresentationLUTSOPClass                                "1.2.840.10008.5.1.1.23"
#define UID_ImageOverlayBoxSOPClass                                "1.2.840.10008.5.1.1.24"
#define UID_BasicPrintImageOverlayBoxSOPClass                      "1.2.840.10008.5.1.1.24.1"
#define UID_PrintQueueSOPInstance                                  "1.2.840.10008.5.1.1.25"
#define UID_PrintQueueManagementSOPClass                           "1.2.840.10008.5.1.1.26"
#define UID_PullPrintRequestSOPClass                               "1.2.840.10008.5.1.1.31"
#define UID_PullStoredPrintManagementMetaSOPClass                  "1.2.840.10008.5.1.1.32"

// Storage Commitment
#define UID_StorageCommitmentPushModelSOPClass                     "1.2.840.10008.1.20.1"
#define UID_StorageCommitmentPushModelSOPInstance                  "1.2.840.10008.1.20.1.1"
#define UID_RETIRED_StorageCommitmentPullModelSOPClass             "1.2.840.10008.1.20.2"
#define UID_RETIRED_StorageCommitmentPullModelSOPInstance          "1.2.840.10008.1.20.2.1"

// MPPS
#define UID_ModalityPerformedProcedureStepSOPClass                 "1.2.840.10008.3.1.2.3.3"
#define UID_ModalityPerformedProcedureStepRetrieveSOPClass         "1.2.840.10008.3.1.2.3.4"
#define UID_ModalityPerformedProcedureStepNotificationSOPClass     "1.2.840.10008.3.1.2.3.5"

// Detached Management
#define UID_DetachedPatientManagementSOPClass                      "1.2.840.10008.3.1.2.1.1"
#define UID_DetachedPatientManagementMetaSOPClass                  "1.2.840.10008.3.1.2.1.4"
#define UID_DetachedVisitManagementSOPClass                        "1.2.840.10008.3.1.2.2.1"
#define UID_DetachedStudyManagementSOPClass                        "1.2.840.10008.3.1.2.3.1"
#define UID_DetachedResultsManagementSOPClass                      "1.2.840.10008.3.1.2.5.1"
#define UID_DetachedResultsManagementMetaSOPClass                  "1.2.840.10008.3.1.2.5.4"
#define UID_DetachedStudyManagementMetaSOPClass                    "1.2.840.10008.3.1.2.5.5"
#define UID_DetachedInterpretationManagementSOPClass               "1.2.840.10008.3.1.2.6.1"

// General Purpose Worklist (Supplement 52 final text)
#define UID_GeneralPurposeScheduledProcedureStepSOPClass           "1.2.840.10008.5.1.4.32.2"
#define UID_GeneralPurposePerformedProcedureStepSOPClass           "1.2.840.10008.5.1.4.32.3"
#define UID_GeneralPurposeWorklistManagementMetaSOPClass           "1.2.840.10008.5.1.4.32"

// Other
#define UID_VerificationSOPClass                                   "1.2.840.10008.1.1"
// official name in DICOM standard: Media Storage Directory Storage
#define UID_BasicDirectoryStorageSOPClass                          "1.2.840.10008.1.3.10"
#define UID_BasicStudyContentNotificationSOPClass                  "1.2.840.10008.1.9"
#define UID_StudyComponentManagementSOPClass                       "1.2.840.10008.3.1.2.3.2"

// UID for DICOM Controlled Terminology, defined in CP 324
#define UID_DICOMControlledTerminologyCodingScheme                 "1.2.840.10008.2.16.4"

// Supplement 66 (final text)
#define UID_ProceduralEventLoggingSOPClass                         "1.2.840.10008.1.40"
#define UID_ProceduralEventLoggingSOPInstance                      "1.2.840.10008.1.40.1"

// Supplement 73 (final text)
#define UID_TalairachBrainAtlasFrameOfReference                    "1.2.840.10008.1.4.1.1"
#define UID_SPM2T1FrameOfReference                                 "1.2.840.10008.1.4.1.2"
#define UID_SPM2T2FrameOfReference                                 "1.2.840.10008.1.4.1.3"
#define UID_SPM2PDFrameOfReference                                 "1.2.840.10008.1.4.1.4"
#define UID_SPM2EPIFrameOfReference                                "1.2.840.10008.1.4.1.5"
#define UID_SPM2FILT1FrameOfReference                              "1.2.840.10008.1.4.1.6"
#define UID_SPM2PETFrameOfReference                                "1.2.840.10008.1.4.1.7"
#define UID_SPM2TRANSMFrameOfReference                             "1.2.840.10008.1.4.1.8"
#define UID_SPM2SPECTFrameOfReference                              "1.2.840.10008.1.4.1.9"
#define UID_SPM2GRAYFrameOfReference                               "1.2.840.10008.1.4.1.10"
#define UID_SPM2WHITEFrameOfReference                              "1.2.840.10008.1.4.1.11"
#define UID_SPM2CSFFrameOfReference                                "1.2.840.10008.1.4.1.12"
#define UID_SPM2BRAINMASKFrameOfReference                          "1.2.840.10008.1.4.1.13"
#define UID_SPM2AVG305T1FrameOfReference                           "1.2.840.10008.1.4.1.14"
#define UID_SPM2AVG152T1FrameOfReference                           "1.2.840.10008.1.4.1.15"
#define UID_SPM2AVG152T2FrameOfReference                           "1.2.840.10008.1.4.1.16"
#define UID_SPM2AVG152PDFrameOfReference                           "1.2.840.10008.1.4.1.17"
#define UID_SPM2SINGLESUBJT1FrameOfReference                       "1.2.840.10008.1.4.1.18"
#define UID_ICBM452T1FrameOfReference                              "1.2.840.10008.1.4.2.1"
#define UID_ICBMSingleSubjectMRIFrameOfReference                   "1.2.840.10008.1.4.2.2"

// Private DCMTK UIDs
// Private SOP Class UID used to shutdown external network applications
#define UID_PrivateShutdownSOPClass                                "1.2.276.0.7230010.3.4.1915765545.18030.917282194.0"


/* DRAFT SUPPLEMENTS - EXPERIMENTAL USE ONLY */

/*
 * The following UIDs were defined in "frozen draft for trial implementation"
 * versions of various DICOM supplements and changed before final text.
 * Since it is likely that trial implementations exist, we leave the UIDs in the dictionary.
 */

/* Supplement 23 Frozen Draft (November 1997) */
#define UID_DRAFT_SRTextStorage                                    "1.2.840.10008.5.1.4.1.1.88.1"
#define UID_DRAFT_SRAudioStorage                                   "1.2.840.10008.5.1.4.1.1.88.2"
#define UID_DRAFT_SRDetailStorage                                  "1.2.840.10008.5.1.4.1.1.88.3"
#define UID_DRAFT_SRComprehensiveStorage                           "1.2.840.10008.5.1.4.1.1.88.4"

/* Supplement 30 Draft 08 for Demonstration (October 1997) */
#define UID_DRAFT_WaveformStorage                                  "1.2.840.10008.5.1.4.1.1.9.1"


#endif /* DCUID_H */

/*
** CVS/RCS Log:
** $Log: dcuid.h,v $
** Revision 1.58  2004-02-11 15:53:08  wilkens
** Updated attribute and UID specification (update before dcmtk 3.5.3 release).
**
** Revision 1.57  2003/11/07 13:57:19  meichel
** Updated version name to reflect interim release 3.5.2a
**
** Revision 1.56  2003/10/09 17:47:25  joergr
** Renamed OFFIS_PRIVATE_CODING_xxx macros to OFFIS_CODING_xxx.
**
** Revision 1.55  2003/10/09 12:49:38  joergr
** Added support for SOP Class "Procedure Log" (Supplement 66).
**
** Revision 1.54  2003/05/20 08:53:27  joergr
** Added support for SOP Class "Chest CAD SR" (Supplement 65).
**
** Revision 1.53  2002/12/23 10:58:21  meichel
** Updated version name and implementation name to reflect release 3.5.2
**
** Revision 1.52  2002/12/09 13:03:19  meichel
** Updated list of UIDs
**
** Revision 1.51  2002/11/27 12:07:23  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.50  2002/06/19 15:35:55  meichel
** Updated list of SOP Class UIDs for Supplement 49
**
** Revision 1.49  2002/01/08 10:43:00  joergr
** Corrected spelling of function dcmGenerateUniqueIdentifier().
** Added comment about another reserved UID root (for OFFIS GO-Kard project).
**
** Revision 1.48  2001/12/20 11:16:53  meichel
** Updated Version Number and Implementation Version Name to reflect the
**   current public release (3.5.1)
**
** Revision 1.47  2001/11/08 16:17:29  meichel
** Updated data dictionary, UIDs and transfer syntaxes for DICOM 2001 edition.
**
** Revision 1.46  2001/11/02 15:44:02  meichel
** Introduced DCMTK version number macro OFFIS_DCMTK_VERSION_NUMBER in dcuid.h
**
** Revision 1.45  2001/06/14 14:04:06  meichel
** Updated Version Number and Implementation Version Name to reflect the
**   current public release (3.5.0)
**
** Revision 1.44  2001/06/01 15:48:46  meichel
** Updated copyright header
**
** Revision 1.43  2001/05/25 09:50:17  meichel
** Updated data dictionary and UID list (again).
**
** Revision 1.42  2001/05/10 16:10:36  meichel
** Updated data dictionary and UID list
**
** Revision 1.41  2000/12/20 10:18:23  meichel
** Updated Version Number and Implementation Version Name to reflect the
**   current public release (3.4.2)
**
** Revision 1.40  2000/12/14 12:47:41  joergr
** Updated SOP Class and Transfer Syntax UIDs for 2000 edition of the DICOM
** standard.
**
** Revision 1.39  2000/11/16 12:49:11  joergr
** Added definition of InstanceCreatorUID.
**
** Revision 1.38  2000/10/12 08:19:05  joergr
** Updated data dictionary to reflect the changes made in supplement 23
** (Structured Reporting) final text.
** Added private coding scheme creator UID for SR codes.  Restructured scheme
** for site UIDs.
**
** Revision 1.37  2000/05/30 13:02:24  joergr
** Added new private SOP class (UID definition) to allow external shutdown
** of console applications via negotiation of this special SOP class
** (currently used for imagectn and dcmpsrcv).
**
** Revision 1.36  2000/04/14 16:04:50  meichel
** Made function dcmGenerateUniqueIdentifer thread safe by protecting
**   the counter with a Mutex and using gethostbyname_r instead of
**   gethostbyname on Posix platforms.
**
** Revision 1.35  2000/03/08 16:26:20  meichel
** Updated copyright header.
**
** Revision 1.34  2000/02/23 15:11:40  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.33  2000/02/03 11:48:22  meichel
** Rebuilt data dictionary based on 1999 standard text,
**   latest supplements and CP packet 6.
**
** Revision 1.32  2000/02/01 10:12:03  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.31  1999/07/14 12:02:33  meichel
** Updated data dictionary for supplement 29, 39, 33_lb, CP packet 4 and 5.
**   Corrected dcmtk applications for changes in attribute name constants.
**
** Revision 1.30  1999/05/06 12:42:09  meichel
** Updated Version Number and Implementation Version Name to reflect the
**   current public release (3.4.0)
**
** Revision 1.29  1999/03/31 09:24:52  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.28  1999/03/22 13:41:22  meichel
** Reworked UID list based on the 1998 DICOM edition and the latest
**   supplement versions.
**
** Revision 1.27  1999/03/17 11:08:25  meichel
** added UIDs for Supplement 32 (DX)
**
** Revision 1.26  1998/10/26 13:02:49  meichel
** Updated data dictionary and UIDs for Grayscale Softcopy Presentation State
** (Supplement 33 frozen draft).
**
** Revision 1.25  1998/02/25 14:26:29  hewett
** Updated status of supplements 17 & 22 to reflect their Final Text status
** Updated data dictionary and UID information to reflect changes in
** Supplement 24 (Stored Print Related SOP Classes).  Some data dictionary
** attibutes and UIDs have changed between the Letter Ballot version and
** the Final Text version.
**
** Revision 1.24  1998/02/17 12:33:58  meichel
** Updated Version Number and Implementation Version Name to reflect the
**   current public release (3.3.1). Updated documentation to reflect changes in 3.3.1.
**
** Revision 1.23  1998/02/06 09:03:09  hewett
** Updated support for Supplements 15 (Visible Light),
** 16 (Postscript Print Management), 17 (Modality Performed Procedure Step),
** 22 (Presentation Look Up Table (LUT)), 23 (Structured Reporting),
** 24 (Stored Print), 30 (Waveform Interchange).
**
** Revision 1.22  1998/01/14 15:14:45  hewett
** Added basic support for the Structured Reporting (SR) SOP Classes.
**
** Revision 1.21  1997/10/07 07:44:20  meichel
** Updated Version Number and Implementation Version Name to reflect the
**   current public release (3.3)
**
** Revision 1.20  1997/10/01 11:42:26  hewett
** Updated DCMTK version number and date for 3.3 release
**
** Revision 1.19  1997/09/22 14:56:46  hewett
** Added a global list of Image SOP Classes UIDs (dcmImageSOPClassUIDs)
** which is used by dcmgpdir to identify SOP Classes which can be
** referenced from a DICOMDIR IMAGE record.
**
** Revision 1.18  1997/06/09 13:28:15  hewett
** Updated Version Number and Implementation Version Name to reflect the
** current public release (3.2.1)
**
** Revision 1.17  1997/06/02 08:27:48  hewett
** Updated Version Number and Implementation Version Name to reflect the
** current public release (3.2).
**
** Revision 1.16  1997/05/29 15:52:54  meichel
** Added constant for dcmtk release date in dcuid.h.
** All dcmtk applications now contain a version string
** which is displayed with the command line options ("usage" message)
** and which can be queried in the binary with the "ident" command.
**
** Revision 1.15  1997/05/23 10:44:17  meichel
** Major rewrite of storescp application. See CHANGES for details.
** Changes to interfaces of some DIMSE functions.
**
** Revision 1.14  1997/05/13 13:57:42  hewett
** Added UIDs for the draft supplements 12 (PET), 13 (Queue Management),
** 15 (Visible Light), 17 (Modality Performed Procedure Step), 22 (User
** Preference LUT) and 24 (Print Storage).  Updated UID tables so that
** recompiled Storage SCP/SCU's will handle the new SOPs.
**
** Revision 1.13  1997/05/09 13:07:27  hewett
** Updated version name.
**
** Revision 1.12  1997/03/27 16:10:43  hewett
** Updated Implementation Version Name and Implementation Class UID to
** reflect the new version 3.1.3
**
** Revision 1.11  1997/03/26 17:21:02  hewett
** Changed DCMTK's implementation version name to be consitent with VR=CS.
**
** Revision 1.10  1997/02/06 11:25:26  hewett
** Updated Implementation Version Name and Implementation Class UID to
** reflect the new version 3.1.2
**
** Revision 1.9  1996/11/27 16:12:17  hewett
** Added recommended uid roots for Patient/Study/Results Management objects.
**
** Revision 1.8  1996/09/24 16:01:28  hewett
** Added SOP Class UIDs for Radiotherapy Objects.
** Added a separate table of Storage SOP Class UIDs (usefull during
** association negotiation).
** Updated Implementation Version to 3.1.1
**
** Revision 1.7  1996/09/03 13:01:09  hewett
** Updated version name to reflect release 3.1.0.
**
** Revision 1.6  1996/05/31 09:56:33  hewett
** Reduced length of ImplementationVersionName to be <= 16 bytes.
**
** Revision 1.5  1996/05/31 09:46:46  hewett
** Updated InplementationVerionName for release 3.0.1.
**
** Revision 1.4  1996/04/19 12:44:07  hewett
** Added Transfer Syntax UID for RLE Lossless (defined in new Ultrasound).
**
** Revision 1.3  1996/03/12 15:34:15  hewett
** Added new SOP Class UIDs.
**
** Revision 1.2  1996/01/29 13:38:14  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.1  1995/11/23 16:38:05  hewett
** Updated for loadable data dictionary + some cleanup (more to do).
**
*/
