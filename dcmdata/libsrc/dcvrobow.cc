/*
**
** Author: Gerd Ehlers      Created:  05-05-94
**                          Modified: 02-07-95
**
** Module: dcvrobow.cc
**
** Purpose:
** Implementation of class DcmOtherByteOtherWord
**
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1995-11-23 17:03:07 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrobow.cc,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/



#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

#include "dcvrobow.h"
#include "dcswap.h"
#include "dcstream.h"
#include "dcdebug.h"



// ********************************


DcmOtherByteOtherWord::DcmOtherByteOtherWord( const DcmTag &tag,
					      T_VR_UL len,
					      iDicomStream *iDStream )
    : DcmElement( tag, len, iDStream )
{
Bdebug((5, "dcvrobow:DcmOtherByteOtherWord::DcmOtherByteOtherWord(DcmTag&,len=%ld,*iDS)",
	   len ));
debug(( 8, "Object pointer this=0x%p", this ));

    OtherValue = NULL;
    mustSwap = FALSE;
    swappedToWord = FALSE;
Edebug(());

}


// ********************************


DcmOtherByteOtherWord::DcmOtherByteOtherWord( const DcmOtherByteOtherWord& old )
    : DcmElement( old )
{
Bdebug((5, "dcvrobow:DcmOtherByteOtherWord::DcmOtherByteOtherWord("
           "DcmOtherByteOtherWord&)"));
debug(( 8, "Object pointer this=0x%p", this ));

    switch ( old.ident() ) {
    case EVR_OB:
    case EVR_OW:
    case EVR_ox:
    case EVR_UNKNOWN:
	mustSwap      = old.mustSwap;
	swappedToWord = old.swappedToWord;
	if ( Length == 0 || old.OtherValue == NULL ) {
	    if ( valueInMemory )
		Length = 0;
	    OtherValue = NULL;
	} else {
	    OtherValue = new BYTE[ Length ];
	    memcpy( OtherValue, old.OtherValue, (int)Length );
	}
        break;
    default:
	OtherValue = NULL;
	errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmOtherByteOtherWord: wrong use of Copy-Constructor"
             << endl;
        break;
    }
Edebug(());

}


// ********************************


DcmOtherByteOtherWord::~DcmOtherByteOtherWord()
{
Bdebug((5, "dcvrobow:DcmOtherByteOtherWord::~DcmOtherByteOtherWord()"));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( OtherValue != NULL )
	delete OtherValue;
Edebug(());

}


// ********************************


E_Condition DcmOtherByteOtherWord::setVR( DcmEVR vr )
{
    errorFlag = EC_Normal;
    DcmEVR oldVR = Tag->getEVR();
    switch ( vr )
    {
	case EVR_ox:
	case EVR_OB:
	    Tag->setVR( vr );
	    break;
	case EVR_OW:
	    {
		Tag->setVR( vr );
		E_ByteOrder localByteOrder = ConvXferToByteOrder( Xfer );
		mustSwap = ( MachineByteOrder() != localByteOrder );
	    }
	    break;
	default:
	    errorFlag = EC_InvalidVR;
	    break;
    }
    if ( oldVR != Tag->getEVR() )
	valueModified = TRUE;
    return errorFlag;
}



// ********************************


DcmEVR DcmOtherByteOtherWord::ident() const
{
    return Tag->getEVR();
}


// ********************************


void DcmOtherByteOtherWord::print( int level )
{
    if ( OtherValue != NULL )
    {
	T_VR_UL mlen = (100 < Length) ? 100 : Length;
	char *ch_words;
	char *tmp = ch_words = new char[ mlen*5 + 6 ];
	unsigned int i;
	if ( Tag->getEVR() == EVR_OW )
	{
	    swapValueToWord( TRUE );
	    T_VR_US *tword = (T_VR_US*)OtherValue;
	    for ( i=0; i<mlen/sizeof(T_VR_US); i++)
	    {
		sprintf( tmp, "%4.4x\\", *tword );
		tmp += 5;
		tword++;
	    }
	}
	else
	{
	    swapValueToWord( FALSE );
	    BYTE *tchar = (BYTE*)OtherValue;
	    for ( i=0; i<mlen/sizeof(BYTE); i++)
	    {
		sprintf( tmp, "%2.2x\\", *tchar);
		tmp += 3;
		tchar++;
	    }
	}
	if ( Length > 0 )
	    tmp--;
	*tmp = '\0';
	DcmObject::printInfoLine( level, ch_words );
	delete ch_words;
    }
    else if ( valueInMemory == TRUE )
    {
	DcmObject::printInfoLine( level, "(no value available)" );
    }
    else
    {
	DcmObject::printInfoLine( level, "(not loaded)" );
    }
}


// ********************************


T_VR_UL DcmOtherByteOtherWord::getVM()
{
    return 1L;
}


// ********************************


void DcmOtherByteOtherWord::swapValueToWord( BOOL toWord )
{
    if ( OtherValue != NULL && mustSwap == TRUE )
    {
	if ( toWord && !swappedToWord )
	{
	    SwapN( (char*)OtherValue,
		   sizeof(T_VR_US),
		   (int)Length/sizeof(T_VR_US) );
	    swappedToWord = TRUE;
	}
	else if ( !toWord && swappedToWord )
	{
	    SwapN( (char*)OtherValue,
		   sizeof(T_VR_US),
		   (int)Length/sizeof(T_VR_US) );
	    swappedToWord = FALSE;
	}
    }
}


// ********************************


E_Condition DcmOtherByteOtherWord::alignValue()
{
	// Hinweis: darf nicht in this->write() verwendet werden,
	//	    da Length veraendert wird,
	//	    sondern muss vorher benutzt werden
	//	    z.B. in read() und put()

    E_Condition l_error = EC_Normal;
    if ( Tag->getEVR() != EVR_OW && Length != 0L && OtherValue != NULL )
    {
	T_VR_UL Length_al = ((Length+1) & 0xfffffffe);
	if ( Length < Length_al )
	{
	    BYTE *str = (BYTE*)OtherValue;
	    str[Length] = '\0';
	    Length = Length_al;
	}
    }
    return l_error;
}


// ********************************


E_Condition DcmOtherByteOtherWord::readValueField( E_TransferSyntax /*xfer*/ )
{
Bdebug((4, "dcvrobow:DcmOtherByteOtherWord::readValueField(xfer)" ));
Vdebug((1, !iDS->good(), "Warning: before read: iDS->rdstate()=(0x%x)",
			 iDS->rdstate() ));

    E_Condition l_error = EC_Normal;
    long actpos;
    iDS->clear();
    if ( iDS->good() && iDS->fromFile() )
    {
	// hier kein!! iDS->setDataByteOrder() , weil swapping spaeter
	if ( OtherValue != NULL )
	    delete OtherValue;
	OtherValue = new BYTE[ Length + 1 ];
	actpos = iDS->tellg();
	iDS->seekg( offsetInFile );
	iDS->read( (BYTE*)OtherValue, (int)Length ); // Warnung: evtl. int=16bit

Vdebug((1, !iDS->good(), "Warning: iDS->rdstate()=(0x%x)", iDS->rdstate() ));

        if ( Length != iDS->gcount() )
        {
            Length = iDS->gcount();                  // falls Datenblock zuende
            cerr << "Error: DcmOtherByteOtherWord::readValueField() "
                    "unexpected EndOfStream, state=("
                 << iDS->rdstate() << ")" << endl;
            iDS->clear();
            l_error = EC_CorruptedData;
        }
        iDS->seekg( actpos );
	valueInMemory = TRUE;
	valueModified = FALSE;
	alignValue();
    }
    else
        l_error = EC_InvalidStream;
Vdebug((4, !valueInMemory, "Value not loaded: in file at pos=[0x%8.8x]",
			   offsetInFile ));
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmOtherByteOtherWord::read( E_TransferSyntax xfer,
                                         E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrobow:DcmOtherByteOtherWord::read()" ));

    Xfer = xfer;
    errorFlag = EC_Normal;
    if ( iDS == (iDicomStream*)NULL )
        errorFlag = EC_InvalidStream;
    else if ( iDS->eof() )
	errorFlag = EC_EndOfFile;
    else if ( !iDS->good() )
	errorFlag = EC_InvalidStream;
    else
    {
	if ( iDS->fromFile() && Length > iDS->maximumLength() )
	{
	    offsetInFile = iDS->tellg();
	    iDS->seekg( offsetInFile + Length );
	    if ( !iDS->good() )
		errorFlag = EC_EndOfFile;
	    valueInMemory = FALSE;
	}
	else
	{
	    // hier kein!! iDS->setDataByteOrder() , weil swapping spaeter
	    if ( OtherValue != NULL )
		delete OtherValue;
	    OtherValue = new BYTE[ Length + 1 ];
	    iDS->read( (BYTE*)OtherValue, (int)Length ); // Wrn.: evtl. int=16bit

            if ( ( Length != 0 ) && ( Length != iDS->gcount() ) )
            {
                Length = iDS->gcount();              // falls Datenblock zuende
                cerr << "Error: DcmOtherByteOtherWord::read() "
                        "unexpected EndOfStream, state=("
                     << iDS->rdstate() << ")" << endl;
                iDS->clear();
                errorFlag = EC_CorruptedData;
            }
	    valueInMemory = TRUE;
	    valueModified = FALSE;
	    alignValue();
	}
Vdebug((3, !valueInMemory, "Value not loaded: in file at pos=[0x%8.8x]",
			   offsetInFile ));
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmOtherByteOtherWord::readBlock( E_TransferSyntax xfer,
                                              E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrobow:DcmOtherByteOtherWord::readBlock()" ));

    Xfer = xfer;
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
	    // hier kein!! iDS->setDataByteOrder() , weil swapping spaeter
	    if ( OtherValue != NULL )
		delete OtherValue;
	    OtherValue = new BYTE[ Length + 1 ];
	    rdStat = ERW_inWork;
	}
	T_VR_UL len = (Length - bytesRead) <= iDS->buffered()
		      ? (Length - bytesRead)
		      : iDS->buffered();
	iDS->read( &((BYTE*)OtherValue)[ bytesRead ], (int)len );
					     // Warnung: evtl. int=16bit
	bytesRead += len;
	if ( bytesRead == Length )
	{
	    rdStat = ERW_ready;
	    alignValue();
	}
	else
	{
	    errorFlag = EC_InvalidStream;
	}
	valueInMemory = TRUE;
	valueModified = FALSE;
    }
debug(( 3, "errorFlag=(%d) bytesRead=%d Length=%d",
	   errorFlag, bytesRead, Length ));
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmOtherByteOtherWord::write( oDicomStream &oDS,
					  E_TransferSyntax oxfer,
                                          E_EncodingType /*enctype*/,
                                          E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrobow:DcmOtherByteOtherWord::write(&oDS,oxfer,enctype,gltype)" ));

    errorFlag = EC_Normal;
    E_Condition l_error = EC_Normal;
    if ( !oDS.good() )
	errorFlag = EC_InvalidStream;
    else
    {
        oDS.setDataByteOrder( oxfer );
	if ( valueInMemory == FALSE )
	{					// Hier Daten aus iDS nachladen
            l_error = readValueField( Xfer );   // Xfer gilt nur fuer iDS
	}
	if ( OtherValue == NULL )
	    Length = 0; 		  // Daten konnten nicht gelesen werden

	T_VR_UL written_bytes;
        errorFlag = DcmObject::writeTagAndLength( oDS, oxfer, &written_bytes );
debug(( 3, "Header_Bytes_written              =[0x%8.8x]", written_bytes ));
Vdebug((3, errorFlag, "Warning: error on writing header=(%d)", errorFlag ));

	if ( OtherValue != NULL && errorFlag == EC_Normal )
	{
	    if ( Tag->getEVR() == EVR_OW )
	    {					// speichere als OtherWord
		swapValueToWord( TRUE );
                oDS.write( (T_VR_US*)OtherValue, (int)Length/sizeof(T_VR_US) );
						// Warnung: evtl. int=16bit
debug(( 3, "Bytes_written  as words           =[0x%8.8x]", written_bytes ));
	    }
	    else
	    {					// speichere als OtherByte
		swapValueToWord( FALSE );
                oDS.write( (BYTE*)OtherValue, (int)Length );
						// Warnung: evtl. int=16bit
debug(( 3, "Bytes_written  as bytes           =[0x%8.8x]", written_bytes ));
	    }
	} // OtherValue != NULL
	written_bytes += Length;

	valueInMemory = TRUE;
	valueModified = FALSE;
    }
Edebug(());

    if ( errorFlag == EC_Normal )     // bisher noch kein Fehler
    {
        errorFlag = l_error;          // merke status von readValueField()
        return EC_Normal;
    }
    return errorFlag;
}


// ********************************


E_Condition DcmOtherByteOtherWord::writeBlock( oDicomStream &oDS,
					       E_TransferSyntax oxfer,
                                               E_EncodingType /*enctype*/,
                                               E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrobow:DcmOtherByteOtherWord::writeBlock(&oDS,oxfer,enctype,gltype)" ));

    errorFlag = EC_Normal;
    if ( !oDS.good() || oDS.intoFile() )
	errorFlag = EC_InvalidStream;
    else
    {
	if ( writeState() != ERW_ready )
	{
	    if ( writeState() == ERW_init )
	    {
                if ( oDS.avail() >= 12 )     // n.B.: Header ist u. U. kleiner
		{
		    if ( valueInMemory == FALSE )
		    {			      // Hier Daten aus iDS nachladen
			errorFlag = readValueField( Xfer );
		    }			      // Xfer gilt nur fuer iDS
		    if ( OtherValue == NULL )
			Length = 0;	   // Daten konnten nicht gelesen werden

		    T_VR_UL written_bytes = 0;
		    errorFlag = DcmObject::writeTagAndLength( oDS,
							      oxfer,
							      &written_bytes );
		    if ( errorFlag == EC_Normal )
			wrStat = ERW_inWork;
		}
		else
		    errorFlag = EC_InvalidStream;
	    }
	    if (    OtherValue != NULL
		 && errorFlag == EC_Normal
		 && writeState() != ERW_init )
	    {
		if ( Tag->getEVR() == EVR_OW )
		{				     // speichere als OtherWord
                    oDS.setDataByteOrder( oxfer );
		    swapValueToWord( TRUE );
                    T_VR_UL len = (Length - bytesWritten) <= oDS.avail()
				  ? (Length - bytesWritten)
                                  : oDS.avail();
		    len -= len % sizeof(T_VR_US);    // schreibe nur ganze Werte
                    oDS.write( &((T_VR_US*)OtherValue)[ bytesWritten/sizeof(T_VR_US) ],
				(int)len / sizeof(T_VR_US) );
						     // Warnung: evtl. int=16bit
		    bytesWritten += len;
debug(( 3, "Bytes_written  as words           =[0x%8.8x]", bytesWritten ));

		}
		else
		{				     // speichere als OtherByte
		    swapValueToWord( FALSE );
                    T_VR_UL len = (Length - bytesWritten) <= oDS.avail()
				  ? (Length - bytesWritten)
                                  : oDS.avail();
                    oDS.write( &((BYTE*)OtherValue)[ bytesWritten ],
				(int)len );
						     // Warnung: evtl. int=16bit
		    bytesWritten += len;
debug(( 3, "Bytes_written  as bytes           =[0x%8.8x]", bytesWritten ));

		}
		if ( bytesWritten == Length )
		    wrStat = ERW_ready;
		else
		    errorFlag = EC_InvalidStream;
	    } // OtherValue != NULL

	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmOtherByteOtherWord::put( BYTE *bytevalue,
					T_VR_UL length )      // number of byte
{
    errorFlag = EC_Normal;
    if ( OtherValue != NULL )
	delete OtherValue;
    Length = ((length+1) & 0xfffffffe);
    if ( bytevalue != (BYTE*)NULL )
    {
	OtherValue = new BYTE[ Length ];
        memcpy( OtherValue, bytevalue, (int)length );
    }
    else
    {
	OtherValue = NULL;
	Length = 0;
	errorFlag = EC_CorruptedData;
    }
    valueInMemory = TRUE;
    valueModified = TRUE;
    alignValue();
    return errorFlag;
}


// ********************************


E_Condition DcmOtherByteOtherWord::put( T_VR_US *wordvalue,
					T_VR_UL length )      // number of word
{
    errorFlag = EC_Normal;
    if ( OtherValue != NULL )
	delete OtherValue;
    Length = length * sizeof(T_VR_US);
    if ( wordvalue != (T_VR_US*)NULL )
    {
	OtherValue = new BYTE[ Length ];
	T_VR_US *words = (T_VR_US*)OtherValue;
	memcpy( words, wordvalue, (int)Length );
    }
    else
    {
	OtherValue = NULL;
	Length = 0;
	errorFlag = EC_CorruptedData;
    }
    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


BYTE* DcmOtherByteOtherWord::getBytes()
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( OtherValue == NULL )
	Length = 0;			   // Daten konnten nicht gelesen werden
    if ( Tag->getEVR() != EVR_OW )
	swapValueToWord( FALSE );
    return (BYTE*)OtherValue;
}


// ********************************


T_VR_US* DcmOtherByteOtherWord::getWords()
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( OtherValue == NULL )
	Length = 0;			   // Daten konnten nicht gelesen werden
    if ( Tag->getEVR() == EVR_OW )
	swapValueToWord( TRUE );
    return (T_VR_US*)OtherValue;
}


// ********************************


E_Condition DcmOtherByteOtherWord::clear()
{
Bdebug((2, "dcvrobow:DcmOtherByteOtherWord::clear()"));

    errorFlag = EC_Normal;
    if ( OtherValue != NULL )
	delete OtherValue;
    OtherValue = NULL;
    Length = 0;
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmOtherByteOtherWord::verify( BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( autocorrect == TRUE )
	alignValue();
    return errorFlag;
}


// ********************************


E_Condition DcmOtherByteOtherWord::loadAllDataIntoMemory()
{
    E_Condition l_error = EC_Normal;
    if ( valueInMemory == FALSE )
    {                                       // Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
        if ( OtherValue == NULL )
            Length = 0;                     // Daten konnten nicht gelesen
                                            // werden, Laenge korrigieren,
                                            // errorFlag enthaelt bereits
                                            // Fehlercode
        if ( Tag->getEVR() != EVR_OW )
            swapValueToWord( FALSE );
        l_error = errorFlag;                // errorFlag nur bei Fehler aendern
    }
    return l_error;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrobow.cc,v $
** Revision 1.2  1995-11-23 17:03:07  hewett
** Updated for loadable data dictionary.  Some cleanup (more to do).
**
**
*/
