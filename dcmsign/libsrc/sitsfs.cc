/*
 *
 *  Copyright (C) 2019-2020, OFFIS e.V.
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
 *    classes: SiTimeStamp
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sitsfs.h"
#include "dcmtk/dcmdata/dcerror.h"   /* for EC_IllegalCall */
#include "dcmtk/dcmdata/dcfilefo.h"  /* for class DcmFileFormat */
#include "dcmtk/dcmdata/dcdeftag.h"  /* for DCM_DigitalSignatureUID */

BEGIN_EXTERN_C
#include <openssl/ts.h>
#include <openssl/bio.h>
END_EXTERN_C



SiTimeStampFS::SiTimeStampFS()
: SiTimeStamp()
, tsqFilename_()
, tsrFilename_()
, uidFilename_()
{
}


SiTimeStampFS::~SiTimeStampFS()
{
}


void SiTimeStampFS::setTSQFilename(const char *fname)
{
  if (fname)
    tsqFilename_ = fname;
    else tsqFilename_ = "";
}


void SiTimeStampFS::setTSRFilename(const char *fname)
{
  if (fname)
    tsrFilename_ = fname;
    else tsrFilename_ = "";
}


void SiTimeStampFS::setUIDFilename(const char *fname)
{
  if (fname)
    uidFilename_ = fname;
    else uidFilename_ = "";
}


OFCondition SiTimeStampFS::stamp(
    const unsigned char *inputData,
    unsigned long inputDataSize)
{
  return create_ts_query(inputData, inputDataSize);
}


OFCondition SiTimeStampFS::write(DcmItem& item)
{

  TS_REQ *query = getTSQ();
  if (query == NULL)
  {
    DCMSIGN_ERROR("No timestamp query object available, cannot write to file.");
    return EC_IllegalCall;
  }

  OFCondition result = EC_Normal;

  // first we write the time stamp query file
  if (tsqFilename_.length() == 0)
  {
    DCMSIGN_WARN("Name of timestamp query file not set, will not write file.");
  }
  else
  {
    BIO *out_bio = BIO_new_file(tsqFilename_.c_str(), "wb");
    if (out_bio == NULL)
    {
      DCMSIGN_WARN("Unable to create time stamp query file '" << tsqFilename_ << "'");
      result = SI_EC_CannotWriteTSQ;
    }
    else
    {
      if (!i2d_TS_REQ_bio(out_bio, query))
      {
        DCMSIGN_WARN("Writing time stamp query file '" << tsqFilename_ << "' failed");
        result = SI_EC_CannotWriteTSQ;
      }
      BIO_free_all(out_bio);
    }
  }

  if (result.bad()) return result;

  // now we write the UID file
  if (uidFilename_.length() == 0)
  {
    DCMSIGN_WARN("Name of UID file for time stamp query not set, will not write file.");
  }
  else
  {
    OFString uid;
    DcmFileFormat dfile;
    if (item.findAndGetOFString(DCM_DigitalSignatureUID, uid).good())
    {
      result = dfile.getDataset()->putAndInsertString(DCM_DigitalSignatureUID, uid.c_str());
      if (result.good()) result = dfile.saveFile(uidFilename_, EXS_LittleEndianImplicit);
    }
    else
    {
      DCMSIGN_ERROR("Signature UID not found in dataset.");
      result = EC_IllegalCall;
    }
  }

  return result;
}


OFCondition SiTimeStampFS::getUIDFromFile(OFString& uid)
{
  if (uidFilename_.length() == 0)
  {
    DCMSIGN_ERROR("Cannot load UID file, filename not set");
    return EC_IllegalCall;
  }

  DcmFileFormat dfile;
  OFCondition result = dfile.loadFile(uidFilename_);
  if (result.good())
  {
    result = dfile.getDataset()->findAndGetOFString(DCM_DigitalSignatureUID, uid);
    if (result.bad()) DCMSIGN_ERROR("No Digital Signature UID found in  UID file '" << uidFilename_ << "'");
  }
  else
  {
    DCMSIGN_ERROR("Cannot load UID file '" << uidFilename_ << "'");
  }

  return result;
}


OFCondition SiTimeStampFS::load_ts_query_from_file()
{
  if (tsqFilename_.length() == 0)
  {
    DCMSIGN_ERROR("Cannot load timestamp query file, filename not set");
    return EC_IllegalCall;
  }

  return SiTimeStamp::load_ts_query(tsqFilename_.c_str());
}


OFCondition SiTimeStampFS::load_ts_response_from_file()
{
  if (tsrFilename_.length() == 0)
  {
    DCMSIGN_ERROR("Cannot load timestamp response file, filename not set");
    return EC_IllegalCall;
  }

  return SiTimeStamp::load_ts_response(tsrFilename_.c_str());
}


OFCondition SiTimeStampFS::check_ts_response(DcmItem& ditem)
{
  if (getTSQ() == NULL)
  {
    DCMSIGN_ERROR("Cannot check timestamp response, timestamp query not loaded");
    return EC_IllegalCall;
  }

  if (getTSR() == NULL)
  {
    DCMSIGN_ERROR("Cannot check timestamp response, not loaded");
    return EC_IllegalCall;
  }

  return  SiTimeStamp::check_ts_response(getTSQ(), getTSR(), ditem);

}


OFCondition SiTimeStampFS::write_ts_token(DcmItem& ditem)
{
  return SiTimeStamp::write_ts_token(getTSR(), ditem);
}



#else /* WITH_OPENSSL */

int sitsfs_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
