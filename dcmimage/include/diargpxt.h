/*********************************************************************
** 
**  DicomARGBPixelTemplate (Header)   -   UNTESTED !!!
**
**  author   : Joerg Riesmeier
**  created  : 06.01.97
**  modified : 25.04.97
**
*********************************************************************/


#ifndef __DIARGPXT_H
#define __DIARGPXT_H

#include "osconfig.h"

#include "dicopxt.h"
#include "diluptab.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T1, class T2>
class DiARGBPixelTemplate : public DiColorPixelTemplate<T2>
{
 public:
	DiARGBPixelTemplate(const DiDocument *docu, const DiInputPixel *pixel, DiLookupTable *palette[3],
		EI_Status &status, const int bits)
  	  : DiColorPixelTemplate<T2>(docu, pixel, 4, status)
	{
		if ((pixel != NULL) && (getCount() > 0) && (status == EIS_Normal))
			convert((const T1 *)pixel->getData(), palette, bits);
	}

 private:
	inline void convert(const T1 *pixel, DiLookupTable *pal[3], const int bits)
	{
		if (Init(pixel))
		{
			register unsigned long i;
			register T1 value;
			register int j;
			const T1 offset = (const T1)maxval(bits - 1);
			if (PlanarConfiguration)
			{
				register const T1 *a = pixel;									// points to alpha plane
				const T1 *rgb[3];
				rgb[0] = a + getCount();										// points to red plane
				rgb[1] = rgb[0] + getCount();									// points to green plane
				rgb[2] = rgb[1] + getCount();									// points to blue plane
				for (i = 0; i < getCount(); i++)
				{
					value = *(a++);												// get alpha value
					if (value > 0)
					{
						for (j = 0; j < 3; j++)									// set palette color
						{
							if ((Sint32)value <= pal[j]->getFirstEntry())
								Data[j][i] = (T2)pal[j]->getFirstValue();
							else if ((Sint32)value >= pal[j]->getLastEntry())
								Data[j][i] = (T2)pal[j]->getLastValue();
							else
								Data[j][i] = (T2)pal[j]->getValue(value);
							rgb[j]++;											// skip RGB values
						}
					}
					else
					{
						for (j = 0; j < 3; j++)
							Data[j][i] = (T2)removeSign(*(rgb[j]++), offset);	// copy RGB values
					}
				}
			} 
			else
			{
				register const T1 *p = pixel;
				for (i = 0; i < getCount(); i++)
				{
					value = *(p++);												// get alpha value
					if (value > 0)
					{
						for (j = 0; j < 3; j++)									// set palette color
						{
							if ((Sint32)value <= pal[j]->getFirstEntry())
								Data[j][i] = (T2)pal[j]->getFirstValue();
							else if ((Sint32)value >= pal[j]->getLastEntry())
								Data[j][i] = (T2)pal[j]->getLastValue();
							else
								Data[j][i] = (T2)pal[j]->getValue(value);
						}
						p += 3;													// skip RGB values
					}
					else
					{
						for (j = 0; j < 3; j++)
							Data[j][i] = (T2)removeSign(*(p++), offset);		// copy RGB values
					}
				}
			}
		}
	}

};


#endif
