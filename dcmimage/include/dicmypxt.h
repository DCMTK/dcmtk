/*
 *
 *  Copyright (C) 1997-99, OFFIS
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
 *  Purpose: DicomCMYKPixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-09-17 14:03:42 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicmypxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICMYPXT_H
#define __DICMYPXT_H

#include "osconfig.h"

#include "dicopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle CMYK pixel data
 */
template<class T1, class T2>
class DiCMYKPixelTemplate
  : public DiColorPixelTemplate<T2>
{

 public:

    DiCMYKPixelTemplate(const DiDocument *docu,
                        const DiInputPixel *pixel,
                        EI_Status &status,
                        const int bits)
      : DiColorPixelTemplate<T2>(docu, pixel, 4, status)
    {
        if ((pixel != NULL) && (Count > 0) && (status == EIS_Normal))
            convert((const T1 *)pixel->getData(), bits);
    }
    
    virtual ~DiCMYKPixelTemplate()
    {
    }


 private:

    inline void convert(const T1 *pixel,
                        const int bits)
    {
        if (Init(pixel))
        {
            register const T1 *p = pixel;
            register unsigned long i;
            const T2 maxvalue = (T2)DicomImageClass::maxval(bits);
            const T1 offset = (T1)DicomImageClass::maxval(bits - 1);
            if (PlanarConfiguration)
            {
                register const T1 *k;
                register T2 *q;
                for (int j = 0; j < 3; j++)
                {
                    q = Data[j];
                    k = pixel + 3 * Count;                     // beginning of 'black' plane
                    for (i = Count; i != 0; i--)
                        *(q++) = maxvalue - removeSign(*(p++), offset) - removeSign(*(k++), offset);
                }
            } 
            else
            {
                register T1 k;
                register int j;
                for (i = 0; i < Count; i++)
                {
                    k = *(p + 3);
                    for (j = 0; j < 3; j++)
                        Data[j][i] = maxvalue - removeSign(*(p++), offset) - removeSign(k, offset);
                    p++;                                            // skip 'black'
                }
            }
        }
    }
};


#endif
                        

/*
 *
 * CVS/RCS Log:
 * $Log: dicmypxt.h,v $
 * Revision 1.9  1999-09-17 14:03:42  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.8  1999/04/28 12:51:56  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.7  1999/02/03 16:48:32  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 *
 * Revision 1.6  1999/01/20 14:37:05  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.5  1998/11/27 13:41:04  joergr
 * Added copyright message.
 *
 * Revision 1.4  1998/05/11 14:53:10  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
