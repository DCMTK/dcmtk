/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:49:32 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diybrpxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
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

    DiYBRPixelTemplate(const DiDocument *docu,
                       const DiInputPixel *pixel,
                       EI_Status &status,
                       const int bits)
      : DiColorPixelTemplate<T2>(docu, pixel, 3, status)
    {
        if ((pixel != NULL) && (Count > 0) && (status == EIS_Normal))
            convert((const T1 *)pixel->getData() + pixel->getPixelStart() * 3, bits);
    }

    virtual ~DiYBRPixelTemplate()
    {
    }


 private:

    inline void convert(const T1 *pixel,
                        const int bits)
    {
        if (Init(pixel))
        {
            register T2 *r = Data[0];
            register T2 *g = Data[1];
            register T2 *b = Data[2];
            register unsigned long i;
            const T2 maxvalue = (T2)DicomImageClass::maxval(bits);
            const T1 offset = (T1)DicomImageClass::maxval(bits - 1);
            
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
                for (i = 0; i < 256; i++)
                {
                    rcr_tab[i] = (Sint16)(1.4020 * (double)i - r_const);
                    gcb_tab[i] = (Sint16)(0.3441 * (double)i);
                    gcr_tab[i] = (Sint16)(0.7141 * (double)i - g_const);
                    bcb_tab[i] = (Sint16)(1.7720 * (double)i - b_const);
                }
                register Sint32 sr;
                register Sint32 sg;
                register Sint32 sb;
                if (PlanarConfiguration)
                {
                    register const T1 *y = pixel;
                    register const T1 *cb = y + Count;
                    register const T1 *cr = cb + Count;
                    for (i = Count; i != 0; i--, y++, cb++, cr++)
                    {
                     	sr = (Sint32)*y + (Sint32)rcr_tab[*cr];
                    	sg = (Sint32)*y - (Sint32)gcb_tab[*cb] - (Sint32)gcr_tab[*cr];
                    	sb = (Sint32)*y + (Sint32)bcb_tab[*cb];
                        *(r++) = (sr < 0) ? 0 : (sr > (Sint32)maxvalue) ? maxvalue : (T2)sr;
                        *(g++) = (sg < 0) ? 0 : (sg > (Sint32)maxvalue) ? maxvalue : (T2)sg;
                        *(b++) = (sb < 0) ? 0 : (sb > (Sint32)maxvalue) ? maxvalue : (T2)sb;
                    }
                }
                else
                {
                    register const T1 *p = pixel;
                    register T1 y;
                    register T1 cb;
                    register T1 cr;
                    for (i = Count; i != 0; i--)
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
                    register const T1 *y = pixel;
                    register const T1 *cb = y + Count;
                    register const T1 *cr = cb + Count;
                    for (i = Count; i != 0; i--)
                        convertValue(*(r++), *(g++), *(b++), removeSign(*(y++), offset), removeSign(*(cb++), offset),
                            removeSign(*(cr++), offset), maxvalue);
                } 
                else
                {
                    register const T1 *p = pixel;
                    register T2 y;
                    register T2 cb;
                    register T2 cr;
                    for (i = Count; i != 0; i--)
                    {
                        y = removeSign(*(p++), offset); 
                        cb = removeSign(*(p++), offset);
                        cr = removeSign(*(p++), offset);
                        convertValue(*(r++), *(g++), *(b++), y, cb, cr, maxvalue);
                    }
                }
            }
        }
    }

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
 * Revision 1.10  2001-06-01 15:49:32  meichel
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
