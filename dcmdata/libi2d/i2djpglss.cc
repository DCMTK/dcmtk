/*
 *
 *  Copyright (C) 2007-2021, OFFIS e.V.
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
 *  Author:  Michael Onken
 *
 *  Purpose: Class to extract pixel data and meta information from JPEG file
 *
 */

#include "dcmtk/dcmdata/libi2d/i2djpglss.h"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/libi2d/i2doutpl.h"

I2DJpegLsSource::I2DJpegLsSource()
    : m_jpegUtil(true)
    , m_lossyCompressed(OFTrue)
{
    DCMDATA_LIBI2D_DEBUG("I2DJpegLsSource: Plugin instantiated");
}

OFString I2DJpegLsSource::inputFormat() const
{
    return "JPEG-LS";
}

void I2DJpegLsSource::setKeepAPPn(const OFBool enabled)
{
    m_jpegUtil.setKeepAPPn(enabled);
}

void I2DJpegLsSource::setKeepCOM(const OFBool enabled)
{
    m_jpegUtil.setKeepCOM(enabled);
}

OFCondition I2DJpegLsSource::readPixelData(Uint16& rows,
                                         Uint16& cols,
                                         Uint16& samplesPerPixel,
                                         OFString& photoMetrInt,
                                         Uint16& bitsAlloc,
                                         Uint16& bitsStored,
                                         Uint16& highBit,
                                         Uint16& pixelRepr,
                                         Uint16& planConf,
                                         Uint16& pixAspectH,
                                         Uint16& pixAspectV,
                                         char*& pixData,
                                         Uint32& length,
                                         E_TransferSyntax& ts)
{
  return m_jpegUtil.readPixelData(m_imageFile, rows, cols, samplesPerPixel,
    photoMetrInt, bitsAlloc, bitsStored, highBit, pixelRepr, planConf, pixAspectH,
    pixAspectV, pixData, length, ts);
}

OFCondition I2DJpegLsSource::getLossyComprInfo(OFBool& srcEncodingLossy, OFString& srcLossyComprMethod) const
{
    if (m_lossyCompressed)
    {
        srcEncodingLossy    = OFTrue;
        srcLossyComprMethod = "ISO_14495_1"; // Defined term for JPEG-LS Lossy Compression
    }
    else
    {
        // (will never get here, no lossless input supported so far)
        srcEncodingLossy = OFFalse;
    }
    return EC_Normal;
}

// close file and free dynamically allocated memory
I2DJpegLsSource::~I2DJpegLsSource()
{
    DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Closing JPEG file and cleaning up memory");
}
