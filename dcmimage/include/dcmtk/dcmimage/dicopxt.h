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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorPixelTemplate (Header)
 *
 */


#ifndef DICOPXT_H
#define DICOPXT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofbmanip.h"

#include "dcmtk/dcmimage/dicopx.h"
#include "dcmtk/dcmimgle/dipxrept.h"


/********************************************************************/


inline Uint8 removeSign(const Uint8 value, const Uint8)
{
    return value;
}


inline Uint16 removeSign(const Uint16 value, const Uint16)
{
    return value;
}


inline Uint32 removeSign(const Uint32 value, const Uint32)
{
    return value;
}


inline Uint8 removeSign(const Sint8 value, const Sint8 offset)
{
    return OFstatic_cast(Uint8, OFstatic_cast(Sint16, value) + OFstatic_cast(Sint16, offset) + 1);
}


inline Uint16 removeSign(const Sint16 value, const Sint16 offset)
{
    return OFstatic_cast(Uint16, OFstatic_cast(Sint32, value) + OFstatic_cast(Sint32, offset) + 1);
}

/*
inline Uint32 removeSign(const Sint32 value, const Sint32 offset)
{
    return (value < 0) ? OFstatic_cast(Uint32, value + offset + 1) : OFstatic_cast(Uint32, value) + OFstatic_cast(Uint32, offset) + 1;
}


inline Uint8 removeSign(const Sint8 value, const Uint8 mask)
{
    return OFstatic_cast(Uint8, value) ^ mask;
}


inline Uint16 removeSign(const Sint16 value, const Uint16 mask)
{
    return OFstatic_cast(Uint16, value) ^ mask;
}
*/

inline Uint32 removeSign(const Sint32 value, const Uint32 mask)
{
    return OFstatic_cast(Uint32, value) ^ mask;
}


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle color pixel data
 */
template<class T>
class DiColorPixelTemplate
  : public DiColorPixel,
    public DiPixelRepresentationTemplate<T>
{

 public:

    /** constructor
     *
     ** @param  docu         pointer to the DICOM document
     *  @param  pixel        pointer to input pixel data
     *  @param  samples      number of expected samples per pixel (for checking purposes)
     *  @param  status       status of the image object (reference variable)
     *  @param  sample_rate  dummy parameter (used for derived classes only)
     */
    DiColorPixelTemplate(const DiDocument *docu,
                         const DiInputPixel *pixel,
                         const Uint16 samples,
                         EI_Status &status,
                         const Uint16 sample_rate = 0)
      : DiColorPixel(docu, pixel, samples, status, sample_rate)
    {
        Data[0] = NULL;
        Data[1] = NULL;
        Data[2] = NULL;
    }

    /** destructor
     */
    virtual ~DiColorPixelTemplate()
    {
#if defined(HAVE_STD__NOTHROW) && defined(HAVE_NOTHROW_DELETE)
        /* use a non-throwing delete (if available) */
        operator delete[] (Data[0], std::nothrow);
        operator delete[] (Data[1], std::nothrow);
        operator delete[] (Data[2], std::nothrow);
#else
        delete[] Data[0];
        delete[] Data[1];
        delete[] Data[2];
#endif
    }

    /** get integer representation
     *
     ** @return integer representation of the internally stored pixel data
     */
    inline EP_Representation getRepresentation() const
    {
        return DiPixelRepresentationTemplate<T>::getRepresentation();
    }

    /** get pointer to internal array of pixel data.
     *  The returned array [0..2] points to the three image planes.
     *
     ** @return pointer to array of pixel data
     */
    inline const void *getData() const
    {
        return OFstatic_cast(const void *, Data);
    }

    /** get pointer to internal array of pixel data.
     *  The returned array [0..2] points to the three image planes.
     *
     ** @return pointer to array of pixel data
     */
    inline void *getDataPtr()
    {
        return OFstatic_cast(void *, Data);
    }

    /** get pointer to internal array of pixel data.
     *  The returned array [0..2] points to the three image planes.
     *
     ** @return reference to pointer to pixel data
     */
    inline void *getDataArrayPtr()
    {
        return OFstatic_cast(void *, Data);
    }

    /** fill given memory block with pixel data (all three image planes, RGB)
     *
     ** @param  data    pointer to memory block (array of 8 or 16 bit values, OB/OW)
     *  @param  count   number of T-size entries allocated in the 'data' array
     *  @param  fcount  number of pixels per frame
     *  @param  frames  total number of frames present in intermediate representation
     *  @param  planar  flag indicating whether data shall be stored color-by-pixel or color-by-plane
     *
     ** @return OFTrue if successful, OFFalse otherwise
     */
    OFBool getPixelData(void *data,
                        const unsigned long count,
                        const unsigned long fcount,
                        const unsigned long frames,
                        const int planar) const
    {
        OFBool result = OFFalse;
        /* check parameters and internal data */
        if ((data != NULL) && (count >= Count * 3) && (frames > 0) && (frames * fcount <= Count) &&
            (Data[0] != NULL) && (Data[1] != NULL) && (Data[2] != NULL))
        {
            T *q = OFstatic_cast(T *, data);
            int j;
            unsigned long k;
            unsigned long offset = 0;
            if (planar)
            {
                /* for all frames ... */
                for (k = 0; k < frames; ++k)
                {
                    /* copy all three planes to the given memory block */
                    for (j = 0; j < 3; ++j)
                    {
                        OFBitmanipTemplate<T>::copyMem(Data[j] + offset, q, fcount);
                        q += fcount;
                    }
                    offset += fcount;
                }
            } else {
                unsigned long i;
                /* for all frames ... */
                for (k = 0; k < frames; ++k)
                {
                    /* copy pixel data values from internal representation */
                    for (i = 0; i < fcount; ++i)
                    {
                        for (j = 0; j < 3; ++j)
                            *(q++) = Data[j][i + offset];
                    }
                    offset += fcount;
                }
            }
            result = OFTrue;
        }
        return result;
    }

    /** create true color (24/32 bit) bitmap for MS Windows.
     *
     ** @param  data        untyped pointer memory buffer (set to NULL if not allocated externally)
     *  @param  size        size of the memory buffer in bytes (if 0 'data' is set to NULL)
     *  @param  width       number of columns of the image
     *  @param  height      number of rows of the image
     *  @param  frame       index of frame to be converted (starting from 0)
     *  @param  fromBits    number of bits per sample used for internal representation of the image
     *  @param  toBits      number of bits per sample used for the output bitmap (<= 8)
     *  @param  mode        color output mode (24 or 32 bits, see dcmimgle/dcmimage.h for details)
     *  @param  upsideDown  specifies the order of lines in the images (0 = top-down, bottom-up otherwise)
     *  @param  padding     align each line to a 32-bit address if true
     *
     ** @return number of bytes allocated by the bitmap, or 0 if an error occurred
     */
    unsigned long createDIB(void *&data,
                            const unsigned long size,
                            const Uint16 width,
                            const Uint16 height,
                            const unsigned long frame,
                            const int fromBits,
                            const int toBits,
                            const int mode,
                            const int upsideDown,
                            const int padding) const
    {
        unsigned long bytes = 0;
        if ((Data[0] != NULL) && (Data[1] != NULL) && (Data[2] != NULL) && (toBits <= 8))
        {
            const unsigned long count = OFstatic_cast(unsigned long, width) * OFstatic_cast(unsigned long, height);
            const unsigned long start = count * frame + ((upsideDown) ?
                OFstatic_cast(unsigned long, height - 1) * OFstatic_cast(unsigned long, width) : 0);
            const signed long nextRow = (upsideDown) ? -2 * OFstatic_cast(signed long, width) : 0;
            const T *r = Data[0] + start;
            const T *g = Data[1] + start;
            const T *b = Data[2] + start;
            Uint16 x;
            Uint16 y;
            if (mode == 24)     // 24 bits per pixel
            {
                const unsigned long wid3 = OFstatic_cast(unsigned long, width) * 3;
                // each line has to start at 32-bit-address, if 'padding' is true
                const int gap = (padding) ? OFstatic_cast(int, (4 - (wid3 & 0x3)) & 0x3) : 0;
                unsigned long fsize = (wid3 + gap) * OFstatic_cast(unsigned long, height);
                if ((data == NULL) || (size >= fsize))
                {
                    if (data == NULL)
                        data = new Uint8[fsize];
                    if (data != NULL)
                    {
                        Uint8 *q = OFstatic_cast(Uint8 *, data);
                        if (fromBits == toBits)
                        {
                            /* copy pixel data as is */
                            for (y = height; y != 0; y--)
                            {
                                for (x = width; x != 0; x--)
                                {
                                    /* reverse sample order: B-G-R */
                                    *(q++) = OFstatic_cast(Uint8, *(b++));
                                    *(q++) = OFstatic_cast(Uint8, *(g++));
                                    *(q++) = OFstatic_cast(Uint8, *(r++));
                                }
                                r += nextRow; g += nextRow; b += nextRow;           // go backwards if 'upsideDown'
                                q += gap;                                           // new line: jump to next 32-bit address
                            }
                        }
                        else if (fromBits < toBits)
                        {
                            /* increase color depth: multiply with factor */
                            const double gradient1 = OFstatic_cast(double, DicomImageClass::maxval(toBits)) /
                                                     OFstatic_cast(double, DicomImageClass::maxval(fromBits));
                            const Uint8 gradient2 = OFstatic_cast(Uint8, gradient1);
                            if (gradient1 == OFstatic_cast(double, gradient2))      // integer multiplication?
                            {
                                for (y = height; y != 0; y--)
                                {
                                    for (x = width; x != 0; x--)
                                    {
                                        /* reverse sample order: B-G-R */
                                        *(q++) = OFstatic_cast(Uint8, *(b++) * gradient2);
                                        *(q++) = OFstatic_cast(Uint8, *(g++) * gradient2);
                                        *(q++) = OFstatic_cast(Uint8, *(r++) * gradient2);
                                    }
                                    r += nextRow; g += nextRow; b += nextRow;       // go backwards if 'upsideDown'
                                    q += gap;                                       // new line: jump to next 32-bit address
                                }
                            } else {
                                for (y = height; y != 0; y--)
                                {
                                    for (x = width; x != 0; x--)
                                    {
                                        /* reverse sample order: B-G-R */
                                        *(q++) = OFstatic_cast(Uint8, OFstatic_cast(double, *(b++)) * gradient1);
                                        *(q++) = OFstatic_cast(Uint8, OFstatic_cast(double, *(g++)) * gradient1);
                                        *(q++) = OFstatic_cast(Uint8, OFstatic_cast(double, *(r++)) * gradient1);
                                    }
                                    r += nextRow; g += nextRow; b += nextRow;       // go backwards if 'upsideDown'
                                    q += gap;                                       // new line: jump to next 32-bit address
                                }
                            }
                        }
                        else /* fromBits > toBits */
                        {
                            /* reduce color depth: right shift */
                            const int shift = fromBits - toBits;
                            for (y = height; y != 0; y--)
                            {
                                for (x = width; x != 0; x--)
                                {
                                    /* reverse sample order: B-G-R */
                                    *(q++) = OFstatic_cast(Uint8, *(b++) >> shift);
                                    *(q++) = OFstatic_cast(Uint8, *(g++) >> shift);
                                    *(q++) = OFstatic_cast(Uint8, *(r++) >> shift);
                                }
                                r += nextRow; g += nextRow; b += nextRow;           // go backwards if 'upsideDown'
                                q += gap;                                           // new line: jump to next 32-bit address
                            }
                        }
                        bytes = fsize;
                    }
                }
            }
            else if (mode == 32)     // 32 bits per pixel
            {
                const unsigned long fsize = count * 4;
                if ((data == NULL) || (size >= fsize))
                {
                    if (data == NULL)
                        data = new Uint32[count];
                    if (data != NULL)
                    {
                        Uint32 *q = OFstatic_cast(Uint32 *, data);
                        if (fromBits == toBits)
                        {
                            /* copy pixel data as is */
                            for (y = height; y != 0; y--)
                            {
                                for (x = width; x != 0; x--)
                                {
                                    /* normal sample order: 0-R-G-B */
                                    *(q++) = (OFstatic_cast(Uint32, *(r++)) << 16) |
                                             (OFstatic_cast(Uint32, *(g++)) << 8) |
                                             OFstatic_cast(Uint32, *(b++));
                                }
                                r += nextRow; g += nextRow; b += nextRow;           // go backwards if 'upsideDown'
                            }
                        }
                        else if (fromBits < toBits)
                        {
                            /* increase color depth: multiply with factor */
                            const double gradient1 = OFstatic_cast(double, DicomImageClass::maxval(toBits)) /
                                                     OFstatic_cast(double, DicomImageClass::maxval(fromBits));
                            const Uint32 gradient2 = OFstatic_cast(Uint32, gradient1);
                            if (gradient1 == OFstatic_cast(double, gradient2))      // integer multiplication?
                            {
                                for (y = height; y != 0; y--)
                                {
                                    for (x = width; x != 0; x--)
                                    {
                                        /* normal sample order: 0-R-G-B */
                                        *(q++) = (OFstatic_cast(Uint32, *(r++) * gradient2) << 16) |
                                                 (OFstatic_cast(Uint32, *(g++) * gradient2) << 8) |
                                                 OFstatic_cast(Uint32, *(b++) * gradient2);
                                    }
                                    r += nextRow; g += nextRow; b += nextRow;       // go backwards if 'upsideDown'
                                }
                            } else {
                                for (y = height; y != 0; y--)
                                {
                                    for (x = width; x != 0; x--)
                                    {
                                        /* normal sample order: 0-R-G-B */
                                        *(q++) = (OFstatic_cast(Uint32, OFstatic_cast(double, *(r++)) * gradient1) << 16) |
                                                 (OFstatic_cast(Uint32, OFstatic_cast(double, *(g++)) * gradient1) << 8) |
                                                 OFstatic_cast(Uint32, OFstatic_cast(double, *(b++)) * gradient1);
                                    }
                                    r += nextRow; g += nextRow; b += nextRow;       // go backwards if 'upsideDown'
                                }
                            }
                        }
                        else /* fromBits > toBits */
                        {
                            /* reduce color depth: right shift */
                            const int shift = fromBits - toBits;
                            for (y = height; y != 0; y--)
                            {
                                for (x = width; x != 0; x--)
                                {
                                    /* normal sample order: 0-R-G-B */
                                    *(q++) = (OFstatic_cast(Uint32, *(r++) >> shift) << 16) |
                                             (OFstatic_cast(Uint32, *(g++) >> shift) << 8) |
                                             OFstatic_cast(Uint32, *(b++) >> shift);
                                }
                                r += nextRow; g += nextRow; b += nextRow;           // go backwards if 'upsideDown'
                            }
                        }
                        bytes = fsize;
                    }
                }
            }
        }
        return bytes;
    }

    /** create true color (32 bit) bitmap for Java (AWT default format).
     *
     ** @param  data      resulting pointer to bitmap data (set to NULL if an error occurred)
     *  @param  width     number of columns of the image
     *  @param  height    number of rows of the image
     *  @param  frame     index of frame to be converted (starting from 0)
     *  @param  fromBits  number of bits per sample used for internal representation of the image
     *  @param  toBits    number of bits per sample used for the output bitmap (<= 8)
     *
     ** @return number of bytes allocated by the bitmap, or 0 if an error occurred
     */
    unsigned long createAWTBitmap(void *&data,
                                  const Uint16 width,
                                  const Uint16 height,
                                  const unsigned long frame,
                                  const int fromBits,
                                  const int toBits) const
    {
        data = NULL;
        unsigned long bytes = 0;
        if ((Data[0] != NULL) && (Data[1] != NULL) && (Data[2] != NULL) && (toBits <= 8))
        {
            const unsigned long count = OFstatic_cast(unsigned long, width) * OFstatic_cast(unsigned long, height);
            data = new Uint32[count];
            if (data != NULL)
            {
                const unsigned long start = count * frame;
                const T *r = Data[0] + start;
                const T *g = Data[1] + start;
                const T *b = Data[2] + start;
                Uint32 *q = OFstatic_cast(Uint32 *, data);
                unsigned long i;
                if (fromBits == toBits)
                {
                    /* copy pixel data as is */
                    for (i = count; i != 0; --i)
                    {
                        /* sample order: R-G-B */
                        *(q++) = (OFstatic_cast(Uint32, *(r++)) << 24) |
                                 (OFstatic_cast(Uint32, *(g++)) << 16) |
                                 (OFstatic_cast(Uint32, *(b++)) << 8);
                    }
                }
                else if (fromBits < toBits)
                {
                    /* increase color depth: multiply with factor */
                    const double gradient1 = OFstatic_cast(double, DicomImageClass::maxval(toBits)) /
                                             OFstatic_cast(double, DicomImageClass::maxval(fromBits));
                    const Uint32 gradient2 = OFstatic_cast(Uint32, gradient1);
                    if (gradient1 == OFstatic_cast(double, gradient2))         // integer multiplication?
                    {
                        for (i = count; i != 0; --i)
                        {
                            /* sample order: R-G-B */
                            *(q++) = (OFstatic_cast(Uint32, *(r++) * gradient2) << 24) |
                                     (OFstatic_cast(Uint32, *(g++) * gradient2) << 16) |
                                     (OFstatic_cast(Uint32, *(b++) * gradient2) << 8);
                        }
                    } else {
                        for (i = count; i != 0; --i)
                        {
                            /* sample order: R-G-B */
                            *(q++) = (OFstatic_cast(Uint32, OFstatic_cast(double, *(r++)) * gradient1) << 24) |
                                     (OFstatic_cast(Uint32, OFstatic_cast(double, *(g++)) * gradient1) << 16) |
                                     (OFstatic_cast(Uint32, OFstatic_cast(double, *(b++)) * gradient1) << 8);
                        }
                    }
                }
                else /* fromBits > toBits */
                {
                    /* reduce color depth: right shift */
                    const int shift = fromBits - toBits;
                    for (i = count; i != 0; --i)
                    {
                        /* sample order: R-G-B */
                        *(q++) = (OFstatic_cast(Uint32, *(r++) >> shift) << 24) |
                                 (OFstatic_cast(Uint32, *(g++) >> shift) << 16) |
                                 (OFstatic_cast(Uint32, *(b++) >> shift) << 8);
                    }
                }
                bytes = count * 4;
            }
        }
        return bytes;
    }


 protected:

    /** constructor
     *
     ** @param  pixel  pointer to intermediate color pixel data
     *  @param  count  number of pixels
     */
    DiColorPixelTemplate(const DiColorPixel *pixel,
                         const unsigned long count)
      : DiColorPixel(pixel, count)
    {
        Data[0] = NULL;
        Data[1] = NULL;
        Data[2] = NULL;
    }

    /** initialize internal memory
     *
     ** @param  pixel  pointer to input pixel data
     *
     ** @return true (1) if successful, false (0) otherwise
     */
    inline int Init(const void *pixel)
    {
        int result = 0;
        if (pixel != NULL)
        {
            result = 1;
            /* allocate data buffer for the 3 planes */
            for (int j = 0; j < 3; j++)
            {
#ifdef HAVE_STD__NOTHROW
                /* use a non-throwing new here (if available) because the allocated buffer can be huge */
                Data[j] = new (std::nothrow) T[Count];
#else
                /* make sure that the pointer is set to NULL in case of error */
                try
                {
                    Data[j] = new T[Count];
                }
                catch (STD_NAMESPACE bad_alloc const &)
                {
                    Data[j] = NULL;
                }
#endif
                if (Data[j] != NULL)
                {
                    /* erase empty part of the buffer (=blacken the background) */
                    if (InputCount < Count)
                        OFBitmanipTemplate<T>::zeroMem(Data[j] + InputCount, Count - InputCount);
                } else {
                    DCMIMAGE_DEBUG("cannot allocate memory buffer for 'Data[" << j << "]' in DiColorPixelTemplate::Init()");
                    result = 0;     // at least one buffer could not be allocated!
                }
            }
        }
        return result;
    }


    /// pointer to pixel data (3 components)
    T *Data[3];


 private:

 // --- declarations to avoid compiler warnings

    DiColorPixelTemplate(const DiColorPixelTemplate<T> &);
    DiColorPixelTemplate<T> &operator=(const DiColorPixelTemplate<T> &);
};


#endif
