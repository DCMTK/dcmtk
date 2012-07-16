/*
 *
 *  Copyright (C) 1993-2011, OFFIS e.V.
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
 *  Purpose: class DcmQueryRetrieveDatabaseStatus
 *
 */

#ifndef DCMQRDBS_H
#define DCMQRDBS_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/dcmqrdb/qrdefine.h"

class DcmDataset;

/** this class describes the result of a database operation (for an
 *  incoming C-FIND, C-MOVE, C-GET or C-STORE request) in a format
 *  that can directly be communicated to the remote SCU in a DIMSE RSP message.
 *  An object of this class comprises a status field along with an
 *  optional dataset containing status detail information.
 */
class DCMTK_DCMQRDB_EXPORT DcmQueryRetrieveDatabaseStatus
{
public:
  /** default constructor
   *  @param s initial DIMSE status, default is 0 (success).
   */
  DcmQueryRetrieveDatabaseStatus(Uint16 s = 0);

  /// copy constructor
  DcmQueryRetrieveDatabaseStatus(const DcmQueryRetrieveDatabaseStatus& org);

  /// destructor
  ~DcmQueryRetrieveDatabaseStatus();

  /// copy assignment operator
  DcmQueryRetrieveDatabaseStatus& operator=(const DcmQueryRetrieveDatabaseStatus& org);

  /// delete status detail, if any
  void deleteStatusDetail();

  /** return pointer to status detail object, then set status detail to NULL
   *  @return status detail, may be NULL
   */
  DcmDataset *extractStatusDetail();

  /// return status value
  Uint16 status() const { return status_; }

  /** set new status value
   *  @param s new status
   */
  void setStatus(Uint16 s) { status_ = s; }

private:
  /// the status flag as defined for the various DIMSE services
  Uint16 status_;

  /// an optional DICOM dataset with status detail, may be NULL.
  DcmDataset *statusDetail_;
};

#endif
