/*
**
** Author:  Joerg Riesmeier
** Created: 06.01.97
**
** Module:  diargimg.h
**
** Purpose: DiARGBImage (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:07 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diargimg.h,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/
                        

#ifndef __DIARGIMG_H
#define __DIARGIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiARGBImage : public DiColorImage {

 public:
    DiARGBImage(const DiDocument *, const EI_Status);
    virtual ~DiARGBImage();
};


#endif


/*
**
** CVS/RCS Log:
** $Log: diargimg.h,v $
** Revision 1.2  1998-05-11 14:53:07  joergr
** Added CVS/RCS header to each file.
**
**
*/
