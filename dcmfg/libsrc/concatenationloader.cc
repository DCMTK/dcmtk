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
 *  Purpose: Class for loading Concatenations
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmfg/concatenationloader.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/ofstd/ofstd.h"

//  --------------------------------- Public API ------------------------------

// Constructor
ConcatenationLoader::ConcatenationLoader()
    : m_Concats()
    , m_FailedFiles()
    , m_ignoreMissingSourceUID(OFFalse)
    , m_Result(NULL)
    , m_Frames()
{
}

ConcatenationLoader::~ConcatenationLoader()
{
    OFMap<OFString, Info*>::iterator it = m_Concats.begin();
    while (it != m_Concats.end())
    {
        delete it->second;
        m_Concats.erase(it);
        it = m_Concats.begin();
    }
}

void ConcatenationLoader::setIgnoreMissingSourceUID(const OFBool ignore)
{
    m_ignoreMissingSourceUID = ignore;
}

OFCondition ConcatenationLoader::scan(const OFFilename& directory, const OFFilename& pattern, OFBool recursive)
{
    OFList<OFFilename> files;
    size_t results = OFStandard::searchDirectoryRecursively(directory, files, pattern, "", recursive);
    DCMFG_DEBUG("Found " << results << " for Concatenation scanning");
    return scan(files);
}

OFCondition ConcatenationLoader::scan(const OFList<OFFilename>& files)
{
    OFCondition result;
    OFListConstIterator(OFFilename) it = files.begin();
    size_t count                       = 1;
    while (it != files.end())
    {
        DCMFG_DEBUG("Scanning file #" << count << "/" << files.size());
        Info info;
        handleFile(*it, info);
        it++;
        count++;
    }

    doScanFinalChecks();
    return result;
}

OFCondition ConcatenationLoader::doScanFinalChecks()
{
    ScanResultIt c = m_Concats.begin();
    while (c != m_Concats.end())
    {
        if ((c->second->m_inConcatTotalNumber > 0) && (c->second->m_Files.size() != (c->second->m_inConcatTotalNumber)))
        {
            DCMFG_WARN("In-Concatenation Total Number ("
                       << c->second->m_inConcatTotalNumber << ") does not match number of Instances ("
                       << c->second->m_Files.size() << ") found for this Concatenation");
        }
        c++;
    }
    return EC_Normal;
}

void ConcatenationLoader::handleFile(const OFFilename& file, ConcatenationLoader::Info& info)
{
    OFCondition result;
    DcmFileFormat dcmff;
    OFString err;
    Info::Instance inst;
    result = dcmff.loadFile(file);
    if (result.good())
    {
        DcmDataset* dset = dcmff.getDataset();
        if (dset)
        {
            dset->findAndGetOFStringArray(DCM_SOPInstanceUID, inst.m_sopInstanceUID);
            dset->findAndGetOFStringArray(DCM_ConcatenationUID, info.m_ConcatenationUID);
            dset->findAndGetOFStringArray(DCM_SOPInstanceUIDOfConcatenationSource, info.m_SourceUID);
            dset->findAndGetUint16(DCM_InConcatenationTotalNumber, info.m_inConcatTotalNumber);
            dset->findAndGetOFStringArray(DCM_PatientID, info.m_PatientID);
            dset->findAndGetOFStringArray(DCM_StudyInstanceUID, info.m_StudyInstanceUID);
            dset->findAndGetOFStringArray(DCM_SeriesInstanceUID, info.m_SeriesInstanceUID);
            dset->findAndGetOFStringArray(DCM_SOPClassUID, info.m_SOPClassUID);
            dset->findAndGetUint16(DCM_BitsAllocated, info.m_BitsAlloc);
            dset->findAndGetUint16(DCM_Rows, info.m_Rows);
            dset->findAndGetUint16(DCM_Columns, info.m_Cols);
            Sint32 numFrames = 0;
            if (dset->findAndGetSint32(DCM_NumberOfFrames, numFrames).good())
            {
                if (numFrames > 0)
                    inst.m_NumberOfFrames = OFstatic_cast(Uint32, numFrames);
            }
            dset->findAndGetUint16(DCM_InConcatenationNumber, inst.m_InConcatenationNumber);
            inst.m_Filename = file;
            checkAndInsertInfo(info, inst, err);
        }
        else
        {
            err = "No dataset found";
        }
    }
    else
    {
        err = "No DICOM file";
    }
    if (!err.empty())
    {
        m_FailedFiles.push_back(Failure(file, err, inst.m_sopInstanceUID));
    }
}

void ConcatenationLoader::checkAndInsertInfo(const ConcatenationLoader::Info& info,
                                             const ConcatenationLoader::Info::Instance& inst,
                                             OFString& error)
{
    if (!info.m_ConcatenationUID.empty())
    {
        OFMap<OFString, Info*>::iterator it = m_Concats.find(info.m_ConcatenationUID);
        if (it == m_Concats.end())
        {
            if ((!info.m_SourceUID.empty() || m_ignoreMissingSourceUID) && (!info.m_StudyInstanceUID.empty())
                && (!info.m_SeriesInstanceUID.empty()) && (!info.m_SOPClassUID.empty()) && (info.m_BitsAlloc != 0)
                && (info.m_Rows != 0) && (info.m_Cols != 0) && (inst.m_InConcatenationNumber != 0)
                && (!inst.m_sopInstanceUID.empty()) && (inst.m_NumberOfFrames != 0))
            {
                //  This is a new concatenation, add to list
                Info* newInfo             = new Info;
                *newInfo                  = info;
                newInfo->m_NumTotalFrames = inst.m_NumberOfFrames;
                newInfo->m_Files.push_back(inst);
                if (!m_Concats.insert(OFMake_pair(info.m_ConcatenationUID, newInfo)).second)
                {
                    error = "Cannot insert into internal data structure";
                    delete newInfo;
                }
            }
            else
            {
                error = "File does not provide all required Concatenation Data";
            }
        }
        else
        {
            Info* c = (*it).second;
            if ((c->m_BitsAlloc == info.m_BitsAlloc) && (c->m_Cols == info.m_Cols) && (c->m_Rows == info.m_Rows)
                && (c->m_SOPClassUID == info.m_SOPClassUID) && (c->m_SeriesInstanceUID == info.m_SeriesInstanceUID)
                && (c->m_StudyInstanceUID == info.m_StudyInstanceUID) && emptyOrEqual(c->m_PatientID, info.m_PatientID)
                && emptyOrEqual(c->m_SourceUID, info.m_SourceUID)
                && zeroOrEqual(c->m_inConcatTotalNumber, info.m_inConcatTotalNumber)
                && (c->m_ConcatenationUID == info.m_ConcatenationUID)) /* should always be true, but...  */
            {
                //  Add file to this concatenation
                OFListIterator(ConcatenationLoader::Info::Instance) sortIt = c->m_Files.begin();
                while (sortIt != c->m_Files.end())
                {
                    if ((*sortIt).m_InConcatenationNumber > inst.m_InConcatenationNumber)
                        break;
                    sortIt++;
                }
                c->m_NumTotalFrames += inst.m_NumberOfFrames;
                c->m_inConcatTotalNumber = getNotZero(c->m_inConcatTotalNumber, info.m_inConcatTotalNumber);
                c->m_Files.insert(sortIt, inst);
            }
            else
            {
                error = "Concatenation Data inconsistent to rest of Concatenation";
            }
        }
    }
    else
    {
        error = "File is not part of Concatenation";
    }
}

const ConcatenationLoader::TScanResult& ConcatenationLoader::getInfo()
{
    return m_Concats;
}

const ConcatenationLoader::TScanFailures& ConcatenationLoader::getFailedFiles()
{
    return m_FailedFiles;
}

OFCondition
ConcatenationLoader::load(const OFString& concatenationUID, DcmDataset* dataset, OFVector<DcmIODTypes::Frame*>& frames)
{
    if (dataset == NULL)
        return EC_IllegalParameter;
    m_Result = dataset;

    OFCondition result;
    ScanResultIt concat = m_Concats.find(concatenationUID);
    if (concat != m_Concats.end())
    {
        Info* c = concat->second;
        if (c || !c->m_Files.empty())
        {
            // Create dataset to work with from first concatenation instance
            DCMFG_DEBUG("Loading Concatenation " << concatenationUID << " from " << c->m_Files.size() << " instances");
            result = prepareTemplate(*concat->second);
            if (result.good())
            {
                OFListIterator(ConcatenationLoader::Info::Instance) inst = c->m_Files.begin();
                while (result.good() && (inst != c->m_Files.end()))
                {
                    DcmFileFormat dcmff;
                    result = dcmff.loadFile(inst->m_Filename);
                    if (result.good())
                    {
                        if ((*c).m_BitsAlloc == 1)
                        {
                            result = extractBinaryFrames(*dcmff.getDataset(), *c, inst->m_NumberOfFrames);
                        }
                        else
                        {
                            result = extractFrames(*dcmff.getDataset(), *c, inst->m_NumberOfFrames);
                        }

                        if (result.good())
                            result = movePerFrameItems(*dcmff.getDataset());
                    }
                    inst++;
                }
                if (result.good())
                {
                    if (result.good())
                    {
                        frames = m_Frames;
                        result = insertDestinationAttributes();
                    }
                    else
                    {
                        DcmIODUtil::freeContainer(m_Frames);
                    }
                }
            }
        }
        else
        {
            result = EC_InternalError;
        }
    }

    return result;
}

OFCondition ConcatenationLoader::prepareTemplate(ConcatenationLoader::Info& firstInstance)
{
    DcmFileFormat dcmff;
    OFCondition result = dcmff.loadFile(firstInstance.m_Files.front().m_Filename);
    if (result.good())
    {
        *m_Result = *dcmff.getDataset();
        OFString srcUID;
        result = m_Result->findAndGetOFStringArray(DCM_SOPInstanceUIDOfConcatenationSource, srcUID);
        if (result.bad() && m_ignoreMissingSourceUID)
            result = EC_Normal;
        if (result.good())
            result = deleteConcatAttributes(*m_Result);
        if (result.good())
            result = m_Result->putAndInsertOFStringArray(DCM_SOPInstanceUID, srcUID);
        if (result.good())
        {
            // Clear Per-Frame Functional Groups Sequence
            result = m_Result->findAndDeleteElement(DCM_PerFrameFunctionalGroupsSequence);
            if (result.good())
            {
                result = m_Result->insertEmptyElement(DCM_PerFrameFunctionalGroupsSequence);
            }
        }
        if (result.good())
        {
            m_Result->findAndDeleteElement(DCM_PixelData);
        }
    }
    return result;
}

OFCondition ConcatenationLoader::deleteConcatAttributes(DcmItem& item)
{
    item.findAndDeleteElement(DCM_ConcatenationUID);
    item.findAndDeleteElement(DCM_InConcatenationNumber);
    item.findAndDeleteElement(DCM_InConcatenationTotalNumber);
    item.findAndDeleteElement(DCM_SOPInstanceUIDOfConcatenationSource);
    item.findAndDeleteElement(DCM_ConcatenationFrameOffsetNumber);
    item.findAndDeleteElement(DCM_NumberOfFrames);
    item.findAndDeleteElement(DCM_ContentDate);
    item.findAndDeleteElement(DCM_ContentTime);
    item.findAndDeleteElement(DCM_SOPInstanceUID);
    return EC_Normal;
}

OFCondition ConcatenationLoader::extractFrames(DcmItem& item, Info& info, const Uint32 numFrames)
{
    const Uint8* pixData = NULL;
    OFCondition result   = item.findAndGetUint8Array(DCM_PixelData, pixData);
    if (result.good() && pixData)
    {
        size_t bytesPerFrame = 0;
        result                 = computeBytesPerFrame(info.m_Rows, info.m_Cols, info.m_BitsAlloc, bytesPerFrame);
        if (result.good())
        {
            const Uint8* ptr = pixData;
            for (Uint32 f = 0; f < numFrames; f++)
            {
                DcmIODTypes::Frame* frame = new DcmIODTypes::Frame();
                if (frame)
                {
                    frame->length  = bytesPerFrame;
                    frame->pixData = new Uint8[frame->length];
                    if (frame->pixData)
                    {
                        memcpy(frame->pixData, ptr, frame->length);
                        ptr += frame->length;
                        m_Frames.push_back(frame);
                    }
                    else
                    {
                        result = EC_MemoryExhausted;
                    }
                }
                else
                {
                    result = EC_MemoryExhausted;
                }
            }
        }
    }
    else
    {
        result = FG_EC_PixelDataMissing;
    }
    return result;
}

OFCondition ConcatenationLoader::extractBinaryFrames(DcmItem& item, Info& info, const Uint32 numFrames)
{
    Uint8* pixData          = NULL;
    DcmElement* pixDataElem = NULL;
    OFCondition result      = item.findAndGetElement(DCM_PixelData, pixDataElem);
    if (result.good())
        result = pixDataElem->getUint8Array(pixData);
    if (result.good() && pixData)
    {
        result = DcmIODUtil::extractBinaryFrames(pixData, numFrames, info.m_Rows * info.m_Cols, m_Frames);
    }
    else
    {
        result = FG_EC_PixelDataMissing;
    }
    return result;
}

OFCondition ConcatenationLoader::computeBytesPerFrame(const Uint16 rows,
                                                      const Uint16 cols,
                                                      const Uint16 bitsAlloc,
                                                      size_t& bytes_per_frame)
{
    // Usually bytes of one frame simply computes using cols * rows * num bytes per pixel.
    // This is only different if bits allocated equals 1, which can happen
    // for binary segmentations or black and white secondary capture objects
    // (second SC generation).
    // Other values than Bits Allocated 16 or 8 are not supported.
    bytes_per_frame = bitsAlloc * cols * rows;
    if ((bitsAlloc == 16) || (bitsAlloc == 8))
    {
        // result in rows * cols * bytes per frame
        bytes_per_frame = bytes_per_frame / 8;
    }
    else if (bitsAlloc == 1)
    {
        // results in rows * cols / 8 if rows*cols is dividable by 8,
        // otherwise rows * cols / 8 +1 (extra byte)
        OFBool remainder = (bytes_per_frame % 8 != 0) ? OFTrue: OFFalse;
        bytes_per_frame  = bytes_per_frame / 8;
        if (remainder)
        {
            bytes_per_frame++;
        }
    }
    else
    {
        DCMFG_ERROR("Bits Allocated=" << bitsAlloc << " not supported, must be 1, 8 or 16");
        return FG_EC_UnsupportedPixelDataLayout;
    }
    return EC_Normal;
}

OFCondition ConcatenationLoader::movePerFrameItems(DcmItem& item)
{
    OFCondition result;
    DcmSequenceOfItems* srcPerFrame = NULL;
    DcmSequenceOfItems* dstPerFrame = NULL;
    m_Result->findAndGetSequence(DCM_PerFrameFunctionalGroupsSequence, dstPerFrame);
    if (item.findAndGetSequence(DCM_PerFrameFunctionalGroupsSequence, srcPerFrame).good() && dstPerFrame)
    {
        while (result.good() && (srcPerFrame->card() != 0))
        {
            result = dstPerFrame->append(srcPerFrame->remove(OFstatic_cast(unsigned long, 0)));
        }
    }
    return result;
}

OFCondition ConcatenationLoader::insertDestinationAttributes()
{
    Info* info   = m_Concats.begin()->second;
    OFString uid = info->m_SourceUID;
    if (uid.empty())
    {
        char buf[100];
        dcmGenerateUniqueIdentifier(buf, SITE_INSTANCE_UID_ROOT);
        uid = buf;
        DCMFG_WARN("SOP Instance UID of Concatenation Source (0020,0242) not set, created new SOP Instance UID "
                   << uid);
    }
    OFCondition result = m_Result->putAndInsertOFStringArray(DCM_SOPInstanceUID, uid);
    if (result.good())
    {
        OFStringStream s;
        s << info->m_NumTotalFrames;
        result = m_Result->putAndInsertOFStringArray(DCM_NumberOfFrames, s.str().c_str());
    }
    if (result.good())
    {
        OFDate date;
        date.setCurrentDate();
        OFString tempstr;
        date.getISOFormattedDate(tempstr, OFFalse /* no delimiters */);
        result = m_Result->putAndInsertOFStringArray(DCM_ContentDate, tempstr);
        if (result.good())
        {
            OFTime time;
            time.setCurrentTime();
            time.getISOFormattedTime(tempstr, OFTrue /* include seconds */, OFFalse, OFFalse, OFFalse);
            result = m_Result->putAndInsertOFStringArray(DCM_ContentTime, tempstr);
        }
    }
    return result;
}

OFBool ConcatenationLoader::zeroOrEqual(const size_t num1, const size_t num2)
{
    if (num1 == num2)
        return OFTrue;
    if ((num1 == 0) || (num2 == 0))
        return OFTrue;
    return OFFalse;
}

OFBool ConcatenationLoader::emptyOrEqual(const OFString& str1, const OFString& str2)
{
    if (str1 == str2)
        return OFTrue;
    if (str1.empty() || str2.empty())
        return OFTrue;
    return OFFalse;
}

template <typename T>
T ConcatenationLoader::getNotZero(const T num1, const T num2)
{
    if (num1 != 0)
        return num1;
    return num2;
}

ConcatenationLoader::Info::Info()
    : m_Files()
    , m_FileConatenationSource()
    , m_ConcatenationUID()
    , m_SourceUID()
    , m_inConcatTotalNumber(0)
    , m_NumTotalFrames(0)
    , m_PatientID()
    , m_StudyInstanceUID()
    , m_SeriesInstanceUID()
    , m_SOPClassUID()
    , m_BitsAlloc(0)
    , m_Rows(0)
    , m_Cols(0)
{
}

ConcatenationLoader::Info& ConcatenationLoader::Info::operator=(const ConcatenationLoader::Info& rhs)
{
    this->m_FileConatenationSource   = rhs.m_FileConatenationSource;
    this->m_ConcatenationUID         = rhs.m_ConcatenationUID;
    this->m_SourceUID                = rhs.m_SourceUID;
    this->m_NumTotalFrames           = rhs.m_NumTotalFrames;
    this->m_inConcatTotalNumber      = rhs.m_inConcatTotalNumber;
    this->m_PatientID                = rhs.m_PatientID;
    this->m_StudyInstanceUID         = rhs.m_StudyInstanceUID;
    this->m_SeriesInstanceUID        = rhs.m_SeriesInstanceUID;
    this->m_SOPClassUID              = rhs.m_SOPClassUID;
    this->m_BitsAlloc                = rhs.m_BitsAlloc;
    this->m_Rows                     = rhs.m_Rows;
    this->m_Cols                     = rhs.m_Cols;
    OFListConstIterator(Instance) it = rhs.m_Files.begin();
    while (it != rhs.m_Files.end())
    {
        this->m_Files.push_back(*it);
        it++;
    }
    return *this;
}

ConcatenationLoader::Info::Info(const ConcatenationLoader::Info& rhs)
{
    if (&rhs == this)
        return;

    *this = rhs;
}

void ConcatenationLoader::Info::print(OFStringStream& out)
{
    out << "Concatenation UID*           : " << m_ConcatenationUID << OFendl;
    out << "  SOP Class UID*             : " << m_SOPClassUID << OFendl;
    out << "  Concatenation Source UID*  : " << m_SourceUID << OFendl;
    out << "  Concatenation Source File  : " << m_FileConatenationSource << OFendl;
    out << "  Number of Frames (computed): " << m_NumTotalFrames << OFendl;
    out << "  In-conc. Total Number      : " << m_inConcatTotalNumber << OFendl;
    out << "  Patient ID                 : " << m_PatientID << OFendl;
    out << "  Study Instance UID*        : " << m_StudyInstanceUID << OFendl;
    out << "  Series Instance UID*       : " << m_SeriesInstanceUID << OFendl;
    out << "  Bits Allocated*            : " << m_BitsAlloc << OFendl;
    out << "  Rows*                      : " << m_Rows << OFendl;
    out << "  Columns*                   : " << m_Cols << OFendl;
    out << "  Files: " << OFendl;
    OFListIterator(Instance) f = m_Files.begin();
    size_t count               = 1;
    while (f != m_Files.end())
    {
        out << "    " << count << ". " << (*f).m_Filename << OFendl;
        out << "    SOP Instance UID: " << (*f).m_sopInstanceUID << OFendl;
        out << "    Number of Frames: " << (*f).m_NumberOfFrames << OFendl;
        out << "    In-Concatenation Frame Offset Number: " << (*f).m_InConcatenationNumber << OFendl;
        f++;
        count++;
    }
}

ConcatenationLoader::Info::~Info()
{
}

ConcatenationLoader::Info::Instance::Instance()
    : m_Filename()
    , m_sopInstanceUID()
    , m_NumberOfFrames(0)
    , m_InConcatenationNumber(0)
{
}

ConcatenationLoader::Info::Instance::~Instance()
{
}
