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
**        DICOM 93
**        Electronic Radiology Laboratory
**      Mallinckrodt Institute of Radiology
**    Washington University School of Medicine
**
** Module Name(s):
** Author, Date:  Stephen M. Moore, 14-Apr-1993
** Intent:        This file defines the public structures and constants
**                and the function prototypes for the DUL (DICOM Upper
**                Layer) facility.
*/


#ifndef DUL_IS_IN
#define DUL_IS_IN 1

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofglobal.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/dcmnet/extneg.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/dcuserid.h"
#include "dcmtk/dcmnet/dntypes.h"

class DcmTransportConnection;
class DcmTransportLayer;
class LST_HEAD;

// include this file in doxygen documentation

/** @file dul.h
 *  @brief type definitions, constants, global variables and functions for the dcmnet/dul module
 */

/** Global flag to enable/disable reverse DNS lookup when accepting associations.
 *  If disabled, the numerical IP address instead of the symbolic hostname is
 *  stored in the callingPresentationAddress field of the association parameters
 *  structure.  Most DICOM applications (except imagectn) don't need the symbolic
 *  hostname anyway, and the reverse DNS lookup can cause a long timeout.
 */
extern DCMTK_DCMNET_EXPORT OFGlobal<OFBool> dcmDisableGethostbyaddr;   /* default: OFFalse */

/** Global timeout in seconds for connecting to remote hosts.
 *  Default value is -1, which selects infinite timeout, i.e. blocking connect().
 */
extern DCMTK_DCMNET_EXPORT OFGlobal<Sint32> dcmConnectionTimeout;   /* default: -1 */

/** This global flag allows to set an already opened socket file descriptor which
 *  will be used by dcmnet the next time receiveTransportConnectionTCP() is called.
 *  Useful for use with proxy applications, but inherently thread unsafe!
 */
extern DCMTK_DCMNET_EXPORT OFGlobal<DcmNativeSocketType> dcmExternalSocketHandle;   /* default: platform specific value that denotes <i>invalid</i> */

/** When compiled with WITH_TCPWRAPPER, DCMTK server processes may use the TCP
 *  wrapper library to enforce access control - see hosts_access(5).  If this
 *  global flag is non-NULL, the TCP wrapper is enabled and the string pointed
 *  to is used as the daemon name.  If the flag is NULL, no access control is
 *  performed.
 */
extern DCMTK_DCMNET_EXPORT OFGlobal<const char *> dcmTCPWrapperDaemonName;   /* default: NULL */

/* Global option flag for compatibility with DCMTK releases prior to version 3.0.
 * Default (0) is automatic handling, which should work in most cases.
 */
extern DCMTK_DCMNET_EXPORT OFGlobal<unsigned long> dcmEnableBackwardCompatibility;

/** Maximum size (in bytes) of incoming A-ASSOCIATE-RQ or A-ASSOCIATE-AC PDUs
 *  that we are willing to accept. Under normal conditions, valid A-ASSOCIATE PDUs
 *  will never be larger than 64 kBytes, so the default of 1 MByte should be safe.
 *  We should limit the maximum accepted PDU size to avoid denial of service
 *  attacks through faulty PDUs that cause the network layer to allocate up
 *  to 4 GByte of memory for one PDU.
 *
 *  Note: This limit can be disabled by setting a value of zero, which
 *  causes the behaviour to revert to that of DCMTK releases up to 3.6.3.
 */
extern DCMTK_DCMNET_EXPORT OFGlobal<size_t> dcmAssociatePDUSizeLimit;   /* default: 1 MB */

typedef void DUL_NETWORKKEY;
typedef void DUL_ASSOCIATIONKEY;
typedef unsigned char DUL_PRESENTATIONCONTEXTID;

/** pure virtual base class for DUL mode callbacks
 */

/*  Define a structure containing fixed length fields that can
 *  be used for requesting or accepting an association.  The
 *  lengths of "titles" and "names" are specified by the DICOM
 *  protocol document.  The fields in the structure are made
 *  longer to allow for zero-terminators.  DICOM doesn't know
 *  about node names, but the DUL_ package needs them.
 */

#define DUL_LEN_TITLE  OFstatic_cast(size_t, 16)  /* required by DICOM protocol */
#define DUL_LEN_NAME   OFstatic_cast(size_t, 64)  /* required by DICOM protocol */
#define DUL_LEN_UID    OFstatic_cast(size_t, 64)  /* required by DICOM protocol */
#define DUL_LEN_NODE   OFstatic_cast(size_t, 127) /* should be "big enough" */


/* DICOM PDU Types */

#define DUL_TYPEASSOCIATERQ  OFstatic_cast(unsigned char, 0x01)
#define DUL_TYPEASSOCIATEAC  OFstatic_cast(unsigned char, 0x02)
#define DUL_TYPEASSOCIATERJ  OFstatic_cast(unsigned char, 0x03)
#define DUL_TYPEDATA         OFstatic_cast(unsigned char, 0x04)
#define DUL_TYPERELEASERQ    OFstatic_cast(unsigned char, 0x05)
#define DUL_TYPERELEASERP    OFstatic_cast(unsigned char, 0x06)
#define DUL_TYPEABORT        OFstatic_cast(unsigned char, 0x07)

#define DUL_MAXTYPE          OFstatic_cast(unsigned char, 0x07)

class DCMTK_DCMNET_EXPORT DUL_ModeCallback
{
public:

  /// destructor
  virtual ~DUL_ModeCallback() {}

  /** callback method
   *  @param mode DUL compatibility mode passed in callback
   */
  virtual void callback(unsigned long mode) = 0;
};

typedef struct {
    char applicationContextName[DUL_LEN_NAME + 1];
    char callingAPTitle[DUL_LEN_TITLE + 1];
    char calledAPTitle[DUL_LEN_TITLE + 1];
    char respondingAPTitle[DUL_LEN_TITLE + 1];
    unsigned long maxPDU;
    unsigned short result;
    unsigned short resultSource;
    unsigned short diagnostic;
    char callingPresentationAddress[64];
    char calledPresentationAddress[64];
    LST_HEAD *requestedPresentationContext;
    LST_HEAD *acceptedPresentationContext;
    unsigned short maximumOperationsInvoked;
    unsigned short maximumOperationsPerformed;
    char callingImplementationClassUID[DICOM_UI_LENGTH + 1];
    char callingImplementationVersionName[16 + 1];
    char calledImplementationClassUID[DICOM_UI_LENGTH + 1];
    char calledImplementationVersionName[16 + 1];
    unsigned long peerMaxPDU;
    SOPClassExtendedNegotiationSubItemList *requestedExtNegList;
    SOPClassExtendedNegotiationSubItemList *acceptedExtNegList;
    UserIdentityNegotiationSubItemRQ *reqUserIdentNeg;
    UserIdentityNegotiationSubItemAC *ackUserIdentNeg;

    OFBool useSecureLayer;
}   DUL_ASSOCIATESERVICEPARAMETERS;

/** Enum describing the possible role settings for role negotiation sub items.
 *  DCMTK implements the following role negotiation behaviour for association
 *  acceptors:
 *  @verbatim
 *  +--------------------+------------------+---------+
 *  | Requestor Proposal | Acceptor Setting | Result  |
 *  +--------------------+------------------+---------+
 *  | SCU                | SCP              | NONE    |
 *  | SCU                | SCU              | SCU     |
 *  | SCU                | SCU/SCP          | SCU     |
 *  | SCU                | DEFAULT          | DEFAULT |
 *  | SCP                | SCP              | SCP     |
 *  | SCP                | SCU              | NONE    |
 *  | SCP                | SCU/SCP          | SCP     |
 *  | SCP                | DEFAULT          | DEFAULT |
 *  | SCU/SCP            | SCP              | SCP     |
 *  | SCU/SCP            | SCU              | SCU     |
 *  | SCU/SCP            | SCU/SCP          | SCU/SCP |
 *  | SCU/SCP            | DEFAULT          | DEFAULT |
 *  | DEFAULT            | SCP              | Reject  |
 *  | DEFAULT            | SCU              | DEFAULT |
 *  | DEFAULT            | SCU/SCP          | DEFAULT |
 *  | DEFAULT            | DEFAULT          | DEFAULT |
 *  +--------------------+------------------+---------+
 *  @endverbatim
 *  NONE, SCU, SCP as well as SCU/SCP denote the related flags in the
 *  association role selection user items. The "Reject" case denotes that
 *  such a presentation context will be rejected by the association acceptor:
 *  If the requestor connects with default role but the acceptor explicitly
 *  requires the SCP role (only) then the presentation context
 *  will be rejected. All other cases do not lead to rejection but to actual
 *  "negotiation".
 *
 *  The Reject case can be avoided by setting a related option available in
 *  association acceptance code like ASC_acceptPresentationContext() or DcmSCP.
 *  to OFTrue (reading something like "alwaysAcceptDefaultRole" since when enabled,
 *  with the Reject being disabled all Default role proposals will be accepted).
 *  This can make sense for faulty Requestors, e.g. faulty Storage Commitment Servers
 *  connecting on a second connection for delivering an N-EVENT-REPORT, or broken
 *  Retrieve requestors proposing GET-based SOP Classes for retrieval using the Default
 *  role instead of the required SCP role.
 */
typedef enum {
    DUL_SC_ROLE_NONE,
    DUL_SC_ROLE_DEFAULT,
    DUL_SC_ROLE_SCU,
    DUL_SC_ROLE_SCP,
    DUL_SC_ROLE_SCUSCP
}   DUL_SC_ROLE;

#define DUL_PRESENTATION_ACCEPT                 0
#define DUL_PRESENTATION_REJECT_USER            1
#define DUL_PRESENTATION_REJECT_NOREASON        2
#define DUL_PRESENTATION_REJECT_ABSTRACT_SYNTAX 3
#define DUL_PRESENTATION_REJECT_TRANSFER_SYNTAX 4

typedef OFList<char *> DUL_TRANSFERSYNTAXLIST;

typedef struct {
    void *reserved[2];
    DUL_PRESENTATIONCONTEXTID presentationContextID;
    char abstractSyntax[DUL_LEN_UID + 1];
    LST_HEAD *proposedTransferSyntax;
    char acceptedTransferSyntax[DUL_LEN_UID + 1];
    unsigned char result;
    DUL_SC_ROLE proposedSCRole;
    DUL_SC_ROLE acceptedSCRole;
}   DUL_PRESENTATIONCONTEXT;

typedef struct {
    void *reserved[2];
    char transferSyntax[DUL_LEN_UID + 1];
}   DUL_TRANSFERSYNTAX;

typedef struct dul_abortitems {
    unsigned char result;
    unsigned char source;
    unsigned char reason;
}   DUL_ABORTITEMS;

typedef enum {
    DUL_COMMANDPDV,   /* A command PDV inside a data PDU */
    DUL_DATASETPDV
}   /* A data set PDV inside a data PDU */
    DUL_DATAPDV;

typedef enum {
    DUL_BLOCK,      /* Block on an operation (read, request) */
    DUL_NOBLOCK
}   /* Or return immediately if nothing avail */
    DUL_BLOCKOPTIONS;

typedef struct {
    unsigned long fragmentLength;
    unsigned char presentationContextID;
    DUL_DATAPDV pdvType;
    OFBool lastPDV;
    void *data;
}   DUL_PDV;

typedef struct {
    unsigned long count;
    void *scratch;
    unsigned long scratchLength;
    DUL_ABORTITEMS abort;
    DUL_PDV *pdv;
}   DUL_PDVLIST;

/*  Define the bits that go in the options field for InitializeNetwork
**
**  The low two bits define the byte order of messages at the DICOM
**  level.  This does not define the order of user data inside of a
**  DICOM PDU.
*/

#define DUL_ORDERMASK          0x03  /* The bottom two bits */
#define DUL_ORDERLITTLEENDIAN  0x01
#define DUL_ORDERBIGENDIAN     0x02

#define DUL_DOMAINMASK         0x04
#define DUL_FULLDOMAINNAME     0x04

#define DUL_AEREQUESTOR        "AE REQUESTOR"
#define DUL_AEACCEPTOR         "AE ACCEPTOR"
#define DUL_AEBOTH             "AE BOTH"

/*  These macros define results and reasons for rejecting an association
**  request.  Result is permanent or transient.  There are a number of
**  different reasons for rejecting requests that occur at different layers
 */
#define DUL_REJ_RSLTPERMANENT  0x01
#define DUL_REJ_RSLTTRANSIENT  0x02

/*  These macros define parameters used to construct an ABORT PDU.
**  These include the source of the abort (SCU or SCP) and the
**  reason for the abort.
*/

#define DUL_SCU_INITIATED_ABORT           0x00
#define DUL_SCP_INITIATED_ABORT           0x02

#define DUL_ABORTSERVICEUSER              0x00
#define DUL_ABORTSERVICEPROVIDER          0x02

#define DUL_ABORTNOREASON                 0x00
#define DUL_ABORTUNRECOGNIZEDPDU          0x01
#define DUL_ABORTUNEXPECTEDPDU            0x02
#define DUL_ABORTUNRECOGNIZEDPDUPARAM     0x04
#define DUL_ABORTUNEXPECTEDPDUPARAM       0x05
#define DUL_ABORTINVALIDPDUPARAM          0x06

/*  These macros define parameters used to construct a REJECT PDU.
**  These include the source of the reject (DICOM UL service-user,
**  (DICOM UL service-provider) and the reason for the reject.
*/
#define DUL_REJECT_PERMANENT              0x01
#define DUL_REJECT_TRANSIENT              0x02

#define DUL_ULSU_REJECT                   0x01
#define DUL_ULSP_ACSE_REJECT              0x02
#define DUL_ULSP_PRESENTATION_REJECT      0x03

#define DUL_ULSU_REJ_NOREASON             0x01
#define DUL_ULSU_REJ_UNSUP_APP_CTX_NAME   0x02
#define DUL_ULSU_REJ_UNREC_CALLING_TITLE  0x03
#define DUL_ULSU_REJ_UNREC_CALLED_TITLE   0x07

#define DUL_ULSP_ACSE_REJ_NOREASON        0x01
#define DUL_ULSP_ACSE_UNSUP_PROTOCOL      0x02

#define DUL_ULSP_PRES_REJ_TEMP_CONGEST    0x01
#define DUL_ULSP_PRES_REJ_LIMIT           0x02

#define DUL_ABORTNOREASON                 0x00
#define DUL_ABORTUNRECOGNIZEDPDU          0x01
#define DUL_ABORTUNEXPECTEDPDU            0x02
#define DUL_ABORTUNRECOGNIZEDPDUPARAM     0x04
#define DUL_ABORTUNEXPECTEDPDUPARAM       0x05
#define DUL_ABORTINVALIDPDUPARAM          0x06

/* Define a set of constants and types that let the user get information
** about the Association
*/

typedef enum {
    DUL_K_INTEGER,    /* An integer type */
    DUL_K_STRING      /* A string type */
}   DUL_DATA_TYPE;

typedef enum {
    DUL_K_MAX_PDV_XMIT
}   DUL_ASSOCIATION_PARAMETER;

#define DUL_TIMEOUT 180

/* Operating mode flags as defined in the 1993 toolkit specification.
** Needed for backward compatibility with DCMTK releases prior to 3.0
** (and possibly older CTN releases).
*/

#define DUL_DULCOMPAT     2768240730UL
#define DUL_DIMSECOMPAT   917504UL
#define DUL_MAXPDUCOMPAT  4278190335UL

/* Define the function prototypes for this facility.
**
** First set of functions are for establishing the network and associations.
*/

DCMTK_DCMNET_EXPORT OFCondition
DUL_AcknowledgeAssociationRQ(
  DUL_ASSOCIATIONKEY ** association,
  DUL_ASSOCIATESERVICEPARAMETERS * params,
  int activatePDUStorage);

DCMTK_DCMNET_EXPORT OFCondition
DUL_InitializeNetwork(
  const char *mode,
  void *param,
  int timeout,
  unsigned long
  options,
  DUL_NETWORKKEY ** network);

DCMTK_DCMNET_EXPORT OFCondition
DUL_ReceiveAssociationRQ(
  DUL_NETWORKKEY ** network,
  DUL_BLOCKOPTIONS block,
  int timeout,
  DUL_ASSOCIATESERVICEPARAMETERS * parameters,
  DUL_ASSOCIATIONKEY ** association,
  int activatePDUStorage);

DCMTK_DCMNET_EXPORT OFCondition
DUL_RejectAssociationRQ(
  DUL_ASSOCIATIONKEY ** association,
  DUL_ABORTITEMS * params,
  int activatePDUStorage);

DCMTK_DCMNET_EXPORT OFCondition
DUL_RequestAssociation(
  DUL_NETWORKKEY ** network,
  DUL_BLOCKOPTIONS block,
  int timeout,
  DUL_ASSOCIATESERVICEPARAMETERS * params,
  DUL_ASSOCIATIONKEY ** association,
  int activatePDUStorage);

/* Define functions for releasing/aborting Associations.
*/
DCMTK_DCMNET_EXPORT OFCondition DUL_AbortAssociation(DUL_ASSOCIATIONKEY ** association);
DCMTK_DCMNET_EXPORT OFCondition DUL_DropAssociation(DUL_ASSOCIATIONKEY ** association);
DCMTK_DCMNET_EXPORT OFCondition DUL_DropNetwork(DUL_NETWORKKEY ** network);
DCMTK_DCMNET_EXPORT OFCondition DUL_ReleaseAssociation(DUL_ASSOCIATIONKEY ** association);
DCMTK_DCMNET_EXPORT OFCondition DUL_AcknowledgeRelease(DUL_ASSOCIATIONKEY ** association);

/* Functions for reading/write PDVs inside P DATA PDUs.
*/
DCMTK_DCMNET_EXPORT OFCondition
DUL_ReadPDVs(DUL_ASSOCIATIONKEY ** association,
       DUL_PDVLIST * pdvList, DUL_BLOCKOPTIONS block, int timeout);
DCMTK_DCMNET_EXPORT OFCondition
DUL_WritePDVs(DUL_ASSOCIATIONKEY ** association,
        DUL_PDVLIST * pdvList);
DCMTK_DCMNET_EXPORT OFCondition DUL_NextPDV(DUL_ASSOCIATIONKEY ** association, DUL_PDV * pdv);


/* Miscellaneous functions.
*/
DCMTK_DCMNET_EXPORT OFCondition
DUL_AssociationParameter(DUL_ASSOCIATIONKEY ** association,
       DUL_ASSOCIATION_PARAMETER param, DUL_DATA_TYPE type,
       void *address, size_t length);
DCMTK_DCMNET_EXPORT OFCondition
DUL_MakePresentationCtx(DUL_PRESENTATIONCONTEXT ** ctx,
         DUL_SC_ROLE proposedSCRole, DUL_SC_ROLE acceptedSCRole,
DUL_PRESENTATIONCONTEXTID ctxID, unsigned char reason, const char *abstractSyntax,
      const char *transferSyntax, ...);
DCMTK_DCMNET_EXPORT OFString& DUL_DumpParams(OFString& str, DUL_ASSOCIATESERVICEPARAMETERS * params);
DCMTK_DCMNET_EXPORT OFString& DUL_DumpConnectionParameters(OFString& str, DUL_ASSOCIATIONKEY *association);

DCMTK_DCMNET_EXPORT OFCondition DUL_ClearServiceParameters(DUL_ASSOCIATESERVICEPARAMETERS * params);
DCMTK_DCMNET_EXPORT OFString& dumpExtNegList(OFString& str, SOPClassExtendedNegotiationSubItemList& lst);

/*
** Additional functions (from dulextra.cc) needed to support
** selecting amongst several concurrent associations.
** Andrew Hewett, Institute OFFIS, Oldenburg, Germany.
*/

DCMTK_DCMNET_EXPORT OFBool
DUL_dataWaiting(DUL_ASSOCIATIONKEY * callerAssociation, int timeout);

DCMTK_DCMNET_EXPORT DcmNativeSocketType DUL_networkSocket(DUL_NETWORKKEY * callerNet);

DCMTK_DCMNET_EXPORT OFBool
DUL_associationWaiting(DUL_NETWORKKEY * callerNet, int timeout);

/*
 * functions allowing to retrieve raw A-ASSOCIATE PDUs from the DUL layer
 */
DCMTK_DCMNET_EXPORT void DUL_activateAssociatePDUStorage(DUL_ASSOCIATIONKEY *dulassoc);
DCMTK_DCMNET_EXPORT void DUL_returnAssociatePDUStorage(DUL_ASSOCIATIONKEY *dulassoc, void *& pdu, unsigned long& pdusize);

/* get pointer to transport connection from opaque association pointer */
DCMTK_DCMNET_EXPORT DcmTransportConnection *DUL_getTransportConnection(DUL_ASSOCIATIONKEY * callerAssociation);

/* change transport layer */
DCMTK_DCMNET_EXPORT OFCondition DUL_setTransportLayer(DUL_NETWORKKEY *callerNetworkKey, DcmTransportLayer *newLayer, int takeoverOwnership);

/* activate compatibility mode and callback */
DCMTK_DCMNET_EXPORT void DUL_activateCompatibilityMode(DUL_ASSOCIATIONKEY *dulassoc, unsigned long mode);
DCMTK_DCMNET_EXPORT void DUL_activateCallback(DUL_ASSOCIATIONKEY *dulassoc, DUL_ModeCallback *cb);

/*
 * function allowing to retrieve the peer certificate from the DUL layer
 */
DCMTK_DCMNET_EXPORT unsigned long DUL_getPeerCertificateLength(DUL_ASSOCIATIONKEY *dulassoc);
DCMTK_DCMNET_EXPORT unsigned long DUL_getPeerCertificate(DUL_ASSOCIATIONKEY *dulassoc, void *buf, unsigned long bufLen);

/*
 * functions for multi-process servers
 */

/** this function returns true if the current process was created by the
 *  DICOM network layer during receipt of a TCP transport connection, i.e.
 *  in receiveTransportConnectionTCP().
 *  @return true if current process is a forked child of a multi-process server
 */
DCMTK_DCMNET_EXPORT OFBool DUL_processIsForkedChild();

/** this function marks the current process as a child created by the
 *  DICOM network layer during receipt of a TCP transport connection, i.e.
 *  in receiveTransportConnectionTCP(). The call is not reversible - use with care.
 */
DCMTK_DCMNET_EXPORT void DUL_markProcessAsForkedChild();

/** this helper function calls DUL_markProcessAsForkedChild(), then reads
 *  the socket handle from the pipe opened by the parent process and
 *  stores it in the global variable dcmExternalSocketHandle. This is
 *  in most cases everything needed to prepare the network layer to act
 *  as a forked child on Win32. On other operating system, the function does nothing.
 * @return EC_Normal if successful, an error code otherwise.
 */
DCMTK_DCMNET_EXPORT OFCondition DUL_readSocketHandleAsForkedChild();

/** this function marks the current process as a multi-process server and enables
 *  the creation of child processes for each incoming TCP transport connection
 *  in receiveTransportConnectionTCP().
 *  @param argc argc command line argument counter variable passed to main().
 *     The content of this parameter is ignored on Posix platforms but required
 *     on Win32, where the child process is created with CreateProcess and the
 *     command line parameters have to be passed from parent to child.
 *  @param argv argv command line argument value array passed to main().
 *     The content of this parameter is ignored on Posix platforms but required
 *     on Win32, where the child process is created with CreateProcess and the
 *     command line parameters have to be passed from parent to child.
 */
DCMTK_DCMNET_EXPORT void DUL_requestForkOnTransportConnectionReceipt(int argc, char *argv[]);


/// @deprecated Use OFString& DUL_DumpParams(OFString&, DUL_ASSOCIATESERVICEPARAMETERS) instead.
DCMTK_DCMNET_EXPORT void DUL_DumpParams(DUL_ASSOCIATESERVICEPARAMETERS * params);
/// @deprecated Use OFString& DUL_DumpConnectionParameters(OFString&, DUL_ASSOCIATIONKEY*) instead.
DCMTK_DCMNET_EXPORT void DUL_DumpConnectionParameters(DUL_ASSOCIATIONKEY *association, STD_NAMESPACE ostream& outstream);
/// @deprecated Use OFString& dumpExtNegList(OFString&, SOPClassExtendedNegotiationSubItemList&) instead.
DCMTK_DCMNET_EXPORT void dumpExtNegList(SOPClassExtendedNegotiationSubItemList& lst);

#endif
