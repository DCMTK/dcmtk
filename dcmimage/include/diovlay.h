/*********************************************************************
** 
**  DicomOverlay (Header)
**
**  author   : Joerg Riesmeier
**  created  : 10.01.97
**  modified : 25.05.97
**
*********************************************************************/


#ifndef __DIOVLAY_H
#define __DIOVLAY_H

#include "osconfig.h"
#include "dctypes.h"

#include "diobjcou.h"
#include "diovpln.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiDocument;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiOverlayData : public DiObjectCounter
{
    friend class DiOverlay;

 public:
    DiOverlayData(unsigned int count = 0, DiOverlayPlane **planes = NULL, Uint16 *data = NULL)
      : Count(count),
        Planes(planes),
        Data(data)
    {
    }
    
    virtual ~DiOverlayData()
    {
        if (Planes != NULL)
        {
            register unsigned int i;
            for (i = 0; i < Count; i++)
                delete Planes[i];
        }
        delete Planes;
        delete Data;
    }
    
 private:
    unsigned int Count;                 // number of planes
    
    DiOverlayPlane **Planes;            // points to an array of planes
    Uint16 *Data;                       // points to overlay data (only if scaled)
};


/********************************************************************/


class DiOverlay
{
 public:
    DiOverlay(const DiDocument *, const Uint16 = 0);
    DiOverlay(const DiOverlay *, const Uint16, const Uint16);
    DiOverlay(const DiOverlay *, const double, const double);
    virtual ~DiOverlay();
    
    int showPlane(const unsigned int);
    int showPlane(const unsigned int, const double, const double, const EM_Overlay);
    int showAllPlanes();
    int showAllPlanes(const double, const double, const EM_Overlay);
    int hidePlane(const unsigned int);
    int hideAllPlanes();
    int placePlane(const unsigned int, const signed int, const signed int);
    
    inline unsigned int getCount() const
        { return (Data != NULL) ? Data->Count : 0; }
    inline Uint16 getLeft() const
        { return Left; }
    inline Uint16 getTop() const
        { return Top; }
        
    inline DiOverlayPlane *getPlane(const unsigned long plane) const
        { return ((Data != NULL) && (Data->Planes != NULL) && (plane < Data->Count)) ? Data->Planes[plane] :
            (DiOverlayPlane *)NULL; }
    
 private:
    Uint16 Left;                        // left offset to all planes
    Uint16 Top;                         // top offset to all planes
    Uint16 Width;                       // maximum width of all planes 
    Uint16 Height;                      // maximum height of all planes
    unsigned long Frames;               // maximum number of frames

    DiOverlayData *Data;                // points to overlay data (provides shared data)
};


#endif
