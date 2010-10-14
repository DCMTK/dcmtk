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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorImage (Header)
 *
 *  Last Update:         $Author: joergr $
 *  Update Date:         $Date: 2010-10-14 13:16:29 $
 *  CVS/RCS Revision:    $Revision: 1.30 $
 *  Status:              $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DICOIMG_H
#define DICOIMG_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/diimage.h"
#include "dcmtk/dcmimage/dicopx.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiColorOutputPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Base class for color images
 */
class DiColorImage
  : public DiImage
{

 public:

    /** constructor
     *
     ** @param  docu    pointer to the DICOM document
     *  @param  status  status of the image object
     *  @param  spp     sample per pixel (1, 3 or 4 depending on the color model)
     *  @param  rgb     specifies whether internal image representation is in RGB format
     */
    DiColorImage(const DiDocument *docu,
                 const EI_Status status,
                 const int spp,
                 const OFBool rgb = OFTrue);

    /** destructor
     */
    virtual ~DiColorImage();

    /** get color model of internal pixel representation.
     *
     ** @return returns EPI_RGB or EPI_YBR_Full depending on the 'rgb' flag in the constructor
     */
    virtual EP_Interpretation getInternalColorModel() const
    {
        return (RGBColorModel) ? EPI_RGB : EPI_YBR_Full;
    }

    /** get number of bytes required for the rendered output of a single frame
     *
     *  @param  bits  number of bits for the output pixel data (depth)
     *
     ** @return number of bytes if successful, 0 otherwise
     */
    virtual unsigned long getOutputDataSize(const int bits = 0) const;

    /** get pixel data with specified format.
     *  (memory is handled internally)
     *  The standard color model of the pixel data is RGB, but if the flag "rgb" is OFFalse
     *  and the original color model was YCbCr_Full or YCbCr_Full_422 YCbCr_Full is used instead.
     *
     ** @param  frame   number of frame to be rendered
     *  @param  bits    number of bits per sample for the output pixel data (depth)
     *  @param  planar  0 = color-by-pixel (R1G1B1...R2G2B2...R3G3B3...),
     *                  1 = color-by-plane (R1R2R3...G1G2G3...B1B2B3...)
     *
     ** @return untyped pointer to the pixel data if successful, NULL otherwise
     */
    const void *getOutputData(const unsigned long frame,
                              const int bits,
                              const int planar = 0);

    /** get pixel data with specified format.
     *  (memory is handled externally)
     *  The standard color model of the pixel data is RGB, but if the flag "rgb" is OFFalse
     *  and the original color model was YCbCr_Full or YCbCr_Full_422 YCbCr_Full is used instead.
     *
     ** @param  buffer  untyped pointer to the externally allocated memory buffer
     *  @param  size    size of the memory buffer in bytes (will be checked)
     *  @param  frame   number of frame to be rendered
     *  @param  bits    number of bits per sample for the output pixel data (depth)
     *  @param  planar  0 = color-by-pixel (R1G1B1...R2G2B2...R3G3B3...)
     *                  1 = color-by-plane (R1R2R3...G1G2G3...B1B2B3...)
     *
     ** @return status, true if successful, false otherwise
     */
    int getOutputData(void *buffer,
                      const unsigned long size,
                      const unsigned long frame,
                      const int bits,
                      const int planar = 0);

    /** get pixel data of specified plane.
     *  (memory is handled internally)
     *
     ** @param  plane  index of color plane
     *
     ** @return untyped pointer to the pixel data if successful, NULL otherwise
     */
    const void *getOutputPlane(const int plane) const;

    /** delete internally handled output memory buffer
     *  Save memory if data is no longer needed.
     */
    void deleteOutputData();

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
     *  @param  src_cols      width of area to be scaled
     *  @param  src_rows      height of area to be scaled
     *  @param  dest_cols     width of scaled image (in pixels)
     *  @param  dest_rows     height of scaled image (in pixels)
     *  @param  interpolate   specifies whether scaling algorithm should use interpolation (if necessary).
     *                        default: no interpolation (0), preferred interpolation algorithm (if applicable):
     *                          1 = pbmplus algorithm, 2 = c't algorithm, 3 = bilinear magnification,
     *                          4 = bicubic magnification
     *  @param  aspect        specifies whether pixel aspect ratio should be taken into consideration
     *                        (if true, width OR height should be 0, i.e. this component will be calculated
     *                         automatically)
     *  @param  pvalue        dummy parameter (only used for monochrome images)
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    DiImage *createScale(const signed long left_pos,
                         const signed long top_pos,
                         const unsigned long src_cols,
                         const unsigned long src_rows,
                         const unsigned long dest_cols,
                         const unsigned long dest_rows,
                         const int interpolate,
                         const int aspect,
                         const Uint16 pvalue) const;

    /** flip current image (horizontally and/or vertically)
     *
     ** @param  horz  flip horizontally if true
     *  @param  vert  flip vertically if true
     *
     ** @return true if successful (1 = flipped at least direction,
     *                              2 = not flipped, because of image resolution - width and/or height equal to 1),
     *          false otherwise
     */
    int flip(const int horz,
             const int vert);

    /** create a flipped copy of the current image
     *
     ** @param  horz  flip horizontally if true
     *  @param  vert  flip vertically if true
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    DiImage *createFlip(const int horz,
                        const int vert) const;

    /** rotate current image (by steps of 90 degrees)
     *
     ** @param  degree  angle by which the image shall be rotated (-360, -270, -180, -90, 0, 90, 180, 270, 360)
     *
     ** @return true if successful (1 = rotated by at least 90 degrees,
     *                              2 = not rotated, because of image resolution or angle),
     *          false otherwise
     */
    int rotate(const int degree);

    /** create a rotated copy of the current image.
     *
     ** @param  degree  angle by which the image shall be rotated
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    DiImage *createRotate(const int degree) const;

    /** create monochrome copy of the current image
     *
     ** @param  red    coefficient by which the red component is weighted
     *  @param  green  coefficient by which the green component is weighted
     *  @param  blue   coefficient by which the blue component is weighted
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    DiImage *createMono(const double red,
                        const double green,
                        const double blue) const;

    /** get pointer to intermediate pixel data representation
     *
     ** @return pointer to intermediate pixel data
     */
    const DiPixel *getInterData() const
    {
        return InterData;
    }

    /** get pointer to intermediate pixel data representation
     *
     ** @return pointer to intermediate pixel data
     */
    const DiColorPixel *getColorInterData() const
    {
        return InterData;
    }

    /** create true color (24/32 bit) bitmap for MS Windows.
     *  memory is not handled internally - must be deleted from calling program.
     *
     ** @param  data        untyped pointer memory buffer (set to NULL if not allocated externally)
     *  @param  size        size of the memory buffer in bytes (if 0 'data' is set to NULL)
     *  @param  frame       index of frame to be converted (starting from 0)
     *  @param  bits        number of bits per pixel used for the output bitmap (24 or 32)
     *  @param  upsideDown  specifies the order of lines in the images (0 = top-down, bottom-up otherwise)
     *  @param  padding     align each line to a 32-bit address if true (default)
     *
     ** @return number of bytes allocated by the bitmap, or 0 if an error occured
     */
    unsigned long createDIB(void *&data,
                            const unsigned long size,
                            const unsigned long frame,
                            const int bits,
                            const int upsideDown,
                            const int padding = 1);

    /** create true color (32 bit) bitmap for Java (AWT default format).
     *  Memory is not handled internally - must be deleted from calling program.
     *
     ** @param  data   resulting pointer to bitmap data (set to NULL if an error occurred)
     *  @param  frame  index of frame to be converted (default: first frame)
     *  @param  bits   number of bits per pixel used for the output bitmap (32)
     *
     ** @return number of bytes allocated by the bitmap, or 0 if an error occured
     */
    unsigned long createAWTBitmap(void *&data,
                                  const unsigned long frame,
                                  const int bits);

    /** write current image and related attributes to DICOM dataset.
     *
     ** @param  dataset  reference to DICOM dataset where the image attributes are stored
     *  @param  mode     dummy parameter (only used for monochrome images)
     *  @param  planar   flag, whether the output data (for multi-planar images) should be planar or not
     *
     ** @return true if successful, false otherwise
     */
    int writeImageToDataset(DcmItem &dataset,
                            const int mode,
                            const int planar);

    /** write pixel data to PPM file.
     *  pixel data is written in ASCII format.
     *
     ** @param  stream  open C++ output stream
     *  @param  frame   index of frame used for output
     *  @param  bits    number of bits used for output of pixel data
     *
     ** @return true if successful, false otherwise
     */
    int writePPM(STD_NAMESPACE ostream& stream,
                 const unsigned long frame,
                 const int bits);

    /** write pixel data to PPM file.
     *  pixel data is written in ASCII format.
     *
     ** @param  stream  open C output stream
     *  @param  frame   index of frame used for output
     *  @param  bits    number of bits used for output of pixel data
     *
     ** @return true if successful, false otherwise
     */
    int writePPM(FILE *stream,
                 const unsigned long frame,
                 const int bits);

    /** write pixel data to raw PPM file
     *
     ** @param  stream  open C output stream
     *  @param  frame   index of frame used for output
     *  @param  bits    number of bits used for output of pixel data
     *
     ** @return true if successful, false otherwise
     */
    int writeRawPPM(FILE *stream,
                    const unsigned long frame,
                    const int bits);

    /** write pixel data to BMP file
     *
     ** @param  stream  open C output stream
     *  @param  frame   index of frame used for output (default: first frame = 0)
     *  @param  bits    number of bits used for output of pixel data (24 or 32, default (0) = 24)
     *
     ** @return true if successful, false otherwise
     */
    int writeBMP(FILE *stream,
                 const unsigned long frame,
                 const int bits);


 protected:

    /** constructor, copy
     *
     ** @param  image   pointer to reference image
     *  @param  fstart  first frame to be processed
     *  @param  fcount  number of frames
     */
    DiColorImage(const DiColorImage *image,
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
     *                        automatically)
     */
    DiColorImage(const DiColorImage *image,
                 const signed long left_pos,
                 const signed long top_pos,
                 const Uint16 src_cols,
                 const Uint16 src_rows,
                 const Uint16 dest_cols,
                 const Uint16 dest_rows,
                 const int interpolate = 0,
                 const int aspect = 0);

    /** constructor, flip
     *
     ** @param  image  pointer to reference image
     ** @param  horz   flip horizontally if true
     *  @param  vert   flip vertically if true
     */
    DiColorImage(const DiColorImage *image,
                 const int horz,
                 const int vert);

    /** constructor, rotate
     *
     ** @param  image   pointer to reference image
     *  @param  degree  angle by which the image shall be rotated
     */
    DiColorImage(const DiColorImage *image,
                 const int degree);

    /** check intermediate pixel representation for consistency
     *
     ** @param  mode  check number of pixels stored in the dataset if true
     */
    int checkInterData(const int mode = 1);

    /** get pixel data with specified format.
     *  (memory is handled externally)
     *
     ** @param  buffer    untyped pointer to the externally allocated memory buffer
     *  @param  size      size of the memory buffer in bytes (will be checked)
     *  @param  frame     number of frame to be rendered
     *  @param  bits      number of bits for the output pixel data (depth)
     *  @param  planar    flag, 0 = color-by-pixel and 1 = color-by-plane
     *
     ** @return untyped pointer to the pixel data if successful, NULL otherwise
     */
    const void *getData(void *buffer,
                        const unsigned long size,
                        const unsigned long frame,
                        const int bits,
                        const int planar);

    /** update Image Pixel Module attributes in the given dataset.
     *  Removes color palette lookup tables.  Used in writeXXXToDataset() routines.
     *
     ** @param  dataset  reference to DICOM image dataset
     */
    virtual void updateImagePixelModuleAttributes(DcmItem &dataset);

    /// flag, indicating whether the intermediate representation uses the RGB color model
    const OFBool RGBColorModel;

    /// points to intermediate pixel data representation (object)
    DiColorPixel *InterData;


 private:

    /// points to current output data (object)
    DiColorOutputPixel *OutputData;

 // --- declarations to avoid compiler warnings

    DiColorImage(const DiColorImage &);
    DiColorImage &operator=(const DiColorImage &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicoimg.h,v $
 * Revision 1.30  2010-10-14 13:16:29  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.29  2010-03-01 09:08:46  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.28  2008-07-11 08:36:05  joergr
 * Fixed typo in API documentation.
 *
 * Revision 1.27  2008-05-20 10:38:50  joergr
 * Added new bilinear and bicubic scaling algorithms for image magnification.
 *
 * Revision 1.26  2008-05-13 10:03:08  joergr
 * Added new parameter to writeImageToDataset() in order to affect the planar
 * configuration of the output image/dataset. Changed behaviour: By default,
 * the output now uses the same planar configuration as the "original" image
 * (previously: always color-by-plane).
 *
 * Revision 1.25  2006/08/15 16:35:01  meichel
 * Updated the code in module dcmimage to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.24  2006/07/10 10:57:50  joergr
 * Added support for 32-bit BMP images.
 *
 * Revision 1.23  2005/12/08 16:01:30  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.22  2005/03/09 17:45:08  joergr
 * Added mode to writeImageToDataset() - only used for monochrome images.
 *
 * Revision 1.21  2004/07/20 18:13:16  joergr
 * Added API method to "officially" access the internal intermediate pixel data
 * representation (e.g. to get Hounsfield Units for CT images).
 *
 * Revision 1.20  2004/02/06 11:18:18  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.19  2003/12/17 18:10:56  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols).
 *
 * Revision 1.18  2003/06/12 15:09:41  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.17  2003/05/20 09:26:05  joergr
 * Added method returning the number of bytes required to store a single
 * rendered frame: getOutputDataSize().
 *
 * Revision 1.16  2002/12/09 13:37:51  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.15  2002/08/02 15:07:02  joergr
 * Added function to write the current image (not only a selected frame) to a
 * DICOM dataset.
 *
 * Revision 1.14  2002/01/29 17:07:07  joergr
 * Added optional flag to the "Windows DIB" methods allowing to switch off the
 * scanline padding.
 *
 * Revision 1.13  2001/11/27 18:22:17  joergr
 * Added support for plugable output formats in class DicomImage. First
 * implementation is JPEG.
 *
 * Revision 1.12  2001/11/09 16:38:36  joergr
 * Added support for Windows BMP file format.
 * Enhanced and renamed createTrueColorDIB() method.
 * Updated/Enhanced comments.
 *
 * Revision 1.11  2001/09/28 13:55:40  joergr
 * Added new flag (CIF_KeepYCbCrColorModel) which avoids conversion of YCbCr
 * color models to RGB.
 *
 * Revision 1.10  2001/06/01 15:49:28  meichel
 * Updated copyright header
 *
 * Revision 1.9  2000/03/08 16:21:50  meichel
 * Updated copyright header.
 *
 * Revision 1.8  1999/08/25 16:58:06  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.7  1999/04/28 12:51:57  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.6  1999/01/20 14:39:52  joergr
 * Added new output method to fill external memory buffer with rendered pixel
 * data.
 *
 * Revision 1.5  1998/11/27 13:43:29  joergr
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 *
 * Revision 1.4  1998/07/01 08:39:18  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.3  1998/05/11 14:53:11  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
