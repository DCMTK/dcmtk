/*
**
** Author:  Joerg Riesmeier
** Created: 13.03.98
**
** Module:  diyp2img.cc
**
** Purpose: DicomYBRPart422Image (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:40 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/diyp2img.cc,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"
#include "dctypes.h"

#include "diyp2img.h"
#include "diyp2pxt.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiYBRPart422Image::DiYBRPart422Image(const DiDocument *docu, const EI_Status status)
  : DiColorImage(docu, status)
{
    if ((Document != NULL) && (InputData != NULL) && (Status == EIS_Normal))
    {
        switch (InputData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiYBRPart422PixelTemplate<Uint8, Uint8>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint8:
                InterData = new DiYBRPart422PixelTemplate<Sint8, Uint8>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Uint16:
                InterData = new DiYBRPart422PixelTemplate<Uint16, Uint16>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint16:
                InterData = new DiYBRPart422PixelTemplate<Sint16, Uint16>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Uint32:
                InterData = new DiYBRPart422PixelTemplate<Uint32, Uint32>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint32:
                InterData = new DiYBRPart422PixelTemplate<Sint32, Uint32>(Document, InputData, Status, BitsPerSample);
                break;
        }
        deleteInputData();
        checkInterData();
    }
} 


/*--------------*
 *  destructor  *
 *--------------*/

DiYBRPart422Image::~DiYBRPart422Image()
{
}


/*
**
** CVS/RCS Log:
** $Log: diyp2img.cc,v $
** Revision 1.2  1998-05-11 14:52:40  joergr
** Added CVS/RCS header to each file.
**
**
*/
