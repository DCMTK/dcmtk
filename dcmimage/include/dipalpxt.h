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
 *  Update Date:      $Date: 1999-04-28 12:52:01 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dipalpxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
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

/** Template class to handle Palette color pixel data
 */
template<class T1, class T2, class T3>
class DiPalettePixelTemplate
  : public DiColorPixelTemplate<T3>
{

 public:

    DiPalettePixelTemplate(const DiDocument *docu,
                           const DiInputPixel *pixel,
                           DiLookupTable *pal[3],
                           EI_Status &status)
      : DiColorPixelTemplate<T3>(docu, pixel, 1, status)
    {
        if ((pixel != NULL) && (Count > 0) && (status == EIS_Normal))
            convert((const T1 *)pixel->getData(), pal);
    }
    
    virtual ~DiPalettePixelTemplate()
    {
    }


 private:

    inline void convert(const T1 *pixel,
                        DiLookupTable *pal[3])
    {                                                                // can be optimized if necessary !
        if (Init(pixel))
        {
            register const T1 *p;
            register T2 value;
            register unsigned long i;
            if (PlanarConfiguration)
            {
                register T3 *q;
                register T2 min;
                register T2 max;
                for (int j = 0; j < 3; j++)
                {
                    p = pixel;
                    q = Data[j];
                    min = pal[j]->getFirstEntry(value);
                    max = pal[j]->getLastEntry(value);
                    const T3 minvalue = (T3)pal[j]->getFirstValue();
                    const T3 maxvalue = (T3)pal[j]->getLastValue();
                    for (i = 0; i < Count; i++)
                    {
                        value = (T2)(*(p++));
                        if (value <= min)
                            *(q++) = minvalue;
                        else if (value >= max)
                            *(q++) = maxvalue;
                        else
                            *(q++) = (T3)pal[j]->getValue(value);
                    }
                }
            } 
            else
            {
                p = pixel;
                register int j;
                for (i = 0; i < Count; i++)
                {
                    value = (T2)(*(p++));
                    for (j = 0; j < 3; j++)
                    {
                        if (value <= pal[j]->getFirstEntry(value))
                            Data[j][i] = (T3)pal[j]->getFirstValue();
                        else if (value >= pal[j]->getLastEntry(value))
                            Data[j][i] = (T3)pal[j]->getLastValue();
                        else
                            Data[j][i] = (T3)pal[j]->getValue(value);
                    }
                }
            }
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dipalpxt.h,v $
 * Revision 1.8  1999-04-28 12:52:01  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.7  1999/01/20 14:46:30  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.6  1998/12/14 17:08:56  joergr
 * Added support for signed values as second entry in look-up tables
 * (= first value mapped).
 *
 * Revision 1.5  1998/11/27 14:17:31  joergr
 * Added copyright message.
 *
 * Revision 1.4  1998/07/01 08:39:27  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.3  1998/05/11 14:53:27  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
