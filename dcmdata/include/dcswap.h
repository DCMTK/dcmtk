/*
**
** Author: Andreas Barth    17.07.95 -- Creation
**                          27.11.95 -- only one swapping mechanism
**
** Module: dcswap.h
**
** Purpose:
** ByteOrder functions
**
**
** Last Update:   $Author: meichel $
** Revision:      $Revision: 1.5 $
** Status:	  $State: Exp $
**
*/

#ifndef DCSWAP_H
#define DCSWAP_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include "dctypes.h"
#include "dcerror.h"
#include "dcxfer.h"

// Swap byteLength bytes, if newByteOrder != oldByteOrder. The bytes are
// parted in valWidth bytes, each swapped separately
E_Condition swapIfNecessary(const E_ByteOrder newByteOrder, 
			    const E_ByteOrder oldByteOrder,
			    void * value, const Uint32 byteLength,
			    const size_t valWidth);


void swapBytes(void * value, const Uint32 byteLength, 
			   const size_t valWidth);

Uint16 swapShort(const Uint16 toSwap);

inline void swap2Bytes(Uint8 * toSwap)
{
    Uint8 tmp = toSwap[0];
    toSwap[0] = toSwap[1];
    toSwap[1] = tmp;
}

inline void swap4Bytes(Uint8 * toSwap)
{
    Uint8 tmp = toSwap[0];
    toSwap[0] = toSwap[3];
    toSwap[3] = tmp;
    tmp = toSwap[1];
    toSwap[1] = toSwap[2];
    toSwap[2] = tmp;
}

#endif // DCSWAP_H


