/*********************************************************************
** 
**  DicomColorMonochromeTemplate (Header)
**
**  author   : Joerg Riesmeier
**  created  : 05.01.97
**  modified : 25.04.97
**
*********************************************************************/


#ifndef __DICOMOT_H
#define __DICOMOT_H

#include "osconfig.h"

#include "dimopxt.h"
#include "dicopx.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiColorMonoTemplate : public DiMonoPixelTemplate<T>
{
 public:
	DiColorMonoTemplate(const DiColorPixel *pixel, const double red, const double green, const double blue)
	  : DiMonoPixelTemplate<T>(pixel)
	{
		if ((pixel != NULL) && (pixel->getCount() > 0))
		{
			convert((const T **)pixel->getData(), red, green, blue);
			determineMinMax();
		}
	}
	
	virtual ~DiColorMonoTemplate()
	{
	}

 private:
	inline void convert(const T *pixel[3], const double red, const double green, const double blue)
	{
		if (pixel != NULL)
		{
			Data = new T[getCount()];
			if (Data != NULL)
			{
				register const T *r = pixel[0];
				register const T *g = pixel[1];
				register const T *b = pixel[2];
				register T *q = Data;
				register unsigned long i;
				for (i = 0; i < getCount(); i++)
					*(q++) = (T)((double)*(r++) * red + (double)*(g++) * green + (double)*(b++) * blue);
			}
		}
	}

};


#endif
