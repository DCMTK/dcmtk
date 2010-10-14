/*
 *
 *  Copyright (C) 1993-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:41 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMQRSRV_H
#define DCMQRSRV_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmqrdb/dcmqrptb.h"

class DcmQueryRetrieveConfig;
class DcmQueryRetrieveOptions;
class DcmQueryRetrieveDatabaseHandle;
class DcmQueryRetrieveDatabaseHandleFactory;

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
class DcmQueryRetrieveSCP
{
public:

  /** constructor
   *  @param config SCP configuration facility
   *  @param options SCP configuration options
   *  @param factory factory object used to create database handles
   */
  DcmQueryRetrieveSCP(
    const DcmQueryRetrieveConfig& config,
    const DcmQueryRetrieveOptions& options,
    const DcmQueryRetrieveDatabaseHandleFactory& factory);

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

private:

  /** perform association negotiation for an incoming A-ASSOCIATE request based
   *  on the SCP configuration and option flags. No A-ASSOCIATE response is generated,
   *  this is left to the caller.
   *  @param assoc incoming association
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition negotiateAssociation(T_ASC_Association * assoc);

  OFCondition refuseAssociation(T_ASC_Association ** assoc, CTN_RefuseReason reason);

  OFCondition handleAssociation(
    T_ASC_Association * assoc,
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

  static void refuseAnyStorageContexts(T_ASC_Association *assoc);

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
};

#endif

/*
 * CVS Log
 * $Log: dcmqrsrv.h,v $
 * Revision 1.4  2010-10-14 13:16:41  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.3  2009-11-24 10:10:42  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.2  2009-08-21 09:50:07  joergr
 * Replaced tabs by spaces and updated copyright date.
 *
 * Revision 1.1  2005/12/16 12:42:50  joergr
 * Renamed file to avoid naming conflicts when linking on SunOS 5.5.1 with
 * Sun CC 2.0.1.
 *
 * Revision 1.2  2005/12/08 16:04:27  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2005/03/30 13:34:50  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */
