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
 *  Purpose: DicomMonochrome1Image (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIMO1IMG_H
#define DIMO1IMG_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/dimoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for MONOCHROME1 images
 */
class DiMono1Image
  : public DiMonoImage
{

 public:

    /** constructor
     *
     ** @param  docu    pointer to dataset (encapsulated)
     *  @param  status  current image status
     */
    DiMono1Image(const DiDocument *docu,
                 const EI_Status status);

    /** destructor
     */
    virtual ~DiMono1Image();

    /** get color model of internal pixel representation.
     *  @return always returns EPI_Monochrome1
     */
    virtual EP_Interpretation getInternalColorModel() const
    {
        return EPI_Monochrome1;
    }

    /** get pixel data with specified format.
     *  (memory is handled internally)
     *
     ** @param  frame   number of frame to be rendered
     *  @param  bits    number of bits for the output pixel data (depth)
     *  @param  planar  flags, whether the output data (for multi-planar images) should be planar or not
     *
     ** @return untyped pointer to the pixel data if successful, NULL otherwise
     */
    virtual const void *getOutputData(const unsigned long frame,
                                      const int bits,
                                      const int planar = 0);

    /** get pixel data with specified format.
     *  (memory is handled externally)
     *
     ** @param  buffer  untyped pointer to the externally allocated memory buffer
     *  @param  size    size of the memory buffer in bytes (will be checked)
     *  @param  frame   number of frame to be rendered
     *  @param  bits    number of bits for the output pixel data (depth)
     *  @param  planar  flags, whether the output data (for multi-planar images) should be planar or not
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int getOutputData(void *buffer,
                              const unsigned long size,
                              const unsigned long frame,
                              const int bits,
                              const int planar = 0);

    /** create copy of current image object
     *
     ** @param  fstart  first frame to be processed
     *  @param  fcount  number of frames
     *
     ** @return pointer to new DicomImage object (NULL if an error occurred)
     */
    DiImage *createImage(const unsigned long fstart,
                         const unsigned long fcount) const;

    /** create scaled copy of specified (clipping) area of the current image object.
     *
     ** @param  left_pos      x coordinate of top left corner of area to be scaled
     *                        (referring to image origin, negative values create a border around the image)
     *  @param  top_pos       y coordinate of top left corner of area to be scaled
     *  @param  clip_width    width of area to be scaled
     *  @param  clip_height   height of area to be scaled
     *  @param  scale_width   width of scaled image (in pixels)
     *  @param  scale_height  height of scaled image (in pixels)
     *  @param  interpolate   specifies whether scaling algorithm should use interpolation (if necessary).
     *                        default: no interpolation (0), preferred interpolation algorithm (if applicable):
     *                          1 = pbmplus algorithm, 2 = c't algorithm, 3 = bilinear magnification,
     *                          4 = bicubic magnification
     *  @param  aspect        specifies whether pixel aspect ratio should be taken into consideration
     *                        (if true, width OR height should be 0, i.e. this component will be calculated
     *                         automatically)
     *  @param  pvalue        P-value used for the border outside the image (0..65535)
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    DiImage *createScale(const signed long left_pos,
                         const signed long top_pos,
                         const unsigned long clip_width,
                         const unsigned long clip_height,
                         const unsigned long scale_width,
                         const unsigned long scale_height,
                         const int interpolate,
                         const int aspect,
                         const Uint16 pvalue) const;

    /** create a flipped copy of the current image
     *
     ** @param  horz  flip horizontally if true
     *  @param  vert  flip vertically if true
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    DiImage *createFlip(const int horz,
                        const int vert) const;

    /** create a rotated copy of the current image.
     *
     ** @param  degree  angle by which the image shall be rotated
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    DiImage *createRotate(const int degree) const;

    /** create monochrome copy of the current image.
     *  Since the image is already monochrome the effect is the same as with createImage().
     *
     ** @param  dummy1  not used
     *  @param  dummy2  not used
     *  @param  dummy3  not used
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    DiImage *createMono(const double dummy1,
                        const double dummy2,
                        const double dummy3) const;


 protected:

    /** constructor, copy
     *
     ** @param  image   pointer to reference image
     *  @param  fstart  first frame to be processed
     *  @param  fcount  number of frames
     */
    DiMono1Image(const DiMonoImage *image,
                 const unsigned long fstart,
                 const unsigned long fcount);

    /** constructor, scale/clip
     *
     ** @param  image        pointer to reference image
     *  @param  left_pos     x coordinate of top left corner of area to be scaled
     *                       (referring to image origin, negative values create a border around the image)
     *  @param  top_pos      y coordinate of top left corner of area to be scaled
     *  @param  src_cols     width of area to be scaled
     *  @param  src_rows     height of area to be scaled
     *  @param  dest_cols    width of scaled image (in pixels)
     *  @param  dest_rows    height of scaled image (in pixels)
     *  @param  interpolate  specifies whether scaling algorithm should use interpolation (if necessary).
     *                       default: no interpolation (0), preferred interpolation algorithm (if applicable):
     *                         1 = pbmplus algorithm, 2 = c't algorithm, 3 = bilinear magnification,
     *                         4 = bicubic magnification
     *  @param  aspect       specifies whether pixel aspect ratio should be taken into consideration
     *                       (if true, width OR height should be 0, i.e. this component will be calculated
     *                       automatically)
     *  @param  pvalue       P-value used for the border outside the image (0..65535)
     */
    DiMono1Image(const DiMonoImage *image,
                 const signed long left_pos,
                 const signed long top_pos,
                 const Uint16 src_cols,
                 const Uint16 src_rows,
                 const Uint16 dest_cols,
                 const Uint16 dest_rows,
                 const int interpolate = 0,
                 const int aspect = 0,
                 const Uint16 pvalue = 0);

    /** constructor, flip
     *
     ** @param  image  pointer to reference image
     ** @param  horz   flip horizontally if true
     *  @param  vert   flip vertically if true
     */
    DiMono1Image(const DiMonoImage *image,
                 const int horz,
                 const int vert);

    /** constructor, rotate
     *
     ** @param  image   pointer to reference image
     *  @param  degree  angle by which the image shall be rotated
     */
    DiMono1Image(const DiMonoImage *image,
                 const int degree);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dimo1img.h,v $
 * Revision 1.17  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.16  2010-03-01 09:08:46  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.15  2008-05-20 10:03:53  joergr
 * Added new bilinear and bicubic scaling algorithms for image magnification.
 *
 * Revision 1.14  2005/12/08 16:47:46  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.13  2004/02/06 11:07:50  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.12  2003/12/08 18:25:12  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated CVS header.
 *
 * Revision 1.11  2003/06/12 15:08:34  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.10  2002/12/09 13:32:51  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.9  2001/11/27 18:18:23  joergr
 * Added support for plugable output formats in class DicomImage. First
 * implementation is JPEG.
 *
 * Revision 1.8  2001/11/09 16:28:03  joergr
 * Updated comments.
 *
 * Revision 1.7  2001/06/01 15:49:43  meichel
 * Updated copyright header
 *
 * Revision 1.6  2000/03/08 16:24:17  meichel
 * Updated copyright header.
 *
 * Revision 1.5  1999/09/17 12:22:54  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.4  1999/08/25 16:41:51  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.3  1999/03/24 17:20:05  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.2  1999/01/20 15:03:19  joergr
 * Added new output method to fill external memory buffer with rendered pixel
 * data.
 *
 * Revision 1.1  1998/11/27 15:12:17  joergr
 * Added copyright message.
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 *
 * Revision 1.2  1998/05/11 14:53:18  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
