/*
 *
 *  Copyright (C) 2011-2019, OFFIS e.V.
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
 *  Module:  dcmnet
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DICOM Storage Service Class User (SCU)
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofdatime.h"
#include "dcmtk/dcmdata/dccodec.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdatutl.h"
#include "dcmtk/dcmnet/dstorscu.h"
#include "dcmtk/dcmnet/diutil.h"


// these are private DIMSE status codes of the class "pending"
#define STATUS_STORE_Pending_NoPresentationContext 0xffff
#define STATUS_STORE_Pending_InvalidDatasetPointer 0xfffe


// helper functions

static const OFString &dicomToHostFilename(const OFString &dicomFilename,
                                           OFString &hostFilename,
                                           OFBool mapToLower = OFFalse)
{
    hostFilename.clear();
    const size_t length = dicomFilename.length();
    hostFilename.reserve(length);
    for (size_t i = 0; i < length; i++)
    {
        const unsigned char c = dicomFilename.at(i);
        // the PATH_SEPARATOR depends on the operating system
        if (c == '\\')
            hostFilename += PATH_SEPARATOR;
        else if (mapToLower)
            hostFilename += OFstatic_cast(char, tolower(c));
        else
            hostFilename += OFstatic_cast(char, c);
    }
    return hostFilename;
}


// implementation of the internal class/struct for a single transfer entry

DcmStorageSCU::TransferEntry::TransferEntry(const OFFilename &filename,
                                            const E_FileReadMode readMode,
                                            const OFString &sopClassUID,
                                            const OFString &sopInstanceUID,
                                            const OFString &transferSyntaxUID)
  : Filename(filename),
    FileReadMode(readMode),
    Dataset(NULL),
    DatasetHandlingMode(HM_doNothing),
    SOPClassUID(sopClassUID),
    SOPInstanceUID(sopInstanceUID),
    TransferSyntaxUID(transferSyntaxUID),
    NetworkTransferSyntax(EXS_Unknown),
    Uncompressed(OFFalse),
    DatasetSize(0),
    AssociationNumber(0),
    PresentationContextID(0),
    RequestSent(OFFalse),
    ResponseStatusCode(0)
{
    Init();
}


DcmStorageSCU::TransferEntry::TransferEntry(DcmDataset *dataset,
                                            const E_HandlingMode handlingMode,
                                            const OFString &sopClassUID,
                                            const OFString &sopInstanceUID,
                                            const OFString &transferSyntaxUID)
  : Filename(),
    FileReadMode(ERM_autoDetect),
    Dataset(dataset),
    DatasetHandlingMode(handlingMode),
    SOPClassUID(sopClassUID),
    SOPInstanceUID(sopInstanceUID),
    TransferSyntaxUID(transferSyntaxUID),
    NetworkTransferSyntax(EXS_Unknown),
    Uncompressed(OFFalse),
    DatasetSize(0),
    AssociationNumber(0),
    PresentationContextID(0),
    RequestSent(OFFalse),
    ResponseStatusCode(0)
{
    Init();
}


DcmStorageSCU::TransferEntry::~TransferEntry()
{
    if ((DatasetHandlingMode == HM_deleteAfterSend) ||
        (DatasetHandlingMode == HM_deleteAfterRemove))
    {
        delete Dataset;
    }
}


void DcmStorageSCU::TransferEntry::Init()
{
    // check whether transfer syntax is uncompressed, because we can convert between
    // these three without any loss of information
    if ((TransferSyntaxUID == UID_LittleEndianExplicitTransferSyntax) ||
        (TransferSyntaxUID == UID_BigEndianExplicitTransferSyntax) ||
        (TransferSyntaxUID == UID_LittleEndianImplicitTransferSyntax))
    {
        Uncompressed = OFTrue;
    }
}


// implementation of the main interface class

DcmStorageSCU::DcmStorageSCU()
  : DcmSCU(),
    AssociationCounter(0),
    PresentationContextCounter(0),
    DecompressionMode(DM_default),
    HaltOnInvalidFileMode(OFTrue),
    HaltOnUnsuccessfulStoreMode(OFTrue),
    AllowIllegalProposalMode(OFTrue),
    ReadFromDICOMDIRMode(OFFalse),
    MoveOriginatorAETitle(),
    MoveOriginatorMsgID(0),
    TransferList(),
    CurrentTransferEntry()
{
    CurrentTransferEntry = TransferList.begin();
}


DcmStorageSCU::~DcmStorageSCU()
{
    // clear internal state
    clear();
}


void DcmStorageSCU::clear()
{
    // DcmSCU::clear();
    AssociationCounter = 0;
    PresentationContextCounter = 0;
    DecompressionMode = DM_default;
    HaltOnInvalidFileMode = OFTrue;
    HaltOnUnsuccessfulStoreMode = OFTrue;
    AllowIllegalProposalMode = OFTrue;
    ReadFromDICOMDIRMode = OFFalse;
    MoveOriginatorAETitle.clear();
    MoveOriginatorMsgID = 0;
    removeAllSOPInstances();
}


unsigned long DcmStorageSCU::getAssociationCounter() const
{
    return AssociationCounter;
}


size_t DcmStorageSCU::getNumberOfSOPInstances() const
{
    return TransferList.size();
}


size_t DcmStorageSCU::getNumberOfSOPInstancesToBeSent() const
{
    size_t numInstances = 0;
    OFListConstIterator(TransferEntry *) transferEntry = TransferList.begin();
    OFListConstIterator(TransferEntry *) lastEntry = TransferList.end();
    // iterate over all SOP instances
    while (transferEntry != lastEntry)
    {
        // count the number of SOP instances that are not (yet) sent
        if (!(*transferEntry)->RequestSent)
            ++numInstances;
        ++transferEntry;
    }
    return numInstances;
}


DcmStorageSCU::E_DecompressionMode DcmStorageSCU::getDecompressionMode() const
{
    return DecompressionMode;
}


OFBool DcmStorageSCU::getHaltOnInvalidFileMode() const
{
    return HaltOnInvalidFileMode;
}


OFBool DcmStorageSCU::getHaltOnUnsuccessfulStoreMode() const
{
    return HaltOnUnsuccessfulStoreMode;
}


OFBool DcmStorageSCU::getAllowIllegalProposalMode() const
{
    return AllowIllegalProposalMode;
}


OFBool DcmStorageSCU::getReadFromDICOMDIRMode() const
{
    return ReadFromDICOMDIRMode;
}


OFBool DcmStorageSCU::getMOVEOriginatorInfo(OFString &aeTitle,
                                            Uint16 &messageID) const
{
    aeTitle = MoveOriginatorAETitle;
    messageID = MoveOriginatorMsgID;
    return !aeTitle.empty() || (messageID != 0);
}


void DcmStorageSCU::setDecompressionMode(const E_DecompressionMode decompressionMode)
{
    DecompressionMode = decompressionMode;
}


void DcmStorageSCU::setHaltOnInvalidFileMode(const OFBool haltMode)
{
    HaltOnInvalidFileMode = haltMode;
}


void DcmStorageSCU::setHaltOnUnsuccessfulStoreMode(const OFBool haltMode)
{
    HaltOnUnsuccessfulStoreMode = haltMode;
}


void DcmStorageSCU::setAllowIllegalProposalMode(const OFBool allowMode)
{
    AllowIllegalProposalMode = allowMode;
}


void DcmStorageSCU::setReadFromDICOMDIRMode(const OFBool readMode)
{
    ReadFromDICOMDIRMode = readMode;
}


void DcmStorageSCU::setMOVEOriginatorInfo(const OFString &aeTitle,
                                          const Uint16 messageID)
{
    MoveOriginatorAETitle = aeTitle;
    MoveOriginatorMsgID = messageID;
}


void DcmStorageSCU::resetSentStatus(const OFBool sameAssociation)
{
    if (!TransferList.empty())
    {
        CurrentTransferEntry = TransferList.begin();
        OFListConstIterator(TransferEntry *) lastEntry = TransferList.end();
        // output some debug information
        DCMNET_DEBUG("resetting sent status of " << TransferList.size() << " SOP instances "
            << "to be sent on " << (sameAssociation ? "the same" : "a new") << " association");
        // iterate over all SOP instances to be sent
        while (CurrentTransferEntry != lastEntry)
        {
            // reset flag indicating that the C-STORE request has been sent
            (*CurrentTransferEntry)->RequestSent = OFFalse;
            if (!sameAssociation)
            {
                // we also need to reset the presentation context ID in order
                // to make sure that it is not reused on a different association
                (*CurrentTransferEntry)->PresentationContextID = 0;
            }
            ++CurrentTransferEntry;
        }
    }
    // also reset the reference to the current entry
    CurrentTransferEntry = TransferList.begin();
}


void DcmStorageSCU::removeAllSOPInstances()
{
    // clear list of SOP instances to be sent (and free memory)
    CurrentTransferEntry = TransferList.begin();
    OFListConstIterator(TransferEntry *) lastEntry = TransferList.end();
    while (CurrentTransferEntry != lastEntry)
    {
        // delete the transfer entry and remove it from the list
        delete (*CurrentTransferEntry);
        CurrentTransferEntry = TransferList.erase(CurrentTransferEntry);
    }
    // make sure that the list is really empty
    TransferList.clear();
    CurrentTransferEntry = TransferList.begin();
}


OFCondition DcmStorageSCU::removeSOPInstance(const OFString &sopClassUID,
                                             const OFString &sopInstanceUID,
                                             const OFBool allOccurrences)
{
    OFCondition status = NET_EC_NoSuchSOPInstance;
    // iterate over the transfer list
    OFListIterator(TransferEntry *) transferEntry = TransferList.begin();
    OFListConstIterator(TransferEntry *) lastEntry = TransferList.end();
    while (transferEntry != lastEntry)
    {
        // check whether this is the entry we are looking for
        if (((*transferEntry)->SOPClassUID == sopClassUID) &&
            ((*transferEntry)->SOPInstanceUID == sopInstanceUID))
        {
            // make sure that the reference to the current entry remains valid
            if (CurrentTransferEntry == transferEntry)
                ++CurrentTransferEntry;
            // delete the transfer entry and remove it from the list
            delete (*transferEntry);
            transferEntry = TransferList.erase(transferEntry);
            // output some debug information
            DCMNET_DEBUG("successfully removed SOP instance " << sopInstanceUID << " from the transfer list");
            status = EC_Normal;
            // decide whether to exit the loop or to search for any duplicates
            if (!allOccurrences)
                break;
        } else {
            ++transferEntry;
        }
    }
    return status;
}


OFCondition DcmStorageSCU::addDicomFile(const OFFilename &filename,
                                        const E_FileReadMode readMode,
                                        const OFBool checkValues)
{
    OFCondition status = EC_IllegalParameter;
    // check for non-empty filename
    if (!filename.isEmpty())
    {
        DCMNET_DEBUG("adding DICOM file '" << filename << "'");
        const size_t numInstances = TransferList.size();
        OFBool isDICOMDIR = OFFalse;
        // get relevant information from the DICOM file
        OFString sopClassUID, sopInstanceUID, transferSyntaxUID;
        status = DcmDataUtil::getSOPInstanceFromFile(filename, sopClassUID, sopInstanceUID, transferSyntaxUID, readMode);
        if (status.good())
        {
            // check whether it is a DICOMDIR and should be treated in a special manner
            if (ReadFromDICOMDIRMode && (sopClassUID == UID_MediaStorageDirectoryStorage))
            {
                DCMNET_DEBUG("processing a DICOMDIR file (Media Storage Directory Storage SOP Class)");
                isDICOMDIR = OFTrue;
                // add all DICOM files referenced from the DICOMDIR
                status = addDicomFilesFromDICOMDIR(filename, readMode, checkValues);
            } else {
                // check the SOP instance before adding it
                status = checkSOPInstance(sopClassUID, sopInstanceUID, transferSyntaxUID, checkValues);
                if (status.good())
                {
                    // create a new entry ...
                    TransferEntry *entry = new TransferEntry(filename, readMode, sopClassUID, sopInstanceUID, transferSyntaxUID);
                    if (entry != NULL)
                    {
                        // ... and add it to the list of SOP instances to be transferred
                        TransferList.push_back(entry);
                    } else
                        status = EC_MemoryExhausted;
                }
            }
        }
        // finally, do some error/debug logging
        if (status.good())
        {
            if (isDICOMDIR)
            {
                // check how many instances have been added
                const size_t count = TransferList.size() - numInstances;
                if (count > 0)
                    DCMNET_DEBUG("successfully added " << count << " SOP instances from DICOMDIR to the transfer list");
            } else
                DCMNET_DEBUG("successfully added SOP instance " << sopInstanceUID << " to the transfer list");
        } else {
            if (isDICOMDIR)
                DCMNET_ERROR("cannot add DICOM files from DICOMDIR to the transfer list: " << filename << ": " << status.text());
            else
                DCMNET_ERROR("cannot add DICOM file to the transfer list: " << filename << ": " << status.text());
        }
    } else {
        DCMNET_ERROR("cannot add DICOM file with empty filename");
    }
    return status;
}


OFCondition DcmStorageSCU::addDataset(DcmDataset *dataset,
                                      const E_TransferSyntax datasetXfer,
                                      const E_HandlingMode handlingMode,
                                      const OFBool checkValues)
{
    OFCondition status = NET_EC_InvalidDatasetPointer;
    // check for invalid dataset pointer
    if (dataset != NULL)
    {
        DCMNET_DEBUG("adding DICOM dataset");
        // get relevant information from the DICOM dataset
        OFString sopClassUID, sopInstanceUID, transferSyntaxUID;
        status = DcmDataUtil::getSOPInstanceFromDataset(dataset, datasetXfer, sopClassUID, sopInstanceUID, transferSyntaxUID);
        if (status.good())
        {
            // check the SOP instance before adding it
            status = checkSOPInstance(sopClassUID, sopInstanceUID, transferSyntaxUID, checkValues);
            if (status.good())
            {
                // create a new entry ...
                TransferEntry *entry = new TransferEntry(dataset, handlingMode, sopClassUID, sopInstanceUID, transferSyntaxUID);
                if (entry != NULL)
                {
                    // ... and add it to the list of SOP instances to be transferred
                    TransferList.push_back(entry);
                } else
                    status = EC_MemoryExhausted;
            }
        }
        // finally, do some error/debug logging
        if (status.good())
        {
            DCMNET_DEBUG("successfully added SOP instance " << sopInstanceUID << " to the transfer list");
        } else {
            DCMNET_ERROR("cannot add DICOM dataset to the transfer list: " << status.text());
        }
    } else {
        DCMNET_ERROR("cannot add DICOM dataset (NULL pointer)");
    }
    return status;
}


OFCondition DcmStorageSCU::addDicomFilesFromDICOMDIR(const OFFilename &filename,
                                                     const E_FileReadMode readMode,
                                                     const OFBool checkValues)
{
    DCMNET_DEBUG("adding DICOM files referenced from '" << filename << "'");
    // read the DICOMDIR file (always require meta-header to be present)
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(filename, EXS_Unknown, EGL_noChange, DCM_MaxReadLength, ERM_fileOnly);
    if (status.good())
    {
        DcmStack stack;
        // do not use the DcmDirectoryRecord class, but access the data elements directly
        DcmDataset *dataset = fileformat.getDataset();
        OFFilename dirName;
        OFStandard::getDirNameFromPath(dirName, filename, OFFalse /* assumeDirName */);
        // iterate over all items (directory records) where ReferencedFileID is present
        while (dataset->search(DCM_ReferencedFileID, stack, ESM_afterStackTop, OFTrue).good())
        {
            // make sure that the dataset and element pointer are there
            if (stack.card() > 1)
            {
                OFFilename pathName;
                OFString fileID, sopClassUID, sopInstanceUID, transferSyntaxUID;
                // first, get the name of the referenced DICOM file
                DcmElement *element = OFstatic_cast(DcmElement *, stack.top());
                if (element != NULL)
                    element->getOFStringArray(fileID);
                // check for non-empty filename
                if (!fileID.empty())
                {
                    DCMNET_DEBUG("adding DICOM file with Referenced File ID '" << fileID << "'");
                    // then get all other relevant element values
                    DcmItem *item = OFstatic_cast(DcmItem *, stack.elem(1));
                    if (item != NULL)
                    {
                        DCMNET_DEBUG("getting SOP Class UID, SOP Instance UID and Transfer Syntax UID from DICOMDIR record");
                        item->findAndGetOFStringArray(DCM_ReferencedSOPClassUIDInFile, sopClassUID);
                        item->findAndGetOFStringArray(DCM_ReferencedSOPInstanceUIDInFile, sopInstanceUID);
                        item->findAndGetOFStringArray(DCM_ReferencedTransferSyntaxUIDInFile, transferSyntaxUID);
                    }
                    // finally, try to add this information to the transfer list
                    status = checkSOPInstance(sopClassUID, sopInstanceUID, transferSyntaxUID, checkValues);
                    if (status.good())
                    {
                        OFString tmpString;
                        const OFFilename tmpFilename(dicomToHostFilename(fileID, tmpString), pathName.usesWideChars() /*convert*/);
                        // consider that the value of ReferencedFileID is relative to the DICOMDIR
                        OFStandard::combineDirAndFilename(pathName, dirName, tmpFilename, OFTrue /* allowEmptyDirName */);
                        // create a new entry ...
                        TransferEntry *entry = new TransferEntry(pathName, readMode, sopClassUID, sopInstanceUID, transferSyntaxUID);
                        if (entry != NULL)
                        {
                            // ... and add it to the list of SOP instances to be transferred
                            TransferList.push_back(entry);
                        } else {
                            // this is considered a severe error, so always halt (see below)
                            status = EC_MemoryExhausted;
                        }
                    }
                    // finally, do some error/debug logging
                    if (status.good())
                    {
                        DCMNET_DEBUG("successfully added SOP instance " << sopInstanceUID << " to the transfer list");
                    } else {
                        DCMNET_ERROR("cannot add DICOM file from DICOMDIR to the transfer list: "
                            << (pathName.isEmpty() ? fileID : pathName) << ": " << status.text());
                    }
                } else {
                    DCMNET_ERROR("cannot add DICOM file from DICOMDIR with empty filename");
                    status = EC_InvalidFilename;
                }
                // halt on invalid file or if memory is exhausted
                if ((HaltOnInvalidFileMode && status.bad()) || (status == EC_MemoryExhausted))
                    break;
            }
        }
    }
    return status;
}


OFCondition DcmStorageSCU::addPresentationContexts()
{
    OFCondition status = EC_Normal;
    // check whether there are any instances in the transfer list
    if (!TransferList.empty())
    {
        unsigned int presContextID = 1;
        OFList<OFString> transferSyntaxes;
        // default list with all three uncompressed transfer syntaxes
        OFList<OFString> uncompressedXfers;
        uncompressedXfers.push_back(UID_LittleEndianExplicitTransferSyntax);
        uncompressedXfers.push_back(UID_BigEndianExplicitTransferSyntax);
        uncompressedXfers.push_back(UID_LittleEndianImplicitTransferSyntax);
        // make sure that the list of presentation contexts is empty before we start
        clearPresentationContexts();
        // iterate over the list of SOP instance to be transferred
        // (continue with next SOP instance if there already was a transmission)
        if (PresentationContextCounter == 0)
            CurrentTransferEntry = TransferList.begin();
        OFListIterator(TransferEntry *) transferEntry = CurrentTransferEntry;
        OFListConstIterator(TransferEntry *) lastEntry = TransferList.end();
        // make sure that the DICOM-inherent limit on the number of presentation contexts
        // is not violated (i.e. 128 presentation contexts in total, each with an odd ID)
        while ((transferEntry != lastEntry) && status.good() && (presContextID < 256))
        {
            // first, check whether SOP instance has already been sent
            if (!(*transferEntry)->RequestSent)
            {
                const char *sopClassUID = (*transferEntry)->SOPClassUID.c_str();
                DcmXfer xfer((*transferEntry)->TransferSyntaxUID.c_str());
                // output some debug information (if enabled)
                if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
                {
                    OFString sopClassName = dcmFindNameOfUID(sopClassUID, sopClassUID);
                    OFString xferName = xfer.getXferName();
                    DCMNET_DEBUG("preparing presentation context for SOP Class / Transfer Syntax: "
                        << sopClassName << " / " << xferName);
                }
                // forget about any previously used presentation context
                (*transferEntry)->PresentationContextID = 0;
                // if this is not the first entry in the _current_ list to be negotiated
                if (transferEntry != CurrentTransferEntry)
                {
                    // go back and check whether there was already a SOP instance of the same kind
                    // during this association; if so reuse the "old" presentation context ID.
                    OFListIterator(TransferEntry *) iter = transferEntry;
                    do
                    {
                        --iter;
                        // check whether SOP class UID is identical and transfer syntax is compatible
                        if (((*iter)->SOPClassUID == (*transferEntry)->SOPClassUID) &&
                            (((*iter)->Uncompressed && (*transferEntry)->Uncompressed) ||
                             ((*iter)->TransferSyntaxUID == (*transferEntry)->TransferSyntaxUID)) &&
                            (((*iter)->PresentationContextID > 0)))
                        {
                            DCMNET_DEBUG("same SOP Class UID and compatible Transfer Syntax UID as for "
                                << "another SOP instance, reusing the presentation context with ID "
                                << OFstatic_cast(unsigned int, (*iter)->PresentationContextID));
                            // if so, use the same presentation context ID as last time
                            (*transferEntry)->PresentationContextID = (*iter)->PresentationContextID;
                            break;
                        }
                    } while (iter != CurrentTransferEntry);
                }
                // a new presentation context has to be created since no appropriate one was found
                if ((*transferEntry)->PresentationContextID == 0)
                {
                    // check whether transfer syntax is known at all
                    if (xfer.getXfer() == EXS_Unknown)
                    {
                        // warn that an unknown (and therefore unsupported) transfer syntax is used
                        DCMNET_WARN("transfer syntax is unknown, trying to propose only this one: "
                            << (*transferEntry)->TransferSyntaxUID);
                        // create list of proposed transfer syntaxes
                        transferSyntaxes.clear();
                        transferSyntaxes.push_back((*transferEntry)->TransferSyntaxUID.c_str());
                        // call the inherited method from the base class doing the real work
                        status = addPresentationContext(sopClassUID, transferSyntaxes);
                    }
                    // check whether transfer syntax uses an unsupported stream compression
                    else if (xfer.getStreamCompression() == ESC_unsupported)
                    {
                        // report an error that we cannot add an appropriate proposal
                        DCMNET_ERROR("transfer syntax uses an unsupported stream compression, "
                            << "so we cannot add a presentation context for this SOP instance");
                        // return with an error code
                        status = EC_UnsupportedEncoding;
                    }
                    // check whether transfer syntax uses any kind of compression
                    else if (xfer.isEncapsulated() || (xfer.getStreamCompression() != ESC_none))
                    {
                        // create list of proposed transfer syntaxes
                        transferSyntaxes.clear();
                        transferSyntaxes.push_back((*transferEntry)->TransferSyntaxUID.c_str());
                        // check whether compression is lossless and we can decompress it
                        if (xfer.isLossless())
                        {
                            if (DecompressionMode == DM_never)
                            {
                                if (AllowIllegalProposalMode)
                                {
                                    // warn that decompression is disabled
                                    DCMNET_WARN("transfer syntax uses a lossless compression but we are not allowed to decompress it, "
                                        << "so we are not proposing any uncompressed transfer syntax" << OFendl
                                        << "  (including the Default Transfer Syntax, which might result in a violation "
                                        << "of the DICOM standard if it is not proposed in another presentation context)");
                                } else {
                                    // report an error that we cannot add an appropriate proposal
                                    DCMNET_ERROR("transfer syntax uses a lossless compression but we are not allowed to decompress it, "
                                        << "so we cannot add a presentation context for this SOP instance");
                                    // return with an error code
                                    status = EC_UnsupportedEncoding;
                                }
                            }
                            else if ((xfer.getStreamCompression() != ESC_none) /* e.g. ZIP compression */ ||
                                DcmCodecList::canChangeCoding(xfer.getXfer(), EXS_LittleEndianExplicit))
                            {
                                DCMNET_DEBUG("also propose the three uncompressed transfer syntaxes, "
                                    << "because we can decompress the SOP instance (if required)");
                                transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
                                transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
                                transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
                            } else {
                                if (AllowIllegalProposalMode)
                                {
                                    // warn that we cannot decompress the SOP instance (if required)
                                    DCMNET_WARN("transfer syntax uses a lossless compression but we cannot decompress it, "
                                        << "so we are not proposing any uncompressed transfer syntax" << OFendl
                                        << "  (including the Default Transfer Syntax, which might result in a violation "
                                        << "of the DICOM standard if it is not proposed in another presentation context)");
                                } else {
                                    // report an error that we cannot add an appropriate proposal
                                    DCMNET_ERROR("transfer syntax uses a lossless compression but we cannot decompress it, "
                                        << "so we cannot add a presentation context for this SOP instance");
                                    // return with an error code
                                    status = EC_UnsupportedEncoding;
                                }
                            }
                        } else {
                            // check whether we can decompress the lossy compression
                            if (DecompressionMode == DM_lossyAndLossless)
                            {
                                if ((xfer.getStreamCompression() != ESC_none) /* is there any lossy stream compression? */ ||
                                    DcmCodecList::canChangeCoding(xfer.getXfer(), EXS_LittleEndianExplicit))
                                {
                                    DCMNET_DEBUG("also propose the three uncompressed transfer syntaxes, "
                                        << "because we can decompress the SOP instance (if required)");
                                    transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
                                    transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
                                    transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
                                } else {
                                    // warn that we cannot decompress the SOP instance (if required)
                                    DCMNET_WARN("transfer syntax uses a lossy compression but we cannot decompress it, "
                                        << "so we are not proposing any uncompressed transfer syntax");
                                }
                            } else {
                                // report that decompression is disabled
                                DCMNET_DEBUG("transfer syntax uses a lossy compression but we are not allowed to decompress it, "
                                    << "so we are not proposing any uncompressed transfer syntax");
                            }
                        }
                        if (status.good())
                        {
                            // call the inherited method from the base class doing the real work
                            status = addPresentationContext(sopClassUID, transferSyntaxes);
                        }
                    }
                    // uncompressed case: always propose all three transfer syntaxes without compression
                    else
                    {
                        // call the inherited method from the base class doing the real work
                        status = addPresentationContext(sopClassUID, uncompressedXfers);
                    }
                    if (status.good())
                    {
                        DCMNET_DEBUG("added new presentation context with ID " << presContextID);
                        // TODO: how to _reliably_ get the ID of the current presentation context?
                        (*transferEntry)->PresentationContextID = OFstatic_cast(T_ASC_PresentationContextID, presContextID);
                        // increase ID for the next presentation context
                        presContextID += 2;
                        // count total number of presentation context
                        ++PresentationContextCounter;
                    } else {
                        if (status == EC_UnsupportedEncoding)
                        {
                            // mark the SOP instance as being sent with an error that is not defined for C-STORE;
                            // the DIMSE status indicates "pending" (see above)
                            (*transferEntry)->RequestSent = OFTrue;
                            (*transferEntry)->ResponseStatusCode = STATUS_STORE_Pending_NoPresentationContext;
                        }
                        // do not exit the loop if the error should be ignored
                        if (!HaltOnUnsuccessfulStoreMode)
                            status = EC_Normal;
                    }
                }
            }
            ++transferEntry;
        }
        // check whether any presentation contexts have been added
        if ((presContextID == 1) && status.good())
            status = NET_EC_NoPresentationContextsDefined;
    } else {
        // report an error to the caller
        status = NET_EC_NoSOPInstancesToSend;
    }
    return status;
}


OFCondition DcmStorageSCU::negotiateAssociation()
{
    OFCondition status = DcmSCU::negotiateAssociation();
    // check whether there were any acceptable presentation contexts
    if (status == NET_EC_NoAcceptablePresentationContexts)
    {
        OFListConstIterator(TransferEntry *) lastEntry = TransferList.end();
        // iterate over all SOP instances to be sent (start with the current entry)
        while (CurrentTransferEntry != lastEntry)
        {
            // check whether SOP instance has already been sent or is not meant to be sent (right now)
            if ((*CurrentTransferEntry)->RequestSent || ((*CurrentTransferEntry)->PresentationContextID == 0))
            {
                // exit the loop on the first occurrence of such a SOP instance
                break;
            }
            // mark the SOP instance as being sent with an error that is not defined for C-STORE;
            // the DIMSE status indicates "pending" (see above)
            (*CurrentTransferEntry)->RequestSent = OFTrue;
            (*CurrentTransferEntry)->ResponseStatusCode = STATUS_STORE_Pending_NoPresentationContext;
            ++CurrentTransferEntry;
        }
    }
    // increase the counter by 1 for every association
    ++AssociationCounter;
    return status;
}


OFCondition DcmStorageSCU::sendSOPInstances()
{
    OFCondition status = EC_Normal;
    // check whether there are any instances in the transfer list
    if (!TransferList.empty())
    {
        DcmDataset *dataset = NULL;
        // iterate over the list of SOP instance to be transferred
        // (continue with next SOP instance if there already was a transmission)
        OFListConstIterator(TransferEntry *) lastEntry = TransferList.end();
        while ((CurrentTransferEntry != lastEntry) && status.good())
        {
            // check whether SOP instance has already been sent
            if (!(*CurrentTransferEntry)->RequestSent)
            {
                DcmFileFormat fileformat;
                // check whether SOP instance can be sent on this association
                // (i.e. whether it has been negotiated for this association)
                if ((*CurrentTransferEntry)->PresentationContextID == 0)
                {
                    // exit the loop if this is not the case (will be sent in another association)
                    break;
                }
                // output debug information on the SOP instance to be sent
                if ((*CurrentTransferEntry)->Filename.isEmpty())
                {
                    if ((*CurrentTransferEntry)->Dataset != NULL)
                    {
                        DCMNET_DEBUG("sending SOP instance with UID: " << (*CurrentTransferEntry)->SOPInstanceUID);
                        dataset = (*CurrentTransferEntry)->Dataset;
                    } else {
                        DCMNET_ERROR("cannot send SOP instance with UID: " << (*CurrentTransferEntry)->SOPInstanceUID
                            << ": invalid dataset pointer");
                        // mark the SOP instance as being sent with an error that is not defined for C-STORE;
                        // the DIMSE status indicates "pending" (see above)
                        (*CurrentTransferEntry)->RequestSent = OFTrue;
                        (*CurrentTransferEntry)->ResponseStatusCode = STATUS_STORE_Pending_InvalidDatasetPointer;
                        // return with an error
                        status = NET_EC_InvalidDatasetPointer;
                    }
                } else {
                    DCMNET_DEBUG("sending SOP instance from file: " << (*CurrentTransferEntry)->Filename);
                    // load SOP instance from DICOM file
                    status = fileformat.loadFile((*CurrentTransferEntry)->Filename, EXS_Unknown, EGL_noChange,
                        DCM_MaxReadLength, (*CurrentTransferEntry)->FileReadMode);
                    if (status.good())
                    {
                        // do not store the dataset pointer in the transfer entry, because this pointer
                        // will become invalid for the next iteration of this while-loop.
                        dataset = fileformat.getDataset();
                    } else {
                        DCMNET_ERROR("cannot send SOP instance from file: " << (*CurrentTransferEntry)->Filename
                            << ": " << status.text());
                    }
                }
                // send SOP instance to the peer using a C-STORE request message
                if (status.good())
                {
                    // check whether UIDs in dataset are consistent with transfer list
                    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::WARN_LOG_LEVEL) && (dataset != NULL))
                    {
                        DCMNET_DEBUG("checking whether SOP Class UID and SOP Instance UID in dataset are consistent with transfer list");
                        OFString sopClassUID, sopInstanceUID, transferSyntaxUID;
                        if (DcmDataUtil::getSOPInstanceFromDataset(dataset, dataset->getOriginalXfer(), sopClassUID, sopInstanceUID, transferSyntaxUID).good())
                        {
                            // differences are usually a result of inconsistent values in meta-header and dataset
                            if ((*CurrentTransferEntry)->SOPClassUID != sopClassUID)
                            {
                                DCMNET_WARN("SOP Class UID in dataset differs from the one in the transfer list");
                                DCMNET_DEBUG("- SOP Class UID in DICOM dataset: " << sopClassUID);
                                DCMNET_DEBUG("- SOP Class UID in transfer list: " << (*CurrentTransferEntry)->SOPClassUID);
                            }
                            if ((*CurrentTransferEntry)->SOPInstanceUID != sopInstanceUID)
                            {
                                DCMNET_WARN("SOP Instance UID in dataset differs from the one in the transfer list");
                                DCMNET_DEBUG("- SOP Instance UID in DICOM dataset: " << sopInstanceUID);
                                DCMNET_DEBUG("- SOP Instance UID in transfer list: " << (*CurrentTransferEntry)->SOPInstanceUID);
                            }
                        }
                    }
                    // determine size of the dataset (in bytes) based on the original transfer syntax
                    (*CurrentTransferEntry)->DatasetSize = dataset->calcElementLength(dataset->getOriginalXfer(), g_dimse_send_sequenceType_encoding);
                    // notify user of this class that the current SOP instance is to be sent
                    notifySOPInstanceToBeSent(**CurrentTransferEntry);
                    // call the inherited method from the base class doing the real work
                    status = sendSTORERequest((*CurrentTransferEntry)->PresentationContextID, "" /* filename */,
                        dataset, (*CurrentTransferEntry)->ResponseStatusCode,
                        MoveOriginatorAETitle, MoveOriginatorMsgID);
                    // store some further information (even in case of error)
                    (*CurrentTransferEntry)->AssociationNumber = AssociationCounter;
                    (*CurrentTransferEntry)->NetworkTransferSyntax = dataset->getCurrentXfer();
                }
                // if it was successful (i.e. even if DIMSE status is not 0x0000 = success) ...
                if (status.good())
                {
                    // ... remember that this SOP instance has already been sent
                    (*CurrentTransferEntry)->RequestSent = OFTrue;
                    // check whether we need to compact or delete the dataset
                    if ((*CurrentTransferEntry)->Filename.isEmpty() && ((*CurrentTransferEntry)->Dataset != NULL))
                    {
                        if ((*CurrentTransferEntry)->DatasetHandlingMode == HM_compactAfterSend)
                        {
                            DCMNET_DEBUG("compacting dataset after successful send");
                            (*CurrentTransferEntry)->Dataset->compactElements(256 /* maxLength */);
                        }
                        else if ((*CurrentTransferEntry)->DatasetHandlingMode == HM_deleteAfterSend)
                        {
                            DCMNET_DEBUG("deleting dataset after successful send");
                            delete (*CurrentTransferEntry)->Dataset;
                            // forget about this dataset (e.g. in order to avoid double deletion)
                            (*CurrentTransferEntry)->Dataset = NULL;
                        }
                    }
                } else {
                    // if the SOP instance could not be sent because no acceptable presentation context was found
                    if (status == DIMSE_NOVALIDPRESENTATIONCONTEXTID)
                    {
                        // mark the SOP instance as being sent with an error that is not defined for C-STORE;
                        // the DIMSE status indicates "pending" (see above)
                        (*CurrentTransferEntry)->RequestSent = OFTrue;
                        (*CurrentTransferEntry)->ResponseStatusCode = STATUS_STORE_Pending_NoPresentationContext;
                    }
                    // do not exit the loop if the error should be ignored
                    if (!HaltOnUnsuccessfulStoreMode && (status != DIMSE_ILLEGALASSOCIATION))
                        status = EC_Normal;
                }
                // notify user of this class that the current SOP instance has been processed
                notifySOPInstanceSent(**CurrentTransferEntry);
            }
            ++CurrentTransferEntry;
            // check whether the sending process should be stopped
            if (shouldStopAfterCurrentSOPInstance())
                break;
        }
    } else {
        // report an error to the caller
        status = NET_EC_NoSOPInstancesToSend;
    }
    return status;
}


void DcmStorageSCU::notifySOPInstanceToBeSent(const TransferEntry & /*transferEntry*/)
{
    // do nothing in the default implementation
}


void DcmStorageSCU::notifySOPInstanceSent(const TransferEntry & /*transferEntry*/)
{
    // do nothing in the default implementation
}


OFBool DcmStorageSCU::shouldStopAfterCurrentSOPInstance()
{
    // should always continue in the default implementation
    return OFFalse;
}


void DcmStorageSCU::getStatusSummary(OFString &summary) const
{
    OFOStringStream stream;
    const size_t numInstances = TransferList.size();
    // output some status information on the overall sending process
    stream << "Status Summary" << OFendl << "--------------";
    stream << OFendl << "Number of associations   : " << AssociationCounter;
    stream << OFendl << "Number of pres. contexts : " << PresentationContextCounter;
    stream << OFendl << "Number of SOP instances  : " << numInstances;
    // determine details on the sent status
    size_t numSent = 0;
    size_t numRefused = 0;
    size_t numError = 0;
    size_t numWarning = 0;
    size_t numSuccess = 0;
    size_t numUnknown = 0;
    size_t numPending = 0;
    size_t numInvalid = 0;
    OFListConstIterator(TransferEntry *) transferEntry = TransferList.begin();
    OFListConstIterator(TransferEntry *) lastEntry = TransferList.end();
    while (transferEntry != lastEntry)
    {
        // check whether instance has been sent
        if ((*transferEntry)->RequestSent)
        {
            ++numSent;
            // check DIMSE status
            const Uint16 rspStatus = (*transferEntry)->ResponseStatusCode;
            if (((rspStatus & 0xff00) == STATUS_STORE_Refused_OutOfResources) ||
                ((rspStatus & 0xff00) == STATUS_STORE_Refused_SOPClassNotSupported))
            {
                ++numRefused;
            }
            else if (((rspStatus & 0xff00) == STATUS_STORE_Error_DataSetDoesNotMatchSOPClass) ||
                     ((rspStatus & 0xf000) == STATUS_STORE_Error_CannotUnderstand))
            {
                ++numError;
            }
            else if ((rspStatus == STATUS_STORE_Warning_CoercionOfDataElements) ||
                     (rspStatus == STATUS_STORE_Warning_DataSetDoesNotMatchSOPClass) ||
                     (rspStatus == STATUS_STORE_Warning_ElementsDiscarded) ||
                      DICOM_WARNING_STATUS(rspStatus) /* not really needed but ... */)
            {
                ++numWarning;
            }
            else if (rspStatus == STATUS_Success)
            {
                ++numSuccess;
            }
            else if (rspStatus == STATUS_STORE_Pending_NoPresentationContext)
            {
                --numSent;
                ++numPending;
            }
            else if (rspStatus == STATUS_STORE_Pending_InvalidDatasetPointer)
            {
                --numSent;
                ++numInvalid;
            } else {
                /* any other (unknown/unsupported) DIMSE status code */
                ++numUnknown;
            }
        }
        ++transferEntry;
    }
    if (numSent > 0)
        stream << OFendl << "- sent to the peer       : " << numSent;
    if (numSuccess > 0)
        stream << OFendl << "  * with status SUCCESS  : " << numSuccess;
    if (numWarning > 0)
        stream << OFendl << "  * with status WARNING  : " << numWarning;
    if (numError > 0)
        stream << OFendl << "  * with status ERROR    : " << numError;
    if (numRefused > 0)
        stream << OFendl << "  * with status REFUSED  : " << numRefused;
    if (numUnknown > 0)
        stream << OFendl << "  * with unknown status  : " << numUnknown;
    if (numSent < numInstances)
        stream << OFendl << "- NOT sent to the peer   : " << (numInstances - numSent);
    if (numPending > 0)
        stream << OFendl << "  * no acceptable pres.  : " << numPending;
    if (numInvalid > 0)
        stream << OFendl << "  * invalid dataset ptr. : " << numInvalid;
    stream << OFStringStream_ends;
    // convert stream to a string
    OFSTRINGSTREAM_GETSTR(stream, tmpString);
    summary = tmpString;
    OFSTRINGSTREAM_FREESTR(tmpString)
}


OFCondition DcmStorageSCU::createReportFile(const OFString &filename) const
{
    OFCondition status = EC_Normal;
    if (!filename.empty())
    {
        DCMNET_INFO("creating detailed report on the transfer ...");
        DCMNET_DEBUG("creating report file '" << filename << "'");
        // check whether file already exists
        if (OFStandard::fileExists(filename))
            DCMNET_WARN("file '" << filename << "' already exists, overwriting");
        STD_NAMESPACE ofstream stream(filename.c_str());
        if (stream)
        {
            // create the textual output
            stream << "Detailed Report on the Transfer of Instances" << OFendl;
            stream << "============================================" << OFendl << OFendl;
            stream << "Communication Peer : " << getPeerHostName() << ":" << getPeerPort() << OFendl;
            stream << "AE Titles used     : " << getAETitle() << " -> " << getPeerAETitle() << OFendl;
            stream << "Current Date/Time  : " << OFDateTime::getCurrentDateTime() << OFendl;
            stream << OFendl << OFendl;
            // output details on each instance in the transfer list
            unsigned long counter = 0;
            OFListConstIterator(TransferEntry *) transferEntry = TransferList.begin();
            OFListConstIterator(TransferEntry *) lastEntry = TransferList.end();
            while (transferEntry != lastEntry)
            {
                DcmXfer orgXfer((*transferEntry)->TransferSyntaxUID.c_str());
                stream << "Number        : " << (++counter) << OFendl;
                if (!(*transferEntry)->Filename.isEmpty())
                    stream << "Filename      : " << (*transferEntry)->Filename << OFendl;
                stream << "SOP Instance  : " << (*transferEntry)->SOPInstanceUID << OFendl;
                stream << "SOP Class     : " << (*transferEntry)->SOPClassUID << " = "
                    << dcmFindNameOfUID((*transferEntry)->SOPClassUID.c_str(), "unknown") << OFendl;
                stream << "Original Xfer : " << (*transferEntry)->TransferSyntaxUID << " = " << orgXfer.getXferName() << OFendl;
                stream << "Dataset Size  : " << (*transferEntry)->DatasetSize << " bytes" << OFendl;
                stream << "Association   : " << (*transferEntry)->AssociationNumber << OFendl;
                stream << "Pres. Context : " << OFstatic_cast(unsigned int, (*transferEntry)->PresentationContextID) << OFendl;
                stream << "Network Xfer  : ";
                if ((*transferEntry)->NetworkTransferSyntax != EXS_Unknown)
                {
                    DcmXfer netXfer((*transferEntry)->NetworkTransferSyntax);
                    stream << netXfer.getXferID() << " = " << netXfer.getXferName();
                }
                stream << OFendl;
                const Uint16 rspStatus = (*transferEntry)->ResponseStatusCode;
                stream << "DIMSE Status  : ";
                if ((*transferEntry)->RequestSent)
                {
                    if (rspStatus == STATUS_STORE_Pending_NoPresentationContext)
                        stream << "<no acceptable presentation context>";
                    else if (rspStatus == STATUS_STORE_Pending_InvalidDatasetPointer)
                        stream << "<invalid dataset pointer>";
                    else {
                        stream << "0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
                            << rspStatus << " (" << DU_cstoreStatusString(rspStatus) << ")" << STD_NAMESPACE dec;
                    }
                } else {
                    stream << "<not sent>";
                }
                stream << OFendl << OFendl;
                ++transferEntry;
            }
            // output some status information on the overall sending process
            OFString summaryText;
            getStatusSummary(summaryText);
            stream << OFendl << summaryText;
        } else {
            // some error occurred while creating the file
            DCMNET_ERROR("cannot write detailed report to file '" << filename << "'");
            status = makeOFCondition(OFM_dcmdata, 19 /* file write */, OF_error, "Cannot create report file");
        }
    } else {
        // report an error to the caller
        status = EC_IllegalParameter;
    }
    return status;
}


OFCondition DcmStorageSCU::checkSOPInstance(const OFString &sopClassUID,
                                            const OFString &sopInstanceUID,
                                            const OFString &transferSyntaxUID,
                                            const OFBool checkValues)
{
    OFCondition status = EC_Normal;
    // check SOP class UID for validity
    if (sopClassUID.empty())
    {
        DCMNET_DEBUG("empty or missing SOP Class UID");
        status = NET_EC_InvalidSOPClassUID;
    }
    // check value representation and value multiplicity (if not disabled)
    if (status.good() && checkValues)
    {
        status = DcmUniqueIdentifier::checkStringValue(sopClassUID, "1" /* vm */);
        if (status.good())
        {
            // in addition, check whether it is a known storage SOP class
            if (!dcmIsaStorageSOPClassUID(sopClassUID.c_str(), ESSC_All))
            {
                // check whether the DICOM standard prefix for storage UIDs is used
                if (sopClassUID.compare(0, 23, "1.2.840.10008.5.1.4.1.1") == 0)
                {
                    DCMNET_DEBUG("unknown storage SOP class (with standard UID prefix): " << sopClassUID);
                }
                // check whether the DICOM standard prefix for UIDs is used
                else if (sopClassUID.compare(0, 13, "1.2.840.10008") == 0)
                {
                    // try to find the human-readable name of this UID
                    const char *uidName = dcmFindNameOfUID(sopClassUID.c_str(), NULL /* defaultValue */);
                    if (uidName != NULL)
                    {
                        DCMNET_DEBUG("unexpected standard UID (no storage SOP class): " << sopClassUID
                            << " (" << uidName << ")");
                        status = NET_EC_InvalidSOPClassUID;
                    } else {
                        DCMNET_DEBUG("unknown standard UID (probably no storage SOP class): " << sopClassUID);
                        status = NET_EC_UnknownStorageSOPClass;
                    }
                } else {
                    // do not reject supposed private SOP classes
                    DCMNET_DEBUG("unknown UID (possibly private storage SOP class): " << sopClassUID);
                }
            }
        } else {
            DCMNET_DEBUG("invalid SOP Class UID " << sopClassUID << ": " << status.text());
            status = NET_EC_InvalidSOPClassUID;
        }
    }
    // check SOP instance UID for validity
    if (status.good())
    {
        if (sopInstanceUID.empty())
        {
            DCMNET_DEBUG("empty or missing SOP Instance UID");
            status = NET_EC_InvalidSOPInstanceUID;
        }
        // check value representation and value multiplicity (if not disabled)
        if (status.good() && checkValues)
        {
            status = DcmUniqueIdentifier::checkStringValue(sopInstanceUID, "1" /* vm */);
            if (status.bad())
            {
                DCMNET_DEBUG("invalid SOP Instance UID " << sopInstanceUID << ": " << status.text());
                status = NET_EC_InvalidSOPInstanceUID;
            }
        }
    }
    // check transfer syntax UID for validity
    if (status.good())
    {
        if (transferSyntaxUID.empty())
        {
            DCMNET_DEBUG("empty or missing Transfer Syntax UID");
            status = NET_EC_InvalidTransferSyntaxUID;
        }
        // check value representation and value multiplicity (if not disabled)
        if (status.good() && checkValues)
        {
            status = DcmUniqueIdentifier::checkStringValue(transferSyntaxUID, "1" /* vm */);
            if (status.good())
            {
                DcmXfer xfer(transferSyntaxUID.c_str());
                // in addition, check whether it is a retired or unknown transfer syntax
                if (xfer.isRetired())
                {
                    DCMNET_DEBUG("retired transfer syntax: " << transferSyntaxUID);
                }
                else if (xfer.getXfer() == EXS_Unknown)
                {
                    // check whether the DICOM standard prefix for transfer syntax UIDs is used
                    if (sopClassUID.compare(0, 17, "1.2.840.10008.1.2") == 0)
                    {
                        DCMNET_DEBUG("unknown standard transfer syntax (with standard UID prefix): " << transferSyntaxUID);
                    }
                    // check whether the DICOM standard prefix for UIDs is used
                    else if (sopClassUID.compare(0, 13, "1.2.840.10008") == 0)
                    {
                        DCMNET_DEBUG("unknown standard UID (probably no transfer syntax): " << transferSyntaxUID);
                    } else {
                        DCMNET_DEBUG("unknown UID (possibly private transfer syntax): " << transferSyntaxUID);
                    }
                    // by default, we are not supporting unknown transfer syntaxes
                    status = NET_EC_UnknownTransferSyntax;
                }
            } else {
                DCMNET_DEBUG("invalid Transfer Syntax UID " << transferSyntaxUID << ": " << status.text());
                status = NET_EC_InvalidTransferSyntaxUID;
            }
        }
    }
    return status;
}
