/*
**
** Author:  Joerg Riesmeier
** Created: 03.01.97
**
** Module:  ditranst.h
**
** Purpose: DicomTransTemplate (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:30 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/ditranst.h,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DITRANST_H
#define __DITRANST_H

#include "osconfig.h"
#include "dctypes.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiTransTemplate
{
 protected:
    DiTransTemplate(const int planes, const Uint16 src_x, const Uint16 src_y, const Uint16 dest_x, const Uint16 dest_y,
        const Uint32 frames, const int bits = 0)
      : Planes(planes), 
        Src_X(src_x),
        Src_Y(src_y),
        Dest_X(dest_x),
        Dest_Y(dest_y),
        Frames(frames),
        Bits(((bits < 1) || (bits > (int)bitsof(T))) ? (int)bitsof(T) : bits)
    {
    }

    virtual ~DiTransTemplate()
    {
    }
    
    inline void copyPixel(const T *src[], T *dest[])
    {
        const unsigned long count = (unsigned long)Dest_X * (unsigned long)Dest_Y * Frames; 
        register unsigned long i;
        for (int j = 0; j < Planes; j++)
        {
            register const T *p = src[j];
            register T *q = dest[j];
            for (i = 0; i < count; i++)
                *(q++) = *(p++);
        }
    }

    const int Planes;
    const Uint16 Src_X;
    const Uint16 Src_Y;
    const Uint16 Dest_X;
    const Uint16 Dest_Y;
    const Uint32 Frames;
    const int Bits;
};


#endif


/*
**
** CVS/RCS Log:
** $Log: ditranst.h,v $
** Revision 1.2  1998-05-11 14:53:30  joergr
** Added CVS/RCS header to each file.
**
**
*/
