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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DiARGBImage (Source) - UNTESTED !!! 
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:21:55 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/diargimg.cc,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"

#include "diargimg.h"
#include "diargpxt.h"
#include "diluptab.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiARGBImage::DiARGBImage(const DiDocument *docu,
                         const EI_Status status)
  : DiColorImage(docu, status, 4)
{
    if ((Document != NULL) && (InputData != NULL) && (ImageStatus == EIS_Normal))
    {
        if (BitsStored <= MAX_TABLE_ENTRY_SIZE)                         // color depth <= 16
        {
            DiLookupTable *palette[3];                                  // create color luts
            palette[0] = new DiLookupTable(Document, DCM_RedPaletteColorLookupTableDescriptor,
                DCM_RedPaletteColorLookupTableData, DcmTagKey(0,0), &ImageStatus);
            palette[1] = new DiLookupTable(Document, DCM_GreenPaletteColorLookupTableDescriptor,
                DCM_GreenPaletteColorLookupTableData, DcmTagKey(0,0), &ImageStatus);
            palette[2] = new DiLookupTable(Document, DCM_BluePaletteColorLookupTableDescriptor,
                DCM_BluePaletteColorLookupTableData, DcmTagKey(0,0), &ImageStatus);
            if ((ImageStatus == EIS_Normal) && (palette[0] != NULL) && (palette[1] != NULL) && (palette[2] != NULL))
            {
                BitsPerSample = BitsStored;
                for (int jj = 0; jj < 3; jj++)                          // determine maximum bit count
                {
                    if (palette[jj]->getBits() > (Uint16)BitsPerSample)
                        BitsPerSample = palette[jj]->getBits();  
                }
                switch (InputData->getRepresentation())
                {
                    case EPR_Uint8:
                        if (BitsPerSample <= 8)
                            InterData = new DiARGBPixelTemplate<Uint8, Uint32, Uint8>(Document, InputData, palette, ImageStatus,
                                BitsStored);
                        else
                            InterData = new DiARGBPixelTemplate<Uint8, Uint32, Uint16>(Document, InputData, palette, ImageStatus,
                                BitsStored);
                        break;
                    case EPR_Sint8:
                        if (BitsPerSample <= 8)
                            InterData = new DiARGBPixelTemplate<Sint8, Sint32, Uint8>(Document, InputData, palette, ImageStatus,
                                BitsStored);
                        else
                            InterData = new DiARGBPixelTemplate<Sint8, Sint32, Uint16>(Document, InputData, palette, ImageStatus,
                                BitsStored);
                        break;
                    case EPR_Uint16:
                        if (BitsPerSample <= 8)
                            InterData = new DiARGBPixelTemplate<Uint16, Uint32, Uint8>(Document, InputData, palette, ImageStatus,
                                BitsStored);
                        else
                            InterData = new DiARGBPixelTemplate<Uint16, Uint32, Uint16>(Document, InputData, palette, ImageStatus,
                                BitsStored);
                        break;
                    case EPR_Sint16:
                        if (BitsPerSample <= 8)
                            InterData = new DiARGBPixelTemplate<Sint16, Sint32, Uint8>(Document, InputData, palette, ImageStatus,
                                BitsStored);
                        else
                            InterData = new DiARGBPixelTemplate<Sint16, Sint32, Uint16>(Document, InputData, palette, ImageStatus,
                                BitsStored);
                        break;
                    default: 
                        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Warnings)
                            CERR << "WARNING: invalid value for inter-representation !";
                }
                deleteInputData();                          // input data is no longer needed
                checkInterData();
            }
            delete palette[0];                              // color luts are no longer needed
            delete palette[1];
            delete palette[2];
        }
        else                                                // color depth > 16
        {
            ImageStatus = EIS_InvalidValue;
            if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
            {
                CERR << "ERROR: invalid value for 'BitsStored' (" << BitsStored << ") ";
                CERR << "... exceeds maximum palette entry size of " << MAX_TABLE_ENTRY_SIZE << " bits !" << endl;
            }
        }
    }
} 


/*--------------*
 *  destructor  *
 *--------------*/

DiARGBImage::~DiARGBImage()
{
}


/*
 *
 * CVS/RCS Log:
 * $Log: diargimg.cc,v $
 * Revision 1.10  2000-03-08 16:21:55  meichel
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
