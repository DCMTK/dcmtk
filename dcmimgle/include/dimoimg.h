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
 *  Purpose: DicomMonochromeImage (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-10-20 10:33:15 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimoimg.h,v $
 *  CVS/RCS Revision: $Revision: 1.23 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIMOIMG_H
#define __DIMOIMG_H

#include "osconfig.h"
#include "dctypes.h"

#include "diimage.h"
#include "dimopx.h"
#include "dimoopx.h"
#include "diovlay.h"
#include "diluptab.h"
#include "didispfn.h"


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

    /** set VOI LUT (given by dcmdata elements).
     *  possibly active window/center is implicitly disabled.
     *
     ** @param  data         contains LUT data
     *  @param  descriptor   describes LUT structure
     *  @param  explanation  free form description of VOI LUT (optional)
     *
     ** @return true if successful, false otherwise
     */
    int setVoiLut(const DcmUnsignedShort &data,
                  const DcmUnsignedShort &descriptor,
                  const DcmLongString *explanation);

    /** set VOI LUT (given by index to VOI LUT sequence stored in image file).
     *  possibly active window/center is implicitly disabled.
     *
     ** @param  pos  index to VOI LUT sequence
     *
     ** @return true if successful, false otherwise
     */
    int setVoiLut(const unsigned long pos);

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

    /** get description of performed modality LUT transformation
     *
     ** @return pointer to description text (NULL if absent)
     */
    inline const char *getModalityLutExplanation() const
    {
        return (InterData != NULL) ? InterData->getModalityLutExplanation() : (const char *)NULL;
    }

    /** set shape for presentation transformation.
     *  possibly active presentation LUT is implicitly disabled.
     *
     ** @param  shape  presentation LUT shape (identity or inverse)
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
     *
     ** @return true if successful, false otherwise
     */
    int setPresentationLut(const DcmUnsignedShort &data,
                           const DcmUnsignedShort &descriptor,
                           const DcmLongString *explanation = NULL);

    /** set inverse LUT for presentation transformation.
     *  this LUT transform is e.g. used for DICOM print (12->8, 8->12 bit)
     *  possibly active presentation LUT will not be considered !
     *
     ** @param  data        contains LUT data
     *  @param  descriptor  describes LUT structure
     *
     ** @return true if successful, false otherwise
     */
    int setInversePresentationLut(const DcmUnsignedShort &data,
                                  const DcmUnsignedShort &descriptor);

    /** get description of active presentation LUT
     *
     ** @return pointer to description text (NULL if absent)
     */
    inline const char *getPresentationLutExplanation() const
    {
        return (PresLutData != NULL) ? PresLutData->getExplanation() : (const char *)NULL;
    }

    /** add specified plane to group of additional overlay planes.
     *  replaces old overlay plane if group number already exists.
     *
     ** @param  group        group number (0x60nn) of overlay plane
     *  @param  left         x coordinate of plane orgin (referring to image origin)
     *  @param  top          y coordinate of plane origin
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
                   const signed int left,
                   const signed int top,
                   const unsigned int columns,
                   const unsigned int rows,
                   const DcmOverlayData &data,
                   const DcmLongString &label,
                   const DcmLongString &description,
                   const EM_Overlay mode);

    /** remove specified (additional) overlay plane
     *
     ** @param  group  group number (0x60nn) of overlay plane
     *
     ** @return false (0) if an error occurred (e.g. plane doesn't exist), true otherwise
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
        return (idx < 2) ? Overlays[idx] : (DiOverlay *)NULL;
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

    /** get pixel data with specified format.
     *  (memory is handled internally)
     *
     ** @param  frame   number of frame to be rendered
     *  @param  bits    number of bits for the output pixel data (depth)
     *  @param  planar  flag, only useful for multi-planar images (color)
     *
     ** @return untyped pointer to the pixel data if successful, NULL otherwise
     */
    virtual void *getOutputData(const unsigned long frame,
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

    /** get pixel data of specified plane .
     *  (memory is handled internally)
     *
     ** @param  dummy  (not used)
     *
     ** @return untyped pointer to the pixel data if successful, NULL otherwise
     */
    void *getOutputPlane(const int) const;

    /** delete internally handled output memory buffer
     *  Save memory if data is no longer needed.
     */
    void deleteOutputData();

    /** create bitmap for specified overlay plane.
     *  (up to 16 bits per pixel with two values: fore and back)
     *
     ** @param  frame   index of frame used for output
     *  @param  plane   number (0..15) or group number (0x60nn) of overlay plane
     *  @param  left    returns x coordinate of plane's origin
     *  @param  top     returns y coordinate of plane's origin
     *  @param  width   returns width of overlay plane (in pixels)
     *  @param  height  returns height of overlay plane (in pixels)
     *  @param  mode    return display mode (see 'diutils.h')
     *  @param  idx     index of overlay group (0 = dataset, 1 = additional, 2 = '1' plane hides '0' plane)
     *  @param  bits    number of bits (stored) in the resulting array, default: 8
     *  @param  fore    foreground color to be set in bitmap, default: 255
     *  @param  back    background color to be set in bitmap (transparent), default: 0
     *
     ** @return pointer to overlay plane data (internal memory buffer)
     */
    const void *getOverlayData(const unsigned long frame,
                                const unsigned int plane,
                                unsigned int &left,
                                unsigned int &top,
                                unsigned int &width,
                                unsigned int &height,
                                EM_Overlay &mode,
                                const unsigned int idx,
                                const int bits = 8,
                                const Uint16 fore = 0xff,
                                const Uint16 back = 0x0);

    /** delete buffer for overlay plane data.
     *  Save memory if data is no longer needed.
     */
    void deleteOverlayData();

    /** get pointer to intermediate pixel data representation
     *
     ** @return pointer to intermediate pixel data
     */
    const DiMonoPixel *getInterData() const
    {
        return InterData;
    }

    /** create true color (24 bit) bitmap for MS Windows.
     *  memory is not handled internally - must be deleted from calling program.
     *
     ** @param  frame  index of frame to be converted
     *
     ** @return pointer to memory buffer containing the bitmap data (NULL if an error occurred)
     */
    void *createDIB(const unsigned long frame);

    /** create true color (32 bit) bitmap for Java (AWT default format).
     *  Memory is not handled internally - must be deleted from calling program.
     *
     ** @param  frame  index of frame to be converted (default: first frame)
     *  @param  bits   number of bits per pixel used for the output bitmap (default: 32)
     *
     ** @return pointer to memory buffer containing the bitmap data (NULL if an error occurred)
     */
    void *createAWTBitmap(const unsigned long frame,
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

    /** write pixel data to PPM file.
     *  pixel data is written in ASCII format.
     *
     ** @param  stream  open C++ output stream
     *  @param  frame   index of frame used for output
     *  @param  bits    number of bits used for output of pixel data
     *
     ** @return true if successful, false otherwise
     */
    int writePPM(ostream &stream,
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

    /** write pixel data to raw PPM file.
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


 protected:

    /** constructor
     *
     ** @param  image   pointer to dataset (encapsulated)
     *  @param  status  current image status
     *  @param  dummy   (necessary to be different from another constructor)
     */
    DiMonoImage(const DiDocument *image,
                const EI_Status status,
                const char /*dummy*/);

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
     *  @param  left          x coordinate of top left corner of area to be scaled
     *                        (referring to image origin, negative values create a border around the image)
     *  @param  top           y coordinate of top left corner of area to be scaled
     *  @param  src_cols      width of area to be scaled
     *  @param  src_rows      height of area to be scaled
     *  @param  dest_cols     width of scaled image (in pixels)
     *  @param  dest_rows     height of scaled image (in pixels)
     *  @param  interpolate   specifies whether scaling algorithm should use interpolation (if necessary)
     *                        default: no interpolation (0), 1 = pbmplus algorithm, 2 = c't algorithm
     *  @param  aspect        specifies whether pixel aspect ratio should be taken into consideration
     *                        (if true, width OR height should be 0, i.e. this component will be calculated
     *                         automatically)
     *  @param  pvalue        P-value used for the border outside the image (0..65535)
     */
    DiMonoImage(const DiMonoImage *image,
                const signed long left,
                const signed long top,
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
                 const DiMonoOutputPixel *pixel,
                 const unsigned long frame,
                 const int stored,
                 const int alloc);

    /** initialize internal data structures and member variables
     *
     ** @param  modality  pointer to object handling the modality transform
     */
    void Init(DiMonoModality *modality);

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
     *  @param  planar    flag, only useful for multi-planar images (color)
     *  @param  negative  invert pixel data if true
     *
     ** @return untyped pointer to the pixel data if successful, NULL otherwise
     */
    void *getData(void *buffer,
                  const unsigned long size,
                  const unsigned long frame,
                  int bits,
                  const int planar,
                  const int negative);


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

    /// presentation LUT shape (identity or inverse)
    ES_PresentationLut PresLutShape;

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
 * Revision 1.23  1999-10-20 10:33:15  joergr
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
