/*
**
** Author:  Joerg Riesmeier
** Created: 14.01.97
**
** Module:  diovlimg.cc
**
** Purpose: DicomOverlayImage (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:35 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/Attic/diovlimg.cc,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"
#include "dctypes.h"

#include "diovlimg.h"
#include "dimopxt.h"
#include "didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiOverlayImage::DiOverlayImage(const DiDocument *docu, const EI_Status status)
  : DiMono2Image(docu, status, 0)
{
    if (Document != NULL)
    {
        Overlays = new DiOverlay(Document);
        if (Overlays != NULL)
        {
            BitsPerSample = 1;
            register unsigned int i;
            register DiOverlayPlane *plane;
            for (i = 0; i < Overlays->getCount(); i++)
            {
                plane = Overlays->getPlane(i);
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
                InterData = new DiMonoPixelTemplate<Uint8>((unsigned long)Rows * (unsigned long)Columns *
                    NumberOfFrames);
                if (InterData == NULL)
                {
                    Status = EIS_MemoryFailure;
                    cerr << "ERROR: can't allocate memory for inter-representation !" << endl;
                }
                else if (InterData->getData() == NULL)
                    Status = EIS_InvalidImage;
            }
            else
            {
                Status = EIS_InvalidValue;
                cerr << "ERROR: invalid value for 'Rows' (" << Rows << ") and/or ";
                cerr << "'Columns' (" << Columns << ") !" << endl;
            }
        }
    }
    else
    {
        Status = EIS_InvalidDocument;
        cerr << "ERROR: this DICOM document is invalid !" << endl;
    }
} 


/*--------------*
 *  destructor  *
 *--------------*/

DiOverlayImage::~DiOverlayImage()
{
}


/*
**
** CVS/RCS Log:
** $Log: diovlimg.cc,v $
** Revision 1.3  1998-05-11 14:52:35  joergr
** Added CVS/RCS header to each file.
**
**
*/
