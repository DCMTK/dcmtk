/*
**
** Author:  Joerg Riesmeier
** Created: 05.01.97
**
** Module:  dicmyimg.cc
**
** Purpose: DicomCMYKImage (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:25 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/dicmyimg.cc,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"
#include "dctypes.h"

#include "dicmyimg.h"
#include "dicmypxt.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiCMYKImage::DiCMYKImage(const DiDocument *docu, const EI_Status status)
  : DiColorImage(docu, status)
{
    if ((Document != NULL) && (InputData != NULL) && (Status == EIS_Normal))
    {
        switch (InputData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiCMYKPixelTemplate<Uint8, Uint8>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint8:
                InterData = new DiCMYKPixelTemplate<Sint8, Uint8>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Uint16:
                InterData = new DiCMYKPixelTemplate<Uint16, Uint16>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint16:
                InterData = new DiCMYKPixelTemplate<Sint16, Uint16>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Uint32:
                InterData = new DiCMYKPixelTemplate<Uint32, Uint32>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint32:
                InterData = new DiCMYKPixelTemplate<Sint32, Uint32>(Document, InputData, Status, BitsPerSample);
                break;
        }
        deleteInputData();
        checkInterData();
    }
} 


/*--------------*
 *  destructor  *
 *--------------*/

DiCMYKImage::~DiCMYKImage()
{
}


/*
**
** CVS/RCS Log:
** $Log: dicmyimg.cc,v $
** Revision 1.3  1998-05-11 14:52:25  joergr
** Added CVS/RCS header to each file.
**
**
*/
