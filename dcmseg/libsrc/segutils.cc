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
  memset(frame->pixData, 0, sizeof(Uint8)*frame->length);

  size_t bytePos = 0;
  for (size_t count = 0; count < numPixels; count++)
  {
    // Compute byte position
    bytePos = count / 8;
    frame->pixData[bytePos] |= (pixelData[count] != 0) /* value to set */ << (count % 8 /* bit position (0-7) within byte */);
  }
  return frame;
}


DcmIODTypes::Frame* DcmSegUtils::unpackBinaryFrame(const DcmIODTypes::Frame* frame,
                                                   const Uint16 rows,
                                                   const Uint16 cols)
{
  // Sanity checking
  if ( (frame->length == 0) || (frame->pixData == NULL) || (rows == 0) || (cols == 0))
  {
    DCMSEG_ERROR("Cannot unpack binary frame, invalid input data: frame length and data, as well as rows and columnst cannot be 0");
    return NULL;
  }
  const Uint32 numPixels = rows*cols;
  if ( getBytesForBinaryFrame(numPixels) > frame->length)
  {
    DCMSEG_ERROR("Cannot unpack binary frame, not enough input data (require " << numPixels / 8 << " but only got " << frame->length << " bytes)");
    return NULL;
  }

  // Create result frame in memory
  DcmIODTypes::Frame* result = new DcmIODTypes::Frame();
  if (result)
  {
    result->pixData = new Uint8[numPixels];
    result->length = numPixels;
  }
  if ( !result || !(result->pixData) )
  {
    DCMSEG_ERROR("Cannot unpack binary frame, memory exhausted");
    return NULL;
  }
  memset(result->pixData, 0, sizeof(Uint8)*result->length);

  // Transform and copy from packed frame to unpacked result frame
  size_t bytePos = 0;
  for (size_t count = 0; count < numPixels; count++)
  {
    // Compute byte position
    bytePos = count / 8;
    // Bit position (0-7) within byte
    Uint8 bitpos = (count % 8);
    if ( (frame->pixData[bytePos] & (1 << bitpos) /* check whether bit at bitpos is set*/) )
    {
      result->pixData[count] = 1;
    }
    else
    {
      result->pixData[count] = 0;
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
