/*
**
** Author: Gerd Ehlers	    15.05.94 -- Created
**         Andreas Barth    03.12.95 -- New Stream classes
** Kuratorium OFFIS e.V.
**
** Module: dcmetinf.cc
**
** Purpose:
** Implementation of class DcmMetaInfo
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-29 13:38:27 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcmetinf.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include <iostream.h>
#include <string.h>

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
    : DcmItem( ItemTag )
{
    Bdebug((5, "dcmetinf:DcmMetaInfo::DcmMetaInfo()" ));
    debug(( 8, "Object pointer this=0x%p", this ));

    Xfer = META_HEADER_DEFAULT_TRANSFERSYNTAX;
    setPreamble();
    fPreambleTransferState = ERW_init;

    Edebug(());
}


// ********************************


DcmMetaInfo::DcmMetaInfo( const DcmMetaInfo &old )
    : DcmItem( old )
{
    Bdebug((5, "dcmetinf:DcmMetaInfo::DcmMetaInfo(DcmMetaInfo&)" ));
    debug(( 8, "Object pointer this=0x%p", this ));
    debug(( 5, "ident()=%d", old.ident() ));

    if ( old.ident() == EVR_metainfo ) 
    {
	Xfer = old.Xfer;
	preambleUsed = old.preambleUsed;
	memcpy( filePreamble, old.filePreamble, 128 );
    } else {
        cerr << "Warning: DcmMetaInfo: wrong use of Copy-Constructor"
	     << endl;
	setPreamble();
    }

    Edebug(());
}


// ********************************


DcmMetaInfo::~DcmMetaInfo()
{
    Bdebug((5, "dcmetinf:DcmMetaInfo::~DcmMetaInfo()" ));
    debug(( 8, "Object pointer this=0x%p", this ));
    Edebug(());

}


// ********************************


void DcmMetaInfo::print(const int level)
{
    int i;
    cout << endl;
    for ( i=0; i<level; i++)
	cout << "    ";
    cout << "# Dicom-Meta-Information-Header" << endl;
    for ( i=0; i<level; i++)
	cout << "    ";
    cout << "# Used TransferSyntax: " << DcmXfer( Xfer ).getXferName();
    cout << endl;
    if ( !elementList->empty() )
    {
	DcmObject *dO;
	elementList->seek( ELP_first );
	do {
	    dO = elementList->get();
	    dO->print( level + 1 );
	} while ( elementList->seek( ELP_next ) );
    }
}


// ********************************


void DcmMetaInfo::setPreamble()
{
    memzero(filePreamble, sizeof(filePreamble) );
    preambleUsed = FALSE;
}


// ********************************


BOOL DcmMetaInfo::checkAndReadPreamble(DcmStream & inStream,
				       E_TransferSyntax & newxfer) 
{
    Bdebug((4, "dcmetinf:DcmMetaInfo::checkAndReadPreamble()" ));

    if (fPreambleTransferState == ERW_init)
    {
	inStream.SetPutbackMark();
	fPreambleTransferState = ERW_inWork;
    }

    BOOL retval = FALSE;

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
	{					     // Datei zu kurz => keine Preamble
	    inStream.ClearError();
	    inStream.Putback();
	    debug(( 4, "No Preamble available: File too short (%d) < %d bytes",
		    preambleLen, DCM_PreambleLen + DCM_MagicLen ));

	    retval = FALSE;
	    this -> setPreamble();
	    fPreambleTransferState = ERW_ready;
	}
	else if (fTransferredBytes == preambleLen)	
	    // pruefe Preamble und DicomPrefix
	{					   // setze prefix auf richtige Position
	    char *prefix = filePreamble + DCM_PreambleLen;
	    if ( memcmp(prefix, DCM_Magic, DCM_MagicLen) == 0 )
	    {
		retval = TRUE;			      // Preamble vorhanden!
		inStream.UnsetPutbackMark();
	    }
	    else
	    {					      // keine Preamble
		retval = FALSE;
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
		cerr << "Info: DcmMetaInfo::checkAndReadPreamble(): "
		    "TransferSyntax of MetaInfo is different from "
		    "passed parameter newxfer: ignoring newxfer!"
		     << endl;
	}
	else
	    newxfer = xferSyn.getXfer();
    }


    Vdebug((4, retval==TRUE, "found Preamble=[0x%8.8x]", (Uint32)(*filePreamble) ));
    Vdebug((4, retval==FALSE, "No Preambel found!" ));
    debug(( 4, "TransferSyntax(-1..3)=(%d)", newxfer ));
    Edebug(());

    return retval;
} // DcmMetaInfo::checkAndReadPreamble


// ********************************


BOOL DcmMetaInfo::nextTagIsMeta(DcmStream & inStream)
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


E_Condition DcmMetaInfo::readGroupLength(DcmStream & inStream,
					 const E_TransferSyntax xfer,
					 const DcmTagKey &xtag,
					 const E_GrpLenEncoding gltype,
					 Uint32 & headerLen,
					 Uint32 & bytesRead,
					 const Uint32 maxReadLength)
{
    Bdebug((4, "dcmetinf:DcmMetaInfo::readGroupLength(xfer=%d,...)",
	    xfer ));

    E_Condition l_error = EC_TagNotFound;
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
					      gltype,
					      maxReadLength);
	    bytesRead += newValueLength;

	    if (l_error == EC_Normal && newTag.getXTag() == xtag &&
		elementList->get() != NULL && newValueLength > 0 )
	    {
                headerLen = ((DcmUnsignedLong*)
			     (elementList->get()))->
		    get((const unsigned long)0L);
		l_error = EC_Normal;
		debug((4, "Group Length of File Meta Header=%d", headerLen+bytesRead));

	    }
	    else
            {
		l_error = EC_CorruptedData;
                cerr << "Warning: DcmMetaInfo::readGroupLength(): No Group"
		    " Length available in Meta Information Header"
		     << endl;
            }
	}
	else
	    l_error = EC_InvalidStream;
    }
    debug((4, "readGroupLength() returns error=%d (=2 if no Meta)", l_error));
    Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmMetaInfo::read(DcmStream & inStream,
			      const E_TransferSyntax xfer,
			      const E_GrpLenEncoding gltype,
			      const Uint32 maxReadLength)
{
    Bdebug((3, "dcmetinf:DcmMetaInfo::readBlock(xfer=%d,gltype=%d)",
	    xfer, gltype ));

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
		    // ueberspringe gegebenenfalls 132 Byte
		    Vdebug((3, inStream.Tell() != 0, "found %ld bytes preamble", 
			    inStream.Tell()));
		}
		else
		    newxfer = xfer;

		if (fPreambleTransferState == ERW_ready)
		{
		    Xfer = newxfer;	// speichere intern Parameter-TSyntax xfer
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
					    DCM_MetaElementGroupLength, gltype,
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
			    break;			   // beende while-Schleife

			lastElementComplete = FALSE;
			errorFlag = DcmItem::readSubElement(inStream, newTag,
							    newValueLength,
							    newxfer, gltype,
							    maxReadLength);
			if ( errorFlag == EC_Normal )
			    lastElementComplete = TRUE;
		    }
		    else
		    {
			errorFlag = elementList->get()->read(inStream, xfer, 
							     gltype, 
							     maxReadLength);
			if ( errorFlag == EC_Normal )
			    lastElementComplete = TRUE;
		    }
		    fTransferredBytes = inStream.Tell() - fStartPosition;

		    if (errorFlag != EC_Normal)
			break;			    // beende while-Schleife

		} //while 
	    } 

	    if ( errorFlag == EC_TagNotFound )
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
		    cerr << "Warning: DcmMetaInfo::readBlock(): "
			"Group Length of Meta"
			" Information Header has incorrect Value!"
			 << endl;
		}
		fTransferState = ERW_ready;	     // MetaInfo ist komplett
	    }
	}
    }
    debug(( 3, "errorFlag=(%d)", errorFlag ));
    Edebug(());

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
    DcmItem::transferInit();
    fPreambleTransferState = ERW_notInitialized;
}


// ********************************


E_Condition DcmMetaInfo::write(DcmStream & outStream,
			       const E_TransferSyntax oxfer,
			       const E_EncodingType enctype,
			       const E_GrpLenEncoding gltype)
{
    Bdebug((3, "DcmMetaInfo::writeBlock(oxfer=%d,enctype=%d,gltype=%d)",
	    oxfer, enctype, gltype ));

    if (fTransferState == ERW_notInitialized)
	errorFlag = EC_IllegalCall;
    else
    {
	E_TransferSyntax outxfer = META_HEADER_DEFAULT_TRANSFERSYNTAX;
	errorFlag = outStream.GetError();
	if (errorFlag == EC_Normal && fTransferState != ERW_ready)
	{
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
			
	    if (!elementList->empty() && fTransferState == ERW_inWork )
	    {
		DcmObject *dO;
		do 
		{
		    dO = elementList->get();
		    errorFlag = dO->write(outStream, outxfer, enctype, gltype);
		} while (errorFlag == EC_Normal && elementList->seek(ELP_next));
	    }

	    if (errorFlag == EC_Normal && fTransferState == ERW_inWork)
		fTransferState = ERW_ready;
	}
    }
    Edebug(());

    return errorFlag;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcmetinf.cc,v $
** Revision 1.5  1996-01-29 13:38:27  andreas
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
**
*/

