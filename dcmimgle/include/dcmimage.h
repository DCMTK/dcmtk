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
 *  Update Date:      $Date: 1999-02-05 16:42:22 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dcmimage.h,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
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

/** Interface class for dcmimage module
 */
class DicomImage
{

 public:

 // --- constructors and destructor
 
    /** constructor, open a dicom file
     *
     ** @param  filename  the dicom file
     *  @param  flags     configuration flags (see diutils.h, CIF_MayDetachPixelData is automatically set)
     *  @param  fstart    first frame to be processed (not implemented!)
     *  @param  fcount    number of frames (not implemented!)
     */
    DicomImage(const char *filename,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

    /** constructor, use a given DcmFileStream
     *
     ** @param  stream  open dicom file stream
     *  @param  flags   configuration flags (see diutils.h, CIF_MayDetachPixelData is automatically set)
     *  @param  fstart  first frame to be processed (not implemented!)
     *  @param  fcount  number of frames (not implemented!)
     */
    DicomImage(DcmFileStream &stream,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

    /** constructor, use a given DcmObject
     *
     ** @param  object  pointer to dicom data structures (do not delete while referenced, not deleted within dcmimage)
     *  @param  xfer    transfer syntax
     *  @param  flags   configuration flags (see diutils.h)
     *  @param  fstart  first frame to be processed (not implemented!)
     *  @param  fcount  number of frames (not implemented!)
     */
    DicomImage(DcmObject *object,
               E_TransferSyntax xfer,
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
     *  @param  fstart     first frame to be processed (not implemented!)
     *  @param  fcount     number of frames (not implemented!)
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
     *  @param  fstart      first frame to be processed (not implemented!)
     *  @param  fcount      number of frames (not implemented!)
     */
    DicomImage(DcmObject *object,
               E_TransferSyntax xfer,
               const DcmUnsignedShort &data,
               const DcmUnsignedShort &descriptor,
               const DcmLongString *explanation = NULL,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

    /** destructor
     */
    virtual ~DicomImage();
    

 // --- information: return requested value if successful

    /** convert status code to status string
     *
     ** @param  status  code of current internal status
     ** @return pointer to status string
     */
    static const char *getString(const EI_Status status); 
            
    /** convert photometric interpretation code to interpretation string
     *
     ** @param  interpret  code of image's photometric interpretation
     ** @return pointer to photometric interpretation string
     */
    static const char *getString(const EP_Interpretation interpret);
            
    /** get status information
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
    
 // --- output: return pointer to output data if successful
    
    /** render pixel data and return pointer to internal memory buffer.
     *  apply VOI/PLUT transformation and (visible) overlay planes
     *
     ** @param  bits    number of bits per sample (image depth)
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

    /** set no display function
     *
     ** @return true if successful, false otherwise
     */
    inline int setNoDisplayFunction()
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setNoDisplayFunction();
        return 0;
    }

    /** delete Barten LUT
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

    /** convert P-value to DDL
     *
     ** @param  pvalue  P-value to be converted (0..65535)
     *  @param  ddl     reference to resulting DDL
     *  @param  bits    number of bits for output
     *
     ** @return true if successful, false otherwise
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
     ** @return true if successful (1 = previous window/LUT has been valid, 2 = otherwise),
     *          false otherwise (image is invalid or not monochrome)
     */
    inline int setNoVoiTransformation() 
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setNoVoiTransformation();
        return 0;
    }

    /** set automatically calculated minimum/maximum window.
     *
     ** @param  idx  ignore global min/max values if false (0)
     *
     ** @return true if sucessful (1 = window has changed, 2 = new window is the same as previous one),
     *          false otherwise
     */
    inline int setMinMaxWindow(const int idx = 1) 
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setMinMaxWindow(idx);
        return 0;
    }

    /** set automatically calculated histogram window.
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

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int setWindow(const unsigned long window) 
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setWindow(window);
        return 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int setWindow(const double center,
                         const double width) 
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setWindow(center, width);
        return 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int getWindow(double &center,
                         double &width) 
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getWindow(center, width);
        return 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline unsigned long getWindowCount() const 
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getWindowCount();
        return 0;
    }
    
    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int setVoiLut(const DcmUnsignedShort &data,
                         const DcmUnsignedShort &descriptor,
                         const DcmLongString *explanation = NULL)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setVoiLut(data, descriptor, explanation);
        return 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int setVoiLut(const unsigned long table)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setVoiLut(table);
        return 0;
    }

    /** .
     *
     ** @return
     */
    inline unsigned long getVoiLutCount() const
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getVoiLutCount();
        return 0;
    }

    /** !! UNTESTED !!
     *
     ** @return
     */
    inline const char *getVoiTransformationExplanation()
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getVoiTransformationExplanation();
        return NULL;
    }

    /** !! UNTESTED !!
     *
     ** @return
     */
    inline const char *getModalityLutExplanation()
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getModalityLutExplanation();
        return NULL;
    }

 // --- presentation LUT:

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int setPresentationLutShape(const ES_PresentationLut shape = ESP_Identity)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setPresentationLutShape(shape);
        return 0;
    }

    /** !! NOT FULLY IMPLEMENTED !!
     *
     ** @param
     *
     ** @return
     */
    inline int setPresentationLut(const DcmUnsignedShort &data,
                                  const DcmUnsignedShort &descriptor,
                                  const DcmLongString *explanation = NULL)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->setPresentationLut(data, descriptor, explanation);
        return 0;
    }

    /** !! UNTESTED !!
     *
     ** @return
     */
    inline const char *getPresentationLutExplanation() const
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getPresentationLutExplanation();
        return NULL;
    }


 // --- overlays: return true (!0) if successful (see also 'diovlay.cc')

    /** add specified plane to group of additional overlay planes (UNTESTED!)
     *
     ** @param  group        group number (0x60nn) of overlay plane
     *  @param  width        .
     *  @param  height       .
     *  @param  left         .
     *  @param  top          .
     *  @param  data         .
     *  @param  label        .
     *  @param  description  .
     *  @param  mode         .
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

    /** remove specified (additional) overlay plane (UNTESTED!)
     *
     ** @param  group  group number (0x60nn) of overlay plane
     *
     ** @return false (0) if an error occurred (e.g. plane doesn't exist), true otherwise
     */
    inline int removeOverlay(const unsigned int group)
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->removeOverlay(group);
        return 0;
    }

    /** remove all additional overlay planes (UNTESTED!)
     *
     ** @return false (0) if an error occurred, true otherwise (1 = all planes deleted,
     *                                                          2 = no planes to be deleted)
     */
    inline int removeAllOverlays()
    {
        if ((Image != NULL) && (Image->getOverlayPtr() != NULL)) 
            return Image->getOverlayPtr()->removeAllPlanes();
        return 0;
    }

    /** check whether specified overlay plane is visible/activated
     *  See show/hideOverlay() to modifiy the visibility status.
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
            return Image->getOverlayPtr()->isPlaneVisible(plane);
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
            return Image->getOverlayPtr()->showPlane(plane);
        return 0;
    }

    /** activate specified overlay plane and change some parameters
     *
     ** @param  plane   number (0..15) or group number (0x60nn) of overlay plane
     *  @param  mode    .
     *  @param  fore    ..., default: 1.0
     *  @param  thresh  ..., default: 0.5
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
            return Image->getOverlayPtr()->showPlane(plane, fore, thresh, mode);
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
            return Image->getOverlayPtr()->showPlane(plane, pvalue);
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
            return Image->getOverlayPtr()->showAllPlanes();
        return 0;
    }

    /** activate all overlay planes and set specified parameters
     *
     ** @param  mode    .
     *  @param  fore    .
     *  @param  thresh  .
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
            return Image->getOverlayPtr()->showAllPlanes(fore, thresh, mode);
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
            return Image->getOverlayPtr()->hidePlane(plane);
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
            return Image->getOverlayPtr()->hideAllPlanes();
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
            return Image->getOverlayPtr()->placePlane(plane, left, top);
        return 0;
    }

    /** .
     *
     ** @param  idx    index of overlay group (0 = dataset, 1 = additional), default: 0
     *
     ** @return number of overlay planes stored in the image
     */
    inline unsigned int getOverlayCount(const unsigned int idx = 0) const
    {
        if ((Image != NULL) && (Image->getOverlayPtr(idx) != NULL))
            return Image->getOverlayPtr()->getCount();
        return 0;
    }

    /** get group number of specified overlay plane (UNTESTED!)
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

    /** get label of specified overlay plane (UNTESTED!)
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

    /** get description of specified overlay plane (UNTESTED!)
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

    /** get mode of specified overlay plane (UNTESTED!)
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

    /** create 8 bits per pixel ...
     *
     ** @param  plane   number (0..15) or group number (0x60nn) of overlay plane
     *  @param  width   .
     *  @param  height  .
     *  @param  left    .
     *  @param  top     .
     *  @param  mode    .
     *  @param  frame   ..., default: 0
     *  @param  idx     index of overlay group (0 = dataset, 1 = additional, 2 = '1' plane hides '0' plane), default: 2
     *  @param  value   ..., default: 255
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
                                       const unsigned int idx = 2,
                                       const Uint8 value = 0xff) const
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            return Image->getMonoImagePtr()->getOverlayData(frame, plane, left, top, width, height, mode, idx, value);
        return NULL;
    }
    
    /** delete buffer for overlay plane data
     *  Save memory if data is no longer needed.
     */
    inline void deleteOverlayData() const 
    {
        if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
            Image->getMonoImagePtr()->deleteOverlayData();
    }
    

 // --- create...Image: return pointer to new 'DicomImage' object, memory isn't handled internally !
    
    /** Method:
     *
     ** @param fstart
     *  @param fcount
     */
    DicomImage *createDicomImage(unsigned long fstart = 0,
                                 unsigned long fcount = 0) const;

    /** .
     *
     ** @param
     *
     ** @return
     */
    DicomImage *createScaledImage(const unsigned long width,
                                  const unsigned long height = 0,
                                  const int interpolate = 0,
                                  int aspect = 0) const;

    /** .
     *
     ** @param
     *
     ** @return
     */
    DicomImage *createScaledImage(const double xfactor,
                                  const double yfactor = 0,
                                  const int interpolate = 0,
                                  const int aspect = 0) const;

    /** !! NOT FULLY IMPLEMENTED !!
     *
     ** @param
     *
     ** @return
     */
    DicomImage *createScaledImage(const unsigned long left,
                                  const unsigned long top,
                                  unsigned long clip_width,
                                  unsigned long clip_height,
                                  unsigned long scale_width,
                                  unsigned long scale_height = 0,
                                  const int interpolate = 0,
                                  int aspect = 0) const;

    /** .
     *
     ** @param
     *
     ** @return
     */
    DicomImage *createScaledImage(const unsigned long left,
                                  const unsigned long top,
                                  unsigned long width,
                                  unsigned long height,
                                  const double xfactor,
                                  const double yfactor = 0,
                                  const int interpolate = 0,
                                  const int aspect = 0) const;

    /** .
     *
     ** @param
     *
     ** @return
     */
    DicomImage *createClippedImage(const unsigned long left,
                                   const unsigned long top,
                                   unsigned long width = 0,
                                   unsigned long height = 0) const;

    /** Flip image
     *
     ** @param  horz
     *  @param  vert
     *
     ** @return
     */
    int flipImage(int horz = 1,
                  int vert = 0) const;

    /** Create flipped image
     *
     ** @param  horz
     *  @param  vert
     *  @param  fstart
     *  @param  fcount
     *
     ** @return
     */
    DicomImage *createFlippedImage(int horz = 1,
                                   int vert = 0) const;

    /** Rotate  image
     *
     ** @param  degree
     *
     ** @return 
     */
    int rotateImage(signed int degree) const;

    /** Create rotated image
     *
     ** @param  degree
     *  @param  fstart
     *  @param  fcount
     *
     ** @return image
     */
    DicomImage *createRotatedImage(signed int degree) const;

    /** Create monochrome image
     *
     ** @param  degree
     *  @param  fstart
     *  @param  fcount
     *
     ** @return image
     */
    DicomImage *createMonochromeImage(const double red = 0.299,
                                      const double green = 0.587,
                                      const double blue = 0.114) const;

    /** .
     *
     ** @param
     *
     ** @return
     */
    void *createTrueColorDIB(const unsigned long frame = 0)
    {
        return (Image != NULL) ? Image->createDIB(frame) : NULL;
    }
    
    /** .
     *
     ** @param
     *
     ** @return
     */
    void *createJavaAWTBitmap(const unsigned long frame = 0,
                              const int bits = 32)
    {
        return (Image != NULL) ? Image->createAWTBitmap(frame, bits) : NULL;
    }

 // --- ppm: return true ('1') if successful

    /** .
     *
     ** @param
     *
     ** @return
     */
    int writePPM(const char *filename,
                 const int bits = 0,
                 const unsigned long frame = 0);
                 
    /** .
     *
     ** @param
     *
     ** @return
     */
    int writePPM(ostream &stream,
                 const int bits = 0,
                 const unsigned long frame = 0);
                 
    /** .
     *
     ** @param
     *
     ** @return
     */
    int writePPM(FILE *stream,
                 const int bits = 0,
                 const unsigned long frame = 0);

    /** .
     *
     ** @param
     *
     ** @return
     */
    int writeRawPPM(const char *filename,
                    const int bits = 0,
                    const unsigned long frame= 0);

    /** .
     *
     ** @param
     *
     ** @return
     */
    int writeRawPPM(FILE *stream,
                    const int bits = 0,
                    const unsigned long frame = 0);


 protected:

    /** Constructor: 
     *
     ** @param dicom
     *  @param image
     *  @param interpret
     */
    DicomImage(const DicomImage *dicom,
               DiImage *image,
               const EP_Interpretation interpret = EPI_Unknown);
    
    /** .
     *
     ** @param
     */
    void Init();

    /** .
     *
     ** @param
     *
     ** @return
     */
    int checkDataDictionary();

    /** .
     *
     ** @param
     *
     ** @return
     */
    const char *getSOPclassUID() const;
    
    /** .
     *
     ** @param
     *
     ** @return
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
 * Revision 1.9  1999-02-05 16:42:22  joergr
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
