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
 *  Purpose: DicomYBRPixelTemplate (Header)
 *
 */


#ifndef DIYBRPXT_H
#define DIYBRPXT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicopxt.h"
#include "dcmtk/dcmimgle/diinpx.h"  /* gcc 3.4 needs this */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle YCbCr pixel data
 */
template<class T1, class T2>
class DiYBRPixelTemplate
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
     *  @param  rgb        flag, convert color model to RGB only if true
     */
    DiYBRPixelTemplate(const DiDocument *docu,
                       const DiInputPixel *pixel,
                       EI_Status &status,
                       const unsigned long planeSize,
                       const int bits,
                       const OFBool rgb)
      : DiColorPixelTemplate<T2>(docu, pixel, 3, status)
    {
        if ((pixel != NULL) && (this->Count > 0) && (status == EIS_Normal))
            convert(OFstatic_cast(const T1 *, pixel->getData()) + pixel->getPixelStart(), planeSize, bits, rgb);
    }

    /** destructor
     */
    virtual ~DiYBRPixelTemplate()
    {
    }


 private:

    /** convert input pixel data to intermediate representation
     *
     ** @param  pixel      pointer to input pixel data
     *  @param  planeSize  number of pixels in a plane
     *  @param  bits       number of bits per sample
     *  @param  rgb        flag, convert color model to RGB only if true
     */
    void convert(const T1 *pixel,
                 const unsigned long planeSize,
                 const int bits,
                 const OFBool rgb)
    {
        if (this->Init(pixel))
        {
            const T1 offset = OFstatic_cast(T1, DicomImageClass::maxval(bits - 1));
            // use the number of input pixels derived from the length of the 'PixelData'
            // attribute), but not more than the size of the intermediate buffer
            const unsigned long count = (this->InputCount < this->Count) ? this->InputCount : this->Count;
            if (rgb)    /* convert to RGB model */
            {
                T2 *r = this->Data[0];
                T2 *g = this->Data[1];
                T2 *b = this->Data[2];
                const T2 maxvalue = OFstatic_cast(T2, DicomImageClass::maxval(bits));
                DiPixelRepresentationTemplate<T1> rep;
                if (bits == 8 && !rep.isSigned())          // only for unsigned 8 bit
                {
                    Sint16 rcr_tab[256];
                    Sint16 gcb_tab[256];
                    Sint16 gcr_tab[256];
                    Sint16 bcb_tab[256];
                    const double r_const = 0.7010 * OFstatic_cast(double, maxvalue);
                    const double g_const = 0.5291 * OFstatic_cast(double, maxvalue);
                    const double b_const = 0.8859 * OFstatic_cast(double, maxvalue);
                    unsigned long l;
                    for (l = 0; l < 256; ++l)
                    {
                        rcr_tab[l] = OFstatic_cast(Sint16, 1.4020 * OFstatic_cast(double, l) - r_const);
                        gcb_tab[l] = OFstatic_cast(Sint16, 0.3441 * OFstatic_cast(double, l));
                        gcr_tab[l] = OFstatic_cast(Sint16, 0.7141 * OFstatic_cast(double, l) - g_const);
                        bcb_tab[l] = OFstatic_cast(Sint16, 1.7720 * OFstatic_cast(double, l) - b_const);
                    }
                    Sint32 sr;
                    Sint32 sg;
                    Sint32 sb;
                    if (this->PlanarConfiguration)
                    {
/*
                        const T1 *y = pixel;
                        const T1 *cb = y + this->InputCount;
                        const T1 *cr = cb + this->InputCount;
                        for (i = count; i != 0; --i, ++y, ++cb, ++cr)
                        {
                            sr = OFstatic_cast(Sint32, *y) + OFstatic_cast(Sint32, rcr_tab[*cr]);
                            sg = OFstatic_cast(Sint32, *y) - OFstatic_cast(Sint32, gcb_tab[*cb]) - OFstatic_cast(Sint32, gcr_tab[*cr]);
                            sb = OFstatic_cast(Sint32, *y) + OFstatic_cast(Sint32, bcb_tab[*cb]);
                            *(r++) = (sr < 0) ? 0 : (sr > OFstatic_cast(Sint32, maxvalue)) ? maxvalue : OFstatic_cast(T2, sr);
                            *(g++) = (sg < 0) ? 0 : (sg > OFstatic_cast(Sint32, maxvalue)) ? maxvalue : OFstatic_cast(T2, sg);
                            *(b++) = (sb < 0) ? 0 : (sb > OFstatic_cast(Sint32, maxvalue)) ? maxvalue : OFstatic_cast(T2, sb);
                        }
*/
                        const T1 *y = pixel;
                        const T1 *cb = y + planeSize;
                        const T1 *cr = cb + planeSize;
                        unsigned long i = count;
                        while (i != 0)
                        {
                            /* convert a single frame */
                            for (l = planeSize; (l != 0) && (i != 0); --l, --i, ++y, ++cb, ++cr)
                            {
                                sr = OFstatic_cast(Sint32, *y) + OFstatic_cast(Sint32, rcr_tab[OFstatic_cast(Uint32, *cr)]);
                                sg = OFstatic_cast(Sint32, *y) - OFstatic_cast(Sint32, gcb_tab[OFstatic_cast(Uint32, *cb)]) - OFstatic_cast(Sint32, gcr_tab[OFstatic_cast(Uint32, *cr)]);
                                sb = OFstatic_cast(Sint32, *y) + OFstatic_cast(Sint32, bcb_tab[OFstatic_cast(Uint32, *cb)]);
                                *(r++) = (sr < 0) ? 0 : (sr > OFstatic_cast(Sint32, maxvalue)) ? maxvalue : OFstatic_cast(T2, sr);
                                *(g++) = (sg < 0) ? 0 : (sg > OFstatic_cast(Sint32, maxvalue)) ? maxvalue : OFstatic_cast(T2, sg);
                                *(b++) = (sb < 0) ? 0 : (sb > OFstatic_cast(Sint32, maxvalue)) ? maxvalue : OFstatic_cast(T2, sb);
                            }
                            /* jump to next frame start (skip 2 planes) */
                            y += 2 * planeSize;
                            cb += 2 * planeSize;
                            cr += 2 * planeSize;
                        }
                    }
                    else
                    {
                        const T1 *p = pixel;
                        T1 y;
                        T1 cb;
                        T1 cr;
                        unsigned long i;
                        for (i = count; i != 0; --i)
                        {
                            y  = *(p++);
                            cb = *(p++);
                            cr = *(p++);
                            sr = OFstatic_cast(Sint32, y) + OFstatic_cast(Sint32, rcr_tab[OFstatic_cast(Uint32, cr)]);
                            sg = OFstatic_cast(Sint32, y) - OFstatic_cast(Sint32, gcb_tab[OFstatic_cast(Uint32, cb)]) - OFstatic_cast(Sint32, gcr_tab[OFstatic_cast(Uint32, cr)]);
                            sb = OFstatic_cast(Sint32, y) + OFstatic_cast(Sint32, bcb_tab[OFstatic_cast(Uint32, cb)]);
                            *(r++) = (sr < 0) ? 0 : (sr > OFstatic_cast(Sint32, maxvalue)) ? maxvalue : OFstatic_cast(T2, sr);
                            *(g++) = (sg < 0) ? 0 : (sg > OFstatic_cast(Sint32, maxvalue)) ? maxvalue : OFstatic_cast(T2, sg);
                            *(b++) = (sb < 0) ? 0 : (sb > OFstatic_cast(Sint32, maxvalue)) ? maxvalue : OFstatic_cast(T2, sb);
                        }
                    }
                }
                else
                {
                    if (this->PlanarConfiguration)
                    {
/*
                        const T1 *y = pixel;
                        const T1 *cb = y + this->InputCount;
                        const T1 *cr = cb + this->InputCount;
                        for (i = count; i != 0; --i)
                            convertValue(*(r++), *(g++), *(b++), removeSign(*(y++), offset), removeSign(*(cb++), offset),
                                removeSign(*(cr++), offset), maxvalue);
*/
                        unsigned long l;
                        unsigned long i = count;
                        const T1 *y = pixel;
                        const T1 *cb = y + planeSize;
                        const T1 *cr = cb + planeSize;
                        while (i != 0)
                        {
                            /* convert a single frame */
                            for (l = planeSize; (l != 0) && (i != 0); --l, --i)
                            {
                                convertValue(*(r++), *(g++), *(b++), removeSign(*(y++), offset), removeSign(*(cb++), offset),
                                    removeSign(*(cr++), offset), maxvalue);
                            }
                            /* jump to next frame start (skip 2 planes) */
                            y += 2 * planeSize;
                            cb += 2 * planeSize;
                            cr += 2 * planeSize;
                        }
                    }
                    else
                    {
                        const T1 *p = pixel;
                        T2 y;
                        T2 cb;
                        T2 cr;
                        unsigned long i;
                        for (i = count; i != 0; --i)
                        {
                            y = removeSign(*(p++), offset);
                            cb = removeSign(*(p++), offset);
                            cr = removeSign(*(p++), offset);
                            convertValue(*(r++), *(g++), *(b++), y, cb, cr, maxvalue);
                        }
                    }
                }
            } else {    /* retain YCbCr model */
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
    }

    /** convert a single YCbCr value to RGB
     */
    inline void convertValue(T2 &red, T2 &green, T2 &blue, const T2 y, const T2 cb, const T2 cr, const T2 maxvalue)
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
