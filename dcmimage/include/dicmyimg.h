/*
**
** Author:  Joerg Riesmeier
** Created: 05.01.97
**
** Module:  dicmyimg.h
**
** Purpose: DicomCMYKImage (Header) 
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:09 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicmyimg.h,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DICMYIMG_H
#define __DICMYIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiCMYKImage : public DiColorImage
{
 public:
    DiCMYKImage(const DiDocument *, const EI_Status);
    virtual ~DiCMYKImage();
};


#endif
                        

/*
**
** CVS/RCS Log:
** $Log: dicmyimg.h,v $
** Revision 1.2  1998-05-11 14:53:09  joergr
** Added CVS/RCS header to each file.
**
**
*/
