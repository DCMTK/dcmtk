/*
**
** Author: Andrew Hewett	Created: 4.11.95
** Kuratorium OFFIS e.V.
**
** Module: dcuid.h
**
** Purpose:
** Definitions of "well known" DICOM Unique Indentifiers,
** routines for finding and creating UIDs.
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-11-27 16:12:17 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcuid.h,v $
** CVS/RCS Revision:	$Revision: 1.9 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef DCUID_H
#define DCUID_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */


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
** char* generateUniqueIdentifer(char* uid)
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
**	the host id (if obtainable, zero otherwise)
**	the process id (if obtainable, zero otherwise)
**	the system calendar time
**	an accumulating counter for this process
**
** NOTE: Thread UNSAFE
*/
char* dcmGenerateUniqueIdentifer(char* uid, const char* prefix=NULL);


/*
** String Constants
*/

/* 
** OFFIS UID is: 1.2.276.0.7230010
** UID Root for dcmtk DICOM project: 1.2.276.0.7230010.3
** 
** Private Suffix: .0
** Software Suffix: .1
** Study Suffix: .2
** Series Suffix: .3
** Image Instance Suffix: .4
** Frame of Reference Suffix: .5
** Instance Creator Suffix: .6
** Fileset Suffix: .7
** Detached Patient Mgmt Suffix: .8
** Detached Visit Mgmt Suffix: .9
** Detached Study Mgmt Suffix: .10
** Study Component Mgmt Suffix: .11
** Detached Results Mgmt Suffix: .12
** Detached Interpretation Mgmt Suffix: .13
** ...
**/

/* NOTE: Implementation version name may not be longer than 16 chars */
#define OFFIS_DTK_IMPLEMENTATION_VERSION_NAME	"OFFIS-DCMTK-311"

#define OFFIS_UID_ROOT		       "1.2.276.0.7230010.3"
#define OFFIS_DCMTK_VERSION	       "3.1.1"
#define OFFIS_IMPLEMENTATION_CLASS_UID OFFIS_UID_ROOT ".0." OFFIS_DCMTK_VERSION

/* 
** Each site should define its own SITE_UID_ROOT
*/
#ifndef SITE_UID_ROOT
#define SITE_UID_ROOT	OFFIS_UID_ROOT	/* default */
#endif

/*
** Useful UID prefixes. These can be whatever you want.
*/
#define SITE_PRIVATE_UID_ROOT				SITE_UID_ROOT ".0"
#define SITE_STUDY_UID_ROOT				SITE_UID_ROOT ".2"
#define SITE_SERIES_UID_ROOT				SITE_UID_ROOT ".3"
#define SITE_INSTANCE_UID_ROOT				SITE_UID_ROOT ".4"
#define SITE_FRAMEOFREFERENCE_UID_ROOT			SITE_UID_ROOT ".5"
#define SITE_INSTANCECREATOR_UID_ROOT			SITE_UID_ROOT ".6"
#define SITE_FILESET_UID_ROOT				SITE_UID_ROOT ".7"
#define SITE_DETACHED_PATIENT_MGMT_UID_ROOT		SITE_UID_ROOT ".8"
#define SITE_DETACHED_VISIT_MGMT_UID_ROOT		SITE_UID_ROOT ".9"
#define SITE_DETACHED_STUDY_MGMT_UID_ROOT		SITE_UID_ROOT ".10"
#define SITE_STUDY_COMPONENT_MGMT_UID_ROOT		SITE_UID_ROOT ".11"
#define SITE_DETACHED_RESULTS_MGMT_UID_ROOT		SITE_UID_ROOT ".12"
#define SITE_DETACHED_INTERPRETATION_MGMT_UID_ROOT	SITE_UID_ROOT ".13"


/*
** A private SOP Class UID which can be used in a file meta-header when
** no real SOP Class is stored in the file. -- NON-STANDARD
*/
#define UID_PrivateGenericFileSOPClass	SITE_PRIVATE_UID_ROOT ".1"


/*
** DICOM Defined Standard Application Context UID
*/

#define UID_StandardApplicationContext		"1.2.840.10008.3.1.1.1"

/*
** Defined Transfer Syntax UIDs
*/

#define UID_LittleEndianImplicitTransferSyntax	"1.2.840.10008.1.2"
#define UID_LittleEndianExplicitTransferSyntax	"1.2.840.10008.1.2.1"
#define UID_BigEndianExplicitTransferSyntax	"1.2.840.10008.1.2.2"
#define UID_JPEGProcess1TransferSyntax		"1.2.840.10008.1.2.4.50"
#define UID_JPEGProcess2_4TransferSyntax	"1.2.840.10008.1.2.4.51"
#define UID_JPEGProcess3_5TransferSyntax	"1.2.840.10008.1.2.4.52"
#define UID_JPEGProcess6_8TransferSyntax	"1.2.840.10008.1.2.4.53"
#define UID_JPEGProcess7_9TransferSyntax	"1.2.840.10008.1.2.4.54"
#define UID_JPEGProcess10_12TransferSyntax	"1.2.840.10008.1.2.4.55"
#define UID_JPEGProcess11_13TransferSyntax	"1.2.840.10008.1.2.4.56"
#define UID_JPEGProcess14TransferSyntax		"1.2.840.10008.1.2.4.57"
#define UID_JPEGProcess15TransferSyntax		"1.2.840.10008.1.2.4.58"
#define UID_JPEGProcess16_18TransferSyntax	"1.2.840.10008.1.2.4.59"
#define UID_JPEGProcess17_19TransferSyntax	"1.2.840.10008.1.2.4.60"
#define UID_JPEGProcess20_22TransferSyntax	"1.2.840.10008.1.2.4.61"
#define UID_JPEGProcess21_23TransferSyntax	"1.2.840.10008.1.2.4.62"
#define UID_JPEGProcess24_26TransferSyntax	"1.2.840.10008.1.2.4.63"
#define UID_JPEGProcess25_27TransferSyntax	"1.2.840.10008.1.2.4.64"
#define UID_JPEGProcess28TransferSyntax		"1.2.840.10008.1.2.4.65"
#define UID_JPEGProcess29TransferSyntax		"1.2.840.10008.1.2.4.66"
#define UID_JPEGProcess14SV1TransferSyntax	"1.2.840.10008.1.2.4.70"
#define UID_RLELossless				"1.2.840.10008.1.2.5"

/*
** Defined SOP UIDs
*/

#define UID_VerificationSOPClass				"1.2.840.10008.1.1"
#define UID_BasicDirectoryStorageSOPClass			"1.2.840.10008.1.3.10"
#define UID_BasicStudyContentNotificationSOPClass		"1.2.840.10008.1.9"
#define UID_DetachedPatientManagementSOPClass			"1.2.840.10008.3.1.2.1.1"
#define UID_DetachedPatientManagementMetaSOPClass		"1.2.840.10008.3.1.2.1.4"
#define UID_DetachedVisitManagementSOPClass			"1.2.840.10008.3.1.2.2.1"
#define UID_DetachedStudyManagementSOPClass			"1.2.840.10008.3.1.2.3.1"
#define UID_StudyComponentManagementSOPClass			"1.2.840.10008.3.1.2.3.2"
#define UID_DetachedResultsManagementSOPClass			"1.2.840.10008.3.1.2.5.1"
#define UID_DetachedResultsManagementMetaSOPClass		"1.2.840.10008.3.1.2.5.4"
#define UID_DetachedStudyManagementMetaSOPClass			"1.2.840.10008.3.1.2.5.5"
#define UID_DetachedInterpretationManagementSOPClass		"1.2.840.10008.3.1.2.6.1"
#define UID_BasicFilmSessionSOPClass				"1.2.840.10008.5.1.1.1"
#define UID_PrintJobSOPClass					"1.2.840.10008.5.1.1.14"
#define UID_BasicAnnotationBoxSOPClass				"1.2.840.10008.5.1.1.15"
#define UID_PrinterSOPClass					"1.2.840.10008.5.1.1.16"
#define UID_PrinterSOPInstance					"1.2.840.10008.5.1.1.17"
#define UID_BasicColorPrintManagementMetaSOPClass		"1.2.840.10008.5.1.1.18"
#define UID_ReferencedColorPrintManagementMetaSOPClass		"1.2.840.10008.5.1.1.18.1"
#define UID_BasicFilmBoxSOPClass				"1.2.840.10008.5.1.1.2"
#define UID_VOILUTBoxSOPClass					"1.2.840.10008.5.1.1.22"
#define UID_ImageOverlayBoxSOPClass				"1.2.840.10008.5.1.1.24"
#define UID_BasicGrayscaleImageBoxSOPClass			"1.2.840.10008.5.1.1.4"
#define UID_BasicColorImageBoxSOPClass				"1.2.840.10008.5.1.1.4.1"
#define UID_ReferencedImageBoxSOPClass				"1.2.840.10008.5.1.1.4.2"
#define UID_BasicGrayscalePrintManagementMetaSOPClass		"1.2.840.10008.5.1.1.9"
#define UID_ReferencedGrayscalePrintManagementMetaSOPClass 	"1.2.840.10008.5.1.1.9.1"

#define UID_ComputedRadiographyImageStorage			"1.2.840.10008.5.1.4.1.1.1"
#define UID_StandaloneModalityLUTStorage			"1.2.840.10008.5.1.4.1.1.10"
#define UID_StandaloneVOILUTStorage				"1.2.840.10008.5.1.4.1.1.11"
#define UID_CTImageStorage					"1.2.840.10008.5.1.4.1.1.2"
#define UID_MRImageStorage					"1.2.840.10008.5.1.4.1.1.4"
#define UID_NuclearMedicineImageStorage				"1.2.840.10008.5.1.4.1.1.20"
#define UID_RETIRED_NuclearMedicineImageStorage			"1.2.840.10008.5.1.4.1.1.5"
#define UID_UltrasoundImageStorage				"1.2.840.10008.5.1.4.1.1.6.1"
#define UID_RETIRED_UltrasoundImageStorage			"1.2.840.10008.5.1.4.1.1.6"
#define UID_UltrasoundMultiframeImageStorage			"1.2.840.10008.5.1.4.1.1.3.1"
#define UID_RETIRED_UltrasoundMultiframeImageStorage		"1.2.840.10008.5.1.4.1.1.3"
#define UID_SecondaryCaptureImageStorage			"1.2.840.10008.5.1.4.1.1.7"
#define UID_StandaloneOverlayStorage				"1.2.840.10008.5.1.4.1.1.8"
#define UID_StandaloneCurveStorage				"1.2.840.10008.5.1.4.1.1.9"
#define UID_XRayAngiographicImageStorage			"1.2.840.10008.5.1.4.1.1.12.1"
#define UID_XRayAngiographicBiPlaneImageStorage			"1.2.840.10008.5.1.4.1.1.12.3"
#define UID_XRayFluoroscopyImageStorage				"1.2.840.10008.5.1.4.1.1.12.2"

#define UID_FINDPatientRootQueryRetrieveInformationModel 	"1.2.840.10008.5.1.4.1.2.1.1"
#define UID_MOVEPatientRootQueryRetrieveInformationModel 	"1.2.840.10008.5.1.4.1.2.1.2"
#define UID_GETPatientRootQueryRetrieveInformationModel		"1.2.840.10008.5.1.4.1.2.1.3"
#define UID_FINDStudyRootQueryRetrieveInformationModel		"1.2.840.10008.5.1.4.1.2.2.1"
#define UID_MOVEStudyRootQueryRetrieveInformationModel		"1.2.840.10008.5.1.4.1.2.2.2"
#define UID_GETStudyRootQueryRetrieveInformationModel		"1.2.840.10008.5.1.4.1.2.2.3"
#define UID_FINDPatientStudyOnlyQueryRetrieveInformationModel	"1.2.840.10008.5.1.4.1.2.3.1"
#define UID_MOVEPatientStudyOnlyQueryRetrieveInformationModel	"1.2.840.10008.5.1.4.1.2.3.2"
#define UID_GETPatientStudyOnlyQueryRetrieveInformationModel	"1.2.840.10008.5.1.4.1.2.3.3"

#define UID_StorageCommitmentPushModelSOPClass			"1.2.840.10008.1.20.1"
#define UID_StorageCommitmentPushModelSOPInstance		"1.2.840.10008.1.20.1.1"
#define UID_StorageCommitmentPullModelSOPClass			"1.2.840.10008.1.20.2"
#define UID_StorageCommitmentPullModelSOPInstance		"1.2.840.10008.1.20.2.1"

#define UID_FINDModalityWorklistInformationModel		"1.2.840.10008.5.1.4.31"

#define UID_RTImageStorage		"1.2.840.10008.5.1.4.1.1.481.1"
#define UID_RTDoseStorage		"1.2.840.10008.5.1.4.1.1.481.2"
#define UID_RTStructureSetStorage	"1.2.840.10008.5.1.4.1.1.481.3"
#define UID_RTTreatmentRecordStorage	"1.2.840.10008.5.1.4.1.1.481.4"
#define UID_RTPlanStorage		"1.2.840.10008.5.1.4.1.1.481.5"

#endif /* DCUID_H */

/*
** CVS/RCS Log:
** $Log: dcuid.h,v $
** Revision 1.9  1996-11-27 16:12:17  hewett
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
**
*/
