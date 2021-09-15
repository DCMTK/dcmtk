/*
 *
 *  Copyright (C) 1994-2021, OFFIS e.V.
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
*/



#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_UNIX_H
#if defined(macintosh) && defined (HAVE_WINSOCK_H)
/* unix.h defines timeval incompatible with winsock.h */
#define timeval _UNWANTED_timeval
#endif
#include <unix.h>       /* for unlink() under Metrowerks C++ (Macintosh) */
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

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcbytstr.h"

static char staticBuf[256];

OFLogger DCM_dcmnetLogger = OFLog::getLogger(DCMNET_LOGGER_NAME);

#define TO_UCHAR(s) OFstatic_cast(unsigned char, (s))
void
DU_stripTrailingSpaces(char *s)
{
    int i, n;

    if (s)
    {
        n = OFstatic_cast(int, strlen(s));
        for (i = n - 1; i >= 0 && isspace(TO_UCHAR(s[i])); i--)
            s[i] = '\0';
    }
}

void
DU_stripLeadingSpaces(char *s)
{
    int i, j, n;

    if (s == NULL) return;
    n = OFstatic_cast(int, strlen(s));
    if (n == 0) return;
    if (!isspace(TO_UCHAR(s[0]))) return; /* no leading space */

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
}

void
DU_stripLeadingAndTrailingSpaces(char *s)
{
    DU_stripLeadingSpaces(s);
    DU_stripTrailingSpaces(s);
}
#undef TO_UCHAR

OFBool
DU_getStringDOElement(DcmItem *obj, DcmTagKey t, char *s, size_t bufsize)
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
            if (ec == EC_Normal)
                OFStandard::strlcpy(s, aString, bufsize);
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

    ec = DcmItem::newDicomElement(e, tag);
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

    ec = DcmItem::newDicomElement(e, tag);
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
  size_t sopClassSize,
  char* sopInstance,
  size_t sopInstanceSize,
  OFBool tolerateSpacePaddedUIDs)
{
    OFBool result = (DU_getStringDOElement(obj, DCM_SOPClassUID, sopClass, sopClassSize) &&
        DU_getStringDOElement(obj, DCM_SOPInstanceUID, sopInstance, sopInstanceSize));

    if (tolerateSpacePaddedUIDs)
    {
        /* gracefully correct space-padded UID strings */
        int slength;

        if ((0 < (slength=OFstatic_cast(int, strlen(sopClass))))&&(sopClass[slength-1]==' '))
            sopClass[slength-1]=0;
        if ((0 < (slength=OFstatic_cast(int, strlen(sopInstance))))&&(sopInstance[slength-1]==' '))
            sopInstance[slength-1]=0;
    }
    return result;
}

OFBool
DU_findSOPClassAndInstanceInFile(
  const char *fname,
  char* sopClass,
  size_t sopClassSize,
  char* sopInstance,
  size_t sopInstanceSize,
  OFBool tolerateSpacePaddedUIDs)
{
    DcmFileFormat ff;
    if (! ff.loadFile(fname, EXS_Unknown, EGL_noChange).good())
        return OFFalse;

    /* look in the meta-header first */
    OFBool found = DU_findSOPClassAndInstanceInDataSet(
        ff.getMetaInfo(), sopClass, sopClassSize, sopInstance, sopInstanceSize, tolerateSpacePaddedUIDs);

    if (!found) {
        found = DU_findSOPClassAndInstanceInDataSet(
            ff.getDataset(), sopClass, sopClassSize, sopInstance, sopInstanceSize, tolerateSpacePaddedUIDs);
    }

    return found;
}

const char *
DU_cechoStatusString(Uint16 statusCode)
{
    const char *s = NULL;

    if (statusCode == STATUS_Success)
        s = "Success";
    else {
        sprintf(staticBuf, "Unknown Status: 0x%x", (unsigned int)statusCode);
        s = staticBuf;
    }
    return s;
}

const char *
DU_cstoreStatusString(Uint16 statusCode)
{
    const char *s = NULL;

    switch (statusCode) {
      case STATUS_Success:
          s = "Success";
          break;
      case STATUS_STORE_Refused_SOPClassNotSupported:
          s = "Refused: SOPClassNotSupported";
          break;
      case STATUS_STORE_Warning_CoercionOfDataElements:
          s = "Warning: CoercionOfDataElements";
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

    switch (statusCode & 0xff00) {      /* high byte significant */
      case STATUS_STORE_Refused_OutOfResources:   /* high byte */
          s = "Refused: OutOfResources";
          break;
      case STATUS_STORE_Error_DataSetDoesNotMatchSOPClass:        /* high byte */
          s = "Error: DataSetDoesNotMatchSOPClass";
          break;
    }
    if (s)
        return s;

    switch (statusCode & 0xf000) {      /* high nibble significant */
      case STATUS_STORE_Error_CannotUnderstand:   /* high nibble */
          s = "Error: CannotUnderstand";
          break;
    }

    if (s == NULL) {
        sprintf(staticBuf, "Unknown Status: 0x%x", (unsigned int)statusCode);
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

    switch (statusCode & 0xf000) {      /* high nibble significant */
    case STATUS_FIND_Failed_UnableToProcess:    /* high nibble */
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

    switch (statusCode & 0xf000) {      /* high nibble significant */
      case STATUS_MOVE_Failed_UnableToProcess:    /* high nibble */
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
DU_cgetStatusString(Uint16 statusCode)
{
    const char *s = NULL;

    switch (statusCode) {
      case STATUS_Success:
          s = "Success";
          break;
      case STATUS_Pending:
          s = "Pending";
          break;
      case STATUS_GET_Refused_OutOfResourcesNumberOfMatches:
          s = "Refused: OutOfResourcesNumberOfMatches";
          break;
      case STATUS_GET_Refused_OutOfResourcesSubOperations:
          s = "Refused: OutOfResourcesSubOperations";
          break;
      case STATUS_GET_Failed_SOPClassNotSupported:
          s = "Failed: SOPClassNotSupported";
          break;
      case STATUS_GET_Failed_IdentifierDoesNotMatchSOPClass:
          s = "Failed: IdentifierDoesNotMatchSOPClass";
          break;
      case STATUS_GET_Cancel_SubOperationsTerminatedDueToCancelIndication:
          s = "Cancel: SubOperationsTerminatedDueToCancelIndication";
          break;
      case STATUS_GET_Warning_SubOperationsCompleteOneOrMoreFailures:
          s = "Warning: SubOperationsCompleteOneOrMoreFailures";
          break;

    }
    if (s)
        return s;

    switch (statusCode & 0xf000) {      /* high nibble significant */
      case STATUS_GET_Failed_UnableToProcess:    /* high nibble */
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
      case STATUS_N_AttributeValueOutOfRange:
          s = "Warning: AttributeValueOutOfRange";
          break;
    }
    if (s)
          return s;

    switch (statusCode & 0xf000) {        /* high nibble significant */
      case STATUS_FIND_Failed_UnableToProcess:    /* high nibble */
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
      case STATUS_N_AttributeListError:
          s = "Warning: AttributeListError";
          break;
      case STATUS_N_AttributeValueOutOfRange:
          s = "Warning: AttributeValueOutOfRange";
          break;
    }
    if (s)
        return s;

    switch (statusCode & 0xf000) {        /* high nibble significant */
      case STATUS_FIND_Failed_UnableToProcess:    /* high nibble */
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
      case STATUS_N_AttributeValueOutOfRange:
          s = "Warning: AttributeValueOutOfRange";
          break;
    }
    if (s)
        return s;

    switch (statusCode & 0xf000) {        /* high nibble significant */
      case STATUS_FIND_Failed_UnableToProcess:    /* high nibble */
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

    switch (statusCode & 0xf000) {        /* high nibble significant */
      case STATUS_FIND_Failed_UnableToProcess:    /* high nibble */
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

    switch (statusCode & 0xf000) {        /* high nibble significant */
      case STATUS_FIND_Failed_UnableToProcess:    /* high nibble */
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

    switch (statusCode & 0xf000) {        /* high nibble significant */
      case STATUS_FIND_Failed_UnableToProcess:    /* high nibble */
          s = "Failed: UnableToProcess";
          break;
    }

    if (s == NULL) {
        sprintf(staticBuf, "Unknown Status: 0x%x", (unsigned int)statusCode);
        s = staticBuf;
    }
    return s;
}

void DU_logSelectResult(int selectReturnValue)
{
  if (selectReturnValue < 0)
  {
#ifdef HAVE_WINSOCK_H
    LPVOID errBuf = NULL;
    OFString err;
    // Obtain an error string from system error code
    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), OFreinterpret_cast(LPTSTR, &errBuf), 0, NULL) > 0)
    {
      err = (OFstatic_cast(const char *, errBuf));
    } else
      err = "Unknown Winsock error code";
    LocalFree(errBuf);
    DCMNET_DEBUG("Windows Socket error while waiting for incoming network data: " << err);
#else
    // POSIX interface
    char buf[256];
    DCMNET_DEBUG("Error while waiting for incoming network data: " << OFStandard::strerror(errno, buf, 256));
#endif
  }
  else if (selectReturnValue == 0)
  {
    DCMNET_TRACE("Timeout while waiting for incoming network data");
  }
  else
  {
    // This function is only meant to be used for return values <= 0, handle
    // normal case anyway
    DCMNET_TRACE("Receiving data via select()");
  }
}
