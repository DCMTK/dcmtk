/*
 *
 * Author: Gerd Ehlers      Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrfl.cc
 *
 * Purpose:
 * Implementation of class DcmFloatingPointSingle
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

#include "dcvrfl.h"
#include "dcdebug.h"


// ********************************


DcmFloatingPointSingle::DcmFloatingPointSingle( const DcmTag &tag,
                                                T_VR_UL len,
                                                iDicomStream *iDStream )
    : DcmElement( tag, len, iDStream )
{
Bdebug((5, "dcvrfl:DcmFloatingPointSingle::DcmFloatingPointSingle(DcmTag&,len=%ld,*iDS)",
           len ));
debug(( 8, "Object pointer this=0x%p", this ));

    FloatValue = (T_VR_FL*)NULL;
Edebug(());

}




// ********************************


DcmFloatingPointSingle::DcmFloatingPointSingle( const DcmFloatingPointSingle& old )
    : DcmElement( old )
{
Bdebug((5, "dcvrfl:DcmFloatingPointSingle::DcmFloatingPointSingle(DcmFloatingPointSingle&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( old.ident() == EVR_FL )
    {
        if ( Length == 0 || old.FloatValue == (T_VR_FL*)NULL ) {
            if ( valueInMemory )
                Length = 0;
            FloatValue = (T_VR_FL*)NULL;
        } else {
            FloatValue = new T_VR_FL[ Length / sizeof(T_VR_FL) ];
            memcpy( FloatValue, old.FloatValue, (int)Length );
        }
    } else {
        FloatValue = (T_VR_FL*)NULL;
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmFloatingPointSingle: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());
}


// ********************************


DcmFloatingPointSingle::~DcmFloatingPointSingle()
{
Bdebug((5, "dcvrfl:DcmFloatingPointSingle::~DcmFloatingPointSingle()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( FloatValue != (T_VR_FL*)NULL )
	delete FloatValue;
Edebug(());

}


// ********************************


DcmEVR DcmFloatingPointSingle::ident() const
{
    return EVR_FL;
}


// ********************************


void DcmFloatingPointSingle::print( int level )
{
    if ( FloatValue != (T_VR_FL*)NULL )
    {
	char *ch_words;
	char *tmp = ch_words = new char[ Length*18/sizeof(T_VR_FL) + 4 ];
	T_VR_FL *tattr = FloatValue;
	unsigned int i;

	for ( i=0; i<( Length/sizeof(T_VR_FL) ); i++ )
	{
// altes Format            sprintf( tmp, "%hg\\", *tattr );
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


T_VR_UL DcmFloatingPointSingle::getVM()
{
    return Length / sizeof(T_VR_FL);
}


// ********************************


E_Condition DcmFloatingPointSingle::readValueField( E_TransferSyntax xfer )
{
Bdebug((4, "dcvrfl:DcmFloatingPointSingle::readValueField(xfer)" ));

    E_Condition l_error = EC_Normal;
    long actpos;
    iDS->clear();
    if ( iDS->good() && iDS->fromFile() )
    {
	iDS->setDataByteOrder( xfer );
	if ( FloatValue != (T_VR_FL*)NULL )
	    delete FloatValue;
	FloatValue = new T_VR_FL[ Length / sizeof(T_VR_FL) ];
	actpos = iDS->tellg();
	iDS->seekg( offsetInFile );
	iDS->read( FloatValue, (int)Length / sizeof(T_VR_FL) );
						 // Warnung: evtl. int=16bit
Vdebug((1, !iDS->good(), "Warning: iDS->rdstate()=(0x%x)", iDS->rdstate() ));

        if ( Length != iDS->gcount() )
        {
            Length = iDS->gcount();                  // falls Datenblock zuende
            cerr << "Error: DcmFloatingPointSingle::readValueField() "
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


E_Condition DcmFloatingPointSingle::read( E_TransferSyntax xfer,
                                          E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrfl:DcmFloatingPointSingle::read()" ));

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
	    if ( FloatValue != (T_VR_FL*)NULL )
		delete FloatValue;
	    FloatValue = new T_VR_FL[ Length / sizeof(T_VR_FL) ];
	    iDS->read( FloatValue, (int)Length / sizeof(T_VR_FL) );
						 // Warnung: evtl. int=16bit
	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return EC_Normal;
}


// ********************************


E_Condition DcmFloatingPointSingle::readBlock( E_TransferSyntax xfer,
                                               E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrfl:DcmFloatingPointSingle::readBlock()" ));

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
	    if ( FloatValue != (T_VR_FL*)NULL )
		delete FloatValue;
	    FloatValue = new T_VR_FL[ Length / sizeof(T_VR_FL) ];
	    rdStat = ERW_inWork;
	}
	T_VR_UL len = (Length - bytesRead) <= iDS->buffered()
		      ? (Length - bytesRead)
		      : iDS->buffered();
	len -= len % sizeof(T_VR_FL);	     // lese nur ganze Werte
	iDS->read( &FloatValue[ bytesRead / sizeof(T_VR_FL) ],
		   (int)len / sizeof(T_VR_FL) );
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


E_Condition DcmFloatingPointSingle::write( oDicomStream &oDS,
                                           E_TransferSyntax oxfer,
                                           E_EncodingType enctype,
                                           E_GrpLenEncoding gltype )
{
Bdebug((3, "dcvrfl:DcmFloatingPointSingle::write(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
	if ( FloatValue == (T_VR_FL*)NULL )
	    Length = 0; 		  // Daten konnten nicht gelesen werden

	T_VR_UL written_bytes;
	errorFlag = DcmObject::writeTagAndLength( oDS, oxfer, &written_bytes );
debug(( 3, "Header_Bytes_written              =[0x%8.8x]", written_bytes ));

	if ( FloatValue != (T_VR_FL*)NULL && errorFlag == EC_Normal )
            oDS.write( FloatValue, (int)Length / sizeof(T_VR_FL) );
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


E_Condition DcmFloatingPointSingle::writeBlock( oDicomStream &oDS,
                                                E_TransferSyntax oxfer,
                                                E_EncodingType enctype,
                                                E_GrpLenEncoding gltype )
{
Bdebug((3, "dcvrfl:DcmFloatingPointSingle::writeBlock(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
		    if ( FloatValue == (T_VR_FL*)NULL )
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
	    if (    FloatValue != (T_VR_FL*)NULL
		 && errorFlag == EC_Normal
		 && writeState() != ERW_init )
	    {
                T_VR_UL len = (Length - bytesWritten) <= oDS.avail()
			      ? (Length - bytesWritten)
                              : oDS.avail();
		len -= len % sizeof(T_VR_FL);	    // lese nur ganze Werte
                oDS.write( &FloatValue[ bytesWritten / sizeof(T_VR_FL) ],
			    (int)len / sizeof(T_VR_FL) );
						     // Warnung: evtl. int=16bit
		bytesWritten += len;

		if ( bytesWritten == Length )
		    wrStat = ERW_ready;
		else
		    errorFlag = EC_InvalidStream;
	    } // FloatValue != NULL

	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointSingle::put( T_VR_FL *floatval,
					 T_VR_UL length )
{
    errorFlag = EC_Normal;
    if ( FloatValue != (T_VR_FL*)NULL )
	delete FloatValue;
    if ( floatval != (T_VR_FL*)NULL )
    {
	Length = length * sizeof(T_VR_FL);
	FloatValue = new T_VR_FL[ length ];
        memcpy( FloatValue, floatval, (int)Length );
    }
    else
    {
	FloatValue = (T_VR_FL*)NULL;
	Length = 0;
	errorFlag = EC_CorruptedData;
    }
    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointSingle::put( T_VR_FL floatval )
{
    errorFlag = EC_Normal;

    if ( FloatValue != (T_VR_FL*)NULL )
        delete FloatValue;
    FloatValue = new T_VR_FL[1];
    Length = 1 * sizeof(T_VR_FL);
    FloatValue[0] = floatval;

    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointSingle::put( T_VR_FL floatval,
                                         T_VR_UL num )
{
Bdebug((2, "dcvrul:DcmFloatingPointSingle::put(floatval=%ld,num=%ld)", floatval, num ));

    errorFlag = EC_Normal;
    T_VR_UL oldMaxNum = Length / sizeof(T_VR_FL);
    if ( num >= oldMaxNum )
    {
        // neuen Speicher alloziieren und alte Values kopieren
        T_VR_FL *newFloatValue = new T_VR_FL[ num+1 ];
        if ( newFloatValue == (T_VR_FL*)NULL )
        {
            cerr << "Error: DcmFloatingPointSingle::put() no more memory available"
                 << endl;
            errorFlag = EC_IllegalCall;
        }
        else
        {
           T_VR_UL newLength = (num+1) * sizeof(T_VR_FL);

           if ( FloatValue != (T_VR_FL*)NULL )
               memcpy( newFloatValue, FloatValue, (int)Length );
           memset( newFloatValue+Length, 0, (size_t)(newLength - Length) );

           if ( FloatValue != (T_VR_FL*)NULL )
               delete FloatValue;
           FloatValue = newFloatValue;
           Length = newLength;
debug(( 2, "DcmFloatingPointSingle:: Internal ValueField enlarged. Actual Length = %ld",
           Length ));

        }
    }

    if ( errorFlag == EC_Normal )
    {
        FloatValue[num] = floatval;

        valueInMemory = TRUE;
        valueModified = TRUE;
    }
Edebug(());

    return errorFlag;
}


// ********************************


T_VR_FL* DcmFloatingPointSingle::get()
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( FloatValue == (T_VR_FL*)NULL )
	Length = 0;		      // Daten konnten nicht gelesen werden
    return FloatValue;
}


// ********************************


T_VR_FL DcmFloatingPointSingle::get( T_VR_UL num )
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( FloatValue == (T_VR_FL*)NULL )
	Length = 0;		      // Daten konnten nicht gelesen werden

    if ( num < Length / sizeof(T_VR_FL) )
    {
        return FloatValue[num];
    }
    else
    {
        if ( errorFlag == EC_Normal )
            errorFlag = EC_IllegalCall;
        return 0;
    }
}


// ********************************


E_Condition DcmFloatingPointSingle::clear()
{
    errorFlag = EC_Normal;
    if ( FloatValue != (T_VR_FL*)NULL )
	delete FloatValue;
    FloatValue = (T_VR_FL*)NULL;
    Length = 0;
    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointSingle::verify( BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( Length % (sizeof(T_VR_FL)) != 0 )
    {
	errorFlag = EC_CorruptedData;
	if ( autocorrect == TRUE )
	{

	    Length = Length - ( Length % (sizeof(T_VR_FL)) );
					    // auf gueltige Laenge kuerzen
            valueModified = TRUE;
	}
    }
    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointSingle::loadAllDataIntoMemory()
{
    E_Condition l_error = EC_Normal;
    if ( valueInMemory == FALSE )
    {                                       // Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
        if ( FloatValue == NULL )
            Length = 0;                     // Daten konnten nicht gelesen
                                            // werden, Laenge korrigieren,
                                            // errorFlag enthaelt bereits
                                            // Fehlercode
        l_error = errorFlag;                // errorFlag nur bei Fehler aendern
    }
    return l_error;
}


// ********************************


