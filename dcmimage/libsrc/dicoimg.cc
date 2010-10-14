/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorImage (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:14 $
 *  CVS/RCS Revision: $Revision: 1.45 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcpixel.h"

#include "dcmtk/dcmimage/dicoimg.h"
#include "dcmtk/dcmimgle/dimo2img.h"
#include "dcmtk/dcmimage/dicopxt.h"
#include "dcmtk/dcmimage/dicocpt.h"
#include "dcmtk/dcmimage/dicosct.h"
#include "dcmtk/dcmimage/dicoflt.h"
#include "dcmtk/dcmimage/dicorot.h"
#include "dcmtk/dcmimage/dicoopxt.h"
#include "dcmtk/dcmimage/dilogger.h"
#include "dcmtk/dcmimgle/diutils.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiColorImage::DiColorImage(const DiDocument *docu,
                           const EI_Status status,
                           const int spp,
                           const OFBool rgb)
  : DiImage(docu, status, spp),
    RGBColorModel(rgb),
    InterData(NULL),
    OutputData(NULL)
{
    detachPixelData();
}


DiColorImage::DiColorImage(const DiColorImage *image,
                           const unsigned long fstart,
                           const unsigned long fcount)
  : DiImage(image, fstart, fcount),
    RGBColorModel(image->RGBColorModel),
    InterData(NULL),
    OutputData(NULL)
{
    if (image->InterData != NULL)
    {
        const unsigned long fsize = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows);
        switch (image->InterData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiColorCopyTemplate<Uint8>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Sint8:
                InterData = new DiColorCopyTemplate<Sint8>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Uint16:
                InterData = new DiColorCopyTemplate<Uint16>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Sint16:
                InterData = new DiColorCopyTemplate<Sint16>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Uint32:
                InterData = new DiColorCopyTemplate<Uint32>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Sint32:
                InterData = new DiColorCopyTemplate<Sint32>(image->InterData, fstart, fcount, fsize);
                break;
        }
        checkInterData(0);
    }
}


DiColorImage::DiColorImage(const DiColorImage *image,
                           const signed long left_pos,
                           const signed long top_pos,
                           const Uint16 src_cols,
                           const Uint16 src_rows,
                           const Uint16 dest_cols,
                           const Uint16 dest_rows,
                           const int interpolate,
                           const int aspect)
  : DiImage(image, dest_cols, dest_rows, aspect),
    RGBColorModel(image->RGBColorModel),
    InterData(NULL),
    OutputData(NULL)
{
    if (image->InterData != NULL)
    {
        switch (image->InterData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiColorScaleTemplate<Uint8>(image->InterData, image->Columns, image->Rows, left_pos, top_pos,
                    src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames, image->BitsPerSample, interpolate);
                break;
            case EPR_Uint16:
                InterData = new DiColorScaleTemplate<Uint16>(image->InterData, image->Columns, image->Rows, left_pos, top_pos,
                    src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames, image->BitsPerSample, interpolate);
                break;
            case EPR_Uint32:
                InterData = new DiColorScaleTemplate<Uint32>(image->InterData, image->Columns, image->Rows, left_pos, top_pos,
                    src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames, image->BitsPerSample, interpolate);
                break;
            default:
                DCMIMAGE_WARN("invalid value for inter-representation");
        }
        checkInterData(0);
    }
}


DiColorImage::DiColorImage(const DiColorImage *image,
                           const int horz,
                           const int vert)
  : DiImage(image),
    RGBColorModel(image->RGBColorModel),
    InterData(NULL),
    OutputData(NULL)
{
    if (image->InterData != NULL)
    {
        switch (image->InterData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiColorFlipTemplate<Uint8>(image->InterData, Columns, Rows, NumberOfFrames, horz, vert);
                break;
            case EPR_Uint16:
                InterData = new DiColorFlipTemplate<Uint16>(image->InterData, Columns, Rows, NumberOfFrames, horz, vert);
                break;
            case EPR_Uint32:
                InterData = new DiColorFlipTemplate<Uint32>(image->InterData, Columns, Rows, NumberOfFrames, horz, vert);
                break;
            default:
                DCMIMAGE_WARN("invalid value for inter-representation");
        }
        checkInterData(0);
    }
}


DiColorImage::DiColorImage(const DiColorImage *image,
                           const int degree)
  : DiImage(image, degree),
    RGBColorModel(image->RGBColorModel),
    InterData(NULL),
    OutputData(NULL)
{
    if (image->InterData != NULL)
    {
        switch (image->InterData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiColorRotateTemplate<Uint8>(image->InterData, image->Columns, image->Rows, Columns, Rows,
                    NumberOfFrames, degree);
                break;
            case EPR_Uint16:
                InterData = new DiColorRotateTemplate<Uint16>(image->InterData, image->Columns, image->Rows, Columns, Rows,
                    NumberOfFrames, degree);
                break;
            case EPR_Uint32:
                InterData = new DiColorRotateTemplate<Uint32>(image->InterData, image->Columns, image->Rows, Columns, Rows,
                    NumberOfFrames, degree);
                break;
            default:
                DCMIMAGE_WARN("invalid value for inter-representation");
        }
        checkInterData(0);
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiColorImage::~DiColorImage()
{
    delete InterData;
    delete OutputData;
}


/*********************************************************************/


int DiColorImage::checkInterData(const int mode)
{
    if (InterData == NULL)
    {
        if (ImageStatus == EIS_Normal)
        {
            ImageStatus = EIS_MemoryFailure;
            DCMIMAGE_ERROR("can't allocate memory for inter-representation");
        } else
            ImageStatus = EIS_InvalidImage;
    }
    else if (InterData->getData() == NULL)
        ImageStatus = EIS_InvalidImage;
    else if (mode && (ImageStatus == EIS_Normal) && isOriginal /* do not check derived images */)
    {
        const unsigned long count = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows) * NumberOfFrames;
        if ((InterData->getInputCount() != count) && ((InterData->getInputCount() >> 1) != ((count + 1) >> 1)))
        {
            DCMIMAGE_WARN("computed (" << count << ") and stored (" << InterData->getInputCount()
                << ") " << "pixel count differ");
        }
    }
    return (ImageStatus == EIS_Normal);
}


void DiColorImage::deleteOutputData()
{
    delete OutputData;
    OutputData = NULL;
}


unsigned long DiColorImage::getOutputDataSize(const int bits) const
{
    unsigned long result = 0;
    if ((ImageStatus == EIS_Normal) && (bits > 0) && (bits <= MAX_BITS))
    {
        int bytesPerPixel = 1;
        if (bits > 16)
            bytesPerPixel = 4;
        else if (bits > 8)
            bytesPerPixel = 2;
        /* compute number of bytes required to store a rendered frame */
        result = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows) * 3 /*samples*/ * bytesPerPixel;
    }
    return result;
}


const void *DiColorImage::getOutputData(const unsigned long frame,
                                        const int bits,
                                        const int planar)
{
    return getData(NULL, 0, frame, bits, planar);
}


int DiColorImage::getOutputData(void *buffer,
                                const unsigned long size,
                                const unsigned long frame,
                                const int bits,
                                const int planar)
{
    return (getData(buffer, size, frame, bits, planar) != NULL);
}


const void *DiColorImage::getData(void *buffer,
                                  const unsigned long size,
                                  const unsigned long frame,
                                  const int bits,
                                  const int planar)
{
    if ((InterData != NULL) && (ImageStatus == EIS_Normal) && (frame < NumberOfFrames) && (bits > 0) && (bits <= MAX_BITS))
    {
        if ((buffer == NULL) || (size >= getOutputDataSize(bits)))
        {
            deleteOutputData();                             // delete old image data
            const unsigned long count = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows);
            const int inverse = (Polarity == EPP_Reverse);
            switch (InterData->getRepresentation())
            {
                case EPR_Uint8:
                    if (bits <= 8)
                        OutputData = new DiColorOutputPixelTemplate<Uint8, Uint8>(buffer, InterData, count, frame,
                            getBits(), bits, planar, inverse);
                    else if (bits <= 16)
                        OutputData = new DiColorOutputPixelTemplate<Uint8, Uint16>(buffer, InterData, count, frame,
                            getBits(), bits, planar, inverse);
                    else
                        OutputData = new DiColorOutputPixelTemplate<Uint8, Uint32>(buffer, InterData, count, frame,
                            getBits(), bits, planar, inverse);
                    break;
                case EPR_Uint16:
                    if (bits <= 8)
                        OutputData = new DiColorOutputPixelTemplate<Uint16, Uint8>(buffer, InterData, count, frame,
                            getBits(), bits, planar, inverse);
                    else if (bits <= 16)
                        OutputData = new DiColorOutputPixelTemplate<Uint16, Uint16>(buffer, InterData, count, frame,
                            getBits(), bits, planar, inverse);
                    else
                        OutputData = new DiColorOutputPixelTemplate<Uint16, Uint32>(buffer, InterData, count, frame,
                            getBits(), bits, planar, inverse);
                    break;
                case EPR_Uint32:
                    if (bits <= 8)
                        OutputData = new DiColorOutputPixelTemplate<Uint32, Uint8>(buffer, InterData, count, frame,
                            getBits(), bits, planar, inverse);
                    else if (bits <= 16)
                        OutputData = new DiColorOutputPixelTemplate<Uint32, Uint16>(buffer, InterData, count, frame,
                            getBits(), bits, planar, inverse);
                    else
                        OutputData = new DiColorOutputPixelTemplate<Uint32, Uint32>(buffer, InterData, count, frame,
                            getBits(), bits, planar, inverse);
                    break;
                default:
                    DCMIMAGE_WARN("invalid value for inter-representation");
            }
            if (OutputData == NULL)
            {
                ImageStatus = EIS_MemoryFailure;
                DCMIMAGE_ERROR("can't allocate memory for inter-representation");
            }
            else
                return OutputData->getData();
        } else {
            DCMIMAGE_ERROR("given output buffer is too small (only " << size << " bytes)");
        }
    }
    return NULL;
}


const void *DiColorImage::getOutputPlane(const int plane) const
{
    if (OutputData != NULL)
        return OutputData->getPlane(plane);
    return NULL;
}


DiImage *DiColorImage::createImage(const unsigned long fstart,
                                   const unsigned long fcount) const
{
    DiImage *image = new DiColorImage(this, fstart, fcount);
    return image;
}


DiImage *DiColorImage::createScale(const signed long left_pos,
                                   const signed long top_pos,
                                   const unsigned long src_cols,
                                   const unsigned long src_rows,
                                   const unsigned long dest_cols,
                                   const unsigned long dest_rows,
                                   const int interpolate,
                                   const int aspect,
                                   const Uint16 /*pvalue*/) const
{
    DiImage *image = new DiColorImage(this, left_pos, top_pos, OFstatic_cast(Uint16, src_cols), OFstatic_cast(Uint16, src_rows),
        OFstatic_cast(Uint16, dest_cols), OFstatic_cast(Uint16, dest_rows), interpolate, aspect);
    return image;
}


int DiColorImage::flip(const int horz,
                       const int vert)
{
    switch (InterData->getRepresentation())
    {
        case EPR_Uint8:
            {
                DiFlipTemplate<Uint8> dummy(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            }
            break;
        case EPR_Uint16:
            {
                DiFlipTemplate<Uint16> dummy(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            }
            break;
        case EPR_Uint32:
            {
                DiFlipTemplate<Uint32> dummy(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            }
            break;
        default:
            DCMIMAGE_WARN("invalid value for inter-representation");
    }
    return 1;
}


DiImage *DiColorImage::createFlip(const int horz,
                                  const int vert) const
{
    DiImage *image = new DiColorImage(this, horz, vert);
    return image;
}


int DiColorImage::rotate(const int degree)
{
    const Uint16 old_cols = Columns;                // save old values
    const Uint16 old_rows = Rows;
    DiImage::rotate(degree);                        // swap width and height if necessary
    if ((Columns > 1) && (Rows > 1))
    {
        switch (InterData->getRepresentation())
        {
            case EPR_Uint8:
                {
                    DiRotateTemplate<Uint8> dummy(InterData, old_cols, old_rows, Columns, Rows,
                        NumberOfFrames, degree);
                }
                break;
            case EPR_Uint16:
                {
                    DiRotateTemplate<Uint16> dummy(InterData, old_cols, old_rows, Columns, Rows,
                        NumberOfFrames, degree);
                }
                break;
            case EPR_Uint32:
                {
                    DiRotateTemplate<Uint32> dummy(InterData, old_cols, old_rows, Columns, Rows,
                        NumberOfFrames, degree);
                }
                break;
            default:
                DCMIMAGE_WARN("invalid value for inter-representation");
        }
    }
    return 1;
}


DiImage *DiColorImage::createRotate(const int degree) const
{
    DiImage *image = new DiColorImage(this, degree);
    return image;
}


DiImage *DiColorImage::createMono(const double red,
                                  const double green,
                                  const double blue) const
{
    DiImage *image = NULL;
    if (RGBColorModel)
        image = new DiMono2Image(this, red, green, blue);
    return image;
}


unsigned long DiColorImage::createDIB(void *&data,
                                      const unsigned long size,
                                      const unsigned long frame,
                                      const int bits,
                                      const int upsideDown,
                                      const int padding)
{
    if (RGBColorModel && (InterData != NULL))
    {
        if (size == 0)
            data = NULL;
        if ((bits == 24) || (bits == 32))
            return InterData->createDIB(data, size, Columns, Rows, frame, getBits() /*fromBits*/, 8 /*toBits*/, bits /*mode*/, upsideDown, padding);
    }
    return 0;
}


unsigned long DiColorImage::createAWTBitmap(void *&data,
                                            const unsigned long frame,
                                            const int bits)
{
    if (RGBColorModel && (InterData != NULL) && (bits == 32))
        return InterData->createAWTBitmap(data, Columns, Rows, frame, getBits() /*fromBits*/, 8 /*toBits*/);
    return 0;
}


void DiColorImage::updateImagePixelModuleAttributes(DcmItem &dataset)
{
    DiImage::updateImagePixelModuleAttributes(dataset);
    /* remove color palette LUTs (if any) */
    delete dataset.remove(DCM_PaletteColorLookupTableUID);
    delete dataset.remove(DCM_RedPaletteColorLookupTableDescriptor);
    delete dataset.remove(DCM_GreenPaletteColorLookupTableDescriptor);
    delete dataset.remove(DCM_BluePaletteColorLookupTableDescriptor);
    delete dataset.remove(DCM_RedPaletteColorLookupTableData);
    delete dataset.remove(DCM_GreenPaletteColorLookupTableData);
    delete dataset.remove(DCM_BluePaletteColorLookupTableData);
    delete dataset.remove(DCM_SegmentedRedPaletteColorLookupTableData);
    delete dataset.remove(DCM_SegmentedGreenPaletteColorLookupTableData);
    delete dataset.remove(DCM_SegmentedBluePaletteColorLookupTableData);
}


// --- write current image to DICOM dataset

int DiColorImage::writeImageToDataset(DcmItem &dataset,
                                      const int /*mode*/,
                                      const int planar)
{
    int result = 0;
    if ((InterData != NULL) && (InterData->getCount() > 0) && (BitsPerSample > 0))
    {
        /* create new pixel data element */
        DcmPolymorphOBOW *pixel = new DcmPolymorphOBOW(DCM_PixelData);
        if (pixel != NULL)
        {
            OFBool ok = OFFalse;
            /* number of samples */
            const unsigned long count = InterData->getCount() * 3 /*planes*/;
            /* number of pixels per frame */
            const unsigned long fcount = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows);
            /* determine planar configuration */
            const int planarConfig = (planar == 2) ? InterData->getPlanarConfiguration() : ((planar == 0) ? 0 : 1);
            switch (InterData->getRepresentation())
            {
                case EPR_Uint8:
                case EPR_Sint8:
                {
                    /* write 8 bit pixel data (OB) */
                    Uint8 *data = NULL;
                    if (pixel->createUint8Array(count, data).good() &&
                        InterData->getPixelData(OFstatic_cast(void *, data), OFstatic_cast(size_t, count), fcount, NumberOfFrames, planarConfig))
                    {
                        ok = OFTrue;
                    }
                    break;
                }
                case EPR_Uint16:
                case EPR_Sint16:
                {
                    /* write 16 bit pixel data (OW) */
                    Uint16 *data = NULL;
                    if (pixel->createUint16Array(count, data).good() &&
                        InterData->getPixelData(OFstatic_cast(void *, data), OFstatic_cast(size_t, count), fcount, NumberOfFrames, planarConfig))
                    {
                        ok = OFTrue;
                    }
                    break;
                }
                case EPR_Uint32:
                case EPR_Sint32:
                {
                    /* write 32 bit pixel data (OW) */
                    Uint16 *data = NULL;
                    if (pixel->createUint16Array(count * 2 /*double-words*/, data).good() &&
                        InterData->getPixelData(OFstatic_cast(void *, data), OFstatic_cast(size_t, count), fcount, NumberOfFrames, planarConfig))
                    {
                        ok = OFTrue;
                    }
                    break;
                }
            }
            /* check whether pixel data has been written and insert element into the dataset */
            if (ok && dataset.insert(pixel, OFTrue /*replaceOld*/).good())
            {
                char numBuf[20];
                /* set color model */
                if (getInternalColorModel() == EPI_YBR_Full)
                    dataset.putAndInsertString(DCM_PhotometricInterpretation, "YBR_FULL");
                else
                    dataset.putAndInsertString(DCM_PhotometricInterpretation, "RGB");
                /* set image resolution */
                dataset.putAndInsertUint16(DCM_Columns, Columns);
                dataset.putAndInsertUint16(DCM_Rows, Rows);
#if SIZEOF_LONG == 8
                sprintf(numBuf, "%d", NumberOfFrames);
#else
                sprintf(numBuf, "%ld", NumberOfFrames);
#endif
                dataset.putAndInsertString(DCM_NumberOfFrames, numBuf);
                dataset.putAndInsertUint16(DCM_SamplesPerPixel, 3);
                dataset.putAndInsertUint16(DCM_PlanarConfiguration, planarConfig);
                /* set pixel encoding and data */
                switch (InterData->getRepresentation())
                {
                    case EPR_Uint8:
                        dataset.putAndInsertUint16(DCM_BitsAllocated, 8);
                        dataset.putAndInsertUint16(DCM_PixelRepresentation, 0);
                        break;
                    case EPR_Sint8:
                        dataset.putAndInsertUint16(DCM_BitsAllocated, 8);
                        dataset.putAndInsertUint16(DCM_PixelRepresentation, 1);
                        break;
                    case EPR_Uint16:
                        dataset.putAndInsertUint16(DCM_BitsAllocated, 16);
                        dataset.putAndInsertUint16(DCM_PixelRepresentation, 0);
                        break;
                    case EPR_Sint16:
                        dataset.putAndInsertUint16(DCM_BitsAllocated, 16);
                        dataset.putAndInsertUint16(DCM_PixelRepresentation, 1);
                        break;
                    case EPR_Uint32:
                        dataset.putAndInsertUint16(DCM_BitsAllocated, 32);
                        dataset.putAndInsertUint16(DCM_PixelRepresentation, 0);
                        break;
                    case EPR_Sint32:
                        dataset.putAndInsertUint16(DCM_BitsAllocated, 32);
                        dataset.putAndInsertUint16(DCM_PixelRepresentation, 1);
                        break;
                }
                dataset.putAndInsertUint16(DCM_BitsStored, BitsPerSample);
                dataset.putAndInsertUint16(DCM_HighBit, BitsPerSample - 1);
                /* update other DICOM attributes */
                updateImagePixelModuleAttributes(dataset);
                result = 1;
            } else {
                /* pixel data element has not been inserted, thus delete it */
                delete pixel;
            }
        }
    }
    return result;
}


int DiColorImage::writePPM(STD_NAMESPACE ostream& stream,
                           const unsigned long frame,
                           const int bits)
{
    if (RGBColorModel)
    {
        getOutputData(frame, bits);
        if (OutputData != NULL)
        {
            stream << "P3" << OFendl;
            stream << Columns << " " << Rows << OFendl;
            stream << DicomImageClass::maxval(bits) << OFendl;
            int ok = OutputData->writePPM(stream);
            deleteOutputData();
            return ok;
        }
    }
    return 0;
}


int DiColorImage::writePPM(FILE *stream,
                           const unsigned long frame,
                           const int bits)
{
    if (RGBColorModel)
    {
        if (stream != NULL)
        {
            getOutputData(frame, bits);
            if (OutputData != NULL)
            {
                fprintf(stream, "P3\n%u %u\n%lu\n", Columns, Rows, DicomImageClass::maxval(bits));
                int ok = OutputData->writePPM(stream);
                deleteOutputData();
                return ok;
            }
        }
    }
    return 0;
}


int DiColorImage::writeRawPPM(FILE *stream,
                              const unsigned long frame,
                              const int bits)
{
    if (RGBColorModel)
    {
        if ((stream != NULL) && (bits <= MAX_RAWPPM_BITS))
        {
            getOutputData(frame, bits);
            if ((OutputData != NULL) && (OutputData->getData() != NULL))
            {
                fprintf(stream, "P6\n%u %u\n%lu\n", Columns, Rows, DicomImageClass::maxval(bits));
                fwrite(OutputData->getData(), OFstatic_cast(size_t, OutputData->getCount()), OutputData->getItemSize(), stream);
                return 1;
            }
        }
    }
    return 0;
}


int DiColorImage::writeBMP(FILE *stream,
                           const unsigned long frame,
                           const int bits)
{
    if (RGBColorModel && ((bits == 0) || (bits == 24) || (bits == 32)))
        return DiImage::writeBMP(stream, frame, (bits == 0) ? 24 : bits);
    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: dicoimg.cc,v $
 * Revision 1.45  2010-10-14 13:14:14  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.44  2010-08-13 13:24:48  joergr
 * Fixed wrong call to DcmItem::putAndInsertSint32() for setting the value of
 * NumberOfFrames (0028,0008) which has a value representation of IS (not SL).
 *
 * Revision 1.43  2009-11-25 14:43:36  joergr
 * Added new source and header file for the module logger.
 *
 * Revision 1.42  2009-10-14 10:23:56  joergr
 * Fixed minor issues in log output. Also updated copyright date (if required).
 *
 * Revision 1.41  2009-10-13 14:08:33  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.40  2008-05-13 10:05:37  joergr
 * Fixed issue with multi-frame color images: writeImageToDataset() used wrong
 * format for color-by-plane output.
 * Added new parameter to writeImageToDataset() in order to affect the planar
 * configuration of the output image/dataset. Changed behaviour: By default,
 * the output now uses the same planar configuration as the "original" image
 * (previously: always color-by-plane).
 *
 * Revision 1.39  2006/11/17 15:10:59  joergr
 * Only compare stored and computed pixel count for "original" images that are
 * directly loaded from DICOM files or datasets.
 *
 * Revision 1.38  2006/10/27 15:00:49  joergr
 * Fixed wrong warning message about length of pixel data.
 *
 * Revision 1.37  2006/08/15 16:35:01  meichel
 * Updated the code in module dcmimage to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.36  2006/07/10 10:56:51  joergr
 * Added support for 32-bit BMP images.
 *
 * Revision 1.35  2005/12/08 15:42:21  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.34  2005/03/09 17:45:36  joergr
 * Added mode to writeImageToDataset() - only used for monochrome images.
 *
 * Revision 1.33  2004/10/19 12:56:17  joergr
 * Fixed wrong warning message about length of pixel data.
 *
 * Revision 1.32  2004/05/10 10:47:47  joergr
 * Fixed bug that prevented the proper rotation of color images.
 * Removed unused template instantiations (flipping class for signed images).
 *
 * Revision 1.31  2004/02/06 11:20:00  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.30  2003/12/17 16:34:57  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.29  2003/05/20 09:26:25  joergr
 * Added method returning the number of bytes required to store a single
 * rendered frame: getOutputDataSize().
 *
 * Revision 1.28  2002/12/09 13:39:56  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.27  2002/11/26 14:51:13  joergr
 * Added PaletteColorLookupTableUID to the list of attributes to be removed
 * from a newly created dataset.
 *
 * Revision 1.26  2002/09/19 08:33:45  joergr
 * Added explicit type casts to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.25  2002/09/12 14:11:52  joergr
 * Replaced "createPixelData" by "getPixelData" which uses a new dcmdata
 * routine and is therefore more efficient.
 *
 * Revision 1.24  2002/08/29 12:58:34  joergr
 * Fixed bug in writeImageToDataset(): color images were encoded incorrectly in
 * some cases.
 *
 * Revision 1.23  2002/08/02 15:07:21  joergr
 * Added function to write the current image (not only a selected frame) to a
 * DICOM dataset.
 *
 * Revision 1.22  2002/06/26 16:28:53  joergr
 * Enhanced handling of corrupted pixel data and/or length.
 * Added support for polarity flag to color images.
 *
 * Revision 1.21  2002/01/29 17:07:43  joergr
 * Added optional flag to the "Windows DIB" methods allowing to switch off the
 * scanline padding.
 *
 * Revision 1.20  2001/11/09 16:48:53  joergr
 * Added support for Windows BMP file format.
 * Enhanced and renamed createTrueColorDIB() method.
 *
 * Revision 1.19  2001/09/28 13:56:34  joergr
 * Added new flag (CIF_KeepYCbCrColorModel) which avoids conversion of YCbCr
 * color models to RGB.
 *
 * Revision 1.18  2001/06/01 15:49:34  meichel
 * Updated copyright header
 *
 * Revision 1.17  2000/04/28 12:40:03  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.16  2000/04/27 13:15:57  joergr
 * Dcmimage library code now consistently uses ofConsole for error output.
 *
 * Revision 1.15  2000/03/30 14:17:07  joergr
 * Corrected wrong bit expansion of output pixel data (left shift is not
 * correct).
 *
 * Revision 1.14  2000/03/08 16:21:56  meichel
 * Updated copyright header.
 *
 * Revision 1.13  2000/03/03 14:07:54  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.12  1999/08/25 16:58:32  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.11  1999/08/17 10:29:14  joergr
 * Removed unused parameter.
 *
 * Revision 1.10  1999/07/23 13:18:48  joergr
 * Enhanced handling of corrupted pixel data (wrong length).
 *
 * Revision 1.9  1999/05/03 11:03:21  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.8  1999/04/28 13:19:28  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.7  1999/02/03 16:56:38  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 *
 * Revision 1.6  1999/01/20 14:49:29  joergr
 * Added new output method to fill external memory buffer with rendered pixel
 * data.
 *
 * Revision 1.5  1998/12/14 17:11:12  joergr
 * Added implementation of method to create rotated color images.
 *
 * Revision 1.4  1998/11/27 14:27:25  joergr
 * Added copyright message.
 * Added method to detach pixel data if it is no longer needed.
 *
 * Revision 1.3  1998/05/11 14:52:26  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
