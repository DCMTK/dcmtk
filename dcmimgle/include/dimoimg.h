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
 *  Update Date:      $Date: 1999-02-03 17:27:58 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimoimg.h,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
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
#include "diovlay.h"
#include "diluptab.h"
#include "didispfn.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiColorImage;
class DiMonoOutputPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Base class for monochrome images
 *
 */
class DiMonoImage
  : public DiImage
{

 public:

    DiMonoImage(const DiDocument *docu,
                const EI_Status status);

    DiMonoImage(const DiDocument *docu,
                const EI_Status status,
                const double slope,
                const double intercept);

    DiMonoImage(const DiDocument *docu,
                const EI_Status status,
                const DcmUnsignedShort &data,
                const DcmUnsignedShort &descriptor,
                const DcmLongString *explanation);

    virtual ~DiMonoImage();

    int getMinMaxValues(double &min,
                        double &max,
                        const int mode) const;

    int setDisplayFunction(DiDisplayFunction *display);

    int setNoDisplayFunction();

    int setNoVoiTransformation();

    inline int deleteBartenLUT(const int bits)
    {
        return (DisplayFunction != NULL) ? DisplayFunction->deleteBartenLUT(bits) : 0;
    }

    int convertPValueToDDL(const Uint16 pvalue,
                           Uint16 &ddl);

    int setMinMaxWindow(const int idx = 1);

    int setHistogramWindow(const double thresh);

    int setWindow(const unsigned long pos = 0);

    int setWindow(const double width,
                  const double height,
                  const char *explanation = NULL);

    int getWindow(double &width,
                  double &height);

    inline unsigned long getWindowCount() const
    {
        return WindowCount;
    }

    int setVoiLut(const DcmUnsignedShort &data,
                  const DcmUnsignedShort &descriptor,
                  const DcmLongString *explanation);
    
    int setVoiLut(const unsigned long pos);

    inline unsigned long getVoiLutCount() const
    {
        return VoiLutCount;
    }

    inline const char *getVoiTransformationExplanation() const
    {
        return VoiExplanation.c_str();
    }

    inline const char *getModalityLutExplanation() const
    {
        return (InterData != NULL) ? InterData->getModalityLutExplanation() : NULL;
    }

    int setPresentationLutShape(const ES_PresentationLut shape);

    int setPresentationLut(const DcmUnsignedShort &data,
                           const DcmUnsignedShort &descriptor,
                           const DcmLongString *explanation = NULL);

    inline const char *getPresentationLutExplanation() const
    {
        return (PresLutData != NULL) ? PresLutData->getExplanation() : NULL;
    }

    int addOverlay(const unsigned int group,
                   const signed int left,
                   const signed int top,
                   const unsigned int columns,
                   const unsigned int rows,
                   const DcmOverlayData &data,
                   const DcmLongString &label,
                   const DcmLongString &description,
                   const EM_Overlay mode);

    int removeOverlay(const unsigned int group);

    inline DiOverlay *getOverlayPtr(const unsigned int idx = 0)
    {
        return (idx < 2) ? Overlays[idx] : NULL;
    }

    inline DiMonoImage *getMonoImagePtr()
    {
        return this;
    }

    int flip(const int, const int);
    int rotate(const int);

    virtual void *getOutputData(const unsigned long frame,
                                const int bits,
                                const int planar = 0) = 0;

    virtual int getOutputData(void *buffer,
                              const unsigned long size,
                              const unsigned long frame,
                              const int bits,
                              const int planar = 0) = 0;

    void *getOutputPlane(const int) const;

    void deleteOutputData();

    const Uint8 *getOverlayData(const unsigned long frame,
                                const unsigned int plane,
                                unsigned int &left,
                                unsigned int &top,
                                unsigned int &width,
                                unsigned int &height,
                                EM_Overlay &mode,
                                const unsigned int idx,
                                const Uint8 value = 0xff);

    void deleteOverlayData();

    const DiMonoPixel *getInterData() const
    {
        return InterData;
    }

    void *createDIB(const unsigned long);
    void *createAWTBitmap(const unsigned long, const int);

    int writePPM(ostream &, const unsigned long, const int); 
    int writePPM(FILE *, const unsigned long, const int); 
    int writeRawPPM(FILE *, const unsigned long, const int); 

 protected:

    DiMonoImage(const DiDocument *image,
                const EI_Status status,
                const char /*dummy*/);

    DiMonoImage(const DiMonoImage *image,
                const unsigned long fstart,
                const unsigned long fcount);

    DiMonoImage(const DiColorImage *image,
                const double red,
                const double green,
                const double blue);

    DiMonoImage(const DiMonoImage *image,
                const Uint16 left,
                const Uint16 top,
                const Uint16 src_cols,
                const Uint16 src_rows,                 
                const Uint16 dest_cols,
                const Uint16 dest_rows,
                const int interpolate,
                const int aspect);

    DiMonoImage(const DiMonoImage *image,
                const int horz,
                const int vert);

    DiMonoImage(const DiMonoImage *image,
                const int degree);

    void Init(DiMonoModality *modality);

    int checkInterData(const int mode = 1);

    void *getData(void *buffer,
                  const unsigned long size,
                  const unsigned long frame,
                  const int bits,
                  const int negative);

    double WindowCenter;                            // center of current VOI-window
    double WindowWidth;                             // width of current VOI-window
    unsigned long WindowCount;                      // number of stored VOI-windows
    unsigned long VoiLutCount;                      // number of stored VOU-LUTs
    
    int ValidWindow;                                // true, if current VOI-window is valid
    
    OFString VoiExplanation;                        // free text explanation of current VOI transformation
    
    ES_PresentationLut PresLutShape;                // presentation LUT shape (identity or inverse)

    DiOverlay *Overlays[2];                         // points to associated overlay-objects ([0] = built-in, [1] = additional)
    DiLookupTable *VoiLutData;                      // points to associated VOI-LUT-object
    DiLookupTable *PresLutData;                     // points to associated presentation-LUT-object
    DiMonoPixel *InterData;                         // points to intermediate pixel data representation (object)

    DiDisplayFunction *DisplayFunction;             // points to grayscale standard display function (only referenced!)


 private:

    DiMonoOutputPixel *OutputData;                  // points to current output data (object)
    Uint8 *OverlayData;                             // points to current overlay plane data (pixel array)

 // --- declarations to avoid compiler warnings
 
    DiMonoImage(const DiMonoImage &);
    DiMonoImage &operator=(const DiMonoImage &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dimoimg.h,v $
 * Revision 1.10  1999-02-03 17:27:58  joergr
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
