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
 *  Author:  Andrew Hewett
 *
 *  Purpose: class DcmQueryRetrieveOptions
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:41 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
extern const OFCondition APP_INVALIDPEER;

/** this class encapsulates all the various options that affect the
 *  operation of the SCP, in addition to those defined in the config file
 */
class DcmQueryRetrieveOptions
{
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

/*
 * CVS Log
 * $Log: dcmqropt.h,v $
 * Revision 1.8  2010-10-14 13:16:41  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.7  2009-11-24 10:10:42  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.6  2009-08-21 09:50:07  joergr
 * Replaced tabs by spaces and updated copyright date.
 *
 * Revision 1.5  2005/12/08 16:04:25  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.4  2005/11/29 11:27:18  meichel
 * Added new flag keepDBHandleDuringAssociation_ which allows to determine
 *   whether a DB handle is kept open for a complete association or a single
 *   DIMSE message only. Also improved error handling of file locking.
 *
 * Revision 1.3  2005/11/29 10:54:54  meichel
 * Added minimal support for compressed transfer syntaxes to dcmqrscp.
 *   No on-the-fly decompression is performed, but compressed images can
 *   be stored and retrieved.
 *
 * Revision 1.2  2005/11/17 13:44:37  meichel
 * Added command line options for DIMSE and ACSE timeouts
 *
 * Revision 1.1  2005/03/30 13:34:50  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */
