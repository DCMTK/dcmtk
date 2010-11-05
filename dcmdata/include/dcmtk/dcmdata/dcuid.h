/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Definitions of "well known" DICOM Unique Indentifiers,
 *  routines for finding and creating UIDs.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-11-05 10:26:10 $
 *  CVS/RCS Revision: $Revision: 1.89 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCUID_H
#define DCUID_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftypes.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

/** return the name of a UID.
 *  Performs a table lookup and returns a pointer to a read-only string.
 *  @param uid UID string for which the name is to be looked up
 *  @param defaultValue default to return if UID not known
 *  @return name string or defaultValue if UID is unknown
 */
const char* dcmFindNameOfUID(const char* uid, const char* defaultValue = NULL);

/** return the UID of a name.
 *  Performs a table lookup and returns a pointer to a read-only string.
 *  @param name name string for which the corresponding UID is to be looked up
 *  @return UID string or NULL if name is unknown
 */
const char* dcmFindUIDFromName(const char * name);

/** an array of const strings containing all known Storage SOP Classes
 *  that fit into the conventional PATIENT-STUDY-SERIES-INSTANCE information
 *  model, i.e. everything a Storage SCP might want to store in a PACS.
 *  Special cases such as hanging protocol storage or the Storage SOP Class
 *  are not included in this list.
 *  WARNING: This list contains more than 64 entries, i.e. it is not possible
 *  to use this list to configure the association negotiation behaviour of
 *  a Storage SCU that always proposes two presentation contexts for each
 *  SOP class.
 */
extern const char* dcmAllStorageSOPClassUIDs[];

/// number of entries in dcmAllStorageSOPClassUIDs.
extern const int numberOfAllDcmStorageSOPClassUIDs;

/** an array of const strings containing all storage SOP classes that
 *  are proposed by default by those Storage SCU components in DCMTK
 *  that always propose one presentation context for each SOP class,
 *  e.g. movescu or dcmqrdb. This list is guaranteed to have at most
 *  120 entries (to leave room for FIND/MOVE presentation contexts).
 */
extern const char* dcmLongSCUStorageSOPClassUIDs[];

/// number of entries in dcmLongSCUStorageSOPClassUIDs.
extern const int numberOfDcmLongSCUStorageSOPClassUIDs;

/** an array of const strings containing all storage SOP classes that
 *  are proposed by default by those Storage SCU components in DCMTK
 *  that always propose TWO presentation context for each SOP class,
 *  e.g. storescu. This list is guaranteed to have at most
 *  64 entries.
 */
extern const char* dcmShortSCUStorageSOPClassUIDs[];

/// number of entries in dcmShortSCUStorageSOPClassUIDs.
extern const int numberOfDcmShortSCUStorageSOPClassUIDs;

/** returns true if the uid is one of the Storage SOP Classes.
 *  Performs a table lookup in the dcmAllStorageSOPClassUIDs table.
 *  @param uid UID string
 *  @return true if UID is a known Storage SOP Class, false otherwise
 */
OFBool dcmIsaStorageSOPClassUID(const char* uid);

/** a global constant array of
 *  string pointers containing the UIDs of all known Image SOP
 *  Classes.  The global variable numberOfDcmImageSOPClassUIDs
 *  defines the size of the array.
 *  NOTE: this list represets a subset of the dcmStorageSOPClassUIDs list
 */
extern const char* dcmImageSOPClassUIDs[];

/// number of entries in dcmImageSOPClassUIDs
extern const int numberOfDcmImageSOPClassUIDs;

/** creates a Unique Identifer in uid and returns uid.
 *  uid must be at least 65 bytes. Care is taken to make sure
 *  that the generated UID is 64 characters or less.
 *  If a prefix string is not passed as the second argument a
 *  default of SITE_INSTANCE_UID_ROOT (see below) will be used.
 *  Otherwise the supplied prefix string will appear at the beginning
 *  of uid.
 *  The UID is created by appending to the prefix the following:
 *       the host id (if obtainable, zero otherwise),
 *       the process id (if obtainable, zero otherwise),
 *       the system calendar time, and
 *       an accumulating counter for tis process.
 *  @param uid pointer to buffer of 65 or more characters in which the UID is returned
 *  @param prefix prefix for UID creation
 *  @return pointer to UID, identical to uid parameter
 */
char *dcmGenerateUniqueIdentifier(char *uid, const char* prefix=NULL);

/** performs a table lookup and returns a short modality identifier
 *  that can be used for building file names etc.
 *  Identifiers are defined for all storage SOP classes.
 *  Returns 'defaultValue' if no modality identifier found or sopClassUID==NULL.
 *  @param sopClassUID UID string
 *  @param defaultValue default to return if UID not known
 *  @return modality string for modality UID, or defaultValue if not found
 */
const char *dcmSOPClassUIDToModality(const char *sopClassUID, const char *defaultValue = NULL);

/** performs a table lookup and returns a guessed average
 *  file size for the given SOP class.
 *  Average sizes are defined for all storage SOP classes, but may be very far off.
 *  @param sopClassUID UID string
 *  @return estimated everage size for objects of this SOP class
 */
unsigned long dcmGuessModalityBytes(const char *sopClassUID);

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

/* NOTE: Implementation version name VR=SH may not be longer than 16 chars
 *       The second name is used to identify files written without dcmdata
 *       (i.e. using the --bit-preserving switch in various tools)
 */

/// implementation version name for this version of the toolkit
#define OFFIS_DTK_IMPLEMENTATION_VERSION_NAME   "OFFIS_DCMTK_" PACKAGE_VERSION_NUMBER

/// implementation version name for this version of the toolkit, used for files received in "bit preserving" mode
#define OFFIS_DTK_IMPLEMENTATION_VERSION_NAME2  "OFFIS_DCMBP_" PACKAGE_VERSION_NUMBER

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
** These site UIDs are arbitary, non-standard, with no meaning
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

/** A private SOP Class UID which is used in a file meta-header when
 *  no real SOP Class is stored in the file. -- NON-STANDARD
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
/// Explicit VR Big Endian
#define UID_BigEndianExplicitTransferSyntax     "1.2.840.10008.1.2.2"
/// Deflated Explicit VR Little Endian
#define UID_DeflatedExplicitVRLittleEndianTransferSyntax "1.2.840.10008.1.2.1.99"
/** JPEG Baseline (Process 1): Default Transfer Syntax
 * for Lossy JPEG 8 Bit Image Compression
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
/// MPEG2 Main Profile @ Main Level
#define UID_MPEG2MainProfileAtMainLevelTransferSyntax "1.2.840.10008.1.2.4.100"
/// MPEG2 Main Profile @ High Level
#define UID_MPEG2MainProfileAtHighLevelTransferSyntax "1.2.840.10008.1.2.4.101"
/// RLE Lossless
#define UID_RLELosslessTransferSyntax           "1.2.840.10008.1.2.5"

/** MIME encapsulation (Supplement 101) is only a pseudo transfer syntax used to
 *  refer to MIME encapsulated HL7 CDA documents from a DICOMDIR when stored
 *  on a DICOM storage medium. It is never used for network communication
 *  or encoding of DICOM objects.
 */
#define UID_RFC2557MIMEEncapsulationTransferSyntax "1.2.840.10008.1.2.6.1"

/** XML encoding (Supplement 114) is only a pseudo transfer syntax used to refer to
 *  encapsulated HL7 CDA documents from a DICOMDIR when stored on a DICOM storage
 *  medium. It is never used for network communication or encoding of DICOM objects.
 */
#define UID_XMLEncodingTransferSyntax "1.2.840.10008.1.2.6.2"

/*
** Defined SOP Class UIDs according to DICOM standard
*/

// Storage
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
#define UID_RETIRED_UltrasoundMultiframeImageStorage               "1.2.840.10008.5.1.4.1.1.3"
#define UID_UltrasoundMultiframeImageStorage                       "1.2.840.10008.5.1.4.1.1.3.1"
#define UID_MRImageStorage                                         "1.2.840.10008.5.1.4.1.1.4"
#define UID_EnhancedMRImageStorage                                 "1.2.840.10008.5.1.4.1.1.4.1"
#define UID_MRSpectroscopyStorage                                  "1.2.840.10008.5.1.4.1.1.4.2"
#define UID_EnhancedMRColorImageStorage                            "1.2.840.10008.5.1.4.1.1.4.3"
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
#define UID_XRayAngiographicImageStorage                           "1.2.840.10008.5.1.4.1.1.12.1"
#define UID_EnhancedXAImageStorage                                 "1.2.840.10008.5.1.4.1.1.12.1.1"
#define UID_XRayRadiofluoroscopicImageStorage                      "1.2.840.10008.5.1.4.1.1.12.2"
#define UID_EnhancedXRFImageStorage                                "1.2.840.10008.5.1.4.1.1.12.2.1"
#define UID_XRay3DAngiographicImageStorage                         "1.2.840.10008.5.1.4.1.1.13.1.1"
#define UID_XRay3DCraniofacialImageStorage                         "1.2.840.10008.5.1.4.1.1.13.1.2"
#define UID_BreastTomosynthesisImageStorage                        "1.2.840.10008.5.1.4.1.1.13.1.3"
#define UID_RETIRED_XRayAngiographicBiPlaneImageStorage            "1.2.840.10008.5.1.4.1.1.12.3"
#define UID_NuclearMedicineImageStorage                            "1.2.840.10008.5.1.4.1.1.20"
#define UID_RawDataStorage                                         "1.2.840.10008.5.1.4.1.1.66"
#define UID_SpatialRegistrationStorage                             "1.2.840.10008.5.1.4.1.1.66.1"
#define UID_SpatialFiducialsStorage                                "1.2.840.10008.5.1.4.1.1.66.2"
#define UID_DeformableSpatialRegistrationStorage                   "1.2.840.10008.5.1.4.1.1.66.3"
#define UID_SegmentationStorage                                    "1.2.840.10008.5.1.4.1.1.66.4"
#define UID_SurfaceSegmentationStorage                             "1.2.840.10008.5.1.4.1.1.66.5"
#define UID_RealWorldValueMappingStorage                           "1.2.840.10008.5.1.4.1.1.67"
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
#define UID_VLWholeSlideMicroscopyImageStorage                     "1.2.840.10008.5.1.4.1.1.77.1.6"
#define UID_RETIRED_VLMultiFrameImageStorage                       "1.2.840.10008.5.1.4.1.1.77.2"
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
#define UID_BasicTextSRStorage                                     "1.2.840.10008.5.1.4.1.1.88.11"
#define UID_EnhancedSRStorage                                      "1.2.840.10008.5.1.4.1.1.88.22"
#define UID_ComprehensiveSRStorage                                 "1.2.840.10008.5.1.4.1.1.88.33"
#define UID_ProcedureLogStorage                                    "1.2.840.10008.5.1.4.1.1.88.40"
#define UID_MammographyCADSRStorage                                "1.2.840.10008.5.1.4.1.1.88.50"
#define UID_KeyObjectSelectionDocumentStorage                      "1.2.840.10008.5.1.4.1.1.88.59"
#define UID_ChestCADSRStorage                                      "1.2.840.10008.5.1.4.1.1.88.65"
#define UID_XRayRadiationDoseSRStorage                             "1.2.840.10008.5.1.4.1.1.88.67"
#define UID_ColonCADSRStorage                                      "1.2.840.10008.5.1.4.1.1.88.69"
#define UID_ImplantationPlanSRDocumentStorage                      "1.2.840.10008.5.1.4.1.1.88.70"
#define UID_EncapsulatedPDFStorage                                 "1.2.840.10008.5.1.4.1.1.104.1"
#define UID_EncapsulatedCDAStorage                                 "1.2.840.10008.5.1.4.1.1.104.2"
#define UID_PositronEmissionTomographyImageStorage                 "1.2.840.10008.5.1.4.1.1.128"
#define UID_RETIRED_StandalonePETCurveStorage                      "1.2.840.10008.5.1.4.1.1.129"
#define UID_EnhancedPETImageStorage                                "1.2.840.10008.5.1.4.1.1.130"
#define UID_BasicStructuredDisplayStorage                          "1.2.840.10008.5.1.4.1.1.131"
#define UID_RTImageStorage                                         "1.2.840.10008.5.1.4.1.1.481.1"
#define UID_RTDoseStorage                                          "1.2.840.10008.5.1.4.1.1.481.2"
#define UID_RTStructureSetStorage                                  "1.2.840.10008.5.1.4.1.1.481.3"
#define UID_RTBeamsTreatmentRecordStorage                          "1.2.840.10008.5.1.4.1.1.481.4"
#define UID_RTPlanStorage                                          "1.2.840.10008.5.1.4.1.1.481.5"
#define UID_RTBrachyTreatmentRecordStorage                         "1.2.840.10008.5.1.4.1.1.481.6"
#define UID_RTTreatmentSummaryRecordStorage                        "1.2.840.10008.5.1.4.1.1.481.7"
#define UID_RTIonPlanStorage                                       "1.2.840.10008.5.1.4.1.1.481.8"
#define UID_RTIonBeamsTreatmentRecordStorage                       "1.2.840.10008.5.1.4.1.1.481.9"
#define UID_GenericImplantTemplateStorage                          "1.2.840.10008.5.1.4.43.1"
#define UID_ImplantAssemblyTemplateStorage                         "1.2.840.10008.5.1.4.44.1"
#define UID_ImplantTemplateGroupStorage                            "1.2.840.10008.5.1.4.45.1"

// DICOMDIR; was UID_BasicDirectoryStorageSOPClass in DCMTK versions prior to 3.5.3
#define UID_MediaStorageDirectoryStorage                           "1.2.840.10008.1.3.10"

/* Hanging Protocols Storage is a special case because hanging protocols use a different
   information model, i.e. there is no patient, study or series in a hanging protocol IOD. */
#define UID_HangingProtocolStorage                                 "1.2.840.10008.5.1.4.38.1"

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
#define UID_RETIRED_MOVECompositeInstanceRootRetrieve              "1.2.840.10008.5.1.4.1.2.4.2"
#define UID_RETIRED_GETCompositeInstanceRootRetrieve               "1.2.840.10008.5.1.4.1.2.4.3"
#define UID_RETIRED_GETCompositeInstanceRetrieveWithoutBulkData    "1.2.840.10008.5.1.4.1.2.5.3"

// Worklist
#define UID_FINDModalityWorklistInformationModel                   "1.2.840.10008.5.1.4.31"
#define UID_FINDGeneralPurposeWorklistInformationModel             "1.2.840.10008.5.1.4.32.1"

// General Purpose Worklist
#define UID_GeneralPurposeScheduledProcedureStepSOPClass           "1.2.840.10008.5.1.4.32.2"
#define UID_GeneralPurposePerformedProcedureStepSOPClass           "1.2.840.10008.5.1.4.32.3"
#define UID_GeneralPurposeWorklistManagementMetaSOPClass           "1.2.840.10008.5.1.4.32"

// MPPS
#define UID_ModalityPerformedProcedureStepSOPClass                 "1.2.840.10008.3.1.2.3.3"
#define UID_ModalityPerformedProcedureStepRetrieveSOPClass         "1.2.840.10008.3.1.2.3.4"
#define UID_ModalityPerformedProcedureStepNotificationSOPClass     "1.2.840.10008.3.1.2.3.5"

// Unified Worklist and Procedure Step
#define UID_UnifiedWorklistAndProcedureStepServiceClass            "1.2.840.10008.5.1.4.34.4"
#define UID_UnifiedProcedureStepPushSOPClass                       "1.2.840.10008.5.1.4.34.4.1"
#define UID_UnifiedProcedureStepWatchSOPClass                      "1.2.840.10008.5.1.4.34.4.2"
#define UID_UnifiedProcedureStepPullSOPClass                       "1.2.840.10008.5.1.4.34.4.3"
#define UID_UnifiedProcedureStepEventSOPClass                      "1.2.840.10008.5.1.4.34.4.4"
#define UID_UnifiedWorklistAndProcedureStepSOPInstance             "1.2.840.10008.5.1.4.34.5"

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

// Other
#define UID_VerificationSOPClass                                   "1.2.840.10008.1.1"
#define UID_RETIRED_BasicStudyContentNotificationSOPClass          "1.2.840.10008.1.9"
#define UID_RETIRED_StudyComponentManagementSOPClass               "1.2.840.10008.3.1.2.3.2"

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

// UTC Synchronization Frame of Reference (CP 432)
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

// Private DCMTK UIDs

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

#endif /* DCUID_H */


/*
** CVS/RCS Log:
** $Log: dcuid.h,v $
** Revision 1.89  2010-11-05 10:26:10  joergr
** Added new SOP Class UIDs from Supplement 131 and 134 (Implant Templates).
**
** Revision 1.88  2010-10-14 13:15:42  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.87  2010-09-30 14:08:19  joergr
** Added two Model UIDs from Supplement 118 (Application Hosting).
**
** Revision 1.86  2010-09-30 07:53:17  joergr
** Fixed typo in the name of a Storage SOP Class (copied from DICOM part 6).
**
** Revision 1.85  2010-09-28 07:55:53  joergr
** Added VL Whole Slide Microscopy Image Storage SOP Class (Supplement 145).
**
** Revision 1.84  2010-09-27 14:04:42  joergr
** Updated list of SOP Class UIDs and well-known SOP Instance UIDs according to
** the current edition of the DICOM standard (including final text supplements).
**
** Revision 1.83  2010-09-27 08:24:03  joergr
** Added comment to retired transfer syntaxes and changed order of transfer
** syntax definitions according to their UID value.
**
** Revision 1.82  2010-09-24 13:24:16  joergr
** Compared names of SOP Class UIDs with 2009 edition of the DICOM standard. The
** resulting name changes are mainly caused by the fact that the corresponding
** SOP Class is now retired.
**
** Revision 1.81  2010-09-15 08:46:16  joergr
** Added definition of XML encoding transfer syntax (Supplement 114) and JPIP
** referenced transfer syntaxes (Supplement 106).
**
** Revision 1.80  2010-09-02 12:12:43  joergr
** Added support for "MPEG2 Main Profile @ High Level" transfer syntax.
**
** Revision 1.79  2010-08-10 11:59:31  uli
** Fixed some cases where dcmFindNameOfUID() returning NULL could cause crashes.
**
** Revision 1.78  2010-07-07 07:28:36  onken
** Added Ophthalmic Tomography Image Storage to list of supported SOP classes.
**
** Revision 1.77  2010-03-01 09:08:45  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.76  2009-08-07 14:37:33  joergr
** Removed package version suffix "+" from development snapshots.
**
** Revision 1.75  2008-04-30 13:30:26  joergr
** Moved package information like version number and release date to a central
** configuration file (one for GNU autoconf and one for CMake systems).
** Added support for new compiler definition "DCMTK_BUILD_DATE" that can be used
** to distinguish binaries of the current development version from the official
** release.
**
** Revision 1.74  2007/11/29 14:30:35  meichel
** Updated doxygen API documentation
**
** Revision 1.73  2006/10/27 11:58:49  joergr
** Added new default parameter to dcmSOPClassUIDToModality() that allows for
** the specification of the return value in case the SOP Class is unknown.
**
** Revision 1.72  2005/12/20 16:24:59  meichel
** Updated version name and implementation name to reflect release 3.5.4.
**
** Revision 1.71  2005/12/08 16:28:48  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.70  2005/11/16 18:31:42  joergr
** Added support for X-Ray Radiation Dose SR SOP class.
**
** Revision 1.69  2005/10/25 08:55:32  meichel
** Updated list of UIDs and added support for new transfer syntaxes
**   and storage SOP classes.
**
** Revision 1.68  2005/03/17 16:31:30  meichel
** Changed CR/LF to LF
**
** Revision 1.67  2005/02/17 13:09:55  joergr
** Renamed "MicroscopicImageStorage" to "VideoMicroscopicImageStorage".
**
** Revision 1.66  2004/11/10 12:37:55  meichel
** Updated directory of UIDs for 2004 DICOM edition. Removed all standalone and
**   most retired storage SOP classes from list of storage SOP classes for
**   storescu, storescp, imagectn etc. to keep list shorter than 64 entries.
**
** Revision 1.65  2004/08/03 11:41:07  meichel
** Headers libc.h and unistd.h are now included via ofstdinc.h
**
** Revision 1.64  2004/05/27 10:31:39  joergr
** Updated version name and implementation name to reflect release 3.5.3.
**
** Revision 1.63  2004/04/14 11:49:28  joergr
** Replaced non-Unix newline characters.
**
** Revision 1.62  2004/04/06 18:02:25  joergr
** Updated data dictionary, UIDs and transfer syntaxes for the latest Final Text
** Supplements (42 and 47) and Correction Proposals (CP 25).
** Added missing suffix "TransferSyntax" to some transfer syntax constants.
**
** Revision 1.61  2004/03/16 13:43:26  joergr
** Renamed UID_BasicDirectoryStorageSOPClass to UID_MediaStorageDirectoryStorage.
**
** Revision 1.60  2004/02/13 14:10:30  joergr
** Fixed wrong spelling of "SpatialFiducialsStorage".
**
** Revision 1.59  2004/02/13 11:42:43  joergr
** Corrected order of tags, modified comments and other minor corrections.
**
** Revision 1.58  2004/02/11 15:53:08  wilkens
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
