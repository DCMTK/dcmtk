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
 *  Purpose: DicomMonochromeImage (Header)
 *
 */


#ifndef DIMOIMG_H
#define DIMOIMG_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/diimage.h"
#include "dcmtk/dcmimgle/dimopx.h"
#include "dcmtk/dcmimgle/dimoopx.h"
#include "dcmtk/dcmimgle/didispfn.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiColorImage;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Base class for monochrome images
 */
class DCMTK_DCMIMGLE_EXPORT DiMonoImage
  : public DiImage
{

 public:

    /** constructor
     *
     ** @param  docu    pointer to the DICOM document
     *  @param  status  status of the image object
     */
    DiMonoImage(const DiDocument *docu,
                const EI_Status status);

    /** constructor, rescale
     *
     ** @param  docu       pointer to the DICOM document
     *  @param  status     status of the image object
     *  @param  slope      rescale slope
     *  @param  intercept  rescale intercept
     */
    DiMonoImage(const DiDocument *docu,
                const EI_Status status,
                const double slope,
                const double intercept);

    /** constructor, modality LUT
     *
     ** @param  docu         pointer to the DICOM document
     *  @param  status       status of the image object
     *  @param  data         element containing the modality LUT data
     *  @param  descriptor   element containing the modality LUT descriptor
     *  @param  explanation  element containing the modality LUT explanation (optional)
     */
    DiMonoImage(const DiDocument *docu,
                const EI_Status status,
                const DcmUnsignedShort &data,
                const DcmUnsignedShort &descriptor,
                const DcmLongString *explanation);

    /** destructor
     */
    virtual ~DiMonoImage();

    /** process next couple of frames
     *
     ** @param  fcount  number of frames to be processed (0 = same number as before)
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int processNextFrames(const unsigned long fcount);

    /** get minimum and maximum pixel values.
     *  the resulting pixel values are stored in 'double' variables to avoid problems
     *  with different number ranges, limited to monochrome images
     *
     ** @param  min   minimum pixel value (reference parameter)
     *  @param  max   maximum pixel value (reference parameter)
     *  @param  mode  0 = min/max 'used' pixel values,
     *                1 = min/max 'possible' pixel values (absolute min/max)
     *
     ** @return status code (true if successful)
     */
    int getMinMaxValues(double &min,
                        double &max,
                        const int mode) const;

    /** get display function
     *
     ** @return pointer to current display function, NULL if absent
     */
    DiDisplayFunction *getDisplayFunction() const
    {
        return DisplayFunction;
    }

    /** set display function
     *
     ** @param  display  object describing the output device characteristic (only referenced!)
     *
     ** @return true if successful, false otherwise
     */
    int setDisplayFunction(DiDisplayFunction *display);

    /** set no display function.
     *  disables display function transformation, object is not deleted!
     *
     ** @return true  if successful (1 = disabled current function,
     *                               2 = there was no function to disable)
     *          false otherwise
     */
    int setNoDisplayFunction();

    /** unset all VOI transformations (windows and LUTs).
     *  only applicable for monochrome images
     *
     ** @return true if successful (1 = previous window/LUT has been valid,
     *                              2 = otherwise),
     *          false otherwise (image is invalid or not monochrome)
     */
    int setNoVoiTransformation();

    /** delete specified display LUT(s)
     *
     ** @param  bits  parameter of LUT to be deleted (0 = all)
     *
     ** @return true if successful, false otherwise
     */
    inline int deleteDisplayLUT(const int bits)
    {
        return (DisplayFunction != NULL) ? DisplayFunction->deleteLookupTable(bits) : 0;
    }

    /** check whether given output value is unused
     *
     ** @param  value  output value to be checked
     *
     ** @return status, true if unused (1 = within output range, 2 = out of range), false otherwise
     */
    inline int isValueUnused(const unsigned long value)
    {
        return (OutputData != NULL) ? OutputData->isUnused(value) : 0;
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
    int convertPValueToDDL(const Uint16 pvalue,
                           Uint16 &ddl,
                           const int bits);

    /** set automatically calculated minimum/maximum window.
     *  possibly active VOI LUT is implicitly disabled.
     *
     ** @param  idx  ignore global min/max values if false (0)
     *
     ** @return true if successful (1 = window has changed,
     *                              2 = new window is the same as previous one),
     *          false otherwise
     */
    int setMinMaxWindow(const int idx = 1);

    /** set automatically calculated VOI window for the specified Region of Interest (ROI).
     *  The ROI is specified by means of a rectangle (left_pos, top_pos, width, height).
     *  Possibly active VOI LUT is implicitly disabled.
     *
     ** @param  left_pos  x-coordinate of the top left-hand corner of the ROI (starting from 0)
     *  @param  top_pos   y-coordinate of the top left-hand corner of the ROI (starting from 0)
     *  @param  width     width in pixels of the rectangular ROI (minimum: 1)
     *  @param  height    height in pixels of the rectangular ROI (minimum: 1)
     *  @param  frame     index of the frame to be used for the calculation
     *
     ** @return true if successful (1 = window has changed,
     *                              2 = new window is the same as previous one),
     *          false otherwise
     */
    int setRoiWindow(const unsigned long left_pos,
                     const unsigned long top_pos,
                     const unsigned long width,
                     const unsigned long height,
                     const unsigned long frame);

    /** set automatically calculated histogram window.
     *  possibly active VOI LUT is implicitly disabled.
     *
     ** @param  thresh  threshold value specifying percentage of histogram border which shall be ignored
     *
     ** @return true if successful, false otherwise
     */
    int setHistogramWindow(const double thresh);

    /** set specified window (given by index to window width/center sequence stored in image file).
     *  possibly active VOI LUT is implicitly disabled.
     *
     ** @param  pos  index to window width/center sequence
     *
     ** @return true if successful, false otherwise
     */
    int setWindow(const unsigned long pos = 0);

    /** set specified window (given by window width and center).
     *  possibly active VOI LUT is implicitly disabled.
     *
     ** @param  center       center of specified window
     *  @param  width        width of specified window
     *  @param  explanation  window explanation (optional)
     *
     ** @return true if successful (1 = window has changed,
     *                              2 = new window is the same as previous one),
     *          false otherwise
     */
    int setWindow(const double center,
                  const double width,
                  const char *explanation = NULL);

    /** get current window center and width values
     *
     ** @param  center  return current window center value
     *  @param  width   return current window width value
     *
     ** @return true if successful, false otherwise
     */
    int getWindow(double &center,
                  double &width);

    /** get number of VOI windows (stored in image file)
     *
     ** @return number of VOI windows
     */
    inline unsigned long getWindowCount() const
    {
        return WindowCount;
    }

    /** get VOI LUT function.
     *  possible values are: EFV_Default, EFV_Linear, EFV_Sigmoid.
     *
     ** @return currently active VOI LUT function
     */
    EF_VoiLutFunction getVoiLutFunction() const;

    /** set VOI LUT function
     *
     ** @param  function  type of VOI LUT function (default, linear or sigmoid).
     *                    'default' basically means the same as 'linear'.
     *
     ** @return true if successful (1 = function has changed,
     *                              2 = function has not changed)
     *          false otherwise
     */
    int setVoiLutFunction(const EF_VoiLutFunction function);

    /** set VOI LUT (given by dcmdata elements).
     *  possibly active window/center is implicitly disabled.
     *
     ** @param  data         contains LUT data
     *  @param  descriptor   describes LUT structure
     *  @param  explanation  free form description of VOI LUT (optional)
     *  @param  descripMode  mode specifying the use of the bits per table entry value
     *
     ** @return true if successful, false otherwise
     */
    int setVoiLut(const DcmUnsignedShort &data,
                  const DcmUnsignedShort &descriptor,
                  const DcmLongString *explanation,
                  const EL_BitsPerTableEntry descripMode = ELM_UseValue);

    /** set VOI LUT (given by index to VOI LUT sequence stored in image file).
     *  possibly active window/center is implicitly disabled.
     *
     ** @param  pos          index to VOI LUT sequence
     *  @param  descripMode  mode specifying the use of the bits per table entry value
     *
     ** @return true if successful, false otherwise
     */
    int setVoiLut(const unsigned long pos,
                  const EL_BitsPerTableEntry descripMode = ELM_UseValue);

    /** get number of VOI LUTs (stored in image file)
     *
     ** @return number of VOI LUTs
     */
    inline unsigned long getVoiLutCount() const
    {
        return VoiLutCount;
    }

    /** get description of active VOI transformation
     *
     ** @return pointer to description text (NULL if absent)
     */
    inline const char *getVoiTransformationExplanation() const
    {
        return VoiExplanation.c_str();
    }

    /** get description of specified VOI window (stored in the image file)
     *
     ** @param  pos          index of the stored VOI window (0..n-1)
     *  @param  explanation  variable in which the result text is stored
     *
     ** @return pointer to description text (NULL if absent or index invalid)
     */
    const char *getVoiWindowExplanation(const unsigned long pos,
                                        OFString &explanation) const;

    /** get description of specified VOI LUT (stored in the image file)
     *
     ** @param  pos          index of the stored VOI LUT (0..n-1)
     *  @param  explanation  variable in which the result text is stored
     *
     ** @return pointer to description text (NULL if absent or index invalid)
     */
    const char *getVoiLutExplanation(const unsigned long pos,
                                     OFString &explanation) const;

    /** get description of performed modality LUT transformation
     *
     ** @return pointer to description text (NULL if absent)
     */
    inline const char *getModalityLutExplanation() const
    {
        return (InterData != NULL) ? InterData->getModalityLutExplanation() : OFstatic_cast(const char *, NULL);
    }

    /** set hardcopy parameters. (used to display LinOD images)
     *
     ** @param  min      minimum density of the print-out (in hundreds of Optical Density, e.g. 150 means 1.5 OD)
     *  @param  max      maximum density of the print-out (ditto)
     *  @param  reflect  reflected ambient light (in candela per square meter - cd/m^2)
     *  @param  illumin  illumination (ditto)
     *
     ** @return true if successful (1 = at least one of the parameters has changed,
     *                              2 = no parameter has changed)
     *          false otherwise
     */
     int setHardcopyParameters(const unsigned int min,
                               const unsigned int max,
                               const unsigned int reflect,
                               const unsigned int illumin);

    /** get shape for presentation transformation.
     *  possible values are: ESP_Default, ESP_Identity, ESP_Inverse, ESP_LinOD
     *  If a presentation LUT is currently active ESP_Default is always returned.
     *
     ** @return currently active presentation LUT shape
     */
    ES_PresentationLut getPresentationLutShape() const;

    /** set shape for presentation transformation.
     *  possibly active presentation LUT is implicitly disabled.
     *
     ** @param  shape  presentation LUT shape (default, identity or inverse).
     *                 'default' means that the output data is always created with 0 for black
     *                 and maxvalue for white (i.e. monochrome2 data is created for output).
     *
     ** @return true if successful (1 = shape has changed,
     *                              2 = shape has not changed)
     *          false otherwise
     */
    int setPresentationLutShape(const ES_PresentationLut shape);

    /** set LUT for presentation transformation.
     *  possibly active presentation LUT is implicitly disabled.
     *
     ** @param  data         contains LUT data
     *  @param  descriptor   describes LUT structure
     *  @param  explanation  free form description of presentation LUT (optional)
     *  @param  descripMode  mode specifying the use of the bits per table entry value
     *
     ** @return true if successful, false otherwise
     */
    int setPresentationLut(const DcmUnsignedShort &data,
                           const DcmUnsignedShort &descriptor,
                           const DcmLongString *explanation = NULL,
                           const EL_BitsPerTableEntry descripMode = ELM_UseValue);

    /** set inverse LUT for presentation transformation.
     *  this LUT transform is e.g. used for DICOM print (12->8, 8->12 bit)
     *  possibly active presentation LUT will not be considered !
     *
     ** @param  data         contains LUT data
     *  @param  descriptor   describes LUT structure
     *  @param  descripMode  mode specifying the use of the bits per table entry value
     *
     ** @return true if successful, false otherwise
     */
    int setInversePresentationLut(const DcmUnsignedShort &data,
                                  const DcmUnsignedShort &descriptor,
                                  const EL_BitsPerTableEntry descripMode = ELM_UseValue);

    /** get description of active presentation LUT
     *
     ** @return pointer to description text (NULL if absent)
     */
    inline const char *getPresentationLutExplanation() const
    {
        return (PresLutData != NULL) ? PresLutData->getExplanation() : OFstatic_cast(const char *, NULL);
    }

    /** add specified plane to group of additional overlay planes.
     *  replaces old overlay plane if group number already exists.
     *
     ** @param  group        group number (0x60nn) of overlay plane
     *  @param  left_pos     x coordinate of plane orgin (referring to image origin)
     *  @param  top_pos      y coordinate of plane origin
     *  @param  columns      width of overlay plane (in pixels)
     *  @param  rows         height of overlay plane (in pixels)
     *  @param  data         overlay plane data (dcmdata element)
     *  @param  label        overlay plane label
     *  @param  description  overlay plane description
     *  @param  mode         display mode (see 'diutils.h')
     *
     ** @return false (0) if an error occurred, true otherwise (1 = added new plane,
     *                                                          2 = replaced existing plane)
     */
    int addOverlay(const unsigned int group,
                   const signed int left_pos,
                   const signed int top_pos,
                   const unsigned int columns,
                   const unsigned int rows,
                   const DcmOverlayData &data,
                   const DcmLongString &label,
                   const DcmLongString &description,
                   const EM_Overlay mode);

    /** remove all additional overlay planes
     *
     ** @return false (0) if an error occurred, true otherwise
     */
    int removeAllOverlays();

    /** get pointer to specified overlay managing object
     *
     ** @param  idx  index of overlay group (0 = dataset, 1 = additional)
     *
     ** @return pointer to overlay managing object (NULL if absent or invalid)
     */
    inline DiOverlay *getOverlayPtr(const unsigned int idx)
    {
        return (idx < 2) ? Overlays[idx] : OFstatic_cast(DiOverlay *, NULL);
    }

    /** get pointer to current monochrome image (this)
     *
     ** @return pointer to this object
     */
    inline DiMonoImage *getMonoImagePtr()
    {
        return this;
    }

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

    /** rotate current image (by steps of 90 degrees)
     *
     ** @param  degree  angle by which the image shall be rotated (-360, -270, -180, -90, 0, 90, 180, 270, 360)
     *
     ** @return true if successful (1 = rotated by at least 90 degrees,
     *                              2 = not rotated, because of image resolution or angle),
     *          false otherwise
     */
    int rotate(const int degree);

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
        return (((bits < 1) || (bits > MAX_BITS)) && (bits != MI_PastelColor)) ? BitsPerSample : bits;
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
     *
     ** @param  frame   number of frame to be rendered
     *  @param  bits    number of bits for the output pixel data (depth)
     *  @param  planar  flag, only useful for multi-planar images (color)
     *
     ** @return untyped pointer to the pixel data if successful, NULL otherwise
     */
    virtual const void *getOutputData(const unsigned long frame,
                                      const int bits,
                                      const int planar = 0) = 0;

    /** get pixel data with specified format.
     *  (memory is handled externally)
     *
     ** @param  buffer  untyped pointer to the externally allocated memory buffer
     *  @param  size    size of the memory buffer in bytes (will be checked)
     *  @param  frame   number of frame to be rendered
     *  @param  bits    number of bits for the output pixel data (depth)
     *  @param  planar  flag, only useful for multi-planar images (color)
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int getOutputData(void *buffer,
                              const unsigned long size,
                              const unsigned long frame,
                              const int bits,
                              const int planar = 0) = 0;

    /** get pixel data of specified plane.
     *  (memory is handled internally)
     *
     ** @param  dummy  (not used)
     *
     ** @return untyped pointer to the pixel data if successful, NULL otherwise
     */
    const void *getOutputPlane(const int dummy) const;

    /** delete internally handled output memory buffer
     *  Save memory if data is no longer needed.
     */
    void deleteOutputData();

    /** create bitmap for specified overlay plane.
     *  (up to 16 bits per pixel with two values: fore and back)
     *
     ** @param  frame     index of frame used for output
     *  @param  plane     number (0..15) or group number (0x60nn) of overlay plane
     *  @param  left_pos  returns x coordinate of plane's origin
     *  @param  top_pos   returns y coordinate of plane's origin
     *  @param  width     returns width of overlay plane (in pixels)
     *  @param  height    returns height of overlay plane (in pixels)
     *  @param  mode      return display mode (see 'diutils.h')
     *  @param  idx       index of overlay group (0 = dataset, 1 = additional, 2 = '1' plane hides '0' plane)
     *  @param  bits      number of bits (stored) in the resulting array, default: 8
     *  @param  fore      foreground color to be set in bitmap, default: 255
     *  @param  back      background color to be set in bitmap (transparent), default: 0
     *
     ** @return pointer to overlay plane data (internal memory buffer)
     */
    const void *getOverlayData(const unsigned long frame,
                               const unsigned int plane,
                               unsigned int &left_pos,
                               unsigned int &top_pos,
                               unsigned int &width,
                               unsigned int &height,
                               EM_Overlay &mode,
                               const unsigned int idx,
                               const int bits = 8,
                               const Uint16 fore = 0xff,
                               const Uint16 back = 0x0);

    /** create bitmap for specified overlay plane.
     *  (up to 16 bits per pixel with two values: fore and back)
     *
     ** @param  frame   index of frame used for output
     *  @param  plane   number (0..15) or group number (0x60nn) of overlay plane
     *  @param  width   returns width of overlay plane (in pixels)
     *  @param  height  returns height of overlay plane (in pixels)
     *  @param  idx     index of overlay group (0 = dataset, 1 = additional, 2 = '1' plane hides '0' plane)
     *  @param  bits    number of bits (stored) in the resulting array, default: 8
     *  @param  fore    foreground color to be set in bitmap, default: 255
     *  @param  back    background color to be set in bitmap (transparent), default: 0
     *
     ** @return pointer to overlay plane data (internal memory buffer)
     */
    const void *getFullOverlayData(const unsigned long frame,
                                   const unsigned int plane,
                                   unsigned int &width,
                                   unsigned int &height,
                                   const unsigned int idx,
                                   const int bits = 8,
                                   const Uint16 fore = 0xff,
                                   const Uint16 back = 0x0);

    /** delete buffer for overlay plane data.
     *  Save memory if data is no longer needed.
     */
    void deleteOverlayData();

    /** create bitmap for specified overlay plane and store it in (6xxx,3000) format.
     *  (1 bit allocated and stored, foreground color is 1, background color is 0,
     *   data is 16 bit padded - even length)
     *  memory is not handled internally - must be deleted from calling program.
     *
     ** @param  buffer  stores pointer to overlay data (memory is allocated internally)
     *  @param  plane   number (0..15) or group number (0x60nn) of overlay plane
     *  @param  width   returns width of overlay plane (in pixels)
     *  @param  height  returns height of overlay plane (in pixels)
     *  @param  frames  returns number of frames (multiple overlay frames possible!)
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
                                            const unsigned int idx = 0);

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
    const DiMonoPixel *getMonoInterData() const
    {
        return InterData;
    }

    /** create true color (24/32 bit) or palette (8 bit) bitmap for MS Windows.
     *  memory is not handled internally - must be deleted from calling program.
     *
     ** @param  data        untyped pointer memory buffer (set to NULL if not allocated externally)
     *  @param  size        size of the memory buffer in bytes (if 0 'data' is set to NULL)
     *  @param  frame       index of frame to be converted (starting from 0)
     *  @param  bits        number of bits per pixel used for the output bitmap (8, 24 or 32)
     *  @param  upsideDown  specifies the order of lines in the images (0 = top-down, bottom-up otherwise)
     *  @param  padding     align each line to a 32-bit address if true (default)
     *
     ** @return number of bytes allocated by the bitmap, or 0 if an error occurred
     */
    unsigned long createDIB(void *&data,
                            const unsigned long size,
                            const unsigned long frame,
                            const int bits,
                            const int upsideDown,
                            const int padding = 1);

    /** create true color (32 bit) or palette (8 bit) bitmap for Java (AWT default format).
     *  Memory is not handled internally - must be deleted from calling program.
     *
     ** @param  data   resulting pointer to bitmap data (set to NULL if an error occurred)
     *  @param  frame  index of frame to be converted (starting from 0)
     *  @param  bits   number of bits per pixel used for the output bitmap (8 or 32)
     *
     ** @return number of bytes allocated by the bitmap, or 0 if an error occurred
     */
    unsigned long createAWTBitmap(void *&data,
                                  const unsigned long frame,
                                  const int bits);

    /** create packed bitmap (e.g. 12/16 bit -> 12/12 bit for DICOM printers).
     *  Memory is not handled internally - must be deleted from calling program.
     *
     ** @param  buffer  pointer to input memory buffer
     *  @param  size    size of memory buffer (will be checked whether it is sufficient)
     *  @param  count   number of entries (pixels) in input buffer
     *  @param  alloc   number of bits allocated for the pixels stored in the buffer (e.g. 16)
     *  @param  stored  number of bits stored for the pixels stored in the buffer (e.g. 12)
     *
     ** @return pointer to memory buffer containing the packed output bitmap data (NULL if an error occurred)
     */
    static void *createPackedBitmap(const void *buffer,
                                    const unsigned long size,
                                    const unsigned long count,
                                    const int alloc,
                                    const int stored);

    /** create new single frame DiImage with applied grayscale transformations.
     *  The method getOutputData() is used internally for the new bitmap.
     *
     ** @param  frame  index of frame to be converted
     *  @param  bits   number of bits per pixel used for the output bitmap
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    DiImage *createOutputImage(const unsigned long frame,
                               const int bits);

    /** write current image and related attributes to DICOM dataset.
     *
     ** @param  dataset  reference to DICOM dataset where the image attributes are stored
     *  @param  mode     0 = determine value of BitsStored from 'used' pixel values,
     *                   1 = determine value of BitsStored from 'possible' pixel values
     *  @param  planar   dummy parameter (only used for color images)
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
     *  @param  bits    number of bits used for output of pixel data (8, 24 or 32, default (0) = 8)
     *
     ** @return true if successful, false otherwise
     */
    int writeBMP(FILE *stream,
                 const unsigned long frame,
                 const int bits);


 protected:

    /** constructor
     *
     ** @param  image   pointer to dataset (encapsulated)
     *  @param  status  current image status
     *  @param  dummy   (necessary to be different from another constructor)
     */
    DiMonoImage(const DiDocument *image,
                const EI_Status status,
                const char dummy);

    /** constructor, copy
     *
     ** @param  image   pointer to reference image
     *  @param  fstart  first frame to be processed
     *  @param  fcount  number of frames
     */
    DiMonoImage(const DiMonoImage *image,
                const unsigned long fstart,
                const unsigned long fcount);

    /** constructor, convert color images to monochrome
     *
     ** @param  image  pointer to reference image
     *  @param  red    coefficient by which the red component is weighted
     *  @param  green  coefficient by which the green component is weighted
     *  @param  blue   coefficient by which the blue component is weighted
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    DiMonoImage(const DiColorImage *image,
                const double red,
                const double green,
                const double blue);

    /** constructor, scale/clip
     *
     ** @param  image         pointer to reference image
     *  @param  left_pos      x coordinate of top left corner of area to be scaled
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
     *  @param  pvalue        P-value used for the border outside the image (0..65535)
     */
    DiMonoImage(const DiMonoImage *image,
                const signed long left_pos,
                const signed long top_pos,
                const Uint16 src_cols,
                const Uint16 src_rows,
                const Uint16 dest_cols,
                const Uint16 dest_rows,
                const int interpolate,
                const int aspect,
                const Uint16 pvalue);

    /** constructor, flip
     *
     ** @param  image  pointer to reference image
     ** @param  horz   flip horizontally if true
     *  @param  vert   flip vertically if true
     */
    DiMonoImage(const DiMonoImage *image,
                const int horz,
                const int vert);

    /** constructor, rotate
     *
     ** @param  image   pointer to reference image
     *  @param  degree  angle by which the image shall be rotated
     */
    DiMonoImage(const DiMonoImage *image,
                const int degree);

    /** constructor, createMonoOutput
     *
     ** @param  image   pointer to reference image
     *  @param  pixel   pointer to output pixel data used for the new image
     *  @param  frame   number of frame stored in the new image object
     *  @param  stored  number of bits stored
     *  @param  alloc   number of bits allocated
     */
     DiMonoImage(const DiMonoImage *image,
                 DiMonoOutputPixel *pixel,
                 const unsigned long frame,
                 const int stored,
                 const int alloc);

    /** initialize internal data structures and member variables
     *
     ** @param  modality  pointer to object handling the modality transform
     *  @param  reuse     reuse particular information determined in a previous call
     */
    void Init(DiMonoModality *modality,
              const OFBool reuse = OFFalse);

    /** initialize internal data structures (for Uint8)
     *
     ** @param  modality  pointer to object handling the modality transform
     */
    void InitUint8(DiMonoModality *modality);

    /** initialize internal data structures (for Sint8)
     *
     ** @param  modality  pointer to object handling the modality transform
     */
    void InitSint8(DiMonoModality *modality);

    /** initialize internal data structures (for Uint16)
     *
     ** @param  modality  pointer to object handling the modality transform
     */
    void InitUint16(DiMonoModality *modality);

    /** initialize internal data structures (for Sint16)
     *
     ** @param  modality  pointer to object handling the modality transform
     */
    void InitSint16(DiMonoModality *modality);

    /** initialize internal data structures (for Uint32)
     *
     ** @param  modality  pointer to object handling the modality transform
     */
    void InitUint32(DiMonoModality *modality);

    /** initialize internal data structures (for Sint32)
     *
     ** @param  modality  pointer to object handling the modality transform
     */
    void InitSint32(DiMonoModality *modality);

    /** check intermediate pixel representation for consistency
     *
     ** @param  mode  check number of pixels stored in the dataset if true
     */
    int checkInterData(const int mode = 1);

    /** update Image Pixel Module attributes in the given dataset.
     *  Removes possibly existing embedded overlay planes and replaces any
     *  present modality transformation by a linear rescale/slope.
     *  Used in writeXXXToDataset() routines.
     *
     ** @param  dataset  reference to DICOM image dataset
     */
    virtual void updateImagePixelModuleAttributes(DcmItem &dataset);

    /** get pixel data with specified format.
     *  (memory is handled externally)
     *
     ** @param  buffer    untyped pointer to the externally allocated memory buffer
     *  @param  size      size of the memory buffer in bytes (will be checked)
     *  @param  frame     number of frame to be rendered
     *  @param  bits      number of bits for the output pixel data (depth)
     *  @param  planar    flag, only useful for multi-planar images (color)
     *  @param  negative  invert pixel data if true
     *
     ** @return untyped pointer to the pixel data if successful, NULL otherwise
     */
    const void *getData(void *buffer,
                        const unsigned long size,
                        const unsigned long frame,
                        int bits,
                        const int planar,
                        const int negative);

    /** get pixel data with specified format for Uint8 input (helper function).
     *  (memory is handled externally)
     *
     ** @param  buffer   untyped pointer to the externally allocated memory buffer
     *  @param  disp     pointer to current display function object
     *  @param  samples  number of samples per pixel
     *  @param  frame    number of frame to be rendered
     *  @param  bits     number of bits for the output pixel data (depth)
     *  @param  low      output pixel value to which 0 is mapped (min)
     *  @param  high     output pixel value to which 2^bits-1 is mapped (max)
     */
    void getDataUint8(void *buffer,
                      DiDisplayFunction *disp,
                      const int samples,
                      const unsigned long frame,
                      const int bits,
                      const Uint32 low,
                      const Uint32 high);

    /** get pixel data with specified format for Sint8 input (helper function).
     *  (memory is handled externally)
     *
     ** @param  buffer   untyped pointer to the externally allocated memory buffer
     *  @param  disp     pointer to current display function object
     *  @param  samples  number of samples per pixel
     *  @param  frame    number of frame to be rendered
     *  @param  bits     number of bits for the output pixel data (depth)
     *  @param  low      output pixel value to which 0 is mapped (min)
     *  @param  high     output pixel value to which 2^bits-1 is mapped (max)
     */
    void getDataSint8(void *buffer,
                      DiDisplayFunction *disp,
                      const int samples,
                      const unsigned long frame,
                      const int bits,
                      const Uint32 low,
                      const Uint32 high);

    /** get pixel data with specified format for Uint16 input (helper function).
     *  (memory is handled externally)
     *
     ** @param  buffer   untyped pointer to the externally allocated memory buffer
     *  @param  disp     pointer to current display function object
     *  @param  samples  number of samples per pixel
     *  @param  frame    number of frame to be rendered
     *  @param  bits     number of bits for the output pixel data (depth)
     *  @param  low      output pixel value to which 0 is mapped (min)
     *  @param  high     output pixel value to which 2^bits-1 is mapped (max)
     */
    void getDataUint16(void *buffer,
                       DiDisplayFunction *disp,
                       const int samples,
                       const unsigned long frame,
                       const int bits,
                       const Uint32 low,
                       const Uint32 high);

    /** get pixel data with specified format for Sint16 input (helper function).
     *  (memory is handled externally)
     *
     ** @param  buffer   untyped pointer to the externally allocated memory buffer
     *  @param  disp     pointer to current display function object
     *  @param  samples  number of samples per pixel
     *  @param  frame    number of frame to be rendered
     *  @param  bits     number of bits for the output pixel data (depth)
     *  @param  low      output pixel value to which 0 is mapped (min)
     *  @param  high     output pixel value to which 2^bits-1 is mapped (max)
     */
    void getDataSint16(void *buffer,
                       DiDisplayFunction *disp,
                       const int samples,
                       const unsigned long frame,
                       const int bits,
                       const Uint32 low,
                       const Uint32 high);

    /** get pixel data with specified format for Uint32 input (helper function).
     *  (memory is handled externally)
     *
     ** @param  buffer   untyped pointer to the externally allocated memory buffer
     *  @param  disp     pointer to current display function object
     *  @param  samples  number of samples per pixel
     *  @param  frame    number of frame to be rendered
     *  @param  bits     number of bits for the output pixel data (depth)
     *  @param  low      output pixel value to which 0 is mapped (min)
     *  @param  high     output pixel value to which 2^bits-1 is mapped (max)
     */
    void getDataUint32(void *buffer,
                       DiDisplayFunction *disp,
                       const int samples,
                       const unsigned long frame,
                       const int bits,
                       const Uint32 low,
                       const Uint32 high);

    /** get pixel data with specified format for Sint32 input (helper function).
     *  (memory is handled externally)
     *
     ** @param  buffer   untyped pointer to the externally allocated memory buffer
     *  @param  disp     pointer to current display function object
     *  @param  samples  number of samples per pixel
     *  @param  frame    number of frame to be rendered
     *  @param  bits     number of bits for the output pixel data (depth)
     *  @param  low      output pixel value to which 0 is mapped (min)
     *  @param  high     output pixel value to which 2^bits-1 is mapped (max)
     */
    void getDataSint32(void *buffer,
                       DiDisplayFunction *disp,
                       const int samples,
                       const unsigned long frame,
                       const int bits,
                       const Uint32 low,
                       const Uint32 high);

    /** create a presentation look-up table converting the pixel data which is linear to
     *  Optical Density to DDLs of the softcopy device (used to display print images on screen).
     *
     ** @param  count  number of LUT entries
     *  @param  bits   number of bits per entry
     *
     ** @return true if successful, false otherwise
     */
    int createLinODPresentationLut(const unsigned long count, const int bits);

    /// center of current VOI-window
    double WindowCenter;
    /// width of current VOI-window
    double WindowWidth;
    /// number of stored VOI-windows
    unsigned long WindowCount;
    /// number of stored VOU-LUTs
    unsigned long VoiLutCount;

    /// true, if current VOI-window is valid
    int ValidWindow;

    /// free text explanation of current VOI transformation
    OFString VoiExplanation;
    /// VOI LUT function (linear or sigmoid)
    EF_VoiLutFunction VoiLutFunction;
    /// presentation LUT shape (identity, inverse or lin od)
    ES_PresentationLut PresLutShape;

    /// minimum density of a print-out (in hundreds of Optical Density)
    unsigned int MinDensity;
    /// maximum density of a print-out (ditto)
    unsigned int MaxDensity;
    /// reflected ambient light (in candela per square meter)
    unsigned int Reflection;
    /// illumination (ditto)
    unsigned int Illumination;

    /// points to associated overlay-objects ([0] = built-in, [1] = additional)
    DiOverlay *Overlays[2];
    /// points to associated VOI-LUT-object
    DiLookupTable *VoiLutData;
    /// points to associated presentation-LUT-object
    DiLookupTable *PresLutData;
    /// points to intermediate pixel data representation (object)
    DiMonoPixel *InterData;

    /// points to grayscale standard display function (only referenced!)
    DiDisplayFunction *DisplayFunction;


 private:

    /// points to current output data (object)
    DiMonoOutputPixel *OutputData;
    /// points to current overlay plane data (pixel array)
    void *OverlayData;

 // --- declarations to avoid compiler warnings

    DiMonoImage(const DiMonoImage &);
    DiMonoImage &operator=(const DiMonoImage &);
};


#endif
