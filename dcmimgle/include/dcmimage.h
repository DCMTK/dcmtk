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
 *  Update Date:      $Date: 1998-11-27 14:50:00 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dcmimage.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
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

#include "dcxfer.h"

#include "diimage.h"
#include "diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmFileStream;
class DcmObject;
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
     *  @param  flags     configuration flags
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
     *  @param  flags   configuration flags
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
     *  @param  flags   configuration flags
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
     *  @param  flags      configuration flags (CIF_UsePresentationState is automatically set)
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
     *  @param  flags       configuration flags (CIF_UsePresentationState is automatically set)
     *  @param  fstart      first frame to be processed (not implemented!)
     *  @param  fcount      number of frames (not implemented!)
     */
    DicomImage(DcmObject *object,
               E_TransferSyntax xfer,
               const DcmUnsignedShort &data,
               const DcmUnsignedShort &descriptor,
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
     ** @return number of pixels in one columns
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
     *
     ** @param  min  
     *  @param  max  
     *
     ** @return status code (true if successful)
     */
    inline int getMinMaxValues(double &min,
                               double &max) const
    {
        return (Image != NULL) ? Image->getMinMaxValues(min, max) : 0;
    }
        
    /** .
     *
     ** @param
     *
     ** @return
     */
    inline double getWidthHeightRatio() const
    {
        return (Image != NULL) ? Image->getColumnRowRatio() : 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline double getHeightWidthRatio() const
    {
        return (Image != NULL) ? Image->getRowColumnRatio() : 0;
    }
    
 // --- output: return pointer to output data if successful
    
    /** .
     *
     ** @param
     *
     ** @return
     */
    inline const void *getOutputData(const int bits = 0,
                                     const unsigned long frame = 0,
                                     const int planar = 0) 
    {
        return (Image != NULL) ? Image->getOutputData(frame, Image->getBits(bits), planar) : NULL;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline const void *getOutputPlane(const int plane) const 
    {
        return (Image != NULL) ? Image->getOutputPlane(plane) : NULL;
    }

    /** .
     *
     ** @param
     */
    inline void deleteOutputData() const 
    {
        if (Image != NULL) Image->deleteOutputData();
    }
    
  // --- misc
  
    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int isMonochrome() const 
    {
        return (PhotometricInterpretation == EPI_Monochrome1) || (PhotometricInterpretation == EPI_Monochrome2);
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline EP_Interpretation getPhotometricInterpretation() const 
    {
        return PhotometricInterpretation;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    int hasSOPclassUID(const char *) const;

 // --- windowing (voi): return true ('1') if successful (see also 'dimoimg.cc')
    
    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int setNoVOITransformation() 
    {
        return (Image != NULL) ? Image->setNoVOITransformation() : 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int setMinMaxWindow(const int idx = 1) 
    {
        return (Image != NULL) ? Image->setMinMaxWindow(idx) : 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int setHistogramWindow(const double thresh = 0.05)
    {
        return (Image != NULL) ? Image->setHistogramWindow(thresh) : 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int setWindow(const unsigned long window) 
    {
        return (Image != NULL) ? Image->setWindow(window) : 0;
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
        return (Image != NULL) ? Image->setWindow(center, width) : 0;
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
        return (Image != NULL) ? Image->getWindow(center, width) : 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline unsigned long getWindowCount() const 
    {
        return (Image != NULL) ? Image->getWindowCount() : 0;
    }
    
    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int setVoiLut(const DcmUnsignedShort &data,
                         const DcmUnsignedShort &descriptor) 
    {
        return (Image != NULL) ? Image->setVoiLut(data, descriptor) : 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int setVoiLut(const unsigned long table) 
    {
        return (Image != NULL) ? Image->setVoiLut(table) : 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline unsigned long getVoiLutCount() const 
    {
        return (Image != NULL) ? Image->getVoiLutCount() : 0;
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
        return (Image != NULL) ? Image->setPresentationLutShape(shape) : 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int setPresentationLut(const DcmUnsignedShort &data,
                                  const DcmUnsignedShort &descriptor)
    {
        return (Image != NULL) ? Image->setPresentationLut(data, descriptor) : 0;
    }

 // --- overlays: return true ('1') if successful (see also 'diovlay.cc')

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int showOverlay(const unsigned int plane)
    {
        return (Image != NULL) ? Image->showOverlay(plane) : 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int showOverlay(const unsigned int plane,
                           const EM_Overlay mode,
                           const double fore = 1,
                           const double thresh = 0.5)
    {
        return (Image != NULL) ? Image->showOverlay(plane, fore, thresh, mode) : 0;
    }
        
    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int showAllOverlays()
    {
        return (Image != NULL) ? Image->showAllOverlays() : 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int showAllOverlays(const EM_Overlay mode,
                               const double fore = 1,
                               const double thresh = 0.5)
    {
        return (Image != NULL) ? Image->showAllOverlays(fore, thresh, mode) : 0;
    }
        
    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int hideOverlay(const unsigned int plane)
    {
        return (Image != NULL) ? Image->hideOverlay(plane) : 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int hideAllOverlays()
    {
        return (Image != NULL) ? Image->hideAllOverlays() : 0;
    }
        
    /** .
     *
     ** @param
     *
     ** @return
     */
    inline int placeOverlay(const unsigned int plane,
                            const signed int left,
                            const signed int top)
    {
        return (Image != NULL) ? Image->placeOverlay(plane, left, top) : 0;
    }

    /** .
     *
     ** @param
     *
     ** @return
     */
    inline unsigned int getOverlayCount() const
    {
        return (Image != NULL) ? Image->getOverlayCount() : 0;
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

    /** .
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

 // --- ppm: return true ('1') if successfull

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
**
** CVS/RCS Log:
** $Log: dcmimage.h,v $
** Revision 1.1  1998-11-27 14:50:00  joergr
** Added copyright message.
** Added methods to convert module defined enum types to strings.
** Added methods to support presentation LUTs and shapes.
** Moved type definitions to diutils.h.
** Added constructors to use external modality transformations.
** Added method to directly create java AWT bitmaps.
** Added methods and constructors for flipping and rotating, changed for
** scaling and clipping.
**
** Revision 1.12  1998/07/01 08:39:17  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.11  1998/06/25 08:50:09  joergr
** Added compatibility mode to support ACR-NEMA images and wrong
** palette attribute tags.
**
** Revision 1.10  1998/05/11 14:53:07  joergr
** Added CVS/RCS header to each file.
**
**
*/

