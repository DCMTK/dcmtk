/*
 *
 *  Copyright (C) 2001-2014, OFFIS e.V.
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
 *  Module:  dcmjpeg
 *
 *  Author:  Marco Eichelberg, Norbert Olges
 *
 *  Purpose: abstract codec class for JPEG encoders.
 *
 */

#ifndef DJCODEC_H
#define DJCODEC_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/dcmdata/dccodec.h"    /* for class DcmCodec */
#include "dcmtk/dcmjpeg/djutils.h"    /* for enums */
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofstring.h"     /* for class OFString */

class DataInterface;
class DJEncoder;
class DcmDataset;
class DJCodecParameter;
class DJDecoder;
class DcmItem;
class DcmPixelItem;
class DicomImage;
class DcmTagKey;


/** abstract codec class for JPEG encoders.
 *  This abstract class contains most of the application logic
 *  needed for a dcmdata codec object that implements a JPEG encoder
 *  using the DJEncoder interface to the underlying JPEG implementation.
 *  This class only supports compression, it neither implements
 *  decoding nor transcoding.
 */
class DCMTK_DCMJPEG_EXPORT DJCodecEncoder : public DcmCodec
{
public:

  /// default constructor
  DJCodecEncoder();

  /// destructor
  virtual ~DJCodecEncoder();

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
    const DcmStack& objStack) const;

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
    OFString& decompressedColorModel) const;

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
    DcmStack & objStack) const;

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
    DcmStack & objStack) const;

  /** checks if this codec is able to convert from the
   *  given current transfer syntax to the given new
   *  transfer syntax
   *  @param oldRepType current transfer syntax
   *  @param newRepType desired new transfer syntax
   *  @return true if transformation is supported by this codec, false otherwise.
   */
  virtual OFBool canChangeCoding(
    const E_TransferSyntax oldRepType,
    const E_TransferSyntax newRepType) const;

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
    OFString &decompressedColorModel) const;

  /** returns the transfer syntax that this particular codec
   *  is able to encode and decode.
   *  @return supported transfer syntax
   */
  virtual E_TransferSyntax supportedTransferSyntax() const = 0;

protected:

  /** format compression ratio as string and append to given string object.
   *  @param arg string to append to
   *  @param ratio compression ratio
   */
  static void appendCompressionRatio(OFString& arg, double ratio);

    /** toggles Planar Configuration of 8-bit pixel data from "by pixel" to
   *  "by plane" and vice versa.
   *  @param pixelData - [in/out] Original pixel data (input), contains
   *                     pixel data with toggled Planar configuration after
   *                     returning (output).
   *  @param numValues - [in] The number of 8 bit values in pixelData
   *  @param samplesPerPixel - [in] Number of components for one pixel
   *  @param oldPlanarConfig - [in] The old Planar Configuration, that should
   *                           be toggled. 0 means "by pixel", 1 "by color"
   *  @return EC_Normal, if conversion was successful; error else
   */
  static OFCondition togglePlanarConfiguration8(
    Uint8 *pixelData,
    const size_t numValues,
    const Uint16 samplesPerPixel,
    const Uint16 oldPlanarConfig);

  /** toggles Planar Configuration of 16-bit pixel data from "by pixel" to
   *  "by plane" and vice versa.
   *  @param pixelData - [in/out] Original pixel data (input), contains
   *                     pixel data with toggled Planar configuration after
   *                     returning (output).
   *  @param numValues - [in] The number of 16 bit values in pixelData
   *  @param samplesPerPixel - [in] Number of components for one pixel
   *  @param oldPlanarConfig - [in] The old Planar Configuration, that should
   *                           be toggled. 0 means "by pixel", 1 "by color"
   *  @return EC_Normal, if conversion was successful; error else
   */
  static OFCondition togglePlanarConfiguration16(
    Uint16 *pixelData,
    const size_t numValues,
    const Uint16 samplesPerPixel,
    const Uint16 oldPlanarConfig);

private:

  /** compresses the given uncompressed DICOM color image and stores
   *  the result in the given pixSeq element.
   *  @param YBRmode true if the source image has YBR_FULL or YBR_FULL_422
   *    photometric interpretation and can thus be compressed without color
   *    space conversion.
   *  @param dataset DICOM dataset containing the pixel data as well as
   *    descriptive attributes
   *  @param toRepParam representation parameter describing the desired
   *    compressed representation (e.g. JPEG quality)
   *  @param pixSeq compressed pixel sequence (pointer to new DcmPixelSequence object
   *    allocated on heap) returned in this parameter upon success.
   *  @param cp codec parameters for this codec
   *  @param compressionRatio compression ratio returned in this parameter
   *    if successful.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  virtual OFCondition encodeColorImage(
    OFBool YBRmode,
    DcmItem *dataset,
    const DcmRepresentationParameter * toRepParam,
    DcmPixelSequence * & pixSeq,
    const DJCodecParameter *cp,
    double& compressionRatio) const;

  /** compresses the given uncompressed monochrome DICOM image and stores
   *  the result in the given pixSeq element.
   *  @param dataset DICOM dataset containing the pixel data as well as
   *    descriptive attributes
   *  @param toRepParam representation parameter describing the desired
   *    compressed representation (e.g. JPEG quality)
   *  @param pixSeq compressed pixel sequence (pointer to new DcmPixelSequence object
   *    allocated on heap) returned in this parameter upon success.
   *  @param cp codec parameters for this codec
   *  @param compressionRatio compression ratio returned in this parameter
   *    if successful.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  virtual OFCondition encodeMonochromeImage(
        DcmItem *dataset,
        const DcmRepresentationParameter * toRepParam,
        DcmPixelSequence * & pixSeq,
        const DJCodecParameter *cp,
        double& compressionRatio) const;

  /** compresses the given uncompressed DICOM image and stores
   *  the result in the given pixSeq element. No colorspace
   *  conversions, modality or windowing transformations are applied
   *  to guarantee, that the quality of the source image is fully preserved.
   *  @param toRepParam - [in] representation parameter describing the desired
   *  @param pixSeq - [out] compressed pixel sequence (pointer to new DcmPixelSequence
   *    object allocated on heap) returned in this parameter upon success
   *  @param cp - [in] codec parameters for this codec
   *  @param objStack - [in/out] stack pointing to the location of the pixel data
   *    element in the current dataset.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  virtual OFCondition encodeTrueLossless(
    const DcmRepresentationParameter * toRepParam,
    DcmPixelSequence * & pixSeq,
    const DcmCodecParameter *cp,
    DcmStack & objStack) const;

  /** create Lossy Image Compression and Lossy Image Compression Ratio.
   *  @param dataset dataset to be modified
   *  @param ratio image compression ratio > 1. This is not the "quality factor"
   *    but the real effective ratio between compressed and uncompressed image,
   *    i. e. 30 means a 30:1 lossy compression.
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition updateLossyCompressionRatio(DcmItem *dataset, double ratio) const;

  /** create Derivation Description.
   *  @param dataset dataset to be modified
   *  @param toRepParam representation parameter passed to encode()
   *  @param cp codec parameter passed to encode()
   *  @param bitsPerSample bits per sample of the original image data prior to compression
   *  @param ratio image compression ratio. This is not the "quality factor"
   *    but the real effective ratio between compressed and uncompressed image,
   *    i. e. 30 means a 30:1 lossy compression.
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition updateDerivationDescription(
    DcmItem *dataset,
    const DcmRepresentationParameter * toRepParam,
    const DJCodecParameter *cp,
    Uint8 bitsPerSample,
    double ratio) const;

  /** for all overlay groups create (60xx,3000) Overlay Data.
   *  @param dataset dataset to be modified
   *  @param image DicomImage object for this dataset
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition adjustOverlays(DcmItem *dataset, DicomImage& image) const;

  /** returns true if the transfer syntax supported by this
   *  codec is lossless.
   *  @return lossless flag
   */
  virtual OFBool isLosslessProcess() const = 0;

  /** creates 'derivation description' string after encoding.
   *  @param toRepParam representation parameter passed to encode()
   *  @param cp codec parameter passed to encode()
   *  @param bitsPerSample bits per sample of the original image data prior to compression
   *  @param ratio image compression ratio. This is not the "quality factor"
   *    but the real effective ratio between compressed and uncompressed image,
   *    i. e. 30 means a 30:1 lossy compression.
   *  @param derivation description returned in this
   *    parameter which is initially empty
   */
  virtual void createDerivationDescription(
    const DcmRepresentationParameter * toRepParam,
    const DJCodecParameter *cp,
    Uint8 bitsPerSample,
    double ratio,
    OFString& derivationDescription) const = 0;

  /** creates an instance of the compression library to be used
   *  for encoding/decoding.
   *  @param toRepParam representation parameter passed to encode()
   *  @param cp codec parameter passed to encode()
   *  @param bitsPerSample bits per sample for the image data
   *  @return pointer to newly allocated codec object
   */
  virtual DJEncoder *createEncoderInstance(
    const DcmRepresentationParameter * toRepParam,
    const DJCodecParameter *cp,
    Uint8 bitsPerSample) const = 0;

  /** modifies all VOI window center/width settings in the image.
   *  Modifications are based on the pixel value mapping
   *  f(x) = (x+voiOffset)*voiFactor
   *  @param dataset dataset to be updated
   *  @param voiOffset offset to be added to each pixel
   *  @param voiFactor factor to be multiplied
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition correctVOIWindows(DcmItem *dataset, double voiOffset, double voiFactor);

  OFCondition updatePlanarConfiguration(
    DcmItem *item,
    const Uint16 newPlanConf) const;
};

#endif
