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
 *  Purpose: DicomHSVPixelTemplate (Header)
 *
 */


#ifndef DIHSVPXT_H
#define DIHSVPXT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicopxt.h"
#include "dcmtk/dcmimgle/diinpx.h"  /* gcc 3.4 needs this */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle HSV pixel data
 */
template<class T1, class T2>
class DiHSVPixelTemplate
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
    DiHSVPixelTemplate(const DiDocument *docu,
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
    virtual ~DiHSVPixelTemplate()
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
            T2 *r = this->Data[0];
            T2 *g = this->Data[1];
            T2 *b = this->Data[2];
            const T2 maxvalue = OFstatic_cast(T2, DicomImageClass::maxval(bits));
            const T1 offset = OFstatic_cast(T1, DicomImageClass::maxval(bits - 1));
            // use the number of input pixels derived from the length of the 'PixelData'
            // attribute), but not more than the size of the intermediate buffer
            const unsigned long count = (this->InputCount < this->Count) ? this->InputCount : this->Count;
            if (this->PlanarConfiguration)
            {
/*
                const T1 *h = pixel;
                const T1 *s = h + this->InputCount;
                const T1 *v = s + this->InputCount;
                for (i = count; i != 0; --i)
                    convertValue(*(r++), *(g++), *(b++), removeSign(*(h++), offset), removeSign(*(s++), offset),
                        removeSign(*(v++), offset), maxvalue);
*/
                unsigned long l;
                unsigned long i = count;
                const T1 *h = pixel;
                const T1 *s = h + planeSize;
                const T1 *v = s + planeSize;
                while (i != 0)
                {
                    /* convert a single frame */
                    for (l = planeSize; (l != 0) && (i != 0); --l, --i)
                    {
                        convertValue(*(r++), *(g++), *(b++), removeSign(*(h++), offset), removeSign(*(s++), offset),
                            removeSign(*(v++), offset), maxvalue);
                    }
                    /* jump to next frame start (skip 2 planes) */
                    h += 2 * planeSize;
                    s += 2 * planeSize;
                    v += 2 * planeSize;
                }
            }
            else
            {
                const T1 *p = pixel;
                T2 h;
                T2 s;
                T2 v;
                unsigned long i;
                for (i = count; i != 0; --i)
                {
                    h = removeSign(*(p++), offset);
                    s = removeSign(*(p++), offset);
                    v = removeSign(*(p++), offset);
                    convertValue(*(r++), *(g++), *(b++), h, s, v, maxvalue);
                }
            }
        }
    }

    /** convert a single HSV value to RGB
     */
    void convertValue(T2 &red,
                      T2 &green,
                      T2 &blue,
                      const T2 hue,
                      const T2 saturation,
                      const T2 value,
                      const T2 maxvalue)
    {
        /*
         *   conversion algorithm taken from Foley et al.: 'Computer Graphics: Principles and Practice' (1990)
         */

        if (saturation == 0)
        {
            red = value;
            green = value;
            blue = value;
        }
        else
        {
            const double h = (OFstatic_cast(double, hue) * 6) / (OFstatic_cast(double, maxvalue) + 1);  // '... + 1' to assert h < 6
            const double s = OFstatic_cast(double, saturation) / OFstatic_cast(double, maxvalue);
            const double v = OFstatic_cast(double, value) / OFstatic_cast(double, maxvalue);
            const T2 hi = OFstatic_cast(T2, h);
            const double hf = h - hi;
            const T2 p = OFstatic_cast(T2, maxvalue * v * (1 - s));
            const T2 q = OFstatic_cast(T2, maxvalue * v * (1 - s * hf));
            const T2 t = OFstatic_cast(T2, maxvalue * v * (1 - s * (1 - hf)));
            switch (hi)
            {
                case 0:
                    red = value;
                    green = t;
                    blue = p;
                    break;
                case 1:
                    red = q;
                    green = value;
                    blue = p;
                    break;
                case 2:
                    red = p;
                    green = value;
                    blue = t;
                    break;
                case 3:
                    red = p;
                    green = q;
                    blue = value;
                    break;
                case 4:
                    red = t;
                    green = p;
                    blue = value;
                    break;
                case 5:
                    red = value;
                    green = p;
                    blue = q;
                    break;
                default:
                    DCMIMAGE_WARN("invalid value for 'hi' while converting HSV to RGB");
            }
        }
    }
};


#endif
