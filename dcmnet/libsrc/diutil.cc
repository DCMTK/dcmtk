/*
**  Copyright (C) 1993/1994, OFFIS, Oldenburg University and CERIUM
**  
**  This software and supporting documentation were
**  developed by 
**  
**    Institut OFFIS
**    Bereich Kommunikationssysteme
**    Westerstr. 10-12
**    26121 Oldenburg, Germany
**    
**    Fachbereich Informatik
**    Abteilung Prozessinformatik
**    Carl von Ossietzky Universitaet Oldenburg 
**    Ammerlaender Heerstr. 114-118
**    26111 Oldenburg, Germany
**    
**    CERIUM
**    Laboratoire SIM
**    Faculte de Medecine
**    2 Avenue du Pr. Leon Bernard
**    35043 Rennes Cedex, France
**  
**  for CEN/TC251/WG4 as a contribution to the Radiological 
**  Society of North America (RSNA) 1993 Digital Imaging and 
**  Communications in Medicine (DICOM) Demonstration.
**  
**  THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER OFFIS,
**  OLDENBURG UNIVERSITY NOR CERIUM MAKE ANY WARRANTY REGARDING 
**  THE SOFTWARE, ITS PERFORMANCE, ITS MERCHANTABILITY OR 
**  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER 
**  DISEASES OR ITS CONFORMITY TO ANY SPECIFICATION.  THE 
**  ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF THE SOFTWARE   
**  IS WITH THE USER. 
**  
**  Copyright of the software and supporting documentation
**  is, unless otherwise stated, jointly owned by OFFIS,
**  Oldenburg University and CERIUM and free access is hereby
**  granted as a license to use this software, copy this
**  software and prepare derivative works based upon this
**  software. However, any distribution of this software
**  source code or supporting documentation or derivative
**  works (source code and supporting documentation) must
**  include the three paragraphs of this copyright notice. 
** 
*/
/*
**
** Author: Andrew Hewett		Created: 11-08-93
** 
** Module: diutil
**
** Purpose: 
**     This file contains the interface to  
**     some general useful dicom utility routines
**
** Module Prefix: DU_
**
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1997-05-13 14:38:02 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/diutil.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/



#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNIX_H
#if defined(macintosh) && defined (HAVE_WINSOCK_H)
/* unix.h defines timeval incompatible with winsock.h */
#define timeval _UNWANTED_timeval
#endif
#include <unix.h>	/* for unlink() under Metrowerks C++ (Macintosh) */
#undef timeval
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_STAT_H
#include <stat.h>
#endif

#include <ctype.h>	/* for isspace() */

#include "diutil.h"

#include "dcdatset.h"
#include "dcfilefo.h"
#include "dcdeftag.h"
#include "dcuid.h"

/*
 * Private Types
 */

typedef struct {
    char *sopClass;
    char *modality;
    unsigned long averageSize;	/* can be way, way out */
} DU_Modality;


/*
 * Private Globals
 */

/*
** The modalities table defines a short character code for each
** Storage SOP Class for use in filenames.
** It also gives a typical size for each SOP Instance.  This will
** ususally be way out, but is useful in user interfaces to give an
** idea of progress when receiving an image (C-STORE does not indicate 
** the size of an image being transmitted).
*/
static DU_Modality modalities[] = {
    { UID_ComputedRadiographyImageStorage,        	"CR", 2 * 1024 * 1024 },
    { UID_CTImageStorage,                         	"CT", 2 * 512 * 512 },
    { UID_MRImageStorage,                         	"MR", 2 * 256 * 256 },
    { UID_NuclearMedicineImageStorage,            	"NM", 2 * 64 * 64 },
    { UID_RETIRED_NuclearMedicineImageStorage,    	"NMr", 2 * 64 * 64 },
    { UID_UltrasoundImageStorage,         		"US", 1 * 512 * 512 },
    { UID_RETIRED_UltrasoundImageStorage,         	"USr", 1 * 512 * 512 },
    { UID_UltrasoundMultiframeImageStorage,      	"US", 1 * 512 * 512 },
    { UID_RETIRED_UltrasoundMultiframeImageStorage,     "USr", 1 * 512 * 512 },
    { UID_XRayAngiographicImageStorage,             	"XA", 2 * 512 * 512 },
    { UID_XRayAngiographicBiPlaneImageStorage,          "XB", 2 * 512 * 512 },
    { UID_XRayFluoroscopyImageStorage,           	"RF", 2 * 512 * 512 },
    { UID_SecondaryCaptureImageStorage,             	"SC", 2 * 512 * 512 },
    { UID_StandaloneOverlayStorage,     		"OV", 512 * 512 },
    { UID_StandaloneCurveStorage,      			"CV", 4096 },
    { UID_StandaloneModalityLUTStorage,           	"ML", 4096*2 },
    { UID_StandaloneVOILUTStorage,                	"VO", 4096*2 },
    /* I've no idea how large the RadioTherapy objects typically are */
    { UID_RTImageStorage,				"RI", 4096 },
    { UID_RTDoseStorage,				"RD", 4096 },
    { UID_RTStructureSetStorage,			"RS", 4096 },
    { UID_RTPlanStorage,				"RP", 4096 },
    /* I've no idea how large PET objects typically are */
    { UID_PETImageStorage, 				"PI", 512*512*2 },
    { UID_PETCurveStorage, 				"PC", 4096 },
    /* I've no idea how large Visible Light objects typically are */
    { UID_VLImageStorage, 				"VL", 512*512*2 },
    { UID_VLMultiFrameImageStorage, 			"VM", 512*512*2*5 },
    /* How big can Print Storage Objects be? */
    { UID_BasicGrayscalePrintStorageSOPClass, 		"PBG", 4096 },
    { UID_BasicColorPrintStorageSOPClass,		"PBC", 4096 },
    { UID_ReferencedGrayscalePrintStorageSOPClass, 	"PRG", 4096 },
    { UID_ReferencedColorPrintStorageSOPClass, 		"PRC", 4096 }

};

static char staticBuf[256];

/*
 * Public Function Prototypes
 */


const 
char* DU_sopClassToModality(const char *sopClassUID)
{
    int i;

    if (sopClassUID == NULL) return NULL;

    for (i = 0; i < (int)DIM_OF(modalities); i++) {
	if (strcmp(modalities[i].sopClass, sopClassUID) == 0) {
	    return modalities[i].modality;
	}
    }

    return NULL;
}

BOOLEAN 
DU_isStorageSOPClass(const char *sopClassUID)
{
    int i = 0;

    if (sopClassUID == NULL) return FALSE;

    for (i=0; i < (int)DIM_OF(modalities); i++) {
	if (strcmp(modalities[i].sopClass, sopClassUID) == 0) {
	    return TRUE;
	}
    }

    return FALSE;
}

unsigned long
DU_guessModalityBytes(const char *sopClassUID)
{
    unsigned long nbytes = 131072;	/* default: 256*256*2 bytes */
    int i, found=0;

    if (sopClassUID == NULL) return nbytes;

    for (i = 0; !found && i < (int)DIM_OF(modalities); i++) {
	found = (strcmp(modalities[i].sopClass, sopClassUID) == 0);
	if (found) nbytes = modalities[i].averageSize;
    }

    return nbytes;
}

char* 
DU_stripTrailingSpaces(char *s)
{
    int i, n;

    if (s == NULL) return s;
    
    n = strlen(s);
    for (i = n - 1; i >= 0 && isspace(s[i]); i--)
	s[i] = '\0';
    return s;
}

char*
DU_stripLeadingSpaces(char *s)
{
    int i, j, n;

    if (s == NULL) return s;
    n = strlen(s);
    if (n == 0) return s;
    if (!isspace(s[0])) return s;	/* no leading space */
    
    /* first non-space */
    for (i=0; i<n && isspace(s[i]); i++)
        /* do nothing, just iterate */
	;
    if (i<n) {
        /* found non-space, pull to front (inclusive '\0') */
        for (j=i; j<=n; j++) {
	    s[j-i] = s[j];
	}
    } else {
        /* all spaces */
	s[0] = '\0';
    }
    return s;
}

char*
DU_stripLeadingAndTrailingSpaces(char *s)
{
    DU_stripLeadingSpaces(s);
    DU_stripTrailingSpaces(s);
    return s;
}

BOOLEAN
DU_getStringDOElement(DcmItem *obj, DcmTagKey t, char *s)
{
    DcmByteString *elem;
    DcmStack stack;
    E_Condition ec = EC_Normal;
    char* aString;
    
    ec = obj->search(t, stack);
    elem = (DcmByteString*) stack.top();
    if (ec == EC_Normal && elem != NULL) {
	if (elem->getLength() == 0) {
	    s[0] = '\0';
        } else {
            ec =  elem->getString(aString);
            strcpy(s, aString);
        }
    }
    return (ec == EC_Normal);
}

BOOLEAN
DU_putStringDOElement(DcmItem *obj, DcmTagKey t, char *s)
{
    E_Condition ec = EC_Normal;
    DcmElement *e = NULL;
    DcmTag tag(t);
    
    ec = newDicomElement(e, tag);
    if (ec == EC_Normal && s != NULL) {
        ec = e->putString(s);
    }
    if (ec == EC_Normal) {
        ec = obj->insert(e, TRUE);
    }

    return (ec == EC_Normal);
}

BOOLEAN
DU_getShortDOElement(DcmItem *obj, DcmTagKey t, Uint16 *us)
{
    DcmElement *elem;
    DcmStack stack;
    E_Condition ec = EC_Normal;

    ec = obj->search(t, stack);
    elem = (DcmElement*) stack.top();
    if (ec == EC_Normal && elem != NULL) {
        ec = elem->getUint16(*us, 0);
    }

    return (ec == EC_Normal);
}

BOOLEAN
DU_putShortDOElement(DcmItem *obj, DcmTagKey t, Uint16 us)
{
    E_Condition ec = EC_Normal;
    DcmElement *e = NULL;
    DcmTag tag(t);
    
    ec = newDicomElement(e, tag);
    if (ec == EC_Normal) {
        ec = e->putUint16(us);
    }
    if (ec == EC_Normal) {
        ec = obj->insert(e, TRUE);
    }
    return (ec == EC_Normal);
}

BOOLEAN
DU_findSOPClassAndInstanceInDataSet(DcmItem *obj,
			      char* sopClass, char* sopInstance)
{
#ifdef TOLERATE_SPACE_PADDED_UIDS
    int slength;
    BOOLEAN result;
    result = (DU_getStringDOElement(obj, DCM_SOPClassUID, sopClass) &&
	DU_getStringDOElement(obj, DCM_SOPInstanceUID, sopInstance));
    /* gracefully correct space-padded UID strings */
    if ((0 < (slength=strlen(sopClass)))&&(sopClass[slength-1]==' '))
        sopClass[slength-1]=0;
    if ((0 < (slength=strlen(sopInstance)))&&(sopInstance[slength-1]==' '))
        sopInstance[slength-1]=0;
    return result;
#else
    return (DU_getStringDOElement(obj, DCM_SOPClassUID, sopClass) &&
	DU_getStringDOElement(obj, DCM_SOPInstanceUID, sopInstance));
#endif
}

BOOLEAN
DU_findSOPClassAndInstanceInFile(const char *fname,
			      char* sopClass, char* sopInstance)
{
    if (fname == NULL) {
	return FALSE;
    }

    DcmFileStream istrm(fname, DCM_ReadMode);
    if ( istrm.Fail() )
        return FALSE;

    DcmFileFormat ff;
    ff.read(istrm, EXS_Unknown, EGL_withGL );

    /* look in the meta-header first */
    BOOLEAN found = DU_findSOPClassAndInstanceInDataSet(
        ff.getMetaInfo(), sopClass, sopInstance);

    if (!found) {
        found = DU_findSOPClassAndInstanceInDataSet(
            ff.getDataset(), sopClass, sopInstance);
    }
    
    return found;
}


unsigned long
DU_fileSize(const char *fname)
{
    struct stat s;
    unsigned long nbytes = 0;

    if (stat(fname, &s) == 0) {
	nbytes = s.st_size;
    }
    return nbytes;
}

const char *
DU_cstoreStatusString(Uint16 statusCode)
{
    const char *s = NULL;

    switch (statusCode) {
    case STATUS_Success:
	s = "Success";
	break;
    case STATUS_STORE_Warning_CoersionOfDataElements:
	s = "Warning: CoersionOfDataElements";
	break;
    case STATUS_STORE_Warning_DataSetDoesNotMatchSOPClass:
	s = "Warning: DataSetDoesNotMatchSOPClass";
	break;
    case STATUS_STORE_Warning_ElementsDiscarded:
	s = "Warning: ElementsDiscarded";
	break;
    }
    if (s)
	return s;

    switch (statusCode & 0xff00) {	/* high byte significant */
    case STATUS_STORE_Refused_OutOfResources:	/* high byte */
	s = "Refused: OutOfResources";
	break;
    case STATUS_STORE_Refused_SOPClassNotSupported:	/* high byte */
	s = "Error: SOPClassNotSupported";
	break;
    case STATUS_STORE_Error_DataSetDoesNotMatchSOPClass:	/* high byte */
	s = "Error: DataSetDoesNotMatchSOPClass";
	break;
    }
    if (s)
	return s;

    switch (statusCode & 0xf000) {	/* high nibble significant */
    case STATUS_STORE_Error_CannotUnderstand:	/* high nibble */
	s = "Error: CannotUnderstand";
	break;
    }

    if (s == NULL) {
	sprintf(staticBuf, "Unknown Status: 0x%x",
		(unsigned int)statusCode);
	s = staticBuf;
    }
    return s;
}


const char *
DU_cfindStatusString(Uint16 statusCode)
{
    const char *s = NULL;

    switch (statusCode) {
    case STATUS_Success:
	s = "Success";
	break;
    case STATUS_Pending:
	s = "Pending";
	break;
    case STATUS_FIND_Refused_OutOfResources:
	s = "Refused: OutOfResources";
	break;
    case STATUS_FIND_Refused_SOPClassNotSupported:
	s = "Refused: SOPClassNotSupported";
	break;
    case STATUS_FIND_Failed_IdentifierDoesNotMatchSOPClass:
	s = "Failed: IdentifierDoesNotMatchSOPClass";
	break;
    case STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest:
	s = "Cancel: MatchingTerminatedDueToCancelRequest";
	break;
    case STATUS_FIND_Pending_WarningUnsupportedOptionalKeys:
	s = "Pending: WarningUnsupportedOptionalKeys";
	break;
    }
    if (s)
	return s;

    switch (statusCode & 0xf000) {	/* high nibble significant */
    case STATUS_FIND_Failed_UnableToProcess:	/* high nibble */
	s = "Failed: UnableToProcess";
	break;
    }

    if (s == NULL) {
	sprintf(staticBuf, "Unknown Status: 0x%x",
		(unsigned int)statusCode);
	s = staticBuf;
    }
    return s;
}

const char *
DU_cmoveStatusString(Uint16 statusCode)
{
    const char *s = NULL;

    switch (statusCode) {
    case STATUS_Success:
	s = "Success";
	break;
    case STATUS_Pending:
	s = "Pending";
	break;
    case STATUS_MOVE_Refused_OutOfResourcesNumberOfMatches:
	s = "Refused: OutOfResourcesNumberOfMatches";
	break;
    case STATUS_MOVE_Refused_OutOfResourcesSubOperations:
	s = "Refused: OutOfResourcesSubOperations";
	break;
    case STATUS_MOVE_Failed_SOPClassNotSupported:
	s = "Failed: SOPClassNotSupported";
	break;
    case STATUS_MOVE_Failed_MoveDestinationUnknown:
	s = "Failed: MoveDestinationUnknown";
	break;
    case STATUS_MOVE_Failed_IdentifierDoesNotMatchSOPClass:
	s = "Failed: IdentifierDoesNotMatchSOPClass";
	break;
    case STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication:
	s = "Cancel: SubOperationsTerminatedDueToCancelIndication";
	break;
    case STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures:
	s = "Warning: SubOperationsCompleteOneOrMoreFailures";
	break;

    }
    if (s)
	return s;

    switch (statusCode & 0xf000) {	/* high nibble significant */
    case STATUS_MOVE_Failed_UnableToProcess:	/* high nibble */
	s = "Failed: UnableToProcess";
	break;
    }

    if (s == NULL) {
	sprintf(staticBuf, "Unknown Status: 0x%x",
		(unsigned int)statusCode);
	s = staticBuf;
    }
    return s;
}

const char *
DU_cgetStatusString(Uint16 statusCode)
{
    
    sprintf(staticBuf,  "Unknown Status: 0x%x", (unsigned int)statusCode);
    return staticBuf;
}

/*
** CVS Log
** $Log: diutil.cc,v $
** Revision 1.5  1997-05-13 14:38:02  hewett
** Added UIDs for the draft supplements 12 (PET), 13 (Queue Management),
** 15 (Visible Light), 17 (Modality Performed Procedure Step), 22 (User
** Preference LUT) and 24 (Print Storage).
**
** Revision 1.4  1997/04/18 08:40:32  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   E_Condition get(Uint16 & value, const unsigned long pos);
**   becomes
**   E_Condition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.3  1997/02/06 12:21:15  hewett
** Updated for Macintosh CodeWarrior 11.  Corrected for incompatibilities
** in the timeval structure between unix.h and winsock.h
**
** Revision 1.2  1996/09/24 15:59:20  hewett
** Added Support for the SOP Class UIDs of Radiotherapy Objects.
**
** Revision 1.1.1.1  1996/03/26 18:38:46  hewett
** Initial Release.
**
**
*/
