/*
 *
 * Author: Gerd Ehlers      Created:  05-15-94
 *                          Modified: 02-15-95
 *
 * Module: dcmetinf.cc
 *
 * Purpose:
 * Implementation of the class DcmMetaInfo
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <memory.h>
#include <iostream.h>

#include "dcmetinf.h"
#include "dcitem.h"
#include "dcxfer.h"
#include "dcvrul.h"
#include "dcdebug.h"


extern char *VRTypesName[];
extern short DIM_OF_VRTypes;


// ********************************


DcmMetaInfo::DcmMetaInfo()
    : DcmItem( InternalUseTag )
{
Bdebug((5, "dcmetinf:DcmMetaInfo::DcmMetaInfo()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    setPreamble();
Edebug(());

}


// ********************************


DcmMetaInfo::DcmMetaInfo( iDicomStream *iDStream )
    : DcmItem( InternalUseTag, UNDEF_LEN, iDStream )
{
Bdebug((5, "dcmetinf:DcmMetaInfo::DcmMetaInfo(*iDS)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    setPreamble();
Edebug(());

}


// ********************************


DcmMetaInfo::DcmMetaInfo( const DcmObject &oldObj )
    : DcmItem( InternalUseTag )
{
Bdebug((5, "dcmetinf:DcmMetaInfo::DcmMetaInfo(DcmObject&)" ));
debug(( 8, "Object pointer this=0x%p", this ));
debug(( 5, "ident()=%d", oldObj.ident() ));

    if ( oldObj.ident() == EVR_metainfo )
    {
	DcmMetaInfo const *old = (DcmMetaInfo const *)&oldObj;
	preambleUsed = ((DcmMetaInfo const *)old)->preambleUsed;
	memcpy( filePreamble,
		((DcmMetaInfo const *)old)->filePreamble,
		128 );
	*Tag = *old->Tag;
	iDS = old->iDS;
	offsetInFile  = old->offsetInFile;
	valueInMemory = old->valueInMemory;
	valueModified = old->valueModified;
	Length = old->Length;
	Xfer = old->Xfer;
	if ( !old->elementList->empty() )
	{
	    DcmObject *oldDO;
	    DcmObject *newDO;
	    elementList->seek( ELP_first );
	    old->elementList->seek( ELP_first );
	    do {
		oldDO = old->elementList->get();
		newDO = DcmItem::copyDcmObject( oldDO );

		elementList->insert( newDO, ELP_next );
	    } while ( old->elementList->seek( ELP_next ) );
	}
    }
    else
    {
        cerr << "Warning: DcmMetaInfo: wrong use of Copy-Constructor"
             << endl;
	setPreamble();
    }
Edebug(());

}


// ********************************


DcmMetaInfo::DcmMetaInfo( const DcmMetaInfo &oldMeta )
    : DcmItem( InternalUseTag )
{
Bdebug((5, "dcmetinf:DcmMetaInfo::DcmMetaInfo(DcmMetaInfo&)" ));
debug(( 8, "Object pointer this=0x%p", this ));
debug(( 5, "ident()=%d", oldMeta.ident() ));

    if ( oldMeta.ident() == EVR_metainfo )
    {
	DcmMetaInfo const *old = &oldMeta;
	preambleUsed = ((DcmMetaInfo const *)old)->preambleUsed;
	memcpy( filePreamble,
		((DcmMetaInfo const *)old)->filePreamble,
		128 );
	*Tag = *old->Tag;
	iDS = old->iDS;
	offsetInFile  = old->offsetInFile;
	valueInMemory = old->valueInMemory;
	valueModified = old->valueModified;
	Length = old->Length;
	Xfer = old->Xfer;
	if ( !old->elementList->empty() )
	{
	    DcmObject *oldDO;
	    DcmObject *newDO;
	    elementList->seek( ELP_first );
	    old->elementList->seek( ELP_first );
	    do {
		oldDO = old->elementList->get();
		newDO = DcmItem::copyDcmObject( oldDO );

		elementList->insert( newDO, ELP_next );
	    } while ( old->elementList->seek( ELP_next ) );
	}
    }
    else
    {
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


EVR DcmMetaInfo::ident() const
{
    return EVR_metainfo;
}


// ********************************


void DcmMetaInfo::print( int level )
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
    memset( filePreamble, 0, sizeof(filePreamble) );
    preambleUsed = FALSE;
}


// ********************************


BOOL DcmMetaInfo::checkAndReadPreamble( E_TransferSyntax *newxfer )  // out
{
Bdebug((4, "dcmetinf:DcmMetaInfo::checkAndReadPreamble()" ));

    BOOL retval = FALSE;
    int preambleLen = DICOM_PREAMBLE_LEN + DICOM_MAGIC_LEN;
    iDS->read( filePreamble, preambleLen );

    if ( iDS->gcount() != preambleLen )
    {					     // Datei zu kurz => keine Preamble
	preambleLen = (int)iDS->gcount();
	iDS->clear();
	for ( int i=preambleLen-1; i>=0; i--)
	    iDS->putback( filePreamble[i] );
debug(( 4, "No Preamble available: File too short (%d) < %d bytes",
	   preambleLen, DICOM_PREAMBLE_LEN + DICOM_MAGIC_LEN ));

	retval = FALSE;
	DcmMetaInfo::setPreamble();
    }
    else				   // pruefe Preamble und DicomPrefix
    {					   // setze prefix auf richtige Position
	char *prefix = filePreamble + DICOM_PREAMBLE_LEN;
	if ( memcmp(prefix, DICOM_MAGIC, DICOM_MAGIC_LEN) == 0 )
	    retval = TRUE;			      // Preamble vorhanden!
	else
	{					      // keine Preamble
	    retval = FALSE;
	    DcmMetaInfo::setPreamble();
	    for ( int i=preambleLen-1; i>=0; i--)
		iDS->putback( filePreamble[i] );
	}
    }

    E_TransferSyntax tmpxfer = checkTransferSyntax();
    DcmXfer tmpxferSyn( tmpxfer );
    DcmXfer xferSyn( *newxfer );
    if (    (	 tmpxferSyn.isExplicitVR()
	      &&    xferSyn.isImplicitVR()
	    )
	 || (	 tmpxferSyn.isImplicitVR()
	      &&    xferSyn.isExplicitVR()
	    )
	 || xferSyn.getXfer() == EXS_UNKNOWN
       )
    {
	*newxfer = tmpxferSyn.getXfer();   // benutze eigene, ermittelte xfer
	if ( xferSyn.getXfer() != EXS_UNKNOWN )
            cerr << "Info: DcmMetaInfo::checkAndReadPreamble(): TransferSyntax"
                    " of MetaInfo is different from passed parameter *newxfer:"
		    " ignoring *newxfer!"
		 << endl;
    }
    else
	*newxfer = xferSyn.getXfer();


Vdebug((4, retval==TRUE, "found Preamble=[0x%8.8x]", (T_VR_UL)(*filePreamble) ));
Vdebug((4, retval==FALSE, "No Preambel found!" ));
debug(( 4, "TransferSyntax(-1..3)=(%d)", *newxfer ));
Edebug(());

    return retval;
} // DcmMetaInfo::checkAndReadPreamble


// ********************************


BOOL DcmMetaInfo::nextTagIsMeta()
{
    char testbytes[2];
    iDS->read( testbytes, 2 );
    for ( int i=1; i>=0; i--)
	iDS->putback( testbytes[i] );

    if (   (	testbytes[0] == 0x02	      // teste nur auf Gruppe 0x0002
	     && testbytes[1] == 0x00
	   )
	|| (	testbytes[0] == 0x00
	     && testbytes[1] == 0x02
	   )
       )
    {
	return TRUE;
    }
    else
    {
	return FALSE;
    }
}


// ********************************


E_Condition DcmMetaInfo::readGroupLength( E_TransferSyntax xfer,
					  ETag xtag,
                                          E_GrpLenEncoding gltype,
					  T_VR_UL *headerLen,
					  T_VR_UL *bytes_read )
{
Bdebug((4, "dcmetinf:DcmMetaInfo::readGroupLength(xfer=%d,...)",
	   xfer ));

    E_Condition l_error = EC_TagNotFound;
    E_TransferSyntax newxfer = xfer;
    *bytes_read = 0;
    *headerLen = 0;

    if ( DcmMetaInfo::nextTagIsMeta() )
    {
	DcmTag newTag( ET_UNKNOWN );
	T_VR_UL newValueLength = 0;
	T_VR_UL bytes_tagAndLen = 0;
	l_error = DcmItem::readTagAndLength( newxfer,
                                             newTag,
                                             &newValueLength,
                                             &bytes_tagAndLen );
	*bytes_read += bytes_tagAndLen;

	if ( iDS->good() && l_error == EC_Normal )
	{
            l_error = DcmItem::readSubElement( newTag,
                                               newValueLength,
                                               bytes_tagAndLen,
                                               newxfer,
                                               gltype );
	    *bytes_read += newValueLength;

	    if (    l_error == EC_Normal
		 && newTag.getXTag() == xtag
		 && elementList->get() != (DcmElement*)NULL
		 && newValueLength > 0 )
	    {
                *headerLen = ((DcmUnsignedLong*)(elementList->get()))->get(0L);
		l_error = EC_Normal;
debug(( 4, "Group Length of File Meta Header=%d", *headerLen + *bytes_read ));

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
debug(( 4, "readGroupLength() returns error=%d (=2 if no Meta)", l_error ));
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmMetaInfo::read( E_TransferSyntax xfer,
                               E_GrpLenEncoding gltype )
{
Bdebug((3, "dcmetinf:DcmMetaInfo::read(xfer=%d,gltype=%d)",
           xfer, gltype ));

    E_TransferSyntax newxfer = EXS_UNKNOWN;
    T_VR_UL bytes_read = 0;
    valueModified = FALSE;

    errorFlag = EC_Normal;
    if ( iDS == (iDicomStream*)NULL )
        errorFlag = EC_InvalidStream;
    else if ( iDS->eof() )
	errorFlag = EC_EndOfFile;
    else if ( !iDS->good() )
	errorFlag = EC_InvalidStream;
    else
    {
	if ( xfer == EXS_UNKNOWN )
	{
	    preambleUsed = checkAndReadPreamble( &newxfer );
					// ueberspringe gegebenenfalls 132 Byte
Vdebug((1, iDS->tellg() != 0 && iDS->tellg() != 132,
           "warning: DcmMetaInfo::read():iDS->tellg()=%d (0,132)",
	   iDS->tellg() ));

	}
	else
	    newxfer = xfer;
	Xfer = newxfer;
	offsetInFile = iDS->tellg();	// Position von MetaInfo-Value
	iDS->setDataByteOrder( newxfer );

	T_VR_UL headerLength = 0;
	BOOL headerLengthFound = FALSE;
					// werte Laenge aus Tag(0002,0000) aus
	errorFlag = readGroupLength( newxfer,
				     ET_GroupLength0002,
                                     gltype,
				     &headerLength,
				     &bytes_read );
	if ( errorFlag == EC_TagNotFound )
        {
	    errorFlag = EC_Normal;	// es existiert kein Meta-Header
            Xfer = EXS_UNKNOWN;
        }
	else if ( errorFlag != EC_InvalidStream )
	{
	    if ( errorFlag == EC_Normal )
	    {
		Length = headerLength + bytes_read;
		headerLengthFound = TRUE;
	    }
            else
                Length = UNDEF_LEN;

	    while (    iDS->good()
		    && (    (	 headerLengthFound
			      && bytes_read < Length
			    )
			 || (	 !headerLengthFound
			      && DcmMetaInfo::nextTagIsMeta()
			    )
		       )
		  )
	    {
		DcmTag newTag( ET_UNKNOWN );
		T_VR_UL newValueLength = 0;
		T_VR_UL bytes_tagAndLen = 0;
		errorFlag = DcmItem::readTagAndLength( newxfer,
                                                       newTag,
						       &newValueLength,
						       &bytes_tagAndLen );
		bytes_read += bytes_tagAndLen;
debug(( 3, "Bytes_read                        =[0x%8.8x]", bytes_read ));

		if ( !iDS->good() )		// evtl. Stream zuende
		    break;			// beende while-Schleife

                errorFlag = DcmItem::readSubElement( newTag,
						     newValueLength,
						     bytes_tagAndLen,
                                                     newxfer,
                                                     gltype );
		bytes_read = (T_VR_UL)(iDS->tellg() - offsetInFile);
debug(( 3, "Bytes_read                        =[0x%8.8x]", bytes_read ));

		if ( errorFlag != EC_Normal )
		    break;			// beende while-Schleife

	    } //while ( iDS->good()..
            if (    headerLengthFound
                 && bytes_read != Length
               )
            {
                cerr << "Warning: DcmMetaInfo::read(): Group Length of Meta"
                        " Information Header has incorrect Value: "
                     << headerLength << " correct: "
                     << bytes_read + headerLength - Length << endl;
            }
	}
    } // else errorFlag
Edebug(());

    return errorFlag;
}  // DcmMetaInfo::read()


// ********************************


E_Condition DcmMetaInfo::readBlock( E_TransferSyntax xfer,
                                    E_GrpLenEncoding gltype )
{
Bdebug((3, "dcmetinf:DcmMetaInfo::readBlock(xfer=%d,gltype=%d)",
           xfer, gltype ));

    E_TransferSyntax newxfer = xfer;
    valueModified = FALSE;

    errorFlag = EC_Normal;
    if ( iDS == (iDicomStream*)NULL )
        errorFlag = EC_InvalidStream;
    else if ( iDS->eof() || iDS->fromFile() )
	errorFlag = EC_EndOfFile;
    else if ( !iDS->good() )
	errorFlag = EC_InvalidStream;
    else if ( readState() != ERW_ready )
    {
	if ( readState() == ERW_init )
	{
	    offsetInFile = iDS->tellg();    // Position von MetaInfo-Value
	    if ( iDS->buffered() >= 132 )
	    {
		if ( xfer == EXS_UNKNOWN )
		{
		    preambleUsed = checkAndReadPreamble( &newxfer );
					// ueberspringe gegebenenfalls 132 Byte
Vdebug((3, iDS->tellg() != 0, "found %d bytes preamble", iDS->tellg() ));

		}
		else
		    newxfer = xfer;
		Xfer = xfer;	      // speichere intern Parameter-TSyntax xfer
		rdStat = ERW_inWork;
		Length = 0;
	    }
	    else
		errorFlag = EC_InvalidStream;
	}
	else
	    newxfer = Xfer;

	if ( readState() != ERW_init && Length == 0 && iDS->buffered() >= 12 )
	{
	    T_VR_UL headerLength = 0;
	    errorFlag = readGroupLength( newxfer,
					 ET_GroupLength0002,
                                         gltype,
					 &headerLength,
					 &bytesRead );
	    if ( errorFlag == EC_Normal )
		Length = headerLength + bytesRead;
	    else
		Length = UNDEF_LEN;
	}

	if ( readState() != ERW_init && Length != 0 && errorFlag == EC_Normal )
	{
	    iDS->setDataByteOrder( newxfer );
	    while (    iDS->good()
		    && (    (	 Length < UNDEF_LEN
			      && bytesRead < Length
			    )
			 || (	 Length == UNDEF_LEN
			      && DcmMetaInfo::nextTagIsMeta()
			    )
			 || !lastElementComplete
		       )
		  )
	    {
		DcmTag newTag( ET_UNKNOWN );
		T_VR_UL newValueLength = 0;
		T_VR_UL bytes_tagAndLen = 0;
		if ( lastElementComplete )
		{
		    errorFlag = DcmItem::readTagAndLength( newxfer,
                                                           newTag,
							   &newValueLength,
							   &bytes_tagAndLen	);
		    bytesRead += bytes_tagAndLen;
                    if ( errorFlag != EC_Normal )
			break;			   // beende while-Schleife

		    lastElementComplete = FALSE;
                    errorFlag = DcmItem::readSubElement( newTag,
							 newValueLength,
							 bytes_tagAndLen,
                                                         newxfer,
                                                         gltype );
		    if ( errorFlag == EC_Normal )
			lastElementComplete = TRUE;
		}
		else
		{
                    errorFlag = elementList->get()->readBlock( xfer, gltype );
		    if ( errorFlag == EC_Normal )
			lastElementComplete = TRUE;
		}
		bytesRead = (T_VR_UL)(iDS->tellg() - offsetInFile);

		if ( errorFlag != EC_Normal )
		    break;			    // beende while-Schleife

	    } //while ( iDS->good()..
	} // if ( readState() != ERW_init )
    } // else errorFlag
    valueModified = FALSE;
    if ( errorFlag == EC_TagNotFound )
    {
        errorFlag = EC_Normal;      // es existiert kein Meta-Header
        Xfer = EXS_UNKNOWN;
    }
    else if ( errorFlag == EC_SequEnd )
	errorFlag = EC_Normal;
    if ( errorFlag == EC_Normal )
    {
        if (    Length != UNDEF_LEN
             && bytesRead != Length
           )
        {
            cerr << "Warning: DcmMetaInfo::readBlock(): Group Length of Meta"
                    " Information Header has incorrect Value!"
                 << endl;
        }
	rdStat = ERW_ready;	     // MetaInfo ist komplett
    }
debug(( 3, "errorFlag=(%d)", errorFlag ));
Edebug(());

    return errorFlag;
} // DcmItem::readBlock()


// ********************************


E_Condition DcmMetaInfo::write( oDicomStream &oDS,
				E_TransferSyntax oxfer,
                                E_EncodingType enctype,
                                E_GrpLenEncoding gltype )
{
Bdebug((3, "dcmetinf:DcmMetaInfo::write(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
           oxfer, enctype, gltype ));

    E_TransferSyntax outxfer = META_HEADER_DEFAULT_TRANSFERSYNTAX;
    errorFlag = EC_Normal;
    if ( !oDS.good() )
	errorFlag = EC_InvalidStream;
    else
    {
	if ( preambleUsed || !elementList->empty() )
	{
            oDS.write( filePreamble, 128 );
            oDS.write( DICOM_MAGIC, 4 );
	}
	if ( !elementList->empty() )
	{
	    DcmObject *dO;
	    elementList->seek( ELP_first );
	    do {
		dO = elementList->get();
                errorFlag = dO->write( oDS, outxfer, enctype, gltype );
	    } while ( elementList->seek( ELP_next ) && errorFlag == EC_Normal );
	}

	valueModified = FALSE;
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmMetaInfo::writeBlock( oDicomStream &oDS,
				     E_TransferSyntax oxfer,
                                     E_EncodingType enctype,
                                     E_GrpLenEncoding gltype )
{
Bdebug((3, "dcmetinf:DcmMetaInfo::writeBlock(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
           oxfer, enctype, gltype ));

    E_TransferSyntax outxfer = META_HEADER_DEFAULT_TRANSFERSYNTAX;
    errorFlag = EC_Normal;
    if ( !oDS.good() || oDS.intoFile() )
	errorFlag = EC_InvalidStream;
    else
    {
	if ( writeState() != ERW_ready )
	{
	    if ( writeState() == ERW_init )
	    {
		if ( preambleUsed || !elementList->empty() )
		{
                    if ( oDS.avail() >= 132 )
		    {
                        oDS.write( filePreamble, 128 );
                        oDS.write( DICOM_MAGIC, 4 );
			wrStat = ERW_inWork;
			elementList->seek( ELP_first );
		    }
		    else
			errorFlag = EC_InvalidStream;
		}
		else
		    wrStat = ERW_inWork;
	    }
	    if ( !elementList->empty() && writeState() != ERW_init )
	    {
		DcmObject *dO;
		do {
		    dO = elementList->get();
                    errorFlag = dO->writeBlock( oDS, outxfer, enctype, gltype );
		} while (    errorFlag == EC_Normal
			  && elementList->seek( ELP_next ) );
	    }

	    if ( errorFlag == EC_Normal && writeState() != ERW_init )
		wrStat = ERW_ready;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


