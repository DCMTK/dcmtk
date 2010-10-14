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
 *  Purpose: DicomMonochrome2Image (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:18 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctypes.h"

#include "dcmtk/dcmimgle/dimo2img.h"
#include "dcmtk/dcmimgle/diutils.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMono2Image::DiMono2Image(const DiDocument *docu,
                           const EI_Status status)
  : DiMonoImage(docu, status)
{
}


DiMono2Image::DiMono2Image(const DiDocument *docu,
                           const EI_Status status,
                           const double slope,
                           const double intercept)
  : DiMonoImage(docu, status, slope, intercept)
{
}


DiMono2Image::DiMono2Image(const DiDocument *docu,
                           const EI_Status status,
                           const DcmUnsignedShort &data,
                           const DcmUnsignedShort &descriptor,
                           const DcmLongString *explanation)
  : DiMonoImage(docu, status, data, descriptor, explanation)
{
}


DiMono2Image::DiMono2Image(const DiDocument *docu,
                           const EI_Status status,
                           const char dummy)
  : DiMonoImage(docu, status, dummy)
{
}


DiMono2Image::DiMono2Image(const DiMonoImage *image,
                           const unsigned long fstart,
                           const unsigned long fcount)
  : DiMonoImage(image, fstart, fcount)
{
}


DiMono2Image::DiMono2Image(const DiColorImage *image,
                           const double red,
                           const double green,
                           const double blue)
  : DiMonoImage(image, red, green, blue)
{
}


DiMono2Image::DiMono2Image(const DiMonoImage *image,
                           const signed long left_pos,
                           const signed long top_pos,
                           const Uint16 src_cols,
                           const Uint16 src_rows,
                           const Uint16 dest_cols,
                           const Uint16 dest_rows,
                           const int interpolate,
                           const int aspect,
                           const Uint16 pvalue)
  : DiMonoImage(image, left_pos, top_pos, src_cols, src_rows, dest_cols, dest_rows, interpolate, aspect, pvalue)
{
}


DiMono2Image::DiMono2Image(const DiMonoImage *image,
                           const int horz,
                           const int vert)
  : DiMonoImage(image, horz, vert)
{
}


DiMono2Image::DiMono2Image(const DiMonoImage *image,
                           const int degree)
  : DiMonoImage(image, degree)
{
}


DiMono2Image::DiMono2Image(const DiMonoImage *image,
                           DiMonoOutputPixel *pixel,
                           const unsigned long frame,
                           const int stored,
                           const int alloc)
  : DiMonoImage(image, pixel, frame, stored, alloc)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMono2Image::~DiMono2Image()
{
}


/*********************************************************************/


const void *DiMono2Image::getOutputData(const unsigned long frame,
                                        const int bits,
                                        const int planar)
{
    return DiMonoImage::getData(NULL, 0, frame, bits, planar, 0);
}


int DiMono2Image::getOutputData(void *buffer,
                                const unsigned long size,
                                const unsigned long frame,
                                const int bits,
                                const int planar)
{
    return (DiMonoImage::getData(buffer, size, frame, bits, planar, 0) != NULL);
}


DiImage *DiMono2Image::createImage(const unsigned long fstart,
                                   const unsigned long fcount) const
{
    DiImage *image = new DiMono2Image(this, fstart, fcount);
    return image;
}


DiImage *DiMono2Image::createScale(const signed long left_pos,
                                   const signed long top_pos,
                                   const unsigned long src_cols,
                                   const unsigned long src_rows,
                                   const unsigned long dest_cols,
                                   const unsigned long dest_rows,
                                   const int interpolate,
                                   const int aspect,
                                   const Uint16 pvalue) const
{
    DiImage *image = new DiMono2Image(this, left_pos, top_pos, OFstatic_cast(Uint16, src_cols),
        OFstatic_cast(Uint16, src_rows), OFstatic_cast(Uint16, dest_cols), OFstatic_cast(Uint16, dest_rows),
        interpolate, aspect, pvalue);
    return image;
}


DiImage *DiMono2Image::createFlip(const int horz,
                                  const int vert) const
{
    DiImage *image = new DiMono2Image(this, horz, vert);
    return image;
}


DiImage *DiMono2Image::createRotate(const int degree) const
{
    DiImage *image = new DiMono2Image(this, degree);
    return image;
}


DiImage *DiMono2Image::createMono(const double,
                                  const double,
                                  const double) const
{
    return createImage(0, 0);
}


/*
 *
 * CVS/RCS Log:
 * $Log: dimo2img.cc,v $
 * Revision 1.15  2010-10-14 13:14:18  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.14  2005-12-08 15:42:55  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.13  2004/02/06 11:10:39  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.12  2003/12/08 17:38:27  joergr
 * Updated CVS header.
 *
 * Revision 1.11  2003/12/08 14:52:07  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.10  2002/12/09 13:34:51  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.9  2001/06/01 15:49:57  meichel
 * Updated copyright header
 *
 * Revision 1.8  2000/03/08 16:24:29  meichel
 * Updated copyright header.
 *
 * Revision 1.7  1999/10/06 13:45:55  joergr
 * Corrected creation of PrintBitmap pixel data: VOI windows should be applied
 * before clipping to avoid that the region outside the image (border) is also
 * windowed (this requires a new method in dcmimgle to create a DicomImage
 * with the grayscale transformations already applied).
 *
 * Revision 1.6  1999/08/25 16:43:07  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.5  1999/04/28 15:02:46  joergr
 * Added experimental support to create grayscale images with more than 256
 * shades of gray to be displayed on a consumer monitor (use pastel colors).
 *
 * Revision 1.4  1999/01/20 14:52:50  joergr
 * Added new output method to fill external memory buffer with rendered pixel
 * data.
 *
 * Revision 1.3  1998/12/16 16:12:36  joergr
 * Added explanation string to LUT class (retrieved from dataset).
 *
 * Revision 1.2  1998/12/14 17:35:58  joergr
 * Added support for presentation shapes.
 *
 * Revision 1.1  1998/11/27 16:07:34  joergr
 * Added copyright message.
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 *
 * Revision 1.5  1998/05/11 14:52:31  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
