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
 *  Update Date:      $Date: 1998-11-27 15:20:57 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimoimg.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
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


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiColorImage;
class DiLookupTable;
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
                const DcmUnsignedShort &descriptor);

    virtual ~DiMonoImage();

    inline int getMinMaxValues(double &min, double &max) const
    {
        return (InterData != NULL) ? InterData->getMinMaxValues(min, max) : 0; 
    }

    int setNoVOITransformation();

    int setMinMaxWindow(const int idx = 1);

    int setHistogramWindow(const double thresh);

    int setWindow(const unsigned long pos = 0);

    int setWindow(const double width,
                  const double height);

    int getWindow(double &width,
                  double &height);

    inline unsigned long getWindowCount() const
    {
        return WindowCount;
    }

    int setVoiLut(const DcmUnsignedShort &data,
                  const DcmUnsignedShort &descriptor);
    
    int setVoiLut(const unsigned long pos);

    inline unsigned long getVoiLutCount() const
    {
        return VoiLutCount;
    }

    int setPresentationLutShape(const ES_PresentationLut shape);

    int setPresentationLut(const DcmUnsignedShort &data,
                           const DcmUnsignedShort &descriptor);
    
    inline int showOverlay(const unsigned int plane)
        { return (Overlays != NULL) ? Overlays->showPlane(plane) : 0; }
    inline int showOverlay(const unsigned int plane, const double fore, const double thresh, const EM_Overlay mode)
        { return (Overlays != NULL) ? Overlays->showPlane(plane, fore, thresh, mode) : 0; }

    inline int showAllOverlays()
        { return (Overlays != NULL) ? Overlays->showAllPlanes() : 0; }
    inline int showAllOverlays(const double fore, const double thresh, const EM_Overlay mode)
        { return (Overlays != NULL) ? Overlays->showAllPlanes(fore, thresh, mode) : 0; }

    inline int hideOverlay(const unsigned int plane)
        { return (Overlays != NULL) ? Overlays->hidePlane(plane) : 0; }
    inline int hideAllOverlays()
        { return (Overlays != NULL) ? Overlays->hideAllPlanes() : 0; }

    inline int placeOverlay(const unsigned int plane, const signed int left, const signed int top)
        { return (Overlays != NULL) ? Overlays->placePlane(plane, left, top) : 0; }
    inline unsigned int getOverlayCount() const
        { return (Overlays != NULL) ? Overlays->getCount() : 0; }

    int flip(const int, const int);
    int rotate(const int);

    virtual void *getOutputData(const unsigned long, const int, const int = 0) = 0;
    void *getOutputPlane(const int) const;
    void deleteOutputData();

    const DiMonoPixel *getInterData() const
        { return InterData; }

    void *createDIB(const unsigned long);
    void *createAWTBitmap(const unsigned long, const int);

    int writePPM(ostream &, const unsigned long, const int); 
    int writePPM(FILE *, const unsigned long, const int); 
    int writeRawPPM(FILE *, const unsigned long, const int); 

 protected:

    DiMonoImage(const DiDocument *image,
                const EI_Status status,
                const char dummy);

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

    void *getData(const unsigned long,
                  const int,
                  const Uint32,
                  const Uint32);

    double WindowCenter;                            // center of current VOI-window
    double WindowWidth;                             // width of current VOI-window
    unsigned long WindowCount;                      // number of stored VOI-windows
    unsigned long VoiLutCount;                      // number of stored VOU-LUTs
    
    int ValidWindow;                                // true, if current VOI-window is valid
    
    ES_PresentationLut PresLutShape;                // presentation LUT shape (identity or inverse)

    DiOverlay *Overlays;                            // points to associated overlay-object
    DiLookupTable *VoiLutData;                      // points to associated VOI-LUT-object
    DiLookupTable *PresLutData;                     // points to associated presentation-LUT-object
    DiMonoPixel *InterData;                         // points to intermediate pixel data representation (object)

 private:
    DiMonoOutputPixel *OutputData;                  // points to current output data (object)

 // --- declarations to avoid compiler warnings
 
    DiMonoImage(const DiMonoImage &);
    DiMonoImage &operator=(const DiMonoImage &);
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimoimg.h,v $
** Revision 1.1  1998-11-27 15:20:57  joergr
** Added copyright message.
** Added methods and constructors for flipping and rotating, changed for
** scaling and clipping.
** Added method to directly create java AWT bitmaps.
** Added constructors to use external modality transformations.
** Added methods to support presentation LUTs and shapes.
**
** Revision 1.6  1998/07/01 08:39:22  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.5  1998/05/11 14:53:20  joergr
** Added CVS/RCS header to each file.
**
**
*/

