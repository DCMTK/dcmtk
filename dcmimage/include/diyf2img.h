/*
**
** Author:  Joerg Riesmeier
** Created: 13.03.98
**
** Module:  diyf2img.h
**
** Purpose: DicomYBR422Image (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:32 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diyf2img.h,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIYF2IMG_H
#define __DIYF2IMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiYBR422Image : public DiColorImage
{
 public:
    DiYBR422Image(const DiDocument *, const EI_Status);
    virtual ~DiYBR422Image();
};


#endif


/*
**
** CVS/RCS Log:
** $Log: diyf2img.h,v $
** Revision 1.2  1998-05-11 14:53:32  joergr
** Added CVS/RCS header to each file.
**
**
*/
