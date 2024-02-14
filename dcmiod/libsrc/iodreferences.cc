/*
 *
 *  Copyright (C) 2016-2024, Open Connections GmbH
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
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class for managing SOP references as used in different modules
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/iodreferences.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmiod/iodtypes.h"
#include "dcmtk/dcmiod/iodutil.h"

// ------------------- class IODReference -------------------------------

IODReference::IODReference(const IODReference::MAX_LEVEL level)
    : m_PatientID()
    , m_StudyInstanceUID()
    , m_SeriesInstanceUID()
    , m_SOPClassUID()
    , m_SOPInstanceUID()
    , m_Level(level)
{
}

IODReference::IODReference()
    : m_PatientID()
    , m_StudyInstanceUID()
    , m_SeriesInstanceUID()
    , m_SOPClassUID()
    , m_SOPInstanceUID()
    , m_Level(LEVEL_STUDY)
{
}

IODReference::~IODReference()
{
    // nothing to do
}

void IODReference::clear()
{
    m_StudyInstanceUID.clear();
    m_SeriesInstanceUID.clear();
    m_SOPClassUID.clear();
    m_SOPInstanceUID.clear();
}

OFBool IODReferences::add(IODReference* ref)
{
    if (ref->check().good())
    {
        m_References.push_back(ref);
        return OFTrue;
    }
    return OFFalse;
}

OFBool IODReference::readFromItem(DcmItem& item)
{
    if (m_Level >= LEVEL_PATIENT)
    {
        item.findAndGetOFString(DCM_PatientID, m_PatientID);
    }
    if (m_Level >= LEVEL_STUDY)
    {
        item.findAndGetOFString(DCM_StudyInstanceUID, m_StudyInstanceUID);
    }
    if (m_Level >= LEVEL_SERIES)
    {
        item.findAndGetOFString(DCM_SeriesInstanceUID, m_SeriesInstanceUID);
    }
    if (m_Level >= LEVEL_INSTANCE)
    {
        item.findAndGetOFString(DCM_SOPClassUID, m_SOPClassUID);
        item.findAndGetOFString(DCM_SOPInstanceUID, m_SOPInstanceUID);
    }
    return check().good();
}

OFBool IODReference::readFromFile(const OFString& filename)
{
    clear();
    DcmFileFormat ff;
    OFCondition result = ff.loadFile(filename.c_str());
    if (result.good())
    {
        DcmDataset* dset = ff.getDataset();
        return readFromItem(*dset);
    }
    return OFFalse;
}

OFCondition IODReferences::readTractographyReferencedInstanceSequence(DcmItem& source)
{
    size_t omitted          = 0;
    size_t added            = 0;
    DcmSequenceOfItems* seq = NULL;
    OFCondition result      = source.findAndGetSequence(DCM_ReferencedInstanceSequence, seq);
    if (result.good())
    {
        DcmItem* item = NULL;
        item          = OFstatic_cast(DcmItem*, seq->nextInContainer(item));
        while ((item != NULL) && result.good())
        {
            IODImageReference* ref = new IODImageReference(IODReference::LEVEL_INSTANCE);
            if (!ref)
            {
                return EC_MemoryExhausted;
            }
            item->findAndGetOFString(DCM_ReferencedSOPClassUID, ref->m_SOPClassUID);
            item->findAndGetOFString(DCM_ReferencedSOPInstanceUID, ref->m_SOPInstanceUID);
            // if present, copy referenced frame numbers
            DcmElement* elem = NULL;
            if (item->findAndGetElement(DCM_ReferencedFrameNumber, elem).good())
            {
                unsigned long vm = elem->getNumberOfValues();
                for (unsigned long f = 0; f < vm; f++)
                {
                    Sint32 val = 0;
                    if (elem->getSint32(val, f).good())
                    {
                        if (val >= 0)
                            ref->m_ReferencedFrameNumber.push_back(OFstatic_cast(Uint32, val));
                        else
                        {
                            DCMIOD_WARN("Referenced Frame Number must be > 0 but is " << val
                                                                                      << ", omitting frame reference");
                        }
                    }
                    else
                    {
                        DCMIOD_WARN("Cannot get Referenced Frame Number from position #"
                                    << f << " omitting frame reference");
                    }
                }
            }
            result = ref->check();
            if (result.good())
            {
                added++;
                m_References.push_back(ref);
            }
            else
            {
                DCMIOD_WARN("Could not read Image reference (invalid?): " << (*ref).toString());
                omitted++;
                delete ref;
                ref = NULL;
            }
            item = OFstatic_cast(DcmItem*, seq->nextInContainer(item));
        }
    }
    if ((omitted > 0) && (added > 0))
    {
        return IOD_EC_ReferencesOmitted;
    }
    else if (omitted > 0)
    {
        return IOD_EC_InvalidReference;
    }
    return EC_Normal;
}

OFCondition IODReferences::writeTractographyReferencedInstanceSequence(DcmItem& item)
{
    OFVector<IODReference*>::iterator it = m_References.begin();
    item.findAndDeleteElement(DCM_ReferencedInstanceSequence);
    DcmItem* seqItem = NULL;
    size_t numItem   = 0;
    OFCondition result;
    while ((it != m_References.end() && result.good()))
    {
        if (result.good())
        {
            result
                = item.findOrCreateSequenceItem(DCM_ReferencedInstanceSequence, seqItem, OFstatic_cast(long, numItem));
            numItem++;
        }
        if (result.good())
        {
            result = seqItem->putAndInsertOFStringArray(DCM_ReferencedSOPClassUID, (*it)->m_SOPClassUID);
        }
        if (result.good())
            result = seqItem->putAndInsertOFStringArray(DCM_ReferencedSOPInstanceUID, (*it)->m_SOPInstanceUID);
        if (result.good())
        {
            if ((*it)->getType() == IODReference::IMAGE)
            {
                if (result.good())
                {
                    IODImageReference* ref = OFstatic_cast(IODImageReference*, *it);
                    if (ref && !ref->m_ReferencedFrameNumber.empty())
                    {
                        OFStringStream oss;
                        for (size_t f = 0; f < ref->m_ReferencedFrameNumber.size(); f++)
                        {
                            oss << ref->m_ReferencedFrameNumber[f] << "\\";
                        }
                        OFSTRINGSTREAM_GETOFSTRING(oss, frameRefs)
                        // Insert all references into Referenced Frame Number attribute.
                        // Remove superfluous "\" at the end of the string.
                        result = seqItem->putAndInsertOFStringArray(DCM_ReferencedFrameNumber,
                                                                    frameRefs.substr(0, frameRefs.size() - 1));
                    }
                }
            }
        }
        it++;
    }
    if (result.bad())
    {
        item.findAndDeleteElement(DCM_ReferencedInstanceSequence);
    }
    return result;
}

OFString IODReference::toString() const
{
    char buf[400];
    sprintf(buf,
            "Study/Series/SOPClass/SOPInstance UIDs: %s/%s/%s/%s",
            m_StudyInstanceUID.c_str(),
            m_SeriesInstanceUID.c_str(),
            m_SOPClassUID.c_str(),
            m_SOPInstanceUID.c_str());
    return buf;
}

OFCondition IODReference::check() const
{
    OFCondition result;
    if (m_Level >= LEVEL_PATIENT)
    {
        if (!m_PatientID.empty())
        {
            result = DcmLongString::checkStringValue(m_PatientID);
        }
        else
        {
            result = IOD_EC_InvalidElementValue;
        }
    }
    if (result.good() && (m_Level >= LEVEL_STUDY))
    {
        if (!m_StudyInstanceUID.empty())
        {
            result = DcmUniqueIdentifier::checkStringValue(m_StudyInstanceUID, "1");
        }
        else
        {
            result = IOD_EC_InvalidElementValue;
        }
    }
    if (result.good() && (m_Level >= LEVEL_SERIES))
    {
        if (!m_SeriesInstanceUID.empty())
        {
            result = DcmUniqueIdentifier::checkStringValue(m_SeriesInstanceUID, "1");
        }
        else
        {
            result = IOD_EC_InvalidElementValue;
        }
    }
    if (result.good() && (m_Level >= LEVEL_INSTANCE))
    {
        if (!m_SOPClassUID.empty())
        {
            result = DcmUniqueIdentifier::checkStringValue(m_SOPClassUID, "1");
        }
        else
        {
            result = IOD_EC_InvalidElementValue;
        }
    }
    if (result.good() && (m_Level >= LEVEL_INSTANCE))
    {
        if (!m_SOPInstanceUID.empty())
        {
            result = DcmUniqueIdentifier::checkStringValue(m_SOPInstanceUID, "1");
        }
        else
        {
            result = IOD_EC_InvalidElementValue;
        }
    }
    return result;
}

IODReference* IODReference::clone() const
{
    IODReference* ref = new IODReference();
    if (ref)
    {
        ref->m_PatientID         = m_PatientID;
        ref->m_StudyInstanceUID  = m_StudyInstanceUID;
        ref->m_SeriesInstanceUID = m_SeriesInstanceUID;
        ref->m_SOPClassUID       = m_SOPClassUID;
        ref->m_SOPInstanceUID    = m_SOPInstanceUID;
        ref->m_Level             = m_Level;
    }
    return ref;
}

// ------------------- class IODImageReference -------------------------------

IODImageReference::IODImageReference(const IODReference::MAX_LEVEL level)
    : IODReference(level)
    , m_ReferencedFrameNumber()
{
}

IODImageReference::IODImageReference()
    : IODReference(LEVEL_INSTANCE)
    , m_ReferencedFrameNumber()
{
}

IODImageReference::IODImageReference(const OFString& patientID,
                                     const OFString& studyUID,
                                     const OFString& seriesUID,
                                     const OFString& sopInstanceUID,
                                     const OFString& sopClassUID,
                                     const OFVector<Uint32>& refFrameNumbers)
    : IODReference(LEVEL_INSTANCE)
    , m_ReferencedFrameNumber(refFrameNumbers)
{
    m_PatientID         = patientID;
    m_StudyInstanceUID  = studyUID;
    m_SeriesInstanceUID = seriesUID;
    m_SOPInstanceUID    = sopInstanceUID;
    m_SOPClassUID       = sopClassUID;
}

IODImageReference::IODImageReference(const OFString& patientID,
                                     const OFString& studyUID,
                                     const OFString& seriesUID,
                                     const OFString& sopInstanceUID,
                                     const OFString& sopClassUID)
    : m_ReferencedFrameNumber()
{
    m_PatientID         = patientID;
    m_StudyInstanceUID  = studyUID;
    m_SeriesInstanceUID = seriesUID;
    m_SOPInstanceUID    = sopInstanceUID;
    m_SOPClassUID       = sopClassUID;
}

IODReference* IODImageReference::clone() const
{
    IODImageReference* ref = new IODImageReference(m_Level);
    if (ref)
    {
        *(OFstatic_cast(IODReference*, ref)) = *this;
        ref->m_ReferencedFrameNumber         = m_ReferencedFrameNumber;
    }
    return ref;
}

void IODImageReference::clear()
{
    IODReference::clear();
    m_ReferencedFrameNumber.clear();
}

OFBool IODImageReference::readFromFile(const OFString& filename, const OFVector<Uint32> frameNumbers)
{
    clear();
    DcmFileFormat ff;
    OFCondition result = ff.loadFile(filename.c_str());
    if (result.good())
    {
        if (readFromItem(*ff.getDataset()))
        {
            m_ReferencedFrameNumber = frameNumbers;
            return OFTrue;
        }
    }
    return OFFalse;
}

// ------------------ class IODSegmentationReference ---------------------------

IODSegmentationReference::IODSegmentationReference(const IODReference::MAX_LEVEL level)
    : IODReference(level)
    , m_ReferencedSegmentNumber()
{
}

IODSegmentationReference::IODSegmentationReference()
    : IODReference(LEVEL_INSTANCE)
    , m_ReferencedSegmentNumber()
{
}

IODReference* IODSegmentationReference::clone() const
{
    IODSegmentationReference* ref = new IODSegmentationReference(m_Level);
    if (ref)
    {
        *(OFstatic_cast(IODReference*, ref)) = *this;
        ref->m_ReferencedSegmentNumber       = m_ReferencedSegmentNumber;
    }
    return ref;
}

OFBool IODSegmentationReference::readFromFile(const OFString& filename, const OFVector<Uint16> segmentNumbers)
{
    clear();
    DcmFileFormat ff;
    OFCondition result = ff.loadFile(filename.c_str());
    if (result.good())
    {
        if (readFromItem(*ff.getDataset()))
        {
            m_ReferencedSegmentNumber = segmentNumbers;
            return OFTrue;
        }
    }
    return OFFalse;
}

void IODSegmentationReference::clear()
{
    IODReference::clear();
    m_ReferencedSegmentNumber.clear();
}

IODReferences::IODReferences()
    : m_References()
{
    // nothing to do
}

IODReferences::~IODReferences()
{
    DcmIODUtil::freeContainer(m_References);
}

IODReferences::IODReferences(const IODReferences& rhs)
    : m_References()
{
    *this = rhs;
}

IODReferences& IODReferences::operator=(const IODReferences& rhs)
{
    if (&rhs == this)
        return *this;

    OFVector<IODReference*>::const_iterator ref = rhs.m_References.begin();
    while (ref != rhs.m_References.end())
    {
        m_References.push_back((*ref)->clone());
        ref++;
    }
    return *this;
}

const OFVector<IODReference*>& IODReferences::get() const
{
    return m_References;
}

size_t IODReferences::size() const
{
    return m_References.size();
}

size_t IODReferences::addFromFiles(const OFVector<OFString>& dcmFiles, const IODReference::MAX_LEVEL level)
{
    if (dcmFiles.empty())
    {
        return 0;
    }

    OFCondition result;
    OFVector<OFString>::const_iterator it = dcmFiles.begin();
    size_t count                          = 0;
    while (it != dcmFiles.end())
    {
        IODReference* ref = new IODReference(level);
        if (ref && ref->readFromFile(*it))
        {
            m_References.push_back(ref);
            count++;
        }
        else
        {
            DCMIOD_WARN("Could not add references from file " << (*it) << " (skipping)");
            delete ref;
        }
        it++;
    }
    return count;
}

void IODReferences::clearData()
{
    DcmIODUtil::freeContainer(m_References);
}
