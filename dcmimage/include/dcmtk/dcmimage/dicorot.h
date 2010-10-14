/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Purpose: DicomColorRotateTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:29 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DICOROT_H
#define DICOROT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicopxt.h"
#include "dcmtk/dcmimgle/dirotat.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to rotate color images (on pixel data level).
 *  by steps of 90 degrees
 */
template<class T>
class DiColorRotateTemplate
  : public DiColorPixelTemplate<T>,
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
    DiColorRotateTemplate(const DiColorPixel *pixel,
                          const Uint16 src_cols,
                          const Uint16 src_rows,
                          const Uint16 dest_cols,
                          const Uint16 dest_rows,
                          const Uint32 frames,
                          const int degree)
      : DiColorPixelTemplate<T>(pixel, OFstatic_cast(unsigned long, dest_cols) * OFstatic_cast(unsigned long, dest_rows) * frames),
        DiRotateTemplate<T>(3, src_cols, src_rows, dest_cols, dest_rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if (pixel->getCount() == OFstatic_cast(unsigned long, src_cols) * OFstatic_cast(unsigned long, src_rows) * frames)
                rotate(OFstatic_cast(const T **, OFconst_cast(void *, pixel->getData())), degree);
            else {
                DCMIMAGE_WARN("could not rotate image ... corrupted data");
            }
        }
    }

    /** destructor
     */
    ~DiColorRotateTemplate()
    {
    }


 private:

    /** rotate pixel data
     *
     ** @param  pixel   pointer to pixel data (3 components) to be rotated
     *  @param  degree  angle by which the pixel data should be rotated
     */
    inline void rotate(const T *pixel[3],
                       const int degree)
    {
        if (Init(pixel))
        {
            if (degree == 90)
                rotateRight(pixel, this->Data);
            else if (degree == 180)
                rotateTopDown(pixel, this->Data);
            else  if (degree == 270)
                rotateLeft(pixel, this->Data);
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicorot.h,v $
 * Revision 1.15  2010-10-14 13:16:29  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.14  2010-03-01 09:08:46  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.13  2009-10-14 10:25:13  joergr
 * Fixed minor issues in log output. Also updated copyright date (if required).
 *
 * Revision 1.12  2009-10-13 14:08:33  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.11  2006-08-15 16:35:01  meichel
 * Updated the code in module dcmimage to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.10  2005/12/08 16:01:36  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.9  2004/04/21 10:00:31  meichel
 * Minor modifications for compilation with gcc 3.4.0
 *
 * Revision 1.8  2004/02/06 11:18:18  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.7  2003/12/23 11:44:32  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.6  2001/11/09 16:45:00  joergr
 * Updated/Enhanced comments.
 *
 * Revision 1.5  2001/06/01 15:49:30  meichel
 * Updated copyright header
 *
 * Revision 1.4  2000/12/08 14:06:01  joergr
 * Added new checking routines to avoid crashes when processing corrupted image
 * data.
 *
 * Revision 1.3  2000/03/08 16:21:51  meichel
 * Updated copyright header.
 *
 * Revision 1.2  1999/04/28 12:51:59  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.1  1998/11/27 14:59:36  joergr
 * Added copyright message.
 * Added methods and classes for flipping and rotating, changed for
 * scaling and clipping.
 *
 *
 */
