/*
**
** Author:  Joerg Riesmeier
** Created: 31.12.96
**
** Module:  dimosct.h
**
** Purpose: DicomMonochromeScaleTemplate (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:24 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dimosct.h,v $
** CVS/RCS Revision: $Revision: 1.4 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIMOSCT_H
#define __DIMOSCT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimopxt.h"
#include "discalet.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiMonoScaleTemplate : public DiMonoPixelTemplate<T>, protected DiScaleTemplate<T>
{
 public:
    DiMonoScaleTemplate(const DiMonoPixel *pixel, const Uint16 src_cols, const Uint16 src_rows, const Uint16 dest_cols,
        const Uint16 dest_rows, const Uint32 frames, const int interpolate) 
      : DiMonoPixelTemplate<T>(pixel, (unsigned long)dest_cols * (unsigned long)dest_rows * frames),
        DiScaleTemplate<T>(1, src_cols, src_rows, dest_cols, dest_rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            scale((const T *)pixel->getData(), interpolate);
            determineMinMax();
        }
    }

    virtual ~DiMonoScaleTemplate()
    {
    }
    
 private:
    inline void scale(const T *pixel, const int interpolate)
    {
        if (pixel != NULL)
        {
            Data = new T[getCount()];
            if (Data != NULL)
                DiScaleTemplate<T>::scaleData(&pixel, &Data, interpolate);
        }
    }

};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimosct.h,v $
** Revision 1.4  1998-05-11 14:53:24  joergr
** Added CVS/RCS header to each file.
**
**
*/
