/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were partly developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  For further copyrights, see the following paragraphs.
 *
 */

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
** Author: Andrew Hewett                Created: 11-08-93
**
** Module: diutil
**
** Purpose:
**     This file contains the interface to
**     some general useful dicom utility routines
**
** Module Prefix: DU_
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 2010-12-01 08:26:36 $
** CVS/RCS Revision:    $Revision: 1.30 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
*/



#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

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

#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcbytstr.h"

static char staticBuf[256];

OFLogger DCM_dcmnetGetLogger()
{
    static OFLogger DCM_dcmnetLogger = OFLog::getLogger("dcmtk.dcmnet");
    return DCM_dcmnetLogger;
}

#define TO_UCHAR(s) OFstatic_cast(unsigned char, (s))
char*
DU_stripTrailingSpaces(char *s)
{
    int i, n;

    if (s == NULL) return s;

    n = strlen(s);
    for (i = n - 1; i >= 0 && isspace(TO_UCHAR(s[i])); i--)
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
    for (i=0; i<n && isspace(TO_UCHAR(s[i])); i++)
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
#undef TO_UCHAR

OFBool
DU_getStringDOElement(DcmItem *obj, DcmTagKey t, char *s)
{
    DcmByteString *elem;
    DcmStack stack;
    OFCondition ec = EC_Normal;
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

OFBool
DU_putStringDOElement(DcmItem *obj, DcmTagKey t, const char *s)
{
    OFCondition ec = EC_Normal;
    DcmElement *e = NULL;
    DcmTag tag(t);

    ec = newDicomElement(e, tag);
    if (ec == EC_Normal && s != NULL) {
        ec = e->putString(s);
    }
    if (ec == EC_Normal) {
        ec = obj->insert(e, OFTrue);
    }

    return (ec == EC_Normal);
}

OFBool
DU_getShortDOElement(DcmItem *obj, DcmTagKey t, Uint16 *us)
{
    DcmElement *elem;
    DcmStack stack;
    OFCondition ec = EC_Normal;

    ec = obj->search(t, stack);
    elem = (DcmElement*) stack.top();
    if (ec == EC_Normal && elem != NULL) {
        ec = elem->getUint16(*us, 0);
    }

    return (ec == EC_Normal);
}

OFBool
DU_putShortDOElement(DcmItem *obj, DcmTagKey t, Uint16 us)
{
    OFCondition ec = EC_Normal;
    DcmElement *e = NULL;
    DcmTag tag(t);

    ec = newDicomElement(e, tag);
    if (ec == EC_Normal) {
        ec = e->putUint16(us);
    }
    if (ec == EC_Normal) {
        ec = obj->insert(e, OFTrue);
    }
    return (ec == EC_Normal);
}

OFBool
DU_findSOPClassAndInstanceInDataSet(
  DcmItem *obj,
  char* sopClass,
  char* sopInstance,
  OFBool tolerateSpacePaddedUIDs)
{
    OFBool result = (DU_getStringDOElement(obj, DCM_SOPClassUID, sopClass) &&
	DU_getStringDOElement(obj, DCM_SOPInstanceUID, sopInstance));

    if (tolerateSpacePaddedUIDs)
    {
        /* gracefully correct space-padded UID strings */
        int slength;

        if ((0 < (slength=strlen(sopClass)))&&(sopClass[slength-1]==' '))
            sopClass[slength-1]=0;
        if ((0 < (slength=strlen(sopInstance)))&&(sopInstance[slength-1]==' '))
            sopInstance[slength-1]=0;
    }
    return result;
}

OFBool
DU_findSOPClassAndInstanceInFile(
  const char *fname,
  char* sopClass,
  char* sopInstance,
  OFBool tolerateSpacePaddedUIDs)
{
    DcmFileFormat ff;
    if (! ff.loadFile(fname, EXS_Unknown, EGL_noChange).good())
        return OFFalse;

    /* look in the meta-header first */
    OFBool found = DU_findSOPClassAndInstanceInDataSet(
        ff.getMetaInfo(), sopClass, sopInstance, tolerateSpacePaddedUIDs);

    if (!found) {
        found = DU_findSOPClassAndInstanceInDataSet(
            ff.getDataset(), sopClass, sopInstance, tolerateSpacePaddedUIDs);
    }

    return found;
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

const char *
DU_ncreateStatusString(Uint16 statusCode)
{
  const char *s = NULL;

  switch (statusCode) {
    case STATUS_Success:
	    s = "Success";
	    break;
    case STATUS_N_ClassInstanceConflict:
      s = "Failure: ClassInstanceConflict";
	    break;
    case STATUS_N_DuplicateInvocation:
	    s = "Failure: DuplicateInvocation";
	    break;
    case STATUS_N_DuplicateSOPInstance:
	    s = "Failure: DuplicateSOPInstance";
	    break;
    case STATUS_N_InvalidAttributeValue:
	    s = "Failure: InvalidAttributeValue";
	    break;
    case STATUS_N_InvalidObjectInstance:
	    s = "Failure: InvalidObjectInstance";
	    break;
    case STATUS_N_MissingAttribute:
	    s = "Failure: MissingAttribute";
	    break;
    case STATUS_N_MissingAttributeValue:
	    s = "Failure: MissingAttributeValue";
	    break;
    case STATUS_N_MistypedArgument:
	    s = "Failure: MistypedArgument";
	    break;
    case STATUS_N_NoSuchAttribute:
	    s = "Failure: NoSuchAttribute";
	    break;
    case STATUS_N_NoSuchSOPClass:
	    s = "Failure: NoSuchSOPClass";
	    break;
    case STATUS_N_NoSuchObjectInstance:
	    s = "Failure: NoSuchObjectInstance";
	    break;
    case STATUS_N_ProcessingFailure:
	    s = "Failure: ProcessingFailure";
	    break;
    case STATUS_N_ResourceLimitation:
	    s = "Failure: ResourceLimitation";
	    break;
    case STATUS_N_UnrecognizedOperation:
	    s = "Failure: UnrecognizedOperation";
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
  	sprintf(staticBuf, "Unknown Status: 0x%x", (unsigned int)statusCode);
	  s = staticBuf;
  }
  return s;
}

const char *
DU_ngetStatusString(Uint16 statusCode)
{
  const char *s = NULL;

  switch (statusCode) {
    case STATUS_Success:
	    s = "Success";
	    break;
    case STATUS_N_AttributeListError:
      s = "Warning: AttributeListError";
	    break;
    case STATUS_N_ClassInstanceConflict:
      s = "Failure: ClassInstanceConflict";
	    break;
    case STATUS_N_DuplicateInvocation:
	    s = "Failure: DuplicateInvocation";
	    break;
    case STATUS_N_InvalidObjectInstance:
	    s = "Failure: InvalidObjectInstance";
	    break;
    case STATUS_N_MistypedArgument:
	    s = "Failure: MistypedArgument";
	    break;
    case STATUS_N_NoSuchSOPClass:
	    s = "Failure: NoSuchSOPClass";
	    break;
    case STATUS_N_NoSuchObjectInstance:
	    s = "Failure: NoSuchObjectInstance";
	    break;
    case STATUS_N_ProcessingFailure:
	    s = "Failure: ProcessingFailure";
	    break;
    case STATUS_N_ResourceLimitation:
	    s = "Failure: ResourceLimitation";
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
  	sprintf(staticBuf, "Unknown Status: 0x%x", (unsigned int)statusCode);
	  s = staticBuf;
  }
  return s;
}

const char *
DU_nsetStatusString(Uint16 statusCode)
{
  const char *s = NULL;

  switch (statusCode) {
    case STATUS_Success:
	    s = "Success";
	    break;
    case STATUS_N_ClassInstanceConflict:
      s = "Failure: ClassInstanceConflict";
	    break;
    case STATUS_N_DuplicateInvocation:
	    s = "Failure: DuplicateInvocation";
	    break;
    case STATUS_N_InvalidAttributeValue:
	    s = "Failure: InvalidAttributeValue";
	    break;
    case STATUS_N_MistypedArgument:
	    s = "Failure: MistypedArgument";
	    break;
    case STATUS_N_InvalidObjectInstance:
	    s = "Failure: InvalidObjectInstance";
	    break;
    case STATUS_N_MissingAttributeValue:
	    s = "Failure: MissingAttributeValue";
	    break;
    case STATUS_N_NoSuchAttribute:
	    s = "Failure: NoSuchAttribute";
	    break;
    case STATUS_N_NoSuchSOPClass:
	    s = "Failure: NoSuchSOPClass";
	    break;
    case STATUS_N_NoSuchObjectInstance:
	    s = "Failure: NoSuchObjectInstance";
	    break;
    case STATUS_N_ProcessingFailure:
	    s = "Failure: ProcessingFailure";
	    break;
    case STATUS_N_ResourceLimitation:
	    s = "Failure: ResourceLimitation";
	    break;
    case STATUS_N_UnrecognizedOperation:
	    s = "Failure: UnrecognizedOperation";
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
  	sprintf(staticBuf, "Unknown Status: 0x%x", (unsigned int)statusCode);
	  s = staticBuf;
  }
  return s;
}

const char *
DU_nactionStatusString(Uint16 statusCode)
{
  const char *s = NULL;

  switch (statusCode) {
    case STATUS_Success:
	    s = "Success";
	    break;
    case STATUS_N_ClassInstanceConflict:
      s = "Failure: ClassInstanceConflict";
	    break;
    case STATUS_N_DuplicateInvocation:
	    s = "Failure: DuplicateInvocation";
	    break;
    case STATUS_N_InvalidArgumentValue:
	    s = "Failure: InvalidArgumentValue";
	    break;
     case STATUS_N_InvalidObjectInstance:
	    s = "Failure: InvalidObjectInstance";
	    break;
     case STATUS_N_MistypedArgument:
	    s = "Failure: MistypedArgument";
	    break;
     case STATUS_N_NoSuchAction:
	    s = "Failure: NoSuchAction";
	    break;
     case STATUS_N_NoSuchArgument:
	    s = "Failure: NoSuchArgument";
	    break;
    case STATUS_N_NoSuchSOPClass:
	    s = "Failure: NoSuchSOPClass";
	    break;
    case STATUS_N_NoSuchObjectInstance:
	    s = "Failure: NoSuchObjectInstance";
	    break;
    case STATUS_N_ProcessingFailure:
	    s = "Failure: ProcessingFailure";
	    break;
    case STATUS_N_ResourceLimitation:
	    s = "Failure: ResourceLimitation";
	    break;
    case STATUS_N_UnrecognizedOperation:
	    s = "Failure: UnrecognizedOperation";
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
  	sprintf(staticBuf, "Unknown Status: 0x%x", (unsigned int)statusCode);
	  s = staticBuf;
  }
  return s;
}

const char *
DU_ndeleteStatusString(Uint16 statusCode)
{
  const char *s = NULL;

  switch (statusCode) {
    case STATUS_Success:
	    s = "Success";
	    break;
    case STATUS_N_ClassInstanceConflict:
      s = "Failure: ClassInstanceConflict";
	    break;
    case STATUS_N_DuplicateInvocation:
	    s = "Failure: DuplicateInvocation";
	    break;
     case STATUS_N_InvalidObjectInstance:
	    s = "Failure: InvalidObjectInstance";
	    break;
     case STATUS_N_MistypedArgument:
	    s = "Failure: MistypedArgument";
	    break;
    case STATUS_N_NoSuchSOPClass:
	    s = "Failure: NoSuchSOPClass";
	    break;
    case STATUS_N_NoSuchObjectInstance:
	    s = "Failure: NoSuchObjectInstance";
	    break;
    case STATUS_N_ProcessingFailure:
	    s = "Failure: ProcessingFailure";
	    break;
    case STATUS_N_ResourceLimitation:
	    s = "Failure: ResourceLimitation";
	    break;
    case STATUS_N_UnrecognizedOperation:
	    s = "Failure: UnrecognizedOperation";
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
  	sprintf(staticBuf, "Unknown Status: 0x%x", (unsigned int)statusCode);
	  s = staticBuf;
  }
  return s;
}

const char *
DU_neventReportStatusString(Uint16 statusCode)
{
  const char *s = NULL;

  switch (statusCode) {
    case STATUS_Success:
	    s = "Success";
	    break;
    case STATUS_N_ClassInstanceConflict:
      s = "Failure: ClassInstanceConflict";
	    break;
    case STATUS_N_DuplicateInvocation:
	    s = "Failure: DuplicateInvocation";
	    break;
    case STATUS_N_InvalidArgumentValue:
	    s = "Failure: InvalidArgumentValue";
	    break;
    case STATUS_N_InvalidObjectInstance:
	    s = "Failure: InvalidObjectInstance";
	    break;
    case STATUS_N_MistypedArgument:
	    s = "Failure: MistypedArgument";
	    break;
    case STATUS_N_NoSuchArgument:
	    s = "Failure: NoSuchArgument";
	    break;
    case STATUS_N_NoSuchEventType:
	    s = "Failure: NoSuchEventType";
	    break;
    case STATUS_N_NoSuchSOPClass:
	    s = "Failure: NoSuchSOPClass";
	    break;
    case STATUS_N_NoSuchObjectInstance:
	    s = "Failure: NoSuchObjectInstance";
	    break;
    case STATUS_N_ProcessingFailure:
	    s = "Failure: ProcessingFailure";
	    break;
    case STATUS_N_ResourceLimitation:
	    s = "Failure: ResourceLimitation";
	    break;
    case STATUS_N_UnrecognizedOperation:
	    s = "Failure: UnrecognizedOperation";
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
  	sprintf(staticBuf, "Unknown Status: 0x%x", (unsigned int)statusCode);
	  s = staticBuf;
  }
  return s;
}


/*
** CVS Log
** $Log: diutil.cc,v $
** Revision 1.30  2010-12-01 08:26:36  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.29  2010-10-20 07:41:36  uli
** Made sure isalpha() & friends are only called with valid arguments.
**
** Revision 1.28  2010-10-14 13:14:28  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.27  2010-05-21 11:47:52  uli
** Replaced DU_fileSize() with OFStandard::getFileSize().
**
** Revision 1.26  2010-03-01 09:08:49  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.25  2009-11-18 11:53:59  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.24  2007-07-13 12:24:51  onken
** Fixed some status code for DIMSE-N.
**
** Revision 1.23  2007/07/12 12:18:42  onken
** Added status codes and corresponding printing routines for DIMSE-N.
**
** Revision 1.22  2005/12/08 15:44:47  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.21  2002/11/27 13:04:43  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.20  2002/11/25 18:00:32  meichel
** Converted compile time option to leniently handle space padded UIDs
**   in the Storage Service Class into command line / config file option.
**
** Revision 1.19  2002/08/20 12:21:25  meichel
** Adapted code to new loadFile and saveFile methods, thus removing direct
**   use of the DICOM stream classes.
**
** Revision 1.18  2001/09/26 12:29:02  meichel
** Implemented changes in dcmnet required by the adaptation of dcmdata
**   to class OFCondition.  Removed some unused code.
**
** Revision 1.17  2000/02/23 15:12:40  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.16  2000/02/03 11:50:12  meichel
** Moved UID related functions from dcmnet (diutil.h) to dcmdata (dcuid.h)
**   where they belong. Renamed access functions to dcmSOPClassUIDToModality
**   and dcmGuessModalityBytes.
**
** Revision 1.15  2000/02/01 10:24:11  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.14  1999/07/14 12:03:38  meichel
** Updated data dictionary for supplement 29, 39, 33_lb, CP packet 4 and 5.
**   Corrected dcmtk applications for changes in attribute name constants.
**
** Revision 1.13  1999/06/10 10:56:37  meichel
** Adapted imagectn to new command line option scheme.
**   Added support for Patient/Study Only Q/R model and C-GET (experimental).
**
** Revision 1.12  1999/03/29 10:13:22  meichel
** Updated list of SOP class name prefixes and estimated sizes
**
** Revision 1.11  1998/10/26 13:07:48  meichel
** Added Grayscale Softcopy Presentation State to DIMSE helper functions.
**
** Revision 1.10  1998/02/25 14:30:01  hewett
** Updated UID information to reflect changes in
** Supplement 24 (Stored Print Related SOP Classes).  Some UIDs have
** changed between the Letter Ballot version and the Final Text version.
**
** Revision 1.9  1998/02/06 09:09:15  hewett
** Updated support for Supplements 15 (Visible Light),
** 16 (Postscript Print Management), 17 (Modality Performed Procedure Step),
** 22 (Presentation Look Up Table (LUT)), 23 (Structured Reporting),
** 24 (Stored Print), 30 (Waveform Interchange).
**
** Revision 1.8  1998/01/14 14:37:15  hewett
** Added basic support for the Structured Reporting (SR) SOP Classes.
**
** Revision 1.7  1997/07/21 08:47:19  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.6  1997/05/16 08:31:38  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.5  1997/05/13 14:38:02  hewett
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
**   OFCondition get(Uint16 & value, const unsigned long pos);
**   becomes
**   OFCondition getUint16(Uint16 & value, const unsigned long pos);
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
