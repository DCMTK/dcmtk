/*
 *
 *  Copyright (C) 1996-99, OFFIS
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
 *  Purpose: DicomMonochromePixel (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 16:16:45 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/dimopx.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "dimopx.h"
#include "dimomod.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMonoPixel::DiMonoPixel(const unsigned long count)
  : DiPixel(count),
    Modality(NULL)
{
}


DiMonoPixel::DiMonoPixel(const DiInputPixel *pixel, DiMonoModality *modality)
  : DiPixel(0),
    Modality(modality)
{
    if (pixel != NULL)
        Count = pixel->getCount();
}


DiMonoPixel::DiMonoPixel(const DiPixel *pixel)
  : DiPixel(pixel->getCount()),
    Modality(NULL)
{
}


DiMonoPixel::DiMonoPixel(const DiMonoPixel *pixel, const unsigned long count)
  : DiPixel(count),
    Modality(pixel->Modality)
{
    if (Modality != NULL)
        Modality->addReference();
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMonoPixel::~DiMonoPixel()
{
    if (Modality != NULL)
        Modality->removeReference();
}


/*
**
** CVS/RCS Log:
** $Log: dimopx.cc,v $
** Revision 1.1  1998-11-27 16:16:45  joergr
** Added copyright message.
** Introduced new pixel base class.
**
** Revision 1.3  1998/05/11 14:52:34  joergr
** Added CVS/RCS header to each file.
**
**
*/
