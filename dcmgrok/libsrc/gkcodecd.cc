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

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmgrok/gkcodecd.h"

#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcpixseq.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcvrpobw.h"
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmgrok/gkcparam.h"
#include "dcmtk/dcmgrok/gkutils.h"

#include <grok.h>

#include <cstring>
#include <memory>


E_TransferSyntax DGRKJ2KLosslessDecoder::supportedTransferSyntax() const
{
    return EXS_JPEG2000LosslessOnly;
}

E_TransferSyntax DGRKJ2KDecoder::supportedTransferSyntax() const
{
    return EXS_JPEG2000;
}

E_TransferSyntax DGRKHTJ2KLosslessDecoder::supportedTransferSyntax() const
{
    return EXS_HighThroughputJPEG2000LosslessOnly;
}

E_TransferSyntax DGRKHTJ2KDecoder::supportedTransferSyntax() const
{
    return EXS_HighThroughputJPEG2000;
}


// --------------------------------------------------------------------------

DGRKDecoderBase::DGRKDecoderBase()
: DcmCodec()
{
}


DGRKDecoderBase::~DGRKDecoderBase()
{
}


OFBool DGRKDecoderBase::canChangeCoding(
    const E_TransferSyntax oldRepType,
    const E_TransferSyntax newRepType) const
{
    DcmXfer newRep(newRepType);
    if (newRep.usesNativeFormat() && (oldRepType == supportedTransferSyntax()))
        return OFTrue;

    return OFFalse;
}


Uint16 DGRKDecoderBase::decodedBitsAllocated(
    Uint16 /* bitsAllocated */,
    Uint16 bitsStored) const
{
    if (bitsStored < 1) return 0;
    if (bitsStored <= 8) return 8;
    if (bitsStored <= 16) return 16;
    return 0;
}


OFCondition DGRKDecoderBase::decode(
    const DcmRepresentationParameter * /* fromRepParam */,
    DcmPixelSequence *pixSeq,
    DcmPolymorphOBOW &uncompressedPixelData,
    const DcmCodecParameter *cp,
    const DcmStack &objStack,
    OFBool &removeOldRep) const
{
    removeOldRep = OFTrue;

    DcmStack localStack(objStack);
    (void)localStack.pop();
    DcmObject *dobject = localStack.pop();
    if ((!dobject) || ((dobject->ident() != EVR_dataset) && (dobject->ident() != EVR_item)))
        return EC_InvalidTag;
    auto *dataset = OFstatic_cast(DcmItem *, dobject);
    OFBool numberOfFramesPresent = OFFalse;

    Uint16 imageSamplesPerPixel = 0;
    if (dataset->findAndGetUint16(DCM_SamplesPerPixel, imageSamplesPerPixel).bad()) return EC_TagNotFound;
    if ((imageSamplesPerPixel != 3) && (imageSamplesPerPixel != 1)) return EC_InvalidTag;

    Uint16 imageRows = 0;
    if (dataset->findAndGetUint16(DCM_Rows, imageRows).bad()) return EC_TagNotFound;
    if (imageRows < 1) return EC_InvalidTag;

    Uint16 imageColumns = 0;
    if (dataset->findAndGetUint16(DCM_Columns, imageColumns).bad()) return EC_TagNotFound;
    if (imageColumns < 1) return EC_InvalidTag;

    Sint32 imageFrames = 0;
    if (dataset->findAndGetSint32(DCM_NumberOfFrames, imageFrames).good()) numberOfFramesPresent = OFTrue;

    if (imageFrames >= OFstatic_cast(Sint32, pixSeq->card()))
        imageFrames = pixSeq->card() - 1;
    if (imageFrames < 1)
        imageFrames = 1;

    Uint16 imageBitsStored = 0;
    if (dataset->findAndGetUint16(DCM_BitsStored, imageBitsStored).bad()) return EC_TagNotFound;

    Uint16 imageBitsAllocated = 0;
    if (dataset->findAndGetUint16(DCM_BitsAllocated, imageBitsAllocated).bad()) return EC_TagNotFound;

    Uint16 imageHighBit = 0;
    if (dataset->findAndGetUint16(DCM_HighBit, imageHighBit).bad()) return EC_TagNotFound;

    if ((imageBitsStored < 1) || (imageBitsStored > 16)) return EC_InvalidTag;

    Uint16 bytesPerSample = 1;
    if (imageBitsStored > 8) bytesPerSample = 2;
    else if (imageBitsAllocated > 8) bytesPerSample = 2;

    Uint32 frameSize = bytesPerSample * imageRows * imageColumns * imageSamplesPerPixel;

    if (imageRows != 0 && frameSize / imageRows != (OFstatic_cast(Uint32, bytesPerSample) * imageColumns * imageSamplesPerPixel))
    {
        DCMGROK_WARN("cannot decompress image because uncompressed representation would exceed maximum possible size of PixelData attribute");
        return EC_ElemLengthExceeds32BitField;
    }

    Uint32 totalSize = frameSize * imageFrames;

    if (totalSize == 0xFFFFFFFF || (frameSize != 0 && totalSize / frameSize != OFstatic_cast(Uint32, imageFrames)))
    {
        DCMGROK_WARN("cannot decompress image because uncompressed representation would exceed maximum possible size of PixelData attribute");
        return EC_ElemLengthExceeds32BitField;
    }

    if (totalSize & 1) totalSize++;

    const auto *gkcp = OFreinterpret_cast(const DGRKCodecParameter *, cp);

    Uint16 *pixeldata16 = NULL;
    auto result = uncompressedPixelData.createUint16Array(totalSize / sizeof(Uint16), pixeldata16);
    if (result.bad()) return result;

    auto *pixeldata8 = OFreinterpret_cast(Uint8 *, pixeldata16);
    Sint32 currentFrame = 0;
    Uint32 currentItem = 1;
    OFBool done = OFFalse;

    while (result.good() && !done)
    {
        DCMGROK_DEBUG("Grok JPEG 2000 decoder processes frame " << (currentFrame + 1));

        result = decodeFrame(pixSeq, gkcp, dataset, currentFrame, currentItem, pixeldata8, frameSize,
            imageFrames, imageColumns, imageRows, imageSamplesPerPixel, bytesPerSample);

        if (result.good())
        {
            if (++currentFrame == imageFrames) done = OFTrue;
            pixeldata8 += frameSize;
        }
    }

    if (result.good() && (numberOfFramesPresent || (imageFrames > 1)))
    {
        char numBuf[20];
        OFStandard::snprintf(numBuf, sizeof(numBuf), "%ld", OFstatic_cast(long, imageFrames));
        result = ((DcmItem *)dataset)->putAndInsertString(DCM_NumberOfFrames, numBuf);
    }

    if (result.good())
    {
        if (bytesPerSample == 1)
        {
            result = swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, pixeldata16,
                OFstatic_cast(Uint32, totalSize), sizeof(Uint16));
        }
    }

    if (result.good() && (dataset->ident() == EVR_dataset))
    {
        auto *ditem = OFreinterpret_cast(DcmItem *, dataset);

        if (gkcp->getUIDCreation() == EGRKUC_always)
        {
            result = DcmCodec::newInstance(ditem, NULL, NULL, NULL);
        }

        if (result.good() && (supportedTransferSyntax() == EXS_JPEG2000 ||
                              supportedTransferSyntax() == EXS_HighThroughputJPEG2000))
        {
            result = ditem->putAndInsertString(DCM_LossyImageCompression, "01");
        }
    }

    return result;
}


OFCondition DGRKDecoderBase::decodeFrame(
    const DcmRepresentationParameter * /* fromParam */,
    DcmPixelSequence *fromPixSeq,
    const DcmCodecParameter *cp,
    DcmItem *dataset,
    Uint32 frameNo,
    Uint32 &currentItem,
    void *buffer,
    Uint32 bufSize,
    OFString &decompressedColorModel) const
{
    OFCondition result = EC_Normal;

    const auto *gkcp = OFreinterpret_cast(const DGRKCodecParameter *, cp);

    Uint16 imageSamplesPerPixel = 0;
    if (dataset->findAndGetUint16(DCM_SamplesPerPixel, imageSamplesPerPixel).bad()) return EC_TagNotFound;
    if ((imageSamplesPerPixel != 3) && (imageSamplesPerPixel != 1)) return EC_InvalidTag;

    Uint16 imageRows = 0;
    if (dataset->findAndGetUint16(DCM_Rows, imageRows).bad()) return EC_TagNotFound;
    if (imageRows < 1) return EC_InvalidTag;

    Uint16 imageColumns = 0;
    if (dataset->findAndGetUint16(DCM_Columns, imageColumns).bad()) return EC_TagNotFound;
    if (imageColumns < 1) return EC_InvalidTag;

    Uint16 imageBitsStored = 0;
    if (dataset->findAndGetUint16(DCM_BitsStored, imageBitsStored).bad()) return EC_TagNotFound;

    Uint16 imageBitsAllocated = 0;
    if (dataset->findAndGetUint16(DCM_BitsAllocated, imageBitsAllocated).bad()) return EC_TagNotFound;

    if ((imageBitsStored < 1) || (imageBitsStored > 16)) return EC_InvalidTag;

    Uint16 bytesPerSample = 1;
    if (imageBitsStored > 8) bytesPerSample = 2;
    else if (imageBitsAllocated > 8) bytesPerSample = 2;

    Sint32 imageFrames = 0;
    dataset->findAndGetSint32(DCM_NumberOfFrames, imageFrames);

    if (imageFrames >= OFstatic_cast(Sint32, fromPixSeq->card()))
        imageFrames = fromPixSeq->card() - 1;
    if (imageFrames < 1)
        imageFrames = 1;

    if (currentItem == 0)
    {
        result = determineStartFragment(frameNo, imageFrames, fromPixSeq, currentItem);
    }

    if (result.good())
    {
        DCMGROK_DEBUG("starting to decode frame " << frameNo << " with fragment " << currentItem);
        result = decodeFrame(fromPixSeq, gkcp, dataset, frameNo, currentItem, buffer, bufSize,
            imageFrames, imageColumns, imageRows, imageSamplesPerPixel, bytesPerSample);
    }

    if (result.good())
    {
        if (bytesPerSample == 1)
        {
            if ((gLocalByteOrder == EBO_BigEndian) && (bufSize & 1))
            {
                DCMGROK_WARN("Size of frame buffer is odd, cannot correct byte order for last pixel value");
            }
            result = swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, buffer, bufSize, sizeof(Uint16));
        }
    }

    if (result.good())
    {
        result = dataset->findAndGetOFString(DCM_PhotometricInterpretation, decompressedColorModel);
    }

    return result;
}


OFCondition DGRKDecoderBase::decodeFrame(
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
    Uint16 bytesPerSample)
{
    DcmPixelItem *pixItem = NULL;
    Uint8 *fragmentData = NULL;
    Uint32 fragmentLength = 0;
    size_t compressedSize = 0;
    Uint32 fragmentsForThisFrame = 0;
    OFCondition result = EC_Normal;
    auto ignoreOffsetTable = cp->ignoreOffsetTable();

    fragmentsForThisFrame = computeNumberOfFragments(imageFrames, frameNo, startFragment, ignoreOffsetTable, fromPixSeq);
    if (fragmentsForThisFrame == 0) result = EC_CorruptedData;

    OFString imageSopClass;
    OFString imagePhotometricInterpretation;
    dataset->findAndGetOFString(DCM_SOPClassUID, imageSopClass);
    dataset->findAndGetOFString(DCM_PhotometricInterpretation, imagePhotometricInterpretation);
    Uint16 imagePlanarConfiguration = 0;

    if (imageSamplesPerPixel > 1)
    {
        imagePlanarConfiguration = 2;
        if (dataset->findAndGetUint16(DCM_PlanarConfiguration, imagePlanarConfiguration).good() && (imagePlanarConfiguration != 0))
            DCMGROK_WARN("invalid value for PlanarConfiguration " << DCM_PlanarConfiguration << ", should be \"0\"");

        switch (cp->getPlanarConfiguration())
        {
            case EGRKPC_restore:
                if (imagePlanarConfiguration > 1)
                    imagePlanarConfiguration = determinePlanarConfiguration(imageSopClass, imagePhotometricInterpretation);
                break;
            case EGRKPC_auto:
                imagePlanarConfiguration = determinePlanarConfiguration(imageSopClass, imagePhotometricInterpretation);
                break;
            case EGRKPC_colorByPixel:
                imagePlanarConfiguration = 0;
                break;
            case EGRKPC_colorByPlane:
                imagePlanarConfiguration = 1;
                break;
        }
    }

    /* gather all fragments for this frame */
    if (result.good())
    {
        Uint32 fragmentsForThisFrame2 = fragmentsForThisFrame;
        Uint32 currentItem2 = startFragment;

        while (result.good() && fragmentsForThisFrame2--)
        {
            result = fromPixSeq->getItem(pixItem, currentItem2++);
            if (result.good() && pixItem)
            {
                fragmentLength = pixItem->getLength();
                if (result.good())
                    compressedSize += fragmentLength;
            }
        }
    }

    std::unique_ptr<Uint8[]> j2kData;
    if (result.good())
    {
        Uint32 offset = 0;
        j2kData.reset(new Uint8[compressedSize]);

        Uint32 remainingFragments = fragmentsForThisFrame;
        while (result.good() && remainingFragments--)
        {
            result = fromPixSeq->getItem(pixItem, startFragment++);
            if (result.good() && pixItem)
            {
                fragmentLength = pixItem->getLength();
                result = pixItem->getUint8Array(fragmentData);
                if (result.good() && fragmentData)
                {
                    memcpy(j2kData.get() + offset, fragmentData, fragmentLength);
                    offset += fragmentLength;
                }
            }
        }
    }

    if (result.good())
    {
        /* set up Grok stream parameters for buffer-based decompression */
        grk_stream_params streamParams;
        memset(&streamParams, 0, sizeof(streamParams));
        streamParams.buf = j2kData.get();
        streamParams.buf_len = compressedSize;
        streamParams.is_read_stream = true;

        grk_decompress_parameters decompressParams;
        memset(&decompressParams, 0, sizeof(decompressParams));
        decompressParams.core.tile_cache_strategy = GRK_TILE_CACHE_NONE;

        auto *codec = grk_decompress_init(&streamParams, &decompressParams);
        if (!codec)
        {
            DCMGROK_ERROR("Grok JPEG 2000: failed to initialize decompressor");
            return EC_CorruptedData;
        }

        grk_header_info headerInfo;
        memset(&headerInfo, 0, sizeof(headerInfo));
        if (!grk_decompress_read_header(codec, &headerInfo))
        {
            DCMGROK_ERROR("Grok JPEG 2000: failed to read codestream header");
            grk_object_unref(codec);
            return EC_CorruptedData;
        }

        /* validate header against DICOM attributes */
        auto j2kWidth = headerInfo.header_image.x1 - headerInfo.header_image.x0;
        auto j2kHeight = headerInfo.header_image.y1 - headerInfo.header_image.y0;
        auto j2kNumComps = headerInfo.header_image.numcomps;

        if (j2kWidth != imageColumns || j2kHeight != imageRows || j2kNumComps != imageSamplesPerPixel)
        {
            DCMGROK_ERROR("Grok JPEG 2000: image dimensions mismatch "
                << "(DICOM: " << imageColumns << "x" << imageRows << "x" << imageSamplesPerPixel
                << ", J2K: " << j2kWidth << "x" << j2kHeight << "x" << j2kNumComps << ")");
            grk_object_unref(codec);
            return EC_CorruptedData;
        }

        if (!grk_decompress(codec, NULL))
        {
            DCMGROK_ERROR("Grok JPEG 2000: decompression failed");
            grk_object_unref(codec);
            return EC_CorruptedData;
        }

        auto *image = grk_decompress_get_image(codec);
        if (!image || !image->comps)
        {
            DCMGROK_ERROR("Grok JPEG 2000: failed to retrieve decompressed image");
            grk_object_unref(codec);
            return EC_CorruptedData;
        }

        /*
         * Grok decodes each component into a separate int32_t* plane.
         * We need to interleave the component data into the DICOM pixel buffer.
         * For single-component (grayscale) images, we just copy from the plane.
         * For multi-component (color) images, Grok always gives us separate planes,
         * so we produce color-by-pixel (planarConfiguration=0) output first,
         * then convert to the desired planar configuration if needed.
         */
        auto *outBuf = OFreinterpret_cast(Uint8 *, buffer);

        if (imageSamplesPerPixel == 1)
        {
            /* grayscale: copy single component to output */
            auto *src = OFreinterpret_cast(int32_t *, image->comps[0].data);
            if (!src)
            {
                grk_object_unref(codec);
                return EC_CorruptedData;
            }

            uint32_t numPixels = OFstatic_cast(uint32_t, imageColumns) * imageRows;
            if (bytesPerSample == 1)
            {
                for (uint32_t i = 0; i < numPixels; i++)
                    outBuf[i] = OFstatic_cast(Uint8, src[i]);
            }
            else
            {
                auto *out16 = OFreinterpret_cast(Uint16 *, outBuf);
                for (uint32_t i = 0; i < numPixels; i++)
                    out16[i] = OFstatic_cast(Uint16, src[i]);
            }
        }
        else
        {
            /* color: interleave components to color-by-pixel */
            auto *src0 = OFreinterpret_cast(int32_t *, image->comps[0].data);
            auto *src1 = OFreinterpret_cast(int32_t *, image->comps[1].data);
            auto *src2 = OFreinterpret_cast(int32_t *, image->comps[2].data);
            if (!src0 || !src1 || !src2)
            {
                grk_object_unref(codec);
                return EC_CorruptedData;
            }

            uint32_t numPixels = OFstatic_cast(uint32_t, imageColumns) * imageRows;
            if (bytesPerSample == 1)
            {
                for (uint32_t i = 0; i < numPixels; i++)
                {
                    outBuf[i * 3]     = OFstatic_cast(Uint8, src0[i]);
                    outBuf[i * 3 + 1] = OFstatic_cast(Uint8, src1[i]);
                    outBuf[i * 3 + 2] = OFstatic_cast(Uint8, src2[i]);
                }
            }
            else
            {
                auto *out16 = OFreinterpret_cast(Uint16 *, outBuf);
                for (uint32_t i = 0; i < numPixels; i++)
                {
                    out16[i * 3]     = OFstatic_cast(Uint16, src0[i]);
                    out16[i * 3 + 1] = OFstatic_cast(Uint16, src1[i]);
                    out16[i * 3 + 2] = OFstatic_cast(Uint16, src2[i]);
                }
            }

            /* convert to desired planar configuration if needed */
            if (imagePlanarConfiguration == 1)
            {
                DCMGROK_DEBUG("converting decompressed color data to planar configuration 1");
                if (bytesPerSample == 1)
                    result = createPlanarConfiguration1Byte(OFreinterpret_cast(Uint8 *, buffer), imageColumns, imageRows);
                else
                    result = createPlanarConfiguration1Word(OFreinterpret_cast(Uint16 *, buffer), imageColumns, imageRows);
            }
        }

        grk_object_unref(codec);

        if (result.good() && (imageSamplesPerPixel > 1))
        {
            dataset->putAndInsertUint16(DCM_PlanarConfiguration, imagePlanarConfiguration);
        }
    }

    return result;
}


OFCondition DGRKDecoderBase::encode(
    const Uint16 * /* pixelData */,
    const Uint32 /* length */,
    const DcmRepresentationParameter * /* toRepParam */,
    DcmPixelSequence * & /* pixSeq */,
    const DcmCodecParameter * /* cp */,
    DcmStack & /* objStack */,
    OFBool & /* removeOldRep */) const
{
    return EC_IllegalCall;
}


OFCondition DGRKDecoderBase::encode(
    const E_TransferSyntax /* fromRepType */,
    const DcmRepresentationParameter * /* fromRepParam */,
    DcmPixelSequence * /* fromPixSeq */,
    const DcmRepresentationParameter * /* toRepParam */,
    DcmPixelSequence * & /* toPixSeq */,
    const DcmCodecParameter * /* cp */,
    DcmStack & /* objStack */,
    OFBool & /* removeOldRep */) const
{
    return EC_IllegalCall;
}


OFCondition DGRKDecoderBase::determineDecompressedColorModel(
    const DcmRepresentationParameter * /* fromParam */,
    DcmPixelSequence * /* fromPixSeq */,
    const DcmCodecParameter * /* cp */,
    DcmItem *dataset,
    OFString &decompressedColorModel) const
{
    OFCondition result = EC_IllegalParameter;
    if (dataset != NULL)
    {
        result = dataset->findAndGetOFString(DCM_PhotometricInterpretation, decompressedColorModel);
        if (result == EC_TagNotFound)
        {
            DCMGROK_WARN("mandatory element PhotometricInterpretation " << DCM_PhotometricInterpretation << " is missing");
            result = EC_MissingAttribute;
        }
        else if (result.bad())
        {
            DCMGROK_WARN("cannot retrieve value of element PhotometricInterpretation " << DCM_PhotometricInterpretation << ": " << result.text());
        }
        else if (decompressedColorModel.empty())
        {
            DCMGROK_WARN("no value for mandatory element PhotometricInterpretation " << DCM_PhotometricInterpretation);
            result = EC_MissingValue;
        }
    }
    return result;
}


Uint16 DGRKDecoderBase::determinePlanarConfiguration(
    const OFString &sopClassUID,
    const OFString &photometricInterpretation)
{
    if (sopClassUID == UID_RETIRED_HardcopyColorImageStorage) return 1;

    if (photometricInterpretation == "YBR_FULL")
    {
        if ((sopClassUID == UID_UltrasoundMultiframeImageStorage) ||
            (sopClassUID == UID_UltrasoundImageStorage)) return 1;
    }

    return 0;
}


Uint32 DGRKDecoderBase::computeNumberOfFragments(
    Sint32 numberOfFrames,
    Uint32 currentFrame,
    Uint32 startItem,
    OFBool ignoreOffsetTable,
    DcmPixelSequence *pixSeq)
{
    auto numItems = pixSeq->card();
    DcmPixelItem *pixItem = NULL;

    if ((numberOfFrames <= 1) || (currentFrame + 1 == OFstatic_cast(Uint32, numberOfFrames)))
    {
        return (numItems - startItem);
    }
    if (OFstatic_cast(Uint32, numberOfFrames + 1) == numItems)
    {
        return 1;
    }

    OFCondition result = EC_Normal;
    if (!ignoreOffsetTable)
    {
        result = pixSeq->getItem(pixItem, 0);
        if (result.good() && pixItem)
        {
            auto offsetTableLength = pixItem->getLength();
            if (offsetTableLength == (OFstatic_cast(Uint32, numberOfFrames) * 4))
            {
                Uint8 *offsetData = NULL;
                result = pixItem->getUint8Array(offsetData);
                if (result.good() && offsetData)
                {
                    auto *offsetData32 = OFreinterpret_cast(Uint32 *, offsetData);
                    Uint32 offset = offsetData32[currentFrame + 1];
                    swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, &offset, sizeof(Uint32), sizeof(Uint32));

                    Uint32 byteCount = 0;
                    Uint32 fragmentIndex = 1;
                    while ((byteCount < offset) && (fragmentIndex < numItems))
                    {
                        pixItem = NULL;
                        result = pixSeq->getItem(pixItem, fragmentIndex++);
                        if (result.good() && pixItem)
                        {
                            byteCount += pixItem->getLength() + 8;
                            if ((byteCount == offset) && (fragmentIndex > startItem))
                            {
                                return fragmentIndex - startItem;
                            }
                        }
                        else break;
                    }
                }
            }
        }
    }

    /* scan for JPEG 2000 SOC (Start Of Codestream) markers: 0xFF4F */
    Uint32 nextItem = startItem;
    Uint8 *scanData = NULL;
    while (++nextItem < numItems)
    {
        pixItem = NULL;
        result = pixSeq->getItem(pixItem, nextItem);
        if (result.good() && pixItem)
        {
            scanData = NULL;
            result = pixItem->getUint8Array(scanData);
            if (result.good() && scanData && (pixItem->getLength() > 1))
            {
                if (scanData[0] == 0xFF && scanData[1] == 0x4F)
                {
                    return (nextItem - startItem);
                }
            }
            else break;
        }
        else break;
    }

    return 0;
}


OFCondition DGRKDecoderBase::createPlanarConfiguration1Byte(
    Uint8 *imageFrame,
    Uint16 columns,
    Uint16 rows)
{
    if (imageFrame == NULL) return EC_IllegalCall;

    unsigned long numPixels = columns * rows;
    if (numPixels == 0) return EC_IllegalCall;

    std::unique_ptr<Uint8[]> buf(new Uint8[3 * numPixels + 3]);
    memcpy(buf.get(), imageFrame, OFstatic_cast(size_t, 3 * numPixels));
    Uint8 *s = buf.get();
    Uint8 *r = imageFrame;
    Uint8 *g = imageFrame + numPixels;
    Uint8 *b = imageFrame + (2 * numPixels);
    for (unsigned long i = numPixels; i; i--)
    {
        *r++ = *s++;
        *g++ = *s++;
        *b++ = *s++;
    }
    return EC_Normal;
}


OFCondition DGRKDecoderBase::createPlanarConfiguration1Word(
    Uint16 *imageFrame,
    Uint16 columns,
    Uint16 rows)
{
    if (imageFrame == NULL) return EC_IllegalCall;

    unsigned long numPixels = columns * rows;
    if (numPixels == 0) return EC_IllegalCall;

    std::unique_ptr<Uint16[]> buf(new Uint16[3 * numPixels + 3]);
    memcpy(buf.get(), imageFrame, OFstatic_cast(size_t, 3 * numPixels * sizeof(Uint16)));
    Uint16 *s = buf.get();
    Uint16 *r = imageFrame;
    Uint16 *g = imageFrame + numPixels;
    Uint16 *b = imageFrame + (2 * numPixels);
    for (unsigned long i = numPixels; i; i--)
    {
        *r++ = *s++;
        *g++ = *s++;
        *b++ = *s++;
    }
    return EC_Normal;
}


OFCondition DGRKDecoderBase::createPlanarConfiguration0Byte(
    Uint8 *imageFrame,
    Uint16 columns,
    Uint16 rows)
{
    if (imageFrame == NULL) return EC_IllegalCall;

    unsigned long numPixels = columns * rows;
    if (numPixels == 0) return EC_IllegalCall;

    std::unique_ptr<Uint8[]> buf(new Uint8[3 * numPixels + 3]);
    memcpy(buf.get(), imageFrame, OFstatic_cast(size_t, 3 * numPixels));
    Uint8 *t = imageFrame;
    Uint8 *r = buf.get();
    Uint8 *g = buf.get() + numPixels;
    Uint8 *b = buf.get() + (2 * numPixels);
    for (unsigned long i = numPixels; i; i--)
    {
        *t++ = *r++;
        *t++ = *g++;
        *t++ = *b++;
    }
    return EC_Normal;
}


OFCondition DGRKDecoderBase::createPlanarConfiguration0Word(
    Uint16 *imageFrame,
    Uint16 columns,
    Uint16 rows)
{
    if (imageFrame == NULL) return EC_IllegalCall;

    unsigned long numPixels = columns * rows;
    if (numPixels == 0) return EC_IllegalCall;

    std::unique_ptr<Uint16[]> buf(new Uint16[3 * numPixels + 3]);
    memcpy(buf.get(), imageFrame, OFstatic_cast(size_t, 3 * numPixels * sizeof(Uint16)));
    Uint16 *t = imageFrame;
    Uint16 *r = buf.get();
    Uint16 *g = buf.get() + numPixels;
    Uint16 *b = buf.get() + (2 * numPixels);
    for (unsigned long i = numPixels; i; i--)
    {
        *t++ = *r++;
        *t++ = *g++;
        *t++ = *b++;
    }
    return EC_Normal;
}
