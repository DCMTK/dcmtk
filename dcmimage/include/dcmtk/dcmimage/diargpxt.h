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
 *  Purpose: DicomARGBPixelTemplate (Header) - UNTESTED !!!
 *
 */


#ifndef DIARGPXT_H
#define DIARGPXT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicopxt.h"
#include "dcmtk/dcmimgle/diluptab.h"
#include "dcmtk/dcmimgle/diinpx.h"  /* gcc 3.4 needs this */

/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle ARGB pixel data
 */
template<class T1, class T2, class T3>
class DiARGBPixelTemplate
  : public DiColorPixelTemplate<T3>
{

 public:

    /** constructor
     *
     ** @param  docu       pointer to DICOM document
     *  @param  pixel      pointer to input pixel representation
     *  @param  palette    pointer to RGB color palette
     *  @param  status     reference to status variable
     *  @param  planeSize  number of pixels in a plane
     *  @param  bits       number of bits per sample
     */
    DiARGBPixelTemplate(const DiDocument *docu,
                        const DiInputPixel *pixel,
                        DiLookupTable *palette[3],
                        EI_Status &status,
                        const unsigned long planeSize,
                        const int bits)
      : DiColorPixelTemplate<T3>(docu, pixel, 4, status)
    {
        if ((pixel != NULL) && (this->Count > 0) && (status == EIS_Normal))
            convert(OFstatic_cast(const T1 *, pixel->getData()) + pixel->getPixelStart(), palette, planeSize, bits);
    }

    /** destructor
     */
    virtual ~DiARGBPixelTemplate()
    {
    }


 private:

    /** convert input pixel data to intermediate representation
     *
     ** @param  pixel      pointer to input pixel data
     *  @param  palette    pointer to RGB color palette
     *  @param  planeSize  number of pixels in a plane
     *  @param  bits       number of bits per sample
     */
    void convert(const T1 *pixel,
                 DiLookupTable *palette[3],
                 const unsigned long planeSize,
                 const int bits)
    {                                             // not very much optimized, but no one really uses ARGB !!
        if (this->Init(pixel))
        {
            T2 value;
            const T1 offset = OFstatic_cast(T1, DicomImageClass::maxval(bits - 1));
            // use the number of input pixels derived from the length of the 'PixelData'
            // attribute), but not more than the size of the intermediate buffer
            const unsigned long count = (this->InputCount < this->Count) ? this->InputCount : this->Count;
            if (this->PlanarConfiguration)
            {
/*
                const T1 *a = pixel;                                            // points to alpha plane
                const T1 *rgb[3];
                rgb[0] = a + this->InputCount;                                  // points to red plane
                rgb[1] = rgb[0] + this->InputCount;                             // points to green plane
                rgb[2] = rgb[1] + this->InputCount;                             // points to blue plane
                for (i = 0; i < count; ++i)
                {
                    value = OFstatic_cast(T2, *(a++));                          // get alpha value
                    if (value > 0)
                    {
                        for (int j = 0; j < 3; ++j)                             // set palette color
                        {
                            if (value <= palette[j]->getFirstEntry(value))
                                this->Data[j][i] = OFstatic_cast(T3, palette[j]->getFirstValue());
                            else if (value >= palette[j]->getLastEntry(value))
                                this->Data[j][i] = OFstatic_cast(T3, palette[j]->getLastValue());
                            else
                                this->Data[j][i] = OFstatic_cast(T3, palette[j]->getValue(value));
                            ++rgb[j];                                           // skip RGB values
                        }
                    }
                    else
                    {
                        for (j = 0; j < 3; ++j)                                 // copy RGB values
                            this->Data[j][i] = OFstatic_cast(T3, removeSign(*(rgb[j]++), offset));
                    }
                }
*/
                unsigned long l;
                unsigned long i = 0;
                const T1 *a = pixel;                                            // points to alpha plane
                const T1 *rgb[3];
                rgb[0] = a + planeSize;                                         // points to red plane
                rgb[1] = rgb[0] + planeSize;                                    // points to green plane
                rgb[2] = rgb[1] + planeSize;                                    // points to blue plane
                while (i < count)
                {
                    /* convert a single frame */
                    for (l = planeSize; (l != 0) && (i < count); --l, ++i)
                    {
                        value = OFstatic_cast(T2, *(a++));                      // get alpha value
                        if (value > 0)
                        {
                            for (int j = 0; j < 3; ++j)                         // set palette color
                            {
                                if (value <= palette[j]->getFirstEntry(value))
                                    this->Data[j][i] = OFstatic_cast(T3, palette[j]->getFirstValue());
                                else if (value >= palette[j]->getLastEntry(value))
                                    this->Data[j][i] = OFstatic_cast(T3, palette[j]->getLastValue());
                                else
                                    this->Data[j][i] = OFstatic_cast(T3, palette[j]->getValue(value));
                                ++rgb[j];                                       // skip RGB values
                            }
                        }
                        else
                        {
                            for (int j = 0; j < 3; ++j)                         // copy RGB values
                                this->Data[j][i] = OFstatic_cast(T3, removeSign(*(rgb[j]++), offset));
                        }
                    }
                    /* jump to next frame start (skip 2 planes) */
                    a += 2 * planeSize;
                    for (int j = 0; j < 3; ++j)
                       rgb[j] += 2 * planeSize;
                }
            } else {
                unsigned long i;
                const T1 *p = pixel;
                for (i = 0; i < count; ++i)
                {
                    value = OFstatic_cast(T2, *(p++));                          // get alpha value
                    if (value > 0)
                    {
                        for (int j = 0; j < 3; ++j)                             // set palette color
                        {
                            if (value <= palette[j]->getFirstEntry(value))
                                this->Data[j][i] = OFstatic_cast(T3, palette[j]->getFirstValue());
                            else if (value >= palette[j]->getLastEntry(value))
                                this->Data[j][i] = OFstatic_cast(T3, palette[j]->getLastValue());
                            else
                                this->Data[j][i] = OFstatic_cast(T3, palette[j]->getValue(value));
                        }
                        p += 3;                                                 // skip RGB values
                    }
                    else
                    {
                        for (int j = 0; j < 3; ++j)                             // copy RGB values
                            this->Data[j][i] = OFstatic_cast(T3, removeSign(*(p++), offset));
                    }
                }
            }
        }
    }
};


#endif
