/*
**
** Author:  Joerg Riesmeier
** Created: 16.12.96
**
** Module:  dimo2img.h
**
** Purpose: DicomMonochrome2Image (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:19 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dimo2img.h,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIMO2IMG_H
#define __DIMO2IMG_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiMono2Image : public DiMonoImage
{
 public:
    DiMono2Image(const DiDocument *, const EI_Status);
    DiMono2Image(const DiColorImage *, const double, const double, const double);
    virtual ~DiMono2Image();

    void *getOutputData(const unsigned long, const int, const int = 0);

    DiImage *scale(const unsigned long, const unsigned long, const int) const;
    DiImage *clip(const unsigned long, const unsigned long, const unsigned long, const unsigned long) const;
    
    DiImage *makeMonochrome(const double, const double, const double) const;

 protected:
    DiMono2Image(const DiDocument *, const EI_Status, const int);
    DiMono2Image(const DiMonoImage *, const Uint16, const Uint16, const int = 0);
    DiMono2Image(const DiMonoImage *, const Uint16, const Uint16, const Uint16, const Uint16);
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimo2img.h,v $
** Revision 1.2  1998-05-11 14:53:19  joergr
** Added CVS/RCS header to each file.
**
**
*/
