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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorPixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 13:50:20 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicopxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICOPXT_H
#define __DICOPXT_H

#include "osconfig.h"
#include "dctypes.h"

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


inline Uint32 removeSign(const Sint32 value, const Sint32 offset)
{
    return (value < 0) ? (Uint32)(value + offset + 1) : (Uint32)value + (Uint32)offset + 1;
}


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiColorPixelTemplate : public DiColorPixel, public DiPixelRepresentationTemplate<T>
{
 public:
    DiColorPixelTemplate(const DiDocument *docu, const DiInputPixel *pixel, const Uint16 samples,
        EI_Status &status, const Uint16 sample_rate = 0)
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
    
    inline void *createDIB(const Uint16 width, const Uint16 height, const unsigned long frame, Sint16 shift) const
    {
        if ((Data[0] != NULL) && (Data[1] != NULL) && (Data[2] != NULL))
        {
            const int gap = width & 0x3;                        // each line has to start at 32-bit-address
            Uint8 *data = new Uint8[(unsigned long)(width + gap) * (unsigned long)height * 3];
            if (data != NULL)
            {
                const unsigned long start = (unsigned long)width * (unsigned long)height * frame;
                register const T *r = Data[0] + start;
                register const T *g = Data[1] + start;
                register const T *b = Data[2] + start;
                register Uint8 *q = data;
                register Uint16 x;
                register Uint16 y;
                if (shift == 0)
                {
                    for (y = 0; y < height; y++)
                    {
                        for (x = 0; x < width; x++)
                        {
                            *(q++) = (Uint8)(*(b++));
                            *(q++) = (Uint8)(*(g++));
                            *(q++) = (Uint8)(*(r++));
                        }
                        q += gap;
                    }
                }
                else if (shift < 0)
                {
                    shift = -shift;
                    for (y = 0; y < height; y++)
                    {
                        for (x = 0; x < width; x++)
                        {
                            *(q++) = (Uint8)(*(b++) << shift);
                            *(q++) = (Uint8)(*(g++) << shift);
                            *(q++) = (Uint8)(*(r++) << shift);
                        }
                        q += gap;
                    }
                }
                else
                {
                    for (y = 0; y < height; y++)
                    {
                        for (x = 0; x < width; x++)
                        {
                            *(q++) = (Uint8)(*(b++) >> shift);
                            *(q++) = (Uint8)(*(g++) >> shift);
                            *(q++) = (Uint8)(*(r++) >> shift);
                        }
                        q += gap;
                    }
                }
                return (void *)data;
            }
        }
        return NULL;
    }

    inline void *createAWTBitmap(const Uint16 width, const Uint16 height, const unsigned long frame, Sint16 shift) const
    {
        if ((Data[0] != NULL) && (Data[1] != NULL) && (Data[2] != NULL))
        {
            Uint32 *data = new Uint32[(unsigned long)width * (unsigned long)height];
            if (data != NULL)
            {
                const unsigned long start = (unsigned long)width * (unsigned long)height * frame;
                register const T *r = Data[0] + start;
                register const T *g = Data[1] + start;
                register const T *b = Data[2] + start;
                register Uint32 *q = data;
                register Uint16 x;
                register Uint16 y;
                if (shift == 0)
                {
                    for (y = 0; y < height; y++)
                    {
                        for (x = 0; x < width; x++)
                            *(q++) = (((Uint32)(*(r++))) << 24) | (((Uint32)(*(g++))) << 16) | (((Uint32)(*(b++))) << 8);
                    }
                }
                else if (shift < 0)
                {
                    shift = -shift;
                    for (y = 0; y < height; y++)
                    {
                        for (x = 0; x < width; x++)
                            *(q++) = (((Uint32)(*(r++) << shift)) << 24) | (((Uint32)(*(g++) << shift)) << 16) | (((Uint32)(*(b++) << shift)) << 8);
                    }
                }
                else
                {
                    for (y = 0; y < height; y++)
                    {
                        for (x = 0; x < width; x++)
                            *(q++) = (((Uint32)(*(r++) >> shift)) << 24) | (((Uint32)(*(g++) >> shift)) << 16) | (((Uint32)(*(b++) >> shift)) << 8);
                    }
                }
                return (void *)data;
            }
        }
        return NULL;
    }

 protected: 
    DiColorPixelTemplate(const DiMonoPixel *pixel)
      : DiColorPixel(pixel)
    {
        Data[0] = NULL;
        Data[1] = NULL;
        Data[2] = NULL;
    }

    DiColorPixelTemplate(const DiColorPixel *pixel, const unsigned long count)
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
            Data[0] = new T[getCount()];
            Data[1] = new T[getCount()];
            Data[2] = new T[getCount()];
            result = (Data[0] != NULL) && (Data[1] != NULL) && (Data[2] != NULL);
        }
        return result;
    }
 
    T *Data[3];
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dicopxt.h,v $
** Revision 1.6  1998-11-27 13:50:20  joergr
** Added copyright message. Replaced delete by delete[] for array types.
** Added method to give direct (non const) access to internal data buffer.
**
** Revision 1.5  1998/05/11 14:53:13  joergr
** Added CVS/RCS header to each file.
**
**
*/
