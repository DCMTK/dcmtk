/*
**
** Author:  Joerg Riesmeier
** Created: 21.12.96
**
** Module:  dipalimg.h
**
** Purpose: DicomPaletteImage (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:26 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dipalimg.h,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIPALIMG_H
#define __DIPALIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiPaletteImage : public DiColorImage {

 public:
    DiPaletteImage(const DiDocument *, const EI_Status);
    virtual ~DiPaletteImage();
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dipalimg.h,v $
** Revision 1.2  1998-05-11 14:53:26  joergr
** Added CVS/RCS header to each file.
**
**
*/
