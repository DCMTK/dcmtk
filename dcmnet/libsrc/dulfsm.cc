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
/* Copyright marker.  Copyright will be inserted above.  Do not remove */
/*
** @$=@$=@$=
*/
/*
**				DICOM 93
**		     Electronic Radiology Laboratory
**		   Mallinckrodt Institute of Radiology
**		Washington University School of Medicine
**
** Module Name(s):	DUL_InitializeFSM
**			PRV_StateMachine
**			fsmDebug
**
** Author, Date:	Stephen M. Moore, 15-Apr-93
** Intent:		Define tables and provide functions that implement
**			the DICOM Upper Layer (DUL) finite state machine.
** Last Update:		$Author: hewett $, $Date: 1996-09-24 16:22:46 $
** Source File:		$RCSfile: dulfsm.cc,v $
** Revision:		$Revision: 1.6 $
** Status:		$State: Exp $
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <signal.h>
#include <time.h>

#ifdef HAVE_GUSI_H
#include <GUSI.h>	/* Use the Grand Unified Sockets Interface (GUSI) on Macintosh */
#endif

#include "dicom.h"
#include "lst.h"
#include "cond.h"
#include "dul.h"
#include "dulstruc.h"
#include "dulpriv.h"
#include "dulfsm.h"
#ifdef BLOG
#include "blg.h"
#endif

static CTNBOOLEAN debug = FALSE;
#ifdef BLOG
static CTNBOOLEAN blog = FALSE;
#endif

static CONDITION
AE_1_TransportConnect(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AE_2_SendAssociateRQPDU(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AE_3_AssociateConfirmationAccept(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AE_4_AssociateConfirmationReject(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AE_5_TransportConnectResponse(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AE_6_ExamineAssociateRequest(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AE_7_SendAssociateAC(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AE_8_SendAssociateRJ(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);

static CONDITION
DT_1_SendPData(PRIVATE_NETWORKKEY ** network,
	 PRIVATE_ASSOCIATIONKEY ** associatin, int nextState, void *params);
static CONDITION
DT_2_IndicatePData(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);

static CONDITION
AA_1_SendAAbort(PRIVATE_NETWORKKEY ** network,
	 PRIVATE_ASSOCIATIONKEY ** associatin, int nextState, void *params);
static CONDITION
AA_2_CloseTransport(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AA_2_CloseTimeout(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AA_3_IndicatePeerAborted(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AA_4_IndicateAPAbort(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AA_5_StopARTIMtimer(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AA_6_IgnorePDU(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AA_7_State13SendAbort(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AA_8_UnrecognizedPDUSendAbort(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);

static CONDITION
AR_1_SendReleaseRQ(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AR_2_IndicateRelease(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AR_3_ConfirmRelease(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AR_4_SendReleaseRP(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AR_5_StopARTIMtimer(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AR_6_IndicatePData(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AR_7_SendPDATA(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AR_8_IndicateARelease(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AR_9_SendAReleaseRP(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);
static CONDITION
AR_10_ConfirmRelease(PRIVATE_NETWORKKEY ** network,
	PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params);

static CONDITION
requestAssociationTCP(PRIVATE_NETWORKKEY ** network,
		      DUL_ASSOCIATESERVICEPARAMETERS * params,
		      PRIVATE_ASSOCIATIONKEY ** association);
static CONDITION
sendAssociationRQTCP(PRIVATE_NETWORKKEY ** network,
		     DUL_ASSOCIATESERVICEPARAMETERS * params,
		     PRIVATE_ASSOCIATIONKEY ** association);
static CONDITION
sendAssociationACTCP(PRIVATE_NETWORKKEY ** network,
		     DUL_ASSOCIATESERVICEPARAMETERS * params,
		     PRIVATE_ASSOCIATIONKEY ** association);
static CONDITION
sendAssociationRJTCP(PRIVATE_NETWORKKEY ** network,
	DUL_ABORTITEMS * abortItems, PRIVATE_ASSOCIATIONKEY ** association);
static CONDITION
sendAbortTCP(DUL_ABORTITEMS * abortItems,
	     PRIVATE_ASSOCIATIONKEY ** association);
static CONDITION sendReleaseRQTCP(PRIVATE_ASSOCIATIONKEY ** association);
static CONDITION sendReleaseRPTCP(PRIVATE_ASSOCIATIONKEY ** association);
static CONDITION
sendPDataTCP(PRIVATE_ASSOCIATIONKEY ** association,
	     DUL_PDVLIST * pdvList);
static CONDITION
writeDataPDU(PRIVATE_ASSOCIATIONKEY ** association,
	     DUL_DATAPDU * pdu);
static void clearPDUCache(PRIVATE_ASSOCIATIONKEY ** association);
static void closeTransport(PRIVATE_ASSOCIATIONKEY ** association);
static void closeTransportTCP(PRIVATE_ASSOCIATIONKEY ** association);
static CONDITION
readPDUHead(PRIVATE_ASSOCIATIONKEY ** association,
	    unsigned char *buffer, unsigned long maxlength,
	    DUL_BLOCKOPTIONS block, int timeout,
	    unsigned char *PDUtype, unsigned char *PDUreserved,
	    unsigned long *PDULength);
static CONDITION
readPDU(PRIVATE_ASSOCIATIONKEY ** association, DUL_BLOCKOPTIONS block,
	int timeout, unsigned char *buffer, unsigned long maxLength,
	unsigned char *pduType, unsigned char *pduReserved,
	unsigned long *pduLength);
static CONDITION
readPDUBody(PRIVATE_ASSOCIATIONKEY ** association,
	    DUL_BLOCKOPTIONS block, int timeout,
	    unsigned char *buffer, unsigned long maxLength,
	    unsigned char *pduType, unsigned char *pduReserved,
	    unsigned long *pduLength);
static CONDITION
readPDUHeadTCP(PRIVATE_ASSOCIATIONKEY ** association,
	       unsigned char *buffer, unsigned long maxLength,
	       DUL_BLOCKOPTIONS block, int timeout,
	       unsigned char *PDUtype, unsigned char *PDUreserved,
	       unsigned long *PDULength);
static CONDITION
readPDUBodyTCP(PRIVATE_ASSOCIATIONKEY ** association,
	       DUL_BLOCKOPTIONS block, int timeout,
	       unsigned char *buffer, unsigned long maxLength,
	       unsigned char *pduType, unsigned char *pduReserved,
	       unsigned long *pduLength);
static CONDITION
defragmentTCP(int socket, DUL_BLOCKOPTIONS block, time_t timerStart,
	      int timeout, void *b, unsigned long l, unsigned long *rtnLen);
static CTNBOOLEAN networkDataAvailable(int s, int timeout);
#ifdef DEBUG
static void recordOutGoing(void *buf, unsigned long length);
#endif
static void dump_pdu(char *type, void *buffer, unsigned long length);
#ifdef DUMP_DATA_PDU
static void dump_data(void *buffer, unsigned long length);
#endif
static void setTCPBufferLength(int sock);
CONDITION
translatePresentationContextList(LST_HEAD ** internalList,
				 LST_HEAD ** SCUSCPRoleList,
				 LST_HEAD ** userContextList);
DUL_PRESENTATIONCONTEXT *
findPresentationCtx(
		    LST_HEAD ** list, DUL_PRESENTATIONCONTEXTID contextID);
PRV_SCUSCPROLE
* findSCUSCPRole(LST_HEAD ** list, char *abstractSyntax);

void destroyPresentationContextList(LST_HEAD ** l);
void destroyUserInformationLists(DUL_USERINFO * userInfo);

static FSM_Event_Description Event_Table[] = {
    {A_ASSOCIATE_REQ_LOCAL_USER, "A-ASSOCIATE request (local user)"},
    {TRANS_CONN_CONFIRM_LOCAL_USER, "Transport conn confirmation (local)"},
    {A_ASSOCIATE_AC_PDU_RCV, "A-ASSOCIATE-AC PDU (on transport)"},
    {A_ASSOCIATE_RJ_PDU_RCV, "A-ASSOCIATE-RJ PDU (on transport)"},
    {TRANS_CONN_INDICATION, "Transport connection indication"},
    {A_ASSOCIATE_RQ_PDU_RCV, "A-ASSOCIATE-RQ PDU (on tranport)"},
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
    {AE_3, AE_3_AssociateConfirmationAccept,
    "AE 3 Associate Confirmation Accept"},
    {AE_4, AE_4_AssociateConfirmationReject,
    "AE 4 Associate Confirmation Reject"},
    {AE_5, AE_5_TransportConnectResponse,
    "AE 5 Transport Connect Response"},
    {AE_6, AE_6_ExamineAssociateRequest,
    "AE 6 Examine Associate Request"},
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
    {AA_8, AA_8_UnrecognizedPDUSendAbort,
    "AA 8 Unrecognized PDU Send Abort"},

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
	{A_ASSOCIATE_RQ_PDU_RCV, STATE5, AE_4, STATE1, "", "", NULL},
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
	{ARTIM_TIMER_EXPIRED, STATE2, AA_2, STATE1, "", "", NULL},
	{ARTIM_TIMER_EXPIRED, STATE3, NOACTION, NOSTATE, "", "", NULL},
	{ARTIM_TIMER_EXPIRED, STATE4, NOACTION, NOSTATE, "", "", NULL},
	{ARTIM_TIMER_EXPIRED, STATE5, NOACTION, NOSTATE, "", "", NULL},
	{ARTIM_TIMER_EXPIRED, STATE6, NOACTION, NOSTATE, "", "", NULL},
/* This next line is not per the standard.  We added a timeout action
** in this state.
*/
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
**	Initialize the DUL finite state machine by filling in addresses of
**	functions.
**
** Parameter Dictionary:
**	None
**
** Return Values:
**	DUL_NORMAL
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
CONDITION
DUL_InitializeFSM()
{
    unsigned long
        index,
        idx2;
    FSM_ENTRY
	* stateEntries;

    stateEntries = (FSM_ENTRY *) StateTable;
    for (index = 0; index < DUL_NUMBER_OF_EVENTS * DUL_NUMBER_OF_STATES; index++) {
	if (stateEntries[index].action != NOACTION) {
	    for (idx2 = 0; idx2 < DIM_OF(FSM_FunctionTable) &&
		 stateEntries[index].actionFunction == NULL; idx2++)
		if (stateEntries[index].action == FSM_FunctionTable[idx2].action) {
		    stateEntries[index].actionFunction =
			FSM_FunctionTable[idx2].actionFunction;
		    (void) sprintf(stateEntries[index].actionName, "%.*s",
				 (int)(sizeof(stateEntries[index].actionName) - 1),
				   FSM_FunctionTable[idx2].actionName);
		}
	}
	for (idx2 = 0; idx2 < DIM_OF(Event_Table) &&
	     strlen(stateEntries[index].eventName) == 0; idx2++) {
	    if (stateEntries[index].event == Event_Table[idx2].event)
		(void) sprintf(stateEntries[index].eventName, "%.*s",
			       (int)(sizeof(stateEntries[index].eventName) - 1),
			       Event_Table[idx2].eventName);
	}
    }

    return DUL_NORMAL;
}



/* PRV_StateMachine
**
** Purpose:
**	Execute the action function, given the current state and the event.
**
** Parameter Dictionary:
**	network		Handle to the network environment
**	association	Handle to the Association
**	event		The event that will trigger this action
**	state		Current state of the finite state machine.
**	params		Service parameters describing this Association
**
** Return Values:
**
**	DUL_FSMERROR
**	DUL_NORMAL
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
CONDITION
PRV_StateMachine(PRIVATE_NETWORKKEY ** network,
		 PRIVATE_ASSOCIATIONKEY ** association, int event, int state,
		 void *params)
{
    FSM_ENTRY
	* entry;

    if (event < 0 || event >= DUL_NUMBER_OF_EVENTS) {
	(void) fprintf(stderr, "Bad event: %d\n", event);
	(void) fprintf(stderr, "This must be a coding error of some kind. \n");
	return COND_PushCondition(DUL_FSMERROR, DUL_Message(DUL_FSMERROR),
				  state, event);
    }
    if (state < 1 || state > DUL_NUMBER_OF_STATES) {
	(void) fprintf(stderr, "Bad state: %d\n", state);
	(void) fprintf(stderr, "This must be a coding error of some kind.\n");
	return COND_PushCondition(DUL_FSMERROR, DUL_Message(DUL_FSMERROR),
				  state, event);
    }
    entry = &StateTable[event][state - 1];

#ifdef DEBUG
    if (debug) {
	(void) fprintf(DEBUG_DEVICE, "DUL  FSM Table: State: %2d Event: %2d\n",
		       state, event);
	(void) fprintf(DEBUG_DEVICE, "DUL  Event:  %s\n", entry->eventName);
	(void) fprintf(DEBUG_DEVICE, "DUL  Action: %s\n", entry->actionName);
    }
#endif

    if (entry->actionFunction != NULL)
	return entry->actionFunction(network, association, entry->nextState,
				     params);
    else {
	(void) fprintf(stderr,
		"PRV_StateMachine: No action defined, state: %d event %d\n",
		       state, event);
	return COND_PushCondition(DUL_FSMERROR, DUL_Message(DUL_FSMERROR),
				  state, event);
    }
}

/* fsmDebug
**
** Purpose:
**	To enable/disable the debugging facility.
**
** Parameter Dictionary:
**	flag	Used to enable/disable the debugging facility.
**
** Return Values:
**	None
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
void
fsmDebug(CTNBOOLEAN flag)
{
    debug = flag;
}

#ifdef BLOG
void
fsmBlog(CTNBOOLEAN flag)
{
    blog = flag;
}
#else
void 
fsmBlog(CTNBOOLEAN)
{
}
#endif


/* ============================================================
**
**  Private functions (local to this module) defined below.
*/

/* AE_1_TransportConnect
**
** Purpose:
**	Issue a TRANSPORT_CONNECT request primitive to local transport
**	service.
**
** Parameter Dictionary:
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_ILLEGALSERVICEPARAMETER
**	DUL_NORMAL
**	DUL_TCPINITERROR
**	DUL_UNKNOWNHOST
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
AE_1_TransportConnect(PRIVATE_NETWORKKEY ** network,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    DUL_ASSOCIATESERVICEPARAMETERS
    * service;
    CONDITION
	cond;

    service = (DUL_ASSOCIATESERVICEPARAMETERS *) params;
    clearPDUCache(association);
    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0) {
	cond = requestAssociationTCP(network, service, association);
	(*association)->protocolState = nextState;
    } else
	cond = COND_PushCondition(DUL_UNSUPPORTEDNETWORK,
	      DUL_Message(DUL_UNSUPPORTEDNETWORK), (*network)->networkType);

    return cond;
}

/* AE_2_SendAssociateRQPDU
**
** Purpose:
**	Send A-ASSOCIATE-RQ PDU.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_LISTCREATEFAILED
**	DUL_LISTERROR
**	DUL_MALLOCERROR
**	DUL_NORMAL
**	DUL_TCPIOERROR
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
AE_2_SendAssociateRQPDU(PRIVATE_NETWORKKEY ** network,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    DUL_ASSOCIATESERVICEPARAMETERS
    * service;
    CONDITION
	cond;

    service = (DUL_ASSOCIATESERVICEPARAMETERS *) params;
    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0) {
	cond = sendAssociationRQTCP(network, service, association);
	(*association)->protocolState = nextState;
	return cond;
    } else
	return COND_PushCondition(DUL_UNSUPPORTEDNETWORK,
	      DUL_Message(DUL_UNSUPPORTEDNETWORK), (*network)->networkType);
}


/* AE_3_AssociateConfirmationAccept
**
** Purpose:
**	Issue an A-ASSOCIATE confirmation (Accept) primitive
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_CODINGERROR
**	DUL_ILLEGALPDU
**	DUL_ILLEGALPDULENGTH
**	DUL_LISTCREATEFAILED
**	DUL_LISTERROR
**	DUL_MALLOCERROR
**	DUL_NETWORKCLOSED
**	DUL_NORMAL
**	DUL_READTIMEOUT
**	DUL_UNEXPECTEDPDU
**	DUL_UNRECOGNIZEDPDUTYPE
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
AE_3_AssociateConfirmationAccept(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    DUL_ASSOCIATESERVICEPARAMETERS
    * service;
    CONDITION
	cond;
    unsigned char
        buffer[8192],
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
	*requestedPresentationCtx;
    DUL_SUBITEM
	* subItem;
    PRV_SCUSCPROLE
	* scuscpRole;

    service = (DUL_ASSOCIATESERVICEPARAMETERS *) params;
    cond = readPDU(association, DUL_BLOCK, 0, buffer, sizeof(buffer),
		   &pduType, &pduReserve, &pduLength);
    if (cond != DUL_NORMAL)
	return cond;

    if (debug)
	dump_pdu("Associate Accept", buffer, pduLength + 6);

    if (pduType == DUL_TYPEASSOCIATEAC) {
	cond = parseAssociate(buffer, pduLength, &assoc);
	if (debug) {
	    (void) fflush(DEBUG_DEVICE);
	}
	if (cond != DUL_NORMAL)
	    return COND_PushCondition(DUL_ILLEGALPDU,
				      DUL_Message(DUL_ILLEGALPDU));

	(void) strcpy(service->respondingAPTitle, assoc.calledAPTitle);
	(void) strcpy(service->callingAPTitle, assoc.callingAPTitle);
	(void) strcpy(service->applicationContextName,
		      assoc.applicationContext.data);

	if ((service->acceptedPresentationContext = LST_Create()) == NULL)
	    return COND_PushCondition(DUL_LISTCREATEFAILED,
				      DUL_Message(DUL_LISTCREATEFAILED),
				      "AE_3_AssociateConfirmationAccept");

	prvCtx = (PRV_PRESENTATIONCONTEXTITEM*)LST_Head(&assoc.presentationContextList);
	if (prvCtx != NULL)
	    (void) LST_Position(&assoc.presentationContextList, (LST_NODE*)prvCtx);
	while (prvCtx != NULL) {
	    userPresentationCtx = (DUL_PRESENTATIONCONTEXT*)malloc(sizeof(*userPresentationCtx));
	    if (userPresentationCtx == NULL)
		return COND_PushCondition(DUL_MALLOCERROR,
					  DUL_Message(DUL_MALLOCERROR),
					  "AE_3_AssociateConfirmationAccept",
					  sizeof(*userPresentationCtx));

	    (void) memset(userPresentationCtx, 0, sizeof(userPresentationCtx));
	    userPresentationCtx->result = prvCtx->result;
	    userPresentationCtx->presentationContextID = prvCtx->contextID;
	    userPresentationCtx->proposedTransferSyntax = NULL;
	    requestedPresentationCtx = findPresentationCtx(
		 &service->requestedPresentationContext, prvCtx->contextID);
	    if (requestedPresentationCtx != NULL) {
		strcpy(userPresentationCtx->abstractSyntax,
		       requestedPresentationCtx->abstractSyntax);
		userPresentationCtx->proposedSCRole =
		    requestedPresentationCtx->proposedSCRole;
	    }
	    userPresentationCtx->acceptedSCRole = DUL_SC_ROLE_DEFAULT;
	    scuscpRole = findSCUSCPRole(
					&assoc.userInfo.SCUSCPRoleList,
					userPresentationCtx->abstractSyntax);
	    if (scuscpRole != NULL) {
		if (scuscpRole->SCURole == scuscpRole->SCPRole)
		    userPresentationCtx->acceptedSCRole = DUL_SC_ROLE_SCUSCP;
		else if (scuscpRole->SCURole == 1)
		    userPresentationCtx->acceptedSCRole = DUL_SC_ROLE_SCU;
		else
		    userPresentationCtx->acceptedSCRole = DUL_SC_ROLE_SCP;
	    }
	    if (prvCtx->transferSyntaxList == NULL)
		return COND_PushCondition(DUL_PEERILLEGALXFERSYNTAXCOUNT,
			    DUL_Message(DUL_PEERILLEGALXFERSYNTAXCOUNT), 0);

	    if ((prvCtx->result == DUL_PRESENTATION_ACCEPT) &&
		(LST_Count(&prvCtx->transferSyntaxList) != 1))
		return COND_PushCondition(DUL_PEERILLEGALXFERSYNTAXCOUNT,
				DUL_Message(DUL_PEERILLEGALXFERSYNTAXCOUNT),
				    LST_Count(&prvCtx->transferSyntaxList));

	    subItem = (DUL_SUBITEM*)LST_Head(&prvCtx->transferSyntaxList);
	    if (subItem != NULL)
		(void) strcpy(userPresentationCtx->acceptedTransferSyntax,
			      subItem->data);

	    if (LST_Enqueue(&service->acceptedPresentationContext,
			    (LST_NODE*)userPresentationCtx) != LST_NORMAL)
		return COND_PushCondition(DUL_LISTERROR,
					  DUL_Message(DUL_LISTERROR),
					"AE_3_AssociateConfirmationAccept");

	    prvCtx = (PRV_PRESENTATIONCONTEXTITEM*)LST_Next(&assoc.presentationContextList);

	}

	destroyPresentationContextList(&assoc.presentationContextList);
	destroyUserInformationLists(&assoc.userInfo);
	service->peerMaxPDU = assoc.userInfo.maxLength.maxLength;
	(*association)->maxPDV = assoc.userInfo.maxLength.maxLength;
	(*association)->maxPDVAcceptor =
	    assoc.userInfo.maxLength.maxLength;
	strcpy(service->calledImplementationClassUID,
	       assoc.userInfo.implementationClassUID.data);
	strcpy(service->calledImplementationVersionName,
	       assoc.userInfo.implementationVersionName.data);

	(*association)->associationState = DUL_ASSOC_ESTABLISHED;
	(*association)->protocolState = nextState;
	return DUL_NORMAL;
    } else {
	return COND_PushCondition(DUL_UNEXPECTEDPDU,
				  DUL_Message(DUL_UNEXPECTEDPDU));
    }
}

/* AE_4_AssociateConfirmationReject
**
** Purpose:
**	Issue A-ASSOCIATE confirmation reject primitive and close transport
**	connection.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_ASSOCIATIONREJECTED
**	DUL_CODINGERROR
**	DUL_ILLEGALPDULENGTH
**	DUL_NETWORKCLOSED
**	DUL_NORMAL
**	DUL_READTIMEOUT
**	DUL_UNEXPECTEDPDU
**	DUL_UNRECOGNIZEDPDUTYPE
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
AE_4_AssociateConfirmationReject(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    DUL_ASSOCIATESERVICEPARAMETERS
    * service;
    CONDITION
	cond;
    unsigned char
        buffer[128],
        pduType,
        pduReserve;
    unsigned long
        pduLength;

    service = (DUL_ASSOCIATESERVICEPARAMETERS *) params;
    cond = readPDUBody(association, DUL_BLOCK, 0, buffer, sizeof(buffer),
		       &pduType, &pduReserve, &pduLength);
    if (cond != DUL_NORMAL)
	return cond;

    if (pduType == DUL_TYPEASSOCIATERJ) {
	service->result = buffer[1];
	service->resultSource = buffer[2];
	service->diagnostic = buffer[3];
	(*association)->protocolState = nextState;
	closeTransport(association);
	cond = COND_PushCondition(DUL_ASSOCIATIONREJECTED,
				  DUL_Message(DUL_ASSOCIATIONREJECTED));
    } else
	cond = COND_PushCondition(DUL_UNEXPECTEDPDU,
				  DUL_Message(DUL_UNEXPECTEDPDU));

    return cond;
}


/* AE_5_TransportConnectResponse
**
** Purpose:
**	Issue Transport connection response primitive and start ARTIM timer.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**	DUL_NORMAL
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
AE_5_TransportConnectResponse(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    clearPDUCache(association);
    (*association)->protocolState = nextState;
/*  Start the timer */

    return DUL_NORMAL;
}



/* AE_6_ExamineAssociateRequest
**
** Purpose:
**	Stop ARTIM timer and if A-ASSOCIATE-RQ acceptable by service-provider,
**	issue A-ASSOCIATE indication primitive else issue A-ASSOCIATE
**	indication primitive.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_CODINGERROR
**	DUL_ILLEGALPDU
**	DUL_ILLEGALPDULENGTH
**	DUL_LISTCREATEFAILED
**	DUL_LISTERROR
**	DUL_MALLOCERROR
**	DUL_NETWORKCLOSED
**	DUL_NORMAL
**	DUL_READTIMEOUT
**	DUL_UNEXPECTEDPDU
**	DUL_UNRECOGNIZEDPDUTYPE
**	DUL_UNSUPPORTEDNETWORK
**	DUL_UNSUPPORTEDPEERPROTOCOL
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
AE_6_ExamineAssociateRequest(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int /*nextState*/, void *params)
{
    DUL_ASSOCIATESERVICEPARAMETERS
    * service;
    CONDITION
	cond;
    unsigned char
        buffer[8192],
        pduType,
        pduReserve;
    unsigned long
        pduLength;
    PRV_ASSOCIATEPDU
	assoc;

    (*association)->timerStart = 0;
    service = (DUL_ASSOCIATESERVICEPARAMETERS *) params;
    cond = readPDU(association, DUL_BLOCK, 0, buffer, sizeof(buffer),
		   &pduType, &pduReserve, &pduLength);
    if (cond != DUL_NORMAL)
	return cond;
#ifdef BLOG
    memcpy((*association)->fragmentBuffer, buffer, pduLength);
    (*association)->nextPDUType = pduType;
    (*association)->nextPDUReserved = pduReserve;
    (*association)->nextPDULength = pduLength;
#endif

    if (pduType == DUL_TYPEASSOCIATERQ) {

	if (debug)
	    dump_pdu("Associate Request", buffer, pduLength + 6);
	cond = parseAssociate(buffer, pduLength, &assoc);
	if (debug) {
	    (void) fflush(DEBUG_DEVICE);
	}
	if (cond != DUL_NORMAL) {
	    if (cond == DUL_UNSUPPORTEDPEERPROTOCOL)	/* Make it look OK */
		(*association)->protocolState = STATE3;
	    return cond;
	}
	(void) strcpy(service->calledAPTitle, assoc.calledAPTitle);
	(void) strcpy(service->callingAPTitle, assoc.callingAPTitle);
	(void) strcpy(service->applicationContextName,
		      assoc.applicationContext.data);

	if ((service->requestedPresentationContext = LST_Create()) == NULL)
	    return COND_PushCondition(DUL_LISTCREATEFAILED,
				      DUL_Message(DUL_LISTCREATEFAILED), "AE_6_ExamineAssociateRequest");
	if (translatePresentationContextList(&assoc.presentationContextList,
					     &assoc.userInfo.SCUSCPRoleList,
		      &service->requestedPresentationContext) != DUL_NORMAL)
	    return COND_PushCondition(DUL_PCTRANSLATIONFAILURE,
				      DUL_Message(DUL_PCTRANSLATIONFAILURE),
				      "AE_6_ExamineAssociateRequest");


	service->peerMaxPDU = assoc.userInfo.maxLength.maxLength;
	(*association)->maxPDV = assoc.userInfo.maxLength.maxLength;
	(*association)->maxPDVRequestor =
	    assoc.userInfo.maxLength.maxLength;
	strcpy(service->callingImplementationClassUID,
	       assoc.userInfo.implementationClassUID.data);
	strcpy(service->callingImplementationVersionName,
	       assoc.userInfo.implementationVersionName.data);
	(*association)->associationState = DUL_ASSOC_ESTABLISHED;

	destroyPresentationContextList(&assoc.presentationContextList);
	destroyUserInformationLists(&assoc.userInfo);

	/* If this PDU is ok with us */
	(*association)->protocolState = STATE3;

	return DUL_NORMAL;
    } else {
	return COND_PushCondition(DUL_UNEXPECTEDPDU,
				  DUL_Message(DUL_UNEXPECTEDPDU));
    }
}


/* AE_7_SendAssociateAC
**
** Purpose:
**	Send A-ASSOCIATE-AC PDU.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_LISTCREATEFAILED
**	DUL_LISTERROR
**	DUL_MALLOCERROR
**	DUL_NORMAL
**	DUL_TCPIOERROR
**	DUL_UNSUPPORTEDNETWORK
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
AE_7_SendAssociateAC(PRIVATE_NETWORKKEY ** network,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    DUL_ASSOCIATESERVICEPARAMETERS
    * service;
    CONDITION
	cond;

    service = (DUL_ASSOCIATESERVICEPARAMETERS *) params;
    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0) {
	cond = sendAssociationACTCP(network, service, association);
	(*association)->protocolState = nextState;
	return cond;
    } else
	return COND_PushCondition(DUL_UNSUPPORTEDNETWORK,
	      DUL_Message(DUL_UNSUPPORTEDNETWORK), (*network)->networkType);
}


/* AE_8_SendAssociateRJ
**
** Purpose:
**	Send A-ASSOCIATE-RJ PDU and start ARTIM timer.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_MALLOCERROR
**	DUL_NORMAL
**	DUL_TCPIOERROR
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
AE_8_SendAssociateRJ(PRIVATE_NETWORKKEY ** network,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    DUL_ABORTITEMS
    * abortItems;
    CONDITION
	cond;

    abortItems = (DUL_ABORTITEMS *) params;
    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0) {
	cond = sendAssociationRJTCP(network, abortItems, association);
	(*association)->protocolState = nextState;
	return cond;
    } else
	return COND_PushCondition(DUL_UNSUPPORTEDNETWORK,
	      DUL_Message(DUL_UNSUPPORTEDNETWORK), (*network)->networkType);
}

/* DT_1_SendPData
**
** Purpose:
**	Send P-DATA-TF PDU
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_NORMAL
**	DUL_TCPIOERROR
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
DT_1_SendPData(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    CONDITION
    cond;
    DUL_PDVLIST
	* pdvList;

    pdvList = (DUL_PDVLIST *) params;
    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0) {
	cond = sendPDataTCP(association, pdvList);
	(*association)->protocolState = nextState;
	return cond;
    } else
	return COND_PushCondition(DUL_UNSUPPORTEDNETWORK,
	  DUL_Message(DUL_UNSUPPORTEDNETWORK), (*association)->networkType);
}

/* DT_2_IndicatePData
**
** Purpose:
**	Send P-DATA indication primitive.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_CODINGERROR
**	DUL_ILLEGALPDU
**	DUL_ILLEGALPDULENGTH
**	DUL_MALLOCERROR
**	DUL_NETWORKCLOSED
**	DUL_NORMAL
**	DUL_PDATAPDUARRIVED
**	DUL_READTIMEOUT
**	DUL_UNRECOGNIZEDPDUTYPE
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
DT_2_IndicatePData(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    CONDITION
    cond;
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

    (*association)->protocolState = nextState;

    cond = readPDUBody(association, DUL_BLOCK, 0,
		       (*association)->fragmentBuffer,
		       (*association)->fragmentBufferLength,
		       &pduType, &pduReserved, &pduLength);
    if (cond != DUL_NORMAL)
	return cond;
#ifdef DUMP_DATA_PDU
    if (debug) {
	dump_data((*association)->fragmentBuffer, pduLength);
    }
#endif
#ifdef BLOG
    if (blog) {
	U32 type;
	type = pduType;
	BLG_Write(&(*association)->logHandle, type,
		  (*association)->fragmentBuffer, pduLength);
    }
#endif

    length = pduLength;
    pdvCount = 0;
    p = (*association)->fragmentBuffer;
    while (length > 0) {
	EXTRACT_LONG_BIG(p, pdvLength);
	p += 4 + pdvLength;
	length -= 4 + pdvLength;
	pdvCount++;
    }
    if (length != 0) {
	(void) fprintf(stderr, "PDV Lengths don't add up correctly: %d pdvs\n",
		       (int)pdvCount);
	(void) fprintf(stderr,
		       "In DT_2_IndicatePData.  This probably indicates a");
	(void) fprintf(stderr, " malformed P DATA PDU.\n");
	return COND_PushCondition(DUL_ILLEGALPDU, DUL_Message(DUL_ILLEGALPDU),
				  (unsigned long) pduType);
    }
    (*association)->pdvCount = pdvCount;
    if (pdvCount > 0)
	(*association)->pdvIndex = 0;
    else
	(*association)->pdvIndex = -1;

    {
	unsigned char u;
	p = (*association)->fragmentBuffer;
	(*association)->pdvPointer = p;
	EXTRACT_LONG_BIG(p, pdvLength);
	(*association)->currentPDV.fragmentLength = pdvLength - 2;
	(*association)->currentPDV.presentationContextID = p[4];

	u = p[5];
	if (u & 2)
	    (*association)->currentPDV.lastPDV = TRUE;
	else
	    (*association)->currentPDV.lastPDV = FALSE;

	if (u & 1)
	    (*association)->currentPDV.pdvType = DUL_COMMANDPDV;
	else
	    (*association)->currentPDV.pdvType = DUL_DATASETPDV;

	(*association)->currentPDV.data = p + 6;
    }
/*
    pdv = malloc(pdvCount * sizeof(*pdv));
    if (pdv == NULL) {
	return COND_PushCondition(DUL_MALLOCERROR,
		DUL_Message(DUL_MALLOCERROR), "DT_2_IndicatePData",
		(unsigned long)pdvCount*sizeof(*pdv));
    }
    scratchPDV = pdv;
    pdvList->pdv = scratchPDV;
    length = pduLength;
    pdvList->count = pdvCount;
    p = (unsigned char *)pdvList->scratch;
    while(length > 0) { unsigned char u;
	EXTRACT_LONG_BIG(p, pdvLength);
	scratchPDV->fragmentLength = pdvLength-2;
	scratchPDV->presentationContextID = p[4];

	u = p[5];
	if (u & 2)
	    scratchPDV->lastPDV = TRUE;
	else
	    scratchPDV->lastPDV = FALSE;

	if (u & 1)
	    scratchPDV->pdvType = DUL_COMMANDPDV;
	else
	    scratchPDV->pdvType = DUL_DATASETPDV;

	scratchPDV->data = p+6;
	p += 4 + pdvLength;
	length -= 4 + pdvLength;

	scratchPDV++;
    }
    if (length != 0) {
	(void)fprintf(stderr,
	    "DT_2_IndicatePData:PDV Lengths don't add up correctly: %d pdvs\n",
		pdvCount);
	return COND_PushCondition(DUL_ILLEGALPDU, DUL_Message(DUL_ILLEGALPDU),
		(unsigned long)pduType);
    }
*/
    return DUL_PDATAPDUARRIVED;
}


/* AR_1_SendReleaseRQ
**
** Purpose:
**	Send A-RELEASE-RQ PDU.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**      DUL_MALLOCERROR
**      DUL_NORMAL
**      DUL_TCPIOERROR
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
AR_1_SendReleaseRQ(PRIVATE_NETWORKKEY ** network,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    CONDITION
    cond;

    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0) {
	cond = sendReleaseRQTCP(association);
	(*association)->protocolState = nextState;
	return cond;
    } else
	return COND_PushCondition(DUL_UNSUPPORTEDNETWORK,
	      DUL_Message(DUL_UNSUPPORTEDNETWORK), (*network)->networkType);
}

/* AR_2_IndicateRelease
**
** Purpose:
**	Issue A-RELEASE indication primitive.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_PEERREQUESTEDRELEASE
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
AR_2_IndicateRelease(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
#ifdef BLOG
    if (blog) {
	U32 type;
	U32 dummy = 0;
	type = DUL_TYPERELEASERQ;
	BLG_Write(&(*association)->logHandle, type,
		  &dummy, (unsigned long) sizeof(dummy));
    }
#endif

    (*association)->protocolState = nextState;
    return DUL_PEERREQUESTEDRELEASE;
}

/* AR_3_ConfirmRelease
**
** Purpose:
**	Issue A-RELEASE confirmation primitive and close transport connection.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_RELEASECONFIRMED
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
AR_3_ConfirmRelease(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    closeTransport(association);
    (*association)->protocolState = nextState;
    return DUL_RELEASECONFIRMED;
}

/* AR_4_SendReleaseRP
**
** Purpose:
**	Issue A-RELEASE-RP PDU and start ARTIM timer.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**      DUL_MALLOCERROR
**      DUL_NORMAL
**      DUL_TCPIOERROR
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
AR_4_SendReleaseRP(PRIVATE_NETWORKKEY ** network,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    CONDITION
    cond;

    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0) {
	cond = sendReleaseRPTCP(association);
	(*association)->timerStart = time(NULL);
	(*association)->protocolState = nextState;
	return cond;
    } else
	return COND_PushCondition(DUL_UNSUPPORTEDNETWORK,
	      DUL_Message(DUL_UNSUPPORTEDNETWORK), (*network)->networkType);
}

/* AR_5_StopARTTIMtimer
**
** Purpose:
**	Stop ARTIM timer.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**	DUL_NORMAL
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
AR_5_StopARTIMtimer(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int /*nextState*/, void * /*params*/)
{
    (*association)->timerStart = 0;
    return DUL_NORMAL;
}

/* AR_6_IndicatePData
**
** Purpose:
**	Issue P-DATA indication.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**	DUL_PDATAPDUARRIVED
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
AR_6_IndicatePData(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** /*association*/, int /*nextState*/, void * /*params*/)
{
    return DUL_PDATAPDUARRIVED;
}

/* AR_7_SendPData
**
** Purpose:
**	Issue P-DATA-TF PDU
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_NORMAL
**	DUL_TCPIOERROR
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
AR_7_SendPDATA(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void *params)
{
    CONDITION
    cond;
    DUL_PDVLIST
	* pdvList;

    pdvList = (DUL_PDVLIST *) params;
    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0) {
	cond = sendPDataTCP(association, pdvList);
	(*association)->protocolState = nextState;
	return cond;
    } else
	return COND_PushCondition(DUL_UNSUPPORTEDNETWORK,
	  DUL_Message(DUL_UNSUPPORTEDNETWORK), (*association)->networkType);
}

/* AR_8_IndicateARelease
**
** Purpose:
**	Issue A-RELEASE indication (release collision):
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**	DUL_PEERREQUESTEDRELEASE
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
AR_8_IndicateARelease(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int /*nextState*/, void * /*params*/)
{
/*    if (strcmp((*association)->applicationType, AE_REQUESTOR) == 0) */
    if ((*association)->applicationFunction == PRV_APPLICATION_REQUESTOR)
	(*association)->protocolState = STATE9;
    else
	(*association)->protocolState = STATE10;

    return DUL_PEERREQUESTEDRELEASE;
}

/* AR_9_SendAReleaseRP
**
** Purpose:
**	Send A-RELEASE-RP PDU
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**      DUL_MALLOCERROR
**      DUL_NORMAL
**      DUL_TCPIOERROR
**	DUL_UNSUPPORTEDNETWORK
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
AR_9_SendAReleaseRP(PRIVATE_NETWORKKEY ** network,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    CONDITION
    cond;

    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0) {
	cond = sendReleaseRPTCP(association);
	(*association)->protocolState = nextState;
	return cond;
    } else
	return COND_PushCondition(DUL_UNSUPPORTEDNETWORK,
	      DUL_Message(DUL_UNSUPPORTEDNETWORK), (*network)->networkType);
}

/* AR_10_ConfirmRelease
**
** Purpose:
**	Issue A-RELEASE confirmation primitive.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**	DUL_RELEASECONFIRMED
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
AR_10_ConfirmRelease(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    (*association)->protocolState = nextState;
    return DUL_RELEASECONFIRMED;
}


/* AA_1_SendAbort
**
** Purpose:
**	Send A-ABORT PDU (service-user source) and start (or restart if
**	already started) ARTIM timer.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**      DUL_MALLOCERROR
**      DUL_NORMAL
**      DUL_TCPIOERROR
**	DUL_UNSUPPORTEDNETWORK
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
AA_1_SendAAbort(PRIVATE_NETWORKKEY ** network,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    DUL_ABORTITEMS
    abortItems;
    CONDITION
	cond;

    abortItems.result = 0x00;
    abortItems.source = DUL_ABORTSERVICEUSER;
    abortItems.reason = 0x00;

    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0) {
	cond = sendAbortTCP(&abortItems, association);
	(*association)->protocolState = nextState;
	(*association)->timerStart = time(NULL);
	return cond;
    } else
	return COND_PushCondition(DUL_UNSUPPORTEDNETWORK,
	      DUL_Message(DUL_UNSUPPORTEDNETWORK), (*network)->networkType);
}

/* AA_2_CloseTransport
**
** Purpose:
**	Close transport connection.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**	DUL_NORMAL
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
AA_2_CloseTransport(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    (*association)->timerStart = 0;
    closeTransport(association);
    (*association)->protocolState = nextState;
    return DUL_NORMAL;
}

/* AA_2_CloseTimeout
**
** Purpose:
**	Stop ARTIM timer.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**	DUL_READTIMEOUT
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
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
**	If (service-user initiated abort)
**	   - issue A-ABORT indication and close transport connection
**	otherwise (service provider initiated abort)
**	   - issue A-P-ABORT indication and close transport connection.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_PEERABORTEDASSOCIATION
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
AA_3_IndicatePeerAborted(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
#ifdef BLOG
    if (blog) {
	U32 type;
	U32 dummy = 0;
	type = DUL_TYPEABORT;
	BLG_Write(&(*association)->logHandle, type,
		  &dummy, (unsigned long) sizeof(dummy));
    }
#endif

    closeTransport(association);
    (*association)->protocolState = nextState;
    return COND_PushCondition(DUL_PEERABORTEDASSOCIATION,
			      DUL_Message(DUL_PEERABORTEDASSOCIATION));


}


/* AA_4_IndicateAPAbort
**
** Purpose:
**	Issue A-P-ABORT indication primitive.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_PEERABORTEDASSOCIATION
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
AA_4_IndicateAPAbort(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    closeTransport(association);
    (*association)->protocolState = nextState;
    return COND_PushCondition(DUL_PEERABORTEDASSOCIATION,
			      DUL_Message(DUL_PEERABORTEDASSOCIATION));
}


/* AA_5_StopARTIMtimer
**
** Purpose:
**	Stop ARTIM timer.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**	DUL_NORMAL
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
AA_5_StopARTIMtimer(PRIVATE_NETWORKKEY ** /*network*/,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    (*association)->timerStart = 0;
    (*association)->protocolState = nextState;
    return DUL_NORMAL;
}


/* AA_6_IgnorePDU
**
** Purpose:
**	Ignore PDU
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**	DUL_CODINGERROR
**	DUL_ILLEGALPDULENGTH
**	DUL_NETWORKCLOSED
**	DUL_NORMAL
**	DUL_READTIMEOUT
**	DUL_UNRECOGNIZEDPDUTYPE
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/


static CONDITION
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
    CONDITION
	cond;

    (*association)->protocolState = nextState;
    cond = readPDUHead(association, buffer, sizeof(buffer), DUL_NOBLOCK,
		    PRV_DEFAULTTIMEOUT, &PDUType, &PDUReserved, &l);
    if (cond != DUL_NORMAL) {
	return cond;
    }
    PDULength = l;
    while (PDULength > 0 && cond == DUL_NORMAL) {
	if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0) {
	    cond = defragmentTCP((*association)->networkSpecific.TCP.socket,
				 DUL_NOBLOCK, (*association)->timerStart,
		       (*association)->timeout, buffer, sizeof(buffer), &l);
	    if (cond != DUL_NORMAL)
		return cond;
	    PDULength -= l;
	} else
	    PDULength = 0;
    }
    return DUL_NORMAL;
}


/* AA_7_State13SendAbort
**
** Purpose:
**	SendA-ABORT PDU
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**      DUL_MALLOCERROR
**      DUL_NORMAL
**      DUL_TCPIOERROR
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
AA_7_State13SendAbort(PRIVATE_NETWORKKEY ** network,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    DUL_ABORTITEMS
    abortItems;
    CONDITION
	cond;

    abortItems.result = 0x00;
    abortItems.source = DUL_ABORTSERVICEPROVIDER;
    abortItems.reason = DUL_ABORTUNEXPECTEDPDU;

    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0) {
	cond = sendAbortTCP(&abortItems, association);
	(*association)->protocolState = nextState;
	return cond;
    } else
	return COND_PushCondition(DUL_UNSUPPORTEDNETWORK,
	      DUL_Message(DUL_UNSUPPORTEDNETWORK), (*network)->networkType);
}


/* AA_8_UnrecognizedPDUSendAbort
**
** Purpose:
**	Send A-ABORT PDU (service provider source), issue an A-P-ABORT
**	indication and start timer.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	association	Handle to the Association
**	nextState	The next state to be reached from the current state
**	params		Service parameters describing the Association
**
** Return Values:
**
**      DUL_MALLOCERROR
**      DUL_NORMAL
**      DUL_TCPIOERROR
**      DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
AA_8_UnrecognizedPDUSendAbort(PRIVATE_NETWORKKEY ** network,
	 PRIVATE_ASSOCIATIONKEY ** association, int nextState, void * /*params*/)
{
    DUL_ABORTITEMS
    abortItems;
    CONDITION
	cond;

    abortItems.result = 0x00;
    abortItems.source = DUL_ABORTSERVICEPROVIDER;
    abortItems.reason = DUL_ABORTUNEXPECTEDPDU;

    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0) {
	cond = sendAbortTCP(&abortItems, association);
	(*association)->protocolState = nextState;
	(*association)->timerStart = time(0);
	return cond;
    } else
	return COND_PushCondition(DUL_UNSUPPORTEDNETWORK,
	      DUL_Message(DUL_UNSUPPORTEDNETWORK), (*network)->networkType);
}

/* requestAssociationTCP
**
** Purpose:
**	Request a TCP Association
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	params		Service parameters describing the Association
**	association	Handle to the Association
**
** Return Values:
**
**	DUL_ILLEGALSERVICEPARAMETER
**	DUL_NORMAL
**	DUL_TCPINITERROR
**	DUL_UNKNOWNHOST
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/


static CONDITION
requestAssociationTCP(PRIVATE_NETWORKKEY ** /*network*/,
		      DUL_ASSOCIATESERVICEPARAMETERS * params,
		      PRIVATE_ASSOCIATIONKEY ** association)
{
    char
        node[128];
    int
        port;
    struct sockaddr_in
        server;
    struct hostent
       *hp;
    int
        s;
    struct linger
        sockarg;

    if (sscanf(params->calledPresentationAddress, "%[^:]:%d", node, &port) != 2)
	return COND_PushCondition(DUL_ILLEGALSERVICEPARAMETER,
				  DUL_Message(DUL_ILLEGALSERVICEPARAMETER),
				  params->calledPresentationAddress);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
	return COND_PushCondition(DUL_TCPINITERROR,
			    DUL_Message(DUL_TCPINITERROR), strerror(errno));
    }
    server.sin_family = AF_INET;
    hp = gethostbyname(node);
    if (hp == NULL) {
	return COND_PushCondition(DUL_UNKNOWNHOST,
				  DUL_Message(DUL_UNKNOWNHOST), node);
    }
    (void) memcpy(&server.sin_addr, hp->h_addr, (unsigned long) hp->h_length);
    server.sin_port = (u_short) htons(port);

    if (connect(s, (struct sockaddr *) & server, sizeof(server)) < 0) {
	(void) close(s);
	(*association)->networkState = NETWORK_DISCONNECTED;
	(*association)->networkSpecific.TCP.socket = -1;
	return COND_PushCondition(DUL_TCPINITERROR,
			    DUL_Message(DUL_TCPINITERROR), strerror(errno));
    } else {
	(*association)->networkState = NETWORK_CONNECTED;
	(*association)->networkSpecific.TCP.socket = s;
	sockarg.l_onoff = 0;
#ifdef HAVE_GUSI_H
        /* GUSI always returns an error for setsockopt(...) */
#else
	if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char *) &sockarg, (int) sizeof(sockarg)) < 0) {
	    return COND_PushCondition(DUL_TCPINITERROR,
			    DUL_Message(DUL_TCPINITERROR), strerror(errno));
	}
#endif
	setTCPBufferLength(s);

	return DUL_NORMAL;
    }
}


/* sendAssociationRQTCP
**
** Purpose:
**	Send a TCP association request PDU.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	params		Service parameters describing the Association
**	association	Handle to the Association
**
** Return Values:
**
**	DUL_LISTCREATEFAILED
**	DUL_LISTERROR
**	DUL_MALLOCERROR
**	DUL_TCPIOERROR
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
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
    CONDITION
	cond;

    associateRequest.presentationContextList = NULL;
    cond = constructAssociatePDU(params, DUL_TYPEASSOCIATERQ,
				 &associateRequest);
    if (cond != DUL_NORMAL)
	return cond;
    if (associateRequest.length + 2 <= sizeof(buffer))
	b = buffer;
    else {
	b = (unsigned char*)malloc(associateRequest.length + 2);
	if (b == NULL) {
	    return COND_PushCondition(DUL_MALLOCERROR,
		       DUL_Message(DUL_MALLOCERROR), "sendAssociationRQTCP",
				      associateRequest.length + 2);
	}
    }
    cond = streamAssociatePDU(&associateRequest, b,
			      associateRequest.length + 2, &length);
    destroyPresentationContextList(&associateRequest.presentationContextList);
    destroyUserInformationLists(&associateRequest.userInfo);
    if (cond != DUL_NORMAL)
	return cond;
#ifdef VERBOSE
    {
	int fd;
	if (debug) {
	    fd = open("associate_rq", O_WRONLY | O_CREAT, 0666);
	    (void) write(fd, (char*)b, associateRequest.length + 6);
	    (void) fprintf(DEBUG_DEVICE, "%d %d\n", length, associateRequest.length + 6);
	    (void) close(fd);
	}
    }
#endif

    nbytes = write((*association)->networkSpecific.TCP.socket, (char*)b,
		   associateRequest.length + 6);
    if ((unsigned long) nbytes != associateRequest.length + 6) {
	return COND_PushCondition(DUL_TCPIOERROR, DUL_Message(DUL_TCPIOERROR),
				  strerror(errno), "requestAssociationTCP");
    }
    if (b != buffer)
	free(b);
    return DUL_NORMAL;
}

/* sendAssociationACTCP
**
** Purpose:
**	Send an Association ACK on a TCP connection
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	params		Service parameters describing the Association
**	association	Handle to the Association
**
** Return Values:
**
**	DUL_LISTCREATEFAILED
**	DUL_LISTERROR
**	DUL_MALLOCERROR
**	DUL_NORMAL
**	DUL_TCPIOERROR
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
sendAssociationACTCP(PRIVATE_NETWORKKEY ** /*network*/,
		     DUL_ASSOCIATESERVICEPARAMETERS * params,
		     PRIVATE_ASSOCIATIONKEY ** association)
{
    PRV_ASSOCIATEPDU
    associateReply;
    unsigned char
        buffer[4096],
       *b;
    unsigned long
        length;
    int
        nbytes;
    CONDITION
	cond;
    DUL_ASSOCIATESERVICEPARAMETERS
	localService;

    associateReply.presentationContextList = NULL;

    localService = *params;
    cond = constructAssociatePDU(&localService, DUL_TYPEASSOCIATEAC,
				 &associateReply);
    if (cond != DUL_NORMAL)
	return cond;

    if (associateReply.length + 2 <= sizeof(buffer))
	b = buffer;
    else {
	b = (unsigned char*)malloc(associateReply.length + 2);
	if (b == NULL) {
	    return COND_PushCondition(DUL_MALLOCERROR,
			DUL_Message(DUL_MALLOCERROR), "ReplyAssociationTCP",
				      associateReply.length + 2);
	}
    }
    cond = streamAssociatePDU(&associateReply, b,
			      associateReply.length + 2, &length);
    destroyPresentationContextList(&associateReply.presentationContextList);
    destroyUserInformationLists(&associateReply.userInfo);

    if (cond != DUL_NORMAL)
	return cond;

#ifdef VERBOSE
    {
	int fd;
	if (debug) {
	    fd = open("associate_rp", O_WRONLY | O_CREAT, 0666);
	    (void) write(fd, (char*)b, associateReply.length + 6);
	    (void) fprintf(DEBUG_DEVICE, "%d %d\n", length, associateReply.length + 6);
	    (void) close(fd);
	}
    }
#endif

    nbytes = write((*association)->networkSpecific.TCP.socket, (char*)b,
		   associateReply.length + 6);
    if ((unsigned long) nbytes != associateReply.length + 6) {
	return COND_PushCondition(DUL_TCPIOERROR, DUL_Message(DUL_TCPIOERROR),
				  strerror(errno), "ReplyAssociationTCP");
    }
    if (b != buffer)
	free(b);
    return DUL_NORMAL;
}

/* sendAssociationRJTCP
**
** Purpose:
**	Send an Association Reject PDU on a TCP connection.
**
** Parameter Dictionary:
**
**	network		Handle to the network environment
**	abortItems	Pointer to structure holding information regarding
**			the reason for rejection, the source and result.
**	association	Handle to the Association
**
** Return Values:
**
**	DUL_MALLOCERROR
**	DUL_NORMAL
**	DUL_TCPIOERROR
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
sendAssociationRJTCP(PRIVATE_NETWORKKEY ** /*network*/,
	 DUL_ABORTITEMS * abortItems, PRIVATE_ASSOCIATIONKEY ** association)
{
    CONDITION
    cond;

    DUL_REJECTRELEASEABORTPDU
	pdu;
    unsigned char
        buffer[64],
       *b;
    unsigned long
        length;
    int
        nbytes;


    cond = constructAssociateRejectPDU((unsigned char) abortItems->result,
     (unsigned char) abortItems->source, (unsigned char) abortItems->reason,
				       &pdu);
    if (pdu.length + 6 <= sizeof(buffer))
	b = buffer;
    else {
	b = (unsigned char*)malloc(pdu.length + 6);
	if (b == NULL) {
	    return COND_PushCondition(DUL_MALLOCERROR,
			DUL_Message(DUL_MALLOCERROR), "ReplyAssociationTCP",
				      pdu.length + 6);
	}
    }
    cond = streamRejectReleaseAbortPDU(&pdu, b, pdu.length + 6, &length);

    if (cond == DUL_NORMAL) {
	nbytes = write((*association)->networkSpecific.TCP.socket, (char*)b,
		       pdu.length + 6);
	if ((unsigned long) nbytes != pdu.length + 6) {
	    cond = COND_PushCondition(DUL_TCPIOERROR, strerror(errno),
				      "sendAssociationRJTCP");
	}
    }
    if (b != buffer)
	free(b);
    return cond;
}

/* sendAbortTCP
**
** Purpose:
**	Send an ABORT PDU over a TCP connection.
**
** Parameter Dictionary:
**	abortItems	Pointer to structure holding information regarding
**			the reason for rejection, the source and result.
**	association	Handle to the Association
**
** Return Values:
**
**	DUL_MALLOCERROR
**	DUL_NORMAL
**	DUL_TCPIOERROR
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
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
    CONDITION
	cond;

    cond = constructAbortPDU(abortItems->source, abortItems->reason, &pdu);
    if (cond != DUL_NORMAL)
	return cond;

    if (pdu.length + 6 <= sizeof(buffer))
	b = buffer;
    else {
	b = (unsigned char*)malloc(pdu.length + 6);
	if (b == NULL) {
	    return COND_PushCondition(DUL_MALLOCERROR,
			       DUL_Message(DUL_MALLOCERROR), "sendAbortTCP",
				      pdu.length + 6);
	}
    }
    cond = streamRejectReleaseAbortPDU(&pdu, b, pdu.length + 6, &length);
    if (cond == DUL_NORMAL) {
	nbytes = write((*association)->networkSpecific.TCP.socket, (char*)b,
		       pdu.length + 6);
	if ((unsigned long) nbytes != pdu.length + 6) {
	    cond = COND_PushCondition(DUL_TCPIOERROR, strerror(errno),
				      "sendAbortTCP");
	}
    }
    if (b != buffer)
	free(b);

    return cond;
}


/* sendReleaseRQTCP
**
** Purpose:
**	Send a Release request PDU over a TCP connection
**
** Parameter Dictionary:
**
**	association	Handle to the Association
**
** Return Values:
**
**	DUL_MALLOCERROR
**	DUL_NORMAL
**	DUL_TCPIOERROR
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
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
    CONDITION
	cond;

    cond = constructReleaseRQPDU(&pdu);
    if (cond != DUL_NORMAL)
	return cond;

    if (pdu.length + 6 <= sizeof(buffer))
	b = buffer;
    else {
	b = (unsigned char*)malloc(pdu.length + 6);
	if (b == NULL) {
	    return COND_PushCondition(DUL_MALLOCERROR,
			   DUL_Message(DUL_MALLOCERROR), "sendReleaseRQTCP",
				      pdu.length + 6);
	}
    }
    cond = streamRejectReleaseAbortPDU(&pdu, b, pdu.length + 6, &length);
    if (cond == DUL_NORMAL) {
	nbytes = write((*association)->networkSpecific.TCP.socket, (char*)b,
		       pdu.length + 6);
	if ((unsigned long) nbytes != pdu.length + 6) {
	    cond = COND_PushCondition(DUL_TCPIOERROR, strerror(errno),
				      "sendReleaseRQTCP");
	}
    }
    if (b != buffer)
	free(b);

    return cond;
}



/* sendReleaseRPTCP
**
** Purpose:
**	Send a release response PDU over a TCP connection
**
** Parameter Dictionary:
**
**	association	Handle to the Association
**
** Return Values:
**
**	DUL_MALLOCERROR
**	DUL_NORMAL
**	DUL_TCPIOERROR
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
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
    CONDITION
	cond;

    cond = constructReleaseRPPDU(&pdu);
    if (cond != DUL_NORMAL)
	return cond;

    if (pdu.length + 6 <= sizeof(buffer))
	b = buffer;
    else {
	b = (unsigned char*)malloc(pdu.length + 6);
	if (b == NULL) {
	    return COND_PushCondition(DUL_MALLOCERROR,
			   DUL_Message(DUL_MALLOCERROR), "sendReleaseRPTCP",
				      pdu.length + 6);
	}
    }
    cond = streamRejectReleaseAbortPDU(&pdu, b, pdu.length + 6, &length);
    if (cond == DUL_NORMAL) {
	nbytes = write((*association)->networkSpecific.TCP.socket, (char*)b,
		       pdu.length + 6);
	if ((unsigned long) nbytes != pdu.length + 6) {
	    cond = COND_PushCondition(DUL_TCPIOERROR, strerror(errno),
				      "sendReleaseRPTCP");
	}
    }
    if (b != buffer)
	free(b);

    return cond;
}


/* SendPDataTCP
**
** Purpose:
**	Send a data PDU over a TCP connection.
**
** Parameter Dictionary:
**
**	association	Handle to the Association
**	pdvList		Pointer to structure holding a list of PDVs
**
** Return Values:
**
**	DUL_NORMAL
**	DUL_TCPIOERROR
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
sendPDataTCP(PRIVATE_ASSOCIATIONKEY ** association,
	     DUL_PDVLIST * pdvList)
{
    DUL_PDV
    * pdv;
    unsigned long
        count,
        length,
        pdvLength;
    int
        localLast;
    CONDITION
	cond;
    unsigned char
       *p;
    DUL_DATAPDU
	dataPDU;
    CTNBOOLEAN
	firstTrip;

    count = pdvList->count;
    pdv = pdvList->pdv;

    cond = DUL_NORMAL;
    while (cond == DUL_NORMAL && count-- > 0) {
	length = pdv->fragmentLength;
	p = (unsigned char *) pdv->data;
	firstTrip = TRUE;
	while ((firstTrip || (length > 0)) && (cond == DUL_NORMAL)) {
	    firstTrip = FALSE;
	    pdvLength = (length <= (*association)->maxPDV - 6) ?
		length : (*association)->maxPDV - 6;
	    localLast = ((pdvLength == length) && pdv->lastPDV);
	    cond = constructDataPDU(p, pdvLength, pdv->pdvType,
			   pdv->presentationContextID, localLast, &dataPDU);
	    cond = writeDataPDU(association, &dataPDU);

	    p += pdvLength;
	    length -= pdvLength;
	}
	pdv++;

    }
    return cond;
}

/* writeDataPDU
**
** Purpose:
**	Send the data through the socket interface (for TCP).
**
** Parameter Dictionary:
**
**	association	Handle to the Association
**	pdu		The data unit that is to be sent thru the socket
**
** Return Values:
**
**	DUL_NORMAL
**	DUL_TCPIOERROR
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
writeDataPDU(PRIVATE_ASSOCIATIONKEY ** association,
	     DUL_DATAPDU * pdu)
{
    CONDITION
    cond;
    unsigned char
        head[24];
    unsigned long
        length;
    int
        nbytes;

    cond = streamDataPDUHead(pdu, head, sizeof(head), &length);
    if (cond != DUL_NORMAL)
	return cond;

    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0) {
#ifdef DEBUG
	if (debug) {
	    recordOutGoing(head, length);
	    recordOutGoing(pdu->presentationDataValue.data,
			   pdu->presentationDataValue.length - 2);
	}
#endif
	nbytes = write((*association)->networkSpecific.TCP.socket, 
		       (char*)head, length);
	if ((unsigned long) nbytes != length)
	    return COND_PushCondition(DUL_TCPIOERROR,
			       DUL_Message(DUL_TCPIOERROR), strerror(errno),
				      "writeDataPDU");
	nbytes = write((*association)->networkSpecific.TCP.socket,
		       (char*)pdu->presentationDataValue.data,
		       pdu->presentationDataValue.length - 2);
	if ((unsigned long) nbytes != pdu->presentationDataValue.length - 2)
	    return COND_PushCondition(DUL_TCPIOERROR,
			       DUL_Message(DUL_TCPIOERROR), strerror(errno),
				      "writeDataPDU");
    }
    return DUL_NORMAL;
}

/* closeTransport
**
** Purpose:
**	Close the transport connection.
**
** Parameter Dictionary:
**
**	association	Handle to the Association
**
** Return Values:
**	None.
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static void
closeTransport(PRIVATE_ASSOCIATIONKEY ** association)
{
    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0)
	closeTransportTCP(association);
}

/* closeTransportTCP
**
** Purpose:
**	Close the TCP transport connection.
**
** Parameter Dictionary:
**
**	association	Handle to the Association
**
** Return Values:
**	None
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static void
closeTransportTCP(PRIVATE_ASSOCIATIONKEY ** association)
{
    if ((*association)->networkSpecific.TCP.socket != 0)
	(void) close((*association)->networkSpecific.TCP.socket);
}


/*static int inputPDU = NO_PDU;
static unsigned char nextPDUType = 0x00;
static unsigned char nextPDUReserved = 0;
static unsigned long nextPDULength = 0;
unsigned char pduHead[6];
*/

/* clearPDUCache()
**
** Purpose:
**	Clear the Association of any PDUs.
**
** Parameter Dictionary:
**
**	association	Handle to the Association
**
** Return Values:
**	None.
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static void
clearPDUCache(PRIVATE_ASSOCIATIONKEY ** association)
{
    (*association)->inputPDU = NO_PDU;
}

/* PRV_NextPDUType
**
** Purpose:
**	Get the next PDU's type.
**
** Parameter Dictionary:
**
**	association	Handle to the Association
**	block		Option for blocking/non-blocking
**	timeout		Timeout interval within to get the Type of the next
**			PDU
**	pduType		The type of next PDU returned to caller.
**
** Return Values:
**
**	DUL_CODINGERROR
**	DUL_ILLEGALPDULENGTH
**	DUL_NETWORKCLOSED
**	DUL_NORMAL
**	DUL_READTIMEOUT
**	DUL_UNRECOGNIZEDPDUTYPE
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

CONDITION
PRV_NextPDUType(PRIVATE_ASSOCIATIONKEY ** association, DUL_BLOCKOPTIONS block,
		int timeout, unsigned char *pduType)
{
    CONDITION
	cond;

    if ((*association)->inputPDU == NO_PDU) {
	cond = readPDUHead(association, (*association)->pduHead,
			   sizeof((*association)->pduHead),
			   block, timeout, &(*association)->nextPDUType,
			   &(*association)->nextPDUReserved,
			   &(*association)->nextPDULength);
	if (cond != DUL_NORMAL)
	    return cond;
	(*association)->inputPDU = PDU_HEAD;
    }
    *pduType = (*association)->nextPDUType;

    return DUL_NORMAL;
}

/* readPDU
**
** Purpose:
**	Read the PDU from the incoming stream.
**
** Parameter Dictionary:
**	association	Handle to the Association
**	block		Blocking/non-blocking options for reading
**	timeout		Timeout interval for reading
**	buffer		Buffer holding the PDU
**	maxLength	Maximum allowable length of the body
**	PDUType		Type of the PDU (returned to the caller)
**	PDUReserved	Reserved field of the PDU (returned to caller)
**	PDULength	Length of PDU read (returned to caller)
**
**
** Return Values:
**
**	DUL_CODINGERROR
**	DUL_ILLEGALPDULENGTH
**	DUL_NETWORKCLOSED
**	DUL_NORMAL
**	DUL_READTIMEOUT
**	DUL_UNRECOGNIZEDPDUTYPE
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
readPDU(PRIVATE_ASSOCIATIONKEY ** association, DUL_BLOCKOPTIONS block,
	int timeout, unsigned char *buffer, unsigned long maxLength,
	unsigned char *pduType, unsigned char *pduReserved,
	unsigned long *pduLength)
{
    CONDITION
    cond;


    if ((*association)->inputPDU == NO_PDU) {
	cond = readPDUHead(association, (*association)->pduHead,
			   sizeof((*association)->pduHead),
			   block, timeout, &(*association)->nextPDUType,
			   &(*association)->nextPDUReserved,
			   &(*association)->nextPDULength);
	if (cond != DUL_NORMAL)
	    return cond;
	(*association)->inputPDU = PDU_HEAD;
    }
    (void) memcpy(buffer, (*association)->pduHead,
		  sizeof((*association)->pduHead));
    cond = readPDUBody(association, block, timeout,
		       buffer + sizeof((*association)->pduHead),
		       maxLength - sizeof((*association)->pduHead),
		       pduType, pduReserved, pduLength);
    return cond;
}

/* readPDUHead
**
** Purpose:
**	Read the header from the PDU. The type of protocol is obtained from
**	the Association handle.
**
** Parameter Dictionary:
**	association	Handle to the Association
**	buffer		Buffer holding the PDU
**	maxLength	Maximum allowable length of the header
**	block		Blocking/non-blocking options for reading
**	timeout		Timeout interval for reading
**	PDUType		Type of the PDU (returned to the caller)
**	PDUReserved	Reserved field of the PDU (returned to caller)
**	PDULength	Length of PDU header read (returned to caller)
**
** Return Values:
**
**	DUL_CODINGERROR
**	DUL_ILLEGALPDULENGTH
**	DUL_NETWORKCLOSED
**	DUL_NORMAL
**	DUL_READTIMEOUT
**	DUL_UNRECOGNIZEDPDUTYPE
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
readPDUHead(PRIVATE_ASSOCIATIONKEY ** association,
	    unsigned char *buffer, unsigned long maxLength,
	    DUL_BLOCKOPTIONS block, int timeout,
	    unsigned char *PDUType, unsigned char *PDUReserved,
	    unsigned long *PDULength)
{
    CONDITION
    cond;

    cond = DUL_NORMAL;
    if ((*association)->inputPDU == NO_PDU) {
	if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0)
	    cond = readPDUHeadTCP(association, buffer, maxLength, block,
				  timeout,
	     &(*association)->nextPDUType, &(*association)->nextPDUReserved,
				  &(*association)->nextPDULength);
	else
	    cond = COND_PushCondition(DUL_UNSUPPORTEDNETWORK,
				      DUL_Message(DUL_UNSUPPORTEDNETWORK),
				      (*association)->networkType);
    }
    if (cond == DUL_NORMAL) {
	(*association)->inputPDU = PDU_HEAD;
	*PDUType = (*association)->nextPDUType;
	*PDUReserved = (*association)->nextPDUReserved;
	*PDULength = (*association)->nextPDULength;
	if (*PDULength > (*association)->maxPDVInput)
	    cond = COND_PushCondition(DUL_ILLEGALPDULENGTH,
			      DUL_Message(DUL_ILLEGALPDULENGTH), *PDULength,
				(*association)->maxPDVInput, "readPDUHead");
    }
    return cond;
}


/* readPDUBody
**
** Purpose:
**	Read the body of the incoming PDU.
**
** Parameter Dictionary:
**	association	Handle to the Association
**	block		For blocking/non-blocking read
**	timeout		Timeout interval for reading
**	buffer		Buffer to hold the PDU
**	maxLength	MAximum number of bytes to read
**	pduType		PDU Type of the incoming PDU (returned to caller)
**	pduReserved	Reserved field in the PDU
**	pduLength	Actual number of bytes read
**
** Return Values:
**
**	DUL_CODINGERROR
**	DUL_ILLEGALPDULENGTH
**	DUL_NETWORKCLOSED
**	DUL_NORMAL
**	DUL_READTIMEOUT
**	DUL_UNRECOGNIZEDPDUTYPE
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
readPDUBody(PRIVATE_ASSOCIATIONKEY ** association,
	    DUL_BLOCKOPTIONS block, int timeout,
	    unsigned char *buffer, unsigned long maxLength,
	    unsigned char *pduType, unsigned char *pduReserved,
	    unsigned long *pduLength)
{
    CONDITION
    cond;

    if (strcmp((*association)->networkType, DUL_NETWORK_TCP) == 0)
	cond = readPDUBodyTCP(association, block, timeout, buffer, maxLength,
			      pduType, pduReserved, pduLength);
    else
	return COND_PushCondition(DUL_UNSUPPORTEDNETWORK,
	  DUL_Message(DUL_UNSUPPORTEDNETWORK), (*association)->networkType);

    (*association)->inputPDU = NO_PDU;
    return cond;
}

/* readPDUHeadTCP
**
** Purpose:
**	Read the TCP header.
**
** Parameter Dictionary:
**	association	Handle to the Association
**	buffer		Buffer containing the header
**	maxLength	Maximum length of the header
**	block		For blocking/non-blocking read
**	timeout		Timeout interval for reading
**	type		One of the many DUL PDU types
**	reserved	For reserved field
**	pduLength	Length of the header finally read in.
**
** Return Values:
**
**	DUL_CODINGERROR
**	DUL_NETWORKCLOSED
**	DUL_NORMAL
**	DUL_READTIMEOUT
**	DUL_UNRECOGNIZEDPDUTYPE
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/


static CONDITION
readPDUHeadTCP(PRIVATE_ASSOCIATIONKEY ** association,
	       unsigned char *buffer, unsigned long maxLength,
	       DUL_BLOCKOPTIONS block, int timeout,
     unsigned char *type, unsigned char *reserved, unsigned long *pduLength)
{
    CONDITION
    cond;
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

    if (maxLength < 6)
	return COND_PushCondition(DUL_CODINGERROR,
	DUL_Message(DUL_CODINGERROR), "buffer too small in readPDUTCPHead");

    if (timeout == PRV_DEFAULTTIMEOUT)
	timeout = (*association)->timeout;
    cond = defragmentTCP((*association)->networkSpecific.TCP.socket, block,
		   (*association)->timerStart, timeout, buffer, 6, &length);
    if (cond != DUL_NORMAL)
	return cond;

#ifdef DEBUG
    if (debug) {
	(void) fprintf(DEBUG_DEVICE, "Read PDU HEAD TCP: ");
	for (idx = 0; idx < 6; idx++)
	    fprintf(DEBUG_DEVICE, " %02x", buffer[idx]);
	fprintf(DEBUG_DEVICE, "\n");
    }
#endif

    *type = *buffer++;
    *reserved = *buffer++;

    for (idx = found = 0; !found && idx < sizeof(legalPDUTypes); idx++) {
	found = (*type == legalPDUTypes[idx]);
    }
    if (!found)
	return COND_PushCondition(DUL_UNRECOGNIZEDPDUTYPE,
			       DUL_Message(DUL_UNRECOGNIZEDPDUTYPE), *type);

    EXTRACT_LONG_BIG(buffer, length);
    buffer += 4;
    *pduLength = length;

#ifdef DEBUG
    if (debug)
	(void) fprintf(DEBUG_DEVICE, "Read PDU HEAD TCP: type: %x, length: %ld (%x)\n",
		       *type, *pduLength, (unsigned int)*pduLength);
#endif
    return DUL_NORMAL;
}


/* readPDUBodyTCP
**
** Purpose:
**	Read the PDU from a TCP socket interface.
**
** Parameter Dictionary:
**	association	Handle to the Association
**	block		For blocking/non-blocking read
**	timeout		Timeout interval for reading
**	buffer		Buffer to hold the PDU
**	maxLength	MAximum number of bytes to read
**	pduType		PDU Type of the incoming PDU (returned to caller)
**	pduReserved	Reserved field in the PDU
**	pduLength	Actual number of bytes read
**
**
** Return Values:
**
**	DUL_CODINGERROR
**	DUL_ILLEGALPDULENGTH
**	DUL_NETWORKCLOSED
**	DUL_NORMAL
**	DUL_READTIMEOUT
**	DUL_UNRECOGNIZEDPDUTYPE
**	DUL_UNSUPPORTEDNETWORK
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
readPDUBodyTCP(PRIVATE_ASSOCIATIONKEY ** association,
	       DUL_BLOCKOPTIONS block, int timeout,
	       unsigned char *buffer, unsigned long /*maxLength*/,
	       unsigned char *pduType, unsigned char *pduReserved,
	       unsigned long *pduLength)
{
    CONDITION
    cond;
    unsigned long
        length;

    if ((*association)->inputPDU == NO_PDU) {
	cond = readPDUHead(association, (*association)->pduHead,
			   sizeof((*association)->pduHead),
			   block, timeout, &(*association)->nextPDUType,
			   &(*association)->nextPDUReserved,
			   &(*association)->nextPDULength);
	if (cond != DUL_NORMAL)
	    return cond;
    }
    *pduType = (*association)->nextPDUType;
    *pduReserved = (*association)->nextPDUReserved;
    *pduLength = (*association)->nextPDULength;

    if (timeout == PRV_DEFAULTTIMEOUT)
	timeout = (*association)->timeout;

    cond = defragmentTCP((*association)->networkSpecific.TCP.socket,
			 block, (*association)->timerStart, timeout,
			 buffer, (*association)->nextPDULength, &length);
    return cond;
}


/* defragmentTCP
**
** Purpose:
**	Actually read the desired number of bytes  of the PDU from the
**	incoming socket stream.
**
** Parameter Dictionary:
**	sock		Socket descriptor
**	block		Blocking/non-blocking option
**	timerStart	Time at which the reading operation is started.
**	timeout		Timeout interval for reading
**	p		Buffer to hold the information
**	l		Maximum number of bytes to read
**	rtnLength	Actual number of bytes that were read (returned
**			to the caller)
**
**
** Return Values:
**
**	DUL_NETWORKCLOSED
**	DUL_NORMAL
**	DUL_READTIMEOUT
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/


static CONDITION
defragmentTCP(int sock, DUL_BLOCKOPTIONS block, time_t timerStart,
	      int timeout, void *p, unsigned long l, unsigned long *rtnLen)
{
    unsigned char
       *b;
    int
        bytesRead;

    b = (unsigned char *) p;
    if (rtnLen != NULL)
	*rtnLen = 0;

    if (block == DUL_NOBLOCK) {
	int timeToWait;
	if (timerStart == 0)
	    timeToWait = timeout;
	else
	    timeToWait = timeout - (int) (time(NULL) - timerStart);
#ifdef SMM
	if (timeToWait <= 0)
	    return DUL_READTIMEOUT;
#endif
	if (!networkDataAvailable(sock, timeToWait))
	    return DUL_READTIMEOUT;
    }
    while (l > 0) {
	bytesRead = read(sock, (char*)b, l);
	if (bytesRead > 0) {
	    b += bytesRead;
	    l -= (unsigned long) bytesRead;
	    if (rtnLen != NULL)
		*rtnLen += (unsigned long) bytesRead;
	} else {
	    return DUL_NETWORKCLOSED;
	}
    }
    return DUL_NORMAL;
}


/* networkDataAvailable
**
** Purpose:
**	Check if data is available to read at the socket interface (for a
**	TCP connection).
**
** Parameter Dictionary:
**	s		Socket descriptor
**	timeout		Timeout interval within which to check if data is
**			available to read
**
** Return Values:
**	TRUE
**	FALSE
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CTNBOOLEAN
networkDataAvailable(int s, int timeout)
{
    struct timeval
        t;
    fd_set
	fdset;
    int
        nfound;


    FD_ZERO(&fdset);
    FD_SET(s, &fdset);
    t.tv_sec = timeout;
    t.tv_usec = 0;
    nfound = select(s + 1, &fdset, NULL, NULL, &t);
    if (nfound <= 0)
	return FALSE;
    else {
	if (FD_ISSET(s, &fdset))
	    return TRUE;
	else			/* This one should not really happen */
	    return FALSE;
    }
}

/* recordOutGoing
**
** Purpose:
**	Debugging routine to record the contents of the outgoing PDU
**
** Parameter Dictionary:
**	buf		Buffer holding the PDU
**	length		Length of the PDU
**
** Return Values:
**	None
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
#ifdef DEBUG
static void
recordOutGoing(
#ifdef VERBOSE
    void * buf, 
    unsigned long length)
#else
    void * /*buf*/, 
    unsigned long /*length*/)
#endif
{

#ifdef VERBOSE
    if (debug) {
	static int fd = -1;
	if (fd < 0) {
	    fd = open("outgoing.pdu", O_CREAT | O_WRONLY, 0666);
	    if (fd < 0)
		return;
	}
	(void) write(fd, buf, length);
    }
#endif
}
#endif

/* dump_pdu
**
** Purpose:
** 	Debugging routine for dumping a pdu
**
** Parameter Dictionary:
**	type		PDU type
**	buffer		Buffer holding the PDU
**	length		Length of the PDU
**
** Return Values:
**	None
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static void
dump_pdu(char *type, void *buffer, unsigned long length)
{
    unsigned char
       *p;
    int
        position = 0;

    (void) fprintf(DEBUG_DEVICE, "PDU Type: %s PDU Length: %ld\n", type, length);
    if (length > 512) {
	(void) fprintf(DEBUG_DEVICE, "Only dumping 512 bytes\n");
	length = 512;
    }
    p = (unsigned char*)buffer;

    while (length-- > 0) {
	(void) fprintf(DEBUG_DEVICE, "  %02x", (unsigned int) (*p++));
	if ((++position) % 16 == 0)
	    (void) fprintf(DEBUG_DEVICE, "\n");
    }
    (void) fprintf(DEBUG_DEVICE, "\n");
}


/* dump_data
**
** Purpose:
**	Dump the data part of the PDU
**
** Parameter Dictionary:
**	buffer		Buffer holding the data
**	length		Size of the data
**
** Return Values:
**	None
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
#ifdef DUMP_DATA_PDU
static void
dump_data(void *buffer, unsigned long length)
{
    unsigned char
       *p;
    int
        position = 0;

    if (length > 512) {
	(void) fprintf(DEBUG_DEVICE, "Only dumping 512 bytes\n");
	length = 512;
    }
    p = buffer;

    while (length-- > 0) {
	(void) fprintf(DEBUG_DEVICE, "  %02x", (unsigned int) (*p++));
	if ((++position) % 16 == 0)
	    (void) fprintf(DEBUG_DEVICE, "\n");
    }
    (void) fprintf(DEBUG_DEVICE, "\n");
}
#endif

/* setTCPBufferLength
**
** Purpose:
**      This routine checks for the existence of an environment
**	variable (TCP_BUFFER_LENGTH).  If that variable is defined (and
**	is a legal integer), this routine sets the socket SNDBUF and RCVBUF
**	variables to the value defined in TCP_BUFFER_LENGTH.
**
** Parameter Dictionary:
**	sock		Socket descriptor (identifier)
**
** Return Values:
**	None
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static void
setTCPBufferLength(int sock)
{
    char
       *TCPBufferLength;
    int
        bufLen,
        optLen;

#ifdef HAVE_GUSI_H
    /* GUSI always returns an error for setsockopt(...) */
#else
    if ((TCPBufferLength = getenv("TCP_BUFFER_LENGTH")) != NULL) {
	if (sscanf(TCPBufferLength, "%d", &bufLen) == 1) {
	    optLen = sizeof(bufLen);
	    (void) setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *) &bufLen, optLen);
	    (void) setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *) &bufLen, optLen);
	}
    }
#endif
}

/* translatePresentationContextList
**
** Purpose:
**	Translate the internal list into a user context list and a
**	SCU-SCP role list
**
** Parameter Dictionary:
**	internalList		Input list from which the two output lists
**				are derived
**	SCUSCPRoleList		Role list (returned to the caller)
**	userContextList		User context list (returend to the caller)
**
** Return Values:
**	DUL_NORMAL
**	0
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
CONDITION
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

    context = (PRV_PRESENTATIONCONTEXTITEM*)LST_Head(internalList);
    (void) LST_Position(internalList, (LST_NODE*)context);
    while (context != NULL) {
	userContext = (DUL_PRESENTATIONCONTEXT*)malloc(sizeof(*userContext));
	if (userContext == NULL)
	    return COND_PushCondition(DUL_MALLOCERROR,
	    DUL_Message(DUL_MALLOCERROR), "translatePresentationContextList",
				      sizeof(*userContext));
	if ((userContext->proposedTransferSyntax = LST_Create()) == NULL)
	    return COND_PushCondition(DUL_LISTCREATEFAILED,
				      DUL_Message(DUL_LISTCREATEFAILED),
				      "translatePresentationContextList");

	userContext->acceptedTransferSyntax[0] = '\0';
	userContext->presentationContextID = context->contextID;
	strcpy(userContext->abstractSyntax, context->abstractSyntax.data);
	userContext->proposedSCRole = DUL_SC_ROLE_DEFAULT;
	userContext->acceptedSCRole = DUL_SC_ROLE_DEFAULT;

	scuscpRole = findSCUSCPRole(SCUSCPRoleList,
				    userContext->abstractSyntax);
	if (scuscpRole != NULL) {
	    if (scuscpRole->SCURole == scuscpRole->SCPRole)
		userContext->proposedSCRole = DUL_SC_ROLE_SCUSCP;
	    else if (scuscpRole->SCURole == 1)
		userContext->proposedSCRole = DUL_SC_ROLE_SCU;
	    else
		userContext->proposedSCRole = DUL_SC_ROLE_SCP;
	}
	subItem = (DUL_SUBITEM*)LST_Head(&context->transferSyntaxList);
	if (subItem == NULL)
	    return COND_PushCondition(DUL_PEERILLEGALXFERSYNTAXCOUNT,
			    DUL_Message(DUL_PEERILLEGALXFERSYNTAXCOUNT), 0);
	(void) LST_Position(&context->transferSyntaxList, (LST_NODE*)subItem);
	while (subItem != NULL) {
	    transfer = (DUL_TRANSFERSYNTAX*)malloc(sizeof(*transfer));
	    if (transfer == NULL)
		return COND_PushCondition(DUL_MALLOCERROR,
					  DUL_Message(DUL_MALLOCERROR), "translatePresentationContextList",
					  sizeof(*transfer));
	    strcpy(transfer->transferSyntax, subItem->data);
	    if (LST_Enqueue(&userContext->proposedTransferSyntax, (LST_NODE*)transfer) !=
		LST_NORMAL)
		return COND_PushCondition(DUL_LISTERROR,
					  DUL_Message(DUL_LISTERROR),
					"translatePresentationContextList");

	    subItem = (DUL_SUBITEM*)LST_Next(&context->transferSyntaxList);
	}
	if (LST_Enqueue(userContextList, (LST_NODE*)userContext) != LST_NORMAL)
	    return COND_PushCondition(DUL_LISTERROR,
	    DUL_Message(DUL_LISTERROR), "translatePresentationContextList");
	context = (PRV_PRESENTATIONCONTEXTITEM*)LST_Next(internalList);
    }
    return DUL_NORMAL;
}

/* findPresentationCtx
**
** Purpose:
**	Find the requested presentation context using the contextID as the
**	key
**
** Parameter Dictionary:
**	list		List to be searched
**	contextID	The search key
**
** Return Values:
**	A presentation context list (if found) else NULL.
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
DUL_PRESENTATIONCONTEXT *
findPresentationCtx(
		    LST_HEAD ** list, DUL_PRESENTATIONCONTEXTID contextID)
{
    DUL_PRESENTATIONCONTEXT
    * ctx;

    if ((ctx = (DUL_PRESENTATIONCONTEXT*)LST_Head(list)) == NULL)
	return NULL;

    (void) LST_Position(list, (LST_NODE*)ctx);
    while (ctx != NULL) {
	if (ctx->presentationContextID == contextID)
	    return ctx;

	ctx = (DUL_PRESENTATIONCONTEXT*)LST_Next(list);
    }
    return NULL;
}


/* findSCUSCPRole
**
** Purpose:
**	Search for a SCUSCP role list, given the abstarct syntax as the
**	key.
**
** Parameter Dictionary:
**	list		List to be searched
**	abstractSyntax	The search key
**
** Return Values:
**	The role list, if found, else NULL
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
PRV_SCUSCPROLE
*
findSCUSCPRole(LST_HEAD ** list, char *abstractSyntax)
{
    PRV_SCUSCPROLE
	* role;

    role = (PRV_SCUSCPROLE*)LST_Head(list);
    if (role != NULL)
	(void) LST_Position(list, (LST_NODE*)role);

    while (role != NULL) {
	if (strcmp(role->SOPClassUID, abstractSyntax) == 0)
	    return role;

	role = (PRV_SCUSCPROLE*)LST_Next(list);
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
    (void) LST_Destroy(l);
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
    (void) LST_Destroy(&userInfo->SCUSCPRoleList);
}

CONDITION
DULPRV_translateAssocReq(unsigned char *buffer,
	  unsigned long pduLength, DUL_ASSOCIATESERVICEPARAMETERS * service)
{
    PRV_ASSOCIATEPDU
	assoc;
    CONDITION cond;

    cond = parseAssociate(buffer, pduLength, &assoc);
    if (debug) {
	(void) fflush(DEBUG_DEVICE);
    }
    if (cond != DUL_NORMAL) {
	return cond;
    }
    (void) strcpy(service->calledAPTitle, assoc.calledAPTitle);
    (void) strcpy(service->callingAPTitle, assoc.callingAPTitle);
    (void) strcpy(service->applicationContextName,
		  assoc.applicationContext.data);

    if ((service->requestedPresentationContext = LST_Create()) == NULL)
	return COND_PushCondition(DUL_LISTCREATEFAILED,
				  DUL_Message(DUL_LISTCREATEFAILED),
				  "DULPRV_translateAssocReq");
    if (translatePresentationContextList(&assoc.presentationContextList,
					 &assoc.userInfo.SCUSCPRoleList,
		      &service->requestedPresentationContext) != DUL_NORMAL)
	return COND_PushCondition(DUL_PCTRANSLATIONFAILURE,
				  DUL_Message(DUL_PCTRANSLATIONFAILURE),
				  "DULPRV_translateAssocReq");


    service->peerMaxPDU = assoc.userInfo.maxLength.maxLength;
    strcpy(service->callingImplementationClassUID,
	   assoc.userInfo.implementationClassUID.data);
    strcpy(service->callingImplementationVersionName,
	   assoc.userInfo.implementationVersionName.data);

    destroyPresentationContextList(&assoc.presentationContextList);
    destroyUserInformationLists(&assoc.userInfo);

    return DUL_NORMAL;
}

/*
** CVS Log
** $Log: dulfsm.cc,v $
** Revision 1.6  1996-09-24 16:22:46  hewett
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.5  1996/06/20 07:35:51  hewett
** Removed inclusion of system header already included by dcompat.h
** and made sure that dcompat.h is always included (via dicom.h).
**
** Revision 1.4  1996/05/03 10:31:55  hewett
** Move some common system include files out to include/dcompat.h which were
** causing problems due to multiple inclusion on some machines.
**
** Revision 1.3  1996/04/27 12:57:59  hewett
** Corrected cause of warnings when compiling under "c++ -O -g -Wall"
** under Solaris 2.4.  Mostly due to unintialized variables.
**
** Revision 1.2  1996/04/25 16:11:20  hewett
** Added parameter casts to char* for bzero calls.  Replaced some declarations
** of DIC_UL with unsigned long (reduces mismatch problems with 32 & 64 bit
** architectures).  Added some protection to inclusion of sys/socket.h (due
** to MIPS/Ultrix).
**
** Revision 1.1.1.1  1996/03/26 18:38:46  hewett
** Initial Release.
**
**
*/
