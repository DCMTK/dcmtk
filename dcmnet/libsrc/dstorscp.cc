/*
 *
 *  Copyright (C) 2013-2017, OFFIS e.V.
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
 *  Purpose: DICOM Storage Service Class Provider (SCP)
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/dstorscp.h"
#include "dcmtk/dcmnet/diutil.h"


// constant definitions

const char *DcmStorageSCP::DEF_StandardSubdirectory  = "data";
const char *DcmStorageSCP::DEF_UndefinedSubdirectory = "undef";
const char *DcmStorageSCP::DEF_FilenameExtension     = "";


// implementation of the main interface class

DcmStorageSCP::DcmStorageSCP()
  : DcmSCP(),
    OutputDirectory(),
    StandardSubdirectory(DEF_StandardSubdirectory),
    UndefinedSubdirectory(DEF_UndefinedSubdirectory),
    FilenameExtension(DEF_FilenameExtension),
    DirectoryGeneration(DGM_Default),
    FilenameGeneration(FGM_Default),
    FilenameCreator(),
    DatasetStorage(DSM_Default)
{
    // make sure that the SCP at least supports C-ECHO with default transfer syntax
    OFList<OFString> transferSyntaxes;
    transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
    addPresentationContext(UID_VerificationSOPClass, transferSyntaxes);
}


DcmStorageSCP::~DcmStorageSCP()
{
    // clear internal state
    clear();
}


void DcmStorageSCP::clear()
{
    // DcmSCP::clear();
    OutputDirectory.clear();
    StandardSubdirectory = DEF_StandardSubdirectory;
    UndefinedSubdirectory = DEF_UndefinedSubdirectory;
    FilenameExtension = DEF_FilenameExtension;
    DirectoryGeneration = DGM_Default;
    FilenameGeneration = FGM_Default;
    DatasetStorage = DSM_Default;
}


// get methods

const OFString &DcmStorageSCP::getOutputDirectory() const
{
    return OutputDirectory;
}


DcmStorageSCP::E_DirectoryGenerationMode DcmStorageSCP::getDirectoryGenerationMode() const
{
    return DirectoryGeneration;
}


DcmStorageSCP::E_FilenameGenerationMode DcmStorageSCP::getFilenameGenerationMode() const
{
    return FilenameGeneration;
}


const OFString &DcmStorageSCP::getFilenameExtension() const
{
    return FilenameExtension;
}


DcmStorageSCP::E_DatasetStorageMode DcmStorageSCP::getDatasetStorageMode() const
{
    return DatasetStorage;
}


// set methods

OFCondition DcmStorageSCP::setOutputDirectory(const OFString &directory)
{
    OFCondition status = EC_Normal;
    if (directory.empty())
    {
        // empty directory refers to the current directory
        if (OFStandard::isWriteable("."))
            OutputDirectory.clear();
        else
            status = EC_DirectoryNotWritable;
    } else {
        // check whether given directory exists and is writable
        if (OFStandard::dirExists(directory))
        {
            if (OFStandard::isWriteable(directory))
                OFStandard::normalizeDirName(OutputDirectory, directory);
            else
                status = EC_DirectoryNotWritable;
        } else
            status = EC_DirectoryDoesNotExist;
    }
    return status;
}


void DcmStorageSCP::setDirectoryGenerationMode(const E_DirectoryGenerationMode mode)
{
    DirectoryGeneration = mode;
}


void DcmStorageSCP::setFilenameGenerationMode(const E_FilenameGenerationMode mode)
{
    FilenameGeneration = mode;
}


void DcmStorageSCP::setFilenameExtension(const OFString &extension)
{
    FilenameExtension = extension;
}


void DcmStorageSCP::setDatasetStorageMode(const E_DatasetStorageMode mode)
{
    DatasetStorage = mode;
}


// further public methods

OFCondition DcmStorageSCP::loadAssociationConfiguration(const OFString &filename,
                                                        const OFString &profile)
{
    // first, try to load the configuration file
    OFCondition status = loadAssociationCfgFile(filename);
    // and then, try to select the desired profile
    if (status.good())
        status = setAndCheckAssociationProfile(profile);
    return status;
}


// protected methods

OFCondition DcmStorageSCP::handleIncomingCommand(T_DIMSE_Message *incomingMsg,
                                                 const DcmPresentationContextInfo &presInfo)
{
    OFCondition status = EC_IllegalParameter;
    if (incomingMsg != NULL)
    {
        // check whether we've received a supported command
        if (incomingMsg->CommandField == DIMSE_C_ECHO_RQ)
        {
            // handle incoming C-ECHO request
            status = handleECHORequest(incomingMsg->msg.CEchoRQ, presInfo.presentationContextID);
        }
        else if (incomingMsg->CommandField == DIMSE_C_STORE_RQ)
        {
            // handle incoming C-STORE request
            T_DIMSE_C_StoreRQ &storeReq = incomingMsg->msg.CStoreRQ;
            Uint16 rspStatusCode = STATUS_STORE_Error_CannotUnderstand;
            // special case: bit preserving mode
            if (DatasetStorage == DGM_StoreBitPreserving)
            {
                OFString filename;
                // generate filename with full path (and create subdirectories if needed)
                status = generateSTORERequestFilename(storeReq, filename);
                if (status.good())
                {
                    if (OFStandard::fileExists(filename))
                        DCMNET_WARN("file already exists, overwriting: " << filename);
                    // receive dataset directly to file
                    status = receiveSTORERequest(storeReq, presInfo.presentationContextID, filename);
                    if (status.good())
                    {
                        // call the notification handler (default implementation outputs to the logger)
                        notifyInstanceStored(filename, storeReq.AffectedSOPClassUID, storeReq.AffectedSOPInstanceUID);
                        rspStatusCode = STATUS_Success;
                    }
                }
            } else {
                DcmFileFormat fileformat;
                DcmDataset *reqDataset = fileformat.getDataset();
                // receive dataset in memory
                status = receiveSTORERequest(storeReq, presInfo.presentationContextID, reqDataset);
                if (status.good())
                {
                    // do we need to store the received dataset at all?
                    if (DatasetStorage == DSM_Ignore)
                    {
                        // output debug message that dataset is not stored
                        DCMNET_DEBUG("received dataset is not stored since the storage mode is set to 'ignore'");
                        rspStatusCode = STATUS_Success;
                    } else {
                        // check and process C-STORE request
                        rspStatusCode = checkAndProcessSTORERequest(storeReq, fileformat);
                    }
                }
            }
            // send C-STORE response (with DIMSE status code)
            if (status.good())
                status = sendSTOREResponse(presInfo.presentationContextID, storeReq, rspStatusCode);
            else if (status == DIMSE_OUTOFRESOURCES)
            {
                // do not overwrite the previous error status
                sendSTOREResponse(presInfo.presentationContextID, storeReq, STATUS_STORE_Refused_OutOfResources);
            }
        } else {
            // unsupported command
            OFString tempStr;
            DCMNET_ERROR("cannot handle this kind of DIMSE command (0x"
                << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
                << OFstatic_cast(unsigned int, incomingMsg->CommandField)
                << "), we are a Storage SCP only");
            DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, *incomingMsg, DIMSE_INCOMING));
            // TODO: provide more information on this error?
            status = DIMSE_BADCOMMANDTYPE;
        }
    }
    return status;
}


Uint16 DcmStorageSCP::checkAndProcessSTORERequest(const T_DIMSE_C_StoreRQ &reqMessage,
                                                  DcmFileFormat &fileformat)
{
    DCMNET_DEBUG("checking and processing C-STORE request");
    Uint16 statusCode = STATUS_STORE_Error_CannotUnderstand;
    DcmDataset *dataset = fileformat.getDataset();
    // perform some basic checks on the request dataset
    if ((dataset != NULL) && !dataset->isEmpty())
    {
        OFString filename;
        OFString directoryName;
        OFString sopClassUID = reqMessage.AffectedSOPClassUID;
        OFString sopInstanceUID = reqMessage.AffectedSOPInstanceUID;
        // generate filename with full path
        OFCondition status = generateDirAndFilename(filename, directoryName, sopClassUID, sopInstanceUID, dataset);
        if (status.good())
        {
            DCMNET_DEBUG("generated filename for received object: " << filename);
            // create the output directory (if needed)
            status = OFStandard::createDirectory(directoryName, OutputDirectory /* rootDir */);
            if (status.good())
            {
                if (OFStandard::fileExists(filename))
                    DCMNET_WARN("file already exists, overwriting: " << filename);
                // store the received dataset to file (with default settings)
                status = fileformat.saveFile(filename);
                if (status.good())
                {
                    // call the notification handler (default implementation outputs to the logger)
                    notifyInstanceStored(filename, sopClassUID, sopInstanceUID, dataset);
                    statusCode = STATUS_Success;
                } else {
                    DCMNET_ERROR("cannot store received object: " << filename << ": " << status.text());
                    statusCode = STATUS_STORE_Refused_OutOfResources;

                    // delete incomplete file
                    OFStandard::deleteFile(filename);
                }
            } else {
                DCMNET_ERROR("cannot create directory for received object: " << directoryName << ": " << status.text());
                statusCode = STATUS_STORE_Refused_OutOfResources;
            }
        } else
            DCMNET_ERROR("cannot generate directory or file name for received object: " << status.text());
    }
    return statusCode;
}


OFCondition DcmStorageSCP::generateSTORERequestFilename(const T_DIMSE_C_StoreRQ &reqMessage,
                                                        OFString &filename)
{
    OFString directoryName;
    OFString sopClassUID = reqMessage.AffectedSOPClassUID;
    OFString sopInstanceUID = reqMessage.AffectedSOPInstanceUID;
    // generate filename (with full path)
    OFCondition status = generateDirAndFilename(filename, directoryName, sopClassUID, sopInstanceUID);
    if (status.good())
    {
        DCMNET_DEBUG("generated filename for object to be received: " << filename);
        // create the output directory (if needed)
        status = OFStandard::createDirectory(directoryName, OutputDirectory /* rootDir */);
        if (status.bad())
            DCMNET_ERROR("cannot create directory for object to be received: " << directoryName << ": " << status.text());
    } else
        DCMNET_ERROR("cannot generate directory or file name for object to be received: " << status.text());
    return status;
}


void DcmStorageSCP::notifyInstanceStored(const OFString &filename,
                                         const OFString & /*sopClassUID*/,
                                         const OFString & /*sopInstanceUID*/,
                                         DcmDataset * /*dataset*/) const
{
    // by default, output some useful information
    DCMNET_INFO("Stored received object to file: " << filename);
}


OFCondition DcmStorageSCP::generateDirAndFilename(OFString &filename,
                                                  OFString &directoryName,
                                                  OFString &sopClassUID,
                                                  OFString &sopInstanceUID,
                                                  DcmDataset *dataset)
{
    OFCondition status = EC_Normal;
    // get SOP class and instance UID (if not yet known from the command set)
    if (dataset != NULL)
    {
        if (sopClassUID.empty())
            dataset->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
        if (sopInstanceUID.empty())
            dataset->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
    }
    // generate directory name
    OFString generatedDirName;
    switch (DirectoryGeneration)
    {
        case DGM_NoSubdirectory:
            // do nothing (default)
            break;
        // use series date (if available) for subdirectory structure
        case DGM_SeriesDate:
            if (dataset != NULL)
            {
                OFString seriesDate;
                DcmElement *element = NULL;
                // try to get the series date from the dataset
                if (dataset->findAndGetElement(DCM_SeriesDate, element).good() && (element->ident() == EVR_DA))
                {
                    OFString dateValue;
                    DcmDate *dateElement = OFstatic_cast(DcmDate *, element);
                    // output ISO format is: YYYY-MM-DD
                    if (dateElement->getISOFormattedDate(dateValue).good() && (dateValue.length() == 10))
                    {
                        OFOStringStream stream;
                        stream << StandardSubdirectory << PATH_SEPARATOR
                            << dateValue.substr(0, 4) << PATH_SEPARATOR
                            << dateValue.substr(5 ,2) << PATH_SEPARATOR
                            << dateValue.substr(8, 2) << OFStringStream_ends;
                        OFSTRINGSTREAM_GETSTR(stream, tmpString)
                        generatedDirName = tmpString;
                        OFSTRINGSTREAM_FREESTR(tmpString);
                    }
                }
                // alternatively, if that fails, use the current system date
                if (generatedDirName.empty())
                {
                    OFString currentDate;
                    status = DcmDate::getCurrentDate(currentDate);
                    if (status.good())
                    {
                        OFOStringStream stream;
                        stream << UndefinedSubdirectory << PATH_SEPARATOR
                            << currentDate << OFStringStream_ends;
                        OFSTRINGSTREAM_GETSTR(stream, tmpString)
                        generatedDirName = tmpString;
                        OFSTRINGSTREAM_FREESTR(tmpString);
                    }
                }
            } else {
                DCMNET_DEBUG("received dataset is not available in order to determine the SeriesDate "
                    << DCM_SeriesDate << ", are you using the bit preserving mode?");
                // no DICOM dataset given, so we cannot determine the series date
                status = EC_CouldNotGenerateDirectoryName;
            }
            break;
    }
    if (status.good())
    {
        // combine the generated directory name with the output directory
        OFStandard::combineDirAndFilename(directoryName, OutputDirectory, generatedDirName);
        // generate filename
        OFString generatedFileName;
        switch (FilenameGeneration)
        {
            // use modality prefix and SOP instance UID (default)
            case FGM_SOPInstanceUID:
            {
                if (sopClassUID.empty())
                    status = NET_EC_InvalidSOPClassUID;
                else if (sopInstanceUID.empty())
                    status = NET_EC_InvalidSOPInstanceUID;
                else {
                    OFOStringStream stream;
                    stream << dcmSOPClassUIDToModality(sopClassUID.c_str(), "UNKNOWN")
                           << '.' << sopInstanceUID << FilenameExtension << OFStringStream_ends;
                    OFSTRINGSTREAM_GETSTR(stream, tmpString)
                    generatedFileName = tmpString;
                    OFSTRINGSTREAM_FREESTR(tmpString);
                    // combine the generated file name with the directory name
                    OFStandard::combineDirAndFilename(filename, directoryName, generatedFileName);
                }
                break;
            }
            // unique filename based on modality prefix and newly generated UID
            case FGM_UniqueFromNewUID:
            {
                char uidBuffer[70];
                dcmGenerateUniqueIdentifier(uidBuffer);
                OFOStringStream stream;
                stream << dcmSOPClassUIDToModality(sopClassUID.c_str(), "UNKNOWN")
                       << ".X." << uidBuffer << FilenameExtension << OFStringStream_ends;
                OFSTRINGSTREAM_GETSTR(stream, tmpString)
                generatedFileName = tmpString;
                OFSTRINGSTREAM_FREESTR(tmpString);
                // combine the generated file name with the directory name
                OFStandard::combineDirAndFilename(filename, directoryName, generatedFileName);
                break;
            }
            // unique pseudo-random filename (also checks for existing files, so we need some special handling)
            case FGM_ShortUniquePseudoRandom:
            {
                OFString prefix = dcmSOPClassUIDToModality(sopClassUID.c_str(), "XX");
                prefix += '_';
                // TODO: we might want to use a more appropriate seed value
                unsigned int seed = OFstatic_cast(unsigned int, time(NULL));
                if (!FilenameCreator.makeFilename(seed, directoryName.c_str(), prefix.c_str(), FilenameExtension.c_str(), filename))
                    status = EC_CouldNotGenerateFilename;
                break;
            }
            // use current system time and modality suffix for filename
            case FGM_CurrentSystemTime:
            {
                OFString timeStamp;
                // get the date/time as: YYYYMMDDHHMMSS.FFFFFF
                if (DcmDateTime::getCurrentDateTime(timeStamp, OFTrue /* seconds */, OFTrue /* fraction */).good())
                {
                    OFOStringStream stream;
                    stream << timeStamp << '.' << dcmSOPClassUIDToModality(sopClassUID.c_str(), "UNKNOWN")
                        << FilenameExtension << OFStringStream_ends;
                    OFSTRINGSTREAM_GETSTR(stream, tmpString)
                    generatedFileName = tmpString;
                    OFSTRINGSTREAM_FREESTR(tmpString);
                    // combine the generated file name
                    OFStandard::combineDirAndFilename(filename, directoryName, generatedFileName);
                } else
                    status = EC_CouldNotGenerateFilename;
                break;
            }

        }
    }
    return status;
}
