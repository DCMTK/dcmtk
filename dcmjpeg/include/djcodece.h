/*
 *
 *  Copyright (C) 1997-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmjpeg
 *
 *  Author:  Marco Eichelberg, Norbert Olges
 *
 *  Purpose: abstract codec class for JPEG encoders.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-11-13 15:56:16 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/include/Attic/djcodece.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DJCODEC_H
#define DJCODEC_H

#include "osconfig.h"
#include "oftypes.h"
#include "dccodec.h"  /* for class DcmCodec */
#include "djutils.h"  /* for enums */
#include "oflist.h"

class OFString;
class DataInterface;
class DJEncoder;
class DcmDataset;
class DJCodecParameter;
class DJDecoder;
class DcmItem;
class DcmPixelItem;
class DicomImage;
class DcmTagKey;

typedef OFList<Uint32> DJOffsetList;

/** abstract codec class for JPEG encoders.
 *  This abstract class contains most of the application logic
 *  needed for a dcmdata codec object that implements a JPEG encoder 
 *  using the DJEncoder interface to the underlying JPEG implementation.
 *  This class only supports compression, it neither implements
 *  decoding nor transcoding.
 */
class DJCodecEncoder : public DcmCodec
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

  /** stores a single compressed frame in a DICOM pixel sequence
   *  @param pixelSequence pointer to pixel sequence in which
   *    compressed frame data is to be inserted. Must not be NULL.
   *  @param offsetList list containing offset table entries.
   *    Upon success, an entry is appended to the list
   *  @param jpegData pointer to compressed JPEG data, must not be NULL
   *  @param jpegLen number of bytes of compressed JPEG data
   *  @param cp codec parameter
   *  @return EC_Normal if successful, an error code otherwise
   */  
  virtual OFCondition storeCompressedFrame(
        DcmPixelSequence *pixelSequence, 
        DJOffsetList& offsetList, 
        Uint8 *jpegData, 
        Uint32 jpegLen,
        const DJCodecParameter *cp) const;

  /** creates an offset table for a compressed pixel sequence.
   *  @param offsetList list of offset entries created by
   *    (possibly multiple) calls to storeCompressedFrame().
   *  @param offsetTable pointer to offset table element to
   *    which the table is written, must not be NULL.
   *  @return EC_Normal if successful, an error code otherwise
   */   
  virtual OFCondition createOffsetTable(
        DJOffsetList& offsetList, 
        DcmPixelItem *offsetTable) const;

  /** create new SOP instance UID and Source Image Sequence
   *  referencing the old SOP instance.
   *  @param dataset dataset to be modified
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition newInstance(DcmItem *dataset) const;

  /** set first two values of Image Type to DERIVED\SECONDARY.
   *  @param dataset dataset to be modified
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition updateImageType(DcmItem *dataset) const;

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
   *  @param tag tag key of attribute to check/insert
   *  @param val string value, may be NULL.
   *  @return EC_Normal if successful, an error code otherwise
   */   
  static OFCondition convertToSecondaryCapture(DcmItem *dataset);

  /** modifies all VOI window center/width settings in the image.
   *  Modifications are based on the pixel value mapping
   *  f(x) = (x+voiOffset)*voiFactor
   *  @param dataset dataset to be updated
   *  @param voiOffset offset to be added to each pixel
   *  @param voiFactor factor to be multiplied
   *  @return EC_Normal if successful, an error code otherwise
   */   
  static OFCondition correctVOIWindows(DcmItem *dataset, double voiOffset, double voiFactor);

};

#endif

/*
 * CVS/RCS Log
 * $Log: djcodece.h,v $
 * Revision 1.1  2001-11-13 15:56:16  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
