/*
 *
 *  Copyright (C) 2012-2021, OFFIS e.V.
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
 *  Module:  dcmrt
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Mid-Level API for RT Dose objects
 *
 */

#include "dcmtk/config/osconfig.h"     /* make sure OS specific configuration is included first */
#include "dcmtk/dcmrt/drmdose.h"
#include "dcmtk/ofstd/ofbmanip.h"

template<typename signedT, typename unsignedT>
class PixelDataAccess {
public:
    PixelDataAccess(const DcmPixelData& data, Uint16 pixelRep)
    /* We cast away the const so that we get a nicer API where stuff can be const */
    : pixelData_(OFconst_cast(DcmPixelData&, data))
    , pixelRep_(pixelRep)
    {
    }

    double getPixel(Uint32 pixelNumber)
    {
        if (pixelRep_ == 1) {
            DCMRT_TRACE("Accessing signed dose image data");
            return getPixelSigned(pixelNumber);
        } else {
            DCMRT_TRACE("Accessing unsigned dose image data");
            return getPixelUnsigned(pixelNumber);
        }
    }

    OFCondition getScaledPixels(OFVector<double> &result, double scaling, Uint32 offset, Uint32 length)
    {
        if (pixelRep_ == 1) {
            return getScaledPixelsSigned(result, scaling, offset, length);
        } else {
            return getScaledPixelsUnsigned(result, scaling, offset, length);
        }
    }

protected:
    // This is supposed to be a template, because getPixelUnsigned has the same
    // code, just a different type. MSC6 doesn't support member function templates.
    signedT getPixelSigned(Uint32 pixelNumber)
    {
        signedT result;
        if (pixelData_.getPartialValue(&result, OFstatic_cast(Uint32, pixelNumber * sizeof(signedT)), OFstatic_cast(Uint32, sizeof(signedT))).bad())
            return -1;
        return result;
    }

    unsignedT getPixelUnsigned(Uint32 pixelNumber)
    {
        unsignedT result;
        if (pixelData_.getPartialValue(&result, OFstatic_cast(Uint32, pixelNumber * sizeof(unsignedT)), OFstatic_cast(Uint32, sizeof(unsignedT))).bad())
            return OFstatic_cast(unsignedT, (-1));
        return result;
    }

    // This is supposed to be a template, because getScaledPixelsUnsigned has the same
    // code, just a different type. MSC6 doesn't support member function templates.
    OFCondition getScaledPixelsSigned(OFVector<double> &result, double scaling, Uint32 offset, Uint32 length)
    {
        OFCondition cond = EC_Normal;
        signedT* tmp = new signedT[length];
        if (!tmp) {
            cond = EC_MemoryExhausted;
        } else {
            // First, get the "raw" pixel data
            cond = pixelData_.getPartialValue(tmp, OFstatic_cast(Uint32, offset * sizeof(signedT)), OFstatic_cast(Uint32, length * sizeof(signedT)));
            if (cond.good()) {
                result.clear();
                result.reserve(length);
                // Now convert it to double
                for (Uint32 i = 0; i < length; i++) {
                    result.push_back(scaling * tmp[i]);
                }
            }
            delete[] tmp;
        }

        return cond;
    }

    OFCondition getScaledPixelsUnsigned(OFVector<double> &result, double scaling, Uint32 offset, Uint32 length)
    {
        OFCondition cond = EC_Normal;
        unsignedT* tmp = new unsignedT[length];
        if (!tmp) {
            cond = EC_MemoryExhausted;
        } else {
            // First, get the "raw" pixel data
            cond = pixelData_.getPartialValue(tmp, OFstatic_cast(Uint32, offset * sizeof(unsignedT)), OFstatic_cast(Uint32, length * sizeof(unsignedT)));
            if (cond.good()) {
                result.clear();
                result.reserve(length);
                // Now convert it to double
                for (Uint32 i = 0; i < length; i++) {
                    result.push_back(scaling * tmp[i]);
                }
            }
            delete[] tmp;
        }

        return cond;
    }

private:
    DcmPixelData &pixelData_;
    Uint16 pixelRep_;
};

OFCondition DRTDose::loadFile(const OFFilename &filename,
                              const E_FileReadMode readMode,
                              const E_TransferSyntax readXfer)
{
    DcmFileFormat format;
    OFCondition cond = format.loadFile(filename, readXfer, EGL_noChange,
                                       DCM_MaxReadLength, readMode);
    if (cond.good())
        cond = read(*format.getDataset());
    return cond;
}

double DRTDose::getDose(unsigned int x, unsigned int y, unsigned int frame) const
{
    double result;
    if (getDose(result, x, y, frame).bad())
        return 0;
    return result;
}

OFCondition DRTDose::getDose(double &result, unsigned int x, unsigned int y, unsigned int frame) const
{
    double gridScaling;
    double dose = 0.0;
    OFCondition cond = getDoseGridScaling(gridScaling);
    if (cond.good())
        cond = getUnscaledDose(dose, x, y, frame);
    if (cond.good())
        result = gridScaling * dose;
    else
        result = 0;
    return cond;
}

double DRTDose::getUnscaledDose(unsigned int x, unsigned int y, unsigned int frame) const
{
    double result;
    if (getUnscaledDose(result, x, y, frame).bad())
        return 0;
    return result;
}

static OFCondition getImageParameters(const DRTDose& dose, Uint32& frames, Uint16& rows, Uint16& columns, Uint16& bitsAllocated, Uint16& pixelRep)
{
    OFCondition cond = EC_Normal;
    Sint32 tmp = 0;
    Uint16 bitsStored = 0;
    Uint16 highBit = 0;
 
    // tbd: Would be nice to know which getter failed
    if (cond.good())
        cond = dose.getNumberOfFrames(tmp);
    if (cond.good())
        cond = dose.getRows(rows);
    if (cond.good())
        cond = dose.getColumns(columns);
    if (cond.good())
        cond = dose.getBitsAllocated(bitsAllocated);
    if (cond.good())
        cond = dose.getBitsStored(bitsStored);
    if (cond.good())
        cond = dose.getHighBit(highBit);
    if (cond.good())
        cond = dose.getPixelRepresentation(pixelRep);
    if (cond.good() && pixelRep != 0 && pixelRep != 1)
    {
        DCMRT_ERROR("Invalid value for PixelRepresentation (" << pixelRep << "), must be 0 or 1");
        cond = RT_EC_InvalidValue;
    }
    if (cond.good() && tmp < 0)
    {
        DCMRT_ERROR("Invalid value for NumberOfFrames (" << tmp << "), must be positive");
        cond = RT_EC_InvalidValue;
    }
    if (cond.good() && bitsStored != bitsAllocated)
    {
        DCMRT_ERROR("Different values for BitsStored (" << bitsStored << ") and BitsAllocated (" << bitsAllocated << "), must be the same");
        cond = RT_EC_InvalidValue;
    }
    if (cond.good() && highBit != bitsStored - 1)
    {
        DCMRT_ERROR("Invalid value for HighBit (" << highBit << "), must be BitsStored - 1 (" << (bitsStored - 1) << ")");
        cond = RT_EC_InvalidValue;
    }
    if (cond.good())
        frames = OFstatic_cast(Uint32, tmp);
    return cond;
}

OFCondition DRTDose::getUnscaledDose(double &result, unsigned int x, unsigned int y, unsigned int frame) const
{
    Uint32 frames;
    Uint16 rows, columns, bitsAllocated, pixelRep;
    OFCondition cond = EC_Normal;

    cond = getImageParameters(*this, frames, rows, columns, bitsAllocated, pixelRep);
    if (cond.bad())
        return cond;
    if (columns < x || rows < y || frames < frame)
        return EC_IllegalParameter;

    Uint32 pixel_number = frame * columns * rows + y * columns + x;
    DCMRT_TRACE("Getting pixel (" << x << "," << y << ") of frame " << frame << " (index " << pixel_number << ")");
    if (bitsAllocated == 16) {
        DCMRT_TRACE("Dose image uses 16 bit per pixel");
        PixelDataAccess<Sint16, Uint16> pixel(getPixelData(), pixelRep);
        result = pixel.getPixel(pixel_number);
    } else if (bitsAllocated == 32) {
        DCMRT_TRACE("Dose image uses 32 bit per pixel");
        PixelDataAccess<Sint32, Uint32> pixel(getPixelData(), pixelRep);
        result = pixel.getPixel(pixel_number);
    } else {
        /* RT Dose Images may only have 16 or 32 bits stored */
        DCMRT_ERROR("Invalid value for BitsAllocated (" << bitsAllocated << "), only 16 and 32 allowed");
        return RT_EC_InvalidValue;
    }

    return EC_Normal;
}

OFCondition DRTDose::getDoseImage(OFVector<double> &result, unsigned int frame) const
{
    Uint32 frames;
    Uint32 offset, length;
    Uint16 rows, columns, bitsAllocated, pixelRep;
    double doseGridScaling = 0.0;
    OFCondition cond = EC_Normal;

    result.clear();

    cond = getImageParameters(*this, frames, rows, columns, bitsAllocated, pixelRep);
    if (cond.good())
        cond = getDoseGridScaling(doseGridScaling);
    if (cond.bad())
        return cond;

    if (frame >= frames)
        return EC_IllegalParameter;

    length = columns * rows;
    offset = frame * length;
    DCMRT_TRACE("Getting dose image for frame " << frame << " (offset=" << offset << ", length=" << length << ")");

    if (bitsAllocated == 16) {
        DCMRT_TRACE("Dose image uses 16 bit per pixel");
        PixelDataAccess<Sint16, Uint16> pixel(getPixelData(), pixelRep);
        cond = pixel.getScaledPixels(result, doseGridScaling, offset, length);
    } else if (bitsAllocated == 32) {
        DCMRT_TRACE("Dose image uses 32 bit per pixel");
        PixelDataAccess<Sint32, Uint32> pixel(getPixelData(), pixelRep);
        cond = pixel.getScaledPixels(result, doseGridScaling, offset, length);
    } else {
        /* RT Dose Images may only have 16 or 32 bits stored */
        DCMRT_ERROR("Invalid value for BitsAllocated (" << bitsAllocated << "), only 16 and 32 allowed");
        cond = RT_EC_InvalidValue;
    }

    return cond;
}

OFCondition DRTDose::getDoseImages(OFVector<OFVector<double> > &result) const
{
    Sint32 frames, i;
    OFCondition cond = getNumberOfFrames(frames);

    if (cond.good() && frames < 0)
    {
        DCMRT_ERROR("Invalid value for NumberOfFrames (" << frames << "), must be positive");
        cond = EC_IllegalParameter;
    }
    if (cond.good())
        result.resize(frames);

    for (i = 0; i < frames; i++)
    {
        if (cond.good())
            cond = getDoseImage(result[i], i);
    }

    if (cond.bad())
        result.clear();

    return cond;
}

Uint16 DRTDose::getDoseImageWidth() const
{
    Uint16 result;
    if (getColumns(result).bad())
        result = 0;
    return result;
}

Uint16 DRTDose::getDoseImageHeight() const
{
    Uint16 result;
    if (getRows(result).bad())
        result = 0;
    return result;
}

OFBool DRTDose::isValid()
{
    Uint32 frames;
    Uint16 rows, columns, bitsAllocated, pixelRep;
    OFCondition cond = EC_Normal;

    cond = getImageParameters(*this, frames, rows, columns, bitsAllocated, pixelRep);
    if (cond.bad())
        return OFFalse;

    if (bitsAllocated != 16 && bitsAllocated != 32)
        return OFFalse;

    return OFTrue;
}
