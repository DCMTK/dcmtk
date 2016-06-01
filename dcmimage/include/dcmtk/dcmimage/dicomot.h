/*
 *
 *  Copyright (C) 1996-2016, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorMonochromeTemplate (Header)
 *
 */


#ifndef DICOMOT_H
#define DICOMOT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/dimopxt.h"
#include "dcmtk/dcmimage/dicopx.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to convert color image to monochrome images.
 *  (on pixel data level)
 */
template<class T>
class DiColorMonoTemplate
  : public DiMonoPixelTemplate<T>
{

 public:

    /** constructor
     *
     ** @param  pixel     intermediate representation of color pixel data
     *  @param  modality  pointer to object managing modality transform
     *  @param  red       coefficient of red pixel component
     *  @param  green     coefficient of green pixel component
     *  @param  blue      coefficient of blue pixel component
     */
    DiColorMonoTemplate(const DiColorPixel *pixel,
                        DiMonoModality *modality,
                        const double red,
                        const double green,
                        const double blue)
      : DiMonoPixelTemplate<T>(pixel, modality)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            convert(OFstatic_cast(const T **, OFconst_cast(void *, pixel->getData())), red, green, blue);
            this->determineMinMax();
        }
    }

    /** destructor
     */
    virtual ~DiColorMonoTemplate()
    {
    }


 private:

    /** convert color pixel data to monochrome format
     *
     ** @param  pixel  intermediate representation of color pixel data
     *  @param  red    coefficient of red pixel component
     *  @param  green  coefficient of green pixel component
     *  @param  blue   coefficient of blue pixel component
     */
    void convert(const T *pixel[3],
                 const double red,
                 const double green,
                 const double blue)
    {
        if (pixel != NULL)
        {
            this->Data = new T[this->Count];
            if (this->Data != NULL)
            {
                const T *r = pixel[0];
                const T *g = pixel[1];
                const T *b = pixel[2];
                T *q = this->Data;
                unsigned long i;
                for (i = this->Count; i != 0; i--)
                {
                    *(q++) = OFstatic_cast(T, OFstatic_cast(double, *(r++)) * red +
                                              OFstatic_cast(double, *(g++)) * green +
                                              OFstatic_cast(double, *(b++)) * blue);
                }
            }
        }
    }
};


#endif
