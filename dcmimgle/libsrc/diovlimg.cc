/*
 *
 *  Copyright (C) 1996-2003, OFFIS
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomOverlayImage (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-12-08 17:38:27 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"

#include "diovlimg.h"
#include "dimopxt.h"
#include "didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiOverlayImage::DiOverlayImage(const DiDocument *docu,
                               const EI_Status status)
  : DiMono2Image(docu, status, 0)
{
    if (Document != NULL)
    {
        Overlays[0] = new DiOverlay(Document);
        if (Overlays[0] != NULL)
        {
            BitsPerSample = 1;
            register unsigned int i;
            register DiOverlayPlane *plane;
            for (i = 0; i < Overlays[0]->getCount(); i++)
            {
                plane = Overlays[0]->getPlane(i);
                if (plane != NULL)
                {
                    plane->show();
                    if (plane->getNumberOfFrames() > NumberOfFrames)
                        NumberOfFrames = plane->getNumberOfFrames();
                    if (plane->getRight() > Columns)                // determine image's width and height
                        Columns = plane->getRight();
                    if (plane->getBottom() > Rows)
                        Rows = plane->getBottom();
                }
            }
            if ((Rows > 0) && (Columns > 0))
            {
                InterData = new DiMonoPixelTemplate<Uint8>(OFstatic_cast(unsigned long, Rows) *
                    OFstatic_cast(unsigned long, Columns) * NumberOfFrames);
                if (InterData == NULL)
                {
                    ImageStatus = EIS_MemoryFailure;
                    if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
                    {
                        ofConsole.lockCerr() << "ERROR: can't allocate memory for inter-representation !" << endl;
                        ofConsole.unlockCerr();
                    }
                }
                else if (InterData->getData() == NULL)
                    ImageStatus = EIS_InvalidImage;
            }
            else
            {
                ImageStatus = EIS_InvalidValue;
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
                {
                    ofConsole.lockCerr() << "ERROR: invalid value for 'Rows' (" << Rows << ") and/or "
                                         << "'Columns' (" << Columns << ") !" << endl;
                    ofConsole.unlockCerr();
                }
            }
        }
    }
    else
    {
        ImageStatus = EIS_InvalidDocument;
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
        {
            ofConsole.lockCerr() << "ERROR: this DICOM document is invalid !" << endl;
            ofConsole.unlockCerr();
        }
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiOverlayImage::~DiOverlayImage()
{
}


/*
 *
 * CVS/RCS Log:
 * $Log: diovlimg.cc,v $
 * Revision 1.11  2003-12-08 17:38:27  joergr
 * Updated CVS header.
 *
 * Revision 1.10  2003/12/08 14:38:44  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.9  2001/06/01 15:49:59  meichel
 * Updated copyright header
 *
 * Revision 1.8  2000/04/28 12:33:47  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.7  2000/04/27 13:10:32  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.6  2000/03/08 16:24:32  meichel
 * Updated copyright header.
 *
 * Revision 1.5  2000/03/03 14:09:22  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.4  1999/04/28 15:04:49  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.3  1999/02/03 17:43:22  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.2  1998/12/14 17:40:31  joergr
 * Added methods to add and remove additional overlay planes (still untested).
 *
 * Revision 1.1  1998/11/27 16:20:53  joergr
 * Added copyright message.
 * Introduced global debug level for dcmimage module to control error output.
 * Renamed variable 'Status' to 'ImageStatus' because of possible conflicts
 * with X windows systems.
 *
 * Revision 1.3  1998/05/11 14:52:35  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
