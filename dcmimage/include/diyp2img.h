/*
**
** Author:  Joerg Riesmeier
** Created: 13.03.98
**
** Module:  diyp2img.h
**
** Purpose: DicomYBRPart422Image (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:33 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diyp2img.h,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIYP2IMG_H
#define __DIYP2IMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiYBRPart422Image : public DiColorImage
{
 public:
    DiYBRPart422Image(const DiDocument *, const EI_Status);
    virtual ~DiYBRPart422Image();
};


#endif


/*
**
** CVS/RCS Log:
** $Log: diyp2img.h,v $
** Revision 1.2  1998-05-11 14:53:33  joergr
** Added CVS/RCS header to each file.
**
**
*/
