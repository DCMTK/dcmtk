/*
 *
 *  Copyright (C) 1996-2011, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomMonochromeScaleTemplate (Header)
 *
 */


#ifndef DIMOSCT_H
#define DIMOSCT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/dimopxt.h"
#include "dcmtk/dcmimgle/discalet.h"
#include "dcmtk/dcmimgle/didispfn.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to scale monochrome images (on pixel data level).
 */
template<class T>
class DiMonoScaleTemplate
  : public DiMonoPixelTemplate<T>,
    protected DiScaleTemplate<T>
{

 public:

    /** constructor
     *
     ** @param  pixel        pointer to intermediate pixel representation
     *  @param  columns      width of source image
     *  @param  rows         height of source image
     *  @param  left_pos     left coordinate of clipping area
     *  @param  top_pos      top coordinate of clipping area
     *  @param  src_cols     width of clipping area
     *  @param  src_rows     height of clipping area
     *  @param  dest_cols    width of destination image (scaled image)
     *  @param  dest_rows    height of destination image
     *  @param  frames       number of frames
     *  @param  bits         number of bits per plane/pixel
     *  @param  interpolate  use of interpolation when scaling
     *  @param  pvalue       value possibly used for regions outside the image boundaries
     */
    DiMonoScaleTemplate(const DiMonoPixel *pixel,
                        const Uint16 columns,
                        const Uint16 rows,
                        const signed long left_pos,
                        const signed long top_pos,
                        const Uint16 src_cols,
                        const Uint16 src_rows,
                        const Uint16 dest_cols,
                        const Uint16 dest_rows,
                        const Uint32 frames,
                        const int bits,
                        const int interpolate,
                        const Uint16 pvalue)
      : DiMonoPixelTemplate<T>(pixel, OFstatic_cast(unsigned long, dest_cols) * OFstatic_cast(unsigned long, dest_rows) * frames),
        DiScaleTemplate<T>(1, columns, rows, left_pos, top_pos, src_cols, src_rows, dest_cols, dest_rows, frames, bits)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if (pixel->getCount() == OFstatic_cast(unsigned long, columns) * OFstatic_cast(unsigned long, rows) * frames)
            {
                scale(OFstatic_cast(const T *, pixel->getData()), pixel->getBits(), interpolate, pvalue);
                this->determineMinMax();
            } else {
                DCMIMGLE_WARN("could not scale image ... corrupted data");
            }
        }
    }

    /** destructor
     */
    virtual ~DiMonoScaleTemplate()
    {
    }


 private:

    /** scale pixel data
     *
     ** @param  pixel        pointer to pixel data to be scaled
     *  @param  bits         bit depth of pixel data
     *  @param  interpolate  use of interpolation when scaling
     *  @param  pvalue       value possibly used for regions outside the image boundaries
     */
    inline void scale(const T *pixel,
                      const unsigned int bits,
                      const int interpolate,
                      const Uint16 pvalue)
    {
        if (pixel != NULL)
        {
            this->Data = new T[this->getCount()];
            if (this->Data != NULL)
            {
                const T value = OFstatic_cast(T, OFstatic_cast(double, DicomImageClass::maxval(bits)) *
                    OFstatic_cast(double, pvalue) / OFstatic_cast(double, DicomImageClass::maxval(WIDTH_OF_PVALUES)));
                this->scaleData(&pixel, &this->Data, interpolate, value);
             }
        }
    }
};


#endif
