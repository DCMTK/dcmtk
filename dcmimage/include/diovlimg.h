/*
**
** Author:  Joerg Riesmeier
** Created: 14.01.97
**
** Module:  diovlimg.h
**
** Purpose: DicomOverlayImage (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:25 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diovlimg.h,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIOVLIMG_H
#define __DIOVLIMG_H

#include "osconfig.h"

#include "dimo2img.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiOverlayImage : public DiMono2Image
{
 public:
    DiOverlayImage(const DiDocument *, const EI_Status);
    virtual ~DiOverlayImage();
};


#endif


/*
**
** CVS/RCS Log:
** $Log: diovlimg.h,v $
** Revision 1.2  1998-05-11 14:53:25  joergr
** Added CVS/RCS header to each file.
**
**
*/
