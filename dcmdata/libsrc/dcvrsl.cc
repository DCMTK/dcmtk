/*
 *
 * Author: Gerd Ehlers      Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrsl.cc
 *
 * Purpose:
 * Implementation of class DcmSignedLong
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */



#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrsl.h"
#include "dcdebug.h"



// ********************************


DcmSignedLong::DcmSignedLong( DcmTag &tag )
    : DcmElement( tag )
{
Bdebug((5, "dcvrsl:DcmSignedLong::DcmSignedLong(DcmTag&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    SigLongValue = (T_VR_SL*)NULL;
Edebug(());

}


// ********************************


DcmSignedLong::DcmSignedLong( DcmTag &tag,
			      T_VR_UL len,
			      iDicomStream *iDStream )
    : DcmElement( tag, len, iDStream )
{
Bdebug((5, "dcvrsl:DcmSignedLong::DcmSignedLong(DcmTag&,len=%ld,*iDS)",
           len ));
debug(( 8, "Object pointer this=0x%p", this ));

    SigLongValue = (T_VR_SL*)NULL;
Edebug(());

}


// ********************************


DcmSignedLong::DcmSignedLong( const DcmObject &oldObj )
    : DcmElement( InternalUseTag )
{
Bdebug((5, "dcvrsl:DcmSignedLong::DcmSignedLong(DcmObject&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( oldObj.ident() == EVR_SL )
    {
        DcmSignedLong const *old = (DcmSignedLong const *)&oldObj;
        *Tag = *old->Tag;
        iDS = old->iDS;
        offsetInFile  = old->offsetInFile;
        valueInMemory = old->valueInMemory;
        valueModified = old->valueModified;
        Length = old->Length;
        Xfer = old->Xfer;
        if (    Length == 0
             || old->SigLongValue == (T_VR_SL*)NULL )
        {
            if ( valueInMemory )
                Length = 0;
            SigLongValue = (T_VR_SL*)NULL;
        }
        else
        {
            SigLongValue = new T_VR_SL[ Length / sizeof(T_VR_SL) ];
            memcpy( SigLongValue,
                    old->SigLongValue,
                    (int)Length );
        }
    }
    else
    {
        SigLongValue = (T_VR_SL*)NULL;
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmSignedLong: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmSignedLong::DcmSignedLong( const DcmSignedLong &newSL )
    : DcmElement( InternalUseTag )
{
Bdebug((5, "dcvrsl:DcmSignedLong::DcmSignedLong(DcmSignedLong&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( newSL.ident() == EVR_SL )
    {
        DcmSignedLong const *old = &newSL;
        *Tag = *old->Tag;
        iDS = old->iDS;
        offsetInFile  = old->offsetInFile;
        valueInMemory = old->valueInMemory;
        valueModified = old->valueModified;
        Length = old->Length;
        Xfer = old->Xfer;
        if (    Length == 0
             || old->SigLongValue == (T_VR_SL*)NULL )
        {
            if ( valueInMemory )
                Length = 0;
            SigLongValue = (T_VR_SL*)NULL;
        }
        else
        {
            SigLongValue = new T_VR_SL[ Length / sizeof(T_VR_SL) ];
            memcpy( SigLongValue,
                    old->SigLongValue,
                    (int)Length );
        }
    }
    else
    {
        SigLongValue = (T_VR_SL*)NULL;
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmSignedLong: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmSignedLong::~DcmSignedLong()
{
Bdebug((5, "dcvrsl:DcmSignedLong::~DcmSignedLong()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( SigLongValue != (T_VR_SL*)NULL )
	delete SigLongValue;
Edebug(());

}


// ********************************


EVR DcmSignedLong::ident() const
{
    return EVR_SL;
}


// ********************************


void DcmSignedLong::print( int level )
{
    if ( SigLongValue != (T_VR_SL*)NULL )
    {
	char *ch_words;
	char *tmp = ch_words = new char[ Length*12/sizeof(T_VR_SL) + 4 ];
	T_VR_SL *tattr = SigLongValue;
	unsigned int i;

	for ( i=0; i<( Length/sizeof(T_VR_SL) ); i++ )
	{
	    sprintf( tmp, "%ld\\", *tattr );
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


T_VR_UL DcmSignedLong::getVM()
{
    return Length / sizeof(T_VR_SL);
}


// ********************************


E_Condition DcmSignedLong::readValueField( E_TransferSyntax xfer )
{
Bdebug((4, "dcvrsl:DcmSignedLong::readValueField(xfer)" ));

    E_Condition l_error = EC_Normal;
    long actpos;
    iDS->clear();
    if ( iDS->good() && iDS->fromFile() )
    {
	iDS->setDataByteOrder( xfer );
	if ( SigLongValue != (T_VR_SL*)NULL )
	    delete SigLongValue;
	SigLongValue = new T_VR_SL[ Length / sizeof(T_VR_SL) ];
	actpos = iDS->tellg();
	iDS->seekg( offsetInFile );
	iDS->read( SigLongValue, (int)Length / sizeof(T_VR_SL) );
						 // Warnung: evtl. int=16bit
Vdebug((1, !iDS->good(), "Warning: iDS->rdstate()=(0x%x)", iDS->rdstate() ));

        if ( Length != iDS->gcount() )
        {
            Length = iDS->gcount();                  // falls Datenblock zuende
            cerr << "Error: DcmSignedLong::readValueField() "
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


E_Condition DcmSignedLong::read( E_TransferSyntax xfer,
                                 E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrsl:DcmSignedLong::read()" ));

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
	    if ( SigLongValue != (T_VR_SL*)NULL )
		delete SigLongValue;
	    SigLongValue = new T_VR_SL[ Length / sizeof(T_VR_SL) ];
	    iDS->read( SigLongValue, (int)Length / sizeof(T_VR_SL) );
						 // Warnung: evtl. int=16bit
	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return EC_Normal;
}


// ********************************


E_Condition DcmSignedLong::readBlock( E_TransferSyntax xfer,
                                      E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrsl:DcmSignedLong::readBlock()" ));

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
	    if ( SigLongValue != (T_VR_SL*)NULL )
		delete SigLongValue;
	    SigLongValue = new T_VR_SL[ Length / sizeof(T_VR_SL) ];
	    rdStat = ERW_inWork;
	}
	T_VR_UL len = (Length - bytesRead) <= iDS->buffered()
		      ? (Length - bytesRead)
		      : iDS->buffered();
	len -= len % sizeof(T_VR_SL);	     // lese nur ganze Werte
	iDS->read( &SigLongValue[ bytesRead / sizeof(T_VR_SL) ],
		   (int)len / sizeof(T_VR_SL) );
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


E_Condition DcmSignedLong::write( oDicomStream &oDS,
                                  E_TransferSyntax oxfer,
                                  E_EncodingType enctype,
                                  E_GrpLenEncoding gltype )
{
Bdebug((3, "dcvrsl:DcmSignedLong::write(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
	if ( SigLongValue == (T_VR_SL*)NULL )
	    Length = 0; 		  // Daten konnten nicht gelesen werden

	T_VR_UL written_bytes;
	errorFlag = DcmObject::writeTagAndLength( oDS, oxfer, &written_bytes );
debug(( 3, "Header_Bytes_written              =[0x%8.8x]", written_bytes ));

	if ( SigLongValue != (T_VR_SL*)NULL && errorFlag == EC_Normal )
            oDS.write( SigLongValue, (int)Length / sizeof(T_VR_SL) );
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


E_Condition DcmSignedLong::writeBlock( oDicomStream &oDS,
				       E_TransferSyntax oxfer,
                                       E_EncodingType enctype,
                                       E_GrpLenEncoding gltype )
{
Bdebug((3, "dcvrsl:DcmSignedLong::writeBlock(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
		    if ( SigLongValue == (T_VR_SL*)NULL )
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
	    if (    SigLongValue != (T_VR_SL*)NULL
		 && errorFlag == EC_Normal
		 && writeState() != ERW_init )
	    {
                T_VR_UL len = (Length - bytesWritten) <= oDS.avail()
			      ? (Length - bytesWritten)
                              : oDS.avail();
		len -= len % sizeof(T_VR_SL);	    // lese nur ganze Werte
                oDS.write( &SigLongValue[ bytesWritten / sizeof(T_VR_SL) ],
			    (int)len / sizeof(T_VR_SL) );
						     // Warnung: evtl. int=16bit
		bytesWritten += len;

		if ( bytesWritten == Length )
		    wrStat = ERW_ready;
		else
		    errorFlag = EC_InvalidStream;
	    } // SigLongValue != NULL

	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmSignedLong::put( T_VR_SL *slong,
				T_VR_UL length )
{
    errorFlag = EC_Normal;
    if ( SigLongValue != (T_VR_SL*)NULL )
	delete SigLongValue;
    if ( slong != (T_VR_SL*)NULL )
    {
	Length = length * sizeof(T_VR_SL);
	SigLongValue = new T_VR_SL[ length ];
        memcpy( SigLongValue, slong, (int)Length );
    }
    else
    {
	SigLongValue = (T_VR_SL*)NULL;
	Length = 0;
	errorFlag = EC_CorruptedData;
    }
    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


E_Condition DcmSignedLong::put( T_VR_SL slong )
{
    errorFlag = EC_Normal;

    if ( SigLongValue != (T_VR_SL*)NULL )
        delete SigLongValue;
    SigLongValue = new T_VR_SL[1];
    Length = 1 * sizeof(T_VR_SL);
    SigLongValue[0] = slong;

    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


E_Condition DcmSignedLong::put( T_VR_SL slong,
                                T_VR_UL num )
{
Bdebug((2, "dcvrul:DcmSignedLong::put(slong=%ld,num=%ld)", slong, num ));

    errorFlag = EC_Normal;
    T_VR_UL oldMaxNum = Length / sizeof(T_VR_SL);
    if ( num >= oldMaxNum )
    {
        // neuen Speicher alloziieren und alte Values kopieren
        T_VR_SL *newSigLongValue = new T_VR_SL[ num+1 ];
        if ( newSigLongValue == (T_VR_SL*)NULL )
        {
            cerr << "Error: DcmSignedLong::put() no more memory available"
                 << endl;
            errorFlag = EC_IllegalCall;
        }
        else
        {
           T_VR_UL newLength = (num+1) * sizeof(T_VR_SL);

           if ( SigLongValue != (T_VR_SL*)NULL )
               memcpy( newSigLongValue, SigLongValue, (int)Length );
           memset( newSigLongValue+Length, 0, (size_t)(newLength - Length) );

           if ( SigLongValue != (T_VR_SL*)NULL )
               delete SigLongValue;
           SigLongValue = newSigLongValue;
           Length = newLength;
debug(( 2, "DcmSignedLong:: Internal ValueField enlarged. Actual Length = %ld",
           Length ));

        }
    }

    if ( errorFlag == EC_Normal )
    {
        SigLongValue[num] = slong;

        valueInMemory = TRUE;
        valueModified = TRUE;
    }
Edebug(());

    return errorFlag;
}


// ********************************


T_VR_SL* DcmSignedLong::get()
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( SigLongValue == (T_VR_SL*)NULL )
	Length = 0;		      // Daten konnten nicht gelesen werden
    return SigLongValue;
}


// ********************************


T_VR_SL DcmSignedLong::get( T_VR_UL num )
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( SigLongValue == (T_VR_SL*)NULL )
	Length = 0;		      // Daten konnten nicht gelesen werden

    if ( num < Length / sizeof(T_VR_SL) )
    {
        return SigLongValue[num];
    }
    else
    {
        if ( errorFlag == EC_Normal )
            errorFlag = EC_IllegalCall;
        return 0;
    }
}


// ********************************


E_Condition DcmSignedLong::clear()
{
    errorFlag = EC_Normal;
    if ( SigLongValue != (T_VR_SL*)NULL )
	delete SigLongValue;
    SigLongValue = (T_VR_SL*)NULL;
    Length = 0;
    return errorFlag;
}


// ********************************


E_Condition DcmSignedLong::verify( BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( Length % (sizeof(T_VR_SL)) != 0 )
    {
	errorFlag = EC_CorruptedData;
	if ( autocorrect == TRUE )
	{

	    Length = Length - ( Length % (sizeof(T_VR_SL)) );
					    // auf gueltige Laenge kuerzen
            valueModified = TRUE;
	}
    }
    return errorFlag;
}


// ********************************


E_Condition DcmSignedLong::loadAllDataIntoMemory()
{
    E_Condition l_error = EC_Normal;
    if ( valueInMemory == FALSE )
    {                                       // Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
        if ( SigLongValue == NULL )
            Length = 0;                     // Daten konnten nicht gelesen
                                            // werden, Laenge korrigieren,
                                            // errorFlag enthaelt bereits
                                            // Fehlercode
        l_error = errorFlag;                // errorFlag nur bei Fehler aendern
    }
    return l_error;
}


// ********************************


