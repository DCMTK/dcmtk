/*
**
** Author:  Joerg Riesmeier
** Created: 19.12.96
**
** Module:  dimoopx.h
**
** Purpose: DicomMonoOutputPixel (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:22 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dimoopx.h,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIMOOPX_H
#define __DIMOOPX_H

#include "osconfig.h"

#include "diutils.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiMonoPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiMonoOutputPixel 
{
 public:
    DiMonoOutputPixel(const DiMonoPixel *, const unsigned long);
    virtual ~DiMonoOutputPixel();
    
    virtual EP_Representation getRepresentation() const = 0;
    virtual void *getData() const = 0;
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
** $Log: dimoopx.h,v $
** Revision 1.3  1998-05-11 14:53:22  joergr
** Added CVS/RCS header to each file.
**
**
*/
