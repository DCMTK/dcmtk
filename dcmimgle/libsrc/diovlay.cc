/*
 *
 *  Copyright (C) 1997-99, OFFIS
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
 *  Purpose: DicomOverlay (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 16:19:25 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/diovlay.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"

#include "diovlay.h"
#include "discalet.h"
#include "diflipt.h"
#include "didocu.h"


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define MAX_OVERLAY_COUNT 16


/*----------------*
 *  constructors  *
 *----------------*/

DiOverlay::DiOverlay(const DiDocument *docu,
                     const Uint16 alloc)
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

DiOverlay::DiOverlay(const DiOverlay *overlay,
                     const Uint16 left,
                     const Uint16 top,
                     const double xfactor,
                     const double yfactor)
  : Left(left),
    Top(top),
    Width((Uint16)(xfactor * overlay->Width)),
    Height((Uint16)(yfactor * overlay->Height)),
    Frames(overlay->Frames),
    Data(NULL)
{
    Uint16 *temp = Init(overlay, xfactor, yfactor);
    if (temp != NULL)
    {
        DiScaleTemplate<Uint16> scale(1, overlay->Width, overlay->Height, Width, Height, Frames);
        scale.scaleData((const Uint16 **)&temp, &(Data->Data), 0);
        if (temp != overlay->Data->Data)
            delete[] temp;
    }
/*
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
        delete[] temp;
    }
*/
}


// --- flip overlay

DiOverlay::DiOverlay(const DiOverlay *overlay,
                     const int horz,
                     const int vert)
  : Left(overlay->Left),
    Top(overlay->Top),
    Width(overlay->Width),
    Height(overlay->Height),
    Frames(overlay->Frames),
    Data(NULL)
{
/*
    Uint16 *temp = Init(overlay);
    if (temp != NULL)
    {
        for (unsigned int i = 0; i < Data->Count; i++)
            Data->Planes[i]->flipOrigin(horz, vert);
        DiFlipTemplate<Uint16> flip(1, Width, Height, Frames);
        flip.flipData((const Uint16 **)&temp, &(Data->Data), horz, vert);
        if (temp != overlay->Data->Data)
            delete[] temp;
    }
*/
    if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
        cerr << "WARNING: flipping of overlays not implemented !" << endl;
}


// --- rotate overlay

DiOverlay::DiOverlay(const DiOverlay *overlay,
                     const int degree)
  : Left(overlay->Left),
    Top(overlay->Top),
    Width(overlay->Width),
    Height(overlay->Height),
    Frames(overlay->Frames),
    Data(NULL)
{
    if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
        cerr << "WARNING: rotation of overlays not implemented !" << endl;
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


Uint16 *DiOverlay::Init(const DiOverlay *overlay,
                        const double xfactor,
                        const double yfactor)
{
    if ((overlay != NULL) && (overlay->Data != NULL))
    {
        Data = new DiOverlayData(overlay->Data->Count);
        const unsigned long count = (unsigned long)overlay->Width * (unsigned long)overlay->Height * overlay->Frames;
        if ((Data != NULL) && (Data->Count > 0) && (count > 0))
        {
            Data->Planes = new DiOverlayPlane *[Data->Count];
            Data->Data = new Uint16[(unsigned long)Width * (unsigned long)Height * Frames];
            if ((Data->Planes != NULL) && (Data->Data != NULL))
            {
                Uint16 *temp = NULL;
                if (overlay->Data->Data == NULL)                    // no data buffer
                    temp = new Uint16[count];                       // create temporary buffer
                register unsigned int i;                            // copy bits from each plane to buffer
                for (i = 0; i < Data->Count; i++)
                    Data->Planes[i] = new DiOverlayPlane(overlay->Data->Planes[i], i, Data->Data, temp, overlay->Width,
                        overlay->Height, Width, Height, xfactor, yfactor);
                if (overlay->Data->Data != NULL)                    // existing data buffer
                    temp = overlay->Data->Data;                     // point to input buffer
                return temp;
            }
            else
                Data->Count = 0;
        }
    }
    return NULL;
}


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


int DiOverlay::hasEmbeddedData() const
{
    if ((Data != NULL) && (Data->Planes != NULL))
    {
        register unsigned int i;
        for (i = 0; i < Data->Count; i++)
        {
            if (Data->Planes[i]->isEmbedded())
                return 1;
        }
    }
    return 0;
}


/*
**
** CVS/RCS Log:
** $Log: diovlay.cc,v $
** Revision 1.1  1998-11-27 16:19:25  joergr
** Added methods and constructors for flipping and rotating, changed for
** scaling and clipping.
** Replaced delete by delete[] for array types.
** Added method to detach pixel data if it is no longer needed.
** Introduced global debug level for dcmimage module to control error output.
**
** Revision 1.5  1998/05/11 14:52:34  joergr
** Added CVS/RCS header to each file.
**
**
*/
