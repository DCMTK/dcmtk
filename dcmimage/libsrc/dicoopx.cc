/*
**
** Author:  Joerg Riesmeier
** Created: 20.12.96
**
** Module:  dicoopx.cc
**
** Purpose: DicomColorOutputPixel (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:26 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/dicoopx.cc,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"

#include "dicoopx.h"
#include "dicopx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiColorOutputPixel::DiColorOutputPixel(const DiColorPixel *pixel, const unsigned long frames)
  : Count(0)
{
    if (pixel != NULL)
        Count = pixel->getCount() / frames;
}


/*--------------*
 *  destructor  *
 *--------------*/

DiColorOutputPixel::~DiColorOutputPixel()
{
}


/*
**
** CVS/RCS Log:
** $Log: dicoopx.cc,v $
** Revision 1.3  1998-05-11 14:52:26  joergr
** Added CVS/RCS header to each file.
**
**
*/
