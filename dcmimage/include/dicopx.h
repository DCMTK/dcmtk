/*
**
** Author:  Joerg Riesmeier
** Created: 20.12.96
**
** Module:  dicopx.h
**
** Purpose: DicomColorPixel (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:13 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicopx.h,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DICOPX_H
#define __DICOPX_H

#include "osconfig.h"

#include "diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiDocument;
class DiInputPixel;
class DiMonoPixel;
class DiColorPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/


class DiColorPixel 
{
 public:
    DiColorPixel(const DiDocument *, const DiInputPixel *, const Uint16, EI_Status &, const Uint16 = 0);
    virtual ~DiColorPixel();
    
    virtual EP_Representation getRepresentation() const = 0;
    virtual void *getData() const = 0;
    virtual void *createDIB(const Uint16, const Uint16, const unsigned long, Sint16) const = 0;
    
    inline unsigned long getCount() const
        { return Count; };

 protected:
    DiColorPixel(const DiMonoPixel *);
    DiColorPixel(const DiColorPixel *, const unsigned long);

    int PlanarConfiguration;

 private:
    unsigned long Count;
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dicopx.h,v $
** Revision 1.3  1998-05-11 14:53:13  joergr
** Added CVS/RCS header to each file.
**
**
*/
