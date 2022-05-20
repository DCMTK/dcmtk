/*
 *
 *  Copyright (C) 1994-2023, OFFIS e.V.
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
**      information.  Also provided are routines for performing association
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
*/

/*
** Include Files
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_WINDOWS_H
// on Windows, we need Winsock2 for network functions
#include <winsock2.h>
#endif

#include "dcmtk/dcmnet/assoc.h"       /* always include the module header */
#include "dcmtk/dcmnet/diutil.h"

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef DCMTK_HAVE_POLL
#include <poll.h>
#endif

#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmnet/dcmtrans.h"
#include "dcmtk/dcmnet/helpers.h"

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

OFCondition
ASC_createAssociationParameters(T_ASC_Parameters ** params,
                                long maxReceivePDUSize,
                                Sint32 tcpConnectTimeout)
{

    *params = (T_ASC_Parameters *) malloc(sizeof(**params));
    if (*params == NULL) return EC_MemoryExhausted;
    memset((char*)*params, 0, sizeof(**params));

    OFStandard::strlcpy((*params)->ourImplementationClassUID,
            OFFIS_IMPLEMENTATION_CLASS_UID,
            sizeof((*params)->ourImplementationClassUID));
    OFStandard::strlcpy((*params)->ourImplementationVersionName,
            OFFIS_DTK_IMPLEMENTATION_VERSION_NAME,
            sizeof((*params)->ourImplementationVersionName));

    if (strlen(OFFIS_DTK_IMPLEMENTATION_VERSION_NAME) > 16)
    {
      DCMNET_WARN("DICOM implementation version name too long: " << OFFIS_DTK_IMPLEMENTATION_VERSION_NAME);
    }

    OFStandard::strlcpy((*params)->DULparams.callingImplementationClassUID,
        (*params)->ourImplementationClassUID, DICOM_UI_LENGTH + 1);
    OFStandard::strlcpy((*params)->DULparams.callingImplementationVersionName,
        (*params)->ourImplementationVersionName, 16+1);

    OFStandard::strlcpy((*params)->DULparams.applicationContextName,
            UID_StandardApplicationContext,
            sizeof((*params)->DULparams.applicationContextName));

    ASC_setAPTitles(*params,
                    "calling AP Title",
                    "called AP Title",
                    "");

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
    (*params)->DULparams.tcpConnectTimeout = tcpConnectTimeout;
    return EC_Normal;
}



OFCondition
ASC_createAssociationParameters(T_ASC_Parameters ** params,
                                long maxReceivePDUSize)
{
    /* pass global TCP connection timeout to the real function */
    return ASC_createAssociationParameters(params, maxReceivePDUSize, dcmConnectionTimeout.get());
}


OFCondition
ASC_destroyAssociationParameters(T_ASC_Parameters ** params)
{

    /* free the elements in the requested presentation context list */
    destroyDULParamPresentationContextList(
        &((*params)->DULparams.requestedPresentationContext));

    /* free the elements in the accepted presentation context list */
    destroyDULParamPresentationContextList(
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
{
    if (callingAPTitle)
        OFStandard::strlcpy(params->DULparams.callingAPTitle, callingAPTitle,
                sizeof(params->DULparams.callingAPTitle));
    if (calledAPTitle)
        OFStandard::strlcpy(params->DULparams.calledAPTitle, calledAPTitle,
                sizeof(params->DULparams.calledAPTitle));
    if (respondingAPTitle)
        OFStandard::strlcpy(params->DULparams.respondingAPTitle, respondingAPTitle,
                sizeof(params->DULparams.respondingAPTitle));

    return EC_Normal;
}

OFCondition
ASC_getAPTitles(T_ASC_Parameters * params,
                char* callingAPTitle,
                size_t callingAPTitleSize,
                char* calledAPTitle,
                size_t calledAPTitleSize,
                char* respondingAPTitle,
                size_t respondingAPTitleSize)
{
    if (callingAPTitle)
        OFStandard::strlcpy(callingAPTitle, params->DULparams.callingAPTitle, callingAPTitleSize);
    if (calledAPTitle)
        OFStandard::strlcpy(calledAPTitle, params->DULparams.calledAPTitle, calledAPTitleSize);
    if (respondingAPTitle)
        OFStandard::strlcpy(respondingAPTitle, params->DULparams.respondingAPTitle, respondingAPTitleSize);

    return EC_Normal;
}

OFCondition
ASC_getApplicationContextName(T_ASC_Parameters * params,
                              char* applicationContextName,
                              size_t applicationContextNameSize)
{
    if (applicationContextName)
        OFStandard::strlcpy(applicationContextName,
            params->DULparams.applicationContextName,
            applicationContextNameSize);
    return EC_Normal;
}

OFCondition
ASC_setPresentationAddresses(T_ASC_Parameters * params,
                             const char* callingPresentationAddress,
                             const char* calledPresentationAddress)
{
    if (callingPresentationAddress)
        OFStandard::strlcpy(params->DULparams.callingPresentationAddress,
                callingPresentationAddress,
                sizeof(params->DULparams.callingPresentationAddress));
    if (calledPresentationAddress)
        OFStandard::strlcpy(params->DULparams.calledPresentationAddress,
                calledPresentationAddress,
                sizeof(params->DULparams.calledPresentationAddress));

    return EC_Normal;
}

OFCondition
ASC_getPresentationAddresses(T_ASC_Parameters * params,
                             char* callingPresentationAddress,
                             size_t callingPresentationAddressSize,
                             char* calledPresentationAddress,
                             size_t calledPresentationAddressSize)
{
    if (callingPresentationAddress)
        OFStandard::strlcpy(callingPresentationAddress,
               params->DULparams.callingPresentationAddress, callingPresentationAddressSize);
    if (calledPresentationAddress)
        OFStandard::strlcpy(calledPresentationAddress,
               params->DULparams.calledPresentationAddress, calledPresentationAddressSize);

    return EC_Normal;
}

OFCondition
ASC_getRejectParameters(T_ASC_Parameters * params,
                        T_ASC_RejectParameters * rejectParameters)
{
    int reason;
    if (rejectParameters) {

        switch (params->DULparams.result)
        {
          case ASC_RESULT_REJECTEDPERMANENT:
          case ASC_RESULT_REJECTEDTRANSIENT:
            rejectParameters->result =
              (T_ASC_RejectParametersResult) params->DULparams.result;
            break;
          default:
            DCMNET_ERROR("Received invalid A-ASSOCIATE-RJ reject result 0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
              << STD_NAMESPACE setw(4) << params->DULparams.result << ", using default.");
            rejectParameters->result = ASC_RESULT_REJECTEDPERMANENT;
            break;
        }

        switch (params->DULparams.resultSource)
        {
          case ASC_SOURCE_SERVICEUSER:
          case ASC_SOURCE_SERVICEPROVIDER_ACSE_RELATED:
          case ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED:
            rejectParameters->source =
                (T_ASC_RejectParametersSource) params->DULparams.resultSource;
            break;
          default:
            DCMNET_ERROR("Received invalid A-ASSOCIATE-RJ reject source 0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
              << STD_NAMESPACE setw(4) << params->DULparams.resultSource << ", using default.");
            rejectParameters->source = ASC_SOURCE_SERVICEUSER;
            break;
        }

        reason = params->DULparams.diagnostic |
            ((params->DULparams.resultSource & 0xff) << 8);

        switch (reason)
        {
          case ASC_REASON_SU_NOREASON:
          case ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED:
          case ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED:
          case ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED:
          case ASC_REASON_SP_ACSE_NOREASON:
          case ASC_REASON_SP_ACSE_PROTOCOLVERSIONNOTSUPPORTED:
          case ASC_REASON_SP_PRES_TEMPORARYCONGESTION:
          case ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED:
            rejectParameters->reason = (T_ASC_RejectParametersReason) reason;
            break;
          default:
            DCMNET_ERROR("Received invalid A-ASSOCIATE-RJ reject reason 0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
              << STD_NAMESPACE setw(4) << reason << ", using default.");
            rejectParameters->reason = ASC_REASON_SU_NOREASON;
            break;
        }
    }
    return EC_Normal;
}

OFString&
ASC_printRejectParameters(OFString& str, const T_ASC_RejectParameters *rej)
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
        source = "Service User"; break;
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

DUL_SC_ROLE
ascRole2dulRole(const T_ASC_SC_ROLE role)
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

const char*
ASC_role2String(const T_ASC_SC_ROLE role)
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
    if (lst == NULL)
    {
        free(pc);
        return EC_MemoryExhausted;
    }
    pc->presentationContextID = presentationContextID;
    OFStandard::strlcpy(pc->abstractSyntax, abstractSyntax, sizeof(pc->abstractSyntax));
    pc->result = ASC_P_NOTYETNEGOTIATED;
    pc->proposedSCRole = ascRole2dulRole(proposedRole);
    pc->acceptedSCRole = ascRole2dulRole(ASC_SC_ROLE_DEFAULT);

    /* there must be at least one transfer syntax */
    if (transferSyntaxListCount < 1)
    {
      free(pc);
      return ASC_MISSINGTRANSFERSYNTAX;
    }

    /* add the transfer syntaxes */
    for (i=0; i<transferSyntaxListCount; i++)
    {
        transfer = (DUL_TRANSFERSYNTAX*)malloc(sizeof(DUL_TRANSFERSYNTAX));
        if (transfer == NULL) return EC_MemoryExhausted;
        OFStandard::strlcpy(transfer->transferSyntax, transferSyntaxList[i], sizeof(transfer->transferSyntax));
        LST_Enqueue(&lst, (LST_NODE*)transfer);
    }
    pc->proposedTransferSyntax = lst;

    /* add to presentation context list */
    lst = params->DULparams.requestedPresentationContext;
    if (lst == NULL) {
        lst = LST_Create();
        if (lst == NULL)
        {
            free(pc);
            return EC_MemoryExhausted;
        }
    }

    LST_Enqueue(&lst, (LST_NODE*)pc);

    params->DULparams.requestedPresentationContext = lst;
    return EC_Normal;
}

int
ASC_countPresentationContexts(T_ASC_Parameters * params)
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
{
    DUL_PRESENTATIONCONTEXT *pc;
    DUL_TRANSFERSYNTAX *transfer;
    LST_HEAD **l;
    int count = 0;

    /* make the presentation context structure clean */
    memset((char*)presentationContext, 0, sizeof(*presentationContext));

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
    OFStandard::strlcpy(presentationContext->abstractSyntax,
           pc->abstractSyntax, sizeof(presentationContext->abstractSyntax));
    if (presentationContext->resultReason == ASC_P_ACCEPTANCE) {
        OFStandard::strlcpy(presentationContext->acceptedTransferSyntax,
            pc->acceptedTransferSyntax, sizeof(presentationContext->acceptedTransferSyntax));
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
        OFStandard::strlcpy(presentationContext->proposedTransferSyntaxes[count],
               transfer->transferSyntax, sizeof(presentationContext->proposedTransferSyntaxes[count]));
        count++;
        transfer = (DUL_TRANSFERSYNTAX*) LST_Next(l);
    }

    presentationContext->transferSyntaxCount = OFstatic_cast(unsigned char, count);

    return EC_Normal;
}

OFCondition
ASC_acceptPresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    const char* transferSyntax,
    T_ASC_SC_ROLE acceptedRole,
    const OFBool alwaysAcceptDefaultRole)
{
    DUL_PRESENTATIONCONTEXT *proposedContext, *acceptedContext;
    OFCondition cond = EC_Normal;
    LST_HEAD *lst;

    proposedContext = findPresentationContextID(
                              params->DULparams.requestedPresentationContext,
                                                presentationContextID);
    if (proposedContext == NULL) return ASC_BADPRESENTATIONCONTEXTID;
    OFStandard::strlcpy(proposedContext->acceptedTransferSyntax, transferSyntax, sizeof(proposedContext->acceptedTransferSyntax));

    /* we want to mark this proposed context as being ok */
    proposedContext->result = ASC_P_ACCEPTANCE;
    proposedContext->acceptedSCRole = ascRole2dulRole(acceptedRole);

    /* Here we check the only role selection case which leads to clear rejection of the
     * proposed presentation context: If the requestor connects with default role but the
     * acceptor explicitly requires the SCP role (only) then the presentation context
     * will be rejected. All other cases do not lead to rejection but to actual "negotiation".
     * DCMTK's behaviour can be seen in the declaration of enum DUL_SC_ROLE (see dul.h).
     * The logic of the role negotiation is implemented in constructSCUSCPRoles() (see dulconst.cc).
     */
    if ( (proposedContext->proposedSCRole == DUL_SC_ROLE_DEFAULT)
      && (proposedContext->acceptedSCRole == DUL_SC_ROLE_SCP) )
    {
      // If user wants to override rejection (e.g. for faulty clients), skip the check but print warning
      if (alwaysAcceptDefaultRole)
      {
        DCMNET_WARN("ASSOC: Deliberately accepting Default role proposed by association requestor, "
            << "while originally being configured for role SCP only");
      }
      else
      {
        proposedContext->result = ASC_P_NOREASON;
          DCMNET_ERROR("ASSOC: SCP/SCU role selection failed, Default role (i.e. SCU) proposed "
              << "but only SCP role configured for acceptance");
          return ASC_SCPSCUROLESELECTIONFAILED;
      }
    }

    acceptedContext = findPresentationContextID(
                              params->DULparams.acceptedPresentationContext,
                                                presentationContextID);

    if (acceptedContext != NULL) {
        /* it is already in the list, mark it as accepted */
        acceptedContext->result = ASC_P_ACCEPTANCE;
        OFStandard::strlcpy(acceptedContext->abstractSyntax,
               proposedContext->abstractSyntax, sizeof(acceptedContext->abstractSyntax));
        OFStandard::strlcpy(acceptedContext->acceptedTransferSyntax, transferSyntax, sizeof(acceptedContext->acceptedTransferSyntax));
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

        LST_Enqueue(&lst, (LST_NODE*)acceptedContext);
        params->DULparams.acceptedPresentationContext = lst;
    }
    return EC_Normal;
}


OFCondition
ASC_refusePresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    T_ASC_P_ResultReason resultReason)
{
    DUL_PRESENTATIONCONTEXT *proposedContext, *acceptedContext;
    OFCondition cond = EC_Normal;
    LST_HEAD *lst;

    proposedContext = findPresentationContextID(
                             params->DULparams.requestedPresentationContext,
                                                presentationContextID);
    if (proposedContext == NULL) return ASC_BADPRESENTATIONCONTEXTID;

    /* we want to mark this proposed context as being refused */
    proposedContext->result = OFstatic_cast(unsigned char, resultReason);

    acceptedContext = findPresentationContextID(
                              params->DULparams.acceptedPresentationContext,
                                                presentationContextID);

    if (acceptedContext != NULL) {
        /* it is already in the list, mark it as refused */
        acceptedContext->result = OFstatic_cast(unsigned char, resultReason);
        OFStandard::strlcpy(acceptedContext->abstractSyntax,
               proposedContext->abstractSyntax, sizeof(acceptedContext->abstractSyntax));
        /* we must send back a transfer syntax even though this
         * presentation context is refused.  Some software implementations
         * seem to get confused if we don't.
         */
        OFStandard::strlcpy(acceptedContext->acceptedTransferSyntax,
                UID_LittleEndianImplicitTransferSyntax, sizeof(acceptedContext->acceptedTransferSyntax));
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
            presentationContextID, OFstatic_cast(unsigned char, resultReason),
            proposedContext->abstractSyntax,
            UID_LittleEndianImplicitTransferSyntax, NULL);
        if (cond.bad()) return cond;

        lst = params->DULparams.acceptedPresentationContext;
        if (lst == NULL) {
            lst = LST_Create();
            if (lst == NULL) return EC_MemoryExhausted;
        }
        LST_Enqueue(&lst, (LST_NODE*)acceptedContext);
        params->DULparams.acceptedPresentationContext = lst;
    }
    return EC_Normal;
}


OFCondition
ASC_findAcceptedPresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    T_ASC_PresentationContext * presentationContext)
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

    presentationContext->transferSyntaxCount = OFstatic_cast(unsigned char, count);
    OFStandard::strlcpy(presentationContext->acceptedTransferSyntax, pc->acceptedTransferSyntax, sizeof(DIC_UI));

    return EC_Normal;
}

T_ASC_PresentationContextID
ASC_findAcceptedPresentationContextID(
    T_ASC_Association *assoc,
    const char* abstractSyntax)
{
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
{
    OFCondition cond = EC_Normal;
    int n, i, j, k;
    DUL_PRESENTATIONCONTEXT *dpc;
    T_ASC_PresentationContext pc;
    OFBool accepted = OFFalse;
    OFBool abstractOK = OFFalse;

    n = ASC_countPresentationContexts(params);
    // No presentation context proposed at all? Return error.
    if (n == 0)
    {
      return ASC_NOPRESENTATIONCONTEXTPROPOSED;
    }

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
    const OFBool requestRsp)
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
  rq->setPrimField(userName.c_str(), OFstatic_cast(Uint16, userName.length()));
  rq->setSecField(password.c_str(), OFstatic_cast(Uint16, password.length()));
  rq->setReqPosResponse(requestRsp);
  params->DULparams.reqUserIdentNeg = rq;
  return EC_Normal;
}


OFCondition ASC_setIdentRQUserOnly(
    T_ASC_Parameters * params,
    const OFString& userName,
    const OFBool requestRsp)
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
  rq->setPrimField(userName.c_str(), OFstatic_cast(Uint16, userName.length()));
  rq->setReqPosResponse(requestRsp);
  params->DULparams.reqUserIdentNeg = rq;
  return EC_Normal;
}


OFCondition ASC_setIdentRQKerberos(
    T_ASC_Parameters * params,
    const char* kerbTicket,
    const Uint16 length,
    const OFBool requestRsp)
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
    const Uint16 length,
    const OFBool requestRsp)
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


OFCondition ASC_setIdentRQJwt(
    T_ASC_Parameters * params,
    const char* jwt,
    const Uint16 length,
    const OFBool requestRsp)
{
  if (params == NULL)
    return ASC_NULLKEY;
  UserIdentityNegotiationSubItemRQ* rq = params->DULparams.reqUserIdentNeg;
  if (rq == NULL)
    rq = new UserIdentityNegotiationSubItemRQ();
  else
    rq->clear();
  rq->setIdentityType(ASC_USER_IDENTITY_JWT);
  rq->setPrimField(jwt, length);
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
    const Uint16 length )
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
        << ASC_role2String(p->proposedRole) << OFendl;

    if (p->resultReason != ASC_P_NOTYETNEGOTIATED) {
        outstream << "    Accepted SCP/SCU Role: "
            << ASC_role2String(p->acceptedRole) << OFendl;
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
        << params->theirImplementationVersionName << OFendl
        << "Application Context Name:    "
        << params->DULparams.applicationContextName << OFendl
        << "Calling Application Name:    "
        << params->DULparams.callingAPTitle << OFendl
        << "Called Application Name:     "
        << params->DULparams.calledAPTitle << OFendl
        << "Responding Application Name: ";

    // the field "respondingAPTitle" in DULparams exists,
    // but is never used for network communication because DICOM
    // requires the responding AE title to be identical to the
    // called AE title. This rule is enforced on the DUL layer
    // but not visible here.
    // To avoid confusion of the user we thus print the called
    // AE title here (but only if respondingAPTitle is non-empty,
    // which happens when an incoming association request is
    // being responded to.
    if (params->DULparams.respondingAPTitle[0] != '\0')
    {
      outstream << params->DULparams.calledAPTitle ;
    }

    outstream
        << OFendl
        << "Our Max PDU Receive Size:    "
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
    // the following output could be useful for debugging purposes
    outstream << "DUL Params --- BEGIN" << OFendl;
    outstream << DUL_DumpParams(temp_str, &params->DULparams);
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
#ifdef _WIN32
    SOCKET s;
#else
    int s;
#endif
    struct timeval t;
    int nfound;

    if (network == NULL) return OFFalse;

    s = DUL_networkSocket(network->network);

#ifdef _WIN32
    if (s == INVALID_SOCKET) return OFFalse;
#else
    if (s < 0)
        return OFFalse;
#endif

#ifndef DCMTK_HAVE_POLL
    fd_set fdset;
    FD_ZERO(&fdset);
#ifdef __MINGW32__
    // on MinGW, FD_SET expects an unsigned first argument
    FD_SET((unsigned int) s, &fdset);
#else
    FD_SET(s, &fdset);
#endif /* __MINGW32__ */
#endif /* DCMTK_HAVE_POLL */
    t.tv_sec = timeout;
    t.tv_usec = 0;

#ifdef DCMTK_HAVE_POLL
    struct pollfd pfd[] =
    {
        { s, POLLIN, 0 }
    };
    nfound = poll(pfd, 1, t.tv_sec*1000+(t.tv_usec/1000));
#else
#ifdef HAVE_INTP_SELECT
    nfound = select(OFstatic_cast(int, s + 1), (int *)(&fdset), NULL, NULL, &t);
#else
    // the typecast is safe because Windows ignores the first select() parameter anyway
    nfound = select(OFstatic_cast(int, s + 1), &fdset, NULL, NULL, &t);
#endif /* HAVE_INTP_SELECT */
#endif /* DCMTK_HAVE_POLL */
    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
      DU_logSelectResult(nfound);
    }
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
        ASC_destroyAssociationParameters(&(*association)->params);
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

    OFCondition cond = ASC_createAssociationParameters(&params, maxReceivePDUSize, dcmConnectionTimeout.get());
    if (cond.bad()) return cond;

    cond = ASC_setTransportLayerType(params, useSecureLayer);
    if (cond.bad())
    {
        ASC_destroyAssociationParameters(&params);
        return cond;
    }

    *assoc = (T_ASC_Association *) malloc(sizeof(**assoc));
    if (*assoc == NULL)
    {
        ASC_destroyAssociationParameters(&params);
        return EC_MemoryExhausted;
    }
    memset((char*)*assoc, 0, sizeof(**assoc));

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
    OFStandard::strlcpy(params->theirImplementationClassUID,
            params->DULparams.callingImplementationClassUID,
            sizeof (params->theirImplementationClassUID));
    OFStandard::strlcpy(params->theirImplementationVersionName,
            params->DULparams.callingImplementationVersionName,
            sizeof (params->theirImplementationVersionName));

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
        OFStandard::strlcpy(rpc->acceptedTransferSyntax, apc->acceptedTransferSyntax, sizeof(rpc->acceptedTransferSyntax));
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
                       unsigned long *associatePDUlength,
                       DUL_BLOCKOPTIONS block,
                       int timeout)
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
    memset((char*)*assoc, 0, sizeof(**assoc));

    (*assoc)->params = params;
    (*assoc)->nextMsgID = 1;
    (*assoc)->sendPDVLength = 0;
    (*assoc)->sendPDVBuffer = NULL;

    params->DULparams.maxPDU = params->ourMaxPDUReceiveSize;
    OFStandard::strlcpy(params->DULparams.callingImplementationClassUID,
        params->ourImplementationClassUID, DICOM_UI_LENGTH + 1);
    OFStandard::strlcpy(params->DULparams.callingImplementationVersionName,
        params->ourImplementationVersionName, 16+1);

    cond = DUL_RequestAssociation(&network->network, block, timeout,
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
        OFStandard::strlcpy(params->theirImplementationClassUID,
           params->DULparams.calledImplementationClassUID, sizeof(params->theirImplementationClassUID));
        OFStandard::strlcpy(params->theirImplementationVersionName,
           params->DULparams.calledImplementationVersionName, sizeof(params->theirImplementationVersionName));

        /* make sure accepted PCs are marked as such in the requested PC list */
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

    OFStandard::strlcpy(assoc->params->DULparams.calledImplementationClassUID,
        assoc->params->ourImplementationClassUID, sizeof(assoc->params->DULparams.calledImplementationClassUID));
    OFStandard::strlcpy(assoc->params->DULparams.calledImplementationVersionName,
        assoc->params->ourImplementationVersionName, sizeof(assoc->params->DULparams.calledImplementationVersionName));

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
    const T_ASC_RejectParameters * rejectParameters,
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
ASC_closeTransportConnection(T_ASC_Association * association)
{
    /* if already dead don't worry */
    if (association == NULL) return EC_Normal;
    if (association->DULassociation == NULL) return EC_Normal;

    return DUL_CloseTransportConnection(&association->DULassociation);
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
void ASC_printRejectParameters(FILE *f, const T_ASC_RejectParameters *rej)
{
    OFString str;
    ASC_printRejectParameters(str, rej);
    fprintf(f, "%s\n", str.c_str());
}

void ASC_printRejectParameters(STD_NAMESPACE ostream& out, const T_ASC_RejectParameters *rej)
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

void
destroyDULParamPresentationContextList(LST_HEAD ** lst)
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

void ASC_setParentProcessMode(T_ASC_Association *association)
{
  if (association) DUL_setParentProcessMode(association->DULassociation);
}
