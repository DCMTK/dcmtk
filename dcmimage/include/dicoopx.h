/*********************************************************************
** 
**  DicomColorOutputPixel (Header)
**
**  author   : Joerg Riesmeier
**  created  : 20.12.96
**  modified : 22.01.97
**
*********************************************************************/


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
