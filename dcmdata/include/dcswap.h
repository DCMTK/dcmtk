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
** Last Update:   $Author: andreas $
** Revision:      $Revision: 1.3 $
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

void swapBytes(void * value, const Uint32 byteLength, 
			   const size_t valWidth);

const Uint16 swapShort(const Uint16 toSwap);

#endif // DCSWAP_H
