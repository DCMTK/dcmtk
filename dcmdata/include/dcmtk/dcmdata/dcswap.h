/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
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
DCMTK_DCMDATA_EXPORT OFCondition swapIfNecessary(
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
DCMTK_DCMDATA_EXPORT void swapBytes(
  void * value, 
  const Uint32 byteLength, 
  const size_t valWidth);

/** swap an Uint16 number from big-endian to little-endian or back
 *  @param toSwap number to be swapped
 *  @return swapped number
 */
DCMTK_DCMDATA_EXPORT Uint16 swapShort(const Uint16 toSwap);

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
