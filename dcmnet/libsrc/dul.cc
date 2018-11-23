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
*/


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_WINDOWS_H
#include <winsock2.h>  /* for SO_EXCLUSIVEADDRUSE */
#include <ws2tcpip.h>  /* for socklen_t */
#endif

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

BEGIN_EXTERN_C
int dcmtk_hosts_access(struct request_info *req);
END_EXTERN_C
#endif
#ifdef HAVE_SIGNAL_H
// On Solaris with Sun Workshop 11, <signal.h> declares signal() but <csignal> does not
#include <signal.h>
#endif
END_EXTERN_C

BEGIN_EXTERN_C
/* declare extern "C" typedef for signal handler function pointer */
#ifdef SIGNAL_HANDLER_WITH_ELLIPSE
typedef void (*mySIG_TYP)(...);
#else
typedef void (*mySIG_TYP)(int);
#endif
END_EXTERN_C

#ifdef DCMTK_HAVE_POLL
#include <poll.h>
#endif

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmnet/dcompat.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmnet/lst.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/ofstd/ofstd.h"

#include "dcmtk/dcmnet/dul.h"
#include "dulstruc.h"
#include "dulpriv.h"
#include "dulfsm.h"
#include "dcmtk/dcmnet/dcmtrans.h"
#include "dcmtk/dcmnet/dcmlayer.h"
#include "dcmtk/ofstd/ofstd.h"

OFGlobal<OFBool> dcmDisableGethostbyaddr(OFFalse);
OFGlobal<Sint32> dcmConnectionTimeout(-1);
OFGlobal<DcmNativeSocketType> dcmExternalSocketHandle(DCMNET_INVALID_SOCKET);
OFGlobal<const char *> dcmTCPWrapperDaemonName((const char *)NULL);
OFGlobal<unsigned long> dcmEnableBackwardCompatibility(0);
OFGlobal<size_t> dcmAssociatePDUSizeLimit(0x100000);

static int networkInitialized = 0;

static OFCondition
createNetworkKey(const char *mode, int timeout, unsigned long opt,
                 PRIVATE_NETWORKKEY ** k);
static OFCondition
createAssociationKey(PRIVATE_NETWORKKEY ** network, const char *node,
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

#ifdef _WIN32
static void setTCPBufferLength(SOCKET sock);
#else
static void setTCPBufferLength(int sock);
#endif

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

OFCondition DUL_readSocketHandleAsForkedChild()
{
  OFCondition result = EC_Normal;

#ifdef _WIN32
  // we are a child process
  DUL_markProcessAsForkedChild();

  char buf[256];
  DWORD bytesRead = 0;
  HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);

  // read socket handle number from stdin, i.e. the anonymous pipe
  // to which our parent process has written the handle number.
  if (ReadFile(hStdIn, buf, sizeof(buf) - 1, &bytesRead, NULL))
  {
    // make sure buffer is zero terminated
    buf[bytesRead] = '\0';
    unsigned __int64 socketHandle = 0;
    sscanf(buf, "%llu", &socketHandle);
    // socketHandle is always 64-bit because we always use this type to
    // pass the handle between parent and chile. Type DcmNativeSocketType
    // can be 32-bit on a 32-bit Windows, however. We, therefore, cast to the
    // appropriate type. This is safe because the handle in the parent
    // also had type DcmNativeSocketType.
    dcmExternalSocketHandle.set(OFstatic_cast(DcmNativeSocketType, socketHandle));
  }
  else
  {
    DCMNET_ERROR("cannot read socket handle: " << GetLastError());
    result = DUL_CANNOTREADSOCKETHANDLE;
  }
#endif

  return result;
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
**      mode              NULL terminated string identifying the application
**                        as a requestor or acceptor.
**      networkParameter  A parameter which is specific to the network type,
**                        which may be needed to initialize the network.
**      timeout           Length of time in seconds for TIMER timeout.
**                        If 0, the function will use a default value.
**      opt               Bitmask which describes options to be used when
**                        initializing the network.
**      networkKey        The handle created by this function and returned
**                        to the caller to access this network environment
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
**      callerNetworkKey  Caller's handle to the network environment
**
** Return Values:
**
**
** Algorithm:
**      Close the socket and free the memory occupied by the network key
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
**      callerNetworkKey    Caller's handle to the network environment.
**      block               Flag indicating blocking/non-blocking mode.
**      timeout             When blocking mode is non-blocking, the timeout in
**                          seconds.
**      params              Pointer to list of parameters which describe the type
**                          of Association requested by the caller.
**      callerAssociation   Handle created by this function which describes the
**                          Association; returned to the caller.
**      activatePDUStorage
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
  DUL_BLOCKOPTIONS block,
  int timeout,
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

    if (block == DUL_NOBLOCK)
        DCMNET_TRACE("setting association request timeout to " << timeout << " seconds");

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

    /* if no timeout is passed, use the default one (do not wait forever) */
    if ((block == DUL_BLOCK) && ((*association)->timeout > 0))
    {
        block = DUL_NOBLOCK;
        timeout = (*association)->timeout;
        DCMNET_TRACE("setting timeout for first PDU to be read to " << timeout << " seconds");
    }
    /* Find the next event */
    cond = PRV_NextPDUType(association, block, timeout, &pduType);

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
**      callerNetworkKey    Caller's handle to the network environment.
**      block               Flag indicating blocking/non-blocking mode.
**      timeout             When blocking mode is non-blocking, the timeout in
**                          seconds.
**      params              Pointer to a structure holding parameters which
**                          describe this Association.
**      callerAssociation   Caller handle for this association that is created
**                          by this function.
**      activatePDUStorage
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

    if (block == DUL_NOBLOCK)
        DCMNET_TRACE("setting association receive timeout to " << timeout << " seconds");

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

    /* This is the first time we read from this new connection, so in case it
     * doesn't speak DICOM, we shouldn't wait forever (= DUL_NOBLOCK).
     */
    DCMNET_TRACE("setting timeout for first PDU to be read to " << (*association)->timeout << " seconds");
    cond = PRV_NextPDUType(association, DUL_NOBLOCK, (*association)->timeout, &pduType);

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
**      callerAssociation   Caller's handle to the Association that is to be
**                          acknowledged and hence established.
**      params              Parameters that describe the type of service handled
**                          by this Association.
**      activatePDUStorage
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
**      callerAssociation   Caller's handle to the association that is to be
**                          rejected.
**      params              Pointer to a structure that gives the reasons for
**                          rejecting this Association.
**      activatePDUStorage
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
    /* check "Source" field */
    {
        unsigned char sourcetable[] = {0x01, 0x02, 0x03};
        int l_index;
        OFBool found = OFFalse;
        for (l_index = 0; l_index < (int) DIM_OF(sourcetable) && !found; l_index++)
            found = (localParams.source == sourcetable[l_index]);

        if (!found)
        {
            OFOStringStream stream;
            stream << "DUL Illegal source for rejecting Association: "
                   << localParams.source << OFStringStream_ends;
            OFSTRINGSTREAM_GETOFSTRING(stream, msg)
            return makeDcmnetCondition(DULC_ILLEGALREJECTSOURCE, OF_error, msg.c_str());
        }
    }
    /* check "Reason/Diag." field */
    {
        OFBool found = OFFalse;
        switch (localParams.source)
        {
            case ASC_SOURCE_SERVICEUSER:                            /* 0x01 */
                found = (localParams.reason == 0x01) || (localParams.reason == 0x02) ||
                        (localParams.reason == 0x03) || (localParams.reason == 0x07);
                break;
            case ASC_SOURCE_SERVICEPROVIDER_ACSE_RELATED:           /* 0x02 */
                found = (localParams.reason == 0x01) || (localParams.reason == 0x02);
                break;
            case ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED:   /* 0x03 */
                found = (localParams.reason == 0x01) || (localParams.reason == 0x02);
                break;
        }

        if (!found)
        {
            OFOStringStream stream;
            stream << "DUL Illegal reason for rejecting Association: "
                   << localParams.reason << OFStringStream_ends;
            OFSTRINGSTREAM_GETOFSTRING(stream, msg)
            return makeDcmnetCondition(DULC_ILLEGALREJECTREASON, OF_error, msg.c_str());
        }
    }
    /* check "Result" field */
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
                   << localParams.result << OFStringStream_ends;
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
**      callerAssociation  Caller's handle to the Association that is to
**                         be dropped.
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
**      callerAssociation  Caller's handle to the Association to be released.
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
**      callerAssociation  Caller's handle to the Association whose
**                         release is acknowledged.
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
**      callerAssociation  The handle for the association to be aborted.
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
    OFCondition tcpError = makeDcmnetCondition(DULC_TCPIOERROR, OF_error, "");

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
        // the comparison with tcpError prevents a potential infinite loop if
        // we are receiving garbage over the network connection.
        if (cond.good() || (cond == tcpError)) done = OFTrue;
    }
    return EC_Normal;
}


/* DUL_WritePDVs
**
** Purpose:
**      Write a list of PDVs on an active Association.
**
** Parameter Dictionary:
**      callerAssociation  Caller's handle to the Association
**      pdvList            Pointer to a structure which describes the
**                         list of PDVs to be written on the active
**                         Association.
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
**      callerAssociation  Caller's handle for the Association.
**      pdvList            Pointer to a structure which describes the
**                         list of PDVs to be read from the active
**                         Association.
**      block              Option used for blocking/non-blocking read.
**      timeout            Timeout interval.
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
**      callerAssociation  Caller's handle to the Association.
**      param              The only supported value is DUL_K_MAX_PDV_XMIT.
**      type               The TYPE of the parameter requested.
**      address            Requested parameter to be obtained from the
**                         Association key.
**      length             The size of the parameter requested.
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
**      callerAssociation  Caller's handle to the Association
**      pdv                The next PDU to be returned
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


/*  ========================================================================
 *  Private functions not meant for users of the package are included below.
 */


/* receiveTransportConnection
**
** Purpose:
**      Receive a transport connection and fill in various fields of the
**      service parameters and Association handle.
** Parameter Dictionary:
**      network      Pointer to a structure maintaining information about
**                   the network environment.
**      block        Option indicating blocking/non-blocking mode
**      timeout      When blocking mode is non-blocking, the timeout in seconds
**      params       Pointer to structure describing the services for the
**                   Association.
**      association  Handle to the association
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
**      network      Pointer to a structure maintaining information about
**                   the network environment.
**      block        Option indicating blocking/non-blocking mode
**      timeout      When blocking mode is non-blocking, the timeout in seconds
**      params       Pointer to structure describing the services for the
**                   Association.
**      association  Handle to the association
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
#ifndef DCMTK_HAVE_POLL
    fd_set fdset;
#endif
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
    struct linger sockarg;

#ifdef HAVE_FORK
    pid_t pid = -1;
#endif

    (void) memset(&sockarg, 0, sizeof(sockarg));

    int reuse = 1;

#ifdef _WIN32
    SOCKET sock = dcmExternalSocketHandle.get();
    if (sock != INVALID_SOCKET)
#else
    int sock = dcmExternalSocketHandle.get();
    if (sock > 0)
#endif
    {
      // use the socket file descriptor provided to us externally
      // instead of calling accept().
      connected = 1;

      len = sizeof(from);
      if (getpeername(sock, &from, &len))
      {
          OFOStringStream stream;
          stream << "TCP Initialization Error: " << OFStandard::getLastNetworkErrorCode().message()
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
#ifdef DCMTK_HAVE_POLL
            struct pollfd pfd[] =
            {
                { (*network)->networkSpecific.TCP.listenSocket, POLLIN, 0 }
            };
#else
             FD_ZERO(&fdset);
#ifdef __MINGW32__
            // on MinGW, FD_SET expects an unsigned first argument
            FD_SET((unsigned int)((*network)->networkSpecific.TCP.listenSocket), &fdset);
#else
            FD_SET((*network)->networkSpecific.TCP.listenSocket, &fdset);
#endif /* __MINGW32__ */
#endif /* DCMTK_HAVE_POLL */

            timeout_val.tv_sec = timeout;
            timeout_val.tv_usec = 0;

#ifdef DCMTK_HAVE_POLL
            nfound = poll(pfd, 1, timeout_val.tv_sec*1000+(timeout_val.tv_usec/1000));
#else
#ifdef HAVE_INTP_SELECT
            nfound = select(
              OFstatic_cast(int, (*network)->networkSpecific.TCP.listenSocket + 1),
                           (int *)(&fdset), NULL, NULL, &timeout_val);
#else
            // On Win32, it is safe to cast the first parameter to int
            // because Windows ignores this parameter anyway.
            nfound = select(
              OFstatic_cast(int, (*network)->networkSpecific.TCP.listenSocket + 1),
                           &fdset, NULL, NULL, &timeout_val);
#endif /* HAVE_INTP_SELECT */
#endif /* DCMTK_HAVE_POLL*/

            if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
            {
                DU_logSelectResult(nfound);
            }
            if (nfound > 0) {
#ifdef DCMTK_HAVE_POLL
                if (pfd[0].revents & POLLIN)
                    connected++;
#else
                if (FD_ISSET((*network)->networkSpecific.TCP.listenSocket, &fdset))
                    connected++;
#endif
            }
            if (!connected) return DUL_NOASSOCIATIONREQUEST;
        }
        else
        {
            connected = 0;
            do {
#ifdef DCMTK_HAVE_POLL
                struct pollfd pfd[]=
                {
                    { (*network)->networkSpecific.TCP.listenSocket, POLLIN, 0 }
                };
#else
                FD_ZERO(&fdset);
#ifdef __MINGW32__
                // on MinGW, FD_SET expects an unsigned first argument
                FD_SET((unsigned int)((*network)->networkSpecific.TCP.listenSocket), &fdset);
#else
                FD_SET((*network)->networkSpecific.TCP.listenSocket, &fdset);
#endif /* __MINGW32__ */
#endif /* DCMTK_HAVE_POLL */
                timeout_val.tv_sec = 5;
                timeout_val.tv_usec = 0;
#ifdef DCMTK_HAVE_POLL
                nfound = poll(pfd, 1, timeout_val.tv_sec*1000+(timeout_val.tv_usec/1000));
#else
#ifdef HAVE_INTP_SELECT
                nfound = select(
                  OFstatic_cast(int, (*network)->networkSpecific.TCP.listenSocket + 1),
                                (int *)(&fdset), NULL, NULL, &timeout_val);
#else
                // On Win32, it is safe to cast the first parameter to int
                // because Windows ignores this parameter anyway.
                nfound = select(
                  OFstatic_cast(int, (*network)->networkSpecific.TCP.listenSocket + 1),
                                &fdset, NULL, NULL, &timeout_val);
#endif /* HAVE_INTP_SELECT */
#endif /* DCMTK_HAVE_POLL */
                if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
                {
                    DU_logSelectResult(nfound);
                }
                if (nfound > 0) {
#ifdef DCMTK_HAVE_POLL
                    if (pfd[0].revents & POLLIN)
                        connected++;
#else
                    if (FD_ISSET((*network)->networkSpecific.TCP.listenSocket, &fdset))
                        connected++;
#endif
                }
            } while (!connected);
        }

        len = sizeof(from);
        do
        {
            sock = accept((*network)->networkSpecific.TCP.listenSocket, &from, &len);
#ifdef _WIN32
        } while (sock == INVALID_SOCKET && WSAGetLastError() == WSAEINTR);
        if (sock == INVALID_SOCKET)
#else
        } while (sock == -1 && errno == EINTR);
        if (sock < 0)
#endif
        {
            OFOStringStream stream;
            stream << "TCP Initialization Error: " << OFStandard::getLastNetworkErrorCode().message()
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

            OFString msg = "Multi-Process Error: ";
            msg += OFStandard::getLastSystemErrorCode().message();
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
            /* if last character in argument value is a backslash, escape it
             * since otherwise it would escape the quote appended in the following
             * step, i.e. make sure that something like '\my\dir\' does not become
             * '"\my\dir\"' but instead ends up as '"\my\dir\\"' (single quotes for
             *  demonstration purposes). Make sure nobody passes a zero length string.
             */
            size_t len = strlen(command_argv[i]);
            if ((len > 0) && (command_argv[i][len - 1] == '\\'))
            {
	            cmdLine += "\\";
            }
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
            // GetCurrentProcess() only returns a pseudo handle which may not
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
                char buf[30];
                // we pass the socket handle as a 64-bit unsigned integer, which should work for 32 and 64 bit Windows
                sprintf(buf, "%llu", OFreinterpret_cast(unsigned __int64, childSocketHandle));
                if (!WriteFile(hChildStdInWriteDup, buf, OFstatic_cast(DWORD, strlen(buf) + 1), &bytesWritten, NULL))
                {
                    CloseHandle(hChildStdInWriteDup);
                    return makeDcmnetCondition(DULC_CANNOTFORK, OF_error, "Multi-Process Error: Writing to anonymous pipe failed");
                }

                // return OF_ok status code DULC_FORKEDCHILD with descriptive text
                OFOStringStream stream;
                stream << "New child process started with pid " << OFstatic_cast(int, pi.dwProcessId)
                       << ", socketHandle " << OFstatic_cast(int, OFreinterpret_cast(size_t, childSocketHandle)) << OFStringStream_ends;
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

    sockarg.l_onoff = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *) &sockarg, sizeof(sockarg)) < 0)
    {
        OFOStringStream stream;
        stream << "TCP Initialization Error: " << OFStandard::getLastNetworkErrorCode().message()
               << ", setsockopt failed on socket " << sock << OFStringStream_ends;
        OFSTRINGSTREAM_GETOFSTRING(stream, msg)
        return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
    }
    reuse = 1;

#ifdef _WIN32
    if (setsockopt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *) &reuse, sizeof(reuse)) < 0)
#else
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0)
#endif
    {
        OFString msg = "TCP Initialization Error: ";
        msg += OFStandard::getLastNetworkErrorCode().message();
        return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
    }
    setTCPBufferLength(sock);

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
        DCMNET_WARN("DUL: cannot parse environment variable TCP_NODELAY=" << tcpNoDelayString);
      }
    } else
      DCMNET_TRACE("  environment variable TCP_NODELAY not set, using the default value (" << tcpNoDelay << ")");
    if (tcpNoDelay) {
#ifdef DISABLE_NAGLE_ALGORITHM
      DCMNET_DEBUG("DUL: disabling Nagle algorithm as defined at compilation time (DISABLE_NAGLE_ALGORITHM)");
#else
      DCMNET_DEBUG("DUL: disabling Nagle algorithm as requested at runtime (TCP_NODELAY=" << tcpNoDelayString << ")");
#endif
      if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&tcpNoDelay, sizeof(tcpNoDelay)) < 0)
      {
        OFString msg = "TCP Initialization Error: ";
        msg += OFStandard::getLastNetworkErrorCode().message();
        return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
      }
#ifdef DISABLE_NAGLE_ALGORITHM
    } else {
      DCMNET_DEBUG("DUL: do not disable Nagle algorithm as requested at runtime (TCP_NODELAY=" << tcpNoDelayString << ")");
#endif
    }

    // create string containing numerical IP address.
    OFString client_dns_name;
    char client_ip_address[20];
    sprintf(client_ip_address, "%-d.%-d.%-d.%-d",  // this code is ugly but thread safe
       ((int) from.sa_data[2]) & 0xff,
       ((int) from.sa_data[3]) & 0xff,
       ((int) from.sa_data[4]) & 0xff,
       ((int) from.sa_data[5]) & 0xff);

    if (! dcmDisableGethostbyaddr.get())
       client_dns_name = OFStandard::getHostnameByAddress(&from.sa_data[2], sizeof(struct in_addr), AF_INET);

    if (client_dns_name.length() == 0)
    {
        // reverse DNS lookup disabled or host not found, use numerical address
        OFStandard::strlcpy(params->callingPresentationAddress, client_ip_address,
          sizeof(params->callingPresentationAddress));
        OFStandard::strlcpy((*association)->remoteNode, client_ip_address, sizeof((*association)->remoteNode));
        DCMNET_DEBUG("Association Received: " << params->callingPresentationAddress );
    }
    else
    {
        char node[260];
        if ((*network)->options & DUL_FULLDOMAINNAME)
            OFStandard::strlcpy(node, client_dns_name.c_str(), sizeof(node));
        else {
            if (sscanf(client_dns_name.c_str(), "%[^.]", node) != 1)
                node[0] = '\0';
        }
        OFStandard::strlcpy((*association)->remoteNode, node, sizeof((*association)->remoteNode));
        OFStandard::strlcpy(params->callingPresentationAddress, node, sizeof(params->callingPresentationAddress));
        DCMNET_DEBUG("Association Received: " << params->callingPresentationAddress );
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

        if (! dcmtk_hosts_access(&request))
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
      OFString msg = "TCP Initialization Error: ";
      msg += OFStandard::getLastNetworkErrorCode().message();
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
**      mode     Role played by the application entity
**      timeout  The timeout value to be used for timers
**      opt
**      key      Pointer to the structure maintaining the entire
**               information of the network environment.
**               This is returned back to the caller.
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
    OFStandard::strlcpy((*key)->keyType, KEY_NETWORK, sizeof((*key)->keyType));

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
**      key        Handle to the network environment
**      parameter  Port number on which to listen for connection
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
    struct linger sockarg;
    int reuse = 1;

    (void) memset(&sockarg, 0, sizeof(sockarg));

    // initialize network layer settings to well-defined state
    (*key)->networkSpecific.TCP.tLayer = NULL;
    (*key)->networkSpecific.TCP.tLayerOwned = 0;
    (*key)->networkSpecific.TCP.port = -1;
#ifdef _WIN32
    (*key)->networkSpecific.TCP.listenSocket = INVALID_SOCKET;
#else
    (*key)->networkSpecific.TCP.listenSocket = -1;
#endif

    // Create listen socket if we're an application acceptor,
    // unless the socket handle has already been passed to us or
    // we are a forked child of an application acceptor, in which
    // case the socket also already exists.
#ifdef _WIN32
    if ((dcmExternalSocketHandle.get() == INVALID_SOCKET) &&
#else
    if ((dcmExternalSocketHandle.get() < 0) &&
#endif
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

#ifdef _WIN32
      SOCKET sock;
#else
      int sock;
#endif
      struct sockaddr_in server;

      /* Create socket for Internet type communication */
      (*key)->networkSpecific.TCP.port = *(int *) parameter;
      (*key)->networkSpecific.TCP.listenSocket = socket(AF_INET, SOCK_STREAM, 0);
      sock = (*key)->networkSpecific.TCP.listenSocket;

#ifdef _WIN32
      if (sock == INVALID_SOCKET)
#else
      if (sock < 0)
#endif
      {
        OFString msg = "TCP Initialization Error: ";
        msg += OFStandard::getLastNetworkErrorCode().message();
        return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
      }
      reuse = 1;
#ifdef _WIN32
      if (setsockopt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *) &reuse, sizeof(reuse)) < 0)
#else
      if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0)
#endif
      {
        OFString msg = "TCP Initialization Error: ";
        msg += OFStandard::getLastNetworkErrorCode().message();
          return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
      }

      /* Name socket using wildcards */
      server.sin_family = AF_INET;
      server.sin_addr.s_addr = INADDR_ANY;
      server.sin_port = (unsigned short) htons((*key)->networkSpecific.TCP.port);
      if (bind(sock, (struct sockaddr *) & server, sizeof(server)))
      {
        OFString msg = "TCP Initialization Error: ";
        msg += OFStandard::getLastNetworkErrorCode().message();
        return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
      }
      /* Find out assigned port number and print it out */
      length = sizeof(server);
      if (getsockname(sock, (struct sockaddr *) &server, &length))
      {
        OFString msg = "TCP Initialization Error: ";
        msg += OFStandard::getLastNetworkErrorCode().message();
        return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
      }
      sockarg.l_onoff = 0;
      if (setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *) &sockarg, sizeof(sockarg)) < 0)
      {
        OFString msg = "TCP Initialization Error: ";
        msg += OFStandard::getLastNetworkErrorCode().message();
        return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
      }

      /* Listen on the socket */
      if (listen(sock, PRV_LISTENBACKLOG) < 0)
      {
        OFString msg = "TCP Initialization Error: ";
        msg += OFStandard::getLastNetworkErrorCode().message();
        return makeDcmnetCondition(DULC_TCPINITERROR, OF_error, msg.c_str());
      }
    }

    (*key)->networkSpecific.TCP.tLayer = new DcmTransportLayer();
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
**      networkKey      Handle to the network environment
**      remoteNode      The remote node to which the association is
**                      to be set up.
**      maxPDU          Size of the maximum PDU.
**      associationKey  The handle to the Association that is to be
**                      returned.
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

    OFStandard::strlcpy(key->keyType, KEY_ASSOCIATION, sizeof(key->keyType));
    key->applicationFunction = (*networkKey)->applicationFunction;

    OFStandard::strlcpy(key->remoteNode, remoteNode, sizeof(key->remoteNode));
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
**      key  Handle to the association.
**
** Return Values:
**
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
**      paramAddress   Source parameter
**      paramType      Type of the source parameter
**      paramLength    Size of the source parameter
**      outputType     Type of the destination parameter
**      outputAddress  Destination parameter returned to caller
**      outputLength   Size of the destination parameter
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
        OFStandard::strlcpy((char*)outputAddress, (char*)paramAddress, outputLength);
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
**      sock  Socket descriptor.
**
** Return Values:
**      None
**
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
            DCMNET_DEBUG("DUL: setting TCP buffer length to " << bufLen << " bytes");
            (void) setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *) &bufLen, sizeof(bufLen));
            (void) setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *) &bufLen, sizeof(bufLen));
#else
            DCMNET_WARN("DUL: setTCPBufferLength: cannot set TCP buffer length socket option: "
                << "code disabled because SO_SNDBUF and SO_RCVBUF constants are unknown");
#endif // SO_SNDBUF and SO_RCVBUF
        } else
            DCMNET_WARN("DUL: cannot parse environment variable TCP_BUFFER_LENGTH=" << TCPBufferLength);
    } else
        DCMNET_TRACE("  environment variable TCP_BUFFER_LENGTH not set, using the system defaults");
}


/* DUL_DumpParams
**
** Purpose:
**      Returns information of various fields of the service parameters.
**
** Parameter Dictionary:
**      ret_str
**      params   Pointer to structure holding the service parameters.
**
** Return Values:
**
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

    str << "APP CTX NAME: " << params->applicationContextName << OFendl;
    str << dump_uid(params->applicationContextName, "%14s") << OFendl;
    str << "AP TITLE:     " << params->callingAPTitle << OFendl
        << "AP TITLE:     " << params->calledAPTitle << OFendl
        << "AP TITLE:     " << params->respondingAPTitle << OFendl
        << "MAX PDU:      " << (int)params->maxPDU << OFendl
        << "Peer MAX PDU: " << (int)params->peerMaxPDU << OFendl
        << "PRES ADDR:    " << params->callingPresentationAddress << OFendl
        << "PRES ADDR:    " << params->calledPresentationAddress << OFendl
        << "REQ IMP UID:  " << params->callingImplementationClassUID << OFendl;
    str << dump_uid(params->callingImplementationClassUID, "%14s") << OFendl;
    str << "REQ VERSION:  " << params->callingImplementationVersionName << OFendl
        << "ACC IMP UID:  " << params->calledImplementationClassUID << OFendl;
    str << dump_uid(params->calledImplementationClassUID, "%14s") << OFendl;
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
**      l  Head of the list of various presentation contexts.
**
** Return Values:
**
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
        str << "  Context ID:            " << (int)ctx->presentationContextID << OFendl
            << "  Abstract Syntax:       " << ctx->abstractSyntax << OFendl;
        str << dump_uid(ctx->abstractSyntax, "%25s") << OFendl;
        str << "  Result field:          " << (int) ctx->result << OFendl;
        for (l_index = 0; l_index < (int) DIM_OF(scMap); l_index++) {
            if (ctx->proposedSCRole == scMap[l_index].role)
                str << "  Proposed SCU/SCP Role: " << scMap[l_index].text << OFendl;
        }
        for (l_index = 0; l_index < (int) DIM_OF(scMap); l_index++) {
            if (ctx->acceptedSCRole == scMap[l_index].role)
                str << "  Accepted SCU/SCP Role: " << scMap[l_index].text << OFendl;
        }
        str << "  Proposed Xfer Syntax(es)" << OFendl;
        if (ctx->proposedTransferSyntax != NULL) {
            transfer = (DUL_TRANSFERSYNTAX*)LST_Head(&ctx->proposedTransferSyntax);
            if (transfer != NULL)
                (void) LST_Position(&ctx->proposedTransferSyntax, (LST_NODE*)transfer);

            while (transfer != NULL) {
                str << "                         " << transfer->transferSyntax << OFendl;
                str << dump_uid(transfer->transferSyntax, "%25s") << OFendl;
                transfer = (DUL_TRANSFERSYNTAX*)LST_Next(&ctx->proposedTransferSyntax);
            }
        }
        str << "  Accepted Xfer Syntax:  " << ctx->acceptedTransferSyntax << OFendl;
        str << dump_uid(ctx->acceptedTransferSyntax, "%25s") << OFendl;
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
** Parameter Dictionary:
**      ret
**      lst
**
** Return Values:
**      None
**
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
**      UID     The UID associated with the structure
**      indent  Useful for printing purposes.
**
** Return Values:
**
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
**      networkKey  Handle to the network to be validated.
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
**      association  Handle to the association to be validated.
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
**      params  Pointer to the service parameters to be reset.
**
** Return Values:
**      None
**
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
**      Free the memory occupied by the given presentation context.
**
** Parameter Dictionary:
**      l  Head of list of the presentation contexts to be freed.
**
** Return Values:
**      None
**
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
            LST_Destroy(&ctx->proposedTransferSyntax);
        }
        free(ctx);
    }
    LST_Destroy(l);
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
