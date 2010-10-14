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
 *  Purpose: DicomMonochromeRotateTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIMOROT_H
#define DIMOROT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/dimopxt.h"
#include "dcmtk/dcmimgle/dirotat.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to rotate monochrome images (on pixel data level).
 *  by steps of 90 degrees
 */
template<class T>
class DiMonoRotateTemplate
  : public DiMonoPixelTemplate<T>,
    protected DiRotateTemplate<T>
{

 public:

    /** constructor
     *
     ** @param  pixel      pointer to intermediate pixel representation
     *  @param  src_cols   width of clipping area
     *  @param  src_rows   height of clipping area
     *  @param  dest_cols  width of destination image (scaled image)
     *  @param  dest_rows  height of destination image
     *  @param  frames     number of frames
     *  @param  degree     angle by which the pixel data should be rotated
     */
    DiMonoRotateTemplate(const DiMonoPixel *pixel,
                         const Uint16 src_cols,
                         const Uint16 src_rows,
                         const Uint16 dest_cols,
                         const Uint16 dest_rows,
                         const Uint32 frames,
                         const int degree)
      : DiMonoPixelTemplate<T>(pixel, OFstatic_cast(unsigned long, dest_cols) * OFstatic_cast(unsigned long, dest_rows) * frames),
        DiRotateTemplate<T>(1, src_cols, src_rows, dest_cols, dest_rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if (pixel->getCount() == OFstatic_cast(unsigned long, src_cols) * OFstatic_cast(unsigned long, src_rows) * frames)
                rotate(OFstatic_cast(const T *, pixel->getData()), degree);
            else {
                DCMIMGLE_WARN("could not rotate image ... corrupted data");
            }
        }
    }

    /** destructor
     */
    ~DiMonoRotateTemplate()
    {
    }


 private:

    /** rotate pixel data
     *
     ** @param  pixel   pointer to pixel data to be rotated
     *  @param  degree  angle by which the pixel data should be rotated
     */
    inline void rotate(const T *pixel,
                       const int degree)
    {
        if (pixel != NULL)
        {
            this->Data = new T[DiMonoPixelTemplate<T>::getCount()];
            if (this->Data != NULL)
            {
                if (degree == 90)
                    rotateRight(&pixel, &(this->Data));
                else if (degree == 180)
                    rotateTopDown(&pixel, &(this->Data));
                else if (degree == 270)
                    rotateLeft(&pixel, &(this->Data));
            }
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dimorot.h,v $
 * Revision 1.16  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.15  2010-03-01 09:08:47  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.14  2009-10-28 14:38:17  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.13  2009-10-28 09:53:40  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.12  2006-08-15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.11  2005/12/08 16:47:57  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.10  2004/04/21 10:00:36  meichel
 * Minor modifications for compilation with gcc 3.4.0
 *
 * Revision 1.9  2003/12/08 18:47:49  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.8  2001/06/01 15:49:48  meichel
 * Updated copyright header
 *
 * Revision 1.7  2000/09/12 10:04:45  joergr
 * Corrected bug: wrong parameter for attribute search routine led to crashes
 * when multiple pixel data attributes were contained in the dataset (e.g.
 * IconImageSequence). Added new checking routines to avoid crashes when
 * processing corrupted image data.
 *
 * Revision 1.6  2000/03/08 16:24:21  meichel
 * Updated copyright header.
 *
 * Revision 1.5  1999/09/17 12:43:23  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.4  1999/03/24 17:20:17  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.3  1999/02/11 16:40:46  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.2  1998/12/14 17:26:26  joergr
 * Corrected a typo.
 *
 * Revision 1.1  1998/11/27 14:57:48  joergr
 * Added copyright message.
 * Added methods and classes for flipping and rotating, changed for
 * scaling and clipping.
 *
 *
 */
