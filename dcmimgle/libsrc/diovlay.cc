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
 *  Update Date:      $Date: 1998-12-14 17:39:45 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/diovlay.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
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


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

const unsigned int DiOverlay::MaxOverlayCount   = 16;
const unsigned int DiOverlay::FirstOverlayGroup = 0x6000;


/*-----------------------------------*
 *  con-/destructor of helper class  *
 *-----------------------------------*/

DiOverlayData::DiOverlayData(unsigned int entries,
                             unsigned int count)
  : Count(count),
    ArrayEntries(entries),
    Planes(NULL),
    Data(NULL)
{
    if ((entries > 0) && (entries <= DiOverlay::MaxOverlayCount))
    {
        Planes = new DiOverlayPlane *[entries];
        OFBitmanipTemplate<DiOverlayPlane *>::zeroMem(Planes, entries);
    }
}


DiOverlayData::~DiOverlayData()
{
    if (Planes != NULL)
    {
        register unsigned int i;
        for (i = 0; i < ArrayEntries; i++)
            delete Planes[i];
    }
    delete[] Planes;
    delete[] Data;
}


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
    AdditionalPlanes(docu == NULL),                                                    // planes are added later
    Data(NULL)
{
    Data = new DiOverlayData(MaxOverlayCount);                                         // can't determine number of overlays :-(
    if ((docu != NULL) && (Data != NULL) && (Data->Planes != NULL))
    {
        register unsigned int i; 
        for (i = 0; i < MaxOverlayCount; i++)
        {
            Data->Planes[Data->Count] = new DiOverlayPlane(docu, convertToGroupNumber(i), alloc);
            if (Data->Planes[Data->Count] != NULL)
            {
                if (checkPlane(Data->Count))
                    (Data->Count)++;
                else {                
                    delete Data->Planes[Data->Count];
                    Data->Planes[Data->Count] = NULL;
                }
            }
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
    AdditionalPlanes(overlay->AdditionalPlanes),
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
    AdditionalPlanes(overlay->AdditionalPlanes),
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
    AdditionalPlanes(overlay->AdditionalPlanes),
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
    if ((overlay != NULL) && (overlay->Data != NULL) && (overlay->Data->Count > 0))
    {
        if (AdditionalPlanes)
            Data = new DiOverlayData(overlay->Data->ArrayEntries);      // use same array size
        else
            Data = new DiOverlayData(overlay->Data->Count);             // shrink array size to minimal size
        const unsigned long count = (unsigned long)overlay->Width * (unsigned long)overlay->Height * overlay->Frames;
        if ((Data != NULL) && (Data->Planes != NULL) && (count > 0))
        {
            register unsigned int i;
            Data->Data = new Uint16[(unsigned long)Width * (unsigned long)Height * Frames];
            if (Data->Data != NULL)
            {
                Uint16 *temp = NULL;
                if (overlay->Data->Data == NULL)                    // no data buffer
                    temp = new Uint16[count];                       // create temporary buffer
                for (i = 0; i < Data->ArrayEntries; i++)
                {
                    if ((overlay->Data->Planes[i] != NULL) && (overlay->Data->Planes[i]->isValid()))
                    {
                        Data->Planes[i] = new DiOverlayPlane(overlay->Data->Planes[i], convertToGroupNumber(i), Data->Data, temp, overlay->Width,
                            overlay->Height, Width, Height, xfactor, yfactor);
                        (Data->Count)++;
                    }
                }
                if (Data->Count != overlay->Data->Count)            // assertion!
                {
                    if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                        cerr << "WARNING: different number of overlay planes for scaled and unscaled image !" << endl;
                }
                if (overlay->Data->Data != NULL)                    // existing data buffer
                    temp = overlay->Data->Data;                     // point to input buffer
                return temp;
            }
        }
    }
    return NULL;
}


int DiOverlay::convertToPlaneNumber(unsigned int &plane,
                                    const int mode) const
{
    if ((Data != NULL) && (Data->Planes != NULL))
    {
        if (isValidGroupNumber(plane))
        {
            if (AdditionalPlanes)
            {
                plane = (plane - FirstOverlayGroup) >> 1;                               // plane = (group - 0x6000) / 2
                return 2;
            } else {
                register unsigned int i; 
                for (i = 0; i < Data->Count; i++)
                {
                    if ((Data->Planes[i] != NULL) && (Data->Planes[i]->getGroupNumber() == plane))
                    {
                        plane = i;                                                      // new plane number
                        return 2;
                    }
                }
            }
        } else if (!mode && (plane < Data->Count) && (Data->Planes[plane] != NULL))     // valid plane number?
            return 1;
    }
    return 0;
}


int DiOverlay::isValidGroupNumber(const unsigned int group) const
{
    return (group >= convertToGroupNumber(0)) && (group <= convertToGroupNumber(MaxOverlayCount - 1)) && !(group & 1);
}


int DiOverlay::checkPlane(const unsigned int plane,
                          const int mode)
{
    if ((Data != NULL) && (Data->Planes != NULL) && (plane < MaxOverlayCount) && (Data->Planes[plane] != NULL))
    {
        if (Data->Planes[plane]->isValid())
        {
            if (Data->Planes[plane]->getWidth() > Width)                              // determine maximum width
                Width = Data->Planes[plane]->getWidth();
            if (Data->Planes[plane]->getHeight() > Height)                            // determine maximum height
                Height = Data->Planes[plane]->getHeight();
            if (mode && (Data->Planes[plane]->getNumberOfFrames() > Frames))          // determine maximum frames
                Frames = Data->Planes[plane]->getNumberOfFrames();
            return 1;
        }
    }
    return 0;
}

int DiOverlay::showPlane(unsigned int plane)
{
    if (convertToPlaneNumber(plane, AdditionalPlanes))
    {
        Data->Planes[plane]->show();
        return 1;
    }
    return 0; 
}


int DiOverlay::showPlane(unsigned int plane,
                         const double fore,
                         const double tresh,
                         const EM_Overlay mode)
{
    if (convertToPlaneNumber(plane, AdditionalPlanes))
    {
        Data->Planes[plane]->show(fore, tresh, mode);
        return 1;
    }
    return 0;
}


int DiOverlay::showAllPlanes()
{
    if ((Data != NULL) && (Data->Planes != NULL) && !AdditionalPlanes)
    {
        register unsigned int i; 
        for (i = 0; i < Data->Count; i++)
            Data->Planes[i]->show();
        return (Data->Count > 0);
    }
    return 0;
}


int DiOverlay::showAllPlanes(const double fore,
                             const double tresh,
                             const EM_Overlay mode)
{
    if ((Data != NULL) && (Data->Planes != NULL) && !AdditionalPlanes)
    {
        register unsigned int i; 
        for (i = 0; i < Data->Count; i++)
            Data->Planes[i]->show(fore, tresh, mode);
        return (Data->Count > 0);
    }
    return 0;
}


int DiOverlay::hidePlane(unsigned int plane)
{
    if (convertToPlaneNumber(plane, AdditionalPlanes))
    {
        Data->Planes[plane]->hide();
        return 1;
    }
    return 0;
}


int DiOverlay::hideAllPlanes()
{
    if ((Data != NULL) && (Data->Planes != NULL) && !AdditionalPlanes)
    {
        register unsigned int i; 
        for (i = 0; i < Data->Count; i++)
            Data->Planes[i]->hide();
        return (Data->Count > 0);
    }
    return 0;
}


int DiOverlay::placePlane(unsigned int plane,
                          const signed int left,
                          const signed int top)
{
    if (convertToPlaneNumber(plane, AdditionalPlanes))
    {
        Data->Planes[plane]->place(left, top);
        return 1;
    }
    return 0;
}


const char *DiOverlay::getPlaneLabel(unsigned int plane) const
{
    if (convertToPlaneNumber(plane, AdditionalPlanes))
        return Data->Planes[plane]->getLabel();
    return NULL;
}


const char *DiOverlay::getPlaneDescription(unsigned int plane) const
{
    if (convertToPlaneNumber(plane, AdditionalPlanes))
        return Data->Planes[plane]->getDescription();
    return NULL;
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


int DiOverlay::addPlane(const unsigned int group,
                        const unsigned long rows,
                        const unsigned long columns,
                        const EM_Overlay mode,
                        const signed int left,
                        const signed int top,
                        const DcmOverlayData &data,
                        const DcmLongString &label,
                        const DcmLongString &description)
{
    int status = 0;
    if (AdditionalPlanes && isValidGroupNumber(group))
    {
        unsigned int plane = group;
        if (convertToPlaneNumber(plane, AdditionalPlanes))                      // group number already exists
        {
            delete Data->Planes[plane];
            Data->Planes[plane] = new DiOverlayPlane(group, rows, columns, mode, left, top, data, label, description);
            status = 2;
        }
        else if ((Data != NULL) && (Data->Planes != NULL) && (Data->Count < DiOverlay::MaxOverlayCount))
        {                                                                      // add new plane
            plane = Data->Count;
            Data->Planes[plane] = new DiOverlayPlane(group, rows, columns, mode, left, top, data, label, description);
            (Data->Count)++;
            status = 1;
        }
        if ((status != 0) && !checkPlane(plane, 0))
        {
            delete Data->Planes[plane];                                        // remove invalid plane
            Data->Planes[plane] = NULL;
            if (status == 1)
                (Data->Count)--;                                               // decrease number of planes
            status = 0;
        }
    }
    return status;
}
                 

int DiOverlay::removePlane(const unsigned int group)
{
    unsigned int plane = group;
    if (AdditionalPlanes && convertToPlaneNumber(plane, AdditionalPlanes))
    {
        delete Data->Planes[plane];                                           // remove invalid plane
        Data->Planes[plane] = NULL;
        (Data->Count)--;                                                      // decrease number of planes
    }
    return 0;
}


/*
**
** CVS/RCS Log:
** $Log: diovlay.cc,v $
** Revision 1.2  1998-12-14 17:39:45  joergr
** Added methods to add and remove additional overlay planes (still untested).
**
** Revision 1.1  1998/11/27 16:19:25  joergr
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
