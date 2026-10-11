/*
 *
 *  Module:  dcmdata
 *
 *  Purpose: DcmInputFileHandleStream and related classes.
 *    Same purpose as DcmInputFileStream/DcmInputFileStreamFactory (see
 *    dcistrmf.h), but instead of opening (and closing) the underlying
 *    file anew for every deferred element load, all streams/factories
 *    created for the same original file share one already-open file
 *    handle (DcmFileHandle). This avoids repeated fopen()/fclose()
 *    calls when a dataset contains many elements whose value loading is
 *    deferred (maxReadLength) and later triggered one element at a time,
 *    e.g. via getXXX() accessors or during write() of an unloaded value.
 *
 *  eRAD customization, not part of upstream DCMTK.
 */

#ifndef DCISTRMFC_H
#define DCISTRMFC_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcistrma.h"
#include "dcmtk/dcmdata/dcobject.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/ofstd/offile.h"
#include "dcmtk/ofstd/ofstd.h"
#ifdef WITH_THREADS
# include "dcmtk/ofstd/ofthread.h"
#endif


/// Controls locking a DcmFileHandle in a threaded environment
enum EDFH_ThreadMode
{
  /// DcmFileHandle is used on a single thread, no lock needed
  EDFH_Single,
  /// DcmFileHandle may be used on multiple threads, but there is no concurrency during the initial load, so locking is only needed for subsequent access.
  EDFH_Lazy,
  /// DcmFileHandle may be used on multiple threads
  EDFH_Multi
};

class DcmFileFormat;
class DcmFileHandleLoader;

/** Reference-counted holder of a single open file handle.
 *  Every DcmFileHandleProducer created for the same input file shares
 *  the same instance, so the file is opened once and closed once, no
 *  matter how many deferred element loads read from it in between.
 *  Instances are always heap-allocated and managed exclusively through
 *  addRef()/release().
 */
class DCMTK_DCMDATA_EXPORT DcmFileHandle {
public:
  /** opens the given file and returns a new handle with a reference
   *  count of 1, or NULL if the file could not be opened.
   *  @param filename name of the file to open
   */
  static DcmFileHandle *open(const OFFilename &filename, EDFH_ThreadMode threadMode = EDFH_Single);

  /// increases the reference count by one
  void addRef();

  /** decreases the reference count by one; once it reaches zero the
   *  file is closed and the object deletes itself.
   */
  void release();

  /** reads buflen bytes starting at the given absolute file offset.
   *  @param offset absolute position in file to read from
   *  @param buf destination buffer, must not be NULL
   *  @param buflen number of bytes to read
   *  @return number of bytes actually read
   */
  offile_off_t readAt(offile_off_t offset, void *buf, offile_off_t buflen);

  /// returns the total size of the file, in bytes
  offile_off_t size() const
  {
    return size_;
  }

  /// returns the status of the handle, EC_Normal if the file was opened successfully
  OFCondition status() const
  {
    return status_;
  }

#ifdef WITH_THREADS
  /** Prepares the handle for the initial load.
   *  In Single or Lazy mode, no other thread is expected to
   *  read from the file by a concurrent thread, no lock is needed.
   */
  void initLoad()
  {
    // avoid accessing lock_ in full multi-threaded mode
    // - lock_ is already set
    // - avoid concurrent access
    if (threadMode_ != EDFH_Multi)
    {
      lock_ = false;
    }
  }

  /** Prepares the handle for lazy-loading large values.
   *  The initial load is expected to complete.
   *  Enables locking for Multi or Lazy modes, concurrent threads may
   *  read from the file concurrently after the initial load
   */
  void endLoad()
  {
    // avoid accessing lock_ in full multi-threaded mode
    // - lock_ is already set
    // - avoid concurrent access
    if (threadMode_ != EDFH_Multi)
    {
      lock_ = threadMode_ == EDFH_Lazy;
    }
  }
#endif

private:
  DcmFileHandle(const OFFilename &filename, EDFH_ThreadMode threadMode);
  ~DcmFileHandle();

  /// private unimplemented copy constructor
  DcmFileHandle(const DcmFileHandle&);
  /// private unimplemented copy assignment operator
  DcmFileHandle& operator=(const DcmFileHandle&);

  /// the single file handle shared by all producers referencing this object
  OFFile file_;

  /// status after opening the file
  OFCondition status_;

  /// number of bytes in the file
  offile_off_t size_;

  /// reference counter
  long refCount_;

  /// Threading - locking mode
  EDFH_ThreadMode threadMode_;

#ifdef WITH_THREADS
  /// protects file_ (seek+read is not atomic) and refCount_
  OFMutex mutex_;

  /// controls whether locking is needed
  bool lock_;
#endif
};

/** producer that reads from a DcmFileHandle shared with other
 *  producers/streams instead of opening its own file handle.
 *  Keeps track of its own logical read position and seeks the shared
 *  handle to that position before every operation, so several producers
 *  can safely take turns reading from the same open file.
 */
class DCMTK_DCMDATA_EXPORT DcmFileHandleProducer : public DcmProducer {
public:
  /** constructor
   *  @param handle shared file handle, reference count is increased by this call
   *  @param offset absolute byte offset in file where this producer starts reading
   */
  DcmFileHandleProducer(DcmFileHandle *handle, offile_off_t offset);

  /// destructor, releases the shared file handle
  virtual ~DcmFileHandleProducer();

  virtual OFBool good() const;
  virtual OFCondition status() const;
  virtual OFBool eos();
  virtual offile_off_t avail();
  virtual offile_off_t read(void *buf, offile_off_t buflen);
  virtual offile_off_t skip(offile_off_t skiplen);
  virtual void putback(offile_off_t num);

  /// returns the current logical (absolute) file position of this producer
  offile_off_t tell() const
  {
    return pos_;
  }

private:
  /// private unimplemented copy constructor
  DcmFileHandleProducer(const DcmFileHandleProducer&);
  /// private unimplemented copy assignment operator
  DcmFileHandleProducer& operator=(const DcmFileHandleProducer&);

  /// shared file handle, not owned exclusively - reference counted
  DcmFileHandle *handle_;

  /// this producer's own read position (absolute offset into the file)
  offile_off_t pos_;

  /// local error status, e.g. after a failed putback
  OFCondition status_;
};

/** input stream that reads from a DcmFileHandle. Used both for the
 *  initial parse of a file (see DcmFileHandle::open()) and for
 *  streams created later by DcmInputFileHandleStreamFactory::create()
 *  to load deferred element values - all sharing the same open file.
 */
class DCMTK_DCMDATA_EXPORT DcmInputFileHandleStream : public DcmInputStream {
public:
  /** constructor
   *  @param handle shared file handle, reference count is increased by this call
   *  @param offset absolute byte offset in file where reading starts
   */
  DcmInputFileHandleStream(DcmFileHandle *handle, offile_off_t offset);

  /// destructor
  virtual ~DcmInputFileHandleStream();

  /** creates a factory that, once activated, creates further
   *  DcmInputFileHandleStream instances sharing the same open file handle.
   *  @return pointer to new factory object, never NULL for this class
   */
  virtual DcmInputStreamFactory *newFactory() const;

private:
  /// private unimplemented copy constructor
  DcmInputFileHandleStream(const DcmInputFileHandleStream&);
  /// private unimplemented copy assignment operator
  DcmInputFileHandleStream& operator=(const DcmInputFileHandleStream&);

  /// the final producer of the filter chain
  DcmFileHandleProducer producer_;

  /// shared file handle (kept only to pass on to factories created by newFactory())
  DcmFileHandle *handle_;
};

/** input stream factory that creates DcmInputFileHandleStream instances,
 *  all sharing the same already-open DcmFileHandle instead of
 *  reopening the underlying file for every deferred element value.
 */
class DCMTK_DCMDATA_EXPORT DcmInputFileHandleStreamFactory : public DcmInputStreamFactory {
public:
  /** constructor
   *  @param handle shared file handle, reference count is increased by this call
   *  @param offset absolute byte offset in file where the created streams start reading
   */
  DcmInputFileHandleStreamFactory(DcmFileHandle *handle, offile_off_t offset);

  /// copy constructor, increases the reference count of the shared file handle
  DcmInputFileHandleStreamFactory(const DcmInputFileHandleStreamFactory &arg);

  /// destructor, releases the shared file handle
  virtual ~DcmInputFileHandleStreamFactory();

  virtual DcmInputStream *create() const;

  virtual DcmInputStreamFactory *clone() const
  {
    return new DcmInputFileHandleStreamFactory(*this);
  }

  virtual DcmInputStreamFactoryType ident() const
  {
    return DFT_DcmInputFileHandleStreamFactory;
  }

private:
  /// private unimplemented copy assignment operator
  DcmInputFileHandleStreamFactory& operator=(const DcmInputFileHandleStreamFactory&);

  /// shared file handle
  DcmFileHandle *handle_;

  /// offset in file
  offile_off_t offset_;
};


/** Convenience class to load the content of a DICOM object to `DcmFileFormat`
 *  using a DcmInputFileHandleStream
 */
class DCMTK_DCMDATA_EXPORT DcmFileHandleLoader
{
public:
  /** constructor
   *  @param filename name of the DICOM file to be loaded
   *  @param threadMode thread safety mode for file handle operations
   */
  DcmFileHandleLoader(const OFFilename &filename,EDFH_ThreadMode threadMode);
  ~DcmFileHandleLoader();

  /** Loads the DICOM object from the file.
   *  This method supports DICOM objects stored as a file (with meta header) or as a
   *  dataset (without meta header).  By default, the presence of a meta header is
   *  detected automatically.
   *  @param dcmff reads the content of the object to this `DcmFileFormat` object.
   *  @param xfer transfer syntax to use when parsing
   *  @param glenc handling of group length parameters
   *  @param maxReadLength attribute values larger than this value are skipped
   *    while parsing and read later upon first access if the stream type supports
   *    this.
   *  @param stopParsingAtElement parsing of the input stream is stopped when
   *                       this tag key or any higher tag is encountered.
   *  @return EC_Normal if successful, an error code otherwise
   */
	OFCondition load(DcmFileFormat &dcmff,
                   const E_TransferSyntax xfer = EXS_Unknown,
                   const E_GrpLenEncoding glenc = EGL_noChange,
                   const Uint32 maxReadLength = DCM_MaxReadLength,
                   const DcmTagKey &stopParsingAtElement = DCM_UndefinedTagKey);

private:
  /// private unimplemented copy constructor
  DcmFileHandleLoader(const DcmFileHandleLoader&);
  /// private unimplemented copy assignment operator
  DcmFileHandleLoader& operator=(const DcmFileHandleLoader&);

  DcmFileHandle* handle_;
};

#endif
