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
 *  Purpose: DicomMonochromeScaleTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-09-12 10:04:45 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimosct.h,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
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

/** Template class to scale monochrome images (on pixel data level).
 */
template<class T>
class DiMonoScaleTemplate
  : public DiMonoPixelTemplate<T>,
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
     *  @param  interpolate  use of interpolation when scaling
     *  @param  pvalue       value possibly used for regions outside the image boundaries
     */     
    DiMonoScaleTemplate(const DiMonoPixel *pixel,
                        const Uint16 columns,
                        const Uint16 rows,
                        const signed long left,
                        const signed long top,
                        const Uint16 src_cols,
                        const Uint16 src_rows,
                        const Uint16 dest_cols,
                        const Uint16 dest_rows,
                        const Uint32 frames,
                        const int interpolate,
                        const Uint16 pvalue)
      : DiMonoPixelTemplate<T>(pixel, (unsigned long)dest_cols * (unsigned long)dest_rows * frames),
        DiScaleTemplate<T>(1, columns, rows, left, top, src_cols, src_rows, dest_cols, dest_rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if (pixel->getCount() == (unsigned long)columns * (unsigned long)rows * frames)
            {
                scale((const T *)pixel->getData(), pixel->getBits(), interpolate, pvalue);
                determineMinMax();
            } else {
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
    virtual ~DiMonoScaleTemplate()
    {
    }
    

 private:

    /** scale pixel data
     *
     ** @param  pixel        pointer to pixel data to be scaled
     *  @param  bits         bit depth of pixel data
     *  @param  interpolate  use of interpolation when scaling
     *  @param  pvalue       value possibly used for regions outside the image boundaries
     */
    inline void scale(const T *pixel,
                      const unsigned int bits,
                      const int interpolate,
                      const Uint16 pvalue)
    {
        if (pixel != NULL)
        {
            Data = new T[getCount()];
            if (Data != NULL)
            {
                const T value = (T)((double)DicomImageClass::maxval(bits) * (double)pvalue /
                    (double)DicomImageClass::maxval(WIDTH_OF_PVALUES));
                scaleData(&pixel, &Data, interpolate, value);
             }
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dimosct.h,v $
 * Revision 1.9  2000-09-12 10:04:45  joergr
 * Corrected bug: wrong parameter for attribute search routine led to crashes
 * when multiple pixel data attributes were contained in the dataset (e.g.
 * IconImageSequence). Added new checking routines to avoid crashes when
 * processing corrupted image data.
 *
 * Revision 1.8  2000/03/08 16:24:21  meichel
 * Updated copyright header.
 *
 * Revision 1.7  1999/09/17 12:43:24  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.6  1999/09/07 09:49:39  joergr
 * Removed register variable declaration to avoid compiler warnings.
 *
 * Revision 1.5  1999/08/25 16:41:54  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.4  1999/03/24 17:20:18  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.3  1999/02/11 16:41:10  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.2  1998/12/16 16:36:11  joergr
 * *** empty log message ***
 *
 * Revision 1.1  1998/11/27 15:39:32  joergr
 * Added copyright message.
 * Combined clipping and scaling methods.
 *
 * Revision 1.4  1998/05/11 14:53:24  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
