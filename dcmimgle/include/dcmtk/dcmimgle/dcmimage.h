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
 *  Purpose: Provides main interface to the "DICOM image toolkit"
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:25 $
 *  CVS/RCS Revision: $Revision: 1.65 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DicomImage
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
     ** @param  object  pointer to DICOM data structures
     *                  (do not delete while referenced, i.e. while this image object or any
     *                   descendant exists; not deleted within dcmimage unless configuration flag
     *                   CIF_TakeOverExternalDataset is set - in this case do not delete it at all)
     *  @param  xfer    transfer syntax
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
     ** @param  object     pointer to DICOM data structures
     *                     (do not delete while referenced, i.e. while this image object or any
     *                      descendant exists; not deleted within dcmimage unless configuration flag
     *                      CIF_TakeOverExternalDataset is set - in this case do not delete it at all)
     *  @param  xfer       transfer syntax
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
     ** @param  object       pointer to DICOM data structures
     *                       (do not delete while referenced, i.e. while this image object or any
     *                        descendant exists; not deleted within dcmimage unless configuration flag
     *                        CIF_TakeOverExternalDataset is set - in this case do not delete it at all)
     *  @param  xfer         transfer syntax
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
     *  DICOM file/dataset.  It rather refers to the number of frames processed by this class
     *  (see constructors for details).
     *
     ** @return number of frames
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
     *  attributes are supported in order to determine this value:
     *  - Pixel Spacing, Imager Pixel Spacing, Nominal Scanned Pixel Spacing, Pixel Aspect Ratio
     ** @return pixel aspect ratio (floating point value)
     */
    inline double getWidthHeightRatio() const
    {
        return (Image != NULL) ?
            Image->getColumnRowRatio() : 0;
    }

    /** get height width ratio (pixel aspect ratio: y/x).
     *  If present in the dataset and not overwritten using setWidthHeightRatio(), the following
     *  attributes are supported in order to determine this value:
     *  - Pixel Spacing, Imager Pixel Spacing, Nominal Scanned Pixel Spacing, Pixel Aspect Ratio
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
     *  Supported output color models: Monochrome 2, RGB (and YCbCr_Full if flag
     *  CIF_KeepYCbCrColorModel set).  The rendered pixel data is alway unsigned.
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
     *  Supported output color models: Monochrome 2, RGB (and YCbCr_Full if flag
     *  CIF_KeepYCbCrColorModel set).  The rendered pixel data is alway unsigned.
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
     *  Supported output color models: Monochrome 2, RGB (and YCbCr_Full if flag
     *  CIF_KeepYCbCrColorModel set).  The rendered pixel data is alway unsigned.
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
     ** @return true if sucessful (1 = window has changed,
     *                             2 = new window is the same as previous one),
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
     ** @param  thresh  threshhold value specifying percentage of histogram border which
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
     ** @return true if sucessful (1 = window has changed,
     *                             2 = new window is the same as previous one),
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

    /** set specified window (given by index to window width/center sequence stored in image file).
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

    /** set specified window (given by window width and center).
     *  possibly active VOI LUT is implicitly disabled.
     *
     ** @param  center  center of specified window
     *  @param  width   width of specified window (> 0.0)
     *
     ** @return true if sucessful (1 = window has changed,
     *                             2 = new window is the same as previous one),
     *          false otherwise
     */
    inline int setWindow(const double center,
                         const double width)
    {
        return ((Image != NULL) && (Image->getMonoImagePtr() != NULL)) ?
            Image->getMonoImagePtr()->setWindow(center, width) : 0;
    }

    /** get current window center and width values
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

    /** set VOI LUT function.
     *  NB: Implementation of sigmoid transformation in "dimoopxt.h" is still missing.
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
     *  NB: Implementation of sigmoid transformation in "dimoopxt.h" is still missing.
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
     *  @param  left_pos     x coordinate of plane orgin (referring to image origin)
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
     *  see show/hideOverlay() to modifiy the visibility status.
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
     *  @param  thresh  treshhold value (in percent, default: 0.5), only for EMO_TreshholdReplace
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
     *  @param  thresh  treshhold value (in percent, default: 0.5), only for EMO_TreshholdReplace
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
     ** @return mode of overlay plane if succesful, EMO_Default otherwise
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
     *                       (referring to image orgin, negative values create a border around the image)
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
     *                    (referring to image orgin, negative values create a border around the image)
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
     *  describes the size (in bytes) of the allocated buffer, is suffiently large, the bitmap is stored
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
     ** @return number of bytes allocated by the bitmap, or 0 if an error occured
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
     ** @return number of bytes allocated by the bitmap, or 0 if an error occured
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
     *  Supported output color models: Monochrome 2, RGB (and YCbCr_Full if flag
     *  CIF_KeepYCbCrColorModel set).
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
     *  Supported output color models: Monochrome 1/2, RGB (and YCbCr_Full if flag
     *  CIF_KeepYCbCrColorModel set).
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

    /** write pixel data to plugable image format file (specified by open C stream).
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

    /** write pixel data to plugable image format file (specified by filename).
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


/*
 *
 * CVS/RCS Log:
 * $Log: dcmimage.h,v $
 * Revision 1.65  2010-10-14 13:16:25  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.64  2010-10-05 15:24:02  joergr
 * Added preliminary support for VOI LUT function. Please note, however, that
 * the sigmoid transformation is not yet implemented.
 *
 * Revision 1.63  2010-03-02 09:23:32  joergr
 * Enhanced comments of some methods, e.g. processNextFrames().
 *
 * Revision 1.62  2009-11-25 15:07:00  joergr
 * Added new method which allows for processing the frames of a multi-frame
 * image successively. The getString() method now returns the Defined Term of
 * the attribute PhotometricInterpretation.
 *
 * Revision 1.61  2009-02-12 12:03:56  joergr
 * Never update value of ImagerPixelSpacing when image is scaled, use
 * PixelSpacing instead.
 * Added support for NominalScannedPixelSpacing in order to determine the pixel
 * aspect ratio (used for the new SC image IODs).
 *
 * Revision 1.60  2008-07-11 08:35:28  joergr
 * Fixed typo in API documentation.
 *
 * Revision 1.59  2008-05-20 10:02:33  joergr
 * Added new bilinear and bicubic scaling algorithms for image magnification.
 *
 * Revision 1.58  2008-05-13 09:54:40  joergr
 * Added new parameter to writeImageToDataset() in order to affect the planar
 * configuration of the output image/dataset. Changed behaviour: By default,
 * the output now uses the same planar configuration as the "original" image
 * (previously: always color-by-plane).
 *
 * Revision 1.57  2007/03/16 11:56:05  joergr
 * Introduced new flag that allows to select how to handle the BitsPerTableEntry
 * value in the LUT descriptor (use, ignore or check).
 *
 * Revision 1.56  2006/08/15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.55  2006/07/10 10:52:27  joergr
 * Added support for 32-bit BMP images.
 *
 * Revision 1.54  2005/12/08 16:47:31  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.53  2005/03/09 17:33:40  joergr
 * Added mode to writeImageToDataset() which allows the value of BitsStored to
 * be determined either from 'used' or from 'possible' pixel values.
 *
 * Revision 1.52  2004/07/20 18:12:16  joergr
 * Added API method to "officially" access the internal intermediate pixel data
 * representation (e.g. to get Hounsfield Units for CT images).
 *
 * Revision 1.51  2003/12/17 16:17:29  joergr
 * Added new compatibility flag that allows to ignore the third value of LUT
 * descriptors and to determine the bits per table entry automatically.
 *
 * Revision 1.50  2003/12/11 17:22:19  joergr
 * Added comment to getOutputData/Plane() methods that the rendered pixel data
 * is always unsigned.
 *
 * Revision 1.49  2003/12/08 18:39:00  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated CVS header.
 *
 * Revision 1.48  2003/06/12 15:08:34  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.47  2003/05/20 09:24:31  joergr
 * Added method returning the number of bytes required to store a single
 * rendered frame: getOutputDataSize().
 *
 * Revision 1.46  2002/12/09 13:32:50  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.45  2002/10/21 10:09:58  joergr
 * Slightly enhanced comments for getOutputData().
 *
 * Revision 1.44  2002/08/21 09:51:43  meichel
 * Removed DicomImage and DiDocument constructors that take a DcmStream
 *   parameter
 *
 * Revision 1.43  2002/08/02 15:02:34  joergr
 * Enhanced writeFrameToDataset() routine (remove out-data DICOM attributes
 * from the dataset).
 * Added function to write the current image (not only a selected frame) to a
 * DICOM dataset.
 *
 * Revision 1.42  2002/07/19 08:24:20  joergr
 * Enhanced/corrected comments.
 *
 * Revision 1.41  2002/07/05 10:37:47  joergr
 * Added comments.
 *
 * Revision 1.40  2002/06/26 16:00:25  joergr
 * Added support for polarity flag to color images.
 * Added new method to write a selected frame to a DICOM dataset (incl. required
 * attributes from the "Image Pixel Module").
 * Added new methods to get the explanation string of stored VOI windows and
 * LUTs (not only of the currently selected VOI transformation).
 *
 * Revision 1.39  2002/01/29 17:05:49  joergr
 * Added optional flag to the "Windows DIB" methods allowing to switch off the
 * scanline padding.
 *
 * Revision 1.38  2001/11/27 18:18:20  joergr
 * Added support for plugable output formats in class DicomImage. First
 * implementation is JPEG.
 *
 * Revision 1.37  2001/11/19 12:54:29  joergr
 * Added parameter 'frame' to setRoiWindow().
 *
 * Revision 1.36  2001/11/09 16:25:13  joergr
 * Added support for Window BMP file format.
 * Enhanced and renamed createTrueColorDIB() method.
 *
 * Revision 1.35  2001/09/28 13:00:55  joergr
 * Changed default behaviour of setMinMaxWindow().
 * Added routines to get the currently active Polarity and PresentationLUTShape.
 * Added method setRoiWindow() which automatically calculates a min-max VOI
 * window for a specified rectangular region of the image.
 * Added method to extract embedded overlay planes from pixel data and store
 * them in group (6xxx,3000) format.
 * Added new flag (CIF_KeepYCbCrColorModel) which avoids conversion of YCbCr
 * color models to RGB.
 *
 * Revision 1.34  2001/06/20 15:12:49  joergr
 * Enhanced multi-frame support for command line tool 'dcm2pnm': extract all
 * or a range of frames with one call.
 *
 * Revision 1.33  2001/05/14 09:49:17  joergr
 * Added support for "1 bit output" of overlay planes; useful to extract
 * overlay planes from the pixel data and store them separately in the dataset.
 *
 * Revision 1.32  2001/05/10 16:46:26  joergr
 * Enhanced comments of some overlay related methods.
 *
 * Revision 1.31  2000/07/07 13:42:11  joergr
 * Added support for LIN OD presentation LUT shape.
 *
 * Revision 1.30  2000/06/07 14:30:26  joergr
 * Added method to set the image polarity (normal, reverse).
 *
 * Revision 1.29  2000/04/27 13:08:37  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.28  2000/03/08 16:24:13  meichel
 * Updated copyright header.
 *
 * Revision 1.27  2000/03/06 18:16:02  joergr
 * Removed inline specifier from a 'large' method (reported by Sun CC 4.2).
 *
 * Revision 1.26  1999/11/19 12:36:55  joergr
 * Added explicit type cast to avoid compiler warnings (reported by gcc
 * 2.7.2.1 on Linux).
 *
 * Revision 1.25  1999/10/20 10:32:05  joergr
 * Enhanced method getOverlayData to support 12 bit data for print.
 *
 * Revision 1.24  1999/10/06 13:26:08  joergr
 * Corrected creation of PrintBitmap pixel data: VOI windows should be applied
 * before clipping to avoid that the region outside the image (border) is also
 * windowed (this requires a new method in dcmimgle to create a DicomImage
 * with the grayscale transformations already applied).
 *
 * Revision 1.23  1999/09/17 12:06:17  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.22  1999/09/10 08:45:17  joergr
 * Added support for CIELAB display function.
 *
 * Revision 1.21  1999/09/08 15:19:23  joergr
 * Completed implementation of setting inverse presentation LUT as needed
 * e.g. for DICOM print (invert 8->12 bits PLUT).
 *
 * Revision 1.20  1999/08/25 16:38:48  joergr
 * Allow clipping region to be outside the image (overlapping).
 *
 * Revision 1.19  1999/07/23 13:50:07  joergr
 * Added methods to set 'PixelAspectRatio'.
 * Added dummy method (no implementation yet) to create inverse LUTs.
 * Added method to create 12 bit packed bitmap data (used for grayscale print
 * storage).
 * Added method to return pointer to currently used display function.
 * Added new interpolation algorithm for scaling.
 *
 * Revision 1.18  1999/05/10 09:33:54  joergr
 * Moved dcm2pnm version definition from module dcmimgle to dcmimage.
 *
 * Revision 1.17  1999/05/03 11:09:27  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.16  1999/04/28 14:45:54  joergr
 * Added experimental support to create grayscale images with more than 256
 * shades of gray to be displayed on a consumer monitor (use pastel colors).
 *
 * Revision 1.15  1999/03/24 17:19:56  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.14  1999/03/22 08:51:06  joergr
 * Added parameter to specify (transparent) background color for method
 * getOverlayData().
 * Added/Changed comments.
 *
 * Revision 1.13  1999/03/03 11:43:39  joergr
 * Changed comments.
 *
 * Revision 1.12  1999/02/11 15:35:04  joergr
 * Added routine to check whether particular grayscale values are unused in
 * the output data.
 *
 * Revision 1.11  1999/02/09 14:21:08  meichel
 * Corrected const signatures of some ctor declarations
 *
 * Revision 1.10  1999/02/08 12:37:35  joergr
 * Changed implementation of removeAllOverlays().
 * Added parameter 'idx' to some overlay methods to distinguish between
 * built-in and additional overlay planes.
 *
 * Revision 1.9  1999/02/05 16:42:22  joergr
 * Added optional parameter to method convertPValueToDDL to specify width
 * of output data (number of bits).
 *
 * Revision 1.8  1999/02/03 16:59:54  joergr
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 * Revision 1.7  1999/01/20 14:58:26  joergr
 * Added new output method to fill external memory buffer with rendered pixel
 * data.
 *
 * Revision 1.6  1999/01/11 09:31:20  joergr
 * Added parameter to method 'getMinMaxValues()' to return absolute minimum
 * and maximum values ('possible') in addition to actually 'used' pixel
 * values.
 *
 * Revision 1.5  1998/12/23 11:31:58  joergr
 * Changed order of parameters for addOverlay() and getOverlayData().
 *
 * Revision 1.3  1998/12/16 16:26:17  joergr
 * Added explanation string to LUT class (retrieved from dataset).
 * Added explanation string for VOI transformations.
 * Added method to export overlay planes (create 8-bit bitmap).
 * Renamed 'setNoVoiLutTransformation' method ('Voi' instead of 'VOI').
 * Removed several methods used for monochrome images only in base class
 * 'DiImage'. Introduced mechanism to use the methods directly.
 *
 * Revision 1.2  1998/12/14 17:14:07  joergr
 * Added methods to add and remove additional overlay planes (still untested).
 * Added methods to support overlay labels and descriptions.
 *
 * Revision 1.1  1998/11/27 14:50:00  joergr
 * Added copyright message.
 * Added methods to convert module defined enum types to strings.
 * Added methods to support presentation LUTs and shapes.
 * Moved type definitions to diutils.h.
 * Added constructors to use external modality transformations.
 * Added method to directly create java AWT bitmaps.
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 *
 * Revision 1.12  1998/07/01 08:39:17  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.11  1998/06/25 08:50:09  joergr
 * Added compatibility mode to support ACR-NEMA images and wrong
 * palette attribute tags.
 *
 * Revision 1.10  1998/05/11 14:53:07  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
