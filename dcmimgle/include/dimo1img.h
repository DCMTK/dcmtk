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
 *  Purpose: DicomMonochrome1Image (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-03-24 17:20:05 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimo1img.h,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIMO1IMG_H
#define __DIMO1IMG_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for MONOCHROME1 images
 */
class DiMono1Image
  : public DiMonoImage
{

 public:

    DiMono1Image(const DiDocument *image,
                 const EI_Status status);

    virtual ~DiMono1Image();

    virtual void *getOutputData(const unsigned long frame,
                                const int bits,
                                const int planar = 0);

    virtual int getOutputData(void *buffer,
                              const unsigned long size,
                              const unsigned long frame,
                              const int bits,
                              const int planar = 0);

    DiImage *createImage(const unsigned long fstart,
                         const unsigned long fcount) const;

    DiImage *createScale(const unsigned long left,
                         const unsigned long top,
                         const unsigned long clip_width,
                         const unsigned long clip_height,
                         const unsigned long scale_width,
                         const unsigned long scale_height,
                         const int interpolate,
                         const int aspect) const;

    DiImage *createFlip(const int horz,
                        const int vert) const;

    DiImage *createRotate(const int degree) const;

    DiImage *createMono(const double,
                        const double,
                        const double) const;
    

 protected:

    DiMono1Image(const DiMonoImage *image,
                 const unsigned long fstart,
                 const unsigned long fcount);

    DiMono1Image(const DiMonoImage *image,
                 const Uint16 left,
                 const Uint16 top,
                 const Uint16 src_cols,
                 const Uint16 src_rows,                 
                 const Uint16 dest_cols,
                 const Uint16 dest_rows,
                 const int interpolate = 0,
                 const int aspect = 0);

    DiMono1Image(const DiMonoImage *image,
                 const int horz,
                 const int vert);

    DiMono1Image(const DiMonoImage *image,
                 const int degree);
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimo1img.h,v $
** Revision 1.3  1999-03-24 17:20:05  joergr
** Added/Modified comments and formatting.
**
** Revision 1.2  1999/01/20 15:03:19  joergr
** Added new output method to fill external memory buffer with rendered pixel
** data.
**
** Revision 1.1  1998/11/27 15:12:17  joergr
** Added copyright message.
** Added methods and constructors for flipping and rotating, changed for
** scaling and clipping.
**
** Revision 1.2  1998/05/11 14:53:18  joergr
** Added CVS/RCS header to each file.
**
**
*/
