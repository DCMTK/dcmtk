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
 *  Purpose: DicomOverlayPlane (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/ofstd/ofbmanip.h"
#include "dcmtk/ofstd/ofutil.h"

#include "dcmtk/dcmimgle/diovpln.h"
#include "dcmtk/dcmimgle/didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiOverlayPlane::DiOverlayPlane(const DiDocument *docu,
                               const unsigned int group,
                               Uint16 alloc,
                               const Uint16 stored,
                               const Uint16 high)
  : NumberOfFrames(0),
    ImageFrameOrigin(0),
    FirstFrame(0),
    Top(0),
    Left(0),
    Height(0),
    Width(0),
    Rows(0),
    Columns(0),
    BitsAllocated(0),
    BitPosition(0),
    Foreground(1),
    Threshold(1),
    PValue(0),
    Mode(EMO_Graphic),
    DefaultMode(EMO_Graphic),
    Label(),
    Description(),
    GroupNumber(OFstatic_cast(Uint16, group)),
    Valid(0),
    Visible(0),
    BitPos(0),
    StartBitPos(0),
    StartLeft(0),
    StartTop(0),
    MultiframeOverlay(0),
    EmbeddedData(0),
    Ptr(NULL),
    StartPtr(NULL),
    Data(NULL)
{
    if (docu != NULL)
    {
        /* determine first frame to be processed */
        FirstFrame = docu->getFrameStart();
        /* specify overlay group number */
        DcmTagKey tag(OFstatic_cast(Uint16, group), DCM_OverlayRows.getElement() /* dummy */);
        /* get descriptive data */
        tag.setElement(DCM_OverlayLabel.getElement());
        docu->getValue(tag, Label);
        tag.setElement(DCM_OverlayDescription.getElement());
        docu->getValue(tag, Description);
        /* get overlay type */
        tag.setElement(DCM_OverlayType.getElement());
        const char *str;
        if ((docu->getValue(tag, str) > 0) && (str != NULL) && (strcmp(str, "R") == 0))
            DefaultMode = Mode = EMO_RegionOfInterest;
        Sint32 sl = 0;
        /* multi-frame overlays */
        tag.setElement(DCM_NumberOfFramesInOverlay.getElement());
        MultiframeOverlay = (docu->search(tag) != NULL);
        docu->getValue(tag, sl);
        NumberOfFrames = (sl < 1) ? 1 : OFstatic_cast(Uint32, sl);
        tag.setElement(DCM_ImageFrameOrigin.getElement());
        docu->getValue(tag, ImageFrameOrigin);
        if (ImageFrameOrigin > 0)                                   // image frame origin is numbered from 1
            --ImageFrameOrigin;
        tag.setElement(DCM_OverlayOrigin.getElement());
#ifdef REVERSE_OVERLAY_ORIGIN_ORDER
        Valid = (docu->getValue(tag, Left, 0) > 0);
        if (Valid)
        {
            DCMIMGLE_DEBUG("processing overlay plane in group 0x" << STD_NAMESPACE hex << group);
            if (MultiframeOverlay)
                DCMIMGLE_TRACE("  this is a multi-frame overlay with " << NumberOfFrames << " frame(s) starting at frame " << (ImageFrameOrigin + 1));
            if (docu->getValue(tag, Top, 1) < 2)
                DCMIMGLE_WARN("missing second value for 'OverlayOrigin' ... assuming 'Top' = " << Top);
        }
#else
        Valid = (docu->getValue(tag, Top, 0) > 0);
        if (Valid)
        {
            DCMIMGLE_DEBUG("processing overlay plane in group 0x" << STD_NAMESPACE hex << group);
            if (MultiframeOverlay)
                DCMIMGLE_TRACE("  this is a multi-frame overlay with " << NumberOfFrames << " frame(s) starting at frame " << (ImageFrameOrigin + 1));
            if (docu->getValue(tag, Left, 1) < 2)
                DCMIMGLE_WARN("missing second value for 'OverlayOrigin' ... assuming 'Left' = " << Left);
        }
#endif
        /* overlay origin is numbered from 1 */
        --Top;
        --Left;
        /* check overlay resolution */
        tag.setElement(DCM_OverlayRows.getElement());
        Valid &= (docu->getValue(tag, Rows) > 0);
        Height = Rows;
        tag.setElement(DCM_OverlayColumns.getElement());
        Valid &= (docu->getValue(tag, Columns) > 0);
        Width = Columns;
        /* check overlay encoding */
        tag.setElement(DCM_OverlayBitsAllocated.getElement());
        Valid &= (docu->getValue(tag, BitsAllocated) > 0);
        tag.setElement(DCM_OverlayBitPosition.getElement());
        Valid &= (docu->getValue(tag, BitPosition) > 0);
        tag.setElement(DCM_OverlayData.getElement());
        /* final validity checks */
        if (Valid)
        {
            /* separate overlay data? */
            unsigned long length = docu->getValue(tag, Data) * 2 /* bytes */;
            if (length == 0)
            {
                if (!(docu->getFlags() & CIF_NeverAccessEmbeddedOverlays))
                {
                    if (!docu->isCompressed())
                    {
                        /* if not, check for embedded overlay data */
                        DcmPixelData *pixelData = docu->getPixelData();
                        if (pixelData != NULL)
                        {
                            ImageFrameOrigin = 0;                           // see supplement 4
                            const OFBool loaded = pixelData->valueLoaded();
                            if (pixelData->getUint16Array(OFconst_cast(Uint16 *&, Data)).good())
                            {
                                length = pixelData->getLength(docu->getTransferSyntax());
                                EmbeddedData = (Data != NULL);
                                if (!loaded)
                                    DCMIMGLE_DEBUG("loaded complete pixel data into memory for embedded overlay data: " << length << " bytes");
                            }
                        }
                    } else
                        DCMIMGLE_ERROR("embedded overlay data cannot be accessed since pixel data is still compressed");
                } else
                    DCMIMGLE_WARN("ignoring possibly embedded overlay data by configuration");
            } else
                alloc = 1;                                          // separately stored overlay data
            /* check for correct value of BitsAllocated */
            if (BitsAllocated != alloc)                             // see correction proposal 87
            {
                DCMIMGLE_WARN("invalid value for 'OverlayBitsAllocated' (" << BitsAllocated << ") ... assuming " << alloc);
                BitsAllocated = alloc;
            }
            /* check for correct value of BitPosition */
            if (BitPosition >= BitsAllocated)
            {
                DCMIMGLE_WARN("invalid value for 'OverlayBitPosition' (" << BitPosition << ") ... assuming " << (BitsAllocated - 1));
                BitPosition = BitsAllocated - 1;
            }
            if (EmbeddedData && (BitPosition <= high) && (BitPosition + stored > high))
            {
                DCMIMGLE_WARN("invalid value for 'OverlayBitPosition' (" << BitPosition << "), refers to bit position within stored pixel value");
                Data = NULL;    // invalid plane
            }
            /* expected length of overlay data */
            const unsigned long expLen = (OFstatic_cast(unsigned long, NumberOfFrames) * OFstatic_cast(unsigned long, Rows) *
                                          OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, BitsAllocated) + 7) / 8;
            if ((Data != NULL) && ((length == 0) || (length < expLen)))
            {
                DCMIMGLE_ERROR("overlay data length is too short, " << expLen << " bytes expected but " << length << " bytes found");
                Valid = 0;
                Data = NULL;
            } else
                Valid = (Data != NULL);
            /* check condition for multi-frame overlay */
            if (NumberOfFrames > 1)
            {
                Sint32 numFrames = 0;
                if (!docu->getValue(DCM_NumberOfFrames, numFrames) || (numFrames == 1))
                    DCMIMGLE_WARN("found multi-frame overlay in group 0x" << STD_NAMESPACE hex << group << " for single frame image");
            }
        }
        if (Valid)
        {
            /* report that this group contains a valid overlay plane */
            DCMIMGLE_TRACE("overlay plane in group 0x" << STD_NAMESPACE hex << group << " is present and can be processed");
        } else {
            /* report that this group does not contain a valid overlay plane */
            DCMIMGLE_TRACE("overlay plane in group 0x" << STD_NAMESPACE hex << group << " is missing or incomplete");
        }
    }
}


DiOverlayPlane::DiOverlayPlane(const unsigned int group,
                               const Sint16 left_pos,
                               const Sint16 top_pos,
                               const Uint16 columns,
                               const Uint16 rows,
                               const DcmOverlayData &data,
                               const DcmLongString &label,
                               const DcmLongString &description,
                               const EM_Overlay mode)
  : NumberOfFrames(1),
    ImageFrameOrigin(0),
    FirstFrame(0),
    Top(top_pos),
    Left(left_pos),
    Height(rows),
    Width(columns),
    Rows(rows),
    Columns(columns),
    BitsAllocated(1),
    BitPosition(0),
    Foreground(1),
    Threshold(1),
    PValue(0),
    Mode(mode),
    DefaultMode(mode),
    Label(),
    Description(),
    GroupNumber(OFstatic_cast(Uint16, group)),
    Valid(0),
    Visible((mode == EMO_BitmapShutter) ? 1 : 0),
    BitPos(0),
    StartBitPos(0),
    StartLeft(0),
    StartTop(0),
    MultiframeOverlay(0),
    EmbeddedData(0),
    Ptr(NULL),
    StartPtr(NULL),
    Data(NULL)
{
    DiDocument::getElemValue(OFreinterpret_cast(const DcmElement *, &label), Label);
    DiDocument::getElemValue(OFreinterpret_cast(const DcmElement *, &description), Description);
    if ((Columns > 0) && (Rows > 0))
    {
        const unsigned long length = DiDocument::getElemValue(OFreinterpret_cast(const DcmElement *, &data), Data) * 2 /* Bytes */;
        /* expected length of overlay data */
        const unsigned long expLen = (OFstatic_cast(unsigned long, Rows) * OFstatic_cast(unsigned long, Columns) + 7) / 8;
        if ((length == 0) || (length < expLen))
        {
            DCMIMGLE_ERROR("overlay data length is too short, " << expLen << " bytes expected but " << length << " bytes found");
            /* Valid = 0;  =>  This is the default. */
            Data = NULL;
        } else
            Valid = (Data != NULL);
    }
    --Top;                                                      // overlay origin is numbered from 1
    --Left;
}


DiOverlayPlane::DiOverlayPlane(DiOverlayPlane *plane,
                               const unsigned int bit,
                               const Uint16 *data,
                               Uint16 *temp,
                               const Uint16 width,
                               const Uint16 height,
                               const Uint16 columns,
                               const Uint16 rows)
  : NumberOfFrames(plane->NumberOfFrames),
    ImageFrameOrigin(plane->ImageFrameOrigin),
    FirstFrame(plane->FirstFrame),
    Top(plane->Top),
    Left(plane->Left),
    Height(plane->Height),
    Width(plane->Width),
    Rows(rows),
    Columns(columns),
    BitsAllocated(16),
    BitPosition(OFstatic_cast(Uint16, bit)),
    Foreground(plane->Foreground),
    Threshold(plane->Threshold),
    PValue(0),
    Mode(plane->Mode),
    DefaultMode(plane->DefaultMode),
    Label(plane->Label),
    Description(plane->Description),
    GroupNumber(plane->GroupNumber),
    Valid(0),
    Visible(plane->Visible),
    BitPos(0),
    StartBitPos(0),
    StartLeft(plane->StartLeft),
    StartTop(plane->StartTop),
    MultiframeOverlay(plane->MultiframeOverlay),
    EmbeddedData(0),
    Ptr(NULL),
    StartPtr(NULL),
    Data(data)
{
    if (temp != NULL)
    {
        Uint16 x;
        Uint16 y;
        Uint16 *q = temp;
        const Uint16 mask = 1 << bit;
        const Uint16 skip_x = width - plane->Columns;
        const unsigned long skip_f = OFstatic_cast(unsigned long, height - plane->Rows) * OFstatic_cast(unsigned long, width);
        for (unsigned long f = 0; f < NumberOfFrames; ++f)
        {
            if (plane->reset(f + ImageFrameOrigin))
            {
                for (y = 0; y < plane->Rows; ++y)
                {
                    for (x = 0; x < plane->Columns; ++x, ++q)
                    {
                        if (plane->getNextBit())
                            *q |= mask;                         // set corresponding bit
                        else
                            *q &= ~mask;                        // unset ... bit
                    }
                    q += skip_x;                                // skip to next line start
                }
                q += skip_f;                                    // skip to next frame start
            }
        }
    }
    Valid = (Data != NULL);
}


/*--------------*
 *  destructor  *
 *--------------*/

DiOverlayPlane::~DiOverlayPlane()
{
}


/********************************************************************/


void *DiOverlayPlane::getData(const unsigned long frame,
                              const Uint16 xmin,
                              const Uint16 ymin,
                              const Uint16 xmax,
                              const Uint16 ymax,
                              const int bits,
                              const Uint16 fore,
                              const Uint16 back,
                              const OFBool useOrigin)
{
    const unsigned long count = OFstatic_cast(unsigned long, xmax - xmin) * OFstatic_cast(unsigned long, ymax - ymin);
    if (Valid && (count > 0))
    {
        const Uint16 mask = OFstatic_cast(Uint16, DicomImageClass::maxval(bits));
        if (bits == 1)
        {
            const unsigned long count8 = (count + 7) / 8;           // round value: 8 bit padding
            Uint8 *data = new Uint8[count8];
            if (data != NULL)
            {
                if ((fore & mask) != (back & mask))
                {
                    OFBitmanipTemplate<Uint8>::setMem(data, 0x0, count8);
                    Uint16 x;
                    Uint16 y;
                    Uint8 value = 0;
                    Uint8 *q = data;
                    int bit = 0;
                    if (reset(frame + ImageFrameOrigin))
                    {
                        for (y = ymin; y < ymax; ++y)
                        {
                            setStart(xmin, y, useOrigin);
                            for (x = xmin; x < xmax; ++x)
                            {
                                if (getNextBit())
                                {
                                    if (fore)
                                        value |= (1 << bit);
                                } else if (back)
                                    value |= (1 << bit);
                                if (bit == 7)
                                {
                                    *(q++) = value;
                                    value = 0;
                                    bit = 0;
                                } else {
                                    ++bit;
                                }
                            }
                        }
                        if (bit != 0)
                            *(q++) = value;
                    }
                } else {
                    OFBitmanipTemplate<Uint8>::setMem(data, (fore) ? 0xff : 0x0, count8);
                }
            }
            return OFstatic_cast(void *, data);
        }
        else if ((bits > 1) && (bits <= 8))
        {
            Uint8 *data = new Uint8[count];
            if (data != NULL)
            {
                const Uint8 fore8 = OFstatic_cast(Uint8, fore & mask);
                const Uint8 back8 = OFstatic_cast(Uint8, back & mask);
                OFBitmanipTemplate<Uint8>::setMem(data, back8, count);
                if (fore8 != back8)                                     // optimization
                {
                    Uint16 x;
                    Uint16 y;
                    Uint8 *q = data;
                    if (reset(frame + ImageFrameOrigin))
                    {
                        for (y = ymin; y < ymax; ++y)
                        {
                            setStart(xmin, y, useOrigin);
                            for (x = xmin; x < xmax; ++x, ++q)
                            {
                                if (getNextBit())
                                    *q = fore8;                         // set pixel value (default: 0xff)
                            }
                        }
                    }
                }
            }
            return OFstatic_cast(void *, data);
        }
        else if ((bits > 8) && (bits <= 16))
        {
            Uint16 *data = new Uint16[count];
            if (data != NULL)
            {
                const Uint16 fore16 = fore & mask;
                const Uint16 back16 = back & mask;
                OFBitmanipTemplate<Uint16>::setMem(data, back16, count);
                if (fore16 != back16)                                   // optimization
                {
                    Uint16 x;
                    Uint16 y;
                    Uint16 *q = data;
                    if (reset(frame + ImageFrameOrigin))
                    {
                        for (y = ymin; y < ymax; ++y)
                        {
                            setStart(xmin, y, useOrigin);
                            for (x = xmin; x < xmax; ++x, ++q)
                            {
                                if (getNextBit())
                                    *q = fore16;                        // set pixel value (default: 0xff)
                            }
                        }
                    }
                }
            }
            return OFstatic_cast(void *, data);
        }
    }
    return NULL;
}


unsigned long DiOverlayPlane::create6xxx3000Data(Uint8 *&buffer,
                                                 unsigned int &width,
                                                 unsigned int &height,
                                                 unsigned long &frames)
{
    buffer = NULL;
    width = Width;
    height = Height;
    frames = NumberOfFrames;
    const unsigned long count = OFstatic_cast(unsigned long, Width) * OFstatic_cast(unsigned long, Height) * NumberOfFrames;
    if (Valid && (count > 0))
    {
        const unsigned long count8 = ((count + 15) / 16) * 2;           // round value: 16 bit padding
        buffer = new Uint8[count8];
        if (buffer != NULL)
        {
            OFBitmanipTemplate<Uint8>::setMem(buffer, 0x0, count8);
            Uint16 x;
            Uint16 y;
            Uint8 value = 0;
            Uint8 *q = buffer;
            int bit = 0;
            for (unsigned long f = 0; f < NumberOfFrames; ++f)
            {
                if (reset(f + ImageFrameOrigin))
                {
                    for (y = 0; y < Height; ++y)
                    {
                        for (x = 0; x < Width; ++x)
                        {
                            if (getNextBit())
                                value |= (1 << bit);
                            if (bit == 7)
                            {
                                *(q++) = value;
                                value = 0;
                                bit = 0;
                            } else {
                                ++bit;
                            }
                        }
                    }
                }
                if (bit != 0)
                    *(q++) = value;
            }
            return count8;      // number of bytes
        }
    }
    return 0;
}


void DiOverlayPlane::show(const double fore,
                          const double thresh,
                          const EM_Overlay mode)
{
    Foreground = (fore < 0) ? 0 : (fore > 1) ? 1 : fore;
    Threshold = (thresh < 0) ? 0 : (thresh > 1) ? 1 : thresh;
    Mode = (mode == EMO_Default) ? DefaultMode : mode;
    Visible = 1;
}


int DiOverlayPlane::show(const Uint16 pvalue)
{
    if (Mode == EMO_BitmapShutter)
    {
        PValue = pvalue;
        Visible = 1;
        return 1;
    }
    return 0;
}


void DiOverlayPlane::place(const signed int left_pos,
                           const signed int top_pos)
{
    Left = OFstatic_cast(Sint16, left_pos);
    Top = OFstatic_cast(Sint16, top_pos);
}


void DiOverlayPlane::setScaling(const double xfactor,
                                const double yfactor)
{
    Left = OFstatic_cast(Sint16, xfactor * Left);
    Top = OFstatic_cast(Sint16, yfactor * Top);
    Width = OFstatic_cast(Uint16, xfactor * Width);
    Height = OFstatic_cast(Uint16, yfactor * Height);
    StartLeft = OFstatic_cast(unsigned int, xfactor * StartLeft);
    StartTop = OFstatic_cast(unsigned int, yfactor * StartTop);
}


void DiOverlayPlane::setFlipping(const int horz,
                                 const int vert,
                                 const signed long columns,
                                 const signed long rows)
{
    if (horz)
    {
        Left = OFstatic_cast(Sint16, columns - Width - Left);
        StartLeft = OFstatic_cast(unsigned int, OFstatic_cast(signed long, Columns) - Width - StartLeft);
    }
    if (vert)
    {
        Top = OFstatic_cast(Sint16, rows - Height - Top);
        StartTop = OFstatic_cast(unsigned int, OFstatic_cast(signed long, Rows) - Height - StartTop);
    }
}


void DiOverlayPlane::setRotation(const int degree,
                                 const signed long left_pos,
                                 const signed long top_pos,
                                 const Uint16 columns,
                                 const Uint16 rows)
{
    if (degree == 180)                          // equal to v/h flip
        setFlipping(1, 1, left_pos + columns, top_pos + rows);
    else if ((degree == 90) || (degree == 270))
    {
        OFswap(Width, Height);                  // swap visible width/height
/*
        OFswap(Columns, Rows);                  // swap stored columns/rows -> already done in the constructor !
*/
        if (degree == 90)                       // rotate right
        {
            const Sint16 ss = Left;
            const unsigned int ui = StartLeft;
            Left = OFstatic_cast(Sint16, OFstatic_cast(signed long, columns) - Width - Top + top_pos);
            StartLeft = OFstatic_cast(unsigned int, OFstatic_cast(signed long, Columns) - Width - StartTop);
            Top = OFstatic_cast(Sint16, ss - left_pos);
            StartTop = ui;
        } else {                                // rotate left
            const Sint16 ss = Left;
            const unsigned int ui = StartLeft;
            Left = OFstatic_cast(Sint16, Top - top_pos);
            StartLeft = StartTop;
            Top = OFstatic_cast(Sint16, OFstatic_cast(signed long, rows) - Height - ss + left_pos);
            StartTop = OFstatic_cast(unsigned int, OFstatic_cast(signed long, Rows) - Height - ui);
        }
    }
}
