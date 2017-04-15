/*
 *
 *  Copyright (C) 1994-2017, OFFIS e.V.
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

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/ofstd/ofstream.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

makeOFConditionConst(ASC_BADPRESENTATIONCONTEXTID,      OFM_dcmnet, ASCC_BADPRESENTATIONCONTEXTID,      OF_error, "ASC Bad presentation context ID");
makeOFConditionConst(ASC_MISSINGTRANSFERSYNTAX,         OFM_dcmnet, ASCC_MISSINGTRANSFERSYNTAX,         OF_error, "ASC Missing transfer syntax");
makeOFConditionConst(ASC_NULLKEY,                       OFM_dcmnet, ASCC_NULLKEY,                       OF_error, "ASC Caller passed in a NULL key");
makeOFConditionConst(ASC_SHUTDOWNAPPLICATION,           OFM_dcmnet, ASCC_SHUTDOWNAPPLICATION,           OF_error, "ASC Application shutdown requested");
makeOFConditionConst(ASC_USERIDENTIFICATIONFAILED,      OFM_dcmnet, ASCC_USERIDENTIFICATIONFAILED,      OF_error, "ASC User Identification Negotiation failed");
makeOFConditionConst(ASC_SCPSCUROLESELECTIONFAILED,     OFM_dcmnet, ASCC_SCPSCUROLESELECTIONFAILED,     OF_error, "ASC SCP/SCU role selection failed");
makeOFConditionConst(ASC_NOPRESENTATIONCONTEXTPROPOSED, OFM_dcmnet, ASCC_NOPRESENTATIONCONTEXTPROPOSED, OF_error, "No presentation context proposed");

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
makeOFConditionConst(DUL_CANNOTREADSOCKETHANDLE,   OFM_dcmnet, DULC_CANNOTREADSOCKETHANDLE  , OF_error, "DUL Cannot Read Socket Handle from Pipe");

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
// codes 1024 to 1073 are used for the association negotiation profile classes
makeOFConditionConst(NET_EC_SCPBusy,                         OFM_dcmnet, 1074, OF_error, "SCP is busy");
makeOFConditionConst(NET_EC_CannotStartSCPThread,            OFM_dcmnet, 1075, OF_error, "Cannot start SCP Thread");
makeOFConditionConst(NET_EC_StopAfterAssociation,            OFM_dcmnet, 1076, OF_ok, "Stop after current association (as requested)");
makeOFConditionConst(NET_EC_StopAfterConnectionTimeout,      OFM_dcmnet, 1077, OF_ok, "Stop after TCP connection timeout (as requested)");


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
