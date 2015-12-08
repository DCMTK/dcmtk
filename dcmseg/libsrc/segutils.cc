/*
 *
 *  Copyright (C) 2015, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Helper functions for the dcmseg module
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmseg/segutils.h"
#include "dcmtk/dcmseg/segdef.h"


DcmIODTypes::Frame* DcmSegUtils::packBinaryFrame(Uint8* pixelData,
                                                 const Uint16 rows,
                                                 const Uint16 columns)
{
  // Sanity checking
  const size_t numPixels = rows*columns;
  if (numPixels == 0)
  {
    DCMSEG_ERROR("Unable to pack binary segmentation frame: Rows or Columns is 0");
    return NULL;
  }
  if (!pixelData)
  {
    DCMSEG_ERROR("Unable to pack binary segmentation frame: No pixel data provided");
    return NULL;
  }
  DcmIODTypes::Frame* frame = new DcmIODTypes::Frame();
  if (frame == NULL)
  {
    DCMSEG_ERROR("Could not pack binary segmentation frame: Memory exhausted");
    return NULL;
  }
  frame->length = getBytesForBinaryFrame(numPixels);
  frame->pixData = new Uint8[frame->length];
  if (frame->pixData == 0)
  {
    delete frame;
    return NULL;
  }
  memset(frame->pixData, 0, frame->length);

  size_t bytePos = 0;
  for (size_t count = 0; count < numPixels; count++)
  {
    // Compute byte position
    bytePos = count / 8;
    frame->pixData[bytePos] |= (pixelData[count] != 0) /* value to set */ << (7-(count % 8) /* bit position (0-7) within byte */);
  }
  return frame;
}


DcmIODTypes::Frame* DcmSegUtils::unpackBinaryFrame(Uint8*& buffer,
                                                   Uint8& bitPos,
                                                   size_t numBits)
{
  // Sanity checking
  if ( (buffer == NULL) || (bitPos > 7) || (numBits== 0) )
  {
    DCMSEG_ERROR("Cannot unpack binary frame, invalid input data");
    return NULL;
  }

  // Create result frame in memory
  DcmIODTypes::Frame* result = new DcmIODTypes::Frame();
  if (result)
  {
    result->pixData = new Uint8[numBits];
    result->length = numBits;
  }
  if ( !result || !(result->pixData) )
  {
    DCMSEG_ERROR("Cannot unpack binary frame, memory exhausted");
    return NULL;
  }
  memset(result->pixData, 0, result->length);
  size_t bytePos = 0;
  for (; numBits > 0; numBits--)
  {
    // check whether bit at bitPos is set. We count bits from the left.
    if ( (buffer[bytePos] & (1 << (7-bitPos))) )
    {
      result->pixData[bytePos] = 1;
    }
    else
    {
      result->pixData[bytePos] = 0;
    }
    bytePos++;
    bitPos++;
    if (bitPos == 8)
    {
      bitPos = 0;
      buffer++;
    }
  }
  return result;
}


size_t DcmSegUtils::getBytesForBinaryFrame(const size_t& numPixels)
{
  // check whether the 1-bit pixels exactly fit into bytes
  size_t remainder = numPixels % 8;
  // number of bytes that work on an exact fit
  size_t bytesRequired =  numPixels / 8;
  // add one byte if we have a remainder
  if (remainder > 0) bytesRequired++;
  return bytesRequired;
}


void DcmSegUtils::shiftRight(Uint8* buf,
                             size_t bufLen,
                             Uint8 numBits)
{
  if (numBits > 7)
  {
    DCMSEG_ERROR("Invalid input data: shiftRight() can only shift 0-7 bits");
    return;
  }
  Uint8 carryOver = 0;
  for (size_t x = 0; x < bufLen; x++)
  {
    // Store current byte since we need its last bits
    Uint8 current = buf[x];
    buf[x] >>= numBits;
    // If there is a carry over from the last handled byte, add it
    buf[x] |= carryOver;
    // Compute carry over bits for next byte handled
    carryOver = current << (8-numBits); // bits we need to shift over to start (left hand side) of buf[x+1]
  }
}


void DcmSegUtils::shiftLeft(Uint8* buf,
                            size_t bufLen,
                            Uint8 numBits)
{
  if (numBits > 7)
  {
    DCMSEG_ERROR("Invalid input data: shiftLeft() can only shift 0-7 bits");
    return;
  }
  for (size_t x = 0; x < bufLen-1; x++)
  {
    // Shift current byte
    buf[x] = buf[x] << numBits;
    // isolate portion of next byte that must be shifted into current byte
    Uint8 next = (buf[x+1] >> (8-numBits));
    // Shift current byte
    buf[x] |= next;
  }
  // Shift last byte manually
  buf[bufLen-1] <<= numBits;
}


void DcmSegUtils::debugDumpBin(Uint8* buffer,
                               size_t length,
                               const char* what)
{
  for (size_t n=0; n<length; n++)
  {
    DCMSEG_DEBUG(what << " #" << n << ": " << (size_t)(buffer[n]) << ", bytepos " << (size_t)(&(buffer[n])) << " (" << debugByte2Bin((buffer[n])) << ")");
  }
  DCMSEG_DEBUG("");
}


OFString DcmSegUtils::debugByte2Bin(Uint8 b)
{
  OFString result("",8);
  for (int i = 7; i >= 0; i--)
  {
    result[i]= (b & 1) + '0';

    b >>= 1;
  }
  return result;
}
