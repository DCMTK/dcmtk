/*
 *
 * Author: Gerd Ehlers      Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrat.cc
 *
 * Purpose:
 * Implementation of class DcmAttributeTag
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

#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrat.h"
#include "dcdebug.h"



// ********************************


DcmAttributeTag::DcmAttributeTag( DcmTag &tag )
    : DcmElement( tag )
{
Bdebug((5, "dcvrat:DcmAttributeTag::DcmAttributeTag(DcmTag&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    AttrValue = (T_VR_US*)NULL;
Edebug(());

}


// ********************************


DcmAttributeTag::DcmAttributeTag( DcmTag &tag,
				  T_VR_UL len,
				  iDicomStream *iDStream )
    : DcmElement( tag, len, iDStream )
{
Bdebug((5, "dcvrat:DcmAttributeTag::DcmAttributeTag(DcmTag&,len=%ld,*iDS)",
           len ));
debug(( 8, "Object pointer this=0x%p", this ));

    AttrValue = (T_VR_US*)NULL;
Edebug(());

}


// ********************************


DcmAttributeTag::DcmAttributeTag( const DcmObject &oldObj )
    : DcmElement( InternalUseTag )
{
Bdebug((5, "dcvrat:DcmAttributeTag::DcmAttributeTag(DcmObject&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( oldObj.ident() == EVR_AT )
    {
        DcmAttributeTag const *old = (DcmAttributeTag const *)&oldObj;
        *Tag = *old->Tag;
        iDS = old->iDS;
        offsetInFile  = old->offsetInFile;
        valueInMemory = old->valueInMemory;
        valueModified = old->valueModified;
        Length = old->Length;
        Xfer = old->Xfer;
        if (    Length == 0
             || old->AttrValue == (T_VR_US*)NULL )
        {
            if ( valueInMemory )
                Length = 0;
            AttrValue = (T_VR_US*)NULL;
        }
        else
        {
            AttrValue = new T_VR_US[ Length / sizeof(T_VR_US) ];
            memcpy( AttrValue,
                    old->AttrValue,
                    (int)Length );
        }
    }
    else
    {
        AttrValue = (T_VR_US*)NULL;
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmAttributeTag: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmAttributeTag::DcmAttributeTag( const DcmAttributeTag &newAT )
    : DcmElement( InternalUseTag )
{
Bdebug((5, "dcvrat:DcmAttributeTag::DcmAttributeTag(DcmAttributeTag&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( newAT.ident() == EVR_AT )
    {
        DcmAttributeTag const *old = &newAT;
        *Tag = *old->Tag;
        iDS = old->iDS;
        offsetInFile  = old->offsetInFile;
        valueInMemory = old->valueInMemory;
        valueModified = old->valueModified;
        Length = old->Length;
        Xfer = old->Xfer;
        if (    Length == 0
             || old->AttrValue == (T_VR_US*)NULL )
        {
            if ( valueInMemory )
                Length = 0;
            AttrValue = (T_VR_US*)NULL;
        }
        else
        {
            AttrValue = new T_VR_US[ Length / sizeof(T_VR_US) ];
            memcpy( AttrValue,
                    old->AttrValue,
                    (int)Length );
        }
    }
    else
    {
        AttrValue = (T_VR_US*)NULL;
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmAttributeTag: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmAttributeTag::~DcmAttributeTag()
{
Bdebug((5, "dcvrat:DcmAttributeTag::~DcmAttributeTag()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( AttrValue != (T_VR_US*)NULL )
	delete AttrValue;
Edebug(());

}


// ********************************


EVR DcmAttributeTag::ident() const
{
    return EVR_AT;
}


// ********************************


void DcmAttributeTag::print( int level )
{
    if ( AttrValue != (T_VR_US*)NULL )
    {
	char *ch_words;
        char *tmp = ch_words = new char[ Length*6/sizeof(T_VR_US) + 9 ];
	T_VR_US *tattr = AttrValue;
	unsigned int i;

	for ( i=0; i<( Length/(2*sizeof(T_VR_US)) ); i++ )
	{
            sprintf( tmp, "(%4.4x,%4.4x)\\", *tattr, *(tattr+1) );
            tmp += 12;
	    tattr += 2;
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


T_VR_UL DcmAttributeTag::getVM()
{
    return Length / ( 2 * sizeof(T_VR_US) );
}


// ********************************


E_Condition DcmAttributeTag::readValueField( E_TransferSyntax xfer )
{
Bdebug((4, "dcvrat:DcmAttributeTag::readValueField(xfer)" ));

    E_Condition l_error = EC_Normal;
    long actpos;
    iDS->clear();
    if ( iDS->good() && iDS->fromFile() )
    {
	iDS->setDataByteOrder( xfer );
	if ( AttrValue != (T_VR_US*)NULL )
	    delete AttrValue;
	AttrValue = new T_VR_US[ Length / sizeof(T_VR_US) ];
	actpos = iDS->tellg();
	iDS->seekg( offsetInFile );
	iDS->read( AttrValue, (int)Length / sizeof(T_VR_US) );
                                                 // Warnung: evtl. int=16bit
Vdebug((1, !iDS->good(), "Warning: iDS->rdstate()=(0x%x)", iDS->rdstate() ));

        if ( Length != iDS->gcount() )
        {
            Length = iDS->gcount();                  // falls Datenblock zuende
            cerr << "Error: DcmAttributeTag::readValueField() "
                    "unexpected EndOfStream, state=("
                 << iDS->rdstate() << ")" << endl;
            iDS->clear();
            l_error = EC_CorruptedData;
        }
	iDS->seekg( actpos );
	valueInMemory = TRUE;
	valueModified = FALSE;
        this->verify( TRUE );     // sicherstellen, dass Tag immer paarweise ist
    }
    else
        l_error = EC_InvalidStream;
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmAttributeTag::read( E_TransferSyntax xfer,
                                   E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrat:DcmAttributeTag::read()" ));

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
	    if ( AttrValue != (T_VR_US*)NULL )
		delete AttrValue;
	    AttrValue = new T_VR_US[ Length / sizeof(T_VR_US) ];
	    iDS->read( AttrValue, (int)Length / sizeof(T_VR_US) );
						 // Warnung: evtl. int=16bit
	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return EC_Normal;
}


// ********************************


E_Condition DcmAttributeTag::readBlock( E_TransferSyntax xfer,
                                        E_GrpLenEncoding /*gltype*/ )
{
Bdebug((3, "dcvrat:DcmAttributeTag::readBlock()" ));

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
	    if ( AttrValue != (T_VR_US*)NULL )
		delete AttrValue;
	    AttrValue = new T_VR_US[ Length / sizeof(T_VR_US) ];
	    rdStat = ERW_inWork;
	}
	T_VR_UL len = (Length - bytesRead) <= iDS->buffered()
		      ? (Length - bytesRead)
		      : iDS->buffered();
	len -= len % sizeof(T_VR_US);	     // lese nur ganze Werte
	iDS->read( &AttrValue[ bytesRead / sizeof(T_VR_US) ],
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


E_Condition DcmAttributeTag::write( oDicomStream &oDS,
                                    E_TransferSyntax oxfer,
                                    E_EncodingType enctype,
                                    E_GrpLenEncoding gltype )
{
Bdebug((3, "dcvrat:DcmAttributeTag::write(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
	if ( AttrValue == (T_VR_US*)NULL )
	    Length = 0; 		  // Daten konnten nicht gelesen werden

	T_VR_UL written_bytes;
	errorFlag = DcmObject::writeTagAndLength( oDS, oxfer, &written_bytes );
debug(( 3, "Header_Bytes_written              =[0x%8.8x]", written_bytes ));

	if ( AttrValue != (T_VR_US*)NULL && errorFlag == EC_Normal )
            oDS.write( AttrValue, (int)Length / sizeof(T_VR_US) );
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


E_Condition DcmAttributeTag::writeBlock( oDicomStream &oDS,
                                         E_TransferSyntax oxfer,
                                         E_EncodingType enctype,
                                         E_GrpLenEncoding gltype )
{
Bdebug((3, "dcvrat:DcmAttributeTag::writeBlock(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
		    if ( AttrValue == (T_VR_US*)NULL )
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
	    if (    AttrValue != (T_VR_US*)NULL
		 && errorFlag == EC_Normal
		 && writeState() != ERW_init )
	    {
                T_VR_UL len = (Length - bytesWritten) <= oDS.avail()
			      ? (Length - bytesWritten)
                              : oDS.avail();
		len -= len % sizeof(T_VR_US);	    // lese nur ganze Werte
                oDS.write( &AttrValue[ bytesWritten / sizeof(T_VR_US) ],
			    (int)len / sizeof(T_VR_US) );
						     // Warnung: evtl. int=16bit
		bytesWritten += len;

		if ( bytesWritten == Length )
		    wrStat = ERW_ready;
		else
		    errorFlag = EC_InvalidStream;
	    } // AttrValue != NULL

	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmAttributeTag::put( T_VR_US *attrvalue,
				  T_VR_UL length )
{
    errorFlag = EC_Normal;
    if ( AttrValue != (T_VR_US*)NULL )
	delete AttrValue;
    if ( attrvalue != (T_VR_US*)NULL )
    {
        length &= 0xfffffffe;                 // shorts immer paarweise!
	Length = length * sizeof(T_VR_US);
	AttrValue = new T_VR_US[ length ];
	memcpy( AttrValue, attrvalue, (int)Length );
    }
    else
    {
	AttrValue = (T_VR_US*)NULL;
	Length = 0;
	errorFlag = EC_CorruptedData;
    }
    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


E_Condition DcmAttributeTag::put( DcmTag &attrTag )
{
    errorFlag = EC_Normal;

    if ( AttrValue != (T_VR_US*)NULL )
        delete AttrValue;
    AttrValue = new T_VR_US[2];
    Length = 2 * sizeof(T_VR_US);
    AttrValue[0] = attrTag.getGTag();
    AttrValue[1] = attrTag.getETag();

    valueInMemory = TRUE;
    valueModified = TRUE;
    return errorFlag;
}


// ********************************


E_Condition DcmAttributeTag::put( DcmTag &attrTag,
                                  T_VR_UL num )
{
Bdebug((2, "dcvrat:DcmAttributeTag::put(DcmTag&,num=%ld)", num ));

    errorFlag = EC_Normal;
    T_VR_UL oldMaxNum = Length / (2*sizeof(T_VR_US));
    if ( num >= oldMaxNum )
    {
        // neuen Speicher alloziieren und alte Attribute kopieren
        T_VR_US *newAttrValue = new T_VR_US[ 2*(num+1) ];
        if ( newAttrValue == (T_VR_US*)NULL )
        {
            cerr << "Error: DcmAttributeTag::put() no more memory available"
                 << endl;
            errorFlag = EC_IllegalCall;
        }
        else
        {
           T_VR_UL newLength = 2 * (num+1) * sizeof(T_VR_US);

           if ( AttrValue != (T_VR_US*)NULL )
               memcpy( newAttrValue, AttrValue, (int)Length );
           memset( newAttrValue+Length, 0, (size_t)(newLength - Length) );

           if ( AttrValue != (T_VR_US*)NULL )
               delete AttrValue;
           AttrValue = newAttrValue;
           Length = newLength;
debug(( 2, "DcmAttributeTag:: Internal ValueField enlarged. Actual Length = %ld",
           Length ));

        }
    }

    if ( errorFlag == EC_Normal )
    {
        AttrValue[2*num]   = attrTag.getGTag();
        AttrValue[2*num+1] = attrTag.getETag();

        valueInMemory = TRUE;
        valueModified = TRUE;
    }
Edebug(());

    return errorFlag;
}


// ********************************


T_VR_US* DcmAttributeTag::get()
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( AttrValue == (T_VR_US*)NULL )
	Length = 0;		      // Daten konnten nicht gelesen werden
    return AttrValue;
}


// ********************************


DcmTag DcmAttributeTag::get( T_VR_UL num )
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( AttrValue == (T_VR_US*)NULL )
	Length = 0;		      // Daten konnten nicht gelesen werden

    if ( num < Length / (2*sizeof(T_VR_US)) )
    {
        DcmTag returnTag( AttrValue[2*num], AttrValue[2*num+1] );
        return returnTag;
    }
    else
    {
        if ( errorFlag == EC_Normal )
            errorFlag = EC_IllegalCall;
        DcmTag returnTag( ET_UNKNOWN );
        return returnTag;
    }
}


// ********************************


E_Condition DcmAttributeTag::clear()
{
    errorFlag = EC_Normal;
    if ( AttrValue != (T_VR_US*)NULL )
	delete AttrValue;
    AttrValue = (T_VR_US*)NULL;
    Length = 0;
    return errorFlag;
}


// ********************************


E_Condition DcmAttributeTag::verify( BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( Length % (2*sizeof(T_VR_US)) != 0 )
    {
	errorFlag = EC_CorruptedData;
	if ( autocorrect == TRUE )
	{

            Length = Length - ( Length % (2*sizeof(T_VR_US)) );
					    // auf gueltige Laenge kuerzen
            valueModified = TRUE;
	}
    }
    return errorFlag;
}


// ********************************


E_Condition DcmAttributeTag::loadAllDataIntoMemory()
{
    E_Condition l_error = EC_Normal;
    if ( valueInMemory == FALSE )
    {                                       // Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
        if ( AttrValue == NULL )
            Length = 0;                     // Daten konnten nicht gelesen
                                            // werden, Laenge korrigieren,
                                            // errorFlag enthaelt bereits
                                            // Fehlercode
        l_error = errorFlag;                // errorFlag nur bei Fehler aendern
    }
    return l_error;
}


// ********************************


