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
 *  Purpose: DicomMonochrome1Image (Source)
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctypes.h"

#include "dcmtk/dcmimgle/dimo1img.h"
#include "dcmtk/dcmimgle/diutils.h"


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


const void *DiMono1Image::getOutputData(const unsigned long frame,
                                        const int bits,
                                        const int planar)
{
    return DiMonoImage::getData(NULL, 0, frame, bits, planar, 1);
}


int DiMono1Image::getOutputData(void *buffer,
                                const unsigned long size,
                                const unsigned long frame,
                                const int bits,
                                const int planar)
{
    return (DiMonoImage::getData(buffer, size, frame, bits, planar, 1) != NULL);
}


DiImage *DiMono1Image::createImage(const unsigned long fstart,
                                   const unsigned long fcount) const
{
    DiImage *image = new DiMono1Image(this, fstart, fcount);
    return image;
}


DiImage *DiMono1Image::createScale(const signed long left_pos,
                                   const signed long top_pos,
                                   const unsigned long src_cols,
                                   const unsigned long src_rows,
                                   const unsigned long dest_cols,
                                   const unsigned long dest_rows,
                                   const int interpolate,
                                   const int aspect,
                                   const Uint16 pvalue) const
{
    DiImage *image = new DiMono1Image(this, left_pos, top_pos, OFstatic_cast(Uint16, src_cols),
        OFstatic_cast(Uint16, src_rows), OFstatic_cast(Uint16, dest_cols), OFstatic_cast(Uint16, dest_rows),
        interpolate, aspect, pvalue);
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
    return createImage(0, 0);
}
