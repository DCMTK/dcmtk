/*
**
** Author:  Joerg Riesmeier
** Created: 20.12.96
**
** Module:  dicoopx.h
**
** Purpose: DicomColorOutputPixel (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:12 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicoopx.h,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DICOOPX_H
#define __DICOOPX_H

#include "osconfig.h"

#include "diutils.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiColorPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiColorOutputPixel 
{
 public:
    DiColorOutputPixel(const DiColorPixel *, const unsigned long);
    virtual ~DiColorOutputPixel();
    
    virtual EP_Representation getRepresentation() const = 0;
    virtual void *getData() const = 0;
    virtual void *getPlane(const int) const = 0;
    virtual size_t getItemSize() const = 0;

    virtual int writePPM(ostream &) const = 0;
    virtual int writePPM(FILE *) const = 0;
    
    inline unsigned long getCount() const
        { return Count; }

 private:
    unsigned long Count;
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dicoopx.h,v $
** Revision 1.3  1998-05-11 14:53:12  joergr
** Added CVS/RCS header to each file.
**
**
*/
