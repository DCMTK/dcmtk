/*
**
** Author:  Joerg Riesmeier
** Created: 17.12.96
**
** Module:  dimopx.cc
**
** Purpose: DicomMonochromePixel (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:34 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/Attic/dimopx.cc,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"

#include "dimopx.h"
#include "dicopx.h"
#include "dimomod.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMonoPixel::DiMonoPixel(const unsigned long count)
  : Modality(NULL),
    Count(count)
{
}


DiMonoPixel::DiMonoPixel(const DiInputPixel *pixel, DiMonoModality *modality)
  : Modality(modality),
    Count(0)
{
    if (pixel != NULL)
        Count = pixel->getCount();
}


DiMonoPixel::DiMonoPixel(const DiColorPixel *pixel)
  : Modality(NULL),
    Count(pixel->getCount())
{
}


DiMonoPixel::DiMonoPixel(const DiMonoPixel *pixel, const unsigned long count)
  : Modality(pixel->Modality),
    Count(count)
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
** Revision 1.3  1998-05-11 14:52:34  joergr
** Added CVS/RCS header to each file.
**
**
*/
