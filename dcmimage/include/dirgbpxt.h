/*********************************************************************
** 
**  DicomRGBPixelTemplate (Header)
**
**  author   : Joerg Riesmeier
**  created  : 21.12.96
**  modified : 25.04.97
**
*********************************************************************/


#ifndef __DIRGBPXT_H
#define __DIRGBPXT_H

#include "osconfig.h"

#include "dicopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T1, class T2>
class DiRGBPixelTemplate : public DiColorPixelTemplate<T2>
{
 public:
	DiRGBPixelTemplate(const DiDocument *docu, const DiInputPixel *pixel, EI_Status &status, const int bits)
	  : DiColorPixelTemplate<T2>(docu, pixel, 3, status)
	{
		if ((pixel != NULL) && (getCount() > 0) && (status == EIS_Normal))
			convert((const T1 *)pixel->getData(), bits);
	}

 private:
	inline void convert(const T1 *pixel, const int bits)
	{
		if (Init(pixel))
		{
			register const T1 *p = pixel;
			register unsigned long i;
			const T1 offset = (const T1)maxval(bits - 1);
			if (PlanarConfiguration)
			{
				register T2 *q;
				for (int j = 0; j < 3; j++)							/* for all planes ... */
				{
					q = Data[j];
					for (i = 0; i < getCount(); i++)
						*(q++) = removeSign(*(p++), offset);		/* ... copy pixel */
				}
			} 
			else
			{
				register int j;
				for (i = 0; i < getCount(); i++)					/* for all pixel ... */
					for (j = 0; j < 3; j++)
						Data[j][i] = removeSign(*(p++), offset);	/* ... copy planes */ 
			}
		}
	}

};


#endif
