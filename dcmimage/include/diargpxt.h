/*
**
** Author:  Joerg Riesmeier
** Created: 06.01.97
**
** Module:  diargpxt.h
**
** Purpose: DicomARGBPixelTemplate (Header) - UNTESTED !!!
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-07-01 08:39:18 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diargpxt.h,v $
** CVS/RCS Revision: $Revision: 1.5 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIARGPXT_H
#define __DIARGPXT_H

#include "osconfig.h"

#include "dicopxt.h"
#include "diluptab.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T1, class T2>
class DiARGBPixelTemplate : public DiColorPixelTemplate<T2>
{
 public:
    DiARGBPixelTemplate(const DiDocument *docu, const DiInputPixel *pixel, DiLookupTable *palette[3],
        EI_Status &status, const int bits)
      : DiColorPixelTemplate<T2>(docu, pixel, 4, status)
    {
        if ((pixel != NULL) && (getCount() > 0) && (status == EIS_Normal))
            convert((const T1 *)pixel->getData(), palette, bits);
    }
    
    virtual ~DiARGBPixelTemplate()
    {
    }

 private:
    inline void convert(const T1 *pixel, DiLookupTable *pal[3], const int bits)
    {
        if (Init(pixel))
        {
            register unsigned long i;
            register T1 value;
            register int j;
            const T1 offset = (T1)maxval(bits - 1);
            if (PlanarConfiguration)
            {
                register const T1 *a = pixel;                                   // points to alpha plane
                const T1 *rgb[3];
                rgb[0] = a + getCount();                                        // points to red plane
                rgb[1] = rgb[0] + getCount();                                   // points to green plane
                rgb[2] = rgb[1] + getCount();                                   // points to blue plane
                for (i = 0; i < getCount(); i++)
                {
                    value = *(a++);                                             // get alpha value
                    if (value > 0)
                    {
                        for (j = 0; j < 3; j++)                                 // set palette color
                        {
                            if ((Sint32)value <= pal[j]->getFirstEntry())
                                Data[j][i] = (T2)pal[j]->getFirstValue();
                            else if ((Sint32)value >= pal[j]->getLastEntry())
                                Data[j][i] = (T2)pal[j]->getLastValue();
                            else
                                Data[j][i] = (T2)pal[j]->getValue(value);
                            rgb[j]++;                                           // skip RGB values
                        }
                    }
                    else
                    {
                        for (j = 0; j < 3; j++)
                            Data[j][i] = (T2)removeSign(*(rgb[j]++), offset);   // copy RGB values
                    }
                }
            } 
            else
            {
                register const T1 *p = pixel;
                for (i = 0; i < getCount(); i++)
                {
                    value = *(p++);                                             // get alpha value
                    if (value > 0)
                    {
                        for (j = 0; j < 3; j++)                                 // set palette color
                        {
                            if ((Sint32)value <= pal[j]->getFirstEntry())
                                Data[j][i] = (T2)pal[j]->getFirstValue();
                            else if ((Sint32)value >= pal[j]->getLastEntry())
                                Data[j][i] = (T2)pal[j]->getLastValue();
                            else
                                Data[j][i] = (T2)pal[j]->getValue(value);
                        }
                        p += 3;                                                 // skip RGB values
                    }
                    else
                    {
                        for (j = 0; j < 3; j++)
                            Data[j][i] = (T2)removeSign(*(p++), offset);        // copy RGB values
                    }
                }
            }
        }
    }

};


#endif
                        

/*
**
** CVS/RCS Log:
** $Log: diargpxt.h,v $
** Revision 1.5  1998-07-01 08:39:18  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.4  1998/05/11 14:53:08  joergr
** Added CVS/RCS header to each file.
**
**
*/
