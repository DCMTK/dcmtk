/*
**
** Author:  Joerg Riesmeier
** Created: 12.12.96
**
** Module:  dimoimg.h
**
** Purpose: DicomMonochromeImage (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-07-01 08:39:22 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dimoimg.h,v $
** CVS/RCS Revision: $Revision: 1.6 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
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

class DiMonoImage : public DiImage {

 public:
    DiMonoImage(const DiDocument *, const EI_Status);
    virtual ~DiMonoImage();

    inline int getMinMaxValues(double &min, double &max) const
        { return (InterData != NULL) ? InterData->getMinMaxValues(min, max) : 0; }

    int setMinMaxWindow(const int = 1);
    int setHistogramWindow(const double);
    int setWindow(const unsigned long = 0);
    int setWindow(const double, const double);
    int getWindow(double &, double &);
    inline unsigned long getWindowCount() const
        { return WindowCount; }

    int setVoiLut(const unsigned long);
    inline unsigned long getVoiLutCount() const
        { return VoiLutCount; }

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

    virtual void *getOutputData(const unsigned long, const int, const int = 0) = 0;
    void *getOutputPlane(const int) const;
    void deleteOutputData();

    const DiMonoPixel *getInterData() const
        { return InterData; }

    void *createDIB(const unsigned long);

    int writePPM(ostream &, const unsigned long, const int); 
    int writePPM(FILE *, const unsigned long, const int); 
    int writeRawPPM(FILE *, const unsigned long, const int); 

 protected:
    DiMonoImage(const DiDocument *, const EI_Status, const int);
    DiMonoImage(const DiColorImage *, const double, const double, const double);
    DiMonoImage(const DiMonoImage *, const Uint16, const Uint16, const int);
    DiMonoImage(const DiMonoImage *, const Uint16, const Uint16, const Uint16, const Uint16);

    void *getData(const unsigned long, const int, const Uint32, const Uint32);

    double WindowCenter;                            // center of current VOI-window
    double WindowWidth;                             // width of current VOI-window
    unsigned long WindowCount;                      // number of stored VOI-windows
    unsigned long VoiLutCount;                      // number of stored VOU-LUTs
    
    int ValidWindow;                                // true, if current VOI-window is valid
    
    DiOverlay *Overlays;                            // points to associated overlay-object
    DiLookupTable *VoiLutData;                      // points to associated VOI-LUT-object
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
** Revision 1.6  1998-07-01 08:39:22  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.5  1998/05/11 14:53:20  joergr
** Added CVS/RCS header to each file.
**
**
*/

