/*
 *
 *  Copyright (C) 1997-99, OFFIS
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
 *  Purpose: DicomHSVImage (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-04-28 13:21:08 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/dihsvimg.cc,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"

#include "dihsvimg.h"
#include "dihsvpxt.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiHSVImage::DiHSVImage(const DiDocument *docu,
                       const EI_Status status)
  : DiColorImage(docu, status, 3)
{
    if ((Document != NULL) && (InputData != NULL) && (ImageStatus == EIS_Normal))
    {
        switch (InputData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiHSVPixelTemplate<Uint8, Uint8>(Document, InputData, ImageStatus, BitsPerSample);
                break;
            case EPR_Sint8:
                InterData = new DiHSVPixelTemplate<Sint8, Uint8>(Document, InputData, ImageStatus, BitsPerSample);
                break;
            case EPR_Uint16:
                InterData = new DiHSVPixelTemplate<Uint16, Uint16>(Document, InputData, ImageStatus, BitsPerSample);
                break;
            case EPR_Sint16:
                InterData = new DiHSVPixelTemplate<Sint16, Uint16>(Document, InputData, ImageStatus, BitsPerSample);
                break;
            case EPR_Uint32:
                InterData = new DiHSVPixelTemplate<Uint32, Uint32>(Document, InputData, ImageStatus, BitsPerSample);
                break;
            case EPR_Sint32:
                InterData = new DiHSVPixelTemplate<Sint32, Uint32>(Document, InputData, ImageStatus, BitsPerSample);
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
 *
 * CVS/RCS Log:
 * $Log: dihsvimg.cc,v $
 * Revision 1.5  1999-04-28 13:21:08  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.4  1998/11/27 14:30:22  joergr
 * Added copyright message.
 * Renamed variable 'Status' to 'ImageStatus' because of possible conflicts
 * with X windows systems.
 *
 * Revision 1.3  1998/05/11 14:52:28  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
