/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Purpose: DicomMonochromeCopyTemplate (Header)
 *
 */


#ifndef DIMOCPT_H
#define DIMOCPT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofbmanip.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/dimopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to copy monochrome pixel data
 */
template<class T>
class DiMonoCopyTemplate
  : public DiMonoPixelTemplate<T>
{

 public:

    /** constructor
     *
     ** @param  pixel   pointer to monochrome intermediate representation of pixel data
     *  @param  fstart  first frame to be copied
     *  @param  fcount  number of frames to be copied
     *  @param  fsize   size of one frame (in bytes)
     */
    DiMonoCopyTemplate(const DiMonoPixel *pixel,
                       const unsigned long fstart,
                       const unsigned long fcount,
                       const unsigned long fsize)
      : DiMonoPixelTemplate<T>(pixel, fcount * fsize)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if ((pixel->getCount() > fstart * fsize) && (pixel->getCount() >= (fstart + fcount) * fsize))
                copy(OFstatic_cast(const T *, pixel->getData()) + fstart * fsize);
            /* need to determine the global min/max value */
            this->determineMinMax();
        }
    }

    /** destructor
     */
    ~DiMonoCopyTemplate()
    {
    }


 private:

    /** copy specified amount of pixel data
     *
     ** @param  pixel  array of pixel data to be copied
     */
    inline void copy(const T *pixel)
    {
        if (pixel != NULL)
        {
            this->Data = new T[this->getCount()];
            if (this->Data != NULL)
                OFBitmanipTemplate<T>::copyMem(pixel, this->Data, this->getCount());
        }
    }
};


#endif
