/*
 *
 *  Copyright (C) 2019-2024, Open Connections GmbH
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
 *
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class representing a Enhanced CT object
 *
 */
 #include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmect/enhanced_ct.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/dcmect/types.h"
#include "dcmtk/dcmfg/concatenationcreator.h"
#include "dcmtk/dcmfg/concatenationloader.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmiod/modimagepixel.h"

struct EctEnhancedCT::SetImagePixelModuleVisitor
{
    SetImagePixelModuleVisitor(const Uint16 r, const Uint16 c)
        : rows(r)
        , cols(c)
    {
    }

    template <typename T>
    OFCondition operator()(T& t)
    {
        if ((rows == 0) || (cols == 0))
        {
            DCMECT_ERROR("Rows/Cols must be non-zero but are : " << rows << "/" << cols);
            return ECT_InvalidDimensions;
        }

        t.setRows(rows);
        t.setColumns(cols);
        t.setBitsAllocated(16);
        t.setBitsStored(16);
        t.setHighBit(15);
        t.setSamplesPerPixel(1);
        t.setPhotometricInterpretation("MONOCHROME2");
        t.setPixelRepresentation(OFis_signed<T>::value ? 1 : 0);

        return EC_Normal;
    }

    // Members
    const Uint16 rows;
    const Uint16 cols;
};

// ----------------------------------------------------------------------------
// Class WriteVisitor:
// Class for adding DICOM pixel data bulk element based on type pixel data type
// ----------------------------------------------------------------------------

// Generic implementation, template specialization is done in inner
// DcmElementOf class
struct EctEnhancedCT::WriteVisitor
{
    // Inner class that implements the specializations for different
    // pixel data types
    // template <typename T> class DcmElementOf;

    // Constructor, sets parameters the visitor works on in operator()
    WriteVisitor(DcmItem& i, EctEnhancedCT& m)
        : m_Item(i)
        , m_CT(m)
    {
        // Nothing else to do
    }

    //
    OFCondition operator()(OFmonostate)
    {
        DCMECT_ERROR("Could not write pixel data: Invalid pixel data type, only 16 bit integer is handled");
        return ECT_NoPixelData;
    }

    template <typename ImagePixel>
    OFCondition operator()(ImagePixel& pixel)
    {
        // Avoid compiler warning about unused parameter "pixel"
        (void)pixel;
        // Input data is checked before
        Uint16 rows = 0;
        Uint16 cols = 0;
        m_CT.getRows(rows);
        m_CT.getColumns(cols);
        const size_t numFrames      = m_CT.m_Frames.size();
        const size_t numBytesFrame  = m_CT.m_Frames[0]->length;
        const size_t numPixelsFrame = rows * cols;
        // Creates the correct pixel data element, based on the image pixel module used.
        DcmPixelData* pixData = new DcmPixelData(DCM_PixelData);
        OFCondition result;
        if (pixData)
        {
            pixData->setVR(EVR_OW);
            Uint16* ptr          = NULL;
            size_t numBytesTotal = numBytesFrame * numFrames / 2;
            if (numBytesTotal <= 4294967294UL)
            {
                result = pixData->createUint16Array(OFstatic_cast(Uint32, numBytesTotal), ptr);
                // copy all frames into CT's frame structure
                if (ptr)
                {
                    for (size_t f = 0; f < numFrames; ++f)
                    {
                        memcpy(ptr, m_CT.m_Frames[f]->pixData, numBytesFrame);
                        ptr += numPixelsFrame;
                    }
                    return m_Item.insert(pixData);
                }
            }
            else
            {
                result = FG_EC_PixelDataTooLarge;
                delete pixData;
            }
        }
        return result;
    }

    // Members, i.e. parameters to operator()
    DcmItem& m_Item;
    EctEnhancedCT& m_CT;
};

// Generic implementation, template specialization is done in inner
// DcmElementOf class
struct EctEnhancedCT::WriteVisitorConcatenation
{
    // Inner class that implements the writing to Concatentions via ConcatenationCreator class

    // Constructor, sets parameters the visitor works on in operator()
    WriteVisitorConcatenation(EctEnhancedCT& m, Uint8*& pixData, size_t& pixDataLength)
        : m_CT(m)
        , m_pixData(pixData)
        , m_pixDataLength(pixDataLength)
    {
        // Nothing else to do
    }

    //
    OFCondition operator()(OFmonostate)
    {
        DCMECT_ERROR("Could not write pixel data: Invalid pixel data type, only 16 bit integer is handled");
        return ECT_NoPixelData;
    }

    template <typename ImagePixel>
    OFCondition operator()(ImagePixel& pixel)
    {
        // Avoid compiler warning about unused parameter "pixel"
        (void)pixel;
        // Input data is checked before
        Uint16 rows = 0;
        Uint16 cols = 0;
        m_CT.getRows(rows);
        m_CT.getColumns(cols);
        const size_t numFrames     = m_CT.m_Frames.size();
        const size_t numBytesFrame = m_CT.m_Frames[0]->length;
        // Creates the correct pixel data element, based on the image pixel module used.
        m_pixDataLength = numBytesFrame * numFrames;
        m_pixData       = new Uint8[m_pixDataLength];
        if (m_pixData)
        {
            Uint8* ptr = m_pixData;
            // copy all frames into CT's frame structure
            if (ptr)
            {
                for (size_t f = 0; f < numFrames; ++f)
                {
                    memcpy(ptr, m_CT.m_Frames[f]->pixData, numBytesFrame);
                    ptr += numBytesFrame;
                }
                return EC_Normal;
            }
        }
        m_pixData       = OFnullptr;
        m_pixDataLength = 0;
        return EC_MemoryExhausted;
    }

    // Members, i.e. parameters to operator()
    EctEnhancedCT& m_CT;
    Uint8*& m_pixData;
    size_t& m_pixDataLength;
};

struct EctEnhancedCT::ReadVisitor
{
    // Inner class that implements the specializations for different
    // pixel data types
    template <typename T>
    class DcmElementOf;

    ReadVisitor(DcmItem& srcItem, EctEnhancedCT& m, const OFBool readPixelData = OFTrue)
        : m_Item(srcItem)
        , m_CT(m)
        , m_readPixelData(readPixelData)
    {
        // Nothing to do
    }

    OFCondition operator()(OFmonostate)
    {
        return ECT_InvalidPixelData;
    }

    template <typename ImagePixel>
    OFCondition operator()(ImagePixel& pixel)
    {
        m_CT.readGeneric(m_Item);
        OFCondition result;
        Uint16 rows, cols;
        Uint32 numFrames;
        size_t numBytesFrame = 0;
        rows = cols = 0;
        m_CT.getRows(rows);
        m_CT.getColumns(cols);
        numFrames = DcmIODUtil::limitMaxFrames(
            m_CT.getFunctionalGroups().getNumberOfFrames(),
            "Functional groups implicate more than 2147483647 frames, only 2147483647 will be used");
        if (!rows || !cols || !numFrames)
        {
            DCMECT_ERROR("Rows (" << rows << "), Columns (" << cols << ") and Number of Frames (" << numFrames
                                  << ") must not be 0");
            return ECT_InvalidPixelInfo;
        }
        numBytesFrame = OFstatic_cast(size_t, rows) * cols * sizeof(typename ImagePixel::value_type);
        if (m_readPixelData)
        {
            result = readSpecific(pixel, numFrames, numBytesFrame);
        }
        return result;
    }

    template <typename T>
    OFCondition readSpecific(IODImagePixelModule<T>& p, const Uint32 numFrames, const size_t numBytesFrame)
    {
        // Avoid compiler warning about unused parameter
        (void)p;
        unsigned long numTotalWords = 0;
        const Uint16* pixData       = NULL;
        if (m_Item.findAndGetUint16Array(DCM_PixelData, pixData, &numTotalWords).good())
        {
            if (numTotalWords == numBytesFrame * numFrames / 2 /* we compare to num words not num bytes */)
            {
                for (Uint32 n = 0; n < numFrames; n++)
                {
                    DcmIODTypes::Frame* f = new DcmIODTypes::Frame;
                    if (f)
                    {
                        f->length  = numBytesFrame;
                        f->pixData = new Uint8[f->length];
                        memcpy(f->pixData, pixData + n * numBytesFrame / 2, numBytesFrame);
                        m_CT.m_Frames.push_back(f);
                    }
                    else
                    {
                        return EC_MemoryExhausted;
                    }
                }
            }
            else
            {
                DCMECT_ERROR("Invalid number of pixels: Expected " << numBytesFrame * numFrames / 2
                                                                   << " pixels but Pixel Data has " << numTotalWords
                                                                   << " pixels");
                return ECT_InvalidPixelInfo;
            }
        }
        else
        {
            DCMECT_ERROR("No Pixel Data element found");
            return ECT_NoPixelData;
        }
        return EC_Normal;
    }

    // Members, i.e. parameters to operator()
    DcmItem& m_Item;
    EctEnhancedCT& m_CT;
    OFBool m_readPixelData;
};

template <typename PixelType>
EctEnhancedCT::Frames<PixelType>::Frames(EctEnhancedCT& ct)
    : m_CT(ct)
{
}

template <typename PixelType>
OFCondition EctEnhancedCT::Frames<PixelType>::addFrame(PixelType* data,
                                                       const size_t numPixels,
                                                       const OFVector<FGBase*>& perFrameInformation)
{
    OFCondition result;
    if (data && numPixels)
    {
        if (!perFrameInformation.empty())
        {
            OFunique_ptr<DcmIODTypes::Frame> f(new DcmIODTypes::Frame);
            if (f)
            {
                f->length  = numPixels * sizeof(PixelType);
                f->pixData = new Uint8[f->length];
                memcpy(f->pixData, data, f->length);
                m_CT.m_Frames.push_back(f.release());
                OFVector<FGBase*>::const_iterator fg = perFrameInformation.begin();
                while (result.good() && (fg != perFrameInformation.end()))
                {
                    result = m_CT.m_FGInterface.addPerFrame(OFstatic_cast(Uint32, m_CT.m_Frames.size()), **fg);
                    if (result.bad())
                    {
                        DCMECT_ERROR(
                            "Could not add functional group: " << DcmFGTypes::FGType2OFString((*fg)->getType()));
                        break;
                    }
                    ++fg;
                }
            }
        }
    }
    // Clean up if necessary
    if (result.bad())
        m_CT.m_FGInterface.deleteFrame(OFstatic_cast(Uint32, m_CT.m_Frames.size()));
    return result;
}

template <typename PixelType>
PixelType* EctEnhancedCT::Frames<PixelType>::getFrame(const size_t frameNumber)
{
    if (frameNumber < m_CT.m_Frames.size())
    {
        return (PixelType*)(m_CT.m_Frames[frameNumber]->pixData);
    }
    return NULL;
}

// Helper "class" that returns Frames offering API to the pixel's frame bulk
// data by offering the dedicated data type, e.g. Float32 instead of the
// internally stored generic Uint8 array.
//
struct EctEnhancedCT::GetFramesVisitor
{
    GetFramesVisitor(EctEnhancedCT& ct)
        : m_CT(ct)
    {
    }

    OFCondition operator()(OFmonostate)
    {
        return ECT_InvalidPixelData;
    }

    template <typename ImagePixel>
    Frames<OFTypename ImagePixel::value_type> operator()(ImagePixel&)
    {
        return Frames<OFTypename ImagePixel::value_type>(m_CT);
    }

    EctEnhancedCT& m_CT;
};

// default constructor (protected, instance creation via create() function)
template <typename ImagePixel>
EctEnhancedCT::EctEnhancedCT(OFin_place_type_t(ImagePixel))
    : IODImage(OFin_place<ImagePixel>)
    , m_SynchronisationModule()
    , m_SynchronisationModuleEnabled(OFFalse)
    , m_EnhancedGeneralEquipmentModule()
    , m_FG()
    , m_DimensionModule()
    , m_AcquisitionContextModule()
    , m_CommonInstanceReferenceModule()
    , m_Frames()
    , m_FGInterface()
    , m_ImageType(DCM_ImageType)
    , m_InstanceNumber(DCM_InstanceNumber)
    , m_MultiEnergyCTAcquisition(DCM_MultienergyCTAcquisition)
    , m_PixelPresentation(DCM_PixelPresentation)
    , m_VolumetricProperties(DCM_VolumetricProperties)
    , m_VolumeBasedCalculationTechnique(DCM_VolumeBasedCalculationTechnique)
    , m_AcquisitionNumber(DCM_AcquisitionNumber)
    , m_AcquisitionDateTime(DCM_AcquisitionDateTime)
    , m_AcquisitionDuration(DCM_AcquisitionDuration)
    , m_ContentQualification(DCM_ContentQualification)
    , m_ImageComments(DCM_ImageComments)
    , m_BurnedInAnnotation(DCM_BurnedInAnnotation)
    , m_RecognizableVisualFeatures(DCM_RecognizableVisualFeatures)
    , m_LossyImageCompression(DCM_LossyImageCompression)
    , m_LossyImageCompressionRatio(DCM_LossyImageCompressionRatio)
    , m_LossyImageCompressionMethod(DCM_LossyImageCompressionMethod)
    , m_PresentationLUTShape(DCM_PresentationLUTShape)
    , m_IsoCenterPosition(DCM_IsocenterPosition)
{
    setGeneralImageModuleEnabled(OFFalse);
}

// ------------------ Constructors, Loading, Saving -----------------------

EctEnhancedCT::~EctEnhancedCT()
{
    DcmIODUtil::freeContainer(m_Frames);
}

OFCondition EctEnhancedCT::loadFile(const OFString& filename, EctEnhancedCT*& ct)
{
    DcmFileFormat dcmff;
    OFCondition cond = dcmff.loadFile(filename.c_str());
    if (cond.good())
    {
        cond = loadDataset(*(dcmff.getDataset()), ct);
    }
    return cond;
}

OFCondition EctEnhancedCT::loadDataset(DcmDataset& dataset, EctEnhancedCT*& ct)
{
    OFCondition result;
    OFString sopClass;
    if (dataset.findAndGetOFStringArray(DCM_SOPClassUID, sopClass).good())
    {
        if (sopClass == UID_EnhancedCTImageStorage)
        {
            result = decompress(dataset);
            if (result.good())
            {

                DcmElement* elem = NULL;
                if (dataset.findAndGetElement(DCM_PixelData, elem).good())
                {
                    Uint16 pr = 0;
                    if (dataset.findAndGetUint16(DCM_PixelRepresentation, pr).good())
                    {
                        if (pr == 0)
                        {
                            ct = new EctEnhancedCT(OFin_place<IODImagePixelModule<Uint16> >);
                        }
                        else
                        {
                            ct = new EctEnhancedCT(OFin_place<IODImagePixelModule<Sint16> >);
                        }
                    }
                    else
                    {
                        DCMECT_WARN("Pixel Data element found but no Pixel Presentation set, assuming 16 bit unsigned "
                                    "integer data");
                        ct = new EctEnhancedCT(OFin_place<IODImagePixelModule<Uint16> >);
                    }
                }
                else
                {
                    return ECT_NoPixelData;
                }
                if (ct == NULL)
                {
                    return EC_MemoryExhausted;
                }
            }
            else
            {
                return result;
            }
        }
        else
        {
            DCMECT_ERROR("Invalid SOP Class: "
                         << sopClass << ", only Enhanced CT Image Storage (1.2.840.10008.5.1.4.1.1.2.1) supported");
            return ECT_InvalidSOPClass;
        }
    }
    result = OFvisit<OFCondition>(ReadVisitor(dataset, *ct), ct->getImagePixel());
    if (result.bad())
    {
        delete ct;
        ct = NULL;
    }
    return result;
}

OFCondition
EctEnhancedCT::loadConcatenation(ConcatenationLoader& cl, const OFString& concatenationUID, EctEnhancedCT*& ct)
{

    DcmDataset dset;
    ct = NULL;
    OFVector<DcmIODTypes::Frame*> frames;
    OFCondition result = cl.load(concatenationUID, &dset, frames);
    if (result.good())
    {
        Uint16 pr = 0;
        if (dset.findAndGetUint16(DCM_PixelRepresentation, pr).good())
        {
            if (pr == 0)
            {
                ct = new EctEnhancedCT(OFin_place<IODImagePixelModule<Uint16> >);
            }
            else
            {
                ct = new EctEnhancedCT(OFin_place<IODImagePixelModule<Sint16> >);
            }
        }
        else
        {
            DCMECT_WARN("No Pixel Presentation set, assuming 16 bit unsigned "
                        "integer data");
            ct = new EctEnhancedCT(OFin_place<IODImagePixelModule<Uint16> >);
        }
        result
            = OFvisit<OFCondition>(ReadVisitor(dset, *ct, OFFalse /* do not read pixel data */), ct->getImagePixel());
        if (result.good())
        {
            ct->m_Frames = frames;
        }
        if (result.bad())
        {
            delete ct;
            ct = NULL;
        }
    }
    return result;
}

OFCondition EctEnhancedCT::saveFile(const OFString& filename, const E_TransferSyntax writeXfer)
{
    DcmFileFormat dcmff;
    OFCondition result;
    if ((result = write(*dcmff.getDataset())).good())
        result = dcmff.saveFile(filename.c_str(), writeXfer);
    if (result.bad())
        DCMECT_ERROR("Cannot save Enhanced CT object to file " << filename << ": " << result.text());
    return result;
}

OFCondition EctEnhancedCT::writeDataset(DcmItem& dataset)
{
    return write(dataset);
}

void EctEnhancedCT::setCheckFGOnWrite(const OFBool doCheck)
{
    m_FGInterface.setCheckOnWrite(doCheck);
}

OFBool EctEnhancedCT::getCheckFGOnWrite()
{
    return m_FGInterface.getCheckOnWrite();
}

// ------------------ Creation -----------------------

OFCondition EctEnhancedCT::create(EctEnhancedCT*& ct,
                                  const Uint16 rows,
                                  const Uint16 columns,
                                  const OFBool signedPixelData,
                                  const EctTypes::E_ImageType1 imageType1,
                                  const OFString& imageType3,
                                  const OFString& imageType4,
                                  const OFString& instanceNumber,
                                  const EctTypes::E_ContentQualification contentQualification,
                                  const EctTypes::E_PixelPresentation pixelPresentation,
                                  const EctTypes::E_VolumetricProperties volumetricProperties,
                                  const OFString& volumeBasedCalculationTechnique,
                                  const IODEnhGeneralEquipmentModule::EquipmentInfo& equipmentInfo,
                                  const OFString& acquisitionDateTime,
                                  const Float64& acquisitionDuration)
{
    ct = OFnullptr;
    OFCondition status;
    if (!signedPixelData)
    {
        ct = new EctEnhancedCT(OFin_place<IODImagePixelModule<Uint16> >);
        if (ct)
            status
                = SetImagePixelModuleVisitor(rows, columns)(*OFget<IODImagePixelModule<Uint16> >(&ct->getImagePixel()));
        else
            status = EC_MemoryExhausted;
    }
    else
    {
        ct = new EctEnhancedCT(OFin_place<IODImagePixelModule<Sint16> >);
        if (ct)
            status
                = SetImagePixelModuleVisitor(rows, columns)(*OFget<IODImagePixelModule<Sint16> >(&ct->getImagePixel()));
        else
            status = EC_MemoryExhausted;
    }
    if (status.bad())
        return status;

    if (status.good())
        status = ct->getSOPCommon().setSOPClassUID(UID_EnhancedCTImageStorage);

    if (status.good())
        status = ct->setImageType(EctTypes::imageType1ToStr(imageType1) + "\\"
                                  + EctTypes::imageType2ToStr(EctTypes::E_ImageType2_Primary) + "\\" + imageType3 + "\\"
                                  + imageType4);

    if (status.good())
        status = ct->getIODMultiFrameFGModule().setInstanceNumber(instanceNumber);
    if (status.good())
        status = ct->setContentQualification(contentQualification);
    if (status.good())
        status = ct->setPixelPresentation(pixelPresentation);
    if (status.good())
        status = ct->setVolumetricProperties(volumetricProperties);
    if (status.good())
        status = ct->setVolumeBasedCalculationTechnique(volumeBasedCalculationTechnique);
    if (status.good())
        status = ct->setLossyImageCompression(OFFalse);
    if (status.good())
        status = ct->getIODEnhGeneralEquipmentModule().set(equipmentInfo);
    if (status.good() && (!acquisitionDateTime.empty()))
        status = ct->setAcquisitionDateTime(acquisitionDateTime);
    if (status.good() && (acquisitionDuration >= 0))
        status = ct->setAcquisitionDuration(acquisitionDuration);
    if (status.good())
        status = ct->m_BurnedInAnnotation.putOFStringArray("NO");

    if (status.good())
        status = DcmIODUtil::setContentDateAndTimeNow(ct->getIODMultiFrameFGModule());

    return status;
}

// ------------------ Access -----------------------

OFBool EctEnhancedCT::check(const OFBool checkFGStructure)
{
    if (m_Frames.size() == 0)
    {
        DCMECT_ERROR("No frame data available");
        return OFFalse;
    }

    if (checkFGStructure)
    {
        if (!m_FGInterface.check())
            return OFFalse;
    }

    return OFTrue;
}

FGInterface& EctEnhancedCT::getFunctionalGroups()
{
    return m_FGInterface;
}

IODMultiFrameFGModule::ConcatenationInfo& EctEnhancedCT::getConcatenationInfo()
{
    return m_FG.getConcatenationInfo();
}

size_t EctEnhancedCT::getNumberOfFrames() const
{
    return OFconst_cast(EctEnhancedCT*, this)->m_FGInterface.getNumberOfFrames();
}

// ------------------ Access -----------------------

OFCondition EctEnhancedCT::addForAllFrames(const FGBase& group)
{
    return m_FGInterface.addShared(group);
}

IODMultiframeDimensionModule& EctEnhancedCT::getDimensions()
{
    return m_DimensionModule;
}

EctEnhancedCT::FramesType EctEnhancedCT::getFrames()
{
    return OFvisit<FramesType>(GetFramesVisitor(*this), getImagePixel());
}

OFCondition EctEnhancedCT::importFromSourceImage(DcmItem& dataset, const OFBool takeOverCharset)
{
    OFString FoR;
    dataset.findAndGetOFString(DCM_FrameOfReferenceUID, FoR);
    return IODImage::importHierarchy(dataset,
                                     OFTrue,       // Patient
                                     OFTrue,       // Study
                                     !FoR.empty(), // Frame of Reference
                                     OFFalse,      // Series
                                     takeOverCharset);
}

OFCondition EctEnhancedCT::importFromSourceImage(const OFString& filename, const OFBool takeOverCharset)
{
    DcmFileFormat dcmff;
    OFCondition result = dcmff.loadFile(filename);
    if (result.good())
    {
        return importFromSourceImage(*(dcmff.getDataset()), takeOverCharset);
    }
    return result;
}

// ------------------- Module getters / setters -------------------------

IODPatientModule& EctEnhancedCT::getIODPatientModule()
{
    return IODImage::getPatient();
}

IODPatientStudyModule& EctEnhancedCT::getIODPatientStudyModule()
{
    return IODImage::getPatientStudy();
}

IODGeneralStudyModule& EctEnhancedCT::getIODGeneralStudyModule()
{
    return IODImage::getStudy();
}

IODGeneralSeriesModule& EctEnhancedCT::getIODGeneralSeriesModule()
{
    return IODImage::getSeries();
}

IODFoRModule& EctEnhancedCT::getIODFrameOfReferenceModule()
{
    return IODImage::getFrameOfReference();
}

IODSynchronizationModule& EctEnhancedCT::getIODSynchronizationModule()
{
    return m_SynchronisationModule;
}

void EctEnhancedCT::setIODSynchronisationModuleEnabled(const OFBool enabled)
{
    m_SynchronisationModuleEnabled = enabled;
}

OFBool EctEnhancedCT::getIODSynchronisationModuleEnabled()
{
    return m_SynchronisationModuleEnabled;
}

IODGeneralEquipmentModule& EctEnhancedCT::getIODGeneralEquipmentModule()
{
    return IODImage::getEquipment();
}

IODEnhGeneralEquipmentModule& EctEnhancedCT::getIODEnhGeneralEquipmentModule()
{
    return m_EnhancedGeneralEquipmentModule;
}

IODMultiFrameFGModule& EctEnhancedCT::getIODMultiFrameFGModule()
{
    return m_FG;
}

IODMultiframeDimensionModule& EctEnhancedCT::getIODMultiframeDimensionModule()
{
    return m_DimensionModule;
}

IODAcquisitionContextModule& EctEnhancedCT::getIODAcquisitionContextModule()
{
    return m_AcquisitionContextModule;
}

IODSOPCommonModule& EctEnhancedCT::getIODSOPCommonModule()
{
    return IODImage::getSOPCommon();
}

IODCommonInstanceReferenceModule& EctEnhancedCT::getIODCommonInstanceReferenceModule()
{
    return m_CommonInstanceReferenceModule;
}

// --------- Getters from Image Pixel, Enhanced CT Series, as far as applicable ---------------

OFCondition EctEnhancedCT::getImageType(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_ImageType, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getAcquisitionNumber(Sint32& value, const unsigned long pos)
{
    return getData()->findAndGetSint32(DCM_AcquisitionNumber, value, pos);
}

OFCondition EctEnhancedCT::getAcquisitionNumber(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_AcquisitionNumber, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getAcquisitionDateTime(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_AcquisitionDateTime, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getAcquisitionDuration(Float64& value, const unsigned long pos)
{
    return getData()->findAndGetFloat64(DCM_AcquisitionDuration, value, pos);
}

OFCondition EctEnhancedCT::getAcquisitionDuration(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_AcquisitionDuration, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getContentQualification(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_ContentQualification, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getImageComments(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_ImageComments, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getBurnedInAnnotation(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_BurnedInAnnotation, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getRecognizableVisualFeatures(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_RecognizableVisualFeatures, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getLossyImageCompression(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_LossyImageCompression, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getLossyImageCompressionRatio(Float64& value, const unsigned long pos)
{
    return getData()->findAndGetFloat64(DCM_LossyImageCompressionRatio, value, pos);
}

OFCondition EctEnhancedCT::getLossyImageCompressionRatio(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_LossyImageCompressionRatio, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getLossyImageCompressionMethod(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_LossyImageCompressionMethod, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getPresentationLUTShape(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_PresentationLUTShape, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getMultiEnergyCTAcquisition(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_MultienergyCTAcquisition, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getPixelPresentation(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_PixelPresentation, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getVolumetricProperties(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_VolumetricProperties, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getVolumeBasedCalculationTechnique(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_VolumeBasedCalculationTechnique, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getIsocenterPosition(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_IsocenterPosition, *getData(), value, pos);
}

OFCondition EctEnhancedCT::getIsocenterPosition(OFVector<Float64>& values)
{
    return DcmIODUtil::getFloat64ValuesFromItem(DCM_IsocenterPosition, *getData(), values);
}

OFCondition EctEnhancedCT::getColumns(Uint16& cols)
{
    return getImagePixel().getColumns(cols);
}

OFCondition EctEnhancedCT::getRows(Uint16& rows)
{
    return getImagePixel().getRows(rows);
}

// --------- Setters from Image Pixel, Enhanced CT Series, as far as applicable ---------------

OFCondition EctEnhancedCT::setImageType(const OFString& value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "4") : EC_Normal;
    if (result.good())
        result = m_ImageType.putOFStringArray(value);
    return result;
}

OFCondition EctEnhancedCT::setAcquisitionNumber(const OFString& value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_AcquisitionNumber.putOFStringArray(value);
    return result;
}

OFCondition EctEnhancedCT::setAcquisitionDateTime(const OFString& value, const OFBool check)
{
    OFCondition result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_AcquisitionDateTime.putOFStringArray(value);
    return result;
}

OFCondition EctEnhancedCT::setAcquisitionDuration(const Float64& value, const bool check)
{
    (void)check;
    return m_AcquisitionDuration.putFloat64(value);
}

OFCondition EctEnhancedCT::setContentQualification(const EctTypes::E_ContentQualification value, const OFBool check)
{
    (void)check;
    if ((value == EctTypes::E_ContentQuali_Invalid) || (value == EctTypes::E_ContentQuali_Empty))
        return ECT_InvalidAttributeValue;

    return m_ContentQualification.putOFStringArray(EctTypes::contentQualiToStr(value));
}

OFCondition EctEnhancedCT::setImageComments(const OFString& value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_ImageComments.putOFStringArray(value);
    return result;
}

OFCondition EctEnhancedCT::setRecognizableVisualFeatures(const EctTypes::E_RecognizableVisualFeatures value,
                                                         const OFBool check)
{
    (void)check;
    if ((value == EctTypes::E_RecoVisFeatures_Invalid) || (value == EctTypes::E_RecoVisFeatures_Empty))
        return ECT_InvalidAttributeValue;

    return m_RecognizableVisualFeatures.putOFStringArray(EctTypes::recoVisFeaturesToStr(value));
}

OFCondition EctEnhancedCT::setLossyImageCompression(const OFBool isLossy,
                                                    const OFString& ratios,
                                                    const OFString& methods,
                                                    const OFBool check)
{
    if (!isLossy)
    {
        return m_LossyImageCompression.putOFStringArray("00");
    }

    OFCondition result = m_LossyImageCompression.putOFStringArray("01");
    if (result.good())
        result = (check) ? DcmDecimalString::checkStringValue(ratios, "1-n") : EC_Normal;
    if (result.good())
        result = m_LossyImageCompression.putOFStringArray(ratios);

    if (result.good())
        result = (check) ? DcmCodeString::checkStringValue(methods, "1-n") : EC_Normal;
    if (result.good())
        result = m_LossyImageCompressionMethod.putOFStringArray(methods);

    return result;
}

OFCondition EctEnhancedCT::setISOCenterPosition(const OFVector<Float64>& values, const OFBool check)
{
    return DcmIODUtil::setFloat64ValuesOnElement(m_IsoCenterPosition, values, "3", check);
}

OFCondition EctEnhancedCT::setISOCenterPosition(const OFString& value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "3") : EC_Normal;
    if (result.good())
        result = m_IsoCenterPosition.putOFStringArray(value);
    return result;
}

OFCondition EctEnhancedCT::setMultiEnergyCTAcquisition(const OFString& value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_MultiEnergyCTAcquisition.putOFStringArray(value);
    return result;
}

OFCondition EctEnhancedCT::setPixelPresentation(const EctTypes::E_PixelPresentation value, const OFBool check)
{
    (void)check;
    if ((value == EctTypes::E_PixelPres_Empty) || (value == EctTypes::E_PixelPres_Invalid))
        return ECT_InvalidAttributeValue;

    return m_PixelPresentation.putOFStringArray(EctTypes::pixelPresToStr(value));
}

OFCondition EctEnhancedCT::setVolumetricProperties(const EctTypes::E_VolumetricProperties value, const OFBool check)
{
    (void)check;
    if ((value == EctTypes::E_VolProps_Empty) || (value == EctTypes::E_VolProps_Invalid))
        return ECT_InvalidAttributeValue;

    return m_VolumetricProperties.putOFStringArray(EctTypes::volPropsToStr(value));
}

OFCondition EctEnhancedCT::setVolumeBasedCalculationTechnique(const OFString& value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_VolumeBasedCalculationTechnique.putOFStringArray(value);
    return result;
}

// -------------------- Protected Helpers --------------------------

OFCondition EctEnhancedCT::read(DcmItem& dataset)
{
    OFCondition result = OFvisit<OFCondition>(ReadVisitor(dataset, *this), getImagePixel());
    return result;
}

OFCondition EctEnhancedCT::write(DcmItem& dataset)
{
    if (!check())
    {
        return IOD_EC_InvalidObject;
    }

    OFCondition result = OFvisit<OFCondition>(WriteVisitor(dataset, *this), getImagePixel());
    if (result.good())
        result = writeGeneric(dataset);
    return result;
}

OFCondition EctEnhancedCT::writeConcatenation(ConcatenationCreator& cc)
{
    if (!check())
    {
        return IOD_EC_InvalidObject;
    }

    DcmItem* item = new DcmItem();
    if (!item)
        return EC_MemoryExhausted;

    Uint8* pixData       = NULL;
    size_t pixDataLength = 0;

    OFCondition result
        = OFvisit<OFCondition>(WriteVisitorConcatenation(*this, pixData, pixDataLength), getImagePixel());

    if (result.good())
        result = writeGeneric(*item);
    if (result.good())
    {
        result = cc.setCfgInput(item, pixData, pixDataLength, OFTrue /* cc should take ownership */);
    }
    return result;
}

OFCondition EctEnhancedCT::writeGeneric(DcmItem& dataset)
{
    OFCondition result;
    if (m_SynchronisationModuleEnabled)
        result = m_SynchronisationModule.write(dataset);
    if (result.good())
        result = m_FG.setNumberOfFrames(
            DcmIODUtil::limitMaxFrames(m_Frames.size(), "Maximum number of frames exceeded, will write 2147483647"));
    if (result.good())
        result = m_FG.write(dataset);
    if (result.good())
        result = m_FGInterface.write(dataset);
    if (result.good())
        result = m_DimensionModule.write(dataset);
    if (result.good())
        result = m_AcquisitionContextModule.write(dataset);
    if (result.good())
        result = m_CommonInstanceReferenceModule.write(dataset);
    if (result.good())
        result = getIODGeneralSeriesModule().setModality("CT");
    if (result.good())
        result = IODImage::write(dataset);
    // Write after General Equipment Module from IODImage
    if (result.good())
        result = m_EnhancedGeneralEquipmentModule.write(dataset);

    if (result.good())
        result = DcmIODUtil::copyElementToDataset(
            result, dataset, m_ImageType, "4" /* vm */, "1" /*requirement type*/, "EnhancedCTImageModule");
    if (result.good())
        result = DcmIODUtil::copyElementToDataset(
            result, dataset, m_MultiEnergyCTAcquisition, "1", "3", "EnhancedCTImageModule");
    if (result.good())
        result
            = DcmIODUtil::copyElementToDataset(result, dataset, m_PixelPresentation, "1", "1", "EnhancedCTImageModule");
    if (result.good())
        result = DcmIODUtil::copyElementToDataset(
            result, dataset, m_VolumetricProperties, "1", "1", "EnhancedCTImageModule");
    if (result.good())
        result = DcmIODUtil::copyElementToDataset(
            result, dataset, m_VolumeBasedCalculationTechnique, "1", "1", "EnhancedCTImageModule");
    if (result.good())
        result
            = DcmIODUtil::copyElementToDataset(result, dataset, m_AcquisitionNumber, "1", "3", "EnhancedCTImageModule");
    if (result.good())
        result = DcmIODUtil::copyElementToDataset(
            result, dataset, m_AcquisitionDateTime, "1", "1C", "EnhancedCTImageModule");
    if (result.good())
        result = DcmIODUtil::copyElementToDataset(
            result, dataset, m_AcquisitionDuration, "1", "1C", "EnhancedCTImageModule");
    if (result.good())
        result = DcmIODUtil::copyElementToDataset(
            result, dataset, m_ContentQualification, "1", "1C", "EnhancedCTImageModule");
    if (result.good())
        result = DcmIODUtil::copyElementToDataset(result, dataset, m_ImageComments, "1", "3", "EnhancedCTImageModule");
    if (result.good())
        result = DcmIODUtil::copyElementToDataset(
            result, dataset, m_BurnedInAnnotation, "1", "1C", "EnhancedCTImageModule");
    if (result.good())
        result = DcmIODUtil::copyElementToDataset(
            result, dataset, m_RecognizableVisualFeatures, "1", "3", "EnhancedCTImageModule");
    if (result.good())
        result = DcmIODUtil::copyElementToDataset(
            result, dataset, m_LossyImageCompression, "1", "1", "EnhancedCTImageModule");
    if (result.good())
        result = DcmIODUtil::copyElementToDataset(
            result, dataset, m_LossyImageCompressionMethod, "1-n", "1C", "EnhancedCTImageModule");

    if (result.good())
        result = DcmIODUtil::copyElementToDataset(
            result, dataset, m_LossyImageCompressionRatio, "1-n", "1C", "EnhancedCTImageModule");

    m_PresentationLUTShape.putOFStringArray("IDENTITY");
    if (result.good())
        result = DcmIODUtil::copyElementToDataset(
            result, dataset, m_PresentationLUTShape, "1", "1", "EnhancedCTImageModule");
    if (result.good())
        result
            = DcmIODUtil::copyElementToDataset(result, dataset, m_IsoCenterPosition, "3", "3", "EnhancedCTImageModule");
    return result;
}

OFCondition EctEnhancedCT::readGeneric(DcmItem& dataset)
{
    OFString sopClass;
    if (DcmIODUtil::checkSOPClass(&dataset, UID_EnhancedCTImageStorage, sopClass).bad())
    {
        DCMECT_ERROR("Given file does not seem to be a Enhanced CT storage object since SOP class is: " << sopClass);
        return IOD_EC_WrongSOPClass;
    }

    IODImage::read(dataset);
    m_SynchronisationModule.read(dataset);
    m_EnhancedGeneralEquipmentModule.read(dataset);
    m_FG.read(dataset);
    m_DimensionModule.read(dataset);
    m_AcquisitionContextModule.read(dataset);
    m_CommonInstanceReferenceModule.read(dataset);
    m_FGInterface.read(dataset);

    // Enhanced CT Image Module
    DcmIODUtil::getAndCheckElementFromDataset(dataset, m_ImageType, "4", "1C", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(dataset, m_MultiEnergyCTAcquisition, "1", "3", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(dataset, m_PixelPresentation, "1", "1", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(dataset, m_VolumetricProperties, "1", "1", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(
        dataset, m_VolumeBasedCalculationTechnique, "1", "1", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(dataset, m_AcquisitionNumber, "1", "3", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(dataset, m_AcquisitionDateTime, "1", "1C", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(dataset, m_AcquisitionDuration, "1", "1C", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(dataset, m_ContentQualification, "1", "1C", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(dataset, m_BurnedInAnnotation, "1", "1C", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(dataset, m_RecognizableVisualFeatures, "1", "3", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(dataset, m_LossyImageCompression, "1", "1", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(
        dataset, m_LossyImageCompressionMethod, "1-n", "1C", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(
        dataset, m_LossyImageCompressionRatio, "1-n", "1C", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(dataset, m_PresentationLUTShape, "1", "1", "EnhancedCTImageModule");
    DcmIODUtil::getAndCheckElementFromDataset(dataset, m_IsoCenterPosition, "3", "3", "EnhancedCTImageModule");
    return EC_Normal;
}

OFCondition EctEnhancedCT::decompress(DcmDataset& dset)
{
    DcmXfer xfer = dset.getOriginalXfer();
    OFCondition result;
    // If the original transfer is encapsulated and we do not already have an uncompressed version, decompress or reject
    // the file
    if (xfer.isEncapsulated())
    {
        DCMECT_DEBUG("Enhanced CT object is compressed, converting to uncompressed transfer syntax first");
        result = DcmIODUtil::decompress(dset);
    }
    return result;
}

// Instantiate Frames template with both possible pixel data types
template class EctEnhancedCT::Frames<Uint16>;
template class EctEnhancedCT::Frames<Sint16>;
