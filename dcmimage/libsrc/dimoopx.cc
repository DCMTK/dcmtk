/*
**
** Author:  Joerg Riesmeier
** Created: 19.12.96
**
** Module:  dimoopx.cc
**
** Purpose: DicomMonoOutputPixel (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:33 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/Attic/dimoopx.cc,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"

#include "dimoopx.h"
#include "dimopx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMonoOutputPixel::DiMonoOutputPixel(const DiMonoPixel *pixel, const unsigned long frames)
  : Count(0)
{
    if (pixel != NULL)
        Count = pixel->getCount() / frames;
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMonoOutputPixel::~DiMonoOutputPixel()
{
}


/*
**
** CVS/RCS Log:
** $Log: dimoopx.cc,v $
** Revision 1.3  1998-05-11 14:52:33  joergr
** Added CVS/RCS header to each file.
**
**
*/
