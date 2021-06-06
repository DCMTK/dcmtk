/*
 *
 *  Copyright (C) 1996-2021, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomOverlay (Source)
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctypes.h"

#include "dcmtk/dcmimgle/diovlay.h"
#include "dcmtk/dcmimgle/diovdat.h"
#include "dcmtk/dcmimgle/diovpln.h"
#include "dcmtk/dcmimgle/discalet.h"
#include "dcmtk/dcmimgle/diflipt.h"
#include "dcmtk/dcmimgle/dirotat.h"
#include "dcmtk/dcmimgle/didocu.h"


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

const unsigned int DiOverlay::MaxOverlayCount   = 16;
const unsigned int DiOverlay::FirstOverlayGroup = 0x6000;


/*----------------*
 *  constructors  *
 *----------------*/

DiOverlay::DiOverlay(const DiDocument *docu,
                     const Uint16 alloc,
                     const Uint16 stored,
                     const Uint16 high)
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
        unsigned int i;
        for (i = 0; i < MaxOverlayCount; ++i)
        {
            Data->Planes[Data->Count] = new DiOverlayPlane(docu, convertToGroupNumber(i), alloc, stored, high);
            if (Data->Planes[Data->Count] != NULL)
            {
                if (checkPlane(Data->Count))
                    ++(Data->Count);
                else {
                    delete Data->Planes[Data->Count];
                    Data->Planes[Data->Count] = NULL;
                }
            }
        }
    }
}


// --- scale/clip overlay

DiOverlay::DiOverlay(const DiOverlay *overlay,
                     const signed long left_pos,
                     const signed long top_pos,
                     const double xfactor,
                     const double yfactor)
  : Left(left_pos),
    Top(top_pos),
    Width(OFstatic_cast(Uint16, xfactor * overlay->Width)),
    Height(OFstatic_cast(Uint16, yfactor * overlay->Height)),
    Frames(overlay->Frames),
    AdditionalPlanes(overlay->AdditionalPlanes),
    Data(NULL)
{
    Uint16 *temp = Init(overlay);
    if (temp != NULL)
    {
        unsigned int i;
        for (i = 0; i < Data->ArrayEntries; ++i)
        {
            if (Data->Planes[i] != NULL)
                Data->Planes[i]->setScaling(xfactor, yfactor);
        }
        DiScaleTemplate<Uint16> scale(1, overlay->Width, overlay->Height, Width, Height, Frames);
        scale.scaleData(OFconst_cast(const Uint16 **, &temp), &(Data->DataBuffer), 0);
        if (temp != overlay->Data->DataBuffer)
            delete[] temp;
    }
}


// --- flip overlay

DiOverlay::DiOverlay(const DiOverlay *overlay,
                     const int horz,
                     const int vert,
                     const Uint16 columns,              // width of surrounding image
                     const Uint16 rows)
  : Left((horz) ? 0 : overlay->Left),
    Top((vert) ? 0 : overlay->Top),
    Width(overlay->Width),
    Height(overlay->Height),
    Frames(overlay->Frames),
    AdditionalPlanes(overlay->AdditionalPlanes),
    Data(NULL)
{
    Uint16 *temp = Init(overlay);
    if (temp != NULL)
    {
        DiFlipTemplate<Uint16> flip(1, Width, Height, Frames);
        flip.flipData(OFconst_cast(const Uint16 **, &temp), &(Data->DataBuffer), horz, vert);
        if (temp != overlay->Data->DataBuffer)
            delete[] temp;
        unsigned int i;
        for (i = 0; i < Data->ArrayEntries; ++i)
        {
            if (Data->Planes[i] != NULL)
            {
                Data->Planes[i]->setFlipping(horz, vert, OFstatic_cast(signed long, columns) + overlay->Left,
                    OFstatic_cast(signed long, rows) + overlay->Top);
            }
        }
    }
}


// --- rotate overlay

DiOverlay::DiOverlay(const DiOverlay *overlay,
                     const int degree,
                     const Uint16 columns,              // width of surrounding image (already rotated)
                     const Uint16 rows)
  : Left(0),
    Top(0),
    Width(((degree == 90) || (degree == 270)) ? overlay->Height : overlay->Width),
    Height(((degree == 90) || (degree == 270)) ? overlay->Width : overlay->Height),
    Frames(overlay->Frames),
    AdditionalPlanes(overlay->AdditionalPlanes),
    Data(NULL)
{
    Uint16 *temp = Init(overlay);
    if (temp != NULL)
    {
        DiRotateTemplate<Uint16> rotate(1, overlay->Width, overlay->Height, Width, Height, Frames);
        rotate.rotateData(OFconst_cast(const Uint16 **, &temp), &(Data->DataBuffer), degree);
        if (temp != overlay->Data->DataBuffer)
            delete[] temp;
        unsigned int i;
        for (i = 0; i < Data->ArrayEntries; ++i)
        {
            if (Data->Planes[i] != NULL)
                Data->Planes[i]->setRotation(degree, overlay->Left, overlay->Top, columns, rows);
        }
    }
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


Uint16 *DiOverlay::Init(const DiOverlay *overlay)
{
    if ((overlay != NULL) && (overlay->Data != NULL) && (overlay->Data->Count > 0))
    {
        if (AdditionalPlanes)
            Data = new DiOverlayData(overlay->Data->ArrayEntries);      // use same array size
        else
            Data = new DiOverlayData(overlay->Data->Count);             // shrink array size to minimal size
        const unsigned long count = OFstatic_cast(unsigned long, overlay->Width) *
            OFstatic_cast(unsigned long, overlay->Height) * overlay->Frames;
        if ((Data != NULL) && (Data->Planes != NULL) && (count > 0))
        {
            const unsigned long bufSize = OFstatic_cast(unsigned long, Width) *
                OFstatic_cast(unsigned long, Height) * Frames;
            if (bufSize > 0)                                            // avoid invalid buffer
            {
                Data->DataBuffer = new Uint16[bufSize];
                if (Data->DataBuffer != NULL)
                {
                    unsigned int i;
                    Uint16 *temp = NULL;
                    if (overlay->Data->DataBuffer == NULL)              // no data buffer
                    {
                        temp = new Uint16[count];                       // create temporary buffer
                        if (temp != NULL)
                            OFBitmanipTemplate<Uint16>::zeroMem(temp, count);
                    }
                    for (i = 0; i < Data->ArrayEntries; ++i)
                    {
                        if ((overlay->Data->Planes[i] != NULL) /*&& (overlay->Data->Planes[i]->isValid())*/)
                        {
                            Data->Planes[i] = new DiOverlayPlane(overlay->Data->Planes[i], i, Data->DataBuffer, temp,
                                overlay->Width, overlay->Height, Width, Height);
                            ++(Data->Count);                            // increase number of valid planes
                        }
                    }
                    if (Data->Count != overlay->Data->Count)            // assertion!
                    {
                        DCMIMGLE_WARN("different number of overlay planes for converted and original image");
                    }
                    if (overlay->Data->DataBuffer != NULL)              // existing data buffer
                        temp = overlay->Data->DataBuffer;               // point to input buffer
                    return temp;
                }
            } else {
                DCMIMGLE_DEBUG("skipping overlay planes for converted image ... calculated buffer size is 0");
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
                if (Data->Planes[plane] != NULL)
                    return 2;                                                           // plane already exists
                return 1;                                                               // ... is new
            } else {
                unsigned int i;
                for (i = 0; i < Data->Count; ++i)
                {
                    if ((Data->Planes[i] != NULL) && (Data->Planes[i]->getGroupNumber() == plane))
                    {
                        plane = i;                                                      // plane number
                        return 2;
                    }
                }
            }
        } else if (!mode && (plane < Data->Count) && (Data->Planes[plane] != NULL))     // valid plane number?
            return 3;
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


int DiOverlay::isPlaneVisible(unsigned int plane)
{
    if (convertToPlaneNumber(plane, AdditionalPlanes) > 1)
        return Data->Planes[plane]->isVisible();
    return 0;
}


int DiOverlay::showPlane(unsigned int plane)
{
    if (convertToPlaneNumber(plane, AdditionalPlanes) > 1)
    {
        if (Data->Planes[plane]->isVisible())
            return 2;
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
    if (convertToPlaneNumber(plane, AdditionalPlanes) > 1)
    {
        Data->Planes[plane]->show(fore, tresh, mode);
        return 1;
    }
    return 0;
}


int DiOverlay::showPlane(unsigned int plane,
                         const Uint16 pvalue)
{
    if (convertToPlaneNumber(plane, AdditionalPlanes) > 1)
        return Data->Planes[plane]->show(pvalue);
    return 0;
}


int DiOverlay::showAllPlanes()
{
    if ((Data != NULL) && (Data->Planes != NULL))
    {
        unsigned int i;
        for (i = 0; i < Data->ArrayEntries; ++i)
        {
            if (Data->Planes[i] != NULL)
                Data->Planes[i]->show();
        }
        if (Data->Count > 0)
            return 1;
        return 2;
    }
    return 0;
}


int DiOverlay::showAllPlanes(const double fore,
                             const double tresh,
                             const EM_Overlay mode)
{
    if ((Data != NULL) && (Data->Planes != NULL))
    {
        unsigned int i;
        for (i = 0; i < Data->ArrayEntries; ++i)
        {
            if ((Data->Planes[i] != NULL))
                Data->Planes[i]->show(fore, tresh, mode);
        }
        if (Data->Count > 0)
            return 1;
        return 2;
    }
    return 0;
}


int DiOverlay::hidePlane(unsigned int plane)
{
    if (convertToPlaneNumber(plane, AdditionalPlanes) > 1)
    {
        if (!Data->Planes[plane]->isVisible())
            return 2;
        Data->Planes[plane]->hide();
        return 1;
    }
    return 0;
}


int DiOverlay::hideAllPlanes()
{
    if ((Data != NULL) && (Data->Planes != NULL))
    {
        unsigned int i;
        for (i = 0; i < Data->ArrayEntries; ++i)
        {
            if (Data->Planes[i] != NULL)
                Data->Planes[i]->hide();
        }
        if (Data->Count > 0)
            return 1;
        return 2;
    }
    return 0;
}


int DiOverlay::placePlane(unsigned int plane,
                          const signed int left_pos,
                          const signed int top_pos)
{
    if (convertToPlaneNumber(plane, AdditionalPlanes) > 1)
    {
        if ((Data->Planes[plane]->getLeft() == left_pos) && (Data->Planes[plane]->getTop() == top_pos))
            return 2;
        Data->Planes[plane]->place(left_pos, top_pos);
        return 1;
    }
    return 0;
}


unsigned int DiOverlay::getPlaneGroupNumber(unsigned int plane) const
{
    if (convertToPlaneNumber(plane, AdditionalPlanes) > 1)
        return Data->Planes[plane]->getGroupNumber();
    return 0;
}


const char *DiOverlay::getPlaneLabel(unsigned int plane) const
{
    if (convertToPlaneNumber(plane, AdditionalPlanes) > 1)
        return Data->Planes[plane]->getLabel();
    return NULL;
}


const char *DiOverlay::getPlaneDescription(unsigned int plane) const
{
    if (convertToPlaneNumber(plane, AdditionalPlanes) > 1)
        return Data->Planes[plane]->getDescription();
    return NULL;
}


EM_Overlay DiOverlay::getPlaneMode(unsigned int plane) const
{
    if (convertToPlaneNumber(plane, AdditionalPlanes) > 1)
        return Data->Planes[plane]->getMode();
    return EMO_Default;
}


int DiOverlay::hasEmbeddedData() const
{
    if ((Data != NULL) && (Data->Planes != NULL))
    {
        unsigned int i;
        for (i = 0; i < Data->ArrayEntries; ++i)
        {
            if ((Data->Planes[i] != NULL) && (Data->Planes[i]->isEmbedded()))
                return 1;
        }
    }
    return 0;
}


int DiOverlay::addPlane(const unsigned int group,
                        const signed int left_pos,
                        const signed int top_pos,
                        const unsigned int columns,
                        const unsigned int rows,
                        const DcmOverlayData &data,
                        const DcmLongString &label,
                        const DcmLongString &description,
                        const EM_Overlay mode)
{
    int status = 0;
    if (AdditionalPlanes && isValidGroupNumber(group))
    {
        unsigned int plane = group;
        status = convertToPlaneNumber(plane, AdditionalPlanes);
        if ((status != 0) && (plane < Data->ArrayEntries))
        {
            if (status == 1)                                                   // add new plane
                ++(Data->Count);
            else if (status == 2)                                              // group number already exists
                delete Data->Planes[plane];
            Data->Planes[plane] = new DiOverlayPlane(group, OFstatic_cast(const Sint16, left_pos), 
                OFstatic_cast(const Sint16, top_pos), OFstatic_cast(const Uint16, columns), 
                OFstatic_cast(const Uint16, rows), data, label, description, mode);
            if (checkPlane(plane, 0))
            {
                if (Data->Planes[plane]->getNumberOfFrames() > Frames)         // set maximum number of frames
                    Frames = Data->Planes[plane]->getNumberOfFrames();
            } else {
                delete Data->Planes[plane];                                    // remove invalid plane
                Data->Planes[plane] = NULL;
                if (status == 1)
                    --(Data->Count);                                           // decrease number of planes
                status = 0;
            }
        }
    }
    return status;
}


int DiOverlay::removePlane(const unsigned int group)
{
    unsigned int plane = group;
    if (AdditionalPlanes && (convertToPlaneNumber(plane, AdditionalPlanes) > 1))
    {
        delete Data->Planes[plane];                                           // remove invalid plane
        Data->Planes[plane] = NULL;
        --(Data->Count);                                                      // decrease number of planes
        return 1;
    }
    return 0;
}


void *DiOverlay::getPlaneData(const unsigned long frame,
                              unsigned int plane,
                              unsigned int &left_pos,
                              unsigned int &top_pos,
                              unsigned int &width,
                              unsigned int &height,
                              EM_Overlay &mode,
                              const Uint16 columns,
                              const Uint16 rows,
                              const int bits,
                              const Uint16 fore,
                              const Uint16 back)
{
    if (convertToPlaneNumber(plane, AdditionalPlanes) > 1)                    // plane does exist
    {
        DiOverlayPlane *op = Data->Planes[plane];
        if ((op != NULL) && op->isValid())
        {
            const Uint16 xmin = (op->getLeft(Left) > 0) ? op->getLeft(Left) : 0;
            const Uint16 ymin = (op->getTop(Top) > 0) ? op->getTop(Top) : 0;
            const Uint16 xmax = (op->getRight(Left) < columns) ? op->getRight(Left) : columns;
            const Uint16 ymax = (op->getBottom(Top) < rows) ? op->getBottom(Top) : rows;
            left_pos = xmin;
            top_pos = ymin;
            width = xmax - xmin;
            height = ymax - ymin;
            mode = op->getMode();
            return op->getData(frame, xmin, ymin, xmax, ymax, bits, fore, back);
        }
    }
    return NULL;
}


void *DiOverlay::getFullPlaneData(const unsigned long frame,
                                  unsigned int plane,
                                  unsigned int &width,
                                  unsigned int &height,
                                  const int bits,
                                  const Uint16 fore,
                                  const Uint16 back)
{
    if (convertToPlaneNumber(plane, AdditionalPlanes) > 1)                    // plane does exist
    {
        DiOverlayPlane *op = Data->Planes[plane];
        if ((op != NULL) && op->isValid())
        {
            width = op->getWidth();
            height = op->getHeight();
            return op->getData(frame, 0, 0, OFstatic_cast(Uint16, width), OFstatic_cast(Uint16, height), bits, fore, back, OFFalse /*useOrigin*/);
        }
    }
    return NULL;
}


unsigned long DiOverlay::create6xxx3000PlaneData(Uint8 *&buffer,
                                                 unsigned int plane,
                                                 unsigned int &width,
                                                 unsigned int &height,
                                                 unsigned long &frames)
{
    if (convertToPlaneNumber(plane, AdditionalPlanes) > 1)                    // plane does exist
    {
        DiOverlayPlane *op = Data->Planes[plane];
        if ((op != NULL) && op->isValid())
            return op->create6xxx3000Data(buffer, width, height, frames);
    }
    return 0;
}
