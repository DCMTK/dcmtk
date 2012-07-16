/*
 *
 *  Copyright (C) 2001-2011, OFFIS e.V.
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
 *  Purpose: abstract codec class for JPEG decoders.
 *
 */

#ifndef DJCODECD_H
#define DJCODECD_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/dcmdata/dccodec.h"    /* for class DcmCodec */
#include "dcmtk/dcmjpeg/djutils.h"    /* for enums */
#include "dcmtk/ofstd/ofstring.h"   /* for class OFString */


class DataInterface;
class DJEncoder;
class DcmDataset;
class DcmItem;
class DJCodecParameter;
class DJDecoder;

/** abstract codec class for JPEG decoders.
 *  This abstract class contains most of the application logic
 *  needed for a dcmdata codec object that implements a JPEG decoder
 *  using the DJDecoder interface to the underlying JPEG implementation.
 *  This class only supports decompression, it neither implements
 *  encoding nor transcoding.
 */
class DCMTK_DCMJPEG_EXPORT DJCodecDecoder : public DcmCodec
{
public:

  /// default constructor
  DJCodecDecoder();

  /// destructor
  virtual ~DJCodecDecoder();

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
   *  is able to decode.
   *  @return supported transfer syntax
   */
  virtual E_TransferSyntax supportedTransferSyntax() const = 0;

private:

  /** creates an instance of the compression library to be used for decoding.
   *  @param toRepParam representation parameter passed to decode()
   *  @param cp codec parameter passed to decode()
   *  @param bitsPerSample bits per sample for the image data
   *  @param isYBR flag indicating whether DICOM photometric interpretation is YCbCr
   *  @return pointer to newly allocated decoder object
   */
  virtual DJDecoder *createDecoderInstance(
    const DcmRepresentationParameter * toRepParam,
    const DJCodecParameter *cp,
    Uint8 bitsPerSample,
    OFBool isYBR) const = 0;

  // static private helper methods

  /** scans the given block of JPEG data for a Start of Frame marker
   *  and returns the number of bits per pixel stored in the
   *  Start of Frame block.
   *  @param data pointer to array, must not be NULL
   *  @param fragmentLength size of array
   *  @return bit depth of JPEG data, 0 if no SOF marker found or parse error
   */
  static Uint8 scanJpegDataForBitDepth(
    const Uint8 *data,
    const Uint32 fragmentLength);

  /** reads two bytes from the given array
   *  of little endian 16-bit values and returns
   *  the value as Uint16 in local byte order.
   *  @param data pointer to array, must not be NULL, must at least 2 bytes large
   *  @return Uint16 read from array
   */
  static Uint16 readUint16(const Uint8 *data);

  /** converts an RGB or YBR frame with 8 bits/sample from
   *  color-by-pixel to color-by-plane planar configuration.
   *  @param imageFrame pointer to image frame, must contain
   *    at least 3*columns*rows bytes of pixel data.
   *  @param columns columns
   *  @param rows rows
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition createPlanarConfigurationByte(
    Uint8 *imageFrame,
    Uint16 columns,
    Uint16 rows);

  /** converts an RGB or YBR frame with 16 bits/sample from
   *  color-by-pixel to color-by-plane planar configuration.
   *  @param imageFrame pointer to image frame, must contain
   *    at least 3*columns*rows words of pixel data.
   *  @param columns columns
   *  @param rows rows
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition createPlanarConfigurationWord(
    Uint16 *imageFrame,
    Uint16 columns,
    Uint16 rows);

  /** examines if a given image requires color-by-plane planar configuration
   *  depending on SOP Class UID (DICOM IOD) and photometric interpretation.
   *  All SOP classes defined in the 2003 edition of the DICOM standard or earlier
   *  are handled correctly.
   *  @param sopClassUID SOP Class UID
   *  @param photometricInterpretation decompressed photometric interpretation
   *  @return true if color-by-plane is required, false otherwise.
   */
  static OFBool requiresPlanarConfiguration(
    const char *sopClassUID,
    EP_Interpretation photometricInterpretation);
};

#endif
