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
 *  Purpose: Provides main interface to the "dicom image toolkit"
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-04-28 14:45:54 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dcmimage.h,v $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DCMIMAGE_H
#define __DCMIMAGE_H

#include "osconfig.h"

#define OFFIS_DCMIMAGE_VERSION     "1.0.2"
#define OFFIS_DCMIMAGE_RELEASEDATE "1998/02/17"

#include "dimoimg.h"
#include "didispfn.h"
#include "diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmXfer;
class DcmObject;
class DcmFileStream;
class DcmOverlayData;
class DcmLongString;
class DcmUnsignedShort;

class DiDocument;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Interface class for dcmimgle/dcmimage module
 */
class DicomImage
{

 public:

 // --- constructors and destructor
 
    /** constructor, open a dicom file.
     *  opens specified file and reads image related data, creates internal representation of image data.
     *  use getStatus() to obtain detailed information about any errors.
     *
     ** @param  filename  the dicom file
     *  @param  flags     configuration flags (see diutils.h, CIF_MayDetachPixelData is automatically set)
     *  @param  fstart    first frame to be processed (not fully implemented!)
     *  @param  fcount    number of frames (not fully implemented!)
     */
    DicomImage(const char *filename,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

    /** constructor, use a given DcmFileStream
     *
     ** @param  stream  open dicom file stream
     *  @param  flags   configuration flags (see diutils.h, CIF_MayDetachPixelData is automatically set)
     *  @param  fstart  first frame to be processed (not fully implemented!)
     *  @param  fcount  number of frames (not fully implemented!)
     */
    DicomImage(DcmFileStream &stream,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

#ifndef STARVIEW
    /** constructor, use a given DcmObject
     *
     ** @param  object  pointer to dicom data structures (do not delete while referenced, not deleted within dcmimage)
     *  @param  xfer    transfer syntax
     *  @param  flags   configuration flags (see diutils.h)
     *  @param  fstart  first frame to be processed (not fully implemented!)
     *  @param  fcount  number of frames (not fully implemented!)
     */
    DicomImage(DcmObject *object,
               const E_TransferSyntax xfer,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

    /** constructor, use a given DcmObject with specified rescale/slope
     *
     ** @param  object     pointer to dicom data structures (do not delete while referenced, not deleted within dcmimage)
     *  @param  xfer       transfer syntax
     *  @param  slope      rescale slope (modality transformation)
     *  @param  intercept  rescale intercept (modality transformation)
     *  @param  flags      configuration flags (see diutils.h)
     *  @param  fstart     first frame to be processed (not fully implemented!)
     *  @param  fcount     number of frames (not fully implemented!)
     */
    DicomImage(DcmObject *object,
               const E_TransferSyntax xfer,
               const double slope,
               const double intercept,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

    /** constructor, use a given DcmObject with specified modality LUT
     *
     ** @param  object      pointer to dicom data structures (do not delete while referenced, not deleted within dcmimage)
     *  @param  xfer        transfer syntax
     *  @param  data        dataset element containing modality LUT data
     *  @param  descriptor  dataset element containing modality LUT descriptor
     *  @param  flags       configuration flags (see diutils.h)
     *  @param  fstart      first frame to be processed (not fully implemented!)
     *  @param  fcount      number of frames (not fully implemented!)
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
    

 // --- information: return requested value if successful

    /** convert status code to status string
     *
     ** @param  status  code of current internal status
     *
     ** @return pointer to status string
     */
    static const char *getString(const EI_Status status); 
            
    /** convert photometric interpretation code to interpretation string
     *
     ** @param  interpret  code of image's photometric interpretation
     *
     ** @return pointer to photometric interpretation string
     */
    static const char *getString(const EP_Interpretation interpret);
            
    /** get status information
     *
     ** @return status code
     */
    inline EI_Status getStatus() const 
    {
        return (Image != NULL) ? Image->getStatus() : ImageStatus;
    }
            
    /** get number of frames
     *
     ** @return number of frames
     */
    inline unsigned long getFrameCount() const
    {
        return (Image != NULL) ? Image->getNumberOfFrames() : 0;
    }

    /** get image width in pixels
     *
     ** @return number of pixels in one row
     */
    inline unsigned long getWidth() const 
    {
        return (Image != NULL) ? Image->getColumns() : 0;
    }

    /** get image height in pixels
     *
     ** @return number of pixels in one column
     */
    inline unsigned long getHeight() const 
    {
        return (Image != NULL) ? Image->getRows() : 0;
    }

    /** get image depth
     *
     ** @return number of bits per sample
     */
    inline int getDepth() const
    {
        return (Image != NULL) ? Image->getBits() : 0;
    }
        
    /** get minimum and maximum pixel values
     *  the resulting pixel values are stored in 'double' variables to avoid problems with different number ranges,
     *  limited to monochrome images
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
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getMinMaxValues(min, max, mode);
        return 0;
    }

    /** get width height ratio (pixel aspect ratio: x/y)
     *
     ** @return pixel aspect ratio (floating point value)
     */
    inline double getWidthHeightRatio() const
    {
        return (Image != NULL) ? Image->getColumnRowRatio() : 0;
    }

    /** get height width ratio (pixel aspect ratio: y/x)
     *
     ** @return pixel aspect ratio (floating point value)
     */
    inline double getHeightWidthRatio() const
    {
        return (Image != NULL) ? Image->getRowColumnRatio() : 0;
    }
    
    /** check whether given output value is unused 
     *
     ** @param  value  output value to be checked
     *
     ** @return status, true if unused (1 = within output range, 2 = out of range), false otherwise
     */
    inline int isOutputValueUnused(const unsigned long value)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->isValueUnused(value);
        return 0;
    }
    
 // --- output: return pointer to output data if successful
    
    /** render pixel data and return pointer to internal memory buffer.
     *  apply VOI/PLUT transformation and (visible) overlay planes
     *
     ** @param  bits    number of bits per sample (image depth)
     *                  (MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *  @param  frame   number of frame to be rendered
     *  @param  planar  0 = color-by-pixel (R1G1B1...R2G2B2...R3G2B2...)
     *                  1 = color-by-plane (R1R2R3...G1G2G3...B1B2B3...)
     *                  (only applicable for multi-planar/color images, otherwise ignored)
     *
     ** @return pointer to internal memory buffer containing rendered pixel data (if successful, NULL otherwise)
     */
    inline const void *getOutputData(const int bits = 0,
                                     const unsigned long frame = 0,
                                     const int planar = 0) 
    {
        return (Image != NULL) ? Image->getOutputData(frame, Image->getBits(bits), planar) : NULL;
    }

    /** render pixel data and output to given memory buffer.
     *  apply VOI/PLUT transformation and (visible) overlay planes
     *
     ** @param  buffer  pointer to memory buffer (must already be allocated)
     *  @param  size    size of memory buffer (will be checked whether it is sufficient)
     *  @param  bits    number of bits per sample (image depth)
     *                  (MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *  @param  frame   number of frame to be rendered
     *  @param  planar  0 = color-by-pixel (R1G1B1...R2G2B2...R3G2B2...)
     *                  1 = color-by-plane (R1R2R3...G1G2G3...B1B2B3...)
     *                  (only applicable for multi-planar/color images, otherwise ignored)
     *
     ** @return status code (true if successful)
     */
    inline int getOutputData(void *buffer,
                             const unsigned long size,
                             const int bits = 0,
                             const unsigned long frame = 0,
                             const int planar = 0) 
    {
        return (Image != NULL) ? Image->getOutputData(buffer, size, frame, Image->getBits(bits), planar) : 0;
    }

    /** render pixel data and return pointer to given plane.
     *  apply VOI/PLUT transformation and (visible) overlay planes
     *
     ** @param  plane  number of plane to be rendered
     *
     ** @return pointer to internal memory buffer containing rendered pixel data (if successful, NULL otherwise)
     */
    inline const void *getOutputPlane(const int plane) const 
    {
        return (Image != NULL) ? Image->getOutputPlane(plane) : NULL;
    }

    /** delete internal memory buffer used for rendered images.
     */
    inline void deleteOutputData() const 
    {
        if (Image != NULL) Image->deleteOutputData();
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

 // --- display function for output device characteristic (calibration)
 
    /** set display function
     *
     ** @param  display  object describing the output device characteristic (only referenced!)
     *
     ** @return true if successful, false otherwise
     */
    inline int setDisplayFunction(DiDisplayFunction *display)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setDisplayFunction(display);
        return 0;
    }

    /** set no display function.
     *  disables Barten transformation.
     *
     ** @return true if successful, false otherwise
     */
    inline int setNoDisplayFunction()
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setNoDisplayFunction();
        return 0;
    }

    /** delete specified Barten LUT(s)
     *
     ** @param  bits  parameter of LUT to be deleted (0 = all)
     *
     ** @return true if successful, false otherwise
     */
    inline int deleteBartenLUT(const int bits = 0)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->deleteBartenLUT(bits);
        return 0;
    }

    /** convert P-value to DDL.
     *  conversion uses Barten LUT if present, linear scaling otherwise.
     *
     ** @param  pvalue  P-value to be converted (0..65535)
     *  @param  ddl     reference to resulting DDL
     *  @param  bits    number of bits for output
     *
     ** @return true if successful (1 = Barten transformation,
     *                              2 = linear scaling),
     *          false otherwise
     */
    inline int convertPValueToDDL(const Uint16 pvalue,
                                  Uint16 &ddl,
                                  const int bits = 8)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->convertPValueToDDL(pvalue, ddl, bits);
        return 0;
    }

 // --- windowing (voi): return true if successful (see also 'dimoimg.cc')
    
    /** unset all VOI transformations (windows and LUTs).
     *  only applicable for monochrome images
     *
     ** @return true if successful (1 = previous window/LUT has been valid,
     *                              2 = otherwise),
     *          false otherwise (image is invalid or not monochrome)
     */
    inline int setNoVoiTransformation() 
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setNoVoiTransformation();
        return 0;
    }

    /** set automatically calculated minimum/maximum window.
     *  possibly active VOI LUT is implicitly disabled.
     *
     ** @param  idx  ignore global min/max values if false (0)
     *
     ** @return true if sucessful (1 = window has changed,
     *                             2 = new window is the same as previous one),
     *          false otherwise
     */
    inline int setMinMaxWindow(const int idx = 1) 
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setMinMaxWindow(idx);
        return 0;
    }

    /** set automatically calculated histogram window.
     *  possibly active VOI LUT is implicitly disabled.
     *
     ** @param  thresh  threshhold value specifying percentage of histogram border which shall be ignored (defaut: 5%).
     *
     ** @return true if successful, false otherwise
     */
    inline int setHistogramWindow(const double thresh = 0.05)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setHistogramWindow(thresh);
        return 0;
    }

    /** set specified window (given by index to window width/center sequence stored in image file).
     *  possibly active VOI LUT is implicitly disabled.
     *
     ** @param  window  index to window width/center sequence
     *
     ** @return true if successful, false otherwise (none monochrome or invalid index)
     */
    inline int setWindow(const unsigned long window) 
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setWindow(window);
        return 0;
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
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setWindow(center, width);
        return 0;
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
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getWindow(center, width);
        return 0;
    }

    /** get number of VOI windows (stored in image file)
     *
     ** @return number of VOI windows
     */
    inline unsigned long getWindowCount() const 
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getWindowCount();
        return 0;
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
    inline int setVoiLut(const DcmUnsignedShort &data,
                         const DcmUnsignedShort &descriptor,
                         const DcmLongString *explanation = NULL)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setVoiLut(data, descriptor, explanation);
        return 0;
    }

    /** set VOI LUT (given by index to VOI LUT sequence stored in image file).
     *  possibly active window/center is implicitly disabled.
     *
     ** @param  table  index to VOI LUT sequence
     *
     ** @return true if successful, false otherwise (none monochrome or invalid index)
     */
    inline int setVoiLut(const unsigned long table)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setVoiLut(table);
        return 0;
    }

    /** get number of VOI LUTs (stored in image file)
     *
     ** @return number of VOI LUTs
     */
    inline unsigned long getVoiLutCount() const
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getVoiLutCount();
        return 0;
    }

    /** get description of active VOI transformation
     *
     ** @return pointer to description text (NULL if absent)
     */
    inline const char *getVoiTransformationExplanation()
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getVoiTransformationExplanation();
        return NULL;
    }

    /** get description of performed modality LUT transformation
     *
     ** @return pointer to description text (NULL if absent)
     */
    inline const char *getModalityLutExplanation()
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getModalityLutExplanation();
        return NULL;
    }

 // --- presentation LUT:

    /** set shape for presentation transformation.
     *  possibly active presentation LUT is implicitly disabled.
     *
     ** @param  shape  presentation LUT shape (identity or inverse)
     *
     ** @return true if successful (1 = shape has changed,
     *                              2 = shape has not changed)
     *          false otherwise
     */
    inline int setPresentationLutShape(const ES_PresentationLut shape = ESP_Identity)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setPresentationLutShape(shape);
        return 0;
    }

    /** set LUT for presentation transformation.
     *  possibly active presentation LUT is implicitly disabled.
     *
     ** @param  data         contains LUT data
     *  @param  descriptor   describes LUT structure
     *  @param  explanation  free form description of presentation LUT (optional)
     *
     ** @return true if successful, false otherwise
     */
    inline int setPresentationLut(const DcmUnsignedShort &data,
                                  const DcmUnsignedShort &descriptor,
                                  const DcmLongString *explanation = NULL)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setPresentationLut(data, descriptor, explanation);
        return 0;
    }

    /** get description of active presentation LUT
     *
     ** @return pointer to description text (NULL if absent)
     */
    inline const char *getPresentationLutExplanation() const
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getPresentationLutExplanation();
        return NULL;
    }


 // --- overlays: return true (!0) if successful (see also 'diovlay.cc')

    /** add specified plane to group of additional overlay planes.
     *  replaces old overlay plane if group number already exists.
     *
     ** @param  group        group number (0x60nn) of overlay plane
     *  @param  width        width of overlay plane (in pixels)
     *  @param  height       height of overlay plane (in pixels)
     *  @param  left         x coordinate of plane orgin (referring to image origin)
     *  @param  top          y coordinate of plane origin
     *  @param  data         overlay plane data (dcmdata element)
     *  @param  label        overlay plane label
     *  @param  description  overlay plane description
     *  @param  mode         display mode (see 'diutils.h')
     *
     ** @return false (0) if an error occurred, true otherwise (1 = added new plane,
     *                                                          2 = replaced existing plane)
     */
    inline int addOverlay(const unsigned int group,
                          const signed int left,
                          const signed int top,
                          const unsigned long width,
                          const unsigned long height,
                          const DcmOverlayData &data,
                          const DcmLongString &label,
                          const DcmLongString &description,
                          const EM_Overlay mode = EMO_Default)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->addOverlay(group, left, top, width, height, data, label, description, mode);
        return 0;
    }

    /** remove specified (additional) overlay plane
     *
     ** @param  group  group number (0x60nn) of overlay plane
     *
     ** @return false (0) if an error occurred (e.g. plane doesn't exist), true otherwise
     */
    inline int removeOverlay(const unsigned int group)
    {
        if ((Image != NULL) && (Image->getOverlayPtr(1) != NULL)) 
            return Image->getOverlayPtr(1)->removePlane(group);
        return 0;
    }

    /** remove all additional overlay planes
     *
     ** @return false (0) if an error occurred, true otherwise (1 = all planes deleted,
     *                                                          2 = no planes to be deleted)
     */
    inline int removeAllOverlays()
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->removeAllOverlays();
        return 0;
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
        if ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) 
            return Image->getOverlayPtr(idx)->isPlaneVisible(plane);
        return 0;
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
        if ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL)) 
            return Image->getOverlayPtr(idx)->showPlane(plane);
        return 0;
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
        if ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL))
            return Image->getOverlayPtr(idx)->showPlane(plane, fore, thresh, mode);
        return 0;
    }

    /** activate specified overlay plane and change 'pvalue' (only for bitmap shutters)
     *
     ** @param  plane   number (0..15) or group number (0x60nn) of overlay plane
     *  @param  pvalue  P-value
     *
     ** @return false (0) if an error occurred, true otherwise
     */
    inline int showOverlay(const unsigned int plane,
                           const Uint16 pvalue)
    {
        if ((Image != NULL) && (Image->getOverlayPtr(1) != NULL))
            return Image->getOverlayPtr(1)->showPlane(plane, pvalue);
        return 0;
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
        if ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL))
            return Image->getOverlayPtr(idx)->showAllPlanes();
        return 0;
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
        if ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL))
            return Image->getOverlayPtr(idx)->showAllPlanes(fore, thresh, mode);
        return 0;
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
        if ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL))
            return Image->getOverlayPtr(idx)->hidePlane(plane);
        return 0;
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
        if ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL))
            return Image->getOverlayPtr(idx)->hideAllPlanes();
        return 0;
    }
        
    /** move origin of specified overlay plane to given position
     *
     ** @param  plane  number (0..15) or group number (0x60nn) of overlay plane
     *  @param  left   x coordinate of new plane origin
     *  @param  top    y coordinate of new plane origin
     *  @param  idx    index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return false (0) if an error occurred, true otherwise (1 = plane has been successfully moved,
     *                                                          2 = old and new position are equal, nothing to do)
     */
    inline int placeOverlay(const unsigned int plane,
                            const signed int left,
                            const signed int top,
                            const unsigned int idx = 0)
    {
        if ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL))
            return Image->getOverlayPtr(idx)->placePlane(plane, left, top);
        return 0;
    }

    /** get number of overlay planes
     *
     ** @param  idx  index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return number of overlay planes stored in the image
     */
    inline unsigned int getOverlayCount(const unsigned int idx = 0) const
    {
        if ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL))
            return Image->getOverlayPtr(idx)->getCount();
        return 0;
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
        if ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL))
            return Image->getOverlayPtr(idx)->getPlaneGroupNumber(plane);
        return 0;
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
        if ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL))
            return Image->getOverlayPtr(idx)->getPlaneLabel(plane);
        return NULL;
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
        if ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL))
            return Image->getOverlayPtr(idx)->getPlaneDescription(plane);
        return NULL;
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
        if ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL))
            return Image->getOverlayPtr(idx)->getPlaneMode(plane);
        return EMO_Default;
    }

    /** create bitmap for specified overlay plane.
     *  (8 bits per pixel with two values: fore and back)
     *
     ** @param  plane   number (0..15) or group number (0x60nn) of overlay plane
     *  @param  width   returns width of overlay plane (in pixels)
     *  @param  height  returns height of overlay plane (in pixels)
     *  @param  left    returns x coordinate of plane's origin 
     *  @param  top     returns y coordinate of plane's origin
     *  @param  mode    return display mode (see 'diutils.h')
     *  @param  frame   index of frame used for output, default: 0
     *  @param  fore    foreground color to be set in bitmap, default: 255
     *  @param  back    background color to be set in bitmap (transparent), default: 0
     *  @param  idx     index of overlay group (0 = dataset, 1 = additional, 2 = '1' plane hides '0' plane), default: 2
     *
     ** @return pointer to overlay plane data (internal memory buffer)
     */
    inline const Uint8 *getOverlayData(const unsigned int plane,
                                       unsigned int &left,
                                       unsigned int &top,
                                       unsigned int &width,
                                       unsigned int &height,
                                       EM_Overlay &mode,
                                       const unsigned long frame = 0,
                                       const Uint8 fore = 0xff,
                                       const Uint8 back = 0x0,
                                       const unsigned int idx = 2) const
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getOverlayData(frame, plane, left, top, width, height, mode, idx, fore, back);
        return NULL;
    }
    
    /** delete buffer for overlay plane data.
     *  Save memory if data is no longer needed.
     */
    inline void deleteOverlayData() const 
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            Image->getMonoImagePtr()->deleteOverlayData();
    }
    

 // --- create...Image: return pointer to new 'DicomImage' object, memory isn't handled internally !
    
    /** create copy of current image object.
     *  memory is not handled internally - must be deleted from calling program.
     *
     ** @param  fstart  first frame to be processed (not fully implemented!)
     *  @param  fcount  number of frames (not fully implemented!)
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
     *  @param  interpolate  specifies whether scaling algorithm should use interpolation (if necessary)
     *                       default: no interpolation
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
     *  @param  interpolate  specifies whether scaling algorithm should use interpolation (if necessary)
     *                       default: no interpolation
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
     *  NB: clipping and interpolated scaling at the same moment is not yet implemented!
     *
     ** @param  left          x coordinate of top left corner of area to be scaled (referring to image orgin)
     *  @param  top           y coordinate of top left corner of area to be scaled
     *  @param  clip_width    width of area to be scaled
     *  @param  clip_height   height of area to be scaled
     *  @param  scale_width   width of scaled image (in pixels)
     *  @param  scale_height  height of scaled image (in pixels)
     *  @param  interpolate   specifies whether scaling algorithm should use interpolation (if necessary)
     *                        default: no interpolation
     *  @param  aspect        specifies whether pixel aspect ratio should be taken into consideration
     *                        (if true, width OR height should be 0, i.e. this component will be calculated
     *                         automatically)
     *
     ** @return pointer to new DicomImage object (NULL if an error occurred)
     */
    DicomImage *createScaledImage(const unsigned long left,
                                  const unsigned long top,
                                  unsigned long clip_width,
                                  unsigned long clip_height,
                                  unsigned long scale_width = 0,
                                  unsigned long scale_height = 0,
                                  const int interpolate = 0,
                                  int aspect = 0) const;

    /** create scaled copy of specified (clipping) area of the current image object.
     *  memory is not handled internally - must be deleted from calling program.
     *  NB: clipping and interpolated scaling at the same moment is not yet implemented!
     *
     ** @param  left         x coordinate of top left corner of area to be scaled (referring to image orgin)
     *  @param  top          y coordinate of top left corner of area to be scaled
     *  @param  width        width of area to be scaled
     *  @param  height       height of area to be scaled
     *  @param  xfactor      width of new image is multiplied with this factor (> 0)
     *  @param  yfactor      height of new image is multiplied with this factor (> 0)
     *  @param  interpolate  specifies whether scaling algorithm should use interpolation (if necessary)
     *                       default: no interpolation
     *  @param  aspect       specifies whether pixel aspect ratio should be taken into consideration
     *                       (if true, width OR height should be 0, i.e. this component will be calculated
     *                        automatically)
     *
     ** @return pointer to new DicomImage object (NULL if an error occurred)
     */
    DicomImage *createScaledImage(const unsigned long left,
                                  const unsigned long top,
                                  unsigned long width,
                                  unsigned long height,
                                  const double xfactor,
                                  const double yfactor = 0,
                                  const int interpolate = 0,
                                  const int aspect = 0) const;

    /** create copy of specified area of the current image object (clipping).
     *  memory is not handled internally - must be deleted from calling program.
     *
     ** @param  left    x coordinate of top left corner of area to be copied (referring to image orgin)
     *  @param  top     y coordinate of top left corner of area to be copied
     *  @param  width   width of area to be copied
     *  @param  height  height of area to be copied
     *
     ** @return pointer to new DicomImage object (NULL if an error occurred)
     */
    DicomImage *createClippedImage(const unsigned long left,
                                   const unsigned long top,
                                   unsigned long width = 0,
                                   unsigned long height = 0) const;

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

    /** create true color (24 bit) bitmap for MS Windows.
     *  memory is not handled internally - must be deleted from calling program.
     *
     ** @param  frame  index of frame to be converted (default: first frame)
     *
     ** @return pointer to memory buffer containing the bitmap data
     */
    void *createTrueColorDIB(const unsigned long frame = 0)
    {
        return (Image != NULL) ? Image->createDIB(frame) : NULL;
    }
    
    /** create true color (32 bit) bitmap for Java (AWT default format).
     *  Memory is not handled internally - must be deleted from calling program.
     *
     ** @param  frame  index of frame to be converted (default: first frame)
     *  @param  bits   number of bits per pixel used for the output bitmap (default: 32)
     *
     ** @return pointer to memory buffer containing the bitmap data
     */
    void *createJavaAWTBitmap(const unsigned long frame = 0,
                              const int bits = 32)
    {
        return (Image != NULL) ? Image->createAWTBitmap(frame, bits) : NULL;
    }

 // --- output ppm file: return true ('1') if successful

    /** write pixel data to PPM file (specified by filename).
     *  pixel data is written in ASCII format.
     *
     ** @param  filename  name of output file
     *  @param  bits      number of bits used for output of pixel data
     *                    (default: full resolution, max: 32;
     *                     MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *  @param  frame     index of frame used for output (default: first frame)
     *
     ** @return true if successful, false otherwise
     */
    int writePPM(const char *filename,
                 const int bits = 0,
                 const unsigned long frame = 0);
                 
    /** write pixel data to PPM file (specified by open C++ stream).
     *  pixel data is written in ASCII format.
     *
     ** @param  stream  open C++ output stream
     *  @param  bits    number of bits used for output of pixel data
     *                  (default: full resolution, max: 32;
     *                   MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *  @param  frame   index of frame used for output (default: first frame)
     *
     ** @return true if successful, false otherwise
     */
    int writePPM(ostream &stream,
                 const int bits = 0,
                 const unsigned long frame = 0);
                 
    /** write pixel data to PPM file (specified by open C stream).
     *  pixel data is written in ASCII format.
     *
     ** @param  stream  open C output stream
     *  @param  bits    number of bits used for output of pixel data
     *                  (default: full resolution, max: 32;
     *                   MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *  @param  frame   index of frame used for output (default: first frame)
     *
     ** @return true if successful, false otherwise
     */
    int writePPM(FILE *stream,
                 const int bits = 0,
                 const unsigned long frame = 0);

    /** write pixel data to raw PPM file (specified by filename).
     *  pixel data is written in binary format.
     *
     ** @param  filename  name of output file
     *  @param  bits      number of bits used for output of pixel data
     *                    (default: full resolution, max: 8;
     *                     MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *  @param  frame     index of frame used for output (default: first frame)
     *
     ** @return true if successful, false otherwise
     */
    int writeRawPPM(const char *filename,
                    const int bits = 0,
                    const unsigned long frame= 0);

    /** write pixel data to raw PPM file (specified by open C stream).
     *  pixel data is written in binary format.
     *
     ** @param  stream  open C output stream
     *  @param  bits    number of bits used for output of pixel data
     *                  (default: full resolution, max: 8;
     *                   MI_PastelColor = -1 for true color pastel mode, EXPERIMENTAL)
     *  @param  frame   index of frame used for output (default: first frame)
     *
     ** @return true if successful, false otherwise
     */
    int writeRawPPM(FILE *stream,
                    const int bits = 0,
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
     *  360 is set to 0, all other value are rejected
     *
     ** @param  degree  value to be normalized, valid values are: 0, 90, 180, 270
     *
     ** @return true if successfull, false otherwise (invalid value)
     */
    int normalizeDegreeValue(signed int &degree) const;
    

 private:

    /// current state of converting progress (error level)
    EI_Status ImageStatus;
    /// dicom color model (enumeration)
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
 * Revision 1.16  1999-04-28 14:45:54  joergr
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
