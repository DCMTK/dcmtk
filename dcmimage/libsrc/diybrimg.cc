/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Purpose: DicomYBRImage (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 14:35:29 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/diybrimg.cc,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
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
  : DiColorImage(docu, status, 3)
{
    if ((Document != NULL) && (InputData != NULL) && (ImageStatus == EIS_Normal))
    {
        switch (InputData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiYBRPixelTemplate<Uint8, Uint8>(Document, InputData, ImageStatus, BitsPerSample);
                break;
            case EPR_Sint8:
                InterData = new DiYBRPixelTemplate<Sint8, Uint8>(Document, InputData, ImageStatus, BitsPerSample);
                break;
            case EPR_Uint16:
                InterData = new DiYBRPixelTemplate<Uint16, Uint16>(Document, InputData, ImageStatus, BitsPerSample);
                break;
            case EPR_Sint16:
                InterData = new DiYBRPixelTemplate<Sint16, Uint16>(Document, InputData, ImageStatus, BitsPerSample);
                break;
            case EPR_Uint32:
                InterData = new DiYBRPixelTemplate<Uint32, Uint32>(Document, InputData, ImageStatus, BitsPerSample);
                break;
            case EPR_Sint32:
                InterData = new DiYBRPixelTemplate<Sint32, Uint32>(Document, InputData, ImageStatus, BitsPerSample);
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
** Revision 1.3  1998-11-27 14:35:29  joergr
** Added copyright message.
** Renamed variable 'Status' to 'ImageStatus' because of possible conflicts
** with X windows systems.
**
** Revision 1.2  1998/05/11 14:52:39  joergr
** Added CVS/RCS header to each file.
**
**
*/
