/*
**
** Author:  Joerg Riesmeier
** Created: 12.03.98
**
** Module:  diybrimg.cc
**
** Purpose: DicomYBRImage (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:39 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/diybrimg.cc,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"
#include "dctypes.h"

#include "diybrimg.h"
#include "diybrpxt.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiYBRImage::DiYBRImage(const DiDocument *docu, const EI_Status status)
  : DiColorImage(docu, status)
{
    if ((Document != NULL) && (InputData != NULL) && (Status == EIS_Normal))
    {
        switch (InputData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiYBRPixelTemplate<Uint8, Uint8>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint8:
                InterData = new DiYBRPixelTemplate<Sint8, Uint8>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Uint16:
                InterData = new DiYBRPixelTemplate<Uint16, Uint16>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint16:
                InterData = new DiYBRPixelTemplate<Sint16, Uint16>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Uint32:
                InterData = new DiYBRPixelTemplate<Uint32, Uint32>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint32:
                InterData = new DiYBRPixelTemplate<Sint32, Uint32>(Document, InputData, Status, BitsPerSample);
                break;
        }
        deleteInputData();
        checkInterData();
    }
} 


/*--------------*
 *  destructor  *
 *--------------*/

DiYBRImage::~DiYBRImage()
{
}


/*
**
** CVS/RCS Log:
** $Log: diybrimg.cc,v $
** Revision 1.2  1998-05-11 14:52:39  joergr
** Added CVS/RCS header to each file.
**
**
*/
