/*
**
** Author:  Joerg Riesmeier
** Created: 01.01.97
**
** Module:  dicoclt.h
**
** Purpose: DicomColorClipTemplate (Header)
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 1998-05-11 14:53:10 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicoclt.h,v $
** CVS/RCS Revision:    $Revision: 1.4 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/



#ifndef __DICOCLT_H
#define __DICOCLT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dicopx.h"
#include "diclipt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiColorClipTemplate : public DiColorPixelTemplate<T>, protected DiClipTemplate<T>
{
 public:
    DiColorClipTemplate(const DiColorPixel *pixel, const Uint16 src_cols, const Uint16 src_rows, const Uint16 left,
        const Uint16 top, const Uint16 columns, const Uint16 rows, const Uint32 frames)
      : DiColorPixelTemplate<T>(pixel, (unsigned long)columns * (unsigned long)rows * frames),
        DiClipTemplate<T>(3, src_cols, src_rows, left, top, columns, rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
            clip((const T **)pixel->getData());
    }

    virtual ~DiColorClipTemplate()
    {
    }

 private:
    inline void clip(const T *pixel[3])
    {
        if (Init(pixel))
            DiClipTemplate<T>::clipData(pixel, Data);
    }

};


#endif


/*
**
** CVS/RCS Log:
** $Log: dicoclt.h,v $
** Revision 1.4  1998-05-11 14:53:10  joergr
** Added CVS/RCS header to each file.
**
**
*/
