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
**      Module Prefix: ASC_
**
**
** Last Update:         $Author: meichel $
** Update Date:         $Date: 2000-12-19 12:06:19 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/assoc.cc,v $
** CVS/RCS Revision:    $Revision: 1.33 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/

/* 
** Include Files
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <errno.h>

#include "dicom.h"
#include "cond.h"
#include "asccond.h"
#include "assoc.h"      /* always include the module header */
#include "dcuid.h"
#include "ofconsol.h"
#include "ofstd.h"
#include "dcmtrans.h"

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
** Internal Functions
*/

static CONDITION 
convertDULtoASCCondition(CONDITION cond)
{
    CONDITION newcond;

    switch (cond) {
    case DUL_NORMAL:
        newcond = ASC_NORMAL;
        break;
    case DUL_ASSOCIATIONREJECTED:
        newcond = ASC_ASSOCIATIONREJECTED;
        break;
#if 0
/* never gets generated */
    case DUL_PEERRELEASEDASSOCIATION:
        newcond = ASC_PEERRELEASEDASSOCIATION;
        break;
#endif
    case DUL_PEERABORTEDASSOCIATION:
        newcond = ASC_PEERABORTEDASSOCIATION;
        break;
    case DUL_PEERDROPPEDASSOCIATION:
        newcond = ASC_PEERDROPPEDASSOCIATION;
        break;
    case DUL_REQUESTASSOCIATIONFAILED:
        newcond = ASC_REQUESTASSOCIATIONFAILED;
        break;
    case DUL_APABORT:
        newcond = ASC_APABORT;
        break;
    case DUL_RELEASECONFIRMED:
        newcond = ASC_RELEASECONFIRMED;
        break;
    case DUL_PEERREQUESTEDRELEASE:
        newcond = ASC_PEERREQUESTEDRELEASE;
        break;
    case DUL_READTIMEOUT:
        newcond = ASC_READTIMEOUT;
        break;

    default:
        return COND_PushCondition(ASC_NETWORKERROR,
                                  ASC_Message(ASC_NETWORKERROR));
        /* break; */ // never reached after return statement
    }

    return newcond;
}

/*
** Function Bodies
*/

/* 
 * Network creation/distroy wrappers.
 * The T_ASC_Network structure will be allocated/freed by
 * these routines.
 */

CONDITION 
ASC_initializeNetwork(T_ASC_NetworkRole role,
                      int acceptorPort,
                      int timeout,
                      T_ASC_Network ** network)
{
    CONDITION cond;
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

    cond = DUL_InitializeNetwork(DUL_NETWORK_TCP, mode, &acceptorPort,
                                 timeout, DUL_ORDERBIGENDIAN, &netkey);
    if (cond != DUL_NORMAL) {
        return convertDULtoASCCondition(cond);
    }
    *network = (T_ASC_Network *) malloc(sizeof(T_ASC_Network));
    if (*network == NULL) {
        return COND_PushCondition(ASC_MALLOCERROR,
                     ASC_Message(ASC_MALLOCERROR), "ASC_initializeNetwork");
    }
    (*network)->role = role;
    (*network)->acceptorPort = acceptorPort;
    (*network)->network = netkey;

    return ASC_NORMAL;
}

CONDITION
ASC_dropNetwork(T_ASC_Network ** network)
{
    CONDITION cond;

    if (network == NULL) return ASC_NORMAL;
    if (*network == NULL) return ASC_NORMAL;

    cond = DUL_DropNetwork(&(*network)->network);
    if (cond != DUL_NORMAL) {
        return convertDULtoASCCondition(cond);
    }
    free(*network);
    *network = NULL;
    return ASC_NORMAL;
}


/*
 * Building Association parameters
 */

/* create association parameters and initialize with default values */
CONDITION 
ASC_createAssociationParameters(T_ASC_Parameters ** params,
        long maxReceivePDUSize)
{

    *params = (T_ASC_Parameters *) malloc(sizeof(**params));
    if (*params == NULL) {
        return COND_PushCondition(ASC_MALLOCERROR,
           ASC_Message(ASC_MALLOCERROR), "ASC_createAssociationParameters");
    }
    bzero((char*)*params, sizeof(**params));

    strncpy((*params)->ourImplementationClassUID,
            DIC_EURO_IMPLEMENTATIONCLASSUID,
            sizeof((*params)->ourImplementationClassUID)-1);
    strncpy((*params)->ourImplementationVersionName,
            DIC_EURO_IMPLEMENTATIONVERSIONNAME,
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
                    "resp AP Title");

    /* make sure max pdv length is even */
    if ((maxReceivePDUSize % 2) != 0) {
        CERR << "ASSOC: Warning: PDV receive length " << maxReceivePDUSize
            << " is odd (using " << (maxReceivePDUSize-1) << ")" << endl;
            maxReceivePDUSize--;
        }
    if (maxReceivePDUSize < ASC_MINIMUMPDUSIZE) {
        CERR << "ASC_createAssociationParameters: Warning: maxReceivePDUSize "
            << maxReceivePDUSize << " too small (using " << ASC_MINIMUMPDUSIZE << ")"
            << endl;
        maxReceivePDUSize = ASC_MINIMUMPDUSIZE;
    }

    (*params)->ourMaxPDUReceiveSize = maxReceivePDUSize;        
    (*params)->DULparams.maxPDU = maxReceivePDUSize; 
    (*params)->theirMaxPDUReceiveSize = 0;      /* not yet negotiated */

    /* set something unusable */
    ASC_setPresentationAddresses(*params,
                                 "calling Presentation Address",
                                 "called Presentation Address");

    /* presentation context lists will be created as needed */
    (*params)->DULparams.requestedPresentationContext = NULL;
    (*params)->DULparams.acceptedPresentationContext = NULL;

    (*params)->DULparams.useSecureLayer = OFFalse;
    return ASC_NORMAL;
}

static void
destroyPresentationContextList(LST_HEAD ** list)
{
    DUL_PRESENTATIONCONTEXT *pc;
    DUL_TRANSFERSYNTAX *ts;

    if ((list == NULL) || (*list == NULL))
        return;
    while ((pc = (DUL_PRESENTATIONCONTEXT*) LST_Dequeue(list)) != NULL) {
        if (pc->proposedTransferSyntax != NULL) {
            while ((ts = (DUL_TRANSFERSYNTAX*) LST_Dequeue(&pc->proposedTransferSyntax)) != NULL) {
                free(ts);
            }
            LST_Destroy(&pc->proposedTransferSyntax);
        }
        free(pc);
    }
    LST_Destroy(list);
}

CONDITION 
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

    free(*params);
    *params = NULL;

    return ASC_NORMAL;
}

CONDITION 
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

    return ASC_NORMAL;
}

CONDITION 
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

    return ASC_NORMAL;
}

CONDITION
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
    return ASC_NORMAL;
}

CONDITION 
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

    return ASC_NORMAL;
}

CONDITION 
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

    return ASC_NORMAL;
}

CONDITION 
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
    return ASC_NORMAL;
}

void 
ASC_printRejectParameters(FILE *f, T_ASC_RejectParameters *rej)
{
    fprintf(f, "Result: ");
    switch (rej->result) {
    case ASC_RESULT_REJECTEDPERMANENT:
        fprintf(f, "Rejected Permanent"); break;
    case ASC_RESULT_REJECTEDTRANSIENT:
        fprintf(f, "Rejected Transient"); break;
    default:
        fprintf(f, "UNKNOWN"); break;
    }
    fprintf(f, ", Source: ");
    switch (rej->source) {
    case ASC_SOURCE_SERVICEUSER:
        fprintf(f, "Service User"); break;
    case ASC_SOURCE_SERVICEPROVIDER_ACSE_RELATED:
        fprintf(f, "Service Provider (ACSE Related)"); break;
    case ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED:
        fprintf(f, "Service Provider (Presentation Related)"); break;
    default:
        fprintf(f, "UNKNOWN"); break;
    }
    fprintf(f, "\n");
    fprintf(f, "Reason: ");
    switch (rej->reason) {
    case ASC_REASON_SU_NOREASON:
    case ASC_REASON_SP_ACSE_NOREASON:
        fprintf(f, "No Reason"); break;
    case ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED:
        fprintf(f, "App Context Name Not Supported"); break;
    case ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED:
        fprintf(f, "Calling AE Title Not Recognized"); break;
    case ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED:
        fprintf(f, "Called AE Title Not Recognized"); break;
    case ASC_REASON_SP_ACSE_PROTOCOLVERSIONNOTSUPPORTED:
        fprintf(f, "Protocol Version Not Supported"); break;
        /* Service Provider Presentation Related reasons */
    case ASC_REASON_SP_PRES_TEMPORARYCONGESTION:
        fprintf(f, "Temporary Congestion"); break;
    case ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED:
        fprintf(f, "Local Limit Exceeded"); break;
    default:
        fprintf(f, "UNKNOWN"); break;
    }
    fprintf(f, "\n");
}

void 
ASC_printRejectParameters(ostream &out, T_ASC_RejectParameters *rej)
{
    out << "Result: ";
    switch (rej->result)
    {
      case ASC_RESULT_REJECTEDPERMANENT:
        out << "Rejected Permanent"; break;
      case ASC_RESULT_REJECTEDTRANSIENT:
        out << "Rejected Transient"; break;
      default:
        out << "UNKNOWN"; break;
    }
    out << ", Source: ";
    switch (rej->source)
    {
      case ASC_SOURCE_SERVICEUSER:
        out << "Service User"; break;
      case ASC_SOURCE_SERVICEPROVIDER_ACSE_RELATED:
        out << "Service Provider (ACSE Related)"; break;
      case ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED:
        out << "Service Provider (Presentation Related)"; break;
      default:
        out << "UNKNOWN"; break;
    }
    out << ", Reason: ";
    switch (rej->reason)
    {
    case ASC_REASON_SU_NOREASON:
    case ASC_REASON_SP_ACSE_NOREASON:
        out << "No Reason"; break;
    case ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED:
        out << "App Context Name Not Supported"; break;
    case ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED:
        out << "Calling AE Title Not Recognized"; break;
    case ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED:
        out << "Called AE Title Not Recognized"; break;
    case ASC_REASON_SP_ACSE_PROTOCOLVERSIONNOTSUPPORTED:
        out << "Protocol Version Not Supported"; break;
        /* Service Provider Presentation Related reasons */
    case ASC_REASON_SP_PRES_TEMPORARYCONGESTION:
        out << "Temporary Congestion"; break;
    case ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED:
        out << "Local Limit Exceeded"; break;
    default:
        out << "UNKNOWN"; break;
    }
    out << endl;
}

static T_ASC_SC_ROLE
dulRole2ascRole(DUL_SC_ROLE role)
{
    T_ASC_SC_ROLE ar = ASC_SC_ROLE_DEFAULT;
    switch (role) {
    case DUL_SC_ROLE_SCU: ar = ASC_SC_ROLE_SCU; break;
    case DUL_SC_ROLE_SCP: ar = ASC_SC_ROLE_SCP; break;
    case DUL_SC_ROLE_SCUSCP: ar = ASC_SC_ROLE_SCUSCP; break;
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

CONDITION 
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

    /* presentation context id's must be even */
    if ((presentationContextID % 2) == 0) {
        return COND_PushCondition(ASC_BADPRESENTATIONCONTEXTID,
                                  ASC_Message(ASC_BADPRESENTATIONCONTEXTID),
                                  presentationContextID);
    }
    /* see if a presentation context with this id already exists in list */
    pc = findPresentationContextID(
                             params->DULparams.requestedPresentationContext,
                                   presentationContextID);
    if (pc) {
        return COND_PushCondition(ASC_DUPLICATEPRESENTATIONCONTEXTID,
                            ASC_Message(ASC_DUPLICATEPRESENTATIONCONTEXTID),
                                  presentationContextID);
    }

    /* we cannot simply use DUL_MakePresentationCtx() because
    ** it takes variable arguments (for transfer syntax).
    */

    pc = (DUL_PRESENTATIONCONTEXT *) calloc(1, 
        sizeof(DUL_PRESENTATIONCONTEXT));
    if (pc == NULL) {
        return COND_PushCondition(ASC_MALLOCERROR,
                ASC_Message(ASC_MALLOCERROR), "ASC_addPresentationContext");
    }
    lst = LST_Create();
    if (lst == NULL) {
        return COND_PushCondition(ASC_MALLOCERROR,
                ASC_Message(ASC_MALLOCERROR), "ASC_addPresentationContext");
    }
    pc->presentationContextID = presentationContextID;
    strcpy(pc->abstractSyntax, abstractSyntax);
    pc->result = ASC_P_NOTYETNEGOTIATED;
    pc->proposedSCRole = ascRole2dulRole(proposedRole);
    pc->acceptedSCRole = ascRole2dulRole(ASC_SC_ROLE_DEFAULT);

    /* there must be at least one transfer syntax */
    if (transferSyntaxListCount < 1 ) {
        return COND_PushCondition(ASC_MISSINGTRANSFERSYNTAX,
                                  ASC_Message(ASC_MISSINGTRANSFERSYNTAX),
                                  "ASC_addPresentationContext");
    }

    /* add the transfer syntaxes */
    for (i=0; i<transferSyntaxListCount; i++) {
        transfer = (DUL_TRANSFERSYNTAX*)malloc(sizeof(DUL_TRANSFERSYNTAX));
        if (transfer == NULL) {
            return COND_PushCondition(ASC_MALLOCERROR,
                ASC_Message(ASC_MALLOCERROR), "ASC_addPresentationContext");
        }
        strcpy(transfer->transferSyntax, transferSyntaxList[i]);
        if (LST_Enqueue(&lst, (LST_NODE*)transfer) != LST_NORMAL) {
            return COND_PushCondition(ASC_CODINGERROR,
                ASC_Message(ASC_CODINGERROR), "ASC_addPresentationContext");
        }
    }
    pc->proposedTransferSyntax = lst;

    /* add to presentation context list */

    lst = params->DULparams.requestedPresentationContext;
    if (lst == NULL) {
        lst = LST_Create();
        if (lst == NULL) {
            return COND_PushCondition(ASC_MALLOCERROR,
                ASC_Message(ASC_MALLOCERROR), "ASC_addPresentationContext");
        }
    }
    if (LST_Enqueue(&lst, (LST_NODE*)pc) != LST_NORMAL) {
        return COND_PushCondition(ASC_CODINGERROR,
                ASC_Message(ASC_CODINGERROR), "ASC_addPresentationContext");
    }
    params->DULparams.requestedPresentationContext = lst;

    return ASC_NORMAL;
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

CONDITION 
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

    if (params->DULparams.requestedPresentationContext == NULL) {
        return COND_PushCondition(ASC_BADPRESENTATIONCONTEXTPOSITION,
                            ASC_Message(ASC_BADPRESENTATIONCONTEXTPOSITION),
                                  listPosition);
    }
    l = &(params->DULparams.requestedPresentationContext);
    if (*l == NULL) {
        return COND_PushCondition(ASC_BADPRESENTATIONCONTEXTPOSITION,
                            ASC_Message(ASC_BADPRESENTATIONCONTEXTPOSITION),
                                  listPosition);
    }
    pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)pc);

    while (pc && count != listPosition) {
        count++;
        pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
    }

    if (pc == NULL) {
        return COND_PushCondition(ASC_BADPRESENTATIONCONTEXTPOSITION,
                            ASC_Message(ASC_BADPRESENTATIONCONTEXTPOSITION),
                                  "ASC_getPresentationContext");
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
    while (transfer != NULL) {
        if (count >= DICOM_MAXTRANSFERSYNTAXES) {
            return COND_PushCondition(ASC_CODINGERROR,
                                      ASC_Message(ASC_CODINGERROR), "ASC_getPresentationContext: too many transfer syntaxes");
        }
        strcpy(presentationContext->proposedTransferSyntaxes[count],
               transfer->transferSyntax);
        count++;
        transfer = (DUL_TRANSFERSYNTAX*) LST_Next(l);
    }

    presentationContext->transferSyntaxCount = count;

    return ASC_NORMAL;
}

CONDITION
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
    CONDITION cond;
    LST_HEAD *lst;

    proposedContext = findPresentationContextID(
                             params->DULparams.requestedPresentationContext,
                                                presentationContextID);
    if (proposedContext == NULL) {
        return COND_PushCondition(ASC_BADPRESENTATIONCONTEXTID,
                                  ASC_Message(ASC_BADPRESENTATIONCONTEXTID),
                                  "ASC_acceptPresentationContext");
    }
    strcpy(proposedContext->acceptedTransferSyntax, transferSyntax);

    /* we want to mark this proposed context as beeing ok */
    proposedContext->result = ASC_P_ACCEPTANCE;
    proposedContext->acceptedSCRole = ascRole2dulRole(acceptedRole);

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
        if (cond != DUL_NORMAL) {
            return cond;
        }

        lst = params->DULparams.acceptedPresentationContext;
        if (lst == NULL) {
            lst = LST_Create();
            if (lst == NULL) {
                return COND_PushCondition(ASC_MALLOCERROR,
                                          ASC_Message(ASC_MALLOCERROR), 
                                          "ASC_acceptPresentationContext");
            }
        }
        if (LST_Enqueue(&lst, (LST_NODE*)acceptedContext) != LST_NORMAL) {
            return COND_PushCondition(ASC_CODINGERROR,
             ASC_Message(ASC_CODINGERROR), "ASC_acceptPresentationContext");
        }
        params->DULparams.acceptedPresentationContext = lst;
    }
    return ASC_NORMAL;
}


CONDITION
ASC_refusePresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    T_ASC_P_ResultReason resultReason)
 /*
  * The presentation context will be marked as refused. 
  */
{
    DUL_PRESENTATIONCONTEXT *proposedContext, *acceptedContext;
    CONDITION cond;
    LST_HEAD *lst;

    proposedContext = findPresentationContextID(
                             params->DULparams.requestedPresentationContext,
                                                presentationContextID);
    if (proposedContext == NULL) {
        return COND_PushCondition(ASC_BADPRESENTATIONCONTEXTID,
                                  ASC_Message(ASC_BADPRESENTATIONCONTEXTID),
                                  "ASC_acceptPresentationContext");
    }
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
        if (cond != DUL_NORMAL) {
            return cond;
        }

        lst = params->DULparams.acceptedPresentationContext;
        if (lst == NULL) {
            lst = LST_Create();
            if (lst == NULL) {
                return COND_PushCondition(ASC_MALLOCERROR,
                                          ASC_Message(ASC_MALLOCERROR), "ASC_acceptPresentationContext");
            }
        }
        if (LST_Enqueue(&lst, (LST_NODE*)acceptedContext) != LST_NORMAL) {
            return COND_PushCondition(ASC_CODINGERROR,
             ASC_Message(ASC_CODINGERROR), "ASC_acceptPresentationContext");
        }
        params->DULparams.acceptedPresentationContext = lst;
    }
    return ASC_NORMAL;
}


CONDITION
ASC_findAcceptedPresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    T_ASC_PresentationContext * presentationContext)
 /*
  * ASC_findAcceptedPresentationContext: You must supply the memory for
  * presentationContext, the values stored in the accepted presentation
  * context list with given ID will be copied into the memory structure.
  * Returns ASC_NORMAL if found, or ASC_BADPRESENTATIONCONTEXTID if not
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

    if ((pc == NULL) || (pc->result != ASC_P_ACCEPTANCE))
        return ASC_BADPRESENTATIONCONTEXTID;

    l = &pc->proposedTransferSyntax;
    transfer = (DUL_TRANSFERSYNTAX*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)transfer);
    while (transfer != NULL) {
        if (count >= DICOM_MAXTRANSFERSYNTAXES) {
            return COND_PushCondition(ASC_CODINGERROR,
                                      ASC_Message(ASC_CODINGERROR),
                                      "ASC_findAcceptedPresentationContext: too many transfer syntaxes");
        }
        OFStandard::strlcpy(presentationContext->proposedTransferSyntaxes[count], transfer->transferSyntax, DIC_UI_LEN);
        count++;
        transfer = (DUL_TRANSFERSYNTAX*) LST_Next(l);
    }

    OFStandard::strlcpy(presentationContext->abstractSyntax, pc->abstractSyntax, DIC_UI_LEN);
    presentationContext->presentationContextID = pc->presentationContextID;
    presentationContext->resultReason = (T_ASC_P_ResultReason) pc->result;
    presentationContext->proposedRole = dulRole2ascRole(pc->proposedSCRole);
    presentationContext->acceptedRole = dulRole2ascRole(pc->acceptedSCRole);

    presentationContext->transferSyntaxCount = count;
    OFStandard::strlcpy(presentationContext->acceptedTransferSyntax, pc->acceptedTransferSyntax, DUL_LEN_UID);

    return ASC_NORMAL;
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

    l = &assoc->params->DULparams.acceptedPresentationContext;
    pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)pc);
    while (pc && !found)
    {
        found = (strcmp(pc->abstractSyntax, abstractSyntax) == 0);
        found &= (pc->result == ASC_P_ACCEPTANCE);
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

    if ((transferSyntax==NULL)||(abstractSyntax==NULL)) return 0;

    /* first of all we look for a presentation context
     * matching both abstract and transfer syntax
     */
    l = &assoc->params->DULparams.acceptedPresentationContext;
    pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)pc);
    while (pc && !found)
    {
        found =  (strcmp(pc->abstractSyntax, abstractSyntax) == 0);
        found &= (pc->result == ASC_P_ACCEPTANCE);
        found &= (strcmp(pc->acceptedTransferSyntax, transferSyntax) == 0);
        if (!found) pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
    }
    if (found) return pc->presentationContextID;

    /* now we look for an explicit VR uncompressed PC. */
    l = &assoc->params->DULparams.acceptedPresentationContext;
    pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)pc);
    while (pc && !found)
    {
        found =  (strcmp(pc->abstractSyntax, abstractSyntax) == 0);
        found &= (pc->result == ASC_P_ACCEPTANCE);
        found &= ((strcmp(pc->acceptedTransferSyntax, UID_LittleEndianExplicitTransferSyntax) == 0) ||
                  (strcmp(pc->acceptedTransferSyntax, UID_BigEndianExplicitTransferSyntax) == 0));
        if (!found) pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
    }
    if (found) return pc->presentationContextID;

    /* now we look for an implicit VR uncompressed PC. */
    l = &assoc->params->DULparams.acceptedPresentationContext;
    pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)pc);
    while (pc && !found)
    {
        found =  (strcmp(pc->abstractSyntax, abstractSyntax) == 0);
        found &= (pc->result == ASC_P_ACCEPTANCE);
        found &= (strcmp(pc->acceptedTransferSyntax, UID_LittleEndianImplicitTransferSyntax) == 0);
        if (!found) pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
    }
    if (found) return pc->presentationContextID;

    /* finally we accept everything we get. */
    l = &assoc->params->DULparams.acceptedPresentationContext;
    pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
    (void)LST_Position(l, (LST_NODE*)pc);
    while (pc && !found)
    {
        found =  (strcmp(pc->abstractSyntax, abstractSyntax) == 0);
        found &= (pc->result == ASC_P_ACCEPTANCE);
        if (!found) pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
    }
    if (found) return pc->presentationContextID;
 
    return 0;   /* otherwise */
}


CONDITION
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
    CONDITION cond;
    int n, i, j, k;
    DUL_PRESENTATIONCONTEXT *dpc;
    T_ASC_PresentationContext pc;
    OFBool accepted = OFFalse;
    OFBool abstractOK = OFFalse;

    n = ASC_countPresentationContexts(params);
    for (i = 0; i < n; i++) {
        cond = ASC_getPresentationContext(params, i, &pc);
        if (cond != ASC_NORMAL) return cond;
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
            if (cond != ASC_NORMAL) return cond;

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
                if (cond != ASC_NORMAL) return cond;
            }
        }

    }
    return ASC_NORMAL;
}

CONDITION
ASC_acceptContextsWithPreferredTransferSyntaxes(
    T_ASC_Parameters * params,
    const char* abstractSyntaxes[], int abstractSyntaxCount,
    const char* transferSyntaxes[], int transferSyntaxCount,
    T_ASC_SC_ROLE acceptedRole)
{
    int i;
    CONDITION cond = ASC_NORMAL;
    /* 
    ** Accept in the order "least wanted" to "most wanted" transfer
    ** syntax.  Accepting a transfer syntax will override previously
    ** accepted transfer syntaxes.
    */
    for (i=transferSyntaxCount-1; i>=0; i--) {
        cond = ASC_acceptContextsWithTransferSyntax(
            params, transferSyntaxes[i],
            abstractSyntaxCount, abstractSyntaxes, acceptedRole);
        if (!SUCCESS(cond)) {
            return cond;
        }
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

void 
ASC_dumpParameters(T_ASC_Parameters * params, ostream& outstream)
 /*
  * Write parameters in textual form to stdout (debugging aid) 
  */
{
    int i;
    T_ASC_PresentationContext pc;
        
    outstream << "Our Implementation Class UID:    "
        << params->ourImplementationClassUID << endl
        << "Our Implementation Version Name: "
        << params->ourImplementationVersionName << endl
        << "Their Implementation Class UID:    "
        << params->theirImplementationClassUID << endl
        << "Their Implementation Version Name: "
        << params->theirImplementationVersionName << endl
        << "Application Context Name:    "
        << params->DULparams.applicationContextName << endl
        << "Calling Application Name:    "
        << params->DULparams.callingAPTitle << endl
        << "Called Application Name:     "
        << params->DULparams.calledAPTitle << endl
        << "Responding Application Name: "
        << params->DULparams.respondingAPTitle << endl
        << "Our Max PDU Receive Size: "
        << params->ourMaxPDUReceiveSize << endl
        << "Their Max PDU Receive Size: "
        << params->theirMaxPDUReceiveSize << endl;

    outstream << "Presentation Contexts:" << endl;
    for (i=0; i<ASC_countPresentationContexts(params); i++) {
        ASC_getPresentationContext(params, i, &pc);
        ASC_dumpPresentationContext(&pc, outstream);
    }

    SOPClassExtendedNegotiationSubItemList* extNegList=NULL;
    ASC_getRequestedExtNegList(params, &extNegList);
    outstream << "Requested Extended Negotiation:";
    if (extNegList != NULL) {
        outstream << endl;
        dumpExtNegList(*extNegList);
    } else {
        outstream << " none" << endl;
    }
    ASC_getAcceptedExtNegList(params, &extNegList);
    outstream << "Accepted Extended Negotiation:";
    if (extNegList != NULL) {
        outstream << endl;
        dumpExtNegList(*extNegList);
    } else {
        outstream << " none" << endl;
    }

#if 0
    outstream << "DUL Params --- BEGIN" << endl;
    DUL_DumpParams(&params->DULparams);
    outstream << "DUL Params --- END" << endl;
#endif
}

void
ASC_dumpPresentationContext(T_ASC_PresentationContext * p, ostream& outstream)
 /*
  * Write presentation context structure in textual form to stdout.
  * (debugging aid) 
  */
{
    int i = 0;

    outstream << "  Context ID:        " << (int)p->presentationContextID << " ";
    switch (p->resultReason) {
    case ASC_P_ACCEPTANCE:
        outstream << "(Accepted)" << endl;
        break;
    case ASC_P_USERREJECTION:
        outstream << "(User Rejection)" << endl;
        break;
    case ASC_P_NOREASON:
        outstream << "(No Reason)" << endl;
        break;
    case ASC_P_ABSTRACTSYNTAXNOTSUPPORTED:
        outstream << "(Abstract Syntax Not Supported)" << endl;
        break;
    case ASC_P_TRANSFERSYNTAXESNOTSUPPORTED:
        outstream << "(Transfer Syntaxes Not Supported)" << endl;
        break;
    case ASC_P_NOTYETNEGOTIATED:
        outstream << "(Proposed)" << endl;
        break;
    default:
        outstream << "(--Invalid Result/Reason--)" << endl;
    }

    const char* l_as = dcmFindNameOfUID(p->abstractSyntax);
    if (l_as) {
        outstream << "    Abstract Syntax: =" << l_as  << endl;
    } else {
        outstream << "    Abstract Syntax: " <<  p->abstractSyntax << endl;
    }
    
    outstream << "    Proposed SCP/SCU Role: "
        << ascRole2String(p->proposedRole) << endl
        << "    Accepted SCP/SCU Role: "
        << ascRole2String(p->acceptedRole) << endl;

    if (p->resultReason == ASC_P_ACCEPTANCE) {
        const char* ts = dcmFindNameOfUID(p->acceptedTransferSyntax);
        if (ts) {
            outstream << "    Accepted Transfer Syntax: =" << ts << endl;
        } else {
            outstream << "    Accepted Transfer Syntax: "
                << p->acceptedTransferSyntax << endl;
        }
    }

    if (p->resultReason == ASC_P_NOTYETNEGOTIATED) {
        outstream << "    Proposed Transfer Syntax(es):" << endl;
        for (i = 0; i < (int)p->transferSyntaxCount; i++) {
            const char* ts = dcmFindNameOfUID(p->proposedTransferSyntaxes[i]);
            if (ts) {
                outstream << "      =" << ts << endl;
            } else {
                outstream << "      " << p->proposedTransferSyntaxes[i] << endl;
            }
        }
    }
}

void
ASC_dumpConnectionParameters(T_ASC_Association *association, ostream& outstream)
{
  if (association==NULL) return;
  DUL_DumpConnectionParameters(association->DULassociation, outstream);
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
    FD_SET(s, &fdset);
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

CONDITION 
ASC_destroyAssociation(T_ASC_Association ** association)
{
    CONDITION cond;

    /* don't worry if already destroyed */
    if (association == NULL) return ASC_NORMAL;
    if (*association == NULL) return ASC_NORMAL;

    if ((*association)->DULassociation != NULL) {
        ASC_dropAssociation(*association);
    }

    if ((*association)->params != NULL) {
        cond = ASC_destroyAssociationParameters(&(*association)->params);
        if (cond != ASC_NORMAL) {
            return cond;
        }
    }

    if ((*association)->sendPDVBuffer != NULL)
        free((*association)->sendPDVBuffer);

    free(*association);
    *association = NULL;

    return ASC_NORMAL;
}

CONDITION
ASC_receiveAssociation(T_ASC_Network * network,
                       T_ASC_Association ** assoc,
                       long maxReceivePDUSize,
                       void **associatePDU,
                       unsigned long *associatePDUlength,
                       OFBool useSecureLayer)
{
    CONDITION cond;
    T_ASC_Parameters *params;
    DUL_ASSOCIATIONKEY *DULassociation;
    DUL_PRESENTATIONCONTEXT *pc;
    LST_HEAD **l;

    int retrieveRawPDU = 0;
    if (associatePDU && associatePDUlength) retrieveRawPDU = 1;

    cond = ASC_createAssociationParameters(&params, maxReceivePDUSize);
    if (cond != ASC_NORMAL) return cond;
    
    cond = ASC_setTransportLayerType(params, useSecureLayer);
    if (cond != ASC_NORMAL) return cond;

    *assoc = (T_ASC_Association *) malloc(sizeof(**assoc));
    if (*assoc == NULL) {
        return COND_PushCondition(ASC_MALLOCERROR,
                    ASC_Message(ASC_MALLOCERROR), "ASC_receiveAssociation");
    }
    bzero((char*)*assoc, sizeof(**assoc));

    (*assoc)->params = params;
    (*assoc)->nextMsgID = 1;

    cond = DUL_ReceiveAssociationRQ(&network->network, DUL_BLOCK,
                                    &(params->DULparams), &DULassociation, retrieveRawPDU);

    (*assoc)->DULassociation = DULassociation;

    if (retrieveRawPDU && DULassociation)
    {
      DUL_returnAssociatePDUStorage((*assoc)->DULassociation, *associatePDU, *associatePDUlength);
    }  

    if (cond != DUL_NORMAL)
    {
        return convertDULtoASCCondition(cond);
    }
    
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

    return ASC_NORMAL;
}

static void
updateRequestedPCFromAcceptedPC(
    DUL_PRESENTATIONCONTEXT *apc,
    DUL_PRESENTATIONCONTEXT *rpc)
{
    if (apc == NULL || rpc == NULL) return;

    if (rpc->presentationContextID != apc->presentationContextID) {
        COND_PushCondition(
            ASC_CODINGERROR, ASC_Message(ASC_CODINGERROR), 
            "ASC::updateRequestedPCFromAcceptedPC: presentationContextIDs differ ");
        return;
    }

    rpc->result = apc->result;
    if (apc->result == ASC_P_ACCEPTANCE) {
        strcpy(rpc->acceptedTransferSyntax, apc->acceptedTransferSyntax);
    } else {
        rpc->acceptedTransferSyntax[0] = '\0';
    }
    rpc->acceptedSCRole = apc->acceptedSCRole;
}
    

static void
updateRequestedPCListFromAcceptedPCList(
    DUL_ASSOCIATESERVICEPARAMETERS *dulParams)
{
    DUL_PRESENTATIONCONTEXT *requestedPc = NULL;
    DUL_PRESENTATIONCONTEXT *acceptedPc = NULL;
    LST_HEAD **acceptedList = NULL;

    acceptedList = &dulParams->acceptedPresentationContext;
    if (*acceptedList != NULL) {
        acceptedPc =(DUL_PRESENTATIONCONTEXT*)  LST_Head(acceptedList);
        if (acceptedPc != NULL)
            (void)LST_Position(acceptedList, (LST_NODE*)acceptedPc);

        while (acceptedPc) {
            requestedPc = findPresentationContextID(
                dulParams->requestedPresentationContext,
                acceptedPc->presentationContextID);
            updateRequestedPCFromAcceptedPC(acceptedPc, requestedPc);
            acceptedPc = (DUL_PRESENTATIONCONTEXT*) LST_Next(acceptedList);
            if (acceptedPc != NULL)
                (void)LST_Position(acceptedList, (LST_NODE*)acceptedPc);
        }
    }

}

CONDITION
ASC_requestAssociation(T_ASC_Network * network,
                       T_ASC_Parameters * params,
                       T_ASC_Association ** assoc,
                       void **associatePDU,
                       unsigned long *associatePDUlength)
{
    CONDITION cond;
    long sendLen;
    int retrieveRawPDU = 0;
    if (associatePDU && associatePDUlength) retrieveRawPDU = 1;
    
    if (network == NULL) {
        return COND_PushCondition(
            ASC_NULLKEY, ASC_Message(ASC_NULLKEY), 
            "ASC_requestAssociation: null network");
    }

    if (params == NULL) {
        return COND_PushCondition(
            ASC_NULLKEY, ASC_Message(ASC_NULLKEY), 
            "ASC_requestAssociation: null params");
    }

    if (ASC_countPresentationContexts(params) == 0) {
        return COND_PushCondition(
            ASC_CODINGERROR, ASC_Message(ASC_CODINGERROR), 
            "ASC_requestAssociation: missing presentation contexts");
    }

    *assoc = (T_ASC_Association *) malloc(sizeof(**assoc));
    if (*assoc == NULL) {
        return COND_PushCondition(ASC_MALLOCERROR,
                    ASC_Message(ASC_MALLOCERROR), "ASC_requestAssociation");
    }
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

    if (cond == DUL_NORMAL)
    {

       /*
        * The params->DULparams.peerMaxPDU parameter contains the 
        * max-pdu-length value in the a-associate-ac (i.e. the max-pdu-length 
        * that the remote AE is prepared to accept).
        */
        params->theirMaxPDUReceiveSize = params->DULparams.peerMaxPDU;

        /* create a sendPDVBuffer */
        sendLen = params->theirMaxPDUReceiveSize;
        if (sendLen < 1) {
            /* the length is unlimited, choose a suitable buffer len */
            sendLen = ASC_MAXIMUMPDUSIZE;
        } else if (sendLen > ASC_MAXIMUMPDUSIZE) {
            sendLen = ASC_MAXIMUMPDUSIZE;
        }
        /* make sure max pdv length is even */
        if ((sendLen % 2) != 0) {
            CERR << "ASSOC: Warning: PDV send length " << sendLen
                << " is odd (using " << (sendLen-1) << ")" << endl;
            sendLen--;
        }
        /* length is minus PDU and PDV header bytes */
        sendLen -= 12;
        if (sendLen < 1) {
            CERR << "ASSOC: Warning: PDV send length " << sendLen << " (using default)"
                << endl;
            sendLen = ASC_MINIMUMPDUSIZE - 12;
        }
        if (sendLen < 12) {
            /* if sendLen is < 12, dcmdata will fail because it needs to put
             * up to 12 bytes into the send PDV buffer at once (tag header).
             * We use a larger value on this level and let the Upper Layer FSM
             * split the buffer for us into many small PDVs.
             */
            CERR << "ASSOC: Warning: PDV send length too small, using DUL to split larger PDVs." << endl;
            sendLen = ASC_MINIMUMPDUSIZE - 12;
        }
        (*assoc)->sendPDVLength = sendLen;
        (*assoc)->sendPDVBuffer = (unsigned char*)malloc(size_t(sendLen));
        if ((*assoc)->sendPDVBuffer == NULL) {
            return COND_PushCondition(ASC_MALLOCERROR,
                    ASC_Message(ASC_MALLOCERROR), "ASC_requestAssociation");
        }
        strcpy(params->theirImplementationClassUID,
           params->DULparams.calledImplementationClassUID);
        strcpy(params->theirImplementationVersionName,
           params->DULparams.calledImplementationVersionName);

        /* make sure accepted PCs are marked as such in the requsted PC list */
        updateRequestedPCListFromAcceptedPCList(&params->DULparams);
    }
    return convertDULtoASCCondition(cond);
}

CONDITION
ASC_acknowledgeAssociation(
    T_ASC_Association * assoc,
    void **associatePDU,
    unsigned long *associatePDUlength)
{
    CONDITION cond;
    long sendLen;

    if (assoc == NULL) return ASC_NULLKEY;
    if (assoc->DULassociation == NULL) return ASC_NULLKEY;

    int retrieveRawPDU = 0;
    if (associatePDU && associatePDUlength) retrieveRawPDU = 1;

    assoc->params->DULparams.maxPDU = assoc->params->ourMaxPDUReceiveSize;
    strcpy(assoc->params->DULparams.calledImplementationClassUID,
        assoc->params->ourImplementationClassUID);
    strcpy(assoc->params->DULparams.calledImplementationVersionName,
        assoc->params->ourImplementationVersionName);


    cond = DUL_AcknowledgeAssociationRQ(&assoc->DULassociation,
                                        &assoc->params->DULparams, 
                                        retrieveRawPDU);

    if (retrieveRawPDU && (assoc->DULassociation))
    {
      DUL_returnAssociatePDUStorage(assoc->DULassociation, *associatePDU, *associatePDUlength);
    }

    if (cond == DUL_NORMAL) {
        /* create a sendPDVBuffer */
        sendLen = assoc->params->theirMaxPDUReceiveSize;
        if (sendLen < 1) {
            /* the length is unlimited, choose a suitable buffer len */
            sendLen = ASC_MAXIMUMPDUSIZE;
        } else if (sendLen > ASC_MAXIMUMPDUSIZE) {
            sendLen = ASC_MAXIMUMPDUSIZE;
        }
        /* make sure max pdv length is even */
        if ((sendLen % 2) != 0) {
           CERR << "ASSOC: Warning: PDV send length " << sendLen
                << " is odd (using " << (sendLen-1) << ")" << endl;
            sendLen--;
        }
        /* length is minus PDU and PDV header bytes */
        sendLen -= 12;
        if (sendLen < 1) {
            CERR << "ASSOC: Warning: PDV send length " << sendLen
                << " (using default)" << endl;
            sendLen = ASC_MINIMUMPDUSIZE - 12;
        }
        if (sendLen < 12) {
            /* if sendLen is < 12, dcmdata will fail because it needs to put
             * up to 12 bytes into the send PDV buffer at once (tag header).
             * We use a larger value on this level and let the Upper Layer FSM
             * split the buffer for us into many small PDVs.
             */
            CERR << "ASSOC: Warning: PDV send length too small, using DUL to split larger PDVs." << endl;
            sendLen = ASC_MINIMUMPDUSIZE - 12;
        }
        assoc->sendPDVLength = sendLen;
        assoc->sendPDVBuffer = (unsigned char*)malloc(size_t(sendLen));
        if (assoc->sendPDVBuffer == NULL) {
            return COND_PushCondition(ASC_MALLOCERROR,
                    ASC_Message(ASC_MALLOCERROR), "ASC_requestAssociation");
        }
    }
    return convertDULtoASCCondition(cond);
}

CONDITION
ASC_rejectAssociation(
    T_ASC_Association * association,
    T_ASC_RejectParameters * rejectParameters,
    void **associatePDU,
    unsigned long *associatePDUlength)
{
    CONDITION cond;
    DUL_ABORTITEMS l_abort;

    if (association == NULL) return ASC_NULLKEY;
    if (association->DULassociation == NULL) return ASC_NULLKEY;
    if (rejectParameters == NULL) return ASC_NULLKEY;

    int retrieveRawPDU = 0;
    if (associatePDU && associatePDUlength) retrieveRawPDU = 1;

    l_abort.result = (unsigned char)(rejectParameters->result & 0xff);
    l_abort.source = (unsigned char)(rejectParameters->source & 0xff);
    l_abort.reason = (unsigned char)(rejectParameters->reason & 0xff);

    cond = DUL_RejectAssociationRQ(
        &association->DULassociation,
        &l_abort,
        retrieveRawPDU);

    if (retrieveRawPDU && (association->DULassociation))
    {
      DUL_returnAssociatePDUStorage(association->DULassociation, *associatePDU, *associatePDUlength);
    }

    if (cond != DUL_NORMAL)
        return convertDULtoASCCondition(cond);

    return ASC_NORMAL;
}

CONDITION
ASC_releaseAssociation(T_ASC_Association * association)
{
    CONDITION cond;

    if (association == NULL) return ASC_NULLKEY;
    if (association->DULassociation == NULL) return ASC_NULLKEY;

    cond = DUL_ReleaseAssociation(&association->DULassociation);
    if (cond != DUL_RELEASECONFIRMED)
        return convertDULtoASCCondition(cond);

    return ASC_NORMAL;
}

CONDITION ASC_acknowledgeRelease(T_ASC_Association *association)
{
    CONDITION cond;

    if (association == NULL) return ASC_NULLKEY;
    if (association->DULassociation == NULL) return ASC_NULLKEY;

    cond = DUL_AcknowledgeRelease(&association->DULassociation);

    if (cond != DUL_NORMAL)
        return convertDULtoASCCondition(cond);

    return ASC_NORMAL;
}


CONDITION
ASC_abortAssociation(T_ASC_Association * association)
{
    CONDITION cond;

    if (association == NULL) return ASC_NULLKEY;
    if (association->DULassociation == NULL) return ASC_NULLKEY;

    cond = DUL_AbortAssociation(&association->DULassociation);

    if (cond != DUL_NORMAL)
        return convertDULtoASCCondition(cond);

    return ASC_NORMAL;
}


CONDITION
ASC_dropSCPAssociation(T_ASC_Association * association)
{
    CONDITION cond;

    /* if already dead don't worry */
    if (association == NULL) return ASC_NORMAL;
    if (association->DULassociation == NULL) return ASC_NORMAL;

        ASC_dataWaiting(association, DUL_TIMEOUT);
    cond = DUL_DropAssociation(&association->DULassociation);

    if (cond != DUL_NORMAL)
        return convertDULtoASCCondition(cond);

    return ASC_NORMAL;
}



CONDITION
ASC_dropAssociation(T_ASC_Association * association)
{
    CONDITION cond;

    /* if already dead don't worry */
    if (association == NULL) return ASC_NORMAL;
    if (association->DULassociation == NULL) return ASC_NORMAL;

    cond = DUL_DropAssociation(&association->DULassociation);

    if (cond != DUL_NORMAL)
        return convertDULtoASCCondition(cond);

    return ASC_NORMAL;
}



CONDITION
ASC_setTransportLayerType(
    T_ASC_Parameters * params,
    OFBool useSecureLayer)
{
  if (params == NULL) return COND_PushCondition(
    ASC_NULLKEY, ASC_Message(ASC_NULLKEY), 
    "ASC_setTransportLayerType: null params");

  params->DULparams.useSecureLayer = useSecureLayer;
  return ASC_NORMAL;
}

CONDITION
ASC_setTransportLayer(T_ASC_Network *network, DcmTransportLayer *newLayer, int takeoverOwnership)
{
  if (network == NULL) return COND_PushCondition(
    ASC_NULLKEY, ASC_Message(ASC_NULLKEY), 
    "ASC_setTransportLayer: null params");
   
  CONDITION cond = DUL_setTransportLayer(network->network, newLayer, takeoverOwnership);

  if (cond != DUL_NORMAL) return convertDULtoASCCondition(cond);
  return ASC_NORMAL;
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


/*
** CVS Log
** $Log: assoc.cc,v $
** Revision 1.33  2000-12-19 12:06:19  meichel
** Renamed typedef T_UID which leads to name clash with system macro on OSF/1
**
** Revision 1.32  2000/10/10 12:06:07  meichel
** Added version of function ASC_printRejectParameters that takes
**   an ostream& instead of a FILE*
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
