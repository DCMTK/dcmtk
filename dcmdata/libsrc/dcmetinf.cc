/*
 *
 *  Copyright (C) 1994-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: class DcmMetaInfo
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-25 10:17:19 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcmetinf.cc,v $
 *  CVS/RCS Revision: $Revision: 1.28 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

#include <string.h>

#include "ofstream.h"
#include "ofstd.h"

#include "dcmetinf.h"
#include "dcitem.h"
#include "dcxfer.h"
#include "dcvrul.h"
#include "dcdebug.h"
#include "dcdeftag.h"
#include "dcdefine.h"

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


// ********************************


DcmMetaInfo::DcmMetaInfo( const DcmMetaInfo &old )
  : DcmItem(old),
    preambleUsed(OFFalse),
    fPreambleTransferState(ERW_init),
    Xfer(META_HEADER_DEFAULT_TRANSFERSYNTAX)
{
    if ( old.ident() == EVR_metainfo ) 
    {
        Xfer = old.Xfer;
        preambleUsed = old.preambleUsed;
        memcpy( filePreamble, old.filePreamble, 128 );
    } else {
        // wrong use of copy constructor
        setPreamble();
    }
}


// ********************************


DcmMetaInfo::~DcmMetaInfo()
{
}


// ********************************


void DcmMetaInfo::print(ostream & out, const OFBool showFullData,
                        const int level, const char *pixelFileName,
                        size_t *pixelCounter)
{
    int i;
    out << endl;
    for ( i=0; i<level; i++)
        out << "    ";
    out << "# Dicom-Meta-Information-Header" << endl;
    for ( i=0; i<level; i++)
        out << "    ";
    out << "# Used TransferSyntax: " << DcmXfer( Xfer ).getXferName();
    out << endl;
    if ( !elementList->empty() )
    {
        DcmObject *dO;
        elementList->seek( ELP_first );
        do {
            dO = elementList->get();
            dO->print(out, showFullData, level + 1, pixelFileName, pixelCounter);
        } while ( elementList->seek( ELP_next ) );
    }
}


// ********************************


OFCondition DcmMetaInfo::writeXML(ostream &out,
                                  const size_t flags)
{
    OFString xmlString;
    DcmXfer xfer(Xfer);
    /* XML start tag for "meta-header" */
    out << "<meta-header xfer=\"" << xfer.getXferID() << "\"";
    out << " name=\"" << OFStandard::convertToMarkupString(xfer.getXferName(), xmlString) << "\">" << endl;
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
    out << "</meta-header>" << endl;
    /* always report success */
    return EC_Normal;
}


// ********************************


void DcmMetaInfo::setPreamble()
{
    memzero(filePreamble, sizeof(filePreamble) );
    preambleUsed = OFFalse;
}


// ********************************


OFBool DcmMetaInfo::checkAndReadPreamble(DcmStream & inStream,
                                       E_TransferSyntax & newxfer) 
{
    if (fPreambleTransferState == ERW_init)
    {
        inStream.SetPutbackMark();
        fPreambleTransferState = ERW_inWork;
    }

    OFBool retval = OFFalse;

    if (fPreambleTransferState == ERW_inWork)
    {
        const Uint32 preambleLen = DCM_PreambleLen + DCM_MagicLen;
        const Uint32 readLen = 
            preambleLen-fTransferredBytes <= inStream.Avail() ?
            preambleLen-fTransferredBytes : inStream.Avail();

        if (readLen > 0)
        {
            inStream.ReadBytes(&filePreamble[fTransferredBytes], readLen);
            fTransferredBytes += inStream.TransferredBytes();
        }

        if (inStream.EndOfStream() && fTransferredBytes != preambleLen)
        {                                            // Datei zu kurz => keine Preamble
            inStream.ClearError();
            inStream.Putback();
            debug(4, ( "DcmMetaInfo::checkAndReadPreamble() No Preamble available: File too short (%d) < %d bytes",
                    preambleLen, DCM_PreambleLen + DCM_MagicLen ));

            retval = OFFalse;
            this -> setPreamble();
            fPreambleTransferState = ERW_ready;
        }
        else if (fTransferredBytes == preambleLen)      
            // pruefe Preamble und DicomPrefix
        {                                          // setze prefix auf richtige Position
            char *prefix = filePreamble + DCM_PreambleLen;
            if ( memcmp(prefix, DCM_Magic, DCM_MagicLen) == 0 )
            {
                retval = OFTrue;                              // Preamble vorhanden!
                inStream.UnsetPutbackMark();
            }
            else
            {                                         // keine Preamble
                retval = OFFalse;
                this -> setPreamble();
                inStream.Putback();
            }
            fPreambleTransferState = ERW_ready;
        }
        else
            errorFlag = EC_StreamNotifyClient;

    }

    if (fPreambleTransferState == ERW_ready)
    {
        E_TransferSyntax tmpxfer = checkTransferSyntax(inStream);
        DcmXfer tmpxferSyn( tmpxfer );
        DcmXfer xferSyn(newxfer);
        if ((tmpxferSyn.isExplicitVR() && xferSyn.isImplicitVR()) ||
            (tmpxferSyn.isImplicitVR() && xferSyn.isExplicitVR()) ||
            xferSyn.getXfer() == EXS_Unknown)
        {
            newxfer = tmpxferSyn.getXfer();   // benutze ermittelte xfer
            if ( xferSyn.getXfer() != EXS_Unknown )
            {
                ofConsole.lockCerr() << "Info: DcmMetaInfo::checkAndReadPreamble(): "
                    "TransferSyntax of MetaInfo is different from "
                    "passed parameter newxfer: ignoring newxfer!" << endl;
                ofConsole.unlockCerr();
            }
        }
        else
            newxfer = xferSyn.getXfer();
    }


    Cdebug(4, retval==OFTrue, ("DcmMetaInfo::checkAndReadPreamble() found Preamble=[0x%8.8x]", (Uint32)(*filePreamble) ));
    Cdebug(4, retval==OFFalse, ("DcmMetaInfo::checkAndReadPreamble() No Preambel found!" ));
    debug(4, ( "DcmMetaInfo::checkAndReadPreamble() TransferSyntax = %s", DcmXfer(newxfer).getXferName() ));
    return retval;
} // DcmMetaInfo::checkAndReadPreamble


// ********************************


OFBool DcmMetaInfo::nextTagIsMeta(DcmStream & inStream)
{
    char testbytes[2];
    inStream.SetPutbackMark();
    inStream.ReadBytes(testbytes, 2);
    inStream.Putback();

    // teste nur auf Gruppe 0x0002
    return (testbytes[0] == 0x02 && testbytes[1] == 0x00) ||
        (testbytes[0] == 0x00 && testbytes[1] == 0x02);
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


OFCondition DcmMetaInfo::readGroupLength(DcmStream & inStream,
                                         const E_TransferSyntax xfer,
                                         const DcmTagKey &xtag,
                                         const E_GrpLenEncoding glenc,
                                         Uint32 & headerLen,
                                         Uint32 & bytesRead,
                                         const Uint32 maxReadLength)
{
    OFCondition l_error = EC_TagNotFound;
    E_TransferSyntax newxfer = xfer;
    bytesRead = 0;
    headerLen = 0;

    if (this->nextTagIsMeta(inStream))
    {
        DcmTag newTag;
        Uint32 newValueLength = 0;
        Uint32 bytes_tagAndLen = 0;
        l_error = DcmItem::readTagAndLength(inStream,
                                            newxfer,
                                            newTag,
                                            newValueLength,
                                            bytes_tagAndLen);
        bytesRead += bytes_tagAndLen;

        if (inStream.GetError() == EC_Normal && l_error == EC_Normal )
        {
            l_error = DcmItem::readSubElement(inStream,
                                              newTag,
                                              newValueLength,
                                              newxfer,
                                              glenc,
                                              maxReadLength);
            bytesRead += newValueLength;

            if (l_error == EC_Normal && newTag.getXTag() == xtag &&
                elementList->get() != NULL && newValueLength > 0 )
            {
                l_error = ((DcmUnsignedLong*)(elementList->get()))->
                    getUint32(headerLen);
                debug(4, ("DcmMetaInfo::readGroupLength() Group Length of File Meta Header=%d", headerLen+bytesRead));

            }
            else
            {
                l_error = EC_CorruptedData;
                ofConsole.lockCerr() << "Warning: DcmMetaInfo::readGroupLength(): No Group"
                    " Length available in Meta Information Header" << endl;
                ofConsole.unlockCerr();
            }
        }
        else
            l_error = EC_InvalidStream;
    }
    debug(4, ("DcmMetaInfo::readGroupLength() returns error = %s", l_error.text()));
    return l_error;
}


// ********************************


OFCondition DcmMetaInfo::read(DcmStream & inStream,
                              const E_TransferSyntax xfer,
                              const E_GrpLenEncoding glenc,
                              const Uint32 maxReadLength)
{
    if (fPreambleTransferState == ERW_notInitialized || 
        fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        Xfer = xfer;
        E_TransferSyntax newxfer = xfer;

        errorFlag = inStream.GetError();
        if (errorFlag == EC_Normal && inStream.EndOfStream())
            errorFlag = EC_EndOfStream;
        else if (errorFlag == EC_Normal && fTransferState != ERW_ready)
        {

            if (fTransferState == ERW_init && 
                fPreambleTransferState != ERW_ready)
            {
                if ( xfer == EXS_Unknown )
                {
                    preambleUsed = checkAndReadPreamble(inStream, newxfer);
                    Cdebug(3, inStream.Tell() != 0, ("DcmMetaInfo::Read() found %ld bytes preamble", 
                            inStream.Tell()));
                }
                else
                    newxfer = xfer;

                if (fPreambleTransferState == ERW_ready)
                {
                    Xfer = newxfer;   // store parameter transfer syntax
                    fTransferState = ERW_inWork;
                    fTransferredBytes = 0;
                    fStartPosition = inStream.Tell(); 
                    Length = 0;
                }
            }

            if (fTransferState == ERW_inWork && Length == 0 && 
                (errorFlag = inStream.Avail(DCM_GroupLengthElementLength))
                == EC_Normal)
            {
                Uint32 headerLength = 0;
                errorFlag = readGroupLength(inStream, newxfer,  
                                            DCM_MetaElementGroupLength, glenc, 
                                            headerLength, fTransferredBytes, 
                                            maxReadLength);
                if (errorFlag == EC_Normal)
                    Length = headerLength + fTransferredBytes;
                else
                    Length = DCM_UndefinedLength;
            }

            if (fTransferState == ERW_inWork && Length != 0 && 
                errorFlag == EC_Normal)
            {
                while (inStream.GetError() == EC_Normal && 
                       !inStream.EndOfStream() && 
                       ((Length < DCM_UndefinedLength && 
                         fTransferredBytes < Length) ||
                        (Length == DCM_UndefinedLength && 
                         nextTagIsMeta(inStream)) ||
                        !lastElementComplete))
                {
                    DcmTag newTag;
                    Uint32 newValueLength = 0;
                    Uint32 bytes_tagAndLen = 0;
                    if ( lastElementComplete )
                    {
                        errorFlag = DcmItem::readTagAndLength(inStream, newxfer,
                                                              newTag, 
                                                              newValueLength,
                                                              bytes_tagAndLen);
                        fTransferredBytes += bytes_tagAndLen;
                        if (errorFlag != EC_Normal)
                            break;                         // beende while-Schleife

                        lastElementComplete = OFFalse;
                        errorFlag = DcmItem::readSubElement(inStream, newTag,
                                                            newValueLength,
                                                            newxfer,
                                                            glenc,
                                                            maxReadLength);
                        if ( errorFlag == EC_Normal )
                            lastElementComplete = OFTrue;
                    }
                    else
                    {
                        errorFlag = elementList->get()->read(inStream, xfer, 
                                                             glenc,
                                                             maxReadLength);
                        if ( errorFlag == EC_Normal )
                            lastElementComplete = OFTrue;
                    }
                    fTransferredBytes = inStream.Tell() - fStartPosition;

                    if (errorFlag != EC_Normal)
                        break;                      // beende while-Schleife

                } //while 
            } 

            if (errorFlag == EC_TagNotFound || errorFlag == EC_EndOfStream)
            {
                errorFlag = EC_Normal;      // es existiert kein Meta-Header
                Xfer = EXS_Unknown;
            }
            else if ( errorFlag == EC_ItemEnd )
                errorFlag = EC_Normal;
            if (errorFlag == EC_Normal)
            {
                if (Length != DCM_UndefinedLength && fTransferredBytes != Length)
                {
                  ofConsole.lockCerr() << "Warning: DcmMetaInfo::read(): "
                        "Group Length of Meta"
                        " Information Header has incorrect Value!" << endl;
                  ofConsole.unlockCerr();
                }
                fTransferState = ERW_ready;          // MetaInfo ist komplett
            }
        }
    }
    return errorFlag;
} // DcmMetaInfo::read()


// ********************************


void DcmMetaInfo::transferInit(void)
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

OFCondition DcmMetaInfo::write(DcmStream & outStream,
                               const E_TransferSyntax /*oxfer*/,
                               const E_EncodingType enctype)
    /*
     * This function writes all data elements which make up the meta header to the stream.
     * For a specification of the elements that make up the meta header see DICOM standard
     * (year 2000) part 10, section 7.1) )or the corresponding section in a later version of
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
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* if this is not an illegal call, do something */

        /* determine the (default) transfer syntax which shall be used (always explicit VR little endian) */
        E_TransferSyntax outxfer = META_HEADER_DEFAULT_TRANSFERSYNTAX;

        /* check if the stream reported an error so far */
        errorFlag = outStream.GetError();

        /* if the stream did not report any error and the transfer state is ERW_ready, */
        /* go ahead and write the meta header information to the out stream */
        if (errorFlag == EC_Normal && fTransferState != ERW_ready)
        {

            /* if some particular conditions are met we need to write the file preamble (128 byte wide) and */
            /* the DICOM prefix "DICM" to the stream. Always check if there is enough space in the stream and */
            /* set the transfer state of certain elements to indicate that they have already been written. */
            if (fTransferState == ERW_init)
            {
                if ( preambleUsed || !elementList->empty() )
                {
                    if (fPreambleTransferState == ERW_init)
                    {
                        Uint32 writeLen = 
                            DCM_PreambleLen-fTransferredBytes < 
                            outStream.Avail() ?
                            DCM_PreambleLen - fTransferredBytes 
                            : outStream.Avail();

                        outStream.WriteBytes(&filePreamble[fTransferredBytes], 
                                             writeLen);
                        fTransferredBytes += outStream.TransferredBytes();

                        if (fTransferredBytes != DCM_PreambleLen)
                            errorFlag = EC_StreamNotifyClient;
                        else
                            fPreambleTransferState = ERW_inWork;
                    }

                    if (fPreambleTransferState == ERW_inWork && 
                        outStream.Avail() >= 4)
                    {
                        outStream.WriteBytes(DCM_Magic, 4);
                        fPreambleTransferState = ERW_ready;
                        fTransferState = ERW_inWork;
                        elementList->seek( ELP_first );
                    }
                    else
                        errorFlag = EC_StreamNotifyClient;

                }
            }

            /* if the file premable and the DICOM prefix have been written, go */
            /* ahead and write the meta header's data elements to the stream. */
            /* (note that at this point elementList->get() should never be NULL, */
            /* but lets play the game safe here...) */
            if (!elementList->empty() && (fTransferState == ERW_inWork) && (elementList->get() != NULL))
            {
                DcmObject *dO;

                /* iterate over the list of data elements and write them to the stream */
                do 
                {
                    dO = elementList->get();
                    errorFlag = dO->write(outStream, outxfer, enctype);
                } while (errorFlag == EC_Normal && elementList->seek(ELP_next));
            }

            /* if the error flag equals ok and the transfer state equals ERW_inWork, all data elements of the meta */
            /* header have been written to the stream. Indicate this by setting the transfer state to ERW_ready */
            if (errorFlag == EC_Normal && fTransferState == ERW_inWork)
                fTransferState = ERW_ready;
        }
    }

    /* return result value */
    return errorFlag;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcmetinf.cc,v $
** Revision 1.28  2002-04-25 10:17:19  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.27  2002/04/16 13:43:18  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
** Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
** contribution.
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

