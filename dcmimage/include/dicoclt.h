/*********************************************************************
** 
**  DicomColorClipTemplate (Header)
**
**  author   : Joerg Riesmeier
**  created  : 01.01.97
**  modified : 25.04.97
**
*********************************************************************/


#ifndef __DICOCLT_H
#define __DICOCLT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dicopx.h"
#include "diclipt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiColorClipTemplate : public DiColorPixelTemplate<T>, protected DiClipTemplate<T>
{
 public:
	DiColorClipTemplate(const DiColorPixel *pixel, const Uint16 src_cols, const Uint16 src_rows, const Uint16 left,
		const Uint16 top, const Uint16 columns, const Uint16 rows, const Uint32 frames)
	  : DiColorPixelTemplate<T>(pixel, (unsigned long)columns * (unsigned long)rows * frames),
		DiClipTemplate<T>(3, src_cols, src_rows, left, top, columns, rows, frames)
	{
		if ((pixel != NULL) && (pixel->getCount() > 0))
			clip((const T **)pixel->getData());
	}

	virtual ~DiColorClipTemplate()
	{
	}

 private:
	inline void clip(const T *pixel[3])
	{
		if (Init(pixel))
			DiClipTemplate<T>::clip(pixel, Data);
	}

};


#endif
