/*
**
** Author:  Joerg Riesmeier
** Created: 01.01.97
**
** Module:  dimoclt.h
**
** Purpose: DicomMonochromeClipTemplate (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:19 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dimoclt.h,v $
** CVS/RCS Revision: $Revision: 1.4 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIMOCLT_H
#define __DIMOCLT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimopxt.h"
#include "diclipt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiMonoClipTemplate : public DiMonoPixelTemplate<T>, protected DiClipTemplate<T>
{
 public:
    DiMonoClipTemplate(const DiMonoPixel *pixel, const Uint16 src_cols, const Uint16 src_rows, const Uint16 left,
        const Uint16 top, const Uint16 columns, const Uint16 rows, const Uint32 frames)
      : DiMonoPixelTemplate<T>(pixel, (unsigned long)columns * (unsigned long)rows * frames),
        DiClipTemplate<T>(1, src_cols, src_rows, left, top, columns, rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            clip((const T *)pixel->getData());
            determineMinMax();
        }
    }

    ~DiMonoClipTemplate()
    {
    }

 private:
    inline void clip(const T *pixel)
    {
        if (pixel != NULL)
        {
            Data = new T[getCount()];
            if (Data != NULL)
                DiClipTemplate<T>::clipData(&pixel, &Data);
        }
    }
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimoclt.h,v $
** Revision 1.4  1998-05-11 14:53:19  joergr
** Added CVS/RCS header to each file.
**
**
*/
