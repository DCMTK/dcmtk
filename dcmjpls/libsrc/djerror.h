/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:20:24 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DJERROR_H
#define DJERROR_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/djlsutil.h" /* For the OFCondition codes */
#include "intrface.h" /* CharLS include */

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
  static const OFCondition& convert(JLS_ERROR error)
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

/*
 * CVS/RCS Log:
 * $Log: djerror.h,v $
 * Revision 1.5  2010-10-14 13:20:24  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.4  2010-02-25 08:50:38  uli
 * Updated to latest CharLS version.
 *
 * Revision 1.3  2010-01-19 15:19:06  uli
 * Made file names fit into 8.3 format.
 *
 * Revision 1.2  2009-10-07 13:16:47  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.1  2009-07-31 09:05:43  meichel
 * Added more detailed error messages, minor code clean-up
 *
 *
 */
