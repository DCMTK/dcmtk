/*
**
** Author:  Joerg Riesmeier
** Created: 12.01.97
**
** Module:  dihsvimg.h
**
** Purpose: DicomHSVImage (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:15 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dihsvimg.h,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIHSVIMG_H
#define __DIHSVIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiHSVImage : public DiColorImage
{
 public:
    DiHSVImage(const DiDocument *, const EI_Status);
    virtual ~DiHSVImage();
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dihsvimg.h,v $
** Revision 1.2  1998-05-11 14:53:15  joergr
** Added CVS/RCS header to each file.
**
**
*/
