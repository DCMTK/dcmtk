/*
**
** Author:  Joerg Riesmeier
** Created: 01.01.97
**
** Module:  dicosct.h
**
** Purpose: DicomColorScaleTemplate (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:14 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicosct.h,v $
** CVS/RCS Revision: $Revision: 1.4 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


/*********************************************************************
** 
**  DicomColorScaleTemplate (Header)
**
**  author   : Joerg Riesmeier
**  created  : 01.01.97
**  modified : 02.03.98 
**
*********************************************************************/


#ifndef __DICOSCT_H
#define __DICOSCT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dicopx.h"
#include "discalet.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiColorScaleTemplate : public DiColorPixelTemplate<T>, protected DiScaleTemplate<T>
{
 public:
    DiColorScaleTemplate(const DiColorPixel *pixel, const Uint16 src_cols, const Uint16 src_rows,
        const Uint16 dest_cols, const Uint16 dest_rows, const Uint32 frames, const int bits, const int interpolate)
      : DiColorPixelTemplate<T>(pixel, (unsigned long)dest_cols * (unsigned long)dest_rows * frames),
        DiScaleTemplate<T>(3, src_cols, src_rows, dest_cols, dest_rows, frames, bits)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
            scale((const T **)pixel->getData(), interpolate);
    }
    
    virtual ~DiColorScaleTemplate()
    {
    }

 private:
    inline void scale(const T *pixel[3], const int interpolate)
    {
        if (Init(pixel))
            DiScaleTemplate<T>::scaleData(pixel, Data, interpolate);
    }
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dicosct.h,v $
** Revision 1.4  1998-05-11 14:53:14  joergr
** Added CVS/RCS header to each file.
**
**
*/
