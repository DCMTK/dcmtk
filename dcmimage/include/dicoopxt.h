/*
 *
 *  Copyright (C) 1996-99, OFFIS
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
 *  Update Date:      $Date: 1999-07-23 13:20:45 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicoopxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.12 $
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
                               const unsigned long frames,
                               const int bits,
                               const Sint16 shift,
                               const int planar)
      : DiColorOutputPixel(pixel, count, frame),
        Data(NULL),
        DeleteData(buffer == NULL),
        isPlanar(planar)
    {
        if ((pixel != NULL) && (Count > 0) && (FrameSize >= Count))
        {
            Data = (T2 *)buffer;
            convert((const T1 **)(pixel->getData()), frame * FrameSize, bits, shift, planar);
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
    
    inline int writePPM(ostream &stream) const
    {
        if (Data != NULL)
        {
            register T2 *p = Data;
            register unsigned long i;
            register int j;
            for (i = 0; i < FrameSize; i++)
                for (j = 0; j < 3; j++)
                    stream << (unsigned long)*(p++) << " ";     // typecast to resolve problems with 'char'
            return 1;
        }
        return 0;
    }

    inline int writePPM(FILE *stream) const
    {
        if (Data != NULL)
        {
            register T2 *p = Data;
            register unsigned long i;
            register int j;
            for (i = 0; i < FrameSize; i++)
                for (j = 0; j < 3; j++)
                    fprintf(stream, "%lu ", (unsigned long)*(p++));
            return 1;
        }
        return 0;
    }


 protected:

    T2 *Data;


 private:

    inline void convert(const T1 *pixel[3],
                        const unsigned long start,
                        const int bits,
                        /*const*/ Sint16 shift,
                        const int planar)
    {
        if ((pixel[0] != NULL) && (pixel[1] != NULL) && (pixel[2] != NULL))
        {
            if (Data == NULL)
                Data = new T2[FrameSize * 3];
            if (Data != NULL)
            {
                register T2 *q = Data;
                register unsigned long i;
                if (planar)
                {
                    register const T1 *p;
                    if (shift == 0)
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            p = pixel[j] + start;
                            for (i = 0; i < Count; i++)
                                *(q++) = (T2)*(p++);                            // copy
                            if (Count < FrameSize)
                            {
                                OFBitmanipTemplate<T2>::zeroMem(q, FrameSize - Count);  // set remaining pixels of frame to zero
                                q += (FrameSize - Count);
                            }
                        }
                    }
                    else if (shift < 0)
                    {
/*                        
                        T2 *lut = NULL;
//                        if (initOptimizationLUT(lut, ocnt))
                        {                                                       // use LUT for optimization
                        
                        }                        
                        if (lut == NULL)                                        // use "normal" transformation
                        {
                            register T2 pv;
                            register T2 qv;
                            register int b;
                            for (int j = 0; j < 3; j++)
                            {
                                p = pixel[j] + start;
                                for (i = 0; i < Count; i++)
                                {                                               // expand depth: 1001 -> 10011001 !!
                                    b = -shift;
                                    pv = qv = (T2)(*(p++));
                                    while (b >= bits)
                                    {
                                        qv = (qv << bits) | pv;
                                        b -= bits;
                                    }
                                    if (b > 0)
                                        *(q++) = (qv << b) | (pv >> (bits - b));
                                    else
                                        *(q++) = qv;
                                }
                            }
                        }
                        delete[] lut;
*/
                        shift = -shift;
                        for (int j = 0; j < 3; j++)
                        {
                            p = pixel[j] + start;
                            for (i = 0; i < Count; i++)
                                *(q++) = (T2)(*(p++) << shift);                 // expand depth: simple left shift is not correct !!
                            if (Count < FrameSize)
                            {
                                OFBitmanipTemplate<T2>::zeroMem(q, FrameSize - Count);  // set remaining pixels of frame to zero
                                q += (FrameSize - Count);
                            }
                        }                                                       // ... to be enhanced !
                    }
                    else /* shift > 0 */
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            p = pixel[j] + start;
                            for (i = 0; i < Count; i++)
                                *(q++) = (T2)(*(p++) >> shift);                 // reduce depth: correct ?
                            if (Count < FrameSize)
                            {
                                OFBitmanipTemplate<T2>::zeroMem(q, FrameSize - Count);  // set remaining pixels of frame to zero
                                q += (FrameSize - Count);
                            }
                        }
                    }
                }
                else
                {
                    register int j;
                    if (shift == 0)
                    {
                        for (i = start; i < start + Count; i++)
                            for (j = 0; j < 3; j++)
                                *(q++) = (T2)pixel[j][i];                       // copy
                    }
                    else if (shift < 0)
                    {
                        shift = -shift;
                        for (i = start; i < start + Count; i++)
                            for (j = 0; j < 3; j++)
                                *(q++) = (T2)(pixel[j][i] << shift);            // expand depth: simple left shift is not correct !!
                    }                                                           // ... to be enhanced !
                    else /* shift > 0 */
                    {
                        for (i = start; i < start + Count; i++)
                            for (j = 0; j < 3; j++)
                                *(q++) = (T2)(pixel[j][i] >> shift);            // reduce depth: correct ?
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
 * Revision 1.12  1999-07-23 13:20:45  joergr
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
