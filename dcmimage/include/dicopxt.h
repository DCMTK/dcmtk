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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorPixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-09-12 14:10:37 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicopxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICOPXT_H
#define __DICOPXT_H

#include "osconfig.h"
#include "dctypes.h"
#include "ofbmanip.h"

#include "dicopx.h"
#include "dipxrept.h"


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
    return (Uint8)((Sint16)value + (Sint16)offset + 1);
}


inline Uint16 removeSign(const Sint16 value, const Sint16 offset)
{
    return (Uint16)((Sint32)value + (Sint32)offset + 1);
}

/*
inline Uint32 removeSign(const Sint32 value, const Sint32 offset)
{
    return (value < 0) ? (Uint32)(value + offset + 1) : (Uint32)value + (Uint32)offset + 1;
}


inline Uint8 removeSign(const Sint8 value, const Uint8 mask)
{
    return (Uint8)value ^ mask;
}


inline Uint16 removeSign(const Sint16 value, const Uint16 mask)
{
    return (Uint16)value ^ mask;
}
*/

inline Uint32 removeSign(const Sint32 value, const Uint32 mask)
{
    return (Uint32)value ^ mask;
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

    virtual ~DiColorPixelTemplate()
    {
        delete[] Data[0];
        delete[] Data[1];
        delete[] Data[2];
    }

    inline EP_Representation getRepresentation() const
    {
        return DiPixelRepresentationTemplate<T>::getRepresentation();
    }

    inline void *getData() const
    {
        return (void *)Data;
    }

    inline void *getDataPtr()
    {
        return (void *)Data;
    }

    OFBool getPixelData(void *data,
                        const size_t count) const
    {
        OFBool result = OFFalse;
        /* check parameters and internal data */
        if ((data != NULL) && (count >= Count * 3) &&
            (Data[0] != NULL) && (Data[1] != NULL) && (Data[2] != NULL))
        {
            /* copy all three planes to the given memory block */
            OFBitmanipTemplate<T>::copyMem(Data[0], (T *)data, Count);
            OFBitmanipTemplate<T>::copyMem(Data[1], (T *)data + Count, Count);
            OFBitmanipTemplate<T>::copyMem(Data[2], (T *)data + 2 * Count, Count);
            result = OFTrue;
        }
        return result;
    }

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
            const unsigned long count = (unsigned long)width * (unsigned long)height;
            const unsigned long start = count * frame + ((upsideDown) ? (unsigned long)(height - 1) * (unsigned long)width : 0);
            const signed long nextRow = (upsideDown) ? -2 * (signed long)width : 0;
            register const T *r = Data[0] + start;
            register const T *g = Data[1] + start;
            register const T *b = Data[2] + start;
            register Uint16 x;
            register Uint16 y;
            if (mode == 24)     // 24 bits per pixel
            {
                const unsigned long wid3 = (unsigned long)width * 3;
                // each line has to start at 32-bit-address, if 'padding' is true
                const int gap = (padding) ? (int)((4 - wid3 & 0x3) & 0x3) : 0;
                unsigned long fsize = (wid3 + gap) * (unsigned long)height;
                if ((data == NULL) || (size >= fsize))
                {
                    if (data == NULL)
                        data = new Uint8[fsize];
                    if (data != NULL)
                    {
                        register Uint8 *q = (Uint8 *)data;
                        if (fromBits == toBits)
                        {
                            /* copy pixel data as is */
                            for (y = height; y != 0; y--)
                            {
                                for (x = width; x != 0; x--)
                                {
                                    /* reverse sample order: B-G-R */
                                    *(q++) = (Uint8)(*(b++));
                                    *(q++) = (Uint8)(*(g++));
                                    *(q++) = (Uint8)(*(r++));
                                }
                                r += nextRow; g += nextRow; b += nextRow;           // go backwards if 'upsideDown'
                                q += gap;                                           // new line: jump to next 32-bit address
                            }
                        }
                        else if (fromBits < toBits)
                        {
                            /* increase color depth: multiply with factor */
                            const double gradient1 = (double)DicomImageClass::maxval(toBits) / (double)DicomImageClass::maxval(fromBits);
                            const Uint8 gradient2 = (Uint8)gradient1;
                            if (gradient1 == (double)gradient2)                     // integer multiplication?
                            {
                                for (y = height; y != 0; y--)
                                {
                                    for (x = width; x != 0; x--)
                                    {
                                        /* reverse sample order: B-G-R */
                                        *(q++) = (Uint8)(*(b++) * gradient2);
                                        *(q++) = (Uint8)(*(g++) * gradient2);
                                        *(q++) = (Uint8)(*(r++) * gradient2);
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
                                        *(q++) = (Uint8)((double)(*(b++)) * gradient1);
                                        *(q++) = (Uint8)((double)(*(g++)) * gradient1);
                                        *(q++) = (Uint8)((double)(*(r++)) * gradient1);
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
                                    *(q++) = (Uint8)(*(b++) >> shift);
                                    *(q++) = (Uint8)(*(g++) >> shift);
                                    *(q++) = (Uint8)(*(r++) >> shift);
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
                        register Uint32 *q = (Uint32 *)data;
                        if (fromBits == toBits)
                        {
                            /* copy pixel data as is */
                            for (y = height; y != 0; y--)
                            {
                                for (x = width; x != 0; x--)
                                {
                                    /* reverse sample order: B-G-R-0 */
                                    *(q++) = (((Uint32)(*(b++))) << 24) |
                                             (((Uint32)(*(g++))) << 16) |
                                             (((Uint32)(*(r++))) << 8);
                                }
                                r += nextRow; g += nextRow; b += nextRow;           // go backwards if 'upsideDown'
                            }
                        }
                        else if (fromBits < toBits)
                        {
                            /* increase color depth: multiply with factor */
                            const double gradient1 = (double)DicomImageClass::maxval(toBits) / (double)DicomImageClass::maxval(fromBits);
                            const Uint32 gradient2 = (Uint32)gradient1;
                            if (gradient1 == (double)gradient2)                     // integer multiplication?
                            {
                                for (y = height; y != 0; y--)
                                {
                                    for (x = width; x != 0; x--)
                                    {
                                        /* reverse sample order: B-G-R-0 */
                                        *(q++) = (((Uint32)(*(b++) * gradient2)) << 24) |
                                                 (((Uint32)(*(g++) * gradient2)) << 16) |
                                                 (((Uint32)(*(r++) * gradient2)) << 8);
                                    }
                                    r += nextRow; g += nextRow; b += nextRow;       // go backwards if 'upsideDown'
                                }
                            } else {
                                for (y = height; y != 0; y--)
                                {
                                    for (x = width; x != 0; x--)
                                    {
                                        /* reverse sample order: B-G-R-0 */
                                        *(q++) = (((Uint32)((double)(*(b++)) * gradient1)) << 24) |
                                                 (((Uint32)((double)(*(g++)) * gradient1)) << 16) |
                                                 (((Uint32)((double)(*(r++)) * gradient1)) << 8);
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
                                    /* reverse sample order: B-G-R-0 */
                                    *(q++) = (((Uint32)(*(b++) >> shift)) << 24) |
                                             (((Uint32)(*(g++) >> shift)) << 16) |
                                             (((Uint32)(*(r++) >> shift)) << 8);
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
            const unsigned long count = (unsigned long)width * (unsigned long)height;
            Uint32 *data = new Uint32[count];
            if (data != NULL)
            {
                const unsigned long start = count * frame;
                register const T *r = Data[0] + start;
                register const T *g = Data[1] + start;
                register const T *b = Data[2] + start;
                register Uint32 *q = data;
                register unsigned long i;
                if (fromBits == toBits)
                {
                    /* copy pixel data as is */
                    for (i = count; i != 0; i--)
                    {
                        /* sample order: R-G-B */
                        *(q++) = (((Uint32)(*(r++))) << 24) |
                                 (((Uint32)(*(g++))) << 16) |
                                 (((Uint32)(*(b++))) << 8);
                    }
                }
                else if (fromBits < toBits)
                {
                    /* increase color depth: multiply with factor */
                    const double gradient1 = (double)DicomImageClass::maxval(toBits) / (double)DicomImageClass::maxval(fromBits);
                    const Uint32 gradient2 = (Uint32)gradient1;
                    if (gradient1 == (double)gradient2)                     // integer multiplication?
                    {
                        for (i = count; i != 0; i--)
                        {
                            /* sample order: R-G-B */
                            *(q++) = (((Uint32)(*(r++) * gradient2)) << 24) |
                                     (((Uint32)(*(g++) * gradient2)) << 16) |
                                     (((Uint32)(*(b++) * gradient2)) << 8);
                        }
                    } else {
                        for (i = count; i != 0; i--)
                        {
                            /* sample order: R-G-B */
                            *(q++) = (((Uint32)((double)(*(r++)) * gradient1)) << 24) |
                                     (((Uint32)((double)(*(g++)) * gradient1)) << 16) |
                                     (((Uint32)((double)(*(b++)) * gradient1)) << 8);
                        }
                    }
                }
                else /* fromBits > toBits */
                {
                    /* reduce color depth: right shift */
                    const int shift = fromBits - toBits;
                    for (i = count; i != 0; i--)
                    {
                        /* sample order: R-G-B */
                        *(q++) = (((Uint32)(*(r++) >> shift)) << 24) |
                                 (((Uint32)(*(g++) >> shift)) << 16) |
                                 (((Uint32)(*(b++) >> shift)) << 8);
                    }
                }
                bytes = count * 4;
            }
        }
        return bytes;
    }


 protected:

    DiColorPixelTemplate(const DiColorPixel *pixel,
                         const unsigned long count)
      : DiColorPixel(pixel, count)
    {
        Data[0] = NULL;
        Data[1] = NULL;
        Data[2] = NULL;
    }

    inline int Init(const void *pixel)
    {
        int result = 0;
        if (pixel != NULL)
        {
            result = 1;
            /* allocate data buffer for the 3 planes */
            for (int j = 0; j < 3; j++)
            {
                Data[j] = new T[Count];
                if (Data[j] != NULL)
                {
                    /* erase empty part of the buffer (=blacken the background) */
                    if (InputCount < Count)
                        OFBitmanipTemplate<T>::zeroMem(Data[j] + InputCount, Count - InputCount);
                } else
                    result = 0;     // at least one buffer could not be allocated!
            }
        }
        return result;
    }

    T *Data[3];
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicopxt.h,v $
 * Revision 1.18  2002-09-12 14:10:37  joergr
 * Replaced "createPixelData" by "getPixelData" which uses a new dcmdata
 * routine and is therefore more efficient.
 *
 * Revision 1.17  2002/08/29 12:57:49  joergr
 * Added method that creates pixel data in DICOM format.
 *
 * Revision 1.16  2002/06/26 16:17:41  joergr
 * Enhanced handling of corrupted pixel data and/or length.
 *
 * Revision 1.15  2002/01/29 17:07:08  joergr
 * Added optional flag to the "Windows DIB" methods allowing to switch off the
 * scanline padding.
 *
 * Revision 1.14  2001/12/11 14:23:44  joergr
 * Added type cast to keep old Sun compilers quiet.
 *
 * Revision 1.13  2001/11/09 16:44:35  joergr
 * Enhanced and renamed createTrueColorDIB() method.
 *
 * Revision 1.12  2001/06/01 15:49:29  meichel
 * Updated copyright header
 *
 * Revision 1.11  2000/03/08 16:21:51  meichel
 * Updated copyright header.
 *
 * Revision 1.10  1999/09/17 14:03:44  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.9  1999/07/23 13:22:29  joergr
 * emoved inline method 'removeSign'which is no longer needed.
 *
 * Revision 1.8  1999/04/28 12:51:58  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.7  1999/01/20 14:44:49  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.6  1998/11/27 13:50:20  joergr
 * Added copyright message. Replaced delete by delete[] for array types.
 * Added method to give direct (non const) access to internal data buffer.
 *
 * Revision 1.5  1998/05/11 14:53:13  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
