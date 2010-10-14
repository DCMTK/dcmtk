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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:18 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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


/*
 *
 * CVS/RCS Log:
 * $Log: dimoimg3.cc,v $
 * Revision 1.7  2010-10-14 13:14:18  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.6  2010-10-05 15:24:06  joergr
 * Added preliminary support for VOI LUT function. Please note, however, that
 * the sigmoid transformation is not yet implemented.
 *
 * Revision 1.5  2005/12/08 15:42:57  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.4  2003/12/08 17:43:04  joergr
 * Updated copyright header.
 *
 * Revision 1.3  2001/06/01 15:49:57  meichel
 * Updated copyright header
 *
 * Revision 1.2  2000/03/08 16:24:30  meichel
 * Updated copyright header.
 *
 * Revision 1.1  1999/12/09 17:28:03  joergr
 * Split source file dimoimg.cc into 4 parts to avoid compiler problems
 * with gcc and additional optimization options.
 *
 *
 */
