/*********************************************************************
** 
**  DicomMonochromeScaleTemplate (Header)
**
**  author   : Joerg Riesmeier
**  created  : 31.12.96
**  modified : 02.03.98 
**
*********************************************************************/


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
