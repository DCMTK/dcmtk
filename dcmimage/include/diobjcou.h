/*********************************************************************
** 
**  DicomObjectCounter (Header)
**
**  author   : Joerg Riesmeier
**  created  : 03.01.97
**  modified : 13.01.97
**
*********************************************************************/


#ifndef __DIOBJCOU_H
#define __DIOBJCOU_H

#include "osconfig.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiObjectCounter
{
 public:
	inline void addReference()
		{ Counter++; }
	inline void removeReference()
		{ if (--Counter == 0) delete this; }

 protected:
    DiObjectCounter() : Counter(1)
		{ }
    virtual ~DiObjectCounter()
		{ }
	
 private:
	unsigned long Counter;
};


#endif
