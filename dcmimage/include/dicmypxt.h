/*********************************************************************
** 
**  DicomCMYKPixelTemplate (Header)
**
**  author   : Joerg Riesmeier
**  created  : 05.01.97
**  modified : 30.04.97
**
*********************************************************************/


#ifndef __DICMYPXT_H
#define __DICMYPXT_H

#include "osconfig.h"

#include "dicopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T1, class T2>
class DiCMYKPixelTemplate : public DiColorPixelTemplate<T2>
{
 public:
	DiCMYKPixelTemplate(const DiDocument *docu, const DiInputPixel *pixel, EI_Status &status, const int bits)
	  : DiColorPixelTemplate<T2>(docu, pixel, 4, status)
	{
		if ((pixel != NULL) && (getCount() > 0) && (status == EIS_Normal))
			convert((const T1 *)pixel->getData(), bits);
	}
	
	virtual ~DiCMYKPixelTemplate()
	{
	}

 private:
	inline void convert(const T1 *pixel, const int bits)
	{
		if (Init(pixel))
		{
			register const T1 *p = pixel;
			register unsigned long i;
			const T2 maxvalue = (const T2)maxval(bits);
			const T1 offset = (const T1)maxval(bits - 1);
			if (PlanarConfiguration)
			{
				register const T1 *k;
				register T2 *q;
				for (int j = 0; j < 3; j++)
				{
					q = Data[j];
					k = pixel + 3 * getCount();						// beginning of 'black' plane
					for (i = 0; i < getCount(); i++)
						*(q++) = maxvalue - removeSign(*(p++), offset) - removeSign(*(k++), offset);
				}
			} 
			else
			{
				register T1 k;
				register int j;
				for (i = 0; i < getCount(); i++)
				{
					k = *(p + 3);
					for (j = 0; j < 3; j++)
						Data[j][i] = maxvalue - removeSign(*(p++), offset) - removeSign(k, offset);
					p++;											// skip 'black'
				}
			}
		}
	}
};


#endif
