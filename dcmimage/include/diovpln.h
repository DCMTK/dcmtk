/*********************************************************************
** 
**  DicomOverlayPlane (Header)   -   Multiframe Overlays UNTESTED !
**
**  author   : Joerg Riesmeier
**  created  : 10.01.97
**  modified : 08.03.98
**
*********************************************************************/


#ifndef __DIOVPLN_H
#define __DIOVPLN_H

#include "osconfig.h"
#include "dctypes.h"

#include "diutils.h"

#include <stddef.h>
#include <iostream.h>


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiDocument;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiOverlayPlane
{
 public:
    DiOverlayPlane(const DiDocument *, const unsigned int, const Uint16);
    DiOverlayPlane(DiOverlayPlane *, const unsigned int, Uint16 *, Uint16 *, const Uint16, const Uint16, const Uint16,
		const Uint16, const double, const double);
    ~DiOverlayPlane();
	
	inline Sint16 getLeft(const Uint16 left = 0) const
		{ return (Sint16)((Sint32)Left - (Sint32)left); }
	inline Sint16 getTop(const Uint16 top = 0) const
		{ return (Sint16)((Sint32)Top - (Sint32)top); }
	inline Uint16 getWidth() const
		{ return Width; }
	inline Uint16 getHeight() const
		{ return Height; }
	inline Uint16 getRight(const Uint16 left = 0) const
		{ return ((Sint32)Left + (Sint32)Width - (Sint32)left > 0) ? (Uint16)((Sint32)Left + (Sint32)Width -
			(Sint32)left) : 0; }
	inline Uint16 getBottom(const Uint16 top = 0) const
		{ return ((Sint32)Top + (Sint32)Height - (Sint32)top > 0) ? (Uint16)((Sint32)Top + (Sint32)Height -
			(Sint32)top) : 0; }

	inline int isValid() const
		{ return Valid; }
	inline int isVisible() const
		{ return Visible; }
	inline void show()
		{ Visible = 1; }
	inline void hide()
		{ Visible = 0; }
	inline void place(const signed int left, const signed int top)
		{ Left = left; Top = top; }
	
	void show(const double, const double, const EM_Overlay);

	inline unsigned long getNumberOfFrames() const
		{ return NumberOfFrames; }
	inline double getForeground() const
		{ return Foreground; }
	inline double getThreshold() const
		{ return Threshold; }
	inline EM_Overlay getMode() const
		{ return Mode; }
    
	inline int reset(const unsigned long);
	inline int getNextBit();
	inline void setStart(const Uint16, const Uint16);

 protected:
	Uint32 NumberOfFrames;			// number of frames
	Uint16 ImageFrameOrigin;		// number of starting frame

	Sint16 Top;						// y-coordinate of overlay plane's origin
	Sint16 Left;					// x-coordinate of overlay plane's origin
	Uint16 Height;					// visible height
	Uint16 Width;					// visible width
	Uint16 Rows;					// number of (stored) rows
	Uint16 Columns;					// number of (stored) columns
	Uint16 BitsAllocated;			// number of allocated bits per pixel
	Uint16 BitPosition;				// position of overlay plane bit
	
	double Foreground;				// color of overlay plane ('0.0' = dark, '1.0' = bright)
	double Threshold;				// threshold value
	EM_Overlay Mode;				// overlay mode
	EM_Overlay DefaultMode;			// default overlay mode

	int Valid;						// validity status
	int Visible;					// visibility status
	
 private:
 	unsigned long BitPos;			// current bit position
	unsigned long StartBitPos;		// starting bit position of current frame
 
 	const Uint16 *Ptr;				// points to current element of 'Data'
	const Uint16 *StartPtr;			// points to starting element of current frame
 	const Uint16 *Data;				// point to overlay data (standalone) or pixel data (embedded)
};


/********************************************************************/


inline int DiOverlayPlane::reset(const unsigned long frame)
{
	if (Valid && (Data != NULL) && (frame >= ImageFrameOrigin) && (frame < ImageFrameOrigin + NumberOfFrames))
	{
		const unsigned long bits = frame * (unsigned long)Rows * (unsigned long)Columns * (unsigned long)BitsAllocated;
		StartBitPos = BitPos = (unsigned long)BitPosition + bits;
		StartPtr = Ptr = Data + (bits >> 4);
		return (getRight() > 0) && (getBottom() > 0);
	} 
	return 0;
}


inline int DiOverlayPlane::getNextBit()
{
	if (BitsAllocated == 16)									// optimization
		return *(Ptr++) & (1 << BitPosition);
	Ptr = StartPtr + (BitPos >> 4);									// div 16
	const Uint16 value = (Uint16)((*Ptr) & (1 << (BitPos & 0xf)));	// mod 16
	BitPos += BitsAllocated;										// next bit
	return value;
}


inline void DiOverlayPlane::setStart(const Uint16 x, const Uint16 y)
{
	if (BitsAllocated == 16)
		Ptr = StartPtr + (unsigned long)(y - Top) * (unsigned long)Columns + (unsigned long)(x - Left);
	else
		BitPos = StartBitPos + ((unsigned long)(y - Top) * (unsigned long)Columns + (unsigned long)(x - Left)) *
			(unsigned long)BitsAllocated;
}


#endif
