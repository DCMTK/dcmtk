/*
 *
 *  Copyright (C) 1996-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomYBRPixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-06-26 16:20:19 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diybrpxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIYBRPXT_H
#define __DIYBRPXT_H

#include "osconfig.h"

#include "dicopxt.h"
#include "dipxrept.h"


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
        if ((pixel != NULL) && (Count > 0) && (status == EIS_Normal))
            convert((const T1 *)pixel->getData() + pixel->getPixelStart(), planeSize, bits, rgb);
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
        if (Init(pixel))
        {
            const T1 offset = (T1)DicomImageClass::maxval(bits - 1);
            // use the number of input pixels derived from the length of the 'PixelData'
            // attribute), but not more than the size of the intermediate buffer
            const unsigned long count = (InputCount < Count) ? InputCount : Count;
            if (rgb)    /* convert to RGB model */
            {
                register T2 *r = Data[0];
                register T2 *g = Data[1];
                register T2 *b = Data[2];
                const T2 maxvalue = (T2)DicomImageClass::maxval(bits);

                DiPixelRepresentationTemplate<T1> rep;
                if (bits == 8 && !rep.isSigned())          // only for unsigned 8 bit
                {
                    Sint16 rcr_tab[256];
                    Sint16 gcb_tab[256];
                    Sint16 gcr_tab[256];
                    Sint16 bcb_tab[256];
                    const double r_const = 0.7010 * double(maxvalue);
                    const double g_const = 0.5291 * double(maxvalue);
                    const double b_const = 0.8859 * double(maxvalue);
                    register unsigned long l;
                    for (l = 0; l < 256; l++)
                    {
                        rcr_tab[l] = (Sint16)(1.4020 * (double)l - r_const);
                        gcb_tab[l] = (Sint16)(0.3441 * (double)l);
                        gcr_tab[l] = (Sint16)(0.7141 * (double)l - g_const);
                        bcb_tab[l] = (Sint16)(1.7720 * (double)l - b_const);
                    }
                    register Sint32 sr;
                    register Sint32 sg;
                    register Sint32 sb;
                    if (PlanarConfiguration)
                    {
/*
                        register const T1 *y = pixel;
                        register const T1 *cb = y + InputCount;
                        register const T1 *cr = cb + InputCount;
                        for (i = count; i != 0; i--, y++, cb++, cr++)
                        {
                         	sr = (Sint32)*y + (Sint32)rcr_tab[*cr];
                        	sg = (Sint32)*y - (Sint32)gcb_tab[*cb] - (Sint32)gcr_tab[*cr];
                        	sb = (Sint32)*y + (Sint32)bcb_tab[*cb];
                            *(r++) = (sr < 0) ? 0 : (sr > (Sint32)maxvalue) ? maxvalue : (T2)sr;
                            *(g++) = (sg < 0) ? 0 : (sg > (Sint32)maxvalue) ? maxvalue : (T2)sg;
                            *(b++) = (sb < 0) ? 0 : (sb > (Sint32)maxvalue) ? maxvalue : (T2)sb;
                        }
*/
                        register const T1 *y = pixel;
                        register const T1 *cb = y + planeSize;
                        register const T1 *cr = cb + planeSize;
                        register unsigned long i = count;
                        while (i != 0)
                        {
                            /* convert a single frame */
                            for (l = planeSize; (l != 0) && (i != 0); l--, i--, y++, cb++, cr++)
                            {
                             	sr = (Sint32)*y + (Sint32)rcr_tab[*cr];
                            	sg = (Sint32)*y - (Sint32)gcb_tab[*cb] - (Sint32)gcr_tab[*cr];
                            	sb = (Sint32)*y + (Sint32)bcb_tab[*cb];
                                *(r++) = (sr < 0) ? 0 : (sr > (Sint32)maxvalue) ? maxvalue : (T2)sr;
                                *(g++) = (sg < 0) ? 0 : (sg > (Sint32)maxvalue) ? maxvalue : (T2)sg;
                                *(b++) = (sb < 0) ? 0 : (sb > (Sint32)maxvalue) ? maxvalue : (T2)sb;
                            }
                            /* jump to next frame start (skip 2 planes) */
                            y += 2 * planeSize;
                            cb += 2 * planeSize;
                            cr += 2 * planeSize;
                        }
                    }
                    else
                    {
                        register const T1 *p = pixel;
                        register T1 y;
                        register T1 cb;
                        register T1 cr;
                        register unsigned long i;
                        for (i = count; i != 0; i--)
                        {
                            y  = *(p++);
                            cb = *(p++);
                            cr = *(p++);
                        	sr = (Sint32)y + (Sint32)rcr_tab[cr];
                        	sg = (Sint32)y - (Sint32)gcb_tab[cb] - (Sint32)gcr_tab[cr];
                        	sb = (Sint32)y + (Sint32)bcb_tab[cb];
                            *(r++) = (sr < 0) ? 0 : (sr > (Sint32)maxvalue) ? maxvalue : (T2)sr;
                            *(g++) = (sg < 0) ? 0 : (sg > (Sint32)maxvalue) ? maxvalue : (T2)sg;
                            *(b++) = (sb < 0) ? 0 : (sb > (Sint32)maxvalue) ? maxvalue : (T2)sb;
                        }
                    }
                }
                else
                {
                    if (PlanarConfiguration)
                    {
/*
                        register const T1 *y = pixel;
                        register const T1 *cb = y + InputCount;
                        register const T1 *cr = cb + InputCount;
                        for (i = count; i != 0; i--)
                            convertValue(*(r++), *(g++), *(b++), removeSign(*(y++), offset), removeSign(*(cb++), offset),
                                removeSign(*(cr++), offset), maxvalue);
*/
                        register unsigned long l;
                        register unsigned long i = count;
                        register const T1 *y = pixel;
                        register const T1 *cb = y + planeSize;
                        register const T1 *cr = cb + planeSize;
                        while (i != 0)
                        {
                            /* convert a single frame */
                            for (l = planeSize; (l != 0) && (i != 0); l--, i--)
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
                        register const T1 *p = pixel;
                        register T2 y;
                        register T2 cb;
                        register T2 cr;
                        register unsigned long i;
                        for (i = count; i != 0; i--)
                        {
                            y = removeSign(*(p++), offset);
                            cb = removeSign(*(p++), offset);
                            cr = removeSign(*(p++), offset);
                            convertValue(*(r++), *(g++), *(b++), y, cb, cr, maxvalue);
                        }
                    }
                }
            } else {    /* retain YCbCr model */
                register const T1 *p = pixel;
                if (PlanarConfiguration)
                {
/*
                    register T2 *q;
                    // number of pixels to be skipped (only applicable if 'PixelData' contains more
                    // pixels than expected)
                    const unsigned long skip = (InputCount > Count) ? (InputCount - Count) : 0;
                    for (int j = 0; j < 3; j++)
                    {
                        q = Data[j];
                        for (i = count; i != 0; i--)
                            *(q++) = removeSign(*(p++), offset);
                        // skip to beginning of next plane
                        p += skip;
                    }
*/
                    register unsigned long l;
                    register unsigned long i = 0;
                    while (i < count)
                    {
                        /* store current pixel index */
                        const unsigned long iStart = i;
                        for (int j = 0; j < 3; j++)
                        {
                            /* convert a single plane */
                            for (l = planeSize, i = iStart; (l != 0) && (i < count); l--, i++)
                                Data[j][i] = removeSign(*(p++), offset);
                        }
                    }
                }
                else
                {
                    register int j;
                    register unsigned long i;
                    for (i = 0; i < count; i++)                         /* for all pixel ... */
                        for (j = 0; j < 3; j++)
                            Data[j][i] = removeSign(*(p++), offset);    /* ... copy planes */
                }
            }
        }
    }

    /** convert a single YCbCr value to RGB
     */
    inline void convertValue(T2 &red, T2 &green, T2 &blue, const T2 y, const T2 cb, const T2 cr, const T2 maxvalue)
    {
        double dr = (double)y + 1.4020 * (double)cr - 0.7010 * double(maxvalue);
        double dg = (double)y - 0.3441 * (double)cb - 0.7141 * (double)cr + 0.5291 * double(maxvalue);
        double db = (double)y + 1.7720 * (double)cb - 0.8859 * double(maxvalue);
        red   = (dr < 0.0) ? 0 : (dr > (double)maxvalue) ? maxvalue : (T2)dr;
        green = (dg < 0.0) ? 0 : (dg > (double)maxvalue) ? maxvalue : (T2)dg;
        blue  = (db < 0.0) ? 0 : (db > (double)maxvalue) ? maxvalue : (T2)db;
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diybrpxt.h,v $
 * Revision 1.13  2002-06-26 16:20:19  joergr
 * Enhanced handling of corrupted pixel data and/or length.
 * Corrected decoding of multi-frame, planar images.
 *
 * Revision 1.12  2001/11/09 16:47:03  joergr
 * Removed 'inline' specifier from certain methods.
 *
 * Revision 1.11  2001/09/28 13:55:41  joergr
 * Added new flag (CIF_KeepYCbCrColorModel) which avoids conversion of YCbCr
 * color models to RGB.
 *
 * Revision 1.10  2001/06/01 15:49:32  meichel
 * Updated copyright header
 *
 * Revision 1.9  2000/04/27 13:15:15  joergr
 * Dcmimage library code now consistently uses ofConsole for error output.
 *
 * Revision 1.8  2000/03/08 16:21:54  meichel
 * Updated copyright header.
 *
 * Revision 1.7  1999/09/17 14:03:46  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.6  1999/04/28 12:52:04  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.5  1999/02/03 16:55:29  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 *
 * Revision 1.4  1999/01/20 14:47:20  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.3  1998/11/27 14:18:56  joergr
 * Added copyright message.
 *
 * Revision 1.2  1998/05/11 14:53:32  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
