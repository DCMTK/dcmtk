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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomMonochromeScaleTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-12-16 16:36:11 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimosct.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIMOSCT_H
#define __DIMOSCT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimopxt.h"
#include "discalet.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiMonoScaleTemplate
  : public DiMonoPixelTemplate<T>,
    protected DiScaleTemplate<T>
{

 public:

    DiMonoScaleTemplate(const DiMonoPixel *pixel,
                        const Uint16 columns,
                        const Uint16 rows,
                        const Uint16 left,
                        const Uint16 top,
                        const Uint16 src_cols,
                        const Uint16 src_rows,
                        const Uint16 dest_cols,
                        const Uint16 dest_rows,
                        const Uint32 frames,
                        const int interpolate)
      : DiMonoPixelTemplate<T>(pixel, (unsigned long)dest_cols * (unsigned long)dest_rows * frames),
        DiScaleTemplate<T>(1, columns, rows, left, top, src_cols, src_rows, dest_cols, dest_rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            scale((const T *)pixel->getData(), interpolate);
            determineMinMax();
        }
    }

    virtual ~DiMonoScaleTemplate()
    {
    }
    

 private:

    inline void scale(const T *pixel,
                      const int interpolate)
    {
        if (pixel != NULL)
        {
            Data = new T[getCount()];
            if (Data != NULL)
                scaleData(&pixel, &Data, interpolate);
        }
    }
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimosct.h,v $
** Revision 1.2  1998-12-16 16:36:11  joergr
** *** empty log message ***
**
** Revision 1.1  1998/11/27 15:39:32  joergr
** Added copyright message.
** Combined clipping and scaling methods.
**
** Revision 1.4  1998/05/11 14:53:24  joergr
** Added CVS/RCS header to each file.
**
**
*/
