/*
 *
 *  Copyright (C) 1994-2022, OFFIS e.V.
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
 *  Author:  Marco Eichelberg / Andrew Hewett
 *
 *  Purpose: Classes for Query/Retrieve Service Class User (C-FIND operation)
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/dfindscu.h"

#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcpath.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofstream.h"

/* ---------------- static functions ---------------- */

#define OFFIS_CONSOLE_APPLICATION "findscu"

static void progressCallback(
    void *callbackData,
    T_DIMSE_C_FindRQ *request,
    int responseCount,
    T_DIMSE_C_FindRSP *rsp,
    DcmDataset *responseIdentifiers)
{
    DcmFindSCUCallback *callback = OFreinterpret_cast(DcmFindSCUCallback *, callbackData);
    if (callback) callback->callback(request, responseCount, rsp, responseIdentifiers);
}

/* ---------------- class DcmFindSCUCallback ---------------- */

DcmFindSCUCallback::DcmFindSCUCallback()
: assoc_(NULL)
, presId_(0)
{
}

void DcmFindSCUCallback::setAssociation(T_ASC_Association *assoc)
{
    assoc_ = assoc;
}

void DcmFindSCUCallback::setPresentationContextID(T_ASC_PresentationContextID presId)
{
    presId_ = presId;
}

/* ---------------- class DcmFindSCUCallback ---------------- */

DcmFindSCUDefaultCallback::DcmFindSCUDefaultCallback(
    DcmFindSCUExtractMode extractResponses,
    int cancelAfterNResponses,
    const char *outputDirectory,
    STD_NAMESPACE ofstream *outputStream,
    const unsigned int limitOutput)
: DcmFindSCUCallback()
, extractResponses_(extractResponses)
, cancelAfterNResponses_(cancelAfterNResponses)
, limitOutput_(limitOutput)
, outputDirectory_(OFSTRING_GUARD(outputDirectory))
, outputStream_(outputStream)
{
}

void DcmFindSCUDefaultCallback::callback(
    T_DIMSE_C_FindRQ *request,
    int &responseCount,
    T_DIMSE_C_FindRSP *rsp,
    DcmDataset *responseIdentifiers)
{
    OFLogger rspLogger = OFLog::getLogger(DCMNET_LOGGER_NAME ".responses");
    /* check whether debug mode is enabled */
    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
        OFString temp_str;
        DCMNET_INFO("Received Find Response " << responseCount);
        DCMNET_DEBUG(DIMSE_dumpMessage(temp_str, *rsp, DIMSE_INCOMING));
        if (rspLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL)) {
            DCMNET_DEBUG("Response Identifiers:" << OFendl << DcmObject::PrintHelper(*responseIdentifiers));
        }
    }
    /* otherwise check whether special response logger is enabled */
    else if (rspLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL))
    {
        OFLOG_INFO(rspLogger, "---------------------------");
        OFLOG_INFO(rspLogger, "Find Response: " << responseCount << " (" << DU_cfindStatusString(rsp->DimseStatus) << ")");
        OFLOG_INFO(rspLogger, DcmObject::PrintHelper(*responseIdentifiers));
    } else {
        DCMNET_INFO("Received Find Response " << responseCount << " (" << DU_cfindStatusString(rsp->DimseStatus) << ")");
    }
    /* should we extract the response dataset to file? */
    if (extractResponses_ != FEM_none)
    {
        /* check for upper limit of extracted responses */
        if ((limitOutput_ > 0) && (OFstatic_cast(unsigned int, responseCount) > limitOutput_))
        {
            /* output warning message on first response after limit has been reached */
            if (OFstatic_cast(unsigned int, responseCount) == limitOutput_ + 1)
                DCMNET_INFO("Maximum number of responses already written to file ... not writing this and all following responses to file");
            else
                DCMNET_DEBUG("Maximum number of responses already written to file ... not writing this response to file");
        } else {
            /* should we extract the response dataset to a DICOM file? */
            if (extractResponses_ == FEM_dicomFile)
            {
                OFString outputFilename;
                char rspIdsFileName[32];
                sprintf(rspIdsFileName, "rsp%04d.dcm", responseCount);
                OFStandard::combineDirAndFilename(outputFilename, outputDirectory_, rspIdsFileName, OFTrue /*allowEmptyDirName*/);
                DCMNET_INFO("Writing response dataset to file: " << outputFilename);
                DcmFindSCU::writeToFile(outputFilename.c_str(), responseIdentifiers);
            }
            /* ... or to an XML file? */
            else if (extractResponses_ == FEM_xmlFile)
            {
                OFString outputFilename;
                char rspIdsFileName[32];
                sprintf(rspIdsFileName, "rsp%04d.xml", responseCount);
                OFStandard::combineDirAndFilename(outputFilename, outputDirectory_, rspIdsFileName, OFTrue /*allowEmptyDirName*/);
                DCMNET_INFO("Writing response dataset to file: " << outputFilename);
                DcmFindSCU::writeToXMLFile(outputFilename.c_str(), responseIdentifiers);
            }
            /* ... or all responses to a single XML file? */
            else if (extractResponses_ == FEM_singleXMLFile)
            {
                if (outputStream_ != NULL)
                {
                    OFCondition cond = EC_Normal;
                    size_t writeFlags = 0;
                    OFString csetString;
                    DCMNET_DEBUG("Writing response dataset to XML file");
                    /* expect that (0008,0005) is set if extended characters are used */
                    if (responseIdentifiers->findAndGetOFStringArray(DCM_SpecificCharacterSet, csetString).good())
                    {
                        /* for ASCII and UTF-8, there is no need to convert the character strings */
                        if (!csetString.empty() && (csetString != "ISO_IR 192"))
                        {
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
                            DCMNET_DEBUG("Converting all element values that are affected by SpecificCharacterSet (0008,0005) to UTF-8");
                            cond = responseIdentifiers->convertToUTF8();
#else
                            if (responseIdentifiers->containsExtendedCharacters(OFFalse /*checkAllStrings*/))
                            {
                                DCMNET_WARN("No support for character set conversion available ... quoting non-ASCII characters");
                                /* make sure that non-ASCII characters are quoted appropriately */
                                writeFlags |= DCMTypes::XF_convertNonASCII;
                            } else {
                                DCMNET_DEBUG("No support for character set conversion available");
                            }
#endif
                        }
                    }
                    /* write response dataset to XML file */
                    if (cond.good())
                        cond = responseIdentifiers->writeXML(*outputStream_, writeFlags);
                    if (cond.bad())
                        DCMNET_ERROR("Writing XML file: " << cond.text());
                }
            }
        }
    }

    /* should we send a cancel back ?? */
    if (cancelAfterNResponses_ == responseCount)
    {
        DCMNET_INFO("Sending Cancel Request (MsgID " << request->MessageID
            << ", PresID " << OFstatic_cast(unsigned int, presId_) << ")");
        OFCondition cond = DIMSE_sendCancelRequest(assoc_, presId_, request->MessageID);
        if (cond.bad())
        {
            OFString temp_str;
            DCMNET_ERROR("Cancel Request Failed: " << DimseCondition::dump(temp_str, cond));
        }
    }
}

/* ---------------- class DcmFindSCU ---------------- */


DcmFindSCU::DcmFindSCU()
: net_(NULL),
  outputResponseLimit_(0)
{
}

DcmFindSCU::~DcmFindSCU()
{
    dropNetwork();
}

OFCondition DcmFindSCU::setOutputResponseLimit(const unsigned int limit)
{
    outputResponseLimit_ = limit;
    return EC_Normal;
}

OFCondition DcmFindSCU::initializeNetwork(int acse_timeout)
{
    return ASC_initializeNetwork(NET_REQUESTOR, 0, acse_timeout, &net_);
}

OFCondition DcmFindSCU::setTransportLayer(DcmTransportLayer *tLayer)
{
    return ASC_setTransportLayer(net_, tLayer, 0);
}

OFCondition DcmFindSCU::dropNetwork()
{
    if (net_) return ASC_dropNetwork(&net_); else return EC_Normal;
}

OFCondition DcmFindSCU::performQuery(
    const char *peer,
    unsigned int port,
    const char *ourTitle,
    const char *peerTitle,
    const char *abstractSyntax,
    E_TransferSyntax preferredTransferSyntax,
    T_DIMSE_BlockingMode blockMode,
    int dimse_timeout,
    Uint32 maxReceivePDULength,
    OFBool secureConnection,
    OFBool abortAssociation,
    unsigned int repeatCount,
    DcmFindSCUExtractMode extractResponses,
    int cancelAfterNResponses,
    OFList<OFString> *overrideKeys,
    DcmFindSCUCallback *callback,
    OFList<OFString> *fileNameList,
    const char *outputDirectory,
    const char *extractFilename)
{
    T_ASC_Association *assoc = NULL;
    T_ASC_Parameters *params = NULL;
    DIC_NODENAME peerHost;
    OFString temp_str;
    OFString outputFilename;
    STD_NAMESPACE ofstream outputStream;
    int responseCount = 0;

    /* check input parameters first */
    if (extractResponses == FEM_singleXMLFile)
    {
        OFStandard::combineDirAndFilename(outputFilename, OFSTRING_GUARD(outputDirectory), OFSTRING_GUARD(extractFilename), OFTrue /*allowEmptyDirName*/);
        if (outputFilename.empty())
        {
            DCMNET_ERROR("Cannot create response file with empty filename");
            return EC_InvalidFilename;
        }
    }

    /* initialize association parameters, i.e. create an instance of T_ASC_Parameters*. */
    OFCondition cond = ASC_createAssociationParameters(&params, maxReceivePDULength, dcmConnectionTimeout.get());
    if (cond.bad())
    {
        DCMNET_ERROR("Creating Association Parameters Failed: " << DimseCondition::dump(temp_str, cond));
        return cond;
    }

    /* sets this application's title and the called application's title in the params */
    /* structure. The default values to be set here are "FINDSCU" and "ANY-SCP". */
    ASC_setAPTitles(params, ourTitle, peerTitle, NULL);

    /* Set the transport layer type (type of network connection) in the params */
    /* structure. The default is an insecure connection; where OpenSSL is  */
    /* available the user is able to request an encrypted,secure connection. */
    cond = ASC_setTransportLayerType(params, secureConnection);
    if (cond.bad())
    {
        DCMNET_ERROR("Setting Transport Layer Type Failed: " << DimseCondition::dump(temp_str, cond));
        (void) ASC_destroyAssociationParameters(&params);
        return cond;
    }

    /* Figure out the presentation addresses and copy the */
    /* corresponding values into the association parameters.*/
    sprintf(peerHost, "%s:%d", peer, OFstatic_cast(int, port));
    ASC_setPresentationAddresses(params, OFStandard::getHostName().c_str(), peerHost);

    /* Set the presentation contexts which will be negotiated */
    /* when the network connection will be established */
    cond = addPresentationContext(params, abstractSyntax, preferredTransferSyntax);
    if (cond.bad())
    {
        DCMNET_ERROR("Adding Presentation Contexts Failed: " << DimseCondition::dump(temp_str, cond));
        (void) ASC_destroyAssociationParameters(&params);
        return cond;
    }

    /* dump presentation contexts if required */
    DCMNET_DEBUG("Request Parameters:" << OFendl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_RQ));

    /* create association, i.e. try to establish a network connection to another */
    /* DICOM application. This call creates an instance of T_ASC_Association*. */
    DCMNET_INFO("Requesting Association");

    cond = ASC_requestAssociation(net_, params, &assoc);

    if (cond.bad())
    {
        if (cond == DUL_ASSOCIATIONREJECTED)
        {
            T_ASC_RejectParameters rej;
            ASC_getRejectParameters(params, &rej);

            DCMNET_ERROR("Association Rejected:" << OFendl << ASC_printRejectParameters(temp_str, &rej));
            (void) ASC_destroyAssociation(&assoc); // this also destroys the T_ASC_Parameters structure
            return cond;
        } else {
            DCMNET_ERROR("Association Request Failed: " << DimseCondition::dump(temp_str, cond));
            (void) ASC_destroyAssociation(&assoc); // this also destroys the T_ASC_Parameters structure
            return cond;
        }
    }

    /* dump the presentation contexts which have been accepted/refused */
    DCMNET_DEBUG("Association Parameters Negotiated:" << OFendl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC));

    /* count the presentation contexts which have been accepted by the SCP */
    /* If there are none, finish the execution */
    if (ASC_countAcceptedPresentationContexts(params) == 0)
    {
        DCMNET_ERROR("No Acceptable Presentation Contexts");
        (void) ASC_destroyAssociation(&assoc); // this also destroys the T_ASC_Parameters structure
        return NET_EC_NoAcceptablePresentationContexts;
    }

    /* dump general information concerning the establishment of the network connection if required */
    DCMNET_INFO("Association Accepted (Max Send PDV: " << assoc->sendPDVLength << ")");

    /* extract all responses to a single XML file? */
    if (extractResponses == FEM_singleXMLFile)
    {
        DCMNET_INFO("Writing all response datasets to file: " << outputFilename);
        /* create output file */
        outputStream.open(outputFilename.c_str());
        if (outputStream.good())
        {
            /* write XML header and top-level element */
            outputStream << "<?xml version=\"1.0\"";
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
            outputStream << " encoding=\"UTF-8\"";
#endif
            outputStream << "?>" << OFendl;
            outputStream << "<responses type=\"C-FIND\">" << OFendl;
        } else {
            /* report details on file i/o error */
            DCMNET_ERROR("Writing file: " << outputFilename << ": " << OFStandard::getLastSystemErrorCode().message());
        }
    }

    /* do the real work, i.e. for all files which were specified in the command line, send a */
    /* C-FIND-RQ to the other DICOM application and receive corresponding response messages. */
    cond = EC_Normal;
    if ((fileNameList == NULL) || fileNameList->empty())
    {
        /* no files provided on command line */
        cond = findSCU(assoc, NULL, repeatCount, responseCount, abstractSyntax, blockMode, dimse_timeout, extractResponses, cancelAfterNResponses, overrideKeys, callback, outputDirectory, &outputStream);
    } else {
        OFListIterator(OFString) iter = fileNameList->begin();
        OFListIterator(OFString) enditer = fileNameList->end();
        while ((iter != enditer) && cond.good())
        {
            cond = findSCU(assoc, (*iter).c_str(), repeatCount, responseCount, abstractSyntax, blockMode, dimse_timeout, extractResponses, cancelAfterNResponses, overrideKeys, callback, outputDirectory, &outputStream);
            ++iter;
        }
    }

    /* close XML file with responses */
    if (extractResponses == FEM_singleXMLFile)
    {
        if (outputStream.good())
            outputStream << "</responses>" << OFendl;
        outputStream.close();
    }

    /* tear down association, i.e. terminate network connection to SCP */
    if (cond == EC_Normal)
    {
        if (abortAssociation) {
            DCMNET_INFO("Aborting Association");
            cond = ASC_abortAssociation(assoc);
            if (cond.bad()) {
                DCMNET_ERROR("Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
                (void) ASC_destroyAssociation(&assoc); // this also destroys the T_ASC_Parameters structure
                return cond;
            }
        } else {
            /* release association */
            DCMNET_INFO("Releasing Association");
            cond = ASC_releaseAssociation(assoc);
            if (cond.bad())
            {
                DCMNET_ERROR("Association Release Failed: " << DimseCondition::dump(temp_str, cond));
                (void) ASC_destroyAssociation(&assoc); // this also destroys the T_ASC_Parameters structure
                return cond;
            }
        }
    }
    else if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        DCMNET_ERROR("Protocol Error: Peer requested release (Aborting)");
        DCMNET_INFO("Aborting Association");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            DCMNET_ERROR("Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
            (void) ASC_destroyAssociation(&assoc); // this also destroys the T_ASC_Parameters structure
            return cond;
        }
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
        DCMNET_INFO("Peer Aborted Association");
    } else {
        DCMNET_ERROR("Find SCU Failed: " << DimseCondition::dump(temp_str, cond));
        DCMNET_INFO("Aborting Association");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            DCMNET_ERROR("Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
            (void) ASC_destroyAssociation(&assoc); // this also destroys the T_ASC_Parameters structure
            return cond;
        }
    }

    /* destroy the association, i.e. free memory of T_ASC_Association* structure. This */
    /* call is the counterpart of ASC_requestAssociation(...) which was called above. */
    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad())
    {
        DCMNET_ERROR("Destroying Association Failed: " << DimseCondition::dump(temp_str, cond));
    }
    return cond;
}

OFCondition DcmFindSCU::addPresentationContext(
    T_ASC_Parameters *params,
    const char *abstractSyntax,
    E_TransferSyntax preferredTransferSyntax)
{
    /*
    ** We prefer to use Explicitly encoded transfer syntaxes.
    ** If we are running on a Little Endian machine we prefer
    ** LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
    ** Some SCP implementations will just select the first transfer
    ** syntax they support (this is not part of the standard) so
    ** organize the proposed transfer syntaxes to take advantage
    ** of such behavior.
    **
    ** The presentation contexts proposed here are only used for
    ** C-FIND and C-MOVE, so there is no need to support compressed
    ** transmission (apart from deflated transfer syntax aka zlib).
    */

    const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL };
    int numTransferSyntaxes = 0;

    switch (preferredTransferSyntax) {
    case EXS_LittleEndianImplicit:
        /* we only support Little Endian Implicit */
        transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 1;
        break;
    case EXS_LittleEndianExplicit:
        /* we prefer Little Endian Explicit */
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    case EXS_BigEndianExplicit:
        /* we prefer Big Endian Explicit */
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
#ifdef WITH_ZLIB
    case EXS_DeflatedLittleEndianExplicit:
        /* we prefer Deflated Little Endian Explicit */
        transferSyntaxes[0] = UID_DeflatedExplicitVRLittleEndianTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
#endif
    default:
        /* We prefer explicit transfer syntaxes.
         * If we are running on a Little Endian machine we prefer
         * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
         */
        if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
        {
            transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        } else {
            transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    }

    return ASC_addPresentationContext(
        params, 1, abstractSyntax,
        transferSyntaxes, numTransferSyntaxes);
}


OFBool DcmFindSCU::writeToFile(const char* ofname, DcmDataset *dataset)
{
    /* write out as a DICOM file format */

    DcmFileFormat fileformat(dataset); // copies dataset
    OFCondition ec = fileformat.error();
    if (ec.bad()) {
        DCMNET_ERROR("Writing file: " << ofname << ": " << ec.text());
        return OFFalse;
    }

    ec = fileformat.saveFile(ofname, dataset->getOriginalXfer());
    if (ec.bad()) {
        DCMNET_ERROR("Writing file: " << ofname << ": " << ec.text());
        return OFFalse;
    }

    return OFTrue;
}


OFBool DcmFindSCU::writeToXMLFile(const char* ofname, DcmDataset *dataset)
{
    if (dataset == NULL) return OFFalse;

    /* write out as an XML file (dataset only) */

    STD_NAMESPACE ofstream stream(ofname);
    if (stream.good())
    {
        size_t writeFlags = 0;

        /* try to determine character set of the dataset */
        OFString encString;
        OFString csetString;
        if (dataset->findAndGetOFStringArray(DCM_SpecificCharacterSet, csetString).good())
        {
            if (csetString == "ISO_IR 6")   // should never be present in a dataset, but ...
                encString = "UTF-8";
            else if (csetString == "ISO_IR 192")
                encString = "UTF-8";
            else if (csetString == "ISO_IR 100")
                encString = "ISO-8859-1";
            else if (csetString == "ISO_IR 101")
                encString = "ISO-8859-2";
            else if (csetString == "ISO_IR 109")
                encString = "ISO-8859-3";
            else if (csetString == "ISO_IR 110")
                encString = "ISO-8859-4";
            else if (csetString == "ISO_IR 144")
                encString = "ISO-8859-5";
            else if (csetString == "ISO_IR 127")
                encString = "ISO-8859-6";
            else if (csetString == "ISO_IR 126")
                encString = "ISO-8859-7";
            else if (csetString == "ISO_IR 138")
                encString = "ISO-8859-8";
            else if (csetString == "ISO_IR 148")
                encString = "ISO-8859-9";
            else if (csetString == "ISO_IR 203")
                encString = "ISO-8859-15";
            else {
                if (!csetString.empty())
                {
                    if (dataset->containsExtendedCharacters(OFFalse /*checkAllStrings*/))
                    {
                        DCMNET_WARN("SpecificCharacterSet (0008,0005) value '" << csetString
                            << "' not supported ... quoting non-ASCII characters");
                    } else {
                        DCMNET_WARN("SpecificCharacterSet (0008,0005) value '" << csetString << "' not supported");
                    }
                }
                /* make sure that non-ASCII characters are quoted appropriately */
                writeFlags |= DCMTypes::XF_convertNonASCII;
            }
        }
        /* we expect that the DICOM encoding is correct, so there is no "else" block */

        /* write XML document header */
        stream << "<?xml version=\"1.0\"";
        /* optional character encoding */
        if (!encString.empty())
            stream << " encoding=\"" << encString << "\"";
        stream << "?>" << OFendl;

        /* write response dataset to XML file */
        OFCondition ec = dataset->writeXML(stream, writeFlags);
        if (ec.bad()) {
            DCMNET_ERROR("Writing file: " << ofname << ": " << ec.text());
            return OFFalse;
        }
    } else {
        DCMNET_ERROR("Writing file: " << ofname << ": " << OFStandard::getLastSystemErrorCode().message());
        return OFFalse;
    }

    return OFTrue;
}


OFCondition DcmFindSCU::findSCU(
    T_ASC_Association * assoc,
    const char *fname,
    int repeatCount,
    int &responseCount,
    const char *abstractSyntax,
    T_DIMSE_BlockingMode blockMode,
    int dimse_timeout,
    DcmFindSCUExtractMode extractResponses,
    int cancelAfterNResponses,
    OFList<OFString> *overrideKeys,
    DcmFindSCUCallback *callback,
    const char *outputDirectory,
    STD_NAMESPACE ofstream *outputStream) const
    /*
     * This function will read all the information from the given file
     * (this information specifies a search mask), figure out a corresponding
     * presentation context which will be used to transmit a C-FIND-RQ message
     * over the network to the SCP, and it will finally initiate the transmission
     * of data to the SCP.
     *
     * Parameters:
     *   assoc - [in] The association (network connection to another DICOM application).
     *   fname - [in] Name of the file which shall be processed.
     */
{
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_FindRQ req;
    T_DIMSE_C_FindRSP rsp;
    DcmFileFormat dcmff;
    OFString temp_str;

    /* if there is a valid filename */
    OFCondition cond;
    if (fname != NULL) {

        /* read information from file (this information specifies a search mask). After the */
        /* call to DcmFileFormat::read(...) the information which is encapsulated in the file */
        /* will be available through the DcmFileFormat object. In detail, it will be available */
        /* through calls to DcmFileFormat::getMetaInfo() (for meta header information) and */
        /* DcmFileFormat::getDataset() (for data set information). */
        cond = dcmff.loadFile(fname);

        /* figure out if an error occurred while the file was read*/
        if (cond.bad()) {
            DCMNET_ERROR("Bad DICOM file: " << fname << ": " << cond.text());
            return cond;
        }
    }

    /* replace specific keys by those in overrideKeys */
    OFListConstIterator(OFString) path = overrideKeys->begin();
    OFListConstIterator(OFString) endOfList = overrideKeys->end();
    DcmDataset* dset = dcmff.getDataset();
    DcmPathProcessor proc;
    /* disable item wildcards since they don't make sense for Q/R or worklist
       where always a single item is sent in the query. Further, do not check
       for private reservations in query dataset.
     */
    proc.setItemWildcardSupport(OFFalse);
    proc.checkPrivateReservations(OFFalse);
    while (path != endOfList)
    {
        cond = proc.applyPathWithValue(dset, *path);
        if (cond.bad())
        {
            DCMNET_ERROR("Bad override key/path: " << *path);
            return cond;
        }
        path++;
    }

    /* figure out which of the accepted presentation contexts should be used */
    presId = ASC_findAcceptedPresentationContextID(assoc, abstractSyntax);

    if (presId == 0) {
        DCMNET_ERROR("No Presentation Context");
        return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    /* repeatCount specifies how many times a certain file shall be processed */
    int n = repeatCount;

    /* prepare C-FIND-RQ message */
    memset(OFreinterpret_cast(char*, &req), 0, sizeof(req));
    OFStandard::strlcpy(req.AffectedSOPClassUID, abstractSyntax, sizeof(req.AffectedSOPClassUID));
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_MEDIUM;

    /* prepare the callback data */
    DcmFindSCUDefaultCallback defaultCallback(extractResponses, cancelAfterNResponses, outputDirectory, outputStream, outputResponseLimit_);
    if (callback == NULL) callback = &defaultCallback;
    callback->setAssociation(assoc);
    callback->setPresentationContextID(presId);

    /* as long as no error occurred and the counter does not equal 0 */
    cond = EC_Normal;
    while (cond.good() && n--)
    {
        DcmDataset *statusDetail = NULL;

        /* complete preparation of C-FIND-RQ message */
        req.MessageID = assoc->nextMsgID++;

        /* if required, dump some more general information */
        if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
        {
            DCMNET_INFO("Sending Find Request");
            DCMNET_DEBUG(DIMSE_dumpMessage(temp_str, req, DIMSE_OUTGOING, NULL, presId));
        } else {
            DCMNET_INFO("Sending Find Request (MsgID " << req.MessageID << ")");
        }
        DCMNET_INFO("Request Identifiers:" << OFendl << DcmObject::PrintHelper(*dset));

        /* finally conduct transmission of data */
        cond = DIMSE_findUser(assoc, presId, &req, dset, responseCount,
            progressCallback, callback, blockMode, dimse_timeout,
            &rsp, &statusDetail);

        /* dump some more general information */
        if (cond.good()) {
            if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL)) {
                DCMNET_INFO("Received Final Find Response");
                DCMNET_DEBUG(DIMSE_dumpMessage(temp_str, rsp, DIMSE_INCOMING));
            } else {
                DCMNET_INFO("Received Final Find Response (" << DU_cfindStatusString(rsp.DimseStatus) << ")");
            }
        } else {
            if (fname) {
                DCMNET_ERROR("Find Failed, file: " << fname << ":");
            } else {
                DCMNET_ERROR("Find Failed, query keys:");
                DCMNET_ERROR(DcmObject::PrintHelper(dcmff));
            }
            DCMNET_ERROR(DimseCondition::dump(temp_str, cond));
        }

        /* dump status detail information if there is some */
        if (statusDetail != NULL) {
            DCMNET_DEBUG("Status Detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
            delete statusDetail;
        }
    }

    /* return */
    return cond;
}
