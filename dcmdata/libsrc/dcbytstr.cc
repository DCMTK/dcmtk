/*
 *
 * Author: Gerd Ehlers      Created:  05-05-94
 *                          Modified: 02-07-95
 *
 * Module: dcbytstr.cc
 *
 * Purpose:
 * Implementation of class DcmByteString
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>


#include "dcbytstr.h"
#include "dcvr.h"
#include "dcdebug.h"



// ********************************


DcmByteString::DcmByteString( const DcmTag &tag,
			      T_VR_UL len,
			      iDicomStream *iDStream )
    : DcmElement( tag, len, iDStream )
{
Bdebug((6, "dcbytstr:DcmByteString::DcmByteString(DcmTag&,len=%ld,*iDS)",
	   len	));
debug(( 8, "Object pointer this=0x%p", this ));

    ByteStringValue = (char*)NULL;
    paddingChar = ' ';
    maxLength = UNDEF_LEN;
    realLength = 0;
Edebug(());

}


// ********************************


DcmByteString::DcmByteString( const DcmByteString& old, DcmEVR oldIdent )
    : DcmElement( old )
{
Bdebug((6, "dcbytstr:DcmByteString::DcmByteString(DcmObject&,oldIdent=%d)",
	   oldIdent ));
debug(( 8, "Object pointer this=0x%p", this ));

    paddingChar = ' ';
    maxLength = UNDEF_LEN;
    realLength = 0;
    if ( old.ident() == oldIdent )
    {
	realLength = old.realLength;
	if ( old.ByteStringValue == (char*)NULL )
	{
	    if ( valueInMemory )
		Length = 0;
	    realLength = 0;
	    ByteStringValue = (char*)NULL;
	}
	else if ( Length == 0 )
	{
	    realLength = 0;
	    ByteStringValue = new char[ 1 ];
	    ByteStringValue[ 0 ] = '\0';
	}
	else
	{
	    ByteStringValue = new char[ Length + 1 ];
	    memcpy( ByteStringValue,
		    old.ByteStringValue,
		    (int)Length + 1 );
	}
    }
    else
    {
	ByteStringValue = (char*)NULL;
	errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmByteString: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmByteString::~DcmByteString()
{
Bdebug((6, "dcbytstr:DcmByteString::~DcmByteString()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( ByteStringValue != (char*)NULL )
	delete ByteStringValue;
    ByteStringValue = (char*)NULL;
Edebug(());

}


// ********************************


void DcmByteString::print( int level )
{
    if ( ByteStringValue != (char*)NULL )
    {
	char *tmp = new char[ realLength + 3 ];
	tmp[0] = '[';
	strncpy( tmp+1, ByteStringValue, (int)realLength );
	tmp[ realLength + 1 ] = ']';
	tmp[ realLength + 2 ] = '\0';
	DcmObject::printInfoLine( level, tmp );
	delete tmp;
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


T_VR_UL DcmByteString::getVM()
{
    char *s = ByteStringValue;
    T_VR_UL vm;
    if ( ByteStringValue == (char*)NULL || Length == 0 )
	vm = 0;
    else
    {
	vm = 1;
	char c;
	while ((c = *s++) != 0)
	    if ( c == '\\' )
		vm++;
    }
    return vm;
}


// ********************************


E_Condition DcmByteString::alignValue()
{
    // Vorbedingungen:	Length ist eine gerade Zahl
    //			realLength ist hoechstens um 1 kleiner

Bdebug((3, "dcbytstr:DcmByteString::AlignValue() with '0x%2.2x'",
	   paddingChar	));

    E_Condition l_error = EC_Normal;
    if ( realLength != 0 && ByteStringValue != (char*)NULL )
    {
	if ( realLength < Length )
	{
	    ByteStringValue[ realLength ] = paddingChar;
	    ByteStringValue[ realLength + 1 ] = '\0';
	}
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmByteString::deAlignValue()
{
Bdebug((3, "dcbytstr:DcmByteString::deAlignValue()" ));

    E_Condition l_error = EC_Normal;
    if ( Length != 0L && ByteStringValue != (char*)NULL )
    {
	T_VR_UL i = strlen( ByteStringValue );
	while ( i > 0
		&& (	ByteStringValue[ i - 1 ] == paddingChar
		     || ByteStringValue[ i - 1 ] <= ' '
		   )
	      )
	{
	    ByteStringValue[ i - 1 ] = '\0';
	    i--;
	}
debug(( 3, "old Length=%d   new realLength=%d", Length, i ));

	realLength = i;
	Length = ((realLength+1) & 0xfffffffe);
					// setze auf naechste gerade Zahl
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmByteString::readValueField( E_TransferSyntax /*xfer*/ )
{
Bdebug((4, "dcbytstr:DcmByteString::readValueField(xfer)" ));

    E_Condition l_error = EC_Normal;
    long actpos;
    iDS->clear();
    if ( iDS->good() && iDS->fromFile() )
    {
	if ( ByteStringValue != (char*)NULL )
	    delete ByteStringValue;
	ByteStringValue = new char[ Length + 1 ];
	actpos = iDS->tellg();
	iDS->seekg( offsetInFile );
        iDS->read( ByteStringValue, (int)Length );  // Warnung: evtl. int=16bit

Vdebug((1, !iDS->good(), "Warning: iDS->rdstate()=(0x%x)", iDS->rdstate() ));

        if ( Length != iDS->gcount() )
        {
            Length = iDS->gcount();                 // falls Datenblock zuende
            cerr << "Error: DcmByteString::readValueField() "
                    "unexpected EndOfStream, state=("
                 << iDS->rdstate() << ")" << endl;
            iDS->clear();
            l_error = EC_CorruptedData;
        }
	ByteStringValue[ Length ] = '\0';
	iDS->seekg( actpos );
	valueInMemory = TRUE;
	valueModified = FALSE;
        realLength = Length;                 // deAlignValue() jetzt in verify()
        Length = ((Length+1) & 0xfffffffe);  // Length muss immer gerade sein
    }
    else
        l_error = EC_InvalidStream;
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmByteString::read( E_TransferSyntax xfer,
                                 E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcbytstr:DcmByteString::read()" ));

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
	    if ( ByteStringValue != (char*)NULL )
		delete ByteStringValue;
	    ByteStringValue = new char[ Length + 1 ];
	    iDS->read( ByteStringValue, (int)Length ); // Warnung: evtl. int=16bit
	    ByteStringValue[ Length ] = '\0';
	    valueInMemory = TRUE;
	    valueModified = FALSE;
            realLength = Length;             // deAlignValue() jetzt in verify()
            Length = ((Length+1) & 0xfffffffe); // Length muss immer gerade sein
	}
    }
Edebug(());

    return EC_Normal;
}


// ********************************


E_Condition DcmByteString::readBlock( E_TransferSyntax xfer,
                                      E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcbytstr:DcmByteString::readBlock(xfer)" ));

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
	    if ( ByteStringValue != (char*)NULL )
		delete ByteStringValue;
	    ByteStringValue = new char[ Length + 1 ];
	    rdStat = ERW_inWork;
	}
	T_VR_UL len = (Length - bytesRead) <= iDS->buffered()
		      ? (Length - bytesRead)
		      : iDS->buffered();
	iDS->read( &ByteStringValue[ bytesRead ], (int)len );
					     // Warnung: evtl. int=16bit
	bytesRead += len;
	if ( bytesRead == Length )
	{
	    rdStat = ERW_ready;
            realLength = Length;             // deAlignValue() jetzt in verify()
            Length = ((Length+1) & 0xfffffffe); // Length muss immer gerade sein
	}
	else
	{
	    errorFlag = EC_InvalidStream;
	}
	ByteStringValue[ Length ] = '\0';
	valueInMemory = TRUE;
	valueModified = FALSE;
    }
debug(( 3, "errorFlag=(%d) bytesRead=%d Length=%d",
	   errorFlag, bytesRead, Length ));
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmByteString::write( oDicomStream &oDS,
                                  E_TransferSyntax oxfer,
                                  E_EncodingType enctype,
                                  E_GrpLenEncoding gltype )
{
Bdebug((3, "dcbytstr:DcmByteString::write(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
           oxfer, enctype, gltype ));

    errorFlag = EC_Normal;
    E_Condition l_error = EC_Normal;
    if ( !oDS.good() )
	errorFlag = EC_InvalidStream;
    else
    {
	if ( valueInMemory == FALSE )
	{					// Hier Daten aus iDS nachladen
            l_error = readValueField( Xfer );   // Xfer gilt nur fuer iDS
	}
	if ( ByteStringValue == (char*)NULL )
	    Length = 0; 		  // Daten konnten nicht gelesen werden

	T_VR_UL written_bytes;
        errorFlag = DcmObject::writeTagAndLength( oDS, oxfer, &written_bytes );
        if ( errorFlag == EC_Normal )     // bisher noch kein Fehler
            errorFlag = l_error;
debug(( 3, "Header_Bytes_written              =[0x%8.8x]", written_bytes ));

	if ( ByteStringValue != (char*)NULL && errorFlag == EC_Normal )
	{
            alignValue();
            oDS.write( ByteStringValue, (int)Length ); // Warnung: evtl. int=16bit
            ByteStringValue[ realLength ] = '\0'; // DeAligning um genau 1 Byte
	}
	written_bytes += Length;
debug(( 3, "Bytes_written                     =[0x%8.8x]", written_bytes ));

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


E_Condition DcmByteString::writeBlock( oDicomStream &oDS,
				       E_TransferSyntax oxfer,
                                       E_EncodingType enctype,
                                       E_GrpLenEncoding gltype )
{
Bdebug((3, "dcbytstr:DcmByteString::writeBlock(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
           oxfer, enctype, gltype ));

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
		    if ( ByteStringValue == (char*)NULL )
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
	    if (    ByteStringValue != (char*)NULL
		 && errorFlag == EC_Normal
		 && writeState() != ERW_init )
	    {
		if ( bytesWritten == 0 )
		    alignValue();
                T_VR_UL len = (Length - bytesWritten) <= oDS.avail()
			      ? (Length - bytesWritten)
                              : oDS.avail();
                oDS.write( &ByteStringValue[ bytesWritten ], (int)len );
						     // Warnung: evtl. int=16bit
		bytesWritten += len;

		if ( bytesWritten == Length )
		{
		    wrStat = ERW_ready;
                    ByteStringValue[ realLength ] = '\0';
                                                   // DeAligning um genau 1 Byte
		}
		else
		    errorFlag = EC_InvalidStream;
	    } // ByteStringValue != NULL

	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition 
DcmByteString::put( const char *bytestringvalue )
{
    errorFlag = EC_Normal;
    if ( ByteStringValue != (char*)NULL )
	delete ByteStringValue;
    if ( bytestringvalue != (char*)NULL )
    {
        realLength = strlen( bytestringvalue );
        Length = ((realLength+1) & 0xfffffffe);
	ByteStringValue = new char[ Length + 1 ];
        memcpy( ByteStringValue, bytestringvalue, (int)realLength + 1 );
    }
    else
    {
	ByteStringValue = (char*)NULL;
	Length = 0;
	realLength = 0;
	errorFlag = EC_CorruptedData;
    }
    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


const char* 
DcmByteString::get()
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( ByteStringValue == (char*)NULL )
    {
	Length = 0;		      // Daten konnten nicht gelesen werden
        realLength = 0;
    }
    return ByteStringValue;
}


// ********************************


E_Condition DcmByteString::clear()
{
Bdebug((2, "dcbytstr:DcmByteString::clear()"));

    errorFlag = EC_Normal;
    if ( ByteStringValue != (char*)NULL )
	delete ByteStringValue;
    ByteStringValue = (char*)NULL;
    Length = 0;
    realLength = 0;
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmByteString::verify( BOOL autocorrect )
{
Bdebug((3, "dcbytstr:DcmByteString::verify(autocorrect=%d)", autocorrect ));
debug(( 3, "Tag=(0x%4.4x,0x%4.4x) \"%s\" \"%s\"",
	   getGTag(), getETag(),
           DcmVR(getVR()).getVRName(), getTag().getTagName() ));

    errorFlag = EC_Normal;
    if ( ByteStringValue != (char*)NULL && Length != 0 )
    {
	if ( autocorrect == TRUE )
	{
	    char *tempstr = new char[ Length + 1 ];
	    T_VR_UL field = 0;
	    T_VR_UL num = getVM();
	    T_VR_UL pos = 0;
	    T_VR_UL temppos = 0;
	    char c;
	    while ( field < num )
	    {
		T_VR_UL fieldlen = 0;
		while ( ((c = ByteStringValue[pos++]) != 0) && (c != '\\') )
		{
		    if ( fieldlen < maxLength )
			tempstr[temppos++] = c;
		    else
			errorFlag = EC_CorruptedData;
		    fieldlen++;
		}
                tempstr[temppos++] = c;      // c ist entweder '\\' oder NULL
		field++;
		if ( pos > Length )
		    break;
	    }
	    delete ByteStringValue;
	    ByteStringValue = tempstr;
            if ( errorFlag == EC_CorruptedData )
                valueModified = TRUE;
            deAlignValue();                  // einziger Aufruf von deAlignValue
	}
	else
	{
	    T_VR_UL field = 0;
	    T_VR_UL num = getVM();
	    T_VR_UL pos = 0;
	    char c;
	    while ( field < num )
	    {
		T_VR_UL fieldlen = 0;
		while ( ((c = ByteStringValue[pos++]) != 0) && (c != '\\') )
		{
		    fieldlen++;
		}
		if ( fieldlen >= maxLength )
		    errorFlag = EC_CorruptedData;
		field++;
		if ( pos > Length )
		    break;
	    }
	}
    }

Vdebug((3, errorFlag!=EC_Normal,
	   "Illegal values in Tag=(0x%4.4x,0x%4.4x) VM=%d",
	   getGTag(), getETag(), getVM() ));
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmByteString::loadAllDataIntoMemory()
{
    E_Condition l_error = EC_Normal;
    if ( valueInMemory == FALSE )
    {                                       // Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
        if ( ByteStringValue == NULL )
            Length = 0;                     // Daten konnten nicht gelesen
                                            // werden, Laenge korrigieren,
                                            // errorFlag enthaelt bereits
                                            // Fehlercode
        l_error = errorFlag;                // errorFlag nur bei Fehler aendern
    }
    return l_error;
}


// ********************************


