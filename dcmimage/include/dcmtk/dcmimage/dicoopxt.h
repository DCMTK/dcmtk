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
 *  Purpose: DicomColorOutputPixelTemplate (Header)
 *
 */


#ifndef DICOOPXT_H
#define DICOOPXT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicoopx.h"
#include "dcmtk/dcmimage/dicopx.h"
#include "dcmtk/dcmimgle/dipxrept.h"

#include "dcmtk/ofstd/ofbmanip.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to create color output data
 */
template<class T1, class T2>
class DiColorOutputPixelTemplate
  : public DiColorOutputPixel,
    public DiPixelRepresentationTemplate<T2>
{

 public:

    /** constructor
     *
     ** @param  buffer   storage area for the output pixel data (optional, maybe NULL)
     *  @param  pixel    pointer to intermediate pixel representation (color)
     *  @param  count    number of pixels per frame
     *  @param  frame    frame to be rendered
     *  @param  bits1    bit depth of input data (intermediate)
     *  @param  bits2    bit depth of output data
     *  @param  planar   flag indicating whether data shall be stored color-by-pixel or color-by-plane
     *  @param  inverse  invert pixel data if true (0/0/0 = white)
     */
    DiColorOutputPixelTemplate(void *buffer,
                               const DiColorPixel *pixel,
                               const unsigned long count,
                               const unsigned long frame,
                               const int bits1, /* input depth */
                               const int bits2, /* output depth */
                               const int planar,
                               const int inverse)
      : DiColorOutputPixel(pixel, count, frame),
        Data(NULL),
        DeleteData(buffer == NULL),
        isPlanar(planar)
    {
        if ((pixel != NULL) && (Count > 0) && (FrameSize >= Count))
        {
            Data = OFstatic_cast(T2 *, buffer);
            convert(OFstatic_cast(const T1 **, OFconst_cast(void *, pixel->getData())), frame * FrameSize, bits1, bits2, planar, inverse);
        }
    }

    /** constructor
     *
     ** @param  buffer  storage area for the output pixel data (optional, maybe NULL)
     *  @param  pixel   pointer to intermediate pixel representation
     *  @param  count   number of pixels per frame
     *  @param  frame   frame to be rendered
     * (#)param frames  (total number of frames present in intermediate representation)
     *  @param  planar  flag indicating whether data shall be stored color-by-pixel or color-by-plane
     */
    DiColorOutputPixelTemplate(void *buffer,
                               const DiPixel *pixel,
                               const unsigned long count,
                               const unsigned long frame,
                               const unsigned long /*frames*/,
                               const int planar)
      : DiColorOutputPixel(pixel, count, frame),
        Data(NULL),
        DeleteData(buffer == NULL),
        isPlanar(planar)
    {
        if ((pixel != NULL) && (Count > 0) && (FrameSize >= Count))
            Data = OFstatic_cast(T2 *, buffer);
    }

    /** destructor
     */
    virtual ~DiColorOutputPixelTemplate()
    {
        if (DeleteData)
            delete[] Data;
    }

    /** get integer representation
     *
     ** @return integer representation
     */
    inline EP_Representation getRepresentation() const
    {
        return DiPixelRepresentationTemplate<T2>::getRepresentation();
    }

    /** get size of one pixel / item in the pixel array
     *
     ** @return item size
     */
    inline size_t getItemSize() const
    {
        return sizeof(T2) * 3;
    }

    /** get pointer to output pixel data
     *
     ** @return pointer to pixel data
     */
    inline const void *getData() const
    {
        return OFstatic_cast(const void *, Data);
    }

    /** get pointer to output pixel data
     *
     ** @return pointer to pixel data
     */
    virtual void *getDataPtr()
    {
        return OFstatic_cast(void *, Data);
    }

    /** get pointer to given plane of output pixel data
     *
     ** @param  plane  number of the plane to be retrieved (0..2)
     *
     ** @return pointer to beginning of plane if successful, NULL otherwise
     */
    inline const void *getPlane(const int plane) const
    {
        void *result = NULL;
        if (Data != NULL)
        {
            if (plane <= 0)
                result = OFstatic_cast(void *, Data);
            else
            {
                if (isPlanar)
                    result = OFstatic_cast(void *, Data + ((plane == 1) ? 1 : 2) * FrameSize);
                else
                    result = OFstatic_cast(void *, Data + ((plane == 1) ? 1 : 2));
            }
        }
        return result;
    }

    /** write pixel data of selected frame to PPM/ASCII file
     *
     ** @param  stream  open C++ output stream
     *
     ** @return status, true if successful, false otherwise
     */
    int writePPM(STD_NAMESPACE ostream& stream) const
    {
        if (Data != NULL)
        {
            T2 *p = Data;
            unsigned long i;
            int j;
            for (i = FrameSize; i != 0; --i)
                for (j = 3; j != 0; --j)
                    stream << OFstatic_cast(unsigned long, *(p++)) << " ";     // typecast to resolve problems with 'char'
            return 1;
        }
        return 0;
    }

    /** write pixel data of selected frame to PPM/ASCII file
     *
     ** @param  stream  open C file stream
     *
     ** @return status, true if successful, false otherwise
     */
    int writePPM(FILE *stream) const
    {
        if (Data != NULL)
        {
            T2 *p = Data;
            unsigned long i;
            int j;
            for (i = FrameSize; i != 0; --i)
                for (j = 3; j != 0; --j)
                    fprintf(stream, "%lu ", OFstatic_cast(unsigned long, *(p++)));
            return 1;
        }
        return 0;
    }


 protected:

    /// pointer to the storage area where the output data should be stored
    T2 *Data;


 private:

    /** convert intermediate pixel data to output format (render pixel data)
     *
     ** @param  pixel    pointer to intermediate pixel representation (color)
     *  @param  start    offset to first pixel to be converted
     *  @param  bits1    bit depth of input data (intermediate)
     *  @param  bits2    bit depth of output data
     *  @param  planar   flag indicating whether data shall be stored color-by-pixel or color-by-plane
     *  @param  inverse  invert pixel data if true (0/0/0 = white)
     */
    void convert(const T1 *pixel[3],
                 const unsigned long start,
                 const int bits1,
                 const int bits2,
                 const int planar,
                 const int inverse)
    {
        if ((pixel[0] != NULL) && (pixel[1] != NULL) && (pixel[2] != NULL))
        {
            if (Data == NULL)
                Data = new T2[FrameSize * 3];
            if (Data != NULL)
            {
                DCMIMAGE_DEBUG("converting color pixel data to output format");
                T2 *q = Data;
                unsigned long i;
                const T2 max2 = OFstatic_cast(T2, DicomImageClass::maxval(bits2));
                if (planar)
                {
                    const T1 *p;
                    if (bits1 == bits2)
                    {
                        for (int j = 0; j < 3; ++j)
                        {
                            p = pixel[j] + start;
                            /* invert output data */
                            if (inverse)
                            {
                                for (i = Count; i != 0; --i)                        // copy inverted data
                                    *(q++) = max2 - OFstatic_cast(T2, *(p++));
                            } else {
                                for (i = Count; i != 0; --i)                        // copy
                                    *(q++) = OFstatic_cast(T2, *(p++));
                            }
                            if (Count < FrameSize)
                            {
                                OFBitmanipTemplate<T2>::zeroMem(q, FrameSize - Count);  // set remaining pixels of frame to zero
                                q += (FrameSize - Count);
                            }
                        }
                    }
                    else if (bits1 < bits2)                                     // optimization possible using LUT
                    {
                        const double gradient1 = OFstatic_cast(double, DicomImageClass::maxval(bits2)) /
                                                 OFstatic_cast(double, DicomImageClass::maxval(bits1));
                        const T2 gradient2 = OFstatic_cast(T2, gradient1);
                        for (int j = 0; j < 3; ++j)
                        {
                            p = pixel[j] + start;
                            if (gradient1 == OFstatic_cast(double, gradient2))  // integer multiplication?
                            {
                                /* invert output data */
                                if (inverse)
                                {
                                    for (i = Count; i != 0; --i)                            // expand depth & invert
                                        *(q++) = max2 - OFstatic_cast(T2, *(p++)) * gradient2;
                                } else {
                                    for (i = Count; i != 0; --i)                            // expand depth
                                        *(q++) = OFstatic_cast(T2, *(p++)) * gradient2;
                                }
                            } else {
                                /* invert output data */
                                if (inverse)
                                {
                                    for (i = Count; i != 0; --i)                            // expand depth & invert
                                        *(q++) = max2 - OFstatic_cast(T2, OFstatic_cast(double, *(p++)) * gradient1);
                                } else {
                                    for (i = Count; i != 0; --i)                            // expand depth
                                        *(q++) = OFstatic_cast(T2, OFstatic_cast(double, *(p++)) * gradient1);
                                }
                            }
                            if (Count < FrameSize)
                            {
                                OFBitmanipTemplate<T2>::zeroMem(q, FrameSize - Count);  // set remaining pixels of frame to zero
                                q += (FrameSize - Count);
                            }
                        }                                                       // ... to be enhanced !
                    }
                    else /* bits1 > bits2 */
                    {
                        const int shift = bits1 - bits2;
                        for (int j = 0; j < 3; ++j)
                        {
                            p = pixel[j] + start;
                            /* invert output data */
                            if (inverse)
                            {
                                for (i = Count; i != 0; --i)                            // reduce depth & invert
                                    *(q++) = max2 - OFstatic_cast(T2, *(p++) >> shift);
                            } else {
                                for (i = Count; i != 0; --i)                            // reduce depth
                                    *(q++) = OFstatic_cast(T2, *(p++) >> shift);
                            }
                            if (Count < FrameSize)
                            {
                                OFBitmanipTemplate<T2>::zeroMem(q, FrameSize - Count);  // set remaining pixels of frame to zero
                                q += (FrameSize - Count);
                            }
                        }
                    }
                }
                else /* not planar */
                {
                    int j;
                    if (bits1 == bits2)
                    {
                        /* invert output data */
                        if (inverse)
                        {
                            for (i = start; i < start + Count; ++i)
                                for (j = 0; j < 3; ++j)                         // copy inverted data
                                    *(q++) = max2 - OFstatic_cast(T2, pixel[j][i]);
                        } else {
                            for (i = start; i < start + Count; ++i)
                                for (j = 0; j < 3; ++j)                         // copy
                                    *(q++) = OFstatic_cast(T2, pixel[j][i]);
                        }
                    }
                    else if (bits1 < bits2)                                     // optimization possible using LUT
                    {
                        const double gradient1 = OFstatic_cast(double, DicomImageClass::maxval(bits2)) /
                                                 OFstatic_cast(double, DicomImageClass::maxval(bits1));
                        const T2 gradient2 = OFstatic_cast(T2, gradient1);
                        if (gradient1 == OFstatic_cast(double, gradient2))      // integer multiplication?
                        {
                            /* invert output data */
                            if (inverse)
                            {
                                for (i = start; i < start + Count; ++i)                 // expand depth & invert
                                    for (j = 0; j < 3; ++j)
                                        *(q++) = max2 - OFstatic_cast(T2, pixel[j][i]) * gradient2;
                            } else {
                                for (i = start; i < start + Count; ++i)
                                    for (j = 0; j < 3; ++j)                             // expand depth
                                        *(q++) = OFstatic_cast(T2, pixel[j][i]) * gradient2;
                            }
                        } else {
                            /* invert output data */
                            if (inverse)
                            {
                                for (i = start; i < start + Count; ++i)
                                    for (j = 0; j < 3; ++j)                             // expand depth & invert
                                        *(q++) = max2 - OFstatic_cast(T2, OFstatic_cast(double, pixel[j][i]) * gradient1);
                            } else {
                                for (i = start; i < start + Count; ++i)
                                    for (j = 0; j < 3; ++j)                             // expand depth
                                        *(q++) = OFstatic_cast(T2, OFstatic_cast(double, pixel[j][i]) * gradient1);
                            }
                        }
                    }
                    else /* bits1 > bits2 */
                    {
                        const int shift = bits1 - bits2;
                        /* invert output data */
                        if (inverse)
                        {
                            for (i = start; i < start + Count; ++i)
                                for (j = 0; j < 3; ++j)                                 // reduce depth & invert
                                    *(q++) = max2 - OFstatic_cast(T2, pixel[j][i] >> shift);
                        } else {
                            for (i = start; i < start + Count; ++i)
                                for (j = 0; j < 3; ++j)                                 // reduce depth
                                    *(q++) = OFstatic_cast(T2, pixel[j][i] >> shift);
                        }
                    }
                    if (Count < FrameSize)
                        OFBitmanipTemplate<T2>::zeroMem(q, 3 * (FrameSize - Count));  // set remaining pixels of frame to zero
                }
            }
        } else
            Data = NULL;
    }

    /// flag indicating whether the output data buffer should be deleted in the destructor
    int DeleteData;
    /// flag indicating whether pixel data is stored color-by-pixel or color-by-plane
    int isPlanar;

 // --- declarations to avoid compiler warnings

    DiColorOutputPixelTemplate(const DiColorOutputPixelTemplate<T1,T2> &);
    DiColorOutputPixelTemplate<T1,T2> &operator=(const DiColorOutputPixelTemplate<T1,T2> &);
};


#endif
