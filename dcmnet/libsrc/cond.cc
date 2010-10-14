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
 *  Update Date:      $Date: 2010-10-14 13:14:27 $
 *  CVS/RCS Revision: $Revision: 1.21 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/ofstd/ofstream.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

const OFConditionConst ASCE_BADPRESENTATIONCONTEXTID( OFM_dcmnet, ASCC_BADPRESENTATIONCONTEXTID,  OF_error, "ASC Bad presentation context ID");
const OFConditionConst ASCE_MISSINGTRANSFERSYNTAX(    OFM_dcmnet, ASCC_MISSINGTRANSFERSYNTAX,     OF_error, "ASC Missing transfer syntax");
const OFConditionConst ASCE_NULLKEY(                  OFM_dcmnet, ASCC_NULLKEY,                   OF_error, "ASC Caller passed in a NULL key");
const OFConditionConst ASCE_SHUTDOWNAPPLICATION(      OFM_dcmnet, ASCC_SHUTDOWNAPPLICATION,       OF_error, "ASC Application shutdown requested");
const OFConditionConst ASCE_USERIDENTIFICATIONFAILED( OFM_dcmnet, ASCC_USERIDENTIFICATIONFAILED,  OF_error, "ASC User Identification Negotiation failed");
const OFConditionConst ASCE_SCPSCUROLESELECTIONFAILED(OFM_dcmnet, ASCC_SCPSCUROLESELECTIONFAILED, OF_error, "ASC SCP/SCU role selection failed");

const OFCondition ASC_BADPRESENTATIONCONTEXTID( ASCE_BADPRESENTATIONCONTEXTID);
const OFCondition ASC_MISSINGTRANSFERSYNTAX(    ASCE_MISSINGTRANSFERSYNTAX);
const OFCondition ASC_NULLKEY(                  ASCE_NULLKEY);
const OFCondition ASC_SHUTDOWNAPPLICATION(      ASCE_SHUTDOWNAPPLICATION);
const OFCondition ASC_USERIDENTIFICATIONFAILED( ASCE_USERIDENTIFICATIONFAILED);
const OFCondition ASC_SCPSCUROLESELECTIONFAILED(ASCE_SCPSCUROLESELECTIONFAILED);

const OFConditionConst DIMSEE_BADCOMMANDTYPE(               OFM_dcmnet, DIMSEC_BADCOMMANDTYPE,               OF_error, "DIMSE Bad command type");
const OFConditionConst DIMSEE_BADDATA(                      OFM_dcmnet, DIMSEC_BADDATA,                      OF_error, "DIMSE Inappropriate data for message");
const OFConditionConst DIMSEE_BADMESSAGE(                   OFM_dcmnet, DIMSEC_BADMESSAGE,                   OF_error, "DIMSE Badly formed message");
const OFConditionConst DIMSEE_ILLEGALASSOCIATION(           OFM_dcmnet, DIMSEC_ILLEGALASSOCIATION,           OF_error, "DIMSE Caller passed in an illegal association");
const OFConditionConst DIMSEE_NODATAAVAILABLE(              OFM_dcmnet, DIMSEC_NODATAAVAILABLE,              OF_error, "DIMSE No data available (timeout in non-blocking mode)");
const OFConditionConst DIMSEE_NOVALIDPRESENTATIONCONTEXTID( OFM_dcmnet, DIMSEC_NOVALIDPRESENTATIONCONTEXTID, OF_error, "DIMSE No valid Presentation Context ID");
const OFConditionConst DIMSEE_NULLKEY(                      OFM_dcmnet, DIMSEC_NULLKEY,                      OF_error, "DIMSE Caller passed in a NULL key");
const OFConditionConst DIMSEE_OUTOFRESOURCES(               OFM_dcmnet, DIMSEC_OUTOFRESOURCES,               OF_error, "DIMSE Out of resources");
const OFConditionConst DIMSEE_PARSEFAILED(                  OFM_dcmnet, DIMSEC_PARSEFAILED,                  OF_error, "DIMSE Failed to parse received message");
const OFConditionConst DIMSEE_READPDVFAILED(                OFM_dcmnet, DIMSEC_READPDVFAILED,                OF_error, "DIMSE Read PDV failed");
const OFConditionConst DIMSEE_RECEIVEFAILED(                OFM_dcmnet, DIMSEC_RECEIVEFAILED,                OF_error, "DIMSE Failed to receive message");
const OFConditionConst DIMSEE_SENDFAILED(                   OFM_dcmnet, DIMSEC_SENDFAILED,                   OF_error, "DIMSE Failed to send message");
const OFConditionConst DIMSEE_UNEXPECTEDPDVTYPE(            OFM_dcmnet, DIMSEC_UNEXPECTEDPDVTYPE,            OF_error, "DIMSE Unexpected PDV type");
const OFConditionConst DIMSEE_NODATADICT(                   OFM_dcmnet, DIMSEC_NODATADICT,                   OF_error, "Data dictionary missing");

const OFCondition DIMSE_BADCOMMANDTYPE(               DIMSEE_BADCOMMANDTYPE);
const OFCondition DIMSE_BADDATA(                      DIMSEE_BADDATA);
const OFCondition DIMSE_BADMESSAGE(                   DIMSEE_BADMESSAGE);
const OFCondition DIMSE_ILLEGALASSOCIATION(           DIMSEE_ILLEGALASSOCIATION);
const OFCondition DIMSE_NODATAAVAILABLE(              DIMSEE_NODATAAVAILABLE);
const OFCondition DIMSE_NULLKEY(                      DIMSEE_NULLKEY);
const OFCondition DIMSE_NOVALIDPRESENTATIONCONTEXTID( DIMSEE_NOVALIDPRESENTATIONCONTEXTID);
const OFCondition DIMSE_OUTOFRESOURCES(               DIMSEE_OUTOFRESOURCES);
const OFCondition DIMSE_PARSEFAILED(                  DIMSEE_PARSEFAILED);
const OFCondition DIMSE_READPDVFAILED(                DIMSEE_READPDVFAILED);
const OFCondition DIMSE_RECEIVEFAILED(                DIMSEE_RECEIVEFAILED);
const OFCondition DIMSE_SENDFAILED(                   DIMSEE_SENDFAILED);
const OFCondition DIMSE_UNEXPECTEDPDVTYPE(            DIMSEE_UNEXPECTEDPDVTYPE);
const OFCondition DIMSE_NODATADICT(                   DIMSEE_NODATADICT);

const OFConditionConst DULE_ASSOCIATIONREJECTED(      OFM_dcmnet, DULC_ASSOCIATIONREJECTED     , OF_error, "DUL Association Rejected");
const OFConditionConst DULE_ILLEGALACCEPT(            OFM_dcmnet, DULC_ILLEGALACCEPT           , OF_error, "Attempt to accept by requestor");
const OFConditionConst DULE_ILLEGALKEY(               OFM_dcmnet, DULC_ILLEGALKEY              , OF_error, "Illegal key");
const OFConditionConst DULE_ILLEGALPDU(               OFM_dcmnet, DULC_ILLEGALPDU              , OF_error, "DUL Illegal or ill-formed PDU");
const OFConditionConst DULE_ILLEGALPDULENGTH(         OFM_dcmnet, DULC_ILLEGALPDULENGTH        , OF_error, "DUL Illegal PDU Length");
const OFConditionConst DULE_ILLEGALREQUEST(           OFM_dcmnet, DULC_ILLEGALREQUEST          , OF_error, "Attempt to request by acceptor");
const OFConditionConst DULE_INCORRECTBUFFERLENGTH(    OFM_dcmnet, DULC_INCORRECTBUFFERLENGTH   , OF_error, "DUL Incorrect buffer space allocated for data");
const OFConditionConst DULE_INSUFFICIENTBUFFERLENGTH( OFM_dcmnet, DULC_INSUFFICIENTBUFFERLENGTH, OF_error, "DUL Insufficient buffer space allocated for data");
const OFConditionConst DULE_LISTERROR(                OFM_dcmnet, DULC_LISTERROR               , OF_error, "DUL List error");
const OFConditionConst DULE_NETWORKCLOSED(            OFM_dcmnet, DULC_NETWORKCLOSED           , OF_error, "DUL network closed");
const OFConditionConst DULE_NOASSOCIATIONREQUEST(     OFM_dcmnet, DULC_NOASSOCIATIONREQUEST    , OF_error, "No requests for associations for this server");
const OFConditionConst DULE_NOPDVS(                   OFM_dcmnet, DULC_NOPDVS                  , OF_error, "DUL No PDVs available in current buffer");
const OFConditionConst DULE_NULLKEY(                  OFM_dcmnet, DULC_NULLKEY                 , OF_error, "NULL key passed to routine");
const OFConditionConst DULE_PCTRANSLATIONFAILURE(     OFM_dcmnet, DULC_PCTRANSLATIONFAILURE    , OF_error, "DUL Presentation Context translation failure");
const OFConditionConst DULE_PDATAPDUARRIVED(          OFM_dcmnet, DULC_PDATAPDUARRIVED         , OF_error, "DUL P-Data PDU arrived");
const OFConditionConst DULE_PEERABORTEDASSOCIATION(   OFM_dcmnet, DULC_PEERABORTEDASSOCIATION  , OF_error, "Peer aborted Association (or never connected)");
const OFConditionConst DULE_PEERREQUESTEDRELEASE(     OFM_dcmnet, DULC_PEERREQUESTEDRELEASE    , OF_error, "DUL Peer Requested Release");
const OFConditionConst DULE_READTIMEOUT(              OFM_dcmnet, DULC_READTIMEOUT             , OF_error, "DUL network read timeout");
const OFConditionConst DULE_REQUESTASSOCIATIONFAILED( OFM_dcmnet, DULC_REQUESTASSOCIATIONFAILED, OF_error, "Failed to establish association");
const OFConditionConst DULE_UNEXPECTEDPDU(            OFM_dcmnet, DULC_UNEXPECTEDPDU           , OF_error, "Received unexpected PDU");
const OFConditionConst DULE_UNSUPPORTEDPEERPROTOCOL(  OFM_dcmnet, DULC_UNSUPPORTEDPEERPROTOCOL , OF_error, "DUL Unsupported peer protocol");
const OFConditionConst DULE_WRONGDATATYPE(            OFM_dcmnet, DULC_WRONGDATATYPE           , OF_error, "DUL Wrong Data Type Specified for Request");

const OFCondition DUL_ASSOCIATIONREJECTED(      DULE_ASSOCIATIONREJECTED);
const OFCondition DUL_ILLEGALACCEPT(            DULE_ILLEGALACCEPT);
const OFCondition DUL_ILLEGALKEY(               DULE_ILLEGALKEY);
const OFCondition DUL_ILLEGALPDU(               DULE_ILLEGALPDU);
const OFCondition DUL_ILLEGALPDULENGTH(         DULE_ILLEGALPDULENGTH);
const OFCondition DUL_ILLEGALREQUEST(           DULE_ILLEGALREQUEST);
const OFCondition DUL_INCORRECTBUFFERLENGTH(    DULE_INCORRECTBUFFERLENGTH);
const OFCondition DUL_INSUFFICIENTBUFFERLENGTH( DULE_INSUFFICIENTBUFFERLENGTH);
const OFCondition DUL_LISTERROR(                DULE_LISTERROR);
const OFCondition DUL_NETWORKCLOSED(            DULE_NETWORKCLOSED);
const OFCondition DUL_NOASSOCIATIONREQUEST(     DULE_NOASSOCIATIONREQUEST);
const OFCondition DUL_NOPDVS(                   DULE_NOPDVS);
const OFCondition DUL_NULLKEY(                  DULE_NULLKEY);
const OFCondition DUL_PCTRANSLATIONFAILURE(     DULE_PCTRANSLATIONFAILURE);
const OFCondition DUL_PDATAPDUARRIVED(          DULE_PDATAPDUARRIVED);
const OFCondition DUL_PEERABORTEDASSOCIATION(   DULE_PEERABORTEDASSOCIATION);
const OFCondition DUL_PEERREQUESTEDRELEASE(     DULE_PEERREQUESTEDRELEASE);
const OFCondition DUL_READTIMEOUT(              DULE_READTIMEOUT);
const OFCondition DUL_REQUESTASSOCIATIONFAILED( DULE_REQUESTASSOCIATIONFAILED);
const OFCondition DUL_UNEXPECTEDPDU(            DULE_UNEXPECTEDPDU);
const OFCondition DUL_UNSUPPORTEDPEERPROTOCOL(  DULE_UNSUPPORTEDPEERPROTOCOL);
const OFCondition DUL_WRONGDATATYPE(            DULE_WRONGDATATYPE);

OFString& DimseCondition::dump(OFString& str, OFCondition cond)
{
  char buf[16];
  sprintf(buf,"%04x:%04x ", cond.module(), cond.code());
  str = buf;
  str += cond.text();
  return str;
}

void DimseCondition::dump(OFCondition cond, OFConsole& console)
{
    OFString str;
    dump(str, cond);
    console.lockCerr() << str << OFendl;
    console.unlockCerr();
}

OFCondition DimseCondition::push(
    unsigned short aModule,
    unsigned short aCode,
    OFStatus aStatus,
    const char *aText,
    OFCondition subCondition)
{
  OFOStringStream os;
  /* declare variable outside the block structure of the OFSTRINGSTREAM_xxx macros */
  OFConditionString *condString;
  char buf[16];
  sprintf(buf,"%04x:%04x ", subCondition.module(), subCondition.code());
  os << aText << OFendl << buf << subCondition.text() << OFStringStream_ends;
  OFSTRINGSTREAM_GETSTR(os, c)
  condString = new OFConditionString(aModule, aCode, aStatus, c);
  OFSTRINGSTREAM_FREESTR(c)
  return OFCondition(condString);
}

OFCondition DimseCondition::push(
  OFCondition newCondition,
  OFCondition subCondition)
{
  return DimseCondition::push(newCondition.module(), newCondition.code(),
    newCondition.status(), newCondition.text(), subCondition);
}

/*
 * CVS Log
 * $Log: cond.cc,v $
 * Revision 1.21  2010-10-14 13:14:27  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.20  2010-09-09 08:32:06  joergr
 * Fixed typo in OFCondition constants for SCP/SCU role selection failures.
 *
 * Revision 1.19  2010-08-26 09:22:24  joergr
 * Fixed incorrect behavior of association acceptors during SCP/SCU role
 * selection negotiation.
 *
 * Revision 1.18  2009-11-18 11:53:59  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.17  2008-09-08 13:17:13  joergr
 * Fixed typo in OFCondition text string.
 *
 * Revision 1.16  2008-04-17 15:27:35  onken
 * Reworked and extended User Identity Negotiation code.
 *
 * Revision 1.15  2006-08-15 16:04:29  meichel
 * Updated the code in module dcmnet to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.14  2005/12/08 15:44:25  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.13  2004/02/04 15:35:17  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.12  2002/12/10 11:00:59  meichel
 * Removed error code DUL_NETWORKINITIALIZED which is not used anymore
 *
 * Revision 1.11  2002/11/27 13:04:37  meichel
 * Adapted module dcmnet to use of new header file ofstdinc.h
 *
 * Revision 1.10  2002/05/02 14:07:37  joergr
 * Added support for standard and non-standard string streams (which one is
 * supported is detected automatically via the configure mechanism).
 *
 * Revision 1.9  2002/04/16 13:57:31  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.8  2001/10/12 10:18:30  meichel
 * Replaced the CONDITION types, constants and functions in the dcmnet module
 *   by an OFCondition based implementation which eliminates the global condition
 *   stack.  This is a major change, caveat emptor!
 *
 *
 */
