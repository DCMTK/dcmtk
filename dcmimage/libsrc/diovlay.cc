/*********************************************************************
** 
**  DicomOverlay (Source)
**
**  author   : Joerg Riesmeier
**  created  : 10.01.97
**  modified : 09.02.97
**
*********************************************************************/


#include "osconfig.h"
#include "dctypes.h"

#include "diovlay.h"
#include "discalet.h"
#include "didocu.h"


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define MAX_OVERLAY_COUNT 16


/*----------------*
 *  constructors  *
 *----------------*/

DiOverlay::DiOverlay(const DiDocument *docu, const Uint16 alloc)
  : Count(0),
	Left(0),
	Top(0),
	Width(0),
	Height(0),
	Frames(0),
	Planes(NULL),
	Data(NULL)
{
	if (docu != NULL)
	{
		Planes = new DiOverlayPlane *[MAX_OVERLAY_COUNT];				// can't determine number of overlays :-(
		if (Planes != NULL)
		{
			do
			{
				Planes[Count] = new DiOverlayPlane(docu, Count, alloc);
				if ((Planes[Count] != NULL) && (Planes[Count]->isValid()))
				{
					if (Planes[Count]->getWidth() > Width)				// determine maximum width
						Width = Planes[Count]->getWidth();
					if (Planes[Count]->getHeight() > Height)			// determine maximum height
						Height = Planes[Count]->getHeight();
					if (Planes[Count]->getNumberOfFrames() > Frames)	// determine maximum frames
						Frames = Planes[Count]->getNumberOfFrames();
				}
				Count++;
			}
			while ((Count <= MAX_OVERLAY_COUNT) && (Planes[Count - 1] != NULL) && (Planes[Count - 1]->isValid()));
			if ((Planes[Count - 1] == NULL) || (!Planes[Count - 1]->isValid()))
				delete Planes[--Count];
		}
	}
}


// --- scale overlay

DiOverlay::DiOverlay(const DiOverlay *overlay, const double xfactor, const double yfactor)
  : Count(overlay->Count),
	Left(overlay->Left),
	Top(overlay->Top),
	Width((Uint16)(xfactor * overlay->Width)),
	Height((Uint16)(yfactor * overlay->Height)),
	Frames(overlay->Frames),
	Planes(NULL),
	Data(NULL)
{
	const unsigned long count = (unsigned long)overlay->Width * (unsigned long)overlay->Height * overlay->Frames;
	if ((Count > 0) && (count > 0))
	{
		Planes = new DiOverlayPlane *[Count];
		Data = new Uint16[(unsigned long)Width * (unsigned long)Height * Frames];
		Uint16 *temp = new Uint16[count];
		if ((Planes != NULL) && (Data != NULL) && (temp != NULL))
		{
/*
			register unsigned long i;
			register Uint16 *q = temp;
			for (i = 0; i < count; i++);					// clear temporary buffer
				*(q++) = 0;
*/	
			register unsigned int i;
			for (i = 0; i < Count; i++)
				Planes[i] = new DiOverlayPlane(overlay->Planes[i], i, Data, temp, overlay->Width,
					overlay->Height, Width, Height, xfactor, yfactor);
			DiScaleTemplate<Uint16> scale(1, overlay->Width, overlay->Height, Width, Height, Frames);
			scale.scale((const Uint16 **)&temp, &Data, 0);
		}
		else
			Count = 0;
		delete temp;
	}
}


// --- clip overlay

DiOverlay::DiOverlay(const DiOverlay *overlay, const Uint16 left, const Uint16 top)
  : Count(overlay->Count),
	Left(left),
	Top(top),
	Width(overlay->Width),
	Height(overlay->Height),
	Frames(overlay->Frames),
	Planes(overlay->Planes),
	Data(overlay->Data)
{
	addReference();							// use the same data as 'overlay'
}


/*--------------*
 *  destructor  *
 *--------------*/

DiOverlay::~DiOverlay()
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


/********************************************************************/


int DiOverlay::showPlane(const unsigned int plane)
{
	if ((Planes != NULL) && (plane < Count))
	{
		Planes[plane]->show();
		return 1;
	}
	return 0; 
}


int DiOverlay::showPlane(const unsigned int plane, const double fore, const double tresh, const EM_Overlay mode)
{
	if ((Planes != NULL) && (plane < Count))
	{
		Planes[plane]->show(fore, tresh, mode);
		return 1;
	}
	return 0;
}


int DiOverlay::showAllPlanes()
{
	if (Planes != NULL)
	{
		register unsigned int i; 
		for (i = 0; i < Count; i++)
			Planes[i]->show();
		return (Count > 0);
	}
	return 0;
}


int DiOverlay::showAllPlanes(const double fore, const double tresh, const EM_Overlay mode)
{
	if (Planes != NULL)
	{
		register unsigned int i; 
		for (i = 0; i < Count; i++)
			Planes[i]->show(fore, tresh, mode);
		return (Count > 0);
	}
	return 0;
}


int DiOverlay::hidePlane(const unsigned int plane)
{
	if ((Planes != NULL) && (plane < Count))
	{
		Planes[plane]->hide();
		return 1;
	}
	return 0;
}


int DiOverlay::hideAllPlanes()
{
	if (Planes != NULL)
	{
		register unsigned int i; 
		for (i = 0; i < Count; i++)
			Planes[i]->hide();
		return (Count > 0);
	}
	return 0;
}


int DiOverlay::placePlane(const unsigned int plane, const signed int left, const signed int top)
{
	if ((Planes != NULL) && (plane < Count))
	{
		Planes[plane]->place(left, top);
		return 1;
	}
	return 0;
}
