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
 *  Purpose: DicomInputPixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-03-24 17:20:03 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diinpxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIINPXT_H
#define __DIINPXT_H

#include "osconfig.h"
#include "dctypes.h"
#include "dcpixel.h"
#include "ofbmanip.h"

#include "diinpx.h"
#include "dipxrept.h"
#include "diutils.h"


/********************************************************************/


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


/********************************************************************/


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

    DiInputPixelTemplate(/*const*/ DcmPixelData *pixel,
                         const Uint16 alloc,
                         const Uint16 stored,
                         const Uint16 high,
                         const unsigned long start,
                         const unsigned long count)
      : DiInputPixel(stored),
        Data(NULL),
        MinValue(0),
        MaxValue(0)
    {
        if (isSigned())
        {
            AbsMinimum = -(double)DicomImageClass::maxval(Bits - 1, 0);
            AbsMaximum = (double)DicomImageClass::maxval(Bits - 1);
        } else {
            AbsMinimum = 0;
            AbsMaximum = (double)DicomImageClass::maxval(Bits);
        }
        if (pixel != NULL)
            convert(pixel, alloc, stored, high, start, count);
    }

    virtual ~DiInputPixelTemplate()
    {
        delete[] Data;
    }

    int determineMinMax()
    {
        if (Data != NULL)
        {
            register T2 *p = Data;
            register T2 value = *p;
            register unsigned long i;
            MinValue = value;
            MaxValue = value;
            for (i = 1; i < Count; i++)
            {
                value = *(++p);
                if (value < MinValue)
                    MinValue = value;
                if (value > MaxValue)
                    MaxValue = value;
            }
            return 1;
        }
        return 0;
    }

    inline EP_Representation getRepresentation() const
    {
        return DiPixelRepresentationTemplate<T2>::getRepresentation();
    }

    inline void *getData() const
    {
        return (void *)Data;
    }
    
    inline double getMinValue() const
    {
        return (double)MinValue;
    }

    inline double getMaxValue() const
    {
        return (double)MaxValue;
    }


 private:

    void convert(/*const*/ DcmPixelData *PixelData,
                 const Uint16 BitsAllocated,
                 const Uint16 BitsStored,
                 const Uint16 HighBit,
#ifdef DEBUG
                 const unsigned long start,
                 const unsigned long count)
#else
                 const unsigned long /*start*/,
                 const unsigned long /*count*/)
#endif
    {
        const Uint16 bitsof_T1 = bitsof(T1);
        const Uint16 bitsof_T2 = bitsof(T2);
        T1 *pixel;
        const Uint32 length_Bytes = getPixelData(PixelData, pixel);
        const Uint32 length_T1 = length_Bytes / sizeof(T1);
        Count = ((length_Bytes * 8) + BitsAllocated - 1) / BitsAllocated;
        register unsigned long i;
#ifdef DEBUG
        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
            cerr << start << " " << count << endl;
#endif
        Data = new T2[Count];
        if (Data != NULL)
        {
#ifdef DEBUG
            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                cerr << BitsAllocated << " " << BitsStored << " " << HighBit << " " << isSigned() << endl;
#endif
            register const T1 *p = pixel;
            register T2 *q = Data;
            if (bitsof_T1 == BitsAllocated)                                             // case 1: equal 8/16 bit
            {
                if (BitsStored == BitsAllocated)
                {
                    if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                        cerr << "convert PixelData: case 1a (single copy)" << endl;
                    for (i = 0; i < Count; i++)
                        *(q++) = (T2)*(p++);
                }
                else /* BitsStored < BitsAllocated */
                {
                    register T1 mask = 0;
                    for (i = 0; i < BitsStored; i++)
                        mask |= (T1)(1 << i);
                    const T2 sign = 1 << (BitsStored - 1);
                    T2 smask = 0;
                    for (i = BitsStored; i < bitsof_T2; i++)
                        smask |= (T2)(1 << i);
                    const Uint16 shift = HighBit + 1 - BitsStored;
                    if (shift == 0)
                    {
                        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                            cerr << "convert PixelData: case 1b (mask & sign)" << endl;
                        for (i = 0; i < length_T1; i++)
                            *(q++) = expandSign((T2)(*(p++) & mask), sign, smask);
                    }
                    else /* shift > 0 */
                    {
                        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                            cerr << "convert PixelData: case 1c (shift & mask & sign)" << endl;
                        for (i = 0; i < length_T1; i++)
                            *(q++) = expandSign((T2)((*(p++) >> shift) & mask), sign, smask);
                    }
                }
            }
            else if ((bitsof_T1 > BitsAllocated) && (bitsof_T1 % BitsAllocated == 0))   // case 2: divisor of 8/16 bit
            {
                const Uint16 times = bitsof_T1 / BitsAllocated;
                register T1 mask = 0;
                for (i = 0; i < BitsStored; i++)
                    mask |= (T1)(1 << i);
                register Uint16 j;
                register T1 value;
                if ((BitsStored == BitsAllocated) && (BitsStored == bitsof_T2))
                {
                    if (times == 2)
                    {
                        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                            cerr << "convert PixelData: case 2a (simple mask)" << endl;
                        for (i = 0; i < length_T1; i++, p++)
                        {
                            *(q++) = (T2)(*p & mask);
                            *(q++) = (T2)(*p >> BitsAllocated);
                        }
                    }   
                    else
                    {
                        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                            cerr << "convert PixelData: case 2b (mask)" << endl;
                        for (i = 0; i < length_T1; i++)
                        {
                            value = *(p++);
                            for (j = 0; j < times; j++)
                            {
                                *(q++) = (T2)(value & mask);
                                value >>= BitsAllocated;
                            }
                        }
                    }   
                }
                else
                {
                    if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                        cerr << "convert PixelData: case 2c (shift & mask & sign)" << endl;
                    const T2 sign = 1 << (BitsStored - 1);
                    T2 smask = 0;
                    for (i = BitsStored; i < bitsof_T2; i++)
                        smask |= (T2)(1 << i);
                    const Uint16 shift = HighBit + 1 - BitsStored;
                    for (i = 0; i < length_T1; i++)
                    {
                        value = *(p++) >> shift;
                        for (j = 0; j < times; j++)
                        {
                            *(q++) = expandSign((T2)(value & mask), sign, smask);
                            value >>= BitsAllocated;
                        }   
                    }
                }
            }
            else if ((bitsof_T1 < BitsAllocated) && (BitsAllocated % bitsof_T1 == 0)    // case 3: multiplicant of 8/16
                && (BitsStored == BitsAllocated))
            {
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                    cerr << "convert PixelData: case 3 (multi copy)" << endl;
                const Uint16 times = BitsAllocated / bitsof_T1;
                register Uint16 j;
                register Uint16 shift;
                register T2 value;
                for (i = 0; i < length_T1; i++)
                {
                    shift = 0;
                    value = (T2)*(p++);
                    for (j = 1; j < times; j++, i++)
                    {
                        shift += bitsof_T1;
                        value |= (T2)*(p++) << shift;
                    }
                    *(q++) = value; 
                }
            }
            else                                                                        // case 4: anything else
            {
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                    cerr << "convert PixelData: case 4 (general)" << endl;
                register T2 value = 0;
                register Uint16 bits = 0;
                register Uint32 skip = HighBit + 1 - BitsStored;
                register Uint32 times;
                T1 mask[bitsof_T1];
                mask[0] = 1;
                for (i = 1; i < bitsof_T1; i++)
                    mask[i] = (mask[i - 1] << 1) | 1;
                T2 smask = 0;
                for (i = BitsStored; i < bitsof_T2; i++)
                smask |= (T2)(1 << i);
                const T2 sign = 1 << (BitsStored - 1);
                const Uint32 gap = BitsAllocated - BitsStored;
                i = 0;
                while (i < length_T1)
                {
                    if (skip < bitsof_T1)
                    {
                        if (skip + BitsStored - bits < bitsof_T1)       // -++- --++
                        {
                            value |= ((T2)((*p >> skip) & mask[BitsStored - bits - 1]) << bits);
                            skip += BitsStored - bits + gap;
                            bits = BitsStored;
                        }
                        else                                            // ++-- ++++
                        {
                            value |= ((T2)((*p >> skip) & mask[bitsof_T1 - skip - 1]) << bits);
                            bits += bitsof_T1 - (Uint16)skip;
                            skip = (bits == BitsStored) ? gap : 0;
                            i++;
                            p++;
                        }
                        if (bits == BitsStored)
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
    }

    T2 *Data;
    T2 MinValue;
    T2 MaxValue;

 // --- declarations to avoid compiler warnings
 
    DiInputPixelTemplate(const DiInputPixelTemplate &);
    DiInputPixelTemplate &operator=(const DiInputPixelTemplate &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diinpxt.h,v $
 * Revision 1.9  1999-03-24 17:20:03  joergr
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
