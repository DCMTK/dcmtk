/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomPaletteImage (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-01-25 17:49:04 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/dipalimg.cc,v $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"
#include "dctag.h"

#include "didocu.h"
#include "dipalimg.h"
#include "dipalpxt.h"
#include "diluptab.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiPaletteImage::DiPaletteImage(const DiDocument *docu,
                               const EI_Status status)
  : DiColorImage(docu, status, 1)
{
    if ((Document != NULL) && (InputData != NULL) && (ImageStatus == EIS_Normal))
    {
        if (BitsStored <= MAX_TABLE_ENTRY_SIZE)
        {
            DiLookupTable *palette[3];
            if (Document->getFlags() & CIF_WrongPaletteAttributeTags)
            {
                palette[0] = new DiLookupTable(Document, DcmTagKey(0x0028, 0x1111), DcmTagKey(0x0028, 0x1211),
                    DcmTagKey(0,0), &ImageStatus);
                palette[1] = new DiLookupTable(Document, DcmTagKey(0x0028, 0x1112), DcmTagKey(0x0028, 0x1212),
                    DcmTagKey(0,0), &ImageStatus);
                palette[2] = new DiLookupTable(Document, DcmTagKey(0x0028, 0x1113), DcmTagKey(0x0028, 0x1213),
                    DcmTagKey(0,0), &ImageStatus);
            }
            else
            {
                palette[0] = new DiLookupTable(Document, DCM_RedPaletteColorLookupTableDescriptor,
                    DCM_RedPaletteColorLookupTableData, DcmTagKey(0,0), &ImageStatus);
                palette[1] = new DiLookupTable(Document, DCM_GreenPaletteColorLookupTableDescriptor,
                    DCM_GreenPaletteColorLookupTableData, DcmTagKey(0,0), &ImageStatus);
                palette[2] = new DiLookupTable(Document, DCM_BluePaletteColorLookupTableDescriptor,
                    DCM_BluePaletteColorLookupTableData, DcmTagKey(0,0), &ImageStatus);
            }
            if ((ImageStatus == EIS_Normal) && (palette[0] != NULL) && (palette[1] != NULL) && (palette[2] != NULL))
            {
                BitsPerSample = 0;
                /* determine the maximum value for bits stored of the three lookup tables */
                for (int jj = 0; jj < 3; jj++)
                {
                    if (palette[jj]->getBits() > (Uint16)BitsPerSample)
                        BitsPerSample = palette[jj]->getBits();
                }
                if ((BitsPerSample < 1) || (BitsPerSample > MAX_TABLE_ENTRY_SIZE))
                {
                    if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                    {
                        ofConsole.lockCerr() << "WARNING: invalid value for 'BitsPerSample' (" << BitsPerSample
                                             << ") computed from color palettes !" << endl;
                        ofConsole.unlockCerr();
                    }
                }
                switch (InputData->getRepresentation())
                {
                    case EPR_Uint8:
                        if (BitsPerSample <= 8)
                            InterData = new DiPalettePixelTemplate<Uint8, Uint32, Uint8>(Document, InputData, palette, ImageStatus);
                        else
                            InterData = new DiPalettePixelTemplate<Uint8, Uint32, Uint16>(Document, InputData, palette, ImageStatus);
                        break;
                    case EPR_Sint8:
                        if (BitsPerSample <= 8)
                            InterData = new DiPalettePixelTemplate<Sint8, Sint32, Uint8>(Document, InputData, palette, ImageStatus);
                        else
                            InterData = new DiPalettePixelTemplate<Sint8, Sint32, Uint16>(Document, InputData, palette, ImageStatus);
                        break;
                    case EPR_Uint16:
                        if (BitsPerSample <= 8)
                            InterData = new DiPalettePixelTemplate<Uint16, Uint32, Uint8>(Document, InputData, palette, ImageStatus);
                        else
                            InterData = new DiPalettePixelTemplate<Uint16, Uint32, Uint16>(Document, InputData, palette, ImageStatus);
                        break;
                    case EPR_Sint16:
                        if (BitsPerSample <= 8)
                            InterData = new DiPalettePixelTemplate<Sint16, Sint32, Uint8>(Document, InputData, palette, ImageStatus);
                        else
                            InterData = new DiPalettePixelTemplate<Sint16, Sint32, Uint16>(Document, InputData, palette, ImageStatus);
                        break;
                    default:
                        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                        {
                            ofConsole.lockCerr() << "WARNING: invalid value for inter-representation !" << endl;
                            ofConsole.unlockCerr();
                        }
                }
                deleteInputData();
                checkInterData();
            }
            delete palette[0];
            delete palette[1];
            delete palette[2];
        }
        else
        {
            ImageStatus = EIS_InvalidValue;
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: invalid value for 'BitsStored' (" << BitsStored << ") "
                                     << "... exceeds maximum palette entry size of " << MAX_TABLE_ENTRY_SIZE
                                     << " bits !" << endl;
                ofConsole.unlockCerr();
            }
        }
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiPaletteImage::~DiPaletteImage()
{
}


/*
 *
 * CVS/RCS Log:
 * $Log: dipalimg.cc,v $
 * Revision 1.15  2002-01-25 17:49:04  joergr
 * Fixed bug with unusual number of entries in the palette color look-up tables.
 *
 * Revision 1.14  2001/06/01 15:49:35  meichel
 * Updated copyright header
 *
 * Revision 1.13  2000/04/28 12:40:04  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.12  2000/04/27 13:15:58  joergr
 * Dcmimage library code now consistently uses ofConsole for error output.
 *
 * Revision 1.11  2000/03/08 16:21:57  meichel
 * Updated copyright header.
 *
 * Revision 1.10  2000/03/03 14:07:55  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.9  1999/04/28 13:22:59  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.8  1998/12/16 16:04:01  joergr
 * Added explanation string to LUT class (retrieved from dataset).
 *
 * Revision 1.7  1998/12/14 17:09:34  joergr
 * Added support for signed values as second entry in look-up tables
 * (= first value mapped).
 *
 * Revision 1.6  1998/11/27 14:32:19  joergr
 * Added copyright message.
 * Introduced global debug level for dcmimage module to control error output.
 * Renamed variable 'Status' to 'ImageStatus' because of possible conflicts
 * with X windows systems.
 *
 * Revision 1.5  1998/07/01 08:39:35  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.4  1998/06/25 08:52:06  joergr
 * Added compatibility mode to support ACR-NEMA images and wrong
 * palette attribute tags.
 *
 * Revision 1.3  1998/05/11 14:52:36  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
