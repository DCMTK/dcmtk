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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomMonochromeFlipTemplate (Header)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:24:18 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimoflt.h,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 * 
 */


#ifndef __DIMOFLT_H
#define __DIMOFLT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimopxt.h"
#include "diflipt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to flip monochrome images (on pixel data level)
 *  horizontally and vertically
 */
template<class T>
class DiMonoFlipTemplate
  : public DiMonoPixelTemplate<T>,
    protected DiFlipTemplate<T>
{

 public:

    /** constructor
     *
     ** @param  pixel    pointer to intermediate pixel representation
     *  @param  columns  number of columns
     *  @param  rows     number of rows
     *  @param  frames   number of frames
     *  @param  horz     flip horizontally if true
     *  @param  vert     flip vertically if true
     */
    DiMonoFlipTemplate(const DiMonoPixel *pixel,
                       const Uint16 columns,
                       const Uint16 rows,
                       const Uint32 frames,
                       const int horz,
                       const int vert)
      : DiMonoPixelTemplate<T>(pixel, (unsigned long)columns * (unsigned long)rows * frames),
        DiFlipTemplate<T>(1, columns, rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
            flip((const T *)pixel->getData(), horz, vert);
    }

    /** destructor
     */
    ~DiMonoFlipTemplate()
    {
    }


 private:

    /** choose flipping algorithm depending on given parameters
     *
     ** @param  pixel  pointer to pixel data which should be flipped
     *  @param  horz   flip horizontally if true
     *  @param  vert   flip vertically if true
     */
    inline void flip(const T *pixel,
                     const int horz,
                     const int vert)
    {
        if (pixel != NULL)
        {
            Data = new T[getCount()];
            if (Data != NULL)
            {
                if (horz && vert)
                    flipHorzVert(&pixel, &Data);
                else if (horz)
                    flipHorz(&pixel, &Data);
                else if (vert)
                    flipVert(&pixel, &Data);
            }
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dimoflt.h,v $
 * Revision 1.5  2000-03-08 16:24:18  meichel
 * Updated copyright header.
 *
 * Revision 1.4  1999/09/17 12:24:46  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.3  1999/03/24 17:20:08  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.2  1999/02/11 16:02:12  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.1  1998/11/27 14:57:47  joergr
 * Added copyright message.
 * Added methods and classes for flipping and rotating, changed for
 * scaling and clipping.
 *
 *
 */
