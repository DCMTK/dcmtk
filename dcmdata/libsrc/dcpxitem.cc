/*
 *
 * Author: Gerd Ehlers      Created:  11-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcpxitem.cc
 *
 * Purpose:
 * Implementation of class DcmPixelItem
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */



#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

#include "dcpxitem.h"
#include "dcstream.h"
#include "dcdebug.h"



// ********************************


DcmPixelItem::DcmPixelItem()
    : DcmItem()
{
Bdebug((5, "dcpxitem:DcmPixelItem::DcmPixelItem()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    PixelValue = (BYTE*)NULL;
Edebug(());

}


// ********************************


DcmPixelItem::DcmPixelItem( const DcmTag &tag,
                            T_VR_UL len,
                            iDicomStream *iDStream )
    : DcmItem( tag, len, iDStream )
{
Bdebug((5, "dcpxitem:DcmPixelItem::DcmPixelItem(DcmTag&,len=%ld,*iDS)",
	   len ));
debug(( 8, "Object pointer this=0x%p", this ));

    PixelValue = (BYTE*)NULL;
Edebug(());

}


// ********************************


DcmPixelItem::DcmPixelItem( const DcmPixelItem &old )
    : DcmItem( old )
{
Bdebug((5, "dcpxitem:DcmPixelItem::DcmPixelItem(DcmPixelItem&)"));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( old.ident() == EVR_pixelItem )
    {
	if (Length == 0 || old.PixelValue == (BYTE*)NULL ) {
	    if ( valueInMemory )
		Length = 0;
            PixelValue = (BYTE*)NULL;
	} else {
            PixelValue = new BYTE[ Length ];
            memcpy( PixelValue, old.PixelValue, (int)Length );
	}
    } else {
        PixelValue = (BYTE*)NULL;
	errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmPixelItem: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmPixelItem::~DcmPixelItem()
{
Bdebug((5, "dcpxitem:DcmPixelItem::~DcmPixelItem()"));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( PixelValue != (BYTE*)NULL )
        delete PixelValue;
Edebug(());

}


// ********************************


DcmEVR DcmPixelItem::ident() const
{
    return EVR_pixelItem;
}


// ********************************


void DcmPixelItem::print( int level )
{
    if ( PixelValue != (BYTE*)NULL )
    {
	T_VR_UL mlen = (100 < Length) ? 100 : Length;
	char *ch_words;
	char *tmp = ch_words = new char[ mlen*5 + 6 ];
	unsigned int i;

        BYTE *tchar = PixelValue;
        for ( i=0; i<mlen/sizeof(BYTE); i++)
        {
            sprintf( tmp, "%2.2x\\", *tchar);
            tmp += 3;
            tchar++;
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


T_VR_UL DcmPixelItem::getVM()
{
    return 1L;
}


// ********************************


E_Condition DcmPixelItem::alignValue()
{
	// Hinweis: darf nicht in this->write() verwendet werden,
	//	    da Length veraendert wird,
	//	    sondern muss vorher benutzt werden
	//	    z.B. in read() und put()

    E_Condition l_error = EC_Normal;
    if ( Length != 0L && PixelValue != (BYTE*)NULL )
    {
	T_VR_UL Length_al = ((Length+1) & 0xfffffffe);
	if ( Length < Length_al )
	{
            BYTE *str = PixelValue;
	    str[Length] = '\0';
	    Length = Length_al;
	}
    }
    return l_error;
}


// ********************************


E_Condition DcmPixelItem::readValueField( E_TransferSyntax xfer )
{
Bdebug((4, "dcpxitem:DcmPixelItem::readValueField(xfer)" ));
Vdebug((1, !iDS->good(), "Warning: before read: iDS->rdstate()=(0x%x)",
			 iDS->rdstate() ));

    E_Condition l_error = EC_Normal;
    long actpos;
    iDS->clear();
    if ( iDS->good() && iDS->fromFile() )
    {
        iDS->setDataByteOrder( xfer );
        if ( PixelValue != (BYTE*)NULL )
            delete PixelValue;
        PixelValue = new BYTE[ Length + 1 ];
	actpos = iDS->tellg();
	iDS->seekg( offsetInFile );
        iDS->read( PixelValue, (int)Length );        // Warnung: evtl. int=16bit
	Length = iDS->gcount(); 		     // falls Datenblock zuende
Vdebug((1, !iDS->good(), "Warning: iDS->rdstate()=(0x%x)", iDS->rdstate() ));

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


E_Condition DcmPixelItem::read( E_TransferSyntax xfer,
                                E_GrpLenEncoding gltype )
{
Bdebug((3, "dcpxitem:DcmPixelItem::read(xfer=%d,gltype=%d)",
           xfer, gltype ));

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
            if ( PixelValue != (BYTE*)NULL )
                delete PixelValue;
            PixelValue = new BYTE[ Length + 1 ];
            iDS->read( PixelValue, (int)Length ); // Wrn.: evtl. int=16bit
	    if ( Length != 0 )
                Length = iDS->gcount();           // falls Datenblock zuende
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


E_Condition DcmPixelItem::readBlock( E_TransferSyntax xfer,
                                     E_GrpLenEncoding gltype )
{
Bdebug((3, "dcpxitem:DcmPixelItem::readBlock(xfer=%d,gltype=%d)",
           xfer, gltype ));

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
            iDS->setDataByteOrder( xfer );
            if ( PixelValue != (BYTE*)NULL )
                delete PixelValue;
            PixelValue = new BYTE[ Length + 1 ];
	    rdStat = ERW_inWork;
	}
	T_VR_UL len = (Length - bytesRead) <= iDS->buffered()
		      ? (Length - bytesRead)
		      : iDS->buffered();
        iDS->read( &PixelValue[ bytesRead ], (int)len );
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


E_Condition DcmPixelItem::write( oDicomStream &oDS,
                                 E_TransferSyntax oxfer,
                                 E_EncodingType enctype,
                                 E_GrpLenEncoding gltype )
{
Bdebug((3, "dcpxitem:DcmPixelItem::write((&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
        if ( PixelValue == (BYTE*)NULL )
	    Length = 0; 		  // Daten konnten nicht gelesen werden

	T_VR_UL written_bytes;
	errorFlag = DcmObject::writeTagAndLength( oDS, oxfer, &written_bytes );
debug(( 3, "Header_Bytes_written              =[0x%8.8x]", written_bytes ));
Vdebug((3, errorFlag, "Warning: error on writing header=(%d)", errorFlag ));

        if ( PixelValue != (BYTE*)NULL && errorFlag == EC_Normal )
	{
            oDS.write( PixelValue, (int)Length );
                                            // Warnung: evtl. int=16bit
debug(( 3, "Bytes_written  as bytes           =[0x%8.8x]", written_bytes ));

        }
	written_bytes += Length;

	valueInMemory = TRUE;
	valueModified = FALSE;
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmPixelItem::writeBlock( oDicomStream &oDS,
                                      E_TransferSyntax oxfer,
                                      E_EncodingType enctype,
                                      E_GrpLenEncoding gltype )
{
Bdebug((3, "dcpxitem:DcmPixelItem::write(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
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
                    if ( PixelValue == (BYTE*)NULL )
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
            if (    PixelValue != (BYTE*)NULL
		 && errorFlag == EC_Normal
		 && writeState() != ERW_init )
	    {
                T_VR_UL len = (Length - bytesWritten) <= oDS.avail()
                              ? (Length - bytesWritten)
                              : oDS.avail();
                oDS.write( &PixelValue[ bytesWritten ],
                             (int)len );
                                                 // Warnung: evtl. int=16bit
                bytesWritten += len;
debug(( 3, "Bytes_written  as bytes           =[0x%8.8x]", bytesWritten ));

		if ( bytesWritten == Length )
		    wrStat = ERW_ready;
		else
		    errorFlag = EC_InvalidStream;
            }

	    valueInMemory = TRUE;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmPixelItem::put( BYTE *bytevalue,
                               T_VR_UL length )      // number of byte
{
    errorFlag = EC_Normal;
    if ( PixelValue != (BYTE*)NULL )
        delete PixelValue;
    Length = ((length+1) & 0xfffffffe);
    if ( bytevalue != (BYTE*)NULL )
    {
        PixelValue = new BYTE[ Length ];
        memcpy( PixelValue, bytevalue, (int)length );
    }
    else
    {
        PixelValue = (BYTE*)NULL;
	Length = 0;
	errorFlag = EC_CorruptedData;
    }
    valueInMemory = TRUE;
    valueModified = TRUE;
    alignValue();
    return errorFlag;
}


// ********************************


BYTE* DcmPixelItem::getBytes()
{
    errorFlag = EC_Normal;
    if ( valueInMemory == FALSE )
    {					    // Hier Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
    }
    if ( PixelValue == (BYTE*)NULL )
	Length = 0;			   // Daten konnten nicht gelesen werden
    return PixelValue;
}


// ********************************


E_Condition DcmPixelItem::clear()
{
Bdebug((2, "dcpxitem:DcmPixelItem::clear()"));

    errorFlag = EC_Normal;
    if ( PixelValue != (BYTE*)NULL )
        delete PixelValue;
    PixelValue = (BYTE*)NULL;
    Length = 0;
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmPixelItem::verify( BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( autocorrect == TRUE )
	alignValue();
    return errorFlag;
}


// ********************************


E_Condition DcmPixelItem::loadAllDataIntoMemory()
{
    E_Condition l_error = EC_Normal;
    if ( valueInMemory == FALSE )
    {                                       // Daten aus iDS nachladen
	errorFlag = readValueField( Xfer ); // Xfer gilt nur fuer iDS
        if ( PixelValue == NULL )
            Length = 0;                     // Daten konnten nicht gelesen
                                            // werden, Laenge korrigieren,
                                            // errorFlag enthaelt bereits
                                            // Fehlercode
        l_error = errorFlag;                // errorFlag nur bei Fehler aendern
    }
    return l_error;
}


// ********************************


