/*
 *
 *  Copyright (C) 1994-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Andreas Barth
 *
 *  Purpose: byte order functions
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:49:10 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcswap.cc,v $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
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



/*
 * CVS/RCS Log:
 * $Log: dcswap.cc,v $
 * Revision 1.11  2001-06-01 15:49:10  meichel
 * Updated copyright header
 *
 * Revision 1.10  2000/03/08 16:26:42  meichel
 * Updated copyright header.
 *
 * Revision 1.9  1999/03/31 09:25:40  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
