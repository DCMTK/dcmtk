/*********************************************************************
** 
**  DicomPalettePixelTemplate (Header)
**
**  author   : Joerg Riesmeier
**  created  : 21.12.96
**  modified : 25.04.97
**
*********************************************************************/


#ifndef __DIPALPXT_H
#define __DIPALPXT_H

#include "osconfig.h"

#include "dicopxt.h"
#include "diluptab.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T1, class T2>
class DiPalettePixelTemplate : public DiColorPixelTemplate<T2>
{
 public:
	DiPalettePixelTemplate(const DiDocument *docu, const DiInputPixel *pixel, DiLookupTable *pal[3], EI_Status &status)
	  : DiColorPixelTemplate<T2>(docu, pixel, 1, status)
	{
		if ((pixel != NULL) && (getCount() > 0) && (status == EIS_Normal))
			convert((const T1 *)pixel->getData(), pal);
	}

 private:
	inline void convert(const T1 *pixel, DiLookupTable *pal[3])
	{
		if (Init(pixel))
		{
			register const T1 *p;
			register T1 value;
			register unsigned long i;
			if (PlanarConfiguration)
			{
				register T2 *q;
				register T1 min;
				register T1 max;
				for (int j = 0; j < 3; j++)
				{
					p = pixel;
					q = Data[j];
					min = (T1)pal[j]->getFirstEntry();
					max = (T1)pal[j]->getLastEntry();
					for (i = 0; i < getCount(); i++)
					{
						value = *(p++);
						if (value <= min)
							*(q++) = (T2)pal[j]->getFirstValue();
						else if (value >= max)
							*(q++) = (T2)pal[j]->getLastValue();
						else
							*(q++) = (T2)pal[j]->getValue(value);
					}
				}
			} 
			else
			{
				p = pixel;
				register int j;
				for (i = 0; i < getCount(); i++)
				{
					value = *(p++);
					for (j = 0; j < 3; j++)
					{
						if (value <= (T1)pal[j]->getFirstEntry())
							Data[j][i] = (T2)pal[j]->getFirstValue();
						else if (value >= (T1)pal[j]->getLastEntry())
							Data[j][i] = (T2)pal[j]->getLastValue();
						else
							Data[j][i] = (T2)pal[j]->getValue(value);
					}
				}
			}
		}
	}

};


#endif
