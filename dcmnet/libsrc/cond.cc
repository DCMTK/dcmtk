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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2012-02-15 14:50:42 $
 *  CVS/RCS Revision: $Revision: 1.27 $
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

makeOFConditionConst(ASC_BADPRESENTATIONCONTEXTID, OFM_dcmnet, ASCC_BADPRESENTATIONCONTEXTID,  OF_error, "ASC Bad presentation context ID");
makeOFConditionConst(ASC_MISSINGTRANSFERSYNTAX,    OFM_dcmnet, ASCC_MISSINGTRANSFERSYNTAX,     OF_error, "ASC Missing transfer syntax");
makeOFConditionConst(ASC_NULLKEY,                  OFM_dcmnet, ASCC_NULLKEY,                   OF_error, "ASC Caller passed in a NULL key");
makeOFConditionConst(ASC_SHUTDOWNAPPLICATION,      OFM_dcmnet, ASCC_SHUTDOWNAPPLICATION,       OF_error, "ASC Application shutdown requested");
makeOFConditionConst(ASC_USERIDENTIFICATIONFAILED, OFM_dcmnet, ASCC_USERIDENTIFICATIONFAILED,  OF_error, "ASC User Identification Negotiation failed");
makeOFConditionConst(ASC_SCPSCUROLESELECTIONFAILED,OFM_dcmnet, ASCC_SCPSCUROLESELECTIONFAILED, OF_error, "ASC SCP/SCU role selection failed");

makeOFConditionConst(DIMSE_BADCOMMANDTYPE,               OFM_dcmnet, DIMSEC_BADCOMMANDTYPE,               OF_error, "DIMSE Bad command type");
makeOFConditionConst(DIMSE_BADDATA,                      OFM_dcmnet, DIMSEC_BADDATA,                      OF_error, "DIMSE Inappropriate data for message");
makeOFConditionConst(DIMSE_BADMESSAGE,                   OFM_dcmnet, DIMSEC_BADMESSAGE,                   OF_error, "DIMSE Badly formed message");
makeOFConditionConst(DIMSE_ILLEGALASSOCIATION,           OFM_dcmnet, DIMSEC_ILLEGALASSOCIATION,           OF_error, "DIMSE Caller passed in an illegal association");
makeOFConditionConst(DIMSE_NODATAAVAILABLE,              OFM_dcmnet, DIMSEC_NODATAAVAILABLE,              OF_error, "DIMSE No data available (timeout in non-blocking mode)");
makeOFConditionConst(DIMSE_NOVALIDPRESENTATIONCONTEXTID, OFM_dcmnet, DIMSEC_NOVALIDPRESENTATIONCONTEXTID, OF_error, "DIMSE No valid Presentation Context ID");
makeOFConditionConst(DIMSE_NULLKEY,                      OFM_dcmnet, DIMSEC_NULLKEY,                      OF_error, "DIMSE Caller passed in a NULL key");
makeOFConditionConst(DIMSE_OUTOFRESOURCES,               OFM_dcmnet, DIMSEC_OUTOFRESOURCES,               OF_error, "DIMSE Out of resources");
makeOFConditionConst(DIMSE_PARSEFAILED,                  OFM_dcmnet, DIMSEC_PARSEFAILED,                  OF_error, "DIMSE Failed to parse received message");
makeOFConditionConst(DIMSE_READPDVFAILED,                OFM_dcmnet, DIMSEC_READPDVFAILED,                OF_error, "DIMSE Read PDV failed");
makeOFConditionConst(DIMSE_RECEIVEFAILED,                OFM_dcmnet, DIMSEC_RECEIVEFAILED,                OF_error, "DIMSE Failed to receive message");
makeOFConditionConst(DIMSE_SENDFAILED,                   OFM_dcmnet, DIMSEC_SENDFAILED,                   OF_error, "DIMSE Failed to send message");
makeOFConditionConst(DIMSE_UNEXPECTEDPDVTYPE,            OFM_dcmnet, DIMSEC_UNEXPECTEDPDVTYPE,            OF_error, "DIMSE Unexpected PDV type");
makeOFConditionConst(DIMSE_NODATADICT,                   OFM_dcmnet, DIMSEC_NODATADICT,                   OF_error, "Data dictionary missing");

makeOFConditionConst(DUL_ASSOCIATIONREJECTED,      OFM_dcmnet, DULC_ASSOCIATIONREJECTED     , OF_error, "DUL Association Rejected");
makeOFConditionConst(DUL_ILLEGALACCEPT,            OFM_dcmnet, DULC_ILLEGALACCEPT           , OF_error, "Attempt to accept by requestor");
makeOFConditionConst(DUL_ILLEGALKEY,               OFM_dcmnet, DULC_ILLEGALKEY              , OF_error, "Illegal key");
makeOFConditionConst(DUL_ILLEGALPDU,               OFM_dcmnet, DULC_ILLEGALPDU              , OF_error, "DUL Illegal or ill-formed PDU");
makeOFConditionConst(DUL_ILLEGALPDULENGTH,         OFM_dcmnet, DULC_ILLEGALPDULENGTH        , OF_error, "DUL Illegal PDU Length");
makeOFConditionConst(DUL_ILLEGALREQUEST,           OFM_dcmnet, DULC_ILLEGALREQUEST          , OF_error, "Attempt to request by acceptor");
makeOFConditionConst(DUL_INCORRECTBUFFERLENGTH,    OFM_dcmnet, DULC_INCORRECTBUFFERLENGTH   , OF_error, "DUL Incorrect buffer space allocated for data");
makeOFConditionConst(DUL_INSUFFICIENTBUFFERLENGTH, OFM_dcmnet, DULC_INSUFFICIENTBUFFERLENGTH, OF_error, "DUL Insufficient buffer space allocated for data");
makeOFConditionConst(DUL_LISTERROR,                OFM_dcmnet, DULC_LISTERROR               , OF_error, "DUL List error");
makeOFConditionConst(DUL_NETWORKCLOSED,            OFM_dcmnet, DULC_NETWORKCLOSED           , OF_error, "DUL network closed");
makeOFConditionConst(DUL_NOASSOCIATIONREQUEST,     OFM_dcmnet, DULC_NOASSOCIATIONREQUEST    , OF_error, "No requests for associations for this server");
makeOFConditionConst(DUL_NOPDVS,                   OFM_dcmnet, DULC_NOPDVS                  , OF_error, "DUL No PDVs available in current buffer");
makeOFConditionConst(DUL_NULLKEY,                  OFM_dcmnet, DULC_NULLKEY                 , OF_error, "NULL key passed to routine");
makeOFConditionConst(DUL_PCTRANSLATIONFAILURE,     OFM_dcmnet, DULC_PCTRANSLATIONFAILURE    , OF_error, "DUL Presentation Context translation failure");
makeOFConditionConst(DUL_PDATAPDUARRIVED,          OFM_dcmnet, DULC_PDATAPDUARRIVED         , OF_error, "DUL P-Data PDU arrived");
makeOFConditionConst(DUL_PEERABORTEDASSOCIATION,   OFM_dcmnet, DULC_PEERABORTEDASSOCIATION  , OF_error, "Peer aborted Association (or never connected)");
makeOFConditionConst(DUL_PEERREQUESTEDRELEASE,     OFM_dcmnet, DULC_PEERREQUESTEDRELEASE    , OF_error, "DUL Peer Requested Release");
makeOFConditionConst(DUL_READTIMEOUT,              OFM_dcmnet, DULC_READTIMEOUT             , OF_error, "DUL network read timeout");
makeOFConditionConst(DUL_REQUESTASSOCIATIONFAILED, OFM_dcmnet, DULC_REQUESTASSOCIATIONFAILED, OF_error, "Failed to establish association");
makeOFConditionConst(DUL_UNEXPECTEDPDU,            OFM_dcmnet, DULC_UNEXPECTEDPDU           , OF_error, "Received unexpected PDU");
makeOFConditionConst(DUL_UNSUPPORTEDPEERPROTOCOL,  OFM_dcmnet, DULC_UNSUPPORTEDPEERPROTOCOL , OF_error, "DUL Unsupported peer protocol");
makeOFConditionConst(DUL_WRONGDATATYPE,            OFM_dcmnet, DULC_WRONGDATATYPE           , OF_error, "DUL Wrong Data Type Specified for Request");

makeOFConditionConst(NET_EC_InvalidSOPClassUID,              OFM_dcmnet, 1000, OF_error, "Invalid SOP Class UID");
makeOFConditionConst(NET_EC_UnknownStorageSOPClass,          OFM_dcmnet, 1001, OF_error, "Unknown Storage SOP Class");
makeOFConditionConst(NET_EC_InvalidSOPInstanceUID,           OFM_dcmnet, 1002, OF_error, "Invalid SOP Instance UID");
makeOFConditionConst(NET_EC_InvalidTransferSyntaxUID,        OFM_dcmnet, 1003, OF_error, "Invalid Transfer Syntax UID");
makeOFConditionConst(NET_EC_UnknownTransferSyntax,           OFM_dcmnet, 1004, OF_error, "Unknown Transfer Syntax");
makeOFConditionConst(NET_EC_NoPresentationContextsDefined,   OFM_dcmnet, 1005, OF_error, "No Presentation Contexts defined");
makeOFConditionConst(NET_EC_NoAcceptablePresentationContexts,OFM_dcmnet, 1006, OF_error, "No acceptable Presentation Contexts");
makeOFConditionConst(NET_EC_NoSOPInstancesToSend,            OFM_dcmnet, 1007, OF_error, "No SOP instances to send");
makeOFConditionConst(NET_EC_NoSuchSOPInstance,               OFM_dcmnet, 1008, OF_error, "No such SOP instance");
makeOFConditionConst(NET_EC_InvalidDatasetPointer,           OFM_dcmnet, 1009, OF_error, "Invalid dataset pointer");
makeOFConditionConst(NET_EC_AlreadyConnected,                OFM_dcmnet, 1010, OF_error, "Already connected");
makeOFConditionConst(NET_EC_InsufficientPortPrivileges,      OFM_dcmnet, 1023, OF_error, "Insufficient port privileges");


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
  OFCondition cond;
  char buf[16];
  sprintf(buf,"%04x:%04x ", subCondition.module(), subCondition.code());
  os << aText << OFendl << buf << subCondition.text() << OFStringStream_ends;
  OFSTRINGSTREAM_GETSTR(os, c)
  cond = makeOFCondition(aModule, aCode, aStatus, c);
  OFSTRINGSTREAM_FREESTR(c)
  return cond;
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
 * Revision 1.27  2012-02-15 14:50:42  uli
 * Removed dependency on static initialization order from OFCondition.
 * All static condition objects are now created via makeOFConditionConst()
 * in a way that doesn't need a constructor to run. This should only break
 * code which defines its own condition objects, all other changes are
 * backwards compatible.
 *
 * Revision 1.26  2011-10-10 14:01:29  uli
 * Moved SCU-specific error condition to the correct place.
 *
 * Revision 1.25  2011-10-06 14:16:10  joergr
 * Now also SOP instances from DICOM datasets can be added to the transfer list.
 * This allows for sending datasets created or received in memory.
 *
 * Revision 1.24  2011-09-29 13:12:01  joergr
 * Introduced new network-related error codes, e.g. in case that none of the
 * proposed presentation contexts were accepted by the association acceptor.
 *
 * Revision 1.23  2011-09-28 13:29:02  joergr
 * Introduced a couple of new network-related error codes (aka OFCondition).
 *
 * Revision 1.22  2011-08-03 13:31:46  joergr
 * Added macro that allows for disabling the port permission check in SCPs.
 *
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
