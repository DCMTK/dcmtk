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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorRotateTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-11-09 16:45:00 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicorot.h,v $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICOROT_H
#define __DICOROT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dicopxt.h"
#include "dirotat.h"


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
      : DiColorPixelTemplate<T>(pixel, (unsigned long)dest_cols * (unsigned long)dest_rows * frames),
        DiRotateTemplate<T>(3, src_cols, src_rows, dest_cols, dest_rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if (pixel->getCount() == (unsigned long)src_cols * (unsigned long)src_rows * frames)
                rotate((const T **)pixel->getData(), degree);
            else {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                   ofConsole.lockCerr() << "WARNING: could not rotate image ... corrupted data." << endl;
                   ofConsole.unlockCerr();
                }
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
                rotateRight(pixel, Data);
            else if (degree == 180)
                rotateTopDown(pixel, Data);
            else  if (degree == 270)
                rotateLeft(pixel, Data);
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicorot.h,v $
 * Revision 1.6  2001-11-09 16:45:00  joergr
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
 * 
 */
