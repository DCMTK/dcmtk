/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:18 $
 *  CVS/RCS Revision: $Revision: 1.30 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
        register unsigned int i;
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
        register unsigned int i;
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
        register unsigned int i;
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
        register unsigned int i;
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
            register unsigned int i;
            Data->DataBuffer = new Uint16[OFstatic_cast(unsigned long, Width) * OFstatic_cast(unsigned long, Height) * Frames];
            if (Data->DataBuffer != NULL)
            {
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
                        ++(Data->Count);
                    }
                }
                if (Data->Count != overlay->Data->Count)            // assertion!
                {
                    DCMIMGLE_WARN("different number of overlay planes for scaled and unscaled image");
                }
                if (overlay->Data->DataBuffer != NULL)              // existing data buffer
                    temp = overlay->Data->DataBuffer;               // point to input buffer
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
                if (Data->Planes[plane] != NULL)
                    return 2;                                                           // plane alreay exists
                return 1;                                                               // ... is new
            } else {
                register unsigned int i;
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
        register unsigned int i;
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
        register unsigned int i;
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
        register unsigned int i;
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
        register unsigned int i;
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
            Data->Planes[plane] = new DiOverlayPlane(group, left_pos, top_pos, columns, rows, data, label, description, mode);
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
            return op->getData(frame, 0, 0, width, height, bits, fore, back);
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


/*
 *
 * CVS/RCS Log:
 * $Log: diovlay.cc,v $
 * Revision 1.30  2010-10-14 13:14:18  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.29  2009-10-28 14:26:02  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.28  2009-10-28 09:53:41  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.27  2008-11-18 10:57:10  joergr
 * Fixed issue with incorrectly encoded overlay planes (wrong values for
 * OverlayBitsAllocated and OverlayBitPosition).
 *
 * Revision 1.26  2006/08/15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.25  2005/12/08 15:43:04  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.24  2003/12/23 16:03:18  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.23  2003/12/08 15:02:33  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.22  2002/12/09 13:34:52  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.21  2001/09/28 13:17:57  joergr
 * Added method to extract embedded overlay planes from pixel data and store
 * them in group (6xxx,3000) format.
 *
 * Revision 1.20  2001/05/22 13:20:27  joergr
 * Enhanced checking routines for corrupt overlay data (e.g. invalid value for
 * OverlayBitsAllocated).
 *
 * Revision 1.19  2001/05/14 09:50:25  joergr
 * Added support for "1 bit output" of overlay planes; useful to extract
 * overlay planes from the pixel data and store them separately in the dataset.
 *
 * Revision 1.18  2000/04/28 12:33:47  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.17  2000/04/27 13:10:31  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.16  2000/03/08 16:24:32  meichel
 * Updated copyright header.
 *
 * Revision 1.15  2000/03/03 14:09:21  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.14  1999/10/20 10:35:56  joergr
 * Enhanced method getOverlayData to support 12 bit data for print.
 *
 * Revision 1.13  1999/09/17 13:18:30  joergr
 * Splitted file diovlay.h into two files (one for each class).
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.12  1999/08/25 16:43:09  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.11  1999/04/28 15:04:48  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.10  1999/03/24 17:24:07  joergr
 * Removed bug in routines rotating and flipping overlay planes in clipped
 * images.
 *
 * Revision 1.9  1999/03/22 08:57:44  joergr
 * Added parameter to specify (transparent) background color for method
 * getOverlayData().
 * Removed bug concerning the rotation and flipping of additional overlay
 * planes (NumberOfFrames has not always been determined correctly).
 *
 * Revision 1.8  1999/02/08 13:10:00  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.7  1999/02/03 17:42:30  joergr
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 * Revision 1.6  1998/12/23 13:22:24  joergr
 * Changed parameter type (long to int) to avoid warning reported by MSVC5.
 *
 * Revision 1.5  1998/12/23 11:27:09  joergr
 * Modified (added/removed) comments. Corrected bug concerning additional
 * overlay planes.
 *
 * Revision 1.4  1998/12/22 13:47:17  joergr
 * Added method to check whether plane is visible, to get plane mode and to
 * remove all planes. Set 'value' used for getOverlay/PlaneData().
 * Change meaning of return values (differentiate between different value for
 * 'true').
 *
 * Revision 1.3  1998/12/16 16:18:36  joergr
 * Added method to export overlay planes (create 8-bit bitmap).
 * Implemented flipping and rotation of overlay planes.
 *
 * Revision 1.2  1998/12/14 17:39:45  joergr
 * Added methods to add and remove additional overlay planes (still untested).
 *
 * Revision 1.1  1998/11/27 16:19:25  joergr
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 * Replaced delete by delete[] for array types.
 * Added method to detach pixel data if it is no longer needed.
 * Introduced global debug level for dcmimage module to control error output.
 *
 * Revision 1.5  1998/05/11 14:52:34  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
