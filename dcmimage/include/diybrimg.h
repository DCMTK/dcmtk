/*
**
** Author:  Joerg Riesmeier
** Created: 12.03.98
**
** Module:  diybrimg.h
**
** Purpose: DicomYBRImage (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:31 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diybrimg.h,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIYBRIMG_H
#define __DIYBRIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiYBRImage : public DiColorImage
{
 public:
    DiYBRImage(const DiDocument *, const EI_Status);
    virtual ~DiYBRImage();
};


#endif


/*
**
** CVS/RCS Log:
** $Log: diybrimg.h,v $
** Revision 1.2  1998-05-11 14:53:31  joergr
** Added CVS/RCS header to each file.
**
**
*/
