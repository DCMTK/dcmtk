/*
 *
 * Author: Gerd Ehlers      Created:  04-26-94
 *                          Modified: 02-07-95
 *
 * Module: dcsequen.cc
 *
 * Purpose:
 * Implementation of the class DcmSequenceOfItems
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

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

#include "dcsequen.h"
#include "dcitem.h"
#include "dicomdir/dcdirrec.h"
#include "dcvr.h"
#include "dcdebug.h"


extern char *VRTypesName[];



// ********************************


DcmSequenceOfItems::DcmSequenceOfItems( DcmTag &tag )
    : DcmElement( tag )
{
Bdebug((5, "dcsequen:DcmSequenceOfItems::DcmSequenceOfItems(DcmTag&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    itemList = new DcmList;
    lastItemComplete = TRUE;
Edebug(());

}


// ********************************


DcmSequenceOfItems::DcmSequenceOfItems( DcmTag &tag,
					T_VR_UL len,
					iDicomStream *iDStream )
    : DcmElement( tag, len, iDStream )
{
Bdebug((5, "dcsequen:DcmSequenceOfItems::DcmSequenceOfItems(DcmTag&,len=%ld,*iDS)",
           len ));
debug(( 8, "Object pointer this=0x%p", this ));

    itemList = new DcmList;
    lastItemComplete = TRUE;
Edebug(());

}


// ********************************


DcmSequenceOfItems::DcmSequenceOfItems( const DcmObject &oldObj )
    : DcmElement( InternalUseTag )
{
Bdebug((5, "dcsequen:DcmSequenceOfItems::DcmSequenceOfItems(DcmObject&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    lastItemComplete = TRUE;
    itemList = new DcmList;
debug(( 3, "ident()=%d", oldObj.ident() ));

    if ( oldObj.ident() == EVR_SQ )
    {
        DcmSequenceOfItems const *old = (DcmSequenceOfItems const *)&oldObj;
	*Tag = *old->Tag;
	iDS = old->iDS;
	offsetInFile  = old->offsetInFile;
	valueInMemory = old->valueInMemory;
	valueModified = old->valueModified;
	Length = old->Length;
	Xfer = old->Xfer;
        if ( !old->itemList->empty() )
	{
	    DcmObject *oldDO;
	    DcmObject *newDO;
	    itemList->seek( ELP_first );
            old->itemList->seek( ELP_first );
	    do {
                oldDO = old->itemList->get();
		if ( oldDO->ident() == EVR_item )
		    newDO = new DcmItem( *oldDO );
		else
		{
		    newDO = new DcmItem( oldDO->getTag() );
                    cerr << "Error: DcmSequenceOfItems(): Element("
                         << hex << oldDO->getGTag() << "," << oldDO->getETag()
                         << dec << ") found, which was not an Item" << endl;
		}

		itemList->insert( newDO, ELP_next );
            } while ( old->itemList->seek( ELP_next ) );
	}
    }
    else
    {
        cerr << "Warning: DcmSequenceOfItems: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmSequenceOfItems::DcmSequenceOfItems( const DcmSequenceOfItems &oldSeq )
    : DcmElement( InternalUseTag )
{
Bdebug((5, "dcsequen:DcmSequenceOfItems::DcmSequenceOfItems(DcmSequenceOfItems&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    lastItemComplete = TRUE;
    itemList = new DcmList;
debug(( 3, "ident()=%d", oldSeq.ident() ));

    if ( oldSeq.ident() == EVR_SQ )
    {
        DcmSequenceOfItems const *old = &oldSeq;
	*Tag = *old->Tag;
	iDS = old->iDS;
	offsetInFile  = old->offsetInFile;
	valueInMemory = old->valueInMemory;
	valueModified = old->valueModified;
	Length = old->Length;
	Xfer = old->Xfer;
        if ( !old->itemList->empty() )
	{
	    DcmObject *oldDO;
	    DcmObject *newDO;
	    itemList->seek( ELP_first );
            old->itemList->seek( ELP_first );
	    do {
                oldDO = old->itemList->get();
		if ( oldDO->ident() == EVR_item )
		    newDO = new DcmItem( *oldDO );
		else
		{
		    newDO = new DcmItem( oldDO->getTag() );
                    cerr << "Error: DcmSequenceOfItems(): Element("
                         << hex << oldDO->getGTag() << "," << oldDO->getETag()
                         << dec << ") found, which was not an Item" << endl;
		}

		itemList->insert( newDO, ELP_next );
            } while ( old->itemList->seek( ELP_next ) );
	}
    }
    else
    {
        cerr << "Warning: DcmSequenceOfItems: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmSequenceOfItems::~DcmSequenceOfItems()
{
Bdebug((5, "dcsequen:DcmSequenceOfItems::~DcmSequenceOfItems()"));
debug(( 8, "Object pointer this=0x%p", this ));

    DcmObject *dO;
    itemList->seek( ELP_first );
    while ( !itemList->empty() )
    {
	dO = itemList->remove();
	if ( dO != (DcmObject*)NULL )
	    delete dO;
    }
    delete itemList;

Edebug(());
}


// ********************************


EVR DcmSequenceOfItems::ident() const
{
    return EVR_SQ;
}


// ********************************


void DcmSequenceOfItems::print( int level )
{
    char *info = new char[200];
    char *title = (char*)NULL;
    if ( Length == UNDEF_LEN )
        title = "\"Sequence with undefined length\"";
    else
        title = "\"Sequence with explicit Length\"";

    sprintf( info, "%s #=%ld ", title, card() );
    DcmObject::printInfoLine( level, info );
    delete info;

    if ( !itemList->empty() )
    {
	DcmObject *dO;
	itemList->seek( ELP_first );
	do {
	    dO = itemList->get();
	    dO->print( level + 1 );
	} while ( itemList->seek( ELP_next ) );
    }
    DcmTag delimItemTag( ET_SequenceDelimitationItem );

    if ( Length == UNDEF_LEN )
        DcmObject::printInfoLine( level, delimItemTag,
				  0, "\"SequenceDelimitationItem\"" );
    else
        DcmObject::printInfoLine( level, delimItemTag,
		   0, "\"SequenceDelimitationItem for re-enc.\"" );
}


// ********************************


T_VR_UL DcmSequenceOfItems::getVM()
{
    return 1L;
}


// ********************************


T_VR_UL DcmSequenceOfItems::getLength( E_TransferSyntax xfer,
                                       E_EncodingType enctype )
{
Bdebug((4, "dcsequen:DcmSequenceOfItems::getLength(xfer=%d,enctype=%d)",
	   xfer, enctype ));

    T_VR_UL templen = 0;
    if ( !itemList->empty() )
    {
	DcmItem *dI;
	itemList->seek( ELP_first );
	do {
	    dI = (DcmItem*)( itemList->get() );
	    T_VR_UL sublength = dI->getLength( xfer, enctype );
Vdebug((1, sublength==UNDEF_LEN, "Warning subitem has undefined Length" ));

	    templen += sublength;
	    templen += 8;		// fuer Tag und Length

	    if ( enctype == EET_UndefinedLength )
                templen += 8;           // fuer ItemDelimitationItem
	} while ( itemList->seek( ELP_next ) );
    }
    else
	templen = 0;
debug(( 4, "Length of Sequence=%ld", templen ));
Edebug(());

    return templen;
}


// ********************************


E_Condition DcmSequenceOfItems::addGroupLengthElements( E_TransferSyntax xfer,
                                                        E_EncodingType enctype )
{
Bdebug((2, "dcitem:DcmSequenceOfItems::addGroupLengthElements(xfer=%d,enctype=%d)",
	   xfer, enctype ));

    E_Condition l_error = EC_Normal;

    if ( !itemList->empty() )
    {
	itemList->seek( ELP_first );
	do {
            DcmItem *dO = (DcmItem*)itemList->get();
            E_Condition err = dO->addGroupLengthElements( xfer, enctype );
            if ( err != EC_Normal )
                l_error = err;
	} while ( itemList->seek( ELP_next ) );
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::removeGroupLengthElements()
{
Bdebug((2, "dcitem:DcmSequenceOfItems::removeGroupLengthElements()" ));

    E_Condition l_error = EC_Normal;

    if ( !itemList->empty() )
    {
        itemList->seek( ELP_first );
	do {
            DcmItem *dO = (DcmItem*)itemList->get();
            E_Condition err = dO->removeGroupLengthElements();
            if ( err != EC_Normal )
                l_error = err;
        } while ( itemList->seek( ELP_next ) );
    }

Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::readTagAndLength( E_TransferSyntax xfer,
                                                  DcmTag &tag,
                                                  T_VR_UL *length )
{
Bdebug((4, "dcsequen:DcmSequenceOfItems::readTagAndLength(xfer=%d,&tag,"
           "*length)", xfer ));

    E_Condition l_error = EC_Normal;
    T_VR_US t1 = UNKNOWN_TAG;
    T_VR_US t2 = UNKNOWN_TAG;

    if ( !iDS->fromFile() && iDS->buffered() < 8 )
    {
        l_error = EC_InvalidStream;
    }
    else
    {

	iDS->setDataByteOrder( xfer );
	*iDS >> t1;			// Transfer Syntax !!
	*iDS >> t2;			// Transfer Syntax !!
					// Tag ist gelesen
	DcmTag newTag( t1, t2 );

	T_VR_UL len_l = 0;
	*iDS >> len_l;			// 4 Byte Laenge; Transfer Syntax !!
	*length = len_l;

debug(( 4, "Tag (0x%4.4x,0x%4.4x) \"%s\" [0x%8.8x] \"%s\"",
	   newTag.getGTag(), newTag.getETag(),
	   DcmVR(newTag.getVR()).getVRName(), len_l, newTag.getTagName() ));

        tag = newTag;                  // Rueckgabewert: assignment-operator
    }

debug(( 4, "errorFlag=(%d) in Sequ.readTag..", l_error ));
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::readSubItem( DcmTag &newTag,
                                             T_VR_UL newLength,
                                             E_TransferSyntax xfer,
                                             E_GrpLenEncoding gltype )
{
Bdebug((4, "dcsequen:DcmSequenceOfItems::readSubItem(&newTag,newLength=%d,"
           "xfer=%d,gltype=%d)", newLength, xfer, gltype ));

    E_Condition l_error = EC_Normal;
    DcmItem *subItem = (DcmItem*)NULL;

    switch ( newTag.getVR() )
    {
	case EVR_na :
            if ( newTag.getXTag() == ET_Item )
            {
                if ( this->getTag().getXTag() == ET_DirectoryRecordSequence )
                    subItem = new DcmDirectoryRecord( newTag, newLength, iDS );
                else
                    subItem = new DcmItem( newTag, newLength, iDS );
            }
            else if ( newTag.getXTag() == ET_SequenceDelimitationItem )
                l_error = EC_SequEnd;
            else if ( newTag.getXTag() == ET_ItemDelimitationItem )
                l_error = EC_InvalidTag;
	    else
                l_error = EC_InvalidTag;
		break;
	default :
	    subItem = new DcmItem( newTag, newLength, iDS );
            l_error = EC_CorruptedData;
	    break;
    }

    if ( l_error == EC_Normal && subItem != (DcmItem*)NULL )
    {
	DcmSequenceOfItems::itemList->insert( subItem, ELP_next );
	if ( !iDS->fromFile() ) 		// && !lastItemComplete )
            l_error = subItem->readBlock( xfer, gltype );
	else
            l_error = subItem->read( xfer, gltype );
                                                // hier wird Subitem eingelesen
    }
    else if ( l_error == EC_InvalidTag )  // versuche das Parsing wieder
    {                                     // einzurenken
        for ( T_VR_UL i=0; i<8; i++ )     // Tag and Length sind immer 8 Bytes
            iDS->unget();                 // schreibe Tag wieder in den Stream
        fprintf(stderr,
                "Warning: DcmSequenceOfItems::readSubItem(): parse error occured:"
                " (0x%4.4hx,0x%4.4hx)", newTag.getGTag(), newTag.getETag() );
debug(( 1, "Warning: DcmSequenceOfItems::readSubItem(): parse error occured:"
           " (0x%4.4hx,0x%4.4hx)", newTag.getGTag(), newTag.getETag() ));

    }
    else if ( l_error != EC_SequEnd )
    {
        fprintf(stderr,
                "Error: DcmSequenceOfItems::readSubItem(): cannot create SubItem"
                " (0x%4.4hx,0x%4.4hx)", newTag.getGTag(), newTag.getETag() );
debug(( 1, "Error: DcmSequenceOfItems::readSubItem(): cannot create SubItem"
           " (0x%4.4hx,0x%4.4hx)", newTag.getGTag(), newTag.getETag() ));

    }

Vdebug((4, l_error!=EC_Normal, "errorFlag(8)=(%d)", l_error ));
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::read( E_TransferSyntax xfer,
                                      E_GrpLenEncoding gltype )
{
Bdebug((3, "dcsrquen:DcmSequenceOfItems::read(xfer=%d,gltype=%d)",
           xfer, gltype ));

    Xfer = xfer;
    T_VR_UL bytes_read = 0;

    errorFlag = EC_Normal;
    if ( iDS == (iDicomStream*)NULL )
        errorFlag = EC_InvalidStream;
    else if ( iDS->eof() )
	errorFlag = EC_EndOfFile;
    else if ( !iDS->good() )
	errorFlag = EC_InvalidStream;
    else
    {
        offsetInFile = iDS->tellg();        // Position von Sequence-Value
	iDS->setDataByteOrder( xfer );
	itemList->seek( ELP_last ); // Die neuen Daten werden ans Ende gehaengt
	while ( iDS->good() && bytes_read < Length )
	{
	    DcmTag newTag( ET_UNKNOWN );
	    T_VR_UL newValueLength = 0;

            errorFlag = readTagAndLength( xfer, newTag, &newValueLength );
	    bytes_read += 8;
debug(( 3, "Seq: Header_Bytes_read            =[0x%8.8x]", bytes_read ));

	    if ( !iDS->good() ) 	    // evtl. Stream zuende
		break;			    // beende while-Schleife

            errorFlag = readSubItem( newTag, newValueLength, xfer, gltype );
            bytes_read = (T_VR_UL)(iDS->tellg() - offsetInFile);
debug(( 3, "Seq: Bytes_read                   =[0x%8.8x]", bytes_read ));

	    if ( errorFlag != EC_Normal )
		break;			    // beende while-Schleife

	} //while ( iDS->good()..
    } // else errorFlag
    valueModified = FALSE;
    if (   errorFlag == EC_SequEnd
        || errorFlag == EC_InvalidTag )
	errorFlag = EC_Normal;
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmSequenceOfItems::readBlock( E_TransferSyntax xfer,
                                           E_GrpLenEncoding gltype )
{
Bdebug((3, "dcsequen:DcmSequenceOfItems::readBlock(xfer=%d,gltype=%d)",
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
            offsetInFile = iDS->tellg();        // Position von Sequence-Value
	iDS->setDataByteOrder( xfer );
	itemList->seek( ELP_last ); // Die neuen Daten werden ans Ende gehaengt
	while (    iDS->good()
		&& ( bytesRead < Length || !lastItemComplete ) )
	{
	    DcmTag newTag( ET_UNKNOWN );
	    T_VR_UL newValueLength = 0;

	    if ( lastItemComplete )
	    {
                errorFlag = readTagAndLength( xfer, newTag, &newValueLength );

                if ( errorFlag != EC_Normal )
		    break;			// beende while-Schleife
		else
		    bytesRead += 8;

		lastItemComplete = FALSE;
                errorFlag = readSubItem( newTag, newValueLength, xfer, gltype );
		if ( errorFlag == EC_Normal )
		    lastItemComplete = TRUE;
	    }
	    else
	    {
                errorFlag = itemList->get()->readBlock( xfer, gltype ) ;
		if ( errorFlag == EC_Normal )
		    lastItemComplete = TRUE;
	    }
            bytesRead = (T_VR_UL)(iDS->tellg() - offsetInFile);

	    if ( errorFlag != EC_Normal )
		break;				// beende while-Schleife

	} //while ( iDS->good()..
	if (	( bytesRead < Length || !lastItemComplete )
	     && errorFlag != EC_SequEnd )
	    errorFlag = EC_InvalidStream;
    } // else errorFlag
    valueModified = FALSE;
    if ( errorFlag == EC_SequEnd )
	errorFlag = EC_Normal;
    if ( errorFlag == EC_Normal )
	rdStat = ERW_ready;	     // Sequence ist komplett
debug(( 3, "errorFlag=(%d)", errorFlag ));
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmSequenceOfItems::write( oDicomStream &oDS,
				       E_TransferSyntax oxfer,
                                       E_EncodingType enctype,
                                       E_GrpLenEncoding gltype )
{
Bdebug((3, "dcsequen:DcmSequenceOfItems::write(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
           oxfer, enctype, gltype ));

    T_VR_UL localLength = 0;
    errorFlag = EC_Normal;
    if ( !oDS.good() )
	errorFlag = EC_InvalidStream;
    else
    {
	localLength = this->getLength( oxfer, enctype );
				  // getLength kann u.U. auch UNDEF_LEN liefern
	if ( enctype == EET_ExplicitLength )
	    Length = localLength;
	else
	    Length = UNDEF_LEN;

	T_VR_UL written_bytes = 0;
	errorFlag = DcmObject::writeTagAndLength( oDS, oxfer, &written_bytes );

debug(( 3, "Header_Bytes_written              =[0x%8.8x]", written_bytes ));

	if ( !itemList->empty() )
	{
	    DcmItem *dI;
	    itemList->seek( ELP_first );
	    do {
		dI = (DcmItem*)( itemList->get() );
                errorFlag = dI->write( oDS, oxfer, enctype, gltype );
	    } while ( itemList->seek( ELP_next ) && errorFlag == EC_Normal );
	}
	written_bytes += localLength;	// ist u.U. ungenau

	if ( Length == UNDEF_LEN )	// schreibe ItemDelimitationItem
	{
	    DcmTag delim( ET_SequenceDelimitationItem );
            oDS << delim.getGTag();    // 2 Byte Laenge; Transfer Syntax !!
            oDS << delim.getETag();    // 2 Byte Laenge; Transfer Syntax !!
	    T_VR_UL delimLen = 0L;
            oDS << delimLen;           // 4 Byte Laenge; Transfer Syntax !!
	    written_bytes += 8;
	}
debug(( 3, "Bytes_written                     =[0x%8.8x]", written_bytes ));

	valueModified = FALSE;
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmSequenceOfItems::writeBlock( oDicomStream &oDS,
					    E_TransferSyntax oxfer,
                                            E_EncodingType enctype,
                                            E_GrpLenEncoding gltype )
{
Bdebug((3, "dcsequen:DcmSequenceOfItems::writeBlock(&oDS,oxfer=%d,enctype=%d)",
	   oxfer, enctype ));

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
		    if ( enctype == EET_ExplicitLength )
			Length = this->getLength( oxfer, enctype );
				  // getLength kann u.U. auch UNDEF_LEN liefern
		    else
			Length = UNDEF_LEN;

		    T_VR_UL written_bytes = 0;
		    errorFlag = DcmObject::writeTagAndLength( oDS,
							      oxfer,
							      &written_bytes );
		    if ( errorFlag == EC_Normal )
		    {
			wrStat = ERW_inWork;
			itemList->seek( ELP_first );
		    }
		}
		else
		    errorFlag = EC_InvalidStream;
	    }
	    if ( !itemList->empty() && writeState() != ERW_init )
	    {
		DcmObject *dO;
		do {
		    dO = itemList->get();
                    errorFlag = dO->writeBlock( oDS, oxfer, enctype, gltype );
		} while (    errorFlag == EC_Normal
			  && itemList->seek( ELP_next ) );
	    }


	    if ( errorFlag == EC_Normal && writeState() != ERW_init )
	    {
                if ( Length == UNDEF_LEN && oDS.avail() >= 8 )
					     // schreibe SquenceDelimitationItem
		{
		    DcmTag delim( ET_SequenceDelimitationItem );
                    oDS << delim.getGTag(); // 2 Byte Laenge; Transfer Syntax !
                    oDS << delim.getETag(); // 2 Byte Laenge; Transfer Syntax !
		    T_VR_UL delimLen = 0L;
                    oDS << delimLen;        // 4 Byte Laenge; Transfer Syntax !
		}
		wrStat = ERW_ready;
	    }
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmSequenceOfItems::readBlockInit()
{
    rdStat = ERW_init;
    lastItemComplete = TRUE;
    if ( !itemList->empty() )
    {
	itemList->seek( ELP_first );
	do {
	    itemList->get()->readBlockInit();

	} while ( itemList->seek( ELP_next ) );
    }
    return EC_Normal;
}


// ********************************


E_Condition DcmSequenceOfItems::writeBlockInit()
{
    wrStat = ERW_init;
    if ( !itemList->empty() )
    {
	itemList->seek( ELP_first );
	do {
	    itemList->get()->readBlockInit();

	} while ( itemList->seek( ELP_next ) );
    }
    return EC_Normal;
}


// ********************************


T_VR_UL DcmSequenceOfItems::card()
{
    return itemList->card();
}


// ********************************


E_Condition DcmSequenceOfItems::insert( DcmItem* item,
					T_VR_UL where )
{
Bdebug((3, "dcsequen:DcmSequenceOfItems::insert(DcmItem*=%p,where=%ld)",
           item, where ));

    errorFlag = EC_Normal;
    if ( item != (DcmItem*)NULL )
    {
	itemList->seek_to( where );
	itemList->insert( item );
	valueModified = TRUE;
Vdebug((3, where< itemList->card(), "item at position %d inserted", where ));
Vdebug((3, where>=itemList->card(), "item at last position inserted" ));

    }
    else
	errorFlag = EC_IllegalCall;
Edebug(());

    return errorFlag;
}


// ********************************


DcmItem* DcmSequenceOfItems::getItem( T_VR_UL num )
{
    errorFlag = EC_Normal;
    DcmItem *item;
    item = (DcmItem*)( itemList->seek_to(num) );  // liest Item aus Liste
    if ( item == (DcmItem*)NULL )
	errorFlag = EC_IllegalCall;
    return item;
}


// ********************************


DcmItem* DcmSequenceOfItems::remove( T_VR_UL num )
{
Bdebug((3, "dcsequen:DcmSequenceOfItems::remove(num=%ld)", num ));

    errorFlag = EC_Normal;
    DcmItem *item;
    item = (DcmItem*)( itemList->seek_to(num) );  // liest Item aus Liste
    if ( item != (DcmItem*)NULL )
    {
debug(( 3, "item p=%p removed, but not deleted", item ));

	itemList->remove();
    }
    else
	errorFlag = EC_IllegalCall;
Edebug(());

    return item;
}


// ********************************


DcmItem* DcmSequenceOfItems::remove( DcmItem* item )
{
Bdebug((3, "dcsequen:DcmSequenceOfItems::remove(DcmItem*)" ));

    DcmItem *retItem = (DcmItem*)NULL;
    errorFlag = EC_IllegalCall;
    if ( !itemList->empty() && item != (DcmItem*)NULL )
    {
	DcmObject *dO;
	itemList->seek( ELP_first );
	do {
	    dO = itemList->get();
	    if ( dO == item )
	    {
debug(( 3, "item p=%p removed, but not deleted", item ));

		itemList->remove();	    // entfernt Element aus Liste,
					    // aber loescht es nicht
		errorFlag = EC_Normal;
		break;
	    }
	} while ( itemList->seek( ELP_next ) );
    }
    if ( errorFlag == EC_IllegalCall )
        retItem = (DcmItem*)NULL;
    else
        retItem = item;
Edebug(());

    return item;
}


// ********************************


E_Condition DcmSequenceOfItems::clear()
{
Bdebug((2, "dcsequen:DcmSequenceOfItems::clear()"));

    errorFlag = EC_Normal;
    DcmObject *dO;
    itemList->seek( ELP_first );
    while ( !itemList->empty() )
    {
	dO = itemList->remove();
	if ( dO != (DcmObject*)NULL )
        {
	    delete dO;
            dO = (DcmObject*)NULL;
        }
    }
    Length = 0;
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmSequenceOfItems::verify( BOOL autocorrect )
{
Bdebug((3, "dcsequen:DcmSequenceOfItems::verify(autocorrect=%d)",
	   autocorrect ));
debug(( 3, "Tag=(0x%4.4x,0x%4.4x) \"%s\" \"%s\"",
	   getGTag(), getETag(),
	   DcmVR(getVR()).getVRName(), Tag->getTagName() ));

    errorFlag = EC_Normal;
    if ( !itemList->empty() )
    {
	DcmObject *dO;
	itemList->seek( ELP_first );
	do {
	    dO = itemList->get();
	    if ( dO->verify( autocorrect ) != EC_Normal )
		errorFlag = EC_CorruptedData;
	} while ( itemList->seek( ELP_next ) );
    }
    if ( autocorrect == TRUE )
	Length = this->getLength();
Edebug(());

    return errorFlag;
}


// ********************************

    // Vorbedingung: itemList ist nicht leer!
    // Ergebnis:     - return EC_Normal;
    //		       gefundener Element-Zeiger auf resultStack
    //		     - return EC_TagNotFound;
    //		       resultStack unveraendert
    // Weitersuchen: schreibe Zeiger des Sub-Elementes auf resultStack und
    //		     starte dann Sub-Suche


E_Condition DcmSequenceOfItems::searchSubFromHere( DcmTag &tag,
						   DcmStack &resultStack,
						   BOOL searchIntoSub )
{
Bdebug((5, "dcsequen:DcmSequenceOfItems::searchSubFromHere(tag=(%4.4x,%4.4x),"
           "Stack&(%ld),sub=%d)", tag.getGTag(), tag.getETag(),
           resultStack.card(), searchIntoSub ));

    DcmObject *dO;
    E_Condition l_error = EC_TagNotFound;
    if ( !itemList->empty() )
    {
        itemList->seek( ELP_first );
        do {
            dO = itemList->get();
            if ( searchIntoSub )
            {
                resultStack.push( dO );
                if ( tag == dO->getTag() )
                    l_error = EC_Normal;
                else
                    l_error = dO->search( tag,
                                          resultStack,
                                          ESM_fromStackTop,
                                          TRUE );
                if ( l_error != EC_Normal )
                    resultStack.pop();
            }
            else
            {
                if ( tag == dO->getTag() )
                {
                    resultStack.push( dO );
                    l_error = EC_Normal;
                }
            }
        } while ( l_error != EC_Normal && itemList->seek( ELP_next ) );
Vdebug((4, l_error==EC_Normal && tag==dO->getTag(), "Search-Tag=(%4.4x,%4.4x)"
           " \"%s\" found!", tag.getGTag(), tag.getETag(),
           DcmVR(tag.getVR()).getVRName() ));

    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::search( DcmTag &tag,
					DcmStack &resultStack,
					E_SearchMode mode,
					BOOL searchIntoSub )
{
Bdebug((5, "dcsequen:DcmSequenceOfItems::search(tag=(%4.4x,%4.4x),Stack&(%ld),"
           "mode=%d,sub=%d)", tag.getGTag(), tag.getETag(), resultStack.card(),
           mode, searchIntoSub ));
debug(( 5, "local Info: Tag=(%4.4x,%4.4x) \"%s\" p=%p",
           getGTag(), getETag(), DcmVR(getVR()).getVRName(), this ));

    DcmObject *dO = (DcmObject*)NULL;
    E_Condition l_error = EC_TagNotFound;
    if ( mode == ESM_afterStackTop && resultStack.top() == this )
    {
        l_error = searchSubFromHere( tag, resultStack, searchIntoSub );
debug(( 5, "mode=ESM_afterStackTop && resultStack.top()=this" ));

    }
    else if ( !itemList->empty() )
    {
	if ( mode == ESM_fromHere || resultStack.empty() )
	{
debug(( 5, "mode=ESM_fromHere || resultStack.empty()" ));

	    resultStack.clear();
            l_error = searchSubFromHere( tag, resultStack, searchIntoSub );
	}
	else if ( mode == ESM_fromStackTop )
	{
debug(( 5, "mode=ESM_fromStackTop" ));

	    dO = resultStack.top();
	    if ( dO == this )
                l_error = searchSubFromHere( tag, resultStack, searchIntoSub );
	    else
	    {	// gehe direkt zu Sub-Baum und suche dort weiter
                l_error = dO->search( tag, resultStack, mode, searchIntoSub );
                if ( l_error != EC_Normal ) // raeumt nur die oberste Stackebene
                    resultStack.pop();      // ab; der Rest ist unveraendert
	    }
	}
	else if ( mode == ESM_afterStackTop && searchIntoSub )
	{
debug(( 5, "mode=ESM_afterStackTop && searchIntoSub" ));

	    // resultStack enthaelt Zielinformationen:
	    // - stelle Zustand der letzen Suche in den einzelnen Suchroutinen
	    //	 wieder her
	    // - finde Position von dO in Baum-Struktur
	    //	 1. suche eigenen Stack-Eintrag
	    //	    - bei Fehlschlag Suche beenden
	    //	 2. nehme naechsthoeheren Eintrag dnO
	    //	 3. stelle eigene Liste auf Position von dnO
	    //	 4. starte Suche ab dnO

	    T_VR_UL i = resultStack.card();
debug(( 5, "resultStack: card()=%d", i ));

	    while ( i > 0 && (dO = resultStack.elem(i-1)) != this )
	    {
debug(( 5, "--dO=elem(%d)=%p  this=%p", i-1, dO, this ));

		i--;
	    }
	    if ( dO != this && resultStack.card() > 0 )
	    {			       // oberste Ebene steht nie in resultStack
		i = resultStack.card()+1;// zeige jetzt auf hoechste Ebene+1
                dO = this;               // Treffer der hoechsten Ebene!
	    }
	    if ( dO == this )
	    {
debug(( 5, "--dO=elem(%d)=%p==this=%p", i-1, dO, this ));
debug(( 5, "currently at resultStack position %d, dO=%p", i-1, dO ));

                if ( i == 1 )                 // habe resultStack.top() gefunden
                    l_error = EC_TagNotFound; // markiere als kein Treffer, s.o.
		else
		{
		    E_SearchMode submode = mode;
		    BOOL searchNode = TRUE;
		    DcmObject *dnO;
		    dnO = resultStack.elem( i-2 ); // Knoten der naechsten Ebene
debug(( 5, "itemList: dnO=%p", dnO ));

		    itemList->seek( ELP_first );
		    do {
			dO = itemList->get();
			searchNode = searchNode ? ( dO != dnO ) : FALSE;
Vdebug((5, searchNode,  "--searching Node dnO=%p, found: dO=%p", dnO, dO ));
Vdebug((5, !searchNode && submode==ESM_afterStackTop,
                        "--searching Node dnO=%p found!", dO ));
Vdebug((5, !searchNode && submode!=ESM_afterStackTop,
                        "--next Node dO=%p for beeing tested", dO ));

			if ( !searchNode )
			{				// suche jetzt weiter
			    if ( submode == ESM_fromStackTop )
				resultStack.push( dO ); // Stack aktualisieren
                            if (    submode == ESM_fromStackTop
                                 && tag == dO->getTag()
                               )
                                l_error = EC_Normal;
                            else
                                l_error = dO->search( tag,
                                                      resultStack,
                                                      submode,
                                                      TRUE );
			    if ( l_error != EC_Normal )
                                    resultStack.pop();
			    else
				break;
                            submode = ESM_fromStackTop; // ab hier normale Suche
			}
		    } while ( itemList->seek( ELP_next ) );
		}
	    }
	    else			      // wird vermutlich nie erreicht
                l_error = EC_IllegalCall;
	} // ( mode == ESM_afterStackTop
	else
            l_error = EC_IllegalCall;
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::search( ETag etag,
					DcmStack &resultStack,
					E_SearchMode mode,
					BOOL searchIntoSub )
{
Bdebug((5, "dcsequen:DcmSequenceOfItems::search(etag=%d,Stack&,mode=%d,sub=%d)",
	   etag, mode, searchIntoSub ));

    DcmTag tag( etag );
    E_Condition l_error = search( tag, resultStack, mode, searchIntoSub );
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::searchErrors( DcmStack &resultStack )
{
Bdebug((5, "dcsequen:DcmSequenceOfItems::searchErrors(Stack&)" ));

    E_Condition l_error = errorFlag;
    DcmObject *dO = (DcmObject*)NULL;
    if ( errorFlag != EC_Normal )
	resultStack.push( this );
    if ( !itemList->empty() )
    {
	itemList->seek( ELP_first );
	do {
	    E_Condition err;
	    dO = itemList->get();
	    if ( (err = dO->searchErrors( resultStack )) != EC_Normal )
                l_error = err;
	} while ( itemList->seek( ELP_next ) );
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::loadAllDataIntoMemory()
{
Bdebug((3, "dcsequen:DcmSequenceOfItems::loadAllDataIntoMemory()"));

    E_Condition l_error = EC_Normal;
    if ( !itemList->empty() )
    {
        itemList->seek( ELP_first );
	do {
	    E_Condition err;
            DcmObject *dO = itemList->get();
            if ( (err = dO->loadAllDataIntoMemory()) != EC_Normal )
		l_error = err;
        } while ( itemList->seek( ELP_next ) );
    }
Edebug(());

    return l_error;
}


// ********************************


