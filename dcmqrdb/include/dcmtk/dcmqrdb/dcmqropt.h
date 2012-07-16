/*
 *
 *  Copyright (C) 1993-2012, OFFIS e.V.
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
 *  Author:  Andrew Hewett
 *
 *  Purpose: class DcmQueryRetrieveOptions
 *
 */

#ifndef DCMQROPT_H
#define DCMQROPT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmqrdb/dcmqrcnf.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmnet/dimse.h"

/// invalid peer for move operation
extern DCMTK_DCMQRDB_EXPORT const OFConditionConst QR_EC_InvalidPeer;
extern DCMTK_DCMQRDB_EXPORT const OFConditionConst QR_EC_IndexDatabaseError;

/** this class encapsulates all the various options that affect the
 *  operation of the SCP, in addition to those defined in the config file
 */
class DCMTK_DCMQRDB_EXPORT DcmQueryRetrieveOptions
{
private:
  /// private undefined copy constructor
  DcmQueryRetrieveOptions(const DcmQueryRetrieveOptions& other);

  /// private undefined assignment operator
  DcmQueryRetrieveOptions& operator=(const DcmQueryRetrieveOptions& other);

public:
  /// default constructor
  DcmQueryRetrieveOptions();

  /// destructor
  ~DcmQueryRetrieveOptions();

  // these member variables should be private but are public for now

  /// enable negotiation of private shutdown SOP class
  OFBool            allowShutdown_;

  /// bit preserving mode for incoming storage requests.
  OFBool            bitPreserving_;

  /// silently correct space-padded UIDs
  OFBool            correctUIDPadding_;

  /// enable/disable C-GET support
  OFBool            disableGetSupport_;

  /// block size for file padding, pad DICOM files to multiple of this value
  OFCmdUnsignedInt  filepad_;

  /// group length encoding when writing DICOM files
  E_GrpLenEncoding  groupLength_;

  /// ignore incoming data, receive but do not store (for debugging)
  OFBool            ignoreStoreData_;

  /// block size for item padding, pad DICOM files to multiple of this value
  OFCmdUnsignedInt  itempad_;

  /// maximum number of parallel associations accepted
  int               maxAssociations_;

  /// maximum PDU size
  OFCmdUnsignedInt  maxPDU_;

  /// pointer to network structure used for requesting C-STORE sub-associations
  T_ASC_Network *   net_;

  /// preferred transfer syntax for incoming associations
  E_TransferSyntax  networkTransferSyntax_;

#ifndef DISABLE_COMPRESSION_EXTENSION
  /// preferred transfer syntax for outgoing associations
  E_TransferSyntax  networkTransferSyntaxOut_;
#endif

  /// padding algorithm for writing DICOM files
  E_PaddingEncoding paddingType_;

  /* refuse storage presentation contexts in incoming associations
   * if a storage presentation context for the application entity already exists
   */
  OFBool            refuseMultipleStorageAssociations_;

  /// refuse all incoming associations
  OFBool            refuse_;

  /// reject associations if implementatino class UID is missing
  OFBool            rejectWhenNoImplementationClassUID_;

  /// refuse MOVE context if no corresponding FIND context is present
  OFBool            requireFindForMove_;

  /// restrict MOVE operations to same Application Entity
  OFBool            restrictMoveToSameAE_;

  /// restrict MOVE operations to same host
  OFBool            restrictMoveToSameHost_;

  /// restrict MOVE operations to same vendor according to vendor table
  OFBool            restrictMoveToSameVendor_;

  /// sequence encoding when writing DICOM files
  E_EncodingType    sequenceType_;

  /// single process mode
  OFBool            singleProcess_;

  /// support for patient root q/r model
  OFBool            supportPatientRoot_;

  /// support for patient/study only q/r model
  OFBool            supportPatientStudyOnly_;

  /// support for study root q/r model
  OFBool            supportStudyRoot_;

  /// write DICOM files with DICOM metaheader
  OFBool            useMetaheader_;

  /** keep DB handle open for the complete lifetime of an association.
   *  If false, the DB handle is created and released for each incoming
   *  DIMSE message that is processed.
   */
  OFBool            keepDBHandleDuringAssociation_;

  /// transfer syntax for writing
  E_TransferSyntax  writeTransferSyntax_;

  /// blocking mode for DIMSE operations
  T_DIMSE_BlockingMode blockMode_;

  /// timeout for DIMSE operations
  int dimse_timeout_;

  /// timeout for ACSE operations
  int acse_timeout_;

};


#endif
