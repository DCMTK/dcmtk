/*
**
** Author:  Joerg Riesmeier
** Created: 10.01.97
**
** Module:  diovlay.cc
**
** Purpose: DicomOverlay (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:34 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/Attic/diovlay.cc,v $
** CVS/RCS Revision: $Revision: 1.5 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


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
  : Left(0),
    Top(0),
    Width(0),
    Height(0),
    Frames(0),
    Data(NULL)
{
    Data = new DiOverlayData();
    if ((docu != NULL) && (Data != NULL))
    {
        Data->Planes = new DiOverlayPlane *[MAX_OVERLAY_COUNT];         // can't determine number of overlays :-(
        if (Data->Planes != NULL)
        {
            do
            {
                Data->Planes[Data->Count] = new DiOverlayPlane(docu, Data->Count, alloc);
                if ((Data->Planes[Data->Count] != NULL) && (Data->Planes[Data->Count]->isValid()))
                {
                    if (Data->Planes[Data->Count]->getWidth() > Width)              // determine maximum width
                        Width = Data->Planes[Data->Count]->getWidth();
                    if (Data->Planes[Data->Count]->getHeight() > Height)            // determine maximum height
                        Height = Data->Planes[Data->Count]->getHeight();
                    if (Data->Planes[Data->Count]->getNumberOfFrames() > Frames)    // determine maximum frames
                        Frames = Data->Planes[Data->Count]->getNumberOfFrames();
                }
                (Data->Count)++;
            }
            while ((Data->Count <= MAX_OVERLAY_COUNT) && (Data->Planes[Data->Count - 1] != NULL) &&
                (Data->Planes[Data->Count - 1]->isValid()));
            if ((Data->Planes[Data->Count - 1] == NULL) || (!Data->Planes[Data->Count - 1]->isValid()))
                delete Data->Planes[--(Data->Count)];
        }
    }
}


// --- scale overlay

DiOverlay::DiOverlay(const DiOverlay *overlay, const double xfactor, const double yfactor)
  : Left(overlay->Left),
    Top(overlay->Top),
    Width((Uint16)(xfactor * overlay->Width)),
    Height((Uint16)(yfactor * overlay->Height)),
    Frames(overlay->Frames),
    Data(NULL)
{
    Data = new DiOverlayData(overlay->Data->Count);
    const unsigned long count = (unsigned long)overlay->Width * (unsigned long)overlay->Height * overlay->Frames;
    if ((Data != NULL) && (Data->Count > 0) && (count > 0))
    {
        Data->Planes = new DiOverlayPlane *[Data->Count];
        Data->Data = new Uint16[(unsigned long)Width * (unsigned long)Height * Frames];
        Uint16 *temp = new Uint16[count];
        if ((Data->Planes != NULL) && (Data->Data != NULL) && (temp != NULL))
        {
            register unsigned int i;
            for (i = 0; i < Data->Count; i++)
                Data->Planes[i] = new DiOverlayPlane(overlay->Data->Planes[i], i, Data->Data, temp, overlay->Width,
                    overlay->Height, Width, Height, xfactor, yfactor);
            DiScaleTemplate<Uint16> scale(1, overlay->Width, overlay->Height, Width, Height, Frames);
            scale.scaleData((const Uint16 **)&temp, &(Data->Data), 0);
        }
        else
            Data->Count = 0;
        delete temp;
    }
}


// --- clip overlay

DiOverlay::DiOverlay(const DiOverlay *overlay, const Uint16 left, const Uint16 top)
  : Left(left),
    Top(top),
    Width(overlay->Width),
    Height(overlay->Height),
    Frames(overlay->Frames),
    Data(overlay->Data)
{
    if (Data != NULL)
        Data->addReference();                           // use the same data as 'overlay data'
}


/*--------------*
 *  destructor  *
 *--------------*/

DiOverlay::~DiOverlay()
{
    if (Data != NULL)
        Data->removeReference();
}


/********************************************************************/


int DiOverlay::showPlane(const unsigned int plane)
{
    if ((Data != NULL) && (Data->Planes != NULL) && (plane < Data->Count))
    {
        Data->Planes[plane]->show();
        return 1;
    }
    return 0; 
}


int DiOverlay::showPlane(const unsigned int plane, const double fore, const double tresh, const EM_Overlay mode)
{
    if ((Data != NULL) && (Data->Planes != NULL) && (plane < Data->Count))
    {
        Data->Planes[plane]->show(fore, tresh, mode);
        return 1;
    }
    return 0;
}


int DiOverlay::showAllPlanes()
{
    if ((Data != NULL) && (Data->Planes != NULL))
    {
        register unsigned int i; 
        for (i = 0; i < Data->Count; i++)
            Data->Planes[i]->show();
        return (Data->Count > 0);
    }
    return 0;
}


int DiOverlay::showAllPlanes(const double fore, const double tresh, const EM_Overlay mode)
{
    if ((Data != NULL) && (Data->Planes != NULL))
    {
        register unsigned int i; 
        for (i = 0; i < Data->Count; i++)
            Data->Planes[i]->show(fore, tresh, mode);
        return (Data->Count > 0);
    }
    return 0;
}


int DiOverlay::hidePlane(const unsigned int plane)
{
    if ((Data != NULL) && (Data->Planes != NULL) && (plane < Data->Count))
    {
        Data->Planes[plane]->hide();
        return 1;
    }
    return 0;
}


int DiOverlay::hideAllPlanes()
{
    if ((Data != NULL) && (Data->Planes != NULL))
    {
        register unsigned int i; 
        for (i = 0; i < Data->Count; i++)
            Data->Planes[i]->hide();
        return (Data->Count > 0);
    }
    return 0;
}


int DiOverlay::placePlane(const unsigned int plane, const signed int left, const signed int top)
{
    if ((Data != NULL) && (Data->Planes != NULL) && (plane < Data->Count))
    {
        Data->Planes[plane]->place(left, top);
        return 1;
    }
    return 0;
}


/*
**
** CVS/RCS Log:
** $Log: diovlay.cc,v $
** Revision 1.5  1998-05-11 14:52:34  joergr
** Added CVS/RCS header to each file.
**
**
*/
