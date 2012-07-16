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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorCopyTemplate (Header)
 *
 */


#ifndef DICOCPT_H
#define DICOCPT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofbmanip.h"

#include "dcmtk/dcmimage/dicopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to copy color pixel data
 */
template<class T>
class DiColorCopyTemplate
  : public DiColorPixelTemplate<T>
{

 public:

    /** constructor
     *
     ** @param  pixel   pointer to color intermediate representation of pixel data
     *  @param  fstart  first frame to be copied
     *  @param  fcount  number of frames to be copied
     *  @param  fsize   size of one frame (in bytes)
     */
    DiColorCopyTemplate(const DiColorPixel *pixel,
                        const unsigned long fstart,
                        const unsigned long fcount,
                        const unsigned long fsize)
      : DiColorPixelTemplate<T>(pixel, fcount * fsize)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if ((pixel->getCount() > fstart * fsize) && (pixel->getCount() >= (fstart + fcount) * fsize))
                copy(OFstatic_cast(const T **, OFconst_cast(void *, pixel->getData())), fstart * fsize);
        }
    }

    /** destructor
     */
    ~DiColorCopyTemplate()
    {
    }


 private:

    /** copy specified amount of pixel data
     *
     ** @param  pixel   array of pixel data (3 color components) to be copied
     *  @param  offset  offset to the first pixel to be copied
     */
    inline void copy(const T *pixel[3],
                     const unsigned long offset)
    {
        if (this->Init(pixel))
        {
            for (int j = 0; j < 3; j++)
                OFBitmanipTemplate<T>::copyMem(pixel[j] + offset, this->Data[j], this->getCount());
        }
    }
};


#endif
