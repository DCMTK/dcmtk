/*********************************************************************
** 
**  DicomMonoOutputPixel (Header)
**
**  author   : Joerg Riesmeier
**  created  : 19.12.96
**  modified : 13.01.97
**
*********************************************************************/


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
