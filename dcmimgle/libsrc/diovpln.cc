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
 *  Purpose: DicomOverlayPlane (Source) - Multiframe Overlays UNTESTED !
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-12-22 13:51:04 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/diovpln.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"
#include "dctagkey.h"
#include "ofbmanip.h"

#include "diovpln.h"
#include "didocu.h"

//#include <string.h>


/*----------------*
 *  constructors  *
 *----------------*/

DiOverlayPlane::DiOverlayPlane(const DiDocument *docu,
                               const unsigned int group,
                               const Uint16 alloc)
  : NumberOfFrames(0),
    ImageFrameOrigin(0),
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
    Mode(EMO_Replace),
    DefaultMode(EMO_Replace),
    Label(),
    Description(),
    GroupNumber(group),
    Valid(0),
    Visible(0),
    BitPos(0),
    StartBitPos(0),
    StartLeft(0),
    StartTop(0),
    EmbeddedData(0),
    Ptr(NULL),
    StartPtr(NULL),
    Data(NULL)
{
    if (docu != NULL)
    {
        DcmTagKey tag(group, DCM_OverlayRows.getElement());
        tag.setElement(DCM_OverlayLabel.getElement());
        docu->getValue(tag, Label);
        tag.setElement(DCM_OverlayDescription.getElement());
        docu->getValue(tag, Description);
        tag.setElement(DCM_OverlayType.getElement());
        const char *str;
        if ((docu->getValue(tag, str) > 0) && (strcmp(str, "R") == 0))
            DefaultMode = Mode = EMO_RegionOfInterest;
        Sint32 sl = 0;
        tag.setElement(DCM_OverlayNumberOfFrames.getElement());
        docu->getValue(tag, sl);
        NumberOfFrames = (sl < 1) ? 1 : (Uint32)sl;
        tag.setElement(DCM_ImageFrameOrigin.getElement());
        docu->getValue(tag, ImageFrameOrigin);
        if (ImageFrameOrigin > 0)                                   // image frame origin is numbered from 1
            ImageFrameOrigin--;
        tag.setElement(DCM_OverlayOrigin.getElement());
#ifdef REVERSE_OVERLAY_ORIGIN_ORDER
        Valid = (docu->getValue(tag, Top, 1) > 0);
        Valid &= (docu->getValue(tag, Left, 0) > 0);
#else
        Valid = (docu->getValue(tag, Top, 0) > 0);
        Valid &= (docu->getValue(tag, Left, 1) > 0);
#endif
        Top--;                                                      // overlay origin is numbered from 1
        Left--;
        tag.setElement(DCM_OverlayRows.getElement());
        Valid &= (docu->getValue(tag, Rows) > 0);
        Height = Rows;
        tag.setElement(DCM_OverlayColumns.getElement());
        Valid &= (docu->getValue(tag, Columns) > 0);
        Width = Columns;
        tag.setElement(DCM_OverlayBitsAllocated.getElement());
        Valid &= (docu->getValue(tag, BitsAllocated) > 0);
        tag.setElement(DCM_OverlayBitPosition.getElement());
        Valid &= (docu->getValue(tag, BitPosition) > 0);
        tag.setElement(DCM_OverlayData.getElement());
        if (Valid && !docu->getValue(tag, Data))
        {
            ImageFrameOrigin = 0;                                   // see supplement 4
            if (BitsAllocated != alloc)                             // see correction proposal 87
            {
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                {
                    cerr << "WARNING: invalid value for 'OverlayBitsAllocated' (" << BitsAllocated << ") ";
                    cerr << "... assuming " << alloc << " !" << endl; 
                }
                BitsAllocated = alloc;
            } 
            docu->getValue(DCM_PixelData, Data);
            Valid = (Data != NULL);
            EmbeddedData = (Data != NULL);
        }
    }
}


DiOverlayPlane::DiOverlayPlane(const unsigned int group,
                               const unsigned long rows,
                               const unsigned long columns,
                               const EM_Overlay mode,
                               const signed int left,
                               const signed int top,
                               const DcmOverlayData &data,
                               const DcmLongString &label,
                               const DcmLongString &description)
  : NumberOfFrames(1),
    ImageFrameOrigin(0),
    Top(top),
    Left(left),
    Height(0),
    Width(0),
    Rows(rows),
    Columns(columns),
    BitsAllocated(1),
    BitPosition(1),
    Foreground(1),
    Threshold(1),
    Mode(mode),
    DefaultMode(mode),
    Label(),
    Description(),
    GroupNumber(group),
    Valid(0),
    Visible(1),
    BitPos(0),
    StartBitPos(0),
    StartLeft(0),
    StartTop(0),
    EmbeddedData(0),
    Ptr(NULL),
    StartPtr(NULL),
    Data(NULL)
{
    const char *str;
    if ((Valid = DiDocument::getElemValue((const DcmElement *)&label, str)))
        Label = str;
    if ((Valid &= DiDocument::getElemValue((const DcmElement *)&description, str)))
        Description = str;
    Valid &= DiDocument::getElemValue((const DcmElement *)&data, Data);
    Top--;                                                      // overlay origin is numbered from 1
    Left--;
    
    /* ... */
}


DiOverlayPlane::DiOverlayPlane(DiOverlayPlane *plane,
                               const unsigned int bit,
                               Uint16 *data,
                               Uint16 *temp,
                               const Uint16 width,
                               const Uint16 height,
                               const Uint16 columns,
                               const Uint16 rows)
  : NumberOfFrames(plane->NumberOfFrames),
    ImageFrameOrigin(plane->ImageFrameOrigin),
    Top(plane->Top),
    Left(plane->Left),
    Height(plane->Height),
    Width(plane->Width),
    Rows(rows),
    Columns(columns),
    BitsAllocated(16),
    BitPosition(bit),
    Foreground(plane->Foreground),
    Threshold(plane->Threshold),
    Mode(plane->Mode),
    DefaultMode(plane->DefaultMode),
    Label(plane->Label),
    Description(plane->Description),
    GroupNumber(plane->GroupNumber),
    Valid(0),
    Visible(plane->Visible),
    BitPos(0),
    StartBitPos(0),
    StartLeft(0),
    StartTop(0),
    EmbeddedData(0),
    Ptr(NULL),
    StartPtr(NULL),
    Data(data)
{
    if (temp != NULL)
    {
        register Uint16 x;
        register Uint16 y;
        register Uint16 *q = temp;
        register const Uint16 mask = 1 << bit;
        const Uint16 skip_x = width - plane->Columns;
        const unsigned long skip_f = (unsigned long)(height - plane->Rows) * (unsigned long)width;
        for (unsigned long f = 0; f < NumberOfFrames; f++)
        {
            if (plane->reset(f + ImageFrameOrigin))
            {
                for (y = 0; y < plane->Rows; y++)
                {
                    for (x = 0; x < plane->Columns; x++, q++)
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


Uint8 *DiOverlayPlane::getData(const unsigned long frame,
                               const Uint16 xmin,
                               const Uint16 ymin,
                               const Uint16 xmax,
                               const Uint16 ymax,
                               const Uint8 value)
{
    const unsigned long count = (unsigned long)(xmax - xmin) * (unsigned long)(ymax - ymin);
    Uint8 *data = new Uint8[count];
    if (data != NULL)
    {
        OFBitmanipTemplate<Uint8>::zeroMem(data, count);
        register Uint16 x;
        register Uint16 y;
        register Uint8 *q = data;
        if (reset(frame + ImageFrameOrigin))
        {
            for (y = ymin; y < ymax; y++)
            {
                setStart(xmin, y);
                for (x = xmin; x < xmax; x++, q++)
                {
                    if (getNextBit())
                        *q = value;                         // set pixel value (default: 0xff)
                }
            }
        }
    }
    return data;
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


void DiOverlayPlane::setScaling(const double xfactor,
                                const double yfactor)
{
    Left = (Sint16)(xfactor * Left);
    Top = (Sint16)(yfactor * Top);
    Width = (Uint16)(xfactor * Width);
    Height = (Uint16)(yfactor * Height);
}


void DiOverlayPlane::setFlipping(const int horz,
                                 const int vert,
                                 const Uint16 columns,
                                 const Uint16 rows)
{
    if (horz)
    {
        Left = (Sint16)((signed long)columns - Width - Left);
        StartLeft = (Uint16)((signed long)Columns - Width - StartLeft);
    }
    if (vert)
    {
        Top = (Sint16)((signed long)rows - Height - Top);
        StartTop = (Uint16)((signed long)Rows - Height - StartTop);
    }
}


void DiOverlayPlane::setRotation(const int degree,
                                 const Uint16 columns,
                                 const Uint16 rows)
{
    if (degree == 180)
        setFlipping(1, 1, columns, rows);
    else if ((degree == 90) || (degree == 270))
    {
        Uint16 us = Height;                     // swap visible width/height
        Height = Width;
        Width = us;
        us = Rows;                              // swap stored width/height
        Rows = Columns;
        Columns = us;
        if (degree == 90)                       // rotate right
        {
            Sint16 ss = Left;
            us = StartLeft;
            Left = (Sint16)((signed long)columns - Width - Top);
            StartLeft = (Uint16)((signed long)Columns - Width - StartTop);
            Top = ss;
            StartTop = us;
        } else {                                // rotate left
            Sint16 ss = Left;
            us = StartLeft;
            Left = Top;
            StartLeft = StartTop;
            Top = (Sint16)((signed long)rows - Height - ss);
            StartTop = (Uint16)((signed long)Rows - Height - us);
        }
    }
}


/*
**
** CVS/RCS Log:
** $Log: diovpln.cc,v $
** Revision 1.4  1998-12-22 13:51:04  joergr
** Removed variable declaration to avoid compiler warnings (reported by
** MSVC5). Changed initialization of member variable 'DefaultMode'.
**
** Revision 1.3  1998/12/16 16:20:10  joergr
** Added method to export overlay planes (create 8-bit bitmap).
** Implemented flipping and rotation of overlay planes.
**
** Revision 1.2  1998/12/14 17:41:56  joergr
** Added methods to add and remove additional overlay planes (still untested).
** Added methods to support overlay labels and descriptions.
**
** Revision 1.1  1998/11/27 16:22:13  joergr
** Added copyright message.
** Introduced global debug level for dcmimage module to control error output.
** Added methods and constructors for flipping and rotating, changed for
** scaling and clipping.
**
** Revision 1.5  1998/05/11 14:52:35  joergr
** Added CVS/RCS header to each file.
**
**
*/
