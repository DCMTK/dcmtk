/********************************************************************
** 
**  DicomDocument (Header)
**
**  author   : Joerg Riesmeier
**  created  : 10.12.96
**  modified : 13.02.97
**
*********************************************************************/


#ifndef __DIDOCU_H
#define __DIDOCU_H

#include "osconfig.h"
#include "dctypes.h"

#include "diobjcou.h"

#include <stddef.h>


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmStream;
class DcmObject;
class DcmTagKey;
class DcmSequenceOfItems;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiDocument : public DiObjectCounter
{
 public:
    DiDocument(const char *);
    DiDocument(DcmStream &);
    DiDocument(DcmObject *);
    virtual ~DiDocument();
   
	inline int good() const
		{ return Object != NULL; }
    
	DcmObject *search(const DcmTagKey &, DcmObject * = NULL) const;
	
    unsigned long getValue(const DcmTagKey &, Uint16 &, const unsigned long = 0, DcmObject * = NULL) const;
    unsigned long getValue(const DcmTagKey &, Sint16 &, const unsigned long = 0) const;
    unsigned long getValue(const DcmTagKey &, Uint32 &, const unsigned long = 0) const;
    unsigned long getValue(const DcmTagKey &, Sint32 &, const unsigned long = 0) const;
    unsigned long getValue(const DcmTagKey &, double &, const unsigned long = 0) const;
	
	unsigned long getValue(const DcmTagKey &, const Uint16 *&, DcmObject * = NULL) const;
	unsigned long getValue(const DcmTagKey &, const char *&) const;

	unsigned long getSequence(const DcmTagKey &, DcmSequenceOfItems *&) const;

 protected:
	void Init(DcmStream &);

 private:
    DcmObject *Object;
	
	int DeleteObject;
};


#endif
