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
 *  Purpose: DicomMonochromeImage (Source, getData 32 bit)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:24:31 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/dimoimg5.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "dimoimg.h"
#include "dimoipxt.h"
#include "dimoopxt.h"
#include "diutils.h"



void DiMonoImage::getDataUint32(void *buffer,
                                DiDisplayFunction *disp,
                                const int samples,
                                const unsigned long frame,
                                const int bits,
                                const Uint32 low,
                                const Uint32 high)
{
    if (InterData != NULL)
    {
        if (InterData->isPotentiallySigned())
        {
            if (bits <= 8)
                OutputData = new DiMonoOutputPixelTemplate<Uint32, Sint32, Uint8>(buffer, InterData, Overlays, VoiLutData,
                    PresLutData, disp, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames, samples > 1);
            else if (bits <= 16)
                OutputData = new DiMonoOutputPixelTemplate<Uint32, Sint32, Uint16>(buffer, InterData, Overlays, VoiLutData,
                    PresLutData, disp, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
            else
                OutputData = new DiMonoOutputPixelTemplate<Uint32, Sint32, Uint32>(buffer, InterData, Overlays, VoiLutData,
                    PresLutData, disp, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
        } else {
            if (bits <= 8)
                OutputData = new DiMonoOutputPixelTemplate<Uint32, Uint32, Uint8>(buffer, InterData, Overlays, VoiLutData,
                    PresLutData, disp, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames, samples > 1);
            else if (bits <= 16)
                OutputData = new DiMonoOutputPixelTemplate<Uint32, Uint32, Uint16>(buffer, InterData, Overlays, VoiLutData,
                    PresLutData, disp, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
            else
                OutputData = new DiMonoOutputPixelTemplate<Uint32, Uint32, Uint32>(buffer, InterData, Overlays, VoiLutData,
                    PresLutData, disp, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
        }
    }
}


void DiMonoImage::getDataSint32(void *buffer,
                                DiDisplayFunction *disp,
                                const int samples,
                                const unsigned long frame,
                                const int bits,
                                const Uint32 low,
                                const Uint32 high)
{
    if (bits <= 8)
        OutputData = new DiMonoOutputPixelTemplate<Sint32, Sint32, Uint8>(buffer, InterData, Overlays, VoiLutData,
            PresLutData, disp, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames, samples > 1);
    else if (bits <= 16)
        OutputData = new DiMonoOutputPixelTemplate<Sint32, Sint32, Uint16>(buffer, InterData, Overlays, VoiLutData,
            PresLutData, disp, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
    else
        OutputData = new DiMonoOutputPixelTemplate<Sint32, Sint32, Uint32>(buffer, InterData, Overlays, VoiLutData,
            PresLutData, disp, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
}


/*
 *
 * CVS/RCS Log:
 * $Log: dimoimg5.cc,v $
 * Revision 1.2  2000-03-08 16:24:31  meichel
 * Updated copyright header.
 *
 * Revision 1.1  1999/12/09 17:28:04  joergr
 * Split source file dimoimg.cc into 4 parts to avoid compiler problems
 * with gcc and additional optimization options.
 *
 *
 */
