/*
**
** Author: Andrew Hewett	Created: 4.11.95
** Kuratorium OFFIS e.V.
**
** Module: dcuid.cc
**
** Purpose:
** Definitions of "well known" DICOM Unique Indentifiers,
** routines for finding and created UIDs.
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-03-22 12:42:22 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcuid.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <iostream.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_LIBC_H
#include <libc.h>
#endif

#include "dcuid.h"

struct UIDNameMap {
    const char* uid;
    const char* name;
};

//
// It is very important that the names of the UIDs may not use the following 
// characters: space  (  )  [  ], =  <  >
 
static UIDNameMap uidNameMap[] = {
    { UID_StandardApplicationContext, "StandardApplicationContext" },
    { UID_VerificationSOPClass, "VerificationSOPClass" },        
    { UID_LittleEndianImplicitTransferSyntax, "LittleEndianImplicit" },
    { UID_LittleEndianExplicitTransferSyntax, "LittleEndianExplicit" },
    { UID_BigEndianExplicitTransferSyntax, "BigEndianExplicit" },
    { UID_JPEGProcess1TransferSyntax, "JPEGBaseline" },
    { UID_JPEGProcess2_4TransferSyntax, "JPEGExtended" },
    { UID_JPEGProcess3_5TransferSyntax, "JPEGExtended" },
    { UID_JPEGProcess6_8TransferSyntax, "JPEGSpectralSelection:Non-hierarchical" },
    { UID_JPEGProcess7_9TransferSyntax, "JPEGSpectralSelection:Non-hierarchical" },
    { UID_JPEGProcess10_12TransferSyntax, "JPEGFullProgression:Non-hierarchical" },
    { UID_JPEGProcess11_13TransferSyntax, "JPEGFullProgression:Non-hierarchical" },
    { UID_JPEGProcess14TransferSyntax, "JPEGLossless:Non-hierarchical" },
    { UID_JPEGProcess15TransferSyntax, "JPEGLossless:Non-hierarchical" },
    { UID_JPEGProcess16_18TransferSyntax, "JPEGExtended:Hierarchical" },
    { UID_JPEGProcess17_19TransferSyntax, "JPEGExtended:Hierarchical" },
    { UID_JPEGProcess20_22TransferSyntax, "JPEGSpectralSelection:Hierarchical" },
    { UID_JPEGProcess21_23TransferSyntax, "JPEGSpectralSelection:Hierarchical" },
    { UID_JPEGProcess24_26TransferSyntax, "JPEGFullProgression:Hierarchical" },
    { UID_JPEGProcess25_27TransferSyntax, "JPEGFullProgression:Hierarchical" },
    { UID_JPEGProcess28TransferSyntax, "JPEGLossless:Hierarchical" },
    { UID_JPEGProcess29TransferSyntax, "JPEGLossless:Hierarchical" },
    { UID_JPEGProcess14SV1TransferSyntax, "JPEGLossless:Hierarchical-1stOrderPrediction" },
    { UID_BasicDirectoryStorageSOPClass, "BasicDirectoryStorageSOPClass" },
    { UID_BasicStudyContentNotificationSOPClass, "BasicStudyContentNotificationSOPClass" },
    { UID_DetachedPatientManagementSOPClass, "DetachedPatientManagementSOPClass" },
    { UID_DetachedPatientManagementMetaSOPClass, "DetachedPatientManagementMetaSOPClass" },
    { UID_DetachedVisitManagementSOPClass, "DetachedVisitManagementSOPClass" },
    { UID_DetachedStudyManagementSOPClass, "DetachedStudyManagementSOPClass" },
    { UID_StudyComponentManagementSOPClass, "StudyComponentManagementSOPClass" },
    { UID_DetachedResultsManagementSOPClass, "DetachedResultsManagementSOPClass" },
    { UID_DetachedResultsManagementMetaSOPClass, "DetachedResultsManagementMetaSOPClass" },
    { UID_DetachedStudyManagementMetaSOPClass, "DetachedStudyManagementMetaSOPClass" },
    { UID_DetachedInterpretationManagementSOPClass, "DetachedInterpretationManagementSOPClass" },
    { UID_BasicFilmSessionSOPClass, "BasicFilmSessionSOPClass" },
    { UID_PrintJobSOPClass, "PrintJobSOPClass" },
    { UID_BasicAnnotationBoxSOPClass, "BasicAnnotationBoxSOPClass" },
    { UID_PrinterSOPClass, "PrinterSOPClass" },
    { UID_PrinterSOPInstance, "PrinterSOPInstance" },
    { UID_BasicColorPrintManagementMetaSOPClass, "BasicColorPrintManagementMetaSOPClass" },
    { UID_ReferencedColorPrintManagementMetaSOPClass, "ReferencedColorPrintManagementMetaSOPClass" },
    { UID_BasicFilmBoxSOPClass, "BasicFilmBoxSOPClass" },
    { UID_VOILUTBoxSOPClass, "VOILUTBoxSOPClass" },
    { UID_ImageOverlayBoxSOPClass, "ImageOverlayBoxSOPClass" },
    { UID_BasicGrayscaleImageBoxSOPClass, "BasicGrayscaleImageBoxSOPClass" },
    { UID_BasicColorImageBoxSOPClass, "BasicColorImageBoxSOPClass" },
    { UID_ReferencedImageBoxSOPClass, "ReferencedImageBoxSOPClass" },
    { UID_BasicGrayscalePrintManagementMetaSOPClass, "BasicGrayscalePrintManagementMetaSOPClass" },
    { UID_ReferencedGrayscalePrintManagementMetaSOPClass, "ReferencedGrayscalePrintManagementMetaSOPClass" },
    { UID_ComputedRadiographyImageStorage, "ComputedRadiographyImageStorage" },
    { UID_StandaloneModalityLUTStorage, "StandaloneModalityLUTStorage" },
    { UID_StandaloneVOILUTStorage, "StandaloneVOILUTStorage" },
    { UID_CTImageStorage, "CTImageStorage" },
    { UID_UltrasoundMultiframeImageStorage, "UltrasoundMultiframeImageStorage" },
    { UID_MRImageStorage, "MRImageStorage" },
    { UID_NuclearMedicineImageStorage, "NuclearMedicineImageStorage" },
    { UID_UltrasoundImageStorage, "UltrasoundImageStorage" },
    { UID_SecondaryCaptureImageStorage, "SecondaryCaptureImageStorage" },
    { UID_StandaloneOverlayStorage, "StandaloneOverlayStorage" },
    { UID_StandaloneCurveStorage, "StandaloneCurveStorage" },
    { UID_FINDPatientRootQueryRetrieveInformationModel, "FINDPatientRootQueryRetrieveInformationModel" },
    { UID_MOVEPatientRootQueryRetrieveInformationModel, "MOVEPatientRootQueryRetrieveInformationModel" },
    { UID_GETPatientRootQueryRetrieveInformationModel, "GETPatientRootQueryRetrieveInformationModel" },
    { UID_FINDStudyRootQueryRetrieveInformationModel, "FINDStudyRootQueryRetrieveInformationModel" },
    { UID_MOVEStudyRootQueryRetrieveInformationModel, "MOVEStudyRootQueryRetrieveInformationModel" },
    { UID_GETStudyRootQueryRetrieveInformationModel, "GETStudyRootQueryRetrieveInformationModel" },
    { UID_FINDPatientStudyOnlyQueryRetrieveInformationModel, "FINDPatientStudyOnlyQueryRetrieveInformationModel" },
    { UID_MOVEPatientStudyOnlyQueryRetrieveInformationModel, "MOVEPatientStudyOnlyQueryRetrieveInformationModel" },
    { UID_GETPatientStudyOnlyQueryRetrieveInformationModel, "GETPatientStudyOnlyQueryRetrieveInformationModel" },
    
    { UID_RETIRED_NuclearMedicineImageStorage, "RETIRED_NuclearMedicineImageStorage" },
    { UID_RETIRED_UltrasoundImageStorage, "RETIRED_UltrasoundImageStorage" },
    { UID_RETIRED_UltrasoundMultiframeImageStorage, "RETIRED_UltrasoundMultiframeImageStorage" },

    { UID_XRayAngiographicImageStorage, "XRayAngiographicImageStorage" },
    { UID_XRayAngiographicBiPlaneImageStorage, "XRayAngiographicBiPlaneImageStorage" },
    { UID_XRayFluoroscopyImageStorage, "XRayFluoroscopyImageStorage" },
    
    { UID_StorageCommitmentPushModelSOPClass, "StorageCommitmentPushModelSOPClass" },
    { UID_StorageCommitmentPushModelSOPInstance, "StorageCommitmentPushModelSOPInstance" },
    { UID_StorageCommitmentPullModelSOPClass, "StorageCommitmentPullModelSOPClass" },
    { UID_StorageCommitmentPullModelSOPInstance, "StorageCommitmentPullModelSOPInstance" },

    { UID_FINDModalityWorklistInformationModel, "FINDModalityWorklistInformationModel" },
    
    { NULL, NULL }
};

static int uidNameMap_size = ( sizeof(uidNameMap) / sizeof(UIDNameMap) );


/*
** dcmFindNameOfUID(const char* uid)
** Return the name of a UID.
** Performs a table lookup and returns a pointer to a read-only string.
** Returns NULL of the UID is not known.
*/

const char* 
dcmFindNameOfUID(const char* uid)
{
    int i = 0;
    if (uid == NULL) return NULL;
    for (i=0; i<uidNameMap_size; i++) {
        if (uidNameMap[i].uid != NULL && strcmp(uid, uidNameMap[i].uid) == 0) {
            return uidNameMap[i].name;
        }
    }
    return NULL;
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
    for(int i = 0; i < uidNameMap_size; i++)
    {
        if (uidNameMap[i].name != NULL && strcmp(name, uidNameMap[i].name) == 0) 
            return uidNameMap[i].uid;
    }
    return NULL;
}



// ********************************

#ifndef HAVE_GETHOSTID
#ifdef HAVE_SYSINFO

#include <sys/systeminfo.h>
static long gethostid(void)
{
    char buf[128];
    if (sysinfo(SI_HW_SERIAL, buf, 128) == -1) {
       perror("sysinfo");
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

static long gethostid(void) { return 0; }   // workaround for MSDOS & MAC

#endif // !HAVE_SYSINFO
#else // !HAVE_GETHOSTID
#ifndef HAVE_PROTOTYPE_GETHOSTID
extern "C" {
long gethostid(void);
}
#endif // !HAVE_PROTOTYPE_GETHOSTID
#endif // HAVE_GETHOSTID

#ifndef HAVE_GETPID
static int getpid(void) { return 0; }   // workaround for MSDOS & MAC
#endif // !HAVE_GETPID

// ********************************

/*		
** char* generateUniqueIdentifer(char* buf)
** Creates a Unique Identifer in buf and returns buf.
** buf must be at least 65 bytes.
**
** NOTE: Thread UNSAFE
*/

static unsigned int counterOfCurrentUID = 0;

static const int maxUIDLen = 64;	/* A UID may be 64 chars or less */

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
    int left = maxUIDLen - strlen(uid);

    if (left > 0) {
	/* copy into uid as much of the contents of s as possible */
	int slen = strlen(s);
	int use = left;
	if (slen < left) use = slen; 
	strncat(uid, s, use);
    }

    stripTrailing(uid, '.');
}

char* dcmGenerateUniqueIdentifer(char* uid, const char* prefix)
{
    char buf[128]; /* be very safe */

    uid[0] = '\0'; /* initialise */

    if (prefix != NULL ) {
	addUIDComponent(uid, prefix);
    } else {
	addUIDComponent(uid, SITE_INSTANCE_UID_ROOT);
    }

    sprintf(buf, ".%ld", (unsigned long)gethostid());
    addUIDComponent(uid, buf);

    sprintf(buf, ".%d", (unsigned int)getpid());
    addUIDComponent(uid, buf);

    sprintf(buf, ".%ld", (unsigned long)time(NULL));
    addUIDComponent(uid, buf);

    sprintf(buf, ".%d", counterOfCurrentUID++);
    addUIDComponent(uid, buf);

    return uid;
}


/*
** CVS/RCS Log:
** $Log: dcuid.cc,v $
** Revision 1.5  1996-03-22 12:42:22  andreas
** findUIDfromName does not dump anymore if name does not correspond to a uid
**
** Revision 1.4  1996/03/11 14:38:01  hewett
** Added new SOP Class UIDs.
**
** Revision 1.3  1996/01/29 13:38:30  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.2  1996/01/23 17:29:25  andreas
** Support for old fashioned make without @^
** Support for machines that have mktemp but do not define it.
**
** Revision 1.1  1995/11/23 17:02:56  hewett
** Updated for loadable data dictionary.  Some cleanup (more to do).
**
**
*/
