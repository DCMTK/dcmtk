/*
**
** Author:  Joerg Riesmeier
** Created: 13.03.98
**
** Module:  diyf2pxt.h
**
** Purpose: DicomYBR422PixelTemplate (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:33 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diyf2pxt.h,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIYF2PXT_H
#define __DIYF2PXT_H

#include "osconfig.h"

#include "dicopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T1, class T2>
class DiYBR422PixelTemplate : public DiColorPixelTemplate<T2>
{
 public:
    DiYBR422PixelTemplate(const DiDocument *docu, const DiInputPixel *pixel, EI_Status &status, const int bits)
      : DiColorPixelTemplate<T2>(docu, pixel, 3, status, 2)
    {
        if ((pixel != NULL) && (getCount() > 0) && (status == EIS_Normal))
        {
            if (PlanarConfiguration)
            {
                status = EIS_InvalidValue;
                cerr << "ERROR: invalid value for 'PlanarConfiguration' (" << PlanarConfiguration << ") ! " << endl;
            }
            else
                convert((const T1 *)pixel->getData(), bits);
        }   
    }

    virtual ~DiYBR422PixelTemplate()
    {
    }

 private:
    inline void convert(const T1 *pixel, const int bits)
    {
        if (Init(pixel))
        {
            register T2 *r = Data[0];
            register T2 *g = Data[1];
            register T2 *b = Data[2];
            register unsigned long i;
            const T2 maxvalue = (T2)maxval(bits);
            const T1 offset = (T1)maxval(bits - 1);
            
            register const T1 *p = pixel;
            register T2 y1;
            register T2 y2;
            register T2 cb;
            register T2 cr;
            for (i = 0; i < getCount() / 2; i++)
            {
                y1 = removeSign(*(p++), offset); 
                y2 = removeSign(*(p++), offset);
                cb = removeSign(*(p++), offset);
                cr = removeSign(*(p++), offset);
                convertValue(*(r++), *(g++), *(b++), y1, cb, cr, maxvalue);
                convertValue(*(r++), *(g++), *(b++), y2, cb, cr, maxvalue);
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
**
** CVS/RCS Log:
** $Log: diyf2pxt.h,v $
** Revision 1.2  1998-05-11 14:53:33  joergr
** Added CVS/RCS header to each file.
**
**
*/
