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
** Author: Andrew Hewett                Created: 03-06-93
**
** Module: association
**
** Purpose:
**      This file contains the routines which provide association management
**      for DICOM V.3 applications.  It maintains structures which describe
**      active associations and provides access to association specific
**      informtion.  Also provided are routines for performing association
**      negotiation (presentation contexts, abstract syntaxes, transfer
**      syntaxes, maximum PDU length, and other extended negotiation).
**
**      This package uses the facilities of the DICOM Upper Layer for
**      receiving/sending association requests/responses.
**
**      Each active association is represented by an T_ASC_Association
**      structure which contains all relevant information.
**
** Module Prefix: ASC_
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 2010-12-01 08:26:35 $
** CVS/RCS Revision:    $Revision: 1.59 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/

/*
** Include Files
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/assoc.h"      /* always include the module header */
#include "dcmtk/dcmnet/diutil.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CERRNO
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmnet/dcmtrans.h"

/*
** Constant Definitions
*/

#define ASC_IMPLEMENTATION_CLASS_UID_ITEM_TYPE          0x52
#define ASC_ASYNCHRONOUS_OPERATIONS_WINDOW_ITEM_TYPE    0x53
#define ASC_SCU_SCP_ROLE_SELECTION_ITEM_TYPE            0x54
#define ASC_IMPLEMENTATION_VERSION_NAME_ITEM_TYPE       0x55
#define ASC_SOP_CLASS_EXTENDED_NEGOTIATION_ITEM_TYPE    0x56

/*
** Type Definitions
*/

/* These should be defined somewhere more general */
typedef unsigned char T_Byte;
typedef unsigned short T_Length;
typedef char T_ASSOC_UID[DUL_LEN_UID + 4]; // T_UID is a macro on OSF/1
typedef char T_TITLE[DUL_LEN_TITLE + 4];
typedef T_Byte *T_Data;

/*
** Implementation Class UID Sub-Item Structure
** (A-ASSOCIATE-RQ & A-ASSOCIATE-AC)
** See Part 7, pp. 115-116
*/
typedef struct {
    T_Byte ItemType;
    T_Byte Reserved1;
    T_Length ItemLength;
    T_ASSOC_UID ImplementationClassUID;
} T_ASC_ImplementationClassUIDItem;


/*
** Implementation Version Name Sub-Item Structure
** (A-ASSOCIATE-RQ & A-ASSOCIATE-AC)
** See Part 7, pp. 116-117
*/
typedef struct {
    T_Byte ItemType;
    T_Byte Reserved1;
    T_Length ItemLength;
    T_TITLE ImplementationVersionName;
} T_ASC_ImplementationVersionNameItem;

/*
** Asynchronous Operation Window Sub-Item Structure
** (A-ASSOCIATE-RQ & A-ASSOCIATE-AC)
** See Part 7, pp. 118-121
*/
typedef struct {
    T_Byte ItemType;
    T_Byte Reserved1;
    T_Length ItemLength;
    T_Length MaximumNumberOperationsInvoked;
    T_Length MaximumNumberOperationsPerformed;
} T_ASC_AsynchronousOperationsWindowItem;

/*
** SCP/SCU Role Selection Sub-Item Structure
** (A-ASSOCIATE-RQ & A-ASSOCIATE-AC)
** See Part 7, pp. 123-124
*/
typedef struct {
    T_Byte ItemType;
    T_Byte Reserved1;
    T_Length ItemLength;
    T_Length SOPClassUIDLength;
    T_ASSOC_UID SOPClassUID;
    T_Byte SCURole;
    T_Byte SCPRole;
} T_ASC_SCPSCURoleSelectionItem;

/*
** SOP Class Extended Negotiation Sub-Item Structure
** (A-ASSOCIATE-RQ & A-ASSOCIATE-AC)
** See Part 7, pp. 125-126
*/
typedef struct {
    T_Byte ItemType;
    T_Byte Reserved1;
    T_Length ItemLength;
    T_Length SOPClassUIDLength;
    T_ASSOC_UID SOPClassUID;
    T_Data ServiceClassApplicationInformation;
} T_ASC_ExtendedNegotiationItem;


/*
** Function Bodies
*/

/*
 * Network creation/distroy wrappers.
 * The T_ASC_Network structure will be allocated/freed by
 * these routines.
 */

OFCondition
ASC_initializeNetwork(T_ASC_NetworkRole role,
                      int acceptorPort,
                      int timeout,
                      T_ASC_Network ** network,
                      unsigned long options)
{
    const char *mode;

    DUL_NETWORKKEY * netkey;

    switch (role) {
    case NET_ACCEPTOR:
        mode = DUL_AEACCEPTOR;
        break;
    case NET_REQUESTOR:
        mode = DUL_AEREQUESTOR;
        break;
    case NET_ACCEPTORREQUESTOR:
        mode = DUL_AEBOTH;
        break;
    default:
        mode = "unknown";
        break;
    }

    OFCondition cond = DUL_InitializeNetwork(mode, &acceptorPort, timeout, DUL_ORDERBIGENDIAN | options, &netkey);
    if (cond.bad()) return cond;

    *network = (T_ASC_Network *) malloc(sizeof(T_ASC_Network));
    if (*network == NULL) return EC_MemoryExhausted;
    (*network)->role = role;
    (*network)->acceptorPort = acceptorPort;
    (*network)->network = netkey;

    return EC_Normal;
}

OFCondition
ASC_dropNetwork(T_ASC_Network ** network)
{
    if (network == NULL) return EC_Normal;
    if (*network == NULL) return EC_Normal;

    OFCondition cond = DUL_DropNetwork(&(*network)->network);
    if (cond.bad()) return cond;

    free(*network);
    *network = NULL;
    return EC_Normal;
}


/*
 * Building Association parameters
 */

/* create association parameters and initialize with default values */
OFCondition
ASC_createAssociationParameters(T_ASC_Parameters ** params,
        long maxReceivePDUSize)
{

    *params = (T_ASC_Parameters *) malloc(sizeof(**params));
    if (*params == NULL) return EC_MemoryExhausted;
    bzero((char*)*params, sizeof(**params));

    strncpy((*params)->ourImplementationClassUID,
            OFFIS_IMPLEMENTATION_CLASS_UID,
            sizeof((*params)->ourImplementationClassUID)-1);
    strncpy((*params)->ourImplementationVersionName,
            OFFIS_DTK_IMPLEMENTATION_VERSION_NAME,
            sizeof((*params)->ourImplementationVersionName)-1);

    strcpy((*params)->DULparams.callingImplementationClassUID,
        (*params)->ourImplementationClassUID);
    strcpy((*params)->DULparams.callingImplementationVersionName,
        (*params)->ourImplementationVersionName);

    strncpy((*params)->DULparams.applicationContextName,
            UID_StandardApplicationContext,
            sizeof((*params)->DULparams.applicationContextName)-1);

    ASC_setAPTitles(*params,
                    "calling AP Title",
                    "called AP Title",
                    "resp. AP Title");

    /* make sure max pdv length is even */
    if ((maxReceivePDUSize % 2) != 0)
    {
      DCMNET_WARN("ASSOC: PDV receive length " << maxReceivePDUSize << " is odd (using " << (maxReceivePDUSize-1) << ")");
      maxReceivePDUSize--;
    }
    if (maxReceivePDUSize < ASC_MINIMUMPDUSIZE)
    {
      DCMNET_WARN("ASC_createAssociationParameters: maxReceivePDUSize "
            << maxReceivePDUSize << " too small (using " << ASC_MINIMUMPDUSIZE << ")");
      maxReceivePDUSize = ASC_MINIMUMPDUSIZE;
    }

    (*params)->ourMaxPDUReceiveSize = maxReceivePDUSize;
    (*params)->DULparams.maxPDU = maxReceivePDUSize;
    (*params)->theirMaxPDUReceiveSize = 0;      /* not yet negotiated */
    (*params)->modeCallback = NULL;

    /* set something unusable */
    ASC_setPresentationAddresses(*params,
                                 "calling Presentation Address",
                                 "called Presentation Address");

    /* presentation context lists will be created as needed */
    (*params)->DULparams.requestedPresentationContext = NULL;
    (*params)->DULparams.acceptedPresentationContext = NULL;

    (*params)->DULparams.useSecureLayer = OFFalse;
    return EC_Normal;
}

static void
destroyPresentationContextList(LST_HEAD ** lst)
{
    DUL_PRESENTATIONCONTEXT *pc;
    DUL_TRANSFERSYNTAX *ts;

    if ((lst == NULL) || (*lst == NULL))
        return;
    while ((pc = (DUL_PRESENTATIONCONTEXT*) LST_Dequeue(lst)) != NULL) {
        if (pc->proposedTransferSyntax != NULL) {
            while ((ts = (DUL_TRANSFERSYNTAX*) LST_Dequeue(&pc->proposedTransferSyntax)) != NULL) {
                free(ts);
            }
            LST_Destroy(&pc->proposedTransferSyntax);
        }
        free(pc);
    }
    LST_Destroy(lst);
}

OFCondition
ASC_destroyAssociationParameters(T_ASC_Parameters ** params)
 /*
  * Free an association parameters structure and embedded information.
  * You do not usually need to do this since the parameters structure will
  * be noted in the association structure and automatically freed when an
  * association terminates.
  */
{

    /* free the elements in the requested presentation context list */
    destroyPresentationContextList(
        &((*params)->DULparams.requestedPresentationContext));

    /* free the elements in the accepted presentation context list */
    destroyPresentationContextList(
        &((*params)->DULparams.acceptedPresentationContext));

    /* free DUL parameters */
    DUL_ClearServiceParameters( &(*params)->DULparams );

    free(*params);
    *params = NULL;

    return EC_Normal;
}

OFCondition
ASC_setAPTitles(T_ASC_Parameters * params,
                const char* callingAPTitle,
                const char* calledAPTitle,
                const char* respondingAPTitle)
 /*
  * Copies the provided Application Titles in the association parameters.
  */
{
    if (callingAPTitle)
        strncpy(params->DULparams.callingAPTitle, callingAPTitle,
                sizeof(params->DULparams.callingAPTitle)-1);
    if (calledAPTitle)
        strncpy(params->DULparams.calledAPTitle, calledAPTitle,
                sizeof(params->DULparams.calledAPTitle)-1);
    if (respondingAPTitle)
        strncpy(params->DULparams.respondingAPTitle, respondingAPTitle,
                sizeof(params->DULparams.respondingAPTitle)-1);

    return EC_Normal;
}

OFCondition
ASC_getAPTitles(T_ASC_Parameters * params,
                char* callingAPTitle,
                char* calledAPTitle,
                char* respondingAPTitle)
 /*
  * Copies the Application Titles stored in the association parameters
  * into the supplied string variables.  You must provide storage to copy
  * into.
  */
{
    if (callingAPTitle)
        strcpy(callingAPTitle, params->DULparams.callingAPTitle);
    if (calledAPTitle)
        strcpy(calledAPTitle, params->DULparams.calledAPTitle);
    if (respondingAPTitle)
        strcpy(respondingAPTitle, params->DULparams.respondingAPTitle);

    return EC_Normal;
}

OFCondition
ASC_getApplicationContextName(T_ASC_Parameters * params,
                              char* applicationContextName)
 /*
  * Copies the Application Context Name stored in the association parameters
  * into the supplied string variable.  You must provide storage to copy
  * into.
  */
{
    if (applicationContextName)
        strcpy(applicationContextName,
               params->DULparams.applicationContextName);
    return EC_Normal;
}

OFCondition
ASC_setPresentationAddresses(T_ASC_Parameters * params,
                             const char* callingPresentationAddress,
                             const char* calledPresentationAddress)
 /*
  * Copies the provided Presentation Addresses into the association
  * parameters.
  */
{
    if (callingPresentationAddress)
        strncpy(params->DULparams.callingPresentationAddress,
                callingPresentationAddress,
                sizeof(params->DULparams.callingPresentationAddress)-1);
    if (calledPresentationAddress)
        strncpy(params->DULparams.calledPresentationAddress,
                calledPresentationAddress,
                sizeof(params->DULparams.calledPresentationAddress)-1);

    return EC_Normal;
}

OFCondition
ASC_getPresentationAddresses(T_ASC_Parameters * params,
                             char* callingPresentationAddress,
                             char* calledPresentationAddress)
 /*
  * Copies the Presentation Addresses stored in the association parameters
  * into the supplied string variables.  You must provide storage to copy
  * into.
  */
{
    if (callingPresentationAddress)
        strcpy(callingPresentationAddress,
               params->DULparams.callingPresentationAddress);
    if (calledPresentationAddress)
        strcpy(calledPresentationAddress,
               params->DULparams.calledPresentationAddress);

    return EC_Normal;
}

OFCondition
ASC_getRejectParameters(T_ASC_Parameters * params,
                        T_ASC_RejectParameters * rejectParameters)
 /*
  * Copies the Rejection Parameters stored in the association parameters into
  * the supplied structure.  Youmust provide storage to copy into.
  */
{
    int reason;
    if (rejectParameters) {
        rejectParameters->result =
            (T_ASC_RejectParametersResult) params->DULparams.result;
        rejectParameters->source =
            (T_ASC_RejectParametersSource) params->DULparams.resultSource;

        reason = params->DULparams.diagnostic |
            ((params->DULparams.resultSource & 0xff) << 8);
        rejectParameters->reason = (T_ASC_RejectParametersReason) reason;
    }
    return EC_Normal;
}

OFString&
ASC_printRejectParameters(OFString& str, T_ASC_RejectParameters *rej)
{
    const char *result;
    const char *source;
    const char *reason;

    switch (rej->result) {
    case ASC_RESULT_REJECTEDPERMANENT:
        result = "Rejected Permanent"; break;
    case ASC_RESULT_REJECTEDTRANSIENT:
        result = "Rejected Transient"; break;
    default:
        result = "UNKNOWN"; break;
    }
    switch (rej->source) {
    case ASC_SOURCE_SERVICEUSER:
        source =  "Service User"; break;
    case ASC_SOURCE_SERVICEPROVIDER_ACSE_RELATED:
        source = "Service Provider (ACSE Related)"; break;
    case ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED:
        source = "Service Provider (Presentation Related)"; break;
    default:
        source = "UNKNOWN"; break;
    }
    switch (rej->reason) {
    case ASC_REASON_SU_NOREASON:
    case ASC_REASON_SP_ACSE_NOREASON:
        reason = "No Reason"; break;
    case ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED:
        reason = "App Context Name Not Supported"; break;
    case ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED:
        reason = "Calling AE Title Not Recognized"; break;
    case ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED:
        reason = "Called AE Title Not Recognized"; break;
    case ASC_REASON_SP_ACSE_PROTOCOLVERSIONNOTSUPPORTED:
        reason = "Protocol Version Not Supported"; break;
        /* Service Provider Presentation Related reasons */
    case ASC_REASON_SP_PRES_TEMPORARYCONGESTION:
        reason = "Temporary Congestion"; break;
    case ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED:
        reason = "Local Limit Exceeded"; break;
    default:
        reason = "UNKNOWN"; break;
    }
    str = OFString("Result: ") + result + ", Source: " + source + "\nReason: " + reason;
    return str;
}

static T_ASC_SC_ROLE
dulRole2ascRole(DUL_SC_ROLE role)
{
    T_ASC_SC_ROLE ar = ASC_SC_ROLE_DEFAULT;
    switch (role) {
    case DUL_SC_ROLE_SCU: ar = ASC_SC_ROLE_SCU; break;
    case DUL_SC_ROLE_SCP: ar = ASC_SC_ROLE_SCP; break;
    case DUL_SC_ROLE_SCUSCP: ar = ASC_SC_ROLE_SCUSCP; break;
    case DUL_SC_ROLE_NONE: ar = ASC_SC_ROLE_NONE; break;
    case DUL_SC_ROLE_DEFAULT:
    default: ar = ASC_SC_ROLE_DEFAULT; break;
    }
    return ar;
}

static DUL_SC_ROLE
ascRole2dulRole(T_ASC_SC_ROLE role)
{
    DUL_SC_ROLE dr = DUL_SC_ROLE_DEFAULT;
    switch (role) {
    case ASC_SC_ROLE_SCU: dr = DUL_SC_ROLE_SCU; break;
    case ASC_SC_ROLE_SCP: dr = DUL_SC_ROLE_SCP; break;
    case ASC_SC_ROLE_SCUSCP: dr = DUL_SC_ROLE_SCUSCP; break;
    case ASC_SC_ROLE_NONE: dr = DUL_SC_ROLE_NONE; break;
    case ASC_SC_ROLE_DEFAULT:
    default: dr = DUL_SC_ROLE_DEFAULT; break;
    }
    return dr;
}

static const char*
ascRole2String(T_ASC_SC_ROLE role)
{
    const char* s = NULL;
    switch (role) {
    case ASC_SC_ROLE_SCU: s = "SCU"; break;
    case ASC_SC_ROLE_SCP: s = "SCP"; break;
    case ASC_SC_ROLE_SCUSCP: s = "SCP/SCU"; break;
    case ASC_SC_ROLE_NONE: s = "None"; break;
    case ASC_SC_ROLE_DEFAULT: s = "Default"; break;
    default: s = "Unknown"; break;
    }
    return s;
}

static
DUL_PRESENTATIONCONTEXT *
findPresentationContextID(LST_HEAD * head,
                          T_ASC_PresentationContextID presentationContextID)
{
    DUL_PRESENTATIONCONTEXT *pc;
    LST_HEAD **l;
    OFBool found = OFFalse;

    if (head == NULL)
        return NULL;

    l = &head;
    if (*l == NULL)
        return NULL;

    pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)pc);

    while (pc && !found) {
        if (pc->presentationContextID == presentationContextID) {
            found = OFTrue;
        } else {
            pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
        }
    }
    return pc;
}

OFCondition
ASC_addPresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    const char* abstractSyntax,
    const char* transferSyntaxList[],
    int transferSyntaxListCount,
    T_ASC_SC_ROLE proposedRole)
 /*
  * Adds a presentation context entry to the presentation context list.
  */
{
    DUL_PRESENTATIONCONTEXT *pc;
    LST_HEAD *lst;
    DUL_TRANSFERSYNTAX *transfer;
    int i;

    /* presentation context id's must be odd */
    if ((presentationContextID % 2) == 0)
    {
      char buf[256];
      sprintf(buf, "ASC Bad presentation context ID: %d", presentationContextID);
      return makeDcmnetCondition(ASCC_BADPRESENTATIONCONTEXTID, OF_error, buf);
    }
    /* see if a presentation context with this id already exists in list */
    pc = findPresentationContextID(
      params->DULparams.requestedPresentationContext,
      presentationContextID);
    if (pc)
    {
      char buf[256];
      sprintf(buf, "ASC Duplicate presentation context ID: %d", presentationContextID);
      return makeDcmnetCondition(ASCC_DUPLICATEPRESENTATIONCONTEXTID, OF_error, buf);
    }

    /* we cannot simply use DUL_MakePresentationCtx() because
    ** it takes variable arguments (for transfer syntax).
    */

    pc = (DUL_PRESENTATIONCONTEXT *) calloc(1, sizeof(DUL_PRESENTATIONCONTEXT));
    if (pc == NULL) return EC_MemoryExhausted;
    lst = LST_Create();
    if (lst == NULL) return EC_MemoryExhausted;
    pc->presentationContextID = presentationContextID;
    strcpy(pc->abstractSyntax, abstractSyntax);
    pc->result = ASC_P_NOTYETNEGOTIATED;
    pc->proposedSCRole = ascRole2dulRole(proposedRole);
    pc->acceptedSCRole = ascRole2dulRole(ASC_SC_ROLE_DEFAULT);

    /* there must be at least one transfer syntax */
    if (transferSyntaxListCount < 1 ) return ASC_MISSINGTRANSFERSYNTAX;

    /* add the transfer syntaxes */
    OFCondition cond = EC_Normal;
    for (i=0; i<transferSyntaxListCount; i++)
    {
        transfer = (DUL_TRANSFERSYNTAX*)malloc(sizeof(DUL_TRANSFERSYNTAX));
        if (transfer == NULL) return EC_MemoryExhausted;
        strcpy(transfer->transferSyntax, transferSyntaxList[i]);
        cond = LST_Enqueue(&lst, (LST_NODE*)transfer);
        if (cond.bad()) return cond;
    }
    pc->proposedTransferSyntax = lst;

    /* add to presentation context list */

    lst = params->DULparams.requestedPresentationContext;
    if (lst == NULL) {
        lst = LST_Create();
        if (lst == NULL) return EC_MemoryExhausted;
    }

    cond = LST_Enqueue(&lst, (LST_NODE*)pc);
    if (cond.bad()) return cond;

    params->DULparams.requestedPresentationContext = lst;
    return EC_Normal;
}

int
ASC_countPresentationContexts(T_ASC_Parameters * params)
 /*
  * Returns the number of presentation contexts contained in the presentation
  * context list.
  */
{
    LST_HEAD **l;

    if (params->DULparams.requestedPresentationContext == NULL)
        return 0;

    l = &(params->DULparams.requestedPresentationContext);
    return LST_Count(l);

}

int
ASC_countAcceptedPresentationContexts(T_ASC_Parameters * params)

 /* ASC_countAcceptedPresentationContexts:
  * Returns the number of presentation contexts contained in the presentation
  * context list which are marked as being accepted.
  */
{
    int n = 0;
    LST_HEAD **l;
    DUL_PRESENTATIONCONTEXT *pc;

    if (params->DULparams.acceptedPresentationContext == NULL)
        return 0;

    l = &(params->DULparams.acceptedPresentationContext);
    pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)pc);

    while (pc) {
        if (pc->result == ASC_P_ACCEPTANCE) n++;
        pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
    }

    return n;
}

OFCondition
ASC_getPresentationContext(T_ASC_Parameters * params,
                           int listPosition,
                           T_ASC_PresentationContext * presentationContext)
 /*
  * You must supply the memory for presentationContext, the values stored in
  * the presentation context list position indicated will be copied into the
  * memory structure.
  */
{
    DUL_PRESENTATIONCONTEXT *pc;
    DUL_TRANSFERSYNTAX *transfer;
    LST_HEAD **l;
    int count = 0;

    /* make the presentation context structure clean */
    bzero((char*)presentationContext, sizeof(*presentationContext));

    if (params->DULparams.requestedPresentationContext == NULL)
    {
      char buf[256];
      sprintf(buf, "ASC Bad presentation context position: %d", listPosition);
      return makeDcmnetCondition(ASCC_BADPRESENTATIONCONTEXTPOSITION, OF_error, buf);
    }
    l = &(params->DULparams.requestedPresentationContext);
    if (*l == NULL)
    {
      char buf[256];
      sprintf(buf, "ASC Bad presentation context position: %d", listPosition);
      return makeDcmnetCondition(ASCC_BADPRESENTATIONCONTEXTPOSITION, OF_error, buf);
    }
    pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)pc);

    while (pc && count != listPosition) {
        count++;
        pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
    }

    if (pc == NULL)
    {
      char buf[256];
      sprintf(buf, "ASC Bad presentation context position: %d", listPosition);
      return makeDcmnetCondition(ASCC_BADPRESENTATIONCONTEXTPOSITION, OF_error, buf);
    }

    presentationContext->presentationContextID = pc->presentationContextID;
    presentationContext->resultReason = (T_ASC_P_ResultReason) pc->result;
    presentationContext->proposedRole = dulRole2ascRole(pc->proposedSCRole);
    presentationContext->acceptedRole = dulRole2ascRole(pc->acceptedSCRole);
    strcpy(presentationContext->abstractSyntax,
           pc->abstractSyntax);
    if (presentationContext->resultReason == ASC_P_ACCEPTANCE) {
        strcpy(presentationContext->acceptedTransferSyntax,
            pc->acceptedTransferSyntax);
    } else {
        presentationContext->acceptedTransferSyntax[0] = '\0';
    }

    /* need to copy the transfer syntaxes */
    count = 0;

    l = &pc->proposedTransferSyntax;
    transfer = (DUL_TRANSFERSYNTAX*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)transfer);
    while (transfer != NULL)
    {
        if (count >= DICOM_MAXTRANSFERSYNTAXES)
        {
          return makeDcmnetCondition(ASCC_CODINGERROR, OF_error, "ASC Coding error in ASC_getPresentationContext: too many transfer syntaxes");
        }
        strcpy(presentationContext->proposedTransferSyntaxes[count],
               transfer->transferSyntax);
        count++;
        transfer = (DUL_TRANSFERSYNTAX*) LST_Next(l);
    }

    presentationContext->transferSyntaxCount = count;

    return EC_Normal;
}

OFCondition
ASC_acceptPresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    const char* transferSyntax,
    T_ASC_SC_ROLE acceptedRole)
 /*
  * The presentation context will be marked as accepted and the provided
  * transfer syntax name chosen.
  */
{
    DUL_PRESENTATIONCONTEXT *proposedContext, *acceptedContext;
    OFCondition cond = EC_Normal;
    LST_HEAD *lst;

    proposedContext = findPresentationContextID(
                              params->DULparams.requestedPresentationContext,
                                                presentationContextID);
    if (proposedContext == NULL) return ASC_BADPRESENTATIONCONTEXTID;
    strcpy(proposedContext->acceptedTransferSyntax, transferSyntax);

    /* we want to mark this proposed context as beeing ok */
    proposedContext->result = ASC_P_ACCEPTANCE;
    proposedContext->acceptedSCRole = ascRole2dulRole(acceptedRole);

    /* check whether the SCP/SCU role selection is successful */
    if (dcmStrictRoleSelection.get())
    {
        if (proposedContext->proposedSCRole != proposedContext->acceptedSCRole)
        {
            if (((proposedContext->proposedSCRole == DUL_SC_ROLE_DEFAULT) && (proposedContext->acceptedSCRole != DUL_SC_ROLE_SCU)) ||
                ((proposedContext->proposedSCRole == DUL_SC_ROLE_SCU) && (proposedContext->acceptedSCRole != DUL_SC_ROLE_DEFAULT)) ||
                ((proposedContext->proposedSCRole != DUL_SC_ROLE_SCUSCP) && (proposedContext->acceptedSCRole != DUL_SC_ROLE_SCUSCP)))
            {
                proposedContext->result = ASC_P_NOREASON;
                DCMNET_ERROR("ASSOC: SCP/SCU role selection failed, proposed ("
                    << ascRole2String(dulRole2ascRole(proposedContext->proposedSCRole))
                    << ") and accepted role (" << ascRole2String(acceptedRole) << ") are incompatible");
                return ASC_SCPSCUROLESELECTIONFAILED;
            }
        }
    }

    acceptedContext = findPresentationContextID(
                              params->DULparams.acceptedPresentationContext,
                                                presentationContextID);

    if (acceptedContext != NULL) {
        /* it is already in the list, mark it as accepted */
        acceptedContext->result = ASC_P_ACCEPTANCE;
        strcpy(acceptedContext->abstractSyntax,
               proposedContext->abstractSyntax);
        strcpy(acceptedContext->acceptedTransferSyntax, transferSyntax);
        acceptedContext->proposedSCRole = proposedContext->proposedSCRole;
        acceptedContext->acceptedSCRole = ascRole2dulRole(acceptedRole);
    } else {
        /*
         * make a new presentation context, mark it as accepted and add to
         * end of accepted list
         */

        cond = DUL_MakePresentationCtx(
            &acceptedContext,
            proposedContext->proposedSCRole, ascRole2dulRole(acceptedRole),
            presentationContextID, ASC_P_ACCEPTANCE,
            proposedContext->abstractSyntax,
            (char*)transferSyntax, NULL);
        if (cond.bad()) return cond;

        lst = params->DULparams.acceptedPresentationContext;
        if (lst == NULL) {
            lst = LST_Create();
            if (lst == NULL) return EC_MemoryExhausted;
        }

        cond = LST_Enqueue(&lst, (LST_NODE*)acceptedContext);
        if (cond.bad()) return cond;
        params->DULparams.acceptedPresentationContext = lst;
    }
    return EC_Normal;
}


OFCondition
ASC_refusePresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    T_ASC_P_ResultReason resultReason)
 /*
  * The presentation context will be marked as refused.
  */
{
    DUL_PRESENTATIONCONTEXT *proposedContext, *acceptedContext;
    OFCondition cond = EC_Normal;
    LST_HEAD *lst;

    proposedContext = findPresentationContextID(
                             params->DULparams.requestedPresentationContext,
                                                presentationContextID);
    if (proposedContext == NULL) return ASC_BADPRESENTATIONCONTEXTID;

    /* we want to mark this proposed context as being refused */
    proposedContext->result = resultReason;

    acceptedContext = findPresentationContextID(
                              params->DULparams.acceptedPresentationContext,
                                                presentationContextID);

    if (acceptedContext != NULL) {
        /* it is already in the list, mark it as refused */
        acceptedContext->result = resultReason;
        strcpy(acceptedContext->abstractSyntax,
               proposedContext->abstractSyntax);
        /* we must send back a transfer syntax even though this
         * presentation context is refused.  Some software implementations
         * seem to get confused if we don't.
         */
        strcpy(acceptedContext->acceptedTransferSyntax,
                UID_LittleEndianImplicitTransferSyntax);
    } else {

        /*
         * make a new presentation context, mark it as refused and add to end
         * of accepted list -- yes, even though it is refused, refused
         * presentation contexts must still be sent back
         */
        /* we must send back a transfer syntax even though this
         * presentation context is refused.  Some software implementations
         * seem to get confused if we don't.
         */

        cond = DUL_MakePresentationCtx(
            &acceptedContext,
            DUL_SC_ROLE_DEFAULT, DUL_SC_ROLE_DEFAULT,
            presentationContextID, resultReason,
            proposedContext->abstractSyntax,
            UID_LittleEndianImplicitTransferSyntax, NULL);
        if (cond.bad()) return cond;

        lst = params->DULparams.acceptedPresentationContext;
        if (lst == NULL) {
            lst = LST_Create();
            if (lst == NULL) return EC_MemoryExhausted;
        }
        cond = LST_Enqueue(&lst, (LST_NODE*)acceptedContext);
        if (cond.bad()) return cond;
        params->DULparams.acceptedPresentationContext = lst;
    }
    return EC_Normal;
}


OFCondition
ASC_findAcceptedPresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    T_ASC_PresentationContext * presentationContext)
 /*
  * ASC_findAcceptedPresentationContext: You must supply the memory for
  * presentationContext, the values stored in the accepted presentation
  * context list with given ID will be copied into the memory structure.
  * Returns EC_Normal if found, or ASC_BADPRESENTATIONCONTEXTID if not
  * found.
  */
{
    DUL_PRESENTATIONCONTEXT *pc;
    DUL_TRANSFERSYNTAX *transfer;
    LST_HEAD **l;
    int count = 0;

    /*
    ** We look in the requestedPresentationContexts since this
    ** has all the proposed information.   The result field will
    ** have been set to ASC_P_ACCEPTANCE in the ASC_requestAssociation
    ** function.
    */
    pc = findPresentationContextID(
                             params->DULparams.requestedPresentationContext,
                                   presentationContextID);

    if ((pc == NULL) || (pc->result != ASC_P_ACCEPTANCE)) return ASC_BADPRESENTATIONCONTEXTID;

    l = &pc->proposedTransferSyntax;
    transfer = (DUL_TRANSFERSYNTAX*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)transfer);
    while (transfer != NULL) {
        if (count >= DICOM_MAXTRANSFERSYNTAXES)
        {
          return makeDcmnetCondition(ASCC_CODINGERROR, OF_error, "ASC Coding error in ASC_findAcceptedPresentationContext: too many transfer syntaxes");
        }
        OFStandard::strlcpy(presentationContext->proposedTransferSyntaxes[count], transfer->transferSyntax, sizeof(DIC_UI));
        count++;
        transfer = (DUL_TRANSFERSYNTAX*) LST_Next(l);
    }

    OFStandard::strlcpy(presentationContext->abstractSyntax, pc->abstractSyntax, sizeof(DIC_UI));
    presentationContext->presentationContextID = pc->presentationContextID;
    presentationContext->resultReason = (T_ASC_P_ResultReason) pc->result;
    presentationContext->proposedRole = dulRole2ascRole(pc->proposedSCRole);
    presentationContext->acceptedRole = dulRole2ascRole(pc->acceptedSCRole);

    presentationContext->transferSyntaxCount = count;
    OFStandard::strlcpy(presentationContext->acceptedTransferSyntax, pc->acceptedTransferSyntax, sizeof(DIC_UI));

    return EC_Normal;
}

T_ASC_PresentationContextID
ASC_findAcceptedPresentationContextID(
    T_ASC_Association *assoc,
    const char* abstractSyntax)
{
    /* ASC_findAcceptedPresentationContextID:
     * Searches in the accepted presentation context list for the given
     * abstract syntax.  If found returns its PresentationContextID, otherwise
     * returns 0 (which is not a valid ID).
     */
    DUL_PRESENTATIONCONTEXT *pc;
    LST_HEAD **l;
    OFBool found = OFFalse;

    if (assoc->params->DULparams.acceptedPresentationContext == NULL)
      return 0;

    l = &assoc->params->DULparams.acceptedPresentationContext;
    pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)pc);
    while (pc && !found)
    {
        found = ( (strcmp(pc->abstractSyntax, abstractSyntax) == 0) && (pc->result == ASC_P_ACCEPTANCE) );
        if (!found) pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
    }
    if (found) return pc->presentationContextID;
    return 0;   /* otherwise */
}


/* transfer syntax aware version of T_ASC_PresentationContextID.
 * Tries to find a presentation context that matches the characteristics
 * of the given DICOM dataset best
 * - if possible finds a presentation context with matching TS
 * - then tries to find an explicit VR uncompressed TS presentation ctx
 * - then tries to find an implicit VR uncompressed TS presentation ctx
 * - finally accepts each matching presentation ctx independent of TS.
 * Returns 0 if no appropriate presentation context could be found at all.
 */
T_ASC_PresentationContextID
ASC_findAcceptedPresentationContextID(
    T_ASC_Association *assoc,
    const char* abstractSyntax,
    const char * transferSyntax)
{
    DUL_PRESENTATIONCONTEXT *pc;
    LST_HEAD **l;
    OFBool found = OFFalse;

    if ( (transferSyntax == NULL) || (abstractSyntax == NULL) )
      return 0;
    if (assoc->params->DULparams.acceptedPresentationContext == NULL)
      return 0;

    /* first of all we look for a presentation context
     * matching both abstract and transfer syntax
     */
    l = &assoc->params->DULparams.acceptedPresentationContext;
    pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)pc);
    while (pc && !found)
    {
        found = (strcmp(pc->abstractSyntax, abstractSyntax) == 0) && (pc->result == ASC_P_ACCEPTANCE) && (strcmp(pc->acceptedTransferSyntax, transferSyntax) == 0);
        if (!found) pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
    }
    if (found) return pc->presentationContextID;

    /* now we look for an explicit VR uncompressed PC. */
    l = &assoc->params->DULparams.acceptedPresentationContext;
    pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)pc);
    while (pc && !found)
    {
        found =  (strcmp(pc->abstractSyntax, abstractSyntax) == 0)
          && (pc->result == ASC_P_ACCEPTANCE)
          && ((strcmp(pc->acceptedTransferSyntax, UID_LittleEndianExplicitTransferSyntax) == 0)
           || (strcmp(pc->acceptedTransferSyntax, UID_BigEndianExplicitTransferSyntax) == 0));
        if (!found) pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
    }
    if (found) return pc->presentationContextID;

    /* now we look for an implicit VR uncompressed PC. */
    l = &assoc->params->DULparams.acceptedPresentationContext;
    pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)pc);
    while (pc && !found)
    {
        found = (strcmp(pc->abstractSyntax, abstractSyntax) == 0)
                && (pc->result == ASC_P_ACCEPTANCE)
                && (strcmp(pc->acceptedTransferSyntax, UID_LittleEndianImplicitTransferSyntax) == 0);
        if (!found) pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
    }
    if (found) return pc->presentationContextID;

    /* finally we accept everything we get.
       returns 0 if abstract syntax is not supported
     */
    return ASC_findAcceptedPresentationContextID(assoc, abstractSyntax);
}


OFCondition
ASC_acceptContextsWithTransferSyntax(
    T_ASC_Parameters * params,
    const char* transferSyntax,
    int abstractSyntaxCount, const char* abstractSyntaxes[],
    T_ASC_SC_ROLE acceptedRole)
/*
  * Any proposed presentation contexts which are found abstractSyntaxes[]
  * which also have proposed a transfer syntax of transferSyntax, will be
  * accepted.  Any presentation contexts already marked as accepted will be
  * left alone but any remaining presentation contexts will be refused.
  */
{
    OFCondition cond = EC_Normal;
    int n, i, j, k;
    DUL_PRESENTATIONCONTEXT *dpc;
    T_ASC_PresentationContext pc;
    OFBool accepted = OFFalse;
    OFBool abstractOK = OFFalse;

    n = ASC_countPresentationContexts(params);
    for (i = 0; i < n; i++) {
        cond = ASC_getPresentationContext(params, i, &pc);
        if (cond.bad()) return cond;
        abstractOK = OFFalse;
        accepted = OFFalse;
        for (j = 0; j < abstractSyntaxCount && !accepted; j++) {
            if (strcmp(pc.abstractSyntax, abstractSyntaxes[j]) == 0) {
                abstractOK = OFTrue;
                /* check the transfer syntax */
                for (k = 0; (k < (int)pc.transferSyntaxCount) && !accepted; k++) {
                    if (strcmp(pc.proposedTransferSyntaxes[k], transferSyntax) == 0) {
                        accepted = OFTrue;
                    }
                }
            }
        }
        if (accepted) {
            cond = ASC_acceptPresentationContext(
                params, pc.presentationContextID,
                transferSyntax, acceptedRole);
            // SCP/SCU role selection failed, reject presentation context
            if (cond == ASC_SCPSCUROLESELECTIONFAILED) {
                cond = ASC_refusePresentationContext(params,
                    pc.presentationContextID, ASC_P_NOREASON);
            }
            if (cond.bad()) return cond;
        } else {
            T_ASC_P_ResultReason reason;

            /* do not refuse if already accepted */
            dpc = findPresentationContextID(
                              params->DULparams.acceptedPresentationContext,
                                            pc.presentationContextID);
            if ((dpc == NULL) ||
                ((dpc != NULL) && (dpc->result != ASC_P_ACCEPTANCE))) {

                if (abstractOK) {
                    reason = ASC_P_TRANSFERSYNTAXESNOTSUPPORTED;
                } else {
                    reason = ASC_P_ABSTRACTSYNTAXNOTSUPPORTED;
                }
                /*
                 * If previously this presentation context was refused
                 * because of bad transfer syntax let it stay that way.
                 */
                if ((dpc != NULL) &&
                    (dpc->result == ASC_P_TRANSFERSYNTAXESNOTSUPPORTED))
                    reason = ASC_P_TRANSFERSYNTAXESNOTSUPPORTED;

                cond = ASC_refusePresentationContext(params,
                                              pc.presentationContextID,
                                              reason);
                if (cond.bad()) return cond;
            }
        }

    }
    return EC_Normal;
}

OFCondition
ASC_acceptContextsWithPreferredTransferSyntaxes(
    T_ASC_Parameters * params,
    const char* abstractSyntaxes[], int abstractSyntaxCount,
    const char* transferSyntaxes[], int transferSyntaxCount,
    T_ASC_SC_ROLE acceptedRole)
{
    int i;
    OFCondition cond = EC_Normal;
    /*
    ** Accept in the order "least wanted" to "most wanted" transfer
    ** syntax.  Accepting a transfer syntax will override previously
    ** accepted transfer syntaxes.
    */
    for (i=transferSyntaxCount-1; i>=0; i--)
    {
        cond = ASC_acceptContextsWithTransferSyntax(
            params, transferSyntaxes[i],
            abstractSyntaxCount, abstractSyntaxes, acceptedRole);
        if (cond.bad()) return cond;
    }
    return cond;
}

void ASC_getRequestedExtNegList(T_ASC_Parameters* params, SOPClassExtendedNegotiationSubItemList** extNegList)
{
    *extNegList = params->DULparams.requestedExtNegList;
}

void ASC_getAcceptedExtNegList(T_ASC_Parameters* params, SOPClassExtendedNegotiationSubItemList** extNegList)
{
    *extNegList = params->DULparams.acceptedExtNegList;
}

void ASC_setRequestedExtNegList(T_ASC_Parameters* params, SOPClassExtendedNegotiationSubItemList* extNegList)
{
    params->DULparams.requestedExtNegList = extNegList;
}

void ASC_setAcceptedExtNegList(T_ASC_Parameters* params, SOPClassExtendedNegotiationSubItemList* extNegList)
{
    params->DULparams.acceptedExtNegList = extNegList;
}


/* User Identity Negotiation */
void ASC_getUserIdentRQ(T_ASC_Parameters* params, UserIdentityNegotiationSubItemRQ** usrIdentRQ)
{
  *usrIdentRQ = params->DULparams.reqUserIdentNeg;
}

void ASC_getUserIdentAC(T_ASC_Parameters* params, UserIdentityNegotiationSubItemAC** usrIdentAC)
{
  *usrIdentAC = params->DULparams.ackUserIdentNeg;
}


OFCondition ASC_setIdentRQUserPassword(
    T_ASC_Parameters * params,
    const OFString& userName,
    const OFString& password,
    const OFBool& requestRsp)
{
  if (params == NULL)
    return ASC_NULLKEY;
  // length field for user identity content is only 2 bytes -> 65535 bytes max
  if ((userName.length() > 65535) || (password.length() > 65535) || (userName.length() + password.length() > 65535))
    return EC_IllegalCall;
  UserIdentityNegotiationSubItemRQ* rq = params->DULparams.reqUserIdentNeg;
  if (rq == NULL)
    rq = new UserIdentityNegotiationSubItemRQ();
  else
    rq->clear();
  rq->setIdentityType(ASC_USER_IDENTITY_USER_PASSWORD);
  rq->setPrimField(userName.c_str(), userName.length());
  rq->setSecField(password.c_str(), password.length());
  rq->setReqPosResponse(requestRsp);
  params->DULparams.reqUserIdentNeg = rq;
  return EC_Normal;
}


OFCondition ASC_setIdentRQUserOnly(
    T_ASC_Parameters * params,
    const OFString& userName,
    const OFBool& requestRsp)
{
  if (params == NULL)
    return ASC_NULLKEY;
  // length field for user identity content is only 2 bytes -> 65535 bytes max
  if (userName.length() > 65535)
    return EC_IllegalCall;
  UserIdentityNegotiationSubItemRQ* rq = params->DULparams.reqUserIdentNeg;
  if (rq == NULL)
    rq = new UserIdentityNegotiationSubItemRQ();
  else
    rq->clear();
  rq->setIdentityType(ASC_USER_IDENTITY_USER);
  rq->setPrimField(userName.c_str(), userName.length());
  rq->setReqPosResponse(requestRsp);
  params->DULparams.reqUserIdentNeg = rq;
  return EC_Normal;
}


OFCondition ASC_setIdentRQKerberos(
    T_ASC_Parameters * params,
    const char* kerbTicket,
    const Uint16& length,
    const OFBool& requestRsp)
{
  if (params == NULL)
    return ASC_NULLKEY;
  UserIdentityNegotiationSubItemRQ* rq = params->DULparams.reqUserIdentNeg;
  if (rq == NULL)
    rq = new UserIdentityNegotiationSubItemRQ();
  else
    rq->clear();
  rq->setIdentityType(ASC_USER_IDENTITY_KERBEROS);
  rq->setPrimField(kerbTicket, length);
  rq->setReqPosResponse(requestRsp);
  params->DULparams.reqUserIdentNeg = rq;
  return EC_Normal;
}


OFCondition ASC_setIdentRQSaml(
    T_ASC_Parameters * params,
    const char* saml,
    const Uint16& length,
    const OFBool& requestRsp)
{
  if (params == NULL)
    return ASC_NULLKEY;
  UserIdentityNegotiationSubItemRQ* rq = params->DULparams.reqUserIdentNeg;
  if (rq == NULL)
    rq = new UserIdentityNegotiationSubItemRQ();
  else
    rq->clear();
  rq->setIdentityType(ASC_USER_IDENTITY_SAML);
  rq->setPrimField(saml, length);
  rq->setReqPosResponse(requestRsp);
  params->DULparams.reqUserIdentNeg = rq;
  return EC_Normal;
}


void ASC_getCopyOfIdentResponse(T_ASC_Parameters * params,
                                char*& buffer,
                                unsigned short& bufferLen)
{
  if (params == NULL)
  {
    buffer = NULL;
    bufferLen = 0;
    return;
  }
  UserIdentityNegotiationSubItemAC *rsp = params->DULparams.ackUserIdentNeg;
  if (rsp == NULL)
  {
    buffer = NULL;
    bufferLen = 0;
    return;
  }
  rsp->getServerResponse(buffer, bufferLen);
  return;
}


OFCondition ASC_setIdentAC(
    T_ASC_Parameters * params,
    const char* response,
    const Uint16& length )
{
  if (params == NULL)
    return ASC_NULLKEY;
  UserIdentityNegotiationSubItemAC* ac = params->DULparams.ackUserIdentNeg;
  if (ac == NULL)
    ac = new UserIdentityNegotiationSubItemAC();
  else
    ac->clear();
  if( response != NULL )
   ac->setServerResponse(response, length);
  params->DULparams.ackUserIdentNeg = ac;
  return EC_Normal;
}


static OFString
ASC_dumpPresentationContext(T_ASC_PresentationContext * p)
 /*
  * Write presentation context structure in textual form to stdout.
  * (debugging aid)
  */
{
    int i = 0;
    OFOStringStream outstream;

    outstream << "  Context ID:        " << (int)p->presentationContextID << " ";
    switch (p->resultReason) {
    case ASC_P_ACCEPTANCE:
        outstream << "(Accepted)" << OFendl;
        break;
    case ASC_P_USERREJECTION:
        outstream << "(User Rejection)" << OFendl;
        break;
    case ASC_P_NOREASON:
        outstream << "(No Reason)" << OFendl;
        break;
    case ASC_P_ABSTRACTSYNTAXNOTSUPPORTED:
        outstream << "(Abstract Syntax Not Supported)" << OFendl;
        break;
    case ASC_P_TRANSFERSYNTAXESNOTSUPPORTED:
        outstream << "(Transfer Syntaxes Not Supported)" << OFendl;
        break;
    case ASC_P_NOTYETNEGOTIATED:
        outstream << "(Proposed)" << OFendl;
        break;
    default:
        outstream << "(--Invalid Result/Reason--)" << OFendl;
    }

    const char* l_as = dcmFindNameOfUID(p->abstractSyntax);
    if (l_as) {
        outstream << "    Abstract Syntax: =" << l_as  << OFendl;
    } else {
        outstream << "    Abstract Syntax: " <<  p->abstractSyntax << OFendl;
    }

    outstream << "    Proposed SCP/SCU Role: "
        << ascRole2String(p->proposedRole) << OFendl;

    if (p->resultReason != ASC_P_NOTYETNEGOTIATED) {
        outstream << "    Accepted SCP/SCU Role: "
            << ascRole2String(p->acceptedRole) << OFendl;
    }

    if (p->resultReason == ASC_P_ACCEPTANCE) {
        const char* ts = dcmFindNameOfUID(p->acceptedTransferSyntax);
        if (ts) {
            outstream << "    Accepted Transfer Syntax: =" << ts << OFendl;
        } else {
            outstream << "    Accepted Transfer Syntax: "
                << p->acceptedTransferSyntax << OFendl;
        }
    }

    if (p->resultReason == ASC_P_NOTYETNEGOTIATED) {
        outstream << "    Proposed Transfer Syntax(es):" << OFendl;
        for (i = 0; i < (int)p->transferSyntaxCount; i++) {
            const char* ts = dcmFindNameOfUID(p->proposedTransferSyntaxes[i]);
            if (ts) {
                outstream << "      =" << ts << OFendl;
            } else {
                outstream << "      " << p->proposedTransferSyntaxes[i] << OFendl;
            }
        }
    }
    outstream << OFStringStream_ends;

    OFSTRINGSTREAM_GETOFSTRING(outstream, ret)
    return ret;
}


OFString&
ASC_dumpParameters(OFString& str, T_ASC_Parameters * params, ASC_associateType dir)
 /*
  * Write parameters in textual form to stdout (debugging aid)
  */
{
    int i;
    T_ASC_PresentationContext pc;
    OFOStringStream outstream;
    OFString temp_str;
    const char *str_dir = NULL;

    switch (dir)
    {
        case ASC_ASSOC_RQ:
            str_dir = "RQ";
            break;
        case ASC_ASSOC_AC:
            str_dir = "AC";
            break;
        case ASC_ASSOC_RJ:
            str_dir = "RJ";
            break;
        default:
            // Should *NEVER* happen
            str_dir = "UNKNOWN";
            break;
    }

    outstream << "====================== BEGIN A-ASSOCIATE-" << str_dir << " =====================" << OFendl
        << "Our Implementation Class UID:      "
        << params->ourImplementationClassUID << OFendl
        << "Our Implementation Version Name:   "
        << params->ourImplementationVersionName << OFendl
        << "Their Implementation Class UID:    "
        << params->theirImplementationClassUID << OFendl
        << "Their Implementation Version Name: "
        << params->theirImplementationVersionName << OFendl;

    outstream << "Application Context Name:    "
        << params->DULparams.applicationContextName << OFendl
        << "Calling Application Name:    "
        << params->DULparams.callingAPTitle << OFendl
        << "Called Application Name:     "
        << params->DULparams.calledAPTitle << OFendl
        << "Responding Application Name: "
        << params->DULparams.respondingAPTitle << OFendl;

    outstream << "Our Max PDU Receive Size:    "
        << params->ourMaxPDUReceiveSize << OFendl
        << "Their Max PDU Receive Size:  "
        << params->theirMaxPDUReceiveSize << OFendl;

    outstream << "Presentation Contexts:" << OFendl;
    for (i=0; i<ASC_countPresentationContexts(params); i++) {
        ASC_getPresentationContext(params, i, &pc);
        outstream << ASC_dumpPresentationContext(&pc);
    }

    SOPClassExtendedNegotiationSubItemList* extNegList=NULL;
    ASC_getRequestedExtNegList(params, &extNegList);
    outstream << "Requested Extended Negotiation:";
    if (extNegList != NULL) {
        outstream << OFendl;
        outstream << dumpExtNegList(temp_str, *extNegList);
    } else {
        outstream << " none" << OFendl;
    }

    ASC_getAcceptedExtNegList(params, &extNegList);
    outstream << "Accepted Extended Negotiation:";
    if (extNegList != NULL) {
        outstream << OFendl;
        outstream << dumpExtNegList(temp_str, *extNegList);
    } else {
        outstream << "  none" << OFendl;
    }

    UserIdentityNegotiationSubItemRQ *userIdentRQ = NULL;
    ASC_getUserIdentRQ(params, &userIdentRQ);
    outstream << "Requested User Identity Negotiation:";
    if (userIdentRQ != NULL) {
        outstream << OFendl;
        userIdentRQ->dump(outstream);
    } else {
        outstream << " none" << OFendl;
    }

    UserIdentityNegotiationSubItemAC *userIdentAC = NULL;
    ASC_getUserIdentAC(params, &userIdentAC);
    outstream << "User Identity Negotiation Response:";
    if (userIdentAC != NULL) {
        outstream << OFendl;
        userIdentAC->dump(outstream);
    } else {
        outstream << "  none" << OFendl;
    }

#if 0
    outstream << "DUL Params --- BEGIN" << OFendl;
    DUL_DumpParams(&params->DULparams);
    outstream << "DUL Params --- END" << OFendl;
#endif

    outstream << "======================= END A-ASSOCIATE-" << str_dir << " ======================"
              << OFStringStream_ends;

    OFSTRINGSTREAM_GETSTR(outstream, res)
    str = res;
    OFSTRINGSTREAM_FREESTR(res)
    return str;
}

OFString&
ASC_dumpConnectionParameters(OFString &str, T_ASC_Association *association)
{
  str.clear();
  if (association==NULL) return str;
  return DUL_DumpConnectionParameters(str, association->DULassociation);
}

/*
 * Extra facility for examining an association for waiting data or
 * examining a network for pending association requests.
 */

extern int
DUL_networkSocket(DUL_NETWORKKEY * callerNet);

typedef DcmTransportConnection *P_DcmTransportConnection;

OFBool
ASC_selectReadableAssociation(T_ASC_Association* assocs[],
        int assocCount, int timeout)
{
  if (assocCount <= 0) return OFFalse;

  P_DcmTransportConnection *connections = new P_DcmTransportConnection[assocCount];
  if (connections == NULL) return OFFalse;

  int i;
  for (i=0; i<assocCount; i++)
  {
    if (assocs[i]) connections[i] = DUL_getTransportConnection(assocs[i]->DULassociation);
    else connections[i] = NULL;
  }
  OFBool result = DcmTransportConnection::selectReadableAssociation(connections, assocCount, timeout);
  if (result)
  {
    for (i=0; i<assocCount; i++)
    {
      if (connections[i]==NULL) assocs[i]=NULL;
    }
  }
  delete[] connections;
  return result;
}

OFBool
ASC_dataWaiting(T_ASC_Association * association, int timeout)
{
  return DUL_dataWaiting(association->DULassociation, timeout);
}

OFBool
ASC_associationWaiting(T_ASC_Network * network, int timeout)
{
    int s;
    struct timeval t;
    fd_set fdset;
    int nfound;

    if (network == NULL) return OFFalse;

    s = DUL_networkSocket(network->network);
    if (s < 0)
        return OFFalse;

    FD_ZERO(&fdset);
#ifdef __MINGW32__
    // on MinGW, FD_SET expects an unsigned first argument
    FD_SET((unsigned int) s, &fdset);
#else
    FD_SET(s, &fdset);
#endif
    t.tv_sec = timeout;
    t.tv_usec = 0;
#ifdef HAVE_INTP_SELECT
    nfound = select(s + 1, (int *)(&fdset), NULL, NULL, &t);
#else
    nfound = select(s + 1, &fdset, NULL, NULL, &t);
#endif
    return nfound > 0;
}

/*
 * Association creation and termination
 */

OFCondition
ASC_destroyAssociation(T_ASC_Association ** association)
{
    OFCondition cond = EC_Normal;

    /* don't worry if already destroyed */
    if (association == NULL) return EC_Normal;
    if (*association == NULL) return EC_Normal;

    if ((*association)->DULassociation != NULL) {
        ASC_dropAssociation(*association);
    }

    if ((*association)->params != NULL) {
        cond = ASC_destroyAssociationParameters(&(*association)->params);
        if (cond.bad()) return cond;
    }

    if ((*association)->sendPDVBuffer != NULL)
        free((*association)->sendPDVBuffer);

    free(*association);
    *association = NULL;

    return EC_Normal;
}

OFCondition
ASC_receiveAssociation(T_ASC_Network * network,
                       T_ASC_Association ** assoc,
                       long maxReceivePDUSize,
                       void **associatePDU,
                       unsigned long *associatePDUlength,
                       OFBool useSecureLayer,
                       DUL_BLOCKOPTIONS block,
                       int timeout)
{
    T_ASC_Parameters *params;
    DUL_ASSOCIATIONKEY *DULassociation;
    DUL_PRESENTATIONCONTEXT *pc;
    LST_HEAD **l;

    int retrieveRawPDU = 0;
    if (associatePDU && associatePDUlength) retrieveRawPDU = 1;

    OFCondition cond = ASC_createAssociationParameters(&params, maxReceivePDUSize);
    if (cond.bad()) return cond;

    cond = ASC_setTransportLayerType(params, useSecureLayer);
    if (cond.bad()) return cond;

    *assoc = (T_ASC_Association *) malloc(sizeof(**assoc));
    if (*assoc == NULL) return EC_MemoryExhausted;
    bzero((char*)*assoc, sizeof(**assoc));

    (*assoc)->params = params;
    (*assoc)->nextMsgID = 1;

    cond = DUL_ReceiveAssociationRQ(&network->network, block, timeout,
                                    &(params->DULparams), &DULassociation, retrieveRawPDU);

    if (cond.code() == DULC_FORKEDCHILD)
    {
        ASC_destroyAssociationParameters(&params);
        free(*assoc);
        *assoc = NULL;
        return cond;
    }

    (*assoc)->DULassociation = DULassociation;

    if (retrieveRawPDU && DULassociation)
    {
      DUL_returnAssociatePDUStorage((*assoc)->DULassociation, *associatePDU, *associatePDUlength);
    }

    if (cond.bad()) return cond;

    /* mark the presentation contexts as being proposed */
    l = &params->DULparams.requestedPresentationContext;
    if (*l != NULL) {
        pc =(DUL_PRESENTATIONCONTEXT*)  LST_Head(l);
        if (pc != NULL)
            (void)LST_Position(l, (LST_NODE*)pc);

        while (pc) {
            pc->result = ASC_P_NOTYETNEGOTIATED;
            pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
        }
    }

    // copy only maximum number of allowed characters
    // trailing zero is always present because
    // ASC_createAssociationParameters zero-initializes the complete struct.
    strncpy(params->theirImplementationClassUID,
            params->DULparams.callingImplementationClassUID,
            sizeof (params->theirImplementationClassUID) - 1);
    strncpy(params->theirImplementationVersionName,
            params->DULparams.callingImplementationVersionName,
            sizeof (params->theirImplementationVersionName) - 1);

    /*
     * The params->DULparams.peerMaxPDU parameter contains the
     * max-pdu-length value in the a-associate-ac (i.e. the max-pdu-length
     * that the remote AE is prepared to accept).
     */
    params->theirMaxPDUReceiveSize = params->DULparams.peerMaxPDU;

    /* the PDV buffer and length get set when we acknowledge the association */
    (*assoc)->sendPDVLength = 0;
    (*assoc)->sendPDVBuffer = NULL;

    return EC_Normal;
}

static OFCondition
updateRequestedPCFromAcceptedPC(
    DUL_PRESENTATIONCONTEXT *apc,
    DUL_PRESENTATIONCONTEXT *rpc)
{
    if (apc == NULL || rpc == NULL) return EC_Normal;

    if (rpc->presentationContextID != apc->presentationContextID)
    {
       return makeDcmnetCondition(ASCC_CODINGERROR, OF_error, "ASC Coding error in updateRequestedPCFromAcceptedPC: presentationContextIDs differ");
    }

    rpc->result = apc->result;
    if (apc->result == ASC_P_ACCEPTANCE) {
        strcpy(rpc->acceptedTransferSyntax, apc->acceptedTransferSyntax);
    } else {
        rpc->acceptedTransferSyntax[0] = '\0';
    }
    rpc->acceptedSCRole = apc->acceptedSCRole;
    return EC_Normal;
}


static OFCondition
updateRequestedPCListFromAcceptedPCList(
    DUL_ASSOCIATESERVICEPARAMETERS *dulParams)
{
    OFCondition cond = EC_Normal;
    DUL_PRESENTATIONCONTEXT *requestedPc = NULL;
    DUL_PRESENTATIONCONTEXT *acceptedPc = NULL;
    LST_HEAD **acceptedList = NULL;

    acceptedList = &dulParams->acceptedPresentationContext;
    if (*acceptedList != NULL) {
        acceptedPc =(DUL_PRESENTATIONCONTEXT*)  LST_Head(acceptedList);
        if (acceptedPc != NULL)
            (void)LST_Position(acceptedList, (LST_NODE*)acceptedPc);

        while (acceptedPc)
        {
            requestedPc = findPresentationContextID(
                dulParams->requestedPresentationContext,
                acceptedPc->presentationContextID);
            cond = updateRequestedPCFromAcceptedPC(acceptedPc, requestedPc);
            if (cond.bad()) return cond;
            acceptedPc = (DUL_PRESENTATIONCONTEXT*) LST_Next(acceptedList);
            if (acceptedPc != NULL)
                (void)LST_Position(acceptedList, (LST_NODE*)acceptedPc);
        }
    }
    return cond;
}

OFCondition
ASC_requestAssociation(T_ASC_Network * network,
                       T_ASC_Parameters * params,
                       T_ASC_Association ** assoc,
                       void **associatePDU,
                       unsigned long *associatePDUlength)
{
    OFCondition cond = EC_Normal;
    long sendLen;
    int retrieveRawPDU = 0;
    if (associatePDU && associatePDUlength) retrieveRawPDU = 1;

    if (network == NULL) return ASC_NULLKEY;
    if (params == NULL) return ASC_NULLKEY;

    if (ASC_countPresentationContexts(params) == 0)
    {
      return makeDcmnetCondition(ASCC_CODINGERROR, OF_error, "ASC Coding error in ASC_requestAssociation: missing presentation contexts");
    }

    *assoc = (T_ASC_Association *) malloc(sizeof(**assoc));
    if (*assoc == NULL) return EC_MemoryExhausted;
    bzero((char*)*assoc, sizeof(**assoc));

    (*assoc)->params = params;
    (*assoc)->nextMsgID = 1;
    (*assoc)->sendPDVLength = 0;
    (*assoc)->sendPDVBuffer = NULL;

    params->DULparams.maxPDU = params->ourMaxPDUReceiveSize;
    strcpy(params->DULparams.callingImplementationClassUID,
        params->ourImplementationClassUID);
    strcpy(params->DULparams.callingImplementationVersionName,
        params->ourImplementationVersionName);

    cond = DUL_RequestAssociation(&network->network,
                                  &(*assoc)->params->DULparams,
                                  &(*assoc)->DULassociation,
                                  retrieveRawPDU);


    if (retrieveRawPDU && assoc && ((*assoc)->DULassociation))
    {
      DUL_returnAssociatePDUStorage((*assoc)->DULassociation, *associatePDU, *associatePDUlength);
    }

    if (cond.good())
    {
       /*
        * The params->DULparams.peerMaxPDU parameter contains the
        * max-pdu-length value in the a-associate-ac (i.e. the max-pdu-length
        * that the remote AE is prepared to accept).
        */
        params->theirMaxPDUReceiveSize = params->DULparams.peerMaxPDU;

        if (!((params->theirMaxPDUReceiveSize & DUL_MAXPDUCOMPAT) ^ DUL_DULCOMPAT))
        {
          /* activate compatibility with DCMTK releases prior to 3.0 */
          DUL_activateCompatibilityMode((*assoc)->DULassociation, dcmEnableBackwardCompatibility.get() | DUL_DULCOMPAT | DUL_DIMSECOMPAT);
          if (params->modeCallback) params->modeCallback->callback(params->theirMaxPDUReceiveSize);
        }

        /* create a sendPDVBuffer */
        sendLen = params->theirMaxPDUReceiveSize;
        if (sendLen < 1) {
            /* the length is unlimited, choose a suitable buffer len */
            sendLen = ASC_MAXIMUMPDUSIZE;
        } else if (sendLen > ASC_MAXIMUMPDUSIZE) {
            sendLen = ASC_MAXIMUMPDUSIZE;
        }
        /* make sure max pdv length is even */
        if ((sendLen % 2) != 0)
        {
          DCMNET_WARN("ASSOC: PDV send length " << sendLen
                << " is odd (using " << (sendLen-1) << ")");
          sendLen--;
        }
        /* length is minus PDU and PDV header bytes */
        sendLen -= 12;
        if (sendLen < 1)
        {
            DCMNET_WARN("ASSOC: PDV send length " << sendLen << " (using default)");
            sendLen = ASC_MINIMUMPDUSIZE - 12;
        }
        if (sendLen < 12)
        {
            /* if sendLen is < 12, dcmdata will fail because it needs to put
             * up to 12 bytes into the send PDV buffer at once (tag header).
             * We use a larger value on this level and let the Upper Layer FSM
             * split the buffer for us into many small PDVs.
             */
            DCMNET_WARN("ASSOC: PDV send length too small, using DUL to split larger PDVs.");
            sendLen = ASC_MINIMUMPDUSIZE - 12;
        }
        (*assoc)->sendPDVLength = sendLen;
        (*assoc)->sendPDVBuffer = (unsigned char*)malloc(size_t(sendLen));
        if ((*assoc)->sendPDVBuffer == NULL) return EC_MemoryExhausted;
        strcpy(params->theirImplementationClassUID,
           params->DULparams.calledImplementationClassUID);
        strcpy(params->theirImplementationVersionName,
           params->DULparams.calledImplementationVersionName);

        /* make sure accepted PCs are marked as such in the requsted PC list */
        cond = updateRequestedPCListFromAcceptedPCList(&params->DULparams);
    }
    return cond;
}

OFCondition
ASC_acknowledgeAssociation(
    T_ASC_Association * assoc,
    void **associatePDU,
    unsigned long *associatePDUlength)
{
    long sendLen;

    if (assoc == NULL) return ASC_NULLKEY;
    if (assoc->DULassociation == NULL) return ASC_NULLKEY;

    int retrieveRawPDU = 0;
    if (associatePDU && associatePDUlength) retrieveRawPDU = 1;

    assoc->params->DULparams.maxPDU = assoc->params->ourMaxPDUReceiveSize;
    if (!((assoc->params->theirMaxPDUReceiveSize & DUL_MAXPDUCOMPAT) ^ DUL_DULCOMPAT))
    {
      assoc->params->DULparams.maxPDU = dcmEnableBackwardCompatibility.get() | DUL_DULCOMPAT | DUL_DIMSECOMPAT;
    }

    strcpy(assoc->params->DULparams.calledImplementationClassUID,
        assoc->params->ourImplementationClassUID);
    strcpy(assoc->params->DULparams.calledImplementationVersionName,
        assoc->params->ourImplementationVersionName);

    OFCondition cond = DUL_AcknowledgeAssociationRQ(&assoc->DULassociation,
                                        &assoc->params->DULparams,
                                        retrieveRawPDU);

    if (retrieveRawPDU && (assoc->DULassociation))
    {
      DUL_returnAssociatePDUStorage(assoc->DULassociation, *associatePDU, *associatePDUlength);
    }

    if (cond.good())
    {
        /* create a sendPDVBuffer */
        sendLen = assoc->params->theirMaxPDUReceiveSize;
        if (sendLen < 1) {
            /* the length is unlimited, choose a suitable buffer len */
            sendLen = ASC_MAXIMUMPDUSIZE;
        } else if (sendLen > ASC_MAXIMUMPDUSIZE) {
            sendLen = ASC_MAXIMUMPDUSIZE;
        }
        /* make sure max pdv length is even */
        if ((sendLen % 2) != 0)
        {
           DCMNET_WARN("ASSOC: PDV send length " << sendLen
                << " is odd (using " << (sendLen-1) << ")");
          sendLen--;
        }
        /* length is minus PDU and PDV header bytes */
        sendLen -= 12;
        if (sendLen < 1)
        {
           DCMNET_WARN("ASSOC: PDV send length " << sendLen
                << " (using default)");
           sendLen = ASC_MINIMUMPDUSIZE - 12;
        }
        if (sendLen < 12)
        {
            /* if sendLen is < 12, dcmdata will fail because it needs to put
             * up to 12 bytes into the send PDV buffer at once (tag header).
             * We use a larger value on this level and let the Upper Layer FSM
             * split the buffer for us into many small PDVs.
             */
            DCMNET_WARN("ASSOC: PDV send length too small, using DUL to split larger PDVs.");
            sendLen = ASC_MINIMUMPDUSIZE - 12;
        }
        assoc->sendPDVLength = sendLen;
        assoc->sendPDVBuffer = (unsigned char*)malloc(size_t(sendLen));
        if (assoc->sendPDVBuffer == NULL) return EC_MemoryExhausted;
    }
    return cond;
}

OFCondition
ASC_rejectAssociation(
    T_ASC_Association * association,
    T_ASC_RejectParameters * rejectParameters,
    void **associatePDU,
    unsigned long *associatePDUlength)
{
    DUL_ABORTITEMS l_abort;

    if (association == NULL) return ASC_NULLKEY;
    if (association->DULassociation == NULL) return ASC_NULLKEY;
    if (rejectParameters == NULL) return ASC_NULLKEY;

    int retrieveRawPDU = 0;
    if (associatePDU && associatePDUlength) retrieveRawPDU = 1;

    l_abort.result = (unsigned char)(rejectParameters->result & 0xff);
    l_abort.source = (unsigned char)(rejectParameters->source & 0xff);
    l_abort.reason = (unsigned char)(rejectParameters->reason & 0xff);

    OFCondition cond = DUL_RejectAssociationRQ(
        &association->DULassociation,
        &l_abort,
        retrieveRawPDU);

    if (retrieveRawPDU && (association->DULassociation))
    {
      DUL_returnAssociatePDUStorage(association->DULassociation, *associatePDU, *associatePDUlength);
    }

    return cond;
}

OFCondition
ASC_releaseAssociation(T_ASC_Association * association)
{
    if (association == NULL) return ASC_NULLKEY;
    if (association->DULassociation == NULL) return ASC_NULLKEY;
    return DUL_ReleaseAssociation(&association->DULassociation);
}

OFCondition ASC_acknowledgeRelease(T_ASC_Association *association)
{
    if (association == NULL) return ASC_NULLKEY;
    if (association->DULassociation == NULL) return ASC_NULLKEY;

    OFCondition cond = DUL_AcknowledgeRelease(&association->DULassociation);

    return cond;
}


OFCondition
ASC_abortAssociation(T_ASC_Association * association)
{
    if (association == NULL) return ASC_NULLKEY;
    if (association->DULassociation == NULL) return ASC_NULLKEY;

    OFCondition cond = DUL_AbortAssociation(&association->DULassociation);
    return cond;
}



OFCondition
ASC_dropSCPAssociation(T_ASC_Association * association, int timeout)
{
    /* if already dead don't worry */
    if (association == NULL) return EC_Normal;
    if (association->DULassociation == NULL) return EC_Normal;

    ASC_dataWaiting(association, timeout);
    OFCondition cond = DUL_DropAssociation(&association->DULassociation);

    return cond;
}



OFCondition
ASC_dropAssociation(T_ASC_Association * association)
{
    /* if already dead don't worry */
    if (association == NULL) return EC_Normal;
    if (association->DULassociation == NULL) return EC_Normal;

    OFCondition cond = DUL_DropAssociation(&association->DULassociation);
    return cond;
}



OFCondition
ASC_setTransportLayerType(
    T_ASC_Parameters * params,
    OFBool useSecureLayer)
{
  if (params == NULL) return ASC_NULLKEY;
  params->DULparams.useSecureLayer = useSecureLayer;
  return EC_Normal;
}

OFCondition
ASC_setTransportLayer(T_ASC_Network *network, DcmTransportLayer *newLayer, int takeoverOwnership)
{
  if (network == NULL) return ASC_NULLKEY;
  return DUL_setTransportLayer(network->network, newLayer, takeoverOwnership);
}

unsigned long ASC_getPeerCertificateLength(T_ASC_Association *assoc)
{
  if (assoc==NULL) return 0;
  return DUL_getPeerCertificateLength(assoc->DULassociation);
}

unsigned long ASC_getPeerCertificate(T_ASC_Association *assoc, void *buf, unsigned long bufLen)
{
  if (assoc==NULL) return 0;
  return DUL_getPeerCertificate(assoc->DULassociation, buf, bufLen);
}

void ASC_activateCallback(T_ASC_Parameters *params, DUL_ModeCallback *cb)
{
  if (params) params->modeCallback = cb;
}


// Deprecated wrapper functions follow
void ASC_printRejectParameters(FILE *f, T_ASC_RejectParameters *rej)
{
    OFString str;
    ASC_printRejectParameters(str, rej);
    fprintf(f, "%s\n", str.c_str());
}

void ASC_printRejectParameters(STD_NAMESPACE ostream& out, T_ASC_RejectParameters *rej)
{
    OFString str;
    ASC_printRejectParameters(str, rej);
    out << str << OFendl;
}

void
ASC_dumpParameters(T_ASC_Parameters * params, STD_NAMESPACE ostream& outstream)
{
    OFString str;
    // FIXME: ASC_ASSOC_AC is just a weird, wrong guess
    ASC_dumpParameters(str, params, ASC_ASSOC_AC);
    outstream << str << OFendl;
}

void
ASC_dumpPresentationContext(T_ASC_PresentationContext * presentationContext, STD_NAMESPACE ostream& outstream)
{
    outstream << ASC_dumpPresentationContext(presentationContext) << OFendl;
}

void
ASC_dumpConnectionParameters(T_ASC_Association *association, STD_NAMESPACE ostream& outstream)
{
    OFString str;
    ASC_dumpConnectionParameters(str, association);
    outstream << str << OFendl;
}


/*
** CVS Log
** $Log: assoc.cc,v $
** Revision 1.59  2010-12-01 08:26:35  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.58  2010-10-14 13:14:27  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.57  2010-09-09 08:32:06  joergr
** Fixed typo in OFCondition constants for SCP/SCU role selection failures.
**
** Revision 1.56  2010-08-26 09:21:18  joergr
** Fixed incorrect behavior of association acceptors during SCP/SCU role
** selection negotiation. Minor changes to output format of ACSE parameters.
** Introduced new global flag which allows for rejecting presentation contexts
** in case of an unsuccessful SCP/SCU role selection (disabled by default).
**
** Revision 1.55  2010-06-14 12:35:47  onken
** Improved efficiency of finding an appropriate presentation context.
**
** Revision 1.54  2009-11-18 11:53:59  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.53  2009-08-03 15:39:13  meichel
** Fixed application crash due to dereferenced NULL pointer that affected
**   some SCP applications when connected by a non-DICOM tool such as nmap.
**
** Revision 1.52  2009-08-03 15:32:35  meichel
** Fixed resource leak the in code creating forked child processes
**
** Revision 1.51  2008-10-07 09:07:28  onken
** Fixed possible memory leak in user identity classes and added code for
** accessing user identity from the server's side.
**
** Revision 1.50  2008-04-17 15:27:35  onken
** Reworked and extended User Identity Negotiation code.
**
** Revision 1.49  2007-10-19 12:02:30  onken
** Fixed typo in debug output.
**
** Revision 1.48  2007/09/07 08:49:29  onken
** Added basic support for Extended Negotiation of User Identity.
**
** Revision 1.47  2006/08/15 16:04:29  meichel
** Updated the code in module dcmnet to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.46  2005/12/08 15:44:24  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.45  2005/11/25 11:31:14  meichel
** StoreSCP now supports multi-process mode both on Posix and Win32 platforms
**   where a separate client process is forked for each incoming association.
**
** Revision 1.44  2004/07/15 08:10:46  meichel
** Added optional timeout parameter to ASC_dropSCPAssociation().
**
** Revision 1.43  2004/04/07 10:22:10  meichel
** Added optional parameter to ASC_initializeNetwork that allows to pass
**   the DUL_FULLDOMAINNAME option to the DUL layer
**
** Revision 1.42  2004/02/25 12:31:17  meichel
** Added global option flag for compatibility with very old DCMTK releases in the
**   DICOM upper layer and ACSE code. Default is automatic handling, which should
**   work in most cases.
**
** Revision 1.41  2003/07/03 14:21:10  meichel
** Added special handling for FD_SET() on MinGW, which expects an
**   unsigned first argument.
**
** Revision 1.40  2003/06/02 16:44:11  meichel
** Renamed local variables to avoid name clashes with STL
**
** Revision 1.39  2002/11/28 17:15:23  meichel
** Fixed incorrect comment.
**
** Revision 1.38  2002/11/27 13:04:37  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.37  2002/07/10 11:43:58  meichel
** Replaced dcmnet specific definitions for implementation class UID and
**   version name by the constants defined in dcmdata.
**
** Revision 1.36  2001/12/04 16:58:12  meichel
** Implemented strlcpy and strlcat routines compatible with the
**   corresponding BSD libc routines in class OFStandard
**
** Revision 1.35  2001/11/27 09:54:56  wilkens
** Updated storescp. 6 new options (--output-directory, --sort-conc-studies,
** --exec-on-reception, --exec-on-eostudy, --rename-on-eostudy, and
** --eostudy-timeout) implemented (requirements from GO-Kard).
**
** Revision 1.34  2001/10/12 10:18:29  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.33  2000/12/19 12:06:19  meichel
** Renamed typedef T_UID which leads to name clash with system macro on OSF/1
**
** Revision 1.32  2000/10/10 12:06:07  meichel
** Added version of function ASC_printRejectParameters that takes
**   an STD_NAMESPACE ostream& instead of a FILE*
**
** Revision 1.31  2000/08/10 14:50:55  meichel
** Added initial OpenSSL support.
**
** Revision 1.30  2000/06/07 13:56:21  meichel
** Output stream now passed as mandatory parameter to ASC_dumpParameters.
**
** Revision 1.29  2000/06/07 08:57:25  meichel
** dcmnet ACSE routines now allow to retrieve a binary copy of the A-ASSOCIATE
**   RQ/AC/RJ PDUs, e.g. for logging purposes.
**
** Revision 1.28  2000/03/03 14:11:18  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.27  2000/03/02 12:44:40  joergr
** Added new class comprising all general purpose helper functions (first
** entry: strlcpy - a mixture of strcpy and strncpy).
**
** Revision 1.26  2000/02/29 12:21:27  meichel
** Dcmtk now supports transmission with very small max PDU size
**   (less than 24 bytes). In this case dcmdata uses a larger block size
**   than dcmnet because it requires at least 12 bytes of buffer space.
**
** Revision 1.25  2000/02/23 15:12:27  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.24  2000/02/02 13:34:29  meichel
** Fixed bug in ACSE code that could cause data in memory to be overwritten
**   if a client sent an A-ASSOCIATE request with a longer ImplementationClassUID
**   or ImplementationVersionName than allowed.
**
** Revision 1.23  2000/02/01 10:24:06  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.22  1999/09/06 13:27:49  meichel
** Fixed bug in network module: Max receive PDU was used for max send PDU,
**   under certain circumstances resulting in a very inefficient splitting
**   of PDUs on the DUL level, severely decreasing network performance.
**
** Revision 1.21  1999/04/26 17:20:58  meichel
** Added new "transfer syntax aware" variant of the dcmnet function
**   ASC_findAcceptedPresentationContextID. This variant tries to find an
**   accepted presentation context that matches both abstract and transfer syntax.
**
** Revision 1.20  1999/04/19 08:38:56  meichel
** Added experimental support for extended SOP class negotiation.
**
** Revision 1.19  1999/03/29 11:20:02  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.18  1999/01/07 14:25:01  meichel
** Changed sequence of include files in some dcmnet modules
**   to keep the Unixware compiler happy.
**
** Revision 1.17  1998/06/29 12:14:31  meichel
** Removed some name clashes (e.g. local variable with same
**   name as class member) to improve maintainability.
**   Applied some code purifications proposed by the gcc 2.8.1 -Weffc++ option.
**
** Revision 1.16  1997/09/18 08:10:56  meichel
** Many minor type conflicts (e.g. long passed as int) solved.
**
** Revision 1.15  1997/08/05 07:38:17  andreas
** Corrected error in DUL finite state machine
** SCPs shall close sockets after the SCU have closed the socket in
** a normal association release. Therfore, an ARTIM timer is described
** in DICOM part 8 that is not implemented correctly in the
** DUL. Since the whole DUL finite state machine is affected, we
** decided to solve the proble outside the fsm. Now it is necessary to call the
** ASC_DropSCPAssociation() after the calling ASC_acknowledgeRelease().
**
** Revision 1.14  1997/07/24 13:10:56  andreas
** - Removed Warnings from SUN CC 2.0.1
**
** Revision 1.13  1997/07/21 08:47:14  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.12  1997/07/04 11:44:31  meichel
** Configure now also tests <sys/select.h> if available
**   when searching for a select() prototype.
**   Updated files using select() to include <sys/select.h> and
**   <sys/types.h> if available (needed for AIX).
**
** Revision 1.11  1997/04/15 16:18:50  hewett
** The network function ASC_destroyAssociation now only destroys the
** association parameters if they are non-NULL.
**
** Revision 1.10  1996/12/03 15:29:46  meichel
** Added support for HP-UX 9.05 systems using GCC 2.7.2.1
**
** Revision 1.9  1996/09/27 08:36:14  hewett
** Eliminated used before set warning (IBM AIX C Set++ Compiler).
**
** Revision 1.8  1996/09/03 11:42:56  hewett
** When requesting an association, added check that presentation
** contexts have been defined.
**
** Revision 1.7  1996/06/20 07:35:47  hewett
** Removed inclusion of system header already included by dcompat.h
** and made sure that dcompat.h is always included (via dicom.h).
**
** Revision 1.6  1996/05/06 07:32:05  hewett
** Fixed problem whereby only the first presentation context was being
** recognised as having been accepted.
**
** Revision 1.5  1996/05/03 10:31:52  hewett
** Move some common system include files out to include/dcompat.h which were
** causing problems due to multiple inclusion on some machines.
**
** Revision 1.4  1996/04/27 12:57:57  hewett
** Corrected cause of warnings when compiling under "c++ -O -g -Wall"
** under Solaris 2.4.  Mostly due to unintialized variables.
**
** Revision 1.3  1996/04/25 16:11:11  hewett
** Added parameter casts to char* for bzero calls.  Replaced some declarations
** of DIC_UL with unsigned long (reduces mismatch problems with 32 & 64 bit
** architectures).  Added some protection to inclusion of sys/socket.h (due
** to MIPS/Ultrix).
**
** Revision 1.2  1996/04/22 10:00:54  hewett
** Corrected incorrect translation of reject reason from DUL to ASC.
**
** Revision 1.1.1.1  1996/03/26 18:38:45  hewett
** Initial Release.
**
**
*/
