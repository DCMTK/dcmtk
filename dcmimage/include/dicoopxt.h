/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorOutputPixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-06-26 16:17:16 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicoopxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICOOPXT_H
#define __DICOOPXT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dicoopx.h"
#include "dicopx.h"
#include "dipxrept.h"
#include "diutils.h"


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
            Data = (T2 *)buffer;
            convert((const T1 **)(pixel->getData()), frame * FrameSize, bits1, bits2, planar, inverse);
        }
    }

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
            Data = (T2 *)buffer;
    }

    virtual ~DiColorOutputPixelTemplate()
    {
        if (DeleteData)
            delete[] Data;
    }

    inline EP_Representation getRepresentation() const
    {
        return DiPixelRepresentationTemplate<T2>::getRepresentation();
    }

    inline size_t getItemSize() const
    {
        return sizeof(T2) * 3;
    }

    inline void *getData() const
    {
        return (void *)Data;
    }

    inline void *getPlane(const int plane) const
    {
        void *result = NULL;
        if (Data != NULL)
        {
            if (plane <= 0)
                result = (void *)Data;
            else
            {
                if (isPlanar)
                    result = (void *)(Data + ((plane == 1) ? 1 : 2) * FrameSize);
                else
                    result = (void *)(Data + ((plane == 1) ? 1 : 2));
            }
        }
        return result;
    }

    int writePPM(ostream &stream) const
    {
        if (Data != NULL)
        {
            register T2 *p = Data;
            register unsigned long i;
            register int j;
            for (i = FrameSize; i != 0; i--)
                for (j = 3; j != 0; j--)
                    stream << (unsigned long)*(p++) << " ";     // typecast to resolve problems with 'char'
            return 1;
        }
        return 0;
    }

    int writePPM(FILE *stream) const
    {
        if (Data != NULL)
        {
            register T2 *p = Data;
            register unsigned long i;
            register int j;
            for (i = FrameSize; i != 0; i--)
                for (j = 3; j != 0; j--)
                    fprintf(stream, "%lu ", (unsigned long)*(p++));
            return 1;
        }
        return 0;
    }


 protected:

    T2 *Data;


 private:

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
                register T2 *q = Data;
                register unsigned long i;
                const T2 max2 = DicomImageClass::maxval(bits2);
                if (planar)
                {
                    register const T1 *p;
                    if (bits1 == bits2)
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            p = pixel[j] + start;
                            /* invert output data */
                            if (inverse)
                            {
                                for (i = Count; i != 0; i--)
                                    *(q++) = max2 - (T2)*(p++);                     // copy inverted data
                            } else {
                                for (i = Count; i != 0; i--)
                                    *(q++) = (T2)*(p++);                            // copy
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
                        const double gradient1 = (double)DicomImageClass::maxval(bits2) / (double)DicomImageClass::maxval(bits1);
                        const T2 gradient2 = (T2)gradient1;
                        for (int j = 0; j < 3; j++)
                        {
                            p = pixel[j] + start;
                            if (gradient1 == (double)gradient2)                 // integer multiplication?
                            {
                                /* invert output data */
                                if (inverse)
                                {
                                    for (i = Count; i != 0; i--)
                                        *(q++) = max2 - (T2)(*(p++)) * gradient2;   // expand depth & invert
                                } else {
                                    for (i = Count; i != 0; i--)
                                        *(q++) = (T2)(*(p++)) * gradient2;          // expand depth
                                }
                            } else {
                                /* invert output data */
                                if (inverse)
                                {
                                    for (i = Count; i != 0; i--)
                                        *(q++) = max2 - (T2)((double)(*(p++)) * gradient1); // expand depth & invert
                                } else {
                                    for (i = Count; i != 0; i--)
                                        *(q++) = (T2)((double)(*(p++)) * gradient1);        // expand depth
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
                        for (int j = 0; j < 3; j++)
                        {
                            p = pixel[j] + start;
                            /* invert output data */
                            if (inverse)
                            {
                                for (i = Count; i != 0; i--)
                                    *(q++) = max2 - (T2)(*(p++) >> shift);          // reduce depth & invert
                            } else {
                                for (i = Count; i != 0; i--)
                                    *(q++) = (T2)(*(p++) >> shift);                 // reduce depth
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
                            for (i = start; i < start + Count; i++)
                                for (j = 0; j < 3; j++)
                                    *(q++) = max2 - (T2)pixel[j][i];            // copy inverted data
                        } else {
                            for (i = start; i < start + Count; i++)
                                for (j = 0; j < 3; j++)
                                    *(q++) = (T2)pixel[j][i];                   // copy
                        }
                    }
                    else if (bits1 < bits2)                                     // optimization possible using LUT
                    {
                        const double gradient1 = (double)DicomImageClass::maxval(bits2) / (double)DicomImageClass::maxval(bits1);
                        const T2 gradient2 = (T2)gradient1;
                        if (gradient1 == (double)gradient2)                     // integer multiplication?
                        {
                            /* invert output data */
                            if (inverse)
                            {
                                for (i = start; i < start + Count; i++)
                                    for (j = 0; j < 3; j++)
                                        *(q++) = max2 - (T2)(pixel[j][i]) * gradient2;  // expand depth & invert
                            } else {
                                for (i = start; i < start + Count; i++)
                                    for (j = 0; j < 3; j++)
                                        *(q++) = (T2)(pixel[j][i]) * gradient2;         // expand depth
                            }
                        } else {
                            /* invert output data */
                            if (inverse)
                            {
                                for (i = start; i < start + Count; i++)
                                    for (j = 0; j < 3; j++)
                                        *(q++) = max2 - (T2)((double)pixel[j][i] * gradient1);  // expand depth & invert
                            } else {
                                for (i = start; i < start + Count; i++)
                                    for (j = 0; j < 3; j++)
                                        *(q++) = (T2)((double)pixel[j][i] * gradient1);         // expand depth
                            }
                        }
                    }
                    else /* bits1 > bits2 */
                    {
                        const int shift = bits1 - bits2;
                        /* invert output data */
                        if (inverse)
                        {
                            for (i = start; i < start + Count; i++)
                                for (j = 0; j < 3; j++)
                                    *(q++) = max2 - (T2)(pixel[j][i] >> shift);     // reduce depth & invert
                        } else {
                            for (i = start; i < start + Count; i++)
                                for (j = 0; j < 3; j++)
                                    *(q++) = (T2)(pixel[j][i] >> shift);            // reduce depth
                        }
                    }
                    if (Count < FrameSize)
                        OFBitmanipTemplate<T2>::zeroMem(q, 3 * (FrameSize - Count));  // set remaining pixels of frame to zero
                }
            }
        } else
            Data = NULL;
    }

    int DeleteData;
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
 * Revision 1.19  2002-06-26 16:17:16  joergr
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
