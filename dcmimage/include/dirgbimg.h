/*
**
** Author:  Joerg Riesmeier
** Created: 20.12.96
**
** Module:  dirgbimg.h
**
** Purpose: DicomRGBImage (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:28 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dirgbimg.h,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIRGBIMG_H
#define __DIRGBIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiRGBImage : public DiColorImage
{
 public:
    DiRGBImage(const DiDocument *, const EI_Status);
    virtual ~DiRGBImage();
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dirgbimg.h,v $
** Revision 1.2  1998-05-11 14:53:28  joergr
** Added CVS/RCS header to each file.
**
**
*/
