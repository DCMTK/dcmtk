/*
 *
 *  Copyright (C) 1996-99, OFFIS
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
 *  Purpose: DicomPalettePixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 14:17:31 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dipalpxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIPALPXT_H
#define __DIPALPXT_H

#include "osconfig.h"

#include "dicopxt.h"
#include "diluptab.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T1, class T2>
class DiPalettePixelTemplate : public DiColorPixelTemplate<T2>
{
 public:
    DiPalettePixelTemplate(const DiDocument *docu, const DiInputPixel *pixel, DiLookupTable *pal[3], EI_Status &status)
      : DiColorPixelTemplate<T2>(docu, pixel, 1, status)
    {
        if ((pixel != NULL) && (getCount() > 0) && (status == EIS_Normal))
            convert((const T1 *)pixel->getData(), pal);
    }
    
    virtual ~DiPalettePixelTemplate()
    {
    }

 private:
    inline void convert(const T1 *pixel, DiLookupTable *pal[3])
    {
        if (Init(pixel))
        {
            register const T1 *p;
            register T1 value;
            register unsigned long i;
            if (PlanarConfiguration)
            {
                register T2 *q;
                register T1 min;
                register T1 max;
                for (int j = 0; j < 3; j++)
                {
                    p = pixel;
                    q = Data[j];
                    min = (T1)pal[j]->getFirstEntry();
                    max = (T1)pal[j]->getLastEntry();
                    for (i = 0; i < getCount(); i++)
                    {
                        value = *(p++);
                        if (value <= min)
                            *(q++) = (T2)pal[j]->getFirstValue();
                        else if (value >= max)
                            *(q++) = (T2)pal[j]->getLastValue();
                        else
                            *(q++) = (T2)pal[j]->getValue(value);
                    }
                }
            } 
            else
            {
                p = pixel;
                register int j;
                for (i = 0; i < getCount(); i++)
                {
                    value = *(p++);
                    for (j = 0; j < 3; j++)
                    {
                        if (value <= (T1)pal[j]->getFirstEntry())
                            Data[j][i] = (T2)pal[j]->getFirstValue();
                        else if (value >= (T1)pal[j]->getLastEntry())
                            Data[j][i] = (T2)pal[j]->getLastValue();
                        else
                            Data[j][i] = (T2)pal[j]->getValue(value);
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
** $Log: dipalpxt.h,v $
** Revision 1.5  1998-11-27 14:17:31  joergr
** Added copyright message.
**
** Revision 1.4  1998/07/01 08:39:27  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.3  1998/05/11 14:53:27  joergr
** Added CVS/RCS header to each file.
**
**
*/

