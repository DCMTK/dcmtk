/*
**
** Author:  Joerg Riesmeier
** Created: 06.01.97
**
** Module:  diargimg.cc
**
** Purpose: DiARGBImage (Source) - UNTESTED !!! 
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:25 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/diargimg.cc,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
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

DiARGBImage::DiARGBImage(const DiDocument *docu, const EI_Status status)
  : DiColorImage(docu, status)
{
    if ((Document != NULL) && (InputData != NULL) && (Status == EIS_Normal))
    {
        if (BitsStored <= MAX_TABLE_ENTRY_SIZE)                         // color depth <= 16
        {
            DiLookupTable *palette[3];                                  // create color luts
            palette[0] = new DiLookupTable(Document, DCM_RedPaletteColorLookupTableDescriptor,
                DCM_RedPaletteColorLookupTableData, &Status);
            palette[1] = new DiLookupTable(Document, DCM_GreenPaletteColorLookupTableDescriptor,
                DCM_GreenPaletteColorLookupTableData, &Status);
            palette[2] = new DiLookupTable(Document, DCM_BluePaletteColorLookupTableDescriptor,
                DCM_BluePaletteColorLookupTableData, &Status);
            if ((Status == EIS_Normal) && (palette[0] != NULL) && (palette[1] != NULL) && (palette[2] != NULL))
            {
                BitsPerSample = BitsStored;
                for (int j = 0; j < 3; j++)                             // determine maximum bit count
                {
                    if (palette[j]->getBits() > (Uint16)BitsPerSample)
                        BitsPerSample = palette[j]->getBits();  
                }
                switch (InputData->getRepresentation())
                {
                    case EPR_Uint8:
                        if (BitsPerSample <= 8)
                            InterData = new DiARGBPixelTemplate<Uint8, Uint8>(Document, InputData, palette, Status,
                                BitsStored);
                        else
                            InterData = new DiARGBPixelTemplate<Uint8, Uint16>(Document, InputData, palette, Status,
                                BitsStored);
                        break;
                    case EPR_Sint8:
                        if (BitsPerSample <= 8)
                            InterData = new DiARGBPixelTemplate<Sint8, Uint8>(Document, InputData, palette, Status,
                                BitsStored);
                        else
                            InterData = new DiARGBPixelTemplate<Sint8, Uint16>(Document, InputData, palette, Status,
                                BitsStored);
                        break;
                    case EPR_Uint16:
                        if (BitsPerSample <= 8)
                            InterData = new DiARGBPixelTemplate<Uint16, Uint8>(Document, InputData, palette, Status,
                                BitsStored);
                        else
                            InterData = new DiARGBPixelTemplate<Uint16, Uint16>(Document, InputData, palette, Status,
                                BitsStored);
                        break;
                    case EPR_Sint16:
                        if (BitsPerSample <= 8)
                            InterData = new DiARGBPixelTemplate<Sint16, Uint8>(Document, InputData, palette, Status,
                                BitsStored);
                        else
                            InterData = new DiARGBPixelTemplate<Sint16, Uint16>(Document, InputData, palette, Status,
                                BitsStored);
                        break;
                    default: 
                        cerr << "WARNING: invalid value for inter-representation !";
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
            Status = EIS_InvalidValue;
            cerr << "ERROR: invalid value for 'BitsStored' (" << BitsStored << ") ";
            cerr << "... exceeds maximum palette entry size of " << MAX_TABLE_ENTRY_SIZE << " bits !" << endl;
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
**
** CVS/RCS Log:
** $Log: diargimg.cc,v $
** Revision 1.3  1998-05-11 14:52:25  joergr
** Added CVS/RCS header to each file.
**
**
*/
