/*
 *
 *  Copyright (C) 2002-2026, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Contributed by Tamas Nemeth (eRAD) as GitHub PR #159.
 *
 *  Purpose: DcmInputFileHandleStream and related classes.
 *    These classes save the same purpose as DcmInputFileStream/
 *    DcmInputFileStreamFactory, but instead of opening (and closing)
 *    the underlying file anew for every deferred element load,
 *    all streams/factories created for the same original file share
 *    one already-open file handle (DcmFileHandle). This avoids
 *    repeated fopen()/fclose() calls when a dataset contains many
 *    elements whose value loading is deferred (maxReadLength)
 *    and later triggered one element at a time, e.g. via getXXX()
 *    accessors or during write() of an unloaded value.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcistrmh.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcerror.h"


DcmFileHandle::DcmFileHandle(const OFFilename &filename, EDFH_ThreadMode threadMode)
: file_()
, status_(EC_Normal)
, size_(0)
, refCount_(1)
, threadMode_(threadMode)
#ifdef WITH_THREADS
, mutex_()
, lock_(true)
#endif
{
  if (file_.fopen(filename, "rb"))
  {
    file_.fseek(0L, SEEK_END);
    size_ = file_.ftell();
    file_.fseek(0L, SEEK_SET);
  }
  else
  {
    OFString s("(unknown error code)");
    file_.getLastErrorString(s);
    status_ = makeOFCondition(OFM_dcmdata, 18, OF_error, s.c_str());
  }
}

DcmFileHandle::~DcmFileHandle()
{
}

DcmFileHandle *DcmFileHandle::open(const OFFilename &filename, EDFH_ThreadMode threadMode)
{
  DcmFileHandle *handle = new DcmFileHandle(filename, threadMode);
  if (handle->status_.bad())
  {
    delete handle;
    return NULL;
  }
  return handle;
}

void DcmFileHandle::addRef()
{
#ifdef WITH_THREADS
	if (lock_)
		mutex_.lock();
#endif
  ++refCount_;
#ifdef WITH_THREADS
  if (lock_)
	  mutex_.unlock();
#endif
}

void DcmFileHandle::release()
{
  OFBool deleteSelf = OFFalse;
#ifdef WITH_THREADS
  if (lock_)
	  mutex_.lock();
#endif
  deleteSelf = (--refCount_ <= 0);
#ifdef WITH_THREADS
  if (lock_)
	  mutex_.unlock();
#endif
  if (deleteSelf)
    delete this;
}

offile_off_t DcmFileHandle::readAt(offile_off_t offset, void *buf, offile_off_t buflen)
{
  offile_off_t result = 0;
#ifdef WITH_THREADS
  if (lock_)
	  mutex_.lock();
#endif
  if (status_.good() && file_.open() && buf && buflen)
  {
    // reposition the shared handle before reading - several producers
    // take turns using the same open file, each with its own position.
    if (0 == file_.fseek(offset, SEEK_SET))
      result = OFstatic_cast(offile_off_t, file_.fread(buf, 1, OFstatic_cast(size_t, buflen)));
  }
#ifdef WITH_THREADS
  if (lock_)
	  mutex_.unlock();
#endif
  return result;
}


/* ======================================================================= */


DcmFileHandleProducer::DcmFileHandleProducer(DcmFileHandle *handle, offile_off_t offset)
: DcmProducer()
, handle_(handle)
, pos_(offset)
, status_(EC_Normal)
{
  if (handle_)
  {
    handle_->addRef();
    status_ = handle_->status();
  }
  else
    status_ = EC_IllegalCall;
}

DcmFileHandleProducer::~DcmFileHandleProducer()
{
  if (handle_)
    handle_->release();
}

OFBool DcmFileHandleProducer::good() const
{
  return status_.good();
}

OFCondition DcmFileHandleProducer::status() const
{
  return status_;
}

OFBool DcmFileHandleProducer::eos()
{
  if (!handle_)
    return OFTrue;
  return (pos_ >= handle_->size());
}

offile_off_t DcmFileHandleProducer::avail()
{
  if (!handle_)
    return 0;
  offile_off_t remaining = handle_->size() - pos_;
  return (remaining > 0) ? remaining : 0;
}

offile_off_t DcmFileHandleProducer::read(void *buf, offile_off_t buflen)
{
  offile_off_t result = 0;
  if (status_.good() && handle_ && buf && buflen)
  {
    result = handle_->readAt(pos_, buf, buflen);
    pos_ += result;
  }
  return result;
}

offile_off_t DcmFileHandleProducer::skip(offile_off_t skiplen)
{
  offile_off_t result = 0;
  if (status_.good() && handle_ && skiplen)
  {
    offile_off_t remaining = handle_->size() - pos_;
    result = (remaining < skiplen) ? remaining : skiplen;
    if (result < 0)
      result = 0;
    pos_ += result;
  }
  return result;
}

void DcmFileHandleProducer::putback(offile_off_t num)
{
  if (status_.good() && handle_ && num)
  {
    if (num <= pos_)
      pos_ -= num;
    else
      status_ = EC_PutbackFailed;  // tried to putback before start of file
  }
}


/* ======================================================================= */


DcmInputFileHandleStream::DcmInputFileHandleStream(DcmFileHandle *handle, offile_off_t offset)
: DcmInputStream(&producer_)  // safe because DcmInputStream only stores pointer
, producer_(handle, offset)
, handle_(handle)
{
  if (handle_)
    handle_->addRef();
}

DcmInputFileHandleStream::~DcmInputFileHandleStream()
{
  if (handle_)
    handle_->release();
}

DcmInputStreamFactory *DcmInputFileHandleStream::newFactory() const
{
  return new DcmInputFileHandleStreamFactory(handle_, tell());
}


/* ======================================================================= */


DcmInputFileHandleStreamFactory::DcmInputFileHandleStreamFactory(DcmFileHandle *handle, offile_off_t offset)
: DcmInputStreamFactory()
, handle_(handle)
, offset_(offset)
{
  if (handle_)
    handle_->addRef();
}

DcmInputFileHandleStreamFactory::DcmInputFileHandleStreamFactory(const DcmInputFileHandleStreamFactory &arg)
: DcmInputStreamFactory(arg)
, handle_(arg.handle_)
, offset_(arg.offset_)
{
  if (handle_)
    handle_->addRef();
}

DcmInputFileHandleStreamFactory::~DcmInputFileHandleStreamFactory()
{
  if (handle_)
    handle_->release();
}

DcmInputStream *DcmInputFileHandleStreamFactory::create() const
{
  return new DcmInputFileHandleStream(handle_, offset_);
}

