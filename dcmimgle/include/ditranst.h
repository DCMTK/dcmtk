/*
 *
 *  Copyright (C) 1997-99, OFFIS
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
 *  Purpose: DicomTransTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 15:48:10 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/ditranst.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DITRANST_H
#define __DITRANST_H

#include "osconfig.h"
#include "dctypes.h"

#include "ofbmanip.h"


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
        for (int j = 0; j < Planes; j++)
            OFBitmanipTemplate<T>::copyMem(src[j], dest[j], count);
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
** Revision 1.1  1998-11-27 15:48:10  joergr
** Added copyright message.
** Added support for new bit manipulation class.
**
** Revision 1.2  1998/05/11 14:53:30  joergr
** Added CVS/RCS header to each file.
**
**
*/
