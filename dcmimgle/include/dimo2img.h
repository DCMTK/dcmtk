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
 *  Purpose: DicomMonochrome2Image (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 15:12:18 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimo2img.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIMO2IMG_H
#define __DIMO2IMG_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for MONOCHROME2 images
 *
 */
class DiMono2Image
  : public DiMonoImage
{

 public:

    DiMono2Image(const DiDocument *docu,
                 const EI_Status status);
                 
    DiMono2Image(const DiDocument *docu,
                 const EI_Status status,
                 const double slope,
                 const double intercept);

    DiMono2Image(const DiDocument *docu,
                 const EI_Status status,
                 const DcmUnsignedShort &data,
                 const DcmUnsignedShort &descriptor);

    DiMono2Image(const DiColorImage *image,
                 const double red,
                 const double green,
                 const double blue);
                 
    virtual ~DiMono2Image();

    void *getOutputData(const unsigned long,
                        const int,
                        const int = 0);

    /** Method: 
     *  @param fstart
     *  @param fcount
     */
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

    DiMono2Image(const DiDocument *image,
                 const EI_Status status,
                 const char dummy);
                 
    DiMono2Image(const DiMonoImage *image,
                 const unsigned long fstart,
                 const unsigned long fcount);

    DiMono2Image(const DiMonoImage *image,
                 const Uint16 left,
                 const Uint16 top,
                 const Uint16 src_cols,
                 const Uint16 src_rows,                 
                 const Uint16 dest_cols,
                 const Uint16 dest_rows,
                 const int interpolate = 0,
                 const int aspect = 0);
                 
    DiMono2Image(const DiMonoImage *image,
                 const int horz,
                 const int vert);

    DiMono2Image(const DiMonoImage *image,
                 const int degree);
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimo2img.h,v $
** Revision 1.1  1998-11-27 15:12:18  joergr
** Added copyright message.
** Added methods and constructors for flipping and rotating, changed for
** scaling and clipping.
**
** Revision 1.2  1998/05/11 14:53:19  joergr
** Added CVS/RCS header to each file.
**
**
*/
