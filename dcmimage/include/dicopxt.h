/*********************************************************************
** 
**  DicomColorPixelTemplate (Header)
**
**  author   : Joerg Riesmeier
**  created  : 20.12.96
**  modified : 25.04.97
**
*********************************************************************/


#ifndef __DICOPXT_H
#define __DICOPXT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dicopx.h"
#include "dipxrept.h"


/********************************************************************/


inline Uint8 removeSign(const Uint8 value, const Uint8)
{
    return value;
}


inline Uint16 removeSign(const Uint16 value, const Uint16)
{
    return value;
}


inline Uint32 removeSign(const Uint32 value, const Uint32)
{
    return value;
}


inline Uint8 removeSign(const Sint8 value, const Sint8 offset)
{
    return (Uint8)((Sint16)value + (Sint16)offset + 1);
}


inline Uint16 removeSign(const Sint16 value, const Sint16 offset)
{
    return (Uint16)((Sint32)value + (Sint32)offset + 1);
}


inline Uint32 removeSign(const Sint32 value, const Sint32 offset)
{
    return (value < 0) ? (Uint32)(value + offset + 1) : (Uint32)value + (Uint32)offset + 1;
}


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiColorPixelTemplate : public DiColorPixel, public DiPixelRepresentationTemplate<T>
{
 public:
	DiColorPixelTemplate(const DiDocument *docu, const DiInputPixel *pixel, const Uint16 samples, EI_Status &status)
	  : DiColorPixel(docu, pixel, samples, status)
	{
 		Data[0] = NULL;
		Data[1] = NULL;
		Data[2] = NULL;
	}

	virtual ~DiColorPixelTemplate()
	{
		delete Data[0];
		delete Data[1];
		delete Data[2];
	}

	inline EP_Representation getRepresentation() const
	{
		return DiPixelRepresentationTemplate<T>::getRepresentation();
	}

	inline void *getData() const
	{
		return (void *)Data;
	}
	
	inline void *createDIB(const Uint16 width, const Uint16 height, const unsigned long frame, Sint16 shift) const
	{
		if ((Data[0] != NULL) && (Data[1] != NULL) && (Data[2] != NULL))
		{
			const int gap = width & 0x3;						// each line has to start at 32-bit-address
			Uint8 *data = new Uint8[(unsigned long)(width + gap) * (unsigned long)height * 3];
			if (data != NULL)
			{
				const unsigned long start = (unsigned long)width * (unsigned long)height * frame;
				register const T *r = Data[0] + start;
				register const T *g = Data[1] + start;
				register const T *b = Data[2] + start;
				register Uint8 *q = data;
				register Uint16 x;
				register Uint16 y;
				if (shift == 0)
				{
					for (y = 0; y < height; y++)
					{
						for (x = 0; x < width; x++)
						{
							*(q++) = (Uint8)(*(b++));
							*(q++) = (Uint8)(*(g++));
							*(q++) = (Uint8)(*(r++));
						}
						q += gap;
					}
				}
				else if (shift < 0)
				{
					shift = -shift;
					for (y = 0; y < height; y++)
					{
						for (x = 0; x < width; x++)
						{
							*(q++) = (Uint8)(*(b++) << shift);
							*(q++) = (Uint8)(*(g++) << shift);
							*(q++) = (Uint8)(*(r++) << shift);
						}
						q += gap;
					}
				}
				else
				{
					for (y = 0; y < height; y++)
					{
						for (x = 0; x < width; x++)
						{
							*(q++) = (Uint8)(*(b++) >> shift);
							*(q++) = (Uint8)(*(g++) >> shift);
							*(q++) = (Uint8)(*(r++) >> shift);
						}
						q += gap;
					}
				}
    			return (void *)data;
			}
		}
		return NULL;
	}

 protected: 
	DiColorPixelTemplate(const DiMonoPixel *pixel)
	  : DiColorPixel(pixel)
	{
 		Data[0] = NULL;
		Data[1] = NULL;
		Data[2] = NULL;
	}

	DiColorPixelTemplate(const DiColorPixel *pixel, const unsigned long count)
	  : DiColorPixel(pixel, count)
	{
 		Data[0] = NULL;
		Data[1] = NULL;
		Data[2] = NULL;
	}

	inline int Init(const void *pixel)
	{
		if (pixel != NULL)
		{
			Data[0] = new T[getCount()];
			Data[1] = new T[getCount()];
			Data[2] = new T[getCount()];
			return (Data[0] != NULL) && (Data[1] != NULL) && (Data[2] != NULL);
		}
		else
			return 0;
	}
 
 	T *Data[3];
};


#endif
