/*
 *
 *  Copyright (C) 1999, OFFIS
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
 *  Purpose: DicomMonochromeColorOutputPixelTemplate (Header)
 *
 *  Last Update:         $Author: joergr $
 *  Update Date:         $Date: 1999-04-28 16:01:55 $
 *  Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimcopxt.h,v $
 *  CVS/RCS Revision:    $Revision: 1.1 $
 *  Status:              $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIMCOPXT_H
#define __DIMCOPXT_H

#include "osconfig.h"

#include "dicoopxt.h"
#include "dimopx.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to convert monochrome image to true color pastel images.
 *  (on pixel data level)
 *  - EXPERIMENTAL version, do not use for any serious task !
 */
template<class T1, class T2>
class DiMonoColorOutputPixelTemplate
  : public DiColorOutputPixelTemplate<T1, T2>
{

 public:

    DiMonoColorOutputPixelTemplate(void *buffer,
                                   const DiMonoPixel *pixel,
                                   const unsigned long frame,
                                   const unsigned long frames,
                                   const int planar = 0)
      : DiColorOutputPixelTemplate<T1, T2>(buffer, pixel, frame, frames, planar)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            cerr << "WARNING: performing EXPERIMENTAL 'pastel color' transformation !" << endl;
            convert(pixel, frame * Count);
        }
    }
    
    virtual ~DiMonoColorOutputPixelTemplate()
    {
    }


 private:

    inline int init(T2 *&rgb,
                    const unsigned long max,
                    const unsigned long cnt)
    {
        if (cnt > 0)
        {
            rgb = new T2[3 * cnt];
            if (rgb != NULL)
            {
                register unsigned long i;
                register T2 *q = rgb;
                for (i = 0; i < max; i++)
                {
                    *(q++) = (T2)i;     *(q++) = (T2)i;     *(q++) = (T2)i;
                    *(q++) = (T2)i;     *(q++) = (T2)i;     *(q++) = (T2)i + 1;
                    *(q++) = (T2)i + 1; *(q++) = (T2)i;     *(q++) = (T2)i;
                    *(q++) = (T2)i + 1; *(q++) = (T2)i;     *(q++) = (T2)i + 1;
                    *(q++) = (T2)i;     *(q++) = (T2)i + 1; *(q++) = (T2)i;
                    *(q++) = (T2)i;     *(q++) = (T2)i + 1; *(q++) = (T2)i + 1;
                    *(q++) = (T2)i + 1; *(q++) = (T2)i + 1; *(q++) = (T2)i;
                }
                *(q++) = (T2)i; *(q++) = (T2)i; *(q++) = (T2)i;
                return 1;
            }
        }
        return 0;
    }

    inline void convert(const DiMonoPixel *inter,
                        const unsigned long start)
    {
        const T1 *pixel = (const T1 *)(inter->getData());
        if (pixel != NULL)
        {
            if (Data == NULL)
                Data = new T2[Count * 3];
            if (Data != NULL)
            {
                T2 *rgb_lut;
                const unsigned long max = DicomImageClass::maxval(bitsof(T2));
                const unsigned long cnt = (max * 7 + 1);
                if (init(rgb_lut, max, cnt))
                {
                    register const T1 *p = pixel + start;
                    register T2 *q = Data;
                    register T2 *v;
                    register unsigned long i;
                    const double gradient = (double)(cnt - 1) / (double)inter->getAbsMaxRange();
                    const T2 absmin = (T2)inter->getAbsMinimum();
                    for (i = 0; i < Count; i++)
                    {
                        v = rgb_lut + (unsigned long)((double)(*(p++) - absmin) * gradient) * 3;
                        *(q++) = *v;
                        *(q++) = *(v + 1);
                        *(q++) = *(v + 2);
                    }
                }
                delete[] rgb_lut;
            }
        } else
            Data = NULL;
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dimcopxt.h,v $
 * Revision 1.1  1999-04-28 16:01:55  joergr
 * Added experimental support to create grayscale images with more than 256
 * shades of gray to be displayed on a consumer monitor (use pastel colors).
 *
 *
 *
 */
