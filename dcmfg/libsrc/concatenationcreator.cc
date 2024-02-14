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
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for creating Concatenations
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmfg/concatenationcreator.h"
#include "dcmtk/dcmfg/fgtypes.h"


// Maximum number of instances that make up a Concatenation
const Uint16 ConcatenationCreator::m_MAX_INSTANCES_PER_CONCATENATION = 65535;

/// Maximum number of bytes for uncompressed pixel data (=2^32-2), derived from
/// 32 bit length field of Pixel Data attribute with even length being required.
Uint32 const ConcatenationCreator::m_MAX_PIXEL_DATA_LENGTH = 4294967294UL;


//  --------------------------------- Public API ------------------------------

// Constructor
ConcatenationCreator::ConcatenationCreator()
    : m_configured(OFFalse)
    , m_cfgTransferOwnership(OFFalse)
    , m_cfgNumFramesPerInstance(25)
    , m_numBitsFrame(0)
    , m_srcDataset(OFnullptr)
    , m_srcSOPInstanceUID()
    , m_srcPixelData(OFnullptr)
    , m_VRPixelData(EVR_UN)
    , m_srcPerFrameFG(OFnullptr)
    , m_srcNumFrames(0)
    , m_dstNumInstances(0)
    , m_dstNumFramesPerInstance(0)
    , m_dstNumFramesLastInstance(0)
    , m_dstConcatenationUID()
    , m_dstInstanceNumber("1")
    , m_currentSrcFrame(0)
    , m_currentPerFrameItem(OFnullptr)
    , m_currentInstanceNum(0)
{
}

ConcatenationCreator::~ConcatenationCreator()
{
    if (m_cfgTransferOwnership)
    {
        delete m_srcDataset;
        delete[] m_srcPixelData;
    }
}

OFCondition ConcatenationCreator::setCfgInput(DcmItem* srcDataset, OFBool transferOwnership)
{
    // Check pixel data exists and is in native format (i.e. uncompressed)
    DcmElement* elem = NULL;
    srcDataset->findAndGetElement(DCM_PixelData, elem);
    if (!elem)
        return FG_EC_PixelDataMissing;
    DcmPixelData* pixDataElem = OFstatic_cast(DcmPixelData*, elem);
    if (!pixDataElem)
        return FG_EC_PixelDataMissing;
    if (!pixDataElem->canWriteXfer(EXS_LittleEndianExplicit, EXS_LittleEndianExplicit /* ignored */))
        return EC_UnsupportedEncoding;
    OFCondition result = pixDataElem->getUint8Array(m_srcPixelData);
    if (!m_srcPixelData || result.bad())
        return FG_EC_PixelDataMissing;

    m_srcDataset = srcDataset;

    m_cfgTransferOwnership = transferOwnership;

    // All fine
    return EC_Normal;
}

OFCondition ConcatenationCreator::setCfgInput(DcmItem* srcDataset,
                                              Uint8* pixelData,
                                              size_t /* pixelDataLength */,
                                              OFBool transferOwnership)
{
    // Check input parameters
    if (!pixelData)
        return FG_EC_PixelDataMissing;

    m_srcDataset           = srcDataset;
    m_srcPixelData         = pixelData;
    m_cfgTransferOwnership = transferOwnership;

    // All fine
    return EC_Normal;
}

OFCondition ConcatenationCreator::setCfgFramesPerInstance(Uint32 numFramesPerInstance)
{
    m_cfgNumFramesPerInstance = numFramesPerInstance;
    return EC_Normal;
}

OFBool ConcatenationCreator::setCfgInstanceNumber(const OFString& instanceNumber)
{
    if (DcmIntegerString::checkStringValue(instanceNumber, "1").bad())
        return OFFalse;

    m_dstInstanceNumber = instanceNumber;
    return OFTrue;
}

OFCondition ConcatenationCreator::writeNextInstance(DcmItem& dstDataset)
{
    OFCondition result;
    if (!m_configured)
    {
        result = configureCommon();
        if (result.bad())
        {
            return result;
        }
    }

    // Store number of frames for this instance for later use
    Uint32 numFramesThisInstance = numFramesCurrentDstInstance();
    if (numFramesThisInstance == 0)
    {
        return FG_EC_ConcatenationComplete;
    }

    // Clone destination dataset from source dataset,
    // Remove pixel data and per-frame functional groups before cloning, and re-insert
    // it again in order to leave the original dataset in unmodified state
    DcmElement* backup1 = m_srcDataset->remove(DCM_PerFrameFunctionalGroupsSequence);
    DcmElement* backup2 = m_srcDataset->remove(DCM_PixelData);
    dstDataset          = *m_srcDataset;
    m_srcDataset->insert(backup1);
    m_srcDataset->insert(backup2);

    // Allocate element/buffer for destination pixel data and other heap memory
    OFunique_ptr<DcmPixelData> dstPixelData(new DcmPixelData(DCM_PixelData));
    OFunique_ptr<DcmSequenceOfItems> dstPerFrameSeq(new DcmSequenceOfItems(DCM_PerFrameFunctionalGroupsSequence));
    if (!dstPixelData || !dstPerFrameSeq)
    {
        return EC_MemoryExhausted;
    }

    Uint8* dstData = NULL;
    size_t numTotalBytesInstance = (m_numBitsFrame * numFramesThisInstance) / 8;
    // Cast is safe, checked in configureCommon()
    dstPixelData->createUint8Array(OFstatic_cast(Uint32, numTotalBytesInstance), dstData);
    if (!dstData)
    {
        return EC_MemoryExhausted;
    }
    dstPixelData->setVR(m_VRPixelData);
    size_t srcPos = (m_numBitsFrame * m_currentSrcFrame) / 8;
    memcpy(dstData, &m_srcPixelData[srcPos], numTotalBytesInstance);
    result = dstDataset.insert(dstPixelData.release());
    if (result.good())
    {
        // Copy per-frame functional groups for the related frames
        for (size_t frameCount = 0; frameCount < numFramesThisInstance; frameCount++)
        {
            if ((m_currentPerFrameItem == NULL) && (m_currentSrcFrame == m_dstNumInstances - 1))
            {
                // Last instance might have less frames
                break;
            }
            else if ((m_currentPerFrameItem == NULL) && (m_currentSrcFrame != m_dstNumInstances - 1))
            {
                DCMFG_ERROR("Not enough items in Per-frame Functional group while copying instance #"
                            << m_currentSrcFrame + 1 << "/" << m_dstNumInstances);
                result = FG_EC_NotEnoughFrames;
                break;
            }
            DcmItem* newPerFrameItem = OFstatic_cast(DcmItem*, m_currentPerFrameItem->clone());
            if (newPerFrameItem)
            {
                dstPerFrameSeq->insert(newPerFrameItem);
            }
            else
            {
                result = EC_MemoryExhausted;
                break;
            }
            goToNextFrame();
        }
        if (result.good())
        {
            result = dstDataset.insert(dstPerFrameSeq.release());
        }
        if (result.good())
        {
            result = setConcatenationAttributes(dstDataset, numFramesThisInstance);
        }
    }
    if (result.good())
    {
        m_currentInstanceNum++;
    }
    return result;
}

OFCondition ConcatenationCreator::writeNextInstance(const OFFilename& fn)
{
    OFCondition result;
    if (!m_configured)
    {
        result = configureCommon();
        if (result.bad())
        {
            return result;
        }
    }
    DcmFileFormat dcmff;
    result = writeNextInstance(*(dcmff.getDataset()));
    if (result.good())
    {
        result = dcmff.saveFile(fn, EXS_LittleEndianExplicit);
    }
    return result;
}

size_t ConcatenationCreator::getNumInstances()
{
    if (!m_configured)
    {
        OFCondition result = configureCommon();
        if (result.bad())
        {
            DCMFG_ERROR("Unable to compute number of instances for Concatenation, maybe input not initialized?)");
            return 0;
        }
    }
    return m_dstNumInstances;
}

//  --------------------------------- Helpers ------------------------------

OFBool ConcatenationCreator::checkSOPClass(DcmItem& srcDataset)
{
    // Check whether the SOP Class is not one of the following:
    // - Ophthalmic Tomography Image Storage
    // - Ophthalmic Optical Coherence Tomography B-Scan Volume Analysis
    // since the standard forbids to make Concatenations of these.
    OFString sopClass;
    srcDataset.findAndGetOFStringArray(DCM_SOPClassUID, sopClass);
    if (sopClass.empty())
        return OFFalse;

    if ((sopClass == UID_OphthalmicTomographyImageStorage)
        || (sopClass == UID_OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage))
        return OFFalse;

    return OFTrue;
}

OFBool ConcatenationCreator::checkColorModel(DcmItem& srcDataset)
{
    OFString str;
    srcDataset.findAndGetOFStringArray(DCM_PhotometricInterpretation, str);
    if (str.empty())
    {
        DCMFG_ERROR("Photometric Interpretation is missing");
        return OFFalse;
    }
    if ((str != "RGB") && (str != "MONOCHROME1") && (str != "MONOCHROME2") && (str != "YBR_FULL"))
    {
        DCMFG_ERROR("Photometric Interpretation '" << str << "' not supported");
        return OFFalse;
    }

    Uint16 p;
    if (srcDataset.findAndGetUint16(DCM_PlanarConfiguration, p).good())
    {
        if (p != 0)
        {
            DCMFG_ERROR("Planar Configuration '" << p << "' not supported (must be 0 if present)");
            return OFFalse;
        }
    }

    return OFTrue;
}

OFCondition ConcatenationCreator::setConcatenationAttributes(DcmItem& dstDataset, Uint32 numFramesCurrentInstance)
{
    OFCondition result = dstDataset.putAndInsertOFStringArray(DCM_ConcatenationUID, m_dstConcatenationUID);
    if (result.good())
        result = dstDataset.putAndInsertOFStringArray(DCM_InstanceNumber, m_dstInstanceNumber);
    if (result.good())
        result = dstDataset.putAndInsertUint32(DCM_ConcatenationFrameOffsetNumber,
                                               m_currentInstanceNum * m_dstNumFramesPerInstance);
    if (result.good())
        result = dstDataset.putAndInsertOFStringArray(DCM_SOPInstanceUIDOfConcatenationSource, m_srcSOPInstanceUID);
    if (result.good())
        result = dstDataset.putAndInsertUint16(DCM_InConcatenationNumber, m_currentInstanceNum + 1 /* first is #1 */);
    if (result.good())
    {
        // safe cast since configureCommon() already checks whether m_dstNumInstances is in Uint16 range
        result = dstDataset.putAndInsertUint16(DCM_InConcatenationTotalNumber, OFstatic_cast(Uint16, m_dstNumInstances));
    }
    if (result.good())
    {

        OFStringStream ss;
        ss << numFramesCurrentInstance;
        result = dstDataset.putAndInsertOFStringArray(DCM_NumberOfFrames, ss.str().c_str());
    }
    if (result.good())
    {
        char buf[100];
        dcmGenerateUniqueIdentifier(buf, SITE_INSTANCE_UID_ROOT);
        result = dstDataset.putAndInsertOFStringArray(DCM_SOPInstanceUID, buf);
    }
    return result;
}

OFCondition ConcatenationCreator::goToNextFrame()
{
    // Check whether converter is initialized, and whether frames are left
    if ((m_currentPerFrameItem == NULL) || (m_currentSrcFrame == m_srcNumFrames))
    {
        return FG_EC_NotEnoughFrames;
    }
    else
    {
        m_currentPerFrameItem = OFstatic_cast(DcmItem*, m_srcPerFrameFG->nextInContainer(m_currentPerFrameItem));
        m_currentSrcFrame++;
    }
    return EC_Normal;
}

Uint32 ConcatenationCreator::numFramesCurrentDstInstance()
{
    Uint32 numFrames = 0;
    // Calculate last frame number that will be part of "regular" bucket size
    Uint32 lastRegular = (m_srcNumFrames / m_dstNumFramesPerInstance) * m_dstNumFramesPerInstance - 1;
    if (m_currentSrcFrame == lastRegular + 1)
    {
        numFrames = m_dstNumFramesLastInstance;
    }
    else if (m_currentSrcFrame > lastRegular + 1)
    {
        return 0;
    }
    else
    {
        numFrames = m_dstNumFramesPerInstance;
    }
    return numFrames;
}

OFCondition ConcatenationCreator::configureCommon()
{
    if (!m_srcDataset || !m_srcPixelData)
        return EC_IllegalCall;

    // Check for Shared/Per-Frame Functional Group Sequence
    OFCondition result = m_srcDataset->findAndGetSequence(DCM_PerFrameFunctionalGroupsSequence, m_srcPerFrameFG);
    if (result.bad())
        return FG_EC_NoPerFrameFG;
    if (!m_srcDataset->tagExists(DCM_SharedFunctionalGroupsSequence))
        return FG_EC_NoSharedFG;

    // Check SOP Class allows Concatenations
    if (!checkSOPClass(*m_srcDataset))
        return FG_EC_SOPClassForbidsConcatenations;

    // Check Photometric Interpretation and Planar Configuration
    if (!checkColorModel(*m_srcDataset))
        return FG_EC_UnsupportedPixelDataLayout;

    // Check whether number of frames valid in conversion context
    m_dstNumFramesPerInstance = m_cfgNumFramesPerInstance;
    Sint32 srcNumFrames       = 0;
    result                    = m_srcDataset->findAndGetSint32(DCM_NumberOfFrames, srcNumFrames);
    if (result.bad() || (srcNumFrames <= 1))
        return FG_EC_NotEnoughFrames;
    m_srcNumFrames = OFstatic_cast(Uint32, srcNumFrames); // safe since we checked value before
    if (m_srcNumFrames < m_dstNumFramesPerInstance)
        return FG_EC_NotEnoughFrames;

    // Check pixel data length correct, i.e. whether there is a sufficient amount of data available
    Uint16 bitsAlloc, rows, cols;
    bitsAlloc = rows = cols = 0;
    m_srcDataset->findAndGetUint16(DCM_BitsAllocated, bitsAlloc);
    m_srcDataset->findAndGetUint16(DCM_Rows, rows);
    m_srcDataset->findAndGetUint16(DCM_Columns, cols);
    if ((rows == 0) || (cols == 0))
        return FG_EC_PixelDataDimensionsInvalid;
    // 8, 16 bit and 1 bit (relevant for Segmentation objects) are supported
    if ((bitsAlloc != 16) && (bitsAlloc != 8) && (bitsAlloc != 1))
        return FG_EC_PixelDataDimensionsInvalid;

    // Compute number of bits per frame
    m_numBitsFrame = rows * cols * bitsAlloc;

    // If Bits Allocated is 1 (i.e. not 8 or 16), the last byte of the frame can only be
    // partly occupied with bits. Since we work with bytewise memcpy later, make sure that
    // number of bytes per instance (number of frames per instance multiplied by the number
    // of bytes per frame) is dividable by 8. If not, increase number of frames per instance
    // so that number of bytes per instance is dividable by 8.
    if ((m_numBitsFrame * m_dstNumFramesPerInstance ) % 8 != 0)
    {
        size_t newDstNumFramesPerInstance = ((m_dstNumFramesPerInstance / 8) + 1) * 8;
        // if we finally have more destination frames per instance than we have in the source image,
        // just write a single concatenation instance containing all frames from source image
        if (newDstNumFramesPerInstance > m_srcNumFrames)
        {
            newDstNumFramesPerInstance = m_srcNumFrames;
        }
        DCMFG_INFO("Adapting Number of Frames per Instance from " << m_dstNumFramesPerInstance << " to " << newDstNumFramesPerInstance);
        m_dstNumFramesPerInstance = OFstatic_cast(Uint32, newDstNumFramesPerInstance);
    }

    // Remember number of instances to be produced
    Uint32 u32 = m_srcNumFrames / m_dstNumFramesPerInstance;
    m_dstNumFramesLastInstance = m_srcNumFrames % m_dstNumFramesPerInstance;
    if (m_dstNumFramesLastInstance > 0)
    {
        u32++;
    }
    if (u32 > m_MAX_INSTANCES_PER_CONCATENATION)
    {
        DCMFG_ERROR("Too many concatenation instances (" << u32 << "), maximum is " << m_MAX_INSTANCES_PER_CONCATENATION);
        return FG_EC_InvalidData;
    }
    m_dstNumInstances = OFstatic_cast(Uint16, u32); // safe now

    // Check whether number of items in per-frame functional groups is identical to Number of Frames attribute
    if (m_srcNumFrames != m_srcPerFrameFG->card())
    {
        DCMFG_ERROR("Number of Frames attribute does not equal number of items in Per-frame Functional Group Sequence");
        return FG_EC_NotEnoughFrames;
    }


    // Check whether pixel data for one instance stays below 4 GB
    size_t numTotalBytesInstance = (m_numBitsFrame * m_dstNumFramesPerInstance) / 8;
    if (numTotalBytesInstance > m_MAX_PIXEL_DATA_LENGTH)
    {
        DCMFG_ERROR("Uncompressed pixel data must not exceed " << m_MAX_PIXEL_DATA_LENGTH << "bytes per concatenation instance");
        return FG_EC_PixelDataDimensionsInvalid;
    }

    if (bitsAlloc <= 8) // 1 or 8 bit
    {
        m_VRPixelData = EVR_OB;
    }
    else
    {
        m_VRPixelData = EVR_OW;
    }
    // Store source SOP Instance UID
    m_srcDataset->findAndGetOFStringArray(DCM_SOPInstanceUID, m_srcSOPInstanceUID);
    if (m_srcSOPInstanceUID.empty())
        return EC_InvalidValue; // better code

    //  Create Concatenation UID
    char buf[65];
    m_dstConcatenationUID = dcmGenerateUniqueIdentifier(buf, SITE_INSTANCE_UID_ROOT);

    // Set initial context, i.e. initialize for first frame
    m_currentPerFrameItem = OFstatic_cast(DcmItem*, m_srcPerFrameFG->nextInContainer(NULL));
    m_currentSrcFrame     = 0;

    m_configured = OFTrue;
    return result;
}
