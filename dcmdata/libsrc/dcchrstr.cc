/*
 *
 * Author: Gerd Ehlers      Created:  05-05-94
 *                          Modified: 02-07-95
 *
 * Module: dcchrstr.cc
 *
 * Purpose:
 * Implementation of class DcmCharString
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include "dcchrstr.h"
#include "dcvr.h"
#include "dcdebug.h"



// ********************************


DcmCharString::DcmCharString( DcmTag &tag )
    : DcmElement( tag )
{
Bdebug((6, "dcchrstr:DcmCharString::DcmCharString(DcmTag&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    CharStringValue = (ALT_CHAR*)NULL;
    paddingChar = (ALT_CHAR)' ';
    maxLength = UNDEF_LEN;
    realLength = 0;
Edebug(());

}


// ********************************


DcmCharString::DcmCharString( DcmTag &tag,
			      T_VR_UL len,
			      iDicomStream *iDStream )
    : DcmElement( tag, len, iDStream )
{
Bdebug((6, "dcchrstr:DcmCharString::DcmCharString(DcmTag&,len=%ld,*iDS)",
	   len ));
debug(( 8, "Object pointer this=0x%p", this ));

    CharStringValue = (ALT_CHAR*)NULL;
    paddingChar = (ALT_CHAR)' ';
    maxLength = UNDEF_LEN;
    realLength = 0;
Edebug(());

}


// ********************************


DcmCharString::DcmCharString( const DcmObject &oldObj, EVR oldIdent )
    : DcmElement( InternalUseTag )
{
Bdebug((6, "dcchrstr:DcmCharString::DcmCharString(DcmObject&,oldIdent=%d)",
	   oldIdent ));
debug(( 8, "Object pointer this=0x%p", this ));

    paddingChar = (ALT_CHAR)' ';
    maxLength = UNDEF_LEN;
    if ( oldObj.ident() == oldIdent )
    {
	DcmCharString const *old = (DcmCharString const *)&oldObj;
	*Tag = *old->Tag;
	iDS = old->iDS;
	offsetInFile  = old->offsetInFile;
	valueInMemory = old->valueInMemory;
	valueModified = old->valueModified;
	Length = old->Length;
	realLength = old->realLength;
	Xfer = old->Xfer;
	if ( old->CharStringValue == (ALT_CHAR*)NULL )
	{
	    if ( valueInMemory )
		Length = 0;
	    realLength = 0;
	    CharStringValue = (ALT_CHAR*)NULL;
	}
	else if ( Length == 0 )
	{
	    realLength = 0;
	    CharStringValue = new ALT_CHAR[ 2 ];
	    CharStringValue[ 0 ] = (ALT_CHAR)'\0';
	}
	else
	{
	    CharStringValue = new ALT_CHAR[ Length / sizeof(ALT_CHAR) + 2 ];
	    memcpy( CharStringValue,
		    old->CharStringValue,
		    (int)Length + 2 );
	}
    }
    else
    {
	CharStringValue = (ALT_CHAR*)NULL;
	errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmCharString: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmCharString::~DcmCharString()
{
Bdebug((6, "dcchrstr:DcmCharString::~DcmCharString()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( CharStringValue != (ALT_CHAR*)NULL )
	delete CharStringValue;
    CharStringValue = (char*)NULL;
Edebug(());

}


// ********************************


void DcmCharString::print( int level )
{
    if ( CharStringValue != (ALT_CHAR*)NULL )
    {
	ALT_CHAR *tmp = new ALT_CHAR[realLength + 3 ];
	tmp[0] = (ALT_CHAR)'[';
				// naechste Zeile ist fuer Typ 8-Bit-char
	strncpy( tmp+1, CharStringValue, (int)realLength );
	tmp[ realLength + 1 ] = (ALT_CHAR)']';
	tmp[ realLength + 2 ] = (ALT_CHAR)'\0';
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


T_VR_UL DcmCharString::getVM()
{
    ALT_CHAR *s = CharStringValue;
    T_VR_UL vm;
    if ( CharStringValue == (ALT_CHAR*)NULL || Length == 0 )
	vm = 0;
    else
    {
	vm = 1;
	ALT_CHAR c;
	while ((c = *s++))
	    if ( c == '\\' )
		vm++;
    }
    return vm;
}


// ********************************


E_Condition DcmCharString::alignValue()
{
    // Vorbedingungen:	Length ist eine gerade Zahl
    //			realLength ist hoechstens um 1 kleiner

Bdebug((3, "dcchrstr:DcmCharString::AlignValue() with '0x%2.2x'",
	   paddingChar	));

    E_Condition l_error = EC_Normal;
    if ( realLength != 0 && CharStringValue != (ALT_CHAR*)NULL )
    {
	if ( realLength < Length )
	{
	    CharStringValue[ realLength ] = paddingChar;
	    CharStringValue[ realLength + 1 ] = (ALT_CHAR)'\0';
	}
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmCharString::deAlignValue()
{
Bdebug((3, "dcchrstr:DcmCharString::deAlignValue()" ));

    E_Condition l_error = EC_Normal;
    if ( Length != 0L && CharStringValue != (ALT_CHAR*)NULL )
    {
	T_VR_UL i = strlen( CharStringValue );
	while ( i > 0
		&& (	CharStringValue[ i - 1 ] == paddingChar
		     || CharStringValue[ i - 1 ] <= (ALT_CHAR)' '
		   )
	      )
	{
	    CharStringValue[ i - 1 ] = (ALT_CHAR)'\0';
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


E_Condition DcmCharString::readValueField( E_TransferSyntax /*xfer*/ )
{
Bdebug((4, "dcchrstr:DcmCharString::readValueField(xfer)" ));

    E_Condition l_error = EC_Normal;
    streampos actpos;
    iDS->clear();
    if ( iDS->good() && iDS->fromFile() )
    {
	if ( CharStringValue != (ALT_CHAR*)NULL )
	    delete CharStringValue;
        CharStringValue = new ALT_CHAR[   Length / sizeof(ALT_CHAR)
                                        + sizeof(ALT_CHAR) ];
	actpos = iDS->tellg();
	iDS->seekg( offsetInFile );
        iDS->read( CharStringValue, (int)Length / sizeof(ALT_CHAR) );
					    // Warnung: evtl. int=16bit
Vdebug((1, !iDS->good(), "Warning: iDS->rdstate()=(0x%x)", iDS->rdstate() ));

        if ( Length != iDS->gcount() )
        {
            Length = iDS->gcount();                  // falls Datenblock zuende
            cerr << "Error: DcmCharString::readValueField() "
                    "unexpected EndOfStream, state=("
                 << iDS->rdstate() << ")" << endl;
            iDS->clear();
            l_error = EC_CorruptedData;
        }
	CharStringValue[ Length / sizeof(ALT_CHAR) ] = (ALT_CHAR)'\0';
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


E_Condition DcmCharString::read( E_TransferSyntax xfer,
                                 E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcchrstr:DcmCharString::read()" ));

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
	    if ( CharStringValue != (ALT_CHAR*)NULL )
		delete CharStringValue;
            CharStringValue = new ALT_CHAR[   Length / sizeof(ALT_CHAR)
                                            + sizeof(ALT_CHAR) ];
	    iDS->read( CharStringValue, (int)Length / sizeof(ALT_CHAR) );
						// Warnung: evtl. int=16bit
	    CharStringValue[ Length / sizeof(ALT_CHAR) ] = '\0';
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


E_Condition DcmCharString::readBlock( E_TransferSyntax xfer,
                                      E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcchrstr:DcmCharString::readBlock(xfer)" ));

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
	    if ( CharStringValue != (ALT_CHAR*)NULL )
		delete CharStringValue;
            CharStringValue = new ALT_CHAR[   Length / sizeof(ALT_CHAR)
                                            + sizeof(ALT_CHAR) ];
	    rdStat = ERW_inWork;
	}
	T_VR_UL len = (Length - bytesRead) <= iDS->buffered()
		      ? (Length - bytesRead)
		      : iDS->buffered();
        len -= len % sizeof(ALT_CHAR);       // lese nur ganze Werte
	iDS->read( &CharStringValue[ bytesRead / sizeof(ALT_CHAR) ],
		   (int)len / sizeof(ALT_CHAR) );
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
	CharStringValue[ Length / sizeof(ALT_CHAR) ] = '\0';
	valueInMemory = TRUE;
	valueModified = FALSE;
    }
debug(( 3, "errorFlag=(%d) bytesRead=%d Length=%d",
	   errorFlag, bytesRead, Length ));
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmCharString::write( oDicomStream &oDS,
                                  E_TransferSyntax oxfer,
                                  E_EncodingType enctype,
                                  E_GrpLenEncoding gltype )
{
Bdebug((3, "dcchrstr:DcmCharString::write(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
	if ( CharStringValue == (ALT_CHAR*)NULL )
	    Length = 0; 		  // Daten konnten nicht gelesen werden

	T_VR_UL written_bytes;
        errorFlag = DcmObject::writeTagAndLength( oDS, oxfer, &written_bytes );
        if ( errorFlag == EC_Normal )     // bisher noch kein Fehler
            errorFlag = l_error;
debug(( 3, "Header_Bytes_written              =[0x%8.8x]", written_bytes ));

	if ( CharStringValue != (ALT_CHAR*)NULL && errorFlag == EC_Normal )
	{
	    alignValue();
            oDS.write( CharStringValue, (int)Length ); // Warnung: evtl. int=16bit
            CharStringValue[ realLength ] = '\0'; // DeAligning um genau 1 Byte
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


E_Condition DcmCharString::writeBlock( oDicomStream &oDS,
				       E_TransferSyntax oxfer,
                                       E_EncodingType enctype,
                                       E_GrpLenEncoding gltype )
{
Bdebug((3, "dcchrstr:DcmCharString::writeBlock(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
		    if ( CharStringValue == (ALT_CHAR*)NULL )
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
	    if (    CharStringValue != (ALT_CHAR*)NULL
		 && errorFlag == EC_Normal
		 && writeState() != ERW_init )
	    {
		if ( bytesWritten == 0 )
		    alignValue();
                T_VR_UL len = (Length - bytesWritten) <= oDS.avail()
			      ? (Length - bytesWritten)
                              : oDS.avail();
		len -= len % sizeof(ALT_CHAR);	     // lese nur ganze Werte
                oDS.write( &CharStringValue[ bytesWritten / sizeof(ALT_CHAR) ],
			    (int)len / sizeof(ALT_CHAR) );
						     // Warnung: evtl. int=16bit
		bytesWritten += len;

		if ( bytesWritten == Length )
		{
		    wrStat = ERW_ready;
                    CharStringValue[ realLength ] = '\0';
                                                   // DeAligning um genau 1 Byte
		}
		else
		    errorFlag = EC_InvalidStream;
	    } // CharStringValue != NULL

	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmCharString::put( ALT_CHAR *charstringvalue )
{
    errorFlag = EC_Normal;
    if ( CharStringValue != (ALT_CHAR*)NULL )
	delete CharStringValue;
    if ( charstringvalue != (ALT_CHAR*)NULL )
    {
        realLength = strlen( charstringvalue );
        Length = ((realLength+1) & 0xfffffffe);
        CharStringValue = new ALT_CHAR[   Length / sizeof(ALT_CHAR)
                                        + sizeof(ALT_CHAR) ];
        memcpy( CharStringValue, charstringvalue, (int)realLength + 1 );
    }
    else
    {
	CharStringValue = (ALT_CHAR*)NULL;
	Length = 0;
	realLength = 0;
	errorFlag = EC_CorruptedData;
    }
    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


ALT_CHAR* DcmCharString::get()
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( CharStringValue == (ALT_CHAR*)NULL )
    {
	Length = 0;		      // Daten konnten nicht gelesen werden
        realLength = 0;
    }
    return CharStringValue;
}


// ********************************


E_Condition DcmCharString::clear()
{
Bdebug((2, "dcchrstr:DcmCharString::clear()"));

    errorFlag = EC_Normal;
    if ( CharStringValue != (ALT_CHAR*)NULL )
	delete CharStringValue;
    CharStringValue = (ALT_CHAR*)NULL;
    Length = 0;
    realLength = 0;
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmCharString::verify( BOOL autocorrect )
{
Bdebug((3, "dcchrstr:DcmCharString::verify(autocorrect=%d)", autocorrect ));
debug(( 3, "Tag=(0x%4.4x,0x%4.4x) \"%s\" \"%s\"",
	   getGTag(), getETag(),
           DcmVR(getVR()).getVRName(), getTag().getTagName() ));

    errorFlag = EC_Normal;
    if ( CharStringValue != (ALT_CHAR*)NULL && Length != 0 )
    {
	if ( autocorrect == TRUE )
	{
            ALT_CHAR *tempstr = new ALT_CHAR[ Length + sizeof(ALT_CHAR) ];
	    T_VR_UL field = 0;
	    T_VR_UL num = getVM();
	    T_VR_UL pos = 0;
	    T_VR_UL temppos = 0;
	    ALT_CHAR c;
	    while ( field < num )
	    {
		T_VR_UL fieldlen = 0;
		while ( (c = CharStringValue[pos++]) && c != '\\' )
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
	    delete CharStringValue;
	    CharStringValue = tempstr;
            if ( errorFlag == EC_CorruptedData )
                valueModified = TRUE;
            deAlignValue();                  // einziger Aufruf von deAlignValue
	}
	else
	{
	    T_VR_UL field = 0;
	    T_VR_UL num = getVM();
	    T_VR_UL pos = 0;
	    ALT_CHAR c;
	    while ( field < num )
	    {
		T_VR_UL fieldlen = 0;
		while ( (c = CharStringValue[pos++]) && c != '\\' )
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


/*
// Version mit String():

E_Condition DcmCharString::verify( BOOL autocorrect )
{
Bdebug((3, "dcchrstr:DcmCharString::verify(autocorrect=%d)", autocorrect ));
debug(( 3, "Tag=(0x%4.4x,0x%4.4x) \"%s\" \"%s\"",
	   getGTag(), getETag(),
           DcmVR(getVR()).getVRName(), getTag().getTagName() ));

    errorFlag = EC_Normal;
    int i;
    String SV( (ALT_CHAR)CharStringValue );
    T_VR_UL vm = SV.index( '\\' );
    vm = ( vm == -1 && !SV.empty()
	   ? 1
	   : vm + 1
	 );
    String arraySV[vm];
    T_VR_UL newvm = split( SV, arraySV, vm, String('\\') );
    for ( i=0; i<newvm; i++ )
	if ( arraySV[i].length() > maxLength )
	    errorFlag = EC_CorruptedData;

    if ( autocorrect == TRUE && errorFlag == EC_CorruptedData )
    {
	for ( i=0; i<newvm; i++ )
	{
	    if ( arraySV[i].length() > maxLength )
	    {
		arraySV[i].del( maxLength, arraySV[i].length() - maxLength );
	    }
	}
	ALT_CHAR *newValue = (ALT_CHAR*)join( arraySV, newvm, String('\\') );
	delete CharStringValue;
	CharStringValue = newValue;
        valueModified = TRUE;
        deAlignValue();                  // einziger Aufruf von deAlignValue
    }

Vdebug((3, TRUE, "Warning: vm=%d != newvm=%d, VM=%d", vm, newvm, getVM() ));
Vdebug((3, errorFlag!=EC_Normal,
	   "Illegal values in Tag=(0x%4.4x,0x%4.4x) VM=%d",
	   getGTag(), getETag(), getVM() ));
Edebug(());

    return errorFlag;
}
*/


// ********************************


E_Condition DcmCharString::loadAllDataIntoMemory()
{
    E_Condition l_error = EC_Normal;
    if ( valueInMemory == FALSE )
    {                                       // Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
        if ( CharStringValue == NULL )
            Length = 0;                     // Daten konnten nicht gelesen
                                            // werden, Laenge korrigieren,
                                            // errorFlag enthaelt bereits
                                            // Fehlercode
        l_error = errorFlag;                // errorFlag nur bei Fehler aendern
    }
    return l_error;
}


// ********************************


