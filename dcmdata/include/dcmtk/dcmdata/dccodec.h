/*
 *
 *  Copyright (C) 1997-2011, OFFIS e.V.
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
 *  Author:  Andreas Barth
 *
 *  Purpose: Interface of abstract class DcmCodec and the class DcmCodecStruct
 *
 */

#ifndef DCCODEC_H
#define DCCODEC_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/ofstd/oflist.h"

class DcmStack;
class DcmRepresentationParameter;
class DcmPixelSequence;
class DcmPolymorphOBOW;
class DcmItem;
class DcmTagKey;

/** abstract base class for a codec parameter object that
 *  describes the settings (modes of operations) for one
 *  particular codec (DcmCodec) object.
 */
class DCMTK_DCMDATA_EXPORT DcmCodecParameter
{
public:
    /// default constructor
    DcmCodecParameter() {}

    /// copy constructor
    DcmCodecParameter(const DcmCodecParameter&) {}

    /// destructor
    virtual ~DcmCodecParameter() {}

    /** this methods creates a copy of type DcmCodecParameter *
     *  it must be overweritten in every subclass.
     *  @return copy of this object
     */
    virtual DcmCodecParameter *clone() const = 0;

    /** returns the class name as string.
     *  can be used as poor man's RTTI replacement.
     */
    virtual const char *className() const = 0;

};


/** abstract base class for a codec object that can be registered
 *  in dcmdata and performs transfer syntax transformation (i.e.
 *  compressing, decompressing or transcoding between different
 *  compressed transfer syntaxes).
 *  When dcmdata is requested to write a transfer syntax that differs
 *  from the current one (i.e. the one in which the object was read),
 *  dcmdata dynamically searches for a DcmCodec object that is able
 *  to create the desired transfer syntax.  If no suitable codec
 *  is found, the write operation fails.
 */
class DCMTK_DCMDATA_EXPORT DcmCodec
{
public:
  /// default constructor
  DcmCodec() {}

  /// destructor
  virtual ~DcmCodec() {}

  /** decompresses the given pixel sequence and
   *  stores the result in the given uncompressedPixelData element.
   *  @param fromRepParam current representation parameter of compressed data, may be NULL
   *  @param pixSeq compressed pixel sequence
   *  @param uncompressedPixelData uncompressed pixel data stored in this element
   *  @param cp codec parameters for this codec
   *  @param objStack stack pointing to the location of the pixel data
   *    element in the current dataset.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  virtual OFCondition decode(
    const DcmRepresentationParameter * fromRepParam,
    DcmPixelSequence * pixSeq,
    DcmPolymorphOBOW& uncompressedPixelData,
    const DcmCodecParameter * cp,
    const DcmStack& objStack) const = 0;

  /** decompresses a single frame from the given pixel sequence and
   *  stores the result in the given buffer.
   *  @param fromParam representation parameter of current compressed
   *    representation, may be NULL.
   *  @param fromPixSeq compressed pixel sequence
   *  @param cp codec parameters for this codec
   *  @param dataset pointer to dataset in which pixel data element is contained
   *  @param frameNo number of frame, starting with 0 for the first frame
   *  @param startFragment index of the compressed fragment that contains
   *    all or the first part of the compressed bitstream for the given frameNo.
   *    Upon successful return this parameter is updated to contain the index
   *    of the first compressed fragment of the next frame.
   *    When unknown, zero should be passed. In this case the decompression
   *    algorithm will try to determine the index by itself, which will always
   *    work if frames are decompressed in increasing order from first to last,
   *    but may fail if frames are decompressed in random order, multiple fragments
   *    per frame and multiple frames are present in the dataset, and the offset
   *    table is empty.
   *  @param buffer pointer to buffer where frame is to be stored
   *  @param bufSize size of buffer in bytes
   *  @param decompressedColorModel upon successful return, the color model
   *    of the decompressed image (which may be different from the one used
   *    in the compressed images) is returned in this parameter.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  virtual OFCondition decodeFrame(
    const DcmRepresentationParameter * fromParam,
    DcmPixelSequence * fromPixSeq,
    const DcmCodecParameter * cp,
    DcmItem *dataset,
    Uint32 frameNo,
    Uint32& startFragment,
    void *buffer,
    Uint32 bufSize,
    OFString& decompressedColorModel) const = 0;

  /** compresses the given uncompressed DICOM image and stores
   *  the result in the given pixSeq element.
   *  @param pixelData pointer to the uncompressed image data in OW format
   *    and local byte order
   *  @param length of the pixel data field in bytes
   *  @param toRepParam representation parameter describing the desired
   *    compressed representation (e.g. JPEG quality)
   *  @param pixSeq compressed pixel sequence (pointer to new DcmPixelSequence object
   *    allocated on heap) returned in this parameter upon success.
   *  @param cp codec parameters for this codec
   *  @param objStack stack pointing to the location of the pixel data
   *    element in the current dataset.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  virtual OFCondition encode(
    const Uint16 * pixelData,
    const Uint32 length,
    const DcmRepresentationParameter * toRepParam,
    DcmPixelSequence * & pixSeq,
    const DcmCodecParameter *cp,
    DcmStack & objStack) const = 0;

  /** transcodes (re-compresses) the given compressed DICOM image and stores
   *  the result in the given toPixSeq element.
   *  @param fromRepType current transfer syntax of the compressed image
   *  @param fromRepParam current representation parameter of compressed data, may be NULL
   *  @param fromPixSeq compressed pixel sequence
   *  @param toRepParam representation parameter describing the desired
   *    new compressed representation (e.g. JPEG quality)
   *  @param toPixSeq compressed pixel sequence (pointer to new DcmPixelSequence object
   *    allocated on heap) returned in this parameter upon success.
   *  @param cp codec parameters for this codec
   *  @param objStack stack pointing to the location of the pixel data
   *    element in the current dataset.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  virtual OFCondition encode(
    const E_TransferSyntax fromRepType,
    const DcmRepresentationParameter * fromRepParam,
    DcmPixelSequence * fromPixSeq,
    const DcmRepresentationParameter * toRepParam,
    DcmPixelSequence * & toPixSeq,
    const DcmCodecParameter * cp,
    DcmStack & objStack) const = 0;

  /** checks if this codec is able to convert from the
   *  given current transfer syntax to the given new
   *  transfer syntax
   *  @param oldRepType current transfer syntax
   *  @param newRepType desired new transfer syntax
   *  @return true if transformation is supported by this codec, false otherwise.
   */
  virtual OFBool canChangeCoding(
    const E_TransferSyntax oldRepType,
    const E_TransferSyntax newRepType) const = 0;

  /** determine color model of the decompressed image
   *  @param fromParam representation parameter of current compressed
   *    representation, may be NULL
   *  @param fromPixSeq compressed pixel sequence
   *  @param cp codec parameters for this codec
   *  @param dataset pointer to dataset in which pixel data element is contained
   *  @param dataset pointer to DICOM dataset in which this pixel data object
   *    is located. Used to access photometric interpretation.
   *  @param decompressedColorModel upon successful return, the color model
   *    of the decompressed image (which may be different from the one used
   *    in the compressed images) is returned in this parameter
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition determineDecompressedColorModel(
    const DcmRepresentationParameter *fromParam,
    DcmPixelSequence *fromPixSeq,
    const DcmCodecParameter *cp,
    DcmItem *dataset,
    OFString &decompressedColorModel) const = 0;

  // static helper methods that have proven useful in codec classes derived from DcmCodec

  /** helper function that inserts a string attribute with a given value into a dataset
   *  if missing in the dataset.
   *  @param dataset dataset to insert to, must not be NULL.
   *  @param tag tag key of attribute to check/insert
   *  @param val string value, may be NULL.
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition insertStringIfMissing(DcmItem *dataset, const DcmTagKey& tag, const char *val);

  /** helper function that converts a dataset containing a DICOM image
   *  into a valid (standard extended) Secondary Capture object
   *  by inserting all attributes that are type 1/2 in Secondary Capture
   *  and missing in the source dataset.  Replaces SOP Class UID
   *  by Secondary Capture. It does not, however, change an existing SOP Instance UID.
   *  @param dataset dataset to insert to, must not be NULL.
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition convertToSecondaryCapture(DcmItem *dataset);

  /** create new SOP instance UID and Source Image Sequence
   *  referencing the old SOP instance (if present)
   *  @param dataset dataset to be modified
   *  @param purposeOfReferenceCodingScheme coding scheme designator for purpose of reference code sequence
   *  @param purposeOfReferenceCodeValue code value for purpose of reference code sequence
   *  @param purposeOfReferenceCodeMeaning code meaning for purpose of reference code sequence
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition newInstance(
    DcmItem *dataset,
    const char *purposeOfReferenceCodingScheme = NULL,
    const char *purposeOfReferenceCodeValue = NULL,
    const char *purposeOfReferenceCodeMeaning = NULL);

  /** set first value of Image Type to DERIVED.
   *  @param dataset dataset to be modified
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition updateImageType(DcmItem *dataset);

  /** insert code sequence into the given dataset
   *  @param dataset dataset to insert into
   *  @param tagKey tag of the code sequence
   *  @param codingSchemeDesignator coding scheme designator for the sequence item
   *  @param codeValue code value for the sequence item
   *  @param codeMeaning code meaning for the sequence item
   *  @return EC_Normal if successul, an error code otherwise
   */
  static OFCondition insertCodeSequence(
    DcmItem *dataset,
    const DcmTagKey &tagKey,
    const char *codingSchemeDesignator,
    const char *codeValue,
    const char *codeMeaning);

  /** determine the index number (starting with zero) of the compressed pixel data fragment
   *  corresponding to the given frame (also starting with zero)
   *  @param frameNo frame number
   *  @param numberOfFrames number of frames of this image
   *  @param fromPixSeq compressed pixel sequence
   *  @param currentItem index of compressed pixel data fragment returned in this parameter on success
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition determineStartFragment(
    Uint32 frameNo,
    Sint32 numberOfFrames,
    DcmPixelSequence * fromPixSeq,
    Uint32& currentItem);
};


/** singleton that keeps list of registered codecs for dcmdata.
 *  All operations on the list are protected by a read/write lock
 *  and, therefore, are safe for multi-thread applications.
 */
class DCMTK_DCMDATA_EXPORT DcmCodecList
{

public:

  /// destructor
  virtual ~DcmCodecList();

  /** registers a codec object in the global list of codecs consulted by dcmdata
   *  whenever conversion to/from compressed transfer syntaxes is requested.
   *  This function must not be called before main() is started, e.g. from
   *  a constructor of a global object.
   *  This call is safe in multi-thread operations.
   *  @param aCodec pointer to codec object.
   *    Must remain unmodified and valid until the codec has been deregistered.
   *  @param aDefaultRepParam default representation parameter.
   *    Must remain unmodified and valid until the codec has been deregistered.
   *  @param aCodecParameter codec parameter.
   *    Must remain unmodified and valid until the codec has been deregistered or the
   *    parameter has been replaced by a call to updateCodecParameter()
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition registerCodec(
    const DcmCodec *aCodec,
    const DcmRepresentationParameter *aDefaultRepParam,
    const DcmCodecParameter *aCodecParameter);

  /** deregisters a codec and it's parameter objects from the global list.
   *  This call is safe in multi-thread operations.
   *  @param aCodec pointer to registered codec
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition deregisterCodec(const DcmCodec *aCodec);

  /** updates the codec parameters object for a codec that has been registered before.
   *  This call is safe in multi-thread operations.
   *  @param aCodec pointer to codec object that has been registered before
   *  @param aCodecParameter codec parameter.
   *    Must remain unmodified and valid until the codec has been deregistered or the
   *    parameter has been replaced by another call to updateCodecParameter()
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition updateCodecParameter(
    const DcmCodec *aCodec,
    const DcmCodecParameter *aCodecParameter);

  /** looks for a codec that is able to decode from the given transfer syntax
   *  and calls the decode() method of the codec.  A read lock on the list of
   *  codecs is acquired until this method returns.
   *  @param fromType transfer syntax to decode from
   *  @param fromParam representation parameter of current compressed
   *    representation, may be NULL.
   *  @param fromPixSeq compressed pixel sequence
   *  @param uncompressedPixelData uncompressed pixel data stored in this element
   *  @param pixelStack stack pointing to the location of the pixel data
   *    element in the current dataset.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  static OFCondition decode(
    const DcmXfer & fromType,
    const DcmRepresentationParameter * fromParam,
    DcmPixelSequence * fromPixSeq,
    DcmPolymorphOBOW& uncompressedPixelData,
    DcmStack & pixelStack);

  /** looks for a codec that is able to decode from the given transfer syntax
   *  and calls the decodeFrame() method of the codec.  A read lock on the list of
   *  codecs is acquired until this method returns.
   *  @param fromType transfer syntax to decode from
   *  @param fromParam representation parameter of current compressed
   *    representation, may be NULL.
   *  @param fromPixSeq compressed pixel sequence
   *  @param dataset pointer to dataset in which pixel data element is contained
   *  @param frameNo number of frame, starting with 0 for the first frame
   *  @param startFragment index of the compressed fragment that contains
   *    all or the first part of the compressed bitstream for the given frameNo.
   *    Upon successful return this parameter is updated to contain the index
   *    of the first compressed fragment of the next frame.
   *    When unknown, zero should be passed. In this case the decompression
   *    algorithm will try to determine the index by itself, which will always
   *    work if frames are decompressed in increasing order from first to last,
   *    but may fail if frames are decompressed in random order, multiple fragments
   *    per frame and multiple frames are present in the dataset, and the offset
   *    table is empty.
   *  @param buffer pointer to buffer where frame is to be stored
   *  @param bufSize size of buffer in bytes
   *  @param decompressedColorModel upon successful return, the color model
   *    of the decompressed image (which may be different from the one used
   *    in the compressed images) is returned in this parameter.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  static OFCondition decodeFrame(
    const DcmXfer & fromType,
    const DcmRepresentationParameter * fromParam,
    DcmPixelSequence * fromPixSeq,
    DcmItem *dataset,
    Uint32 frameNo,
    Uint32& startFragment,
    void *buffer,
    Uint32 bufSize,
    OFString& decompressedColorModel);

  /** looks for a codec that is able to encode from the given transfer syntax
   *  and calls the encode() method of the codec.  A read lock on the list of
   *  codecs is acquired until this method returns.
   *  @param fromRepType transfer syntax to encode from
   *  @param pixelData pointer to the uncompressed image data in OW format
   *    and local byte order
   *  @param length of the pixel data field in bytes
   *  @param toRepType transfer syntax to compress to
   *  @param toRepParam representation parameter describing the desired
   *    compressed representation (e.g. JPEG quality)
   *  @param pixSeq compressed pixel sequence (pointer to new DcmPixelSequence object
   *    allocated on heap) returned in this parameter upon success.
   *  @param pixelStack stack pointing to the location of the pixel data
   *    element in the current dataset.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  static OFCondition encode(
    const E_TransferSyntax fromRepType,
    const Uint16 * pixelData,
    const Uint32 length,
    const E_TransferSyntax toRepType,
    const DcmRepresentationParameter * toRepParam,
    DcmPixelSequence * & pixSeq,
    DcmStack & pixelStack);

  /** looks for a codec that is able to transcode (re-compresses)
   *  from the given transfer syntax to the given transfer syntax
   *  and calls the encode() method of the codec.
   *  A read lock on the list of
   *  codecs is acquired until this method returns.
   *  @param fromRepType current transfer syntax of the compressed image
   *  @param fromParam current representation parameter of compressed data, may be NULL
   *  @param fromPixSeq compressed pixel sequence
   *  @param toRepType transfer syntax to compress to
   *  @param toRepParam representation parameter describing the desired
   *    new compressed representation (e.g. JPEG quality)
   *  @param toPixSeq compressed pixel sequence (pointer to new DcmPixelSequence object
   *    allocated on heap) returned in this parameter upon success.
   *  @param pixelStack stack pointing to the location of the pixel data
   *    element in the current dataset.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  static OFCondition encode(
    const E_TransferSyntax fromRepType,
    const DcmRepresentationParameter * fromParam,
    DcmPixelSequence * fromPixSeq,
    const E_TransferSyntax toRepType,
    const DcmRepresentationParameter * toRepParam,
    DcmPixelSequence * & toPixSeq,
    DcmStack & pixelStack);

  /** looks for a codec that claims to be able to convert
   *  between the given transfer syntaxes.
   *  A read lock on the list of
   *  codecs is acquired until this method returns.
   *  @param fromRepType current transfer syntax
   *  @param toRepType desired new transfer syntax
   *  @return true if transformation is supported by a codec, false otherwise.
   */
  static OFBool canChangeCoding(
    const E_TransferSyntax fromRepType,
    const E_TransferSyntax toRepType);

  /** determine color model of the decompressed image
   *  @param fromType transfer syntax to decode from
   *  @param fromParam representation parameter of current compressed
   *    representation, may be NULL
   *  @param fromPixSeq compressed pixel sequence
   *  @param dataset pointer to dataset in which pixel data element is contained
   *  @param decompressedColorModel upon successful return, the color model
   *    of the decompressed image (which may be different from the one used
   *    in the compressed images) is returned in this parameter
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition determineDecompressedColorModel(
    const DcmXfer &fromType,
    const DcmRepresentationParameter *fromParam,
    DcmPixelSequence *fromPixSeq,
    DcmItem *dataset,
    OFString &decompressedColorModel);

private:

  /** constructor
   *  @param aCodec pointer to codec object.
   *  @param aDefaultRepParam default representation parameter.
   *  @param aCodecParameter codec parameter.
   */
  DcmCodecList(
    const DcmCodec *aCodec,
    const DcmRepresentationParameter *aDefaultRepParam,
    const DcmCodecParameter *aCodecParameter);

  /// private undefined copy constructor
  DcmCodecList(const DcmCodecList &);

  /// private undefined copy assignment operator
  DcmCodecList &operator=(const DcmCodecList &);

  /// pointer to codec object
  const DcmCodec * codec;

  /// pointer to default representation parameter
  const DcmRepresentationParameter * defaultRepParam;

  /// pointer to codec parameter set
  const DcmCodecParameter * codecParameter;

  /// singleton list of registered codecs
  static OFList<DcmCodecList *> registeredCodecs;

#ifdef WITH_THREADS
  /// read/write lock guarding access to singleton list
  static OFReadWriteLock codecLock;
#endif

  // dummy friend declaration to prevent gcc from complaining
  // that this class only defines private constructors and has no friends.
  friend class DcmCodecListDummyFriend;
};


#endif
