/*
 *
 *  Copyright (C) 1997-2012, OFFIS e.V.
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
 *  Module:  dcmjpls
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Helper function than converts between CharLS and dcmjpgls errors
 *
 */

#ifndef DJERROR_H
#define DJERROR_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/djlsutil.h" /* For the OFCondition codes */
#include "CharLS/charls.h" /* CharLS include */
#include "CharLS/publictypes.h" /* CharLS include */

/** Helper class for converting between dcmjpls and CharLS error codes
 */
class DJLSError
{
private:
  /// private undefined constructor
  DJLSError();

public:

  /** This method converts a CharLS error code into a dcmjpls OFCondition
   *  @param error The CharLS error code
   *  @return The OFCondition
   */
  static const OFConditionConst& convert(CharlsApiResultType error)
  {
    switch (error)
    {
      case OK:
        return EC_Normal;
      case UncompressedBufferTooSmall:
        return EC_JLSUncompressedBufferTooSmall;
      case CompressedBufferTooSmall:
        return EC_JLSCompressedBufferTooSmall;
      case ImageTypeNotSupported:
        return EC_JLSCodecUnsupportedImageType;
      case InvalidJlsParameters:
        return EC_JLSCodecInvalidParameters;
      case ParameterValueNotSupported:
        return EC_JLSCodecUnsupportedValue;
      case InvalidCompressedData:
        return EC_JLSInvalidCompressedData;
      case UnsupportedBitDepthForTransform:
        return EC_JLSUnsupportedBitDepthForTransform;
      case UnsupportedColorTransform:
        return EC_JLSUnsupportedColorTransform;
      case TooMuchCompressedData:
        return EC_JLSTooMuchCompressedData;
      default:
        return EC_IllegalParameter;
    }
  }
};

#endif
