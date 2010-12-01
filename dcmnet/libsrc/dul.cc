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
** Module Name(s):
**                DUL_InitializeNetwork
**                DUL_DropNetwork
**                DUL_RequestAssociation
**                DUL_ReceiveAssociationRQ
**                DUL_AcknowledgeAssociateRQ
**                DUL_RejectAssociateRQ
**                DUL_ReleaseAssociation
**                DUL_AbortAssociation
**                DUL_DropAssociation
**                DUL_WritePDVs
**                DUL_ReadPDVs
** Author, Date:  Stephen M. Moore, 14-Apr-93
** Intent:        This module contains the public entry points for the
**                DICOM Upper Layer (DUL) protocol package.
** Last Update:   $Author: joergr $, $Date: 2010-12-01 08:26:36 $
** Revision:      $Revision: 1.94 $
** Status:        $State: Exp $
*/


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/diutil.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CERRNO
#define INCLUDE_CSIGNAL
#define INCLUDE_CTIME
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_ERRNO_H
#include <sys/errno.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
/* sys/socket.h included via dcompat.h - needed for Ultrix */
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_NETINET_IN_SYSTM_H
#include <netinet/in_systm.h>   /* prerequisite for netinet/in.h on NeXT */
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>         /* prerequisite for netinet/tcp.h on NeXT */
#endif
#ifdef HAVE_NETINET_TCP_H
#include <netinet/tcp.h>        /* for TCP_NODELAY */
#endif
#ifdef WITH_TCPWRAPPER
#include <tcpd.h>               /* for hosts_ctl */
#include <syslog.h>
#endif
#ifdef HAVE_SIGNAL_H
// On Solaris with Sun Workshop 11, <signal.h> declares signal() but <csignal> does not
#include <signal.h>
#endif
END_EXTERN_C

#ifdef WITH_TCPWRAPPER
/* libwrap expects that two global flags, deny_severity and allow_severity,
 * are defined and initialized by user code. If these flags are already present
 * somewhere else, compile DCMTK with TCPWRAPPER_SEVERITY_EXTERN defined
 * to avoid linker errors due to duplicate symbols.
 */
#ifndef TCPWRAPPER_SEVERITY_EXTERN
int deny_severity = LOG_WARNING;
int allow_severity = LOG_INFO;
#endif
#endif

BEGIN_EXTERN_C
/* declare extern "C" typedef for signal handler function pointer */
#ifdef SIGNAL_HANDLER_WITH_ELLIPSE
typedef void (*mySIG_TYP)(...);
#else
typedef void (*mySIG_TYP)(int);
#endif
END_EXTERN_C

#ifdef HAVE_GUSI_H
#include <GUSI.h>       /* Use the Grand Unified Sockets Interface (GUSI) on Macintosh */
#endif

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmnet/dcompat.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmnet/lst.h"
#include "dcmtk/ofstd/ofconsol.h"

#include "dcmtk/dcmnet/dul.h"
#include "dulstruc.h"
#include "dulpriv.h"
#include "dulfsm.h"
#include "dcmtk/dcmnet/dcmtrans.h"
#include "dcmtk/dcmnet/dcmlayer.h"
#include "dcmtk/ofstd/ofstd.h"

OFGlobal<OFBool> dcmDisableGethostbyaddr(OFFalse);
OFGlobal<OFBool> dcmStrictRoleSelection(OFFalse);
OFGlobal<Sint32> dcmConnectionTimeout(-1);
OFGlobal<int>    dcmExternalSocketHandle(-1);
OFGlobal<const char *> dcmTCPWrapperDaemonName((const char *)NULL);
OFGlobal<unsigned long> dcmEnableBackwardCompatibility(0);

static int networkInitialized = 0;

static OFCondition
createNetworkKey(const char *mode, int timeout, unsigned long opt,
                 PRIVATE_NETWORKKEY ** k);
static OFCondition
createAssociationKey(PRIVATE_NETWORKKEY ** net, const char *node,
                     unsigned long maxPDU,
                     PRIVATE_ASSOCIATIONKEY ** assoc);
static OFCondition initializeNetworkTCP(PRIVATE_NETWORKKEY ** k, void *p);
static OFCondition
receiveTransportConnection(PRIVATE_NETWORKKEY ** network,
                           DUL_BLOCKOPTIONS block,
                           int timeout,
                           DUL_ASSOCIATESERVICEPARAMETERS * params,
                           PRIVATE_ASSOCIATIONKEY ** association);
static OFCondition
receiveTransportConnectionTCP(PRIVATE_NETWORKKEY ** network,
                              DUL_BLOCKOPTIONS block,
                              int timeout,
                              DUL_ASSOCIATESERVICEPARAMETERS * params,
                              PRIVATE_ASSOCIATIONKEY ** association);


static void destroyAssociationKey(PRIVATE_ASSOCIATIONKEY ** key);
#if 0
static OFCondition writeDataPDU(PRIVATE_ASSOCIATIONKEY ** key, DUL_DATAPDU * pdu);
static OFCondition dropPDU(PRIVATE_ASSOCIATIONKEY ** association, unsigned long l);
#endif
static OFCondition
get_association_parameter(void *paramAddress,
  DUL_DATA_TYPE paramType, size_t paramLength,
  DUL_DATA_TYPE outputType, void *outputAddress, size_t outputLength);
static void setTCPBufferLength(int sock);
static OFCondition checkNetwork(PRIVATE_NETWORKKEY ** networkKey);
static OFCondition checkAssociation(PRIVATE_ASSOCIATIONKEY ** association);
static OFString dump_presentation_ctx(LST_HEAD ** l);
static OFString dump_uid(const char *UID, const char *indent);
static void clearRequestorsParams(DUL_ASSOCIATESERVICEPARAMETERS * params);
static void clearPresentationContext(LST_HEAD ** l);

#define MIN_PDU_LENGTH  4*1024
#define MAX_PDU_LENGTH  128*1024

static OFBool processIsForkedChild = OFFalse;
static OFBool shouldFork = OFFalse;

#ifdef _WIN32
static char **command_argv = NULL;
static int command_argc = 0;
#endif

OFBool DUL_processIsForkedChild()
{
  return processIsForkedChild;
}

void DUL_markProcessAsForkedChild()
{
  processIsForkedChild = OFTrue;
}

void DUL_requestForkOnTransportConnectionReceipt(int argc, char *argv[])
{
  shouldFork = OFTrue;
#ifdef _WIN32
  command_argc = argc;
  command_argv = argv;
#else
  // Work around "Unused parameters"
  (void) argc;
  (void) argv;
#endif
}

void DUL_activateAssociatePDUStorage(DUL_ASSOCIATIONKEY *dulassoc)
{
  if (dulassoc)
  {
    PRIVATE_ASSOCIATIONKEY *assoc = (PRIVATE_ASSOCIATIONKEY *)dulassoc;
    assoc->associatePDUFlag = 1;
  }
}

void DUL_activateCompatibilityMode(DUL_ASSOCIATIONKEY *dulassoc, unsigned long mode)
{
  if (dulassoc)
  {
    PRIVATE_ASSOCIATIONKEY *assoc = (PRIVATE_ASSOCIATIONKEY *)dulassoc;
    assoc->compatibilityMode = mode;
  }
}

void DUL_activateCallback(DUL_ASSOCIATIONKEY *dulassoc, DUL_ModeCallback *cb)
{
  if (dulassoc)
  {
    PRIVATE_ASSOCIATIONKEY *assoc = (PRIVATE_ASSOCIATIONKEY *)dulassoc;
    assoc->modeCallback = cb;
  }
}

void DUL_returnAssociatePDUStorage(DUL_ASSOCIATIONKEY *dulassoc, void *& pdu, unsigned long& pdusize)
{
  if (dulassoc)
  {
    PRIVATE_ASSOCIATIONKEY *assoc = (PRIVATE_ASSOCIATIONKEY *)dulassoc;
    pdu = assoc->associatePDU;
    pdusize = assoc->associatePDULength;
    assoc->associatePDUFlag = 0;
    assoc->associatePDU = NULL;
    assoc->associatePDULength = 0;
  } else {
    pdu = NULL;
    pdusize = 0;
  }
}

unsigned long DUL_getPeerCertificate(DUL_ASSOCIATIONKEY *dulassoc, void *buf, unsigned long bufLen)
{
  PRIVATE_ASSOCIATIONKEY *assoc = (PRIVATE_ASSOCIATIONKEY *)dulassoc;
  if (assoc && assoc->connection)
  {
    return assoc->connection->getPeerCertificate(buf, bufLen);
  }
  return 0;
}

unsigned long DUL_getPeerCertificateLength(DUL_ASSOCIATIONKEY *dulassoc)
{
  PRIVATE_ASSOCIATIONKEY *assoc = (PRIVATE_ASSOCIATIONKEY *)dulassoc;
  if (assoc && assoc->connection)
  {
    return assoc->connection->getPeerCertificateLength();
  }
  return 0;
}

/* DUL_InitializeNetwork
**
** Purpose:
**  Identify and initialize a network to request or accept Associations.
**  The caller identifies
**  whether the application wishes to be an Acceptor (AE_ACCEPTOR) or a
**  Requestor (AE_REQUESTOR) or both (AE_BOTH).  Upon successful
**  initialization of the network, the function returns a key to be used
**  for further network access.
**
** Parameter Dictionary:
**      mode                    NULL terminated string identifying the appli-
**                              cation as a requestor or acceptor.
**      networkParameter        A parameter which is specific to the network
**                              type, which may be needed to initialize the n/w
**      timeout                 Length of time in seconds for TIMER timeout.
**                              If 0, the function will use a default value.
**      opt                     Bitmask which describes options to be used
**                              when initializing the network.
**      networkKey              The handle created by this function and returned
**                              to the caller to access this network environment
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
DUL_InitializeNetwork(const char *mode,
                      void *networkParameter, int timeout, unsigned long opt,
                      DUL_NETWORKKEY ** networkKey)
{
    // default return value if something goes wrong
    *networkKey = NULL;

    // a few initializations must only be done the first time this function is called.
    if (! networkInitialized)
    {
      // make sure "Broken Pipe" signals don't terminate the application
#ifdef SIGPIPE
      (void) signal(SIGPIPE, (mySIG_TYP)SIG_IGN);
#endif

      // initialize DUL FSM
      (void) DUL_InitializeFSM();

      ++networkInitialized;
    }

    // create PRIVATE_NETWORKKEY structure
    PRIVATE_NETWORKKEY *key = NULL;
    OFCondition cond = createNetworkKey(mode, timeout, opt, &key);

    // initialize network
    if (cond.good()) cond = initializeNetworkTCP(&key, networkParameter);

    if (cond.good())
    {
      // everything worked well, return network key
      *networkKey = (DUL_NETWORKKEY *) key;
    }
    else
    {
      // initializeNetworkTCP has failed, destroy PRIVATE_NETWORKKEY structure
      PRIVATE_NETWORKKEY **pkey = &key;
      (void) DUL_DropNetwork((DUL_NETWORKKEY **) pkey);
    }

    return cond;
}

/* DUL_DropNetwork
**
** Purpose:
**      This function drops the network connection established by
**      by DUL_InitializeNetwork.  The caller passes the pointer to the
**      network key.  The network connection is closed and the caller's
**      key is zeroed to prevent misuse.
**
** Parameter Dictionary:
**      callerNetworkKey        Caller's handle to the network environment
**
** Return Values:
**
**
** Algorithm:
**      Close the socket and free the memeory occupied by the network key
*/

OFCondition
DUL_DropNetwork(DUL_NETWORKKEY ** callerNetworkKey)
{
    PRIVATE_NETWORKKEY
        ** networkKey;

    networkKey = (PRIVATE_NETWORKKEY **) callerNetworkKey;

    OFCondition cond = checkNetwork(networkKey);
    if (cond.bad()) return cond;

    if ((*networkKey)->networkSpecific.TCP.tLayerOwned) delete (*networkKey)->networkSpecific.TCP.tLayer;

    if ((*networkKey)->applicationFunction & DICOM_APPLICATION_ACCEPTOR)
    {
#ifdef HAVE_WINSOCK_H
            (void) shutdown((*networkKey)->networkSpecific.TCP.listenSocket, 1 /* SD_SEND */);
            (void) closesocket((*networkKey)->networkSpecific.TCP.listenSocket);
#else
            (void) close((*networkKey)->networkSpecific.TCP.listenSocket);
#endif
    }

    free(*networkKey);
    *networkKey = NULL;
    return EC_Normal;
}

/* DUL_RequestAssociation
**
** Purpose:
**      This function requests an Association with another node.  The caller
**      identifies the network to be used, the name of the other node, a
**      network dependent parameter and a structure which defines the
**      Association parameters needed to establish an Association (the
**      syntax items).  This function establishes the Association and
**      returns the modified list of Association parameters and a key to
**      be used to access this Association.  If the caller does not choose
**      to recognize the parameters returned by the Acceptor, the caller
**      should abort the Association.
**
** Parameter Dictionary:
**      network         Caller's handle whcih describes the network environment
**      params          Pointer to list of parameters which describe the type
**                      of Association requested by the caller.
**      association     Handle created by this function which describes the
**                      Association; returned to the caller.
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
DUL_RequestAssociation(
  DUL_NETWORKKEY ** callerNetworkKey,
  DUL_ASSOCIATESERVICEPARAMETERS * params,
  DUL_ASSOCIATIONKEY ** callerAssociation,
  int activatePDUStorage)
{
    PRIVATE_ASSOCIATIONKEY
        ** association;
    PRIVATE_NETWORKKEY
        ** network;
    unsigned char
        pduType;
    int
        event;

    network = (PRIVATE_NETWORKKEY **) callerNetworkKey;
    association = (PRIVATE_ASSOCIATIONKEY **) callerAssociation;
    OFCondition cond = checkNetwork(network);
    if (cond.bad()) return cond;

    if (((*network)->applicationFunction & DICOM_APPLICATION_REQUESTOR) == 0)
    {
        return DUL_ILLEGALREQUEST;
    }

    if (params->maxPDU < MIN_PDU_LENGTH || params->maxPDU > MAX_PDU_LENGTH)
    {
        return makeDcmnetCondition(DULC_ILLEGALPARAMETER, OF_error, "DUL Illegal parameter (maxPDU) in function DUL_RequestAssociation");
    }

    cond = createAssociationKey(network, "", params->maxPDU, association);
    if (cond.bad())
        return cond;

    if (activatePDUStorage) DUL_activateAssociatePDUStorage(*association);

    /* send a request primitive */
    cond = PRV_StateMachine(network, association,
             A_ASSOCIATE_REQ_LOCAL_USER, (*network)->protocolState, params);

    if (cond.bad())
    {
        /*
         * In case of an error, close the connection and go to the initial
         * state
         */
        OFCondition cond2 = PRV_StateMachine(network, association, TRANS_CONN_CLOSED, (*association)->protocolState, NULL);
        destroyAssociationKey(association);
        return makeDcmnetSubCondition(DULC_REQUESTASSOCIATIONFAILED, OF_error, "Failed to establish association", DimseCondition::push(cond2, cond));
    }
    cond = PRV_StateMachine(network, association,
      TRANS_CONN_CONFIRM_LOCAL_USER, (*association)->protocolState, params);
    if (cond.bad()) {
        destroyAssociationKey(association);
        return cond;
    }
    /* Find the next event */
    cond = PRV_NextPDUType(association, DUL_BLOCK, PRV_DEFAULTTIMEOUT, &pduType);
    if (cond == DUL_NETWORKCLOSED)
        event = TRANS_CONN_CLOSED;
    else if (cond == DUL_READTIMEOUT)
        event = ARTIM_TIMER_EXPIRED;
    else if (cond.bad()) {
        destroyAssociationKey(association);
        return cond;
    } else {
        switch (pduType) {
        case DUL_TYPEASSOCIATERQ:
            event = A_ASSOCIATE_RQ_PDU_RCV;
            break;
        case DUL_TYPEASSOCIATEAC:
            event = A_ASSOCIATE_AC_PDU_RCV;
            break;
        case DUL_TYPEASSOCIATERJ:
            event = A_ASSOCIATE_RJ_PDU_RCV;
            break;
        case DUL_TYPEDATA:
            event = P_DATA_TF_PDU_RCV;
            break;
        case DUL_TYPERELEASERQ:
            event = A_RELEASE_RQ_PDU_RCV;
            break;
        case DUL_TYPERELEASERP:
            event = A_RELEASE_RP_PDU_RCV;
            break;
        case DUL_TYPEABORT:
            event = A_ABORT_PDU_RCV;
            break;
        default:
            event = INVALID_PDU;
            break;
        }
    }

    cond = PRV_StateMachine(network, association, event, (*association)->protocolState, params);
    if (cond.bad() && (cond != DUL_PEERREQUESTEDRELEASE))
    {
        destroyAssociationKey(association);
        return cond;
    }
    return cond;
}

/* DUL_ReceiveAssociationRQ
**
** Purpose:
**      This function allows a caller to ask to receive an Association Request.
**      If the function does receive such a request, it returns a list of
**      Association Items which describe the proposed Association and an
**      AssociationKey used to access this Association.  The user should call
**      DUL_AcknowledgeAssociateRQ if the Association is to be accepted.
**
** Parameter Dictionary:
**      network         Caller's handle to the network environment.
**      block           Flag indicating blocking/non blocking mode
**      timeout         When blocking mode is non blocking, the timeout in seconds
**      params          Pointer to a structure holding parameters which
**                      describe this Association.
**      association     Caller handle for thsi association that is created
**                      by this function.
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
DUL_ReceiveAssociationRQ(
  DUL_NETWORKKEY ** callerNetworkKey,
  DUL_BLOCKOPTIONS block,
  int timeout,
  DUL_ASSOCIATESERVICEPARAMETERS * params,
  DUL_ASSOCIATIONKEY ** callerAssociation,
  int activatePDUStorage)
{
    PRIVATE_NETWORKKEY
        ** network;
    PRIVATE_ASSOCIATIONKEY
        ** association;
    unsigned char
        pduType;
    int
        event;
    DUL_ABORTITEMS
        abortItems;

    network = (PRIVATE_NETWORKKEY **) callerNetworkKey;
    association = (PRIVATE_ASSOCIATIONKEY **) callerAssociation;
    OFCondition cond = checkNetwork(network);
    if (cond.bad()) return cond;

    if (((*network)->applicationFunction & DICOM_APPLICATION_ACCEPTOR) == 0) {
        return DUL_ILLEGALACCEPT;
    }

    if (params->maxPDU < MIN_PDU_LENGTH || params->maxPDU > MAX_PDU_LENGTH)
        return makeDcmnetCondition(DULC_ILLEGALPARAMETER, OF_error, "DUL Illegal parameter (maxPDU) in function DUL_ReceiveAssociationRQ");

    cond = createAssociationKey(network, "", params->maxPDU, association);
    if (cond.bad())
        return cond;

    if (activatePDUStorage) DUL_activateAssociatePDUStorage(*association);
    clearRequestorsParams(params);

    cond = receiveTransportConnection(network, block, timeout, params, association);

    if (cond.bad() || (cond.code() == DULC_FORKEDCHILD))
    {
        destroyAssociationKey(association);
        *association = NULL;
        return cond;
    }

    cond = PRV_StateMachine(network, association,
                  TRANS_CONN_INDICATION, (*network)->protocolState, params);
    if (cond.bad())
        return cond;

    cond = PRV_NextPDUType(association, block, timeout, &pduType);

    if (cond == DUL_NETWORKCLOSED)
        event = TRANS_CONN_CLOSED;
    else if (cond == DUL_READTIMEOUT)
        event = ARTIM_TIMER_EXPIRED;
    else if (cond.bad())
        return cond;
    else {
        switch (pduType) {
        case DUL_TYPEASSOCIATERQ:
            event = A_ASSOCIATE_RQ_PDU_RCV;
            break;
        case DUL_TYPEASSOCIATEAC:
            event = A_ASSOCIATE_AC_PDU_RCV;
            break;
        case DUL_TYPEASSOCIATERJ:
            event = A_ASSOCIATE_RJ_PDU_RCV;
            break;
        case DUL_TYPEDATA:
            event = P_DATA_TF_PDU_RCV;
            break;
        case DUL_TYPERELEASERQ:
            event = A_RELEASE_RQ_PDU_RCV;
            break;
        case DUL_TYPERELEASERP:
            event = A_RELEASE_RP_PDU_RCV;
            break;
        case DUL_TYPEABORT:
            event = A_ABORT_PDU_RCV;
            break;
        default:
            event = INVALID_PDU;
            break;
        }
    }
    cond = PRV_StateMachine(network, association, event,
                            (*association)->protocolState, params);
    if (cond == DUL_UNSUPPORTEDPEERPROTOCOL) {
        abortItems.result = DUL_REJECT_PERMANENT;
        abortItems.source = DUL_ULSP_ACSE_REJECT;
        abortItems.reason = DUL_ULSP_ACSE_UNSUP_PROTOCOL;
        (void) PRV_StateMachine(NULL, association,
                                A_ASSOCIATE_RESPONSE_REJECT,
                                (*association)->protocolState, &abortItems);
    }
    return cond;
}



/* DUL_AcknowledgeAssociationRQ
**
** Purpose:
**      This function is used by an Acceptor to acknowledge an Association
**      Request.  The caller passes the partially completed Association Key
**      and the Association Items (syntax items) which describe the Association.
**      This function constructs an Acknowledge PDU and sends it to the
**      Requestor.  At this point, an Association is established.
**
** Parameter Dictionary:
**      association     Caller's handle to the Association that is to be
**                      acknowledged and hence established.
**      params          Parameters that describe the type of service handled
**                      by this Association.
**
** Return Values:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
DUL_AcknowledgeAssociationRQ(
  DUL_ASSOCIATIONKEY ** callerAssociation,
  DUL_ASSOCIATESERVICEPARAMETERS * params,
  int activatePDUStorage)
{
    PRIVATE_ASSOCIATIONKEY
        ** association;

    association = (PRIVATE_ASSOCIATIONKEY **) callerAssociation;
    OFCondition cond = checkAssociation(association);
    if (cond.bad()) return cond;

    if (activatePDUStorage) DUL_activateAssociatePDUStorage(*association);

    cond = PRV_StateMachine(NULL, association,
        A_ASSOCIATE_RESPONSE_ACCEPT, (*association)->protocolState, params);

    return cond;

}


/* DUL_RejectAssociationRQ
**
** Purpose:
**      This function is used to reject an Association Request and destroy
**      the corresponding Association Key.  The caller indicates if the
**      reject is permanent or transient and the reason for the reject.
**      This function constructs a Reject PDU and sends it to the connected
**      peer.  The Association Key is then destroyed and the caller's reference
**      to the key destroyed to prevent misuse.
**
** Parameter Dictionary:
**      association     Caller's handle to the association that is to be
**                      rejected.
**      params          Pointer to a structure that gives the reasons for
**                      rejecting this Association.
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
DUL_RejectAssociationRQ(
  DUL_ASSOCIATIONKEY ** callerAssociation,
  DUL_ABORTITEMS * params,
  int activatePDUStorage)
{
    PRIVATE_ASSOCIATIONKEY
        ** association;
    DUL_ABORTITEMS
        localParams;

    association = (PRIVATE_ASSOCIATIONKEY **) callerAssociation;
    OFCondition cond = checkAssociation(association);
    if (cond.bad()) return cond;

    if (activatePDUStorage) DUL_activateAssociatePDUStorage(*association);

    localParams = *params;
    localParams.source = 0x01;
    {
        unsigned char diagtable[] = {0x01, 0x02, 0x03, 0x07};
        int l_index;
        OFBool found = OFFalse;
        for (l_index = 0; l_index < (int) DIM_OF(diagtable) && !found; l_index++)
            found = (localParams.reason == diagtable[l_index]);

        if (!found)
        {
            OFOStringStream stream;
            stream << "DUL Illegal reason for rejecting Association: "
                   << STD_NAMESPACE hex << localParams.reason << OFStringStream_ends;
            OFSTRINGSTREAM_GETOFSTRING(stream, msg)
            return makeDcmnetCondition(DULC_ILLEGALREJECTREASON, OF_error, msg.c_str());
        }
    }
    {
        unsigned char resulttable[] = {0x01, 0x02};
        int l_index;
        OFBool found = OFFalse;
        for (l_index = 0; l_index < (int) DIM_OF(resulttable) && !found; l_index++)
            found = (localParams.result == resulttable[l_index]);

        if (!found)
        {
            OFOStringStream stream;
            stream << "DUL Illegal result for rejecting Association: "
                   << STD_NAMESPACE hex << localParams.result << OFStringStream_ends;
            OFSTRINGSTREAM_GETOFSTRING(stream, msg)
            return makeDcmnetCondition(DULC_ILLEGALREJECTRESULT, OF_error, msg.c_str());
        }
    }
    cond = PRV_StateMachine(NULL, association,
                            A_ASSOCIATE_RESPONSE_REJECT, (*association)->protocolState, &localParams);

    return cond;
}


/* DUL_DropAssociation
**
** Purpose:
**      This function drops an Association without notifying the peer
**      application and destroys the Association Key.  The caller's
**      reference to the Association Key is also destroyed to prevent misuse.
**      This routine is called if a peer disconnects from you or if you
**      want to drop an Association without notifying the peer (can be
**      used in the unix environment if you use the fork call and have
**      two open channels to the peer).
**
** Parameter Dictionary:
**      callerAssociation       Caller's handle to the Association that is
**                              to be dropped.
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
DUL_DropAssociation(DUL_ASSOCIATIONKEY ** callerAssociation)
{
    PRIVATE_ASSOCIATIONKEY
        ** association;

    association = (PRIVATE_ASSOCIATIONKEY **) callerAssociation;
    OFCondition cond = checkAssociation(association);
    if (cond.bad()) return cond;

    if ((*association)->connection)
    {
     (*association)->connection->close();
     delete (*association)->connection;
     (*association)->connection = NULL;
    }
    destroyAssociationKey(association);
    return EC_Normal;
}


/* DUL_ReleaseAssociation
**
** Purpose:
**      Request the orderly release of an Association.
**
** Parameter Dictionary:
**      callerAssociation       Caller's handle to the Association to be
**                              released.
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
DUL_ReleaseAssociation(DUL_ASSOCIATIONKEY ** callerAssociation)
{
    PRIVATE_ASSOCIATIONKEY
        ** association;
    unsigned char
        pduType;
    int
        event;

    association = (PRIVATE_ASSOCIATIONKEY **) callerAssociation;
    OFCondition cond = checkAssociation(association);
    if (cond.bad()) return cond;

    cond = PRV_StateMachine(NULL, association,
                        A_RELEASE_REQ, (*association)->protocolState, NULL);
    if (cond.bad())
        return cond;

    cond = PRV_NextPDUType(association, DUL_NOBLOCK, PRV_DEFAULTTIMEOUT, &pduType);
    if (cond == DUL_NETWORKCLOSED)
        event = TRANS_CONN_CLOSED;
    else if (cond == DUL_READTIMEOUT)
        event = ARTIM_TIMER_EXPIRED;
    else if (cond.bad())
        return cond;
    else {
        switch (pduType) {
        case DUL_TYPEASSOCIATERQ:
            event = A_ASSOCIATE_RQ_PDU_RCV;
            break;
        case DUL_TYPEASSOCIATEAC:
            event = A_ASSOCIATE_AC_PDU_RCV;
            break;
        case DUL_TYPEASSOCIATERJ:
            event = A_ASSOCIATE_RJ_PDU_RCV;
            break;
        case DUL_TYPEDATA:
            event = P_DATA_TF_PDU_RCV;
            break;
        case DUL_TYPERELEASERQ:
            event = A_RELEASE_RQ_PDU_RCV;
            break;
        case DUL_TYPERELEASERP:
            event = A_RELEASE_RP_PDU_RCV;
            break;
        case DUL_TYPEABORT:
            event = A_ABORT_PDU_RCV;
            break;
        default:
            event = INVALID_PDU;
            break;
        }
    }
    return PRV_StateMachine(NULL, association, event,(*association)->protocolState, NULL);
}

/* DUL_AcknowledgeRelease
**
** Purpose:
**      Send Acknowledgement to the Release request
**
** Parameter Dictionary:
**      callerAssociation       Caller's handle to the Association whose
**                              release is acknowledged.
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
DUL_AcknowledgeRelease(DUL_ASSOCIATIONKEY ** callerAssociation)
{
    PRIVATE_ASSOCIATIONKEY
        ** association;

    association = (PRIVATE_ASSOCIATIONKEY **) callerAssociation;
    OFCondition cond = checkAssociation(association);
    if (cond.bad()) return cond;

    cond = PRV_StateMachine(NULL, association,
                       A_RELEASE_RESP, (*association)->protocolState, NULL);
    if (cond.bad())
        return cond;
//    cond = PRV_StateMachine(NULL, association,
//                ARTIM_TIMER_EXPIRED, (*association)->protocolState, NULL);
    return cond;
}



/* DUL_AbortAssociation
**
** Purpose:
**      Abort an association by sending an A-ABORT PDU and waiting for the
**      network to close.
**
** Parameter Dictionary:
**      callerAssociation       The handle for the association to be
**                              aborted.
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
DUL_AbortAssociation(DUL_ASSOCIATIONKEY ** callerAssociation)
{
    DUL_ABORTITEMS abortItems = { 0, DUL_SCU_INITIATED_ABORT, 0 };
    int event = 0;
    unsigned char pduType = 0;

    PRIVATE_ASSOCIATIONKEY ** association = (PRIVATE_ASSOCIATIONKEY **) callerAssociation;
    OFCondition cond = checkAssociation(association);
    if (cond.bad()) return cond;

    cond = PRV_StateMachine(NULL, association, A_ABORT_REQ, (*association)->protocolState, &abortItems);
    if (cond.bad()) return cond;

    OFBool done = OFFalse;
    while (!done)
    {
        cond = PRV_NextPDUType(association, DUL_NOBLOCK, PRV_DEFAULTTIMEOUT, &pduType); // may return DUL_NETWORKCLOSED.

        if (cond == DUL_NETWORKCLOSED) event = TRANS_CONN_CLOSED;
        else if (cond == DUL_READTIMEOUT) event = ARTIM_TIMER_EXPIRED;
        else
        {
            switch (pduType)
            {
              case DUL_TYPEASSOCIATERQ:
                event = A_ASSOCIATE_RQ_PDU_RCV;
                break;
              case DUL_TYPEASSOCIATEAC:
                event = A_ASSOCIATE_AC_PDU_RCV;
                break;
              case DUL_TYPEASSOCIATERJ:
                event = A_ASSOCIATE_RJ_PDU_RCV;
                break;
              case DUL_TYPEDATA:
                event = P_DATA_TF_PDU_RCV;
                break;
              case DUL_TYPERELEASERQ:
                event = A_RELEASE_RQ_PDU_RCV;
                break;
              case DUL_TYPERELEASERP:
                event = A_RELEASE_RP_PDU_RCV;
                break;
              case DUL_TYPEABORT:
                event = A_ABORT_PDU_RCV;
                break;
              default:
                event = INVALID_PDU;
                break;
            }
        }

        cond = PRV_StateMachine(NULL, association, event, (*association)->protocolState, NULL);
        if (cond.bad()) // cond may be DUL_NETWORKCLOSED.
        {
          if (cond == DUL_NETWORKCLOSED) event = TRANS_CONN_CLOSED;
          else if (cond == DUL_READTIMEOUT) event = ARTIM_TIMER_EXPIRED;
          else event = INVALID_PDU;
          cond = PRV_StateMachine(NULL, association, event, (*association)->protocolState, NULL);
        }
        if (cond.good()) done = OFTrue;
    }
    return EC_Normal;
}



/* DUL_WritePDVs
**
** Purpose:
**      Write a list of PDVs on an active Association.
**
** Parameter Dictionary:
**      callerAssociation       Caller's handle to the Association
**      pdvList                 Pointer to a structure which describes
**                              the list of PDVs to be written on the active
**                              Association.
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
DUL_WritePDVs(DUL_ASSOCIATIONKEY ** callerAssociation,
              DUL_PDVLIST * pdvList)
{
    PRIVATE_ASSOCIATIONKEY
        ** association;

    /* assign association to local variable */
    association = (PRIVATE_ASSOCIATIONKEY **) callerAssociation;

    /* check if association is valid, if not return an error */
    OFCondition cond = checkAssociation(association);
    if (cond.bad()) return cond;

    /* call the finite state machine to invoke an action function given the current */
    /* event (P_DATA_REQ) and state (captured in (*association)->protocolState) */
    cond = PRV_StateMachine(NULL, association, P_DATA_REQ,
                            (*association)->protocolState, pdvList);

    return cond;
}


/* DUL_ReadPDVs
**
** Purpose:
**      Read the next available PDU.
**
** Parameter Dictionary:
**      callerAssociation       Caller's handle for the Association.
**      pdvList                 Pointer to a structure which describes
**                              the list of PDVs to be read from the active
**                              Association.
**      block                   Option used for blocking/non-blocking read.
**      timeout                 Timeout interval.
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
DUL_ReadPDVs(DUL_ASSOCIATIONKEY ** callerAssociation,
             DUL_PDVLIST * pdvList, DUL_BLOCKOPTIONS block, int timeout)
{
    PRIVATE_ASSOCIATIONKEY
        ** association;
    unsigned char
        pduType;
    int
        event;

    /* assign the association to local variable */
    association = (PRIVATE_ASSOCIATIONKEY **) callerAssociation;

    /* check if association is valid, if not return an error */
    OFCondition cond = checkAssociation(association);
    if (cond.bad()) return cond;

    /* determine the type of the next PDU (in case the association */
    /* does not contain PDU header information yet, we need to try */
    /* to receive PDU header information on the network) */
    cond = PRV_NextPDUType(association, block, timeout, &pduType);

    /* evaluate the return value of the above called function */
    if (cond == DUL_NETWORKCLOSED) event = TRANS_CONN_CLOSED;
    else if ((cond == DUL_READTIMEOUT) && (block == DUL_NOBLOCK)) return cond;
    else if (cond == DUL_READTIMEOUT) event = ARTIM_TIMER_EXPIRED;
    else if (cond.bad()) return cond;
    else
    {
        /* if we successfully determined the type of the next PDU, */
        /* we need to set the event variable correspondingly */
        switch (pduType)
        {
          case DUL_TYPEASSOCIATERQ:
            event = A_ASSOCIATE_RQ_PDU_RCV;
            break;
          case DUL_TYPEASSOCIATEAC:
            event = A_ASSOCIATE_AC_PDU_RCV;
            break;
          case DUL_TYPEASSOCIATERJ:
            event = A_ASSOCIATE_RJ_PDU_RCV;
            break;
          case DUL_TYPEDATA:
            event = P_DATA_TF_PDU_RCV;
            break;
          case DUL_TYPERELEASERQ:
            event = A_RELEASE_RQ_PDU_RCV;
            break;
          case DUL_TYPERELEASERP:
            event = A_RELEASE_RP_PDU_RCV;
            break;
          case DUL_TYPEABORT:
            event = A_ABORT_PDU_RCV;
            break;
          default:
            event = INVALID_PDU;
            break;
        }
    }

    /* call the finite state machine to invoke an action function given the current */
    /* event and state (captured in (*association)->protocolState) */
    cond = PRV_StateMachine(NULL, association, event,
                            (*association)->protocolState, pdvList);

    /* return result value */
    return cond;
}


/* DUL_AssociationParameter
**
** Purpose:
**      This function examines the Association parameters and returns
**      a single parameter requested by the caller.
**
** Parameter Dictionary:
**      callerAssociation       Caller's handle to the Association.
**      param                   The only supported value is DUL_K_MAX_PDV_XMIT.
**      type                    The TYPE of the parameter requested.
**      address                 Requested parameter to be obtained from the
**                              Association key.
**      length                  The size of the parameter requested.
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
DUL_AssociationParameter(DUL_ASSOCIATIONKEY ** callerAssociation,
                         DUL_ASSOCIATION_PARAMETER param, DUL_DATA_TYPE type,
                         void *address, size_t length)
{
    PRIVATE_ASSOCIATIONKEY
        ** association;

    association = (PRIVATE_ASSOCIATIONKEY **) callerAssociation;
    OFCondition cond = checkAssociation(association);
    if (cond.bad()) return cond;

    switch (param)
    {
    case DUL_K_MAX_PDV_XMIT:
        if ((*association)->applicationFunction == DICOM_APPLICATION_ACCEPTOR)
        {
            cond = get_association_parameter(
                                         &((*association)->maxPDVRequestor),
                     DUL_K_INTEGER, sizeof((*association)->maxPDVRequestor),
                                             type, address, length);
        } else {
            cond = get_association_parameter(
                                          &((*association)->maxPDVAcceptor),
                      DUL_K_INTEGER, sizeof((*association)->maxPDVAcceptor),
                                             type, address, length);
        }
        if (cond.bad()) return cond;
        break;
    }
    return cond;
}


/* DUL_NextPDV
**
** Purpose:
**      Return the next PDV to the caller.
**
** Parameter Dictionary:
**      callerAssociation       Caller's handle to the Association
**      pdv                     The next PDU to be returned
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
DUL_NextPDV(DUL_ASSOCIATIONKEY ** callerAssociation, DUL_PDV * pdv)
{
    PRIVATE_ASSOCIATIONKEY
        ** association;
    unsigned char
       *p;
    unsigned long
        pdvLength;

    /* assign the association to local variable */
    association = (PRIVATE_ASSOCIATIONKEY **) callerAssociation;

    /* check if association is valid, if not return an error */
    OFCondition cond = checkAssociation(association);
    if (cond.bad()) return cond;

    /* check if the association does currently not contain any */
    /* (unprocessed) PDVs, return a corresponding result value */
    if ((*association)->pdvIndex == -1)
     /* in this special case we want to avoid a message on the
        condition stack because this is no real error, but normal
        behaviour - A callback function registered in the condition stack
        would (unnecessarily) be called once for each PDV.
        the #ifdef allows to mimick the old behaviour.
      */
    return DUL_NOPDVS;

    /* if there is an unprocessed PDV, assign it to the out parameter */
    *pdv = (*association)->currentPDV;

    /* indicate that the current PDV was read by increasing the association's PDV index variable */
    (*association)->pdvIndex++;

    /* if the association's pdvIndex does now show a value equal to or greater */
    /* than pdvCount there are no more unprocessed PDVs contained in the association. */
    /* Indicate this fact by setting the association's pdvIndex to -1.*/
    if ((*association)->pdvIndex >= (*association)->pdvCount)
        (*association)->pdvIndex = -1;
    else {
        /* if the association still contains unprocessed PDVs, we need to reset the association's */
        /* currentPDV variable so that it contains the data of the next unprocessed PDV */
        /* (currentPDV shall always contain the next unprocessed PDV.) */

        /* The variable (*association)->pdvPointer always points to the buffer address */
        /* where the information of the PDV which is represented by the association's */
        /* currentPDV variable can be found. Remember this address in a local variable. */
        p = (*association)->pdvPointer;

        /* move this pointer to the next (unprocessed) PDV which is contained in the association. */
        /* (We need to move the pointer 4 bytes to the right (over the PDV length field of the */
        /* current PDV), another 2 bytes to the right (over the presentation context ID and */
        /* message control header field of the current PDV) and another currentPDV.fragmentLength */
        /* bytes to the right (over the actual data fragment of the current PDV). */
        p += (*association)->currentPDV.fragmentLength + 2 + 4;

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

        /* and finally, after the association's currentPDV variable has been completely set to the next */
        /* (unprocessed) PDV, we need to update the association's pdvPointer, so that it again points */
        /* to the address of the PDV which is represented by the association's currentPDV variable. */
        (*association)->pdvPointer += 4 + pdvLength;
    }
    return EC_Normal;
}


/* DUL_ClearServiceParameters
**
** Purpose:
**      DUL_ClearServiceParameters is used to clear the lists that were
**      constructed in a DUL_ASSOCIATEDSERVICEPARAMETERS structure.  The
**      lists in that structure were built to contain the requested
**      and accepted Presentation Contexts.  Once the lists were built,
**      an Association request could be made.  After the Association
**      has been closed, the Presentation Contexts need to be discarded
**      so that the same structure can be reused for a new Association.
**      This function removes all of the nodes from the requested
**      and accepted Presentation Context lists and then destroys the
**      lists themselves.
**
** Parameter Dictionary:
**      params  Pointer to a DUL_ASSOCIATESERVICEPARAMETERS structure
**              to be cleared.
**
** Return Values:
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
DUL_ClearServiceParameters(DUL_ASSOCIATESERVICEPARAMETERS * params)
{
    // Clear requested and accepted presentation contexts
    clearPresentationContext(&params->requestedPresentationContext);
    clearPresentationContext(&params->acceptedPresentationContext);
    // Delete structures of Extended SOP Class Negotiation request
    if (params->requestedExtNegList)
    {
      deleteListMembers(*params->requestedExtNegList);
      delete params->requestedExtNegList; params->requestedExtNegList = NULL;
    }
    // Delete structures of Extended SOP Class Negotiation acknowledge
    if (params->acceptedExtNegList)
    {
      deleteListMembers(*params->acceptedExtNegList);
      delete params->acceptedExtNegList; params->acceptedExtNegList = NULL;
    }
    // Delete User Identity Negotiation request and ack structures
    delete params->reqUserIdentNeg; params->reqUserIdentNeg = NULL;
    delete params->ackUserIdentNeg; params->ackUserIdentNeg = NULL;
    return EC_Normal;
}

/* DUL_DefaultServiceParameters
**
** Purpose:
**      DUL_DefaultServiceParameters is used to set a number of default
**      parameters in a DUL_ASSOCIATESERVICEPARAMETERS structure.  These
**      parameters are the default MIR parameters (which should be
**      adequate for most Unix installations).
**
** Parameter Dictionary:
**      params  Pointer to a DUL_ASSOCIATESERVICEPARAMETERS structure
**              to be set.
**
** Return Values:
**      none
** Notes:
**
** Algorithm:
*/

void
DUL_DefaultServiceParameters(DUL_ASSOCIATESERVICEPARAMETERS * params)
{
    static DUL_ASSOCIATESERVICEPARAMETERS p = {
        DICOM_STDAPPLICATIONCONTEXT,    /* Application Ctx Name */
        "Calling AP Title",     /* Calling AP Title */
        "Called AP Title",      /* Called AP Title */
        "",                     /* Responding AP Title */
        16384,                  /* Max PDU */
        0,                      /* result */
        0,                      /* result source */
        0,                      /* diagnostic */
        "localhost",            /* Calling presentation addr */
        "localhost:104",        /* Called presentation addr */
        NULL,                   /* Requested presentation ctx list */
        NULL,                   /* Accepted presentation ctx list */
        0,                      /* Maximum operations invoked */
        0,                      /* Maximum operations performed */
        DICOM_NET_IMPLEMENTATIONCLASSUID, /* Calling implementation class UID */
        DICOM_NET_IMPLEMENTATIONVERSIONNAME, /* Calling implementation vers name */
        "",                     /* Called implementation class UID */
        "",                     /* Called implementation vers name */
        0,                      /* peer max pdu */
        NULL,                   /* Requested Extended Negotation List */
        NULL,                   /* Accepted Extended Negotation List */
        NULL,                   /* Requested User Identify Negotiation */
        NULL,                   /* Accepted User Identify Negotiation */
        OFFalse                 /* don't use Secure Transport Layer */
    };

    *params = p;
}

/*      ==============================================================
 *  Private functions not meant for users of the package are included below.
 */


/* receiveTransportConnection
**
** Purpose:
**      Receive a transport connection and fill in various fields of the
**      service parameters and Association handle.
** Parameter Dictionary:
**      network         Pointer to a structure maintaining information about
**                      the network environment.
**      block           Option indicating blocking/non-blocking mode
**      timeout         When blocking mode is non blocking, the timeout in seconds
**      params          Pointer to structure describing the services for the
**                      Association.
**      association     Handle to the association
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
receiveTransportConnection(PRIVATE_NETWORKKEY ** network,
                           DUL_BLOCKOPTIONS block,
                           int timeout,
                           DUL_ASSOCIATESERVICEPARAMETERS * params,
                           PRIVATE_ASSOCIATIONKEY ** association)
{
  return receiveTransportConnectionTCP(network, block, timeout, params, association);
}


/* receiveTransportConnectionTCP
**
** Purpose:
**      Receive a TCP transport connection and fill in the fields of the
**      service parameters and the Association handle.
**
** Parameter Dictionary:
**      network         Pointer to a structure maintaining information about
**                      the network environment.
**      block           Option indicating blocking/non-blocking mode
**      timeout         When blocking mode is non blocking, the timeout in seconds
**      params          Pointer to structure describing the services for the
**                      Association.
**      association     Handle to the association
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
receiveTransportConnectionTCP(PRIVATE_NETWORKKEY ** network,
                              DUL_BLOCKOPTIONS block,
                              int timeout,
                              DUL_ASSOCIATESERVICEPARAMETERS * params,
                              PRIVATE_ASSOCIATIONKEY ** association)
{
    fd_set
    fdset;
    struct timeval timeout_val;
#ifdef HAVE_DECLARATION_SOCKLEN_T
    socklen_t len;
#elif !defined(HAVE_PROTOTYPE_ACCEPT) || defined(HAVE_INTP_ACCEPT)
    int len;
#else
    size_t len;
#endif
    int nfound, connected;
    struct sockaddr from;
    struct hostent *remote = NULL;
    struct linger sockarg;

#ifdef HAVE_FORK
    pid_t pid = -1;
#endif

    (void) memset(&sockarg, 0, sizeof(sockarg));

    int reuse = 1;

    int sock = dcmExternalSocketHandle.get();
    if (sock > 0)
    {
      // use the socket file descriptor provided to us externally
      // instead of calling accept().
      connected = 1;

      len = sizeof(from);
      if (getpeername(sock, &from, &len))
      {
          char buf[256];
          OFOStringStream stream;
          stream << "TCP Initialization Error: " << OFStandard::strerror(errno, buf, sizeof(buf))
                 << ", getpeername failed on socket " << sock << OFStringStream_ends;
          OFSTRINGSTREAM_GETOFSTRING(stream, msg)
          return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
      }
    }
    else
    {
        if (block == DUL_NOBLOCK)
        {
            connected = 0;
            FD_ZERO(&fdset);
#ifdef __MINGW32__
            // on MinGW, FD_SET expects an unsigned first argument
            FD_SET((unsigned int)((*network)->networkSpecific.TCP.listenSocket), &fdset);
#else
            FD_SET((*network)->networkSpecific.TCP.listenSocket, &fdset);
#endif

            timeout_val.tv_sec = timeout;
            timeout_val.tv_usec = 0;
#ifdef HAVE_INTP_SELECT
            nfound = select((*network)->networkSpecific.TCP.listenSocket + 1,
                            (int *)(&fdset), NULL, NULL, &timeout_val);
#else
            nfound = select((*network)->networkSpecific.TCP.listenSocket + 1,
                            &fdset, NULL, NULL, &timeout_val);
#endif
            if (nfound > 0) {
                if (FD_ISSET((*network)->networkSpecific.TCP.listenSocket, &fdset))
                    connected++;
            }
            if (!connected) return DUL_NOASSOCIATIONREQUEST;
        }
        else
        {
            connected = 0;
            do {
                FD_ZERO(&fdset);
#ifdef __MINGW32__
                // on MinGW, FD_SET expects an unsigned first argument
                FD_SET((unsigned int)((*network)->networkSpecific.TCP.listenSocket), &fdset);
#else
                FD_SET((*network)->networkSpecific.TCP.listenSocket, &fdset);
#endif

                timeout_val.tv_sec = 5;
                timeout_val.tv_usec = 0;
#ifdef HAVE_INTP_SELECT
                nfound = select((*network)->networkSpecific.TCP.listenSocket + 1,
                                (int *)(&fdset), NULL, NULL, &timeout_val);
#else
                nfound = select((*network)->networkSpecific.TCP.listenSocket + 1,
                                &fdset, NULL, NULL, &timeout_val);
#endif
                if (nfound > 0) {
                    if (FD_ISSET((*network)->networkSpecific.TCP.listenSocket,
                                 &fdset))
                        connected++;
                }
            } while (!connected);
        }

        len = sizeof(from);
        do
        {
            sock = accept((*network)->networkSpecific.TCP.listenSocket, &from, &len);
        } while (sock == -1 && errno == EINTR);

        if (sock < 0)
        {
            char buf[256];
            OFOStringStream stream;
            stream << "TCP Initialization Error: " << OFStandard::strerror(errno, buf, sizeof(buf))
                   << ", accept failed on socket " << sock << OFStringStream_ends;
            OFSTRINGSTREAM_GETOFSTRING(stream, msg)
            return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
        }
    }

#ifdef HAVE_FORK
    if (shouldFork)
    {
        // fork for unix
        pid = fork();
        if (pid > 0)
        {
            // we're the parent process, close accepted socket and return
            close(sock);

            OFOStringStream stream;
            stream << "New child process started with pid " << pid << OFStringStream_ends;
            OFSTRINGSTREAM_GETOFSTRING(stream, msg)
            return makeDcmnetCondition(DULC_FORKEDCHILD, OF_ok, msg.c_str());
        }
        else if (pid < 0)
        {
            // fork failed, return error code
            close(sock);

            char buf[256];
            OFString msg = "Multi-Process Error: ";
            msg += OFStandard::strerror(errno, buf, sizeof(buf));
            msg += ", fork failed";
            return makeDcmnetCondition(DULC_CANNOTFORK, OF_error, msg.c_str());
        }
        else
        {
          // we're the child process, continue normally
          processIsForkedChild = OFTrue;
        }
    }
#elif defined(_WIN32)
    if (shouldFork && (command_argc > 0) && command_argv)
    {
        // win32 code to create new child process
        HANDLE childSocketHandle;
        HANDLE hChildStdInRead;
        HANDLE hChildStdInWrite;
        HANDLE hChildStdInWriteDup;

        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;

        // prepare the command line
        OFString cmdLine = command_argv[0];
        cmdLine += " --forked-child";
        for (int i=1; i < command_argc; ++i)
        {
            /* copy each argv value and surround it with double quotes
             * to keep option values containing a space glued together
             */
            cmdLine += " \"";
            cmdLine += command_argv[i];
            cmdLine += "\"";
        }

        // create anonymous pipe
        if (!CreatePipe(&hChildStdInRead, &hChildStdInWrite, &sa,0))
        {
            OFOStringStream stream;
            stream << "Multi-Process Error: Creating anonymous pipe failed with error code "
                   << GetLastError() << OFStringStream_ends;
            OFSTRINGSTREAM_GETOFSTRING(stream, msg)
            return makeDcmnetCondition(DULC_CANNOTFORK, OF_error, msg.c_str());
        }

        // create duplicate of write end handle of pipe
        if (!DuplicateHandle(GetCurrentProcess(), hChildStdInWrite,
                             GetCurrentProcess(), &hChildStdInWriteDup, 0,
                             FALSE, DUPLICATE_SAME_ACCESS))
        {
            return makeDcmnetCondition(DULC_CANNOTFORK, OF_error, "Multi-Process Error: Duplicating handle failed");
        }

        // destroy original write end handle of pipe
        CloseHandle(hChildStdInWrite);

        // we need a STARTUPINFO and a PROCESS_INFORMATION structure for CreateProcess.
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        memset(&pi,0,sizeof(pi));
        memset(&si,0,sizeof(si));

        // prepare startup info for child process:
        // the child uses the same stdout and stderr as the parent, but
        // stdin is the read end of our anonymous pipe.
        si.cb = sizeof(si);
        si.dwFlags |= STARTF_USESTDHANDLES;
        si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
        si.hStdInput = hChildStdInRead;

        // create child process.
        if (!CreateProcess(NULL,OFconst_cast(char *, cmdLine.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
        {
            OFOStringStream stream;
            stream << "Multi-Process Error: Creating process failed with error code "
                   << GetLastError() << OFStringStream_ends;
            OFSTRINGSTREAM_GETOFSTRING(stream, msg)
            return makeDcmnetCondition(DULC_CANNOTFORK, OF_error, msg.c_str());
        }
        else
        {
            // call OpenProcess to retrieve the REAL process handle.  using
            // GetCurrentProcess() only returns a psuedo handle which may not
            // allow DuplicateHandle to create the child process socket with
            // sufficient permissions on certain versions of Windows.
            HANDLE hParentProcessHandle = OpenProcess(PROCESS_DUP_HANDLE, FALSE, GetCurrentProcessId());
            if (hParentProcessHandle == NULL)
            {
                // unable to get process handle...
                // ...this should really never happen as we are opening the current process.
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                CloseHandle((HANDLE)sock);

                OFOStringStream stream;
                stream << "Multi-Process Error: Opening process to get real process handle failed with error code "
                       << GetLastError() << OFStringStream_ends;
                OFSTRINGSTREAM_GETOFSTRING(stream, msg)
                return makeDcmnetCondition(DULC_CANNOTFORK, OF_error, msg.c_str());
            }

            // PROCESS_INFORMATION pi now contains various handles for the new process.
            // Now that we have a handle to the new process, we can duplicate the
            // socket handle into the new child process.
            if (DuplicateHandle(hParentProcessHandle, (HANDLE)sock, pi.hProcess,
                &childSocketHandle, 0, TRUE, DUPLICATE_SAME_ACCESS))
            {
                // close handles in PROCESS_INFORMATION structure
                // and our local copy of the socket handle.
                CloseHandle(hParentProcessHandle);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                closesocket(sock);

                // send number of socket handle in child process over anonymous pipe
                DWORD bytesWritten;
                char buf[20];
                sprintf(buf, "%i", OFreinterpret_cast(int, childSocketHandle));
                if (!WriteFile(hChildStdInWriteDup, buf, strlen(buf) + 1, &bytesWritten, NULL))
                {
                    CloseHandle(hChildStdInWriteDup);
                    return makeDcmnetCondition(DULC_CANNOTFORK, OF_error, "Multi-Process Error: Writing to anonymous pipe failed");
                }

                // return OF_ok status code DULC_FORKEDCHILD with descriptive text
                OFOStringStream stream;
                stream << "New child process started with pid " << OFstatic_cast(int, pi.dwProcessId)
                       << ", socketHandle " << OFreinterpret_cast(int, childSocketHandle) << OFStringStream_ends;
                OFSTRINGSTREAM_GETOFSTRING(stream, msg)
                return makeDcmnetCondition(DULC_FORKEDCHILD, OF_ok, msg.c_str());
            }
            else
            {
                // unable to duplicate handle. Close handles nevertheless
                // to avoid resource leak.
                CloseHandle(hParentProcessHandle);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                return makeDcmnetCondition(DULC_CANNOTFORK, OF_error, "Multi-Process Error: Duplicating socket handle failed");
            }
        }
    }
#endif

#ifndef HAVE_GUSI_H
    /* GUSI always returns an error for setsockopt() */
    sockarg.l_onoff = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *) &sockarg, sizeof(sockarg)) < 0)
    {
        char buf[256];
        OFOStringStream stream;
        stream << "TCP Initialization Error: " << OFStandard::strerror(errno, buf, sizeof(buf))
               << ", setsockopt failed on socket " << sock << OFStringStream_ends;
        OFSTRINGSTREAM_GETOFSTRING(stream, msg)
        return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
    }
    reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0)
    {
        char buf[256];
        OFString msg = "TCP Initialization Error: ";
        msg += OFStandard::strerror(errno, buf, sizeof(buf));
        return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
    }
#ifndef DISABLE_RECV_TIMEOUT
    /* use a timeout of 60 seconds for the recv() function */
    const int recvTimeout = 60;
    DCMNET_DEBUG("setting network receive timeout to " << recvTimeout << " seconds");
#ifdef HAVE_WINSOCK_H
    // for Windows, specify receive timeout in milliseconds
    int timeoutVal = recvTimeout * 1000;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeoutVal, sizeof(timeoutVal)) < 0)
#else
    // for other systems, specify receive timeout as timeval struct
    struct timeval timeoutVal;
    timeoutVal.tv_sec = recvTimeout;
    timeoutVal.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeoutVal, sizeof(timeoutVal)) < 0)
#endif
    {
        // according to MSDN: available in the Microsoft implementation of Windows Sockets 2,
        // so we are reporting a warning message but are not returning with an error code;
        // this also applies to all other systems where the call to this function might fail
        DCMNET_WARN("cannot set network receive timeout to " << recvTimeout << " seconds");
    }
#endif
#endif
    setTCPBufferLength(sock);

#ifndef DONT_DISABLE_NAGLE_ALGORITHM
    /*
     * Disable the Nagle algorithm.
     * This provides a 2-4 times performance improvement (WinNT4/SP4, 100Mbit/s Ethernet).
     * Effects on other environments are unknown.
     * The code below allows the Nagle algorithm to be enabled by setting the TCP_NODELAY environment
     * variable to have value 0.
     */
    int tcpNoDelay = 1; // disable
    char* tcpNoDelayString = NULL;
    if ((tcpNoDelayString = getenv("TCP_NODELAY")) != NULL) {
        if (sscanf(tcpNoDelayString, "%d", &tcpNoDelay) != 1) {
            DCMNET_WARN("DUL: cannot parse environment variable TCP_NODELAY=" << tcpNoDelayString);
        }
    }
    if (tcpNoDelay) {
        if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&tcpNoDelay, sizeof(tcpNoDelay)) < 0)
        {
            char buf[256];
            OFString msg = "TCP Initialization Error: ";
            msg += OFStandard::strerror(errno, buf, sizeof(buf));
            return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
        }
    }
#endif // DONT_DISABLE_NAGLE_ALGORITHM

    // create string containing numerical IP address.
    OFString client_dns_name;
    char client_ip_address[20];
    sprintf(client_ip_address, "%-d.%-d.%-d.%-d",  // this code is ugly but thread safe
       ((int) from.sa_data[2]) & 0xff,
       ((int) from.sa_data[3]) & 0xff,
       ((int) from.sa_data[4]) & 0xff,
       ((int) from.sa_data[5]) & 0xff);

    if (! dcmDisableGethostbyaddr.get()) remote = gethostbyaddr(&from.sa_data[2], 4, 2);
    if (remote == NULL)
    {
        // reverse DNS lookup disabled or host not found, use numerical address
        OFStandard::strlcpy(params->callingPresentationAddress, client_ip_address,
          sizeof(params->callingPresentationAddress));
        OFStandard::strlcpy((*association)->remoteNode, client_ip_address, sizeof((*association)->remoteNode));
    }
    else
    {
        client_dns_name = remote->h_name;

        char node[128];
        if ((*network)->options & DUL_FULLDOMAINNAME)
            OFStandard::strlcpy(node, remote->h_name, sizeof(node));
        else {
            if (sscanf(remote->h_name, "%[^.]", node) != 1)
                node[0] = '\0';
        }

        OFStandard::strlcpy((*association)->remoteNode, node, sizeof((*association)->remoteNode));
        OFStandard::strlcpy(params->callingPresentationAddress, node, sizeof(params->callingPresentationAddress));
    }

#ifdef WITH_TCPWRAPPER
    const char *daemon = dcmTCPWrapperDaemonName.get();
    if (daemon)
    {
        // enforce access control using the TCP wrapper - see hosts_access(5).

        // if reverse DNS lookup is disabled, use default value
        if (client_dns_name.empty()) client_dns_name = STRING_UNKNOWN;

        struct request_info request;
        request_init(&request, RQ_CLIENT_NAME, client_dns_name.c_str(), 0);
        request_set(&request, RQ_CLIENT_ADDR, client_ip_address, 0);
        request_set(&request, RQ_USER, STRING_UNKNOWN, 0);
        request_set(&request, RQ_DAEMON, daemon, 0);

        if (! hosts_access(&request))
        {
#ifdef HAVE_WINSOCK_H
          (void) shutdown(sock,  1 /* SD_SEND */);
          (void) closesocket(sock);
#else
          (void) close(sock);
#endif
          OFOStringStream stream;
          stream << "TCP wrapper: denied connection from " << client_dns_name
                 << " (" << client_ip_address << ")" << OFStringStream_ends;
          OFSTRINGSTREAM_GETOFSTRING(stream, msg)
          return makeDcmnetCondition(DULC_TCPWRAPPER, OF_error, msg.c_str());
        }
    }
#endif

    if ((*association)->connection) delete (*association)->connection;

    if ((*network)->networkSpecific.TCP.tLayer)
    {
      (*association)->connection = ((*network)->networkSpecific.TCP.tLayer)->createConnection(sock, params->useSecureLayer);
    }
    else (*association)->connection = NULL;

    if ((*association)->connection == NULL)
    {
#ifdef HAVE_WINSOCK_H
      (void) shutdown(sock,  1 /* SD_SEND */);
      (void) closesocket(sock);
#else
      (void) close(sock);
#endif
      char buf[256];
      OFString msg = "TCP Initialization Error: ";
      msg += OFStandard::strerror(errno, buf, sizeof(buf));
      return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
    }

    DcmTransportLayerStatus tcsStatus;
    if (TCS_ok != (tcsStatus = (*association)->connection->serverSideHandshake()))
    {
      OFString msg = "DUL secure transport layer: ";
      msg += (*association)->connection->errorString(tcsStatus);
      return makeDcmnetCondition(DULC_TLSERROR, OF_error, msg.c_str());
    }

    return EC_Normal;
}


/* createNetworkKey
**
** Purpose:
**      Create a network key for the network environment.
**
** Parameter Dictionary:
**      mode            Role played by the application entity
**      timeout         The timeout value to be used for timers
**      key             Pointer to the structure maintaining the
**                      entire information of the network environment.
**                      This is returned back to the caller.
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
createNetworkKey(const char *mode,
                 int timeout, unsigned long opt, PRIVATE_NETWORKKEY ** key)
{
    if (strcmp(mode, AE_REQUESTOR) != 0 &&
        strcmp(mode, AE_ACCEPTOR) != 0 &&
        strcmp(mode, AE_BOTH) != 0)
    {
        OFString msg = "Unrecognized Network Mode: ";
        msg += mode;
        return makeDcmnetCondition(DULC_ILLEGALPARAMETER, OF_error, msg.c_str());
    }
    *key = (PRIVATE_NETWORKKEY *) malloc(sizeof(PRIVATE_NETWORKKEY));
    if (*key == NULL) return EC_MemoryExhausted;
    (void) strcpy((*key)->keyType, KEY_NETWORK);

    (*key)->applicationFunction = 0;

    if (strcmp(mode, AE_REQUESTOR) == 0)
        (*key)->applicationFunction |= DICOM_APPLICATION_REQUESTOR;
    else if (strcmp(mode, AE_ACCEPTOR) == 0)
        (*key)->applicationFunction |= DICOM_APPLICATION_ACCEPTOR;
    else
        (*key)->applicationFunction |=
            DICOM_APPLICATION_ACCEPTOR | DICOM_APPLICATION_REQUESTOR;

    (*key)->networkState = NETWORK_DISCONNECTED;
    (*key)->protocolState = STATE1;

    if (timeout > 0)
        (*key)->timeout = timeout;
    else
        (*key)->timeout = DEFAULT_TIMEOUT;

    (*key)->options = opt;

    return EC_Normal;
}


/* initializeNetworkTCP
**
** Purpose:
**      Create a socket and listen for connections on the port number
**
** Parameter Dictionary:
**      key             Handle to the network environment
**      parameter       port number on which to listen for connection
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
initializeNetworkTCP(PRIVATE_NETWORKKEY ** key, void *parameter)
{
    struct linger
        sockarg;
    int
        reuse = 1;

    (void) memset(&sockarg, 0, sizeof(sockarg));

    // initialize network layer settings to well-defined state
    (*key)->networkSpecific.TCP.tLayer = NULL;
    (*key)->networkSpecific.TCP.tLayerOwned = 0;
    (*key)->networkSpecific.TCP.port = -1;
    (*key)->networkSpecific.TCP.listenSocket = -1;

    // Create listen socket if we're an application acceptor,
    // unless the socket handle has already been passed to us or
    // we are a forked child of an application acceptor, in which
    // case the socket also already exists.
    if ((dcmExternalSocketHandle.get() < 0) &&
        ((*key)->applicationFunction & DICOM_APPLICATION_ACCEPTOR) &&
        (! processIsForkedChild))
    {

#ifdef HAVE_DECLARATION_SOCKLEN_T
    socklen_t length;
#elif !defined(HAVE_PROTOTYPE_ACCEPT) || defined(HAVE_INTP_ACCEPT)
    int length;
#else
    size_t length;
#endif

    struct sockaddr_in server;

/* Create socket for internet type communication */
        (*key)->networkSpecific.TCP.port = *(int *) parameter;
        (*key)->networkSpecific.TCP.listenSocket = socket(AF_INET, SOCK_STREAM, 0);
        if ((*key)->networkSpecific.TCP.listenSocket < 0)
        {
          char buf[256];
          OFString msg = "TCP Initialization Error: ";
          msg += OFStandard::strerror(errno, buf, sizeof(buf));
          return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
        }
        reuse = 1;
#ifdef HAVE_GUSI_H
        /* GUSI always returns an error for setsockopt(...) */
#else
        if (setsockopt((*key)->networkSpecific.TCP.listenSocket,
            SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0)
        {
          char buf[256];
          OFString msg = "TCP Initialization Error: ";
          msg += OFStandard::strerror(errno, buf, sizeof(buf));
          return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
        }
#endif
/* Name socket using wildcards */
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = (unsigned short) htons((*key)->networkSpecific.TCP.port);
        if (bind((*key)->networkSpecific.TCP.listenSocket,
                 (struct sockaddr *) & server, sizeof(server)))
        {
          char buf[256];
          OFString msg = "TCP Initialization Error: ";
          msg += OFStandard::strerror(errno, buf, sizeof(buf));
          return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
        }
        /* Find out assigned port number and print it out */
        length = sizeof(server);
        if (getsockname((*key)->networkSpecific.TCP.listenSocket,
                        (struct sockaddr *) & server, &length))
        {
          char buf[256];
          OFString msg = "TCP Initialization Error: ";
          msg += OFStandard::strerror(errno, buf, sizeof(buf));
          return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
        }
#ifdef HAVE_GUSI_H
        /* GUSI always returns an error for setsockopt(...) */
#else
        sockarg.l_onoff = 0;
        if (setsockopt((*key)->networkSpecific.TCP.listenSocket,
           SOL_SOCKET, SO_LINGER, (char *) &sockarg, sizeof(sockarg)) < 0)
        {
          char buf[256];
          OFString msg = "TCP Initialization Error: ";
          msg += OFStandard::strerror(errno, buf, sizeof(buf));
          return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
        }
#endif
        listen((*key)->networkSpecific.TCP.listenSocket, PRV_LISTENBACKLOG);
    }

    (*key)->networkSpecific.TCP.tLayer = new DcmTransportLayer((*key)->applicationFunction);
    (*key)->networkSpecific.TCP.tLayerOwned = 1;
    if (NULL == (*key)->networkSpecific.TCP.tLayer)
    {
      return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, "Cannot initialize DcmTransportLayer");
    }

    return EC_Normal;
}



/* createAssociationKey
**
** Purpose:
**      Create handle to the Association.
**
** Parameter Dictionary:
**      networkKey              Handle to the network environment
**      remoteNode              The remote node to whcih the association
**                              is to be set up.
**      maxPDU                  Size of the maximum PDU.
**      associationKey          The handle to the Association that is to be
**                              returned.
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
createAssociationKey(PRIVATE_NETWORKKEY ** networkKey,
                     const char *remoteNode, unsigned long maxPDU,
                     PRIVATE_ASSOCIATIONKEY ** associationKey)
{
    PRIVATE_ASSOCIATIONKEY *key;

    key = (PRIVATE_ASSOCIATIONKEY *) malloc(
        size_t(sizeof(PRIVATE_ASSOCIATIONKEY) + maxPDU + 100));
    if (key == NULL) return EC_MemoryExhausted;
    key->receivePDUQueue = NULL;

    (void) strcpy(key->keyType, KEY_ASSOCIATION);
    key->applicationFunction = (*networkKey)->applicationFunction;

    (void) strcpy(key->remoteNode, remoteNode);
    key->presentationContextID = 0;
    key->timeout = (*networkKey)->timeout;
    key->timerStart = 0;
    key->maxPDVInput = maxPDU;
    key->fragmentBufferLength = maxPDU + 100;
    key->fragmentBuffer = (unsigned char *) key + sizeof(*key);

    key->pdvList.count = 0;
    key->pdvList.scratch = key->fragmentBuffer;
    key->pdvList.scratchLength = maxPDU;
    key->pdvList.abort.result = 0x00;
    key->pdvList.abort.source = 0x00;
    key->pdvList.abort.reason = 0x00;
    key->pdvList.pdv = NULL;

    key->inputPDU = NO_PDU;
    key->nextPDUType = 0x00;
    key->nextPDUReserved = 0;
    key->nextPDULength = 0;
    key->compatibilityMode = 0;
    key->pdvCount = 0;
    key->pdvIndex = -1;
    key->pdvPointer = NULL;
    (void) memset(&key->currentPDV, 0, sizeof(key->currentPDV));

    key->associatePDUFlag = 0;
    key->associatePDU = NULL;
    key->associatePDULength = 0;

    key->logHandle = NULL;
    key->connection = NULL;
    key->modeCallback = NULL;
    *associationKey = key;
    return EC_Normal;
}



/* destroyAssociationKey
**
** Purpose:
**      Destroy the handle to the Association.
**
** Parameter Dictionary:
**      key             Handle to the association.
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static void
destroyAssociationKey(PRIVATE_ASSOCIATIONKEY ** key)
{
    if (*key && (*key)->connection) delete (*key)->connection;
    free(*key);
    *key = NULL;
}


/* get_association_parameter
**
** Purpose:
**      Get a single parameter.
**
** Parameter Dictionary:
**      paramAddress            Source parameter
**      paramType               Type of the source parameter
**      paramLength             Size of the source parameter
**      outputType              Type of the destination parameter
**      outputAddress           Destination parameter returned to caller
**      outputLength            Size of the destination parameter
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
get_association_parameter(void *paramAddress,
                          DUL_DATA_TYPE paramType, size_t paramLength,
         DUL_DATA_TYPE outputType, void *outputAddress, size_t outputLength)
{
    if (paramType != outputType) return DUL_WRONGDATATYPE;
    if ((paramType == DUL_K_INTEGER) && (outputLength != paramLength)) return DUL_INCORRECTBUFFERLENGTH;
    if ((paramType == DUL_K_STRING) && (outputLength < strlen((char*)paramAddress))) return DUL_INSUFFICIENTBUFFERLENGTH;

    switch (paramType) {
    case DUL_K_INTEGER:
        (void) memcpy(outputAddress, paramAddress, paramLength);
        break;
    case DUL_K_STRING:
        strcpy((char*)outputAddress, (char*)paramAddress);
        break;
    }
    return EC_Normal;
}


/* setTCPBufferLength
**
** Purpose:
**      Initialize the length of the buffer.
**
** Parameter Dictionary:
**      sock            Socket descriptor.
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
setTCPBufferLength(int sock)
{
    char *TCPBufferLength;
    int bufLen;

    /*
     * Use a 32K default socket buffer length.
     * This value has provided optimal throughput during performance testing.
     * Test environment: Windows NT 4 (SP4), 100Mbit/s Fast Ethernet.
     * Other environments, particularly slower networks may require different values for optimal performance.
     */
#ifdef HAVE_GUSI_H
    /* GUSI always returns an error for setsockopt(...) */
#else
    bufLen = 32768; // a socket buffer size of 32K gives best throughput for image transmission
    if ((TCPBufferLength = getenv("TCP_BUFFER_LENGTH")) != NULL) {
        if (sscanf(TCPBufferLength, "%d", &bufLen) != 1) {
            DCMNET_WARN("DUL: cannot parse environment variable TCP_BUFFER_LENGTH=" << TCPBufferLength);
        }
    }
#if defined(SO_SNDBUF) && defined(SO_RCVBUF)
    (void) setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *) &bufLen, sizeof(bufLen));
    (void) setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *) &bufLen, sizeof(bufLen));
#else
    DCMNET_WARN("DULFSM: setTCPBufferLength: "
        "cannot set TCP buffer length socket option: "
        "code disabled because SO_SNDBUF and SO_RCVBUF constants are unknown");
#endif // SO_SNDBUF and SO_RCVBUF
#endif // HAVE_GUSI_H
}

/* DUL_DumpParams
**
** Purpose:
**      Returns information of various fields of the service parameters.
**
** Parameter Dictionary:
**      params          Pointer to structure holding the service parameters.
**
** Return Values:
**      None
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
OFString&
DUL_DumpParams(OFString& ret_str, DUL_ASSOCIATESERVICEPARAMETERS * params)
{
    OFOStringStream str;
    OFString temp_str;

    str << "APP CTX NAME:" << params->applicationContextName << OFendl;
    str << dump_uid(params->applicationContextName, "%13s");
    str << "AP TITLE:     " << params->callingAPTitle << OFendl
        << "AP TITLE:     " << params->calledAPTitle << OFendl
        << "AP TITLE:     " << params->respondingAPTitle << OFendl
        << "MAX PDU:      " << (int)params->maxPDU << OFendl
        << "Peer MAX PDU: " << (int)params->peerMaxPDU << OFendl
        << "PRES ADDR:    " << params->callingPresentationAddress << OFendl
        << "PRES ADDR:    " << params->calledPresentationAddress << OFendl
        << "REQ IMP UID:  " << params->callingImplementationClassUID << OFendl;
    str << dump_uid(params->callingImplementationClassUID, "%13s");
    str << "REQ VERSION:  " << params->callingImplementationVersionName << OFendl
        << "ACC IMP UID:  " << params->calledImplementationClassUID << OFendl;
    str << dump_uid(params->calledImplementationClassUID, "%13s");
    str << "ACC VERSION:  " << params->calledImplementationVersionName << OFendl
        << "Requested Presentation Ctx" << OFendl;
    str << dump_presentation_ctx(&params->requestedPresentationContext);
    str << "Accepted Presentation Ctx" << OFendl;
    str << dump_presentation_ctx(&params->acceptedPresentationContext);
    if (params->requestedExtNegList != NULL) {
        str << "Requested Extended Negotiation" << OFendl;
        str << dumpExtNegList(temp_str, *params->requestedExtNegList);
    }
    if (params->acceptedExtNegList != NULL) {
        str << "Accepted Extended Negotiation" << OFendl;
        str << dumpExtNegList(temp_str, *params->acceptedExtNegList);
    }
    str << OFStringStream_ends;

    OFSTRINGSTREAM_GETSTR(str, ret)
    ret_str = ret;
    OFSTRINGSTREAM_FREESTR(ret)

    return ret_str;
}

typedef struct {
    DUL_SC_ROLE role;
    const char *text;
}   SC_MAP;

static SC_MAP scMap[] = {
    {DUL_SC_ROLE_NONE, "None"},
    {DUL_SC_ROLE_DEFAULT, "Default"},
    {DUL_SC_ROLE_SCU, "SCU"},
    {DUL_SC_ROLE_SCP, "SCP"},
    {DUL_SC_ROLE_SCUSCP, "SCUSCP"},
};

/* dump_presentation_ctx
**
** Purpose:
**      Display the contents of the presentation context list
**
** Parameter Dictionary:
**      l       Head of the list of various presentation conmtexts.
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
dump_presentation_ctx(LST_HEAD ** l)
{
    DUL_PRESENTATIONCONTEXT
    * ctx;
    DUL_TRANSFERSYNTAX
        * transfer;
    int
        l_index;
    OFOStringStream
        str;

    if (*l == NULL)
        return "";

    ctx = (DUL_PRESENTATIONCONTEXT*)LST_Head(l);
    if (ctx == NULL)
        return "";

    (void) LST_Position(l, (LST_NODE*)ctx);

    while (ctx != NULL) {
        str << "  Context ID:           " << ctx->presentationContextID << OFendl
            << "  Abstract Syntax:      " << ctx->abstractSyntax << OFendl;
        str << dump_uid(ctx->abstractSyntax, "%24s");
        str << "  Result field:         " << (int) ctx->result << OFendl;
        for (l_index = 0; l_index < (int) DIM_OF(scMap); l_index++) {
            if (ctx->proposedSCRole == scMap[l_index].role)
                str << "  Proposed SCU/SCP Role:  " << scMap[l_index].text << OFendl;
        }
        for (l_index = 0; l_index < (int) DIM_OF(scMap); l_index++) {
            if (ctx->acceptedSCRole == scMap[l_index].role)
                str << "  Accepted SCU/SCP Role:  " << scMap[l_index].text << OFendl;
        }
        str << "  Proposed Xfer Syntax(es)" << OFendl;
        if (ctx->proposedTransferSyntax != NULL) {
            transfer = (DUL_TRANSFERSYNTAX*)LST_Head(&ctx->proposedTransferSyntax);
            if (transfer != NULL)
                (void) LST_Position(&ctx->proposedTransferSyntax, (LST_NODE*)transfer);

            while (transfer != NULL) {
                str << "                  " << transfer->transferSyntax << OFendl;
                str << dump_uid(transfer->transferSyntax, "%18s");
                transfer = (DUL_TRANSFERSYNTAX*)LST_Next(&ctx->proposedTransferSyntax);
            }
        }
        str << "  Accepted Xfer Syntax: " << ctx->acceptedTransferSyntax << OFendl;
        str << dump_uid(ctx->acceptedTransferSyntax, "%24s");
        ctx = (DUL_PRESENTATIONCONTEXT*)LST_Next(l);
    }

    str << OFStringStream_ends;
    OFSTRINGSTREAM_GETOFSTRING(str, ret)
    return ret;
}

/* dumpExtNegList
**
** Purpose:
**      Display the extended negotiation structure
**
** Return Values:
**      None
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFString& dumpExtNegList(OFString& ret, SOPClassExtendedNegotiationSubItemList& lst)
{
    OFOStringStream str;
    OFListIterator(SOPClassExtendedNegotiationSubItem*) i = lst.begin();
    while (i != lst.end()) {
        SOPClassExtendedNegotiationSubItem* extNeg = *i;
        const char* uidName = dcmFindNameOfUID(extNeg->sopClassUID.c_str(), "Unknown-UID");
        str << "  =" << uidName
            << " (" << extNeg->sopClassUID.c_str() << ")" << OFendl
            << "    [";
        for (int k=0; k<(int)extNeg->serviceClassAppInfoLength; k++) {
            str << "0x";
            str << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(2) << (int)(extNeg->serviceClassAppInfo[k]);
            if (k < (int)(extNeg->serviceClassAppInfoLength-1)) str << ", ";
        }
        str << "]" << STD_NAMESPACE dec << OFendl;
        ++i;
    }

    str << OFStringStream_ends;
    OFSTRINGSTREAM_GETSTR(str, res)
    ret = res;
    OFSTRINGSTREAM_FREESTR(res);
    return ret;
}

/* dump_uid
**
** Purpose:
**      Display the UID structure
**
** Parameter Dictionary:
**      UID             The UID associated with the structure
**      indent          Useful for printing purposes.
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
dump_uid(const char *UID, const char *indent)
{
    const char* uidName;
    char buf[4096];

    if ((UID==NULL)||(UID[0] == '\0'))
    {
        sprintf(buf, indent, " ");
        return OFString(buf) + "No UID";
    } else {
        uidName = dcmFindNameOfUID(UID, "Unknown UID");
        sprintf(buf, indent, " ");
        return OFString(buf) + uidName;
    }
}
/* checkNetwork
**
** Purpose:
**      Verify the validity of the network handle.
**
** Parameter Dictionary:
**      networkKey              Handle to the network to be validated.
**      caller                  Used only for better error messages, contains
**                              the string indicating the name of the
**                              routine that called this one.
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
checkNetwork(PRIVATE_NETWORKKEY ** networkKey)
{
    if (networkKey == NULL) return DUL_NULLKEY;
    if (*networkKey == NULL) return DUL_NULLKEY;
    if (strcmp((*networkKey)->keyType, KEY_NETWORK) != 0) return DUL_ILLEGALKEY;

    return EC_Normal;
}


/* checkAssociation
**
** Purpose:
**      Verify the validity of the Association handle
**
** Parameter Dictionary:
**      association     Handle to the association to be validated.
**      caller          Name of routine that called this one.
**
** Return Values:
**
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
checkAssociation(PRIVATE_ASSOCIATIONKEY ** association)
{
    if (association == NULL) return DUL_NULLKEY;
    if (*association == NULL)  return DUL_NULLKEY;
    if (strcmp((*association)->keyType, KEY_ASSOCIATION) != 0) return DUL_ILLEGALKEY;

    return EC_Normal;
}


/* clearRequestorsParams
**
** Purpose:
**      Reset all the fields of the service parameters.
**
** Parameter Dictionary:
**      params          Pointer to the service parameters to be reset.
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
clearRequestorsParams(DUL_ASSOCIATESERVICEPARAMETERS * params)
{
    params->applicationContextName[0] = '\0';
    params->callingAPTitle[0] = '\0';
    params->calledAPTitle[0] = '\0';
    params->respondingAPTitle[0] = '\0';
    params->result = params->resultSource = params->diagnostic = 0;
    params->callingPresentationAddress[0] = '\0';
    params->calledPresentationAddress[0] = '\0';
    params->requestedPresentationContext = NULL;
    params->acceptedPresentationContext = NULL;
    params->maximumOperationsInvoked = 0;
    params->maximumOperationsPerformed = 0;
    params->callingImplementationClassUID[0] = '\0';
    params->callingImplementationVersionName[0] = '\0';
    params->requestedExtNegList = NULL;
    params->acceptedExtNegList = NULL;
}

/* clearPresentationContext
**
** Purpose:
**      Free the memory oocupied by the given presentation context.
**
** Parameter Dictionary:
**      l       Head of list of the presentation contexts to be freed.
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
clearPresentationContext(LST_HEAD ** l)
{
    DUL_PRESENTATIONCONTEXT
    * ctx;
    DUL_TRANSFERSYNTAX
        * transfer;

    if (*l == NULL)
        return;

    while ((ctx = (DUL_PRESENTATIONCONTEXT*)LST_Pop(l)) != NULL) {
        if (ctx->proposedTransferSyntax != NULL) {
            while ((transfer = (DUL_TRANSFERSYNTAX*)LST_Pop(&ctx->proposedTransferSyntax)) != NULL)
                free(transfer);
            (void) LST_Destroy(&ctx->proposedTransferSyntax);
        }
        free(ctx);
    }
    (void) LST_Destroy(l);
}

OFCondition DUL_setTransportLayer(DUL_NETWORKKEY *callerNetworkKey, DcmTransportLayer *newLayer, int takeoverOwnership)
{
  if (callerNetworkKey && newLayer)
  {
    PRIVATE_NETWORKKEY * key = (PRIVATE_NETWORKKEY *) callerNetworkKey;
    if ((key->networkSpecific.TCP.tLayerOwned) && (key->networkSpecific.TCP.tLayer)) delete key->networkSpecific.TCP.tLayer;
    key->networkSpecific.TCP.tLayer = newLayer;
    key->networkSpecific.TCP.tLayerOwned = takeoverOwnership;
    return EC_Normal;
  }
  return DUL_NULLKEY;
}

OFString& DUL_DumpConnectionParameters(OFString& str, DUL_ASSOCIATIONKEY *association)
{
  if (association)
  {
    PRIVATE_ASSOCIATIONKEY *assoc = (PRIVATE_ASSOCIATIONKEY *)association;
    if (assoc->connection)
        return assoc->connection->dumpConnectionParameters(str);
  }
  str.clear();
  return str;
}

// Legacy functions!
void DUL_DumpParams(DUL_ASSOCIATESERVICEPARAMETERS * params)
{
    OFString str;
    COUT << DUL_DumpParams(str, params) << OFendl;
}

void DUL_DumpConnectionParameters(DUL_ASSOCIATIONKEY *association, STD_NAMESPACE ostream& outstream)
{
    OFString str;
    outstream << DUL_DumpConnectionParameters(str, association) << OFendl;
}

void dumpExtNegList(SOPClassExtendedNegotiationSubItemList& lst)
{
    OFString str;
    COUT << dumpExtNegList(str, lst) << OFendl;
}

/*
** CVS Log
** $Log: dul.cc,v $
** Revision 1.94  2010-12-01 08:26:36  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.93  2010-11-29 13:40:59  uli
** Correctly determine the remote address if only a socket fd is given.
**
** Revision 1.92  2010-10-14 13:14:28  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.91  2010-10-01 12:25:29  uli
** Fixed most compiler warnings in remaining modules.
**
** Revision 1.90  2010-08-26 09:24:21  joergr
** Fixed incorrect behavior of association acceptors during SCP/SCU role
** selection negotiation.
** Introduced new global flag which allows for rejecting presentation contexts
** in case of an unsuccessful SCP/SCU role selection (disabled by default).
**
** Revision 1.89  2010-08-10 11:59:31  uli
** Fixed some cases where dcmFindNameOfUID() returning NULL could cause crashes.
**
** Revision 1.88  2010-08-06 08:41:36  uli
** Fixed some more compiler warnings.
**
** Revision 1.87  2010-06-16 08:45:21  joergr
** Introduced a new timeout of 60 seconds for the recv() function in order to
** make sure that the association is terminated if the receiver looses the
** connection to the sender. This can be disabled by DISABLE_RECV_TIMEOUT.
**
** Revision 1.86  2010-06-03 13:32:51  joergr
** Fixed issues on Windows platforms introduced with last commit.
**
** Revision 1.85  2010-06-02 15:47:56  joergr
** Replaced calls to strerror() by new helper function OFStandard::strerror()
** which results in using the thread safe version of strerror() if available.
** Replaced use of sprintf() on a static char array by class OFString or
** OFOStringStream respectively.
** Slightly modified some condition texts for reasons of consistency.
**
** Revision 1.84  2009-12-08 16:39:21  joergr
** Fixed use of wrong logging macro. Fixed inconsistent source code formatting.
**
** Revision 1.83  2009-11-18 11:53:59  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.82  2009-08-19 11:55:44  meichel
** Added additional includes needed for Sun Studio 11 on Solaris.
**
** Revision 1.81  2009-08-06 16:42:58  meichel
** Fixed bug in DUL_ReceiveAssociationRQ() that under very rare circumstances
**   caused ASC_receiveAssociation() to return successfully although no DICOM
**   association request had been received.
**
** Revision 1.80  2009-08-03 15:32:35  meichel
** Fixed resource leak the in code creating forked child processes
**
** Revision 1.79  2009-05-29 08:57:59  onken
** Fixed exception thrown by later WSACleanup call because of socket handle
** being (apparently) not closed corretly after socket handle duplication.
**
** Revision 1.78  2009-01-29 12:08:04  joergr
** Replaced remaining tab characters by spaces. Fixed source code formatting.
**
** Revision 1.77  2008-04-17 15:27:35  onken
** Reworked and extended User Identity Negotiation code.
**
** Revision 1.76  2007-09-07 08:49:29  onken
** Added basic support for Extended Negotiation of User Identity.
**
** Revision 1.75  2007/07/10 09:40:57  onken
** Fixed bug in windows multiprocess code that invalidated option values
** containing spaces. All cmdline arguments are now surrounded by double qoutes
** when spawning a new process.
**
** Revision 1.74  2007/05/24 16:10:08  onken
** Removed duplicate closing of socket handle in "windows multiprocess" code.
**
** Revision 1.73  2006/08/15 16:04:29  meichel
** Updated the code in module dcmnet to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.72  2006/03/23 11:22:45  meichel
** Fixed Win32 multi-process SCP code to also work correctly on Windows 2000.
**
** Revision 1.71  2005/12/20 11:20:37  meichel
** Added various typecasts needed to avoid warnings on MinGW.
**
** Revision 1.70  2005/12/15 17:44:16  joergr
** Removed unused variables on non-Windows systems, reported by Sun CC 2.0.1.
**
** Revision 1.69  2005/12/15 11:01:17  joergr
** Added explicit typecast, required for Sun CC 4.2 on Solaris.
**
** Revision 1.68  2005/12/08 15:44:48  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.67  2005/11/25 11:31:14  meichel
** StoreSCP now supports multi-process mode both on Posix and Win32 platforms
**   where a separate client process is forked for each incoming association.
**
** Revision 1.66  2005/11/03 17:27:14  meichel
** The movescu tool does not open any listen port by default anymore.
**
** Revision 1.65  2005/08/30 08:35:27  meichel
** Added command line option --inetd, which allows storescp to run from inetd.
**
** Revision 1.64  2004/07/28 08:15:19  meichel
** Fixed bug in DUL that could cause an application to block when waiting
**   for an association if a select() was interrupted by a signal.
**
** Revision 1.63  2004/05/17 14:39:14  wilkens
** Fixed problem reported by valgrind: passing uninitialized variable sockarg.
**
** Revision 1.62  2004/02/25 12:31:17  meichel
** Added global option flag for compatibility with very old DCMTK releases in the
**   DICOM upper layer and ACSE code. Default is automatic handling, which should
**   work in most cases.
**
** Revision 1.61  2004/02/24 15:05:50  meichel
** Fixed infinite loop in DUL_AbortAssociation that could occur on Win2K systems.
**
** Revision 1.60  2004/02/04 15:34:29  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.59  2003/12/09 10:55:27  meichel
** Removed unused debug output
**
** Revision 1.58  2003/07/09 14:00:27  meichel
** Removed unused debug output
**
** Revision 1.57  2003/07/03 14:21:10  meichel
** Added special handling for FD_SET() on MinGW, which expects an
**   unsigned first argument.
**
** Revision 1.56  2003/06/10 13:37:43  meichel
** Added support for TCP wrappers in DICOM network layer
**
** Revision 1.55  2003/06/06 13:07:30  meichel
** Introduced global flag dcmExternalSocketHandle which allows
**   to pass an already opened socket file descriptor to dcmnet.
**
** Revision 1.54  2003/06/02 16:44:11  meichel
** Renamed local variables to avoid name clashes with STL
**
** Revision 1.53  2003/05/12 13:02:15  meichel
** Fixed formatting bug in dumpExtNegList()
**
** Revision 1.52  2002/12/10 12:44:31  meichel
** Fixed bug in DUL code that caused a hang in DUL_AbortAssociation
**   when used on Windows 2000
**
** Revision 1.51  2002/12/10 11:00:42  meichel
** Modified DUL_InitializeNetwork to allow multiple network instances to
**   be created.
**
** Revision 1.50  2002/11/29 12:15:22  wilkens
** Modified call to getsockopt() in order to avoid compiler warning.
** Modified variable initialization in order to avoid compiler warning.
** Corrected dumping of hex values.
**
** Revision 1.49  2002/11/28 16:57:40  meichel
** Added global flag dcmConnectionTimeout that defines a timeout for
**   outgoing association requests in the DICOM upper layer.
**
** Revision 1.48  2002/11/27 13:04:43  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.47  2002/07/08 07:06:22  meichel
** Fixed bug in network initialization code
**
** Revision 1.46  2002/05/24 14:36:14  meichel
** Fixed typecast problem reported by egcs and VC6
**
** Revision 1.45  2002/05/15 11:24:16  meichel
** Fixed problem with DICOM upper layer: If network initialization for
**   an acceptor failed (e.g. because the listen port was already occupied),
**   further calls to ASC_initializeNetwork() with different port numbers
**   would also fail.
**
** Revision 1.44  2002/04/16 13:57:32  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.43  2001/12/19 16:37:01  meichel
** Introduced function pointer typedef to avoid warning on Sun Workshop 6.
**
** Revision 1.42  2001/11/27 09:54:58  wilkens
** Updated storescp. 6 new options (--output-directory, --sort-conc-studies,
** --exec-on-reception, --exec-on-eostudy, --rename-on-eostudy, and
** --eostudy-timeout) implemented (requirements from GO-Kard).
**
** Revision 1.41  2001/11/09 15:58:51  joergr
** Added '#include <iostream.h>' to avoid compiler errors reported by Sun CC
** 2.0.1 (required before <iomanip.h> is included).
**
** Revision 1.40  2001/11/01 13:49:07  wilkens
** Added lots of comments.
**
** Revision 1.39  2001/10/12 10:18:36  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.38  2001/09/28 13:28:53  joergr
** Added "#include <iomanip.h>" to keep gcc 3.0 quiet.
**
** Revision 1.37  2001/09/26 12:29:02  meichel
** Implemented changes in dcmnet required by the adaptation of dcmdata
**   to class OFCondition.  Removed some unused code.
**
** Revision 1.36  2001/06/01 11:02:02  meichel
** Implemented global flag and command line option to disable reverse
**   DNS hostname lookup using gethostbyaddr when accepting associations.
**
** Revision 1.35  2001/03/28 15:44:37  meichel
** Fixed memory leak: a DcmTransportLayer instance was not deallocated upon
**   destruction of a DUL_NETWORKKEY if the network was declared as
**   DICOM_APPLICATION_REQUESTOR.
**
** Revision 1.34  2000/12/15 13:27:47  meichel
** Declared qsort() and signal() callback functions as extern "C", avoids
**   warnings on Sun C++ 5.x compiler.
**
** Revision 1.33  2000/12/12 16:44:49  meichel
** Minor changes to keep gcc 2.7.x on SunOS 4.1.3 happy
**
** Revision 1.32  2000/10/10 12:06:56  meichel
** Updated transport layer error codes and routines for printing
**   connection parameters.
**
** Revision 1.31  2000/09/08 14:28:44  meichel
** Adapted calls to accept() to new flag HAVE_INTP_ACCEPT, required on OSF/1.
**
** Revision 1.30  2000/09/08 14:27:50  meichel
** Removed use of u_short and u_long types which are not defined in POSIX.
**   Required when compiling on Solaris with Posix threads.
**
** Revision 1.29  2000/08/10 14:50:57  meichel
** Added initial OpenSSL support.
**
** Revision 1.28  2000/06/07 08:57:26  meichel
** dcmnet ACSE routines now allow to retrieve a binary copy of the A-ASSOCIATE
**   RQ/AC/RJ PDUs, e.g. for logging purposes.
**
** Revision 1.27  2000/03/10 11:54:21  meichel
** Call to signal() now depending on SIGNAL_HANDLER_WITH_ELLIPSE
** compile time flag, required for Irix5.
**
** Revision 1.26  2000/03/08 16:32:03  meichel
** Now including netinet/in_systm.h and netinet/in.h wherever netinet/tcp.h
**   is used. Required for NeXTStep 3.3.
**
** Revision 1.25  2000/03/06 16:11:46  meichel
** Avoiding to include <sys/socket.h> directly, using dcompat.h instead.
**   <sys/socket.h> is not protected against multiple inclusion on Ultrix.
**
** Revision 1.24  2000/03/03 14:11:22  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.23  2000/02/24 13:45:27  meichel
** Calls to accept() and getsockname() now use socklen_t * for the third
**   parameter if this type is defined. Avoids warning on recent Linux distributions.
**
** Revision 1.22  2000/02/23 15:12:43  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.21  2000/02/07 13:27:03  meichel
** Significant speed improvement for network transmission.
**   Now using a default socket buffer length of 32K and disabling the Nagle
**   algorithm by default (TCP_NODELAY). Disabling of the Nagle algorithm
**   can be switched off by compiling the toolkit with the symbol
**   DONT_DISABLE_NAGLE_ALGORITHM defined, or by setting the environment
**   variable TCP_NODELAY to the value "0".
**
** Revision 1.20  2000/02/01 10:24:12  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.19  1999/05/03 14:12:31  joergr
** Minor code purifications to keep Sun CC 2.0.1 quiet.
**
** Revision 1.18  1999/04/19 08:38:57  meichel
** Added experimental support for extended SOP class negotiation.
**
** Revision 1.17  1999/03/29 11:20:04  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.16  1999/02/05 14:35:09  meichel
** Added a call to shutdown() immediately before closesocket() on Win32.
**   This causes any pending data to be sent before the socket is destroyed.
**   Fixes a problem causing A-RELEASE-RSP messages to get lost under certain
**   circumstances when the SCP runs on Win32.
**
** Revision 1.15  1999/01/07 14:25:02  meichel
** Changed sequence of include files in some dcmnet modules
**   to keep the Unixware compiler happy.
**
** Revision 1.14  1998/06/29 12:14:33  meichel
** Removed some name clashes (e.g. local variable with same
**   name as class member) to improve maintainability.
**   Applied some code purifications proposed by the gcc 2.8.1 -Weffc++ option.
**
** Revision 1.13  1998/01/28 17:38:15  meichel
** Removed minor bug from DICOM Upper Layer / DIMSE modules.
**   For each PDV received, an error condition was pushed on the error stack
**   and then again pulled from it. If a callback function was registered
**   with the condition stack, it was flooded with error messages.
**
** Revision 1.12  1997/09/11 15:58:46  hewett
** DUL code now only tries to set the send/receive TCP buffer length
** socket options if the SO_SNDBUF and SO_RCVBUF preprocessor macros
** are defined.  Attempts to set these socket options will generate an
** error message on stderr if unavailable.  This modification was
** needed to compiled the dcmnet code under the Signus GnuWin32
** environment.
**
** Revision 1.11  1997/08/05 07:38:18  andreas
** Corrected error in DUL finite state machine
** SCPs shall close sockets after the SCU have closed the socket in
** a normal association release. Therfore, an ARTIM timer is described
** in DICOM part 8 that is not implemented correctly in the
** DUL. Since the whole DUL finite state machine is affected, we
** decided to solve the proble outside the fsm. Now it is necessary to call the
** ASC_DropSCPAssociation() after the calling ASC_acknowledgeRelease().
**
** Revision 1.10  1997/07/24 13:10:58  andreas
** - Removed Warnings from SUN CC 2.0.1
**
** Revision 1.9  1997/07/21 08:47:20  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.8  1997/07/04 11:44:33  meichel
** Configure now also tests <sys/select.h> if available
**   when searching for a select() prototype.
**   Updated files using select() to include <sys/select.h> and
**   <sys/types.h> if available (needed for AIX).
**
** Revision 1.7  1996/12/03 15:29:47  meichel
** Added support for HP-UX 9.05 systems using GCC 2.7.2.1
**
** Revision 1.6  1996/09/27 08:38:40  hewett
** Support for WINSOCK socket library.  Use send instead of write, recv
** instead of read, closesocket instead of close.
**
** Revision 1.5  1996/09/24 16:22:45  hewett
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.4  1996/06/20 07:35:48  hewett
** Removed inclusion of system header already included by dcompat.h
** and made sure that dcompat.h is always included (via dicom.h).
**
** Revision 1.3  1996/05/03 10:31:53  hewett
** Move some common system include files out to include/dcompat.h which were
** causing problems due to multiple inclusion on some machines.
**
** Revision 1.2  1996/04/25 16:11:18  hewett
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
