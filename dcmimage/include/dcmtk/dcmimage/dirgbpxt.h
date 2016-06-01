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
 *  Purpose: DicomRGBPixelTemplate (Header)
 *
 */


#ifndef DIRGBPXT_H
#define DIRGBPXT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicopxt.h"
#include "dcmtk/dcmimgle/diinpx.h"  /* gcc 3.4 needs this */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle RGB pixel data
 */
template<class T1, class T2>
class DiRGBPixelTemplate
  : public DiColorPixelTemplate<T2>
{

 public:

    /** constructor
     *
     ** @param  docu       pointer to DICOM document
     *  @param  pixel      pointer to input pixel representation
     *  @param  status     reference to status variable
     *  @param  planeSize  number of pixel in a single plane
     *  @param  bits       number of bits per sample
     */
    DiRGBPixelTemplate(const DiDocument *docu,
                       const DiInputPixel *pixel,
                       EI_Status &status,
                       const unsigned long planeSize,
                       const int bits)
      : DiColorPixelTemplate<T2>(docu, pixel, 3, status)
    {
        if ((pixel != NULL) && (this->Count > 0) && (status == EIS_Normal))
            convert(OFstatic_cast(const T1 *, pixel->getData()) + pixel->getPixelStart(), planeSize, bits);
    }

    /** destructor
     */
    virtual ~DiRGBPixelTemplate()
    {
    }


 private:

    /** convert input pixel data to intermediate representation
     *
     ** @param  pixel      pointer to input pixel data
     *  @param  planeSize  number of pixels in a plane
     *  @param  bits       number of bits per sample
     */
    void convert(const T1 *pixel,
                 const unsigned long planeSize,
                 const int bits)
    {
        if (this->Init(pixel))
        {
            // use the number of input pixels derived from the length of the 'PixelData'
            // attribute), but not more than the size of the intermediate buffer
            const unsigned long count = (this->InputCount < this->Count) ? this->InputCount : this->Count;
            const T1 offset = OFstatic_cast(T1, DicomImageClass::maxval(bits - 1));
            const T1 *p = pixel;
            if (this->PlanarConfiguration)
            {
/*
                T2 *q;
                // number of pixels to be skipped (only applicable if 'PixelData' contains more
                // pixels than expected)
                const unsigned long skip = (this->InputCount > this->Count) ? (this->InputCount - this->Count) : 0;
                for (int j = 0; j < 3; ++j)
                {
                    q = this->Data[j];
                    for (i = count; i != 0; --i)
                        *(q++) = removeSign(*(p++), offset);
                    // skip to beginning of next plane
                    p += skip;
                }
*/
                unsigned long l;
                unsigned long i = 0;
                while (i < count)
                {
                    /* store current pixel index */
                    const unsigned long iStart = i;
                    /* for all planes ... */
                    for (int j = 0; j < 3; ++j)
                    {
                        /* convert a single plane */
                        for (l = planeSize, i = iStart; (l != 0) && (i < count); --l, ++i)
                            this->Data[j][i] = removeSign(*(p++), offset);
                    }
                }
            }
            else
            {
                int j;
                unsigned long i;
                for (i = 0; i < count; ++i)                             /* for all pixel ... */
                    for (j = 0; j < 3; ++j)
                        this->Data[j][i] = removeSign(*(p++), offset);  /* ... copy planes */
            }
        }
    }
};


#endif
