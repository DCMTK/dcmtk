/*
 *
 *  Copyright (C) 1994-2018, OFFIS e.V.
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

#ifndef DCUID_H
#define DCUID_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/dcmdata/dcdefine.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

// include this file in doxygen documentation

/** @file dcuid.h
 *  @brief global definitions and functions for UID handling
 */


/// type of Storage SOP Class
typedef enum {
    /// patient objects
    ESSC_Patient    = 0x01,
    /// non-patient objects
    ESSC_NonPatient = 0x02,
    /// image objects (subset of patient objects)
    ESSC_Image      = 0x04,
    /// all types (patient and non-patient objects)
    ESSC_All        = 0x03
} E_StorageSOPClassType;


/** return the name of a UID.
 *  Performs a table lookup and returns a pointer to a read-only string.
 *  @param uid UID string for which the name is to be looked up
 *  @param defaultValue default to return if UID not known
 *  @return name string or defaultValue if UID is unknown
 */
DCMTK_DCMDATA_EXPORT const char* dcmFindNameOfUID(const char* uid, const char* defaultValue = NULL);

/** return the UID of a name.
 *  Performs a table lookup and returns a pointer to a read-only string.
 *  @param name name string for which the corresponding UID is to be looked up
 *  @return UID string or NULL if name is unknown
 */
DCMTK_DCMDATA_EXPORT const char* dcmFindUIDFromName(const char* name);

/** an array of const strings containing all known Storage SOP Classes
 *  that fit into the conventional PATIENT-STUDY-SERIES-INSTANCE information
 *  model, i.e. everything a Storage SCP might want to store in a PACS.
 *  Special cases such as Hanging Protocol or Color Palette Storage SOP Class
 *  are not included in this list.
 *  WARNING: This list contains more than 64 entries, i.e. it is not possible
 *  to use this list to configure the association negotiation behavior of
 *  a Storage SCU that always proposes two presentation contexts for each
 *  SOP class.
 */
extern DCMTK_DCMDATA_EXPORT const char* dcmAllStorageSOPClassUIDs[];

/// number of entries in dcmAllStorageSOPClassUIDs.
extern DCMTK_DCMDATA_EXPORT const int numberOfDcmAllStorageSOPClassUIDs;

/** an array of const strings containing all storage SOP classes that
 *  are proposed by default by those Storage SCU components in DCMTK
 *  that always propose one presentation context for each SOP class,
 *  e.g. movescu or dcmqrdb. This list is guaranteed to have at most
 *  120 entries (to leave room for FIND/MOVE presentation contexts).
 */
extern DCMTK_DCMDATA_EXPORT const char* dcmLongSCUStorageSOPClassUIDs[];

/// number of entries in dcmLongSCUStorageSOPClassUIDs.
extern DCMTK_DCMDATA_EXPORT const int numberOfDcmLongSCUStorageSOPClassUIDs;

/** an array of const strings containing all storage SOP classes that
 *  are proposed by default by those Storage SCU components in DCMTK
 *  that always propose TWO presentation context for each SOP class,
 *  e.g. storescu. This list is guaranteed to have at most
 *  64 entries.
 */
extern DCMTK_DCMDATA_EXPORT const char* dcmShortSCUStorageSOPClassUIDs[];

/// number of entries in dcmShortSCUStorageSOPClassUIDs.
extern DCMTK_DCMDATA_EXPORT const int numberOfDcmShortSCUStorageSOPClassUIDs;

/** returns true if the uid is one of the Storage SOP Classes.
 *  Performs a table lookup in the dcmAllStorageSOPClassUIDs, dcmImageSOPClassUIDs
 *  and/or other tables depending on the requested type of Storage SOP Class.
 *  Please note that, by default, this function only covers those Storage SOP
 *  Classes that fit into the conventional PATIENT-STUDY-SERIES-INSTANCE
 *  information model, i.e. non-patient DICOM objects are missing. This can
 *  be changed by setting the optional type parameter to ESSC_NonPatient.
 *  @param uid UID string
 *  @param type type of Storage SOP Class (default: patient objects only)
 *  @return true if UID is a known Storage SOP Class, false otherwise
 */
DCMTK_DCMDATA_EXPORT OFBool dcmIsaStorageSOPClassUID(const char* uid, const E_StorageSOPClassType type = ESSC_Patient);

/** a global constant array of
 *  string pointers containing the UIDs of all known Image SOP
 *  Classes.  The global variable numberOfDcmImageSOPClassUIDs
 *  defines the size of the array.
 *  NOTE: this list represents a subset of the dcmAllStorageSOPClassUIDs list
 */
extern DCMTK_DCMDATA_EXPORT const char* dcmImageSOPClassUIDs[];

/// number of entries in dcmImageSOPClassUIDs
extern DCMTK_DCMDATA_EXPORT const int numberOfDcmImageSOPClassUIDs;

/** returns true if the uid is one of the Image Storage SOP Classes.
 *  This is just a shortcut for dcmIsaStorageSOPClassUID(uid, ESSC_Image).
 *  @param uid UID string
 *  @return true if UID is a known Image Storage SOP Class, false otherwise
 */
DCMTK_DCMDATA_EXPORT OFBool dcmIsImageStorageSOPClassUID(const char* uid);

/** creates a Unique Identifier in uid and returns uid.
 *  uid must be at least 65 bytes. Care is taken to make sure
 *  that the generated UID is 64 characters or less.
 *  If a prefix string is not passed as the second argument a
 *  default of SITE_INSTANCE_UID_ROOT (see below) will be used.
 *  Otherwise the supplied prefix string will appear at the beginning
 *  of uid.
 *  The UID is created by appending to the prefix the following:
 *  - the host ID (if obtainable, zero otherwise),
 *  - the process ID (if obtainable, zero otherwise),
 *  - the system calendar time, and
 *  - an accumulating counter for this process.
 *  @param uid pointer to buffer of 65 or more characters in which the UID is returned
 *  @param prefix prefix for UID creation
 *  @return pointer to UID, identical to uid parameter
 */
DCMTK_DCMDATA_EXPORT char *dcmGenerateUniqueIdentifier(char *uid, const char* prefix=NULL);

/** performs a table lookup and returns a short modality identifier
 *  that can be used for building file names etc.
 *  Identifiers are defined for all storage SOP classes.
 *  Returns 'defaultValue' if no modality identifier found or sopClassUID==NULL.
 *  @param sopClassUID UID string
 *  @param defaultValue default to return if UID not known
 *  @return modality string for modality UID, or defaultValue if not found
 */
DCMTK_DCMDATA_EXPORT const char *dcmSOPClassUIDToModality(const char *sopClassUID, const char *defaultValue = NULL);

/** performs a table lookup and returns a guessed average
 *  file size for the given SOP class.
 *  Average sizes are defined for all storage SOP classes, but may be very far off.
 *  @param sopClassUID UID string
 *  @return estimated average size for objects of this SOP class
 */
DCMTK_DCMDATA_EXPORT unsigned long dcmGuessModalityBytes(const char *sopClassUID);

/*
** String Constants
*/

/*
** OFFIS UID is: 1.2.276.0.7230010
** UID root for OFFIS DCMTK project:   1.2.276.0.7230010.3
**          for OFFIS GO-Kard project: 1.2.276.0.7230010.8
*/

#ifndef PACKAGE_DATE
#error Required compiler definition PACKAGE_DATE undefined
#endif
#ifndef PACKAGE_VERSION
#error Required compiler definition PACKAGE_VERSION undefined
#endif
#ifndef PACKAGE_VERSION_NUMBER
#error Required compiler definition PACKAGE_VERSION_NUMBER undefined
#endif

// helper macros for version number conversion

#define PACKAGE_VERSION_NUMBER_TO_STRING_( x ) #x
#define PACKAGE_VERSION_NUMBER_TO_STRING( x ) PACKAGE_VERSION_NUMBER_TO_STRING_( x )

/* NOTE: Implementation version name VR=SH may not be longer than 16 chars
 *       The second name is used to identify files written without dcmdata
 *       (i.e. using the --bit-preserving switch in various tools)
 */

/// implementation version name for this version of the toolkit
#define OFFIS_DTK_IMPLEMENTATION_VERSION_NAME   "OFFIS_DCMTK_" PACKAGE_VERSION_NUMBER_TO_STRING(PACKAGE_VERSION_NUMBER)

/// implementation version name for this version of the toolkit, used for files received in "bit preserving" mode
#define OFFIS_DTK_IMPLEMENTATION_VERSION_NAME2  "OFFIS_DCMBP_" PACKAGE_VERSION_NUMBER_TO_STRING(PACKAGE_VERSION_NUMBER)

/// release date of current toolkit release
#ifdef DCMTK_BUILD_DATE
#define OFFIS_DCMTK_RELEASEDATE                 DCMTK_BUILD_DATE
#else
#define OFFIS_DCMTK_RELEASEDATE                 PACKAGE_DATE
#endif

/// UID root for DCMTK, registered for OFFIS with DIN in Germany
#define OFFIS_UID_ROOT                          "1.2.276.0.7230010.3"

/// DCMTK version number for this release
#define OFFIS_DCMTK_VERSION_NUMBER              PACKAGE_VERSION_NUMBER

/// DCMTK version number (as string) for this release
#define OFFIS_DCMTK_VERSION_STRING              PACKAGE_VERSION

/// DCMTK version number suffix string for this release
#define OFFIS_DCMTK_VERSION_SUFFIX              PACKAGE_VERSION_SUFFIX

/// DCMTK version number string including suffix
#define OFFIS_DCMTK_VERSION                     OFFIS_DCMTK_VERSION_STRING OFFIS_DCMTK_VERSION_SUFFIX

/// Implementation class UID for this release of the toolkit
#define OFFIS_IMPLEMENTATION_CLASS_UID          OFFIS_UID_ROOT ".0." OFFIS_DCMTK_VERSION_STRING

/// Instance creator UID for this release of the toolkit
#define OFFIS_INSTANCE_CREATOR_UID              OFFIS_IMPLEMENTATION_CLASS_UID

/// private coding scheme UID root for coding schemes generated by OFFIS
#define OFFIS_CODING_SCHEME_UID_ROOT            OFFIS_UID_ROOT ".0.0"

/// private coding scheme version for coding schemes generated by OFFIS
#define OFFIS_CODING_SCHEME_VERSION             "1"

/// private coding scheme UID for coding schemes generated by OFFIS
#define OFFIS_CODING_SCHEME_UID                 OFFIS_CODING_SCHEME_UID_ROOT "." OFFIS_CODING_SCHEME_VERSION

/*
** Each site should define its own SITE_UID_ROOT
*/
#ifndef SITE_UID_ROOT
/// UID root to be used when generating UIDs. By default uses the DCMTK root, but can be replaced at compile time.
#define SITE_UID_ROOT                           OFFIS_UID_ROOT  /* default */
#endif

/*
** Useful UID prefixes. These can be whatever you want.
**
** These site UIDs are arbitrary, non-standard, with no meaning
** and can be changed at any time.  Do _not_ rely on these values.
** Do _not_ assume any semantics when using these suffixes.
**
*/

/// UID root for study instance UIDs
#define SITE_STUDY_UID_ROOT                     SITE_UID_ROOT ".1.2"

/// UID root for series instance UIDs
#define SITE_SERIES_UID_ROOT                    SITE_UID_ROOT ".1.3"

/// UID root for SOP instance UIDs
#define SITE_INSTANCE_UID_ROOT                  SITE_UID_ROOT ".1.4"

/** A private SOP Class UID which is used in a file meta-header when no
 *  instance of a Storage SOP Class is stored in the file. -- NON-STANDARD
 */
#define UID_PrivateGenericFileSOPClass          SITE_UID_ROOT ".1.0.1"

/// DICOM Defined Standard Application Context UID
#define UID_StandardApplicationContext          "1.2.840.10008.3.1.1.1"

/*
** Defined Transfer Syntax UIDs
*/

/// Implicit VR Little Endian: Default Transfer Syntax for DICOM
#define UID_LittleEndianImplicitTransferSyntax  "1.2.840.10008.1.2"
/// Explicit VR Little Endian
#define UID_LittleEndianExplicitTransferSyntax  "1.2.840.10008.1.2.1"
/// Explicit VR Big Endian - RETIRED
#define UID_BigEndianExplicitTransferSyntax     "1.2.840.10008.1.2.2"
/// Deflated Explicit VR Little Endian
#define UID_DeflatedExplicitVRLittleEndianTransferSyntax "1.2.840.10008.1.2.1.99"
/** JPEG Baseline (Process 1): Default Transfer Syntax
 *  for Lossy JPEG 8 Bit Image Compression
 */
#define UID_JPEGProcess1TransferSyntax          "1.2.840.10008.1.2.4.50"
/** JPEG Extended (Process 2 & 4): Default Transfer Syntax
 *  for Lossy JPEG 12 Bit Image Compression (Process 4 only)
 */
#define UID_JPEGProcess2_4TransferSyntax        "1.2.840.10008.1.2.4.51"
/// JPEG Extended (Process 3 & 5) - RETIRED
#define UID_JPEGProcess3_5TransferSyntax        "1.2.840.10008.1.2.4.52"
/// JPEG Spectral Selection, Non-Hierarchical (Process 6 & 8) - RETIRED
#define UID_JPEGProcess6_8TransferSyntax        "1.2.840.10008.1.2.4.53"
/// JPEG Spectral Selection, Non-Hierarchical (Process 7 & 9) - RETIRED
#define UID_JPEGProcess7_9TransferSyntax        "1.2.840.10008.1.2.4.54"
/// JPEG Full Progression, Non-Hierarchical (Process 10 & 12) - RETIRED
#define UID_JPEGProcess10_12TransferSyntax      "1.2.840.10008.1.2.4.55"
/// JPEG Full Progression, Non-Hierarchical (Process 11 & 13) - RETIRED
#define UID_JPEGProcess11_13TransferSyntax      "1.2.840.10008.1.2.4.56"
/// JPEG Lossless, Non-Hierarchical (Process 14)
#define UID_JPEGProcess14TransferSyntax         "1.2.840.10008.1.2.4.57"
/// JPEG Lossless, Non-Hierarchical (Process 15) - RETIRED
#define UID_JPEGProcess15TransferSyntax         "1.2.840.10008.1.2.4.58"
/// JPEG Extended, Hierarchical (Process 16 & 18) - RETIRED
#define UID_JPEGProcess16_18TransferSyntax      "1.2.840.10008.1.2.4.59"
/// JPEG Extended, Hierarchical (Process 17 & 19) - RETIRED
#define UID_JPEGProcess17_19TransferSyntax      "1.2.840.10008.1.2.4.60"
/// JPEG Spectral Selection, Hierarchical (Process 20 & 22) - RETIRED
#define UID_JPEGProcess20_22TransferSyntax      "1.2.840.10008.1.2.4.61"
/// JPEG Spectral Selection, Hierarchical (Process 21 & 23) - RETIRED
#define UID_JPEGProcess21_23TransferSyntax      "1.2.840.10008.1.2.4.62"
/// JPEG Full Progression, Hierarchical (Process 24 & 26) - RETIRED
#define UID_JPEGProcess24_26TransferSyntax      "1.2.840.10008.1.2.4.63"
/// JPEG Full Progression, Hierarchical (Process 25 & 27) - RETIRED
#define UID_JPEGProcess25_27TransferSyntax      "1.2.840.10008.1.2.4.64"
/// JPEG Lossless, Hierarchical (Process 28) - RETIRED
#define UID_JPEGProcess28TransferSyntax         "1.2.840.10008.1.2.4.65"
/// JPEG Lossless, Hierarchical (Process 29) - RETIRED
#define UID_JPEGProcess29TransferSyntax         "1.2.840.10008.1.2.4.66"
/** JPEG Lossless, Non-Hierarchical, First-Order Prediction (Process 14
 *  [Selection Value 1]): Default Transfer Syntax for Lossless JPEG Image Compression
 */
#define UID_JPEGProcess14SV1TransferSyntax      "1.2.840.10008.1.2.4.70"
/// JPEG-LS Lossless Image Compression
#define UID_JPEGLSLosslessTransferSyntax        "1.2.840.10008.1.2.4.80"
/// JPEG-LS Lossy (Near-Lossless) Image Compression
#define UID_JPEGLSLossyTransferSyntax           "1.2.840.10008.1.2.4.81"
/// JPEG 2000 Image Compression (Lossless Only)
#define UID_JPEG2000LosslessOnlyTransferSyntax  "1.2.840.10008.1.2.4.90"
/// JPEG 2000 Image Compression (Lossless or Lossy)
#define UID_JPEG2000TransferSyntax              "1.2.840.10008.1.2.4.91"
/// JPEG 2000 Part 2 Multi-component Image Compression (Lossless Only)
#define UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnlyTransferSyntax "1.2.840.10008.1.2.4.92"
/// JPEG 2000 Part 2 Multi-component Image Compression (Lossless or Lossy)
#define UID_JPEG2000Part2MulticomponentImageCompressionTransferSyntax "1.2.840.10008.1.2.4.93"
/// JPIP Referenced
#define UID_JPIPReferencedTransferSyntax        "1.2.840.10008.1.2.4.94"
/// JPIP Referenced Deflate
#define UID_JPIPReferencedDeflateTransferSyntax "1.2.840.10008.1.2.4.95"
/// MPEG2 Main Profile @ Main Level (changed with DICOM 2016e to: MPEG2 Main Profile / Main Level)
#define UID_MPEG2MainProfileAtMainLevelTransferSyntax "1.2.840.10008.1.2.4.100"
/// MPEG2 Main Profile @ High Level (changed with DICOM 2016e to: MPEG2 Main Profile / High Level)
#define UID_MPEG2MainProfileAtHighLevelTransferSyntax "1.2.840.10008.1.2.4.101"
/// MPEG-4 AVC/H.264 High Profile / Level 4.1
#define UID_MPEG4HighProfileLevel4_1TransferSyntax "1.2.840.10008.1.2.4.102"
/// MPEG-4 AVC/H.264 BD-compatible High Profile / Level 4.1
#define UID_MPEG4BDcompatibleHighProfileLevel4_1TransferSyntax "1.2.840.10008.1.2.4.103"
/// MPEG-4 AVC/H.264 High Profile / Level 4.2 For 2D Video
#define UID_MPEG4HighProfileLevel4_2_For2DVideoTransferSyntax "1.2.840.10008.1.2.4.104"
/// MPEG-4 AVC/H.264 High Profile / Level 4.2 For 3D Video
#define UID_MPEG4HighProfileLevel4_2_For3DVideoTransferSyntax "1.2.840.10008.1.2.4.105"
/// MPEG-4 AVC/H.264 Stereo High Profile / Level 4.2
#define UID_MPEG4StereoHighProfileLevel4_2TransferSyntax "1.2.840.10008.1.2.4.106"
/// HEVC/H.265 Main Profile / Level 5.1
#define UID_HEVCMainProfileLevel5_1TransferSyntax "1.2.840.10008.1.2.4.107"
/// HEVC/H.265 Main 10 Profile / Level 5.1
#define UID_HEVCMain10ProfileLevel5_1TransferSyntax "1.2.840.10008.1.2.4.108"
/// RLE Lossless
#define UID_RLELosslessTransferSyntax           "1.2.840.10008.1.2.5"

/** RFC 2557 MIME Encapsulation (RETIRED) was only a pseudo transfer syntax used
 *  to refer to MIME encapsulated HL7 CDA documents from a DICOMDIR when stored
 *  on a DICOM storage medium. It was never used for network communication
 *  or encoding of DICOM objects.
 */
#define UID_RFC2557MIMEEncapsulationTransferSyntax "1.2.840.10008.1.2.6.1"

/** XML Encoding (RETIRED) was only a pseudo transfer syntax used to refer to
 *  encapsulated HL7 CDA documents from a DICOMDIR when stored on a DICOM storage
 *  medium. It was never used for network communication or encoding of DICOM objects.
 */
#define UID_XMLEncodingTransferSyntax "1.2.840.10008.1.2.6.2"

/** Private transfer syntax defined by GE. This transfer syntax is identical to
 *  Implicit VR Little Endian, except that Pixel Data are encoded in big endian.
 */
#define UID_PrivateGE_LEI_WithBigEndianPixelDataTransferSyntax "1.2.840.113619.5.2"

/*
** Defined SOP Class UIDs according to DICOM standard
*/

// Storage (DICOM)
#define UID_RETIRED_StoredPrintStorage                             "1.2.840.10008.5.1.1.27"
#define UID_RETIRED_HardcopyGrayscaleImageStorage                  "1.2.840.10008.5.1.1.29"
#define UID_RETIRED_HardcopyColorImageStorage                      "1.2.840.10008.5.1.1.30"
#define UID_ComputedRadiographyImageStorage                        "1.2.840.10008.5.1.4.1.1.1"
#define UID_DigitalXRayImageStorageForPresentation                 "1.2.840.10008.5.1.4.1.1.1.1"
#define UID_DigitalXRayImageStorageForProcessing                   "1.2.840.10008.5.1.4.1.1.1.1.1"
#define UID_DigitalMammographyXRayImageStorageForPresentation      "1.2.840.10008.5.1.4.1.1.1.2"
#define UID_DigitalMammographyXRayImageStorageForProcessing        "1.2.840.10008.5.1.4.1.1.1.2.1"
#define UID_DigitalIntraOralXRayImageStorageForPresentation        "1.2.840.10008.5.1.4.1.1.1.3"
#define UID_DigitalIntraOralXRayImageStorageForProcessing          "1.2.840.10008.5.1.4.1.1.1.3.1"
#define UID_CTImageStorage                                         "1.2.840.10008.5.1.4.1.1.2"
#define UID_EnhancedCTImageStorage                                 "1.2.840.10008.5.1.4.1.1.2.1"
#define UID_LegacyConvertedEnhancedCTImageStorage                  "1.2.840.10008.5.1.4.1.1.2.2"
#define UID_RETIRED_UltrasoundMultiframeImageStorage               "1.2.840.10008.5.1.4.1.1.3"
#define UID_UltrasoundMultiframeImageStorage                       "1.2.840.10008.5.1.4.1.1.3.1"
#define UID_MRImageStorage                                         "1.2.840.10008.5.1.4.1.1.4"
#define UID_EnhancedMRImageStorage                                 "1.2.840.10008.5.1.4.1.1.4.1"
#define UID_MRSpectroscopyStorage                                  "1.2.840.10008.5.1.4.1.1.4.2"
#define UID_EnhancedMRColorImageStorage                            "1.2.840.10008.5.1.4.1.1.4.3"
#define UID_LegacyConvertedEnhancedMRImageStorage                  "1.2.840.10008.5.1.4.1.1.4.4"
#define UID_RETIRED_NuclearMedicineImageStorage                    "1.2.840.10008.5.1.4.1.1.5"
#define UID_RETIRED_UltrasoundImageStorage                         "1.2.840.10008.5.1.4.1.1.6"
#define UID_UltrasoundImageStorage                                 "1.2.840.10008.5.1.4.1.1.6.1"
#define UID_EnhancedUSVolumeStorage                                "1.2.840.10008.5.1.4.1.1.6.2"
#define UID_SecondaryCaptureImageStorage                           "1.2.840.10008.5.1.4.1.1.7"
#define UID_MultiframeSingleBitSecondaryCaptureImageStorage        "1.2.840.10008.5.1.4.1.1.7.1"
#define UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage    "1.2.840.10008.5.1.4.1.1.7.2"
#define UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage    "1.2.840.10008.5.1.4.1.1.7.3"
#define UID_MultiframeTrueColorSecondaryCaptureImageStorage        "1.2.840.10008.5.1.4.1.1.7.4"
#define UID_RETIRED_StandaloneOverlayStorage                       "1.2.840.10008.5.1.4.1.1.8"
#define UID_RETIRED_StandaloneCurveStorage                         "1.2.840.10008.5.1.4.1.1.9"
#define UID_TwelveLeadECGWaveformStorage                           "1.2.840.10008.5.1.4.1.1.9.1.1"
#define UID_GeneralECGWaveformStorage                              "1.2.840.10008.5.1.4.1.1.9.1.2"
#define UID_AmbulatoryECGWaveformStorage                           "1.2.840.10008.5.1.4.1.1.9.1.3"
#define UID_HemodynamicWaveformStorage                             "1.2.840.10008.5.1.4.1.1.9.2.1"
#define UID_CardiacElectrophysiologyWaveformStorage                "1.2.840.10008.5.1.4.1.1.9.3.1"
#define UID_BasicVoiceAudioWaveformStorage                         "1.2.840.10008.5.1.4.1.1.9.4.1"
#define UID_GeneralAudioWaveformStorage                            "1.2.840.10008.5.1.4.1.1.9.4.2"
#define UID_ArterialPulseWaveformStorage                           "1.2.840.10008.5.1.4.1.1.9.5.1"
#define UID_RespiratoryWaveformStorage                             "1.2.840.10008.5.1.4.1.1.9.6.1"
#define UID_RETIRED_StandaloneModalityLUTStorage                   "1.2.840.10008.5.1.4.1.1.10"
#define UID_RETIRED_StandaloneVOILUTStorage                        "1.2.840.10008.5.1.4.1.1.11"
#define UID_GrayscaleSoftcopyPresentationStateStorage              "1.2.840.10008.5.1.4.1.1.11.1"
#define UID_ColorSoftcopyPresentationStateStorage                  "1.2.840.10008.5.1.4.1.1.11.2"
#define UID_PseudoColorSoftcopyPresentationStateStorage            "1.2.840.10008.5.1.4.1.1.11.3"
#define UID_BlendingSoftcopyPresentationStateStorage               "1.2.840.10008.5.1.4.1.1.11.4"
#define UID_XAXRFGrayscaleSoftcopyPresentationStateStorage         "1.2.840.10008.5.1.4.1.1.11.5"
#define UID_GrayscalePlanarMPRVolumetricPresentationStateStorage   "1.2.840.10008.5.1.4.1.1.11.6"
#define UID_CompositingPlanarMPRVolumetricPresentationStateStorage "1.2.840.10008.5.1.4.1.1.11.7"
#define UID_AdvancedBlendingPresentationStateStorage               "1.2.840.10008.5.1.4.1.1.11.8"
#define UID_VolumeRenderingVolumetricPresentationStateStorage      "1.2.840.10008.5.1.4.1.1.11.9"
#define UID_SegmentedVolumeRenderingVolumetricPresentationStateStorage "1.2.840.10008.5.1.4.1.1.11.10"
#define UID_MultipleVolumeRenderingVolumetricPresentationStateStorage "1.2.840.10008.5.1.4.1.1.11.11"
#define UID_XRayAngiographicImageStorage                           "1.2.840.10008.5.1.4.1.1.12.1"
#define UID_EnhancedXAImageStorage                                 "1.2.840.10008.5.1.4.1.1.12.1.1"
#define UID_XRayRadiofluoroscopicImageStorage                      "1.2.840.10008.5.1.4.1.1.12.2"
#define UID_EnhancedXRFImageStorage                                "1.2.840.10008.5.1.4.1.1.12.2.1"
#define UID_RETIRED_XRayAngiographicBiPlaneImageStorage            "1.2.840.10008.5.1.4.1.1.12.3"
#define UID_XRay3DAngiographicImageStorage                         "1.2.840.10008.5.1.4.1.1.13.1.1"
#define UID_XRay3DCraniofacialImageStorage                         "1.2.840.10008.5.1.4.1.1.13.1.2"
#define UID_BreastTomosynthesisImageStorage                        "1.2.840.10008.5.1.4.1.1.13.1.3"
#define UID_BreastProjectionXRayImageStorageForPresentation        "1.2.840.10008.5.1.4.1.1.13.1.4"
#define UID_BreastProjectionXRayImageStorageForProcessing          "1.2.840.10008.5.1.4.1.1.13.1.5"
#define UID_IntravascularOpticalCoherenceTomographyImageStorageForPresentation "1.2.840.10008.5.1.4.1.1.14.1"
#define UID_IntravascularOpticalCoherenceTomographyImageStorageForProcessing "1.2.840.10008.5.1.4.1.1.14.2"
#define UID_NuclearMedicineImageStorage                            "1.2.840.10008.5.1.4.1.1.20"
#define UID_ParametricMapStorage                                   "1.2.840.10008.5.1.4.1.1.30"
#define UID_RawDataStorage                                         "1.2.840.10008.5.1.4.1.1.66"
#define UID_SpatialRegistrationStorage                             "1.2.840.10008.5.1.4.1.1.66.1"
#define UID_SpatialFiducialsStorage                                "1.2.840.10008.5.1.4.1.1.66.2"
#define UID_DeformableSpatialRegistrationStorage                   "1.2.840.10008.5.1.4.1.1.66.3"
#define UID_SegmentationStorage                                    "1.2.840.10008.5.1.4.1.1.66.4"
#define UID_SurfaceSegmentationStorage                             "1.2.840.10008.5.1.4.1.1.66.5"
#define UID_TractographyResultsStorage                             "1.2.840.10008.5.1.4.1.1.66.6"
#define UID_RealWorldValueMappingStorage                           "1.2.840.10008.5.1.4.1.1.67"
#define UID_SurfaceScanMeshStorage                                 "1.2.840.10008.5.1.4.1.1.68.1"
#define UID_SurfaceScanPointCloudStorage                           "1.2.840.10008.5.1.4.1.1.68.2"
#define UID_RETIRED_VLImageStorage                                 "1.2.840.10008.5.1.4.1.1.77.1"
#define UID_VLEndoscopicImageStorage                               "1.2.840.10008.5.1.4.1.1.77.1.1"
#define UID_VideoEndoscopicImageStorage                            "1.2.840.10008.5.1.4.1.1.77.1.1.1"
#define UID_VLMicroscopicImageStorage                              "1.2.840.10008.5.1.4.1.1.77.1.2"
#define UID_VideoMicroscopicImageStorage                           "1.2.840.10008.5.1.4.1.1.77.1.2.1"
#define UID_VLSlideCoordinatesMicroscopicImageStorage              "1.2.840.10008.5.1.4.1.1.77.1.3"
#define UID_VLPhotographicImageStorage                             "1.2.840.10008.5.1.4.1.1.77.1.4"
#define UID_VideoPhotographicImageStorage                          "1.2.840.10008.5.1.4.1.1.77.1.4.1"
#define UID_OphthalmicPhotography8BitImageStorage                  "1.2.840.10008.5.1.4.1.1.77.1.5.1"
#define UID_OphthalmicPhotography16BitImageStorage                 "1.2.840.10008.5.1.4.1.1.77.1.5.2"
#define UID_StereometricRelationshipStorage                        "1.2.840.10008.5.1.4.1.1.77.1.5.3"
#define UID_OphthalmicTomographyImageStorage                       "1.2.840.10008.5.1.4.1.1.77.1.5.4"
#define UID_WideFieldOphthalmicPhotographyStereographicProjectionImageStorage "1.2.840.10008.5.1.4.1.1.77.1.5.5"
#define UID_WideFieldOphthalmicPhotography3DCoordinatesImageStorage "1.2.840.10008.5.1.4.1.1.77.1.5.6"
#define UID_OphthalmicOpticalCoherenceTomographyEnFaceImageStorage "1.2.840.10008.5.1.4.1.1.77.1.5.7"
#define UID_OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage "1.2.840.10008.5.1.4.1.1.77.1.5.8"
#define UID_VLWholeSlideMicroscopyImageStorage                     "1.2.840.10008.5.1.4.1.1.77.1.6"
#define UID_RETIRED_VLMultiframeImageStorage                       "1.2.840.10008.5.1.4.1.1.77.2"
#define UID_LensometryMeasurementsStorage                          "1.2.840.10008.5.1.4.1.1.78.1"
#define UID_AutorefractionMeasurementsStorage                      "1.2.840.10008.5.1.4.1.1.78.2"
#define UID_KeratometryMeasurementsStorage                         "1.2.840.10008.5.1.4.1.1.78.3"
#define UID_SubjectiveRefractionMeasurementsStorage                "1.2.840.10008.5.1.4.1.1.78.4"
#define UID_VisualAcuityMeasurementsStorage                        "1.2.840.10008.5.1.4.1.1.78.5"
#define UID_SpectaclePrescriptionReportStorage                     "1.2.840.10008.5.1.4.1.1.78.6"
#define UID_OphthalmicAxialMeasurementsStorage                     "1.2.840.10008.5.1.4.1.1.78.7"
#define UID_IntraocularLensCalculationsStorage                     "1.2.840.10008.5.1.4.1.1.78.8"
#define UID_MacularGridThicknessAndVolumeReportStorage             "1.2.840.10008.5.1.4.1.1.79.1"
#define UID_OphthalmicVisualFieldStaticPerimetryMeasurementsStorage "1.2.840.10008.5.1.4.1.1.80.1"
#define UID_OphthalmicThicknessMapStorage                          "1.2.840.10008.5.1.4.1.1.81.1"
#define UID_CornealTopographyMapStorage                            "1.2.840.10008.5.1.4.1.1.82.1"
#define UID_BasicTextSRStorage                                     "1.2.840.10008.5.1.4.1.1.88.11"
#define UID_EnhancedSRStorage                                      "1.2.840.10008.5.1.4.1.1.88.22"
#define UID_ComprehensiveSRStorage                                 "1.2.840.10008.5.1.4.1.1.88.33"
#define UID_Comprehensive3DSRStorage                               "1.2.840.10008.5.1.4.1.1.88.34"
#define UID_ExtensibleSRStorage                                    "1.2.840.10008.5.1.4.1.1.88.35"
#define UID_ProcedureLogStorage                                    "1.2.840.10008.5.1.4.1.1.88.40"
#define UID_MammographyCADSRStorage                                "1.2.840.10008.5.1.4.1.1.88.50"
#define UID_KeyObjectSelectionDocumentStorage                      "1.2.840.10008.5.1.4.1.1.88.59"
#define UID_ChestCADSRStorage                                      "1.2.840.10008.5.1.4.1.1.88.65"
#define UID_XRayRadiationDoseSRStorage                             "1.2.840.10008.5.1.4.1.1.88.67"
#define UID_RadiopharmaceuticalRadiationDoseSRStorage              "1.2.840.10008.5.1.4.1.1.88.68"
#define UID_ColonCADSRStorage                                      "1.2.840.10008.5.1.4.1.1.88.69"
#define UID_ImplantationPlanSRDocumentStorage                      "1.2.840.10008.5.1.4.1.1.88.70"
#define UID_AcquisitionContextSRStorage                            "1.2.840.10008.5.1.4.1.1.88.71"
#define UID_SimplifiedAdultEchoSRStorage                           "1.2.840.10008.5.1.4.1.1.88.72"
#define UID_PatientRadiationDoseSRStorage                          "1.2.840.10008.5.1.4.1.1.88.73"
#define UID_PlannedImagingAgentAdministrationSRStorage             "1.2.840.10008.5.1.4.1.1.88.74"
#define UID_PerformedImagingAgentAdministrationSRStorage           "1.2.840.10008.5.1.4.1.1.88.75"
#define UID_ContentAssessmentResultsStorage                        "1.2.840.10008.5.1.4.1.1.90.1"
#define UID_EncapsulatedPDFStorage                                 "1.2.840.10008.5.1.4.1.1.104.1"
#define UID_EncapsulatedCDAStorage                                 "1.2.840.10008.5.1.4.1.1.104.2"
#define UID_EncapsulatedSTLStorage                                 "1.2.840.10008.5.1.4.1.1.104.3"
#define UID_PositronEmissionTomographyImageStorage                 "1.2.840.10008.5.1.4.1.1.128"
#define UID_LegacyConvertedEnhancedPETImageStorage                 "1.2.840.10008.5.1.4.1.1.128.1"
#define UID_RETIRED_StandalonePETCurveStorage                      "1.2.840.10008.5.1.4.1.1.129"
#define UID_EnhancedPETImageStorage                                "1.2.840.10008.5.1.4.1.1.130"
#define UID_BasicStructuredDisplayStorage                          "1.2.840.10008.5.1.4.1.1.131"
#define UID_CTDefinedProcedureProtocolStorage                      "1.2.840.10008.5.1.4.1.1.200.1"
#define UID_CTPerformedProcedureProtocolStorage                    "1.2.840.10008.5.1.4.1.1.200.2"
#define UID_ProtocolApprovalStorage                                "1.2.840.10008.5.1.4.1.1.200.3"
#define UID_RTImageStorage                                         "1.2.840.10008.5.1.4.1.1.481.1"
#define UID_RTDoseStorage                                          "1.2.840.10008.5.1.4.1.1.481.2"
#define UID_RTStructureSetStorage                                  "1.2.840.10008.5.1.4.1.1.481.3"
#define UID_RTBeamsTreatmentRecordStorage                          "1.2.840.10008.5.1.4.1.1.481.4"
#define UID_RTPlanStorage                                          "1.2.840.10008.5.1.4.1.1.481.5"
#define UID_RTBrachyTreatmentRecordStorage                         "1.2.840.10008.5.1.4.1.1.481.6"
#define UID_RTTreatmentSummaryRecordStorage                        "1.2.840.10008.5.1.4.1.1.481.7"
#define UID_RTIonPlanStorage                                       "1.2.840.10008.5.1.4.1.1.481.8"
#define UID_RTIonBeamsTreatmentRecordStorage                       "1.2.840.10008.5.1.4.1.1.481.9"
#define UID_RTPhysicianIntentStorage                               "1.2.840.10008.5.1.4.1.1.481.10"
#define UID_RTSegmentAnnotationStorage                             "1.2.840.10008.5.1.4.1.1.481.11"
#define UID_RTBeamsDeliveryInstructionStorage                      "1.2.840.10008.5.1.4.34.7"
#define UID_RTBrachyApplicationSetupDeliveryInstructionStorage     "1.2.840.10008.5.1.4.34.10"
#define UID_HangingProtocolStorage                                 "1.2.840.10008.5.1.4.38.1"
#define UID_GenericImplantTemplateStorage                          "1.2.840.10008.5.1.4.43.1"
#define UID_ImplantAssemblyTemplateStorage                         "1.2.840.10008.5.1.4.44.1"
#define UID_ImplantTemplateGroupStorage                            "1.2.840.10008.5.1.4.45.1"

// DICOMDIR (was UID_BasicDirectoryStorageSOPClass in DCMTK versions prior to 3.5.3)
#define UID_MediaStorageDirectoryStorage                           "1.2.840.10008.1.3.10"

// DICOS Storage
#define UID_DICOS_CTImageStorage                                   "1.2.840.10008.5.1.4.1.1.501.1"
#define UID_DICOS_DigitalXRayImageStorageForPresentation           "1.2.840.10008.5.1.4.1.1.501.2.1"
#define UID_DICOS_DigitalXRayImageStorageForProcessing             "1.2.840.10008.5.1.4.1.1.501.2.2"
#define UID_DICOS_ThreatDetectionReportStorage                     "1.2.840.10008.5.1.4.1.1.501.3"
#define UID_DICOS_2DAITStorage                                     "1.2.840.10008.5.1.4.1.1.501.4"
#define UID_DICOS_3DAITStorage                                     "1.2.840.10008.5.1.4.1.1.501.5"
#define UID_DICOS_QuadrupoleResonanceStorage                       "1.2.840.10008.5.1.4.1.1.501.6"

// DICONDE Storage
#define UID_DICONDE_EddyCurrentImageStorage                        "1.2.840.10008.5.1.4.1.1.601.1"
#define UID_DICONDE_EddyCurrentMultiframeImageStorage              "1.2.840.10008.5.1.4.1.1.601.2"

// Query/Retrieve
#define UID_FINDPatientRootQueryRetrieveInformationModel           "1.2.840.10008.5.1.4.1.2.1.1"
#define UID_MOVEPatientRootQueryRetrieveInformationModel           "1.2.840.10008.5.1.4.1.2.1.2"
#define UID_GETPatientRootQueryRetrieveInformationModel            "1.2.840.10008.5.1.4.1.2.1.3"
#define UID_FINDStudyRootQueryRetrieveInformationModel             "1.2.840.10008.5.1.4.1.2.2.1"
#define UID_MOVEStudyRootQueryRetrieveInformationModel             "1.2.840.10008.5.1.4.1.2.2.2"
#define UID_GETStudyRootQueryRetrieveInformationModel              "1.2.840.10008.5.1.4.1.2.2.3"
#define UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel "1.2.840.10008.5.1.4.1.2.3.1"
#define UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel "1.2.840.10008.5.1.4.1.2.3.2"
#define UID_RETIRED_GETPatientStudyOnlyQueryRetrieveInformationModel  "1.2.840.10008.5.1.4.1.2.3.3"
#define UID_MOVECompositeInstanceRootRetrieve                      "1.2.840.10008.5.1.4.1.2.4.2"
#define UID_GETCompositeInstanceRootRetrieve                       "1.2.840.10008.5.1.4.1.2.4.3"
#define UID_GETCompositeInstanceRetrieveWithoutBulkData            "1.2.840.10008.5.1.4.1.2.5.3"

// Modality Worklist
#define UID_FINDModalityWorklistInformationModel                   "1.2.840.10008.5.1.4.31"

// General Purpose Worklist
#define UID_RETIRED_FINDGeneralPurposeWorklistInformationModel     "1.2.840.10008.5.1.4.32.1"
#define UID_RETIRED_GeneralPurposeScheduledProcedureStepSOPClass   "1.2.840.10008.5.1.4.32.2"
#define UID_RETIRED_GeneralPurposePerformedProcedureStepSOPClass   "1.2.840.10008.5.1.4.32.3"
#define UID_RETIRED_GeneralPurposeWorklistManagementMetaSOPClass   "1.2.840.10008.5.1.4.32"

// MPPS
#define UID_ModalityPerformedProcedureStepSOPClass                 "1.2.840.10008.3.1.2.3.3"
#define UID_ModalityPerformedProcedureStepRetrieveSOPClass         "1.2.840.10008.3.1.2.3.4"
#define UID_ModalityPerformedProcedureStepNotificationSOPClass     "1.2.840.10008.3.1.2.3.5"

// Radiotherapy
#define UID_RTConventionalMachineVerification                      "1.2.840.10008.5.1.4.34.8"
#define UID_RTIonMachineVerification                               "1.2.840.10008.5.1.4.34.9"

// Unified Worklist and Procedure Step
#define UID_UnifiedWorklistAndProcedureStepServiceClass            "1.2.840.10008.5.1.4.34.6"
#define UID_UnifiedProcedureStepPushSOPClass                       "1.2.840.10008.5.1.4.34.6.1"
#define UID_UnifiedProcedureStepWatchSOPClass                      "1.2.840.10008.5.1.4.34.6.2"
#define UID_UnifiedProcedureStepPullSOPClass                       "1.2.840.10008.5.1.4.34.6.3"
#define UID_UnifiedProcedureStepEventSOPClass                      "1.2.840.10008.5.1.4.34.6.4"
#define UID_UPSGlobalSubscriptionSOPInstance                       "1.2.840.10008.5.1.4.34.5"
#define UID_UPSFilteredGlobalSubscriptionSOPInstance               "1.2.840.10008.5.1.4.34.5.1"

// Storage Commitment
#define UID_StorageCommitmentPushModelSOPClass                     "1.2.840.10008.1.20.1"
#define UID_StorageCommitmentPushModelSOPInstance                  "1.2.840.10008.1.20.1.1"
#define UID_RETIRED_StorageCommitmentPullModelSOPClass             "1.2.840.10008.1.20.2"
#define UID_RETIRED_StorageCommitmentPullModelSOPInstance          "1.2.840.10008.1.20.2.1"

// Hanging Protocols
#define UID_FINDHangingProtocolInformationModel                    "1.2.840.10008.5.1.4.38.2"
#define UID_MOVEHangingProtocolInformationModel                    "1.2.840.10008.5.1.4.38.3"

// Relevant Patient Information Query
#define UID_GeneralRelevantPatientInformationQuery                 "1.2.840.10008.5.1.4.37.1"
#define UID_BreastImagingRelevantPatientInformationQuery           "1.2.840.10008.5.1.4.37.2"
#define UID_CardiacRelevantPatientInformationQuery                 "1.2.840.10008.5.1.4.37.3"

// Color Palette Storage and Query/Retrieve
#define UID_ColorPaletteStorage                                    "1.2.840.10008.5.1.4.39.1"
#define UID_FINDColorPaletteInformationModel                       "1.2.840.10008.5.1.4.39.2"
#define UID_MOVEColorPaletteInformationModel                       "1.2.840.10008.5.1.4.39.3"
#define UID_GETColorPaletteInformationModel                        "1.2.840.10008.5.1.4.39.4"

// Implant Template Query/Retrieve
#define UID_FINDGenericImplantTemplateInformationModel             "1.2.840.10008.5.1.4.43.2"
#define UID_MOVEGenericImplantTemplateInformationModel             "1.2.840.10008.5.1.4.43.3"
#define UID_GETGenericImplantTemplateInformationModel              "1.2.840.10008.5.1.4.43.4"
#define UID_FINDImplantAssemblyTemplateInformationModel            "1.2.840.10008.5.1.4.44.2"
#define UID_MOVEImplantAssemblyTemplateInformationModel            "1.2.840.10008.5.1.4.44.3"
#define UID_GETImplantAssemblyTemplateInformationModel             "1.2.840.10008.5.1.4.44.4"
#define UID_FINDImplantTemplateGroupInformationModel               "1.2.840.10008.5.1.4.45.2"
#define UID_MOVEImplantTemplateGroupInformationModel               "1.2.840.10008.5.1.4.45.3"
#define UID_GETImplantTemplateGroupInformationModel                "1.2.840.10008.5.1.4.45.4"

// Defined Procedure Protocol Query/Retrieve
#define UID_FINDDefinedProcedureProtocolInformationModel           "1.2.840.10008.5.1.4.20.1"
#define UID_MOVEDefinedProcedureProtocolInformationModel           "1.2.840.10008.5.1.4.20.2"
#define UID_GETDefinedProcedureProtocolInformationModel            "1.2.840.10008.5.1.4.20.3"

// Protocol Approval Query/Retrieve
#define UID_FINDProtocolApprovalInformationModel                   "1.2.840.10008.5.1.4.1.1.200.4"
#define UID_MOVEProtocolApprovalInformationModel                   "1.2.840.10008.5.1.4.1.1.200.5"
#define UID_GETProtocolApprovalInformationModel                    "1.2.840.10008.5.1.4.1.1.200.6"

// Print Management
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
#define UID_RETIRED_ImageOverlayBoxSOPClass                        "1.2.840.10008.5.1.1.24"
#define UID_RETIRED_BasicPrintImageOverlayBoxSOPClass              "1.2.840.10008.5.1.1.24.1"
#define UID_RETIRED_PrintQueueSOPInstance                          "1.2.840.10008.5.1.1.25"
#define UID_RETIRED_PrintQueueManagementSOPClass                   "1.2.840.10008.5.1.1.26"
#define UID_RETIRED_PullPrintRequestSOPClass                       "1.2.840.10008.5.1.1.31"
#define UID_RETIRED_PullStoredPrintManagementMetaSOPClass          "1.2.840.10008.5.1.1.32"

// Detached Management
#define UID_RETIRED_DetachedPatientManagementSOPClass              "1.2.840.10008.3.1.2.1.1"
#define UID_RETIRED_DetachedPatientManagementMetaSOPClass          "1.2.840.10008.3.1.2.1.4"
#define UID_RETIRED_DetachedVisitManagementSOPClass                "1.2.840.10008.3.1.2.2.1"
#define UID_RETIRED_DetachedStudyManagementSOPClass                "1.2.840.10008.3.1.2.3.1"
#define UID_RETIRED_DetachedResultsManagementSOPClass              "1.2.840.10008.3.1.2.5.1"
#define UID_RETIRED_DetachedResultsManagementMetaSOPClass          "1.2.840.10008.3.1.2.5.4"
#define UID_RETIRED_DetachedStudyManagementMetaSOPClass            "1.2.840.10008.3.1.2.5.5"
#define UID_RETIRED_DetachedInterpretationManagementSOPClass       "1.2.840.10008.3.1.2.6.1"

// Procedure Log
#define UID_ProceduralEventLoggingSOPClass                         "1.2.840.10008.1.40"
#define UID_ProceduralEventLoggingSOPInstance                      "1.2.840.10008.1.40.1"

// Substance Administration
#define UID_SubstanceAdministrationLoggingSOPClass                 "1.2.840.10008.1.42"
#define UID_SubstanceAdministrationLoggingSOPInstance              "1.2.840.10008.1.42.1"
#define UID_ProductCharacteristicsQuerySOPClass                    "1.2.840.10008.5.1.4.41"
#define UID_SubstanceApprovalQuerySOPClass                         "1.2.840.10008.5.1.4.42"

// Media Creation
#define UID_MediaCreationManagementSOPClass                        "1.2.840.10008.5.1.1.33"

// SOP Class Relationship Negotiation
#define UID_StorageServiceClass                                    "1.2.840.10008.4.2"

// Instance Availability Notification
#define UID_InstanceAvailabilityNotificationSOPClass               "1.2.840.10008.5.1.4.33"

// Application Hosting
#define UID_NativeDICOMModel                                       "1.2.840.10008.7.1.1"
#define UID_AbstractMultiDimensionalImageModel                     "1.2.840.10008.7.1.2"

// Communication of Display Parameters
#define UID_DisplaySystemSOPClass                                  "1.2.840.10008.5.1.1.40"
#define UID_DisplaySystemSOPInstance                               "1.2.840.10008.5.1.1.40.1"

// Other
#define UID_VerificationSOPClass                                   "1.2.840.10008.1.1"
#define UID_RETIRED_BasicStudyContentNotificationSOPClass          "1.2.840.10008.1.9"
#define UID_RETIRED_StudyComponentManagementSOPClass               "1.2.840.10008.3.1.2.3.2"

/*
** All other UIDs according to DICOM standard
*/

// Mapping Resources
#define UID_DICOMContentMappingResource                            "1.2.840.10008.8.1.1"

// Coding Schemes
#define UID_DICOMControlledTerminologyCodingScheme                 "1.2.840.10008.2.16.4"
#define UID_DICOMUIDRegistryCodingScheme                           "1.2.840.10008.2.6.1"

// Configuration Management LDAP UIDs
#define UID_LDAP_dicomDeviceName                                   "1.2.840.10008.15.0.3.1"
#define UID_LDAP_dicomDescription                                  "1.2.840.10008.15.0.3.2"
#define UID_LDAP_dicomManufacturer                                 "1.2.840.10008.15.0.3.3"
#define UID_LDAP_dicomManufacturerModelName                        "1.2.840.10008.15.0.3.4"
#define UID_LDAP_dicomSoftwareVersion                              "1.2.840.10008.15.0.3.5"
#define UID_LDAP_dicomVendorData                                   "1.2.840.10008.15.0.3.6"
#define UID_LDAP_dicomAETitle                                      "1.2.840.10008.15.0.3.7"
#define UID_LDAP_dicomNetworkConnectionReference                   "1.2.840.10008.15.0.3.8"
#define UID_LDAP_dicomApplicationCluster                           "1.2.840.10008.15.0.3.9"
#define UID_LDAP_dicomAssociationInitiator                         "1.2.840.10008.15.0.3.10"
#define UID_LDAP_dicomAssociationAcceptor                          "1.2.840.10008.15.0.3.11"
#define UID_LDAP_dicomHostname                                     "1.2.840.10008.15.0.3.12"
#define UID_LDAP_dicomPort                                         "1.2.840.10008.15.0.3.13"
#define UID_LDAP_dicomSOPClass                                     "1.2.840.10008.15.0.3.14"
#define UID_LDAP_dicomTransferRole                                 "1.2.840.10008.15.0.3.15"
#define UID_LDAP_dicomTransferSyntax                               "1.2.840.10008.15.0.3.16"
#define UID_LDAP_dicomPrimaryDeviceType                            "1.2.840.10008.15.0.3.17"
#define UID_LDAP_dicomRelatedDeviceReference                       "1.2.840.10008.15.0.3.18"
#define UID_LDAP_dicomPreferredCalledAETitle                       "1.2.840.10008.15.0.3.19"
#define UID_LDAP_dicomTLSCyphersuite                               "1.2.840.10008.15.0.3.20"
#define UID_LDAP_dicomAuthorizedNodeCertificateReference           "1.2.840.10008.15.0.3.21"
#define UID_LDAP_dicomThisNodeCertificateReference                 "1.2.840.10008.15.0.3.22"
#define UID_LDAP_dicomInstalled                                    "1.2.840.10008.15.0.3.23"
#define UID_LDAP_dicomStationName                                  "1.2.840.10008.15.0.3.24"
#define UID_LDAP_dicomDeviceSerialNumber                           "1.2.840.10008.15.0.3.25"
#define UID_LDAP_dicomInstitutionName                              "1.2.840.10008.15.0.3.26"
#define UID_LDAP_dicomInstitutionAddress                           "1.2.840.10008.15.0.3.27"
#define UID_LDAP_dicomInstitutionDepartmentName                    "1.2.840.10008.15.0.3.28"
#define UID_LDAP_dicomIssuerOfPatientID                            "1.2.840.10008.15.0.3.29"
#define UID_LDAP_dicomPreferredCallingAETitle                      "1.2.840.10008.15.0.3.30"
#define UID_LDAP_dicomSupportedCharacterSet                        "1.2.840.10008.15.0.3.31"
#define UID_LDAP_dicomConfigurationRoot                            "1.2.840.10008.15.0.4.1"
#define UID_LDAP_dicomDevicesRoot                                  "1.2.840.10008.15.0.4.2"
#define UID_LDAP_dicomUniqueAETitlesRegistryRoot                   "1.2.840.10008.15.0.4.3"
#define UID_LDAP_dicomDevice                                       "1.2.840.10008.15.0.4.4"
#define UID_LDAP_dicomNetworkAE                                    "1.2.840.10008.15.0.4.5"
#define UID_LDAP_dicomNetworkConnection                            "1.2.840.10008.15.0.4.6"
#define UID_LDAP_dicomUniqueAETitle                                "1.2.840.10008.15.0.4.7"
#define UID_LDAP_dicomTransferCapability                           "1.2.840.10008.15.0.4.8"

// UTC Synchronization Frame of Reference
#define UID_UniversalCoordinatedTimeSynchronizationFrameOfReference "1.2.840.10008.15.1.1"

// Well-known Frame of References
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

// Well-known SOP Instances for Color Palettes
#define UID_HotIronColorPaletteSOPInstance                         "1.2.840.10008.1.5.1"
#define UID_PETColorPaletteSOPInstance                             "1.2.840.10008.1.5.2"
#define UID_HotMetalBlueColorPaletteSOPInstance                    "1.2.840.10008.1.5.3"
#define UID_PET20StepColorPaletteSOPInstance                       "1.2.840.10008.1.5.4"
#define UID_SpringColorPaletteSOPInstance                          "1.2.840.10008.1.5.5"
#define UID_SummerColorPaletteSOPInstance                          "1.2.840.10008.1.5.6"
#define UID_FallColorPaletteSOPInstance                            "1.2.840.10008.1.5.7"
#define UID_WinterColorPaletteSOPInstance                          "1.2.840.10008.1.5.8"

/*
** Private DCMTK UIDs
*/

// Private SOP Class UID used to shutdown external network applications
#define UID_PrivateShutdownSOPClass                                "1.2.276.0.7230010.3.4.1915765545.18030.917282194.0"


/* DRAFT SUPPLEMENTS - EXPERIMENTAL USE ONLY */

/*
 * The following UIDs were defined in "frozen draft for trial implementation" versions
 * of various DICOM supplements and are or will be changed before final text.
 * Since it is likely that trial implementations exist, we leave the UIDs in the dictionary.
 */

/* Supplement 23 Frozen Draft (November 1997) */
#define UID_DRAFT_SRTextStorage                                    "1.2.840.10008.5.1.4.1.1.88.1"
#define UID_DRAFT_SRAudioStorage                                   "1.2.840.10008.5.1.4.1.1.88.2"
#define UID_DRAFT_SRDetailStorage                                  "1.2.840.10008.5.1.4.1.1.88.3"
#define UID_DRAFT_SRComprehensiveStorage                           "1.2.840.10008.5.1.4.1.1.88.4"

/* Supplement 30 Draft 08 for Demonstration (October 1997) */
#define UID_DRAFT_WaveformStorage                                  "1.2.840.10008.5.1.4.1.1.9.1"

/* Supplement 74 Frozen Draft (October 2007) */
#define UID_DRAFT_RTBeamsDeliveryInstructionStorage                "1.2.840.10008.5.1.4.34.1"
#define UID_DRAFT_RTConventionalMachineVerification                "1.2.840.10008.5.1.4.34.2"
#define UID_DRAFT_RTIonMachineVerification                         "1.2.840.10008.5.1.4.34.3"

// Supplement 96 Frozen Draft (October 2007), also part of DICOM 2008 and 2009
#define UID_DRAFT_UnifiedWorklistAndProcedureStepServiceClass      "1.2.840.10008.5.1.4.34.4"
#define UID_DRAFT_UnifiedProcedureStepPushSOPClass                 "1.2.840.10008.5.1.4.34.4.1"
#define UID_DRAFT_UnifiedProcedureStepWatchSOPClass                "1.2.840.10008.5.1.4.34.4.2"
#define UID_DRAFT_UnifiedProcedureStepPullSOPClass                 "1.2.840.10008.5.1.4.34.4.3"
#define UID_DRAFT_UnifiedProcedureStepEventSOPClass                "1.2.840.10008.5.1.4.34.4.4"

#endif /* DCUID_H */
