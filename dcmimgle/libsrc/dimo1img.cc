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
 *  Purpose: DicomMonochrome1Image (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-01-20 14:52:50 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/dimo1img.cc,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"

#include "dimo1img.h"
#include "diutils.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMono1Image::DiMono1Image(const DiDocument *docu,
                           const EI_Status status)
  : DiMonoImage(docu, status)
{
} 


DiMono1Image::DiMono1Image(const DiMonoImage *image,
                           const unsigned long fstart,
                           const unsigned long fcount)
  : DiMonoImage(image, fstart, fcount)
{
}


DiMono1Image::DiMono1Image(const DiMonoImage *image,
                           const Uint16 left,
                           const Uint16 top,
                           const Uint16 src_cols,
                           const Uint16 src_rows,                 
                           const Uint16 dest_cols,
                           const Uint16 dest_rows,
                           const int interpolate,
                           const int aspect)
  : DiMonoImage(image, left, top, src_cols, src_rows, dest_cols, dest_rows, interpolate, aspect)
{
}


DiMono1Image::DiMono1Image(const DiMonoImage *image,
                           const int horz,
                           const int vert)
  : DiMonoImage(image, horz, vert)
{
}


DiMono1Image::DiMono1Image(const DiMonoImage *image,
                           const int degree)
  : DiMonoImage(image, degree)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMono1Image::~DiMono1Image()
{
}


/*********************************************************************/


void *DiMono1Image::getOutputData(const unsigned long frame,
                                  const int bits,
                                  const int)
{
    return DiMonoImage::getData(NULL, 0, frame, bits, 1);
}


int DiMono1Image::getOutputData(void *buffer,
                                const unsigned long size,
                                const unsigned long frame,
                                const int bits,
                                const int)
{
    return (DiMonoImage::getData(buffer, size, frame, bits, 1) != NULL);
}


DiImage *DiMono1Image::createImage(const unsigned long fstart,
                                   const unsigned long fcount) const
{
    DiImage *image = new DiMono1Image(this, fstart, fcount);
    return image;
}


DiImage *DiMono1Image::createScale(const unsigned long left,
                                   const unsigned long top,
                                   const unsigned long src_cols,
                                   const unsigned long src_rows,                 
                                   const unsigned long dest_cols,
                                   const unsigned long dest_rows,
                                   const int interpolate,
                                   const int aspect) const
{
    DiImage *image = new DiMono1Image(this, (Uint16)left, (Uint16)top, (Uint16)src_cols, (Uint16)src_rows,
        (Uint16)dest_cols, (Uint16)dest_rows, interpolate, aspect);
    return image;
}


DiImage *DiMono1Image::createFlip(const int horz,
                                  const int vert) const
{
    DiImage *image = new DiMono1Image(this, horz, vert);
    return image;
}


DiImage *DiMono1Image::createRotate(const int degree) const
{
    DiImage *image = new DiMono1Image(this, degree);
    return image;
}


DiImage *DiMono1Image::createMono(const double,
                                  const double,
                                  const double) const
{
    DiImage *image = new DiMono1Image(this, (Uint16)Columns, (Uint16)Rows);
    return image;
}


/*
**
** CVS/RCS Log:
** $Log: dimo1img.cc,v $
** Revision 1.3  1999-01-20 14:52:50  joergr
** Added new output method to fill external memory buffer with rendered pixel
** data.
**
** Revision 1.2  1998/12/14 17:35:57  joergr
** Added support for presentation shapes.
**
** Revision 1.1  1998/11/27 16:07:33  joergr
** Added copyright message.
** Added methods and constructors for flipping and rotating, changed for
** scaling and clipping.
**
** Revision 1.5  1998/05/11 14:52:31  joergr
** Added CVS/RCS header to each file.
**
**
*/
