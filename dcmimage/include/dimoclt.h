/*********************************************************************
** 
**  DicomMonochromeClipTemplate (Header)
**
**  author   : Joerg Riesmeier
**  created  : 01.01.97
**  modified : 25.04.97
**
*********************************************************************/


#ifndef __DIMOCLT_H
#define __DIMOCLT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimopxt.h"
#include "diclipt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiMonoClipTemplate : public DiMonoPixelTemplate<T>, protected DiClipTemplate<T>
{
 public:
	DiMonoClipTemplate(const DiMonoPixel *pixel, const Uint16 src_cols, const Uint16 src_rows, const Uint16 left,
		const Uint16 top, const Uint16 columns, const Uint16 rows, const Uint32 frames)
	  : DiMonoPixelTemplate<T>(pixel, (unsigned long)columns * (unsigned long)rows * frames),
		DiClipTemplate<T>(1, src_cols, src_rows, left, top, columns, rows, frames)
	{
		if ((pixel != NULL) && (pixel->getCount() > 0))
		{
			clip((const T *)pixel->getData());
			determineMinMax();
		}
	}

	~DiMonoClipTemplate()
	{
	}

 private:
	inline void clip(const T *pixel)
	{
		if (pixel != NULL)
		{
			Data = new T[getCount()];
			if (Data != NULL)
				DiClipTemplate<T>::clip(&pixel, &Data);
		}
	}
};


#endif
