/*********************************************************************
** 
**  DicomColorOutputPixelTemplate (Header)
**
**  author   : Joerg Riesmeier
**  created  : 19.12.96
**  modified : 05.03.98
**
*********************************************************************/


#ifndef __DICOOPXT_H
#define __DICOOPXT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dicoopx.h"
#include "dipxrept.h"
#include "diutils.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T1, class T2>
class DiColorOutputPixelTemplate : public DiColorOutputPixel, public DiPixelRepresentationTemplate<T2>
{
 public:
	DiColorOutputPixelTemplate(const DiColorPixel *pixel, const unsigned long frame, const unsigned long frames,
		Sint16 shift, const int planar)
	  : DiColorOutputPixel(pixel, frames),
		Data(NULL),
		isPlanar(planar)
	{
		if ((pixel != NULL) && (getCount() > 0))
			convert((const T1 **)(pixel->getData()), frame * getCount(), shift, planar);
	}

	virtual ~DiColorOutputPixelTemplate()
	{
	    delete Data;
	}

	inline EP_Representation getRepresentation() const
	{
		return DiPixelRepresentationTemplate<T2>::getRepresentation();
	}

	inline size_t getItemSize() const
	{
    	return sizeof(T2) * 3;
	}
	
	inline void *getData() const
	{
    	return (void *)Data;
	}
	
	inline void *getPlane(const int plane) const
	{
		if (Data != NULL)
		{
			if (plane <= 0)
	   	 		return (void *)Data;
			else
			{
				if (isPlanar)
					return (void *)(Data + ((plane == 1) ? 1 : 2) * getCount());
				else
					return (void *)(Data + ((plane == 1) ? 1 : 2));
			}
		}
		return NULL;
	}
	
	inline int writePPM(ostream &stream) const
	{
		if (Data != NULL)
		{
			register T2 *p = Data;
			register unsigned long i;
			register int j;
			for (i = 0; i < getCount(); i++)
				for (j = 0; j < 3; j++)
					stream << (unsigned long)*(p++) << " ";		// typecast to resolve problems with 'char'
			return 1;
		}
		return 0;
	}

	inline int writePPM(FILE *stream) const
	{
		if (Data != NULL)
		{
			register T2 *p = Data;
			register unsigned long i;
			register int j;
			for (i = 0; i < getCount(); i++)
				for (j = 0; j < 3; j++)
					fprintf(stream, "%lu ", (unsigned long)*(p++));
			return 1;
		}
		return 0;
	}

 private:
	inline void convert(const T1 *pixel[3], const unsigned long start, Sint16 shift, const int planar)
	{
		if ((pixel[0] != NULL) && (pixel[1] != NULL) && (pixel[2] != NULL))
		{
			Data = new T2[getCount() * 3];
			if (Data != NULL)
			{
				register T2 *q = Data;
				register unsigned long i;
				if (planar)
				{
					register const T1 *p;
					if (shift == 0)
					{
						for (int j = 0; j < 3; j++)
						{
							p = pixel[j] + start;
							for (i = 0; i < getCount(); i++)
								*(q++) = (T2)*(p++);							// copy
						}
					}
					else if (shift < 0)
					{
						shift = -shift;
						for (int j = 0; j < 3; j++)
						{
							p = pixel[j] + start;
							for (i = 0; i < getCount(); i++)
								*(q++) = (T2)(*(p++) << shift);					// expand depth
						}
					}
					else /* shift > 0 */
					{
						for (int j = 0; j < 3; j++)
						{
							p = pixel[j] + start;
							for (i = 0; i < getCount(); i++)
								*(q++) = (T2)(*(p++) >> shift);					// reduce depth
						}
					}
				}
				else
				{
					register int j;
					if (shift == 0)
					{
						for (i = start; i < start + getCount(); i++)
							for (j = 0; j < 3; j++)
								*(q++) = (T2)pixel[j][i];						// copy
					}
					else if (shift < 0)
					{
						shift = -shift;
						for (i = start; i < start + getCount(); i++)
							for (j = 0; j < 3; j++)
								*(q++) = (T2)(pixel[j][i] << shift);			// expand depth
					}
					else /* shift > 0 */
					{
						for (i = start; i < start + getCount(); i++)
							for (j = 0; j < 3; j++)
								*(q++) = (T2)(pixel[j][i] >> shift);			// reduce depth
					}
				}
			}
		}
	}
 
 	T2 *Data;
	int isPlanar;
};


#endif
