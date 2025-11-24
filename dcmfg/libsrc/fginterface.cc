/*
 *
 *  Copyright (C) 2015-2025, Open Connections GmbH
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
 *  Purpose: Main interface class for managing Functional Groups
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmfg/fg.h"
#include "dcmtk/dcmfg/fgfact.h" // for creating new functional groups
#include "dcmtk/dcmfg/fginterface.h"
#include "dcmtk/ofstd/ofmap.h"
#include "dcmtk/ofstd/ofthread.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcsequen.h"


// ---------------------------------- ThreadedFGWriter ----------------------------------
// This class is used to write functional groups in parallel for a group of frames, each.
FGInterface::ThreadedFGWriter::ThreadedFGWriter()
    : OFThread()
    , m_frameGroups(OFnullptr)
    , m_perFrameResultItems(OFnullptr)
    , m_startFrame(0)
    , m_endFrame(0)
    , m_errorMutex(OFnullptr)
{
}

void FGInterface::ThreadedFGWriter::init(OFVector<OFPair<Uint32, FunctionalGroups*> >* frameGroups,
                                        OFVector<DcmItem*>* perFrameResultItems,
                                        OFMutex* perFrameResultItemsMutex,
                                        const Uint32 startFrame,
                                        const Uint32 endFrame,
                                        OFConditionConst* errorOccurred,
                                        OFMutex* errorMutex)
{
    // Store the parameters in member variables
    m_frameGroups = frameGroups;
    m_perFrameResultItems = perFrameResultItems;
    m_perFrameResultItemsMutex = perFrameResultItemsMutex;
    m_startFrame = startFrame;
    m_endFrame = endFrame;
    m_errorOccurred = errorOccurred;
    m_errorMutex = errorMutex;
}


FGInterface::ThreadedFGWriter::~ThreadedFGWriter()
{
    // Nothing to do here
}

void FGInterface::ThreadedFGWriter::run()
{
    if (!m_frameGroups || !m_perFrameResultItems)
    {
        DCMFG_ERROR("ThreadedFGWriter: Not properly initialized, cannot run.");
        m_errorMutex->lock();
        *m_errorOccurred = FG_EC_ParallelProcessingFailed;
        m_errorMutex->unlock();
        return;
    }

    // Iterate over all frames and write the functional groups
    for (Uint32 idx = m_startFrame; idx < m_endFrame; ++idx)
    {
        Uint32 frameNo = m_frameGroups->at(idx).first;
        FunctionalGroups* fgPtr = m_frameGroups->at(idx).second;
        OFunique_ptr<DcmItem> perFrameItem(new DcmItem());

        FunctionalGroups::iterator groupIt = fgPtr->begin();
        while ((groupIt != fgPtr->end()))
        {
            m_errorMutex->lock();
            if (*m_errorOccurred != EC_Normal)
            {
                m_errorMutex->unlock();
                DCMFG_ERROR("Error occurred, stopping further processing.");
                return;
            }
            m_errorMutex->unlock();
            DCMFG_DEBUG("Writing per-frame group: "
                        << DcmFGTypes::FGType2OFString((*groupIt).second->getType())
                        << " for frame #" << frameNo);
            OFCondition result = (*groupIt).second->write(*perFrameItem);
            groupIt++;
            if (result.bad())
            {
                DCMFG_ERROR("Error writing functional group for frame #" << frameNo << ": " << result.text());
                m_errorMutex->lock();
                *m_errorOccurred = result.condition(); // Store the error
                m_errorMutex->unlock();
                return;
            }
        }
        // Lock the mutex before modifying shared data
        m_perFrameResultItemsMutex->lock();
        (*m_perFrameResultItems)[idx] = perFrameItem.release();
        m_perFrameResultItemsMutex->unlock();
    }
}


// ---------------------------------- ThreadedFGWReader ----------------------------------

FGInterface::ThreadedFGReader::ThreadedFGReader()
    : OFThread()
    , m_perFrameItems(OFnullptr)
    , m_frameResultGroups(OFnullptr)
    , m_frameResultGroupsMutex(OFnullptr)
    , m_startFrame(0)
    , m_endFrame(0)
    , m_errorMutex(OFnullptr)
    , m_errorOccurred(OFnullptr)
{
}


void FGInterface::ThreadedFGReader::init(OFVector<DcmItem*>* perFrameItems,
                                         OFMutex* perFrameItemsMutex,
                                         PerFrameGroups* frameResultGroups,
                                         OFMutex* frameResultGroupsMutex,
                                         Uint32 startFrame,
                                         Uint32 endFrame,
                                         OFMutex* errorMutex,
                                         OFConditionConst* errorOccurred,
                                         FGInterface* fgInterfacePtr)
{
    m_perFrameItems = perFrameItems;
    m_perFrameItemsMutex = perFrameItemsMutex;
    m_frameResultGroups = frameResultGroups;
    m_frameResultGroupsMutex = frameResultGroupsMutex;
    m_startFrame = startFrame;
    m_endFrame = endFrame;
    m_errorMutex = errorMutex;
    m_errorOccurred = errorOccurred;
    m_fgInterfacePtr = fgInterfacePtr;
}

FGInterface::ThreadedFGReader::~ThreadedFGReader()
{
    // Nothing to do here
}


void FGInterface::ThreadedFGReader::run()
{
    if (!m_perFrameItems || !m_frameResultGroups)
    {
        DCMFG_ERROR("ThreadedFGReader: Not properly initialized, cannot run.");
        m_errorMutex->lock();
        *m_errorOccurred = FG_EC_ParallelProcessingFailed;
        m_errorMutex->unlock();
        return;
    }

    // Iterate over all frames and read the functional groups
    for (Uint32 idx = m_startFrame; idx < m_endFrame; ++idx)
    {
        Uint32 frameNo = idx;
        // No need to lock for reading from m_perFrameItems, as each thread only reads its assigned range
        if (frameNo >= m_perFrameItems->size())
        {
            DCMFG_ERROR("Frame index " << frameNo << " out of bounds for per-frame items vector");
            m_errorMutex->lock();
            *m_errorOccurred = FG_EC_ParallelProcessingFailed;
            m_errorMutex->unlock();
            return;
        }
        DcmItem* perFrameItem = (*m_perFrameItems)[frameNo];
        if (!perFrameItem)
        {
            DCMFG_ERROR("No per-frame item found for frame #" << frameNo);
            m_errorMutex->lock();
            *m_errorOccurred = FG_EC_ParallelProcessingFailed; // Store the error
            m_errorMutex->unlock();
            return;
        }

        DCMFG_DEBUG("Reading per-frame groups for frame #" << frameNo);
        // Create a new FunctionalGroups object to store the read groups
        OFunique_ptr<FunctionalGroups> groupsOneFrame(new FunctionalGroups());
        if (!groupsOneFrame)
        {
            DCMFG_ERROR("Memory exhausted while creating FunctionalGroups object for frame #" << frameNo);
            m_errorMutex->lock();
            *m_errorOccurred = EC_MemoryExhausted; // Store the error
            m_errorMutex->unlock();
            return;
        }
        // Read the functional groups from the per-frame item
        // Remove parent item since otherwise the code will try to access the parent item
        // to find the Specific Character Set, and this will crash if multiple threads
        // try to access the same parent item.
        DcmSequenceOfItems* perFrameFGSeq = OFstatic_cast(DcmSequenceOfItems*, perFrameItem->getParent());
        perFrameItem->setParent(OFnullptr);
        OFCondition result = m_fgInterfacePtr->readSingleFG(*perFrameItem, *groupsOneFrame);
        perFrameItem->setParent(perFrameFGSeq); // Restore parent item
        if (result.bad())
        {
            DCMFG_ERROR("Error reading functional groups for frame #" << frameNo << ": " << result.text());
            m_errorMutex->lock();
            *m_errorOccurred = result.condition(); // Store the error
            m_errorMutex->unlock();
            return;
        }
        // Lock the mutex before modifying shared data
        m_frameResultGroupsMutex->lock();
        // cast is safe since we check earlier that it is not
        // larger than DcmFGTypes::DCMFG_MAX_FRAMES (2^32-1)
        m_frameResultGroups->insert(OFMake_pair(frameNo, groupsOneFrame.release()));
        m_frameResultGroupsMutex->unlock();
        DCMFG_DEBUG("Finished reading functional groups for frame #" << frameNo);
    }
}



// ----------------------------------- FGInterface -----------------------------------

FGInterface::FGInterface()
    : m_shared()
    , m_perFrame()
    , m_checkOnWrite(OFTrue)
    , m_numThreads(1) // Default to 1 thread
{
}

FGInterface::~FGInterface()
{
    clear();
}

void FGInterface::clear()
{
    // Clear per frame functional groups
    while (m_perFrame.size() > 0)
    {
        OFMap<Uint32, FunctionalGroups*>::iterator it = m_perFrame.begin();
        FunctionalGroups* fg                          = (*it).second;
        m_perFrame.erase(it);
        delete fg;
    }

    // Clear shared functional groups
    m_shared.clear();
}

size_t FGInterface::getNumberOfFrames()
{
    return m_perFrame.size();
}

OFCondition FGInterface::addShared(const FGBase& group)
{
    DcmFGTypes::E_FGSharedType sharedType = group.getSharedType();
    if (sharedType == DcmFGTypes::EFGS_ONLYPERFRAME)
    {
        DCMFG_ERROR("Cannot add group as shared, per DICOM, group type " << DcmFGTypes::FGType2OFString(group.getType())
                                                                         << " is always per-frame");
        return FG_EC_CouldNotAddFG;
    }

    // Delete all per frame groups of this type
    for (size_t count = 0; count < m_perFrame.size(); count++)
    {
        deletePerFrame(OFstatic_cast(Uint32, count), group.getType());
    }

    // Create copy for insertion
    FGBase* copy = group.clone();
    if (!copy)
    {
        return EC_MemoryExhausted;
    }

    // Insert shared one, replace old one if existing
    OFCondition result = insertShared(copy, OFTrue /* replace */);
    if (result.bad())
    {
        DCMFG_ERROR("Could not add shared group of type: " << DcmFGTypes::FGType2OFString(group.getType()));
        delete copy;
    }

    return result;
}

OFCondition FGInterface::addPerFrame(const Uint32 frameNo, const FGBase& group)
{
    OFCondition result                    = EC_Normal;
    DcmFGTypes::E_FGSharedType sharedType = group.getSharedType();
    if (sharedType == DcmFGTypes::EFGS_ONLYSHARED)
    {
        DCMFG_ERROR("Cannot add group as per-frame, group type " << DcmFGTypes::FGType2OFString(group.getType())
                                                                 << " is always shared");
        return FG_EC_CouldNotAddFG;
    }

    // Check whether there is already a shared group of this type.
    // If the content is equal to the given group, we re-use the shared one
    FGBase* shared = getShared(group.getType());
    // If there is a shared group
    if (shared)
    {
        // If shared has identical values as given group, nothing has to be done.
        // Else if shared group with such type exists, but content differs,
        // we must the make the existing shared FG "per-frame", i.e. distribute
        // it to all frames, and add the given group for the given frame.
        if ((*shared).compare(group) != 0)
        {
            // We need to unshare this group, i.e. distribute it to frames
            DCMFG_DEBUG("Converting shared group of type " << DcmFGTypes::FGType2OFString(
                            group.getType()) << " to per-frame, triggered by deviating per-frame insertion");
            result = convertSharedToPerFrame(group.getType());
        }
        else
        {
            DCMFG_DEBUG("Re-using shared group instead of adding per-frame for frame "
                        << frameNo << ", type " << DcmFGTypes::FGType2OFString(group.getType()));
            return EC_Normal;
        }
    }

    if (result.good())
    {
        FGBase* copy = group.clone();
        if (!copy)
        {
            return EC_MemoryExhausted;
        }
        result = insertPerFrame(frameNo, copy);
        if (result.bad())
            delete copy;
    }

    return result;
}

// Get specific functional group for a frame,
// no matter whether it is stored per frame or shared
FGBase* FGInterface::get(const Uint32 frameNo, const DcmFGTypes::E_FGType fgType)
{
    OFBool helpShared; // throw-away variable
    return get(frameNo, fgType, helpShared);
}

const FunctionalGroups* FGInterface::getPerFrame(const Uint32 frameNo) const
{
    if (frameNo > m_perFrame.size())
    {
        return NULL;
    }
    else
    {
        return (*(m_perFrame.find(frameNo))).second;
    }
}

const FunctionalGroups* FGInterface::getShared() const
{
    return &m_shared;
}

// Read enhanced multi-frame information from DICOM item, usually DcmDataset
OFCondition FGInterface::read(DcmItem& dataset)
{
    OFCondition result = EC_Normal;

    // clear any old values
    clear();

    /* read shared functional groups */
    if (result.good())
    {
        result = readSharedFG(dataset);
    }

    /* read per frame functional groups */
    if (result.good())
    {
        result = readPerFrameFG(dataset);
    }

    return result;
}

OFCondition FGInterface::readSharedFG(DcmItem& dataset)
{
    /* read shared functional groups */
    DcmSequenceOfItems* shared = NULL;
    OFCondition result         = dataset.findAndGetSequence(DCM_SharedFunctionalGroupsSequence, shared);
    if (result.bad())
    {
        DCMFG_ERROR("Could not find Shared Functional Group Sequence");
        return FG_EC_NoSharedFG;
    }

    if (shared->card() > 1)
    {
        DCMFG_WARN("More than one item in Shared Functional Group Sequence, only considering the first one");
    }
    else if (shared->card() == 0)
    {
        DCMFG_WARN("No Item in Shared Functional Group Sequence but exactly one expected");
        return FG_EC_NoSharedFG;
    }

    // get the only item of shared functional group sequence
    DcmItem* sharedFGs = shared->getItem(0);
    // read all functional groups from shared fg sequence item
    result = readSingleFG(*sharedFGs, m_shared);

    return result;
}

OFCondition FGInterface::readPerFrameFG(DcmItem& dataset)
{
    /* read per-frame functional groups */
    DcmSequenceOfItems* perFrame = NULL;
    OFCondition result           = dataset.findAndGetSequence(DCM_PerFrameFunctionalGroupsSequence, perFrame);
    if (result.bad())
    {
        DCMFG_ERROR("Could not find Per-Frame Functional Group Sequence");
        return FG_EC_NoPerFrameFG;
    }

    /* 1-n items required */
    size_t numFrames = perFrame->card();
    if (numFrames == 0)
    {
        DCMFG_WARN("No Item in Per-Frame Functional Group Sequence but exactly one or more expected");
        return FG_EC_NoPerFrameFG;
    }
    if (numFrames > DcmFGTypes::DCMFG_MAX_FRAMES)
    {
        DCMFG_ERROR("Too many items in Per-Frame Functional Group Sequence: " << numFrames
                                << ", maximum is " << DcmFGTypes::DCMFG_MAX_FRAMES);
        return FG_EC_TooManyItems;
    }

    // We want to either read sequentially or in parallel, depending on the number of threads.
    // Cast is safe since e checked range above.
    Uint32 threadsUsed = findAdequateNumberOfThreads(OFstatic_cast(Uint32, numFrames), m_numThreads);
    if (threadsUsed > 1)
    {
        result = readPerFrameFGParallel(*perFrame, m_numThreads);
    }
    else
    {
        result = readPerFrameFGSequential(*perFrame);
    }
    return EC_Normal; // for now we always return EC_Normal...
}


OFCondition FGInterface::readPerFrameFGSequential(DcmSequenceOfItems& perFrameFGSeq)
{
    /* Read functional groups for each item (one per frame) */
    OFCondition result;
    DcmItem* oneFrameItem = OFstatic_cast(DcmItem*, perFrameFGSeq.nextInContainer(NULL));
    Uint32 count          = 0;
    while (oneFrameItem != NULL)
    {
        OFunique_ptr<FunctionalGroups> perFrameGroups(new FunctionalGroups());
        if (!oneFrameItem)
        {
            DCMFG_ERROR("Could not get functional group item for frame #" << count << " (internal error)");
        }
        else if (!perFrameGroups.get())
        {
            DCMFG_ERROR("Could not create functional groups for frame #" << count << ": Memory exhausted?");
        }
        else
        {
            result = readSingleFG(*oneFrameItem, *perFrameGroups);
            if (result.good())
            {
                if (!m_perFrame.insert(OFMake_pair(count, perFrameGroups.release())).second)
                {
                    DCMFG_ERROR("Could not store functional groups for frame #" << count << " (internal error)");
                }
            }
            else
            {
                DCMFG_ERROR("Could not read functional groups for frame #" << count << ": " << result.text());
            }
        }
        oneFrameItem = OFstatic_cast(DcmItem*, perFrameFGSeq.nextInContainer(oneFrameItem));
        count++;
    }
    return EC_Normal; // for now we always return EC_Normal...
}


OFCondition FGInterface::readPerFrameFGParallel(DcmSequenceOfItems& perFrameFGSeq, const Uint32 numThreads)
{
    DCMFG_DEBUG("Reading per-frame functional groups in parallel using " << numThreads << " threads");
    // Read functional groups for each item (one per frame)
    OFCondition result;
    size_t numFrames   = perFrameFGSeq.card();

    // Create a vector to hold the functional groups for each frame and protect it with a mutex
    PerFrameGroups& perFrameResultGroups = m_perFrame;
    OFMutex perFrameResultGroupsMutex;

    // Create a vector to hold the results of all threads, protected by a mutex
    OFMutex perFrameInputMutex;
    OFVector<DcmItem*> perFrameInputItems(numFrames, NULL);
    // Fill the vector with items from the sequence
    DcmItem* oneFrameItem = OFstatic_cast(DcmItem*, perFrameFGSeq.nextInContainer(NULL));
    size_t count          = 0;
    while (oneFrameItem != NULL)
    {
        // Store the item in the vector
        perFrameInputItems[count] = oneFrameItem;
        oneFrameItem = OFstatic_cast(DcmItem*, perFrameFGSeq.nextInContainer(oneFrameItem));
        count++;
    }

    // Create a mutex for error handling
    OFConditionConst errorOccurred = EC_Normal;
    OFMutex errorMutex;

    // Create and start threads
    OFVector<ThreadedFGReader*> threads(numThreads);
    // Range check on numFrames has been performed earlier so computation should be safe
    Uint32 framesPerThread = OFstatic_cast(Uint32, (numFrames + numThreads - 1) / numThreads);
    for (Uint32 i = 0; i < numThreads; ++i)
    {
        threads[i] = new ThreadedFGReader();
        Uint32 startFrame = i * framesPerThread;
        // numFrame is range-checked earlier
        Uint32 endFrame = (startFrame + framesPerThread < numFrames) ? startFrame + framesPerThread : OFstatic_cast(Uint32, numFrames);

        threads[i]->init(&perFrameInputItems, &perFrameInputMutex,
                         &perFrameResultGroups, &perFrameResultGroupsMutex,
                         startFrame,
                         endFrame,
                         &errorMutex, &errorOccurred, this);
    }
    // Start all threads
    for (Uint32 i = 0; i < numThreads; ++i)
    {
        threads[i]->start();
    }

    // Wait for all threads to finish
    for (Uint32 i = 0; i < numThreads; ++i)
    {
        threads[i]->join();
        delete threads[i];
        threads[i] = NULL;
    }

    // Check if any thread encountered an error
    if (errorOccurred != EC_Normal)
    {
        DCMFG_ERROR("Error occurred while reading functional groups in parallel: " << OFCondition(errorOccurred).text());
        return errorOccurred;
    }

    // Store the results in m_perFrame
    return EC_Normal;
}



OFCondition FGInterface::readSingleFG(DcmItem& fgItem, FunctionalGroups& groups)
{
    OFCondition result;
    size_t card = fgItem.card();
    OFString fgname;
    for (size_t count = 0; count < card; count++)
    {
        DcmElement* elem = fgItem.getElement(OFstatic_cast(unsigned long, count));
        // TODO: non-sequence elements are not explicitly forbidden here(?), we could store them and re-store them later
        if (elem->getVR() != EVR_SQ)
        {
            DCMFG_WARN("Found non-sequence element in functional group sequence item (ignored): " << elem->getTag());
        }
        else
        {
            FGBase* fg = FGFactory::instance().create(elem->getTag());
            if (fg != NULL)
            {
                OFStringStream stream;
                stream << DcmFGTypes::tagKey2FGString(elem->getTag()) << " " << elem->getTag();
                OFSTRINGSTREAM_GETSTR(stream, tmpstr)
                fgname = tmpstr;
                OFSTRINGSTREAM_FREESTR(tmpstr)
                result = fg->read(fgItem);
                if (result.bad())
                {
                    DCMFG_WARN("Cannot read functional group: " << fgname << " (ignored)");
                }
                // we also accept groups while reading which could instantiated but not could not be read
                result = groups.insert(fg, OFTrue);
                if (result.good())
                {
                    DCMFG_DEBUG("Inserted functional group: " << fgname);
                }
                else
                {
                    DCMFG_ERROR("Could not insert functional group: " << fgname << " (internal error)");
                    delete fg;
                }
            }
            else
            {
                DCMFG_WARN("Cannot understand functional group for sequence tag: " << elem->getTag());
            }
        }
    }
    return EC_Normal; // for now we always return EC_Normal...
}

// Write enhanced multi-frame information to DICOM item, usually DcmDataset
OFCondition FGInterface::write(DcmItem& dataset)
{
    // Check data integrity of functional group macros */
    if (m_checkOnWrite)
    {
        if (!check())
            return FG_EC_CouldNotWriteFG;
    }

    // Write shared functional Groups
    OFCondition result = writeSharedFG(dataset);

    DcmItem* seqItem;
    dataset.findAndGetSequenceItem(DCM_SharedFunctionalGroupsSequence, seqItem);
    // Write per frame functional groups
    if (result.good())
        result = writePerFrameFG(dataset);

    return result;
}

FGBase* FGInterface::getShared(const DcmFGTypes::E_FGType fgType)
{
    return m_shared.find(fgType);
}

OFCondition FGInterface::insertShared(FGBase* group, const OFBool replaceExisting)
{
    return m_shared.insert(group, replaceExisting);
}

FGBase* FGInterface::getPerFrame(const Uint32 frameNo, const DcmFGTypes::E_FGType fgType)
{
    FGBase* group                                 = NULL;
    OFMap<Uint32, FunctionalGroups*>::iterator it = m_perFrame.find(frameNo);
    if (it != m_perFrame.end())
    {
        FunctionalGroups* perFrameGroups = (*it).second;
        group                            = perFrameGroups->find(fgType);
    }

    return group;
}

OFBool FGInterface::deleteShared(const DcmFGTypes::E_FGType fgType)
{
    FGBase* group = m_shared.find(fgType);
    if (group)
    {
        delete m_shared.remove(fgType);
        return OFTrue;
    }
    return OFFalse;
}

OFBool FGInterface::deletePerFrame(const Uint32 frameNo, const DcmFGTypes::E_FGType fgType)
{
    OFMap<Uint32, FunctionalGroups*>::iterator it = m_perFrame.find(frameNo);
    if (it != m_perFrame.end())
    {
        if ((*it).second)
        {
            FGBase* remove = (*it).second->remove(fgType);
            if (remove)
            {
                DCMFG_DEBUG("Deleting FG for frame " << frameNo << ", type: " << DcmFGTypes::FGType2OFString(fgType));
                delete remove;
                remove = NULL;
                return OFTrue;
            }
        }
    }
    return OFFalse;
}

size_t FGInterface::deletePerFrame(const DcmFGTypes::E_FGType fgType)
{
    size_t numDeleted      = 0;
    const size_t numFrames = m_perFrame.size();
    for (size_t frameNo = 0; frameNo < numFrames; frameNo++)
    {
        if (deletePerFrame(OFstatic_cast(Uint32, frameNo), fgType))
        {
            numDeleted++;
        }
    }
    return numDeleted;
}

size_t FGInterface::deleteFrame(const Uint32 frameNo)
{
    OFMap<Uint32, FunctionalGroups*>::iterator it = m_perFrame.find(frameNo);
    if (it != m_perFrame.end())
    {
        if ((*it).second)
        {
            FunctionalGroups::iterator fg = (*it).second->begin();
            while (fg != (*it).second->end())
            {
                delete (*fg).second;
                fg++;
            }
        }
        m_perFrame.erase(it);
    }
    return OFFalse;
}

void FGInterface::setCheckOnWrite(const OFBool doCheck)
{
    m_checkOnWrite = doCheck;
}

OFBool FGInterface::getCheckOnWrite()
{
    return m_checkOnWrite;
}

void FGInterface::setUseThreads(const Uint32 numThreads)
{
    if (numThreads > 0)
    {
        m_numThreads = numThreads;
    }
    else
    {
        m_numThreads = 1; // Fallback to single-threaded mode
    }
}

Uint32 FGInterface::getUseThreads() const
{
    return m_numThreads;
}

FunctionalGroups* FGInterface::getOrCreatePerFrameGroups(const Uint32 frameNo)
{
    OFMap<Uint32, FunctionalGroups*>::iterator it = m_perFrame.find(frameNo);
    if (it != m_perFrame.end())
        return (*it).second;

    FunctionalGroups* fg = new FunctionalGroups();
    if (fg != NULL)
    {
        if (!(m_perFrame.insert(OFMake_pair(frameNo, fg))).second)
        {
            DCMFG_ERROR("Could not insert Per-frame Functional Groups for frame " << frameNo << ": "
                                                                                  << "Internal error");
            delete fg;
            fg = NULL;
        }
    }
    else
    {
        DCMFG_ERROR("Could not create Per-frame Functional Groups for frame " << frameNo << ": "
                                                                              << "Memory exhausted");
    }

    return fg;
}


OFCondition FGInterface::writePerFrameFG(DcmItem& dataset)
{
    OFCondition result;

    DCMFG_DEBUG("Writing per-frame functional groups");
    result = dataset.insertEmptyElement(DCM_PerFrameFunctionalGroupsSequence, OFTrue); // start with empty sequence
    if (result.bad())
    {
        DCMFG_ERROR("Could not create Per-frame Functional Groups Sequence");
        return result;
    }

    /* Iterate over frames */
    size_t numFrames = m_perFrame.size();
    if (numFrames > DcmFGTypes::DCMFG_MAX_FRAMES)
    {
        DCMFG_ERROR("Too many frames in Per-frame Functional Groups: " << numFrames
                                << ", maximum is " << DcmFGTypes::DCMFG_MAX_FRAMES);
        return FG_EC_TooManyItems;
    }

    // Use parallel processing for writing functional groups, if desired and adequate.
    // Cast is safe since we checked range above.
    Uint32 threadsUsed = findAdequateNumberOfThreads(OFstatic_cast(Uint32, numFrames), m_numThreads);
    if (threadsUsed > 1)
    {
        result = writePerFrameFGParallel(dataset, m_numThreads);
    }
    else
    {
        // Fallback to sequential processing for small datasets or single-threaded environment
        result = writePerFrameFGSequential(dataset);
    }

    return result;
}

OFCondition FGInterface::writePerFrameFGSequential(DcmItem& dataset)
{
    DCMFG_DEBUG("Writing per-frame functional groups");
    OFCondition result
        = dataset.insertEmptyElement(DCM_PerFrameFunctionalGroupsSequence, OFTrue); // start with empty sequence
    if (result.bad())
    {
        DCMFG_ERROR("Could not create Per-frame Functional Groups Sequence");
        return result;
    }

    /* Iterate over frames */
    OFMap<Uint32, FunctionalGroups*>::iterator it = m_perFrame.begin();
    size_t numFrames                              = m_perFrame.size();
    for (size_t count = 0; (count < numFrames) && result.good(); count++)
    {
        DcmItem* perFrameItem = NULL;
        result                = dataset.findOrCreateSequenceItem(
            DCM_PerFrameFunctionalGroupsSequence, perFrameItem, OFstatic_cast(long, count));
        if (result.good())
        {
            /* Iterate over groups for each frame */
            FunctionalGroups::iterator groupIt = (*it).second->begin();
            while (result.good() && (groupIt != (*it).second->end()))
            {
                DCMFG_DEBUG("Writing per-frame group: " << DcmFGTypes::FGType2OFString((*groupIt).second->getType())
                                                        << " for frame #" << count);
                result = (*groupIt).second->write(*perFrameItem);
                groupIt++;
            }
        }
        else
        {
            DCMFG_ERROR("Cannot create item in Per-frame Functional Groups Sequence");
        }
        it++;
    }
    return result;
}


OFCondition FGInterface::writePerFrameFGParallel(DcmItem& dataset, const Uint32 numThreads)
{
    DCMFG_DEBUG("Writing per-frame functional groups in parallel using " << numThreads << " threads");
    OFConditionConst errorOccurred(EC_Normal);
    OFMutex errorMutex;
    OFMutex perFrameItemsMutex;

    // Prepare a vector of frame numbers and their corresponding FunctionalGroups pointers.
    // We need this to distribute the work across threads. Using the OFMap directly in threads
    // could lead to data races and inconsistencies since each thread would invalidate the iterators
    // being used in the other threads.
    OFVector<OFPair<Uint32, FunctionalGroups*> > frameGroups;
    frameGroups.reserve(m_perFrame.size());
    for (OFMap<Uint32, FunctionalGroups*>::iterator it = m_perFrame.begin(); it != m_perFrame.end(); ++it)
    {
        frameGroups.push_back(OFMake_pair((*it).first, (*it).second));
    }
    // We check earlier that number of frames is not larger than max allowed
    Uint32 numFrames = OFstatic_cast(Uint32, frameGroups.size());

    // Prepare output vector for per-frame items
    OFVector<DcmItem*> perFrameItems(numFrames, NULL);

    // Thread pool
    OFVector<ThreadedFGWriter*> threads;
    threads.reserve(numThreads);

    Uint32 framesPerThread = (numFrames + numThreads - 1) / numThreads;

    for (Uint32 threadIndex = 0; threadIndex < numThreads; ++threadIndex)
    {
        Uint32 startFrame = threadIndex * framesPerThread;
        Uint32 endFrame = (startFrame + framesPerThread < numFrames) ? startFrame + framesPerThread : numFrames;

        ThreadedFGWriter* writer = new ThreadedFGWriter();
        writer->init(&frameGroups, &perFrameItems, &perFrameItemsMutex, startFrame, endFrame, &errorOccurred, &errorMutex);
        threads.push_back(writer);
        writer->start(); // Start the thread
    }

    // Wait for all threads to complete
    OFVector<ThreadedFGWriter*>::iterator thread = threads.begin();
    while (thread != threads.end())
    {
        (*thread)->join();
        delete (*thread); // Clean up the thread object
        thread++;
    }
    threads.clear();

    if (errorOccurred == EC_Normal)
    {
        DCMFG_DEBUG("All threads completed successfully, inserting per-frame items into the dataset");
    }
    else
    {
        DCMFG_ERROR("Error occurred in one or more threads: " << OFCondition(errorOccurred).text());
        return OFCondition(errorOccurred);
    }
    // insert all per-frame items into the dataset
    OFCondition result;
    for (size_t idx = 0; idx < perFrameItems.size(); ++idx)
    {
        DcmItem* perFrameItem = perFrameItems[idx];
        Uint32 frameNo = frameGroups[idx].first;
        if (perFrameItem != NULL)
        {
            DCMFG_DEBUG("Inserting per-frame item for frame #" << frameNo);
            result = dataset.insertSequenceItem(DCM_PerFrameFunctionalGroupsSequence, perFrameItem, OFstatic_cast(long, frameNo));
            if (result.bad())
            {
                DCMFG_ERROR("Error inserting per-frame item for frame #" << frameNo << ": " << result.text());
                break;
            }
        }
        else
        {
            DCMFG_ERROR("Per-frame item for frame #" << frameNo << " is NULL, cannot insert into dataset");
        }
    }
    return result; // Return the result of the last insertion
}


OFCondition FGInterface::writeSharedFG(DcmItem& dataset)
{
    DCMFG_DEBUG("Writing shared functional groups");
    OFCondition result
        = dataset.insertEmptyElement(DCM_SharedFunctionalGroupsSequence, OFTrue); // start with empty sequence
    DcmItem* sharedFGItem = NULL;
    if (result.good())
    {
        result = dataset.findOrCreateSequenceItem(DCM_SharedFunctionalGroupsSequence, sharedFGItem, 0);
    }
    if (result.bad())
    {
        DCMFG_ERROR("Could not create Shared Functional Groups Sequence with single item");
        return result;
    }
    DCMFG_DEBUG("Writing " << m_shared.size() << "  shared functional groups to item");
    FunctionalGroups::iterator it  = m_shared.begin();
    FunctionalGroups::iterator end = m_shared.end();
    while ((it != end) && result.good())
    {
        DCMFG_DEBUG("Writing shared group: " << DcmFGTypes::FGType2OFString((*it).second->getType()));
        result = (*it).second->write(*sharedFGItem);
        it++;
    }
    return result;
}

OFCondition FGInterface::insertPerFrame(const Uint32 frameNo, FGBase* group, const OFBool replaceExisting)
{
    if (group == NULL)
        return EC_IllegalParameter;

    OFCondition result = EC_Normal;
    FGBase* existing   = getPerFrame(frameNo, group->getType());
    if (existing)
    {
        if (replaceExisting)
        {
            deletePerFrame(frameNo, group->getType());
        }
        else
        {
            result = FG_EC_DoubledFG;
        }
    }

    // Insert per-frame functional group
    if (result.good())
    {
        FunctionalGroups* perFrameGroups = getOrCreatePerFrameGroups(frameNo);
        if (perFrameGroups != NULL)
        {
            result = perFrameGroups->insert(group, replaceExisting);
        }
        else
        {
            result = FG_EC_CouldNotInsertFG;
        }
    }
    return result;
}

OFCondition FGInterface::convertSharedToPerFrame(const DcmFGTypes::E_FGType fgType)
{
    FGBase* shared = m_shared.remove(fgType);
    if (!shared)
    {
        return FG_EC_NoSuchGroup;
    }

    OFCondition result;
    size_t numFrames = m_perFrame.size();
    // Walk over all existing frames and copy "old" shared group to them
    size_t count = 0;
    for (count = 0; result.good() && (count < numFrames); count++)
    {
        FGBase* clone = shared->clone();
        if (!clone)
        {
            result = EC_MemoryExhausted;
        }
        else
        {
            result = insertPerFrame(OFstatic_cast(Uint32, count), clone, OFTrue /* replace existing */);
            if (result.bad())
            {
                delete clone;
            }
        }
    }
    return result;
}


Uint32 FGInterface::findAdequateNumberOfThreads(const Uint32 numFrames, const Uint32 userThreadSetting)
{
    // If no threads are requested, we use one thread
    if (userThreadSetting == 0)
    {
        DCMFG_DEBUG("No threads requested, using 1 thread by default.");
        return 1;
    }
    // We should have at least 10 frames per thread, lower number of threads so
    // that every thread has at least 10 frames to process.
    Uint32 framesPerThread = numFrames / userThreadSetting;
    if (framesPerThread < 10)
    {
        DCMFG_DEBUG("Requested number of threads (" << userThreadSetting
                                                    << ") is too high for the number of frames (" << numFrames
                                                    << "), reducing to a minimum of 10 frames per thread.");
        // If the user requested more threads than we can use, we limit the number of threads
        // to the number of frames divided by 10.
        Uint32 numThreads = numFrames / 10;
        if (numThreads < 1)
        {
            numThreads = 1; // At least one thread is required
        }
        return numThreads;
    }
    DCMFG_DEBUG("Using " << userThreadSetting << " threads for " << numFrames << " frames");

    // If we have enough frames per thread, we can use the user setting
    DCMFG_DEBUG("Using user-defined number of threads: " << userThreadSetting);
    return userThreadSetting;
}


FGBase* FGInterface::get(const Uint32 frameNo, const DcmFGTypes::E_FGType fgType, OFBool& isPerFrame)
{
    FGBase* group = m_shared.find(fgType);
    if (!group)
    {
        group      = getPerFrame(frameNo, fgType);
        isPerFrame = OFTrue;
    }
    else
    {
        isPerFrame = OFFalse;
    }

    return group;
}

OFBool FGInterface::check()
{
    size_t numFrames = m_perFrame.size();
    DCMFG_DEBUG("Checking functional group structure for " << numFrames << " frames");
    size_t numErrors = 0;
    for (size_t frameCount = 0; frameCount < numFrames; frameCount++)
    {
        DCMFG_TRACE("Checking frame " << frameCount << "...");
        // Most IODS require the FrameContent functional group, check "en passant"
        OFBool foundFrameContent                           = OFFalse;
        OFMap<Uint32, FunctionalGroups*>::iterator frameFG = m_perFrame.begin();
        OFMap<Uint32, FunctionalGroups*>::iterator end     = m_perFrame.end();
        while (frameFG != end)
        {
            FunctionalGroups::iterator group    = (*frameFG).second->begin();
            FunctionalGroups::iterator groupEnd = (*frameFG).second->end();
            while (group != groupEnd)
            {
                // Check that per-frame group is not a shared group at the same time
                DcmFGTypes::E_FGType groupType = group->second->getType();
                if ((groupType != DcmFGTypes::EFG_UNDEFINED) && (groupType != DcmFGTypes::EFG_UNKNOWN))
                {
                    if (m_shared.find(groupType) != NULL)
                    {
                        DCMFG_ERROR("Functional group of type " << DcmFGTypes::FGType2OFString(groupType)
                                                                << " is shared AND per-frame for frame " << frameCount);
                        numErrors++;
                    }
                    if (groupType == DcmFGTypes::EFG_FRAMECONTENT)
                        foundFrameContent = OFTrue;
                }
                // Check if "per-frame" is allowed for this group;
                if (group->second->getSharedType() == DcmFGTypes::EFGS_ONLYSHARED)
                {
                    DCMFG_ERROR("Functional group of type " << DcmFGTypes::FGType2OFString(groupType)
                                                            << " can never be per-frame, but found for frame "
                                                            << frameCount);
                    numErrors++;
                }
                group++;
            }
            frameFG++;
        }
        if (!foundFrameContent)
        {
            // TODO: This is an error for some IODs but not all (it used to be in earlier editions of the standard)
            DCMFG_WARN("Frame Content Functional group missing for frame #" << frameCount);
            // numErrors++;
        }
    }

    // Check whether shared groups contain FGs that are only permitted per-frame
    FunctionalGroups::iterator it  = m_shared.begin();
    FunctionalGroups::iterator end = m_shared.end();
    while (it != end)
    {
        if ((*it).second->getSharedType() == DcmFGTypes::EFGS_ONLYPERFRAME)
        {
            DCMFG_ERROR("Functional group of type " << DcmFGTypes::FGType2OFString((*it).second->getType())
                                                    << " used as shared functional group but must be per-frame");
            numErrors++;
        }
        it++;
    }

    if (numErrors > 0)
        return OFFalse;

    return OFTrue;
}
