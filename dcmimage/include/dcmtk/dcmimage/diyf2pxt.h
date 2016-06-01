/*
 *
 *  Copyright (C) 1998-2016, OFFIS e.V.
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
 *  Purpose: DicomYBR422PixelTemplate (Header)
 *
 */


#ifndef DIYF2PXT_H
#define DIYF2PXT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicopxt.h"
#include "dcmtk/dcmimgle/diinpx.h"  /* gcc 3.4 needs this */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle YCbCr Full 4:2:2 pixel data
 */
template<class T1, class T2>
class DiYBR422PixelTemplate
  : public DiColorPixelTemplate<T2>
{

 public:

    /** constructor
     *
     ** @param  docu    pointer to DICOM document
     *  @param  pixel   pointer to input pixel representation
     *  @param  status  reference to status variable
     *  @param  bits    number of bits per sample
     *  @param  rgb     flag, convert color model to RGB only if true
     */
    DiYBR422PixelTemplate(const DiDocument *docu,
                          const DiInputPixel *pixel,
                          EI_Status &status,
                          const int bits,
                          const OFBool rgb)
      : DiColorPixelTemplate<T2>(docu, pixel, 3, status, 2)
    {
        if ((pixel != NULL) && (this->Count > 0) && (status == EIS_Normal))
        {
            if (this->PlanarConfiguration)
            {
                status = EIS_InvalidValue;
                DCMIMAGE_ERROR("invalid value for 'PlanarConfiguration' (" << this->PlanarConfiguration << ")");
            }
            else
                convert(OFstatic_cast(const T1 *, pixel->getData()) + pixel->getPixelStart(), bits, rgb);
        }
    }

    /** destructor
     */
    virtual ~DiYBR422PixelTemplate()
    {
    }


 private:

    /** convert input pixel data to intermediate representation
     *
     ** @param  pixel      pointer to input pixel data
     *  @param  bits       number of bits per sample
     *  @param  rgb        flag, convert color model to RGB only if true
     */
    void convert(const T1 *pixel,
                 const int bits,
                 const OFBool rgb)
    {
        if (this->Init(pixel))
        {
            const T1 offset = OFstatic_cast(T1, DicomImageClass::maxval(bits - 1));
            unsigned long i;
            const T1 *p = pixel;
            T2 *r = this->Data[0];
            T2 *g = this->Data[1];
            T2 *b = this->Data[2];
            T2 y1;
            T2 y2;
            T2 cb;
            T2 cr;
            // use the number of input pixels derived from the length of the 'PixelData'
            // attribute), but not more than the size of the intermediate buffer
            const unsigned long count = (this->InputCount < this->Count) ? this->InputCount : this->Count;
            if (rgb)    /* convert to RGB model */
            {
                const T2 maxvalue = OFstatic_cast(T2, DicomImageClass::maxval(bits));
                for (i = count / 2; i != 0; --i)
                {
                    y1 = removeSign(*(p++), offset);
                    y2 = removeSign(*(p++), offset);
                    cb = removeSign(*(p++), offset);
                    cr = removeSign(*(p++), offset);
                    convertValue(*(r++), *(g++), *(b++), y1, cb, cr, maxvalue);
                    convertValue(*(r++), *(g++), *(b++), y2, cb, cr, maxvalue);
                }
            } else {    /* retain YCbCr model: YCbCr_422_full -> YCbCr_full */
                for (i = count / 2; i != 0; --i)
                {
                    y1 = removeSign(*(p++), offset);
                    y2 = removeSign(*(p++), offset);
                    cb = removeSign(*(p++), offset);
                    cr = removeSign(*(p++), offset);
                    *(r++) = y1;
                    *(g++) = cb;
                    *(b++) = cr;
                    *(r++) = y2;
                    *(g++) = cb;
                    *(b++) = cr;
                }
            }
        }
    }

    /** convert a single YCbCr value to RGB
     */
    inline void convertValue(T2 &red,
                             T2 &green,
                             T2 &blue,
                             const T2 y,
                             const T2 cb,
                             const T2 cr,
                             const T2 maxvalue)
    {
        double dr = OFstatic_cast(double, y) + 1.4020 * OFstatic_cast(double, cr) - 0.7010 * OFstatic_cast(double, maxvalue);
        double dg = OFstatic_cast(double, y) - 0.3441 * OFstatic_cast(double, cb) - 0.7141 * OFstatic_cast(double, cr) + 0.5291 * OFstatic_cast(double, maxvalue);
        double db = OFstatic_cast(double, y) + 1.7720 * OFstatic_cast(double, cb) - 0.8859 * OFstatic_cast(double, maxvalue);
        red   = (dr < 0.0) ? 0 : (dr > OFstatic_cast(double, maxvalue)) ? maxvalue : OFstatic_cast(T2, dr);
        green = (dg < 0.0) ? 0 : (dg > OFstatic_cast(double, maxvalue)) ? maxvalue : OFstatic_cast(T2, dg);
        blue  = (db < 0.0) ? 0 : (db > OFstatic_cast(double, maxvalue)) ? maxvalue : OFstatic_cast(T2, db);
    }
};


#endif
