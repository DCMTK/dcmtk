/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.42 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
     *  @param  high       position of bigh bit within bits allocated
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
        delete[] Data;
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
            register T2 *p = Data;
            register unsigned long i;
            const unsigned long ocnt = OFstatic_cast(unsigned long, getAbsMaxRange());
            Uint8 *lut = NULL;
            if ((sizeof(T2) <= 2) && (Count > 3 * ocnt))               // optimization criteria
            {
                lut = new Uint8[ocnt];
                if (lut != NULL)
                {
                    DCMIMGLE_DEBUG("using optimized routine with additional LUT");
                    OFBitmanipTemplate<Uint8>::zeroMem(lut, ocnt);
                    register Uint8 *q = lut - OFstatic_cast(T2, getAbsMinimum());
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
                register T2 value = *p;
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
     *  @param  highBit        position of bigh bit within bits allocated
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
        if ((document->getFlags() & CIF_UsePartialAccessToPixelData) && (PixelCount > 0) &&
            (!uncompressed || !pixelData->valueLoaded()) && (bitsAllocated % 8 == 0))
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
            pixel = new T1[count_T1 + extraByte];
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
            }
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
            register unsigned long i;
            Data = new T2[Count];
            if (Data != NULL)
            {
                DCMIMGLE_TRACE("Input length: " << lengthBytes << " bytes, Pixel count: " << Count
                    << " (" << PixelCount << "), In: " << bitsof_T1 << " bits, Out: " << bitsof_T2
                    << " bits (" << (this->isSigned() ? "signed" : "unsigned") << ")");
                register const T1 *p = pixel;
                register T2 *q = Data;
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
                        register T1 mask = 0;
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
                    register T1 mask = 0;
                    for (i = 0; i < bitsStored; ++i)
                        mask |= OFstatic_cast(T1, 1 << i);
                    register Uint16 j;
                    register T1 value;
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
                else if ((bitsof_T1 < bitsAllocated) && (bitsAllocated % bitsof_T1 == 0)    // case 3: multiplicant of 8/16
                    && (bitsStored == bitsAllocated))
                {
                    DCMIMGLE_DEBUG("convert input pixel data: case 3 (multi copy)");
                    const Uint16 times = bitsAllocated / bitsof_T1;
                    register Uint16 j;
                    register Uint16 shift;
                    register T2 value;
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
                    register T2 value = 0;
                    register Uint16 bits = 0;
                    register Uint32 skip = highBit + 1 - bitsStored;
                    register Uint32 times;
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
            }
        } else {
            // in case of error, reset pixel count variable
            Count = 0;
        }
        if (deletePixel)
        {
            // delete temporary buffer
            delete[] pixel;
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


/*
 *
 * CVS/RCS Log:
 * $Log: diinpxt.h,v $
 * Revision 1.42  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.41  2010-07-22 10:27:30  joergr
 * Made sure that the size of the buffer for partial access to pixel data is
 * always an even number of bytes.
 *
 * Revision 1.40  2010-04-16 12:56:46  joergr
 * Further enhanced computation of buffer size when using partial read access
 * to pixel data. Now also some rare cases of BitsAllocated are supported.
 *
 * Revision 1.39  2010-04-15 14:18:36  joergr
 * Fixed possibly wrong computation of a buffer size when using partial read
 * access to pixel data. This could lead to a crash under certain conditions.
 *
 * Revision 1.38  2010-03-01 09:08:46  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.37  2009-11-25 16:05:40  joergr
 * Adapted code for new approach to access individual frames of a DICOM image.
 * Added more logging messages. Revised logging messages.
 *
 * Revision 1.36  2009-10-28 14:38:16  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.35  2009-10-28 09:53:40  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.34  2007-08-30 13:39:30  joergr
 * Added further check on pixel pointer (possibly avoids crash under certain
 * conditions).
 *
 * Revision 1.33  2006/10/27 14:59:26  joergr
 * Fixed possible integer overflow for images with very large pixel data.
 *
 * Revision 1.32  2006/08/15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.31  2006/01/17 18:35:42  joergr
 * Fixed compilation problem with gcc 4.0 on Linux x86_64.
 *
 * Revision 1.30  2005/12/08 16:47:44  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.29  2004/04/21 10:00:36  meichel
 * Minor modifications for compilation with gcc 3.4.0
 *
 * Revision 1.28  2004/02/06 11:07:50  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.27  2004/01/05 14:52:20  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.26  2003/12/23 15:53:22  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.25  2003/12/08 19:10:52  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.24  2002/10/21 10:13:50  joergr
 * Corrected wrong calculation of min/max pixel value in cases where the
 * stored pixel data exceeds the expected size.
 *
 * Revision 1.23  2001/11/13 18:07:36  joergr
 * Fixed bug occurring when processing monochrome images with an odd number of
 * pixels.
 *
 * Revision 1.22  2001/10/10 15:25:09  joergr
 * Removed redundant variable declarations to avoid compiler warnings
 * ("declaration of ... shadows previous local").
 *
 * Revision 1.21  2001/09/28 13:04:59  joergr
 * Enhanced algorithm to determine the min and max value.
 *
 * Revision 1.20  2001/06/01 15:49:42  meichel
 * Updated copyright header
 *
 * Revision 1.19  2000/05/03 09:46:28  joergr
 * Removed most informational and some warning messages from release built
 * (#ifndef DEBUG).
 *
 * Revision 1.18  2000/04/28 12:32:30  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.17  2000/04/27 13:08:39  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.16  2000/03/08 16:24:17  meichel
 * Updated copyright header.
 *
 * Revision 1.15  2000/03/03 14:09:12  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.14  1999/09/17 12:21:57  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 * Enhanced efficiency of some "for" loops and of the implementation to
 * determine min/max values of the input pixels.
 *
 * Revision 1.13  1999/07/23 13:54:38  joergr
 * Optimized memory usage for converting input pixel data (reference instead
 * of copying where possible).
 *
 * Revision 1.12  1999/05/04 09:20:39  meichel
 * Minor code purifications to keep IBM xlC quiet
 *
 * Revision 1.11  1999/04/30 16:23:59  meichel
 * Minor code purifications to keep IBM xlC quiet
 *
 * Revision 1.10  1999/04/28 14:48:39  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.9  1999/03/24 17:20:03  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.8  1999/02/11 16:00:54  joergr
 * Removed inline declarations from several methods.
 *
 * Revision 1.7  1999/02/03 17:04:37  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 *
 * Revision 1.6  1999/01/20 15:01:31  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.5  1999/01/11 09:34:28  joergr
 * Corrected bug in determing 'AbsMaximum' (removed '+ 1').
 *
 * Revision 1.4  1998/12/22 14:23:16  joergr
 * Added calculation of member variables AbsMinimum/AbsMaximum.
 * Replaced method copyMem by for-loop copying each item.
 * Removed some '#ifdef DEBUG'.
 *
 * Revision 1.3  1998/12/16 16:30:34  joergr
 * Added methods to determine absolute minimum and maximum value for given
 * value representation.
 *
 * Revision 1.2  1998/12/14 17:18:23  joergr
 * Reformatted source code.
 *
 * Revision 1.1  1998/11/27 15:08:21  joergr
 * Added copyright message.
 * Introduced global debug level for dcmimage module to control error output.
 * Added support for new bit manipulation class.
 *
 * Revision 1.8  1998/07/01 08:39:21  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.7  1998/05/11 14:53:17  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
