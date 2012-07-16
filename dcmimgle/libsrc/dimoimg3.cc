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
 *  Purpose: DicomMonochromeImage (Source, getData 8 bit)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/dimoimg.h"
#include "dcmtk/dcmimgle/dimoipxt.h"
#include "dcmtk/dcmimgle/dimoopxt.h"
#include "dcmtk/dcmimgle/diutils.h"


void DiMonoImage::getDataUint8(void *buffer,
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
                OutputData = new DiMonoOutputPixelTemplate<Uint8, Sint32, Uint8>(buffer, InterData, Overlays, VoiLutData, PresLutData,
                    disp, VoiLutFunction, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames, samples > 1);
            else if (bits <= 16)
                OutputData = new DiMonoOutputPixelTemplate<Uint8, Sint32, Uint16>(buffer, InterData, Overlays, VoiLutData, PresLutData,
                    disp, VoiLutFunction, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
            else
                OutputData = new DiMonoOutputPixelTemplate<Uint8, Sint32, Uint32>(buffer, InterData, Overlays, VoiLutData, PresLutData,
                    disp, VoiLutFunction, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
        } else {
            if (bits <= 8)
                OutputData = new DiMonoOutputPixelTemplate<Uint8, Uint32, Uint8>(buffer, InterData, Overlays, VoiLutData, PresLutData,
                    disp, VoiLutFunction, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames, samples > 1);
            else if (bits <= 16)
                OutputData = new DiMonoOutputPixelTemplate<Uint8, Uint32, Uint16>(buffer, InterData, Overlays, VoiLutData, PresLutData,
                    disp, VoiLutFunction, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
            else
                OutputData = new DiMonoOutputPixelTemplate<Uint8, Uint32, Uint32>(buffer, InterData, Overlays, VoiLutData, PresLutData,
                    disp, VoiLutFunction, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
        }
    }
}


void DiMonoImage::getDataSint8(void *buffer,
                               DiDisplayFunction *disp,
                               const int samples,
                               const unsigned long frame,
                               const int bits,
                               const Uint32 low,
                               const Uint32 high)
{
    if (bits <= 8)
        OutputData = new DiMonoOutputPixelTemplate<Sint8, Sint32, Uint8>(buffer, InterData, Overlays, VoiLutData, PresLutData,
            disp, VoiLutFunction, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames, samples > 1);
    else if (bits <= 16)
        OutputData = new DiMonoOutputPixelTemplate<Sint8, Sint32, Uint16>(buffer, InterData, Overlays, VoiLutData, PresLutData,
            disp, VoiLutFunction, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
    else
        OutputData = new DiMonoOutputPixelTemplate<Sint8, Sint32, Uint32>(buffer, InterData, Overlays, VoiLutData, PresLutData,
            disp, VoiLutFunction, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
}
