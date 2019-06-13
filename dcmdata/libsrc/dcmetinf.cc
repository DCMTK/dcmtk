/*
 *
 *  Copyright (C) 1994-2019, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmMetaInfo
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstd.h"

#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcvrul.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/ofstd/ofdefine.h"
#include "dcmtk/dcmdata/dcistrma.h"    /* for class DcmInputStream */
#include "dcmtk/dcmdata/dcistrmf.h"    /* for class DcmInputFileStream */
#include "dcmtk/dcmdata/dcostrma.h"    /* for class DcmOutputStream */
#include "dcmtk/dcmdata/dcjson.h"


const Uint32 DCM_GroupLengthElementLength = 12;


// ********************************


DcmMetaInfo::DcmMetaInfo()
  : DcmItem(DCM_ItemTag),
    preambleUsed(OFFalse),
    fPreambleTransferState(ERW_init),
    Xfer(META_HEADER_DEFAULT_TRANSFERSYNTAX)
{
    setPreamble();
}


DcmMetaInfo::DcmMetaInfo(const DcmMetaInfo &old)
  : DcmItem(old),
    preambleUsed(old.preambleUsed),
    fPreambleTransferState(ERW_init),
    Xfer(old.Xfer)
{
    memcpy(filePreamble, old.filePreamble, 128);
}


DcmMetaInfo& DcmMetaInfo::operator=(const DcmMetaInfo& obj)
{
    if (this != &obj)
    {
        // copy parent's member variables
        DcmItem::operator=(obj);
        // copy DcmMetaInfo's member variables
        preambleUsed = obj.preambleUsed;
        fPreambleTransferState = obj.fPreambleTransferState;
        Xfer = obj.Xfer;
        memcpy(filePreamble, obj.filePreamble, 128);
    }
    return *this;
}


OFCondition DcmMetaInfo::copyFrom(const DcmObject& rhs)
{
    if (this != &rhs)
    {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmMetaInfo &, rhs);
    }
    return EC_Normal;
}


DcmMetaInfo::~DcmMetaInfo()
{
}


// ********************************


DcmEVR DcmMetaInfo::ident() const
{
    return EVR_metainfo;
}


E_TransferSyntax DcmMetaInfo::getOriginalXfer() const
{
    return Xfer;
}


void DcmMetaInfo::removeInvalidGroups()
{
    DcmStack stack;
    DcmObject *object = NULL;
    /* iterate over all elements */
    while (nextObject(stack, OFTrue).good())
    {
        object = stack.top();
        /* delete invalid elements */
        if (object->getGTag() != 0x0002)
        {
            DCMDATA_DEBUG("DcmMetaInfo::removeInvalidGroups() removing element "
                << object->getTag() << " from meta header");
            stack.pop();
            /* remove element from meta information header and free memory */
            delete OFstatic_cast(DcmItem *, stack.top())->remove(object);
        }
    }
}


// ********************************


void DcmMetaInfo::print(STD_NAMESPACE ostream &out,
                        const size_t flags,
                        const int level,
                        const char *pixelFileName,
                        size_t *pixelCounter)
{
    out << OFendl;
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << DCMDATA_ANSI_ESCAPE_CODE_COMMENT;
    printNestingLevel(out, flags, level);
    out << "# Dicom-Meta-Information-Header" << OFendl;
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << DCMDATA_ANSI_ESCAPE_CODE_COMMENT;
    printNestingLevel(out, flags, level);
    out << "# Used TransferSyntax: " << DcmXfer(Xfer).getXferName();
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << DCMDATA_ANSI_ESCAPE_CODE_RESET;
    out << OFendl;
    if (!elementList->empty())
    {
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
        } while (elementList->seek(ELP_next));
    }
}


// ********************************


OFCondition DcmMetaInfo::writeXML(STD_NAMESPACE ostream &out,
                                  const size_t flags)
{
    OFCondition l_error = EC_Normal;
    if (flags & DCMTypes::XF_useNativeModel)
    {
        /* in Native DICOM Model, there is no concept of a "file format" */
        l_error = makeOFCondition(OFM_dcmdata, EC_CODE_CannotConvertToXML, OF_error,
            "Cannot convert File Meta Information to Native DICOM Model");
    } else {
        OFString xmlString;
        DcmXfer xfer(Xfer);
        /* XML start tag for "meta-header" */
        out << "<meta-header xfer=\"" << xfer.getXferID() << "\"";
        out << " name=\"" << OFStandard::convertToMarkupString(xfer.getXferName(), xmlString) << "\">" << OFendl;
        /* write content of file meta information */
        if (!elementList->empty())
        {
            /* write content of all children */
            DcmObject *dO;
            elementList->seek(ELP_first);
            do
            {
                dO = elementList->get();
                l_error = dO->writeXML(out, flags);
            } while (l_error.good() && elementList->seek(ELP_next));
        }
        if (l_error.good())
        {
            /* XML end tag for "meta-header" */
            out << "</meta-header>" << OFendl;
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmMetaInfo::writeJson(STD_NAMESPACE ostream &out,
                                   DcmJsonFormat &format)
{
    if (format.printMetaheaderInformation)
    {
        // write content of file meta information
        if (!elementList->empty())
        {
            elementList->seek(ELP_first);
            OFCondition status = EC_Normal;
            status = elementList->get()->writeJson(out, format);
            while (status.good() && elementList->seek(ELP_next))
            {
                out << "," << format.newline();
                status = elementList->get()->writeJson(out, format);
            }
            return status;
        }
    }
    return EC_Normal;
}

// ********************************

void DcmMetaInfo::setPreamble()
{
    memzero(filePreamble, sizeof(filePreamble));
    preambleUsed = OFFalse;
}


// ********************************


OFBool DcmMetaInfo::checkAndReadPreamble(DcmInputStream &inStream,
                                         E_TransferSyntax &newxfer)
{
    if (fPreambleTransferState == ERW_init)
    {
        inStream.mark();
        fPreambleTransferState = ERW_inWork;
    }
    // by default, we assume that there is no preamble
    OFBool hasPreamble = OFFalse;
    if (fPreambleTransferState == ERW_inWork)
    {
        const Uint32 preambleLen = DCM_PreambleLen + DCM_MagicLen;
        const Uint32 readLen = preambleLen - getTransferredBytes();
        if (readLen > 0)
            incTransferredBytes(OFstatic_cast(Uint32, inStream.read(&filePreamble[getTransferredBytes()], readLen)));
        // file too short, no preamble
        if (inStream.eos() && getTransferredBytes() != preambleLen)
        {
            inStream.putback();
            DCMDATA_TRACE("DcmMetaInfo::checkAndReadPreamble() No Preamble available: File too short ("
                << preambleLen << ") < " << DCM_PreambleLen + DCM_MagicLen << " bytes");
            this -> setPreamble();
            fPreambleTransferState = ERW_ready;
        }
        // check preamble and DICOM prefix
        else if (getTransferredBytes() == preambleLen)
        {
            // set prefix to appropriate position
            char *prefix = filePreamble + DCM_PreambleLen;
            if (memcmp(prefix, DCM_Magic, DCM_MagicLen) == 0)
            {
                // preamble present
                hasPreamble = OFTrue;
                // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
            } else {
                // no preamble
                this -> setPreamble();
                inStream.putback();
            }
            fPreambleTransferState = ERW_ready;
        } else
            errorFlag = EC_StreamNotifyClient;
    }

    if (fPreambleTransferState == ERW_ready)
    {
        E_TransferSyntax tmpXfer = checkTransferSyntax(inStream);
        DcmXfer tmpXferSyn(tmpXfer);
        DcmXfer xferSyn(newxfer);
        // check determined transfer syntax
        if ((tmpXferSyn.isExplicitVR() && xferSyn.isImplicitVR()) ||
            (tmpXferSyn.isImplicitVR() && xferSyn.isExplicitVR()) ||
            xferSyn.getXfer() == EXS_Unknown)
        {
            // use determined transfer syntax
            newxfer = tmpXferSyn.getXfer();
            if (xferSyn.getXfer() != EXS_Unknown)
                DCMDATA_WARN("DcmMetaInfo: TransferSyntax of MetaInfo is other than expected");
        } else
            newxfer = xferSyn.getXfer();
    }
    if (hasPreamble)
    {
        DCMDATA_TRACE("DcmMetaInfo::checkAndReadPreamble() Preamble = 0x"
            << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
            << STD_NAMESPACE setw(4) << OFstatic_cast(Uint32, *filePreamble));
        DCMDATA_DEBUG("DcmMetaInfo::checkAndReadPreamble() TransferSyntax=\""
            << DcmXfer(newxfer).getXferName() << "\"");
    } else
        DCMDATA_TRACE("DcmMetaInfo::checkAndReadPreamble() No Preamble found");

    return hasPreamble;
}


// ********************************


OFBool DcmMetaInfo::nextTagIsMeta(DcmInputStream &inStream)
{
    char testbytes[2];
    inStream.mark();
    inStream.read(testbytes, 2);
    inStream.putback();
    // check for group 0x0002 only
    return (testbytes[0] == 0x02 && testbytes[1] == 0x00) || (testbytes[0] == 0x00 && testbytes[1] == 0x02);
}


// ********************************


Uint32 DcmMetaInfo::calcElementLength(const E_TransferSyntax /*xfer*/,
                                      const E_EncodingType enctype)
{
    Uint32 metaLength = DcmItem::getLength(META_HEADER_DEFAULT_TRANSFERSYNTAX, enctype);
    metaLength += DCM_PreambleLen + DCM_MagicLen;
    return metaLength;
}


// ********************************


OFCondition DcmMetaInfo::readGroupLength(DcmInputStream &inStream,
                                         const E_TransferSyntax xfer,
                                         const DcmTagKey &xtag,
                                         const E_GrpLenEncoding glenc,
                                         Uint32 &headerLen,
                                         Uint32 &bytesRead,
                                         const Uint32 maxReadLength)
{
    OFCondition l_error = EC_TagNotFound;
    E_TransferSyntax newxfer = xfer;
    bytesRead = 0;
    headerLen = 0;
    if (nextTagIsMeta(inStream))
    {
        DcmTag newTag;
        Uint32 newValueLength = 0;
        Uint32 bytes_tagAndLen = 0;
        l_error = DcmItem::readTagAndLength(inStream, newxfer, newTag, newValueLength, bytes_tagAndLen);
        bytesRead += bytes_tagAndLen;
        if (l_error.good() && !inStream.good())
            l_error = inStream.status();
        if (l_error.good())
        {
            l_error = DcmItem::readSubElement(inStream, newTag, newValueLength, newxfer, glenc, maxReadLength);
            bytesRead += newValueLength;
            if (l_error.good() && newTag.getXTag() == xtag && elementList->get() != NULL && newValueLength > 0)
            {
                l_error = (OFstatic_cast(DcmUnsignedLong *, elementList->get()))->getUint32(headerLen);
                DCMDATA_TRACE("DcmMetaInfo::readGroupLength() Group Length of File Meta Header = " << headerLen + bytesRead);
            } else {
                l_error = EC_CorruptedData;
                DCMDATA_WARN("DcmMetaInfo: No Group Length available in Meta Information Header");
            }
        }
    }

    DCMDATA_TRACE("DcmMetaInfo::readGroupLength() returns error = " << l_error.text());
    return l_error;
}


// ********************************


OFCondition DcmMetaInfo::read(DcmInputStream &inStream,
                              const E_TransferSyntax xfer,
                              const E_GrpLenEncoding glenc,
                              const Uint32 maxReadLength)
{
    if (fPreambleTransferState == ERW_notInitialized || getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        Xfer = xfer;
        E_TransferSyntax newxfer = xfer;
        // figure out if the stream reported an error
        errorFlag = inStream.status();
        if (errorFlag.good() && inStream.eos())
            errorFlag = EC_EndOfStream;
        else if (errorFlag.good() && getTransferState() != ERW_ready)
        {
            if (getTransferState() == ERW_init && fPreambleTransferState != ERW_ready)
            {
                if (xfer == EXS_Unknown)
                    preambleUsed = checkAndReadPreamble(inStream, newxfer);
                else
                    newxfer = xfer;
                if (fPreambleTransferState == ERW_ready)
                {
                    Xfer = newxfer;   // store parameter transfer syntax
                    setTransferState(ERW_inWork);
                    setTransferredBytes(0);
                    fStartPosition = inStream.tell();
                    setLengthField(0);
                }
            }
            if (getTransferState() == ERW_inWork && getLengthField() == 0)
            {
                if (inStream.avail() < OFstatic_cast(offile_off_t, DCM_GroupLengthElementLength))
                    errorFlag = EC_StreamNotifyClient;
                else
                {
                    Uint32 headerLength = 0;
                    Uint32 bytesRead = 0;
                    errorFlag = readGroupLength(inStream, newxfer, DCM_FileMetaInformationGroupLength, glenc,
                        headerLength, bytesRead, maxReadLength);

                    setTransferredBytes(bytesRead);

                    if (errorFlag.good())
                    {
                        /* FileMetaInformationGroupLength (0002,0000) is present but should be ignored  */
                        if (dcmIgnoreFileMetaInformationGroupLength.get())
                        {
                            DCMDATA_WARN("DcmMetaInfo: Ignoring Group Length of Meta Information Header");
                            setLengthField(DCM_UndefinedLength);
                        } else
                            setLengthField(headerLength + getTransferredBytes());
                    } else
                        setLengthField(DCM_UndefinedLength);
                }
            }
#ifdef REJECT_FILE_IF_META_GROUP_LENGTH_ABSENT
            // this is the old behaviour up to DCMTK 3.5.3: fail with EC_CorruptedData error code
            // if the file meta header group length (0002,0000) is absent.
            if (getTransferState() == ERW_inWork && getLengthField() != 0 && errorFlag.good())
            {
#else
            // new behaviour: accept file without meta header group length, determine end of
            // meta header based on heuristic that checks for group 0002 tags.
            if (getTransferState() == ERW_inWork && getLengthField() != 0 && (errorFlag.good() ||
                ((errorFlag == EC_CorruptedData) && (getLengthField() == DCM_UndefinedLength))))
            {
                // start with "no error" in order to handle meta-header with only one data element
                errorFlag = EC_Normal;
#endif
                while (inStream.good() && !inStream.eos() &&
                       ((getLengthField() < DCM_UndefinedLength && getTransferredBytes() < getLengthField()) ||
                        (getLengthField() == DCM_UndefinedLength && nextTagIsMeta(inStream)) ||
                        !lastElementComplete))
                {
                    DcmTag newTag;
                    Uint32 newValueLength = 0;
                    Uint32 bytes_tagAndLen = 0;
                    if (lastElementComplete)
                    {
                        errorFlag = DcmItem::readTagAndLength(inStream, newxfer, newTag, newValueLength, bytes_tagAndLen);
                        incTransferredBytes(bytes_tagAndLen);
                        if (errorFlag != EC_Normal)
                            break;                         // terminate while loop
                        lastElementComplete = OFFalse;
                        errorFlag = DcmItem::readSubElement(inStream, newTag, newValueLength, newxfer, glenc, maxReadLength);
                        if (errorFlag.good())
                            lastElementComplete = OFTrue;
                        /* check for valid meta-header elements */
                        if (newTag.getGroup() != 0x0002)
                            DCMDATA_WARN("DcmMetaInfo: Invalid Element " << newTag << " found in Meta Information Header");
                    } else {
                        errorFlag = elementList->get()->read(inStream, xfer, glenc, maxReadLength);
                        if (errorFlag.good())
                            lastElementComplete = OFTrue;
                    }
                    setTransferredBytes(OFstatic_cast(Uint32, inStream.tell() - fStartPosition));
                    if (errorFlag.bad())
                        break;                      // terminate while loop

                } //while
            }
            if (errorFlag == EC_TagNotFound || errorFlag == EC_EndOfStream)
            {
                errorFlag = EC_Normal;      // there is no meta header
                Xfer = EXS_Unknown;
                if (preambleUsed)           // ... but a preamble!
                    DCMDATA_WARN("DcmMetaInfo: Found Preamble but no Meta Information Header");
            } else if (errorFlag == EC_ItemEnd)
                errorFlag = EC_Normal;
            if (errorFlag.good())
            {
                if (getLengthField() != DCM_UndefinedLength && getTransferredBytes() != getLengthField())
                    DCMDATA_WARN("DcmMetaInfo: Group Length of Meta Information Header has incorrect value");
                setTransferState(ERW_ready);          // MetaInfo is complete
            }
        }
    }
    return errorFlag;
} // DcmMetaInfo::read()


// ********************************


void DcmMetaInfo::transferInit()
{
    DcmItem::transferInit();
    fPreambleTransferState = ERW_init;
}


// ********************************


void DcmMetaInfo::transferEnd()
{
    DcmItem::transferEnd();
    fPreambleTransferState = ERW_notInitialized;
}


// ********************************


OFCondition DcmMetaInfo::write(
    DcmOutputStream &outStream,
    const E_TransferSyntax /*oxfer*/,
    const E_EncodingType enctype,
    DcmWriteCache *wcache)
    /*
     * This function writes all data elements which make up the meta header to the stream.
     * For a specification of the elements that make up the meta header see DICOM standard
     * (year 2000) part 10, section 7.1)) or the corresponding section in a later version of
     * the standard).
     *
     * Parameters:
     *   outStream - [inout] The stream that the information will be written to.
     *   oxfer     - [in]    The transfer syntax which shall be used. (is not necessary since the meta header
     *                       shall always be encoded in the explicit VR little endian transfer syntax)
     *   enctype   - [in]    Encoding type for sequences. Specifies how sequences will be handled.
     */
{
    /* if the transfer state of this is not initialized, this is an illegal call */
    if (getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* if this is not an illegal call, do something */

        /* determine the (default) transfer syntax which shall be used (always explicit VR little endian) */
        E_TransferSyntax outxfer = META_HEADER_DEFAULT_TRANSFERSYNTAX;
        /* check if the stream reported an error so far */
        errorFlag = outStream.status();
        /* if the stream did not report any error and the transfer state is ERW_ready, */
        /* go ahead and write the meta header information to the out stream */
        if (errorFlag.good() && getTransferState() != ERW_ready)
        {
            /* if some particular conditions are met we need to write the file preamble (128 byte wide) and */
            /* the DICOM prefix "DICM" to the stream. Always check if there is enough space in the stream and */
            /* set the transfer state of certain elements to indicate that they have already been written. */
            if (getTransferState() == ERW_init)
            {
                if (preambleUsed || !elementList->empty())
                {
                    if (fPreambleTransferState == ERW_init)
                    {
                        incTransferredBytes(OFstatic_cast(Uint32, outStream.write(&filePreamble[getTransferredBytes()],
                            DCM_PreambleLen - getTransferredBytes())));
                        if (getTransferredBytes() != DCM_PreambleLen)
                            errorFlag = EC_StreamNotifyClient;
                        else
                            fPreambleTransferState = ERW_inWork;
                    }
                    if (fPreambleTransferState == ERW_inWork && outStream.avail() >= 4)
                    {
                        outStream.write(DCM_Magic, 4);
                        fPreambleTransferState = ERW_ready;
                        setTransferState(ERW_inWork);
                        elementList->seek(ELP_first);
                    } else
                        errorFlag = EC_StreamNotifyClient;
                }
            }
            /* if the file preamble and the DICOM prefix have been written, go */
            /* ahead and write the meta header's data elements to the stream. */
            /* (note that at this point elementList->get() should never be NULL, */
            /* but lets play the game safe here...) */
            if (!elementList->empty() && (getTransferState() == ERW_inWork) && (elementList->get() != NULL))
            {
                DcmObject *dO;
                /* iterate over the list of data elements and write them to the stream */
                do {
                    dO = elementList->get();
                    errorFlag = dO->write(outStream, outxfer, enctype, wcache);
                } while (errorFlag.good() && elementList->seek(ELP_next));
            }
            /* if the error flag equals ok and the transfer state equals ERW_inWork, all data elements of the meta */
            /* header have been written to the stream. Indicate this by setting the transfer state to ERW_ready */
            if (errorFlag.good() && getTransferState() == ERW_inWork)
                setTransferState(ERW_ready);
        }
    }
    /* return result value */
    return errorFlag;
}


// ********************************


OFCondition DcmMetaInfo::loadFile(const OFFilename &fileName,
                                  const E_TransferSyntax readXfer,
                                  const E_GrpLenEncoding groupLength,
                                  const Uint32 maxReadLength)
{
    OFCondition l_error = EC_InvalidFilename;
    /* check parameters first */
    if (!fileName.isEmpty())
    {
        /* open file for input */
        DcmInputFileStream fileStream(fileName);
        /* check stream status */
        l_error = fileStream.status();
        if (l_error.good())
        {
            /* clear this object */
            l_error = clear();
            if (l_error.good())
            {
                /* read data from file */
                transferInit();
                l_error = read(fileStream, readXfer, groupLength, maxReadLength);
                transferEnd();
                /* make sure that the file preamble is present */
                if (l_error.good() && !preambleUsed)
                    l_error = EC_FileMetaInfoHeaderMissing;
            }
        }
    }
    return l_error;
}
