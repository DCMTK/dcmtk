/*
**
** Author:  Joerg Riesmeier
** Created: 23.12.96
**
** Module:  dirgbimg.cc
**
** Purpose: DicomRGBImage (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:37 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/dirgbimg.cc,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"
#include "dctypes.h"

#include "dirgbimg.h"
#include "dirgbpxt.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiRGBImage::DiRGBImage(const DiDocument *docu, const EI_Status status)
  : DiColorImage(docu, status)
{
    if ((Document != NULL) && (InputData != NULL) && (Status == EIS_Normal))
    {
        switch (InputData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiRGBPixelTemplate<Uint8, Uint8>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint8:
                InterData = new DiRGBPixelTemplate<Sint8, Uint8>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Uint16:
                InterData = new DiRGBPixelTemplate<Uint16, Uint16>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint16:
                InterData = new DiRGBPixelTemplate<Sint16, Uint16>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Uint32:
                InterData = new DiRGBPixelTemplate<Uint32, Uint32>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint32:
                InterData = new DiRGBPixelTemplate<Sint32, Uint32>(Document, InputData, Status, BitsPerSample);
                break;
        }
        deleteInputData();
        checkInterData();
    }
} 


/*--------------*
 *  destructor  *
 *--------------*/

DiRGBImage::~DiRGBImage()
{
}


/*
**
** CVS/RCS Log:
** $Log: dirgbimg.cc,v $
** Revision 1.3  1998-05-11 14:52:37  joergr
** Added CVS/RCS header to each file.
**
**
*/
