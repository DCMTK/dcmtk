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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorFlipTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 14:59:36 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicoflt.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICOFLT_H
#define __DICOFLT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dicopxt.h"
#include "diflipt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiColorFlipTemplate
  : public DiColorPixelTemplate<T>,
    protected DiFlipTemplate<T>
{

 public:

    DiColorFlipTemplate(const DiColorPixel *pixel,
                        const Uint16 columns,
                        const Uint16 rows,
                        const Uint32 frames,
                        const int horz,
                        const int vert)
      : DiColorPixelTemplate<T>(pixel, (unsigned long)columns * (unsigned long)rows * frames),
        DiFlipTemplate<T>(3, columns, rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
            flip((const T **)pixel->getData(), horz, vert);
    }

    ~DiColorFlipTemplate()
    {
    }


 private:

    inline void flip(const T *pixel[3], const int horz, const int vert)
    {
        if (Init(pixel))
        {
            if (horz && vert)
                flipHorzVert(pixel, Data);
            else if (horz)
                flipHorz(pixel, Data);
            else if (vert)
                flipVert(pixel, Data);
        }
    }
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dicoflt.h,v $
** Revision 1.1  1998-11-27 14:59:36  joergr
** Added copyright message.
** Added methods and classes for flipping and rotating, changed for
** scaling and clipping.
**
** Revision 1.4  1998/05/11 14:53:19  joergr
** Added CVS/RCS header to each file.
**
**
*/
