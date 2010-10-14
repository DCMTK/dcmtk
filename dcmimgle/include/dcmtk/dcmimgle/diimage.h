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
 *  Purpose: DicomImage (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.47 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIIMAGE_H
#define DIIMAGE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcfcache.h"

#ifdef SUNCC
#include "dcmtk/dcmimgle/didocu.h"
#endif

#include "dcmtk/dcmimgle/diovlay.h"
#include "dcmtk/dcmimgle/diutils.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmPixelData;
class DcmUnsignedShort;

#ifndef SUNCC
 class DiDocument;
#endif

class DiPixel;
class DiMonoImage;
class DiInputPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Base class for images.
 *  NB: This is an internal class of module dcmimgle/dcmimage.  Please do not
 *      use it directly.  Instead use the main interface class DicomImage.
 */
class DiImage
{

 public:

    /** constructor
     *
     ** @param  docu    pointer to the DICOM document
     *  @param  status  status of the image object
     *  @param  spp     samples per pixel
     */
    DiImage(const DiDocument *docu,
            const EI_Status status,
            const int spp);

    /** destructor
     */
    virtual ~DiImage();

    /** process next couple of frames
     *
     ** @param  fcount  number of frames to be processed (0 = same number as before)
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int processNextFrames(const unsigned long fcount);

    /** get status of the image object
     *
     ** @return status of the image object
     */
    inline EI_Status getStatus() const
    {
        return ImageStatus;
    }

    /** get number of frames
     *
     ** @return number of frames
     */
    inline Uint32 getNumberOfFrames() const
    {
        return NumberOfFrames;
    }

    /** get index of first frame
     *
     ** @return index of first frame
     */
    inline Uint32 getFirstFrame() const
    {
        return FirstFrame;
    }

    /** get representative frame
     *
     ** @return representative frame
     */
    inline Uint32 getRepresentativeFrame() const
    {
        return RepresentativeFrame;
    }

    /** get number of rows
     *
     ** @return number of rows
     */
    inline Uint16 getRows() const
    {
        return Rows;
    }

    /** get number of columns
     *
     ** @return number of columns
     */
    inline Uint16 getColumns() const
    {
         return Columns;
    }

    /** get pixel's width
     *
     ** @return pixel's width
     */
    inline double getPixelWidth() const
    {
        return (PixelWidth > 0) ? PixelWidth : 1;
    }

    /** get pixel's height
     *
     ** @return pixel's height
     */
    inline double getPixelHeight() const
    {
        return (PixelHeight > 0) ? PixelHeight : 1;
    }

    /** get pixel's rows/column ratio
     *
     ** @return pixel's rows/column ratio
     */
    inline double getRowColumnRatio() const
    {
        return getPixelHeight() / getPixelWidth();
    }

    /** get pixel's column/rows ratio
     *
     ** @return pixel's column/rows ratio
     */
    inline double getColumnRowRatio() const
    {
        return getPixelWidth() / getPixelHeight();
    }

    /** set pixel's rows/column ratio
     *
     ** @param  ratio  pixel's rows/column ratio
     *
     ** @return status, true if successful, false otherwise
     */
    int setRowColumnRatio(const double ratio);

    /** set pixel's column/rows ratio
     *
     ** @param  ratio  pixel's column/rows ratio
     *
     ** @return status, true if successful, false otherwise
     */
    int setColumnRowRatio(const double ratio);

    /** get polarity.
     *  possible values are EPP_Normal and EPP_Reverse
     *
     ** @return currently active polarity mode
     */
    inline EP_Polarity getPolarity() const
    {
        return Polarity;
    }

    /** set polarity.
     *
     ** @param  polarity  polarity (normal or reverse)
     *
     ** @return true if successful (1 = polarity has changed,
     *                              2 = polarity has not changed)
     *          false otherwise
     */
    int setPolarity(const EP_Polarity polarity);

    /** get number of bits per sample.
     *  If the optional parameter is specified the value will be checked and in any case
     *  a valid value will be returned.
     *
     ** @param  bits  value to be returned (if less than 1 or greater than the maximum (32)
     *                the default value will be used which is equal to the bits per sample
     *                value stored in the DICOM dataset)
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int getBits(const int bits = 0) const
    {
        return ((bits < 1) || (bits > MAX_BITS)) ? BitsPerSample : bits;
    }

    /** get color model of internal pixel representation.
     *  Possible values are: EPI_Monochrome1, EPI_Monochrome2, EPI_RGB and EPI_YBR_Full
     *
     ** @return color model of internal pixel representation
     */
    virtual EP_Interpretation getInternalColorModel() const = 0;

    /** get access to intermediate pixel data representation (abstract)
     *
     ** @return pointer to intermediate pixel data
     */
    virtual const DiPixel *getInterData() const = 0;

    /** get number of bytes required for the rendered output of a single frame
     *
     *  @param  bits  number of bits for the output pixel data (depth)
     *
     ** @return number of bytes if successful, 0 otherwise
     */
    virtual unsigned long getOutputDataSize(const int bits = 0) const = 0;

    /** get pixel data with specified format (abstract).
     *  (memory is handled internally)
     *
     ** @param  frame   number of frame to be rendered
     *  @param  bits    number of bits for the output pixel data (depth)
     *  @param  planar  flag, whether the output data (for multi-planar images) should be planar or not
     *
     ** @return untyped pointer to the pixel data if successful, NULL otherwise
     */
    virtual const void *getOutputData(const unsigned long frame,
                                      const int bits,
                                      const int planar) = 0;

    /** get pixel data with specified format (abstract).
     *  (memory is handled externally)
     *
     ** @param  buffer  untyped pointer to the externally allocated memory buffer
     *  @param  size    size of the memory buffer in bytes (will be checked)
     *  @param  frame   number of frame to be rendered
     *  @param  bits    number of bits for the output pixel data (depth)
     *  @param  planar  flag, whether the output data (for multi-planar images) should be planar or not
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int getOutputData(void *buffer,
                              const unsigned long size,
                              const unsigned long frame,
                              const int bits,
                              const int planar) = 0;

    /** get pixel data of specified plane (abstract).
     *  (memory is handled internally)
     *
     ** @param  plane  number of plane which should be rendered (starting from 0)
     *
     ** @return untyped pointer to the pixel data if successful, NULL otherwise
     */
    virtual const void *getOutputPlane(const int plane) const = 0;

    /** delete internally handled output memory buffer (abstract)
     */
    virtual void deleteOutputData() = 0;

    /** get pointer to the object managing the overlay planes
     *
     ** (#)param  idx  index of overlay group (here: not used, since only applicable for monochrome images)
     *
     ** @return pointer to the overlay managing object, here: NULL
     */
    virtual DiOverlay *getOverlayPtr(const unsigned int /*idx*/)
    {
        return NULL;
    }

    /** get pointer to monochrome image object
     *
     ** @return pointer to monochrome image object (here: always NULL)
     */
    virtual DiMonoImage *getMonoImagePtr()
    {
        return NULL;
    }

    /** create copy of current image object (abstract)
     *
     ** @param  fstart  first frame to be processed (not fully implemented!)
     *  @param  fcount  number of frames (not fully implemented!)
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    virtual DiImage *createImage(const unsigned long fstart,
                                 const unsigned long fcount) const = 0;

    /** create scaled copy of specified (clipping) area of the current image object (abstract).
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
    virtual DiImage *createScale(const signed long left_pos,
                                 const signed long top_pos,
                                 const unsigned long clip_width,
                                 const unsigned long clip_height,
                                 const unsigned long scale_width,
                                 const unsigned long scale_height,
                                 const int interpolate,
                                 const int aspect,
                                 const Uint16 pvalue) const = 0;

    /** flip current image horizontally and/or vertically (abstract)
     *
     ** @param  horz  flip horizontally if true
     *  @param  vert  flip vertically if true
     *
     ** @return true if successful, false otherwise
     */
    virtual int flip(const int horz,
                     const int vert) = 0;

    /** create a flipped copy of the current image (abstract).
     *
     ** @param  horz  flip horizontally if true
     *  @param  vert  flip vertically if true
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    virtual DiImage *createFlip(const int horz,
                                const int vert) const = 0;

    /** rotate current image (by steps of 90 degrees)
     *
     ** @param  degree  angle by which the image shall be rotated
     *
     ** @return true if successful, false otherwise
     */
    virtual int rotate(const int degree);

    /** create a rotated copy of the current image (abstract).
     *
     ** @param  degree  angle by which the image shall be rotated
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    virtual DiImage *createRotate(const int degree) const = 0;

    /** create monochrome copy of the current image (abstract).
     *
     ** @param  red    coefficient by which the red component is weighted
     *  @param  green  coefficient by which the green component is weighted
     *  @param  blue   coefficient by which the blue component is weighted
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    virtual DiImage *createMono(const double red,
                                const double green,
                                const double blue) const = 0;

    /** create true color (24/32 bit) or palette (8 bit) bitmap for MS Windows (abstract).
     *
     ** @param  data        untyped pointer memory buffer (set to NULL if not allocated externally)
     *  @param  size        size of the memory buffer in bytes (if 0 'data' is set to NULL)
     *  @param  frame       index of frame to be converted (starting from 0)
     *  @param  bits        number of bits per pixel used for the output bitmap (8, 24 or 32)
     *  @param  upsideDown  specifies the order of lines in the images (0 = top-down, bottom-up otherwise)
     *  @param  padding     align each line to a 32-bit address if true (default)
     *
     ** @return number of bytes allocated by the bitmap, or 0 if an error occured
     */
    virtual unsigned long createDIB(void *&data,
                                    const unsigned long size,
                                    const unsigned long frame,
                                    const int bits,
                                    const int upsideDown,
                                    const int padding = 1) = 0;

    /** create true color (32 bit) bitmap for Java AWT (abstract).
     *
     ** @param  data   resulting pointer to bitmap data (set to NULL if an error occurred)
     *  @param  frame  index of frame to be converted
     *  @param  bits   number of bits per pixel used for the output bitmap (8 or 32)
     *
     ** @return number of bytes allocated by the bitmap, or 0 if an error occured
     */
    virtual unsigned long createAWTBitmap(void *&data,
                                          const unsigned long frame,
                                          const int bits) = 0;

    /** render pixel data of given frame and write image related attributes to DICOM dataset.
     *
     ** @param  dataset  reference to DICOM dataset where the image attributes are stored
     *  @param  frame    index of frame used for output
     *  @param  bits     number of bits used for output of pixel data
     *  @param  planar   flag, whether the output data (for multi-planar images) should be planar or not
     *
     ** @return true if successful, false otherwise
     */
    int writeFrameToDataset(DcmItem &dataset,
                            const unsigned long frame = 0,
                            const int bits = 0,
                            const int planar = 0);

    /** write current image and related attributes to DICOM dataset.
     *
     ** @param  dataset  reference to DICOM dataset where the image attributes are stored
     *  @param  mode     determine value of BitsStored from 'used' or 'possible' pixel values
     *  @param  planar   flag, whether the output data (for multi-planar images) should be planar or not
     *
     ** @return true if successful, false otherwise
     */
    virtual int writeImageToDataset(DcmItem &dataset,
                                    const int mode = 0,
                                    const int planar = 2) = 0;

    /** write pixel data to PPM file (abstract).
     *  pixel data is written in ASCII format.
     *
     ** @param  stream  open C++ output stream
     *  @param  frame   index of frame used for output
     *  @param  bits    number of bits used for output of pixel data
     *
     ** @return true if successful, false otherwise
     */
    virtual int writePPM(STD_NAMESPACE ostream& stream,
                         const unsigned long frame,
                         const int bits) = 0;

    /** write pixel data to PPM file (abstract).
     *  pixel data is written in ASCII format.
     *
     ** @param  stream  open C output stream
     *  @param  frame   index of frame used for output
     *  @param  bits    number of bits used for output of pixel data
     *
     ** @return true if successful, false otherwise
     */
    virtual int writePPM(FILE *stream,
                         const unsigned long frame,
                         const int bits) = 0;

    /** write pixel data to raw PPM file (abstract)
     *
     ** @param  stream  open C output stream
     *  @param  frame   index of frame used for output
     *  @param  bits    number of bits used for output of pixel data
     *
     ** @return true if successful, false otherwise
     */
    virtual int writeRawPPM(FILE *stream,
                            const unsigned long frame,
                            const int bits) = 0;

    /** write pixel data to BMP file
     *
     ** @param  stream  open C output stream
     *  @param  frame   index of frame used for output (default: first frame = 0)
     *  @param  bits    number of bits used for output of pixel data (8, 24 or 32)
     *
     ** @return true if successful, false otherwise
     */
    virtual int writeBMP(FILE *stream,
                         const unsigned long frame,
                         const int bits);


 protected:

    /** constructor
     *
     ** @param  docu    pointer to the DICOM document
     *  @param  status  status of the image object
     */
    DiImage(const DiDocument *docu,
            const EI_Status status);

    /** constructor, copy
     *
     ** @param  image   pointer to reference image
     *  @param  fstart  first frame to be processed
     *  @param  fcount  number of frames
     */
    DiImage(const DiImage *image,
            const unsigned long fstart,
            const unsigned long fcount);

    /** constructor, scale/clip
     *
     ** @param  image   pointer to reference image
     *  @param  width   number of columns of the new image
     *  @param  height  number of rows of the new image
     *  @param  aspect  flag indicating whether pixel aspect ratio should be used or not
     */
    DiImage(const DiImage *image,
            const Uint16 width,
            const Uint16 height,
            const int aspect = 0);

    /** constructor, rotate
     *
     ** @param  image   pointer to reference image
     *  @param  degree  angle by which the image shall be rotated
     */
    DiImage(const DiImage *image,
            const int degree = 0);

    /** constructor, createMonoOutput
     *
     ** @param  image   pointer to reference image
     *  @param  frame   number of frame stored in the new image object
     *  @param  stored  number of bits stored
     *  @param  alloc   number of bits allocated
     */
    DiImage(const DiImage *image,
            const unsigned long frame,
            const int stored,
            const int alloc);

    /** delete internally handled object for the input pixel data conversion
     */
    void deleteInputData();

    /** check and possibly correct values for pixel spacing, aspect ratio etc.
     */
    void checkPixelExtension();

    /** create input pixel data representation from DICOM dataset structures
     */
    void convertPixelData();

    /** update Image Pixel Module attributes in the given dataset.
     *  Removes smallest/largest pixel value and updates pixel aspect ratio as well
     *  as pixel spacing (if required).
     *  Used in writeXXXToDataset() routines.
     *
     ** @param  dataset  reference to DICOM image dataset
     */
    virtual void updateImagePixelModuleAttributes(DcmItem &dataset);

    /** detach pixel data.
     *  removes storage area used for the pixel data from memory
     */
    int detachPixelData();

    /// copy of status variable declared in class 'DicomImage'
    EI_Status ImageStatus;
    /// points to special object, which capsulates the dcmdata module
    const DiDocument *Document;

    /// first frame to be processed
    Uint32 FirstFrame;
    /// number of frames in case of multi-frame images (otherwise '1')
    Uint32 NumberOfFrames;
    /// total number of frames stored in the dataset
    Uint32 TotalNumberOfFrames;
    /// number of representative frame, type 3 attribute (default '0')
    Uint32 RepresentativeFrame;
    /// number of rows (in pixel)
    Uint16 Rows;
    /// number of columns (in pixel)
    Uint16 Columns;
    /// width of each pixel according to 'PixelSpacing/AspectRatio'
    double PixelWidth;
    /// height of each pixel according to 'PixelSpacing/AspectRatio'
    double PixelHeight;
    /// number of bits allocated for each pixel
    Uint16 BitsAllocated;
    /// number of bits stored for each pixel (see 'BitsPerSample')
    Uint16 BitsStored;
    /// position of highest stored bit
    Uint16 HighBit;

    /// actual number of bits per sample (depth)
    int BitsPerSample;
    /// number of samples per pixel (1, 3 or 4)
    int SamplesPerPixel;

    /// polarity (normal or reverse)
    EP_Polarity Polarity;

    /// is 'true' if pixel data is signed
    int hasSignedRepresentation;
    /// is 'true' if attribute 'PixelSpacing' is present
    int hasPixelSpacing;
    /// is 'true' if attribute 'ImagerPixelSpacing' is present
    int hasImagerPixelSpacing;
    /// is 'true' if attribute 'hasNominalScannedPixelSpacing' is present
    int hasNominalScannedPixelSpacing;
    /// is 'true' if attribute 'PixelAspectRatio' is present
    int hasPixelAspectRatio;
    /// is 'false' if derived from original image data (e.g. scaled)
    int isOriginal;

    /// points to intermediate pixel representation (template object)
    DiInputPixel *InputData;
    /// file cache object used for partial read
    DcmFileCache FileCache;
    /// current pixel item fragment (for encapsulated pixel data)
    Uint32 CurrentFragment;

 // --- declarations to avoid compiler warnings

    DiImage(const DiImage &);
    DiImage &operator=(const DiImage &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diimage.h,v $
 * Revision 1.47  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.46  2010-10-05 15:26:28  joergr
 * Fixed various Doxygen API documentation issues.
 *
 * Revision 1.45  2010-03-01 09:08:46  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.44  2009-11-25 15:59:10  joergr
 * Adapted code for new approach to access individual frames of a DICOM image.
 *
 * Revision 1.43  2009-02-12 12:01:17  joergr
 * Added support for NominalScannedPixelSpacing in order to determine the pixel
 * aspect ratio (used for the new SC image IODs).
 *
 * Revision 1.42  2008-05-20 10:03:53  joergr
 * Added new bilinear and bicubic scaling algorithms for image magnification.
 *
 * Revision 1.41  2008-05-13 09:54:40  joergr
 * Added new parameter to writeImageToDataset() in order to affect the planar
 * configuration of the output image/dataset. Changed behaviour: By default,
 * the output now uses the same planar configuration as the "original" image
 * (previously: always color-by-plane).
 *
 * Revision 1.40  2006/08/15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.39  2006/07/10 10:52:27  joergr
 * Added support for 32-bit BMP images.
 *
 * Revision 1.38  2005/12/08 16:47:42  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.37  2005/03/09 17:32:35  joergr
 * Added mode to writeImageToDataset() which allows the value of BitsStored to
 * be determined either from 'used' or from 'possible' pixel values.
 *
 * Revision 1.36  2004/09/22 11:33:14  joergr
 * Introduced new member variable "TotalNumberOfFrames".
 *
 * Revision 1.35  2004/07/20 18:12:16  joergr
 * Added API method to "officially" access the internal intermediate pixel data
 * representation (e.g. to get Hounsfield Units for CT images).
 *
 * Revision 1.34  2004/02/06 11:07:50  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.33  2004/01/05 14:52:20  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.32  2003/12/08 18:22:26  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated CVS header.
 *
 * Revision 1.31  2003/06/12 15:08:34  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.30  2003/05/20 09:20:41  joergr
 * Added method returning the number of bytes required to store a single
 * rendered frame: getOutputDataSize().
 *
 * Revision 1.29  2002/12/09 13:32:51  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.28  2002/11/27 14:08:04  meichel
 * Adapted module dcmimgle to use of new header file ofstdinc.h
 *
 * Revision 1.27  2002/11/26 14:48:32  joergr
 * Added Smallest/LargestImagePixelValue to the list of attributes to be
 * removed from a newly created dataset.
 *
 * Revision 1.26  2002/08/02 15:03:20  joergr
 * Enhanced writeFrameToDataset() routine (remove out-data DICOM attributes
 * from the dataset).
 * Added function to write the current image (not only a selected frame) to a
 * DICOM dataset.
 *
 * Revision 1.25  2002/06/26 16:01:55  joergr
 * Added support for polarity flag to color images.
 * Added new method to write a selected frame to a DICOM dataset (incl. required
 * attributes from the "Image Pixel Module").
 *
 * Revision 1.24  2002/04/16 13:53:11  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.23  2002/01/29 17:05:50  joergr
 * Added optional flag to the "Windows DIB" methods allowing to switch off the
 * scanline padding.
 *
 * Revision 1.22  2001/11/27 18:18:22  joergr
 * Added support for plugable output formats in class DicomImage. First
 * implementation is JPEG.
 *
 * Revision 1.21  2001/11/09 16:26:37  joergr
 * Added support for Window BMP file format.
 * Enhanced and renamed createTrueColorDIB() method.
 *
 * Revision 1.20  2001/06/20 15:12:49  joergr
 * Enhanced multi-frame support for command line tool 'dcm2pnm': extract all
 * or a range of frames with one call.
 *
 * Revision 1.19  2001/06/01 15:49:42  meichel
 * Updated copyright header
 *
 * Revision 1.18  2000/03/08 16:24:16  meichel
 * Updated copyright header.
 *
 * Revision 1.17  2000/02/02 11:02:38  joergr
 * Removed space characters before preprocessor directives.
 *
 * Revision 1.16  1999/10/06 13:28:21  joergr
 * Corrected creation of PrintBitmap pixel data: VOI windows should be applied
 * before clipping to avoid that the region outside the image (border) is also
 * windowed (this requires a new method in dcmimgle to create a DicomImage
 * with the grayscale transformations already applied).
 *
 * Revision 1.15  1999/09/17 12:12:18  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.14  1999/08/25 16:39:31  joergr
 * Allow clipping region to be outside the image (overlapping).
 *
 * Revision 1.13  1999/07/23 13:53:00  joergr
 * Added support for attribute 'ImagerPixelSpacing'.
 * Added support for attribute 'RepresentativeFrameNumber'.
 * Added methods to set 'PixelAspectRatio'.
 *
 * Revision 1.12  1999/04/28 14:47:34  joergr
 * Added experimental support to create grayscale images with more than 256
 * shades of gray to be displayed on a consumer monitor (use pastel colors).
 *
 * Revision 1.11  1999/03/24 17:20:01  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.10  1999/02/08 12:38:12  joergr
 * Added parameter 'idx' to some overlay methods to distinguish between
 * built-in and additional overlay planes.
 *
 * Revision 1.9  1999/02/03 17:01:45  joergr
 * Added BEGIN_EXTERN_C and END_EXTERN_C to some C includes.
 *
 * Revision 1.8  1999/01/20 14:59:37  joergr
 * Added new output method to fill external memory buffer with rendered pixel
 * data.
 *
 * Revision 1.7  1999/01/11 09:32:32  joergr
 * Removed method 'getMinMaxValues()' in class 'DicomImage'.
 *
 * Revision 1.6  1998/12/23 11:33:08  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.5  1998/12/22 14:03:53  joergr
 * Changed parameter declaration to avoid compiler warnings (hide parameter
 * name).
 *
 * Revision 1.4  1998/12/16 16:29:04  joergr
 * Removed several methods used for monochrome images only in base class
 * 'DiImage'. Introduced mechanism to use the methods directly.
 *
 * Revision 1.3  1998/12/14 17:17:29  joergr
 * Added methods to add and remove additional overlay planes (still untested).
 *
 * Revision 1.2  1998/11/30 12:24:07  joergr
 * Added const type qualifier to some parameters to avoid errors with MSVC5
 * (couldn't create instance of abstract class).
 *
 * Revision 1.1  1998/11/27 15:06:08  joergr
 * Added copyright message.
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 * Added method to directly create java AWT bitmaps.
 * Renamed variable 'Status' to 'ImageStatus' because of possible conflicts
 * with X windows systems.
 * Added method to detach pixel data if it is no longer needed.
 * Added methods to support presentation LUTs and shapes.
 *
 * Revision 1.6  1998/07/01 08:39:21  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.5  1998/05/11 14:53:16  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
