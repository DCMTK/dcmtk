/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomMonochromeFlipTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIMOFLT_H
#define DIMOFLT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/dimopxt.h"
#include "dcmtk/dcmimgle/diflipt.h"


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
      : DiMonoPixelTemplate<T>(pixel, OFstatic_cast(unsigned long, columns) * OFstatic_cast(unsigned long, rows) * frames),
        DiFlipTemplate<T>(1, columns, rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if (pixel->getCount() == OFstatic_cast(unsigned long, columns) * OFstatic_cast(unsigned long, rows) * frames)
                flip(OFstatic_cast(const T *, pixel->getData()), horz, vert);
            else {
                DCMIMGLE_WARN("could not flip image ... corrupted data");
            }
        }
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
            this->Data = new T[this->getCount()];
            if (this->Data != NULL)
            {
                if (horz && vert)
                    flipHorzVert(&pixel, &this->Data);
                else if (horz)
                    flipHorz(&pixel, &this->Data);
                else if (vert)
                    flipVert(&pixel, &this->Data);
            }
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dimoflt.h,v $
 * Revision 1.15  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.14  2010-03-01 09:08:47  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.13  2009-10-28 14:38:16  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.12  2009-10-28 09:53:40  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.11  2006-08-15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.10  2005/12/08 16:47:49  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.9  2004/04/21 10:00:36  meichel
 * Minor modifications for compilation with gcc 3.4.0
 *
 * Revision 1.8  2003/12/08 18:41:55  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.7  2001/06/01 15:49:44  meichel
 * Updated copyright header
 *
 * Revision 1.6  2000/09/12 10:04:44  joergr
 * Corrected bug: wrong parameter for attribute search routine led to crashes
 * when multiple pixel data attributes were contained in the dataset (e.g.
 * IconImageSequence). Added new checking routines to avoid crashes when
 * processing corrupted image data.
 *
 * Revision 1.5  2000/03/08 16:24:18  meichel
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
