/*
**
** Author:  Joerg Riesmeier
** Created: 05.01.97
**
** Module:  dicmypxt.h
**
** Purpose: DicomCMYKPixelTemplate (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:10 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicmypxt.h,v $
** CVS/RCS Revision: $Revision: 1.4 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DICMYPXT_H
#define __DICMYPXT_H

#include "osconfig.h"

#include "dicopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T1, class T2>
class DiCMYKPixelTemplate : public DiColorPixelTemplate<T2>
{
 public:
    DiCMYKPixelTemplate(const DiDocument *docu, const DiInputPixel *pixel, EI_Status &status, const int bits)
      : DiColorPixelTemplate<T2>(docu, pixel, 4, status)
    {
        if ((pixel != NULL) && (getCount() > 0) && (status == EIS_Normal))
            convert((const T1 *)pixel->getData(), bits);
    }
    
    virtual ~DiCMYKPixelTemplate()
    {
    }

 private:
    inline void convert(const T1 *pixel, const int bits)
    {
        if (Init(pixel))
        {
            register const T1 *p = pixel;
            register unsigned long i;
            const T2 maxvalue = (T2)maxval(bits);
            const T1 offset = (T1)maxval(bits - 1);
            if (PlanarConfiguration)
            {
                register const T1 *k;
                register T2 *q;
                for (int j = 0; j < 3; j++)
                {
                    q = Data[j];
                    k = pixel + 3 * getCount();                     // beginning of 'black' plane
                    for (i = 0; i < getCount(); i++)
                        *(q++) = maxvalue - removeSign(*(p++), offset) - removeSign(*(k++), offset);
                }
            } 
            else
            {
                register T1 k;
                register int j;
                for (i = 0; i < getCount(); i++)
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
**
** CVS/RCS Log:
** $Log: dicmypxt.h,v $
** Revision 1.4  1998-05-11 14:53:10  joergr
** Added CVS/RCS header to each file.
**
**
*/
