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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DiARGBImage (Source) - UNTESTED !!!
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:14 $
 *  CVS/RCS Revision: $Revision: 1.25 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcdeftag.h"

#include "dcmtk/dcmimage/diargimg.h"
#include "dcmtk/dcmimage/diargpxt.h"
#include "dcmtk/dcmimage/diqttype.h"
#include "dcmtk/dcmimage/dilogger.h"
#include "dcmtk/dcmimgle/diluptab.h"
#include "dcmtk/dcmimgle/diinpx.h"
#include "dcmtk/dcmimgle/didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiARGBImage::DiARGBImage(const DiDocument *docu,
                         const EI_Status status)
  : DiColorImage(docu, status, 4),
    Palette()  // initializes the three color palettes to NULL
{
    if ((Document != NULL) && (InputData != NULL) && (ImageStatus == EIS_Normal))
    {
        if (BitsStored <= MAX_TABLE_ENTRY_SIZE)                         // color depth <= 16
        {
            const EL_BitsPerTableEntry descMode = (Document->getFlags() & CIF_CheckLutBitDepth) ? ELM_CheckValue : ELM_UseValue;
            Palette[0] = new DiLookupTable(Document, DCM_RedPaletteColorLookupTableDescriptor,
                DCM_RedPaletteColorLookupTableData, DCM_UndefinedTagKey, descMode, &ImageStatus);
            Palette[1] = new DiLookupTable(Document, DCM_GreenPaletteColorLookupTableDescriptor,
                DCM_GreenPaletteColorLookupTableData, DCM_UndefinedTagKey, descMode, &ImageStatus);
            Palette[2] = new DiLookupTable(Document, DCM_BluePaletteColorLookupTableDescriptor,
                DCM_BluePaletteColorLookupTableData, DCM_UndefinedTagKey, descMode, &ImageStatus);
            if ((ImageStatus == EIS_Normal) && (Palette[0] != NULL) && (Palette[1] != NULL) && (Palette[2] != NULL))
            {
                BitsPerSample = BitsStored;
                for (int jj = 0; jj < 3; jj++)                          // determine maximum bit count
                {
                    if (Palette[jj]->getBits() > OFstatic_cast(Uint16, BitsPerSample))
                        BitsPerSample = Palette[jj]->getBits();
                }
                Init();                                                 // create intermediate representation
            }
        }
        else                                                            // color depth > 16
        {
            ImageStatus = EIS_InvalidValue;
            DCMIMAGE_ERROR("invalid value for 'BitsStored' (" << BitsStored << ") "
                << "... exceeds maximum palette entry size of " << MAX_TABLE_ENTRY_SIZE << " bits");
        }
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiARGBImage::~DiARGBImage()
{
    delete Palette[0];
    delete Palette[1];
    delete Palette[2];
}


/*********************************************************************/


void DiARGBImage::Init()
{
    /* number of pixels per plane */
    const unsigned long planeSize = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows);
    switch (InputData->getRepresentation())
    {
        case EPR_Uint8:
            if (BitsPerSample <= 8)
                InterData = new DiARGBPixelTemplate<Uint8, Uint32, Uint8>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            else
                InterData = new DiARGBPixelTemplate<Uint8, Uint32, Uint16>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            break;
        case EPR_Sint8:
            if (BitsPerSample <= 8)
                InterData = new DiARGBPixelTemplate<Sint8, Sint32, Uint8>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            else
                InterData = new DiARGBPixelTemplate<Sint8, Sint32, Uint16>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            break;
        case EPR_Uint16:
            if (BitsPerSample <= 8)
                InterData = new DiARGBPixelTemplate<Uint16, Uint32, Uint8>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            else
                InterData = new DiARGBPixelTemplate<Uint16, Uint32, Uint16>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            break;
        case EPR_Sint16:
            if (BitsPerSample <= 8)
                InterData = new DiARGBPixelTemplate<Sint16, Sint32, Uint8>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            else
                InterData = new DiARGBPixelTemplate<Sint16, Sint32, Uint16>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            break;
        default:
            DCMIMAGE_WARN("invalid value for inter-representation");
    }
    deleteInputData();                          // input data is no longer needed
    checkInterData();
}


/*********************************************************************/


int DiARGBImage::processNextFrames(const unsigned long fcount)
{
    if (DiImage::processNextFrames(fcount))
    {
        delete InterData;
        InterData = NULL;
        Init();
        return (ImageStatus == EIS_Normal);
    }
    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: diargimg.cc,v $
 * Revision 1.25  2010-10-14 13:14:14  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.24  2010-10-06 14:50:25  joergr
 * Replaced numeric tags by pre-defined tag names for old ACR/NEMA palettes.
 * Use DCM_UndefinedTagKey if no explanation tag can be specified for a LUT.
 *
 * Revision 1.23  2010-03-26 14:12:02  joergr
 * Fixed missing initialization of color palette LUTs in case of invalid
 * images.
 *
 * Revision 1.22  2009-11-25 14:48:46  joergr
 * Adapted code for new approach to access individual frames of a DICOM image.
 *
 * Revision 1.21  2009-10-14 10:23:56  joergr
 * Fixed minor issues in log output. Also updated copyright date (if required).
 *
 * Revision 1.20  2009-10-13 14:08:33  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.19  2007-03-16 11:48:11  joergr
 * Introduced new flag that allows to select how to handle the BitsPerTableEntry
 * value in the LUT descriptor (use, ignore or check).
 *
 * Revision 1.18  2006/08/15 16:35:01  meichel
 * Updated the code in module dcmimage to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.17  2005/12/08 15:42:19  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.16  2003/12/17 17:50:37  joergr
 * Added new compatibility flag that allows to ignore the third value of LUT
 * descriptors and to determine the bits per table entry automatically.
 *
 * Revision 1.15  2003/12/17 16:34:57  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.14  2002/06/26 16:22:52  joergr
 * Corrected decoding of multi-frame, planar images.
 *
 * Revision 1.13  2001/06/01 15:49:34  meichel
 * Updated copyright header
 *
 * Revision 1.12  2000/04/28 12:40:02  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.11  2000/04/27 13:15:56  joergr
 * Dcmimage library code now consistently uses ofConsole for error output.
 *
 * Revision 1.10  2000/03/08 16:21:55  meichel
 * Updated copyright header.
 *
 * Revision 1.9  2000/03/03 14:07:54  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.8  1999/04/28 13:18:21  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.7  1998/12/16 16:03:47  joergr
 * Added explanation string to LUT class (retrieved from dataset).
 *
 * Revision 1.6  1998/12/14 17:09:44  joergr
 * Added support for signed values as second entry in look-up tables
 * (= first value mapped).
 *
 * Revision 1.5  1998/11/27 14:25:09  joergr
 * Added copyright message.
 * Renamed variable 'Status' to 'ImageStatus' because of possible conflicts
 * with X windows systems.
 *
 * Revision 1.4  1998/07/01 08:39:34  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.3  1998/05/11 14:52:25  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
