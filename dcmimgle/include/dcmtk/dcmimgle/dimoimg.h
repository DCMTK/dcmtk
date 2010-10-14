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
 *  Purpose: DicomMonochromeImage (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.57 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DiMonoImage
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
     ** @return true if sucessful (1 = window has changed,
     *                             2 = new window is the same as previous one),
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
     ** @return true if sucessful (1 = window has changed,
     *                             2 = new window is the same as previous one),
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
     ** @param  thresh  threshhold value specifying percentage of histogram border which shall be ignored
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
     ** @return true if sucessful (1 = window has changed,
     *                             2 = new window is the same as previous one),
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
     ** @return number of bytes allocated by the bitmap, or 0 if an error occured
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
     ** @return number of bytes allocated by the bitmap, or 0 if an error occured
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
    /// reflected ambient light (in candela per squaremeter)
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


/*
 *
 * CVS/RCS Log:
 * $Log: dimoimg.h,v $
 * Revision 1.57  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.56  2010-10-05 15:24:02  joergr
 * Added preliminary support for VOI LUT function. Please note, however, that
 * the sigmoid transformation is not yet implemented.
 *
 * Revision 1.55  2010-07-21 13:08:56  joergr
 * Fixed memory leak when using processNextFrames(): DiOverlay object was
 * created multiple times.
 *
 * Revision 1.54  2010-03-01 09:08:47  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.53  2009-11-25 16:06:14  joergr
 * Adapted code for new approach to access individual frames of a DICOM image.
 *
 * Revision 1.52  2008-11-18 11:01:28  joergr
 * Fixed issue with incorrectly encoded overlay planes (wrong values for
 * OverlayBitsAllocated and OverlayBitPosition).
 *
 * Revision 1.51  2008-05-20 10:03:53  joergr
 * Added new bilinear and bicubic scaling algorithms for image magnification.
 *
 * Revision 1.50  2008-05-13 09:54:40  joergr
 * Added new parameter to writeImageToDataset() in order to affect the planar
 * configuration of the output image/dataset. Changed behaviour: By default,
 * the output now uses the same planar configuration as the "original" image
 * (previously: always color-by-plane).
 *
 * Revision 1.49  2007/03/16 11:56:06  joergr
 * Introduced new flag that allows to select how to handle the BitsPerTableEntry
 * value in the LUT descriptor (use, ignore or check).
 *
 * Revision 1.48  2006/08/15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.47  2006/07/10 10:52:27  joergr
 * Added support for 32-bit BMP images.
 *
 * Revision 1.46  2005/12/08 16:47:50  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.45  2005/03/09 17:33:16  joergr
 * Added mode to writeImageToDataset() which allows the value of BitsStored to
 * be determined either from 'used' or from 'possible' pixel values.
 *
 * Revision 1.44  2004/07/20 18:12:16  joergr
 * Added API method to "officially" access the internal intermediate pixel data
 * representation (e.g. to get Hounsfield Units for CT images).
 *
 * Revision 1.43  2004/02/06 11:07:50  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.42  2003/12/23 10:51:52  joergr
 * Updated documentation to get rid of doxygen warnings.
 *
 * Revision 1.41  2003/12/17 16:17:29  joergr
 * Added new compatibility flag that allows to ignore the third value of LUT
 * descriptors and to determine the bits per table entry automatically.
 *
 * Revision 1.40  2003/12/08 18:30:16  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated CVS header.
 *
 * Revision 1.39  2003/06/12 15:08:34  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.38  2003/05/20 09:20:21  joergr
 * Added method returning the number of bytes required to store a single
 * rendered frame: getOutputDataSize().
 *
 * Revision 1.37  2002/12/09 13:32:52  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.36  2002/08/02 15:03:53  joergr
 * Added function to write the current image (not only a selected frame) to a
 * DICOM dataset.
 *
 * Revision 1.35  2002/06/26 17:20:04  joergr
 * Removed superfluous "inline" method specifyer (MSVC6 linker reported an
 * error).
 *
 * Revision 1.34  2002/06/26 16:04:44  joergr
 * Added support for polarity flag to color images.
 * Added new methods to get the explanation string of stored VOI windows and
 * LUTs (not only of the currently selected VOI transformation).
 *
 * Revision 1.33  2002/01/29 17:05:50  joergr
 * Added optional flag to the "Windows DIB" methods allowing to switch off the
 * scanline padding.
 *
 * Revision 1.32  2001/11/19 12:55:54  joergr
 * Added parameter 'frame' to setRoiWindow().
 *
 * Revision 1.31  2001/11/09 16:27:34  joergr
 * Added support for Window BMP file format.
 * Enhanced and renamed createTrueColorDIB() method.
 *
 * Revision 1.30  2001/09/28 13:06:10  joergr
 * Added routines to get the currently active Polarity and PresentationLUTShape.
 * Added method setRoiWindow() which automatically calculates a min-max VOI
 * window for a specified rectangular region of the image.
 * Added method to extract embedded overlay planes from pixel data and store
 * them in group (6xxx,3000) format.
 *
 * Revision 1.29  2001/06/01 15:49:44  meichel
 * Updated copyright header
 *
 * Revision 1.28  2001/05/14 09:49:17  joergr
 * Added support for "1 bit output" of overlay planes; useful to extract
 * overlay planes from the pixel data and store them separately in the dataset.
 *
 * Revision 1.27  2000/07/07 13:42:30  joergr
 * Added support for LIN OD presentation LUT shape.
 *
 * Revision 1.26  2000/06/07 14:30:27  joergr
 * Added method to set the image polarity (normal, reverse).
 *
 * Revision 1.25  2000/03/08 16:24:18  meichel
 * Updated copyright header.
 *
 * Revision 1.24  1999/12/09 17:26:24  joergr
 * Split source file dimoimg.cc into 4 parts to avoid compiler problems
 * with gcc and additional optimization options.
 *
 * Revision 1.23  1999/10/20 10:33:15  joergr
 * Enhanced method getOverlayData to support 12 bit data for print.
 *
 * Revision 1.22  1999/10/06 13:38:46  joergr
 * Corrected creation of PrintBitmap pixel data: VOI windows should be applied
 * before clipping to avoid that the region outside the image (border) is also
 * windowed (this requires a new method in dcmimgle to create a DicomImage
 * with the grayscale transformations already applied).
 *
 * Revision 1.21  1999/09/17 12:24:47  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.20  1999/09/10 08:45:18  joergr
 * Added support for CIELAB display function.
 *
 * Revision 1.19  1999/08/25 16:41:52  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.18  1999/07/23 14:03:05  joergr
 * Added dummy method (no implementation yet) to create inverse LUTs.
 * Added method to create 12 bit packed bitmap data (used for grayscale print
 * storage).
 * Added method to return pointer to currently used display function.
 *
 * Revision 1.17  1999/04/28 17:03:47  joergr
 * Added type casts to NULL pointers returned as 'const char *' to avoid
 * compiler warnings reported by gcc 2.7.2.1 (Linux).
 *
 * Revision 1.16  1999/04/28 14:49:54  joergr
 * Added experimental support to create grayscale images with more than 256
 * shades of gray to be displayed on a consumer monitor (use pastel colors).
 *
 * Revision 1.15  1999/03/24 17:20:09  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.14  1999/03/22 08:51:39  joergr
 * Added parameter to specify (transparent) background color for method
 * getOverlayData().
 *
 * Revision 1.13  1999/02/11 16:34:36  joergr
 * Added routine to check whether particular grayscale values are unused in
 * the output data.
 *
 * Revision 1.12  1999/02/08 12:39:05  joergr
 * Changed implementation of removeAllOverlays().
 *
 * Revision 1.11  1999/02/05 16:43:13  joergr
 * Added optional parameter to method convertPValueToDDL to specify width
 * of output data (number of bits).
 *
 * Revision 1.10  1999/02/03 17:27:58  joergr
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 * Revision 1.9  1999/01/20 15:03:40  joergr
 * Added new output method to fill external memory buffer with rendered pixel
 * data.
 *
 * Revision 1.8  1999/01/11 09:35:10  joergr
 * Added parameter to method 'getMinMaxValues()' to return absolute minimum
 * and maximum values ('possible') in addition to actually 'used' pixel
 * values.
 *
 * Revision 1.7  1998/12/23 13:21:28  joergr
 * Changed parameter type (long to int) to avoid warning reported by MSVC5.
 *
 * Revision 1.6  1998/12/23 12:38:16  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.4  1998/12/22 14:26:19  joergr
 * Added new parameters to method addOverlay().
 *
 * Revision 1.3  1998/12/16 16:34:06  joergr
 * Added explanation string to LUT class (retrieved from dataset).
 * Added explanation string for VOI transformations.
 * Renamed 'setNoVoiLutTransformation' method ('Voi' instead of 'VOI').
 * Removed several methods used for monochrome images only in base class
 * 'DiImage'. Introduced mechanism to use the methods directly.
 *
 * Revision 1.2  1998/12/14 17:20:03  joergr
 * Added methods to add and remove additional overlay planes (still untested).
 *
 * Revision 1.1  1998/11/27 15:20:57  joergr
 * Added copyright message.
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 * Added method to directly create java AWT bitmaps.
 * Added constructors to use external modality transformations.
 * Added methods to support presentation LUTs and shapes.
 *
 * Revision 1.6  1998/07/01 08:39:22  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.5  1998/05/11 14:53:20  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
