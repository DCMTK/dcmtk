/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Update Date:      $Date: 2001-11-09 16:45:23 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicosct.h,v $
 *  CVS/RCS Revision: $Revision: 1.11 $
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

    /** constructor
     *
     ** @param  pixel        pointer to intermediate pixel representation
     *  @param  columns      width of source image
     *  @param  rows         height of source image
     *  @param  left         left coordinate of clipping area
     *  @param  top          top coordinate of clipping area
     *  @param  src_cols     width of clipping area
     *  @param  src_rows     height of clipping area
     *  @param  dest_cols    width of destination image (scaled image)
     *  @param  dest_rows    height of destination image
     *  @param  frames       number of frames 
     *  @param  bits         number of bits per plane/pixel
     *  @param  interpolate  use of interpolation when scaling
     */     
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
        {
            if (pixel->getCount() == (unsigned long)columns * (unsigned long)rows * frames)
                scale((const T **)pixel->getData(), interpolate);
            else {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                   ofConsole.lockCerr() << "WARNING: could not scale image ... corrupted data." << endl;
                   ofConsole.unlockCerr();
                }
            }
        }
    }
    
    /** destructor
     */
    virtual ~DiColorScaleTemplate()
    {
    }


 private:

    /** scale pixel data
     *
     ** @param  pixel        pointer to pixel data (3 components9 to be scaled
     *  @param  interpolate  use of interpolation when scaling
     */
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
 * Revision 1.11  2001-11-09 16:45:23  joergr
 * Updated/Enhanced comments.
 *
 * Revision 1.10  2001/06/01 15:49:30  meichel
 * Updated copyright header
 *
 * Revision 1.9  2000/12/08 14:06:02  joergr
 * Added new checking routines to avoid crashes when processing corrupted image
 * data.
 *
 * Revision 1.8  2000/03/08 16:21:52  meichel
 * Updated copyright header.
 *
 * Revision 1.7  1999/08/25 16:58:06  joergr
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
