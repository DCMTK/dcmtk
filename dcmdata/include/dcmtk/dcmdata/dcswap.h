/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Andreas Barth
 *
 *  Purpose: byte order functions
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:42 $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCSWAP_H
#define DCSWAP_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/dcxfer.h"     /* for E_ByteOrder */

/** swap block of data from big-endian to little-endian or back if neccessary
 *  @param newByteOrder desired byte order of data block
 *  @param oldByteOrder current byte order of data block
 *  @param value pointer to block of data
 *  @param byteLength size of data block in bytes
 *  @param valWidth size of each value in the data block, in bytes
 *  @return EC_Normal if successful, an error code otherwise
 */
OFCondition swapIfNecessary(
  const E_ByteOrder newByteOrder, 
  const E_ByteOrder oldByteOrder,
  void * value, 
  const Uint32 byteLength,
  const size_t valWidth);

/** swap block of data from big-endian to little-endian or back
 *  @param value pointer to block of data
 *  @param byteLength size of data block in bytes
 *  @param valWidth size of each value in the data block, in bytes
 */
void swapBytes(
  void * value, 
  const Uint32 byteLength, 
  const size_t valWidth);

/** swap an Uint16 number from big-endian to little-endian or back
 *  @param toSwap number to be swapped
 *  @return swapped number
 */
Uint16 swapShort(const Uint16 toSwap);

/** swap a sequence of two bytes from big-endian to little-endian or back
 *  @param toSwap pointer to object to swap
 */
inline void swap2Bytes(Uint8 * toSwap)
// swaps [byte0][byte1] to [byte1][byte0]
{
    Uint8 tmp = toSwap[0];
    toSwap[0] = toSwap[1];
    toSwap[1] = tmp;
}

/** swap a sequence of four bytes from big-endian to little-endian or back
 *  @param toSwap pointer to object to swap
 */
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
 * Revision 1.19  2010-10-14 13:15:42  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.18  2010-03-01 09:08:44  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.17  2009-11-04 09:58:07  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.16  2007-11-29 14:30:35  meichel
 * Updated doxygen API documentation
 *
 * Revision 1.15  2005/12/08 16:28:43  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.14  2002/11/27 12:07:23  meichel
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
