/*
**
** Author:  Joerg Riesmeier
** Created: 09.12.96
**
** Module:  dcmimage.h
**
** Purpose: Provides main interface to the "dicom image toolkit". 
** (see implementation file 'dcmimage.cc' for parameter description
**  and written elaboration (diploma thesis) for futher details)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-06-25 08:50:09 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dcmimage.h,v $
** CVS/RCS Revision: $Revision: 1.11 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DCMIMAGE_H
#define __DCMIMAGE_H

#include "osconfig.h"

#define OFFIS_DCMIMAGE_VERSION     "1.0.2"
#define OFFIS_DCMIMAGE_RELEASEDATE "1998/02/17"

#include "dcxfer.h"
#include "diimage.h"
#include "diutils.h"


/*--------------------*
 *  type definitions  *
 *--------------------*/

enum EP_Interpretation
{
    EPI_Unknown,
    EPI_Monochrome1,
    EPI_Monochrome2,
    EPI_PaletteColor,
    EPI_RGB,
    EPI_HSV,
    EPI_ARGB,
    EPI_CMYK,
    EPI_YBR_Full,
    EPI_YBR_Full_422,
    EPI_YBR_Partial_422
};


struct SP_Interpretation
{
    char *Name;
    EP_Interpretation Type;
};


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmFileStream;
class DcmObject;
class DiDocument;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DicomImage
{
 public:
 
 // --- constructors and destructor
 
    DicomImage(const char *, const unsigned long = 0);
    DicomImage(DcmFileStream &, const unsigned long = 0);
    DicomImage(DcmObject *, E_TransferSyntax, const unsigned long = 0);

    virtual ~DicomImage();
    
 // --- information: return requested value if successful

    inline EI_Status getStatus() const 
        { return (Image != NULL) ? Image->getStatus() : Status; }
            
    inline unsigned long getFrameCount() const
        { return (Image != NULL) ? Image->getNumberOfFrames() : 0; }

    inline unsigned long getWidth() const 
        { return (Image != NULL) ? Image->getColumns() : 0; }
    inline unsigned long getHeight() const 
        { return (Image != NULL) ? Image->getRows() : 0; }
    inline int getDepth() const
        { return (Image != NULL) ? Image->getBits() : 0; }
        
    inline int getMinMaxValues(double &min, double &max) const
        { return (Image != NULL) ? Image->getMinMaxValues(min, max) : 0; }
        
    inline double getWidthHeightRatio() const
        { return (Image != NULL) ? Image->getColumnRowRatio() : 0; }
    inline double getHeightWidthRatio() const
        { return (Image != NULL) ? Image->getRowColumnRatio() : 0; }
    
 // --- output: return pointer to output data if successful
    
    inline void *getOutputData(const int bits = 0, const unsigned long frame = 0, const int planar = 0) 
        { return (Image != NULL) ? Image->getOutputData(frame, Image->getBits(bits), planar) : NULL; }
    inline void *getOutputPlane(const int plane) const 
        { return (Image != NULL) ? Image->getOutputPlane(plane) : NULL; }
    inline void deleteOutputData() const 
        { if (Image != NULL) Image->deleteOutputData(); }
    
  // --- misc
  
    inline int isMonochrome() const 
        { return (PhotometricInterpretation == EPI_Monochrome1) || (PhotometricInterpretation == EPI_Monochrome2); }
    inline EP_Interpretation getPhotometricInterpretation() const 
        { return PhotometricInterpretation; }

    int hasSOPclassUID(const char *) const;

 // --- windowing (voi): return true ('1') if successful (see also 'dimoimg.cc')
    
    inline int setMinMaxWindow(const int index = 1) 
        { return (Image != NULL) ? Image->setMinMaxWindow(index) : 0; }
    inline int setHistogramWindow(const double thresh = 0.05)
        { return (Image != NULL) ? Image->setHistogramWindow(thresh) : 0; }

    inline int setWindow(const unsigned long window) 
        { return (Image != NULL) ? Image->setWindow(window) : 0; }
    inline int setWindow(const double center = 0, const double width = 0) 
        { return (Image != NULL) ? Image->setWindow(center, width) : 0; }
    inline int getWindow(double &center, double &width) 
        { return (Image != NULL) ? Image->getWindow(center, width) : 0; }
    inline unsigned long getWindowCount() const 
        { return (Image != NULL) ? Image->getWindowCount() : 0; }
    
    inline int setVoiLut(const unsigned long table) 
        { return (Image != NULL) ? Image->setVoiLut(table) : 0; }
    inline unsigned long getVoiLutCount() const 
        { return (Image != NULL) ? Image->getVoiLutCount() : 0; }

 // --- overlays: return true ('1') if successful (see also 'diovlay.cc')

    inline int showOverlay(const unsigned int plane)
        { return (Image != NULL) ? Image->showOverlay(plane) : 0; }
    inline int showOverlay(const unsigned int plane, const EM_Overlay mode, const double fore = 1,
        const double thresh = 0.5)
        { return (Image != NULL) ? Image->showOverlay(plane, fore, thresh, mode) : 0; }
        
    inline int showAllOverlays()
        { return (Image != NULL) ? Image->showAllOverlays() : 0; }
    inline int showAllOverlays(const EM_Overlay mode, const double fore = 1, const double thresh = 0.5)
        { return (Image != NULL) ? Image->showAllOverlays(fore, thresh, mode) : 0; }
        
    inline int hideOverlay(const unsigned int plane)
        { return (Image != NULL) ? Image->hideOverlay(plane) : 0; }
    inline int hideAllOverlays()
        { return (Image != NULL) ? Image->hideAllOverlays() : 0; }
        
    inline int placeOverlay(const unsigned int plane, const signed int left, const signed int top)
        { return (Image != NULL) ? Image->placeOverlay(plane, left, top) : 0; }
    inline unsigned int getOverlayCount() const
        { return (Image != NULL) ? Image->getOverlayCount() : 0; }
    
 // --- create...Image: return pointer to new 'DicomImage' object, memory isn't handled internally !
    
    DicomImage *createScaledImage(unsigned long, unsigned long = 0, const int = 0, const int = 0) const;
    DicomImage *createScaledImage(const double, const double = 0, const int = 0, const int = 0) const;

    DicomImage *createClippedImage(const unsigned long, const unsigned long, unsigned long = 0,
        unsigned long = 0) const;

    DicomImage *createMonochromeImage(const double = 0.299, const double = 0.587, const double = 0.114) const;

    void *createTrueColorDIB(const unsigned long frame = 0)
        { return (Image != NULL) ? Image->createDIB(frame) : NULL; }

 // --- ppm: return true ('1') if successfull

    int writePPM(const char *, const int = 0, const unsigned long = 0);
    int writePPM(ostream &, const int = 0, const unsigned long = 0);
    int writePPM(FILE *, const int = 0, const unsigned long = 0);
    int writeRawPPM(const char *, const int = 0, const unsigned long = 0);
    int writeRawPPM(FILE *, const int = 0, const unsigned long = 0);

 protected:
    DicomImage(const DicomImage *, DiImage *, const EP_Interpretation);
    
    void Init();
    int checkDataDictionary();
    const char *getSOPclassUID() const;
    
 private:
    EI_Status Status;                                   // current state of converting progress (error level)
    EP_Interpretation PhotometricInterpretation;        // dicom color model (enumeration)

    DiDocument *Document;                               // points to document object
    DiImage    *Image;                                  // points to image object
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dcmimage.h,v $
** Revision 1.11  1998-06-25 08:50:09  joergr
** Added compatibility mode to support ACR-NEMA images and wrong
** palette attribute tags.
**
** Revision 1.10  1998/05/11 14:53:07  joergr
** Added CVS/RCS header to each file.
**
**
*/
