/*********************************************************************
** 
**  DicomClipTemplate (Header)
**
**  author   : Joerg Riesmeier
**  created  : 03.01.97
**  modified : 25.04.97
**
*********************************************************************/


#ifndef __DICLIPT_H
#define __DICLIPT_H

#include "osconfig.h"
#include "dctypes.h"

#include "ditranst.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiClipTemplate : public DiTransTemplate<T>
{
 public:
	DiClipTemplate(const int planes, const Uint16 src_x, const Uint16 src_y, const Uint16 left, const Uint16 top,
		const Uint16 dest_x, const Uint16 dest_y, const Uint32 frames)
	  : DiTransTemplate<T>(planes, src_x, src_y, dest_x, dest_y, frames), Left(left), Top(top)
	{
	}

	virtual ~DiClipTemplate()
	{
	}

	inline void clip(const T *src[], T *dest[])
	{
		if ((src != NULL) && (dest != NULL))
		{
			if ((Left == 0) && (Top == 0) && (Src_X == Dest_X) && (Src_Y == Dest_Y))
				copyPixel(src, dest);
			else
				clipPixel(src, dest);
		}
	}

	
 protected:
	const Uint16 Left;
	const Uint16 Top;

 private:
	inline void clipPixel(const T *src[], T *dest[])
	{
		const unsigned long x_feed = Src_X - Dest_X;
		const unsigned long y_feed = (unsigned long)(Src_Y - Dest_Y) * (unsigned long)Src_X;
		register Uint16 x;
		register Uint16 y;
		register const T *p;
		register T *q;
		for (int j = 0; j < Planes; j++)
		{
			p = src[j] + (unsigned long)Top * (unsigned long)Src_X + Left;
			q = dest[j];
			for (unsigned long f = 0; f < Frames; f++)
			{
				for (y = 0; y < Dest_Y; y++)
				{
						for (x = 0; x < Dest_X; x++)
						*(q++) = *(p++);
					p += x_feed;
				}
				p += y_feed;
			}
		}
	}
};


#endif
