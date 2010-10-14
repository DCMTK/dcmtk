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
 *  Purpose: DicomColorOutputPixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:29 $
 *  CVS/RCS Revision: $Revision: 1.28 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
     ** @return pointer to beginning of plane if sucessful, NULL otherwise
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
            register T2 *p = Data;
            register unsigned long i;
            register int j;
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
            register T2 *p = Data;
            register unsigned long i;
            register int j;
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
                register T2 *q = Data;
                register unsigned long i;
                const T2 max2 = OFstatic_cast(T2, DicomImageClass::maxval(bits2));
                if (planar)
                {
                    register const T1 *p;
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
                    register int j;
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


/*
 *
 * CVS/RCS Log:
 * $Log: dicoopxt.h,v $
 * Revision 1.28  2010-10-14 13:16:29  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.27  2010-10-05 15:34:41  joergr
 * Output information on conversion process to the logger (debug mode).
 * Fixed various Doxygen API documentation issues.
 *
 * Revision 1.26  2010-03-01 09:08:46  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.25  2006-08-15 16:35:01  meichel
 * Updated the code in module dcmimage to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.24  2005/12/08 16:01:33  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.23  2004/02/06 11:18:18  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.22  2003/12/23 16:06:21  joergr
 * Replaced additional post-increment/decrement operators by pre-increment/
 * decrement operators.
 *
 * Revision 1.21  2003/12/23 11:36:24  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.20  2002/06/26 17:20:31  joergr
 * Added type cast to keep MSVC6 quiet.
 *
 * Revision 1.19  2002/06/26 16:17:16  joergr
 * Added support for polarity flag to color images.
 *
 * Revision 1.18  2001/11/09 16:42:04  joergr
 * Removed 'inline' specifier from certain methods.
 *
 * Revision 1.17  2001/06/01 15:49:29  meichel
 * Updated copyright header
 *
 * Revision 1.16  2000/03/30 14:15:44  joergr
 * Corrected wrong bit expansion of output pixel data (left shift is not
 * correct).
 *
 * Revision 1.15  2000/03/08 16:21:50  meichel
 * Updated copyright header.
 *
 * Revision 1.14  1999/09/17 14:03:43  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.13  1999/08/17 10:28:47  joergr
 * Commented unused parameter name to avoid compiler warnings.
 * Removed unused parameter.
 *
 * Revision 1.12  1999/07/23 13:20:45  joergr
 * Enhanced handling of corrupted pixel data (wrong length).
 *
 * Revision 1.11  1999/04/30 16:12:03  meichel
 * Minor code purifications to keep IBM xlC quiet
 *
 * Revision 1.10  1999/04/29 09:31:13  joergr
 * Moved color related image files back to non-public part.
 *
 * Revision 1.1  1999/04/28 14:57:31  joergr
 * Moved files from dcmimage module to dcmimgle to support new pastel color
 * output format.
 *
 * Revision 1.8  1999/01/20 14:43:12  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 * Added new output method to fill external memory buffer with rendered pixel
 * data.
 *
 * Revision 1.7  1998/12/22 13:23:57  joergr
 * Added comments that the routines for expanding pixel's depth have to be
 * enhanced in the future (replicate bit pattern instead of shifting). Same
 * question for reducing depth.
 *
 * Revision 1.6  1998/11/27 13:46:00  joergr
 * Added copyright message. Replaced delete by delete[] for array types.
 *
 * Revision 1.5  1998/07/01 08:39:19  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.4  1998/05/11 14:53:12  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
