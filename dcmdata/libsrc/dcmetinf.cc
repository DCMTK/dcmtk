/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-12-20 13:41:27 $
 *  CVS/RCS Revision: $Revision: 1.57 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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


const Uint32 DCM_GroupLengthElementLength = 12;


// ********************************


DcmMetaInfo::DcmMetaInfo()
  : DcmItem(ItemTag),
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
        if (object->getTag().getGroup() != 0x0002)
        {
            stack.pop();
            /* remove element from meta information header and free memory */
            delete OFstatic_cast(DcmItem *, stack.top())->remove(object);
        }
    }
}


// ********************************


void DcmMetaInfo::print(STD_NAMESPACE ostream&out,
                        const size_t flags,
                        const int level,
                        const char *pixelFileName,
                        size_t *pixelCounter)
{
    out << OFendl;
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << ANSI_ESCAPE_CODE_COMMENT;
    printNestingLevel(out, flags, level);
    out << "# Dicom-Meta-Information-Header" << OFendl;
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << ANSI_ESCAPE_CODE_COMMENT;
    printNestingLevel(out, flags, level);
    out << "# Used TransferSyntax: " << DcmXfer(Xfer).getXferName() << OFendl;
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << ANSI_ESCAPE_CODE_RESET;
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


OFCondition DcmMetaInfo::writeXML(STD_NAMESPACE ostream&out,
                                  const size_t flags)
{
    OFString xmlString;
    DcmXfer xfer(Xfer);
    /* XML start tag for "meta-header" */
    out << "<meta-header xfer=\"" << xfer.getXferID() << "\"";
    out << " name=\"" << OFStandard::convertToMarkupString(xfer.getXferName(), xmlString) << "\">" << OFendl;
    if (!elementList->empty())
    {
        /* write content of all children */
        DcmObject *dO;
        elementList->seek(ELP_first);
        do
        {
            dO = elementList->get();
            dO->writeXML(out, flags);
        } while (elementList->seek(ELP_next));
    }
    /* XML end tag for "meta-header" */
    out << "</meta-header>" << OFendl;
    /* always report success */
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
    OFBool retval = OFFalse;
    if (fPreambleTransferState == ERW_inWork)
    {
        const Uint32 preambleLen = DCM_PreambleLen + DCM_MagicLen;
        const Uint32 readLen = preambleLen - getTransferredBytes();
        if (readLen > 0)
            incTransferredBytes(OFstatic_cast(Uint32, inStream.read(&filePreamble[getTransferredBytes()], readLen)));
        if (inStream.eos() && getTransferredBytes() != preambleLen)
        {   // file too short, no preamble
            inStream.putback();
            DCMDATA_TRACE("DcmMetaInfo::checkAndReadPreamble() No Preamble available: File too short ("
                << preambleLen << ") < " << DCM_PreambleLen + DCM_MagicLen << " bytes");
            retval = OFFalse;
            this -> setPreamble();
            fPreambleTransferState = ERW_ready;
        }
        else if (getTransferredBytes() == preambleLen)    // check Preamble and Dicom Prefix
        {   // set prefix to appropriate position
            char *prefix = filePreamble + DCM_PreambleLen;
            if (memcmp(prefix, DCM_Magic, DCM_MagicLen) == 0)
            {
                retval = OFTrue;  // Preamble present
                // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
            } else {              // no Preamble
                retval = OFFalse;
                this -> setPreamble();
                inStream.putback();
            }
            fPreambleTransferState = ERW_ready;
        } else
            errorFlag = EC_StreamNotifyClient;
    }

    if (fPreambleTransferState == ERW_ready)
    {
        E_TransferSyntax tmpxfer = checkTransferSyntax(inStream);
        DcmXfer tmpxferSyn(tmpxfer);
        DcmXfer xferSyn(newxfer);
        if ((tmpxferSyn.isExplicitVR() && xferSyn.isImplicitVR()) ||
            (tmpxferSyn.isImplicitVR() && xferSyn.isExplicitVR()) ||
            xferSyn.getXfer() == EXS_Unknown)
        {
            newxfer = tmpxferSyn.getXfer();   // use determined xfer
            if (xferSyn.getXfer() != EXS_Unknown)
                DCMDATA_WARN("DcmMetaInfo: TransferSyntax of MetaInfo is other than expected");
        } else
            newxfer = xferSyn.getXfer();
    }
    if (retval == OFTrue)
    {
        DCMDATA_TRACE("DcmMetaInfo::checkAndReadPreamble() Preamble = 0x"
            << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
            << STD_NAMESPACE setw(4) << OFstatic_cast(Uint32, *filePreamble));
    } else
        DCMDATA_TRACE("DcmMetaInfo::checkAndReadPreamble() No Preambel found");

    DCMDATA_TRACE("DcmMetaInfo::checkAndReadPreamble() TransferSyntax=\""
        << DcmXfer(newxfer).getXferName() << "\"");
    return retval;
} // DcmMetaInfo::checkAndReadPreamble


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
        /* figure out if the stream reported an error */
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
                /* start with "no error" in order to handle meta-header with only one data element */
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
     *   oxfer     - [in] The transfer syntax which shall be used. (is not necessary since the meta header
     *                    shall always be encoded in the explicit VR little endian transfer syntax)
     *   enctype   - [in] Encoding type for sequences. Specifies how sequences will be handled.
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
            /* if the file premable and the DICOM prefix have been written, go */
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


OFCondition DcmMetaInfo::loadFile(const char *fileName,
                                  const E_TransferSyntax readXfer,
                                  const E_GrpLenEncoding groupLength,
                                  const Uint32 maxReadLength)
{
    OFCondition l_error = EC_IllegalParameter;
    /* check parameters first */
    if ((fileName != NULL) && (strlen(fileName) > 0))
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
            }
        }
    }
    return l_error;
}


/*
** CVS/RCS Log:
** $Log: dcmetinf.cc,v $
** Revision 1.57  2010-12-20 13:41:27  joergr
** Added explicit type cast in order to keep gcc 2.95.3 quiet.
**
** Revision 1.56  2010-11-12 12:16:11  joergr
** Output ANSI escape codes at the beginnig of each line in order to make sure
** that always the correct color is used in case of truncated multi-line output.
**
** Revision 1.55  2010-10-29 10:57:21  joergr
** Added support for colored output to the print() method.
**
** Revision 1.54  2010-10-20 16:44:16  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.53  2010-10-14 13:14:08  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.52  2010-06-07 13:55:53  joergr
** Added new method that allows for loading the meta-header only.
**
** Revision 1.51  2010-03-25 16:30:17  joergr
** Made log messages more consistent within this module.
**
** Revision 1.50  2009-11-13 13:11:21  joergr
** Fixed minor issues in log output.
**
** Revision 1.49  2009-11-04 09:58:10  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.48  2009-09-28 13:30:59  joergr
** Moved general purpose definition file from module dcmdata to ofstd, and
** added new defines in order to make the usage easier.
**
** Revision 1.47  2009-08-25 12:54:57  joergr
** Added new methods which remove all data elements with an invalid group number
** from the meta information header, dataset and/or fileformat.
**
** Revision 1.46  2009-06-04 16:58:24  joergr
** Added new parsing flag that allows for ignoring the value of File Meta
** Information Group Length (0002,0000).
** Report a warning on all data elements in the meta-header which have an
** incorrect group number, i.e. everything but 0x0002.
**
** Revision 1.45  2009-03-02 11:16:36  joergr
** Moved variable definition.
**
** Revision 1.44  2009-03-02 11:15:18  joergr
** Fixed issue with incorrectly encoded file meta information header consisting
** of one data element only (e.g. TransferSyntaxUID).
**
** Revision 1.43  2009-02-04 17:57:19  joergr
** Fixes various type mismatches reported by MSVC introduced with OFFile class.
**
** Revision 1.42  2008-07-17 10:31:31  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.41  2008-04-30 12:38:42  meichel
** Fixed compile errors due to changes in attribute tag names
**
** Revision 1.40  2007/11/29 14:30:21  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.39  2007/06/29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.38  2006/08/15 15:49:54  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.37  2005/12/08 15:41:18  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.36  2005/11/28 15:53:13  meichel
** Renamed macros in dcdebug.h
**
** Revision 1.35  2005/11/07 16:59:26  meichel
** Cleaned up some copy constructors in the DcmObject hierarchy.
**
** Revision 1.34  2005/05/26 11:51:12  meichel
** Now reading DICOM files in which the meta header group length attribute
**   (0002,0000) is absent, based on a heuristic that checks for group 0002
**   attribute tags. New behaviour can be disabled by compiling with the macro
**   REJECT_FILE_IF_META_GROUP_LENGTH_ABSENT defined.
**
** Revision 1.33  2004/02/04 16:35:00  joergr
** Adapted type casts to new-style typecast operators defined in ofcast.h.
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.32  2003/10/15 16:55:43  meichel
** Updated error messages for parse errors
**
** Revision 1.31  2002/12/06 12:59:39  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
** Replaced some German comments by English translations.
**
** Revision 1.30  2002/11/27 12:06:49  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.29  2002/08/27 16:55:51  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.28  2002/04/25 10:17:19  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.27  2002/04/16 13:43:18  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.26  2001/11/01 14:55:40  wilkens
** Added lots of comments.
**
** Revision 1.25  2001/09/26 15:49:30  meichel
** Modified debug messages, required by OFCondition
**
** Revision 1.24  2001/09/25 17:19:51  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.23  2001/06/01 15:49:06  meichel
** Updated copyright header
**
** Revision 1.22  2001/05/10 12:46:52  meichel
** Fixed minor bug in DcmMetaInfo::transferEnd()
**
** Revision 1.21  2001/05/03 08:15:22  meichel
** Fixed bug in dcmdata sequence handling code that could lead to application
**   failure in rare cases during parsing of a correct DICOM dataset.
**
** Revision 1.20  2000/04/14 15:55:06  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.19  2000/03/08 16:26:38  meichel
** Updated copyright header.
**
** Revision 1.18  2000/03/03 14:05:35  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.17  2000/02/23 15:11:56  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.16  2000/02/10 10:52:20  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.15  2000/02/01 10:12:08  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.14  1999/03/31 09:25:33  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.13  1998/07/15 15:52:02  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.12  1997/07/21 08:25:28  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.11  1997/07/03 15:10:00  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.10  1997/05/16 08:23:54  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
** - Added a new method calcElementLength to calculate the length of an
**   element, item or sequence. For elements it returns the length of
**   tag, length field, vr field, and value length, for item and
**   sequences it returns the length of the whole item. sequence including
**   the Delimitation tag (if appropriate).  It can never return
**   UndefinedLength.
**
** Revision 1.9  1997/04/18 08:17:18  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   OFCondition get(Uint16 & value, const unsigned long pos);
**   becomes
**   OFCondition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.8  1996/08/05 08:46:13  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.7  1996/07/31 13:14:31  andreas
** - Minor corrections: error code for swapping to or from byteorder unknown
**                      correct read of dataset in fileformat
**
** Revision 1.6  1996/04/27 14:04:56  hewett
** Eliminated compiler warnings when compiling without -DDEBUG.  Very
** minor corrections, mostly unused parameters and uninitialized variables.
**
** Revision 1.5  1996/01/29 13:38:27  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.4  1996/01/09 11:06:47  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.3  1996/01/05 13:27:39  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
