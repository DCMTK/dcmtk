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
 *  Purpose: DicomImage (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-30 12:24:07 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diimage.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIIMAGE_H
#define __DIIMAGE_H

#include "osconfig.h"
#include "dctypes.h"

#ifdef SUNCC
 #include "didocu.h"
#endif

#include "diutils.h"

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmPixelData;
class DcmUnsignedShort;

#ifndef SUNCC
 class DiDocument;
#endif

class DiInputPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Base class for images
 *
 */
class DiImage
{
 
 public:
 
    /** constructor
     *
     ** @param  docu
     *  @param  status
     *  @param  spp
     */
    DiImage(const DiDocument *docu,
            const EI_Status status,
            const int spp);

    virtual ~DiImage();

    inline EI_Status getStatus() const
    {
        return ImageStatus;
    }
    
    inline Uint32 getNumberOfFrames() const
    { 
        return NumberOfFrames; 
    }

    inline Uint16 getRows() const
    {
        return Rows;
    }
    inline Uint16 getColumns() const
    {
         return Columns;
    }

    inline double getPixelWidth() const
    {
        return (PixelWidth > 0) ? PixelWidth : 1;
    }
    
    inline double getPixelHeight() const
    {
        return (PixelHeight > 0) ? PixelHeight : 1;
    }

    inline double getRowColumnRatio() const
    {
        return getPixelHeight() / getPixelWidth();
    }
    
    inline double getColumnRowRatio() const
    {
        return getPixelWidth() / getPixelHeight();
    }

    virtual int getMinMaxValues(double &, double &) const
    {
        return 0;
    }
    
    inline int getBits(const int bits = 0) const
    {
        return ((bits < 1) || (bits > MAX_BITS)) ? BitsPerSample : bits;
    }
    
    virtual void *getOutputData(const unsigned long frame,
                                const int bits,
                                const int planar) = 0;

    virtual void *getOutputPlane(const int plane) const = 0;

    virtual void deleteOutputData() = 0;
    
    virtual int setNoVOITransformation()
        { return 0; }

    virtual int setMinMaxWindow(const int)
        { return 0; }
    virtual int setHistogramWindow(const double)
        { return 0; }
        
    virtual int setWindow(const unsigned long)
        { return 0; }
    virtual int setWindow(const double, const double)
        { return 0; }
    virtual int getWindow(double &, double &)
        { return 0; }
    virtual unsigned long getWindowCount() const
        { return 0; }
    
    virtual int setVoiLut(const DcmUnsignedShort &,
                          const DcmUnsignedShort &)
        { return 0; }
    
    virtual int setVoiLut(const unsigned long)
        { return 0; }
    virtual unsigned long getVoiLutCount() const
        { return 0; }
    
    virtual int setPresentationLutShape(const ES_PresentationLut)
        { return 0; }
    
    virtual int setPresentationLut(const DcmUnsignedShort &,
                                   const DcmUnsignedShort &)
        { return 0; }
    
    virtual int showOverlay(const unsigned int)
        { return 0; }
    virtual int showOverlay(const unsigned int, const double, const double, const EM_Overlay)
        { return 0; }
        
    virtual int showAllOverlays()
        { return 0; }
    virtual int showAllOverlays(const double, const double, const EM_Overlay)
        { return 0; }

    virtual int hideOverlay(const unsigned int)
        { return 0; }
    virtual int hideAllOverlays()
        { return 0; }

    virtual int placeOverlay(const unsigned int, const signed int, const signed int)
        { return 0; }
    virtual unsigned int getOverlayCount() const
        { return 0; }

    /** Method: 
     *  @param  fstart
     *  @param  fcount
     *  @return image
     */
    virtual DiImage *createImage(const unsigned long fstart,
                                 const unsigned long fcount) const = 0;

    virtual DiImage *createScale(const unsigned long left,
                                 const unsigned long top,
                                 const unsigned long clip_width,
                                 const unsigned long clip_height,
                                 const unsigned long scale_width,
                                 const unsigned long scale_height,
                                 const int interpolate,
                                 const int aspect) const = 0;

    virtual int flip(const int horz,
                     const int vert) = 0;
    
    virtual DiImage *createFlip(const int horz,
                                const int vert) const = 0;
    
    virtual int rotate(const int degree);

    virtual DiImage *createRotate(const int degree) const = 0;

    virtual DiImage *createMono(const double red,
                                const double green,
                                const double blue) const = 0;

    virtual void *createDIB(const unsigned long frame) = 0;

    virtual void *createAWTBitmap(const unsigned long frame,
                                  const int bits) = 0;

    virtual int writePPM(ostream &stream,
                         const unsigned long frame,
                         const int bits) = 0;
    
    virtual int writePPM(FILE *stream,
                         const unsigned long frame,
                         const int bits) = 0;
    
    virtual int writeRawPPM(FILE *stream,
                            const unsigned long frame,
                            const int bits) = 0; 
    

 protected:

    DiImage(const DiDocument *docu,
            const EI_Status status);
            
    /** Constructor: 
     *  @param image
     *  @param fstart
     *  @param fcount
     */
    DiImage(const DiImage *image,
            const unsigned long fstart,
            const unsigned long fcount);

    DiImage(const DiImage *image,
            const Uint16 width,
            const Uint16 height,
            const int aspect = 0);

    DiImage(const DiImage *image,
            const int degree = 0);

    void deleteInputData();
    void checkPixelExtension();
    void convertPixelData(/*const*/ DcmPixelData *pixel,
                          const int);
    int detachPixelData();
        
    EI_Status ImageStatus;                          // copy of status variable declared in class 'DicomImage'
    const DiDocument *Document;                     // points to special object, which capsulates the DCMTK

    Uint32 FirstFrame;                              // first frame to be processed
    Uint32 NumberOfFrames;                          // number of frames in case of multi-frame images (otherwise '1')
    Uint16 Rows;                                    // number of rows (in pixel)
    Uint16 Columns;                                 // number of columns (in pixel)
    double PixelWidth;                              // width of each pixel according to 'PixelSpacing/AspectRatio'
    double PixelHeight;                             // height of each pixel according to 'PixelSpacing/AspectRatio'
    Uint16 BitsAllocated;                           // number of bits allocated for each pixel
    Uint16 BitsStored;                              // number of bits stored for each pixel (see 'BitsPerSample')
    Uint16 HighBit;                                 // position of highest stored bit 
    
    int BitsPerSample;                              // actual number of bits per sample (depth)
    
    int hasSignedRepresentation;                    // is 'true' if pixel data is signed
    int hasPixelSpacing;                            // is 'true' if attribute 'PixelSpacing' is present
    int hasPixelAspectRatio;                        // is 'true' if attribute 'PixelAspectRatio' is present
    int isOriginal;                                 // is 'false' if derived from original image data (e.g. scaled)

    DiInputPixel *InputData;                        // points to intermediate pixel representation (template object)

 // --- declarations to avoid compiler warnings
 
    DiImage(const DiImage &);
    DiImage &operator=(const DiImage &);
};


#endif




/*
**
** CVS/RCS Log:
** $Log: diimage.h,v $
** Revision 1.2  1998-11-30 12:24:07  joergr
** Added const type qualifier to some parameters to avoid errors with MSVC5
** (couldn't create instance of abstract class).
**
** Revision 1.1  1998/11/27 15:06:08  joergr
** Added copyright message.
** Added methods and constructors for flipping and rotating, changed for
** scaling and clipping.
** Added method to directly create java AWT bitmaps.
** Renamed variable 'Status' to 'ImageStatus' because of possible conflicts
** with X windows systems.
** Added method to detach pixel data if it is no longer needed.
** Added methods to support presentation LUTs and shapes.
**
** Revision 1.6  1998/07/01 08:39:21  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.5  1998/05/11 14:53:16  joergr
** Added CVS/RCS header to each file.
**
**
*/

