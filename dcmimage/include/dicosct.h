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
 *  Purpose: DicomColorScaleTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-08-25 16:58:06 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicosct.h,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICOSCT_H
#define __DICOSCT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dicopx.h"
#include "discalet.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to scale color images (on pixel data level).
 */
template<class T>
class DiColorScaleTemplate
  : public DiColorPixelTemplate<T>,
    protected DiScaleTemplate<T>
{

 public:

    DiColorScaleTemplate(const DiColorPixel *pixel,
                         const Uint16 columns,
                         const Uint16 rows,
                         const signed long left,
                         const signed long top,
                         const Uint16 src_cols,
                         const Uint16 src_rows,
                         const Uint16 dest_cols,
                         const Uint16 dest_rows,
                         const Uint32 frames,
                         const int bits,
                         const int interpolate)
      : DiColorPixelTemplate<T>(pixel, (unsigned long)dest_cols * (unsigned long)dest_rows * frames),
        DiScaleTemplate<T>(3, columns, rows, left, top, src_cols, src_rows, dest_cols, dest_rows, frames, bits)
   {
        if ((pixel != NULL) && (pixel->getCount() > 0))
            scale((const T **)pixel->getData(), interpolate);
    }
    
    virtual ~DiColorScaleTemplate()
    {
    }


 private:

    inline void scale(const T *pixel[3],
                      const int interpolate)
    {
        if (Init(pixel))
            scaleData(pixel, Data, interpolate);
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicosct.h,v $
 * Revision 1.7  1999-08-25 16:58:06  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.6  1999/04/28 12:52:00  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.5  1998/11/27 13:51:07  joergr
 * Added copyright message.
 *
 * Revision 1.4  1998/05/11 14:53:14  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
