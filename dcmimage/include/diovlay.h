/*********************************************************************
** 
**  DicomOverlay (Header)
**
**  author   : Joerg Riesmeier
**  created  : 10.01.97
**  modified : 07.02.97
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

class DiOverlay : public DiObjectCounter
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
		{ return Count; }
	inline Uint16 getLeft() const
		{ return Left; }
	inline Uint16 getTop() const
		{ return Top; }
		
	inline DiOverlayPlane *getPlane(const unsigned long plane) const
		{ return ((Planes != NULL) && (plane < Count)) ? Planes[plane] : (DiOverlayPlane *)NULL; }
	
 private:
 	unsigned int Count;					// number of planes
	
	Uint16 Left;						// left offset to all planes
	Uint16 Top;							// top offset to all planes
	Uint16 Width;						// maximum width of all planes 
	Uint16 Height;						// maximum height of all planes
	unsigned long Frames;				// maximum number of frames
	
	DiOverlayPlane **Planes;			// points to an array of planes
	Uint16 *Data;						// points to overlay data (only if scaled)
};


#endif
