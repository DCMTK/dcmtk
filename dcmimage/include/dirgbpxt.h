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
 *  Purpose: DicomRGBPixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 14:18:18 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dirgbpxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIRGBPXT_H
#define __DIRGBPXT_H

#include "osconfig.h"

#include "dicopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T1, class T2>
class DiRGBPixelTemplate : public DiColorPixelTemplate<T2>
{
 public:
    DiRGBPixelTemplate(const DiDocument *docu, const DiInputPixel *pixel, EI_Status &status, const int bits)
      : DiColorPixelTemplate<T2>(docu, pixel, 3, status)
    {
        if ((pixel != NULL) && (getCount() > 0) && (status == EIS_Normal))
            convert((const T1 *)pixel->getData(), bits);
    }
    
    virtual ~DiRGBPixelTemplate()
    {
    }

 private:
    inline void convert(const T1 *pixel, const int bits)
    {
        if (Init(pixel))
        {
            register const T1 *p = pixel;
            register unsigned long i;
            const T1 offset = (T1)maxval(bits - 1);
            if (PlanarConfiguration)
            {
                register T2 *q;
                for (int j = 0; j < 3; j++)                         /* for all planes ... */
                {
                    q = Data[j];
                    for (i = 0; i < getCount(); i++)
                        *(q++) = removeSign(*(p++), offset);        /* ... copy pixel */
                }
            } 
            else
            {
                register int j;
                for (i = 0; i < getCount(); i++)                    /* for all pixel ... */
                    for (j = 0; j < 3; j++)
                        Data[j][i] = removeSign(*(p++), offset);    /* ... copy planes */ 
            }
        }
    }

};


#endif


/*
**
** CVS/RCS Log:
** $Log: dirgbpxt.h,v $
** Revision 1.5  1998-11-27 14:18:18  joergr
** Added copyright message.
**
** Revision 1.4  1998/05/11 14:53:29  joergr
** Added CVS/RCS header to each file.
**
**
*/
