/*
 *
 * Author: Gerd Ehlers      Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrufd.cc
 *
 * Purpose:
 * Implementation of class DcmFloatingPointDouble
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */



#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrfd.h"
#include "dcdebug.h"


// ********************************


DcmFloatingPointDouble::DcmFloatingPointDouble( const DcmTag &tag,
                                                T_VR_UL len,
                                                iDicomStream *iDStream )
    : DcmElement( tag, len, iDStream )
{
Bdebug((5, "dcvrfd:DcmFloatingPointDouble::DcmFloatingPointDouble"
           "(DcmTag&,len=%ld,*iDS)", len ));
debug(( 8, "Object pointer this=0x%p", this ));

    DoubleValue = (T_VR_FD*)NULL;
Edebug(());

}


// ********************************


DcmFloatingPointDouble::DcmFloatingPointDouble( const DcmFloatingPointDouble& old )
    : DcmElement( old )
{
Bdebug((5, "dcvrfd:DcmFloatingPointDouble::DcmFloatingPointDouble(DcmFloatingPointDouble&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( old.ident() == EVR_FD ) {
        if (    Length == 0 || old.DoubleValue == (T_VR_FD*)NULL ) {
            if ( valueInMemory ) Length = 0;
            DoubleValue = (T_VR_FD*)NULL;
        } else  {
            DoubleValue = new T_VR_FD[ Length / sizeof(T_VR_FD) ];
            memcpy( DoubleValue, old.DoubleValue, (int)Length );
        }
    } else {
        DoubleValue = (T_VR_FD*)NULL;
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmFloatingPointDouble: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());
}


// ********************************


DcmFloatingPointDouble::~DcmFloatingPointDouble()
{
Bdebug((5, "dcvrfd:DcmFloatingPointDouble::~DcmFloatingPointDouble()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( DoubleValue != (T_VR_FD*)NULL )
	delete DoubleValue;
Edebug(());

}


// ********************************


DcmEVR DcmFloatingPointDouble::ident() const
{
    return EVR_FD;
}


// ********************************


void DcmFloatingPointDouble::print( int level )
{
    if ( DoubleValue != (T_VR_FD*)NULL )
    {
	char *ch_words;
	char *tmp = ch_words = new char[ Length*26/sizeof(T_VR_FD) + 8 ];
	T_VR_FD *tattr = DoubleValue;
	unsigned int i;

	for ( i=0; i<( Length/sizeof(T_VR_FD) ); i++ )
	{
	    sprintf( tmp, "%g\\", *tattr );
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


T_VR_UL DcmFloatingPointDouble::getVM()
{
    return Length / sizeof(T_VR_FD);
}


// ********************************


E_Condition DcmFloatingPointDouble::readValueField( E_TransferSyntax xfer )
{
Bdebug((4, "dcvrfd:DcmFloatingPointDouble::readValueField(xfer)" ));

    E_Condition l_error = EC_Normal;
    long actpos;
    iDS->clear();
    if ( iDS->good() && iDS->fromFile() )
    {
	iDS->setDataByteOrder( xfer );
	if ( DoubleValue != (T_VR_FD*)NULL )
	    delete DoubleValue;
	DoubleValue = new T_VR_FD[ Length / sizeof(T_VR_FD) ];
	actpos = iDS->tellg();
	iDS->seekg( offsetInFile );
	iDS->read( DoubleValue, (int)Length / sizeof(T_VR_FD) );
						 // Warnung: evtl. int=16bit
Vdebug((1, !iDS->good(), "Warning: iDS->rdstate()=(0x%x)", iDS->rdstate() ));

        if ( Length != iDS->gcount() )
        {
            Length = iDS->gcount();                  // falls Datenblock zuende
            cerr << "Error: DcmFloatingPointDouble::readValueField() "
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


E_Condition DcmFloatingPointDouble::read( E_TransferSyntax xfer,
                                          E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrfd:DcmFloatingPointDouble::read()" ));

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
	    if ( DoubleValue != (T_VR_FD*)NULL )
		delete DoubleValue;
	    DoubleValue = new T_VR_FD[ Length / sizeof(T_VR_FD) ];
	    iDS->read( DoubleValue, (int)Length / sizeof(T_VR_FD) );
						 // Warnung: evtl. int=16bit
	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return EC_Normal;
}


// ********************************


E_Condition DcmFloatingPointDouble::readBlock( E_TransferSyntax xfer,
                                               E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrfd:DcmFloatingPointDouble::readBlock()" ));

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
	    if ( DoubleValue != (T_VR_FD*)NULL )
		delete DoubleValue;
	    DoubleValue = new T_VR_FD[ Length / sizeof(T_VR_FD) ];
	    rdStat = ERW_inWork;
	}
	T_VR_UL len = (Length - bytesRead) <= iDS->buffered()
		      ? (Length - bytesRead)
		      : iDS->buffered();
	len -= len % sizeof(T_VR_FD);	     // lese nur ganze Werte
	iDS->read( &DoubleValue[ bytesRead / sizeof(T_VR_FD) ],
		   (int)len / sizeof(T_VR_FD) );
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


E_Condition DcmFloatingPointDouble::write( oDicomStream &oDS,
                                           E_TransferSyntax oxfer,
                                           E_EncodingType enctype,
                                           E_GrpLenEncoding gltype )
{
Bdebug((3, "dcvrfd:DcmFloatingPointDouble::write(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
	if ( DoubleValue == (T_VR_FD*)NULL )
	    Length = 0; 		  // Daten konnten nicht gelesen werden

	T_VR_UL written_bytes;
	errorFlag = DcmObject::writeTagAndLength( oDS, oxfer, &written_bytes );
debug(( 3, "Header_Bytes_written              =[0x%8.8x]", written_bytes ));

	if ( DoubleValue != (T_VR_FD*)NULL && errorFlag == EC_Normal )
            oDS.write( DoubleValue, (int)Length / sizeof(T_VR_FD) );
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


E_Condition DcmFloatingPointDouble::writeBlock( oDicomStream &oDS,
                                                E_TransferSyntax oxfer,
                                                E_EncodingType enctype,
                                                E_GrpLenEncoding gltype )
{
Bdebug((3, "dcvrfd:DcmFloatingPointDouble::writeBlock(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
		    if ( DoubleValue == (T_VR_FD*)NULL )
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
	    if (    DoubleValue != (T_VR_FD*)NULL
		 && errorFlag == EC_Normal
		 && writeState() != ERW_init )
	    {
                T_VR_UL len = (Length - bytesWritten) <= oDS.avail()
			      ? (Length - bytesWritten)
                              : oDS.avail();
		len -= len % sizeof(T_VR_FD);	    // lese nur ganze Werte
                oDS.write( &DoubleValue[ bytesWritten / sizeof(T_VR_FD) ],
			    (int)len / sizeof(T_VR_FD) );
						     // Warnung: evtl. int=16bit
		bytesWritten += len;

		if ( bytesWritten == Length )
		    wrStat = ERW_ready;
		else
		    errorFlag = EC_InvalidStream;
	    } // DoubleValue != NULL

	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointDouble::put( T_VR_FD *doubleval,
					 T_VR_UL length )
{
    errorFlag = EC_Normal;
    if ( DoubleValue != (T_VR_FD*)NULL )
	delete DoubleValue;
    if ( doubleval != (T_VR_FD*)NULL )
    {
	Length = length * sizeof(T_VR_FD);
	DoubleValue = new T_VR_FD[ length ];
        memcpy( DoubleValue, doubleval, (int)Length );
    }
    else
    {
	DoubleValue = (T_VR_FD*)NULL;
	Length = 0;
	errorFlag = EC_CorruptedData;
    }
    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointDouble::put( T_VR_FD doubleval )
{
    errorFlag = EC_Normal;

    if ( DoubleValue != (T_VR_FD*)NULL )
        delete DoubleValue;
    DoubleValue = new T_VR_FD[1];
    Length = 1 * sizeof(T_VR_FD);
    DoubleValue[0] = doubleval;

    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointDouble::put( T_VR_FD doubleval,
                                         T_VR_UL num )
{
Bdebug((2, "dcvrul:DcmFloatingPointDouble::put(doubleval=%ld,num=%ld)", doubleval, num ));

    errorFlag = EC_Normal;
    T_VR_UL oldMaxNum = Length / sizeof(T_VR_FD);
    if ( num >= oldMaxNum )
    {
        // neuen Speicher alloziieren und alte Values kopieren
        T_VR_FD *newDoubleValue = new T_VR_FD[ num+1 ];
        if ( newDoubleValue == (T_VR_FD*)NULL )
        {
            cerr << "Error: DcmFloatingPointDouble::put() no more memory available"
                 << endl;
            errorFlag = EC_IllegalCall;
        }
        else
        {
           T_VR_UL newLength = (num+1) * sizeof(T_VR_FD);

           if ( DoubleValue != (T_VR_FD*)NULL )
               memcpy( newDoubleValue, DoubleValue, (int)Length );
           memset( newDoubleValue+Length, 0, (size_t)(newLength - Length) );

           if ( DoubleValue != (T_VR_FD*)NULL )
               delete DoubleValue;
           DoubleValue = newDoubleValue;
           Length = newLength;
debug(( 2, "DcmFloatingPointDouble:: Internal ValueField enlarged. Actual Length = %ld",
           Length ));

        }
    }

    if ( errorFlag == EC_Normal )
    {
        DoubleValue[num] = doubleval;

        valueInMemory = TRUE;
        valueModified = TRUE;
    }
Edebug(());

    return errorFlag;
}


// ********************************


T_VR_FD* DcmFloatingPointDouble::get()
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( DoubleValue == (T_VR_FD*)NULL )
	Length = 0;		      // Daten konnten nicht gelesen werden
    return DoubleValue;
}


// ********************************


T_VR_FD DcmFloatingPointDouble::get( T_VR_UL num )
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( DoubleValue == (T_VR_FD*)NULL )
	Length = 0;		      // Daten konnten nicht gelesen werden

    if ( num < Length / sizeof(T_VR_FD) )
    {
        return DoubleValue[num];
    }
    else
    {
        if ( errorFlag == EC_Normal )
            errorFlag = EC_IllegalCall;
        return 0;
    }
}


// ********************************


E_Condition DcmFloatingPointDouble::clear()
{
    errorFlag = EC_Normal;
    if ( DoubleValue != (T_VR_FD*)NULL )
	delete DoubleValue;
    DoubleValue = (T_VR_FD*)NULL;
    Length = 0;
    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointDouble::verify( BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( Length % (sizeof(T_VR_FD)) != 0 )
    {
	errorFlag = EC_CorruptedData;
	if ( autocorrect == TRUE )
	{

	    Length = Length - ( Length % (sizeof(T_VR_FD)) );
					    // auf gueltige Laenge kuerzen
            valueModified = TRUE;
	}
    }
    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointDouble::loadAllDataIntoMemory()
{
    E_Condition l_error = EC_Normal;
    if ( valueInMemory == FALSE )
    {                                       // Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
        if ( DoubleValue == NULL )
            Length = 0;                     // Daten konnten nicht gelesen
                                            // werden, Laenge korrigieren,
                                            // errorFlag enthaelt bereits
                                            // Fehlercode
        l_error = errorFlag;                // errorFlag nur bei Fehler aendern
    }
    return l_error;
}


// ********************************


