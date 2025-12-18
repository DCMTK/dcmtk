/*
 *  Copyright (C) 2015-2025, Open Connections GmbH
 *
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
 *  Module:  dcmseg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class representing a Segmentation object
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmfg/concatenationcreator.h"
#include "dcmtk/dcmfg/fgderimg.h"
#include "dcmtk/dcmfg/fgseg.h"
#include "dcmtk/dcmiod/iodtypes.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmseg/segdoc.h"
#include "dcmtk/dcmseg/segment.h"
#include "dcmtk/dcmseg/segtypes.h"
#include "dcmtk/dcmseg/segutils.h"
#include "dcmtk/ofstd/ofutil.h"


struct DcmSegmentation::SetRowsAndCols
{
    SetRowsAndCols(const Uint16 rows, const Uint16 cols)
        : m_rows(rows)
        , m_cols(cols)
    {
    }

    template <typename T>
    OFCondition operator()(T& t)
    {
        t.setRows(m_rows);
        t.setColumns(m_cols);
        return EC_Normal;
    }

    // Members
    const Uint16 m_rows;
    const Uint16 m_cols;
};


struct DcmSegmentation::SetImagePixelModuleVisitor
{
    SetImagePixelModuleVisitor(const Uint16 rows, const Uint16 columns, const Uint16 bitsAllocated, const Uint16 bitsStored,
                               const Uint16 highBit, const Uint16 samplesPerPixel, const OFString& photometricInterpretation)
        : m_rows(rows)
        , m_cols(columns)
        , m_bitsAllocated(bitsAllocated)
        , m_bitsStored(bitsStored)
        , m_highBit(highBit)
        , m_samplesPerPixel(samplesPerPixel)
        , m_photometricInterpretation(photometricInterpretation)
        , m_pixelRepresentation(2) // invalid default value
    {
    }

    template <typename T>
    OFCondition operator()(T& t)
    {
        if ((m_rows == 0) || (m_cols == 0))
        {
            DCMSEG_ERROR("Rows/Cols must be non-zero but are : " << m_rows << "/" << m_cols);
            return IOD_EC_InvalidDimensions;
        }

        t.setRows(m_rows);
        t.setColumns(m_cols);
        t.setBitsAllocated(m_bitsAllocated);
        t.setBitsStored(m_bitsStored);
        t.setHighBit(m_highBit);
        t.setSamplesPerPixel(m_samplesPerPixel);
        t.setPhotometricInterpretation(m_photometricInterpretation);
        t.setPixelRepresentation(OFis_signed<T>::value ? 1 : 0);

        return EC_Normal;
    }

    // Members
    const Uint16 m_rows;
    const Uint16 m_cols;
    const Uint16 m_bitsAllocated;
    const Uint16 m_bitsStored;
    const Uint16 m_highBit;
    const Uint16 m_samplesPerPixel;
    const OFString m_photometricInterpretation;
    const Uint16 m_pixelRepresentation;

};

template <typename ImagePixel>
DcmSegmentation::DcmSegmentation(OFin_place_type_t(ImagePixel))
    : IODImage(OFin_place<ImagePixel>)
    , m_SegmentationSeries(DcmSegmentation::IODImage::getData(), DcmSegmentation::IODImage::getRules())
    , m_EnhancedGeneralEquipmentModule(DcmSegmentation::IODImage::getData(), DcmSegmentation::IODImage::getRules())
    , m_FG(DcmSegmentation::IODImage::getData(), DcmSegmentation::IODImage::getRules())
    , m_DimensionModule(DcmSegmentation::IODImage::getData(), DcmSegmentation::IODImage::getRules())
    , m_PaletteColorLUTModule(DcmSegmentation::IODImage::getData(), DcmSegmentation::IODImage::getRules())
    , m_Frames()
    , m_16BitPixelData(OFFalse)
    , m_LabelmapColorModel(DcmSegTypes::SLCM_UNKNOWN)
    , m_ImageType("DERIVED\\PRIMARY")
    , m_ContentIdentificationMacro()
    , m_SegmentationType(DcmSegTypes::ST_BINARY)
    , m_SegmentationFractionalType(DcmSegTypes::SFT_OCCUPANCY)
    , m_MaximumFractionalValue(DCM_MaximumFractionalValue)
    , m_Segments()
    , m_FGInterface()
    , m_inputXfer(EXS_Unknown)
{
    DcmSegmentation::initIODRules();
}

void DcmSegmentation::initIODRules()
{
    // ------------ Segmentation Image Module -------------

    // Partly overrides rules from General Image Module
    DcmSegmentation::IODImage::getRules()->addRule(
        new IODRule(DCM_ImageType, "2", "1", "SegmentationImageModule", DcmIODTypes::IE_IMAGE), OFTrue);
    DcmSegmentation::IODImage::getRules()->addRule(
        new IODRule(DCM_SegmentationType, "1", "1", "SegmentationImageModule", DcmIODTypes::IE_IMAGE), OFTrue);
    DcmSegmentation::IODImage::getRules()->addRule(
        new IODRule(DCM_SegmentationFractionalType, "1", "1C", "SegmentationImageModule", DcmIODTypes::IE_IMAGE),
        OFTrue);
    DcmSegmentation::IODImage::getRules()->addRule(
        new IODRule(DCM_MaximumFractionalValue, "1", "1C", "SegmentationImageModule", DcmIODTypes::IE_IMAGE), OFTrue);
    // Instance Number is Type 1 in Segmentation Image Module
    DcmSegmentation::IODImage::getRules()->addRule(
        new IODRule(DCM_InstanceNumber, "1", "1", "SegmentationImageModule", DcmIODTypes::IE_IMAGE), OFTrue);


    // Re-use General Image Module instead of Segmentation Image Module
    DcmSegmentation::IODImage::getRules()->addRule(
        new IODRule(DCM_LossyImageCompression, "1", "1", "GeneralImageModule", DcmIODTypes::IE_IMAGE), OFTrue);
    DcmSegmentation::IODImage::getRules()->addRule(
        new IODRule(DCM_LossyImageCompressionMethod, "1-n", "1C", "GeneralImageModule", DcmIODTypes::IE_IMAGE), OFTrue);
    DcmSegmentation::IODImage::getRules()->addRule(
        new IODRule(DCM_LossyImageCompressionRatio, "1-n", "1C", "GeneralImageModule", DcmIODTypes::IE_IMAGE), OFTrue);

    // ------------ Segmentation Series Module -------------

    // Override rule from General Series Module
    DcmSegmentation::IODImage::getRules()->addRule(new IODRule(DCM_ReferencedPerformedProcedureStepSequence,
                                                               "1",
                                                               "1C",
                                                               "SegmentationSeriesModule",
                                                               DcmIODTypes::IE_SERIES),
                                                   OFTrue);
    DcmSegmentation::IODImage::getRules()->addRule(
        new IODRule(DCM_SeriesNumber, "1", "1", "SegmentationSeriesModule", DcmIODTypes::IE_SERIES), OFTrue);
}

DcmSegmentation::~DcmSegmentation()
{
    clearData();
}

// static method for loading segmentation objects
OFCondition DcmSegmentation::loadFile(const OFString& filename, DcmSegmentation*& segmentation, const DcmSegmentation::LoadingFlags& flags)
{
    DcmFileFormat dcmff;
    DcmDataset* dataset = NULL;
    OFCondition result  = loadFile(dcmff, filename, dataset, flags.m_readTransferSyntax);
    if (result.bad())
        return result;

    return loadDataset(*dataset, segmentation, flags);
}


// static method for loading segmentation objects
OFCondition DcmSegmentation::loadDataset(DcmDataset& dataset, DcmSegmentation*& segmentation, const DcmSegmentation::LoadingFlags& flags)
{
    segmentation       = NULL;
    OFCondition result = DcmSegmentation::decompress(dataset);
    if (result.bad())
        return result;

    DcmSegmentation *temp = NULL;
    result = createRequiredBitDepth(dataset, temp);
    if (result.bad())
    {
        return result;
    }
    // Apply the loading flags
    temp->getFunctionalGroups().setUseThreads(flags.m_numThreads);
    // Start actual reading
    result = temp->read(dataset);
    if (result.good())
    {
        segmentation = temp;
        segmentation->m_inputXfer = dataset.getOriginalXfer();
    }
    else
    {
        delete segmentation;
    }
    return result;
}

OFCondition DcmSegmentation::loadConcatenation(ConcatenationLoader& cl,
                                               const OFString& concatenationUID,
                                               DcmSegmentation*& segmentation)
{
    DcmDataset dset;
    segmentation = NULL;
    OFVector<DcmIODTypes::FrameBase*> frames;
    OFCondition result = cl.load(concatenationUID, &dset, frames);
    if (result.good())
    {
        result = createRequiredBitDepth(dset, segmentation);
        if (result.good())
        {
            if (segmentation)
            {
                result = segmentation->readWithoutPixelData(dset);
                if (result.good())
                {
                    segmentation->m_Frames = frames;
                    // We don't check the transfer syntax in the input files,
                    // so even all files had the same transfer syntax, we always
                    // set it to EXS_Unknown.
                    segmentation->m_inputXfer = EXS_Unknown;
                }
            }
            else
            {
                result = EC_MemoryExhausted;
            }
        }
    }
    if (result.bad())
    {
        DcmIODUtil::freeContainer(frames);
        delete segmentation;
    }
    return result;
}

OFCondition DcmSegmentation::createBinarySegmentation(DcmSegmentation*& segmentation,
                                                      const Uint16 rows,
                                                      const Uint16 columns,
                                                      const IODGeneralEquipmentModule::EquipmentInfo& equipmentInfo,
                                                      const ContentIdentificationMacro& contentIdentification)
{

    OFCondition result = createCommon(segmentation, rows, columns, equipmentInfo, contentIdentification, 8);
    if (result.bad())
        return result;

    segmentation->m_SegmentationType = DcmSegTypes::ST_BINARY;

    return result;
}


OFCondition DcmSegmentation::createLabelmapSegmentation(DcmSegmentation *&segmentation,
                                                        const Uint16 rows,
                                                        const Uint16 columns,
                                                        const IODGeneralEquipmentModule::EquipmentInfo &equipmentInfo,
                                                        const ContentIdentificationMacro &contentIdentification,
                                                        const OFBool use16Bit,
                                                        const DcmSegTypes::E_SegmentationLabelmapColorModel colorModel)
{
    if (colorModel == DcmSegTypes::SLCM_UNKNOWN)
    {
        DCMSEG_ERROR("Cannot create labelmap segmentation: Color model not set");
        return EC_IllegalParameter;
    }

    OFCondition result = createCommon(segmentation, rows, columns, equipmentInfo, contentIdentification, use16Bit ? 16 : 8);
    if (result.bad())
        return result;

    segmentation->m_SegmentationType = DcmSegTypes::ST_LABELMAP;
    segmentation->m_16BitPixelData = use16Bit;
    segmentation->m_LabelmapColorModel = colorModel;
    return result;
}

OFCondition DcmSegmentation::createFractionalSegmentation(DcmSegmentation*& segmentation,
                                                          const Uint16 rows,
                                                          const Uint16 columns,
                                                          const DcmSegTypes::E_SegmentationFractionalType fractType,
                                                          const Uint16& maxFractionalValue,
                                                          const IODGeneralEquipmentModule::EquipmentInfo& equipmentInfo,
                                                          const ContentIdentificationMacro& contentIdentification)
{
    OFCondition result = createCommon(segmentation, rows, columns, equipmentInfo, contentIdentification, 8);
    if (result.bad())
        return result;

    segmentation->m_SegmentationType           = DcmSegTypes::ST_FRACTIONAL;
    segmentation->m_SegmentationFractionalType = fractType;
    segmentation->m_MaximumFractionalValue.putUint16(maxFractionalValue);

    return result;
}

OFCondition DcmSegmentation::createCommon(DcmSegmentation*& segmentation,
                                          const Uint16 rows,
                                          const Uint16 columns,
                                          const IODGeneralEquipmentModule::EquipmentInfo& equipmentInfo,
                                          const ContentIdentificationMacro& contentIdentification,
                                          const Uint16 bitsAllocated)
{
    if ((rows == 0) || (columns == 0))
    {
        DCMSEG_ERROR("Segmentation must have at least 1 row and 1 column");
        return EC_IllegalParameter;
    }

    OFCondition result = createRequiredBitDepth(bitsAllocated, segmentation);
    if (segmentation == NULL)
        return EC_MemoryExhausted;

    segmentation->getImagePixel().setRows(rows);
    segmentation->getImagePixel().setColumns(columns);

    DCMSEG_DEBUG("Setting segmentation content identification");
    result = segmentation->setContentIdentification(contentIdentification);
    if (result.good())
    {
        DcmIODUtil::setContentDateAndTimeNow(segmentation->getGeneralImage());
        DCMSEG_DEBUG("Setting segmentation equipment information");
        result = segmentation->setEquipmentInfo(equipmentInfo, OFTrue /* check */);
    }

    if (result.bad())
    {
        delete segmentation;
        segmentation = NULL;
    }

    return result;
}

OFCondition DcmSegmentation::createRequiredBitDepth(const Uint16 bitsAllocated, DcmSegmentation*& seg)
{
    OFCondition result;
    seg = NULL;
    if (bitsAllocated == 16)
    {
        seg = new DcmSegmentation(OFin_place<IODImagePixelModule<Uint16> >);
    }
    else if ( (bitsAllocated == 8) || (bitsAllocated == 1) )
    {
        seg = new DcmSegmentation(OFin_place<IODImagePixelModule<Uint8> >);
    }
    else
    {
        DCMSEG_ERROR("Cannot read segmentation object: Bits Allocated is neither 8 nor 16");
        result = IOD_EC_InvalidPixelData;
    }
    if (result.good() && (seg == NULL))
    {
        result = EC_MemoryExhausted;
    }
    return result;

}

OFCondition DcmSegmentation::createRequiredBitDepth(DcmItem& item, DcmSegmentation*& seg)
{
    Uint16 bitsAllocated = 0;
    OFCondition result = item.findAndGetUint16(DCM_BitsAllocated, bitsAllocated);
    if (result.bad())
    {
        DCMSEG_ERROR("Cannot read Bits Allocated from dataset: " << result.text());
        return result;
    }
    return createRequiredBitDepth(bitsAllocated, seg);
}


FGDerivationImage*
DcmSegmentation::createDerivationImageFG(const OFVector<ImageSOPInstanceReferenceMacro>& derivationImages,
                                         const OFString& derivationDescription)
{
    CodeSequenceMacro derivationCode("113076", "DCM", "Segmentation");
    CodeSequenceMacro purpose("121322", "DCM", "Source Image for Image Processing Operation");
    return FGDerivationImage::createMinimal(derivationImages, derivationDescription, derivationCode, purpose);
}

OFCondition DcmSegmentation::read(DcmItem& dataset)
{

    OFCondition result = readWithoutPixelData(dataset);
    if (result.good())
        result = readFrames(dataset);
    return result;
}

OFCondition DcmSegmentation::readWithoutPixelData(DcmItem& dataset)
{
    OFString sopClass;
    if (DcmIODUtil::checkSOPClass(&dataset, UID_SegmentationStorage, sopClass).bad()
    && DcmIODUtil::checkSOPClass(&dataset, UID_LabelMapSegmentationStorage, sopClass).bad())
    {
        DCMSEG_ERROR("Given file does not seem to be a segmentation storage object since SOP class is: " << sopClass);
        return IOD_EC_WrongSOPClass;
    }

    // Read attributes in base classes
    OFCondition result = DcmSegmentation::IODImage::read(dataset);
    if (result.bad())
        DCMSEG_WARN("Problem with reading Image Pixel attributes: " << result.text() << " (ignoring)");

    // Read Segmentation Series Module
    m_SegmentationSeries.read(dataset);

    // Read Enhanced General Equipment (i.e. make sure all type 1 elements are
    // there, which is not checked in General Equipment Module being part of
    // DcmIODImage.
    m_EnhancedGeneralEquipmentModule.read(dataset);

    // Read functional groups module
    m_FG.read(dataset);

    // Read functional groups itself
    m_FGInterface.read(dataset);

    // Read dimension information
    m_DimensionModule.read(dataset);

    readSegmentationType(dataset);

    readSegments(dataset);

    if (m_SegmentationType == DcmSegTypes::ST_FRACTIONAL)
        readSegmentationFractionalType(dataset);

    m_ContentIdentificationMacro.read(dataset);

    // Read Photometric Interpretation and check whether it fits the segmentation type
    OFString colorModel;
    if (dataset.findAndGetOFString(DCM_PhotometricInterpretation, colorModel).good() && (colorModel == "PALETTE COLOR"))
    {
        // For palette, read Palette Color LUT Module and ICC Profile Module
        m_PaletteColorLUTModule.read(dataset);
        m_ICCProfileModule.read(dataset);
    }
    checkColorModel(colorModel);

    // Read specific segmentation elements
    DcmIODUtil::getAndCheckElementFromDataset(
        dataset, m_MaximumFractionalValue, DcmSegmentation::IODImage::getRules()->getByTag(DCM_MaximumFractionalValue));

    return EC_Normal;
}

void DcmSegmentation::setCheckFGOnWrite(const OFBool doCheck)
{
    m_FGInterface.setCheckOnWrite(doCheck);
}

OFBool DcmSegmentation::getCheckFGOnWrite()
{
    return m_FGInterface.getCheckOnWrite();
}

void DcmSegmentation::setValueCheckOnWrite(const OFBool doCheck)
{
    m_SegmentationSeries.setValueCheckOnWrite(doCheck);
    m_EnhancedGeneralEquipmentModule.setValueCheckOnWrite(doCheck);
    m_PaletteColorLUTModule.setValueCheckOnWrite(doCheck);
    m_DimensionModule.setValueCheckOnWrite(doCheck);
    m_ICCProfileModule.setValueCheckOnWrite(doCheck);
    DcmSegmentation::IODImage::setValueCheckOnWrite(doCheck);
}

void DcmSegmentation::setCheckDimensionsOnWrite(const OFBool doCheck)
{
    m_DimensionModule.setCheckOnWrite(doCheck);
}

OFBool DcmSegmentation::getCheckDimensionsOnWrite()
{
    return m_DimensionModule.getCheckOnWrite();
}

E_TransferSyntax DcmSegmentation::getInputTransferSyntax() const
{
    return m_inputXfer;
}

OFCondition DcmSegmentation::writeWithSeparatePixelData(DcmItem& dataset, Uint8*& pixData, size_t& pixDataLength)
{
    // FGInterface::write() will know whether it has to check FG structure
    // so we do not need to check FG structure here (OFFalse).
    if (!check(OFFalse))
    {
        return IOD_EC_InvalidObject;
    }

    OFCondition result;

    // -- Set constant default values written by external modules --
    DcmSegmentation::IODImage::getGeneralImage().setLossyImageCompression("00");
    DcmSegmentation::IODImage::getGeneralImage().setImageType(m_ImageType);
    // Set Instance Number if not set
    OFString InstanceNumber;
    getData()->findAndGetOFString(DCM_InstanceNumber, InstanceNumber);
    if (InstanceNumber.empty())
    {
        DcmSegmentation::IODImage::getGeneralImage().setInstanceNumber("1");
    }

    // Set SOP Class UID based on segmentation type
    setSOPClassUIDBasedOnSegmentationType();

    // -- Extra Study level data --

    // Enhanced Equipment Module
    if (result.good())
        result = m_EnhancedGeneralEquipmentModule.write(dataset);

    // -- Extra Series level data --

    // Write segmentation-specific series level attribute (Segmentation Series Module)
    if (result.good())
        result = m_SegmentationSeries.write(dataset);

    // -- Extra Image level data --

    // Write Multi-Frame Functional Groups Module
    if (result.good())
        result = writeMultiFrameFunctionalGroupsModule(dataset);

    // Write Multi-Frame Dimension Module
    if (result.good())
        result = writeMultiFrameDimensionModule(dataset);

    // Write Palette Color Lookup Table Module and ICC Profile Module
    if (result.good())
    {
        if (m_LabelmapColorModel == DcmSegTypes::SLCM_PALETTE)
        {
            result = m_PaletteColorLUTModule.write(dataset);
            if (result.good())
            {
                result = m_ICCProfileModule.write(dataset);
            }
        }
    }

    // Write segmentation image module and image pixel module
    if (result.good())
        result = writeSegmentationImageModule(dataset);

    // -- Write common multi frame image IOD attributes --

    // Patient Module
    // General Study Module
    // General Series Module
    // Frame of Reference Module
    // General Equipment Module
    // General Image Module
    // Multi-frame Functional Groups Module (except functional groups itself)
    // SOP Common Module
    // Common Instance Reference Module
    if (result.good())
        result = DcmSegmentation::IODImage::write(dataset);

    // Write frame pixel data
    if (result.good())
    {
        Uint32 numFrames = DcmIODUtil::limitMaxFrames(
            m_Frames.size(), "More than 2147483647 frames provided, will only write 2147483647");
        Uint16 rows, cols, bytesPerPixel;
        rows = cols = bytesPerPixel = 0;
        DcmSegmentation::getImagePixel().getRows(rows);
        DcmSegmentation::getImagePixel().getColumns(cols);
        DcmSegmentation::getImagePixel().getBitsAllocated(bytesPerPixel);
        if (bytesPerPixel >= 8) // 8 or 16
        {
            bytesPerPixel = bytesPerPixel / 8;
        }
        result = getTotalBytesRequired(rows, cols, bytesPerPixel, numFrames, pixDataLength);
        if (result.bad())
            return result;

        pixData = new Uint8[pixDataLength];
        if (!pixData)
            return EC_MemoryExhausted;

        switch (m_SegmentationType)
        {
            case DcmSegTypes::ST_BINARY:
                result = writeBinaryFrames(pixData, rows, cols, pixDataLength);
                break;
            case DcmSegTypes::ST_FRACTIONAL:
            case DcmSegTypes::ST_LABELMAP:
                result = writeByteBasedFrames(pixData);
                break;
            default:
                result = SG_EC_UnknownSegmentationType;
        }
        if (result.bad())
        {
            delete[] pixData;
        }
    }

    return result;
}

OFCondition DcmSegmentation::writeWithSeparatePixelData(DcmItem& dataset, Uint16*& pixData, size_t& numPixelDataBytes)
{
    // FGInterface::write() will know whether it has to check FG structure
    // so we do not need to check FG structure here (OFFalse).
    if (!check(OFFalse))
    {
        return IOD_EC_InvalidObject;
    }

    OFCondition result;

    // -- Set constant default values written by external modules --

    DcmSegmentation::IODImage::getGeneralImage().setLossyImageCompression("00");
    // Labelmap segmentations have a different SOP Class UID
    DcmSegmentation::IODImage::getGeneralImage().setImageType(m_ImageType);

    // Set SOP Class UID based on segmentation type
    setSOPClassUIDBasedOnSegmentationType();

    // -- Extra Study level data --

    // Enhanced Equipment Module
    if (result.good())
        result = m_EnhancedGeneralEquipmentModule.write(dataset);

    // -- Extra Series level data --

    // Write segmentation-specific series level attribute (Segmentation Series Module)
    if (result.good())
        result = m_SegmentationSeries.write(dataset);

    // -- Extra Image level data --

    // Write Multi-Frame Functional Groups Module
    if (result.good())
        result = writeMultiFrameFunctionalGroupsModule(dataset);

    // Write Multi-Frame Dimension Module
    if (result.good())
        result = writeMultiFrameDimensionModule(dataset);

    // Write Palette Color Lookup Table Module and ICC Profile Module
    if (result.good())
    {
        if (m_LabelmapColorModel == DcmSegTypes::SLCM_PALETTE)
        {
            result = m_PaletteColorLUTModule.write(dataset);
            if (result.good())
            {
                result = m_ICCProfileModule.write(dataset);
            }
        }
    }

    // Write segmentation image module and image pixel module
    if (result.good())
        result = writeSegmentationImageModule(dataset);

    // -- Write common multi frame image IOD attributes --

    // Patient Module
    // General Study Module
    // General Series Module
    // Frame of Reference Module
    // General Equipment Module
    // General Image Module
    // Multi-frame Functional Groups Module (except functional groups itself)
    // SOP Common Module
    // Common Instance Reference Module
    if (result.good())
        result = DcmSegmentation::IODImage::write(dataset);

    // Write frame pixel data
    if (result.good())
    {
        Uint32 numFrames = DcmIODUtil::limitMaxFrames(
            m_Frames.size(), "More than 2147483647 frames provided, will only write 2147483647");
        Uint16 rows, cols, bytesPerPixel;
        rows = cols = bytesPerPixel = 0;
        getImagePixel().getRows(rows);
        getImagePixel().getColumns(cols);
        getImagePixel().getBitsAllocated(bytesPerPixel);
        if (bytesPerPixel >= 8) // 8 or 16
        {
            bytesPerPixel = bytesPerPixel / 8;
        }
        result = getTotalBytesRequired(rows, cols, bytesPerPixel, numFrames, numPixelDataBytes);
        if (result.bad())
            return result;

        pixData = new Uint16[numPixelDataBytes / 2]; // 16 bit data
        if (!pixData)
            return EC_MemoryExhausted;

        switch (m_SegmentationType)
        {
            case DcmSegTypes::ST_BINARY:
                DCMSEG_ERROR("Binary segmentations must be instantiated with 8 bit pixel data (Uint8)");
                result = IOD_EC_InvalidPixelData;
                break;
            case DcmSegTypes::ST_FRACTIONAL:
                DCMSEG_ERROR("Fractional segmentations must be instantiated with 8 bit pixel data (Uint8)");
                result = IOD_EC_InvalidPixelData;
                break;
            case DcmSegTypes::ST_LABELMAP:
                result = writeByteBasedFrames(pixData);
                break;
            default:
                result = SG_EC_UnknownSegmentationType;
        }
        if (result.bad())
        {
            delete[] pixData;
        }
    }

    return result;
}

FGInterface& DcmSegmentation::getFunctionalGroups()
{
    return m_FGInterface;
}

IODMultiFrameFGModule::ConcatenationInfo& DcmSegmentation::getConcatenationInfo()
{
    return m_FG.getConcatenationInfo();
}

size_t DcmSegmentation::getNumberOfFrames()
{
    return m_FGInterface.getNumberOfFrames();
}

OFBool DcmSegmentation::has16BitPixelData() const
{
    return this->m_16BitPixelData;
}

Uint16 DcmSegmentation::getRows()
{
    Uint16 rows = 0;
    DcmSegmentation::getImagePixel().getRows(rows);
    return rows;
}

Uint16 DcmSegmentation::getColumns()
{
    Uint16 cols = 0;
    DcmSegmentation::getImagePixel().getColumns(cols);
    return cols;
}

size_t DcmSegmentation::getNumberOfSegments()
{
    return m_Segments.size();
}

IODGeneralEquipmentModule& DcmSegmentation::getEquipment()
{
    return DcmSegmentation::IODImage::getEquipment();
}

IODSegmentationSeriesModule& DcmSegmentation::getSegmentationSeriesModule()
{
    return m_SegmentationSeries;
}

OFCondition DcmSegmentation::addSegment(DcmSegment* seg, Uint16& segmentNumber)
{
    if (seg == NULL)
        return EC_IllegalParameter;

    if (m_Segments.size() >= DCM_SEG_MAX_SEGMENTS)
    {
        return SG_EC_MaxSegmentsReached;
    }
    // For labelmaps, use the given segment number
    if (m_SegmentationType == DcmSegTypes::ST_LABELMAP)
    {
        m_Segments.insert(OFMake_pair(segmentNumber, seg));
        return EC_Normal;
    }
    // For binary/fractional, start with 1 if no segment exists
    if (m_Segments.empty())
    {
        segmentNumber = 1;
        m_Segments.insert(OFMake_pair(segmentNumber, seg));
        return EC_Normal;
    }
    // Use next free segment number and insert
    // (i.e. get the highest segment number and increment by 1).
    OFMap<Uint16, DcmSegment*>::iterator it = m_Segments.end();
    it--;
    segmentNumber = it->first + 1;
    m_Segments.insert(OFMake_pair(segmentNumber, seg));
    return EC_Normal;
}


SOPInstanceReferenceMacro& DcmSegmentation::getReferencedPPS()
{
    return DcmSegmentation::IODImage::getSeries().getReferencedPPS();
}

const DcmIODTypes::FrameBase* DcmSegmentation::getFrame(const size_t& frameNo)
{
    if (frameNo > m_Frames.size() - 1)
    {
        return NULL;
    }

    return m_Frames[frameNo];
}

void DcmSegmentation::getFramesForSegment(const size_t& segmentNumber, OFVector<size_t>& frameNumbers)
{
    size_t numFrames = getNumberOfFrames();
    for (size_t count = 0; count < numFrames; count++)
    {
        FGSegmentation* fg = OFstatic_cast(
            FGSegmentation*, m_FGInterface.get(OFstatic_cast(Uint32, count), DcmFGTypes::EFG_SEGMENTATION));
        if (fg == NULL)
        {
            DCMSEG_ERROR("Cannot get segmentation functional group for frame " << count);
            return;
        }
        Uint16 refSeg;
        if (fg->getReferencedSegmentNumber(refSeg).good())
        {
            if (OFstatic_cast(size_t, refSeg) == segmentNumber)
            {
                frameNumbers.push_back(count);
            }
        }
    }
}

OFCondition DcmSegmentation::addForAllFrames(const FGBase& group)
{
    return m_FGInterface.addShared(group);
}

ContentIdentificationMacro& DcmSegmentation::getContentIdentification()
{
    return m_ContentIdentificationMacro;
}

IODMultiframeDimensionModule& DcmSegmentation::getDimensions()
{
    return m_DimensionModule;
}

IODPaletteColorLUTModule& DcmSegmentation::getPaletteColorLUT()
{
    return m_PaletteColorLUTModule;
}

IODICCProfileModule& DcmSegmentation::getICCProfile()
{
    return m_ICCProfileModule;
}

OFCondition
DcmSegmentation::setLossyImageCompressionFlag(const OFString& ratios, const OFString& methods, const OFBool checkValues)
{
    OFCondition result = DcmSegmentation::IODImage::getGeneralImage().setLossyImageCompression("01");
    if (result.good() || !checkValues)
        result = DcmSegmentation::IODImage::getGeneralImage().setLossyImageCompressionMethod(methods);
    if (result.good() || !checkValues)
        result = DcmSegmentation::IODImage::getGeneralImage().setLossyImageCompressionRatio(ratios);

    if (checkValues)
        return result;
    else
        return EC_Normal;
}

OFCondition DcmSegmentation::saveFile(const OFString& filename, const E_TransferSyntax writeXfer)
{
    if ((writeXfer != EXS_LittleEndianExplicit) && (writeXfer != EXS_BigEndianExplicit)
        && (writeXfer != EXS_LittleEndianImplicit)
#ifdef WITH_ZLIB
        && (writeXfer != EXS_DeflatedLittleEndianExplicit)
#endif
    )
    {
        if ((writeXfer == EXS_RLELossless) && (m_SegmentationType != DcmSegTypes::ST_LABELMAP))
        {
            DcmXfer ts(writeXfer);
#ifdef WITH_ZLIB
            DCMSEG_ERROR("Cannot write transfer syntax: " << ts.getXferName()
                                                      << ": Can only write uncompressed or Deflated)");
#else
            if (writeXfer == EXS_DeflatedLittleEndianExplicit)
            {
                DCMSEG_ERROR("Cannot write transfer syntax: "
                             << ts.getXferName() << ": Deflate (ZLIB) support disabled");
            }
#endif
            return EC_CannotChangeRepresentation;
        }
    }

    DcmFileFormat dcmff;
    OFCondition result = writeDataset(*(dcmff.getDataset()));
    if (result.good())
    {
        if (dcmff.chooseRepresentation(writeXfer, NULL).good() && dcmff.getDataset()->canWriteXfer(writeXfer))
        {
            result = dcmff.saveFile(filename.c_str(), writeXfer, EET_ExplicitLength);
        }
        else
        {
            DcmXfer ts(writeXfer);
            DCMSEG_ERROR("Cannot write transfer syntax: " << ts.getXferName());
            result = EC_CannotChangeRepresentation;
        }
    }
    if (result.bad())
    {
        DCMSEG_ERROR("Cannot save segmentation document to file " << filename << ": " << result.text());
    }

    return result;
}

/* -- Setter for DICOM attributes -- */

OFCondition DcmSegmentation::setEquipmentInfo(const IODGeneralEquipmentModule::EquipmentInfo& equipmentInfo,
                                              const OFBool checkValue)
{

    if (checkValue)
    {
        if (equipmentInfo.m_Manufacturer.empty() || equipmentInfo.m_ManufacturerModelName.empty()
            || equipmentInfo.m_DeviceSerialNumber.empty() || equipmentInfo.m_SoftwareVersions.empty())
        {
            return EC_InvalidValue;
        }
    }

    OFCondition result = getEquipment().setManufacturer(equipmentInfo.m_Manufacturer, checkValue);
    if (result.good())
        result = getEquipment().setManufacturerModelName(equipmentInfo.m_ManufacturerModelName, checkValue);
    if (result.good())
        result = getEquipment().setDeviceSerialNumber(equipmentInfo.m_DeviceSerialNumber, checkValue);
    if (result.good())
        result = getEquipment().setSoftwareVersions(equipmentInfo.m_SoftwareVersions, checkValue);

    return result;
}

OFCondition DcmSegmentation::setContentIdentification(const ContentIdentificationMacro& contentIdentification,
                                                      const OFBool checkValue)
{
    // Instance Number and Content Label must be filled out, rest can be empty
    OFCondition result;
    if (checkValue)
    {
        result = OFconst_cast(ContentIdentificationMacro*, &contentIdentification)->check();
    }
    if (result.bad())
        return result;

    m_ContentIdentificationMacro = contentIdentification;

    return result;
}

/* -- Getter for DICOM attributes -- */

DcmSegment* DcmSegmentation::getSegment(const Uint16 segmentNumber)
{
    // check for invalid index
    if ( (m_SegmentationType != DcmSegTypes::ST_LABELMAP) && (segmentNumber == 0)  )
    {
        DCMSEG_ERROR("Cannot get segment 0: No such Segment Number allowed segmentation if segmentation is of type " << DcmSegTypes::segtype2OFString(m_SegmentationType));
        return NULL;
    }
    OFMap<Uint16, DcmSegment*>::iterator it = m_Segments.find(segmentNumber);
    if (it == m_Segments.end())
    {
        return NULL;
    }

    return it->second;
}

OFBool DcmSegmentation::getSegmentNumber(const DcmSegment* segment, size_t& segmentNumber)
{
    if (segment == NULL)
        return OFFalse;

    OFMap<Uint16, DcmSegment*>::iterator it = m_Segments.begin();
    while (it != m_Segments.end())
    {
        if (it->second == segment)
        {
            segmentNumber = it->first;
            return OFTrue;
        }
        it++;
    }
    return OFFalse;
}

const OFMap<Uint16, DcmSegment*>& DcmSegmentation::getSegments()
{
    return m_Segments;
}

OFCondition DcmSegmentation::getModality(OFString& value, const long signed int pos) const
{
    (void)pos;
    // Fixed for Segmentations to value "SEG"
    value = "SEG";
    return EC_Normal;
}

OFCondition DcmSegmentation::importFromSourceImage(const OFString& filename, const bool takeOverCharset)
{
    DcmFileFormat dcmff;
    OFCondition result = dcmff.loadFile(filename);
    if (result.good())
    {
        return importFromSourceImage(*(dcmff.getDataset()), takeOverCharset);
    }
    return result;
}

OFCondition DcmSegmentation::importFromSourceImage(DcmItem& dataset, const bool takeOverCharset)
{
    OFString FoR;
    dataset.findAndGetOFStringArray(DCM_FrameOfReferenceUID, FoR);
    return DcmIODCommon::importHierarchy(dataset,
                                         OFTrue,       // Patient
                                         OFTrue,       // Study
                                         !FoR.empty(), // Frame of Reference
                                         OFFalse,      // Series
                                         takeOverCharset);
}

/* protected functions */

OFCondition DcmSegmentation::writeSegments(DcmItem& item)
{
    OFCondition result;
    // writeSubSequence cannot handle a map, copy to vector instead and use that for this purpose
    OFVector<DcmSegment*> segments;
    OFMap<Uint16, DcmSegment*>::iterator it = m_Segments.begin();
    while (it != m_Segments.end())
    {
        segments.push_back(it->second);
        it++;
    }
    DcmIODUtil::writeSubSequence<OFVector<DcmSegment*> >(
        result, DCM_SegmentSequence, segments, item, "1-n", "1", "SegmentationImageModule");
    return result;
}

OFCondition DcmSegmentation::readSegments(DcmItem& item)
{
    // readSubSequence cannot handle a map, read into vector instead and fill into map afterwards
    OFVector<DcmSegment*> segments;

    OFCondition result = DcmIODUtil::readSubSequence<OFVector<DcmSegment*> >(
        item, DCM_SegmentSequence, segments, "1-n", "1", "SegmentationImageModule");
    if (result.good())
    {
        for (size_t count = 0; count < segments.size(); count++)
        {
            if (result.good())
            {
                if (m_Segments.insert(OFMake_pair(segments[count]->getSegmentNumberRead(), segments[count])).second
                    == false)
                {
                    DCMSEG_ERROR("Cannot insert segment " << segments[count]->getSegmentNumberRead()
                                                          << " since it already exists");
                    result = EC_IllegalCall;
                    break;
                }
            }
            else
            {
                DCMSEG_ERROR("Cannot read segment number for segment " << count << ": " << result.text());
                result = EC_IllegalCall;
                break;
            }
        }
    }

    return result;
}

OFCondition DcmSegmentation::readFrames(DcmItem& dataset)
{
    OFCondition result;
    Uint16 allocated, stored, high, spp, pixelRep, rows, cols;
    Uint32 numberOfFrames = 0;
    allocated = stored = high = spp = rows = cols = 0;
    pixelRep                                      = 2; // invalid value for this attribute
    OFString colorModel;

    /* check the typical image pixel attributes and get correct(ed) values */
    result = getAndCheckImagePixelAttributes(
        dataset, allocated, stored, high, spp, pixelRep, rows, cols, numberOfFrames, colorModel);
    if (result.bad())
        return result;

    /* Check length of pixel data element */
    DcmElement* pixelData = NULL;
    if (dataset.findAndGetElement(DCM_PixelData, pixelData).bad())
        return IOD_EC_InvalidPixelData;
    Uint16 bytesPerPixel = (allocated >= 8) ? allocated / 8 : 1;
    if (!checkPixDataLength(pixelData, rows, cols, bytesPerPixel, numberOfFrames))
        return IOD_EC_InvalidPixelData;
    if (bytesPerPixel == 2)
    {
        m_16BitPixelData = OFTrue;
    }

    /* Get pixel data values */
    size_t pixelsPerFrame = OFstatic_cast(size_t, rows) * cols;
    result                = readPixelData(pixelData, numberOfFrames, pixelsPerFrame, allocated);
    return result;
}

OFCondition DcmSegmentation::readPixelData(DcmElement* pixelData, const size_t numFrames, const size_t pixelsPerFrame, const Uint16 bitsAlloc)
{
    Uint8*  pixels8    = NULL;
    Uint16* pixels16   = NULL;
    OFCondition result;
    if (( bitsAlloc == 1) || (bitsAlloc == 8))
    {
        result = pixelData->getUint8Array(pixels8);
    }
    else if (bitsAlloc == 16)
    {
        result = pixelData->getUint16Array(pixels16);
    }
    else
    {
        DCMSEG_ERROR("Cannot read pixel data: Bits Allocated is neither 1, 8 nor 16");
        return IOD_EC_InvalidPixelData;
    }
    if (result.bad())
    {
        DCMSEG_ERROR("Cannot read pixel data");
        return result;
    }
    /* Read all frames into dedicated data structure */
    switch (m_SegmentationType)
    {
        case DcmSegTypes::ST_BINARY:
            result = DcmIODUtil::extractBinaryFrames(pixels8, numFrames, pixelsPerFrame, m_Frames);
            break;
        case DcmSegTypes::ST_FRACTIONAL:
        case DcmSegTypes::ST_LABELMAP:
            for (size_t count = 0; count < numFrames; count++)
            {
                DcmIODTypes::FrameBase* frame = NULL;
                if (bitsAlloc == 8)
                {
                    frame = new DcmIODTypes::Frame<Uint8>(pixelsPerFrame);
                }
                else if (bitsAlloc == 16)
                {
                    frame = new DcmIODTypes::Frame<Uint16>(pixelsPerFrame);
                }
                if (!frame || !frame->getPixelData())
                {
                    delete frame;
                    result = EC_MemoryExhausted;
                    break;
                }
                if (bitsAlloc == 8)
                {
                    memcpy(frame->getPixelData(), pixels8 + count * pixelsPerFrame, pixelsPerFrame);
                }
                else // 16
                {
                    memcpy(frame->getPixelData(), pixels16 + count * pixelsPerFrame, frame->getLengthInBytes());
                }
                // print frame->pixData to cout
                // frame->print();
                m_Frames.push_back(frame);
            }
            break;
        case DcmSegTypes::ST_UNKNOWN:
            result = SG_EC_UnknownSegmentationType;
    }
    return result;
}


OFCondition DcmSegmentation::getAndCheckImagePixelAttributes(DcmItem& dataset,
                                                             Uint16& allocated,
                                                             Uint16& stored,
                                                             Uint16& high,
                                                             Uint16& spp,
                                                             Uint16& pixelRep,
                                                             Uint16& rows,
                                                             Uint16& cols,
                                                             Uint32& numberOfFrames,
                                                             OFString& colorModel)
{
    OFBool fail = OFFalse;
    dataset.findAndGetUint16(DCM_BitsAllocated, allocated);
    dataset.findAndGetUint16(DCM_BitsStored, stored);
    dataset.findAndGetUint16(DCM_HighBit, high);
    dataset.findAndGetUint16(DCM_PixelRepresentation, pixelRep);
    dataset.findAndGetUint16(DCM_SamplesPerPixel, spp);
    dataset.findAndGetOFStringArray(DCM_PhotometricInterpretation, colorModel);

    /* Rows and Columns */
    OFCondition result = getImagePixel().getRows(rows);
    if (result.good())
        result = getImagePixel().getColumns(cols);
    if (result.bad())
    {
        DCMSEG_ERROR("Cannot find Rows or Columns in dataset");
        fail = OFTrue;
    }

    /* Number of Frames */
    Sint32 numFrames = 0;
    result           = m_FG.getNumberOfFrames(numFrames);
    if (result.bad())
    {
        DCMSEG_ERROR("Number of Frames not set");
        fail = OFTrue;
    }
    else
    {
        if (numFrames < 0)
        {
            DCMSEG_ERROR("Number of Frames must be greater than 0");
            fail = OFTrue;
        }
        else
        {
            numberOfFrames = OFstatic_cast(Uint32, numFrames);
        }
    }

    switch (m_SegmentationType)
    {
        case DcmSegTypes::ST_BINARY:
            if (allocated != 1)
            {
                DCMSEG_WARN("Bits Allocated is not set correctly ("
                            << allocated << ", ignored), assuming value 1 as required for binary segmentation objects");
                allocated = 1;
            }
            break;
        case DcmSegTypes::ST_FRACTIONAL:
            if (allocated != 8)
            {
                DCMSEG_WARN("Bits Allocated is not set correctly ("
                            << allocated
                            << ", ignored), assuming value 8 as required for fractional segmentation "
                               "objects");
                allocated = 8;
            }
            break;
        case DcmSegTypes::ST_LABELMAP:
            if ((allocated != 8) && (allocated != 16))
            {
                DCMSEG_ERROR("Bits Allocated is not set correctly (" << allocated << ", giving up");
                fail = OFTrue;
            }
            break;
        case DcmSegTypes::ST_UNKNOWN:
            fail = OFTrue;
            break;
    }
    if (fail)
    {
        return EC_InvalidValue;
    }
    if (stored != allocated)
    {
        DCMSEG_WARN("Bits Stored is not set correctly ("
                    << stored << ", ignored), assuming value " << allocated << " as required for "
                    << DcmSegTypes::segtype2OFString(m_SegmentationType) << " segmentation objects");
        stored = allocated;
    }
    if (m_SegmentationType == DcmSegTypes::ST_BINARY)
    {
        if (high != 0)
        {
            DCMSEG_WARN("High Bit is not set correctly ("
                        << high << ", ignored), assuming value 0 as required for binary segmentation objects");
            high = 0;
        }
    }
    else if (high != allocated - 1)
    {
        DCMSEG_WARN("High Bit is not set correctly ("
                    << high << ", ignored), assuming value " << allocated - 1 << " as required for "
                    << DcmSegTypes::segtype2OFString(m_SegmentationType) << " segmentation objects");
        high = allocated - 1;
    }
    if (spp != 1)
    {
        DCMSEG_WARN("Samples per Pixel is not set correctly ("
                    << spp << ", ignored), assuming value 1 as required for segmentation objects");
        spp = 1;
    }
    if (pixelRep != 0)
    {
        DCMSEG_WARN("Pixel Representation is not set correctly ("
                    << pixelRep << ", ignored), assuming value 0 as required for segmentation objects");
        pixelRep = 0;
    }
    if ((colorModel != "MONOCHROME2")
        && ((colorModel != "PALETTE COLOR") && (m_SegmentationType == DcmSegTypes::ST_LABELMAP)))
    {
        DCMSEG_WARN("Photometric Interpretation is not set correctly (" << colorModel << "): Must be MONOCHROME2 or PALETTE COLOR (only Labelmap segmentations)");
        fail = OFTrue;
    }
    if (rows == 0)
    {
        DCMSEG_ERROR("Rows is not set correctly (0)");
        fail = OFTrue;
    }
    if (cols == 0)
    {
        DCMSEG_ERROR("Columns is not set correctly (0)");
        fail = OFTrue;
    }

    if (fail)
        return EC_InvalidValue;

    return EC_Normal;
}

OFCondition DcmSegmentation::writeDataset(DcmItem& dataset)
{
    Uint8* pixData8 = NULL;
    Uint16* pixData16 = NULL;
    size_t pixDataLength = 0;
    OFCondition result;
    if (has16BitPixelData())
    {
        result = writeWithSeparatePixelData(dataset, pixData16, pixDataLength);
    }
    else
    {
       result = writeWithSeparatePixelData(dataset, pixData8, pixDataLength);
    }
    if (result.good())
    {
        // Check whether pixel data length exceeds maximum number of bytes for uncompressed pixel data,
        // enforced by length field of Pixel Data attribute VR OB/OW if written in explicit VR transfer syntax.
        if (pixDataLength <= 4294967294UL)
        {
            if (has16BitPixelData())
            {
                result = dataset.putAndInsertUint16Array(DCM_PixelData, pixData16, OFstatic_cast(unsigned long, pixDataLength / 2));
            }
            else
            {
                result = dataset.putAndInsertUint8Array(DCM_PixelData, pixData8, OFstatic_cast(unsigned long, pixDataLength));
            }
        }
        else
        {
            result = FG_EC_PixelDataTooLarge;
        }
        delete[] pixData8;
        delete[] pixData16;
    }
    return result;
}

OFCondition DcmSegmentation::writeConcatenation(ConcatenationCreator& cc)
{
    size_t pixDataLength = 0;
    DcmItem* item        = new DcmItem();
    if (!item)
        return EC_MemoryExhausted;
    OFCondition result;
    if (has16BitPixelData())
    {
        Uint16* pixData = NULL;
        result          = writeWithSeparatePixelData(*item, pixData, pixDataLength);
        if (result.good())
        {
            result = cc.setCfgInput(item, pixData, pixDataLength, OFTrue /* transfer ownership */);
        }
    }
    else
    {
        Uint8* pixData = NULL;
        result         = writeWithSeparatePixelData(*item, pixData, pixDataLength);
        if (result.good())
        {
            result = cc.setCfgInput(item, pixData, pixDataLength, OFTrue /* transfer ownership */);
        }
    }
    return result;
}

OFCondition DcmSegmentation::writeMultiFrameFunctionalGroupsModule(DcmItem& dataset)
{
    Uint32 numFrames = DcmIODUtil::limitMaxFrames(
        m_Frames.size(), "More than 2147483647 frames provided, limiting Number of Frames to 2147483647");
    m_FG.setNumberOfFrames(numFrames);
    OFCondition result = m_FG.write(dataset);
    if (result.good())
        m_FGInterface.write(dataset);
    return result;
}

OFCondition DcmSegmentation::writeMultiFrameDimensionModule(DcmItem& dataset)
{
    return m_DimensionModule.write(dataset);
}

template <typename T>
OFCondition DcmSegmentation::writeByteBasedFrames(T* pixData)
{
    Uint8 bytesPerPixel = sizeof(T);
    typename OFVector<DcmIODTypes::FrameBase*>::iterator it = m_Frames.begin();
    // Just copy bytes for each frame as is
    for (size_t count = 0; it != m_Frames.end(); count++)
    {
        memcpy(pixData + count * (*it)->getLengthInBytes() / bytesPerPixel, (*it)->getPixelData(), (*it)->getLengthInBytes());
        it++;
    }
    return EC_Normal;
}

OFCondition DcmSegmentation::writeBinaryFrames(Uint8* pixData, Uint16 rows, Uint16 cols, const size_t pixDataLength)
{
    // Fill Pixel Data Element
    // Compute size of all bit-packed frames in bytes
    return DcmSegUtils::concatBinaryFrames(m_Frames, rows, cols, pixData, pixDataLength);
}

OFCondition DcmSegmentation::writeSegmentationImageModule(DcmItem& dataset)
{
    dataset.putAndInsertOFStringArray(DCM_ImageType, "DERIVED\\PRIMARY");

    OFCondition result = m_ContentIdentificationMacro.write(dataset);
    OFString colorModel;

    /* Write hardcoded values */
    if (result.good())
    {   Uint16 rows, cols, bitsAlloc, bitsStored, highBit, pixelRep, samplesPixel;
        rows = cols = bitsAlloc = bitsStored = highBit = pixelRep = samplesPixel = 0;
        OFString photometricInterpretation;
        samplesPixel = 1;

        colorModel = determineColorModel();
        pixelRep = 0;

        /* Write Bits Allocated/Stored, High Bit, Segmentation Fractional Type,
         * Segmentation Type, Maximum Fractional Value
         */
        switch (m_SegmentationType)
        {
            case DcmSegTypes::ST_BINARY:
            {
                bitsAlloc = 1;
                bitsStored = 1;
                highBit = 0;
                dataset.putAndInsertOFStringArray(DCM_SegmentationType, "BINARY");
                dataset.putAndInsertOFStringArray(DCM_SegmentsOverlap, "UNDEFINED");
                break;
            }
            case DcmSegTypes::ST_FRACTIONAL:
            case DcmSegTypes::ST_LABELMAP:
            {
                Uint8 numBits = 8;
                if (has16BitPixelData())
                {
                    numBits = 16;
                }
                bitsAlloc = bitsStored = numBits;
                highBit = numBits -1;

                if (m_SegmentationType == DcmSegTypes::ST_LABELMAP)
                {
                    dataset.putAndInsertOFStringArray(DCM_SegmentationType, "LABELMAP");
                    dataset.putAndInsertOFStringArray(DCM_SegmentsOverlap, "NO");
                }
                else
                {
                    dataset.putAndInsertOFStringArray(DCM_SegmentationType, "FRACTIONAL");
                    dataset.putAndInsertOFStringArray(DCM_SegmentsOverlap, "UNDEFINED");
                    if (m_SegmentationFractionalType == DcmSegTypes::SFT_OCCUPANCY)
                    {
                        dataset.putAndInsertOFStringArray(DCM_SegmentationFractionalType, "OCCUPANCY");
                    }
                    else
                    {
                        dataset.putAndInsertOFStringArray(DCM_SegmentationFractionalType, "PROBABILITY");
                    }
                    // Maximum Fractional Value: Attribute is type 1C but "required if .. FRACTIONAL", i.e. write type 1
                    DcmIODUtil::copyElementToDataset(
                        result, dataset, m_MaximumFractionalValue, "1", "1", "SegmentationImageModule");
                }
                break;
            }
            case DcmSegTypes::ST_UNKNOWN:
            {
                DCMSEG_ERROR("Internal error, segmentation type not set");
                result = EC_InternalError;
                break;
            }
        }
        getImagePixel().getRows(rows);
        getImagePixel().getColumns(cols);
        if (bitsAlloc == 16)
        {
            result = SetImagePixelModuleVisitor(rows, cols,bitsAlloc, bitsStored, highBit, samplesPixel, colorModel)
            (*OFget<IODImagePixelModule<Uint16> >(&getImagePixel()));
        }
        else
        {
            result = SetImagePixelModuleVisitor(rows, cols,bitsAlloc, bitsStored, highBit, samplesPixel, colorModel)
            (*OFget<IODImagePixelModule<Uint8> >(&getImagePixel()));
        }

    }

    /* Write segments */
    OFVector<DcmItem*> segmentItems;
    if (result.good())
    {
        OFMap<Uint16, DcmSegment*>::iterator it = m_Segments.begin();
        dataset.findAndDeleteElement(DCM_SegmentSequence);
        for (Uint16 itemCount = 0; (it != m_Segments.end()) && result.good(); itemCount++)
        {
            DcmItem* segmentItem = NULL;
            dataset.findOrCreateSequenceItem(DCM_SegmentSequence, segmentItem, itemCount);
            if (segmentItem)
            {
                result = (*it).second->write(*segmentItem);
                /* Insert segment number for the segment, starting from 1 and increasing monotonically. */
                if (result.good())
                {
                    segmentItem->putAndInsertUint16(DCM_SegmentNumber, (*it).first);
                }
            }
            else
            {
                DCMIOD_ERROR("Cannot create/get item in Segment Sequence (internal error)");
                result = EC_InternalError;
            }
            it++;
        }
    }

    return result;
}

// -- private helpers --

void DcmSegmentation::clearData()
{
    DcmSegmentation::IODImage::clearData();
    m_FG.clearData();
    m_FGInterface.clear();
    DcmIODUtil::freeContainer(m_Frames);
    DcmIODUtil::freeMap(m_Segments);
    m_MaximumFractionalValue.clear();
    m_SegmentationFractionalType = DcmSegTypes::SFT_UNKNOWN;
    m_SegmentationType           = DcmSegTypes::ST_UNKNOWN;
}

OFBool DcmSegmentation::checkPixDataLength(DcmElement* pixelData,
                                           const Uint16 rows,
                                           const Uint16 cols,
                                           const Uint16 bytesPerPixel,
                                           const Uint32& numberOfFrames)
{
    // Get actual length of pixel data in bytes
    size_t length = pixelData->getLengthField();

    // Find out how many bytes are needed
    size_t bytesRequired = 0;
    OFCondition result   = getTotalBytesRequired(rows, cols, bytesPerPixel, numberOfFrames, bytesRequired);
    if (result.bad())
        return OFFalse;
    // Length found in Pixel Data element is always even
    if (bytesRequired % 2 == 1)
        bytesRequired++;
    /* Compare expected and actual length */
    if (length < bytesRequired)
    {
        DCMSEG_ERROR("Not enough bytes found in Pixel Data element. Found " << length << " bytes but " << bytesRequired
                                                                            << " bytes expected");
        return OFFalse;
    }
    else if (length > bytesRequired)
    {
        DCMSEG_WARN("Too many bytes found in Pixel Data element. Found " << length << " bytes but " << bytesRequired
                                                                         << " bytes expected");
        return OFTrue;
    }
    else
    {
        DCMSEG_TRACE("Found " << length << " bytes in Pixel Data element as expected");
    }
    return OFTrue;
}

OFCondition DcmSegmentation::getTotalBytesRequired(const Uint16& rows,
                                                   const Uint16& cols,
                                                   const Uint16& bytesPerPixel,
                                                   const Uint32& numberOfFrames,
                                                   size_t& bytesRequired)
{
    // Compute space needed for all frames, first assume 1 byte per pixel (we adapt later for binary segmentations)
    // Rows * Cols = num pixels per frame
    OFBool ok = OFStandard::safeMult(OFstatic_cast(size_t, rows), OFstatic_cast(size_t, cols), bytesRequired);
    // Pixels per frame * num frames = total num pixels
    if (ok)
        OFStandard::safeMult(bytesRequired, OFstatic_cast(size_t, numberOfFrames), bytesRequired);
    // In case of 16 bit pixel data, we need twice as much space
    if (ok && (bytesPerPixel == 2))
        OFStandard::safeMult(bytesRequired, OFstatic_cast(size_t, 2), bytesRequired);
    if (!ok)
    {
        DCMSEG_ERROR("Cannot compute number of bytes required for Pixel Data since size_t type is too small");
        return EC_TooManyBytesRequested;
    }

    // Until this point we assumed bytes per pixel = 1. Now we need to adjust this
    // for binary segmentation objects.with 1 bit per pixel.
    size_t remainder = 0;
    if (m_SegmentationType == DcmSegTypes::ST_BINARY)
    {
        // check whether the 1-bit pixels exactly fit into bytes
        remainder = bytesRequired % 8;
        // number of bytes that work on an exact fit
        bytesRequired = bytesRequired / 8;
        // add one byte if we have a remainder
        if (remainder > 0)
        {
            bytesRequired++;
        }
    }
    return EC_Normal;
}

OFCondition DcmSegmentation::loadFile(DcmFileFormat& dcmff, const OFString& filename, DcmDataset*& dset, const E_TransferSyntax xfer)
{
    dset               = NULL;
    OFCondition result = dcmff.loadFile(filename.c_str(), xfer);
    if (result.bad())
    {
        DCMSEG_ERROR("Could not load file " << filename << ": " << result.text());
        return result;
    }
    dset = dcmff.getDataset();
    if (dset == NULL)
    {
        DCMSEG_ERROR("Could not load file " << filename << ": No dataset");
        return IOD_EC_InvalidObject;
    }
    return result;
}


OFCondition DcmSegmentation::readSegmentationFractionalType(DcmItem& item)
{
    m_SegmentationFractionalType = DcmSegTypes::SFT_UNKNOWN;
    if (!item.tagExists(DCM_SegmentationFractionalType))
    {
        return EC_TagNotFound;
    }
    DcmCodeString element(DCM_SegmentationFractionalType);
    OFCondition result = DcmIODUtil::getAndCheckElementFromDataset(
        item, element, DcmSegmentation::IODImage::getRules()->getByTag(DCM_SegmentationFractionalType));
    OFString str;
    if (result.good())
    {
        element.getOFStringArray(str);
        m_SegmentationFractionalType = DcmSegTypes::OFString2FractionalType(str);
    }

    if (m_SegmentationFractionalType == DcmSegTypes::SFT_UNKNOWN)
    {
        DCMSEG_ERROR("Invalid value for attribute Segmentation Fractional Type: " << str);
        return EC_InvalidValue;
    }
    else
        return EC_Normal;
}

OFCondition DcmSegmentation::readSegmentationType(DcmItem& item)
{
    m_SegmentationType = DcmSegTypes::ST_UNKNOWN;
    if (!item.tagExists(DCM_SegmentationType))
    {
        return EC_TagNotFound;
    }

    DcmCodeString element(DCM_SegmentationType);
    OFCondition result = DcmIODUtil::getAndCheckElementFromDataset(
        item, element, DcmSegmentation::IODImage::getRules()->getByTag(DCM_SegmentationType));
    OFString str;
    if (result.good())
    {
        element.getOFStringArray(str);
        m_SegmentationType = DcmSegTypes::OFString2Segtype(str);
    }

    if (m_SegmentationType == DcmSegTypes::ST_UNKNOWN)
    {
        DCMSEG_ERROR("Invalid value for attribute Segmentation Type: " << str);
        result = EC_InvalidValue;
    }

    return result;
}


OFBool DcmSegmentation::check(const OFBool checkFGStructure)
{
    if (m_Frames.size() == 0)
    {
        DCMSEG_ERROR("No frame data available");
        return OFFalse;
    }
    if (m_Segments.size() == 0)
    {
        DCMSEG_ERROR("No segments defined");
        return OFFalse;
    }
    if (m_Segments.size() > DCM_SEG_MAX_SEGMENTS)
    {
        DCMSEG_ERROR("Too many segments defined");
        return OFFalse;
    }
    // Check that all segments are referenced by at least one frame.
    // This is not required for label maps, since frames are only indirectly (through their pixel values)
    // referencing segments.
    if ( (m_Segments.size() > m_Frames.size()) && (m_SegmentationType != DcmSegTypes::ST_LABELMAP) )
    {
        DCMSEG_ERROR("More segments than frames defined");
        return OFFalse;
    }

    if (checkFGStructure)
    {
        if (!m_FGInterface.check())
            return OFFalse;
    }

    // Check rules around Frame of Reference

    // 1. If Derivation Image FG is not present, Frame of Reference is required.
    OFBool frameOfRefRequired = OFFalse;
    FGBase* group             = m_FGInterface.get(OFstatic_cast(Uint32, 0), DcmFGTypes::EFG_DERIVATIONIMAGE);
    if (group == NULL)
    {
        // Derivation Image FG is not present, FoR is required
        frameOfRefRequired = OFTrue;
    }
    else
    {
        // Derivation Image FG present, Frame of Reference is not required
        frameOfRefRequired = OFFalse;
    }
    OFString frameOfRef;
    DcmSegmentation::IODImage::getFrameOfReference().getFrameOfReferenceUID(frameOfRef);
    if (frameOfRefRequired && frameOfRef.empty())
    {
        DCMSEG_ERROR("Frame of Reference UID is not set for Segmentation but is required");
        return OFFalse;
    }

    // 2. When a Frame of Reference UID is present the segment shall be specified
    // within that coordinate system, using the Pixel Measures, Plane Position
    // (Patient) and Plane Orientation (Patient) Functional Groups.
    if (!frameOfRef.empty())
    {
        // Check that each of above FGs is present. We do not check this for
        // all frames since if it exists for one frame it must exist for all others.
        // This is a general rule and applies for all FGs, so it is not checked here.
        group = m_FGInterface.get(OFstatic_cast(Uint32, 0), DcmFGTypes::EFG_PIXELMEASURES);
        if (!group)
        {
            DCMSEG_ERROR("Frame of Reference UID is present but Pixel Measures FG is missing");
            return OFFalse;
        }
        group = m_FGInterface.get(OFstatic_cast(Uint32, 0), DcmFGTypes::EFG_PLANEPOSPATIENT);
        if (!group)
        {
            DCMSEG_ERROR("Frame of Reference UID is present but Plane Position (Patient) FG is missing");
            return OFFalse;
        }
        group = m_FGInterface.get(OFstatic_cast(Uint32, 0), DcmFGTypes::EFG_PLANEORIENTPATIENT);
        if (!group)
        {
            DCMSEG_ERROR("Frame of Reference UID is present but Plane Orientation (Patient) FG is missing");
            return OFFalse;
        }
    }
    // Another condition cannot be checked since we do not have access to the
    // datasets of the source images:
    // 3. If FoR is present but not the same in images this segmentation applies to,
    // (those in Derivation Image FG), each pixel of the segmentation shall
    // correspond to a pixel in a referenced image (i.e. they must share the same
    // size and resolution).

    return OFTrue;
}

OFCondition DcmSegmentation::decompress(DcmDataset& dset)
{
    DcmXfer xfer = dset.getOriginalXfer();
    OFCondition result;
    // If the original transfer syntax could have been lossy, print warning
    if (dset.hasRepresentation(EXS_LittleEndianExplicit, NULL))
    {
        if (xfer.isPixelDataCompressed() && (xfer != EXS_RLELossless))
        {
            DCMSEG_WARN("Dataset has been compressed using a (possibly) lossy compression scheme (ignored)");
        }
    }
    // If the original transfer syntax refers to compressed pixel data and we do not
    // already have an uncompressed version, decompress or reject the file
    else if (xfer.isPixelDataCompressed())
    {
        // RLE compression is fine (truly lossless)
        if (xfer == EXS_RLELossless)
        {
            DCMSEG_DEBUG("DICOM file is RLE-compressed, converting to uncompressed transfer syntax first");
            result = DcmIODUtil::decompress(dset);
        }
        else // We do not accept any transfer syntax that could be lossy compressed
        {
            DCMSEG_ERROR("Transfer syntax " << DcmXfer(xfer).getXferName()
                                            << " uses lossy compression, not supported for Segmentation objects!");
            result = IOD_EC_CannotDecompress;
        }
    }
    return result;
}


OFString DcmSegmentation::determineColorModel()
{
    OFString colorModel;
    if (m_SegmentationType == DcmSegTypes::ST_LABELMAP)
    {
        colorModel = DcmSegTypes::labelmapColorModel2OFString(m_LabelmapColorModel, "MONOCHROME2");
    }
    else
    {
        colorModel = "MONOCHROME2";
    }
    return colorModel;
}


OFBool DcmSegmentation::checkColorModel(const OFString& photometricInterpretation)
{
    // For labelmaps, MONOCHROME2 and PALETTE is permitted
    if (m_SegmentationType == DcmSegTypes::ST_LABELMAP)
    {
        if ((photometricInterpretation != "MONOCHROME2") && (photometricInterpretation != "PALETTE COLOR"))
        {
            DCMSEG_WARN("Photometric Interpretation is not set correctly (" << photometricInterpretation << "): Must be MONOCHROME2 or PALETTE COLOR for labelmaps");
            m_LabelmapColorModel = DcmSegTypes::SLCM_UNKNOWN;
            return OFFalse;
        }
        m_LabelmapColorModel = DcmSegTypes::OFString2LabelmapColorModel(photometricInterpretation);
    }
    else
    {
        if (photometricInterpretation != "MONOCHROME2")
        {
            DCMSEG_WARN("Photometric Interpretation is not set correctly (" << photometricInterpretation << "): Must be MONOCHROME2 for binary and fractional segmentations");
            return OFFalse;
        }
        m_LabelmapColorModel = DcmSegTypes::SLCM_UNKNOWN; // not used for binary/fractional segmentations
    }
    // If we found PALETTE COLOR, check whether the Palette Color Lookup Table is present
    if (m_LabelmapColorModel == DcmSegTypes::SLCM_PALETTE)
    {
        if (getPaletteColorLUT().numBits() == 0) // checks at least basic descriptor availability
        {
            DCMSEG_WARN("Photometric Interpretation is set to PALETTE COLOR but no or invalid Palette Color Lookup Table is present");
            return OFFalse;
        }
    }
    return OFTrue;
}


void DcmSegmentation::setSOPClassUIDBasedOnSegmentationType()
{
   switch (m_SegmentationType)
    {
        case DcmSegTypes::ST_LABELMAP:
            DcmSegmentation::IODImage::getSOPCommon().setSOPClassUID(UID_LabelMapSegmentationStorage);
            break;
        case DcmSegTypes::ST_FRACTIONAL:
        case DcmSegTypes::ST_BINARY:
            DcmSegmentation::IODImage::getSOPCommon().setSOPClassUID(UID_SegmentationStorage);
            break;
        case DcmSegTypes::ST_UNKNOWN:
            // Print warning if segmentation type is unknown
            DCMSEG_WARN("Segmentation type is unknown, setting SOP Class UID to Segmentation Storage SOP Class");
            DcmSegmentation::IODImage::getSOPCommon().setSOPClassUID(UID_SegmentationStorage);
            break;
    }
}

// Explicit instantiations
template OFCondition DcmSegmentation::addFrame<Uint8>(Uint8*, const Uint16, const OFVector<FGBase*>&);
template OFCondition DcmSegmentation::addFrame<Uint16>(Uint16*, const Uint16, const OFVector<FGBase*>&);
template OFCondition DcmSegmentation::addFrame<Uint8>(Uint8* pixData);
template OFCondition DcmSegmentation::addFrame<Uint16>(Uint16* pixData);
