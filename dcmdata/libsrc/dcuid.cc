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
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1997-09-22 14:56:56 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcuid.cc,v $
** CVS/RCS Revision:	$Revision: 1.10 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */


#ifdef HAVE_WINSOCK_H
/* Use the WinSock sockets library on Windows */
#include <WINSOCK.H>
#endif


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
END_EXTERN_C


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
    { UID_RLELossless, "RLELossless" },
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
    
    { UID_RTImageStorage, "RTImageStorage" },
    { UID_RTDoseStorage, "RTDoseStorage" },
    { UID_RTStructureSetStorage, "RTStructureSetStorage" },
    { UID_RTPlanStorage, "RTPlanStorage" },

    { UID_PETImageStorage, "PETImageStorage" },
    { UID_PETCurveStorage, "PETCurveStorage" },

    { UID_PrintQueueSOPInstance, "PrintQueueSOPInstance" },
    { UID_PrintQueueManagementSOPClass, "PrintQueueManagementSOPClass" },

    { UID_VLImageStorage, "VLImageStorage" },
    { UID_VLMultiFrameImageStorage, "VLMultiFrameImageStorage" },

    { UID_ModalityPerformedProcedureStepSOPClass, "ModalityPerformedProcedureStepSOPClass" },
    { UID_ModalityManagementMetaSOPClass, "ModalityManagementMetaSOPClass" },

    { UID_UserPreferenceLUTSOPClass, "UserPreferenceLUTSOPClass" },

    { UID_BasicGrayscalePrintStorageSOPClass, "BasicGrayscalePrintStorageSOPClass" },
    { UID_BasicColorPrintStorageSOPClass, "BasicColorPrintStorageSOPClass" },
    { UID_ReferencedGrayscalePrintStorageSOPClass, "ReferencedGrayscalePrintStorageSOPClass" },
    { UID_ReferencedColorPrintStorageSOPClass, "ReferencedColorPrintStorageSOPClass" },

    { NULL, NULL }
};

static int uidNameMap_size = ( sizeof(uidNameMap) / sizeof(UIDNameMap) );


/*
** The global variable dcmStorageSOPClassUIDs is an array of 
** string pointers containing the UIDs of all known Storage SOP
** Classes.  The global variable numberOfDcmStorageStopClassUIDs
** defines the size of the array.
*/

const char* dcmStorageSOPClassUIDs[] = {
    UID_ComputedRadiographyImageStorage, 
    UID_CTImageStorage,
    UID_UltrasoundMultiframeImageStorage,
    UID_MRImageStorage,
    UID_NuclearMedicineImageStorage,
    UID_UltrasoundImageStorage,
    UID_SecondaryCaptureImageStorage,

    UID_StandaloneOverlayStorage,
    UID_StandaloneCurveStorage,
    UID_StandaloneModalityLUTStorage,
    UID_StandaloneVOILUTStorage,

    UID_RETIRED_NuclearMedicineImageStorage,
    UID_RETIRED_UltrasoundImageStorage,
    UID_RETIRED_UltrasoundMultiframeImageStorage,

    UID_XRayAngiographicImageStorage,
    UID_XRayAngiographicBiPlaneImageStorage,
    UID_XRayFluoroscopyImageStorage,
    
    UID_RTImageStorage,
    UID_RTDoseStorage,
    UID_RTStructureSetStorage,
    UID_RTPlanStorage,

    UID_PETImageStorage,
    UID_PETCurveStorage,

    UID_VLImageStorage,
    UID_VLMultiFrameImageStorage,

    UID_BasicGrayscalePrintStorageSOPClass, 
    UID_BasicColorPrintStorageSOPClass, 
    UID_ReferencedGrayscalePrintStorageSOPClass, 
    UID_ReferencedColorPrintStorageSOPClass, 

    NULL
};

const int numberOfDcmStorageSOPClassUIDs = 
    (sizeof(dcmStorageSOPClassUIDs) / sizeof(const char*)) - 1;


/*
** The global variable dcmImageSOPClassUIDs is an array of 
** string pointers containing the UIDs of all known Image SOP
** Classes.  The global variable numberOfDcmImageSOPClassUIDs
** defines the size of the array.
** NOTE: this list represets a subset of the dcmStorageSOPClassUIDs list
*/

const char* dcmImageSOPClassUIDs[] = {
    UID_ComputedRadiographyImageStorage, 
    UID_CTImageStorage,
    UID_UltrasoundMultiframeImageStorage,
    UID_MRImageStorage,
    UID_NuclearMedicineImageStorage,
    UID_UltrasoundImageStorage,
    UID_SecondaryCaptureImageStorage,

// The Standalone objects are not really images
//    UID_StandaloneOverlayStorage,
//    UID_StandaloneCurveStorage,
//    UID_StandaloneModalityLUTStorage,
//    UID_StandaloneVOILUTStorage,

    UID_RETIRED_NuclearMedicineImageStorage,
    UID_RETIRED_UltrasoundImageStorage,
    UID_RETIRED_UltrasoundMultiframeImageStorage,

    UID_XRayAngiographicImageStorage,
    UID_XRayAngiographicBiPlaneImageStorage,
    UID_XRayFluoroscopyImageStorage,
    
    UID_RTImageStorage,
// The other RT objects are not images and therefore cannot be put 
// in DICOMDIR IMAGE records
//    UID_RTDoseStorage,
//    UID_RTStructureSetStorage,
//    UID_RTPlanStorage,

    UID_PETImageStorage,
// Curves are not images
//    UID_PETCurveStorage,

    UID_VLImageStorage,
    UID_VLMultiFrameImageStorage,

// The print objects are not really images
//    UID_BasicGrayscalePrintStorageSOPClass, 
//    UID_BasicColorPrintStorageSOPClass, 
//    UID_ReferencedGrayscalePrintStorageSOPClass, 
//    UID_ReferencedColorPrintStorageSOPClass, 

    NULL
};

const int numberOfDcmImageSOPClassUIDs = 
    (sizeof(dcmImageSOPClassUIDs) / sizeof(const char*)) - 1;


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
    if (name == NULL) return NULL;
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

/*
** There is no implementation of gethostid() and we cannot implement it in
** terms of sysinfo() so define a workaround.
*/
#if defined(HAVE_GETHOSTNAME) && defined(HAVE_GETHOSTBYNAME)

#ifdef HAVE_PROTOTYPE_GETHOSTID
/* CW10 has a prototype but no implementation (gethostid() is already declared extern */
long gethostid(void)
#else
static long gethostid(void)   
#endif
{
    char name[1024];
    struct hostent *hent = NULL;
    char **p = NULL;
    struct in_addr in;
    
    /*
    ** Define the hostid to be the system's main TCP/IP address.
    ** This is not perfect but it is better than nothing (i.e. using zero)
    */
    if (gethostname(name, 1024) < 0) {
        return 0;
    }
    if ((hent = gethostbyname(name)) == NULL) {
        return 0;
    }
    p = hent->h_addr_list;
    if (p && *p) {
        (void) memcpy(&in.s_addr, *p, sizeof(in.s_addr));
        return (long)in.s_addr;
    }
    return 0;
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

inline static long
forcePositive(long i)
{
    return (i<0)?(-i):(i);
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

    sprintf(buf, ".%ld", forcePositive(gethostid()));
    addUIDComponent(uid, buf);

    sprintf(buf, ".%ld", forcePositive(getpid()));
    addUIDComponent(uid, buf);

    sprintf(buf, ".%ld", forcePositive(time(NULL)));
    addUIDComponent(uid, buf);

    sprintf(buf, ".%ld", forcePositive(counterOfCurrentUID++));
    addUIDComponent(uid, buf);

    return uid;
}


/*
** CVS/RCS Log:
** $Log: dcuid.cc,v $
** Revision 1.10  1997-09-22 14:56:56  hewett
** Added a global list of Image SOP Classes UIDs (dcmImageSOPClassUIDs)
** which is used by dcmgpdir to identify SOP Classes which can be
** referenced from a DICOMDIR IMAGE record.
**
** Revision 1.9  1997/05/13 13:54:27  hewett
** Added UIDs and data dictionary attributes for the draft supplements 12 (PET),
** 13 (Queue Management), 15 (Visible Light), 17 (Modality Performed Procedure
** Step), 22 (User Preference LUT) and 24 (Print Storage).  Updated UID tables
** so that recompiled Storage SCP/SCU's will handle the new SOPs.
**
** Revision 1.8  1997/01/13 15:50:49  hewett
** Fixed bug when creating unique identifers.  No check was made to ensure
** that negative numbers never appeared in a UID.  Also added an
** implementation of a simple gethostid() function for systems which
** lack this function (e.g. Mac & Windows).  The implementation requires
** TCP/IP to be available.
**
** Revision 1.7  1996/09/24 16:00:59  hewett
** Added SOP Class UIDs for Radiotherapy Objects.
** Added a separate table of Storage SOP Class UIDs (usefull during
** association negotiation).
**
** Revision 1.6  1996/04/19 12:46:21  hewett
** Added UID for RLE Lossless transfer syntax
**
** Revision 1.5  1996/03/22 12:42:22  andreas
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
