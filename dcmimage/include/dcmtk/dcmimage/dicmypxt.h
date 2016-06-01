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
 *  Purpose: DicomCMYKPixelTemplate (Header)
 *
 */


#ifndef DICMYPXT_H
#define DICMYPXT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicopxt.h"
#include "dcmtk/dcmimgle/diinpx.h"  /* gcc 3.4 needs this */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle CMYK pixel data
 */
template<class T1, class T2>
class DiCMYKPixelTemplate
  : public DiColorPixelTemplate<T2>
{

 public:

    /** constructor
     *
     ** @param  docu       pointer to DICOM document
     *  @param  pixel      pointer to input pixel representation
     *  @param  status     reference to status variable
     *  @param  planeSize  number of pixels in a plane
     *  @param  bits       number of bits per sample
     */
    DiCMYKPixelTemplate(const DiDocument *docu,
                        const DiInputPixel *pixel,
                        EI_Status &status,
                        const unsigned long planeSize,
                        const int bits)
      : DiColorPixelTemplate<T2>(docu, pixel, 4, status)
    {
        if ((pixel != NULL) && (this->Count > 0) && (status == EIS_Normal))
            convert(OFstatic_cast(const T1 *, pixel->getData()) + pixel->getPixelStart(), planeSize, bits);
    }

    /** destructor
     */
    virtual ~DiCMYKPixelTemplate()
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
            const T2 maxvalue = OFstatic_cast(T2, DicomImageClass::maxval(bits));
            const T1 offset = OFstatic_cast(T1, DicomImageClass::maxval(bits - 1));
            const T1 *p = pixel;
            if (this->PlanarConfiguration)
            {
/*
                const T1 *k;
                T2 *q;
                for (int j = 0; j < 3; ++j)
                {
                    q = this->Data[j];
                    k = pixel + 3 * this->InputCount;                     // beginning of 'black' plane
                    for (i = count; i != 0; --i)
                        *(q++) = maxvalue - removeSign(*(p++), offset) - removeSign(*(k++), offset);
                }
*/
                const T1 *k;
                T2 *q;
                unsigned long l;
                unsigned long i = 0;
                while (i < count)
                {
                    /* store current pixel index */
                    const unsigned long iStart = i;
                    /* beginning of 'black' plane */
                    const T1 *kStart = p + 3 * planeSize;
                    /* for all planes ... */
                    for (int j = 0; j < 3; ++j)
                    {
                        q = this->Data[j] + iStart;
                        /* reset to beginning of 'black' plane */
                        k = kStart;
                        /* convert a single plane */
                        for (l = planeSize, i = iStart; (l != 0) && (i < count); --l, ++i)
                            *(q++) = maxvalue - removeSign(*(p++), offset) - removeSign(*(k++), offset);
                    }
                    /* skip black plane */
                    p += planeSize;
                }
            }
            else
            {
                T1 k;
                int j;
                unsigned long i;
                for (i = 0; i < count; ++i)
                {
                    k = *(p + 3);
                    for (j = 0; j < 3; ++j)
                        this->Data[j][i] = maxvalue - removeSign(*(p++), offset) - removeSign(k, offset);
                    ++p;                                            // skip 'black'
                }
            }
        }
    }
};


#endif
