/*********************************************************************
** 
**  DicomPixelRepresentationTemplate (Header)
**
**  author   : Joerg Riesmeier
**  created  : 15.12.96
**  modified : 13.01.97
**
*********************************************************************/


#ifndef __DIPXREPT_H
#define __DIPXREPT_H

#include "osconfig.h"

#include "diutils.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiPixelRepresentationTemplate
{
 public:
	inline int isSigned() const;

 protected:
	virtual inline EP_Representation getRepresentation() const;
};


/********************************************************************/


inline EP_Representation DiPixelRepresentationTemplate<Uint8>::getRepresentation() const
{
	return EPR_Uint8;
}


inline EP_Representation DiPixelRepresentationTemplate<Sint8>::getRepresentation() const
{
	return EPR_Sint8;
}


inline EP_Representation DiPixelRepresentationTemplate<Uint16>::getRepresentation() const
{
	return EPR_Uint16;
}


inline EP_Representation DiPixelRepresentationTemplate<Sint16>::getRepresentation() const
{
	return EPR_Sint16;
}


inline EP_Representation DiPixelRepresentationTemplate<Uint32>::getRepresentation() const
{
	return EPR_Uint32;
}


inline EP_Representation DiPixelRepresentationTemplate<Sint32>::getRepresentation() const
{
	return EPR_Sint32;
}


inline int DiPixelRepresentationTemplate<Uint8>::isSigned() const
{
	return 0;
}


inline int DiPixelRepresentationTemplate<Uint16>::isSigned() const
{
	return 0;
}


inline int DiPixelRepresentationTemplate<Uint32>::isSigned() const
{
	return 0;
}


inline int DiPixelRepresentationTemplate<Sint8>::isSigned() const
{
	return 1;
}


inline int DiPixelRepresentationTemplate<Sint16>::isSigned() const
{
	return 1;
}


inline int DiPixelRepresentationTemplate<Sint32>::isSigned() const
{
	return 1;
}


#endif
