/*
 *
 * Author: Gerd Ehlers      Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrul.cc
 *
 * Purpose:
 * Implementation of class DcmUnsignedLong
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

#include "dcvrul.h"
#include "dcdebug.h"


// ********************************


DcmUnsignedLong::DcmUnsignedLong( const DcmTag &tag,
				  T_VR_UL len,
				  iDicomStream *iDStream )
    : DcmElement( tag, len, iDStream )
{
Bdebug((5, "dcvrul:DcmUnsignedLong::DcmUnsignedLong(DcmTag&,len=%ld,*iDS)",
	   len ));
debug(( 8, "Object pointer this=0x%p", this ));

    UnsigLongValue = (T_VR_UL*)NULL;
Edebug(());

}


// ********************************


DcmUnsignedLong::DcmUnsignedLong( const DcmUnsignedLong& old )
    : DcmElement( old )
{
Bdebug((5, "dcvrul:DcmUnsignedLong::DcmUnsignedLong(DcmUnsignedLong&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( old.ident() == EVR_UL || old.ident() == EVR_up ) {
	if (Length == 0 || old.UnsigLongValue == (T_VR_UL*)NULL ) {
	    if ( valueInMemory )
		Length = 0;
	    UnsigLongValue = (T_VR_UL*)NULL;
	} else {
	    UnsigLongValue = new T_VR_UL[ Length / sizeof(T_VR_UL) ];
	    memcpy( UnsigLongValue, old.UnsigLongValue, (int)Length );
	}
    } else {
	UnsigLongValue = (T_VR_UL*)NULL;
	errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmUnsignedLong: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmUnsignedLong::~DcmUnsignedLong()
{
Bdebug((5, "dcvrul:DcmUnsignedLong::~DcmUnsignedLong()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( UnsigLongValue != (T_VR_UL*)NULL )
	delete UnsigLongValue;
Edebug(());

}


// ********************************


DcmEVR DcmUnsignedLong::ident() const
{
    return EVR_UL;
}


// ********************************


void DcmUnsignedLong::print( int level )
{
    if ( UnsigLongValue != (T_VR_UL*)NULL )
    {
	char *ch_words = new char[ Length*12/sizeof(T_VR_UL) + 4 ];
	char *tmp = ch_words;
	T_VR_UL *tattr = UnsigLongValue;
	unsigned int i;

	for ( i=0; i<( Length/sizeof(T_VR_UL) ); i++ )
	{
	    sprintf( tmp, "%lu\\", (unsigned long)*tattr );
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


T_VR_UL DcmUnsignedLong::getVM()
{
    return Length / sizeof(T_VR_UL);
}


// ********************************


E_Condition DcmUnsignedLong::readValueField( E_TransferSyntax xfer )
{
Bdebug((4, "dcvrul:DcmUnsignedLong::readValueField(xfer)" ));

    E_Condition l_error = EC_Normal;
    long actpos;
    iDS->clear();
    if ( iDS->good() && iDS->fromFile() )
    {
	iDS->setDataByteOrder( xfer );
	if ( UnsigLongValue != (T_VR_UL*)NULL )
	    delete UnsigLongValue;
	UnsigLongValue = new T_VR_UL[ Length / sizeof(T_VR_UL) ];
	actpos = iDS->tellg();
	iDS->seekg( offsetInFile );
	iDS->read( UnsigLongValue, (int)Length / sizeof(T_VR_UL) );
						 // Warnung: evtl. int=16bit
Vdebug((1, !iDS->good(), "Warning: iDS->rdstate()=(0x%x)", iDS->rdstate() ));

        if ( Length != iDS->gcount() )
        {
            Length = iDS->gcount();                  // falls Datenblock zuende
            cerr << "Error: DcmUnsignedLong::readValueField() "
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


E_Condition DcmUnsignedLong::read( E_TransferSyntax xfer,
                                   E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrul:DcmUnsignedLong::read()" ));

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
	    if ( UnsigLongValue != (T_VR_UL*)NULL )
		delete UnsigLongValue;
	    UnsigLongValue = new T_VR_UL[ Length / sizeof(T_VR_UL) ];
	    iDS->read( UnsigLongValue, (int)Length / sizeof(T_VR_UL) );
						 // Warnung: evtl. int=16bit
	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return EC_Normal;
}


// ********************************


E_Condition DcmUnsignedLong::readBlock( E_TransferSyntax xfer,
                                        E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrul:DcmUnsignedLong::readBlock()" ));

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
	    if ( UnsigLongValue != (T_VR_UL*)NULL )
		delete UnsigLongValue;
	    UnsigLongValue = new T_VR_UL[ Length / sizeof(T_VR_UL) ];
	    rdStat = ERW_inWork;
	}
	T_VR_UL len = (Length - bytesRead) <= iDS->buffered()
		      ? (Length - bytesRead)
		      : iDS->buffered();
	len -= len % sizeof(T_VR_UL);	     // lese nur ganze Werte
	iDS->read( &UnsigLongValue[ bytesRead / sizeof(T_VR_UL) ],
		   (int)len / sizeof(T_VR_UL) );
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


E_Condition DcmUnsignedLong::write( oDicomStream &oDS,
                                    E_TransferSyntax oxfer,
                                    E_EncodingType enctype,
                                    E_GrpLenEncoding gltype )
{
Bdebug((3, "dcvrul:DcmUnsignedLong::write(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
           oxfer, enctype, gltype ));

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
	if ( UnsigLongValue == (T_VR_UL*)NULL )
	    Length = 0; 		  // Daten konnten nicht gelesen werden

	T_VR_UL written_bytes;
	errorFlag = DcmObject::writeTagAndLength( oDS, oxfer, &written_bytes );
debug(( 3, "Header_Bytes_written              =[0x%8.8x]", written_bytes ));

	if ( UnsigLongValue != (T_VR_UL*)NULL && errorFlag == EC_Normal )
            oDS.write( UnsigLongValue, (int)Length / sizeof(T_VR_UL) );
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


E_Condition DcmUnsignedLong::writeBlock( oDicomStream &oDS,
                                         E_TransferSyntax oxfer,
                                         E_EncodingType enctype,
                                         E_GrpLenEncoding gltype )
{
Bdebug((3, "dcvrul:DcmUnsignedLong::writeBlock(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
		    if ( UnsigLongValue == (T_VR_UL*)NULL )
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
	    if (    UnsigLongValue != (T_VR_UL*)NULL
		 && errorFlag == EC_Normal
		 && writeState() != ERW_init )
	    {
                T_VR_UL len = (Length - bytesWritten) <= oDS.avail()
			      ? (Length - bytesWritten)
                              : oDS.avail();
		len -= len % sizeof(T_VR_UL);	    // lese nur ganze Werte
                oDS.write( &UnsigLongValue[ bytesWritten / sizeof(T_VR_UL) ],
			    (int)len / sizeof(T_VR_UL) );
						     // Warnung: evtl. int=16bit
		bytesWritten += len;

		if ( bytesWritten == Length )
		    wrStat = ERW_ready;
		else
		    errorFlag = EC_InvalidStream;
	    } // UnsigLongValue != NULL

	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedLong::put( T_VR_UL *ulong,
				  T_VR_UL length )
{
    errorFlag = EC_Normal;
    if ( UnsigLongValue != (T_VR_UL*)NULL )
	delete UnsigLongValue;
    if ( ulong != (T_VR_UL*)NULL )
    {
	Length = length * sizeof(T_VR_UL);
	UnsigLongValue = new T_VR_UL[ length ];
        memcpy( UnsigLongValue, ulong, (int)Length );
    }
    else
    {
	UnsigLongValue = (T_VR_UL*)NULL;
	Length = 0;
	errorFlag = EC_CorruptedData;
    }
    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedLong::put( T_VR_UL ulong )
{
    errorFlag = EC_Normal;

    if ( UnsigLongValue != (T_VR_UL*)NULL )
        delete UnsigLongValue;
    UnsigLongValue = new T_VR_UL[1];
    Length = 1 * sizeof(T_VR_UL);
    UnsigLongValue[0] = ulong;

    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedLong::put( T_VR_UL ulong,
                                  T_VR_UL num )
{
Bdebug((2, "dcvrul:DcmUnsignedLong::put(ulong=%ld,num=%ld)", ulong, num ));

    errorFlag = EC_Normal;
    T_VR_UL oldMaxNum = Length / sizeof(T_VR_UL);
    if ( num >= oldMaxNum )
    {
        // neuen Speicher alloziieren und alte Values kopieren
        T_VR_UL *newUnsigLongValue = new T_VR_UL[ num+1 ];
        if ( newUnsigLongValue == (T_VR_UL*)NULL )
        {
            cerr << "Error: DcmUnsignedLong::put() no more memory available"
                 << endl;
            errorFlag = EC_IllegalCall;
        }
        else
        {
           T_VR_UL newLength = (num+1) * sizeof(T_VR_UL);

           if ( UnsigLongValue != (T_VR_UL*)NULL )
               memcpy( newUnsigLongValue, UnsigLongValue, (int)Length );
           memset( newUnsigLongValue+Length, 0, (size_t)(newLength - Length) );

           if ( UnsigLongValue != (T_VR_UL*)NULL )
               delete UnsigLongValue;
           UnsigLongValue = newUnsigLongValue;
           Length = newLength;
debug(( 2, "DcmUnsignedLong:: Internal ValueField enlarged. Actual Length = %ld",
           Length ));

        }
    }

    if ( errorFlag == EC_Normal )
    {
        UnsigLongValue[num] = ulong;

        valueInMemory = TRUE;
        valueModified = TRUE;
    }
Edebug(());

    return errorFlag;
}


// ********************************


T_VR_UL* DcmUnsignedLong::get()
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( UnsigLongValue == (T_VR_UL*)NULL )
	Length = 0;		      // Daten konnten nicht gelesen werden
    return UnsigLongValue;
}


// ********************************


T_VR_UL DcmUnsignedLong::get( T_VR_UL num )
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( UnsigLongValue == (T_VR_UL*)NULL )
	Length = 0;		      // Daten konnten nicht gelesen werden

    if ( num < Length / sizeof(T_VR_UL) )
    {
        return UnsigLongValue[num];
    }
    else
    {
        if ( errorFlag == EC_Normal )
            errorFlag = EC_IllegalCall;
        return 0;
    }
}


// ********************************


E_Condition DcmUnsignedLong::clear()
{
    errorFlag = EC_Normal;
    if ( UnsigLongValue != (T_VR_UL*)NULL )
	delete UnsigLongValue;
    UnsigLongValue = (T_VR_UL*)NULL;
    Length = 0;
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedLong::verify( BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( Length % (sizeof(T_VR_UL)) != 0 )
    {
	errorFlag = EC_CorruptedData;
	if ( autocorrect == TRUE )
	{

	    Length = Length - ( Length % (sizeof(T_VR_UL)) );
					    // auf gueltige Laenge kuerzen
            valueModified = TRUE;
	}
    }
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedLong::loadAllDataIntoMemory()
{
    E_Condition l_error = EC_Normal;
    if ( valueInMemory == FALSE )
    {                                       // Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
        if ( UnsigLongValue == NULL )
            Length = 0;                     // Daten konnten nicht gelesen
                                            // werden, Laenge korrigieren,
                                            // errorFlag enthaelt bereits
                                            // Fehlercode
        l_error = errorFlag;                // errorFlag nur bei Fehler aendern
    }
    return l_error;
}


// ********************************


