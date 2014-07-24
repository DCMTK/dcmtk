/*
 *
 *  Copyright (C) 1994-2012, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: network conditions and helper class
 *
 */

#ifndef COND_H
#define COND_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmnet/dndefine.h"

/* NOTES ON THE CONVERSION OF THE DCMNET MODULE FROM CONDITION TO OFCONDITION
 *
 * All functions in this module have been adapted to the new OFCondition
 * class which - unlike the CONDITION functions - does not create problems
 * in multi-threaded code. The following notes explain the major differences
 * and modifications needed in client code.
 *
 * - OFCondition is a class (CONDITION was just a typedef for long).  This
 *   means that switch statements on conditions are not possible anymore.
 *   Use if/else if/else constructs instead.
 *
 * - The condition ASC_RELEASECONFIRMED has been removed.
 *   ASC_releaseAssociation() now returns EC_Normal upon successful completion.
 *
 * - No translation of error codes that are passed from the DUL module to the
 *   ASC or DIMSE module is performed anymore.  That means that error codes
 *   such as DUL_PEERABORTEDASSOCIATION or DUL_PEERREQUESTEDRELEASE are now
 *   passed back to client code instead of their "aliases" with ASC_ or
 *   DIMSE_ prefix.  Macros have been defined to catch most of these cases.
 *
 * - All normal status codes (DUL_NORMAL, DIMSE_NORMAL, ASC_NORMAL) have been
 *   mapped to EC_Normal. Macros have also been defined for these identifiers.
 *
 * - DUL_PEERREQUESTEDRELEASE is now an error code, i.e. unlike prior releases
 *   SUCCESS(DUL_PEERREQUESTEDRELEASE) is false.
 */

// condition code constants used in the association module
const unsigned short ASCC_BADPRESENTATIONCONTEXTID       = 0x101;
const unsigned short ASCC_BADPRESENTATIONCONTEXTPOSITION = 0x102;
const unsigned short ASCC_CODINGERROR                    = 0x103;
const unsigned short ASCC_DUPLICATEPRESENTATIONCONTEXTID = 0x104;
const unsigned short ASCC_MISSINGTRANSFERSYNTAX          = 0x105;
const unsigned short ASCC_NULLKEY                        = 0x106;
const unsigned short ASCC_SHUTDOWNAPPLICATION            = 0x107;
const unsigned short ASCC_USERIDENTIFICATIONFAILED       = 0x108;
const unsigned short ASCC_SCPSCUROLESELECTIONFAILED      = 0x109;
const unsigned short ASCC_NOPRESENTATIONCONTEXTPROPOSED  = 0x110;

// condition constants used in the association module
extern DCMTK_DCMNET_EXPORT const OFConditionConst ASC_BADPRESENTATIONCONTEXTID;      /* Bad presentation context ID */
extern DCMTK_DCMNET_EXPORT const OFConditionConst ASC_MISSINGTRANSFERSYNTAX;         /* Missing transfer syntax */
extern DCMTK_DCMNET_EXPORT const OFConditionConst ASC_NULLKEY;                       /* Caller passed in a NULL key */
extern DCMTK_DCMNET_EXPORT const OFConditionConst ASC_SHUTDOWNAPPLICATION;           /* Peer requested application shutdown */
extern DCMTK_DCMNET_EXPORT const OFConditionConst ASC_USERIDENTIFICATIONFAILED;      /* User Identity Negotiation failed */
extern DCMTK_DCMNET_EXPORT const OFConditionConst ASC_SCPSCUROLESELECTIONFAILED;     /* SCP/SCU role selection failed */
extern DCMTK_DCMNET_EXPORT const OFConditionConst ASC_NOPRESENTATIONCONTEXTPROPOSED; /* No presentation context proposed */

// condition code constants used in the DIMSE module
const unsigned short DIMSEC_BADCOMMANDTYPE               = 0x201;
const unsigned short DIMSEC_BADDATA                      = 0x202;
const unsigned short DIMSEC_BADMESSAGE                   = 0x203;
const unsigned short DIMSEC_BUILDFAILED                  = 0x204;
const unsigned short DIMSEC_ILLEGALASSOCIATION           = 0x205;
const unsigned short DIMSEC_INVALIDPRESENTATIONCONTEXTID = 0x206;
const unsigned short DIMSEC_NODATAAVAILABLE              = 0x207;
const unsigned short DIMSEC_NOVALIDPRESENTATIONCONTEXTID = 0x208;
const unsigned short DIMSEC_NULLKEY                      = 0x209;
const unsigned short DIMSEC_OUTOFRESOURCES               = 0x20a;
const unsigned short DIMSEC_PARSEFAILED                  = 0x20b;
const unsigned short DIMSEC_READPDVFAILED                = 0x20c;
const unsigned short DIMSEC_RECEIVEFAILED                = 0x20d;
const unsigned short DIMSEC_SENDFAILED                   = 0x20e;
const unsigned short DIMSEC_UNEXPECTEDPDVTYPE            = 0x20f;
const unsigned short DIMSEC_UNEXPECTEDREQUEST            = 0x210;
const unsigned short DIMSEC_UNEXPECTEDRESPONSE           = 0x211;
const unsigned short DIMSEC_UNSUPPORTEDTRANSFERSYNTAX    = 0x212;
const unsigned short DIMSEC_NODATADICT                   = 0x213;

// condition constants used in the DIMSE module
extern DCMTK_DCMNET_EXPORT const OFConditionConst DIMSE_BADCOMMANDTYPE;           /* Bad command type */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DIMSE_BADDATA;                  /* Inappropriate data for message (send) */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DIMSE_BADMESSAGE;               /* Badly formed message (send) */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DIMSE_ILLEGALASSOCIATION;       /* Caller passed in an illegal association */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DIMSE_NODATAAVAILABLE;          /* No data (timeout in non-blocking mode) */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DIMSE_NOVALIDPRESENTATIONCONTEXTID;  /* No valid presentation context ID */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DIMSE_NULLKEY;                  /* Caller passed in a NULL key */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DIMSE_OUTOFRESOURCES;           /* Out of resources */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DIMSE_PARSEFAILED;              /* Failed to parse received message*/
extern DCMTK_DCMNET_EXPORT const OFConditionConst DIMSE_READPDVFAILED;            /* Read PDV failed */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DIMSE_RECEIVEFAILED;            /* Failed to receive message */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DIMSE_SENDFAILED;               /* Failed to send message */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DIMSE_UNEXPECTEDPDVTYPE;        /* Unexpected PDV type */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DIMSE_NODATADICT;               /* Data dictionary missing */

// condition code constants used in the DUL module
const unsigned short DULC_ASSOCIATIONREJECTED              = 0x301;
const unsigned short DULC_CODINGERROR                      = 0x302;
const unsigned short DULC_FSMERROR                         = 0x303;
const unsigned short DULC_ILLEGALACCEPT                    = 0x304;
const unsigned short DULC_ILLEGALKEY                       = 0x305;
const unsigned short DULC_ILLEGALPARAMETER                 = 0x306;
const unsigned short DULC_ILLEGALPDU                       = 0x307;
const unsigned short DULC_ILLEGALPDULENGTH                 = 0x308;
const unsigned short DULC_ILLEGALREJECTREASON              = 0x309;
const unsigned short DULC_ILLEGALREJECTRESULT              = 0x30a;
const unsigned short DULC_ILLEGALREQUEST                   = 0x30b;
const unsigned short DULC_ILLEGALSERVICEPARAMETER          = 0x30c;
const unsigned short DULC_INCORRECTBUFFERLENGTH            = 0x30d;
const unsigned short DULC_INSUFFICIENTBUFFERLENGTH         = 0x30e;
const unsigned short DULC_LISTERROR                        = 0x30f;
const unsigned short DULC_NETWORKCLOSED                    = 0x310;
// 0x311 was NETWORKINITIALIZED (Network already initialized) in previous releases
const unsigned short DULC_NOASSOCIATIONREQUEST             = 0x312;
const unsigned short DULC_NOPDVS                           = 0x313;
const unsigned short DULC_NULLKEY                          = 0x314;
const unsigned short DULC_PCTRANSLATIONFAILURE             = 0x315;
const unsigned short DULC_PDATAPDUARRIVED                  = 0x316;
const unsigned short DULC_PEERABORTEDASSOCIATION           = 0x317;
const unsigned short DULC_PEERILLEGALXFERSYNTAXCOUNT       = 0x318;
const unsigned short DULC_PEERREQUESTEDRELEASE             = 0x319;
const unsigned short DULC_READTIMEOUT                      = 0x31a;
const unsigned short DULC_REQUESTASSOCIATIONFAILED         = 0x31b;
const unsigned short DULC_TCPINITERROR                     = 0x31c;
const unsigned short DULC_TCPIOERROR                       = 0x31d;
const unsigned short DULC_TLSERROR                         = 0x31e;
const unsigned short DULC_UNEXPECTEDPDU                    = 0x31f;
const unsigned short DULC_UNKNOWNHOST                      = 0x320;
const unsigned short DULC_UNRECOGNIZEDPDUTYPE              = 0x321;
const unsigned short DULC_UNSUPPORTEDPEERPROTOCOL          = 0x322;
const unsigned short DULC_WRONGDATATYPE                    = 0x323;
const unsigned short DULC_TCPWRAPPER                       = 0x324;
const unsigned short DULC_FORKEDCHILD                      = 0x325;
const unsigned short DULC_CANNOTFORK                       = 0x326;
const unsigned short DULC_ILLEGALREJECTSOURCE              = 0x327;

// condition constants used in the DUL module
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_ASSOCIATIONREJECTED;       /* DUL Association Rejected */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_ILLEGALACCEPT;             /* Attempt to accept by requestor */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_ILLEGALKEY;                /* Illegal key */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_ILLEGALPDU;                /* DUL Illegal or ill-formed PDU */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_ILLEGALPDULENGTH;          /* DUL Illegal PDU Length */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_ILLEGALREQUEST;            /* Attempt to request by acceptor */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_INCORRECTBUFFERLENGTH;     /* DUL Incorrect buffer space allocated for data */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_INSUFFICIENTBUFFERLENGTH;  /* DUL Insufficient buffer space allocated for data */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_LISTERROR;                 /* DUL List error */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_NETWORKCLOSED;             /* DUL network closed */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_NOASSOCIATIONREQUEST;      /* No requests for associations for this server */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_NOPDVS;                    /* DUL No PDVs available in current buffer */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_NULLKEY;                   /* NULL key passed to routine */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_PCTRANSLATIONFAILURE;      /* DUL Presentation Context translation failure */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_PDATAPDUARRIVED;           /* DUL P-Data PDU arrived */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_PEERABORTEDASSOCIATION;    /* Peer aborted Association (or never connected) */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_PEERREQUESTEDRELEASE;      /* DUL Peer Requested Release */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_READTIMEOUT;               /* DUL network read timeout */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_REQUESTASSOCIATIONFAILED;  /* Failed to establish association */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_UNEXPECTEDPDU;             /* Received unexpected PDU */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_UNSUPPORTEDPEERPROTOCOL;   /* DUL Unsupported peer protocol */
extern DCMTK_DCMNET_EXPORT const OFConditionConst DUL_WRONGDATATYPE;             /* DUL Wrong Data Type Specified for Request */

// other condition constants used in the network module (codes 1000 to 1023)
extern DCMTK_DCMNET_EXPORT const OFConditionConst NET_EC_InvalidSOPClassUID;               /* Invalid SOP Class UID */
extern DCMTK_DCMNET_EXPORT const OFConditionConst NET_EC_UnknownStorageSOPClass;           /* Unknown Storage SOP Class */
extern DCMTK_DCMNET_EXPORT const OFConditionConst NET_EC_InvalidSOPInstanceUID;            /* Invalid SOP Instance UID */
extern DCMTK_DCMNET_EXPORT const OFConditionConst NET_EC_InvalidTransferSyntaxUID;         /* Invalid Transfer Syntax UID */
extern DCMTK_DCMNET_EXPORT const OFConditionConst NET_EC_UnknownTransferSyntax;            /* Unknown Transfer Syntax */
extern DCMTK_DCMNET_EXPORT const OFConditionConst NET_EC_NoPresentationContextsDefined;    /* No Presentation Contexts defined */
extern DCMTK_DCMNET_EXPORT const OFConditionConst NET_EC_NoAcceptablePresentationContexts; /* No acceptable Presentation Contexts */
extern DCMTK_DCMNET_EXPORT const OFConditionConst NET_EC_NoSOPInstancesToSend;             /* No SOP instances to send */
extern DCMTK_DCMNET_EXPORT const OFConditionConst NET_EC_NoSuchSOPInstance;                /* No such SOP instance */
extern DCMTK_DCMNET_EXPORT const OFConditionConst NET_EC_InvalidDatasetPointer;            /* Invalid dataset pointer */
extern DCMTK_DCMNET_EXPORT const OFConditionConst NET_EC_AlreadyConnected;                 /* Already connected */
extern DCMTK_DCMNET_EXPORT const OFConditionConst NET_EC_InsufficientPortPrivileges;       /* Insufficient Port Privileges */
// codes 1024 to 1073 are used for the association negotiation profile classes
extern DCMTK_DCMNET_EXPORT const OFConditionConst NET_EC_SCPBusy;                          /* SCP is busy */
extern DCMTK_DCMNET_EXPORT const OFConditionConst NET_EC_CannotStartSCPThread;             /* Cannot start SCP thread */

// This macro creates a condition with given code, severity and text.
// Making this a macro instead of a function saves the creation of a temporary.
#define makeDcmnetCondition(A, B, C) makeOFCondition(OFM_dcmnet, (A), (B), (C))

// This macro creates a condition with given code, severity, text, and subcondition.
// This method is intended as a replacement for COND_PushCondition().
// Instead of maintaining a global condition stack (which is difficult
// in multi-threaded applications), the error text of the sub-condition
// is appended to the newly created condition.
// Making this a macro instead of a function saves the creation of a temporary.
#define makeDcmnetSubCondition(A, B, C, D) DimseCondition::push(OFM_dcmnet, (A), (B), (C), (D))


/** helper class with static methods that facilitate the
 *  migration from the old stack-based condition handling code
 *  in module dcmnet to the new class based code.
 */
class DCMTK_DCMNET_EXPORT DimseCondition
{
public:

  /** Get a string representation for the given condition code
   *  This method is intended as a replacement for COND_DumpCondition().
   *  Since no global condition stack exists anymore, the condition
   *  must be passed to this method.
   *  @param str string to dump into
   *  @param cond condition to be dumped
   *  @return reference to string
   */
  static OFString& dump(OFString& str, OFCondition cond);

  /** dumps a given condition code to the console.
   *  This method is intended as a replacement for COND_DumpCondition().
   *  Since no global condition stack exists anymore, the condition
   *  must be passed to this method.
   *  @param cond condition to be dumped
   *  @param console console to dump to
   *  @deprecated Please use the other dump() function instead!
   */
  static void dump(OFCondition cond, OFConsole& console = ofConsole);

  /** creates a new condition from the given parameters and the sub-condition.
   *  This method is intended as a replacement for COND_PushCondition().
   *  Instead of maintaining a global condition stack (which is difficult
   *  in multi-threaded applications), the error text of the sub-condition
   *  is appended to the newly created condition.
   */
  static OFCondition push(
    unsigned short aModule,
    unsigned short aCode,
    OFStatus aStatus,
    const char *aText,
    OFCondition subCondition);

  /** creates a new hierarchical condition from the given condition and sub-condition.
   *  This method is intended as a replacement for COND_PushCondition().
   *  Instead of maintaining a global condition stack (which is difficult
   *  in multi-threaded applications), the error text of the sub-condition
   *  is appended to the newly created condition.
   */
  static OFCondition push(
    OFCondition newCondition,
    OFCondition subCondition);

};


#endif
