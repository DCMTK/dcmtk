/*
 *
 *  Copyright (C) 1996-2016, OFFIS e.V.
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
 *  Purpose: Provides main interface to the "DICOM image toolkit"
 *
 */


#ifndef DCMIMAGE_H
#define DCMIMAGE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/dimoimg.h"
#include "dcmtk/dcmimgle/didispfn.h"
#include "dcmtk/dcmimgle/diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmXfer;
class DcmObject;
class DcmOverlayData;
class DcmLongString;
class DcmUnsignedShort;

class DiPixel;
class DiDocument;
class DiPluginFormat;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Interface class for dcmimgle/dcmimage module.
 *  The main purpose of these modules is image display.
 */
class DCMTK_DCMIMGLE_EXPORT DicomImage
{

 public:

 // --- constructors and destructor

    /** constructor, open a DICOM file.
     *  Opens specified file and reads image related data, creates internal representation
     *  of image data.  Use getStatus() to obtain detailed information about any errors.
     *
     ** @param  filename  the DICOM file
     *  @param  flags     configuration flags (see diutils.h, CIF_MayDetachPixelData is set automatically)
     *  @param  fstart    first frame to be processed (optional, 0 = 1st frame), all subsequent use
     *                    of parameters labeled 'frame' in this class refers to this start frame.
     *  @param  fcount    number of frames (optional, 0 = all frames)
     */
    DicomImage(const char *filename,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

#ifndef STARVIEW
    /** constructor, use a given DcmObject
     *
     ** @param  object  pointer to DICOM data structures (fileformat, dataset or item).
     *                  (do not delete while referenced, i.e. while this image object or any
     *                   descendant exists; not deleted within dcmimage unless configuration flag
     *                   CIF_TakeOverExternalDataset is set - in this case do not delete it at all)
     *  @param  xfer    transfer syntax of the 'object'.
     *                  (could also be EXS_Unknown in case of fileformat or dataset)
     *  @param  flags   configuration flags (CIF_xxx, see diutils.h)
     *  @param  fstart  first frame to be processed (optional, 0 = 1st frame), all subsequent use
     *                  of parameters labeled 'frame' in this class refers to this start frame.
     *  @param  fcount  number of frames (optional, 0 = all frames)
     */
    DicomImage(DcmObject *object,
               const E_TransferSyntax xfer,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

    /** constructor, use a given DcmObject with specified rescale/slope.
     *  NB: This constructor ignores the Photometric Interpretation stored in the DICOM dataset
     *      and always creates a MONOCHROME2 image - useful in combination with Presentation States.
     *
     ** @param  object     pointer to DICOM data structures (fileformat, dataset or item).
     *                     (do not delete while referenced, i.e. while this image object or any
     *                      descendant exists; not deleted within dcmimage unless configuration flag
     *                      CIF_TakeOverExternalDataset is set - in this case do not delete it at all)
     *  @param  xfer       transfer syntax of the 'object'.
     *                     (could also be EXS_Unknown in case of fileformat or dataset)
     *  @param  slope      rescale slope (modality transformation)
     *  @param  intercept  rescale intercept (modality transformation)
     *  @param  flags      configuration flags (CIF_xxx, see diutils.h)
     *  @param  fstart     first frame to be processed (optional, 0 = 1st frame), all subsequent use
     *                     of parameters labeled 'frame' in this class refers to this start frame.
     *  @param  fcount     number of frames (optional, 0 = all frames)
     */
    DicomImage(DcmObject *object,
               const E_TransferSyntax xfer,
               const double slope,
               const double intercept,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

    /** constructor, use a given DcmObject with specified modality LUT.
     *  NB: This constructor ignores the Photometric Interpretation stored in the DICOM dataset
     *      and always creates a MONOCHROME2 image - useful in combination with Presentation States.
     *
     ** @param  object       pointer to DICOM data structures (fileformat, dataset or item).
     *                       (do not delete while referenced, i.e. while this image object or any
     *                        descendant exists; not deleted within dcmimage unless configuration flag
     *                        CIF_TakeOverExternalDataset is set - in this case do not delete it at all)
     *  @param  xfer         transfer syntax of the 'object'.
     *                       (could also be EXS_Unknown in case of fileformat or dataset)
     *  @param  data         dataset element containing modality LUT data
     *  @param  descriptor   dataset element containing modality LUT descriptor
     *  @param  explanation  dataset element containing modality LUT explanation
     *  @param  flags        configuration flags (CIF_xxx, see diutils.h)
     *  @param  fstart       first frame to be processed (optional, 0 = 1st frame), all subsequent use
     *                       of parameters labeled 'frame' in this class refers to this start frame.
     *  @param  fcount       number of frames (optional, 0 = all frames)
     */
    DicomImage(DcmObject *object,
               E_TransferSyntax xfer,
               const DcmUnsignedShort &data,
               const DcmUnsignedShort &descriptor,
               const DcmLongString *explanation = NULL,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);
#endif

    /** destructor
     */
    virtual ~DicomImage();


 // --- multi-frame handling

    /** process next couple of frames.  If the image object has been created with less than the number
     *  of frames stored in the DICOM image, this function allows for accessing the subsequent frames.
     *  Multiple calls to this function allow for successively processing all frames stored in the
     *  file or dataset.  See parameters 'fstart' and 'fcount' of the constructor for how to initially
     *  create an instance of this class.
     *  NB: Only "original" images can be processed in this way, i.e. images that were created by one
     *      of the above constructors.  Images that were created by one of the createXXX() methods can
     *      not be processed since they are derived from original image data in a certain way, e.g.
     *      scaled.
     *
     ** @param  fcount  number of frames to be processed (0 = same number as before)
     *
     ** @return status, true if successful, false otherwise
     */
    inline int processNextFrames(const unsigned long fcount = 0)
    {
        return (Image != NULL) ?
            Image->processNextFrames(fcount) : 0;
    }


 // --- information: return requested value if successful

    /** convert status code to status string
     *
     ** @param  status  code of current internal status
     *
     ** @return pointer to status string
     */
    static const char *getString(const EI_Status status);

    /** convert photometric interpretation code to interpretation string (defined term)
     *
     ** @param  interpret  code of image's photometric interpretation
     *
     ** @return pointer to photometric interpretation string
     */
    static const char *getString(const EP_Interpretation interpret);

    /** get current status information
     *
     ** @return status code
     */
    inline EI_Status getStatus() const
    {
        return (Image != NULL) ?
            Image->getStatus() : ImageStatus;
    }

    /** get number of frames.
     *  Please note that this function does not return the number of frames stored in the
     *  DICOM file/dataset (use getNumberOfFrames() instead).  It rather refers to the
     *  number of frames processed by this class (see constructors for details).
     *
     ** @return number of frames processed by this class
     */
    inline unsigned long getFrameCount() const
    {
        return (Image != NULL) ?
            Image->getNumberOfFrames() : 0;
    }

    /** get index of first frame.
     *  This functions returns the index of the first frame processed by this class (see
     *  constructors for details).
     *
     ** @return index of first frame (0..n-1)
     */
    inline unsigned long getFirstFrame() const
    {
        return (Image != NULL) ?
            Image->getFirstFrame() : 0;
    }

    /** get number of frames stored in the DICOM file/dataset.
     *  This attribute is mandatory for DICOM images with multiple frames.
     *  For single frame images, the value defaults to 1.
     *
     ** @return number of frames stored in the DICOM file/dataset
     */
    inline unsigned long getNumberOfFrames() const
    {
        return (Image != NULL) ?
            Image->getTotalNumberOfFrames() : 0;
    }

    /** get index of representative frame.
     *  This attribute is optionally stored in the DICOM dataset (type 3).
     *
     ** @return index of representative frame (0..n-1)
     */
    inline unsigned long getRepresentativeFrame() const
    {
        return (Image != NULL) ?
            Image->getRepresentativeFrame() : 0;
    }

    /** get nominal time (in milliseconds) between individual frames.
     *  This attribute might not always be stored in the DICOM dataset.
     *
     ** @return nominal time between individual frames, 0 if absent
     */
    inline double getFrameTime() const
    {
        return (Image != NULL) ?
            Image->getFrameTime() : 0;
    }

    /** get image width in pixels
     *
     ** @return number of pixels in one row
     */
    inline unsigned long getWidth() const
    {
        return (Image != NULL) ?
            Image->getColumns() : 0;
    }

    /** get image height in pixels
     *
     ** @return number of pixels in one column
     */
    inline unsigned long getHeight() const
    {
        return (Image != NULL) ?
            Image->getRows() : 0;
    }

    /** get image depth
     *
     ** @return number of bits per sample of the internal representation
     */
    inline int getDepth() const
    {
        return (Image != NULL) ?
            Image->getBits() : 0;
    }

    /** get minimum and maximum pixel values.
     *  the resulting pixel values are stored in 'double' variables to avoid problems
     *  with different number ranges, limited to monochrome images.
     *  Please note that the min/max values refer to the full pixel data (i.e. including
     *  all possible present frames as specified in the constructor of this class).
     *
     ** @param  min   minimum pixel value (reference parameter)
     *  @param  max   maximum pixel value (reference parameter)
     *  @param  mode  0 = min/max 'used' pixel values,
     *                1 = min/max 'possible' pixel values (absolute min/max)
     *
     ** @return status code (true if successful)
     */
    inline int getMinMaxValues(double &min,
                               double &max,
                               const int mode = 0) const
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->getMinMaxValues(min, max, mode) : 0;
    }

    /** get width height ratio (pixel aspect ratio: x/y).
     *  If present in the dataset and not overwritten using setWidthHeightRatio(), the following
     *  attributes are checked in order to determine this value:
     *  - Pixel Spacing, Imager Pixel Spacing, Nominal Scanned Pixel Spacing, Pixel Aspect Ratio.
     *  The first attribute (Pixel Spacing) is checked both on the main dataset level and within
     *  the Pixel Measures Sequence of the Shared Functional Groups Sequence (if present).
     *
     ** @return pixel aspect ratio (floating point value)
     */
    inline double getWidthHeightRatio() const
    {
        return (Image != NULL) ?
            Image->getColumnRowRatio() : 0;
    }

    /** get height width ratio (pixel aspect ratio: y/x).
     *  If present in the dataset and not overwritten using setWidthHeightRatio(), the following
     *  attributes are checked in order to determine this value:
     *  - Pixel Spacing, Imager Pixel Spacing, Nominal Scanned Pixel Spacing, Pixel Aspect Ratio
     *  The first attribute (Pixel Spacing) is checked both on the main dataset level and within
     *  the Pixel Measures Sequence of the Shared Functional Groups Sequence (if present).
     *
     ** @return pixel aspect ratio (floating point value)
     */
    inline double getHeightWidthRatio() const
    {
        return (Image != NULL) ?
            Image->getRowColumnRatio() : 0;
    }

    /** set width height ratio (pixel aspect ratio: x/y)
     *
     ** @param  ratio  pixel aspect ratio (x/y)
     *
     ** @return status code (true if successful)
     */
    inline int setWidthHeightRatio(const double ratio) const
    {
        return (Image != NULL) ?
            Image->setColumnRowRatio(ratio) : 0;
    }

    /** set height width ratio (pixel aspect ratio: y/x)
     *
     ** @param  ratio  pixel aspect ratio (y/x)
     *
     ** @return status code (true if successful)
     */
    inline int setHeightWidthRatio(const double ratio) const
    {
        return (Image != NULL) ?
            Image->setRowColumnRatio(ratio) : 0;
    }

    /** check whether given output value is unused
     *
     ** @param  value  output value to be checked
     *
     ** @return status, true if unused (1 = within output range, 2 = out of range), false otherwise
     */
    inline int isOutputValueUnused(const unsigned long value)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->isValueUnused(value) : 0;
    }

  // --- output: return pointer to output data if successful

    /** get number of bytes required for the rendered output of a single frame.
     *  This function determines the size of a rendered frame as created by getOutputData().
     *  Therefore, it can be used to allocate a sufficiently large memory buffer and pass
     *  its size to the second variant of getOutputData().
     *
     ** @param  bits  number of bits per sample used to render the pixel data
     *                (image depth, 1..MAX_BITS, 0 means 'bits stored' in the image)
     *                (MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *
     ** @return number of bytes if successful, 0 otherwise
     */
    inline unsigned long getOutputDataSize(const int bits = 0) const
    {
        return (Image != NULL) ?
            Image->getOutputDataSize(Image->getBits(bits)) : 0;
    }

    /** render pixel data and return pointer to internal memory buffer.
     *  apply VOI/PLUT transformation and (visible) overlay planes.
     *  internal memory buffer will be delete for the next getBitmap/Output operation.
     *  output data is always padded to 8, 16, 32, ... bits (bits allocated).
     *  Supported output color models: Monochrome 2 for monochrome images and RGB
     *  (or YCbCr_Full if flag CIF_KeepYCbCrColorModel is set) for color images.
     *  The rendered pixel data is always unsigned.
     *
     ** @param  bits    number of bits per sample used to render the pixel data
     *                  (image depth, 1..MAX_BITS, 0 means 'bits stored' in the image)
     *                  (MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *  @param  frame   number of frame to be rendered (0..n-1)
     *  @param  planar  0 = color-by-pixel (R1G1B1...R2G2B2...R3G3B3...),
     *                  1 = color-by-plane (R1R2R3...G1G2G3...B1B2B3...)
     *                  (only applicable to multi-planar/color images, otherwise ignored)
     *
     ** @return pointer to internal memory buffer containing rendered pixel data
     *          (if successful, NULL otherwise)
     */
    inline const void *getOutputData(const int bits = 0,
                                     const unsigned long frame = 0,
                                     const int planar = 0)
    {
        return (Image != NULL) ?
            Image->getOutputData(frame, Image->getBits(bits), planar) : NULL;
    }

    /** render pixel data and output to given memory buffer.
     *  apply VOI/PLUT transformation and (visible) overlay planes.
     *  output data is always padded to 8, 16, 32, ... bits (bits allocated).
     *  Supported output color models: Monochrome 2 for monochrome images and RGB
     *  (or YCbCr_Full if flag CIF_KeepYCbCrColorModel is set) for color images.
     *  The rendered pixel data is always unsigned.
     *
     ** @param  buffer  pointer to memory buffer (must already be allocated)
     *  @param  size    size of memory buffer (will be checked whether it is sufficient)
     *  @param  bits    number of bits per sample used to render the pixel data
     *                  (image depth, 1..MAX_BITS, 0 means 'bits stored' in the image)
     *                  (MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *  @param  frame   number of frame to be rendered (0..n-1)
     *  @param  planar  0 = color-by-pixel (R1G1B1...R2G2B2...R3G3B3...),
     *                  1 = color-by-plane (R1R2R3...G1G2G3...B1B2B3...)
     *                  (only applicable to multi-planar/color images, otherwise ignored)
     *
     ** @return status code (true if successful)
     */
    inline int getOutputData(void *buffer,
                             const unsigned long size,
                             const int bits = 0,
                             const unsigned long frame = 0,
                             const int planar = 0)
    {
        return (Image != NULL) ?
            Image->getOutputData(buffer, size, frame, Image->getBits(bits), planar) : 0;
    }

    /** render pixel data and return pointer to given plane (internal memory buffer).
     *  apply VOI/PLUT transformation and (visible) overlay planes
     *  internal memory buffer will be delete for the next getBitmap/Output operation.
     *  Supported output color models: Monochrome 2 for monochrome images and RGB
     *  (or YCbCr_Full if flag CIF_KeepYCbCrColorModel is set) for color images.
     *  The rendered pixel data is always unsigned.
     *
     ** @param  plane  number of plane to be rendered
     *
     ** @return pointer to internal memory buffer containing rendered pixel data
     *          (if successful, NULL otherwise)
     */
    inline const void *getOutputPlane(const int plane) const
    {
        return (Image != NULL) ?
            Image->getOutputPlane(plane) : NULL;
    }

    /** delete internal memory buffer used for rendered images.
     *  Save memory if data is no longer needed.
     */
    inline void deleteOutputData() const
    {
        if (Image != NULL)
            Image->deleteOutputData();
    }

  // --- misc

    /** check whether image is monochrome or not.
     *
     ** @return true if image is monochrome, false otherwise (i.e. color image)
     */
    inline int isMonochrome() const
    {
        return (PhotometricInterpretation == EPI_Monochrome1) || (PhotometricInterpretation == EPI_Monochrome2);
    }

    /** get code for photometric interpretation (color model).
     *
     ** @return code for photometric interpretation of the image
     */
    inline EP_Interpretation getPhotometricInterpretation() const
    {
        return PhotometricInterpretation;
    }

    /** check whether image has given SOP class UID.
     *
     ** @return true if image has given SOP class UID, false otherwise
     */
    int hasSOPclassUID(const char *uid) const;

    /** get intermediate pixel data representation (read-only).
     *  This function allows to access the pixel data after they have been extracted
     *  from the DICOM data element and the modality transformation has been applied
     *  (if present and not disabled).  Please note that for monochrome images the
     *  internal representation might be signed whereas color images are automatically
     *  converted to unsigned RGB format.  Pixels are aligned to 8, 16 or 32 bits.
     *  Always use DiPixel::getRepresentation() in order to determine the integer
     *  representation of the internally stored pixel data.
     *
     ** @return pointer to intermediate pixel data representation
     */
    inline const DiPixel *getInterData() const
    {
        return (Image != NULL) ?
            Image->getInterData() : NULL;
    }

 // --- display function for output device characteristic (calibration):
 //     only applicable to grayscale images

    /** get display function
     *
     ** @return pointer to current display function, NULL if absent
     */
    inline DiDisplayFunction *getDisplayFunction() const
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->getDisplayFunction() : OFstatic_cast(DiDisplayFunction *, NULL);
    }

    /** set display function
     *
     ** @param  display  object describing the output device characteristic (only referenced!)
     *
     ** @return true if successful, false otherwise
     */
    inline int setDisplayFunction(DiDisplayFunction *display)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setDisplayFunction(display) : 0;
    }

    /** set no display function.
     *  disables display function transformation, object is not deleted!
     *
     ** @return true if successful (1 = disabled current function,
     *                              2 = there was no function to disable),
     *          false otherwise
     */
    inline int setNoDisplayFunction()
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setNoDisplayFunction() : 0;
    }

    /** delete specified display LUT(s)
     *
     ** @param  bits  parameter of LUT to be deleted (0 = all)
     *
     ** @return true if successful, false otherwise
     */
    inline int deleteDisplayLUT(const int bits = 0)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->deleteDisplayLUT(bits) : 0;
    }

    /** convert P-value to DDL.
     *  conversion uses display LUT if present, linear scaling otherwise.
     *
     ** @param  pvalue  P-value to be converted (0..65535)
     *  @param  ddl     reference to resulting DDL
     *  @param  bits    number of bits for output
     *
     ** @return true if successful (1 = display function transformation,
     *                              2 = linear scaling),
     *          false otherwise
     */
    inline int convertPValueToDDL(const Uint16 pvalue,
                                  Uint16 &ddl,
                                  const int bits = 8)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->convertPValueToDDL(pvalue, ddl, bits) : 0;
    }

 // --- windowing (voi): only applicable to grayscale images
 //                      return true if successful (see also 'dimoimg.cc')

    /** unset all VOI transformations (windows and LUTs).
     *  only applicable to monochrome images
     *
     ** @return true if successful (1 = previous window/LUT has been valid,
     *                              2 = otherwise),
     *          false otherwise (image is invalid or not monochrome)
     */
    inline int setNoVoiTransformation()
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setNoVoiTransformation() : 0;
    }

    /** set automatically calculated minimum/maximum window.
     *  possibly active VOI LUT is implicitly disabled.
     *  Please note that the min/max values refer to the full pixel data (i.e. including
     *  all possible present frames as specified in the constructor of this class).
     *
     ** @param  idx  ignore global min/max values if true (1)
     *
     ** @return true if successful (1 = window has changed,
     *                              2 = new window is the same as previous one),
     *          false otherwise
     */
    inline int setMinMaxWindow(const int idx = 0)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setMinMaxWindow(idx) : 0;
    }

    /** set automatically calculated histogram window.
     *  possibly active VOI LUT is implicitly disabled.
     *
     ** @param  thresh  threshold value specifying percentage of histogram border which
     *                  shall be ignored (defaut: 5%).
     *
     ** @return true if successful, false otherwise
     */
    inline int setHistogramWindow(const double thresh = 0.05)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setHistogramWindow(thresh) : 0;
    }

    /** set automatically calculated VOI window for the specified Region of Interest (ROI).
     *  The ROI is specified by means of a rectangle (left, top, width, height).  Only the part
     *  of the ROI that overlaps with the image is regarded - if the overlapping area is empty
     *  this method returns false (0).
     *  Possibly active VOI LUT is implicitly disabled.
     *
     ** @param  left_pos  x-coordinate of the top left-hand corner of the ROI (starting from 0)
     *  @param  top_pos   y-coordinate of the top left-hand corner of the ROI (starting from 0)
     *  @param  width     width in pixels of the rectangular ROI (minimum: 1)
     *  @param  height    height in pixels of the rectangular ROI (minimum: 1)
     *  @param  frame     index of the frame to be used for calculation (default: 0 = first)
     *
     ** @return true if successful (1 = window has changed,
     *                              2 = new window is the same as previous one),
     *          false otherwise
     */
    inline int setRoiWindow(const unsigned long left_pos,
                            const unsigned long top_pos,
                            const unsigned long width,
                            const unsigned long height,
                            const unsigned long frame = 0)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setRoiWindow(left_pos, top_pos, width, height, frame) : 0;
    }

    /** set specified VOI window (given by index to window width/center sequence stored in image
     *  file).
     *  possibly active VOI LUT is implicitly disabled.
     *  NB: This function does nothing if the flag CIF_UsePresentationState is set.
     *
     ** @param  window  index to window width/center sequence (0..n-1)
     *
     ** @return true if successful, false otherwise (none monochrome or invalid index)
     */
    inline int setWindow(const unsigned long window)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setWindow(window) : 0;
    }

    /** set specified VOI window (given by window width and center).
     *  possibly active VOI LUT is implicitly disabled.
     *
     ** @param  center  center of specified window
     *  @param  width   width of specified window (>= 1.0)
     *
     ** @return true if successful (1 = window has changed,
     *                              2 = new window is the same as previous one),
     *          false otherwise
     */
    inline int setWindow(const double center,
                         const double width)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setWindow(center, width) : 0;
    }

    /** get current VOI window as window center and width values
     *
     ** @param  center  return current window center value
     *  @param  width   return current window width value
     *
     ** @return true if successful, false otherwise
     */
    inline int getWindow(double &center,
                         double &width)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->getWindow(center, width) : 0;
    }

    /** get number of VOI windows (stored in image file).
     *  NB: This function does nothing if the flag CIF_UsePresentationState is set.
     *
     ** @return number of VOI windows
     */
    inline unsigned long getWindowCount() const
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->getWindowCount() : 0;
    }

    /** set VOI LUT function
     *
     ** @param  function  type of VOI LUT function (default, linear or sigmoid).
     *                    'default' basically means the same as 'linear'.
     *
     ** @return true if successful (1 = function has changed,
     *                              2 = function has not changed),
     *          false otherwise
     */
    inline int setVoiLutFunction(const EF_VoiLutFunction function)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setVoiLutFunction(function) : 0;
    }

    /** get VOI LUT function.
     *  possible values are: EFV_Default, EFV_Linear, EFV_Sigmoid.
     *
     ** @return currently active VOI LUT function or EFV_Default if not set
     */
    inline EF_VoiLutFunction getVoiLutFunction() const
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->getVoiLutFunction() : EFV_Default;
    }

    /** set VOI LUT (given by dcmdata elements).
     *  possibly active window/center is implicitly disabled.
     *  Given data is only referenced and not copied! Make sure that the corresponding DcmXXX
     *  objects exist as long as the VOI LUT is set.
     *
     ** @param  data         contains LUT data
     *  @param  descriptor   describes LUT structure
     *  @param  explanation  free form description of VOI LUT (optional)
     *  @param  descripMode  mode specifying the use of the bits per table entry value
     *
     ** @return true if successful, false otherwise
     */
    inline int setVoiLut(const DcmUnsignedShort &data,
                         const DcmUnsignedShort &descriptor,
                         const DcmLongString *explanation = NULL,
                         const EL_BitsPerTableEntry descripMode = ELM_UseValue)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setVoiLut(data, descriptor, explanation, descripMode) : 0;
    }

    /** set VOI LUT (given by index to VOI LUT sequence stored in image file).
     *  possibly active window/center is implicitly disabled.
     *  NB: This function does nothing if the flag CIF_UsePresentationState is set.
     *
     ** @param  table  index to VOI LUT sequence (0..n-1)
     *  @param  descripMode  mode specifying the use of the bits per table entry value
     *
     ** @return true if successful, false otherwise (none monochrome or invalid index)
     */
    inline int setVoiLut(const unsigned long table,
                         const EL_BitsPerTableEntry descripMode = ELM_UseValue)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setVoiLut(table, descripMode) : 0;
    }

    /** get number of VOI LUTs (stored in image file)
     *
     ** @return number of VOI LUTs
     */
    inline unsigned long getVoiLutCount() const
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->getVoiLutCount() : 0;
    }

    /** get description of active VOI transformation
     *
     ** @return pointer to description text (NULL if absent)
     */
    inline const char *getVoiTransformationExplanation() const
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->getVoiTransformationExplanation() : OFstatic_cast(const char *, NULL);
    }

    /** get description of specified VOI window (stored in the image file)
     *
     ** @param  window       index of the stored VOI window (0..n-1)
     *  @param  explanation  variable in which the explanation string is stored
     *
     ** @return pointer to description text (NULL if absent or index invalid)
     */
    inline const char *getVoiWindowExplanation(const unsigned long window,
                                               OFString &explanation) const
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->getVoiWindowExplanation(window, explanation) : OFstatic_cast(const char *, NULL);
    }

    /** get description of specified VOI LUT (stored in the image file)
     *
     ** @param  table        index of the stored VOI LUT (0..n-1)
     *  @param  explanation  variable in which the explanation string is stored
     *
     ** @return pointer to description text (NULL if absent or index invalid)
     */
    inline const char *getVoiLutExplanation(const unsigned long table,
                                            OFString &explanation) const
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->getVoiLutExplanation(table, explanation) : OFstatic_cast(const char *, NULL);
    }

    /** get description of performed modality LUT transformation
     *
     ** @return pointer to description text (NULL if absent)
     */
    inline const char *getModalityLutExplanation() const
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->getModalityLutExplanation() : OFstatic_cast(const char *, NULL);
    }

 // --- hardcopy parameters

    /** get polarity. applicable to monochrome and color images.
     *  possible values are EPP_Normal and EPP_Reverse
     *
     ** @return currently active polarity mode or EPP_Normal if not applicable
     */
    inline EP_Polarity getPolarity() const
    {
        return (Image != NULL) ?
            Image->getPolarity() : EPP_Normal;
    }

    /** set polarity. applicable to monochrome and color images.
     *
     ** @param  polarity  polarity (normal or reverse)
     *
     ** @return true if successful (1 = polarity has changed,
     *                              2 = polarity has not changed),
     *          false otherwise
     */
    inline int setPolarity(const EP_Polarity polarity)
    {
        return (Image != NULL) ?
            Image->setPolarity(polarity) : 0;
    }

    /** set hardcopy parameters. only applicable to monochrome images.
     *  used to display LinOD images
     *
     ** @param  min      minimum density of the print-out (in hundreds of Optical Density,
     *                   e.g. 150 means 1.5 OD)
     *  @param  max      maximum density of the print-out (ditto)
     *  @param  reflect  reflected ambient light (in candela per square meter - cd/m^2)
     *  @param  illumin  illumination (ditto)
     *
     ** @return true if successful (1 = at least one of the parameters has changed,
     *                              2 = no parameter has changed),
     *          false otherwise
     */
    inline int setHardcopyParameters(const unsigned int min,
                                     const unsigned int max,
                                     const unsigned int reflect,
                                     const unsigned int illumin)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setHardcopyParameters(min, max, reflect, illumin) : 0;
    }

 // --- presentation LUT: only applicable to grayscale images

    /** get shape for presentation transformation.
     *  possible values are: ESP_Default, ESP_Identity, ESP_Inverse, ESP_LinOD.
     *  If a presentation LUT is currently active ESP_Default is always returned.
     *
     ** @return currently active presentation LUT shape or ESP_Default if not set
     */
    inline ES_PresentationLut getPresentationLutShape() const
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->getPresentationLutShape() : ESP_Default;
    }

    /** set shape for presentation transformation.
     *  possibly active presentation LUT is implicitly disabled.
     *
     ** @param  shape  presentation LUT shape (default, identity, inverse or lin OD).
     *                 'default' means that the output data is always created with 0 for black
     *                 and maxvalue for white (i.e. monochrome2 data is created for output).
     *
     ** @return true if successful (1 = shape has changed,
     *                              2 = shape has not changed),
     *          false otherwise
     */
    inline int setPresentationLutShape(const ES_PresentationLut shape)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setPresentationLutShape(shape) : 0;
    }

    /** set LUT for presentation transformation.
     *  possibly active presentation LUT is implicitly disabled.
     *  Given data is only referenced and not copied! Make sure that the corresponding DcmXXX
     *  objects exist as long as the presentation LUT is set.
     *
     ** @param  data         contains LUT data
     *  @param  descriptor   describes LUT structure
     *  @param  explanation  free form description of presentation LUT (optional)
     *  @param  descripMode  mode specifying the use of the bits per table entry value
     *
     ** @return true if successful, false otherwise
     */
    inline int setPresentationLut(const DcmUnsignedShort &data,
                                  const DcmUnsignedShort &descriptor,
                                  const DcmLongString *explanation = NULL,
                                  const EL_BitsPerTableEntry descripMode = ELM_UseValue)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setPresentationLut(data, descriptor, explanation, descripMode) : 0;
    }

    /** get description of active presentation LUT
     *
     ** @return pointer to description text (NULL if absent)
     */
    inline const char *getPresentationLutExplanation() const
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->getPresentationLutExplanation() : OFstatic_cast(const char *, NULL);
    }

    /** set inverse LUT for presentation transformation.
     *  this LUT transform is e.g. used for DICOM print (12->8, 8->12 bit)
     *  possibly active presentation LUT will not be considered!
     *
     ** @param  data         contains LUT data
     *  @param  descriptor   describes LUT structure
     *  @param  descripMode  mode specifying the use of the bits per table entry value
     *
     ** @return true if successful, false otherwise
     */
    inline int setInversePresentationLut(const DcmUnsignedShort &data,
                                         const DcmUnsignedShort &descriptor,
                                         const EL_BitsPerTableEntry descripMode = ELM_UseValue)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setInversePresentationLut(data, descriptor, descripMode) : 0;
    }

 // --- overlays: return true (!0) if successful (see also 'diovlay.cc')
 //               only applicable to grayscale images

    /** add specified plane to group of additional overlay planes.
     *  replaces old overlay plane if group number already exists.
     *
     ** @param  group        group number (0x60nn) of overlay plane
     *  @param  width        width of overlay plane (in pixels)
     *  @param  height       height of overlay plane (in pixels)
     *  @param  left_pos     x coordinate of plane origin (referring to image origin)
     *  @param  top_pos      y coordinate of plane origin
     *  @param  data         overlay plane data (dcmdata element)
     *  @param  label        overlay plane label
     *  @param  description  overlay plane description
     *  @param  mode         display mode (see 'diutils.h')
     *
     ** @return false (0) if an error occurred, true otherwise (1 = added new plane,
     *                                                          2 = replaced existing plane)
     */
    inline int addOverlay(const unsigned int group,
                          const signed int left_pos,
                          const signed int top_pos,
                          const unsigned int width,
                          const unsigned int height,
                          const DcmOverlayData &data,
                          const DcmLongString &label,
                          const DcmLongString &description,
                          const EM_Overlay mode = EMO_Default)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->addOverlay(group, left_pos, top_pos, width, height, data, label, description, mode) : 0;
    }

    /** remove specified (additional) overlay plane
     *
     ** @param  group  group number (0x60nn) of overlay plane
     *
     ** @return false (0) if an error occurred (e.g. plane doesn't exist), true otherwise
     */
    inline int removeOverlay(const unsigned int group)
    {
        return ((Image != NULL) && (Image->getOverlayPtr(1) != NULL)) ?
            Image->getOverlayPtr(1)->removePlane(group) : 0;
    }

    /** remove all additional overlay planes
     *
     ** @return false (0) if an error occurred, true otherwise (1 = all planes deleted,
     *                                                          2 = no planes to be deleted)
     */
    inline int removeAllOverlays()
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->removeAllOverlays() : 0;
    }

    /** check whether specified overlay plane is visible/activated.
     *  see show/hideOverlay() to modify the visibility status.
     *
     ** @param  plane  number (0..15) or group number (0x60nn) of overlay plane
     *  @param  idx    index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return true (1) if overlay plane is visible, false (0) otherwise
     */
    inline int isOverlayVisible(const unsigned int plane,
                                const unsigned int idx = 0)
    {
        return ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) ?
            Image->getOverlayPtr(idx)->isPlaneVisible(plane) : 0;
    }

    /** activate specified overlay plane
     *
     ** @param  plane  number (0..15) or group number (0x60nn) of overlay plane
     *  @param  idx    index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return false (0) if an error occurred, true otherwise (1 = plane successfully activated,
     *                                                          2 = plane has already been visible)
     */
    inline int showOverlay(const unsigned int plane,
                           const unsigned int idx = 0)
    {
        return ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) ?
            Image->getOverlayPtr(idx)->showPlane(plane) : 0;
    }

    /** activate specified overlay plane and change some parameters
     *
     ** @param  plane   number (0..15) or group number (0x60nn) of overlay plane
     *  @param  mode    display mode (see 'diutils.h')
     *  @param  fore    plane's foreground color (in percent, default: 1.0)
     *  @param  thresh  threshold value (in percent, default: 0.5), only for EMO_ThresholdReplace
     *  @param  idx     index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return false (0) if an error occurred, true otherwise
     */
    inline int showOverlay(const unsigned int plane,
                           const EM_Overlay mode,
                           const double fore = 1.0,
                           const double thresh = 0.5,
                           const unsigned int idx = 0)
    {
        return ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) ?
            Image->getOverlayPtr(idx)->showPlane(plane, fore, thresh, mode) : 0;
    }

    /** activate specified overlay plane and change 'pvalue' (only for bitmap shutters)
     *
     ** @param  plane   number (0..15) or group number (0x60nn) of overlay plane
     *  @param  pvalue  P-value used to display overlay plane
     *
     ** @return false (0) if an error occurred, true otherwise
     */
    inline int showOverlay(const unsigned int plane,
                           const Uint16 pvalue)
    {
        return ((Image != NULL) && (Image->getOverlayPtr(1) != NULL)) ?
            Image->getOverlayPtr(1)->showPlane(plane, pvalue) : 0;
    }

    /** activate all overlay planes (make them visible)
     *
     ** @param  idx  index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return false (0) if an error occurred, true otherwise (1 = planes have been successfully activated,
     *                                                          2 = no planes to be activated)
     */
    inline int showAllOverlays(const unsigned int idx = 0)
    {
        return ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) ?
            Image->getOverlayPtr(idx)->showAllPlanes() : 0;
    }

    /** activate all overlay planes and set specified parameters
     *
     ** @param  mode    display mode (see 'diutils.h')
     *  @param  fore    plane's foreground color (in percent, default: 1.0)
     *  @param  thresh  threshold value (in percent, default: 0.5), only for EMO_ThresholdReplace
     *  @param  idx     index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return false (0) if an error occurred, true otherwise (1 = planes have been successfully activated,
     *                                                          2 = no planes to be activated)
     */
    inline int showAllOverlays(const EM_Overlay mode,
                               const double fore = 1,
                               const double thresh = 0.5,
                               const unsigned int idx = 0)
    {
        return ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) ?
            Image->getOverlayPtr(idx)->showAllPlanes(fore, thresh, mode) : 0;
    }

    /** deactivate specified overlay plane
     *
     ** @param  plane  number (0..15) or group number (0x60nn) of overlay plane
     *  @param  idx    index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return false (0) if an error occurred, true otherwise (1 = plane successfully deactivated,
     *                                                          2 = plane has already been invisible)
     */
    inline int hideOverlay(const unsigned int plane,
                           const unsigned int idx = 0)
    {
        return ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) ?
            Image->getOverlayPtr(idx)->hidePlane(plane) : 0;
    }

    /** deactivate all overlay planes (make them invisible)
     *
     ** @param  idx  index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return false (0) if an error occurred, true otherwise (1 = planes have been successfully deactivated,
     *                                                          2 = no planes to be deactivated)
     */
    inline int hideAllOverlays(const unsigned int idx = 0)
    {
        return ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) ?
            Image->getOverlayPtr(idx)->hideAllPlanes() : 0;
    }

    /** move origin of specified overlay plane to given position
     *
     ** @param  plane     number (0..15) or group number (0x60nn) of overlay plane
     *  @param  left_pos  x coordinate of new plane origin (origin = 0)
     *  @param  top_pos   y coordinate of new plane origin (origin = 0)
     *  @param  idx       index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return false (0) if an error occurred, true otherwise (1 = plane has been successfully moved,
     *                                                          2 = old and new position are equal, nothing to do)
     */
    inline int placeOverlay(const unsigned int plane,
                            const signed int left_pos,
                            const signed int top_pos,
                            const unsigned int idx = 0)
    {
        return ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) ?
            Image->getOverlayPtr(idx)->placePlane(plane, left_pos, top_pos) : 0;
    }

    /** get number of overlay planes
     *
     ** @param  idx  index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return number of overlay planes stored in the image
     */
    inline unsigned int getOverlayCount(const unsigned int idx = 0) const
    {
        return ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) ?
            Image->getOverlayPtr(idx)->getCount() : 0;
    }

    /** get group number of specified overlay plane
     *
     ** @param  plane  number (0..15) or group number (0x60nn) of overlay plane
     *  @param  idx    index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return group number of given overlay plane if successful, false (0) otherwise
     */
    inline unsigned int getOverlayGroupNumber(const unsigned int plane,
                                              const unsigned int idx = 0) const
    {
        return ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) ?
            Image->getOverlayPtr(idx)->getPlaneGroupNumber(plane) : 0;
    }

    /** get label of specified overlay plane
     *
     ** @param  plane  number (0..15) or group number (0x60nn) of overlay plane
     *  @param  idx    index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return pointer to overlay plane label if successful, false (NULL) otherwise
     */
    inline const char *getOverlayLabel(const unsigned int plane,
                                       const unsigned int idx = 0) const
    {
        return ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) ?
            Image->getOverlayPtr(idx)->getPlaneLabel(plane) : OFstatic_cast(const char *, NULL);
    }

    /** get description of specified overlay plane
     *
     ** @param  plane  number (0..15) or group number (0x60nn) of overlay plane
     *  @param  idx    index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return pointer to overlay plane description if successful, false (NULL) otherwise
     */
    inline const char *getOverlayDescription(const unsigned int plane,
                                             const unsigned int idx = 0) const
    {
        return ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) ?
            Image->getOverlayPtr(idx)->getPlaneDescription(plane) : OFstatic_cast(const char *, NULL);
    }

    /** get mode of specified overlay plane
     *
     ** @param  plane  number (0..15) or group number (0x60nn) of overlay plane
     *  @param  idx    index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return mode of overlay plane if successful, EMO_Default otherwise
     */
    inline EM_Overlay getOverlayMode(const unsigned int plane,
                                     const unsigned int idx = 0) const
    {
        return ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) ?
            Image->getOverlayPtr(idx)->getPlaneMode(plane) : EMO_Default;
    }

    /** create bitmap for specified overlay plane.
     *  (up to 16 bits per pixel with two values: 'fore' and 'back')
     *  Please note that the current origin of the overlay plane is used.  Furthermore, the size
     *  of the overlay plane is restricted to the size of the surrounding image.  Use the method
     *  getFullOverlayData() if the complete bitmap data is required regardless of its position.
     *
     ** @param  plane     number (0..15) or group number (0x60nn) of overlay plane
     *  @param  width     returns width of overlay plane (in pixels)
     *  @param  height    returns height of overlay plane (in pixels)
     *  @param  left_pos  returns x coordinate of plane's origin
     *  @param  top_pos   returns y coordinate of plane's origin
     *  @param  mode      returns display mode (see 'diutils.h')
     *  @param  frame     index of frame used for output (default: 0 = first)
     *  @param  bits      number of bits (stored) in the resulting array, default: 8, range: 1..16
     *                    Used to mask the values for foreground and background color.  The resulting
     *                    array is always padded to 8 or 16 bits with 1, 8 or 16 bits allocated
     *                    depending on the value of 'bits'.
     *  @param  fore      foreground color to be set in bitmap, default: 255, range: 0..2^bits-1
     *  @param  back      background color to be set in bitmap (transparent), default: 0, range: 0..2^bits-1
     *  @param  idx       index of overlay group (0 = dataset, planes stored in the image dataset;
     *                                            1 = additional, planes added by addOverlay();
     *                                            2 = 'additional' overlay planes hide 'dataset' planes
     *                                                when the overlay group number exists in both),
     *                    default: 2
     *
     ** @return pointer to overlay plane data (internal memory buffer)
     */
    const void *getOverlayData(const unsigned int plane,
                               unsigned int &left_pos,
                               unsigned int &top_pos,
                               unsigned int &width,
                               unsigned int &height,
                               EM_Overlay &mode,
                               const unsigned long frame = 0,
                               const int bits = 8,
                               const Uint16 fore = 0xff,
                               const Uint16 back = 0x0,
                               const unsigned int idx = 2) const
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->getOverlayData(frame, plane, left_pos, top_pos, width, height, mode, idx, bits, fore, back) : NULL;
    }

    /** create bitmap for specified overlay plane.
     *  (up to 16 bits per pixel with two values: 'fore' and 'back')
     *  In contrast to the previous method the full bitmap data is always returned.
     *
     ** @param  plane   number (0..15) or group number (0x60nn) of overlay plane
     *  @param  width   returns width of overlay plane (in pixels)
     *  @param  height  returns height of overlay plane (in pixels)
     *  @param  frame   index of frame used for output (default: 0 = first)
     *  @param  bits    number of bits (stored) in the resulting array, default: 8, range: 1..16
     *                  Used to mask the values for foreground and background color.  The resulting
     *                  array is always padded to 8 or 16 bits with 1, 8 or 16 bits allocated
     *                  depending on the value of 'bits'.
     *  @param  fore    foreground color to be set in bitmap, default: 255, range: 0..2^bits-1
     *  @param  back    background color to be set in bitmap (transparent), default: 0, range: 0..2^bits-1
     *  @param  idx     index of overlay group (0 = dataset, planes stored in the image dataset;
     *                                          1 = additional, planes added by addOverlay()),
     *                  default: 0
     *
     ** @return pointer to overlay plane data (internal memory buffer)
     */
    const void *getFullOverlayData(const unsigned int plane,
                                   unsigned int &width,
                                   unsigned int &height,
                                   const unsigned long frame = 0,
                                   const int bits = 8,
                                   const Uint16 fore = 0xff,
                                   const Uint16 back = 0x0,
                                   const unsigned int idx = 0) const
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->getFullOverlayData(frame, plane, width, height, idx, bits, fore, back) : NULL;
    }

    /** delete buffer for overlay plane data.
     *  Save memory if data is no longer needed.
     */
    inline void deleteOverlayData() const
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            Image->getMonoImagePtr()->deleteOverlayData();
    }

    /** create bitmap for specified overlay plane and store it in (6xxx,3000) format.
     *  (1 bit allocated and stored, foreground color is 1, background color is 0,
     *   data is 16 bit padded - even length)
     *  memory is allocated but not handled internally - must be deleted from calling program.
     *
     ** @param  buffer  stores pointer to overlay data (memory is allocated internally)
     *  @param  plane   number (0..15) or group number (0x60nn) of overlay plane
     *  @param  width   returns width of overlay plane (in pixels)
     *  @param  height  returns height of overlay plane (in pixels)
     *  @param  frames  returns number of frames
     *  @param  idx     index of overlay group (0 = dataset, planes stored in the image dataset;
     *                                          1 = additional, planes added by addOverlay()),
     *                  default: 0
     *
     ** @return number of bytes allocated for the 'buffer' if successful, 0 otherwise
     */
    unsigned long create6xxx3000OverlayData(Uint8 *&buffer,
                                            const unsigned int plane,
                                            unsigned int &width,
                                            unsigned int &height,
                                            unsigned long &frames,
                                            const unsigned int idx = 0) const
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->create6xxx3000OverlayData(buffer, plane, width, height, frames, idx) : 0;
    }


 // --- create...Image: return pointer to new 'DicomImage' object, memory isn't handled internally !

    /** create copy of current image object.
     *  memory is not handled internally - must be deleted from calling program.
     *
     ** @param  fstart  first frame to be processed (0 = 1st frame)
     *  @param  fcount  number of frames (0 = all frames)
     *
     ** @return pointer to new DicomImage object (NULL if an error occurred)
     */
    DicomImage *createDicomImage(unsigned long fstart = 0,
                                 unsigned long fcount = 0) const;

    /** create scaled copy of current image object (given by exact size).
     *  memory is not handled internally - must be deleted from calling program.
     *
     ** @param  width        width of new image (in pixels)
     *  @param  height       height of new image (in pixels)
     *  @param  interpolate  specifies whether scaling algorithm should use interpolation (if necessary).
     *                       default: no interpolation (0), preferred interpolation algorithm (if applicable):
     *                         1 = pbmplus algorithm, 2 = c't algorithm, 3 = bilinear magnification,
     *                         4 = bicubic magnification
     *  @param  aspect       specifies whether pixel aspect ratio should be taken into consideration
     *                       (if true, width OR height should be 0, i.e. this component will be calculated
     *                        automatically)
     *
     ** @return pointer to new DicomImage object (NULL if an error occurred)
     */
    DicomImage *createScaledImage(const unsigned long width,
                                  const unsigned long height = 0,
                                  const int interpolate = 0,
                                  int aspect = 0) const;

    /** create scaled copy of current image object (given by scaling factors).
     *  memory is not handled internally - must be deleted from calling program.
     *
     ** @param  xfactor      width of new image is multiplied with this factor (> 0)
     *  @param  yfactor      height of new image is multiplied with this factor (> 0)
     *  @param  interpolate  specifies whether scaling algorithm should use interpolation (if necessary).
     *                       default: no interpolation (0), preferred interpolation algorithm (if applicable):
     *                         1 = pbmplus algorithm, 2 = c't algorithm, 3 = bilinear magnification,
     *                         4 = bicubic magnification
     *  @param  aspect       specifies whether pixel aspect ratio should be taken into consideration
     *                       (if true, width OR height should be 0, i.e. this component will be calculated
     *                        automatically)
     *
     ** @return pointer to new DicomImage object (NULL if an error occurred)
     */
    DicomImage *createScaledImage(const double xfactor,
                                  const double yfactor = 0,
                                  const int interpolate = 0,
                                  const int aspect = 0) const;

    /** create scaled copy of specified (clipping) area of the current image object.
     *  memory is not handled internally - must be deleted from calling program.
     *  NB: Clipping and interpolated scaling at the same moment is not yet fully implemented!
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
     ** @return pointer to new DicomImage object (NULL if an error occurred)
     */
    DicomImage *createScaledImage(const signed long left_pos,
                                  const signed long top_pos,
                                  unsigned long clip_width,
                                  unsigned long clip_height,
                                  unsigned long scale_width = 0,
                                  unsigned long scale_height = 0,
                                  const int interpolate = 0,
                                  int aspect = 0,
                                  const Uint16 pvalue = 0) const;

    /** create scaled copy of specified (clipping) area of the current image object.
     *  memory is not handled internally - must be deleted from calling program.
     *  NB: Clipping and interpolated scaling at the same moment is not yet fully implemented!
     *
     ** @param  left_pos     x coordinate of top left corner of area to be scaled
     *                       (referring to image origin, negative values create a border around the image)
     *  @param  top_pos      y coordinate of top left corner of area to be scaled
     *  @param  width        width of area to be scaled
     *  @param  height       height of area to be scaled
     *  @param  xfactor      width of new image is multiplied with this factor (> 0)
     *  @param  yfactor      height of new image is multiplied with this factor (> 0)
     *  @param  interpolate  specifies whether scaling algorithm should use interpolation (if necessary).
     *                       default: no interpolation (0), preferred interpolation algorithm (if applicable):
     *                         1 = pbmplus algorithm, 2 = c't algorithm, 3 = bilinear magnification,
     *                         4 = bicubic magnification
     *  @param  aspect       specifies whether pixel aspect ratio should be taken into consideration
     *                       (if true, width OR height should be 0, i.e. this component will be calculated
     *                        automatically)
     *  @param  pvalue       P-value used for the border outside the image (0..65535)
     *
     ** @return pointer to new DicomImage object (NULL if an error occurred)
     */
    DicomImage *createScaledImage(const signed long left_pos,
                                  const signed long top_pos,
                                  unsigned long width,
                                  unsigned long height,
                                  const double xfactor,
                                  const double yfactor = 0,
                                  const int interpolate = 0,
                                  const int aspect = 0,
                                  const Uint16 pvalue = 0) const;

    /** create copy of specified area of the current image object (clipping).
     *  memory is not handled internally - must be deleted from calling program.
     *
     ** @param  left_pos  x coordinate of top left corner of area to be copied
     *                    (referring to image origin, negative values create a border around the image)
     *  @param  top_pos   y coordinate of top left corner of area to be copied
     *  @param  width     width of area to be copied/clipped
     *  @param  height    height of area to be copied/clipped
     *  @param  pvalue    P-value used for the border outside the image (0..65535)
     *
     ** @return pointer to new DicomImage object (NULL if an error occurred)
     */
    DicomImage *createClippedImage(const signed long left_pos,
                                   const signed long top_pos,
                                   unsigned long width = 0,
                                   unsigned long height = 0,
                                   const Uint16 pvalue = 0) const;

    /** flip current image (horizontally and/or vertically)
     *
     ** @param  horz  flip horizontally if true
     *  @param  vert  flip vertically if true
     *
     ** @return true if successful (1 = flipped at least direction,
     *                              2 = not flipped, because of image resolution - width and/or height equal to 1),
     *          false otherwise
     */
    int flipImage(int horz = 1,
                  int vert = 0) const;

    /** create a flipped copy of the current image.
     *  memory is not handled internally - must be deleted from calling program.
     *
     ** @param  horz  flip horizontally if true
     *  @param  vert  flip vertically if true
     *
     ** @return pointer to new DicomImage object (NULL if an error occurred)
     */
    DicomImage *createFlippedImage(int horz = 1,
                                   int vert = 0) const;

    /** rotate current image (by steps of 90 degrees)
     *
     ** @param  degree  angle by which the image shall be rotated (-360, -270, -180, -90, 0, 90, 180, 270, 360)
     *
     ** @return true if successful (1 = rotated by at least 90 degrees,
     *                              2 = not rotated, because of image resolution or angle),
     *          false otherwise
     */
    int rotateImage(signed int degree) const;

    /** create a rotated copy of the current image.
     *  memory is not handled internally - must be deleted from calling program.
     *
     ** @param  degree  angle by which the image shall be rotated (-360, -270, -180, -90, 0, 90, 180, 270, 360)
     *
     ** @return pointer to new DicomImage object (NULL if an error occurred)
     */
    DicomImage *createRotatedImage(signed int degree) const;

    /** create monochrome copy of the current image.
     *  equal to createDicomImage() for monochrome images.
     *  memory is not handled internally - must be deleted from calling program.
     *  This method does not work if original YCbCr color model is retained (see CIF_KeepYCbCrColorModel).
     *
     ** @param  red    coefficient by which the red component is weighted (default: NTSC value)
     *  @param  green  coefficient by which the green component is weighted (default: NTSC value)
     *  @param  blue   coefficient by which the blue component is weighted (default: NTSC value)
     *
     ** @return pointer to new DicomImage object (NULL if an error occurred)
     */
    DicomImage *createMonochromeImage(const double red = 0.299,
                                      const double green = 0.587,
                                      const double blue = 0.114) const;

    /** create true color (24/32 bit) or palette (8 bit) bitmap for MS Windows.
     *  8 bit images require an appropriate color palette (256 entries, values: 0 to 255) and are only
     *  applicable to monochrome images, the beginning of a each line starts on a 32-bit address (if
     *  'padding' is true); 24 bit images store 24 bits per pixel (BGR) and do align each line to a
     *  32-bit address (if 'padding' is true); 32 bit images store 32 bits per pixel (ARGB), but only
     *  use the lower 24 bits (-RGB).
     *  The memory buffer can be allocated both externally (from the calling program) and internally
     *  (inside this class/module).  If the 'data' parameter is not NULL and the 'size' parameter, which
     *  describes the size (in bytes) of the allocated buffer, is sufficiently large, the bitmap is stored
     *  in this buffer.  Otherwise (i.e. 'data' is NULL) the memory is allocated internally.  Please note
     *  that in both cases the memory is not handled internally after this method has finished and,
     *  therefore, must be deleted from the calling program.
     *  This method does not work if original YCbCr color model is retained (see CIF_KeepYCbCrColorModel).
     *
     ** @param  data        untyped pointer memory buffer (set to NULL if not allocated externally)
     *  @param  size        size of the memory buffer in bytes (if 0 'data' is set to NULL)
     *  @param  frame       index of frame to be converted (default: 0 = first frame)
     *  @param  bits        number of bits per pixel used for the output bitmap (8, 24 or 32, default: 24)
     *  @param  upsideDown  flag indicating whether the first line stored is the top-most (default: 0) or
     *                      the bottom-most of the source image (as required by the BMP file format)
     *  @param  padding     align each line to a 32-bit address if true (default)
     *
     ** @return number of bytes allocated by the bitmap, or 0 if an error occurred
     */
    unsigned long createWindowsDIB(void *&data,
                                   const unsigned long size,
                                   const unsigned long frame = 0,
                                   const int bits = 24,
                                   const int upsideDown = 0,
                                   const int padding = 1)
    {
        return (Image != NULL) ?
            Image->createDIB(data, size, frame, bits, upsideDown, padding) : 0;
    }

    /** create true color (32 bit) or palette (8 bit) bitmap for Java (AWT default format).
     *  32 bit images allocate 32 bits per pixel (RGB), but only use the upper 24 bits.  The sample
     *  order for color images is: Red, Green, Blue.
     *  Memory is not handled internally - must be deleted from calling program.
     *  This method does not work if original YCbCr color model is retained (see CIF_KeepYCbCrColorModel).
     *
     ** @param  data   resulting pointer to bitmap data (set to NULL if an error occurred)
     *  @param  frame  index of frame to be converted (default: 0 = first frame)
     *  @param  bits   number of bits per pixel used for the output bitmap (8 or 32, default: 32)
     *
     ** @return number of bytes allocated by the bitmap, or 0 if an error occurred
     */
    unsigned long createJavaAWTBitmap(void *&data,
                                      const unsigned long frame = 0,
                                      const int bits = 32)
    {
        return (Image != NULL) ?
            Image->createAWTBitmap(data, frame, bits) : 0;
    }

    /** create 12 bit packed (monochrome) bitmap for DICOM printers.
     *  Memory is not handled internally - must be deleted from calling program.
     *
     ** @param  buffer  pointer to input memory buffer (16 bits allocated, 12 bits stored)
     *  @param  size    size of memory buffer (will be checked whether it is sufficient)
     *  @param  count   number of entries (pixels) in input buffer
     *
     ** @return pointer to memory buffer containing the packed output bitmap data (NULL if an error occurred)
     */
    static void *create12BitPackedBitmap(const void *buffer,
                                         const unsigned long size,
                                         const unsigned long count)
    {
        return DiMonoImage::createPackedBitmap(buffer, size, count, 16, 12);
    }

    /** create new single frame DicomImage with applied grayscale transformations.
     *  The method getOutputData() is used internally for the new bitmap.
     *  Limited to monochrome images.
     *
     ** @param  frame  index of frame to be converted
     *  @param  bits   number of bits per pixel used for the output bitmap
     *
     ** @return pointer to new DicomImage object (NULL if an error occurred)
     */
    DicomImage *createMonoOutputImage(const unsigned long frame,
                                      const int bits);

 // --- output image file: return true ('1') if successful

    /** render pixel data of the given frame and write image related attributes to DICOM dataset.
     *  Applies VOI/PLUT transformation and (visible) overlay planes, output data is
     *  always padded to 8, 16, 32, ... bits (bits allocated).  Replaces any modality
     *  transformation in the dataset by a linear rescale/slope since the modality
     *  transformation is rendered into the pixel data.  Replaces the VOI transformations
     *  in the dataset by a "max range" VOI window.  Removes all Overlay Plane Module
     *  attributes for embedded overlay planes from the dataset.
     *  Writes the following DICOM attributes (from Image Pixel Module):
     *    - Photometric Interpretation, Samples per Pixel
     *    - Columns, Rows, Number of Frames
     *    - Bits Allocated, Bits Stored, High Bit
     *    - Planar Configuration (only if "Samples per Pixel" is greater than 1)
     *    - Pixel Representation, Pixel Data
     *  Updates the following DICOM attributes (if present in the original image dataset):
     *    - Pixel Spacing and/or Pixel Aspect Ratio
     *  Supported output color models: Monochrome 2 for monochrome images and RGB
     *  (or YCbCr_Full if flag CIF_KeepYCbCrColorModel is set) for color images.
     *
     ** @param  dataset  reference to DICOM dataset where the image attributes are stored
     *  @param  bits     number of bits per sample (image depth, 1..MAX_BITS)
     *  @param  frame    number of frame to be rendered (0..n-1)
     *  @param  planar   0 = color-by-pixel (R1G1B1...R2G2B2...R3G3B3...),
     *                   1 = color-by-plane (R1R2R3...G1G2G3...B1B2B3...)
     *                   (only applicable to multi-planar/color images, otherwise ignored)
     *
     ** @return true if successful, false otherwise
     */
    inline int writeFrameToDataset(DcmItem &dataset,
                                   const int bits = 0,
                                   const unsigned long frame = 0,
                                   const int planar = 0)
    {
        return (Image != NULL) ?
            Image->writeFrameToDataset(dataset, frame, bits, planar) : 0;
    }

    /** write current image and related attributes to DICOM dataset.
     *  Uses the internal representation of the pixel data, therefore the output data is
     *  always padded to 8, 16, 32, ... bits (bits allocated).  Replaces any modality
     *  transformation in the dataset by a linear rescale/slope since the modality
     *  transformation is rendered into the pixel data.  Removes all Overlay Plane Module
     *  attributes for embedded overlay planes from the dataset.
     *  Writes the following DICOM attributes (from Image Pixel Module):
     *    - Photometric Interpretation, Samples per Pixel
     *    - Columns, Rows, Number of Frames
     *    - Bits Allocated, Bits Stored, High Bit
     *    - Planar Configuration (only if "Samples per Pixel" is greater than 1)
     *    - Pixel Representation, Pixel Data
     *  Updates the following DICOM attributes (if present in the original image dataset):
     *    - Pixel Spacing and/or Pixel Aspect Ratio
     *  Supported output color models: Monochrome 1 or 2 for monochrome images and RGB
     *  (or YCbCr_Full if flag CIF_KeepYCbCrColorModel is set) for color images.
     *
     ** @param  dataset  reference to DICOM dataset where the image attributes are stored
     *  @param  mode     0 = determine value of BitsStored from 'used' pixel values,
     *                   1 = determine value of BitsStored from 'possible' pixel values
     *                   (used for monochrome images only)
     *  @param  planar   0 = color-by-pixel (R1G1B1...R2G2B2...R3G3B3...),
     *                   1 = color-by-plane (R1R2R3...G1G2G3...B1B2B3...),
     *                   2 = same as original DICOM image (i.e. color-by-pixel or color-by-plane)
     *                   (only applicable to multi-planar/color images, otherwise ignored)
     *
     ** @return true if successful, false otherwise
     */
    inline int writeImageToDataset(DcmItem &dataset,
                                   const int mode = 0,
                                   const int planar = 2)
    {
        return (Image != NULL) ?
            Image->writeImageToDataset(dataset, mode, planar) : 0;
    }

    /** write pixel data to PPM file (specified by filename).
     *  pixel data is written in ASCII format.
     *  This method does not work if original YCbCr color model is retained
     *  (see CIF_KeepYCbCrColorModel).
     *
     ** @param  filename  name of output file (%d is replaced by frame number if present)
     *  @param  bits      number of bits used for output of pixel data
     *                    (default: full resolution, max: 32;
     *                     MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *  @param  frame     index of frame used for output (default: first frame = 0)
     *
     ** @return true if successful, false otherwise
     */
    int writePPM(const char *filename,
                 const int bits = 0,
                 const unsigned long frame = 0);

    /** write pixel data to PPM file (specified by open C++ stream).
     *  pixel data is written in ASCII format.
     *  This method does not work if original YCbCr color model is retained
     *  (see CIF_KeepYCbCrColorModel).
     *
     ** @param  stream  open C++ output stream
     *  @param  bits    number of bits used for output of pixel data
     *                  (default: full resolution, max: 32;
     *                   MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *  @param  frame   index of frame used for output (default: first frame = 0)
     *
     ** @return true if successful, false otherwise
     */
    int writePPM(STD_NAMESPACE ostream& stream,
                 const int bits = 0,
                 const unsigned long frame = 0);

    /** write pixel data to PPM file (specified by open C stream).
     *  pixel data is written in ASCII format.
     *  This method does not work if original YCbCr color model is retained
     *  (see CIF_KeepYCbCrColorModel).
     *
     ** @param  stream  open C output stream
     *  @param  bits    number of bits used for output of pixel data
     *                  (default: full resolution, max: 32;
     *                   MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *  @param  frame   index of frame used for output (default: first frame = 0)
     *
     ** @return true if successful, false otherwise
     */
    int writePPM(FILE *stream,
                 const int bits = 0,
                 const unsigned long frame = 0);

    /** write pixel data to raw PPM file (specified by filename).
     *  pixel data is written in binary format.
     *  This method does not work if original YCbCr color model is retained
     *  (see CIF_KeepYCbCrColorModel).
     *
     ** @param  filename  name of output file (%d is replaced by frame number if present)
     *  @param  bits      number of bits used for output of pixel data
     *                    (default: full resolution, max: 8;
     *                     MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *  @param  frame     index of frame used for output (default: first frame = 0)
     *
     ** @return true if successful, false otherwise
     */
    int writeRawPPM(const char *filename,
                    const int bits = 0,
                    const unsigned long frame= 0);

    /** write pixel data to raw PPM file (specified by open C stream).
     *  pixel data is written in binary format.
     *  This method does not work if original YCbCr color model is retained
     *  (see CIF_KeepYCbCrColorModel).
     *
     ** @param  stream  open C output stream (binary mode required!)
     *  @param  bits    number of bits used for output of pixel data
     *                  (default: full resolution, max: 8;
     *                   MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *  @param  frame   index of frame used for output (default: first frame = 0)
     *
     ** @return true if successful, false otherwise
     */
    int writeRawPPM(FILE *stream,
                    const int bits = 0,
                    const unsigned long frame = 0);

    /** write pixel data to BMP file (specified by open C stream).
     *  pixel data is written in palette or truecolor mode.
     *  This method does not work if original YCbCr color model is retained
     *  (see CIF_KeepYCbCrColorModel).
     *
     ** @param  stream  open C output stream (binary mode required!)
     *  @param  bits    number of bits used for output of pixel data
     *                  (8, 24 or 32, default (0): 8 for monochrome and 24 for color images)
     *  @param  frame   index of frame used for output (default: first frame = 0)
     *
     ** @return true if successful, false otherwise
     */
    int writeBMP(FILE *stream,
                 const int bits = 0,
                 const unsigned long frame = 0);

    /** write pixel data to BMP file (specified by filename).
     *  pixel data is written in palette or truecolor mode.
     *  This method does not work if original YCbCr color model is retained
     *  (see CIF_KeepYCbCrColorModel).
     *
     ** @param  filename  name of output file (%d is replaced by frame number if present)
     *  @param  bits      number of bits used for output of pixel data
     *                    (8, 24 or 32, default (0): 8 for monochrome and 24 for color images)
     *  @param  frame     index of frame used for output (default: first frame = 0)
     *
     ** @return true if successful, false otherwise
     */
    int writeBMP(const char *filename,
                 const int bits = 0,
                 const unsigned long frame = 0);

    /** write pixel data to pluggable image format file (specified by open C stream).
     *  Format specific parameters may be set directly in the instantiated 'plugin' class.
     *
     ** @param  plugin  pointer to image format plugin (derived from abstract class DiPluginFormat)
     *  @param  stream  open C output stream (binary mode required!)
     *  @param  frame   index of frame used for output (default: first frame = 0)
     *
     ** @return true if successful, false otherwise
     */
    int writePluginFormat(const DiPluginFormat *plugin,
                          FILE *stream,
                          const unsigned long frame = 0);

    /** write pixel data to pluggable image format file (specified by filename).
     *  Format specific parameters may be set directly in the instantiated 'plugin' class.
     *
     ** @param  plugin    pointer to image format plugin (derived from abstract class DiPluginFormat)
     *  @param  filename  name of output file (%d is replaced by frame number if present)
     *  @param  frame     index of frame used for output (default: first frame = 0)
     *
     ** @return true if successful, false otherwise
     */
    int writePluginFormat(const DiPluginFormat *plugin,
                          const char *filename,
                          const unsigned long frame = 0);


 protected:

    /** constructor, create copy of given image object with different image data and photometric interpretation
     *
     ** @param  dicom      source object to be copied
     *  @param  image      new image data
     *  @param  interpret  new photometric interpretation
     */
    DicomImage(const DicomImage *dicom,
               DiImage *image,
               const EP_Interpretation interpret = EPI_Unknown);

    /** initialize object.
     *  create internal image object depending on color model. is used for all 'real' constructors.
     */
    void Init();

    /** check whether data dictionary is present
     *
     ** @return true if dictionary is present, false otherwise
     */
    int checkDataDictionary();

    /** get SOP class UID of current image object
     *
     ** @return SOP class UID (or NULL if an error occurred)
     */
    const char *getSOPclassUID() const;

    /** normalize given degree value (for internal use).
     *  negative value are mapped to positive range (-360 -> 0, -270 -> 90, -180 -> 180, -90 -> 270),
     *  360 is set to 0, all other values are rejected
     *
     ** @param  degree  value to be normalized, valid values are: 0, 90, 180, 270
     *
     ** @return true if successful, false otherwise (invalid value)
     */
    int normalizeDegreeValue(signed int &degree) const;


 private:

    /// current state of converting progress (error level)
    EI_Status ImageStatus;
    /// DICOM color model (enumeration)
    EP_Interpretation PhotometricInterpretation;

    /// points to document object
    DiDocument *Document;
    /// points to image object
    DiImage    *Image;

 // --- declarations to avoid compiler warnings

    DicomImage(const DicomImage &);
    DicomImage &operator=(const DicomImage &);
};


#endif
