/*********************************************************************
** 
**  DicomScaleTemplates (Header)
**
**  author   : Joerg Riesmeier
**  created  : 31.12.96
**  modified : 25.04.97
**
*********************************************************************/


#ifndef __DISCALET_H
#define __DISCALET_H

#include "osconfig.h"
#include "dctypes.h"

#include "ditranst.h"
#include "dipxrept.h"
#include "diutils.h"

#include <iostream.h>


#define SCALE 4096
#define HALFSCALE 2048


/*
 *   help function to set scaling values
 */

static inline void setScaleValues(Uint16 data[], const Uint16 min, const Uint16 max)
{
	register Uint16 remainder = max % min;
	Uint16 step0 = max / min;
	Uint16 step1 = max / min;
	if (remainder > (Uint16)(min / 2))
	{
		remainder = min - remainder;
		step0++;
	}
	else
		step1++;
	const double count = (double)min / ((double)remainder + 1);
	register Uint16 i;
	register double c = count;
	for (i = 0; i < min; i++)
	{
		if ((i >= (Uint16)c) && (remainder > 0))
		{
			remainder--;
			c += count;
			data[i] = step1;
		}
		else 
			data[i] = step0;
	}
}


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiScaleTemplate : public DiTransTemplate<T>
{
 public:
	DiScaleTemplate(const int planes, const Uint16 src_x, const Uint16 src_y, const Uint16 dest_x,
		const Uint16 dest_y, const Uint32 frames, const int bits = 0)
  	  : DiTransTemplate<T>(planes, src_x, src_y, dest_x, dest_y, frames, bits)
	{
	}

	virtual ~DiScaleTemplate()
	{
	}
	
	inline void scale(const T *src[], T *dest[], const int interpolate)
	{
		if ((src != NULL) && (dest != NULL))
		{
			if ((Src_X == Dest_X) && (Src_Y == Dest_Y))
				copyPixel(src, dest);
			else if (interpolate && (Bits <= MAX_INTERPOLATION_BITS))
				interpolatePixel(src, dest);
			else if ((Dest_X % Src_X == 0) && (Dest_Y % Src_Y == 0))
				replicatePixel(src, dest);
			else if ((Src_X % Dest_X == 0) && (Src_Y % Dest_Y == 0))
				suppressPixel(src, dest);
			else
				scalePixel(src, dest);
		}
	}

 private:

   /*
 	*   enlarge image by an integer factor
 	*/

	inline void replicatePixel(const T *src[], T *dest[])
	{
		const Uint16 x_factor = Dest_X / Src_X;
		const Uint16 y_factor = Dest_Y / Src_Y;
		const T *sp;
		register Uint16 x;
		register Uint16 y;
			register Uint16 dx;
		register Uint16 dy;
		register const T *p;
		register T *q;
		register T value;
		for (int j = 0; j < Planes; j++)
		{
			sp = src[j];
			q = dest[j];
			for (Uint32 f = 0; f < Frames; f++)
			{
				for (y = 0; y < Src_Y; y++)
				{
					for (dy = 0; dy < y_factor; dy++)
					{
						for (x = 0, p = sp; x < Src_X; x++)
						{
							value = *(p++);
							for (dx = 0; dx < x_factor; dx++)
								*(q++) = value;
						}
					}
					sp += Src_X;
				}
			}
		}
	}
	

   /*
 	*   shrink image by an integer divisor
 	*/

	inline void suppressPixel(const T *src[], T *dest[])
	{
		const Uint16 x_divisor = Src_X / Dest_X;
		const unsigned long y_feed = (unsigned long)(Src_Y / Dest_Y - 1) * (unsigned long)Src_X;
		register Uint16 x;
		register Uint16 y;
		register const T *p;
		register T *q;
		for (int j = 0; j < Planes; j++)
			{
			p = src[j];
			q = dest[j];
			for (Uint32 f = 0; f < Frames; f++)
			{
				for (y = 0; y < Dest_Y; y++)
				{
					for (x = 0; x < Dest_X; x++)
						{
						*(q++) = *p;
							p += x_divisor;
					}
					p += y_feed;
				}
			}
		}
	}
	

   /*
 	*   free scaling method without interpolation (necessary for overlays) ... it was a hard job ;-)
 	*/

	inline void scalePixel(const T *src[], T *dest[])
	{
		const Uint16 xmin = (Dest_X < Src_X) ? Dest_X : Src_X;		// minimum width
		const Uint16 ymin = (Dest_Y < Src_Y) ? Dest_Y : Src_Y;		// minimum height
		Uint16 *x_step = new Uint16[xmin];
		Uint16 *y_step = new Uint16[ymin];
			Uint16 *x_fact = new Uint16[xmin];
		Uint16 *y_fact = new Uint16[ymin];
		if ((x_step != NULL) && (y_step != NULL) && (x_fact != NULL) && (y_fact != NULL))
		{
			register Uint16 x;
				register Uint16 y;
			if (Dest_X < Src_X)
				setScaleValues(x_step, Dest_X, Src_X);
			else if (Dest_X > Src_X)
				setScaleValues(x_fact, Src_X, Dest_X);
			if (Dest_X <= Src_X)
			{
				for (x = 0; x < xmin; x++)						// initialize with default values
					x_fact[x] = 1;
			}
			if (Dest_X >= Src_X)
			{
				for (x = 0; x < xmin; x++)						// initialize with default values
					x_step[x] = 1;
			}
			if (Dest_Y < Src_Y)
				setScaleValues(y_step, Dest_Y, Src_Y);
			else if (Dest_Y > Src_Y)
				setScaleValues(y_fact, Src_Y, Dest_Y);
			if (Dest_Y <= Src_Y)
			{
				for (y = 0; y < ymin; y++)						// initialize with default values
					y_fact[y] = 1;
			}
			if (Dest_Y >= Src_Y)
			{
				for (y = 0; y < ymin; y++)						// initialize with default values
					y_step[y] = 1;
			}
			const T *sp;
			register Uint16 dx;
			register Uint16 dy;
			register const T *p;
			register T *q;
			register T value;
			for (int j = 0; j < Planes; j++)
			{
				sp = src[j];
				q = dest[j];
				for (Uint32 f = 0; f < Frames; f++)
				{
					for (y = 0; y < ymin; y++)
					{
						for (dy = 0; dy < y_fact[y]; dy++)
						{
							for (x = 0, p = sp; x < xmin; x++)
							{
								value = *p;
								for (dx = 0; dx < x_fact[x]; dx++)
									*(q++) = value;
									 p += x_step[x];
							}
						}
						sp += ((unsigned long)y_step[y] * (unsigned long)Src_X);
					}
				}
			}
		}
		delete x_step;
		delete y_step;
		delete x_fact;
		delete y_fact;
	}


	inline void interpolatePixel(const T *src[], T *dest[])
	{
		/*
		 *   based on scaling algorithm of "Extended Portable Bitmap Toolkit" (pbmplus10dec91)
		 *   (adapted to be used with signed pixel representation and inverse images - mono2)
		 */

		register Uint16 x;
		register Uint16 y;
		register const T *p;
		register T *q;
		T const *sp = NULL;							// initialization avoids compiler warning
		T const *fp;
		T *sq;

		const unsigned long sxscale = (unsigned long)(((double)Dest_X / (double)Src_X) * SCALE);
		const unsigned long syscale = (unsigned long)(((double)Dest_Y / (double)Src_Y) * SCALE);
		DiPixelRepresentationTemplate<T> rep;
		const signed long maxvalue = maxval(Bits - rep.isSigned());

		T *xtemp = new T[Src_X];
		signed long *xvalue = new signed long[Src_X];

		if ((xtemp == NULL) || (xvalue == NULL))
		{
			cerr << "ERROR: can't allocate temporary buffers for interpolation scaling !" << endl;
	
			const unsigned long count = (unsigned long)Dest_X * (unsigned long)Dest_Y * Frames; 
			register unsigned long i;
			for (int j = 0; j < Planes; j++)
			{
				register T *q = dest[j];
				for (i = 0; i < count; i++)					// delete destination buffer
					*(q++) = 0;
			}
		}
		else
		{ 
			for (int j = 0; j < Planes; j++)
			{
				fp = src[j];
				sq = dest[j];
				for (Uint32 f = 0; f < Frames; f++)
				{
					for (x = 0; x < Src_X; x++)
						xvalue[x] = HALFSCALE;
						register unsigned long yfill = SCALE;
					register unsigned long yleft = syscale;
					register int yneed = 1;
					int ysrc = 0;
						for (y = 0; y < Dest_Y; y++)
					{
						if (Src_Y == Dest_Y)
							{
							sp = fp;
							for (x = 0, p = sp, q = xtemp; x < Src_X; x++)
								*(q++) = *(p++);
							fp += Src_X;
						}
						else
						{
							while (yleft < yfill)
							{
								if (yneed && (ysrc < (int)Src_Y))
								{
									sp = fp;
									fp += Src_X;
									ysrc++;
								}
								for (x = 0, p = sp; x < Src_X; x++)
									xvalue[x] += yleft * (signed long)(*(p++));
								yfill -= yleft;
								yleft = syscale;
								yneed = 1;
							}
							if (yneed && (ysrc < (int)Src_Y))
							{
								sp = fp;
								fp += Src_X;
								ysrc++;
								yneed = 0;
							}
							for (x = 0, p = sp, q = xtemp; x < Src_X; x++)
							{
								register signed long v = xvalue[x] + yfill * (signed long)(*(p++));
								v /= SCALE;
								*(q++) = (T)((v > maxvalue) ? maxvalue : v);
								xvalue[x] = HALFSCALE;
							}
							yleft -= yfill;
							if (yleft == 0)
							{
								yleft = syscale;
								yneed = 1;
							}
							yfill = SCALE;
						}
						if (Src_X == Dest_X)
						{
							for (x = 0, p = xtemp, q = sq; x < Dest_X; x++)
								*(q++) = *(p++);
							sq += Dest_X;
						}
						else
						{
							register signed long v = HALFSCALE;
							register unsigned long xfill = SCALE;
							register unsigned long xleft;
							register int xneed = 0;
							q = sq;
							for (x = 0, p = xtemp; x < Src_X; x++, p++)
							{
								xleft = sxscale;
								while (xleft >= xfill)
								{
									if (xneed)
									{
										q++;
										v = HALFSCALE;
									}
									v += xfill * (signed long)(*p);
									v /= SCALE;
									*q = (T)((v > maxvalue) ? maxvalue : v);
									xleft -= xfill;
									xfill = SCALE;
									xneed = 1;
								}
								if (xleft > 0)
								{
									if (xneed)
									{
										q++;
										v = HALFSCALE;
										xneed = 0;
									}
									v += xleft * (signed long)(*p);
									xfill -= xleft;
								}
							}
							if (xfill > 0)
								v += xfill * (signed long)(*(--p));
							if (!xneed)
							{
								v /= SCALE;
								*q = (T)((v > maxvalue) ? maxvalue : v);
							}
							sq += Dest_X;
						}
					}
				}
			}
		}
		delete xtemp;
		delete xvalue;
	}

};


#endif
