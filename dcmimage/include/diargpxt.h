/*
 *
 *  Copyright (C) 1996-2000, OFFIS
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
 *  Purpose: DicomARGBPixelTemplate (Header) - UNTESTED !!!
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:21:48 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diargpxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIARGPXT_H
#define __DIARGPXT_H

#include "osconfig.h"

#include "dicopxt.h"
#include "diluptab.h"


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

    DiARGBPixelTemplate(const DiDocument *docu,
                        const DiInputPixel *pixel,
                        DiLookupTable *palette[3],
                        EI_Status &status,
                        const int bits)
      : DiColorPixelTemplate<T3>(docu, pixel, 4, status)
    {
        if ((pixel != NULL) && (Count > 0) && (status == EIS_Normal))
            convert((const T1 *)pixel->getData(), palette, bits);
    }
    
    virtual ~DiARGBPixelTemplate()
    {
    }


 private:

    inline void convert(const T1 *pixel,
                        DiLookupTable *pal[3],
                        const int bits)
    {                                                                           // not very much optimized, but no one uses ARGB !!
        if (Init(pixel))
        {
            register unsigned long i;
            register T2 value;
            register int j;
            const T1 offset = (T1)DicomImageClass::maxval(bits - 1);
            if (PlanarConfiguration)
            {
                register const T1 *a = pixel;                                   // points to alpha plane
                const T1 *rgb[3];
                rgb[0] = a + Count;                                             // points to red plane
                rgb[1] = rgb[0] + Count;                                        // points to green plane
                rgb[2] = rgb[1] + Count;                                        // points to blue plane
                for (i = 0; i < Count; i++)
                {
                    value = (T2)(*(a++));                                       // get alpha value
                    if (value > 0)
                    {
                        for (j = 0; j < 3; j++)                                 // set palette color
                        {
                            if (value <= pal[j]->getFirstEntry(value))
                                Data[j][i] = (T3)pal[j]->getFirstValue();
                            else if (value >= pal[j]->getLastEntry(value))
                                Data[j][i] = (T3)pal[j]->getLastValue();
                            else
                                Data[j][i] = (T3)pal[j]->getValue(value);
                            rgb[j]++;                                           // skip RGB values
                        }
                    }
                    else
                    {
                        for (j = 0; j < 3; j++)
                            Data[j][i] = (T3)removeSign(*(rgb[j]++), offset);   // copy RGB values
                    }
                }
            } 
            else
            {
                register const T1 *p = pixel;
                for (i = 0; i < Count; i++)
                {
                    value = (T2)(*(p++));                                       // get alpha value
                    if (value > 0)
                    {
                        for (j = 0; j < 3; j++)                                 // set palette color
                        {
                            if (value <= pal[j]->getFirstEntry(value))
                                Data[j][i] = (T3)pal[j]->getFirstValue();
                            else if (value >= pal[j]->getLastEntry(value))
                                Data[j][i] = (T3)pal[j]->getLastValue();
                            else
                                Data[j][i] = (T3)pal[j]->getValue(value);
                        }
                        p += 3;                                                 // skip RGB values
                    }
                    else
                    {
                        for (j = 0; j < 3; j++)
                            Data[j][i] = (T3)removeSign(*(p++), offset);        // copy RGB values
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
 * $Log: diargpxt.h,v $
 * Revision 1.11  2000-03-08 16:21:48  meichel
 * Updated copyright header.
 *
 * Revision 1.10  1999/04/28 12:51:54  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.9  1999/02/03 16:47:54  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 *
 * Revision 1.8  1999/01/20 14:36:22  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.7  1998/12/14 17:08:43  joergr
 * Added support for signed values as second entry in look-up tables
 * (= first value mapped).
 *
 * Revision 1.6  1998/11/27 13:40:11  joergr
 * Added copyright message.
 *
 * Revision 1.5  1998/07/01 08:39:18  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.4  1998/05/11 14:53:08  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
