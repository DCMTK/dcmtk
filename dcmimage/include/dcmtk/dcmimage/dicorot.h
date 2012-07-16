/*
 *
 *  Copyright (C) 1998-2011, OFFIS e.V.
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
 *  Purpose: DicomColorRotateTemplate (Header)
 *
 */


#ifndef DICOROT_H
#define DICOROT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicopxt.h"
#include "dcmtk/dcmimgle/dirotat.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to rotate color images (on pixel data level).
 *  by steps of 90 degrees
 */
template<class T>
class DiColorRotateTemplate
  : public DiColorPixelTemplate<T>,
    protected DiRotateTemplate<T>
{

 public:

    /** constructor
     *
     ** @param  pixel      pointer to intermediate pixel representation
     *  @param  src_cols   width of clipping area
     *  @param  src_rows   height of clipping area
     *  @param  dest_cols  width of destination image (scaled image)
     *  @param  dest_rows  height of destination image
     *  @param  frames     number of frames
     *  @param  degree     angle by which the pixel data should be rotated
     */
    DiColorRotateTemplate(const DiColorPixel *pixel,
                          const Uint16 src_cols,
                          const Uint16 src_rows,
                          const Uint16 dest_cols,
                          const Uint16 dest_rows,
                          const Uint32 frames,
                          const int degree)
      : DiColorPixelTemplate<T>(pixel, OFstatic_cast(unsigned long, dest_cols) * OFstatic_cast(unsigned long, dest_rows) * frames),
        DiRotateTemplate<T>(3, src_cols, src_rows, dest_cols, dest_rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if (pixel->getCount() == OFstatic_cast(unsigned long, src_cols) * OFstatic_cast(unsigned long, src_rows) * frames)
                rotate(OFstatic_cast(const T **, OFconst_cast(void *, pixel->getData())), degree);
            else {
                DCMIMAGE_WARN("could not rotate image ... corrupted data");
            }
        }
    }

    /** destructor
     */
    ~DiColorRotateTemplate()
    {
    }


 private:

    /** rotate pixel data
     *
     ** @param  pixel   pointer to pixel data (3 components) to be rotated
     *  @param  degree  angle by which the pixel data should be rotated
     */
    inline void rotate(const T *pixel[3],
                       const int degree)
    {
        if (this->Init(pixel))
        {
            if (degree == 90)
                this->rotateRight(pixel, this->Data);
            else if (degree == 180)
                this->rotateTopDown(pixel, this->Data);
            else  if (degree == 270)
                this->rotateLeft(pixel, this->Data);
        }
    }
};


#endif
