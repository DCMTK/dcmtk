/*
**
** Author:  Joerg Riesmeier
** Created: 12.01.97
**
** Module:  dihsvimg.cc
**
** Purpose: DicomHSVImage (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:28 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/dihsvimg.cc,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"
#include "dctypes.h"

#include "dihsvimg.h"
#include "dihsvpxt.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiHSVImage::DiHSVImage(const DiDocument *docu, const EI_Status status)
  : DiColorImage(docu, status)
{
    if ((Document != NULL) && (InputData != NULL) && (Status == EIS_Normal))
    {
        switch (InputData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiHSVPixelTemplate<Uint8, Uint8>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint8:
                InterData = new DiHSVPixelTemplate<Sint8, Uint8>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Uint16:
                InterData = new DiHSVPixelTemplate<Uint16, Uint16>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint16:
                InterData = new DiHSVPixelTemplate<Sint16, Uint16>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Uint32:
                InterData = new DiHSVPixelTemplate<Uint32, Uint32>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint32:
                InterData = new DiHSVPixelTemplate<Sint32, Uint32>(Document, InputData, Status, BitsPerSample);
                break;
        }
        deleteInputData();
        checkInterData();
    }
} 


/*--------------*
 *  destructor  *
 *--------------*/

DiHSVImage::~DiHSVImage()
{
}


/*
**
** CVS/RCS Log:
** $Log: dihsvimg.cc,v $
** Revision 1.3  1998-05-11 14:52:28  joergr
** Added CVS/RCS header to each file.
**
**
*/
