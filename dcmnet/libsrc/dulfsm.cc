/*
 *
 *  Copyright (C) 1994-2018, OFFIS e.V.
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
          Copyright (C) 1993, 1994, RSNA and Washington University

          The software and supporting documentation for the Radiological
          Society of North America (RSNA) 1993, 1994 Digital Imaging and
          Communications in Medicine (DICOM) Demonstration were developed
          at the
                  Electronic Radiology Laboratory
                  Mallinckrodt Institute of Radiology
                  Washington University School of Medicine
                  510 S. Kingshighway Blvd.
                  St. Louis, MO 63110
          as part of the 1993, 1994 DICOM Central Test Node project for, and
          under contract with, the Radiological Society of North America.

          THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER RSNA NOR
          WASHINGTON UNIVERSITY MAKE ANY WARRANTY ABOUT THE SOFTWARE, ITS
          PERFORMANCE, ITS MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
          USE, FREEDOM FROM ANY COMPUTER DISEASES OR ITS CONFORMITY TO ANY
          SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF
          THE SOFTWARE IS WITH THE USER.

          Copyright of the software and supporting documentation is
          jointly owned by RSNA and Washington University, and free access
          is hereby granted as a license to use this software, copy this
          software and prepare derivative works based upon this software.
          However, any distribution of this software source code or
          supporting documentation or derivative works (source code and
          supporting documentation) must include the three paragraphs of
          the copyright notice.
*/

/*
**          DICOM 93
**        Electronic Radiology Laboratory
**      Mallinckrodt Institute of Radiology
**    Washington University School of Medicine
**
** Module Name(s):  DUL_InitializeFSM
**      PRV_StateMachine
**      fsmDebug
**
** Author, Date:  Stephen M. Moore, 15-Apr-93
** Intent:        Define tables and provide functions that implement
**                the DICOM Upper Layer (DUL) finite state machine.
*/


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_WINDOWS_H
// on Windows, we need Winsock2 for network functions
#include <winsock2.h>
// and ws2tcpip for socklen_t
#include <ws2tcpip.h>
#endif

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CERRNO
#define INCLUDE_CSIGNAL
#define INCLUDE_CTIME
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#endif

BEGIN_EXTERN_C
#ifdef HAVE_NETINET_IN_SYSTM_H
#include <netinet/in_systm.h>   /* prerequisite for netinet/in.h on NeXT */
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>         /* prerequisite for netinet/tcp.h on NeXT */
#endif
#ifdef HAVE_NETINET_TCP_H
#include <netinet/tcp.h>        /* for TCP_NODELAY */
#endif
END_EXTERN_C
#ifdef DCMTK_HAVE_POLL
#include <poll.h>
#endif

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/lst.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmnet/dul.h"
#include "dulstruc.h"
#include "dulpriv.h"
#include "dulfsm.h"
#include "dcmtk/ofstd/ofbmanip.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/dcmnet/assoc.h"    /* for ASC_MAXIMUMPDUSIZE */
#include "dcmtk/dcmnet/dcmtrans.h"
#include "dcmtk/dcmnet/dcmlayer.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/ofstd/ofsockad.h" /* for class OFSockAddr */

/* At least Solaris doesn't define this */
#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

/* platform independent definition of EINTR */
enum
{
#ifdef HAVE_WINSOCK_H
    DCMNET_EINTR = WSAEINTR
#else
    DCMNET_EINTR = EINTR
#endif
};

static OFCondition
AE_1_TransportConnect(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AE_2_SendAssociateRQPDU(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AE_3_AssociateConfirmationAccept(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AE_4_AssociateConfirmationReject(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AE_5_TransportConnectResponse(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AE_6_ExamineAssociateRequest(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AE_7_SendAssociateAC(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AE_8_SendAssociateRJ(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);

static OFCondition
DT_1_SendPData(PRIVATE_NETWORKKEY ** network,
         PRIVATE_ASSOCIATIONKEY ** associatin, int nextState, void *params);
static OFCondition
DT_2_IndicatePData(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);

static OFCondition
AA_1_SendAAbort(PRIVATE_NETWORKKEY ** network,
         PRIVATE_ASSOCIATIONKEY ** associatin, int nextState, void *params);
static OFCondition
AA_2_CloseTransport(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AA_2_CloseTimeout(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AA_3_IndicatePeerAborted(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AA_4_IndicateAPAbort(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AA_5_StopARTIMtimer(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AA_6_IgnorePDU(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AA_7_State13SendAbort(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AA_8_UnrecognizedPDUSendAbort(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);

static OFCondition
AR_1_SendReleaseRQ(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AR_2_IndicateRelease(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AR_3_ConfirmRelease(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AR_4_SendReleaseRP(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AR_5_StopARTIMtimer(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AR_6_IndicatePData(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AR_7_SendPDATA(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AR_8_IndicateARelease(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AR_9_SendAReleaseRP(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static OFCondition
AR_10_ConfirmRelease(PRIVATE_NETWORKKEY ** network,
        PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);

static OFCondition
requestAssociationTCP(PRIVATE_NETWORKKEY ** network,
                      DUL_ASSOCIATESERVICEPARAMETERS * params,
                      PRIVATE_ASSOCIATIONKEY ** association);
static OFCondition
sendAssociationRQTCP(PRIVATE_NETWORKKEY ** network,
                     DUL_ASSOCIATESERVICEPARAMETERS * params,
                     PRIVATE_ASSOCIATIONKEY ** association);
static OFCondition
sendAssociationACTCP(PRIVATE_NETWORKKEY ** network,
                     DUL_ASSOCIATESERVICEPARAMETERS * params,
                     PRIVATE_ASSOCIATIONKEY ** association);
static OFCondition
sendAssociationRJTCP(PRIVATE_NETWORKKEY ** network,
        DUL_ABORTITEMS * abortItems, PRIVATE_ASSOCIATIONKEY ** association);
static OFCondition
sendAbortTCP(DUL_ABORTITEMS * abortItems,
             PRIVATE_ASSOCIATIONKEY ** association);
static OFCondition sendReleaseRQTCP(PRIVATE_ASSOCIATIONKEY ** association);
static OFCondition sendReleaseRPTCP(PRIVATE_ASSOCIATIONKEY ** association);
static OFCondition
sendPDataTCP(PRIVATE_ASSOCIATIONKEY ** association,
             DUL_PDVLIST * pdvList);
static OFCondition
writeDataPDU(PRIVATE_ASSOCIATIONKEY ** association,
             DUL_DATAPDU * pdu);
static void clearPDUCache(PRIVATE_ASSOCIATIONKEY ** association);
static void closeTransport(PRIVATE_ASSOCIATIONKEY ** association);
static void closeTransportTCP(PRIVATE_ASSOCIATIONKEY ** association);
static OFCondition
readPDUHead(PRIVATE_ASSOCIATIONKEY ** association,
            unsigned char *buffer, unsigned long maxlength,
            DUL_BLOCKOPTIONS block, int timeout,
            unsigned char *PDUtype, unsigned char *PDUreserved,
            unsigned long *PDULength);
static OFCondition
readPDU(PRIVATE_ASSOCIATIONKEY ** association, DUL_BLOCKOPTIONS block,
        int timeout, unsigned char **buffer,
        unsigned char *pduType, unsigned char *pduReserved,
        unsigned long *pduLength);
static OFCondition
readPDUBody(PRIVATE_ASSOCIATIONKEY ** association,
            DUL_BLOCKOPTIONS block, int timeout,
            unsigned char *buffer, unsigned long maxLength,
            unsigned char *pduType, unsigned char *pduReserved,
            unsigned long *pduLength);
static OFCondition
readPDUHeadTCP(PRIVATE_ASSOCIATIONKEY ** association,
               unsigned char *buffer, unsigned long maxLength,
               DUL_BLOCKOPTIONS block, int timeout,
               unsigned char *PDUtype, unsigned char *PDUreserved,
               unsigned long *PDULength);
static OFCondition
readPDUBodyTCP(PRIVATE_ASSOCIATIONKEY ** association,
               DUL_BLOCKOPTIONS block, int timeout,
               unsigned char *buffer, unsigned long maxLength,
               unsigned char *pduType, unsigned char *pduReserved,
               unsigned long *pduLength);
static OFCondition
defragmentTCP(DcmTransportConnection *connection, DUL_BLOCKOPTIONS block, time_t timerStart,
              int timeout, void *b, unsigned long l, unsigned long *rtnLen);

static OFString dump_pdu(const char *type, void *buffer, unsigned long length);

#ifdef _WIN32
static void setTCPBufferLength(SOCKET sock);
#else
static void setTCPBufferLength(int sock);
#endif

OFCondition
translatePresentationContextList(LST_HEAD ** internalList,
                                 LST_HEAD ** SCUSCPRoleList,
                                 LST_HEAD ** userContextList);
DUL_PRESENTATIONCONTEXT *
findPresentationCtx(LST_HEAD ** lst, DUL_PRESENTATIONCONTEXTID contextID);

PRV_SCUSCPROLE *
findSCUSCPRole(LST_HEAD ** lst, char *abstractSyntax);

void destroyPresentationContextList(LST_HEAD ** l);
void destroyUserInformationLists(DUL_USERINFO * userInfo);

static FSM_Event_Description Event_Table[] = {
    {A_ASSOCIATE_REQ_LOCAL_USER, "A-ASSOCIATE request (local user)"},
    {TRANS_CONN_CONFIRM_LOCAL_USER, "Transport conn confirmation (local)"},
    {A_ASSOCIATE_AC_PDU_RCV, "A-ASSOCIATE-AC PDU (on transport)"},
    {A_ASSOCIATE_RJ_PDU_RCV, "A-ASSOCIATE-RJ PDU (on transport)"},
    {TRANS_CONN_INDICATION, "Transport connection indication"},
    {A_ASSOCIATE_RQ_PDU_RCV, "A-ASSOCIATE-RQ PDU (on transport)"},
    {A_ASSOCIATE_RESPONSE_ACCEPT, "A-ASSOCIATE resp prim (accept)"},
    {A_ASSOCIATE_RESPONSE_REJECT, "A-ASSOCIATE resp prim (reject)"},
    {P_DATA_REQ, "P-DATA request primitive"},
    {P_DATA_TF_PDU_RCV, "P-DATA-TF PDU (on transport)"},
    {A_RELEASE_REQ, "A-RELEASE request primitive"},
    {A_RELEASE_RQ_PDU_RCV, "A-RELEASE-RQ PDU (on transport)"},
    {A_RELEASE_RP_PDU_RCV, "A-RELEASE-RP PDU (on transport)"},
    {A_RELEASE_RESP, "A-RELEASE response primitive"},
    {A_ABORT_REQ, "A-ABORT request primitive"},
    {A_ABORT_PDU_RCV, "A-ABORT PDU (on transport)"},
    {TRANS_CONN_CLOSED, "Transport connection closed"},
    {ARTIM_TIMER_EXPIRED, "ARTIM timer expired (rej/rel)"},
    {INVALID_PDU, "Unrecognized/invalid PDU"}
};

static FSM_FUNCTION FSM_FunctionTable[] = {
    {AE_1, AE_1_TransportConnect, "AE 1 Transport Connect"},
    {AE_2, AE_2_SendAssociateRQPDU, "AE 2 Send Associate RQ PDU"},
    {AE_3, AE_3_AssociateConfirmationAccept, "AE 3 Associate Confirmation Accept"},
    {AE_4, AE_4_AssociateConfirmationReject, "AE 4 Associate Confirmation Reject"},
    {AE_5, AE_5_TransportConnectResponse, "AE 5 Transport Connect Response"},
    {AE_6, AE_6_ExamineAssociateRequest, "AE 6 Examine Associate Request"},
    {AE_7, AE_7_SendAssociateAC, "AE 7 Send Associate AC"},
    {AE_8, AE_8_SendAssociateRJ, "AE 8 Send Associate RJ"},

    {DT_1, DT_1_SendPData, "DT 1 Send P DATA PDU"},
    {DT_2, DT_2_IndicatePData, "DT 2 Indicate P DATA PDU Received"},

    {AA_1, AA_1_SendAAbort, "AA 1 Send A ABORT PDU"},
    {AA_2, AA_2_CloseTransport, "AA 2 Close Transport"},
    {AA_2T, AA_2_CloseTimeout, "AA 2 Close Transport (Read Timeout)"},
    {AA_3, AA_3_IndicatePeerAborted, "AA 3 Indicate Peer Aborted"},
    {AA_4, AA_4_IndicateAPAbort, "AA 4 Indicate AP Abort"},
    {AA_5, AA_5_StopARTIMtimer, "AA 5 Stop ARTIM timer"},
    {AA_6, AA_6_IgnorePDU, "AA 6 Ignore PDU"},
    {AA_7, AA_7_State13SendAbort, "AA 7 State 13 Send Abort"},
    {AA_8, AA_8_UnrecognizedPDUSendAbort, "AA 8 Unrecognized PDU Send Abort"},

    {AR_1, AR_1_SendReleaseRQ, "AR 1 Send Release RQ"},
    {AR_2, AR_2_IndicateRelease, "AR 2 Indicate Release"},
    {AR_3, AR_3_ConfirmRelease, "AR 3 Confirm Release"},
    {AR_4, AR_4_SendReleaseRP, "AR 4 Send Release RP"},
    {AR_5, AR_5_StopARTIMtimer, "AR 5 Stop ARTIM timer"},
    {AR_6, AR_6_IndicatePData, "AR 6 Indicate P DATA PDU"},
    {AR_7, AR_7_SendPDATA, "AR 7 Send P DATA PDU"},
    {AR_8, AR_8_IndicateARelease, "AR 8 Indicate A RELEASE"},
    {AR_9, AR_9_SendAReleaseRP, "AR 9 Send A RELEASE RP"},
    {AR_10, AR_10_ConfirmRelease, "AR 10 Confirm Release"}
};

static FSM_ENTRY StateTable[DUL_NUMBER_OF_EVENTS][DUL_NUMBER_OF_STATES] = {
    {
        // EVENT,                    STATE,  ACTION,   NEXT_STATE
        {A_ASSOCIATE_REQ_LOCAL_USER, STATE1, AE_1, STATE4, "", "", NULL},
        {A_ASSOCIATE_REQ_LOCAL_USER, STATE2, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_REQ_LOCAL_USER, STATE3, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_REQ_LOCAL_USER, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_REQ_LOCAL_USER, STATE5, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_REQ_LOCAL_USER, STATE6, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_REQ_LOCAL_USER, STATE7, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_REQ_LOCAL_USER, STATE8, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_REQ_LOCAL_USER, STATE9, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_REQ_LOCAL_USER, STATE10, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_REQ_LOCAL_USER, STATE11, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_REQ_LOCAL_USER, STATE12, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_REQ_LOCAL_USER, STATE13, NOACTION, NOSTATE, "", "", NULL}},

    {
        {TRANS_CONN_CONFIRM_LOCAL_USER, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_CONFIRM_LOCAL_USER, STATE2, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_CONFIRM_LOCAL_USER, STATE3, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_CONFIRM_LOCAL_USER, STATE4, AE_2, STATE5, "", "", NULL},
        {TRANS_CONN_CONFIRM_LOCAL_USER, STATE5, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_CONFIRM_LOCAL_USER, STATE6, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_CONFIRM_LOCAL_USER, STATE7, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_CONFIRM_LOCAL_USER, STATE8, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_CONFIRM_LOCAL_USER, STATE9, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_CONFIRM_LOCAL_USER, STATE10, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_CONFIRM_LOCAL_USER, STATE11, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_CONFIRM_LOCAL_USER, STATE12, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_CONFIRM_LOCAL_USER, STATE13, NOACTION, NOSTATE, "", "", NULL}},

    {
        {A_ASSOCIATE_AC_PDU_RCV, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_AC_PDU_RCV, STATE2, AA_1, STATE13, "", "", NULL},
        {A_ASSOCIATE_AC_PDU_RCV, STATE3, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_AC_PDU_RCV, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_AC_PDU_RCV, STATE5, AE_3, STATE6, "", "", NULL},
        {A_ASSOCIATE_AC_PDU_RCV, STATE6, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_AC_PDU_RCV, STATE7, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_AC_PDU_RCV, STATE8, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_AC_PDU_RCV, STATE9, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_AC_PDU_RCV, STATE10, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_AC_PDU_RCV, STATE11, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_AC_PDU_RCV, STATE12, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_AC_PDU_RCV, STATE13, AA_6, STATE13, "", "", NULL}},

    {
        {A_ASSOCIATE_RJ_PDU_RCV, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RJ_PDU_RCV, STATE2, AA_1, STATE13, "", "", NULL},
        {A_ASSOCIATE_RJ_PDU_RCV, STATE3, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RJ_PDU_RCV, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RJ_PDU_RCV, STATE5, AE_4, STATE1, "", "", NULL},
        {A_ASSOCIATE_RJ_PDU_RCV, STATE6, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RJ_PDU_RCV, STATE7, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RJ_PDU_RCV, STATE8, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RJ_PDU_RCV, STATE9, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RJ_PDU_RCV, STATE10, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RJ_PDU_RCV, STATE11, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RJ_PDU_RCV, STATE12, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RJ_PDU_RCV, STATE13, AA_6, STATE13, "", "", NULL}},

    {
        {TRANS_CONN_INDICATION, STATE1, AE_5, STATE2, "", "", NULL},
        {TRANS_CONN_INDICATION, STATE2, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_INDICATION, STATE3, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_INDICATION, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_INDICATION, STATE5, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_INDICATION, STATE6, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_INDICATION, STATE7, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_INDICATION, STATE8, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_INDICATION, STATE9, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_INDICATION, STATE10, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_INDICATION, STATE11, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_INDICATION, STATE12, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_INDICATION, STATE13, NOACTION, NOSTATE, "", "", NULL}},

    {
        {A_ASSOCIATE_RQ_PDU_RCV, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RQ_PDU_RCV, STATE2, AE_6, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RQ_PDU_RCV, STATE3, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RQ_PDU_RCV, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RQ_PDU_RCV, STATE5, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RQ_PDU_RCV, STATE6, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RQ_PDU_RCV, STATE7, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RQ_PDU_RCV, STATE8, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RQ_PDU_RCV, STATE9, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RQ_PDU_RCV, STATE10, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RQ_PDU_RCV, STATE11, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RQ_PDU_RCV, STATE12, AA_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RQ_PDU_RCV, STATE13, AA_7, STATE13, "", "", NULL}},

    {
        {A_ASSOCIATE_RESPONSE_ACCEPT, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_ACCEPT, STATE2, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_ACCEPT, STATE3, AE_7, STATE6, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_ACCEPT, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_ACCEPT, STATE5, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_ACCEPT, STATE6, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_ACCEPT, STATE7, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_ACCEPT, STATE8, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_ACCEPT, STATE9, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_ACCEPT, STATE10, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_ACCEPT, STATE11, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_ACCEPT, STATE12, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_ACCEPT, STATE13, NOACTION, NOSTATE, "", "", NULL}},

    {
        {A_ASSOCIATE_RESPONSE_REJECT, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_REJECT, STATE2, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_REJECT, STATE3, AE_8, STATE13, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_REJECT, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_REJECT, STATE5, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_REJECT, STATE6, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_REJECT, STATE7, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_REJECT, STATE8, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_REJECT, STATE9, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_REJECT, STATE10, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_REJECT, STATE11, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_REJECT, STATE12, NOACTION, NOSTATE, "", "", NULL},
        {A_ASSOCIATE_RESPONSE_REJECT, STATE13, NOACTION, NOSTATE, "", "", NULL}},

    {
        {P_DATA_REQ, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {P_DATA_REQ, STATE2, NOACTION, NOSTATE, "", "", NULL},
        {P_DATA_REQ, STATE3, NOACTION, NOSTATE, "", "", NULL},
        {P_DATA_REQ, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {P_DATA_REQ, STATE5, NOACTION, NOSTATE, "", "", NULL},
        {P_DATA_REQ, STATE6, DT_1, STATE6, "", "", NULL},
        {P_DATA_REQ, STATE7, NOACTION, NOSTATE, "", "", NULL},
        {P_DATA_REQ, STATE8, AR_7, STATE8, "", "", NULL},
        {P_DATA_REQ, STATE9, NOACTION, NOSTATE, "", "", NULL},
        {P_DATA_REQ, STATE10, NOACTION, NOSTATE, "", "", NULL},
        {P_DATA_REQ, STATE11, NOACTION, NOSTATE, "", "", NULL},
        {P_DATA_REQ, STATE12, NOACTION, NOSTATE, "", "", NULL},
        {P_DATA_REQ, STATE13, NOACTION, NOSTATE, "", "", NULL}},

    {
        {P_DATA_TF_PDU_RCV, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {P_DATA_TF_PDU_RCV, STATE2, AA_1, STATE13, "", "", NULL},
        {P_DATA_TF_PDU_RCV, STATE3, AA_8, STATE13, "", "", NULL},
        {P_DATA_TF_PDU_RCV, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {P_DATA_TF_PDU_RCV, STATE5, AA_8, STATE13, "", "", NULL},
        {P_DATA_TF_PDU_RCV, STATE6, DT_2, STATE6, "", "", NULL},
        {P_DATA_TF_PDU_RCV, STATE7, AR_6, STATE7, "", "", NULL},
        {P_DATA_TF_PDU_RCV, STATE8, AA_8, STATE13, "", "", NULL},
        {P_DATA_TF_PDU_RCV, STATE9, AA_8, STATE13, "", "", NULL},
        {P_DATA_TF_PDU_RCV, STATE10, AA_8, STATE13, "", "", NULL},
        {P_DATA_TF_PDU_RCV, STATE11, AA_8, STATE13, "", "", NULL},
        {P_DATA_TF_PDU_RCV, STATE12, AA_8, STATE13, "", "", NULL},
        {P_DATA_TF_PDU_RCV, STATE13, AA_6, STATE13, "", "", NULL}},

    {
        {A_RELEASE_REQ, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_REQ, STATE2, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_REQ, STATE3, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_REQ, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_REQ, STATE5, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_REQ, STATE6, AR_1, STATE7, "", "", NULL},
        {A_RELEASE_REQ, STATE7, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_REQ, STATE8, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_REQ, STATE9, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_REQ, STATE10, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_REQ, STATE11, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_REQ, STATE12, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_REQ, STATE13, NOACTION, NOSTATE, "", "", NULL}},

    {
        {A_RELEASE_RQ_PDU_RCV, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_RQ_PDU_RCV, STATE2, AA_1, STATE13, "", "", NULL},
        {A_RELEASE_RQ_PDU_RCV, STATE3, AA_8, STATE13, "", "", NULL},
        {A_RELEASE_RQ_PDU_RCV, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_RQ_PDU_RCV, STATE5, AA_8, STATE13, "", "", NULL},
        {A_RELEASE_RQ_PDU_RCV, STATE6, AR_2, STATE8, "", "", NULL},
        {A_RELEASE_RQ_PDU_RCV, STATE7, AR_8, NOSTATE, "", "", NULL},
        {A_RELEASE_RQ_PDU_RCV, STATE8, AA_8, STATE13, "", "", NULL},
        {A_RELEASE_RQ_PDU_RCV, STATE9, AA_8, STATE13, "", "", NULL},
        {A_RELEASE_RQ_PDU_RCV, STATE10, AA_8, STATE13, "", "", NULL},
        {A_RELEASE_RQ_PDU_RCV, STATE11, AA_8, STATE13, "", "", NULL},
        {A_RELEASE_RQ_PDU_RCV, STATE12, AA_8, STATE13, "", "", NULL},
        {A_RELEASE_RQ_PDU_RCV, STATE13, AA_6, STATE13, "", "", NULL}},

    {
        {A_RELEASE_RP_PDU_RCV, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_RP_PDU_RCV, STATE2, AA_1, STATE13, "", "", NULL},
        {A_RELEASE_RP_PDU_RCV, STATE3, AA_8, STATE13, "", "", NULL},
        {A_RELEASE_RP_PDU_RCV, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_RP_PDU_RCV, STATE5, AA_8, STATE13, "", "", NULL},
        {A_RELEASE_RP_PDU_RCV, STATE6, AA_8, STATE13, "", "", NULL},
        {A_RELEASE_RP_PDU_RCV, STATE7, AR_3, STATE1, "", "", NULL},
        {A_RELEASE_RP_PDU_RCV, STATE8, AA_8, STATE13, "", "", NULL},
        {A_RELEASE_RP_PDU_RCV, STATE9, AA_8, STATE13, "", "", NULL},
        {A_RELEASE_RP_PDU_RCV, STATE10, AR_10, STATE12, "", "", NULL},
        {A_RELEASE_RP_PDU_RCV, STATE11, AR_3, STATE1, "", "", NULL},
        {A_RELEASE_RP_PDU_RCV, STATE12, AA_8, STATE13, "", "", NULL},
        {A_RELEASE_RP_PDU_RCV, STATE13, AA_6, STATE13, "", "", NULL}},

    {
        {A_RELEASE_RESP, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_RESP, STATE2, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_RESP, STATE3, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_RESP, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_RESP, STATE5, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_RESP, STATE6, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_RESP, STATE7, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_RESP, STATE8, AR_4, STATE13, "", "", NULL},
        {A_RELEASE_RESP, STATE9, AR_9, STATE11, "", "", NULL},
        {A_RELEASE_RESP, STATE10, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_RESP, STATE11, NOACTION, NOSTATE, "", "", NULL},
        {A_RELEASE_RESP, STATE12, AR_4, STATE13, "", "", NULL},
        {A_RELEASE_RESP, STATE13, NOACTION, NOSTATE, "", "", NULL}},

    {
        {A_ABORT_REQ, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {A_ABORT_REQ, STATE2, NOACTION, NOSTATE, "", "", NULL},
        {A_ABORT_REQ, STATE3, AA_1, STATE13, "", "", NULL},
        {A_ABORT_REQ, STATE4, AA_2, STATE1, "", "", NULL},
        {A_ABORT_REQ, STATE5, AA_1, STATE13, "", "", NULL},
        {A_ABORT_REQ, STATE6, AA_1, STATE13, "", "", NULL},
        {A_ABORT_REQ, STATE7, AA_1, STATE13, "", "", NULL},
        {A_ABORT_REQ, STATE8, AA_1, STATE13, "", "", NULL},
        {A_ABORT_REQ, STATE9, AA_1, STATE13, "", "", NULL},
        {A_ABORT_REQ, STATE10, AA_1, STATE13, "", "", NULL},
        {A_ABORT_REQ, STATE11, AA_1, STATE13, "", "", NULL},
        {A_ABORT_REQ, STATE12, AA_1, STATE13, "", "", NULL},
        {A_ABORT_REQ, STATE13, NOACTION, NOSTATE, "", "", NULL}},

    {
        {A_ABORT_PDU_RCV, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {A_ABORT_PDU_RCV, STATE2, AA_2, STATE1, "", "", NULL},
        {A_ABORT_PDU_RCV, STATE3, AA_3, STATE1, "", "", NULL},
        {A_ABORT_PDU_RCV, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {A_ABORT_PDU_RCV, STATE5, AA_3, STATE1, "", "", NULL},
        {A_ABORT_PDU_RCV, STATE6, AA_3, STATE1, "", "", NULL},
        {A_ABORT_PDU_RCV, STATE7, AA_3, STATE1, "", "", NULL},
        {A_ABORT_PDU_RCV, STATE8, AA_3, STATE1, "", "", NULL},
        {A_ABORT_PDU_RCV, STATE9, AA_3, STATE1, "", "", NULL},
        {A_ABORT_PDU_RCV, STATE10, AA_3, STATE1, "", "", NULL},
        {A_ABORT_PDU_RCV, STATE11, AA_3, STATE1, "", "", NULL},
        {A_ABORT_PDU_RCV, STATE12, AA_3, STATE1, "", "", NULL},
        {A_ABORT_PDU_RCV, STATE13, AA_2, STATE1, "", "", NULL}},

    {
        {TRANS_CONN_CLOSED, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {TRANS_CONN_CLOSED, STATE2, AA_5, STATE1, "", "", NULL},
        {TRANS_CONN_CLOSED, STATE3, AA_4, STATE1, "", "", NULL},
        {TRANS_CONN_CLOSED, STATE4, AA_4, STATE1, "", "", NULL},
        {TRANS_CONN_CLOSED, STATE5, AA_4, STATE1, "", "", NULL},
        {TRANS_CONN_CLOSED, STATE6, AA_4, STATE1, "", "", NULL},
        {TRANS_CONN_CLOSED, STATE7, AA_4, STATE1, "", "", NULL},
        {TRANS_CONN_CLOSED, STATE8, AA_4, STATE1, "", "", NULL},
        {TRANS_CONN_CLOSED, STATE9, AA_4, STATE1, "", "", NULL},
        {TRANS_CONN_CLOSED, STATE10, AA_4, STATE1, "", "", NULL},
        {TRANS_CONN_CLOSED, STATE11, AA_4, STATE1, "", "", NULL},
        {TRANS_CONN_CLOSED, STATE12, AA_4, STATE1, "", "", NULL},
        {TRANS_CONN_CLOSED, STATE13, AR_5, STATE1, "", "", NULL}},

    {
        {ARTIM_TIMER_EXPIRED, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {ARTIM_TIMER_EXPIRED, STATE2, AA_2T, STATE1, "", "", NULL},
        {ARTIM_TIMER_EXPIRED, STATE3, NOACTION, NOSTATE, "", "", NULL},
        {ARTIM_TIMER_EXPIRED, STATE4, NOACTION, NOSTATE, "", "", NULL},

        // DICOM part 8 does not define an action and state for the
        // situation where a timeout occurs while we are waiting for an
        // incoming A-ASSOCIATE-AC or A-ASSOCIATE-RJ. We close the transport
        // connection, return an error code indicating a timeout,
        // and reset the FSM to idle state (STATE1).
        {ARTIM_TIMER_EXPIRED, STATE5, AA_2T, STATE1, "", "", NULL},

        {ARTIM_TIMER_EXPIRED, STATE6, NOACTION, NOSTATE, "", "", NULL},

        // DICOM part 8 does not define an action and state for the
        // situation where a timeout occurs while we are waiting for an
        // incoming A-RELEASE-RSP. We close the transport
        // connection, return an error code indicating a timeout,
        // and reset the FSM to idle state (STATE1).
        {ARTIM_TIMER_EXPIRED, STATE7, AA_2T, STATE1, "", "", NULL},

        {ARTIM_TIMER_EXPIRED, STATE8, NOACTION, NOSTATE, "", "", NULL},
        {ARTIM_TIMER_EXPIRED, STATE9, NOACTION, NOSTATE, "", "", NULL},
        {ARTIM_TIMER_EXPIRED, STATE10, NOACTION, NOSTATE, "", "", NULL},
        {ARTIM_TIMER_EXPIRED, STATE11, NOACTION, NOSTATE, "", "", NULL},
        {ARTIM_TIMER_EXPIRED, STATE12, NOACTION, NOSTATE, "", "", NULL},
        {ARTIM_TIMER_EXPIRED, STATE13, AA_2, STATE1, "", "", NULL}},

    {
        {INVALID_PDU, STATE1, NOACTION, NOSTATE, "", "", NULL},
        {INVALID_PDU, STATE2, AA_1, STATE13, "", "", NULL},
        {INVALID_PDU, STATE3, AA_8, STATE13, "", "", NULL},
        {INVALID_PDU, STATE4, NOACTION, NOSTATE, "", "", NULL},
        {INVALID_PDU, STATE5, AA_8, STATE13, "", "", NULL},
        {INVALID_PDU, STATE6, AA_8, STATE13, "", "", NULL},
        {INVALID_PDU, STATE7, AA_8, STATE13, "", "", NULL},
        {INVALID_PDU, STATE8, AA_8, STATE13, "", "", NULL},
        {INVALID_PDU, STATE9, AA_8, STATE13, "", "", NULL},
        {INVALID_PDU, STATE10, AA_8, STATE13, "", "", NULL},
        {INVALID_PDU, STATE11, AA_8, STATE13, "", "", NULL},
        {INVALID_PDU, STATE12, AA_8, STATE13, "", "", NULL},
        {INVALID_PDU, STATE13, AA_7, STATE13, "", "", NULL}}
};


/* Dul_InitializeFSM
**
** Purpose:
**      Initialize the DUL finite state machine by filling in addresses of
**      functions.
**
** Parameter Dictionary:
**      None
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
OFCondition
DUL_InitializeFSM()
{
    unsigned long
        l_index,
        idx2;
    FSM_ENTRY
        * stateEntries;

    stateEntries = (FSM_ENTRY *) StateTable;
    for (l_index = 0; l_index < DUL_NUMBER_OF_EVENTS * DUL_NUMBER_OF_STATES; l_index++) {
        if (stateEntries[l_index].action != NOACTION) {
            for (idx2 = 0; idx2 < DIM_OF(FSM_FunctionTable) &&
                 stateEntries[l_index].actionFunction == NULL; idx2++)
                if (stateEntries[l_index].action == FSM_FunctionTable[idx2].action) {
                    stateEntries[l_index].actionFunction =
                        FSM_FunctionTable[idx2].actionFunction;
                    (void) sprintf(stateEntries[l_index].actionName, "%.*s",
                                 (int)(sizeof(stateEntries[l_index].actionName) - 1),
                                   FSM_FunctionTable[idx2].actionName);
                }
        }
        for (idx2 = 0; idx2 < DIM_OF(Event_Table) &&
             strlen(stateEntries[l_index].eventName) == 0; idx2++) {
            if (stateEntries[l_index].event == Event_Table[idx2].event)
                (void) sprintf(stateEntries[l_index].eventName, "%.*s",
                               (int)(sizeof(stateEntries[l_index].eventName) - 1),
                               Event_Table[idx2].eventName);
        }
    }

    return EC_Normal;
}



/* PRV_StateMachine
**
** Purpose:
**      Execute the action function, given the current state and the event.
**
** Parameter Dictionary:
**      network         Handle to the network environment
**      association     Handle to the Association
**      event           The event that will trigger this action
**      state           Current state of the finite state machine.
**      params          Service parameters describing this Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
OFCondition
PRV_StateMachine(PRIVATE_NETWORKKEY ** network,
                 PRIVATE_ASSOCIATIONKEY ** association, int event, int state,
                 void *params)
{
    FSM_ENTRY
        * entry;

    /* check if the given event is valid, if not return an error */
    if (event < 0 || event >= DUL_NUMBER_OF_EVENTS)
    {
      char buf1[256];
      sprintf(buf1, "DUL Finite State Machine Error: Bad event, state %d event %d", state, event);
      return makeDcmnetCondition(DULC_FSMERROR, OF_error, buf1);
    }

    /* check if the given state is valid, if not return an error */
    if (state < 1 || state > DUL_NUMBER_OF_STATES)
    {
      char buf1[256];
      sprintf(buf1, "DUL Finite State Machine Error: Bad state, state %d event %d", state, event);
      return makeDcmnetCondition(DULC_FSMERROR, OF_error, buf1);
    }

    /* depending on the given event and state, determine the state table's entry (the state */
    /* table implements the state transition table of DICOM's Upper Layer State Machine which */
    /* in turn implements the DICOM upper layer protocol) (see DICOM standard (year 2000) part */
    /* 8, section 9) (or the corresponding section in a later version of the standard) */
    entry = &StateTable[event][state - 1];

    /* dump information if required */
    DCMNET_TRACE("DUL  FSM Table: State: " << state << " Event: " << event << OFendl
            << "DUL  Event:  " << entry->eventName << OFendl
            << "DUL  Action: " << entry->actionName);

    /* if the state table's entry specifies an action function, execute this function and return */
    /* it's result value. If there is no action function defined, return a corresponding error. */
    if (entry->actionFunction != NULL)
        return entry->actionFunction(network, association, entry->nextState, params);
    else
    {
      char buf1[256];
      sprintf(buf1, "DUL Finite State Machine Error: No action defined, state %d event %d", state, event);
      return makeDcmnetCondition(DULC_FSMERROR, OF_error, buf1);
    }
}

/* ============================================================
**
**  Private functions (local to this module) defined below.
*/

/* AE_1_TransportConnect
**
** Purpose:
**      Issue a TRANSPORT_CONNECT request primitive to local transport
**      service.
**
** Parameter Dictionary:
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
AE_1_TransportConnect(PRIVATE_NETWORKKEY ** network,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    DUL_ASSOCIATESERVICEPARAMETERS
    * service;
    OFCondition cond = EC_Normal;

    service = (DUL_ASSOCIATESERVICEPARAMETERS *) params;
    clearPDUCache(association);
    cond = requestAssociationTCP(network, service, association);
    (*association)->protocolState = nextState;
    return cond;
}

/* AE_2_SendAssociateRQPDU
**
** Purpose:
**      Send A-ASSOCIATE-RQ PDU.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
AE_2_SendAssociateRQPDU(PRIVATE_NETWORKKEY ** network,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    DUL_ASSOCIATESERVICEPARAMETERS
    * service;
    OFCondition cond = EC_Normal;

    service = (DUL_ASSOCIATESERVICEPARAMETERS *) params;

    cond = sendAssociationRQTCP(network, service, association);
    (*association)->protocolState = nextState;
    return cond;
}


/* AE_3_AssociateConfirmationAccept
**
** Purpose:
**      Issue an A-ASSOCIATE confirmation (Accept) primitive
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
AE_3_AssociateConfirmationAccept(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    DUL_ASSOCIATESERVICEPARAMETERS
    * service;
    unsigned char
        * buffer = NULL,
        pduType,
        pduReserve;
    unsigned long
        pduLength;
    PRV_ASSOCIATEPDU
        assoc;
    PRV_PRESENTATIONCONTEXTITEM
        * prvCtx;
    DUL_PRESENTATIONCONTEXT
        * userPresentationCtx,
        * requestedPresentationCtx;
    DUL_SUBITEM
        * subItem;
    PRV_SCUSCPROLE
        * scuscpRole;

    service = (DUL_ASSOCIATESERVICEPARAMETERS *) params;
    OFCondition cond = readPDU(association, DUL_BLOCK, 0, &buffer, &pduType, &pduReserve, &pduLength);

    if (cond.bad())
    {
       if (buffer) free(buffer);
       return cond;
    }

    /* cond is good so we know that buffer exists */

    DCMNET_DEBUG(dump_pdu("Associate Accept", buffer, pduLength + 6));

    if (pduType == DUL_TYPEASSOCIATEAC)
    {
        if ((*association)->associatePDUFlag)
        {
          // copy A-ASSOCIATE-AC PDU
          (*association)->associatePDU = new char[pduLength+6];
          if ((*association)->associatePDU)
          {
            memcpy((*association)->associatePDU, buffer, (size_t) pduLength+6);
            (*association)->associatePDULength = pduLength+6;
          }
        }

        cond = parseAssociate(buffer, pduLength, &assoc);
        free(buffer);
        if (cond.bad()) return makeDcmnetSubCondition(DULC_ILLEGALPDU, OF_error, "DUL Illegal or ill-formed PDU", cond);

        OFStandard::strlcpy(service->respondingAPTitle, assoc.calledAPTitle, sizeof(service->respondingAPTitle));
        OFStandard::strlcpy(service->callingAPTitle, assoc.callingAPTitle, sizeof(service->callingAPTitle));
        OFStandard::strlcpy(service->applicationContextName, assoc.applicationContext.data, sizeof(service->applicationContextName));

        if ((service->acceptedPresentationContext = LST_Create()) == NULL) return EC_MemoryExhausted;

        prvCtx = (PRV_PRESENTATIONCONTEXTITEM*)LST_Head(&assoc.presentationContextList);
        if (prvCtx != NULL)
            (void) LST_Position(&assoc.presentationContextList, (LST_NODE*)prvCtx);
        while (prvCtx != NULL) {
            userPresentationCtx = (DUL_PRESENTATIONCONTEXT*)malloc(sizeof(DUL_PRESENTATIONCONTEXT));
            if (userPresentationCtx == NULL) return EC_MemoryExhausted;

            (void) memset(userPresentationCtx, 0, sizeof(DUL_PRESENTATIONCONTEXT));
            userPresentationCtx->result = prvCtx->result;
            userPresentationCtx->presentationContextID = prvCtx->contextID;
            userPresentationCtx->proposedTransferSyntax = NULL;
            requestedPresentationCtx = findPresentationCtx(
                 &service->requestedPresentationContext, prvCtx->contextID);
            if (requestedPresentationCtx != NULL) {
                OFStandard::strlcpy(userPresentationCtx->abstractSyntax,
                    requestedPresentationCtx->abstractSyntax,
                    sizeof(userPresentationCtx->abstractSyntax));
                userPresentationCtx->proposedSCRole =
                    requestedPresentationCtx->proposedSCRole;
            }
            userPresentationCtx->acceptedSCRole = DUL_SC_ROLE_DEFAULT;
            scuscpRole = findSCUSCPRole(&assoc.userInfo.SCUSCPRoleList,
                                        userPresentationCtx->abstractSyntax);
            if (scuscpRole != NULL) {
                if ((scuscpRole->SCURole == 0) && (scuscpRole->SCPRole == 0))
                    userPresentationCtx->acceptedSCRole = DUL_SC_ROLE_NONE;
                else if ((scuscpRole->SCURole == 1) && (scuscpRole->SCPRole == 1))
                    userPresentationCtx->acceptedSCRole = DUL_SC_ROLE_SCUSCP;
                else if (scuscpRole->SCURole == 1)
                    userPresentationCtx->acceptedSCRole = DUL_SC_ROLE_SCU;
                else  // SCPRole == 1
                    userPresentationCtx->acceptedSCRole = DUL_SC_ROLE_SCP;
            }
            if (prvCtx->transferSyntaxList == NULL)
            {
              char buf1[256];
              sprintf(buf1, "DUL Peer supplied illegal number of transfer syntaxes (%d)", 0);
              free(userPresentationCtx);
              return makeDcmnetCondition(DULC_PEERILLEGALXFERSYNTAXCOUNT, OF_error, buf1);
            }

            if ((prvCtx->result == DUL_PRESENTATION_ACCEPT) && (LST_Count(&prvCtx->transferSyntaxList) != 1))
            {
              char buf2[256];
              sprintf(buf2, "DUL Peer supplied illegal number of transfer syntaxes (%ld)", LST_Count(&prvCtx->transferSyntaxList));
              free(userPresentationCtx);
              return makeDcmnetCondition(DULC_PEERILLEGALXFERSYNTAXCOUNT, OF_error, buf2);
            }
            subItem = (DUL_SUBITEM*)LST_Head(&prvCtx->transferSyntaxList);
            if (subItem != NULL)
                OFStandard::strlcpy(userPresentationCtx->acceptedTransferSyntax,
                              subItem->data, sizeof(userPresentationCtx->acceptedTransferSyntax));
            LST_Enqueue(&service->acceptedPresentationContext, (LST_NODE*)userPresentationCtx);

            prvCtx = (PRV_PRESENTATIONCONTEXTITEM*)LST_Next(&assoc.presentationContextList);

        }

        /* extended negotiation */
        if (assoc.userInfo.extNegList != NULL) {
            service->acceptedExtNegList = new SOPClassExtendedNegotiationSubItemList;
            if (service->acceptedExtNegList == NULL)  return EC_MemoryExhausted;
            appendList(*assoc.userInfo.extNegList, *service->acceptedExtNegList);
        }

        /* user identity negotiation */
        if (assoc.userInfo.usrIdent != NULL) {
          service->ackUserIdentNeg =
            new UserIdentityNegotiationSubItemAC( *(OFstatic_cast(UserIdentityNegotiationSubItemAC*, assoc.userInfo.usrIdent)));
          if (service->ackUserIdentNeg == NULL)  return EC_MemoryExhausted;

        }

        destroyPresentationContextList(&assoc.presentationContextList);
        destroyUserInformationLists(&assoc.userInfo);
        service->peerMaxPDU = assoc.userInfo.maxLength.maxLength;
        (*association)->maxPDV = assoc.userInfo.maxLength.maxLength;
        (*association)->maxPDVAcceptor =
            assoc.userInfo.maxLength.maxLength;
        OFStandard::strlcpy(service->calledImplementationClassUID,
               assoc.userInfo.implementationClassUID.data, DICOM_UI_LENGTH + 1);
        OFStandard::strlcpy(service->calledImplementationVersionName,
               assoc.userInfo.implementationVersionName.data, 16 + 1);

        (*association)->associationState = DUL_ASSOC_ESTABLISHED;
        (*association)->protocolState = nextState;
        return EC_Normal;
    }
    return DUL_UNEXPECTEDPDU;
}

/* AE_4_AssociateConfirmationReject
**
** Purpose:
**      Issue A-ASSOCIATE confirmation reject primitive and close transport
**      connection.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
AE_4_AssociateConfirmationReject(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    DUL_ASSOCIATESERVICEPARAMETERS
    * service;
    unsigned char
        buffer[128],
        pduType,
        pduReserve;
    unsigned long
        pduLength;

    service = (DUL_ASSOCIATESERVICEPARAMETERS *) params;
    OFCondition cond = readPDUBody(association, DUL_BLOCK, 0, buffer, sizeof(buffer),
                       &pduType, &pduReserve, &pduLength);
    if (cond.bad())
        return cond;

    if (pduType == DUL_TYPEASSOCIATERJ) {
        service->result = buffer[1];
        service->resultSource = buffer[2];
        service->diagnostic = buffer[3];
        (*association)->protocolState = nextState;
        closeTransport(association);
        cond = DUL_ASSOCIATIONREJECTED;
    } else cond = DUL_UNEXPECTEDPDU;

    return cond;
}


/* AE_5_TransportConnectResponse
**
** Purpose:
**      Issue Transport connection response primitive and start ARTIM timer.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:

**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
AE_5_TransportConnectResponse(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    clearPDUCache(association);
    (*association)->protocolState = nextState;

    /* Start the timer (?) */

    return EC_Normal;
}



/* AE_6_ExamineAssociateRequest
**
** Purpose:
**      Stop ARTIM timer and if A-ASSOCIATE-RQ acceptable by service-provider,
**      issue A-ASSOCIATE indication primitive else issue A-ASSOCIATE
**      indication primitive.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
AE_6_ExamineAssociateRequest(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int /*nextState*/, void *params)
{
    DUL_ASSOCIATESERVICEPARAMETERS
    * service;
    unsigned char
        *buffer=NULL,
        pduType,
        pduReserve;
    unsigned long
        pduLength;
    PRV_ASSOCIATEPDU
        assoc;

    (*association)->timerStart = 0;
    service = (DUL_ASSOCIATESERVICEPARAMETERS *) params;
    OFCondition cond = readPDU(association, DUL_BLOCK, 0, &buffer,
                   &pduType, &pduReserve, &pduLength);

    if (cond.bad())
    {
       if (buffer) free(buffer);
       return cond;
    }

    /* cond is good so we know that buffer exists */

    if (pduType == DUL_TYPEASSOCIATERQ)
    {
        if ((*association)->associatePDUFlag)
        {
          // copy A-ASSOCIATE-RQ PDU
          (*association)->associatePDU = new char[pduLength+6];
          if ((*association)->associatePDU)
          {
            memcpy((*association)->associatePDU, buffer, (size_t) pduLength+6);
            (*association)->associatePDULength = pduLength+6;
          }
        }

        DCMNET_DEBUG(dump_pdu("Associate Request", buffer, pduLength + 6));
        cond = parseAssociate(buffer, pduLength, &assoc);
        free(buffer);
        buffer = NULL;

        if (cond.bad()) {
            if (cond == DUL_UNSUPPORTEDPEERPROTOCOL)    /* Make it look OK */
                (*association)->protocolState = STATE3;
            return cond;
        }
        OFStandard::strlcpy(service->calledAPTitle, assoc.calledAPTitle, sizeof(service->calledAPTitle));
        OFStandard::strlcpy(service->callingAPTitle, assoc.callingAPTitle, sizeof(service->callingAPTitle));
        OFStandard::strlcpy(service->applicationContextName, assoc.applicationContext.data, sizeof(service->applicationContextName));

        if ((service->requestedPresentationContext = LST_Create()) == NULL) return EC_MemoryExhausted;
        if (translatePresentationContextList(&assoc.presentationContextList,
                                             &assoc.userInfo.SCUSCPRoleList,
                                             &service->requestedPresentationContext).bad())
        {
            return DUL_PCTRANSLATIONFAILURE;
        }

        /* extended negotiation */
        if (assoc.userInfo.extNegList != NULL) {
            service->requestedExtNegList = new SOPClassExtendedNegotiationSubItemList;
            if (service->requestedExtNegList == NULL) return EC_MemoryExhausted;
            appendList(*assoc.userInfo.extNegList, *service->requestedExtNegList);
        }

        /* user identity negotiation: Remember request values in association parameters (copy)*/
        if (assoc.userInfo.usrIdent != NULL) {
          service->reqUserIdentNeg = new UserIdentityNegotiationSubItemRQ();
          if (service->reqUserIdentNeg == NULL) return EC_MemoryExhausted;
            *(service->reqUserIdentNeg) = *(OFstatic_cast(UserIdentityNegotiationSubItemRQ*,assoc.userInfo.usrIdent));
        }

        service->peerMaxPDU = assoc.userInfo.maxLength.maxLength;
        (*association)->maxPDV = assoc.userInfo.maxLength.maxLength;
        (*association)->maxPDVRequestor =
            assoc.userInfo.maxLength.maxLength;
        OFStandard::strlcpy(service->callingImplementationClassUID,
               assoc.userInfo.implementationClassUID.data, DICOM_UI_LENGTH + 1);
        OFStandard::strlcpy(service->callingImplementationVersionName,
               assoc.userInfo.implementationVersionName.data, 16 + 1);
        (*association)->associationState = DUL_ASSOC_ESTABLISHED;

        destroyPresentationContextList(&assoc.presentationContextList);
        destroyUserInformationLists(&assoc.userInfo);

        /* If this PDU is ok with us */
        (*association)->protocolState = STATE3;

        return EC_Normal;
    }
    return DUL_UNEXPECTEDPDU;
}


/* AE_7_SendAssociateAC
**
** Purpose:
**      Send A-ASSOCIATE-AC PDU.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
AE_7_SendAssociateAC(PRIVATE_NETWORKKEY ** network,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    DUL_ASSOCIATESERVICEPARAMETERS
    * service;
    OFCondition cond = EC_Normal;

    service = (DUL_ASSOCIATESERVICEPARAMETERS *) params;
    cond = sendAssociationACTCP(network, service, association);
    (*association)->protocolState = nextState;
    return cond;
}


/* AE_8_SendAssociateRJ
**
** Purpose:
**      Send A-ASSOCIATE-RJ PDU and start ARTIM timer.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
AE_8_SendAssociateRJ(PRIVATE_NETWORKKEY ** network,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    DUL_ABORTITEMS
    * abortItems;
    OFCondition cond = EC_Normal;

    abortItems = (DUL_ABORTITEMS *) params;
    cond = sendAssociationRJTCP(network, abortItems, association);
    (*association)->protocolState = nextState;

    /* Start the timer (?) */

    return cond;
}

/* DT_1_SendPData
**
** Purpose:
**      Send P-DATA-TF PDU
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
DT_1_SendPData(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    OFCondition cond = EC_Normal;
    DUL_PDVLIST
        * pdvList;

    /* Remember that params used to be a variable of type DUL_PDVLIST * and restore this variable */
    pdvList = (DUL_PDVLIST *) params;

    /* send the data which is contained in pdvList over the network */
    cond = sendPDataTCP(association, pdvList);

    /* and go to the next specified state */
    (*association)->protocolState = nextState;

    /* return return value */
    return cond;
}

/* DT_2_IndicatePData
**
** Purpose:
**      Send P-DATA indication primitive.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
DT_2_IndicatePData(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    unsigned char
        pduType,
        pduReserved;
    unsigned long
        pduLength,
        pdvLength,
        pdvCount;
    long
        length;
    unsigned char
       *p;

    /* determine the finite state machine's next state */
    (*association)->protocolState = nextState;

    /* read PDU body information from the incoming socket stream. In case the incoming */
    /* PDU's header information has not yet been read, also read this information. */
    OFCondition cond = readPDUBody(association, DUL_BLOCK, 0,
                       (*association)->fragmentBuffer,
                       (*association)->fragmentBufferLength,
                       &pduType, &pduReserved, &pduLength);

    /* return error if there was one */
    if (cond.bad())
        return cond;

    /* count the amount of PDVs in the current PDU */
    length = pduLength;                     //set length to the PDU's length
    pdvCount = 0;                           //set counter variable to 0
    p = (*association)->fragmentBuffer;     //set p to the buffer which contains the PDU's PDVs
    while (length >= 4) {                   //as long as length is at least 4 (= a length field can be read)
        EXTRACT_LONG_BIG(p, pdvLength);     //determine the length of the current PDV (the PDV p points to)
        p += 4 + pdvLength;                 //move p so that it points to the next PDV (move p 4 bytes over the length field plus the amount of bytes which is captured in the PDV's length field (over presentation context.Id, message information header and data fragment))
        length -= 4 + pdvLength;            //update length (i.e. determine the length of the buffer which has not been evaluated yet.)
        pdvCount++;                         //increase counter by one, since we've found another PDV

        // There must be at least a presentation context ID and a message
        // control header (see below), else the calculation pdvLength - 2 below
        // will underflow.
        if (pdvLength < 2)
        {
           char buf[256];
           sprintf(buf, "PDV with invalid length %lu encountered. This probably indicates a malformed P DATA PDU.", pdvLength);
           return makeDcmnetCondition(DULC_ILLEGALPDULENGTH, OF_error, buf);
        }
    }

    /* if after having counted the PDVs the length variable does not equal */
    /* 0, the PDV lengths did not add up correctly. Something is fishy. */
    if (length != 0)
    {
       char buf[256];
       sprintf(buf, "PDV lengths don't add up correctly: %d PDVs. This probably indicates a malformed P-DATA PDU. PDU type is %02x.", (int)pdvCount, (unsigned int) pduType);
       return makeDcmnetCondition(DULC_ILLEGALPDU, OF_error, buf);
    }

    /* let the the association indicate how many PDVs are contained in the PDU */
    (*association)->pdvCount = (int)pdvCount;

    /* if at least one PDV is contained in the PDU, the association's pdvIndex has to be set to 0 */
    if (pdvCount > 0)
        (*association)->pdvIndex = 0;
    else
    {
        /* if there is no PDV contained in the PDU, the association's pdvIndex has to be set to -1 */
        (*association)->pdvIndex = -1;

        /* bugfix by wilkens 01/10/12: return error if PDU does not contain any PDVs: */
        /* Additionally, it is not allowed to have a PDU that does not contain any PDVs. */
        /* Hence, return an error (see DICOM standard (year 2000) part 8, section 9.3.1, */
        /* figure 9-2) (or the corresponding section in a later version of the standard) */
       char buf[256];
       sprintf(buf, "PDU without any PDVs encountered. In DT_2_IndicatePData.  This probably indicates a  malformed P DATA PDU." );
       return makeDcmnetCondition(DULC_ILLEGALPDU, OF_error, buf);
    }

    /* at this point we need to set the association's currentPDV variable so that it contains the data */
    /* of the next unprocessed PDV (currentPDV shall always contain the next unprocessed PDV's data.) */

    /* set the fragment buffer (the buffer which contains all PDVs of the current PDU) to a local variable */
    p = (*association)->fragmentBuffer;

    /* The variable (*association)->pdvPointer always points to the buffer */
    /* address where the information of the PDV which is represented by the */
    /* association's currentPDV variable can be found. Set this variable. */
    (*association)->pdvPointer = p;

    /* determine the value in the PDV length field of the next (unprocessed) PDV */
    EXTRACT_LONG_BIG(p, pdvLength);

    /* set the data fragment length in the association's currentPDV.fragmentLength variable */
    /* (we start now overwriting all the entries in (*association)->currentPDV). The actual */
    /* length of the data fragment of the next (unprocessed) PDV equals the above determined */
    /* length minus 1 byte (for the presentation context ID) and minus another byte (for the */
    /* message control header). */
    (*association)->currentPDV.fragmentLength = pdvLength - 2;

    /* set the presentation context ID value in the association's currentPDV.presentationContextID */
    /* variable. This value is 1 byte wide and contained in the 5th byte of p (the first four bytes */
    /* contain the PDV length value, the fifth byte the presentation context ID value) */
    (*association)->currentPDV.presentationContextID = p[4];

    /* now determine if the next (unprocessed) PDV contains the last fragment of a data set or DIMSE */
    /* command and if the next (unprocessed) PDV is a data set PDV or command PDV. This information */
    /* is captured in the 6th byte of p: */
    unsigned char u = p[5];
    if (u & 2)
        (*association)->currentPDV.lastPDV = OFTrue;            //if bit 1 of the message control header is 1, this fragment does contain the last fragment of a data set or command
    else
        (*association)->currentPDV.lastPDV = OFFalse;           //if bit 1 of the message control header is 0, this fragment does not contain the last fragment of a data set or command

    if (u & 1)
        (*association)->currentPDV.pdvType = DUL_COMMANDPDV;    //if bit 0 of the message control header is 1, this is a command PDV
    else
        (*association)->currentPDV.pdvType = DUL_DATASETPDV;    //if bit 0 of the message control header is 0, this is a data set PDV

    /* now assign the data fragment of the next (unprocessed) PDV to the association's */
    /* currentPDV.data variable. The fragment starts 6 bytes to the right of the address */
    /* p currently points to. */
    (*association)->currentPDV.data = p + 6;

    /* return ok */
    return DUL_PDATAPDUARRIVED;
}


/* AR_1_SendReleaseRQ
**
** Purpose:
**      Send A-RELEASE-RQ PDU.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
AR_1_SendReleaseRQ(PRIVATE_NETWORKKEY ** /* network */,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    OFCondition cond = EC_Normal;

    cond = sendReleaseRQTCP(association);
    (*association)->protocolState = nextState;
    return cond;
}

/* AR_2_IndicateRelease
**
** Purpose:
**      Issue A-RELEASE indication primitive.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
AR_2_IndicateRelease(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    unsigned char
        buffer[128],
        pduType,
        pduReserve;
    unsigned long
        pduLength;

    /* Read remaining unimportant bytes of the A-RELEASE-RQ PDU */
    OFCondition cond = readPDUBody(association, DUL_BLOCK, 0, buffer, sizeof(buffer),
                       &pduType, &pduReserve, &pduLength);
    if (cond.bad())
        return cond;

    if (pduLength == 4)
    {
      unsigned long mode = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
      if ((*association)->modeCallback && !((mode & DUL_MAXPDUCOMPAT) ^ DUL_DULCOMPAT))
      {
        (*association)->modeCallback->callback(mode);
      }
    }

    (*association)->protocolState = nextState;
    return DUL_PEERREQUESTEDRELEASE;
}

/* AR_3_ConfirmRelease
**
** Purpose:
**      Issue A-RELEASE confirmation primitive and close transport connection.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
AR_3_ConfirmRelease(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    unsigned char
        buffer[128],
        pduType,
        pduReserve;
    unsigned long
        pduLength;

    /* Read remaining unimportant bytes of the A-RELEASE-RSP PDU */
    OFCondition cond = readPDUBody(association, DUL_BLOCK, 0, buffer, sizeof(buffer),
                       &pduType, &pduReserve, &pduLength);

    closeTransport(association);
    (*association)->protocolState = nextState;
    return cond;
}

/* AR_4_SendReleaseRP
**
** Purpose:
**      Issue A-RELEASE-RP PDU and start ARTIM timer.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
AR_4_SendReleaseRP(PRIVATE_NETWORKKEY ** /* network */,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    OFCondition cond = EC_Normal;

    cond = sendReleaseRPTCP(association);
    (*association)->timerStart = time(NULL);
    (*association)->protocolState = nextState;
    return cond;
}

/* AR_5_StopARTTIMtimer
**
** Purpose:
**      Stop ARTIM timer.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:

**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
AR_5_StopARTIMtimer(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int /*nextState*/, void * /*params*/)
{
    (*association)->timerStart = 0;
    return EC_Normal;
}

/* AR_6_IndicatePData
**
** Purpose:
**      Issue P-DATA indication.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
AR_6_IndicatePData(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** /*association*/, int /*nextState*/, void * /*params*/)
{
    return DUL_PDATAPDUARRIVED;
}

/* AR_7_SendPData
**
** Purpose:
**      Issue P-DATA-TF PDU
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
AR_7_SendPDATA(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    OFCondition cond = EC_Normal;
    DUL_PDVLIST
        * pdvList;

    pdvList = (DUL_PDVLIST *) params;
    cond = sendPDataTCP(association, pdvList);
    (*association)->protocolState = nextState;
    return cond;
}

/* AR_8_IndicateARelease
**
** Purpose:
**      Issue A-RELEASE indication (release collision):
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
AR_8_IndicateARelease(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int /*nextState*/, void * /*params*/)
{
/*    if (strcmp((*association)->applicationType, AE_REQUESTOR) == 0) */

    unsigned char
        buffer[128],
        pduType,
        pduReserve;
    unsigned long
        pduLength;

    /* Read remaining unimportant bytes of the A-RELEASE-RQ PDU */
    OFCondition cond = readPDUBody(association, DUL_BLOCK, 0, buffer, sizeof(buffer),
                       &pduType, &pduReserve, &pduLength);
    if (cond.bad())
        return cond;

    if ((*association)->applicationFunction == DICOM_APPLICATION_REQUESTOR)
        (*association)->protocolState = STATE9;
    else
        (*association)->protocolState = STATE10;

    return DUL_PEERREQUESTEDRELEASE;
}

/* AR_9_SendAReleaseRP
**
** Purpose:
**      Send A-RELEASE-RP PDU
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
AR_9_SendAReleaseRP(PRIVATE_NETWORKKEY ** /* network */,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    OFCondition cond = sendReleaseRPTCP(association);
    (*association)->protocolState = nextState;
    return cond;
}

/* AR_10_ConfirmRelease
**
** Purpose:
**      Issue A-RELEASE confirmation primitive.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
AR_10_ConfirmRelease(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    (*association)->protocolState = nextState;
    return EC_Normal;
}


/* AA_1_SendAbort
**
** Purpose:
**      Send A-ABORT PDU (service-user source) and start (or restart if
**      already started) ARTIM timer.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
AA_1_SendAAbort(PRIVATE_NETWORKKEY ** /* network */,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    DUL_ABORTITEMS abortItems;

    abortItems.result = 0x00;
    abortItems.source = DUL_ABORTSERVICEUSER;
    abortItems.reason = 0x00;

    OFCondition cond = sendAbortTCP(&abortItems, association);
    (*association)->protocolState = nextState;
    (*association)->timerStart = time(NULL);
    return cond;
}

/* AA_2_CloseTransport
**
** Purpose:
**      Close transport connection.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:

**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
AA_2_CloseTransport(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    (*association)->timerStart = 0;
    closeTransport(association);
    (*association)->protocolState = nextState;
    return EC_Normal;
}

/* AA_2_CloseTimeout
**
** Purpose:
**      Stop ARTIM timer.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
AA_2_CloseTimeout(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    (*association)->timerStart = 0;
    closeTransport(association);
    (*association)->protocolState = nextState;
    return DUL_READTIMEOUT;
}


/* AA_3_IndicatePeerAborted
**
** Purpose:
**      If (service-user initiated abort)
**         - issue A-ABORT indication and close transport connection
**      otherwise (service provider initiated abort)
**         - issue A-P-ABORT indication and close transport connection.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
AA_3_IndicatePeerAborted(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    unsigned char
        buffer[128],
        pduType,
        pduReserve;
    unsigned long
        pduLength;

    /* Read remaining unimportant bytes of the A-ABORT PDU */
    OFCondition cond = readPDUBody(association, DUL_BLOCK, 0, buffer, sizeof(buffer),
                       &pduType, &pduReserve, &pduLength);
    if (cond.bad()) return cond;

    if (pduLength == 4)
    {
      unsigned long mode = pduReserve << 24 | buffer[0] << 16 | buffer[1] << 8 | buffer[3];
      if ((*association)->modeCallback && !((mode & DUL_MAXPDUCOMPAT) ^ DUL_DULCOMPAT))
      {
        (*association)->modeCallback->callback(mode);
      }
    }

    closeTransport(association);
    (*association)->protocolState = nextState;
    return DUL_PEERABORTEDASSOCIATION;
}


/* AA_4_IndicateAPAbort
**
** Purpose:
**      Issue A-P-ABORT indication primitive.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
AA_4_IndicateAPAbort(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    closeTransport(association);
    (*association)->protocolState = nextState;
    return DUL_PEERABORTEDASSOCIATION;
}


/* AA_5_StopARTIMtimer
**
** Purpose:
**      Stop ARTIM timer.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
AA_5_StopARTIMtimer(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    (*association)->timerStart = 0;
    (*association)->protocolState = nextState;
    return EC_Normal;
}


/* AA_6_IgnorePDU
**
** Purpose:
**      Ignore PDU
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/


static OFCondition
AA_6_IgnorePDU(PRIVATE_NETWORKKEY ** /*network*/,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    unsigned char
        buffer[1024],
        PDUType,
        PDUReserved;
    long
        PDULength;
    unsigned long
        l;

    (*association)->protocolState = nextState;
    OFCondition cond = readPDUHead(association, buffer, sizeof(buffer), DUL_NOBLOCK,
                    PRV_DEFAULTTIMEOUT, &PDUType, &PDUReserved, &l);
    if (cond.bad()) {
        return cond;
    }
    PDULength = l;
    while (PDULength > 0 && cond.good())
    {
        cond = defragmentTCP((*association)->connection,
                             DUL_NOBLOCK, (*association)->timerStart,
                   (*association)->timeout, buffer, sizeof(buffer), &l);
        if (cond.bad()) return cond;
        PDULength -= l;
    }
    return EC_Normal;
}


/* AA_7_State13SendAbort
**
** Purpose:
**      SendA-ABORT PDU
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
AA_7_State13SendAbort(PRIVATE_NETWORKKEY ** /* network */,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    DUL_ABORTITEMS abortItems;

    abortItems.result = 0x00;
    abortItems.source = DUL_ABORTSERVICEPROVIDER;
    abortItems.reason = DUL_ABORTUNEXPECTEDPDU;

    OFCondition cond = sendAbortTCP(&abortItems, association);
    (*association)->protocolState = nextState;
    return cond;
}


/* AA_8_UnrecognizedPDUSendAbort
**
** Purpose:
**      Send A-ABORT PDU (service provider source), issue an A-P-ABORT
**      indication and start timer.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      association     Handle to the Association
**      nextState       The next state to be reached from the current state
**      params          Service parameters describing the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
AA_8_UnrecognizedPDUSendAbort(PRIVATE_NETWORKKEY ** /* network */,
         PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    DUL_ABORTITEMS abortItems;

    abortItems.result = 0x00;
    abortItems.source = DUL_ABORTSERVICEPROVIDER;
    abortItems.reason = DUL_ABORTUNEXPECTEDPDU;

    OFCondition cond = sendAbortTCP(&abortItems, association);
    (*association)->protocolState = nextState;
    (*association)->timerStart = time(0);
    return cond;
}

/* requestAssociationTCP
**
** Purpose:
**      Request a TCP Association
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      params          Service parameters describing the Association
**      association     Handle to the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/


static OFCondition
requestAssociationTCP(PRIVATE_NETWORKKEY ** network,
                      DUL_ASSOCIATESERVICEPARAMETERS * params,
                      PRIVATE_ASSOCIATIONKEY ** association)
{
    char node[128];
    int  port;
    OFSockAddr server;
#ifdef _WIN32
    SOCKET s;
#else
    int s;
#endif
    struct linger sockarg;

    if (sscanf(params->calledPresentationAddress, "%[^:]:%d", node, &port) != 2)
    {
        char buf[1024];
        sprintf(buf,"Illegal service parameter: %s", params->calledPresentationAddress);
        return makeDcmnetCondition(DULC_ILLEGALSERVICEPARAMETER, OF_error, buf);
    }

    /*
     * At least officially, gethostbyname will not accept an IP address on many
     * operating systems (e.g. Windows or FreeBSD), so we need to explicitly
     * handle the IP address case.
     */
    unsigned long addr = inet_addr(node);
    if (addr != INADDR_NONE)
    {
        // it is an IPv4 address
        server.setFamily(AF_INET);
        struct sockaddr_in *sa = server.getSockaddr_in();
        sa->sin_addr.s_addr = addr;
    }
    else
    {
        // must be a host name or an IPv6 address
        OFStandard::getAddressByHostname(node, server);
        if (server.getFamily() == 0)
        {
          char buf2[4095]; // node could be a long string
          sprintf(buf2, "Attempt to connect to unknown host: %s", node);
          return makeDcmnetCondition(DULC_UNKNOWNHOST, OF_error, buf2);
        }
    }
    server.setPort(OFstatic_cast(unsigned short, htons(port)));

    // get global connection timeout
    Sint32 connectTimeout = dcmConnectionTimeout.get();

    s = socket(server.getFamily(), SOCK_STREAM, 0);
#ifdef _WIN32
    if (s == INVALID_SOCKET)
#else
    if (s < 0)
#endif
    {
      OFString msg = "TCP Initialization Error: ";
      msg += OFStandard::getLastNetworkErrorCode().message();
      return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
    }

#ifdef HAVE_WINSOCK_H
    u_long arg = TRUE;
#else
    int flags = 0;
#endif

    if (connectTimeout >= 0)
    {
      // user has specified a timeout, switch socket to non-blocking mode
#ifdef HAVE_WINSOCK_H
      ioctlsocket(s, FIONBIO, (u_long FAR *) &arg);
#else
      flags = fcntl(s, F_GETFL, 0);
      fcntl(s, F_SETFL, O_NONBLOCK | flags);
#endif
    }

    // depending on the socket mode, connect will block or return immediately
    int rc;
    do {
        rc = connect(s, server.getSockaddr(), server.size());
    } while (rc == -1 && OFStandard::getLastNetworkErrorCode().value() == DCMNET_EINTR);

#ifdef HAVE_WINSOCK_H
    if (rc == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
#else
    if (rc < 0 && errno == EINPROGRESS)
#endif
    {
#ifndef DCMTK_HAVE_POLL
        // we're in non-blocking mode. Prepare to wait for timeout.
        fd_set fdSet;
        FD_ZERO(&fdSet);
#ifdef __MINGW32__
        // on MinGW, FD_SET expects an unsigned first argument
        FD_SET((unsigned int) s, &fdSet);
#else
        FD_SET(s, &fdSet);
#endif /* __MINGW32__ */
#endif /* DCMTK_HAVE_POLL */

        struct timeval timeout;
        timeout.tv_sec = connectTimeout;
        timeout.tv_usec = 0;

        do {
#ifdef DCMTK_HAVE_POLL
            struct pollfd pfd[] =
            {
                { s, POLLOUT, 0 }
            };
            rc = poll(pfd, 1, timeout.tv_sec*1000+(timeout.tv_usec/1000));
#else
            // the typecast is safe because Windows ignores the first select() parameter anyway
            rc = select(OFstatic_cast(int, s + 1), NULL, &fdSet, NULL, &timeout);
#endif
        } while (rc == -1 && OFStandard::getLastNetworkErrorCode().value() == DCMNET_EINTR);

        if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
        {
            DU_logSelectResult(rc);
        }

        // reset socket to blocking mode
#ifdef HAVE_WINSOCK_H
        arg = FALSE;
        ioctlsocket(s, FIONBIO, (u_long FAR *) &arg);
#else
        fcntl(s, F_SETFL, flags);
#endif
        if (rc == 0)
        {
            // timeout reached, bail out with error return code
#ifdef HAVE_WINSOCK_H
            (void) shutdown(s,  1 /* SD_SEND */);
            (void) closesocket(s);
#else
            (void) close(s);
#endif
            (*association)->networkState = NETWORK_DISCONNECTED;
            if ((*association)->connection) delete (*association)->connection;
            (*association)->connection = NULL;

            OFString msg = "TCP Initialization Error: ";
            msg += OFStandard::getLastNetworkErrorCode().message();
            msg += " (Timeout)";
            return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
  }
#ifndef HAVE_WINSOCK_H
        else if (rc > 0)
        {
            // select reports that our connection request has proceeded.
            // This could either mean success or an asynchronous error condition.
            // use getsockopt to check the socket status.
            int socketError = 0;

#ifdef HAVE_DECLARATION_SOCKLEN_T
            // some platforms (e.g. Solaris 7) declare socklen_t
            socklen_t socketErrorLen = sizeof(socketError);
#elif defined(HAVE_INTP_GETSOCKOPT)
            // some platforms (e.g. Solaris 2.5.1) prefer int
            int socketErrorLen = (int) sizeof(socketError);
#else
            // some platforms (e.g. OSF1 4.0) prefer size_t
            size_t socketErrorLen = sizeof(socketError);
#endif

            // Solaris 2.5.1 expects a char * as argument 4 of getsockopt. Most other
            // platforms expect void *, so casting to a char * should be safe.
            getsockopt(s, SOL_SOCKET, SO_ERROR, (char *)(&socketError), &socketErrorLen);
            if (socketError)
            {
                // asynchronous error on our socket, bail out.
                (void) close(s);
                (*association)->networkState = NETWORK_DISCONNECTED;
                if ((*association)->connection) delete (*association)->connection;
                (*association)->connection = NULL;

                char buf[256];
                OFString msg = "TCP Initialization Error: ";
                msg += OFStandard::strerror(socketError, buf, sizeof(buf));
                return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
            }
        }
#endif
    }
    else
    {
        // The connect() returned without using the select(), reset the socket if needed
        if (connectTimeout >= 0)
        {
            // reset socket to blocking mode
#ifdef HAVE_WINSOCK_H
            arg = FALSE;
            ioctlsocket(s, FIONBIO, (u_long FAR *) &arg);
#else
            fcntl(s, F_SETFL, flags);
#endif
        }
    }

    if (rc < 0)
    {
        // an error other than timeout in non-blocking mode has occurred,
        // either in connect() or in select().
#ifdef HAVE_WINSOCK_H
        (void) shutdown(s,  1 /* SD_SEND */);
        (void) closesocket(s);
#else
        (void) close(s);
#endif
        (*association)->networkState = NETWORK_DISCONNECTED;
        if ((*association)->connection) delete (*association)->connection;
        (*association)->connection = NULL;

        OFString msg = "TCP Initialization Error: ";
        msg += OFStandard::getLastNetworkErrorCode().message();
        return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
    } else {
        // success - we've opened a TCP transport connection

        (*association)->networkState = NETWORK_CONNECTED;
        if ((*association)->connection) delete (*association)->connection;

        if (network && (*network) && ((*network)->networkSpecific.TCP.tLayer))
        {
          (*association)->connection = ((*network)->networkSpecific.TCP.tLayer)->createConnection(s, params->useSecureLayer);
        }
        else (*association)->connection = NULL;

        if ((*association)->connection == NULL)
        {
#ifdef HAVE_WINSOCK_H
          (void) shutdown(s,  1 /* SD_SEND */);
          (void) closesocket(s);
#else
          (void) close(s);
#endif
          (*association)->networkState = NETWORK_DISCONNECTED;

          OFString msg = "TCP Initialization Error: ";
          msg += OFStandard::getLastNetworkErrorCode().message();
          return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
        }
        sockarg.l_onoff = 0;
        sockarg.l_linger = 0;

        if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char *) &sockarg, (int) sizeof(sockarg)) < 0)
        {
          OFString msg = "TCP Initialization Error: ";
          msg += OFStandard::getLastNetworkErrorCode().message();
          return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
        }
        setTCPBufferLength(s);

        /*
         * Disable the so-called Nagle algorithm (if requested).
         * This might provide a better network performance on some systems/environments.
         * By default, the algorithm is not disabled unless DISABLE_NAGLE_ALGORITHM is defined.
         * The default behavior can be changed by setting the environment variable TCP_NODELAY.
         */

#ifdef DONT_DISABLE_NAGLE_ALGORITHM
#warning The macro DONT_DISABLE_NAGLE_ALGORITHM is not supported anymore. See "macros.txt" for details.
#endif

#ifdef DISABLE_NAGLE_ALGORITHM
        int tcpNoDelay = 1; // disable
#else
        int tcpNoDelay = 0; // don't disable
#endif
        char* tcpNoDelayString = NULL;
        DCMNET_TRACE("checking whether environment variable TCP_NODELAY is set");
        if ((tcpNoDelayString = getenv("TCP_NODELAY")) != NULL)
        {
          if (sscanf(tcpNoDelayString, "%d", &tcpNoDelay) != 1)
          {
            DCMNET_WARN("DULFSM: cannot parse environment variable TCP_NODELAY=" << tcpNoDelayString);
          }
        } else
          DCMNET_TRACE("  environment variable TCP_NODELAY not set, using the default value (" << tcpNoDelay << ")");
        if (tcpNoDelay) {
#ifdef DISABLE_NAGLE_ALGORITHM
          DCMNET_DEBUG("DULFSM: disabling Nagle algorithm as defined at compilation time (DISABLE_NAGLE_ALGORITHM)");
#else
          DCMNET_DEBUG("DULFSM: disabling Nagle algorithm as requested at runtime (TCP_NODELAY=" << tcpNoDelayString << ")");
#endif
          if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&tcpNoDelay, sizeof(tcpNoDelay)) < 0)
          {
            OFString msg = "TCP Initialization Error: ";
            msg += OFStandard::getLastNetworkErrorCode().message();
            return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
          }
#ifdef DISABLE_NAGLE_ALGORITHM
        } else {
          DCMNET_DEBUG("DULFSM: do not disable Nagle algorithm as requested at runtime (TCP_NODELAY=" << tcpNoDelayString << ")");
#endif
        }

       DcmTransportLayerStatus tcsStatus;
       if (TCS_ok != (tcsStatus = (*association)->connection->clientSideHandshake()))
       {
         DCMNET_ERROR("TLS client handshake failed");
         OFString msg = "DUL secure transport layer: ";
         msg += (*association)->connection->errorString(tcsStatus);
         return makeDcmnetCondition(DULC_TLSERROR, OF_error, msg.c_str());
       }
       return EC_Normal;
    }
}


/* sendAssociationRQTCP
**
** Purpose:
**      Send a TCP association request PDU.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      params          Service parameters describing the Association
**      association     Handle to the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
sendAssociationRQTCP(PRIVATE_NETWORKKEY ** /*network*/,
                     DUL_ASSOCIATESERVICEPARAMETERS * params,
                     PRIVATE_ASSOCIATIONKEY ** association)
{
    PRV_ASSOCIATEPDU
    associateRequest;
    unsigned char
        buffer[4096],
       *b;
    unsigned long
        length;
    int
        nbytes;

    OFBitmanipTemplate<char>::zeroMem((char *)&associateRequest, sizeof(PRV_ASSOCIATEPDU)); // initialize PDU
    // associateRequest.presentationContextList = NULL;
    OFCondition cond = constructAssociatePDU(params, DUL_TYPEASSOCIATERQ,
                                 &associateRequest);
    if (cond.bad())
    {
        DCMNET_ERROR(cond.text());
        return cond;
    }
    if (associateRequest.length + 6 <= sizeof(buffer))
        b = buffer;
    else {
        b = (unsigned char*)malloc(size_t(associateRequest.length + 6));
        if (b == NULL)  return EC_MemoryExhausted;
    }
    cond = streamAssociatePDU(&associateRequest, b,
                              associateRequest.length + 6, &length);

    if ((*association)->associatePDUFlag)
    {
      // copy A-ASSOCIATE-RQ PDU
      (*association)->associatePDU = new char[length];
      if ((*association)->associatePDU)
      {
        memcpy((*association)->associatePDU, b, (size_t) length);
        (*association)->associatePDULength = length;
      }
    }

    destroyPresentationContextList(&associateRequest.presentationContextList);
    destroyUserInformationLists(&associateRequest.userInfo);
    if (cond.bad())
        return cond;

    do {
      nbytes = (*association)->connection ? (*association)->connection->write((char*)b, size_t(associateRequest.length + 6)) : 0;
    } while (nbytes == -1 && OFStandard::getLastNetworkErrorCode().value() == DCMNET_EINTR);
    if ((unsigned long) nbytes != associateRequest.length + 6)
    {
      OFString msg = "TCP I/O Error (";
      msg += OFStandard::getLastNetworkErrorCode().message();
      msg += ") occurred in routine: sendAssociationRQTCP";
      return makeDcmnetCondition(DULC_TCPIOERROR, OF_error, msg.c_str());
    }
    if (b != buffer) free(b);
    return EC_Normal;
}

/* sendAssociationACTCP
**
** Purpose:
**      Send an Association ACK on a TCP connection
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      params          Service parameters describing the Association
**      association     Handle to the Association
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
sendAssociationACTCP(PRIVATE_NETWORKKEY ** /*network*/,
                     DUL_ASSOCIATESERVICEPARAMETERS * params,
                     PRIVATE_ASSOCIATIONKEY ** association)
{
    PRV_ASSOCIATEPDU
    associateReply;
    unsigned char
        buffer[4096],
       *b;
    unsigned long length = 0;
    int nbytes;
    DUL_ASSOCIATESERVICEPARAMETERS localService;

    OFBitmanipTemplate<char>::zeroMem((char *)&associateReply, sizeof(PRV_ASSOCIATEPDU)); // initialize PDU
    // associateReply.presentationContextList = NULL;

    localService = *params;
    OFCondition cond = constructAssociatePDU(&localService, DUL_TYPEASSOCIATEAC,
                                 &associateReply);
    if (cond.bad())
    {
        DCMNET_ERROR(cond.text());
        return cond;
    }

    // we need to have length+6 bytes in buffer, but 4 bytes reserve won't hurt
    if (associateReply.length + 10 <= sizeof(buffer)) b = buffer;
    else {
        b = (unsigned char*)malloc(size_t(associateReply.length + 10));
        if (b == NULL)  return EC_MemoryExhausted;
    }
    cond = streamAssociatePDU(&associateReply, b,
                              associateReply.length + 10, &length);

    if ((*association)->associatePDUFlag)
    {
      // copy A-ASSOCIATE-AC PDU
      (*association)->associatePDU = new char[length];
      if ((*association)->associatePDU)
      {
        memcpy((*association)->associatePDU, b, (size_t) length);
        (*association)->associatePDULength = length;
      }
    }

    destroyPresentationContextList(&associateReply.presentationContextList);
    destroyUserInformationLists(&associateReply.userInfo);

    if (cond.bad()) return cond;

    do {
      nbytes = (*association)->connection ? (*association)->connection->write((char*)b, size_t(associateReply.length + 6)) : 0;
    } while (nbytes == -1 && OFStandard::getLastNetworkErrorCode().value() == DCMNET_EINTR);
    if ((unsigned long) nbytes != associateReply.length + 6)
    {
      OFString msg = "TCP I/O Error (";
      msg += OFStandard::getLastNetworkErrorCode().message();
      msg += ") occurred in routine: sendAssociationACTCP";
      return makeDcmnetCondition(DULC_TCPIOERROR, OF_error, msg.c_str());
    }
    if (b != buffer) free(b);
    return EC_Normal;
}

/* sendAssociationRJTCP
**
** Purpose:
**      Send an Association Reject PDU on a TCP connection.
**
** Parameter Dictionary:
**
**      network         Handle to the network environment
**      abortItems      Pointer to structure holding information regarding
**                      the reason for rejection, the source and result.
**      association     Handle to the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
sendAssociationRJTCP(PRIVATE_NETWORKKEY ** /*network*/,
         DUL_ABORTITEMS * abortItems, PRIVATE_ASSOCIATIONKEY ** association)
{

    DUL_REJECTRELEASEABORTPDU
        pdu;
    unsigned char
        buffer[64],
       *b;
    unsigned long
        length;
    int
        nbytes;


    OFCondition cond = constructAssociateRejectPDU((unsigned char) abortItems->result,
        (unsigned char) abortItems->source, (unsigned char) abortItems->reason, &pdu);
    if (pdu.length + 6 <= sizeof(buffer))
        b = buffer;
    else {
        b = (unsigned char*)malloc(size_t(pdu.length + 6));
        if (b == NULL)  return EC_MemoryExhausted;
    }
    cond = streamRejectReleaseAbortPDU(&pdu, b, pdu.length + 6, &length);

    if ((*association)->associatePDUFlag)
    {
      // copy A-ASSOCIATE-RJ PDU
      (*association)->associatePDU = new char[length];
      if ((*association)->associatePDU)
      {
        memcpy((*association)->associatePDU, b, (size_t) length);
        (*association)->associatePDULength = length;
      }
    }

    if (cond.good())
    {
        do {
          nbytes = (*association)->connection ? (*association)->connection->write((char*)b, size_t(pdu.length + 6)) : 0;
        } while (nbytes == -1 && OFStandard::getLastNetworkErrorCode().value() == DCMNET_EINTR);
        if ((unsigned long) nbytes != pdu.length + 6)
        {
          OFString msg = "TCP I/O Error (";
          msg += OFStandard::getLastNetworkErrorCode().message();
          msg += ") occurred in routine: sendAssociationRJTCP";
          return makeDcmnetCondition(DULC_TCPIOERROR, OF_error, msg.c_str());
        }
    }
    if (b != buffer) free(b);
    return cond;
}

/* sendAbortTCP
**
** Purpose:
**      Send an ABORT PDU over a TCP connection.
**
** Parameter Dictionary:
**      abortItems      Pointer to structure holding information regarding
**                      the reason for rejection, the source and result.
**      association     Handle to the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
sendAbortTCP(DUL_ABORTITEMS * abortItems,
             PRIVATE_ASSOCIATIONKEY ** association)
{
    DUL_REJECTRELEASEABORTPDU
    pdu;
    unsigned char
        buffer[64],
       *b;
    unsigned long
        length;
    int
        nbytes;

    OFCondition cond = constructAbortPDU(abortItems->source, abortItems->reason, &pdu, (*association)->compatibilityMode);
    if (cond.bad())
        return cond;

    if (pdu.length + 6 <= sizeof(buffer))
        b = buffer;
    else {
        b = (unsigned char*)malloc(size_t(pdu.length + 6));
        if (b == NULL)  return EC_MemoryExhausted;
    }
    cond = streamRejectReleaseAbortPDU(&pdu, b, pdu.length + 6, &length);
    if (cond.good()) {
        do {
          nbytes = (*association)->connection ? (*association)->connection->write((char*)b, size_t(pdu.length + 6)) : 0;
        } while (nbytes == -1 && OFStandard::getLastNetworkErrorCode().value() == DCMNET_EINTR);
        if ((unsigned long) nbytes != pdu.length + 6)
        {
          OFString msg = "TCP I/O Error (";
          msg += OFStandard::getLastNetworkErrorCode().message();
          msg += ") occurred in routine: sendAbortTCP";
          return makeDcmnetCondition(DULC_TCPIOERROR, OF_error, msg.c_str());
        }
    }
    if (b != buffer) free(b);

    return cond;
}


/* sendReleaseRQTCP
**
** Purpose:
**      Send a Release request PDU over a TCP connection
**
** Parameter Dictionary:
**
**      association     Handle to the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
sendReleaseRQTCP(PRIVATE_ASSOCIATIONKEY ** association)
{
    DUL_REJECTRELEASEABORTPDU
    pdu;
    unsigned char
        buffer[64],
       *b;
    unsigned long
        length;
    int
        nbytes;

    OFCondition cond = constructReleaseRQPDU(&pdu, (*association)->compatibilityMode);
    if (cond.bad())
        return cond;

    if (pdu.length + 6 <= sizeof(buffer))
        b = buffer;
    else {
        b = (unsigned char*)malloc(size_t(pdu.length + 6));
        if (b == NULL)  return EC_MemoryExhausted;
    }
    cond = streamRejectReleaseAbortPDU(&pdu, b, pdu.length + 6, &length);
    if (cond.good()) {
        do {
          nbytes = (*association)->connection ? (*association)->connection->write((char*)b, size_t(pdu.length + 6)) : 0;
        } while (nbytes == -1 && OFStandard::getLastNetworkErrorCode().value() == DCMNET_EINTR);
        if ((unsigned long) nbytes != pdu.length + 6)
        {
          OFString msg = "TCP I/O Error (";
          msg += OFStandard::getLastNetworkErrorCode().message();
          msg += ") occurred in routine: sendReleaseRQTCP";
          return makeDcmnetCondition(DULC_TCPIOERROR, OF_error, msg.c_str());
        }
    }
    if (b != buffer)
        free(b);

    return cond;
}



/* sendReleaseRPTCP
**
** Purpose:
**      Send a release response PDU over a TCP connection
**
** Parameter Dictionary:
**
**      association     Handle to the Association
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
sendReleaseRPTCP(PRIVATE_ASSOCIATIONKEY ** association)
{
    DUL_REJECTRELEASEABORTPDU
    pdu;
    unsigned char buffer[64],
       *b;
    unsigned long
        length;
    int
        nbytes;

    OFCondition cond = constructReleaseRPPDU(&pdu);
    if (cond.bad())
        return cond;

    if (pdu.length + 6 <= sizeof(buffer))
        b = buffer;
    else {
        b = (unsigned char*)malloc(size_t(pdu.length + 6));
        if (b == NULL)  return EC_MemoryExhausted;
    }
    cond = streamRejectReleaseAbortPDU(&pdu, b, pdu.length + 6, &length);
    if (cond.good()) {
        do {
          nbytes = (*association)->connection ? (*association)->connection->write((char*)b, size_t(pdu.length + 6)) : 0;
        } while (nbytes == -1 && OFStandard::getLastNetworkErrorCode().value() == DCMNET_EINTR);
        if ((unsigned long) nbytes != pdu.length + 6)
        {
          OFString msg = "TCP I/O Error (";
          msg += OFStandard::getLastNetworkErrorCode().message();
          msg += ") occurred in routine: sendReleaseRPTCP";
          return makeDcmnetCondition(DULC_TCPIOERROR, OF_error, msg.c_str());
        }
    }
    if (b != buffer) free(b);

    return cond;
}


/* SendPDataTCP
**
** Purpose:
**      Send a data PDU over a TCP connection.
**
** Parameter Dictionary:
**
**      association     Handle to the Association
**      pdvList         Pointer to structure holding a list of PDVs
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
sendPDataTCP(PRIVATE_ASSOCIATIONKEY ** association,
             DUL_PDVLIST * pdvList)
{
    DUL_PDV *pdv;
    unsigned long
        count,
        length,
        pdvLength,
        maxLength;

    OFBool localLast;
    unsigned char *p;
    DUL_DATAPDU dataPDU;
    OFBool firstTrip;

    /* assign the amount of PDVs in the array and the PDV array itself to local variables */
    count = pdvList->count;
    pdv = pdvList->pdv;

    /* determine the maximum size (length) of a PDU which can be sent over the network. */
    /* Note that the name "maxPDV" here is misleading. This field contains the maxPDU */
    /* size which is max PDV size +6 or max PDV data field + 12. */
    maxLength = (*association)->maxPDV;

    /* send the error indicator variable to ok */
    OFCondition cond = EC_Normal;

    /* adjust maxLength (maximum length of a PDU) */
    if (maxLength == 0) maxLength = ASC_MAXIMUMPDUSIZE - 12;
    else if (maxLength < 14)
    {
       char buf[256];
       sprintf(buf, "DUL Cannot send P-DATA PDU because receiver's max PDU size of %lu is illegal (must be > 12)", maxLength);
       cond = makeDcmnetCondition(DULC_ILLEGALPDULENGTH, OF_error, buf);
    }
    else maxLength -= 12;

    /* start a loop iterate over all PDVs in the given */
    /* list and send every PDVs data over the network */
    while (cond.good() && count > 0)
    {
        --count;
        /* determine length of PDV */
        length = pdv->fragmentLength;
        /* determine data to be set */
        p = (unsigned char *) pdv->data;
        /* because the current PDV's length can be greater than maxLength, we need */
        /* to start another loop so that we are able to send data gradually. So, */
        /* as long as this is the first iteration or length is greater than 0 and */
        /* at the same time no error occurred, do the following */
        firstTrip = OFTrue;
        while ((firstTrip || (length > 0)) && (cond.good())) {
            /* indicate that the first iteration has been executed */
            firstTrip = OFFalse;
            /* determine the length of the data fragment that will be sent */
            pdvLength = (length <= maxLength) ? length : maxLength;
            /* determine if the following fragment will be the last fragment to send */
            localLast = ((pdvLength == length) && pdv->lastPDV);
            /* construct a data PDU */
            cond = constructDataPDU(p, pdvLength, pdv->pdvType,
                           pdv->presentationContextID, localLast, &dataPDU);
            /* send the constructed PDU over the network */
            cond = writeDataPDU(association, &dataPDU);

            /* adjust the pointer to the data, so that he points to data which still has to be sent */
            p += pdvLength;
            /* adjust the length of the fragment which still has to be sent */
            length -= pdvLength;
        }
        /* advance to the next PDV */
        pdv++;

    }
    /* return corresponding result value */
    return cond;
}

/* writeDataPDU
**
** Purpose:
**      Send the data through the socket interface (for TCP).
**
** Parameter Dictionary:
**
**      association     Handle to the Association
**      pdu             The data unit that is to be sent thru the socket
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
writeDataPDU(PRIVATE_ASSOCIATIONKEY ** association,
             DUL_DATAPDU * pdu)
{
    unsigned char
        head[24];
    unsigned long
        length;
    int
        nbytes;

    /* construct a stream variable that will contain PDU head information */
    /* (in detail, this variable will contain PDU type, PDU reserved field, */
    /* PDU length, PDV length, presentation context ID, message control header) */
    /* (note that our representation of a PDU can only contain one PDV.) */
    OFCondition cond = streamDataPDUHead(pdu, head, sizeof(head), &length);
    if (cond.bad()) return cond;

    /* send the PDU head information (see above) */
    do
    {
      nbytes = (*association)->connection ? (*association)->connection->write((char*)head, size_t(length)) : 0;
    } while (nbytes == -1 && OFStandard::getLastNetworkErrorCode().value() == DCMNET_EINTR);

    /* if not all head information was sent, return an error */
    if ((unsigned long) nbytes != length)
    {
        OFString msg = "TCP I/O Error (";
        msg += OFStandard::getLastNetworkErrorCode().message();
        msg += ") occurred in routine: writeDataPDU";
        return makeDcmnetCondition(DULC_TCPIOERROR, OF_error, msg.c_str());
    }

    /* send the PDU's PDV data (note that our representation of a PDU can only contain one PDV.) */
    do
    {
      nbytes = (*association)->connection ? (*association)->connection->write((char*)pdu->presentationDataValue.data,
        size_t(pdu->presentationDataValue.length - 2)) : 0;
    } while (nbytes == -1 && OFStandard::getLastNetworkErrorCode().value() == DCMNET_EINTR);

        /* if not all head information was sent, return an error */
    if ((unsigned long) nbytes != pdu->presentationDataValue.length - 2)
    {
        OFString msg = "TCP I/O Error (";
        msg += OFStandard::getLastNetworkErrorCode().message();
        msg += ") occurred in routine: writeDataPDU";
        return makeDcmnetCondition(DULC_TCPIOERROR, OF_error, msg.c_str());
    }

    /* return ok */
    return EC_Normal;
}

/* closeTransport
**
** Purpose:
**      Close the transport connection.
**
** Parameter Dictionary:
**
**      association     Handle to the Association
**
** Return Values:
**      None.
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static void
closeTransport(PRIVATE_ASSOCIATIONKEY ** association)
{
    closeTransportTCP(association);
}

/* closeTransportTCP
**
** Purpose:
**      Close the TCP transport connection.
**
** Parameter Dictionary:
**
**      association     Handle to the Association
**
** Return Values:
**      None
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static void
closeTransportTCP(PRIVATE_ASSOCIATIONKEY ** association)
{
  if ((*association)->connection)
  {
   (*association)->connection->close();
   delete (*association)->connection;
   (*association)->connection = NULL;
  }
}


/* clearPDUCache()
**
** Purpose:
**      Clear the Association of any PDUs.
**
** Parameter Dictionary:
**
**      association     Handle to the Association
**
** Return Values:
**      None.
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static void
clearPDUCache(PRIVATE_ASSOCIATIONKEY ** association)
{
    (*association)->inputPDU = NO_PDU;
}

/* PRV_NextPDUType
**
** Purpose:
**      Get the next PDU's type.
**
** Parameter Dictionary:
**
**      association     Handle to the Association
**      block           Option for blocking/non-blocking
**      timeout         Timeout interval within to get the Type of the next
**                      PDU
**      pduType         The type of next PDU returned to caller.
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
PRV_NextPDUType(PRIVATE_ASSOCIATIONKEY ** association, DUL_BLOCKOPTIONS block,
                int timeout, unsigned char *pduType)
{
    OFCondition cond = EC_Normal;

    /* if the association does not contain PDU header information, do something */
    if ((*association)->inputPDU == NO_PDU) {
        /* try to receive a PDU header on the network */
        cond = readPDUHead(association, (*association)->pduHead,
                           sizeof((*association)->pduHead),
                           block, timeout, &(*association)->nextPDUType,
                           &(*association)->nextPDUReserved,
                           &(*association)->nextPDULength);
        /* if receiving was not successful, return this error */
        if (cond.bad())
            return cond;
        /* indicate that the association does contain PDU header information */
        (*association)->inputPDU = PDU_HEAD;
    }
    /* determine PDU type and assign it to reference parameter */
    *pduType = (*association)->nextPDUType;

    /* return ok */
    return EC_Normal;
}

/* readPDU
**
** Purpose:
**      Read the PDU from the incoming stream.
**
** Parameter Dictionary:
**      association     Handle to the Association
**      block           Blocking/non-blocking options for reading
**      timeout         Timeout interval for reading
**      buffer          Buffer holding the PDU (returned to the caller)
**      PDUType         Type of the PDU (returned to the caller)
**      PDUReserved     Reserved field of the PDU (returned to caller)
**      PDULength       Length of PDU read (returned to caller)
**
**
** Return Values:
**
**
** Notes:
**    The buffer is allocated (malloc) when the PDU size is known.
**    If malloc fails, EC_MemoryExhausted is returned.
**    Otherwise, the buffer must be released (free) by the caller!
**
**    This function is only used to receive incoming A-ASSOCIATE-RQ
**    and A-ASSOCIATE-AC PDUs.
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
readPDU(PRIVATE_ASSOCIATIONKEY ** association, DUL_BLOCKOPTIONS block,
        int timeout, unsigned char **buffer,
        unsigned char *pduType, unsigned char *pduReserved,
        unsigned long *pduLength)
{
    OFCondition cond = EC_Normal;
    unsigned long maxLength;

    *buffer = NULL;
    if ((*association)->inputPDU == NO_PDU) {
        cond = readPDUHead(association, (*association)->pduHead,
                           sizeof((*association)->pduHead),
                           block, timeout, &(*association)->nextPDUType,
                           &(*association)->nextPDUReserved,
                           &(*association)->nextPDULength);
        if (cond.bad())
            return cond;
        (*association)->inputPDU = PDU_HEAD;
    }

    size_t limit = dcmAssociatePDUSizeLimit.get();
    if ((limit > 0) && ((*association)->nextPDULength > limit))
    {
      DCMNET_ERROR("A-ASSOCIATE PDU too large: " << (*association)->nextPDULength << " bytes, refusing." );
      return NET_EC_AssociatePDUTooLarge;
    }

    maxLength = ((*association)->nextPDULength)+100;
    *buffer = (unsigned char *)malloc(size_t(maxLength));
    if (*buffer)
    {
      (void) memcpy(*buffer, (*association)->pduHead, sizeof((*association)->pduHead));
      cond = readPDUBody(association, block, timeout,
        (*buffer) + sizeof((*association)->pduHead),
        maxLength - sizeof((*association)->pduHead),
        pduType, pduReserved, pduLength);
    } else cond = EC_MemoryExhausted;
    return cond;
}


/* readPDUHead
**
** Purpose:
**      Read the header from the PDU. The type of protocol is obtained from
**      the Association handle.
**
** Parameter Dictionary:
**      association     Handle to the Association
**      buffer          Buffer holding the PDU
**      maxLength       Maximum allowable length of the header
**      block           Blocking/non-blocking options for reading
**      timeout         Timeout interval for reading
**      PDUType         Type of the PDU (returned to the caller)
**      PDUReserved     Reserved field of the PDU (returned to caller)
**      PDULength       Length of PDU header read (returned to caller)
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
readPDUHead(PRIVATE_ASSOCIATIONKEY ** association,
            unsigned char *buffer, unsigned long maxLength,
            DUL_BLOCKOPTIONS block, int timeout,
            unsigned char *PDUType, unsigned char *PDUReserved,
            unsigned long *PDULength)
{
    /* initialize return value */
    OFCondition cond = EC_Normal;

    /* if the association does not already contain PDU header */
    /* information, we need to try to receive a PDU on the network */
    if ((*association)->inputPDU == NO_PDU)
    {
        /* try to receive data */
        cond = readPDUHeadTCP(association, buffer, maxLength, block, timeout,
             &(*association)->nextPDUType, &(*association)->nextPDUReserved, &(*association)->nextPDULength);
    }
    /* if everything was ok */
    if (cond.good()) {
        /* indicate that the association does contain PDU header information */
        (*association)->inputPDU = PDU_HEAD;
        /* determine PDU type and the PDU's value in the reserved field and */
        /* in the PDU length field and assign it to reference parameter */
        *PDUType = (*association)->nextPDUType;
        *PDUReserved = (*association)->nextPDUReserved;
        *PDULength = (*association)->nextPDULength;

        /* check if the value in the length field of the PDU shows a legal value; */
        /* there is a maximum length for PDUs which shall be sent over the network. */
        /* the length of this PDU must not be greater than the specified maximum length. */
        /* (bugfix - thanks to B. Gorissen, Philips Medical Systems) */
        if ((*PDUType == DUL_TYPEDATA) && (*PDULength > (*association)->maxPDVInput))
        {
          char buf1[256];
          sprintf(buf1, "DUL Illegal PDU Length %ld.  Max expected %ld", *PDULength, (*association)->maxPDVInput);
          cond = makeDcmnetCondition(DULC_ILLEGALPDULENGTH, OF_error, buf1);
        }
    }

    /* return result value */
    return cond;
}


/* readPDUBody
**
** Purpose:
**      Read the body of the incoming PDU.
**
** Parameter Dictionary:
**      association     Handle to the Association
**      block           For blocking/non-blocking read
**      timeout         Timeout interval for reading
**      buffer          Buffer to hold the PDU
**      maxLength       MAximum number of bytes to read
**      pduType         PDU Type of the incoming PDU (returned to caller)
**      pduReserved     Reserved field in the PDU
**      pduLength       Actual number of bytes read
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
readPDUBody(PRIVATE_ASSOCIATIONKEY ** association,
            DUL_BLOCKOPTIONS block, int timeout,
            unsigned char *buffer, unsigned long maxLength,
            unsigned char *pduType, unsigned char *pduReserved,
            unsigned long *pduLength)
{
    OFCondition cond = EC_Normal;

    /* read PDU body information */
    cond = readPDUBodyTCP(association, block, timeout, buffer, maxLength,
                          pduType, pduReserved, pduLength);

    /* indicate that the association does not contain PDU information any more */
    /* (in detail we indicate that the association can be used to store new PDU */
    /* information, since the current PDU's information is available through */
    /* the variables pduType, pduReserved, pduLength, and buffer now. */
    (*association)->inputPDU = NO_PDU;

    /* return return value */
    return cond;
}

/* readPDUHeadTCP
**
** Purpose:
**      Read the TCP header.
**
** Parameter Dictionary:
**      association     Handle to the Association
**      buffer          Buffer containing the header
**      maxLength       Maximum length of the header
**      block           For blocking/non-blocking read
**      timeout         Timeout interval for reading
**      type            One of the many DUL PDU types
**      reserved        For reserved field
**      pduLength       Length of the header finally read in.
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/


static OFCondition
readPDUHeadTCP(PRIVATE_ASSOCIATIONKEY ** association,
               unsigned char *buffer, unsigned long maxLength,
               DUL_BLOCKOPTIONS block, int timeout,
     unsigned char *type, unsigned char *reserved, unsigned long *pduLength)
{
    unsigned long
        length;
    static unsigned char
        legalPDUTypes[] = {
        DUL_TYPEASSOCIATERQ, DUL_TYPEASSOCIATEAC,
        DUL_TYPEASSOCIATERJ, DUL_TYPEDATA,
        DUL_TYPERELEASERQ, DUL_TYPERELEASERP,
        DUL_TYPEABORT
    };
    int
        found;
    unsigned long
        idx;

    /* check if the buffer is too small to capture the PDU head we are about to receive */
    if (maxLength < 6)
    {
        return makeDcmnetCondition(DULC_CODINGERROR, OF_error, "Coding Error in DUL routine: buffer too small in readPDUTCPHead");
    }

    /* (for non-blocking reading) if the timeout refers to */
    /* the default timeout, set timeout correspondingly */
    if (timeout == PRV_DEFAULTTIMEOUT)
        timeout = (*association)->timeout;

    /* try to receive PDU header (6 bytes) over the network, mind blocking */
    /* options; in the end, buffer will contain the 6 bytes that were read. */
    OFCondition cond = defragmentTCP((*association)->connection, block, (*association)->timerStart, timeout, buffer, 6, &length);

    /* if receiving was not successful, return the corresponding error value */
    if (cond.bad()) return cond;

    DCMNET_TRACE("Read PDU HEAD TCP:" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
              << " " << STD_NAMESPACE setw(2) << (unsigned short)(buffer[0])
              << " " << STD_NAMESPACE setw(2) << (unsigned short)(buffer[1])
              << " " << STD_NAMESPACE setw(2) << (unsigned short)(buffer[2])
              << " " << STD_NAMESPACE setw(2) << (unsigned short)(buffer[3])
              << " " << STD_NAMESPACE setw(2) << (unsigned short)(buffer[4])
              << " " << STD_NAMESPACE setw(2) << (unsigned short)(buffer[5]));

    /* determine PDU type (captured in byte 0 of buffer) and assign it to reference parameter */
    *type = *buffer++;

    /* determine value in the PDU header's reserved field (captured */
    /* in byte 1 of buffer) and assign it to reference parameter */
    *reserved = *buffer++;

    /* check if the PDU which was received shows a legal PDU type */
    for (idx = found = 0; !found && idx < sizeof(legalPDUTypes); idx++) {
        found = (*type == legalPDUTypes[idx]);
    }

    /* if the PDU's type was not legal, return a corresponding message */
    if (!found)
    {
        char buf[256];
        sprintf(buf, "Unrecognized PDU type: %2x", *type);
        return makeDcmnetCondition(DULC_UNRECOGNIZEDPDUTYPE, OF_error, buf);
    }

    /* determine the value in the PDU length field of the PDU */
    /* which was received and assign it to reference parameter */
    EXTRACT_LONG_BIG(buffer, length);
    buffer += 4;
    *pduLength = length;

    DCMNET_TRACE("Read PDU HEAD TCP: type: "
              << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(2) << (unsigned short)(*type)
              << ", length: " << STD_NAMESPACE dec << (*pduLength) << " ("
              << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(2) << (unsigned int)*pduLength
              << ")");

    /* return ok */
    return EC_Normal;
}


/* readPDUBodyTCP
**
** Purpose:
**      Read the PDU from a TCP socket interface.
**
** Parameter Dictionary:
**      association     Handle to the Association
**      block           For blocking/non-blocking read
**      timeout         Timeout interval for reading
**      buffer          Buffer to hold the PDU
**      maxLength       Maximum number of bytes to read
**      pduType         PDU Type of the incoming PDU (returned to caller)
**      pduReserved     Reserved field in the PDU
**      pduLength       Actual number of bytes read
**
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
readPDUBodyTCP(PRIVATE_ASSOCIATIONKEY ** association,
               DUL_BLOCKOPTIONS block, int timeout,
               unsigned char *buffer, unsigned long maxLength,
               unsigned char *pduType, unsigned char *pduReserved,
               unsigned long *pduLength)
{
    OFCondition cond = EC_Normal;
    unsigned long
        length;

    /* check if the association does not already contain PDU head information. */
    if ((*association)->inputPDU == NO_PDU) {
        /* If it does not, we need to try to receive PDU head information over the network */
        cond = readPDUHead(association, (*association)->pduHead,
                           sizeof((*association)->pduHead),
                           block, timeout, &(*association)->nextPDUType,
                           &(*association)->nextPDUReserved,
                           &(*association)->nextPDULength);
        /* return error if there was one */
        if (cond.bad())
            return cond;
    }
    /* determine PDU type and its values in the reserved field and in the PDU length field */
    *pduType = (*association)->nextPDUType;
    *pduReserved = (*association)->nextPDUReserved;
    *pduLength = (*association)->nextPDULength;

    /* (for non-blocking reading) if the timeout refers to */
    /* the default timeout, set timeout correspondingly */
    if (timeout == PRV_DEFAULTTIMEOUT)
        timeout = (*association)->timeout;

    /* bugfix by meichel 97/04/28: test if PDU fits into buffer */
    if (*pduLength > maxLength)
    {
      /* if it doesn't, set error indicator variable */
      cond = DUL_ILLEGALPDULENGTH;
    } else {
      /* if it does, read the current PDU's PDVs from the incoming socket stream. (Note that the */
      /* PDVs of the current PDU are (*association)->nextPDULength bytes long. Hence, in detail */
      /* we want to try to receive (*association)->nextPDULength bytes of data on the network) */
      /* The information that was received will be available through the buffer variable. */
      cond = defragmentTCP((*association)->connection,
                         block, (*association)->timerStart, timeout,
                         buffer, (*association)->nextPDULength, &length);
    }

    /* return result value */
    return cond;
}


/* defragmentTCP
**
** Purpose:
**      Actually read the desired number of bytes  of the PDU from the
**      incoming socket stream.
**
** Parameter Dictionary:
**      sock            Socket descriptor
**      block           Blocking/non-blocking option
**      timerStart      Time at which the reading operation is started.
**      timeout         Timeout interval for reading
**      p               Buffer to hold the information
**      l               Maximum number of bytes to read
**      rtnLength       Actual number of bytes that were read (returned
**                      to the caller)
**
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/


static OFCondition
defragmentTCP(DcmTransportConnection *connection, DUL_BLOCKOPTIONS block, time_t timerStart,
              int timeout, void *p, unsigned long l, unsigned long *rtnLen)
{
    unsigned char *b;
    int bytesRead;

    /* assign buffer to local variable */
    b = (unsigned char *) p;

    /* if reference parameter is a valid pointer, initialize its value */
    if (rtnLen != NULL)
        *rtnLen = 0;

    /* if there is no network connection, return an error */
    if (connection == NULL) return DUL_NULLKEY;

    int timeToWait = 0;
    if (block == DUL_NOBLOCK)
    {
        /* figure out how long we want to wait: if timerStart equals 0 we want to wait exactly */
        /* timeout seconds starting from the call to select(...) within the below called function; */
        /* if timerStart does not equal 0 we want to subtract the time which has already passed */
        /* after the timer was started from timeout and wait the resulting amount of seconds */
        /* starting from the call to select(...) within the below called function. */
        if (timerStart == 0) timerStart = time(NULL);
    }

    /* start a loop: since we want to receive l bytes of data over the network, */
    /* we won't stop waiting for data until we actually did receive l bytes. */
    while (l > 0)
    {
        /* receive data from the network connection; wait until */
        /* we actually did receive data or an error occurred */
        do
        {
            /* if DUL_NOBLOCK is specified as a blocking option, we only want to wait a certain
             * time for receiving data over the network. If no data was received during that time,
             * DUL_READTIMEOUT shall be returned. Note that if DUL_BLOCK is specified the application
             * will not stop waiting until data is actually received over the network.
             */
            if (block == DUL_NOBLOCK)
            {
                /* determine remaining time to wait */
                timeToWait = timeout - (int) (time(NULL) - timerStart);

                /* go ahead and see if within timeout seconds data will be received over the network. */
                /* if not, return DUL_READTIMEOUT, if yes, stay in this function. */
                if (!connection->networkDataAvailable(timeToWait)) return DUL_READTIMEOUT;
            }

            /* data has become available, now call read(). */
            bytesRead = connection->read((char*)b, size_t(l));

        } while (bytesRead == -1 && OFStandard::getLastNetworkErrorCode().value() == DCMNET_EINTR);

        /* if we actually received data, move the buffer pointer to its own end, update the variable */
        /* that determines the end of the first loop, and update the reference parameter return variable */
        if (bytesRead > 0) {
            b += bytesRead;
            l -= (unsigned long) bytesRead;
            if (rtnLen != NULL)
                *rtnLen += (unsigned long) bytesRead;
        } else {
            /* in case we did not receive any data, an error must have occurred; return a corresponding result value */
            return DUL_NETWORKCLOSED;
        }
    }
    return EC_Normal;
}

/* dump_pdu
**
** Purpose:
**      Debugging routine for dumping a pdu
**
** Parameter Dictionary:
**      type            PDU type
**      buffer          Buffer holding the PDU
**      length          Length of the PDU
**
** Return Values:
**      None
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFString
dump_pdu(const char *type, void *buffer, unsigned long length)
{
    unsigned char
       *p;
    int
        position = 0;
    OFOStringStream
        str;

    str << "PDU Type: " << type << ", PDU Length: " << length-6 << " + 6 bytes PDU header" << OFendl;
    if (length > 512) {
        str << "Only dumping 512 bytes." << OFendl;
        length = 512;
    }
    p = (unsigned char*)buffer;

    while (length-- > 0) {
        str << "  "
            << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(2) << ((unsigned int)(*p++))
            << STD_NAMESPACE dec;
        if ((++position) % 16 == 0) str << OFendl;
    }
    str << OFStringStream_ends;
    OFSTRINGSTREAM_GETOFSTRING(str, ret)
    return ret;
}



/* setTCPBufferLength
**
** Purpose:
**      This routine checks for the existence of an environment
**      variable (TCP_BUFFER_LENGTH).  If that variable is defined (and
**      is a legal integer), this routine sets the socket SNDBUF and RCVBUF
**      variables to the value defined in TCP_BUFFER_LENGTH.
**
** Parameter Dictionary:
**      sock            Socket descriptor (identifier)
**
** Return Values:
**      None
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

#ifdef _WIN32
static void setTCPBufferLength(SOCKET sock)
#else
static void setTCPBufferLength(int sock)
#endif
{
    char *TCPBufferLength;
    int bufLen;

    /*
     * check whether environment variable TCP_BUFFER_LENGTH is set.
     * If not, the the operating system is responsible for selecting
     * appropriate values for the TCP send and receive buffer lengths.
     */
    DCMNET_TRACE("checking whether environment variable TCP_BUFFER_LENGTH is set");
    if ((TCPBufferLength = getenv("TCP_BUFFER_LENGTH")) != NULL) {
        if (sscanf(TCPBufferLength, "%d", &bufLen) == 1) {
#if defined(SO_SNDBUF) && defined(SO_RCVBUF)
            if (bufLen == 0)
                bufLen = 65536; // a socket buffer size of 64K gives good throughput for image transmission
            DCMNET_DEBUG("DULFSM: setting TCP buffer length to " << bufLen << " bytes");
            (void) setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *) &bufLen, sizeof(bufLen));
            (void) setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *) &bufLen, sizeof(bufLen));
#else
            DCMNET_WARN("DULFSM: setTCPBufferLength: cannot set TCP buffer length socket option: "
                << "code disabled because SO_SNDBUF and SO_RCVBUF constants are unknown");
#endif // SO_SNDBUF and SO_RCVBUF
        } else
            DCMNET_WARN("DULFSM: cannot parse environment variable TCP_BUFFER_LENGTH=" << TCPBufferLength);
    } else
        DCMNET_TRACE("  environment variable TCP_BUFFER_LENGTH not set, using the system defaults");
}

/* translatePresentationContextList
**
** Purpose:
**      Translate the internal list into a user context list and a
**      SCU-SCP role list
**
** Parameter Dictionary:
**      internalList            Input list from which the two output lists
**                              are derived
**      SCUSCPRoleList          Role list (returned to the caller)
**      userContextList         User context list (returned to the caller)
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
OFCondition
translatePresentationContextList(LST_HEAD ** internalList,
                                 LST_HEAD ** SCUSCPRoleList,
                                 LST_HEAD ** userContextList)
{
    PRV_PRESENTATIONCONTEXTITEM
        * context;
    DUL_PRESENTATIONCONTEXT
        * userContext;
    DUL_SUBITEM
        * subItem;
    DUL_TRANSFERSYNTAX
        * transfer;
    PRV_SCUSCPROLE
        * scuscpRole;
    OFCondition cond = EC_Normal;

    context = (PRV_PRESENTATIONCONTEXTITEM*)LST_Head(internalList);
    (void) LST_Position(internalList, (LST_NODE*)context);
    while (context != NULL) {
        userContext = (DUL_PRESENTATIONCONTEXT*)malloc(sizeof(DUL_PRESENTATIONCONTEXT));
        if (userContext == NULL) return EC_MemoryExhausted;
        if ((userContext->proposedTransferSyntax = LST_Create()) == NULL) return EC_MemoryExhausted;

        userContext->acceptedTransferSyntax[0] = '\0';
        userContext->presentationContextID = context->contextID;
        OFStandard::strlcpy(userContext->abstractSyntax, context->abstractSyntax.data, sizeof(userContext->abstractSyntax));
        userContext->proposedSCRole = DUL_SC_ROLE_DEFAULT;
        userContext->acceptedSCRole = DUL_SC_ROLE_DEFAULT;

        scuscpRole = findSCUSCPRole(SCUSCPRoleList,
                                    userContext->abstractSyntax);
        if (scuscpRole != NULL) {
            if (scuscpRole->SCURole == scuscpRole->SCPRole) {
                userContext->proposedSCRole = DUL_SC_ROLE_SCUSCP;
                if (scuscpRole->SCURole == 0)
                    DCMNET_WARN("DULFSM: both role fields are 0 in SCP/SCU role selection sub-item");
            }
            else if (scuscpRole->SCURole == 1)
                userContext->proposedSCRole = DUL_SC_ROLE_SCU;
            else  // SCPRole == 1
                userContext->proposedSCRole = DUL_SC_ROLE_SCP;
        }
        subItem = (DUL_SUBITEM*)LST_Head(&context->transferSyntaxList);
        if (subItem == NULL)
        {
            char buf1[256];
            sprintf(buf1, "DUL Peer supplied illegal number of transfer syntaxes (%d)", 0);
            free(userContext);
            return makeDcmnetCondition(DULC_PEERILLEGALXFERSYNTAXCOUNT, OF_error, buf1);
        }
        (void) LST_Position(&context->transferSyntaxList, (LST_NODE*)subItem);
        while (subItem != NULL) {
            transfer = (DUL_TRANSFERSYNTAX*)malloc(sizeof(DUL_TRANSFERSYNTAX));
            if (transfer == NULL) return EC_MemoryExhausted;
            OFStandard::strlcpy(transfer->transferSyntax, subItem->data, sizeof(transfer->transferSyntax));

            LST_Enqueue(&userContext->proposedTransferSyntax, (LST_NODE*)transfer);
            subItem = (DUL_SUBITEM*)LST_Next(&context->transferSyntaxList);
        }
        LST_Enqueue(userContextList, (LST_NODE*)userContext);
        context = (PRV_PRESENTATIONCONTEXTITEM*)LST_Next(internalList);
    }
    return EC_Normal;
}

/* findPresentationCtx
**
** Purpose:
**      Find the requested presentation context using the contextID as the
**      key
**
** Parameter Dictionary:
**      list            List to be searched
**      contextID       The search key
**
** Return Values:
**      A presentation context list (if found) else NULL.
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
DUL_PRESENTATIONCONTEXT *
findPresentationCtx(
                    LST_HEAD ** lst, DUL_PRESENTATIONCONTEXTID contextID)
{
    DUL_PRESENTATIONCONTEXT
    * ctx;

    if ((ctx = (DUL_PRESENTATIONCONTEXT*)LST_Head(lst)) == NULL)
        return NULL;

    (void) LST_Position(lst, (LST_NODE*)ctx);
    while (ctx != NULL) {
        if (ctx->presentationContextID == contextID)
            return ctx;

        ctx = (DUL_PRESENTATIONCONTEXT*)LST_Next(lst);
    }
    return NULL;
}


/* findSCUSCPRole
**
** Purpose:
**      Search for a SCUSCP role list, given the abstract syntax as the
**      key.
**
** Parameter Dictionary:
**      list            List to be searched
**      abstractSyntax  The search key
**
** Return Values:
**      The role list, if found, else NULL
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
PRV_SCUSCPROLE
*
findSCUSCPRole(LST_HEAD ** lst, char *abstractSyntax)
{
    PRV_SCUSCPROLE
        * role;

    role = (PRV_SCUSCPROLE*)LST_Head(lst);
    if (role != NULL)
        (void) LST_Position(lst, (LST_NODE*)role);

    while (role != NULL) {
        if (strcmp(role->SOPClassUID, abstractSyntax) == 0)
            return role;

        role = (PRV_SCUSCPROLE*)LST_Next(lst);
    }
    return NULL;
}

void
destroyPresentationContextList(LST_HEAD ** l)
{
    PRV_PRESENTATIONCONTEXTITEM
    * prvCtx;
    DUL_SUBITEM
        * subItem;

    if (*l == NULL)
        return;

    prvCtx = (PRV_PRESENTATIONCONTEXTITEM*)LST_Dequeue(l);
    while (prvCtx != NULL) {
        subItem = (DUL_SUBITEM*)LST_Dequeue(&prvCtx->transferSyntaxList);
        while (subItem != NULL) {
            free(subItem);
            subItem = (DUL_SUBITEM*)LST_Dequeue(&prvCtx->transferSyntaxList);
        }
        LST_Destroy(&prvCtx->transferSyntaxList);
        free(prvCtx);
        prvCtx = (PRV_PRESENTATIONCONTEXTITEM*)LST_Dequeue(l);
    }
    LST_Destroy(l);
}

void
destroyUserInformationLists(DUL_USERINFO * userInfo)
{
    PRV_SCUSCPROLE
    * role;

    role = (PRV_SCUSCPROLE*)LST_Dequeue(&userInfo->SCUSCPRoleList);
    while (role != NULL) {
        free(role);
        role = (PRV_SCUSCPROLE*)LST_Dequeue(&userInfo->SCUSCPRoleList);
    }
    LST_Destroy(&userInfo->SCUSCPRoleList);

    /* extended negotiation */
    delete userInfo->extNegList; userInfo->extNegList = NULL;

    /* user identity negotiation */
    delete userInfo->usrIdent; userInfo->usrIdent = NULL;
}
