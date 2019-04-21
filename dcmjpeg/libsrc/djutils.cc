/*
 *
 *  Copyright (C) 1997-2019, OFFIS e.V.
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
 *  Module:  dcmjpeg
 *
 *  Author:  Norbert Olges, Marco Eichelberg
 *
 *  Purpose: Various helper methods for the JPEG encoder
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djutils.h"
#include "dcmtk/dcmdata/dcdeftag.h"  /* for tag constants */
#include "dcmtk/dcmdata/dcitem.h"    /* for class DcmItem */

#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

OFLogger DCM_dcmjpegLogger = OFLog::getLogger("dcmtk.dcmjpeg");

makeOFConditionConst(EJ_Suspension,                           OFM_dcmjpeg,  1, OF_error, "IJG codec suspension return"  );
makeOFConditionConst(EJ_IJG8_FrameBufferTooSmall,             OFM_dcmjpeg,  2, OF_error, "Buffer for decompressed image (8 bits/sample) too small"  );
makeOFConditionConst(EJ_IJG12_FrameBufferTooSmall,            OFM_dcmjpeg,  3, OF_error, "Buffer for decompressed image (12 bits/sample) too small"  );
makeOFConditionConst(EJ_IJG16_FrameBufferTooSmall,            OFM_dcmjpeg,  4, OF_error, "Buffer for decompressed image (16 bits/sample) too small"  );
makeOFConditionConst(EJ_UnsupportedPhotometricInterpretation, OFM_dcmjpeg,  5, OF_error, "Codec does not support this PhotometricInterpretation"  );
makeOFConditionConst(EJ_UnsupportedColorConversion,           OFM_dcmjpeg,  6, OF_error, "Codec does not support this kind of color conversion"  );
makeOFConditionConst(EJ_UnsupportedBitDepth,                  OFM_dcmjpeg,  7, OF_error, "Unsupported bit depth for this codec"  );

EP_Interpretation DcmJpegHelper::getPhotometricInterpretation(DcmItem *item)
{
  if (item)
  {
    OFString photometric;
    if (item->findAndGetOFString(DCM_PhotometricInterpretation, photometric).good() && !photometric.empty())
    {
      const char *c = photometric.c_str(); // guaranteed to be zero-terminated
      char cp[17]; // legal CS cannot be larger than 16 characters plus 0 byte
      int i=0; // current character index
      unsigned char cur; // current character
      while (*c && (i<16))
      {
        cur = OFstatic_cast(unsigned char, *c);
        if (isalpha(cur)) cp[i++] = OFstatic_cast(char, toupper(cur));
        else if (isdigit(cur)) cp[i++] = cur;
        c++;
      }
      cp[i] = 0; // write terminating zero
      photometric = cp; // copy back into OFString

      // now browse PhotometricInterpretationNames
      i = 0;
      while (PhotometricInterpretationNames[i].Name)
      {
        if (photometric == PhotometricInterpretationNames[i].Name) return PhotometricInterpretationNames[i].Type;
        i++;
      }
      return EPI_Unknown;
    }
  }
  return EPI_Missing;
}

void DcmJpegHelper::fixPadding(
    Uint8 *buffer,
    Uint32 bufSize)
{
  if (buffer && (bufSize > 0))
  {
    // first write a zero pad byte after the end of the JPEG bitstream
    buffer[bufSize - 1] = 0;

#ifndef DISABLE_FF_JPEG_BITSTREAM_PADDING
    // look for the EOI marker
    if ((bufSize > 2) && (buffer[bufSize-3] == 0xFF) && (buffer[bufSize-2] == 0xD9))
    {
      // we now have ff/d9/00 at the end of the JPEG bitstream,
      // i.e. an end of image (EOI) marker followed by a pad byte.
      // Replace this with ff/ff/d9, which is an "extended" EOI marker
      // ending on an even byte boundary.
      buffer[bufSize-2] = 0xFF;
      buffer[bufSize-1] = 0xD9;
    }
  }
#endif

  return;
}
