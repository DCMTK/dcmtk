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
        case charls::ApiResult::OK:
        return EC_Normal;
      case charls::ApiResult::UncompressedBufferTooSmall:
        return EC_JLSUncompressedBufferTooSmall;
      case charls::ApiResult::CompressedBufferTooSmall:
        return EC_JLSCompressedBufferTooSmall;
      case charls::ApiResult::ImageTypeNotSupported:
        return EC_JLSCodecUnsupportedImageType;
      case charls::ApiResult::InvalidJlsParameters:
        return EC_JLSCodecInvalidParameters;
      case charls::ApiResult::ParameterValueNotSupported:
        return EC_JLSCodecUnsupportedValue;
      case charls::ApiResult::InvalidCompressedData:
        return EC_JLSInvalidCompressedData;
      case charls::ApiResult::UnsupportedBitDepthForTransform:
        return EC_JLSUnsupportedBitDepthForTransform;
      case charls::ApiResult::UnsupportedColorTransform:
        return EC_JLSUnsupportedColorTransform;
      case charls::ApiResult::TooMuchCompressedData:
        return EC_JLSTooMuchCompressedData;
      default:
        return EC_IllegalParameter;
    }
  }
};

#endif
