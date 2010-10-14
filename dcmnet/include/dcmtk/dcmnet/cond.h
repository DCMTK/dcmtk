/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:21 $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef COND_H
#define COND_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/dcmdata/dcerror.h"

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
 *
 * - In strict mode (compiled with OFCONDITION_STRICT_MODE), additional
 *   restrictions apply.  OFCondition requires explicit copy construction
 *   (not default constructor), and all compatibility aliases/typedefs
 *   are disabled. SUCCESS(cond) is undefined, use cond.good() instead.
 *
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

// condition constants used in the association module
extern const OFCondition ASC_BADPRESENTATIONCONTEXTID;   /* Bad presentation context ID */
extern const OFCondition ASC_MISSINGTRANSFERSYNTAX;      /* Missing transfer syntax */
extern const OFCondition ASC_NULLKEY;                    /* Caller passed in a NULL key */
extern const OFCondition ASC_SHUTDOWNAPPLICATION;        /* Peer requested application shutdown */
extern const OFCondition ASC_USERIDENTIFICATIONFAILED;   /* User Identity Negotiation failed */
extern const OFCondition ASC_SCPSCUROLESELECTIONFAILED;  /* SCP/SCU role selection failed */

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
extern const OFCondition DIMSE_BADCOMMANDTYPE;           /* Bad command type */
extern const OFCondition DIMSE_BADDATA;                  /* Inappropriate data for message (send) */
extern const OFCondition DIMSE_BADMESSAGE;               /* Badly formed message (send) */
extern const OFCondition DIMSE_ILLEGALASSOCIATION;       /* Caller passed in an illegal association */
extern const OFCondition DIMSE_NODATAAVAILABLE;          /* no data (timeout in non-blocking mode) */
extern const OFCondition DIMSE_NOVALIDPRESENTATIONCONTEXTID;  /* no valid presentation context ID */
extern const OFCondition DIMSE_NULLKEY;                  /* Caller passed in a NULL key */
extern const OFCondition DIMSE_OUTOFRESOURCES;           /* out of resources */
extern const OFCondition DIMSE_PARSEFAILED;              /* Failed to parse received message*/
extern const OFCondition DIMSE_READPDVFAILED;            /* Read PDV failed */
extern const OFCondition DIMSE_RECEIVEFAILED;            /* Failed to receive message */
extern const OFCondition DIMSE_SENDFAILED;               /* Failed to send message */
extern const OFCondition DIMSE_UNEXPECTEDPDVTYPE;        /* Unexpected PDV type */
extern const OFCondition DIMSE_NODATADICT;               /* Data dictionary missing */

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

// condition constants used in the DUL module
extern const OFCondition DUL_ASSOCIATIONREJECTED;       /* DUL Association Rejected */
extern const OFCondition DUL_ILLEGALACCEPT;             /* Attempt to accept by requestor */
extern const OFCondition DUL_ILLEGALKEY;                /* Illegal key */
extern const OFCondition DUL_ILLEGALPDU;                /* DUL Illegal or ill-formed PDU */
extern const OFCondition DUL_ILLEGALPDULENGTH;          /* DUL Illegal PDU Length */
extern const OFCondition DUL_ILLEGALREQUEST;            /* Attempt to request by acceptor */
extern const OFCondition DUL_INCORRECTBUFFERLENGTH;     /* DUL Incorrect buffer space allocated for data */
extern const OFCondition DUL_INSUFFICIENTBUFFERLENGTH;  /* DUL Insufficient buffer space allocated for data */
extern const OFCondition DUL_LISTERROR;                 /* DUL List error */
extern const OFCondition DUL_NETWORKCLOSED;             /* DUL network closed */
extern const OFCondition DUL_NOASSOCIATIONREQUEST;      /* No requests for associations for this server */
extern const OFCondition DUL_NOPDVS;                    /* DUL No PDVs available in current buffer */
extern const OFCondition DUL_NULLKEY;                   /* NULL key passed to routine */
extern const OFCondition DUL_PCTRANSLATIONFAILURE;      /* DUL Presentation Context translation failure */
extern const OFCondition DUL_PDATAPDUARRIVED;           /* DUL P-Data PDU arrived */
extern const OFCondition DUL_PEERABORTEDASSOCIATION;    /* Peer aborted Association (or never connected) */
extern const OFCondition DUL_PEERREQUESTEDRELEASE;      /* DUL Peer Requested Release */
extern const OFCondition DUL_READTIMEOUT;               /* DUL network read timeout */
extern const OFCondition DUL_REQUESTASSOCIATIONFAILED;  /* Failed to establish association */
extern const OFCondition DUL_UNEXPECTEDPDU;             /* Received unexpected PDU */
extern const OFCondition DUL_UNSUPPORTEDPEERPROTOCOL;   /* DUL Unsupported peer protocol */
extern const OFCondition DUL_WRONGDATATYPE;             /* DUL Wrong Data Type Specified for Request */


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
class DimseCondition
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


// if not in strict mode, we define a couple of things
// to improve compatibility with existing code.
#ifndef OFCONDITION_STRICT_MODE

// CONDITION is now identical to OFCondition
typedef OFCondition CONDITION;

// the success macro was defined in dicom.h in earlier releases
#define SUCCESS(cond)                 ((cond).good())

// aliases for condition codes
#define ASC_ASSOCIATIONREJECTED       DUL_ASSOCIATIONREJECTED
#define ASC_NORMAL                    EC_Normal
#define ASC_PEERABORTEDASSOCIATION    DUL_PEERABORTEDASSOCIATION
#define ASC_PEERREQUESTEDRELEASE      DUL_PEERREQUESTEDRELEASE
#define ASC_READTIMEOUT               DUL_READTIMEOUT
#define DIMSE_NORMAL                  EC_Normal
#define DIMSE_PEERABORTEDASSOCIATION  DUL_PEERABORTEDASSOCIATION
#define DIMSE_PEERREQUESTEDRELEASE    DUL_PEERREQUESTEDRELEASE

#endif /* !OFCONDITION_STRICT_MODE */

#endif

/*
 * CVS Log
 * $Log: cond.h,v $
 * Revision 1.16  2010-10-14 13:17:21  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.15  2010-09-09 08:32:13  joergr
 * Fixed typo in OFCondition constants for SCP/SCU role selection failures.
 *
 * Revision 1.14  2010-08-26 09:27:21  joergr
 * Fixed incorrect behavior of association acceptors during SCP/SCU role
 * selection negotiation.
 *
 * Revision 1.13  2009-11-18 11:53:58  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.12  2008-09-08 13:16:11  joergr
 * Fixed typo in OFCondition text string.
 *
 * Revision 1.11  2008-04-17 15:28:33  onken
 * Reworked and extended User Identity Negotiation code.
 *
 * Revision 1.10  2005-12-08 16:02:07  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.9  2005/11/25 11:31:11  meichel
 * StoreSCP now supports multi-process mode both on Posix and Win32 platforms
 *   where a separate client process is forked for each incoming association.
 *
 * Revision 1.8  2003/06/10 13:37:36  meichel
 * Added support for TCP wrappers in DICOM network layer
 *
 * Revision 1.7  2002/12/10 11:00:57  meichel
 * Removed error code DUL_NETWORKINITIALIZED which is not used anymore
 *
 * Revision 1.6  2001/10/12 10:18:25  meichel
 * Replaced the CONDITION types, constants and functions in the dcmnet module
 *   by an OFCondition based implementation which eliminates the global condition
 *   stack.  This is a major change, caveat emptor!
 *
 *
 */
