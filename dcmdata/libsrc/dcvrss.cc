/*
 *
 * Author: Gerd Ehlers      Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrss.cc
 *
 * Purpose:
 * Implementation of class DcmSignedShort
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

#include "dcvrss.h"
#include "dcdebug.h"



// ********************************


DcmSignedShort::DcmSignedShort( DcmTag &tag )
    : DcmElement( tag )
{
Bdebug((5, "dcvrss:DcmSignedShort::DcmSignedShort(DcmTag&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    SigShortValue = (T_VR_SS*)NULL;
Edebug(());

}


// ********************************


DcmSignedShort::DcmSignedShort( DcmTag &tag,
				T_VR_UL len,
				iDicomStream *iDStream )
    : DcmElement( tag, len, iDStream )
{
Bdebug((5, "dcvrss:DcmSignedShort::DcmSignedShort(DcmTag&,len=%ld,*iDS)",
           len ));
debug(( 8, "Object pointer this=0x%p", this ));

    SigShortValue = (T_VR_SS*)NULL;
Edebug(());

}


// ********************************


DcmSignedShort::DcmSignedShort( const DcmObject &oldObj )
    : DcmElement( InternalUseTag )
{
Bdebug((5, "dcvrss:DcmSignedShort::DcmSignedShort(DcmObject&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( oldObj.ident() == EVR_SS )
    {
        DcmSignedShort const *old = (DcmSignedShort const *)&oldObj;
        *Tag = *old->Tag;
        iDS = old->iDS;
        offsetInFile  = old->offsetInFile;
        valueInMemory = old->valueInMemory;
        valueModified = old->valueModified;
        Length = old->Length;
        Xfer = old->Xfer;
        if (    Length == 0
             || old->SigShortValue == (T_VR_SS*)NULL )
        {
            if ( valueInMemory )
                Length = 0;
            SigShortValue = (T_VR_SS*)NULL;
        }
        else
        {
            SigShortValue = new T_VR_SS[ Length / sizeof(T_VR_SS) ];
            memcpy( SigShortValue,
                    old->SigShortValue,
                    (int)Length );
        }
    }
    else
    {
        SigShortValue = (T_VR_SS*)NULL;
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmSignedShort: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmSignedShort::DcmSignedShort( const DcmSignedShort &newSS )
    : DcmElement( InternalUseTag )
{
Bdebug((5, "dcvrss:DcmSignedShort::DcmSignedShort(DcmSignedShort&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( newSS.ident() == EVR_SS )
    {
        DcmSignedShort const *old = &newSS;
        *Tag = *old->Tag;
        iDS = old->iDS;
        offsetInFile  = old->offsetInFile;
        valueInMemory = old->valueInMemory;
        valueModified = old->valueModified;
        Length = old->Length;
        Xfer = old->Xfer;
        if (    Length == 0
             || old->SigShortValue == (T_VR_SS*)NULL )
        {
            if ( valueInMemory )
                Length = 0;
            SigShortValue = (T_VR_SS*)NULL;
        }
        else
        {
            SigShortValue = new T_VR_SS[ Length / sizeof(T_VR_SS) ];
            memcpy( SigShortValue,
                    old->SigShortValue,
                    (int)Length );
        }
    }
    else
    {
        SigShortValue = (T_VR_SS*)NULL;
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmSignedShort: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmSignedShort::~DcmSignedShort()
{
Bdebug((5, "dcvrss:DcmSignedShort::~DcmSignedShort()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( SigShortValue != (T_VR_SS*)NULL )
	delete SigShortValue;
Edebug(());

}


// ********************************


EVR DcmSignedShort::ident() const
{
    return EVR_SS;
}


// ********************************


void DcmSignedShort::print( int level )
{
    if ( SigShortValue != (T_VR_SS*)NULL )
    {
	char *ch_words;
	char *tmp = ch_words = new char[ Length*8/sizeof(T_VR_SS) + 2 ];
	T_VR_SS *tattr = SigShortValue;
	unsigned int i;

	for ( i=0; i<( Length/sizeof(T_VR_SS) ); i++ )
	{
	    sprintf( tmp, "%hd\\", *tattr );
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


T_VR_UL DcmSignedShort::getVM()
{
    return Length / sizeof(T_VR_SS);
}


// ********************************


E_Condition DcmSignedShort::readValueField( E_TransferSyntax xfer )
{
Bdebug((4, "dcvrss:DcmSignedShort::readValueField(xfer)" ));

    E_Condition l_error = EC_Normal;
    long actpos;
    iDS->clear();
    if ( iDS->good() && iDS->fromFile() )
    {
	iDS->setDataByteOrder( xfer );
	if ( SigShortValue != (T_VR_SS*)NULL )
	    delete SigShortValue;
	SigShortValue = new T_VR_SS[ Length / sizeof(T_VR_SS) ];
	actpos = iDS->tellg();
	iDS->seekg( offsetInFile );
	iDS->read( SigShortValue, (int)Length / sizeof(T_VR_SS) );
						 // Warnung: evtl. int=16bit
Vdebug((1, !iDS->good(), "Warning: iDS->rdstate()=(0x%x)", iDS->rdstate() ));

        if ( Length != iDS->gcount() )
        {
            Length = iDS->gcount();                  // falls Datenblock zuende
            cerr << "Error: DcmSignedShort::readValueField() "
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


E_Condition DcmSignedShort::read( E_TransferSyntax xfer,
                                  E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrss:DcmSignedShort::read()" ));

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
	    if ( SigShortValue != (T_VR_SS*)NULL )
		delete SigShortValue;
	    SigShortValue = new T_VR_SS[ Length / sizeof(T_VR_SS) ];
	    iDS->read( SigShortValue, (int)Length / sizeof(T_VR_SS) );
						 // Warnung: evtl. int=16bit
	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return EC_Normal;
}


// ********************************


E_Condition DcmSignedShort::readBlock( E_TransferSyntax xfer,
                                       E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrss:DcmSignedShort::readBlock()" ));

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
	    if ( SigShortValue != (T_VR_SS*)NULL )
		delete SigShortValue;
	    SigShortValue = new T_VR_SS[ Length / sizeof(T_VR_SS) ];
	    rdStat = ERW_inWork;
	}
	T_VR_UL len = (Length - bytesRead) <= iDS->buffered()
		      ? (Length - bytesRead)
		      : iDS->buffered();
	len -= len % sizeof(T_VR_SS);	     // lese nur ganze Werte
	iDS->read( &SigShortValue[ bytesRead / sizeof(T_VR_SS) ],
		   (int)len / sizeof(T_VR_SS) );
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


E_Condition DcmSignedShort::write( oDicomStream &oDS,
                                   E_TransferSyntax oxfer,
                                   E_EncodingType enctype,
                                   E_GrpLenEncoding gltype )
{
Bdebug((3, "dcvrss:DcmSignedShort::write(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
	if ( SigShortValue == (T_VR_SS*)NULL )
	    Length = 0; 		  // Daten konnten nicht gelesen werden

	T_VR_UL written_bytes;
	errorFlag = DcmObject::writeTagAndLength( oDS, oxfer, &written_bytes );
debug(( 3, "Header_Bytes_written              =[0x%8.8x]", written_bytes ));

	if ( SigShortValue != (T_VR_SS*)NULL && errorFlag == EC_Normal )
            oDS.write( SigShortValue, (int)Length / sizeof(T_VR_SS) );
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


E_Condition DcmSignedShort::writeBlock( oDicomStream &oDS,
                                        E_TransferSyntax oxfer,
                                        E_EncodingType enctype,
                                        E_GrpLenEncoding gltype )
{
Bdebug((3, "dcvrss:DcmSignedShort::writeBlock(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
		    if ( SigShortValue == (T_VR_SS*)NULL )
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
	    if (    SigShortValue != (T_VR_SS*)NULL
		 && errorFlag == EC_Normal
		 && writeState() != ERW_init )
	    {
                T_VR_UL len = (Length - bytesWritten) <= oDS.avail()
			      ? (Length - bytesWritten)
                              : oDS.avail();
		len -= len % sizeof(T_VR_SS);	    // lese nur ganze Werte
                oDS.write( &SigShortValue[ bytesWritten / sizeof(T_VR_SS) ],
			    (int)len / sizeof(T_VR_SS) );
						     // Warnung: evtl. int=16bit
		bytesWritten += len;

		if ( bytesWritten == Length )
		    wrStat = ERW_ready;
		else
		    errorFlag = EC_InvalidStream;
	    } // SigShortValue != NULL

	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmSignedShort::put( T_VR_SS *sshort,
				 T_VR_UL length )
{
    errorFlag = EC_Normal;
    if ( SigShortValue != (T_VR_SS*)NULL )
	delete SigShortValue;
    if ( sshort != (T_VR_SS*)NULL )
    {
	Length = length * sizeof(T_VR_SS);
	SigShortValue = new T_VR_SS[ length ];
        memcpy( SigShortValue, sshort, (int)Length );
    }
    else
    {
	SigShortValue = (T_VR_SS*)NULL;
	Length = 0;
	errorFlag = EC_CorruptedData;
    }
    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


E_Condition DcmSignedShort::put( T_VR_SS sshort )
{
    errorFlag = EC_Normal;

    if ( SigShortValue != (T_VR_SS*)NULL )
        delete SigShortValue;
    SigShortValue = new T_VR_SS[1];
    Length = 1 * sizeof(T_VR_SS);
    SigShortValue[0] = sshort;

    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


E_Condition DcmSignedShort::put( T_VR_SS sshort,
                                 T_VR_UL num )
{
Bdebug((2, "dcvrul:DcmSignedShort::put(sshort=%ld,num=%ld)", sshort, num ));

    errorFlag = EC_Normal;
    T_VR_UL oldMaxNum = Length / sizeof(T_VR_SS);
    if ( num >= oldMaxNum )
    {
        // neuen Speicher alloziieren und alte Values kopieren
        T_VR_SS *newSigShortValue = new T_VR_SS[ num+1 ];
        if ( newSigShortValue == (T_VR_SS*)NULL )
        {
            cerr << "Error: DcmSignedShort::put() no more memory available"
                 << endl;
            errorFlag = EC_IllegalCall;
        }
        else
        {
           T_VR_UL newLength = (num+1) * sizeof(T_VR_SS);

           if ( SigShortValue != (T_VR_SS*)NULL )
               memcpy( newSigShortValue, SigShortValue, (int)Length );
           memset( newSigShortValue+Length, 0, (size_t)(newLength - Length) );

           if ( SigShortValue != (T_VR_SS*)NULL )
               delete SigShortValue;
           SigShortValue = newSigShortValue;
           Length = newLength;
debug(( 2, "DcmSignedShort:: Internal ValueField enlarged. Actual Length = %ld",
           Length ));

        }
    }

    if ( errorFlag == EC_Normal )
    {
        SigShortValue[num] = sshort;

        valueInMemory = TRUE;
        valueModified = TRUE;
    }
Edebug(());

    return errorFlag;
}


// ********************************


T_VR_SS* DcmSignedShort::get()
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( SigShortValue == (T_VR_SS*)NULL )
	Length = 0;		      // Daten konnten nicht gelesen werden
    return SigShortValue;
}


// ********************************


T_VR_SS DcmSignedShort::get( T_VR_UL num )
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( SigShortValue == (T_VR_SS*)NULL )
	Length = 0;		      // Daten konnten nicht gelesen werden

    if ( num < Length / sizeof(T_VR_SS) )
    {
        return SigShortValue[num];
    }
    else
    {
        if ( errorFlag == EC_Normal )
            errorFlag = EC_IllegalCall;
        return 0;
    }
}


// ********************************


E_Condition DcmSignedShort::clear()
{
    errorFlag = EC_Normal;
    if ( SigShortValue != (T_VR_SS*)NULL )
	delete SigShortValue;
    SigShortValue = (T_VR_SS*)NULL;
    Length = 0;
    return errorFlag;
}


// ********************************


E_Condition DcmSignedShort::verify( BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( Length % (sizeof(T_VR_SS)) != 0 )
    {
	errorFlag = EC_CorruptedData;
	if ( autocorrect == TRUE )
	{

	    Length = Length - ( Length % (sizeof(T_VR_SS)) );
					    // auf gueltige Laenge kuerzen
            valueModified = TRUE;
	}
    }
    return errorFlag;
}


// ********************************


E_Condition DcmSignedShort::loadAllDataIntoMemory()
{
    E_Condition l_error = EC_Normal;
    if ( valueInMemory == FALSE )
    {                                       // Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
        if ( SigShortValue == NULL )
            Length = 0;                     // Daten konnten nicht gelesen
                                            // werden, Laenge korrigieren,
                                            // errorFlag enthaelt bereits
                                            // Fehlercode
        l_error = errorFlag;                // errorFlag nur bei Fehler aendern
    }
    return l_error;
}


// ********************************


