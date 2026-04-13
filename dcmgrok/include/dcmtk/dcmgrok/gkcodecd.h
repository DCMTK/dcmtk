/*
 *
 *  Copyright (C) 2026, Grok Image Compression Inc.
 *  All rights reserved.
 *
 *  This software and supporting documentation were developed by
 *  Grok Image Compression Inc.
 *
 *  Module:  dcmgrok
 *
 *  Author:  Aaron Boxer
 *
 *  Purpose: codec classes for Grok JPEG 2000 decoders.
 *
 */

#ifndef DCMGROK_GKCODECD_H
#define DCMGROK_GKCODECD_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dccodec.h"  /* for class DcmCodec */
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmgrok/gkdefine.h"

/* forward declaration */
class DGRKCodecParameter;

/** abstract codec class for Grok JPEG 2000 decoders.
 *  This abstract class contains most of the application logic
 *  needed for a dcmdata codec object that implements a JPEG 2000 decoder
 *  using the Grok library.  This class only supports decompression,
 *  it neither implements encoding nor transcoding.
 */
class DCMTK_DCMGROK_EXPORT DGRKDecoderBase : public DcmCodec
{
public:

    /// default constructor
    DGRKDecoderBase();

    /// destructor
    virtual ~DGRKDecoderBase();

    /** decompresses the given pixel sequence and
     *  stores the result in the given uncompressedPixelData element.
     */
    virtual OFCondition decode(
        const DcmRepresentationParameter *fromRepParam,
        DcmPixelSequence *pixSeq,
        DcmPolymorphOBOW &uncompressedPixelData,
        const DcmCodecParameter *cp,
        const DcmStack &objStack,
        OFBool &removeOldRep) const;

    /** decompresses a single frame from the given pixel sequence and
     *  stores the result in the given buffer.
     */
    virtual OFCondition decodeFrame(
        const DcmRepresentationParameter *fromParam,
        DcmPixelSequence *fromPixSeq,
        const DcmCodecParameter *cp,
        DcmItem *dataset,
        Uint32 frameNo,
        Uint32 &startFragment,
        void *buffer,
        Uint32 bufSize,
        OFString &decompressedColorModel) const;

    /** compresses the given uncompressed DICOM image — not supported.
     */
    virtual OFCondition encode(
        const Uint16 *pixelData,
        const Uint32 length,
        const DcmRepresentationParameter *toRepParam,
        DcmPixelSequence *&pixSeq,
        const DcmCodecParameter *cp,
        DcmStack &objStack,
        OFBool &removeOldRep) const;

    /** transcodes (re-compresses) — not supported.
     */
    virtual OFCondition encode(
        const E_TransferSyntax fromRepType,
        const DcmRepresentationParameter *fromRepParam,
        DcmPixelSequence *fromPixSeq,
        const DcmRepresentationParameter *toRepParam,
        DcmPixelSequence *&toPixSeq,
        const DcmCodecParameter *cp,
        DcmStack &objStack,
        OFBool &removeOldRep) const;

    /** checks if this codec is able to convert from the
     *  given current transfer syntax to the given new
     *  transfer syntax
     */
    virtual OFBool canChangeCoding(
        const E_TransferSyntax oldRepType,
        const E_TransferSyntax newRepType) const;

    /** determines the effective value of BitsAllocated after decompression.
     */
    virtual Uint16 decodedBitsAllocated(
        Uint16 bitsAllocated,
        Uint16 bitsStored) const;

    /** determine color model of the decompressed image
     */
    virtual OFCondition determineDecompressedColorModel(
        const DcmRepresentationParameter *fromParam,
        DcmPixelSequence *fromPixSeq,
        const DcmCodecParameter *cp,
        DcmItem *dataset,
        OFString &decompressedColorModel) const;

private:

    /** returns the transfer syntax that this particular codec
     *  is able to decode
     *  @return supported transfer syntax
     */
    virtual E_TransferSyntax supportedTransferSyntax() const = 0;

    /** decompresses a single frame.
     *  @param fromPixSeq compressed pixel sequence
     *  @param cp codec parameters
     *  @param dataset dataset containing image attributes
     *  @param frameNo frame number (0-based)
     *  @param startFragment starting fragment index
     *  @param buffer output buffer
     *  @param bufSize buffer size in bytes
     *  @param imageFrames total number of frames
     *  @param imageColumns columns
     *  @param imageRows rows
     *  @param imageSamplesPerPixel samples per pixel
     *  @param bytesPerSample bytes per sample
     *  @return EC_Normal if successful
     */
    static OFCondition decodeFrame(
        DcmPixelSequence *fromPixSeq,
        const DGRKCodecParameter *cp,
        DcmItem *dataset,
        Uint32 frameNo,
        Uint32 &startFragment,
        void *buffer,
        Uint32 bufSize,
        Sint32 imageFrames,
        Uint16 imageColumns,
        Uint16 imageRows,
        Uint16 imageSamplesPerPixel,
        Uint16 bytesPerSample);

    /** determine number of fragments for a given frame
     */
    static Uint32 computeNumberOfFragments(
        Sint32 numberOfFrames,
        Uint32 frameNo,
        Uint32 startItem,
        OFBool ignoreOffsetTable,
        DcmPixelSequence *pixSeq);

    /** determine planar configuration for uncompressed data
     */
    static Uint16 determinePlanarConfiguration(
        const OFString &sopClassUID,
        const OFString &photometricInterpretation);

    /** create planar configuration 1 byte interleaving
     */
    static OFCondition createPlanarConfiguration1Byte(
        Uint8 *imageFrame,
        Uint16 columns,
        Uint16 rows);

    /** create planar configuration 1 word interleaving
     */
    static OFCondition createPlanarConfiguration1Word(
        Uint16 *imageFrame,
        Uint16 columns,
        Uint16 rows);

    /** create planar configuration 0 byte interleaving
     */
    static OFCondition createPlanarConfiguration0Byte(
        Uint8 *imageFrame,
        Uint16 columns,
        Uint16 rows);

    /** create planar configuration 0 word interleaving
     */
    static OFCondition createPlanarConfiguration0Word(
        Uint16 *imageFrame,
        Uint16 columns,
        Uint16 rows);
};


/** codec class for JPEG 2000 Lossless Only decoding
 */
class DCMTK_DCMGROK_EXPORT DGRKJ2KLosslessDecoder : public DGRKDecoderBase
{
    virtual E_TransferSyntax supportedTransferSyntax() const;
};

/** codec class for JPEG 2000 (lossy) decoding
 */
class DCMTK_DCMGROK_EXPORT DGRKJ2KDecoder : public DGRKDecoderBase
{
    virtual E_TransferSyntax supportedTransferSyntax() const;
};

/** codec class for High-Throughput JPEG 2000 Lossless Only decoding
 */
class DCMTK_DCMGROK_EXPORT DGRKHTJ2KLosslessDecoder : public DGRKDecoderBase
{
    virtual E_TransferSyntax supportedTransferSyntax() const;
};

/** codec class for High-Throughput JPEG 2000 (lossy) decoding
 */
class DCMTK_DCMGROK_EXPORT DGRKHTJ2KDecoder : public DGRKDecoderBase
{
    virtual E_TransferSyntax supportedTransferSyntax() const;
};

#endif
