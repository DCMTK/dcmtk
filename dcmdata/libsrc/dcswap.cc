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
** Last Update:   $Author: meichel $
** Revision:      $Revision: 1.8 $
** Status:	  $State: Exp $
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcswap.h"

E_Condition swapIfNecessary(const E_ByteOrder newByteOrder, 
			    const E_ByteOrder oldByteOrder,
			    void * value, const Uint32 byteLength,
			    const size_t valWidth)
{
    if(oldByteOrder != EBO_unknown && newByteOrder != EBO_unknown )
    {
	if (oldByteOrder != newByteOrder && valWidth != 1)
	{
	    if (byteLength == valWidth)
	    {
		if (valWidth == 2)
		    swap2Bytes((Uint8 *)value);
		else if (valWidth == 4)
		    swap4Bytes((Uint8 *)value);
		else
		    swapBytes(value, byteLength, valWidth);
	    }
	    else
		swapBytes(value, byteLength, valWidth);
	}
	return EC_Normal;
    }
    return EC_IllegalCall;
}



void swapBytes(void * value, const Uint32 byteLength, 
			   const size_t valWidth)
{
    register Uint8 save;

    if (valWidth == 2)
    {
	register Uint8 * first = &((Uint8*)value)[0];
	register Uint8 * second = &((Uint8*)value)[1];
	register Uint32 times = byteLength/2;
	while(times--)
	{
	    save = *first;
	    *first = *second;
	    *second = save;
	    first +=2;
	    second +=2;
	}
    }
    else if (valWidth > 2)
    {
	register size_t i; 
	const size_t halfWidth = valWidth/2;
	const size_t offset = valWidth-1;
	register Uint8 *start;
	register Uint8 *end;

	Uint32 times = byteLength/valWidth;
	Uint8  *base = (Uint8 *)value;

	while (times--)
	{
	    i = halfWidth;
	    start = base;
	    end = base+offset;
	    while (i--)
	    {
		save = *start;
		*start++ = *end;
		*end-- = save;
	    } 
	    base += valWidth;
	}
    }
}


Uint16 swapShort(const Uint16 toSwap)
{
	Uint8 * swapped = (Uint8 *)&toSwap;
	swap2Bytes(swapped);
	return * ((Uint16*)swapped);
}


