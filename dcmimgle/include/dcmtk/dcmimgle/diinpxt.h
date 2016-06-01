/*
 *
 *  Copyright (C) 1996-2016, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomInputPixelTemplate (Header)
 *
 */


#ifndef DIINPXT_H
#define DIINPXT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcpixel.h"

#include "dcmtk/ofstd/ofbmanip.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/diinpx.h"
#include "dcmtk/dcmimgle/didocu.h"
#include "dcmtk/dcmimgle/dipxrept.h"


/*--------------------*
 *  helper functions  *
 *--------------------*/

static inline Uint8 expandSign(const Uint8 Value,
                               const Uint8,
                               const Uint8)
{
    return Value;
}


static inline Uint16 expandSign(const Uint16 Value,
                                const Uint16,
                                const Uint16)
{
    return Value;
}


static inline Uint32 expandSign(const Uint32 Value,
                                const Uint32,
                                const Uint32)
{
    return Value;
}


static inline Sint8 expandSign(const Sint8 Value,
                               const Sint8 SignBit,
                               const Sint8 SignMask)
{
    return (Value & SignBit) ? (Value | SignMask) : Value;
}


static inline Sint16 expandSign(const Sint16 Value,
                                const Sint16 SignBit,
                                const Sint16 SignMask)
{
    return (Value & SignBit) ? (Value | SignMask) : Value;
}


static inline Sint32 expandSign(const Sint32 Value,
                                const Sint32 SignBit,
                                const Sint32 SignMask)
{
    return (Value & SignBit) ? (Value | SignMask) : Value;
}


static Uint32 getPixelData(DcmPixelData *PixelData,
                           Uint8 *&pixel)
{
    PixelData->getUint8Array(pixel);
    return PixelData->getLength();
}


static Uint32 getPixelData(DcmPixelData *PixelData,
                           Uint16 *&pixel)
{
    PixelData->getUint16Array(pixel);
    return PixelData->getLength();
}


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to convert DICOM pixel stream to intermediate representation
 */
template<class T1, class T2>
class DiInputPixelTemplate
  : public DiInputPixel,
    public DiPixelRepresentationTemplate<T2>
{

 public:

    /** constructor
     *
     ** @param  document   pointer to DICOM image object
     *  @param  alloc      number of bits allocated for each pixel
     *  @param  stored     number of bits stored for each pixel
     *  @param  high       position of high bit within bits allocated
     *  @param  first      first frame to be processed
     *  @param  number     number of frames to be processed
     *  @param  fsize      number of pixels per frame (frame size)
     *  @param  fileCache  pointer to file cache object used for partial read
     *  @param  fragment   current pixel item fragment (for encapsulated pixel data)
     */
    DiInputPixelTemplate(const DiDocument *document,
                         const Uint16 alloc,
                         const Uint16 stored,
                         const Uint16 high,
                         const unsigned long first,
                         const unsigned long number,
                         const unsigned long fsize,
                         DcmFileCache *fileCache,
                         Uint32 &fragment)
      : DiInputPixel(stored, first, number, fsize),
        Data(NULL)
    {
        MinValue[0] = 0;
        MinValue[1] = 0;
        MaxValue[0] = 0;
        MaxValue[1] = 0;
        if (this->isSigned())
        {
            AbsMinimum = -OFstatic_cast(double, DicomImageClass::maxval(Bits - 1, 0));
            AbsMaximum = OFstatic_cast(double, DicomImageClass::maxval(Bits - 1));
        } else {
            AbsMinimum = 0;
            AbsMaximum = OFstatic_cast(double, DicomImageClass::maxval(Bits));
        }
        if ((document != NULL) && (document->getPixelData() != NULL))
            convert(document, alloc, stored, high, fileCache, fragment);
        if ((PixelCount == 0) || (PixelStart + PixelCount > Count))         // check for corrupt pixel length
        {
            PixelCount = Count - PixelStart;
            DCMIMGLE_DEBUG("setting number of pixels to be processed (PixelCount) to: " << PixelCount);
        }
    }

    /** destructor
     */
    virtual ~DiInputPixelTemplate()
    {
#if defined(HAVE_STD__NOTHROW) && defined(HAVE_NOTHROW_DELETE)
        /* use a non-throwing delete (if available) */
        operator delete[] (Data, std::nothrow);
#else
        delete[] Data;
#endif
    }

    /** determine minimum and maximum pixel value
     *
     ** @return status, true if successful, false otherwise
     */
    int determineMinMax()
    {
        if (Data != NULL)
        {
            DCMIMGLE_DEBUG("determining minimum and maximum pixel values for input data");
            T2 *p = Data;
            unsigned long i;
            const unsigned long ocnt = OFstatic_cast(unsigned long, getAbsMaxRange());
            Uint8 *lut = NULL;
            if ((sizeof(T2) <= 2) && (Count > 3 * ocnt))               // optimization criteria
            {
                lut = new Uint8[ocnt];
                if (lut != NULL)
                {
                    DCMIMGLE_DEBUG("using optimized routine with additional LUT");
                    OFBitmanipTemplate<Uint8>::zeroMem(lut, ocnt);
                    Uint8 *q = lut - OFstatic_cast(T2, getAbsMinimum());
                    for (i = Count; i != 0; --i)                       // fill lookup table
                        *(q + *(p++)) = 1;
                    q = lut;
                    for (i = 0; i < ocnt; ++i)                         // search for minimum
                    {
                        if (*(q++) != 0)
                        {
                            MinValue[0] = OFstatic_cast(T2, OFstatic_cast(double, i) + getAbsMinimum());
                            break;
                        }
                    }
                    q = lut + ocnt;
                    for (i = ocnt; i != 0; --i)                        // search for maximum
                    {
                        if (*(--q) != 0)
                        {
                            MaxValue[0] = OFstatic_cast(T2, OFstatic_cast(double, i - 1) + getAbsMinimum());
                            break;
                        }
                    }
                    if (Count >= PixelCount)                           // use global min/max value
                    {
                        MinValue[1] = MinValue[0];
                        MaxValue[1] = MaxValue[0];
                    } else {                                           // calculate min/max for selected range
                        OFBitmanipTemplate<Uint8>::zeroMem(lut, ocnt);
                        p = Data + PixelStart;
                        q = lut - OFstatic_cast(T2, getAbsMinimum());
                        for (i = PixelCount; i != 0; --i)                  // fill lookup table
                            *(q + *(p++)) = 1;
                        q = lut;
                        for (i = 0; i < ocnt; ++i)                         // search for minimum
                        {
                            if (*(q++) != 0)
                            {
                                MinValue[1] = OFstatic_cast(T2, OFstatic_cast(double, i) + getAbsMinimum());
                                break;
                            }
                        }
                        q = lut + ocnt;
                        for (i = ocnt; i != 0; --i)                         // search for maximum
                        {
                            if (*(--q) != 0)
                            {
                                MaxValue[1] = OFstatic_cast(T2, OFstatic_cast(double, i - 1) + getAbsMinimum());
                                break;
                            }
                        }
                    }
                }
            }
            if (lut == NULL)                                           // use conventional method
            {
                T2 value = *p;
                MinValue[0] = value;
                MaxValue[0] = value;
                for (i = Count; i > 1; --i)
                {
                    value = *(++p);
                    if (value < MinValue[0])
                        MinValue[0] = value;
                    else if (value > MaxValue[0])
                        MaxValue[0] = value;
                }
                if (Count <= PixelCount)                               // use global min/max value
                {
                    MinValue[1] = MinValue[0];
                    MaxValue[1] = MaxValue[0];
                } else {                                               // calculate min/max for selected range
                    p = Data + PixelStart;
                    value = *p;
                    MinValue[1] = value;
                    MaxValue[1] = value;
                    for (i = PixelCount; i > 1; --i)
                    {
                        value = *(++p);
                        if (value < MinValue[1])
                            MinValue[1] = value;
                        else if (value > MaxValue[1])
                            MaxValue[1] = value;
                    }
                }
            }
            delete[] lut;
            return 1;
        }
        return 0;
    }

    /** get pixel representation
     *
     ** @return pixel representation
     */
    inline EP_Representation getRepresentation() const
    {
        return DiPixelRepresentationTemplate<T2>::getRepresentation();
    }

    /** get pointer to input pixel data
     *
     ** @return pointer to input pixel data
     */
    inline const void *getData() const
    {
        return OFstatic_cast(const void *, Data);
    }

    /** get reference to pointer to input pixel data
     *
     ** @return reference to pointer to input pixel data
     */
    virtual void *getDataPtr()
    {
        return OFstatic_cast(void *, Data);
    }

    /** remove reference to (internally handled) pixel data
     */
    inline void removeDataReference()
    {
        Data = NULL;
    }

    /** get minimum pixel value
     *
     ** @param  idx  specifies whether to return the global minimum (0) or
     *               the minimum of the selected pixel range (1, see PixelStart/Range)
     *
     ** @return minimum pixel value
     */
    inline double getMinValue(const int idx) const
    {
        return (idx == 0) ? OFstatic_cast(double, MinValue[0]) : OFstatic_cast(double, MinValue[1]);
    }

    /** get maximum pixel value
     *
     ** @param  idx  specifies whether to return the global maximum (0) or
     *               the maximum of the selected pixel range (1, see PixelStart/Range)
     *
     ** @return maximum pixel value
     */
    inline double getMaxValue(const int idx) const
    {
        return (idx == 0) ? OFstatic_cast(double, MaxValue[0]) : OFstatic_cast(double, MaxValue[1]);
    }


 private:

    /** convert pixel data from DICOM dataset to input representation
     *
     ** @param  document       pointer to DICOM image object
     *  @param  bitsAllocated  number of bits allocated for each pixel
     *  @param  bitsStored     number of bits stored for each pixel
     *  @param  highBit        position of high bit within bits allocated
     *  @param  fileCache      pointer to file cache object used for partial read
     *  @param  fragment       current pixel item fragment (for encapsulated pixel data)
     */
    void convert(const DiDocument *document,
                 const Uint16 bitsAllocated,
                 const Uint16 bitsStored,
                 const Uint16 highBit,
                 DcmFileCache *fileCache,
                 Uint32 &fragment)
    {
        T1 *pixel = NULL;
        OFBool deletePixel = OFFalse;
        Uint32 lengthBytes = 0;
        DcmPixelData *pixelData = document->getPixelData();
        const Uint16 bitsof_T1 = bitsof(T1);
        const Uint16 bitsof_T2 = bitsof(T2);
        const OFBool uncompressed = pixelData->canWriteXfer(EXS_LittleEndianExplicit, EXS_Unknown);
        /* check whether to use partial read */
        if ((document->getFlags() & CIF_UsePartialAccessToPixelData) && (PixelCount > 0) && (bitsAllocated % 8 == 0))
        {
            /* Bits Allocated is always a multiple of 8 (see above), same for bits of T1 */
            const Uint32 byteFactor = bitsAllocated / 8;
            const Uint32 bytes_T1 = bitsof_T1 / 8;
            const Uint32 count_T1 = (byteFactor == bytes_T1) ? PixelCount : (PixelCount * byteFactor + bytes_T1 - 1) / bytes_T1;
#ifdef DEBUG
            DCMIMGLE_TRACE("PixelCount: " << PixelCount << ", byteFactor: " << byteFactor << ", bytes_T1: " << bytes_T1 << ", count_T1: " << count_T1);
#endif
            /* allocate temporary buffer, even number of bytes required for getUncompressedFrame() */
            const Uint32 extraByte = ((sizeof(T1) == 1) && (count_T1 & 1)) ? 1 : 0;
#ifdef HAVE_STD__NOTHROW
            /* use a non-throwing new here (if available) because the allocated buffer can be huge */
            pixel = new (std::nothrow) T1[count_T1 + extraByte];
#else
            /* make sure that the pointer is set to NULL in case of error */
            try
            {
                pixel = new T1[count_T1 + extraByte];
            }
            catch (STD_NAMESPACE bad_alloc const &)
            {
                pixel = NULL;
            }
#endif
            if (pixel != NULL)
            {
                if (uncompressed)
                {
                    DCMIMGLE_DEBUG("using partial read access to uncompressed pixel data");
                    const Uint32 offset = PixelStart * byteFactor;
                    const Uint32 bufSize = PixelCount * byteFactor;
                    const OFCondition status = pixelData->getPartialValue(pixel, offset, bufSize, fileCache);
                    if (status.good())
                    {
                        PixelStart = 0;
                        lengthBytes = bufSize;
                    } else {
                        DCMIMGLE_ERROR("can't access partial value from byte offset " << offset << " to "
                            << (offset + bufSize - 1) << ": " << status.text());
                    }
                } else {
                    DCMIMGLE_DEBUG("using partial read access to compressed pixel data");
                    OFCondition status = EC_IllegalCall;
                    OFString decompressedColorModel;
                    const Uint32 fsize = FrameSize * byteFactor;
                    for (Uint32 frame = 0; frame < NumberOfFrames; ++frame)
                    {
                        /* make sure that the buffer always has an even number of bytes as required for getUncompressedFrame() */
                        const Uint32 bufSize = (fsize & 1) ? fsize + 1 : fsize;
                        status = pixelData->getUncompressedFrame(document->getDataset(), FirstFrame + frame, fragment,
                            OFreinterpret_cast(Uint8 *, pixel) + lengthBytes, bufSize, decompressedColorModel, fileCache);
                        if (status.good())
                        {
                            DCMIMGLE_TRACE("successfully decompressed frame " << FirstFrame + frame);
                            lengthBytes += fsize;
                        } else {
                            DCMIMGLE_ERROR("can't decompress frame " << FirstFrame + frame << ": " << status.text());
                            break;
                        }
                    }
                    if (status.good())
                        PixelStart = 0;
                    /* check whether color model changed during decompression */
                    if (!decompressedColorModel.empty() && (decompressedColorModel != document->getPhotometricInterpretation()))
                    {
                        DCMIMGLE_WARN("Photometric Interpretation of decompressed pixel data deviates from original image: "
                            << decompressedColorModel);
                    }
                }
                deletePixel = OFTrue;
            } else
                DCMIMGLE_DEBUG("cannot allocate memory buffer for 'pixel' in DiInputPixelTemplate::convert()");
        } else {
            DCMIMGLE_DEBUG("reading uncompressed pixel data completely into memory");
            /* always access complete pixel data */
            lengthBytes = getPixelData(pixelData, pixel);
        }
        if ((pixel != NULL) && (lengthBytes > 0))
        {
            const Uint32 length_T1 = lengthBytes / sizeof(T1);
            /* need to split 'length' in order to avoid integer overflow for large pixel data */
            const Uint32 length_B1 = lengthBytes / bitsAllocated;
            const Uint32 length_B2 = lengthBytes % bitsAllocated;
//          # old code: Count = ((lengthBytes * 8) + bitsAllocated - 1) / bitsAllocated;
            Count = 8 * length_B1 + (8 * length_B2 + bitsAllocated - 1) / bitsAllocated;
            unsigned long i;
#ifdef HAVE_STD__NOTHROW
            /* use a non-throwing new here (if available) because the allocated buffer can be huge */
            Data = new (std::nothrow) T2[Count];
#else
            /* make sure that the pointer is set to NULL in case of error */
            try
            {
                Data = new T2[Count];
            }
            catch (STD_NAMESPACE bad_alloc const &)
            {
                Data = NULL;
            }
#endif
            if (Data != NULL)
            {
                DCMIMGLE_TRACE("Input length: " << lengthBytes << " bytes, Pixel count: " << Count
                    << " (" << PixelCount << "), In: " << bitsof_T1 << " bits, Out: " << bitsof_T2
                    << " bits (" << (this->isSigned() ? "signed" : "unsigned") << ")");
                const T1 *p = pixel;
                T2 *q = Data;
                if (bitsof_T1 == bitsAllocated)                                             // case 1: equal 8/16 bit
                {
                    if (bitsStored == bitsAllocated)
                    {
                        DCMIMGLE_DEBUG("convert input pixel data: case 1a (single copy)");
                        for (i = Count; i != 0; --i)
                            *(q++) = OFstatic_cast(T2, *(p++));
                    }
                    else /* bitsStored < bitsAllocated */
                    {
                        T1 mask = 0;
                        for (i = 0; i < bitsStored; ++i)
                            mask |= OFstatic_cast(T1, 1 << i);
                        const T2 sign = 1 << (bitsStored - 1);
                        T2 smask = 0;
                        for (i = bitsStored; i < bitsof_T2; ++i)
                            smask |= OFstatic_cast(T2, 1 << i);
                        const Uint16 shift = highBit + 1 - bitsStored;
                        if (shift == 0)
                        {
                            DCMIMGLE_DEBUG("convert input pixel data: case 1b (mask & sign)");
                            for (i = length_T1; i != 0; --i)
                                *(q++) = expandSign(OFstatic_cast(T2, *(p++) & mask), sign, smask);
                        }
                        else /* shift > 0 */
                        {
                            DCMIMGLE_DEBUG("convert input pixel data: case 1c (shift & mask & sign)");
                            for (i = length_T1; i != 0; --i)
                                *(q++) = expandSign(OFstatic_cast(T2, (*(p++) >> shift) & mask), sign, smask);
                        }
                    }
                }
                else if ((bitsof_T1 > bitsAllocated) && (bitsof_T1 % bitsAllocated == 0))   // case 2: divisor of 8/16 bit
                {
                    const Uint16 times = bitsof_T1 / bitsAllocated;
                    T1 mask = 0;
                    for (i = 0; i < bitsStored; ++i)
                        mask |= OFstatic_cast(T1, 1 << i);
                    Uint16 j;
                    T1 value;
                    if ((bitsStored == bitsAllocated) && (bitsStored == bitsof_T2))
                    {
                        if (times == 2)
                        {
                            DCMIMGLE_DEBUG("convert input pixel data: case 2a (simple mask)");
                            for (i = length_T1; i != 0; --i, ++p)
                            {
                                *(q++) = OFstatic_cast(T2, *p & mask);
                                *(q++) = OFstatic_cast(T2, *p >> bitsAllocated);
                            }
                            /* check for additional input pixel (in case of odd length when using partial access) */
                            if (length_T1 * 2 /* times */ < lengthBytes)
                            {
                                DCMIMGLE_TRACE("found trailing pixel at the end of odd-length input data ... copying value");
                                *(q++) = OFstatic_cast(T2, *p & mask);
                            }
                        }
                        else
                        {
                            DCMIMGLE_DEBUG("convert input pixel data: case 2b (mask)");
                            for (i = length_T1; i != 0; --i)
                            {
                                value = *(p++);
                                for (j = times; j != 0; --j)
                                {
                                    *(q++) = OFstatic_cast(T2, value & mask);
                                    value >>= bitsAllocated;
                                }
                            }
                        }
                    }
                    else
                    {
                        DCMIMGLE_DEBUG("convert input pixel data: case 2c (shift & mask & sign)");
                        const T2 sign = 1 << (bitsStored - 1);
                        T2 smask = 0;
                        for (i = bitsStored; i < bitsof_T2; ++i)
                            smask |= OFstatic_cast(T2, 1 << i);
                        const Uint16 shift = highBit + 1 - bitsStored;
                        for (i = length_T1; i != 0; --i)
                        {
                            value = *(p++) >> shift;
                            for (j = times; j != 0; --j)
                            {
                                *(q++) = expandSign(OFstatic_cast(T2, value & mask), sign, smask);
                                value >>= bitsAllocated;
                            }
                        }
                    }
                }
                else if ((bitsof_T1 < bitsAllocated) && (bitsAllocated % bitsof_T1 == 0)    // case 3: multiplicand of 8/16
                    && (bitsStored == bitsAllocated))
                {
                    DCMIMGLE_DEBUG("convert input pixel data: case 3 (multi copy)");
                    const Uint16 times = bitsAllocated / bitsof_T1;
                    Uint16 j;
                    Uint16 shift;
                    T2 value;
                    for (i = length_T1; i != 0; --i)
                    {
                        shift = 0;
                        value = OFstatic_cast(T2, *(p++));
                        for (j = times; j > 1; --j, --i)
                        {
                            shift += bitsof_T1;
                            value |= OFstatic_cast(T2, *(p++)) << shift;
                        }
                        *(q++) = value;
                    }
                }
                else                                                                        // case 4: anything else
                {
                    DCMIMGLE_DEBUG("convert input pixel data: case 4 (general)");
                    T2 value = 0;
                    Uint16 bits = 0;
                    Uint32 skip = highBit + 1 - bitsStored;
                    Uint32 times;
                    T1 mask[bitsof_T1];
                    mask[0] = 1;
                    for (i = 1; i < bitsof_T1; ++i)
                        mask[i] = (mask[i - 1] << 1) | 1;
                    T2 smask = 0;
                    for (i = bitsStored; i < bitsof_T2; ++i)
                        smask |= OFstatic_cast(T2, 1 << i);
                    const T2 sign = 1 << (bitsStored - 1);
                    const Uint32 gap = bitsAllocated - bitsStored;
                    i = 0;
                    while (i < length_T1)
                    {
                        if (skip < bitsof_T1)
                        {
                            if (skip + bitsStored - bits < bitsof_T1)       // -++- --++
                            {
                                value |= (OFstatic_cast(T2, (*p >> skip) & mask[bitsStored - bits - 1]) << bits);
                                skip += bitsStored - bits + gap;
                                bits = bitsStored;
                            }
                            else                                            // ++-- ++++
                            {
                                value |= (OFstatic_cast(T2, (*p >> skip) & mask[bitsof_T1 - skip - 1]) << bits);
                                bits += bitsof_T1 - OFstatic_cast(Uint16, skip);
                                skip = (bits == bitsStored) ? gap : 0;
                                ++i;
                                ++p;
                            }
                            if (bits == bitsStored)
                            {
                                *(q++) = expandSign(value, sign, smask);
                                value = 0;
                                bits = 0;
                            }
                        }
                        else
                        {
                            times = skip / bitsof_T1;
                            i += times;
                            p += times;
                            skip -= times * bitsof_T1;
                        }
                    }
                }
            } else
                DCMIMGLE_DEBUG("cannot allocate memory buffer for 'Data' in DiInputPixelTemplate::convert()");
        } else {
            /* in case of error, reset pixel count variable */
            Count = 0;
        }
        if (deletePixel)
        {
            /* delete temporary buffer */
#if defined(HAVE_STD__NOTHROW) && defined(HAVE_NOTHROW_DELETE)
            /* use a non-throwing delete (if available) */
            operator delete[] (pixel, std::nothrow);
#else
            delete[] pixel;
#endif
        }
    }

    /// pointer to pixel data
    T2 *Data;

    /// minimum pixel value ([0] = global, [1] = selected pixel range)
    T2 MinValue[2];
    /// maximum pixel value ([0] = global, [1] = selected pixel range)
    T2 MaxValue[2];

 // --- declarations to avoid compiler warnings

    DiInputPixelTemplate(const DiInputPixelTemplate<T1,T2> &);
    DiInputPixelTemplate<T1,T2> &operator=(const DiInputPixelTemplate<T1,T2> &);
};


#endif
