/*
 *
 *  Copyright (C) 1993-2026, OFFIS e.V.
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
 *  Module:  dcmqrdb
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmQueryRetrieveSCP
 *
 */

#ifndef DCMQRSRV_H
#define DCMQRSRV_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/dcasccfg.h"
#include "dcmtk/dcmqrdb/dcmqrptb.h"

class DcmQueryRetrieveConfig;
class DcmQueryRetrieveOptions;
class DcmQueryRetrieveDatabaseHandle;
class DcmQueryRetrieveDatabaseHandleFactory;
class DcmTLSOptions;

/// enumeration describing reasons for refusing an association request
enum CTN_RefuseReason
{
    /// too many concurrent associations
    CTN_TooManyAssociations,
    /// fork system function failed
    CTN_CannotFork,
    /// bad application context (not DICOM)
    CTN_BadAppContext,
    /// unknown peer application entity title (access not authorised)
    CTN_BadAEPeer,
    /// unknown peer application entity title (access not authorised)
    CTN_BadAEService,
    /// other non-specific reason
    CTN_NoReason
};

/** main class for Query/Retrieve Service Class Provider
 */
class DCMTK_DCMQRDB_EXPORT DcmQueryRetrieveSCP
{
public:

  /** constructor
   *  @param config SCP configuration facility
   *  @param options SCP configuration options
   *  @param factory factory object used to create database handles
   *  @param object managing the TLS options for network connections
   */
  DcmQueryRetrieveSCP(
    const DcmQueryRetrieveConfig& config,
    const DcmQueryRetrieveOptions& options,
    const DcmQueryRetrieveDatabaseHandleFactory& factory,
    const DcmAssociationConfiguration& associationConfiguration,
    DcmTLSOptions& tlsOptions);

  /// destructor
  virtual ~DcmQueryRetrieveSCP() { }

  /** wait for incoming A-ASSOCIATE requests, perform association negotiation
   *  and serve the requests. May fork child processes depending on availability
   *  of the fork() system function and configuration options.
   *  @param theNet network structure for listen socket
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition waitForAssociation(T_ASC_Network *theNet);

  /** set database flags
   *  @param dbCheckFindIdentifier flag indicating that a check should be performed for C-FIND requests
   *  @param dbCheckMoveIdentifier flag indicating that a check should be performed for C-MOVE requests
   */
  void setDatabaseFlags(
    OFBool dbCheckFindIdentifier,
    OFBool dbCheckMoveIdentifier);

  /** clean up terminated child processes.
   */
  void cleanChildren();

  /** check whether a DIMSE command is permitted for the given abstract syntax.
   *  DICOM PS3.7 requires that a DIMSE message is only sent on a presentation
   *  context whose abstract syntax supports the corresponding DIMSE service.
   *  The DIMSE layer itself does not enforce this, i.e. the Service Class
   *  Provider has to check the combination of command and abstract syntax
   *  before acting upon a message. Otherwise a peer could, for example, send
   *  a C-STORE request on a Verification presentation context and thus bypass
   *  the access control that is implemented by refusing storage presentation
   *  contexts during association negotiation.
   *  @param command DIMSE command field of the received message
   *  @param abstractSyntax abstract syntax (SOP Class UID) that was negotiated
   *    for the presentation context on which the message was received.
   *    May be NULL, in which case OFFalse is returned.
   *  @return OFTrue if the command may be processed on the given abstract
   *    syntax, OFFalse otherwise
   */
  static OFBool isCommandAllowedForAbstractSyntax(
    T_DIMSE_Command command,
    const char *abstractSyntax);

private:

  /// private undefined copy constructor
  DcmQueryRetrieveSCP(const DcmQueryRetrieveSCP& other);

  /// private undefined assignment operator
  DcmQueryRetrieveSCP& operator=(const DcmQueryRetrieveSCP& other);

  /** perform association negotiation for an incoming A-ASSOCIATE request based
   *  on the SCP configuration and option flags. No A-ASSOCIATE response is generated,
   *  this is left to the caller.
   *  @param assoc incoming association
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition negotiateAssociation(T_ASC_Association * assoc);

  OFCondition refuseAssociation(T_ASC_Association ** pAssoc, CTN_RefuseReason reason);

  OFCondition handleAssociation(
    T_ASC_Association ** assoc,
    OFBool correctUIDPadding);

  OFCondition echoSCP(
    T_ASC_Association * assoc,
    T_DIMSE_C_EchoRQ * req,
    T_ASC_PresentationContextID presId);

  OFCondition findSCP(
    T_ASC_Association * assoc,
    T_DIMSE_C_FindRQ * request,
    T_ASC_PresentationContextID presID,
    DcmQueryRetrieveDatabaseHandle& dbHandle);

  OFCondition getSCP(
    T_ASC_Association * assoc,
    T_DIMSE_C_GetRQ * request,
    T_ASC_PresentationContextID presID,
    DcmQueryRetrieveDatabaseHandle& dbHandle);

  OFCondition moveSCP(
    T_ASC_Association * assoc,
    T_DIMSE_C_MoveRQ * request,
    T_ASC_PresentationContextID presID,
    DcmQueryRetrieveDatabaseHandle& dbHandle);

  OFCondition storeSCP(
    T_ASC_Association * assoc,
    T_DIMSE_C_StoreRQ * req,
    T_ASC_PresentationContextID presId,
    DcmQueryRetrieveDatabaseHandle& dbHandle,
    OFBool correctUIDPadding);

  OFCondition dispatch(
    T_ASC_Association *assoc,
    OFBool correctUIDPadding);

  /** check whether the DIMSE command received on the given presentation
   *  context may be processed, i.e. whether it matches the abstract syntax
   *  that was negotiated for that presentation context.
   *  @param assoc association on which the message was received
   *  @param command DIMSE command field of the received message
   *  @param presID presentation context ID on which the message was received
   *  @return OFTrue if the command may be processed, OFFalse otherwise
   */
  static OFBool checkPresentationContextForCommand(
    T_ASC_Association *assoc,
    T_DIMSE_Command command,
    T_ASC_PresentationContextID presID);

  static void refuseAnyStorageContexts(T_ASC_Association *assoc);

  static void refuseAnyQueryRetrieveContexts(T_ASC_Association *assoc);

  /// configuration facility
  const DcmQueryRetrieveConfig *config_;

  /// child process table, only used in multi-processing mode
  DcmQueryRetrieveProcessTable processtable_;

  /// flag for database interface: check C-FIND identifier
  OFBool dbCheckFindIdentifier_;

  /// flag for database interface: check C-MOVE identifier
  OFBool dbCheckMoveIdentifier_;

  /// factory object used to create database handles
  const DcmQueryRetrieveDatabaseHandleFactory& factory_;

  /// SCP configuration options
  const DcmQueryRetrieveOptions& options_;

  /// Association configuration profiles read from configuration file
  const DcmAssociationConfiguration& associationConfiguration_;

  /// reference to object managing the TLS options
  DcmTLSOptions& tlsOptions_;
};

#endif
