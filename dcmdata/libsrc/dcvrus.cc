/*
 *
 * Author: Gerd Ehlers      Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrus.cc
 *
 * Purpose:
 * Implementation of class DcmUnsignedShort
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */



#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrus.h"
#include "dcdebug.h"


// ********************************


DcmUnsignedShort::DcmUnsignedShort( const DcmTag &tag,
				    T_VR_UL len,
				    iDicomStream *iDStream )
	     : DcmElement( tag, len, iDStream )
{
Bdebug((5, "dcvrus:DcmUnsignedShort::DcmUnsignedShort(DcmTag&,len=%ld,*iDS)",
           len ));
debug(( 8, "Object pointer this=0x%p", this ));

    UnsigShortValue = (T_VR_US*)NULL;
Edebug(());

}


// ********************************


DcmUnsignedShort::DcmUnsignedShort( const DcmUnsignedShort& old )
    : DcmElement( old )
{
Bdebug((5, "dcvrus:DcmUnsignedShort::DcmUnsignedShort(DcmUnsignedShort&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( old.ident() == EVR_US )
    {
	if (Length == 0 || old.UnsigShortValue == (T_VR_US*)NULL ) {
            if ( valueInMemory )
                Length = 0;
	    UnsigShortValue = (T_VR_US*)NULL;
	} else {
	    UnsigShortValue = new T_VR_US[ Length / sizeof(T_VR_US) ];
	    memcpy( UnsigShortValue, old.UnsigShortValue, (int)Length );
	}
    } else {
	UnsigShortValue = (T_VR_US*)NULL;
	errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmUnsignedShort: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmUnsignedShort::~DcmUnsignedShort()
{
Bdebug((5, "dcvrus:DcmUnsignedShort::~DcmUnsignedShort()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( UnsigShortValue != (T_VR_US*)NULL )
	delete UnsigShortValue;
Edebug(());

}


// ********************************


DcmEVR DcmUnsignedShort::ident() const
{
    return EVR_US;
}


// ********************************


void DcmUnsignedShort::print( int level )
{
    if ( UnsigShortValue != (T_VR_US*)NULL )
    {
	char *ch_words;
	char *tmp = ch_words = new char[ Length*8/sizeof(T_VR_US) + 2 ];
	T_VR_US *tattr = UnsigShortValue;
	unsigned int i;

	for ( i=0; i<( Length/sizeof(T_VR_US) ); i++ )
	{
	    sprintf( tmp, "%hu\\", *tattr );
	    tmp += strlen(tmp);
	    tattr++;
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


T_VR_UL DcmUnsignedShort::getVM()
{
    return Length / sizeof(T_VR_US);
}


// ********************************


E_Condition DcmUnsignedShort::readValueField( E_TransferSyntax xfer )
{
Bdebug((4, "dcvrus:DcmUnsignedShort::readValueField(xfer)" ));

    E_Condition l_error = EC_Normal;
    long actpos;
    iDS->clear();
    if ( iDS->good() && iDS->fromFile() )
    {
	iDS->setDataByteOrder( xfer );
	if ( UnsigShortValue != (T_VR_US*)NULL )
	    delete UnsigShortValue;
	UnsigShortValue = new T_VR_US[ Length / sizeof(T_VR_US) ];
	actpos = iDS->tellg();
	iDS->seekg( offsetInFile );
	iDS->read( UnsigShortValue, (int)Length / sizeof(T_VR_US) );
						 // Warnung: evtl. int=16bit
Vdebug((1, !iDS->good(), "Warning: iDS->rdstate()=(0x%x)", iDS->rdstate() ));

        if ( Length != iDS->gcount() )
        {
            Length = iDS->gcount();                  // falls Datenblock zuende
            cerr << "Error: DcmUnsignedShort::readValueField() "
                    "unexpected EndOfStream, state=("
                 << iDS->rdstate() << ")" << endl;
            iDS->clear();
            l_error = EC_CorruptedData;
        }
	iDS->seekg( actpos );
	valueInMemory = TRUE;
	valueModified = FALSE;
    }
    else
        l_error = EC_InvalidStream;
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmUnsignedShort::read( E_TransferSyntax xfer,
                                    E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrus:DcmUnsignedShort::read()" ));

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
	    iDS->setDataByteOrder( xfer );
	    if ( UnsigShortValue != (T_VR_US*)NULL )
		delete UnsigShortValue;
	    UnsigShortValue = new T_VR_US[ Length / sizeof(T_VR_US) ];
	    iDS->read( UnsigShortValue, (int)Length / sizeof(T_VR_US) );
						 // Warnung: evtl. int=16bit
	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return EC_Normal;
}


// ********************************


E_Condition DcmUnsignedShort::readBlock( E_TransferSyntax xfer,
                                         E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrus:DcmUnsignedShort::readBlock()" ));

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
	iDS->setDataByteOrder( xfer );
	if ( readState() == ERW_init )
	{
	    if ( UnsigShortValue != (T_VR_US*)NULL )
		delete UnsigShortValue;
	    UnsigShortValue = new T_VR_US[ Length / sizeof(T_VR_US) ];
	    rdStat = ERW_inWork;
	}
	T_VR_UL len = (Length - bytesRead) <= iDS->buffered()
		      ? (Length - bytesRead)
		      : iDS->buffered();
	len -= len % sizeof(T_VR_US);	     // lese nur ganze Werte
	iDS->read( &UnsigShortValue[ bytesRead / sizeof(T_VR_US) ],
		   (int)len / sizeof(T_VR_US) );
					     // Warnung: evtl. int=16bit
	bytesRead += len;
	if ( bytesRead == Length )
	{
	    rdStat = ERW_ready;
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


E_Condition DcmUnsignedShort::write( oDicomStream &oDS,
                                     E_TransferSyntax oxfer,
                                     E_EncodingType enctype,
                                     E_GrpLenEncoding gltype )
{
Bdebug((3, "dcvrus:DcmUnsignedShort::write(&oDS,oxfer=%d,enctype=%d,"
           "gltype=%d)", oxfer, enctype, gltype ));

    errorFlag = EC_Normal;
    if ( !oDS.good() )
	errorFlag = EC_InvalidStream;
    else
    {
        oDS.setDataByteOrder( oxfer );
	if ( valueInMemory == FALSE )
	{					// Hier Daten aus iDS nachladen
	    errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
	}
	if ( UnsigShortValue == (T_VR_US*)NULL )
	    Length = 0; 		  // Daten konnten nicht gelesen werden

	T_VR_UL written_bytes;
	errorFlag = DcmObject::writeTagAndLength( oDS, oxfer, &written_bytes );
debug(( 3, "Header_Bytes_written              =[0x%8.8x]", written_bytes ));

	if ( UnsigShortValue != (T_VR_US*)NULL && errorFlag == EC_Normal )
            oDS.write( UnsigShortValue, (int)Length / sizeof(T_VR_US) );
						 // Warnung: evtl. int=16bit
	written_bytes += Length;
debug(( 3, "Bytes_written                     =[0x%8.8x]", written_bytes ));

	valueInMemory = TRUE;
	valueModified = FALSE;
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedShort::writeBlock( oDicomStream &oDS,
				       E_TransferSyntax oxfer,
                                       E_EncodingType enctype,
                                       E_GrpLenEncoding gltype )
{
Bdebug((3, "dcvrus:DcmUnsignedShort::writeBlock(&oDS,oxfer=%d,enctype=%d,"
           "gltype=%d)", oxfer, enctype, gltype ));

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
		    if ( UnsigShortValue == (T_VR_US*)NULL )
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
	    if (    UnsigShortValue != (T_VR_US*)NULL
		 && errorFlag == EC_Normal
		 && writeState() != ERW_init )
	    {
                T_VR_UL len = (Length - bytesWritten) <= oDS.avail()
			      ? (Length - bytesWritten)
                              : oDS.avail();
		len -= len % sizeof(T_VR_US);	    // lese nur ganze Werte
                oDS.write( &UnsigShortValue[ bytesWritten / sizeof(T_VR_US) ],
			    (int)len / sizeof(T_VR_US) );
						     // Warnung: evtl. int=16bit
		bytesWritten += len;

		if ( bytesWritten == Length )
		    wrStat = ERW_ready;
		else
		    errorFlag = EC_InvalidStream;
	    } // UnsigShortValue != NULL

	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedShort::put( T_VR_US *ushort,
				   T_VR_UL length )
{
    errorFlag = EC_Normal;
    if ( UnsigShortValue != (T_VR_US*)NULL )
	delete UnsigShortValue;
    if ( ushort != (T_VR_US*)NULL )
    {
	Length = length * sizeof(T_VR_US);
	UnsigShortValue = new T_VR_US[ length ];
        memcpy( UnsigShortValue, ushort, (int)Length );
    }
    else
    {
	UnsigShortValue = (T_VR_US*)NULL;
	Length = 0;
	errorFlag = EC_CorruptedData;
    }
    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedShort::put( T_VR_US ushort )
{
    errorFlag = EC_Normal;

    if ( UnsigShortValue != (T_VR_US*)NULL )
        delete UnsigShortValue;
    UnsigShortValue = new T_VR_US[1];
    Length = 1 * sizeof(T_VR_US);
    UnsigShortValue[0] = ushort;

    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedShort::put( T_VR_US ushort,
                                   T_VR_UL num )
{
Bdebug((2, "dcvrul:DcmUnsignedShort::put(ushort=%ld,num=%ld)", ushort, num ));

    errorFlag = EC_Normal;
    T_VR_UL oldMaxNum = Length / sizeof(T_VR_US);
    if ( num >= oldMaxNum )
    {
        // neuen Speicher alloziieren und alte Values kopieren
        T_VR_US *newUnsigShortValue = new T_VR_US[ num+1 ];
        if ( newUnsigShortValue == (T_VR_US*)NULL )
        {
            cerr << "Error: DcmUnsignedShort::put() no more memory available"
                 << endl;
            errorFlag = EC_IllegalCall;
        }
        else
        {
           T_VR_UL newLength = (num+1) * sizeof(T_VR_US);

           if ( UnsigShortValue != (T_VR_US*)NULL )
               memcpy( newUnsigShortValue, UnsigShortValue, (int)Length );
           memset( newUnsigShortValue+Length, 0, (size_t)(newLength - Length) );

           if ( UnsigShortValue != (T_VR_US*)NULL )
               delete UnsigShortValue;
           UnsigShortValue = newUnsigShortValue;
           Length = newLength;
debug(( 2, "DcmUnsignedShort:: Internal ValueField enlarged. Actual Length = %ld",
           Length ));

        }
    }

    if ( errorFlag == EC_Normal )
    {
        UnsigShortValue[num] = ushort;

        valueInMemory = TRUE;
        valueModified = TRUE;
    }
Edebug(());

    return errorFlag;
}


// ********************************


T_VR_US* DcmUnsignedShort::get()
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( UnsigShortValue == (T_VR_US*)NULL )
	Length = 0;		      // Daten konnten nicht gelesen werden
    return UnsigShortValue;
}


// ********************************


T_VR_US DcmUnsignedShort::get( T_VR_UL num )
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( UnsigShortValue == (T_VR_US*)NULL )
	Length = 0;		      // Daten konnten nicht gelesen werden

    if ( num < Length / sizeof(T_VR_US) )
    {
        return UnsigShortValue[num];
    }
    else
    {
        if ( errorFlag == EC_Normal )
            errorFlag = EC_IllegalCall;
        return 0;
    }
}


// ********************************


E_Condition DcmUnsignedShort::clear()
{
    errorFlag = EC_Normal;
    if ( UnsigShortValue != (T_VR_US*)NULL )
	delete UnsigShortValue;
    UnsigShortValue = (T_VR_US*)NULL;
    Length = 0;
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedShort::verify( BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( Length % (sizeof(T_VR_US)) != 0 )
    {
	errorFlag = EC_CorruptedData;
	if ( autocorrect == TRUE )
	{

	    Length = Length - ( Length % (sizeof(T_VR_US)) );
					    // auf gueltige Laenge kuerzen
            valueModified = TRUE;
	}
    }
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedShort::loadAllDataIntoMemory()
{
    E_Condition l_error = EC_Normal;
    if ( valueInMemory == FALSE )
    {                                       // Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
        if ( UnsigShortValue == NULL )
            Length = 0;                     // Daten konnten nicht gelesen
                                            // werden, Laenge korrigieren,
                                            // errorFlag enthaelt bereits
                                            // Fehlercode
        l_error = errorFlag;                // errorFlag nur bei Fehler aendern
    }
    return l_error;
}


// ********************************


