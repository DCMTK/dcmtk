/*
 *
 *  Copyright (C) 1996-2000, OFFIS
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
 *  Update Date:      $Date: 2001-05-14 09:50:25 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/diovpln.cc,v $
 *  CVS/RCS Revision: $Revision: 1.22 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "ofconsol.h"
#include "dctypes.h"
#include "dcdeftag.h"
#include "dctagkey.h"
#include "ofbmanip.h"

#include "diovpln.h"
#include "didocu.h"


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
    PValue(0),
    Mode(EMO_Graphic),
    DefaultMode(EMO_Graphic),
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
        tag.setElement(DCM_NumberOfFramesInOverlay.getElement());
        docu->getValue(tag, sl);
        NumberOfFrames = (sl < 1) ? 1 : (Uint32)sl;
        tag.setElement(DCM_ImageFrameOrigin.getElement());
        docu->getValue(tag, ImageFrameOrigin);
        if (ImageFrameOrigin > 0)                                   // image frame origin is numbered from 1
            ImageFrameOrigin--;
        tag.setElement(DCM_OverlayOrigin.getElement());
#ifdef REVERSE_OVERLAY_ORIGIN_ORDER
        Valid = (docu->getValue(tag, Left, 0) > 0);
        if (Valid)
        {
            if (docu->getValue(tag, Top, 1) < 2)
            {
                ofConsole.lockCerr() << "WARNING: missing second value for 'OverlayOrigin' ... "
                                     << "assuming 'Top' = " << Top << " !" << endl;
                ofConsole.unlockCerr();
            }
        }
#else
        Valid = (docu->getValue(tag, Top, 0) > 0);
        if (Valid)
        {
            if (docu->getValue(tag, Left, 1) < 2)
            {
                ofConsole.lockCerr() << "WARNING: missing second value for 'OverlayOrigin' ... "
                                     << "assuming 'Left' = " << Left << " !" << endl;
                ofConsole.unlockCerr();
            }
        }
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
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid value for 'OverlayBitsAllocated' (" << BitsAllocated
                                         << ") ... assuming " << alloc << " !" << endl;
                    ofConsole.unlockCerr();
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
                               const Sint16 left,
                               const Sint16 top,
                               const Uint16 columns,
                               const Uint16 rows,
                               const DcmOverlayData &data,
                               const DcmLongString &label,
                               const DcmLongString &description,
                               const EM_Overlay mode)
  : NumberOfFrames(1),
    ImageFrameOrigin(0),
    Top(top),
    Left(left),
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
    GroupNumber(group),
    Valid(0),
    Visible((mode == EMO_BitmapShutter) ? 1 : 0),
    BitPos(0),
    StartBitPos(0),
    StartLeft(0),
    StartTop(0),
    EmbeddedData(0),
    Ptr(NULL),
    StartPtr(NULL),
    Data(NULL)
{
    DiDocument::getElemValue((const DcmElement *)&label, Label);
    DiDocument::getElemValue((const DcmElement *)&description, Description);
    Valid = (DiDocument::getElemValue((const DcmElement *)&data, Data) > 0);
    Top--;                                                      // overlay origin is numbered from 1
    Left--;
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


void *DiOverlayPlane::getData(const unsigned long frame,
                              const Uint16 xmin,
                              const Uint16 ymin,
                              const Uint16 xmax,
                              const Uint16 ymax,
                              const int bits,
                              const Uint16 fore,
                              const Uint16 back)
{
    const unsigned long count = (unsigned long)(xmax - xmin) * (unsigned long)(ymax - ymin);
    if (count > 0)
    {
        const Uint16 mask = (Uint16)DicomImageClass::maxval(bits);
        if (bits == 1)
        {
            const unsigned long count1 = (count + 7) / 8;           // round value
            Uint8 *data = new Uint8[count1];
            if (data != NULL)
            {
                if ((fore & mask) != (back & mask))
                {
                    OFBitmanipTemplate<Uint8>::setMem(data, 0x0, count1);
                    register Uint16 x;
                    register Uint16 y;
                    register Uint8 value = 0;
                    register Uint8 *q = data;
                    register int bit = 0;
                    if (reset(frame + ImageFrameOrigin))
                    {
                        for (y = ymin; y < ymax; y++)
                        {
                            setStart(xmin, y);
                            for (x = xmin; x < xmax; x++)
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
                                    bit++;
                                }
                            }
                        }
                        if (bit != 0)
                            *(q++) = value;
                    }
                } else {
                    OFBitmanipTemplate<Uint8>::setMem(data, (fore) ? 0xff : 0x0, count1);
                }
            }
            return (void *)data;
        }
        else if ((bits > 1) && (bits <= 8))
        {
            Uint8 *data = new Uint8[count];
            if (data != NULL)
            {
                const Uint8 fore8 = (Uint8)(fore & mask);
                const Uint8 back8 = (Uint8)(back & mask);
                OFBitmanipTemplate<Uint8>::setMem(data, back8, count);
                if (fore8 != back8)                                     // optimization
                {
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
                                    *q = fore8;                         // set pixel value (default: 0xff)
                            }
                        }
                    }
                }
            }
            return (void *)data;
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
                    register Uint16 x;
                    register Uint16 y;
                    register Uint16 *q = data;
                    if (reset(frame + ImageFrameOrigin))
                    {
                        for (y = ymin; y < ymax; y++)
                        {
                            setStart(xmin, y);
                            for (x = xmin; x < xmax; x++, q++)
                            {
                                if (getNextBit())
                                    *q = fore16;                        // set pixel value (default: 0xff)
                            }
                        }
                    }
                }
            }
            return (void *)data;
        }
    }
    return NULL;
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
                                 const signed long columns,
                                 const signed long rows)
{
    if (horz)
    {
        Left = (Sint16)(columns - Width - Left);
        StartLeft = (Uint16)((signed long)Columns - Width - StartLeft);
    }
    if (vert)
    {
        Top = (Sint16)(rows - Height - Top);
        StartTop = (Uint16)((signed long)Rows - Height - StartTop);
    }
}


void DiOverlayPlane::setRotation(const int degree,
                                 const signed long left,
                                 const signed long top,
                                 const Uint16 columns,
                                 const Uint16 rows)
{
    if (degree == 180)                          // equal to v/h flip
        setFlipping(1, 1, left + columns, top + rows);
    else if ((degree == 90) || (degree == 270))
    {
        Uint16 us = Height;                     // swap visible width/height
        Height = Width;
        Width = us;
/*
        us = Rows;                              // swap stored width/height -> already done in constructor !
        Rows = Columns;
        Columns = us;
*/
        if (degree == 90)                       // rotate right
        {
            Sint16 ss = Left;
            us = StartLeft;
            Left = (Sint16)((signed long)columns - Width - Top + top);
            StartLeft = (Uint16)((signed long)Columns - Width - StartTop);
            Top = (Sint16)(ss - left);
            StartTop = us;
        } else {                                // rotate left
            Sint16 ss = Left;
            us = StartLeft;
            Left = (Sint16)(Top - top);
            StartLeft = StartTop;
            Top = (Sint16)((signed long)rows - Height - ss + left);
            StartTop = (Uint16)((signed long)Rows - Height - us);
        }
    }
}


/*
 *
 * CVS/RCS Log:
 * $Log: diovpln.cc,v $
 * Revision 1.22  2001-05-14 09:50:25  joergr
 * Added support for "1 bit output" of overlay planes; useful to extract
 * overlay planes from the pixel data and store them separately in the dataset.
 *
 * Revision 1.21  2000/04/28 12:33:48  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.20  2000/04/27 13:10:32  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.19  2000/03/08 16:24:33  meichel
 * Updated copyright header.
 *
 * Revision 1.18  2000/03/03 14:09:22  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.17  1999/10/20 18:40:13  joergr
 * Added explicit type cast to make MSVC happy.
 *
 * Revision 1.16  1999/10/20 10:35:58  joergr
 * Enhanced method getOverlayData to support 12 bit data for print.
 *
 * Revision 1.15  1999/08/25 16:43:09  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.14  1999/07/23 13:46:55  joergr
 * Enhanced robustness of reading the attribute 'OverlayOrigin'.
 *
 * Revision 1.13  1999/04/28 15:04:49  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.12  1999/03/24 17:24:07  joergr
 * Removed bug in routines rotating and flipping overlay planes in clipped
 * images.
 *
 * Revision 1.11  1999/03/22 09:37:33  meichel
 * Reworked data dictionary based on the 1998 DICOM edition and the latest
 *   supplement versions. Corrected dcmtk applications for minor changes
 *   in attribute name constants.
 *
 * Revision 1.10  1999/03/22 08:58:32  joergr
 * Added parameter to specify (transparent) background color for method
 * getOverlayData().
 *
 * Revision 1.9  1999/02/28 16:42:21  joergr
 * Corrected bug: the bit position for bitmap shutters was 1 instead of 0
 * (i.e. the first bit was always been skipped and the all following bits were
 * wrong).
 *
 * Revision 1.8  1999/02/03 17:44:05  joergr
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 * Revision 1.7  1999/01/20 14:57:12  joergr
 * Added new overlay plane mode for bitmap shutters.
 *
 * Revision 1.6  1998/12/23 13:22:26  joergr
 * Changed parameter type (long to int) to avoid warning reported by MSVC5.
 *
 * Revision 1.5  1998/12/23 11:31:12  joergr
 * Change order of parameters for addOverlay() and getOverlayData().
 * Introduced new overlay mode item EMO_Graphic (= EMO_Replace).
 * Corrected bug concerning flipping and rotating overlay planes (same
 * image object).
 *
 * Revision 1.4  1998/12/22 13:51:04  joergr
 * Removed variable declaration to avoid compiler warnings (reported by
 * MSVC5). Changed initialization of member variable 'DefaultMode'.
 *
 * Revision 1.3  1998/12/16 16:20:10  joergr
 * Added method to export overlay planes (create 8-bit bitmap).
 * Implemented flipping and rotation of overlay planes.
 *
 * Revision 1.2  1998/12/14 17:41:56  joergr
 * Added methods to add and remove additional overlay planes (still untested).
 * Added methods to support overlay labels and descriptions.
 *
 * Revision 1.1  1998/11/27 16:22:13  joergr
 * Added copyright message.
 * Introduced global debug level for dcmimage module to control error output.
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 *
 * Revision 1.5  1998/05/11 14:52:35  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
