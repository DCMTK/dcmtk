/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomMonochromeRotateTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 14:57:48 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimorot.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIMOROT_H
#define __DIMOROT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimopxt.h"
#include "dirotat.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiMonoRotateTemplate
  : public DiMonoPixelTemplate<T>,
    protected DiRotateTemplate<T>
{

 public:

    DiMonoRotateTemplate(const DiMonoPixel *pixel,
                         const Uint16 src_cols,
                         const Uint16 src_rows,
                         const Uint16 dest_cols,
                         const Uint16 dest_rows,
                         const Uint32 frames,
                         const int degree)
      : DiMonoPixelTemplate<T>(pixel, (unsigned long)dest_cols * (unsigned long)dest_rows * frames),
        DiRotateTemplate<T>(1, src_cols, src_rows, dest_cols, dest_rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
            rotate((const T *)pixel->getData(), degree);
    }

    ~DiMonoRotateTemplate()
    {
    }


 private:

    inline void rotate(const T *pixel, const int degree)
    {
        if (pixel != NULL)
        {
            Data = new T[getCount()];
            if (Data != NULL)
            {
                if (degree == 90)
                    rotateRight(&pixel, &Data);
                else if (degree == 180)
                    rotateTopDown(&pixel, &Data);
                else  if (degree == 270)
                    rotateLeft(&pixel, &Data);
            }
        }
    }
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimorot.h,v $
** Revision 1.1  1998-11-27 14:57:48  joergr
** Added copyright message.
** Added methods and classes for flipping and rotating, changed for
** scaling and clipping.
**
**
**
*/
