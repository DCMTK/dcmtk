/*
 *
 *  Copyright (C) 1994-2002, OFFIS
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
 *  Update Date:      $Date: 2002-11-27 12:07:23 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcswap.h,v $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCSWAP_H
#define DCSWAP_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctypes.h"
#include "dcerror.h"
#include "dcxfer.h"

#define INCLUDE_CSTDLIB
#include "ofstdinc.h"


// Swap byteLength bytes, if newByteOrder != oldByteOrder. The bytes are
// parted in valWidth bytes, each swapped separately
OFCondition swapIfNecessary(const E_ByteOrder newByteOrder, 
			    const E_ByteOrder oldByteOrder,
			    void * value, const Uint32 byteLength,
			    const size_t valWidth);


void swapBytes(void * value, const Uint32 byteLength, 
			   const size_t valWidth);

Uint16 swapShort(const Uint16 toSwap);

inline void swap2Bytes(Uint8 * toSwap)
// swaps [byte0][byte1] to [byte1][byte0]
{
    Uint8 tmp = toSwap[0];
    toSwap[0] = toSwap[1];
    toSwap[1] = tmp;
}

inline void swap4Bytes(Uint8 * toSwap)
// swaps [byte0][byte1][byte2][byte3] to [byte3][byte2][byte1][byte0]
{
    Uint8 tmp = toSwap[0];
    toSwap[0] = toSwap[3];
    toSwap[3] = tmp;
    tmp = toSwap[1];
    toSwap[1] = toSwap[2];
    toSwap[2] = tmp;
}

#endif // DCSWAP_H

/*
 * CVS/RCS Log:
 * $Log: dcswap.h,v $
 * Revision 1.14  2002-11-27 12:07:23  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.13  2001/11/01 14:41:43  wilkens
 * Added lots of comments.
 *
 * Revision 1.12  2001/09/25 17:19:29  meichel
 * Adapted dcmdata to class OFCondition
 *
 * Revision 1.11  2001/06/01 15:48:44  meichel
 * Updated copyright header
 *
 * Revision 1.10  2000/03/08 16:26:18  meichel
 * Updated copyright header.
 *
 * Revision 1.9  2000/02/23 15:11:39  meichel
 * Corrected macro for Borland C++ Builder 4 workaround.
 *
 * Revision 1.8  2000/02/01 10:12:02  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.7  1999/06/10 10:44:50  meichel
 * Replaced some #if statements by more robust #ifdef
 *
 * Revision 1.6  1999/03/31 09:24:48  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
