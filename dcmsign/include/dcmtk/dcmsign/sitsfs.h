/*
 *
 *  Copyright (C) 1998-2019, OFFIS e.V.
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
 *  Module: dcmsign
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiTimeStampFS
 *
 */

#ifndef SITSFS_H
#define SITSFS_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sitstamp.h" /* for class SiTimeStamp */
#include "dcmtk/ofstd/ofstring.h"   /* for class OFString */

/** timestamp client based on filesystem interactions.
 *  This timestamp client will write a timestamp query in DES encoding to file.
 *  The interaction with the time stamp authority must then take place separately
 *  (e.g. manually, using a command line tool like curl). This client then offers
 *  functionality to import a timestamp reponse (i.e. a timestamp) from file
 *  and place it into the signed DICOM file for which the timestamp was requested.
 *  @remark this class is only available if DCMTK is compiled with OpenSSL support enabled.
 */
class DCMTK_DCMSIGN_EXPORT SiTimeStampFS : public SiTimeStamp
{
public:

  /// default constructor
  SiTimeStampFS();

  /// destructor
  virtual ~SiTimeStampFS();

  /** takes a block of raw data, creates a time stamp query for a hash of this
   *  raw data, and stores it locally in this object.
   *  @param inputData pointer to raw data
   *  @param inputDataSize length of raw data block in bytes
   *  @return status code
   */
  virtual OFCondition stamp(
    const unsigned char *inputData,
    unsigned long inputDataSize);

  /** writes the time stamp query created with SiTimeStampFS::stamp() to a file.
   *  Also writes a "UID file" that contains the Digital Signature UID of the signature
   *  to which the timestamp request belongs. This file will be read later by the
   *  import function. The filenames of timestamp query file and UID file must be
   *  set prior to calling this method.
   *  @param item item of the DigitalSignatureSQ to which the timestamp is written
   *  @return dcmdata OFCondition status code
   */
  virtual OFCondition write(DcmItem& item);

  /** set the time stamp query filename to be written
   *  @param fname filename
   */
  virtual void setTSQFilename(const char *fname);

  /** set the time stamp response filename to be read
   *  @param fname filename
   */
  virtual void setTSRFilename(const char *fname);

  /** set the uid file filename to be written
   *  @param fname filename
   */
  virtual void setUIDFilename(const char *fname);

  /** load UID file and retrieve digital signature UID
   *  @param uid digital signature returned in this parameter if successful
   * @return EC_Normal if successful, an error code otherwise.
   */
  virtual OFCondition getUIDFromFile(OFString& uid);

  /** load timestamp query from file
   *  @return status code
   */
  virtual OFCondition load_ts_query_from_file();

  /** load timestamp response from file
   *  @return status code
   */
  virtual OFCondition load_ts_response_from_file();

  /** check consistency between timestamp query (if available), timestamp
   *  response and DICOM digital signature. Query and response files must have
   *  been loaded prior to this method call.
   *  @param ditem item of the DigitalSignaturesSequence to which this timestamp belongs
   *  @return status code
   */
  virtual OFCondition check_ts_response(DcmItem& ditem);

  /** insert timestamp token into DICOM dataset
   *  The response file must have been loaded and checked prior to this method call.
   *  @param ditem item of the DigitalSignaturesSequence to which this timestamp is written
   *  @return status code
   */
  virtual OFCondition write_ts_token(DcmItem& ditem);

private:

  /// filename of the timestamp query file to be written
  OFString tsqFilename_;

  /// filename of the timestamp response file to be read
  OFString tsrFilename_;

  /// filename of the UID file to be written
  OFString uidFilename_;

};

#endif
#endif
