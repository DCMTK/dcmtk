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

#include "dcmtk/dcmdata/libi2d/i2djpgs.h"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/libi2d/i2doutpl.h"

I2DJpegSource::I2DJpegSource()
    : m_jpegUtil()
    , m_lossyCompressed(OFTrue)
{
    DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Plugin instantiated");
}

OFString I2DJpegSource::inputFormat() const
{
    return "JPEG";
}

void I2DJpegSource::setExtSeqSupport(const OFBool enabled)
{
    m_jpegUtil.setExtSeqSupport(enabled);
}

void I2DJpegSource::setProgrSupport(const OFBool enabled)
{
    m_jpegUtil.setProgrSupport(enabled);
}

void I2DJpegSource::setInsistOnJFIF(const OFBool enabled)
{
    m_jpegUtil.setInsistOnJFIF(enabled);
}

void I2DJpegSource::setKeepAPPn(const OFBool enabled)
{
    m_jpegUtil.setKeepAPPn(enabled);
}

void I2DJpegSource::setKeepCOM(const OFBool enabled)
{
    m_jpegUtil.setKeepCOM(enabled);
}

OFCondition I2DJpegSource::readPixelData(Uint16& rows,
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

OFCondition I2DJpegSource::getLossyComprInfo(OFBool& srcEncodingLossy, OFString& srcLossyComprMethod) const
{
    if (m_lossyCompressed)
    {
        srcEncodingLossy    = OFTrue;
        srcLossyComprMethod = "ISO_10918_1"; // Defined term for JPEG Lossy Compression
    }
    else
    {
        // (will never get here, no lossless input supported so far)
        srcEncodingLossy = OFFalse;
    }
    return EC_Normal;
}

// close file and free dynamically allocated memory
I2DJpegSource::~I2DJpegSource()
{
    DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Closing JPEG file and cleaning up memory");
}
