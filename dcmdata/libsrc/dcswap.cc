/*
**
** Author: Andreas Barth    17.07.95 -- Creation
**                          27.11.95 -- Support for one Byte swapping alg.
**
** Module: dcswap.cc
**
** Purpose:
** ByteOrder functions
**
**
** Last Update:   $Author: andreas $
** Revision:      $Revision: 1.3 $
** Status:	  $State: Exp $
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcswap.h"
#include "dcdebug.h"


void swapBytes(void * value, const Uint32 byteLength, 
			   const size_t valWidth)
{
	Bdebug((6, "dcswap:swapBytes(value,byteLength=%ld,valWidth=%d)", byteLength, valWidth));
    

	if (valWidth > 1)
	{
		Uint8 * src = (Uint8 *)value;
		Uint32 times = byteLength/valWidth;
		Uint8  *last;
		Uint8  *start;
		Uint8  save;

		for (Uint32 t = 0; t<times; t++ )
		{
			start = src + valWidth * t;

			for (size_t i = 0; i < valWidth/2; i++ )
			{
				last = start + valWidth - 1 - i;
				save = *last;
				*last = *(start + i);
				*(start + i) = save;
			}
		}
	}
	Edebug(());
}


const Uint16 swapShort(Uint16 toSwap)
{
	Uint8 * swapped = (Uint8 *)&toSwap;
	Uint8 tmp = swapped[0];
	swapped[0] = swapped[1];
	swapped[1] = tmp;
	return * ((Uint16*)swapped);
}


