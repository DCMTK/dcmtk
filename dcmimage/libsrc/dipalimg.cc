/*
**
** Author:  Joerg Riesmeier
** Created: 21.12.96
**
** Module:  dipalimg.cc
**
** Purpose: DicomPaletteImage (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:36 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/dipalimg.cc,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"

#include "dipalimg.h"
#include "dipalpxt.h"
#include "diluptab.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiPaletteImage::DiPaletteImage(const DiDocument *docu, const EI_Status status)
  : DiColorImage(docu, status)
{
    if ((Document != NULL) && (InputData != NULL) && (Status == EIS_Normal))
    {
        if (BitsStored <= MAX_TABLE_ENTRY_SIZE)
        {
            DiLookupTable *palette[3];
            palette[0] = new DiLookupTable(Document, DCM_RedPaletteColorLookupTableDescriptor,
                DCM_RedPaletteColorLookupTableData, &Status);
            palette[1] = new DiLookupTable(Document, DCM_GreenPaletteColorLookupTableDescriptor,
                DCM_GreenPaletteColorLookupTableData, &Status);
            palette[2] = new DiLookupTable(Document, DCM_BluePaletteColorLookupTableDescriptor,
                DCM_BluePaletteColorLookupTableData, &Status);
            if ((Status == EIS_Normal) && (palette[0] != NULL) && (palette[1] != NULL) && (palette[2] != NULL))
            {
                for (int j = 0; j < 3; j++)
                {
                    if (palette[j]->getBits() > (Uint16)BitsPerSample)
                        BitsPerSample = palette[j]->getBits();  
                }
                switch (InputData->getRepresentation())
                {
                    case EPR_Uint8:
                        if (BitsPerSample <= 8)
                            InterData = new DiPalettePixelTemplate<Uint8, Uint8>(Document, InputData,
                                palette, Status);
                        else
                            InterData = new DiPalettePixelTemplate<Uint8, Uint16>(Document, InputData,
                                palette, Status);
                        break;
                    case EPR_Sint8:
                        if (BitsPerSample <= 8)
                            InterData = new DiPalettePixelTemplate<Sint8, Uint8>(Document, InputData,
                                palette, Status);
                        else
                            InterData = new DiPalettePixelTemplate<Sint8, Uint16>(Document, InputData,
                                palette, Status);
                        break;
                    case EPR_Uint16:
                        if (BitsPerSample <= 8)
                            InterData = new DiPalettePixelTemplate<Uint16, Uint8>(Document, InputData,
                                palette, Status);
                        else
                            InterData = new DiPalettePixelTemplate<Uint16, Uint16>(Document, InputData,
                                palette, Status);
                        break;
                    case EPR_Sint16:
                        if (BitsPerSample <= 8)
                            InterData = new DiPalettePixelTemplate<Sint16, Uint8>(Document, InputData,
                                palette, Status);
                        else
                            InterData = new DiPalettePixelTemplate<Sint16, Uint16>(Document, InputData,
                                palette, Status);
                        break;
                    default:
                        cerr << "WARNING: invalid value for inter-representation !" << endl;
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
            Status = EIS_InvalidValue;
            cerr << "ERROR: invalid value for 'BitsStored' (" << BitsStored << ") ";
            cerr << "... exceeds maximum palette entry size of " << MAX_TABLE_ENTRY_SIZE << " bits !" << endl;
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
**
** CVS/RCS Log:
** $Log: dipalimg.cc,v $
** Revision 1.3  1998-05-11 14:52:36  joergr
** Added CVS/RCS header to each file.
**
**
*/
